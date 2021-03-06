// Copyright (c) 2019, Facebook, Inc.
// All rights reserved.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the "hack" directory of this source tree.

use crate::source_text::SourceText;
use crate::trivia_kind::TriviaKind;

pub trait LexableTrivia: Clone + PartialEq {
    fn make_whitespace(source: &SourceText, offset: usize, width: usize) -> Self;
    fn make_eol(source: &SourceText, offset: usize, width: usize) -> Self;
    fn make_single_line_comment(source: &SourceText, offset: usize, width: usize) -> Self;
    fn make_fallthrough(source: &SourceText, offset: usize, width: usize) -> Self;
    fn make_unsafe(source: &SourceText, offset: usize, width: usize) -> Self;
    fn make_unsafe_expression(source: &SourceText, offset: usize, width: usize) -> Self;
    fn make_fix_me(source: &SourceText, offset: usize, width: usize) -> Self;
    fn make_ignore_error(source: &SourceText, offset: usize, width: usize) -> Self;
    fn make_extra_token_error(source: &SourceText, offset: usize, width: usize) -> Self;
    fn make_delimited_comment(source: &SourceText, offset: usize, width: usize) -> Self;
    fn make_after_halt_compiler(source: &SourceText, offset: usize, width: usize) -> Self;

    fn kind(&self) -> TriviaKind;
    fn width(&self) -> usize;
}
