/*
 * Extract and compile cheats
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cheats_common.h"
#include "cb2_crypto.h"

int extract_cheats(FILE *fp, const uint8_t *buf, int buflen, int decrypt)
{
	char *p;
	int off = 0;
	uint16_t numdesc, numlines;
	uint32_t code[2];
	int totcodes = 0;
	int beefcodf;
	int fix_beef;

	while (off < buflen) {
		/* Check for end marker */
		if (*(uint16_t*)&buf[off] == 0xFFFF)
			break;

		/* Reset code encryption */
		cb_reset();
		beefcodf = 0;
		fix_beef = 0;

		if (totcodes)
			fprintf(fp, "\n");

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
			/* Skip description tag, we won't use it */
			off++;

			numlines = *(uint16_t*)&buf[off];
			off += sizeof(uint16_t);

			fprintf(fp, "%s\n", p);

			if (numlines) {
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

					/*
					 * decrypt code
					 */
					if (decrypt == DECRYPT_FORCE)
						cb_decrypt_code(&code[0], &code[1]);
					else if (decrypt == DECRYPT_AUTO)
						cb_decrypt_code2(&code[0], &code[1]);

					/*
					 * discard beefcode and other junk
					 */
					if (beefcodf) {
						beefcodf = 0;
						continue;
					}
					if (fix_beef) {
						if (code[0] == 0x000FFFFE || code[0] == 0x000FFFFF) {
							fix_beef--;
							continue;
						} else {
							fix_beef = 0;
						}
					}
					if ((code[0] & 0xFFFFFFFE) == 0xBEEFC0DE) {
						beefcodf = code[0] & 1;
						fix_beef = 2;
						continue;
					} else {
						beefcodf = 0;
						fix_beef = 0;
					}

					/*
					 * finally, output result
					 */
					fprintf(fp, "%08X %08X\n", code[0], code[1]);
				}
			}
		}
	}

	/* Return total number of codes */
	return totcodes;
}

int compile_cheats(uint8_t **dst, size_t *dstlen, const cheats_t *cheats)
{
	game_t *game;
	cheat_t *cheat;
	code_t *code;
	uint8_t *p;
	uint16_t *numdesc;
	uint16_t *numlines;
	uint8_t *tag;
	size_t used;

	*dstlen = 1024 * 1024;
	*dst = malloc(*dstlen);
	if (*dst == NULL)
		return -1;
	p = *dst;

	GAMES_FOREACH(game, &cheats->games) {
		p += sprintf((char*)p, "%s%c", game->title, '\0');
		numdesc = (uint16_t*)p;
		*numdesc = 0;
		p += sizeof(uint16_t);

		CHEATS_FOREACH(cheat, &game->cheats) {
			p += sprintf((char*)p, "%s%c", cheat->desc, '\0');
			tag = p++;
			numlines = (uint16_t*)p;
			*numlines = 0;
			p += sizeof(uint16_t);

			CODES_FOREACH(code, &cheat->codes) {
				*(uint32_t*)p = code->addr;
				p += sizeof(uint32_t);
				*(uint32_t*)p = code->val;
				p += sizeof(uint32_t);

				(*numlines)++;
			}

			/* descriptions w/o codes get a special tag */
			*tag = *numlines ? 0 : 4;

			(*numdesc)++;
		}

		/* give us some more memory */
		used = p - *dst;
		if (used > (*dstlen / 2)) {
			*dstlen *= 2;
			*dst = realloc(*dst, *dstlen);
			if (*dst == NULL)
				return -1;
			p = *dst + used;
		}
	}

	*(uint16_t*)p = 0xffff;
	p += sizeof(uint16_t);

	*dstlen = p - *dst;

	return 0;
}
