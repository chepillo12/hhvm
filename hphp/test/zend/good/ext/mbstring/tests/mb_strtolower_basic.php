<?php
/* Prototype  : string mb_strtolower(string $sourcestring [, string $encoding])
 * Description: Returns a lowercased version of $sourcestring
 * Source code: ext/mbstring/mbstring.c
 */

/*
 * Test basic functionality of mb_strtolower
 */
<<__EntryPoint>> function main() {
echo "*** Testing mb_strtolower() : basic functionality***\n";

$ascii_lower = 'abcdefghijklmnopqrstuvwxyz';
$ascii_upper = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ';
$greek_lower = base64_decode('zrHOss6zzrTOtc62zrfOuM65zrrOu868zr3Ovs6/z4DPgc+Dz4TPhc+Gz4fPiM+J');
$greek_upper = base64_decode('zpHOks6TzpTOlc6WzpfOmM6ZzprOm86czp3Ons6fzqDOoc6jzqTOpc6mzqfOqM6p');

echo "\n-- ASCII String --\n";
$ascii = mb_strtolower($ascii_upper);
var_dump($ascii);

if($ascii == $ascii_lower) {
    echo "Correctly converted\n";
} else {
    echo "Incorrectly converted\n";
}

echo "\n-- Multibyte String --\n";
$mb = mb_strtolower($greek_upper, 'UTF-8');
var_dump(base64_encode($mb));

if ($mb == $greek_lower) {
    echo "Correctly converted\n";
} else {
    echo "Incorreclty converted\n";
}

echo "Done";
}
