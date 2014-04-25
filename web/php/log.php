/*
  print the output of command `tail -10 /var/log/pcs.log` to the browser.
*/
<?php
header('Content-type: text/plain');
header('Cache-Control: no-cache, no-store, max-age=0, must-revalidate');
header('Expires: Mon, 26 Jul 1997 05:00:00 GMT');
header('Pragma: no-cache');

$n = isset($_GET['n']) && !empty($_GET['n']) ? intval($_GET['n']) : 10;
//echo "$n";
system("tail -$n /var/log/pcs.log");

