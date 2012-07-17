/*
 * "cheats" file handling
 *
 * Copyright (C) 2006-2012 Mathias Lafeldt <mathias.lafeldt@gmail.com>
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

#ifndef _CHEATS_H_
#define _CHEATS_H_

#include <stdint.h>
#include <stdio.h>
#include <libcheats.h>

enum {
	DECRYPT_OFF,
	DECRYPT_FORCE,
	DECRYPT_AUTO
};

int extract_cheats(FILE *fp, const uint8_t *buf, int buflen, int decrypt);
int compile_cheats(uint8_t **dst, size_t *dstlen, const cheats_t *cheats);

#endif /* _CHEATS_H_ */
