/*
 * "cheats" file handling
 *
 * Copyright (C) 2006-2010 Mathias Lafeldt <misfire@debugon.org>
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
#include <string.h>
#include <getopt.h>
#include "cb2_crypto.h"

/* "cheats" file header */
typedef struct {
	uint32_t	fileid;		/* must be CHEATS_FILE_ID */
	uint32_t	unknown;	/* always 0x00010000 */
	uint8_t		data[0];
} cheats_hdr_t;

#define CHEATS_FILE_ID		0x00554643 /* "CFU\0" */


int extract_cheats(FILE *fp, const uint8_t *buf, int buflen, int decrypt)
{
	char *p;
	int off = 0;
	uint16_t numdesc, numlines;
	uint32_t code[2];
	int totcodes = 0;

	while (off < buflen) {
		/* Check for end marker */
		if (*(uint16_t*)&buf[off] == 0xFFFF)
			break;

		/* Reset code encryption */
		CBReset();

		/*
		 * Process game title
		 * Example: "007 Agent Under Fire\0"
		 */
		p = (char*)&buf[off];
		fprintf(fp, "\"%s\"\n", p);
		off += strlen(p) + 1;

		numdesc = *(uint16_t*)&buf[off];
		off += sizeof(uint16_t);

		/*
		 * Process code description(s)
		 * Example: "Infinite Ammo\0"
		 */
		while (numdesc--) {
			p = (char*)&buf[off];
			off += strlen(p) + 1;
			/* Skip desc type, we won't use it */
			off++;

			numlines = *(uint16_t*)&buf[off];
			off += sizeof(uint16_t);

			/* If there is no code line, it's just a heading */
			if (!numlines) {
				fprintf(fp, "*%s\n", p);
			} else {
				fprintf(fp, "%s\n", p);
				totcodes++;

				/*
				 * Process code line(s)
				 * Example: 1A3EDED4 000003E7
				 */
				while (numlines--) {
					code[0] = *(uint32_t*)&buf[off];
					off += sizeof(uint32_t);
					code[1] = *(uint32_t*)&buf[off];
					off += sizeof(uint32_t);

					if (decrypt)
						CBDecryptCode(&code[0], &code[1]);

					fprintf(fp, "%08X %08X\n", code[0], code[1]);
				}
			}
		}
	}

	/* Return total number of codes */
	return totcodes;
}
