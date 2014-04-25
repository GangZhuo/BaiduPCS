/*
  print the output of command `pcs ls-op --config=/etc/pcs/default.json` to the browser.
*/
<?php
header('Content-type: text/plain');
header('Cache-Control: no-cache, no-store, max-age=0, must-revalidate');
header('Expires: Mon, 26 Jul 1997 05:00:00 GMT');
header('Pragma: no-cache');

system("pcs ls-op --config=/etc/pcs/default.json");

