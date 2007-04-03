/*
 * cb2util/src/cbc.c
 *
 * CBC file handling
 *
 * Copyright (C) 2006-2007 misfire
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

#include <stdio.h>
#include <string.h>
#include "cbc.h"

/* Print CBC file header (V8+) */
void CbcPrintHeader(const cbc_hdr_t *hdr)
{
	printf("CBC File Header\n");
	printf("  File ID             : 0x%08X\n", hdr->fileid);
	printf("  RSA signature       : %02X %02X %02X %02X ... %02X %02X %02X %02X\n",
		hdr->rsasig[0], hdr->rsasig[1], hdr->rsasig[2], hdr->rsasig[3],
		hdr->rsasig[252], hdr->rsasig[253], hdr->rsasig[254], hdr->rsasig[255]);
	printf("  Required CB version : %d.%d\n",
		(hdr->cbvers >> 8) & 0xFF, (hdr->cbvers & 0xFF));
	printf("  Game title          : %s\n", hdr->gametitle);
	printf("  Data section offset : 0x%08X\n", hdr->dataoff);
}

/* Check whether it's a valid V7 header */
int CbcIsV7Header(const u8 *hdr, const u8 *data)
{
	/* Compare game title in header with game title in data section */
	return !strcmp(hdr, data);
}

/* Extract cheats from data section and print them to stdout */
int CbcExtractCheats(const u8 *data, int datasize, int dodecrypt)
{
	char *p;
	int off = 0;
	u16 numdesc, numlines;
	u32 code[2];
	int totcodes = 0;

	/* Parse data section, print results to stdout */
	while (off < datasize) {
		/* Check for end marker */
		if (*(u32*)&data[off] == 0xFFFFFFFF)
			break;

		/* Reset CB encryption */
		CBReset();

		/* Process game title
		   Example: "007 Agent Under Fire" */
		p = (char*)&data[off];
		printf("\"%s\"\n", p);
		off += strlen(p) + 1;

		numdesc = *(u16*)&data[off];
		off += sizeof(u16);

		/* Process code description(s)
		   Example: Infinite Ammo */
		while (numdesc--) {
			p = (char*)&data[off];
			off += strlen(p) + 1;
			/* Skip desc type, we won't use it */
			off++;

			numlines = *(u16*)&data[off];
			off += sizeof(u16);

			/* If there is no code line, it's just a heading
			   Example: *Action Mission Codes */
			if (!numlines)
				printf("*%s\n", p);
			else {
				printf("%s\n", p);
				totcodes++;

				/* Process code line(s)
				   Example: 1A3EDED4 000003E7 */
				while (numlines--) {
					code[0] = *(u32*)&data[off];
					off += sizeof(u32);
					code[1] = *(u32*)&data[off];
					off += sizeof(u32);

					/* Decrypt code */
					if (dodecrypt)
						CBDecryptCode(&code[0], &code[1]);

					printf("%08X %08X\n", code[0], code[1]);
				}
			}
		}
	}

	/* Return total number of codes */
	return totcodes;
}
