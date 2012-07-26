#!/bin/bash
# cb2get.sh - CodeBreaker PS2 Download Script
# Copyright (C) 2007-2008 misfire <misfire@xploderfreax.de>
#
# Downloads all Day1 Code Saves, Game Saves, and Firmware
# Updates for CodeBreaker PS2 from the CMGSCCC website.
#
# Tested under Ubuntu 8.04.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

# This script's version
VERSION=1.51

# The URLs to get the files from
URL_ROOT=http://www.cmgsccc.com/ps2/
URL_DAY1=${URL_ROOT}day1/cb.php
URL_SAVES=${URL_ROOT}saves/cb.php
URL_UPDATES=${URL_ROOT}updates.php

# HTTP User-Agent
AGENT="PS2NET Client"

# Location of some executables
CURL=/usr/bin/curl
SORT=/usr/bin/sort

# Downloads all files based on the IDs in an index file
# $1 - URL of PHP index file
# $2 - Optional GET parameter
function download {
	# Download index file, sort lines by ID (ascending)
	echo "Index file"
	$CURL -0 -A "$AGENT" -H "Accept:" $1$2 2>&- | $SORT -n > index.txt

	# Parse line by line
	while read line; do
		# Build URL
		id=$(echo $line | awk '{ print $1 }')
		url=${1}?did=$id

		# Get actual file name from HTTP header
		# We are looking for such a line:
		# Content-Disposition: attachment; filename=jak3.cbc
		fname=$($CURL -0 -A "$AGENT" -H "Accept:" -I $url 2>&- | grep -i "content-disposition" | tr -d '\r')
		fname=${fname#*filename=}

		# Download file
		if [ $fname ]; then
			size=$(echo $line | awk '{ print $2 }')
			echo -n "[$id] $fname ($size) ... "

			# Check if the file has been downloaded already
			download=1
			[ -e $fname ] && [ $(stat -c%s $fname) = $size ] && download=$?

			if [ $download = 1 ]; then
				$CURL -0 -A "$AGENT" -H "Accept:" --retry 3 -o hello.cmx $url 2>&-
				if [ $? = 0 ]; then echo "OK."; else echo "Error."; fi
				mv hello.cmx $fname
			else
				echo "Up to date."
			fi
		else
			echo "[$id] ERROR: No file name"
		fi
	done < index.txt
}

# Show banner
echo "CodeBreaker PS2 Download Script v$VERSION"
echo "Copyright (C) 2007-2008 misfire"
echo ""

# Check if executables are installed
[ -x $CURL ] || { echo "ERROR: Can't find curl executable"; exit 1; }
[ -x $SORT ] || { echo "ERROR: Can't find sort executable"; exit 1; }

# Download Day1 Code Saves (*.cbc)
echo "Downloading Day1 Code Saves..."
[ -d day1 ] || mkdir day1
cd day1
download $URL_DAY1 "?date=2007-04-03"
cd ..

# Download Game Saves (*.cbs)
echo "Downloading Game Saves..."
[ -d saves ] || mkdir saves
cd saves
download $URL_SAVES
cd ..

# Download Firmware Updates (*.bin)
echo "Downloading Firmware Updates..."
[ -d updates ] || mkdir updates
cd updates
download $URL_UPDATES

echo "Done!"

