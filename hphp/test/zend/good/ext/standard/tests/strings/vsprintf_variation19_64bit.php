<?php
/* Prototype  : string vsprintf(string $format , array $args)
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/
<<__EntryPoint>> function main() {
echo "*** Testing vsprintf() : with  white spaces in format strings ***\n";

// initializing the format array
$formats = array(
  "% d  %  d  %   d",
  "% f  %  f  %   f",
  "% F  %  F  %   F",
  "% b  %  b  %   b",
  "% c  %  c  %   c",
  "% e  %  e  %   e",
  "% u  %  u  %   u",
  "% o  %  o  %   o",
  "% x  %  x  %   x",
  "% X  %  X  %   X",
  "% E  %  E  %   E"
);

// initializing the args array

$args_array = array(
  array(111, 222, 333),
  array(1.1, .2, -0.6),
  array(1.12, -1.13, +0.23),
  array(1, 2, 3),
  array(65, 66, 67),
  array(2e1, 2e-1, -2e1),
  array(-11, +22, 33),
  array(012, -02394, +02389),
  array(0x11, -0x22, +0x33),
  array(0x11, -0x22, +0x33),
  array(2e1, 2e-1, -2e1)
);

$counter = 1;
foreach($formats as $format) {
  echo"\n-- Iteration $counter --\n";
  var_dump( vsprintf($format, $args_array[$counter-1]) );
  $counter++;
}

echo "Done";
}
