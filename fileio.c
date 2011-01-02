/*
 * File I/O functions
 *
 * Copyright (C) 2006-2011 Mathias Lafeldt <misfire@debugon.org>
 * All rights reserved.
 *
 * This file is part of cb2util, the CodeBreaker PS2 File Utility.
 *
 * cb2util is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * cb2util is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int read_file(uint8_t **buf, size_t *size, const char *path)
{
	FILE *fp;

	fp = fopen(path, "rb");
	if (fp == NULL)
		return -1;

	fseek(fp, 0, SEEK_END);
	*size = ftell(fp);

	*buf = malloc(*size);
	if (*buf == NULL) {
		fclose(fp);
		return -1;
	}

	fseek(fp, 0, SEEK_SET);
	if (fread(*buf, *size, 1, fp) != 1) {
		fclose(fp);
		return -1;
	}

	fclose(fp);
	return 0;
}

int write_file(const uint8_t *buf, size_t size, const char *path)
{
	FILE *fp;

	fp = fopen(path, "wb");
	if (fp == NULL)
		return -1;

	if (fwrite(buf, size, 1, fp) != 1) {
		fclose(fp);
		return -1;
	}

	fclose(fp);
	return 0;
}
