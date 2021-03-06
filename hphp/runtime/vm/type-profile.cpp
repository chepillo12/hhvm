/*
   +----------------------------------------------------------------------+
   | HipHop for PHP                                                       |
   +----------------------------------------------------------------------+
   | Copyright (c) 2010-present Facebook, Inc. (http://www.facebook.com)  |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
*/

#include "hphp/runtime/vm/type-profile.h"

#include "hphp/runtime/base/execution-context.h"
#include "hphp/runtime/base/init-fini-node.h"
#include "hphp/runtime/base/request-info.h"
#include "hphp/runtime/base/runtime-option.h"
#include "hphp/runtime/base/stats.h"
#include "hphp/runtime/ext/server/ext_server.h"
#include "hphp/runtime/vm/func.h"
#include "hphp/runtime/vm/jit/mcgen-translate.h"
#include "hphp/runtime/vm/jit/relocation.h"
#include "hphp/runtime/vm/jit/tc.h"
#include "hphp/runtime/vm/jit/write-lease.h"
#include "hphp/runtime/vm/treadmill.h"
#include "hphp/util/atomic-vector.h"
#include "hphp/util/boot-stats.h"
#include "hphp/util/lock.h"
#include "hphp/util/logger.h"
#include "hphp/util/struct-log.h"
#include "hphp/util/trace.h"

#include <array>
#include <atomic>
#include <cstdint>
#include <queue>
#include <utility>

namespace HPHP {

TRACE_SET_MOD(typeProfile);

//////////////////////////////////////////////////////////////////////

/*
 * Warmup/profiling.
 *
 * In cli mode, we only record samples if we're in recording to replay later.
 *
 * In server mode, we exclude warmup document requests from profiling.
 */

RDS_LOCAL_NO_CHECK(TypeProfileLocals, rl_typeProfileLocals)
  {TypeProfileLocals{}};

namespace {

bool warmingUp;
std::atomic<int64_t> numRequests;
std::atomic<int> relocateRequests;

/*
 * RFH, or "requests served in first hour" is used as a performance metric that
 * is affected by warmup speed as well as steady-state performance. For every
 * element n in this list, we log a point along the RFH curve, which is the
 * total number of requests served when server uptime hits n seconds.
 */
constexpr std::array<uint32_t, 32> rfhBuckets = {{
  30, 60, 90, 120, 150, 180, 210, 240, 270, 300,             // every 30s, to 5m
  360, 420, 480, 540, 600,                                   // every 1m, to 10m
  900, 1200, 1500, 1800, 2100, 2400, 2700, 3000, 3300, 3600, // every 5m, to 1h
  4500, 5400, 6300, 7200,                                    // every 15m, to 2h
  3 * 3600, 4 * 3600, 6 * 3600
}};
std::atomic<size_t> nextRFH{0};

}

ProfileNonVMThread::ProfileNonVMThread() {
  always_assert(!rl_typeProfileLocals->nonVMThread);
  rl_typeProfileLocals->nonVMThread = true;
}

ProfileNonVMThread::~ProfileNonVMThread() {
  rl_typeProfileLocals->nonVMThread = false;
}

void setRelocateRequests(int32_t n) {
  relocateRequests.store(n);
}

void profileWarmupStart() {
  warmingUp = true;
}

void profileWarmupEnd() {
  warmingUp = false;
}

typedef std::pair<const Func*, uint32_t> FuncHotness;
static bool comp(const FuncHotness& a, const FuncHotness& b) {
  return a.second > b.second;
}

int64_t requestCount() {
  return numRequests.load(std::memory_order_relaxed);
}

static inline RequestKind getRequestKind() {
  if (rl_typeProfileLocals->nonVMThread) return RequestKind::NonVM;
  if (warmingUp) return RequestKind::Warmup;
  return RequestKind::Standard;
}

void profileRequestStart() {
  rl_typeProfileLocals->requestKind = getRequestKind();

  // Force the request to use interpreter (not even running jitted code) during
  // retranslateAll when we need to dump out precise profile data.
  auto const forceInterp = jit::mcgen::pendingRetranslateAllScheduled() &&
                           RuntimeOption::DumpPreciseProfData;
  bool okToJit = !forceInterp &&
                 (rl_typeProfileLocals->requestKind == RequestKind::Standard);
  if (!RequestInfo::s_requestInfo.isNull()) {
    if (RID().isJittingDisabled()) {
      okToJit = false;
    } else if (!okToJit) {
      RID().setJittingDisabled(true);
    }
  }
  jit::setMayAcquireLease(okToJit);

  // Force interpretation if needed.
  if (rl_typeProfileLocals->forceInterpret != forceInterp) {
    rl_typeProfileLocals->forceInterpret = forceInterp;
    if (!RequestInfo::s_requestInfo.isNull()) {
      RID().updateJit();
    }
  }

  if (okToJit && relocateRequests > 0 && !--relocateRequests) {
    jit::tc::liveRelocate(true);
  }
}

static void checkRFH(int64_t finished) {
  auto i = nextRFH.load(std::memory_order_relaxed);
  if (i == rfhBuckets.size() || !StructuredLog::enabled()) {
    return;
  }

  auto const uptime = f_server_uptime();
  if (uptime == -1) return;
  assertx(uptime >= 0);

  while (i < rfhBuckets.size() && uptime >= rfhBuckets[i]) {
    assertx(i == 0 || rfhBuckets[i - 1] < rfhBuckets[i]);
    if (!nextRFH.compare_exchange_strong(i, i + 1, std::memory_order_relaxed)) {
      // Someone else reported the sample at i. Try again with the current
      // value of nextRFH.
      continue;
    }

    // "bucket" and "uptime" will always be the same as long as the server
    // retires at least one request in each second of wall time.
    StructuredLogEntry cols;
    cols.setInt("requests", finished);
    cols.setInt("bucket", rfhBuckets[i]);
    cols.setInt("uptime", uptime);
    StructuredLog::log("hhvm_rfh", cols);

    ++i;
  }
}

void profileRequestEnd() {
  if (warmingUp ||
      rl_typeProfileLocals->requestKind == RequestKind::NonVM) {
    return;
  }
  auto const finished = numRequests.fetch_add(1, std::memory_order_relaxed) + 1;
  static auto const requestSeries = ServiceData::createTimeSeries(
    "vm.requests",
    {ServiceData::StatsType::RATE, ServiceData::StatsType::SUM},
    {std::chrono::seconds(60), std::chrono::seconds(0)}
  );
  requestSeries->addValue(1);
  checkRFH(finished);
}

}
