<?php
<<__EntryPoint>> function main() {
$v1 = headers_sent();
$v2 = headers_list();
var_dump(header("HTTP 1.0", true, 200));

var_dump($v1);
var_dump($v2);

var_dump(header(""));
var_dump(header("", true));
var_dump(headers_sent());
var_dump(headers_list());
var_dump(header("HTTP blah"));
var_dump(header("HTTP blah", true));
var_dump(headers_sent());
var_dump(headers_list());

echo "Done\n";
}
