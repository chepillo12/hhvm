<?php

class TestClass {}
abstract class TestAbstractClass {}
<<__EntryPoint>> function main() {
$testClass = new ReflectionClass('TestClass');
$abstractClass = new ReflectionClass('TestAbstractClass');

var_dump($testClass->isAbstract());
var_dump($abstractClass->isAbstract());
}
