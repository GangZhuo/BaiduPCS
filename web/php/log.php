/*
  view log on browser.
*/
<?php
require_once(__DIR__.DIRECTORY_SEPARATOR.'common.php');
require_once(APP_ROOT.'config.php');

show_as_text();

$n = isset($_GET['n']) && !empty($_GET['n']) ? intval($_GET['n']) : 10;

system("tail -$n $pcs_log_file");

