<?php
class A
{
    private static $p = "A::p (static)";
    static function showA()
    {
        echo self::$p . "\n";
    }
}

class B extends A
{
    public $p = "B::p";
    function showB()
    {
        echo $this->p . "\n";
    }
}

<<__EntryPoint>> function main() {
A::showA();

$b = new B;
B::showA();
$b->showB();
}
