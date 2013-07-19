<?php
/*
 * /ps2/common.php
 *
 * Global defines und functions
 *
 * Copyright (C) 2007-2008 misfire <misfire@xploderfreax.de>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Global defines

// File server's version
define("VERSION", "1.2");

// Show file names instead of game titles in CB's Day1 file menu
define("DAY1_SHOW_FILENAME", FALSE);

// Show file names instead of game titles in CB's Device Manager
define("SAVES_SHOW_FILENAME", FALSE);

// "?info" shows Web server information
define("SHOW_WEBSERVER_INFO", FALSE);


// Global functions

// Removes trailing zeros from a string.
function cstr($str)
{
	return substr($str, 0, strpos($str, "\0"));
}

// Returns a file name's extension.
function fileext($filename)
{
	$info = pathinfo($filename);
	return strtolower($info['extension']);
}

// Gets all files with a specified extension in the current directory.
function filesrv_get($ext)
{
	$file_arr = NULL;

	if ($dirhandle = opendir('.')) {
		while ($filename = readdir($dirhandle)) {
			if (is_file($filename) && fileext($filename) == $ext)
				$file_arr[] = $filename;
		}
		closedir($dirhandle);
	}

	return $file_arr;
}

// Download a file by its ID.
function filesrv_download($did, &$file_arr)
{
	if (!count($file_arr))
		exit("Error: No downloadable files in directory");

	// Check download ID
	if (is_numeric($did) && (intval($did) >= 0) && (intval($did) < count($file_arr))) {
		$filename = $file_arr[intval($did)];
		if (file_exists($filename) && is_readable($filename)) {
			// Set up HTTP header for browsers
			header("Content-type: application/octet-stream");
			header("Content-Disposition: attachment; filename=$filename");
			header("Connection: close");
			// Output file
			readfile($filename);
		}
	} else exit("Error: Invalid download ID ($did), Min=0 Max=" . (count($file_arr)-1));
}

// Output some information about the file server.
function filesrv_info($numfiles)
{
	header("Content-type: text/plain");
	echo "CodeBreaker PS2 File Server v" . VERSION . "\n";
	echo "Copyright (C) 2007-2008 misfire\n";
	echo "Covered by the GNU Affero General Public License.\n\n";
	echo "Script: " . $_SERVER['PHP_SELF'] . "\n";
	echo "Currently hosting " . $numfiles . " file(s).\n\n";

	if (SHOW_WEBSERVER_INFO) {
		echo "Web server information\n";
		echo "  SERVER_ADDR     : " . $_SERVER['SERVER_ADDR'] . "\n";
		echo "  SERVER_NAME     : " . $_SERVER['SERVER_NAME'] . "\n";
		echo "  SERVER_SOFTWARE : " . $_SERVER['SERVER_SOFTWARE'] . "\n";
		echo "  SERVER_PROTOCOL : " . $_SERVER['SERVER_PROTOCOL'] . "\n";
		echo "  REMOTE_ADDR     : " . $_SERVER['REMOTE_ADDR'] . "\n";
		echo "  REMOTE_PORT     : " . $_SERVER['REMOTE_PORT'] . "\n";
	}
}
?>
