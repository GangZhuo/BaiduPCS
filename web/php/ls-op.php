<?php
/*
  execute `pcs ls-op` on browser.
*/
require_once(__DIR__.DIRECTORY_SEPARATOR.'common.php');
require_once(APP_ROOT.'config.php');

show_as_text();

system("pcs ls-op --config=$pcs_config_file");

