<?php

function haricow($a = 'one') {
    var_dump($a);
    $a = 'two';
}
<<__EntryPoint>> function main() {
haricow();
haricow();
echo "===DONE===\n";
}
