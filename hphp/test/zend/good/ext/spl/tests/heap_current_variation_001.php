<?php

class myHeap extends SplHeap
{
    public function compare($v1, $v2)
    {
        throw new Exception('');
    }
}
<<__EntryPoint>> function main() {
$heap = new myHeap();
var_dump($heap->current());
}
