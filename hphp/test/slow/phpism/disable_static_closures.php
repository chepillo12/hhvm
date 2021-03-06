<?hh
// Copyright 2004-present Facebook. All Rights Reserved.

class StaticClosureTest {
  public function foo(): string { return 'foo'; }


  public function getLambda(): mixed {
    return function() { return $this->foo(); };
  }


  public function getStaticLambda(): mixed {
    return static function() {
      return $this->foo();
    };
  }

  public function getNamedStaticLambda(): mixed {
    $f = static function () {
      return $this->foo();
    };
    return $f;
  }
}

<<__EntryPoint>> function main(): void {}
