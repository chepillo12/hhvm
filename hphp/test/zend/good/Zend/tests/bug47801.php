<?php

class A
{
  function __call($name, $args)
  {
    echo("magic method called: $name\n");
  }
}

class B extends A
{
  function getFoo()
  {
    parent::getFoo();
  }
}
<<__EntryPoint>> function main() {
$a = new A();
$a->getFoo();

$b = new B();
$b->getFoo();
}
