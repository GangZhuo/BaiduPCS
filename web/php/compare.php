/*
  execute `pcs compare` on browser.
*/
<?php
require_once(__DIR__.DIRECTORY_SEPARATOR.'common.php');
require_once(APP_ROOT.'config.php');

show_as_text();

$local_path = '';
$remote_path = '';

system("pcs compare --config=$pcs_config_file $local_path $remote_path");
