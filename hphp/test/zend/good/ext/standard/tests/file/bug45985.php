<?php <<__EntryPoint>> function main() {
$file = __DIR__ . '/' . '__tmp_35740.dat';
file_put_contents($file, 'test');
$f = fopen($file, 'r');
touch($file);
fclose($f);
@unlink($file);
echo "ok";
}
