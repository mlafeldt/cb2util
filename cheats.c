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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <libcheats.h>
#include "cb2_crypto.h"
#include "cheats.h"
#include "compress.h"
#include "fileio.h"

/* "cheats" file header */
typedef struct {
	uint32_t	fileid;		/* must be CHEATS_FILE_ID */
	uint32_t	unknown;	/* always 0x00010000 */
	uint8_t		data[0];
} cheats_hdr_t;

#define CHEATS_FILE_ID		0x00554643 /* "CFU\0" */

/* standard header */
static const cheats_hdr_t _cheats_hdr = {
	.fileid		= CHEATS_FILE_ID,
	.unknown	= 0x00010000
};

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

static const char *cheats_usage =
	"usage: cb2util cheats [-d[mode]] <file>...\n"
	"   or: cb2util cheats -c <infile> <outfile>...\n\n"
	"    no option\n"
	"        extract cheats\n\n"
	"    -d[mode], --decrypt[=mode]\n"
	"        decrypt extracted cheats\n"
	"        mode can be \"auto\" (default) or \"force\"\n\n"
	"    -c, --compile\n"
	"        compile text to cheats file\n";

int cmd_cheats(int argc, char **argv)
{
	const char *shortopts = "cd::h";
	const struct option longopts[] = {
		{ "compile", no_argument, NULL, 'c' },
		{ "decrypt", optional_argument, NULL, 'd' },
		{ "help", no_argument, NULL, 'h' },
		{ NULL, 0, NULL, 0 }
	};
	enum {
		MODE_COMPILE,
		MODE_EXTRACT
	};
	int mode = MODE_EXTRACT;
	int decrypt = DECRYPT_OFF;
	int numcodes = 0;
	int errors = 0;
	int ret;

	while ((ret = getopt_long(argc, argv, shortopts, longopts, NULL)) != -1) {
		switch (ret) {
		case 'c':
			mode = MODE_COMPILE;
			break;
		case 'd':
			if (optarg != NULL) {
				if (!strcmp(optarg, "auto")) {
					decrypt = DECRYPT_AUTO;
				} else if (!strcmp(optarg, "force")) {
					decrypt = DECRYPT_FORCE;
				} else {
					fprintf(stderr, "%s\n", cheats_usage);
					return 1;
				}
			} else {
				decrypt = DECRYPT_AUTO;
			}
			break;
		case 'h':
			printf("%s\n", cheats_usage);
			return 0;
		default:
			fprintf(stderr, "%s\n", cheats_usage);
			return 1;
		}
	}

	if ((optind == argc) || (mode == MODE_COMPILE && (argc - optind) & 1)) {
		fprintf(stderr, "%s\n", cheats_usage);
		return 1;
	}

	while (optind < argc) {
		const char *infile = argv[optind++];
		const char *outfile = mode == MODE_COMPILE ? argv[optind++] : NULL;
		uint8_t *buf = NULL;
		size_t buflen;

		if (mode == MODE_COMPILE) {
			FILE *fp = NULL;
			cheats_t cheats;
			uint8_t *pack = NULL;
			unsigned long packlen;

			cheats_init(&cheats);
			if (cheats_read_file(&cheats, infile) != CHEATS_TRUE) {
				fprintf(stderr, "%s: line: %i\nerror: %s\n", infile,
					cheats.error_line, cheats.error_text);
				errors++;
				goto compile_end;
			}
			if (compile_cheats(&buf, &buflen, &cheats)) {
				fprintf(stderr, "%s: compile error\n", infile);
				errors++;
				goto compile_end;
			}
			if (zlib_compress(&pack, &packlen, buf, buflen)) {
				fprintf(stderr, "%s: compress error\n", infile);
				errors++;
				goto compile_end;
			}

			fp = fopen(outfile, "wb");
			if (fp == NULL) {
				fprintf(stderr, "%s: write error\n", outfile);
				errors++;
				goto compile_end;
			}

			fwrite(&_cheats_hdr, sizeof(_cheats_hdr), 1, fp);
			fwrite(pack, packlen, 1, fp);
			fclose(fp);
compile_end:
			cheats_destroy(&cheats);
			if (pack != NULL)
				free(pack);
		} else if (mode == MODE_EXTRACT) {
			cheats_hdr_t *hdr;
			int datalen;
			uint8_t *unpack;
			unsigned long unpacklen;

			if (read_file(&buf, &buflen, infile)) {
				fprintf(stderr, "%s: read error\n", infile);
				errors++;
				continue;
			}

			datalen = buflen - sizeof(cheats_hdr_t);
			if (datalen < 0) {
				fprintf(stderr, "%s: not a cheats file\n", infile);
				errors++;
				goto next_file;
			}

			hdr = (cheats_hdr_t*)buf;
			if (hdr->fileid != CHEATS_FILE_ID) {
				fprintf(stderr, "%s: invalid cheats file ID\n", infile);
				errors++;
				goto next_file;
			}

			if (zlib_uncompress(&unpack, &unpacklen, hdr->data, datalen)) {
				fprintf(stderr, "%s: uncompress error\n", infile);
				errors++;
				goto next_file;
			}

			if (numcodes)
				printf("\n");
			numcodes += extract_cheats(stdout, unpack, unpacklen, decrypt);
			free(unpack);
		}
next_file:
		free(buf);
	}

	return errors ? 1 : 0;
}
