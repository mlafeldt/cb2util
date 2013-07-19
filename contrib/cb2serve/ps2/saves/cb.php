<?php
/*
 * /ps2/saves/cb.php
 *
 * Mimics the functionality of the original PHP script for Game Saves from
 * http://www.cmgsccc.com/ps2/saves/cb.php
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

// Gets a Game Saves's game title and note.
function saves_data($filename, &$gametitle, &$note)
{
	if ($fp = fopen($filename, "rb")) {
		$id = fread($fp, 4);
		if ($id == "CFU\x0" && filesize($filename) > 296) { // Game Save
			if (SAVES_SHOW_FILENAME) {
				$gametitle = $filename;
			} else {
				fseek($fp, 0x5C);
				$gametitle = cstr(fread($fp, 72));
			}
			fseek($fp, 0xA4);
			$note = cstr(fread($fp, 132));
		} else { // Not a Game Save
			$gametitle = $filename;
			$note = "N/A";
		}
		fclose($fp);
	}
}

// Outputs a formatted list of Game Saves.
function saves_index(&$saves_arr)
{
	header("Content-type: text/plain"); // Provide MIME type
	$date = date("Y-m-d"); // Today's date
	$numfiles = count($saves_arr);

	for ($i = 0; $i < $numfiles; $i++) {
		$filename = $saves_arr[$i];
		saves_data($filename, $gametitle, $note);
		// Format of one line:
		// [ID] [TAB] [FILESIZE] [TAB] [TAB] [GAME TITLE] [TAB] [NOTE] [LF]
		echo "$i\t" . filesize($filename) . "\t\t$gametitle\t$note\n";
	}
}

// Entry point

$saves_arr = filesrv_get("cbs");

// Handle GET request
if (isset($_GET['did']))
	filesrv_download($_GET['did'], $saves_arr);
else if (isset($_GET['info']))
	filesrv_info(count($saves_arr));
else
	saves_index($saves_arr);
?>
