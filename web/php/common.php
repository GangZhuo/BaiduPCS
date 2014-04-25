<?php
define('APP_ROOT', dirname(__FILE__).DIRECTORY_SEPARATOR);

function show_as_text()
{
	header('Content-type: text/plain');
	header('Cache-Control: no-cache, no-store, max-age=0, must-revalidate');
	header('Expires: Mon, 26 Jul 1997 05:00:00 GMT');
	header('Pragma: no-cache');
}
