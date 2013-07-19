<?php
/*
 * /ps2/day1/cb.php
 *
 * Mimics the functionality of the original PHP script for Day1 Code Saves from
 * http://www.cmgsccc.com/ps2/day1/cb.php
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

include "../common.php";

// Gets a Day1 file's game title.
function day1_gametitle($filename)
{
	if (DAY1_SHOW_FILENAME)
		return $filename;

	if ($fp = fopen($filename, "rb")) {
		$id = fread($fp, 4);
		if ($id == "CFU\x1" && filesize($filename) > 344) { // Day1 V8+ files
			fseek($fp, 0x108);
			$gametitle = cstr(fread($fp, 72)); // There must be a better way to do this?
		} else if (filesize($filename) > 64) { // Day1 V7 files
			fseek($fp, 0);
			$gametitle = cstr(fread($fp, 64)) . " (V7)";
		}
		fclose($fp);
	}

	return $gametitle;
}

// Outputs a formatted list of Day1 files.
function day1_index(&$day1_arr)
{
	header("Content-type: text/plain"); // Provide MIME type
	$date = date("Y-m-d"); // Today's date
	$numfiles = count($day1_arr);

	for ($i = 0; $i < $numfiles; $i++) {
		$filename = $day1_arr[$i];
		$gametitle = day1_gametitle($filename);
		// Format of one line:
		// [ID] [TAB] [FILESIZE] [TAB] [DATE] [TAB] [GAME TITLE] [LF]
		echo "$i\t" . filesize($filename) . "\t$date\t$gametitle\n";
	}
}

// Entry point

$day1_arr = filesrv_get("cbc");

// Handle GET request
if (isset($_GET['did']))
	filesrv_download($_GET['did'], $day1_arr);
else if (isset($_GET['info']))
	filesrv_info(count($day1_arr));
else // Don't care about $_GET['date']
	day1_index($day1_arr);
?>
