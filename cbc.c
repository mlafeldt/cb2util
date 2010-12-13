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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "cb2_crypto.h"

/* CBC file header */
typedef struct {
	uint32_t	fileid;		/* must be CBC_FILE_ID */
	uint8_t		rsasig[256];	/* 2048-bit RSA signature */
	uint32_t	cbvers;		/* required CB version */
	char		gametitle[72];
	uint32_t	dataoff;	/* file offset of data section */
	uint32_t	zero;		/* always 0 */
	uint8_t		data[0];
} cbc_hdr_t;

#define CBC_FILE_ID	0x01554643	/* "CFU", 1 */

/* offset of data hashed for RSA signature */
#define CBC_HASH_OFFSET	0x00000104

/* CBC v7 file header */
typedef struct {
	char		gametitle[64];
	uint8_t		data[0];
} cbc7_hdr_t;


/* Extract cheats from data section and print them to stdout */
static int CbcExtractCheats(const uint8_t *data, int datasize, int dodecrypt)
{
	char *p;
	int off = 0;
	uint16_t numdesc, numlines;
	uint32_t code[2];
	int totcodes = 0;

	/* Parse data section, print results to stdout */
	while (off < datasize) {
		/* Check for end marker */
		if (*(uint32_t*)&data[off] == 0xFFFFFFFF)
			break;

		/* Reset CB encryption */
		CBReset();

		/* Process game title
		   Example: "007 Agent Under Fire" */
		p = (char*)&data[off];
		printf("\"%s\"\n", p);
		off += strlen(p) + 1;

		numdesc = *(uint16_t*)&data[off];
		off += sizeof(uint16_t);

		/* Process code description(s)
		   Example: Infinite Ammo */
		while (numdesc--) {
			p = (char*)&data[off];
			off += strlen(p) + 1;
			/* Skip desc type, we won't use it */
			off++;

			numlines = *(uint16_t*)&data[off];
			off += sizeof(uint16_t);

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
					code[0] = *(uint32_t*)&data[off];
					off += sizeof(uint32_t);
					code[1] = *(uint32_t*)&data[off];
					off += sizeof(uint32_t);

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

extern int read_file(uint8_t **buf, size_t *size, const char *path);

int cmd_cbc(int argc, char **argv)
{
	const char *usage = "cb2util cbc [-7] [-c|-d] <file>...";
	const char *shortopts = "7cd";
	const struct option longopts[] = {
		{ "check", no_argument, NULL, 'c' },
		{ "decrypt", no_argument, NULL, 'd' },
		{ NULL, 0, NULL, 0 }
	};
	enum {
		MODE_DEFAULT,
		MODE_CHECK,
		MODE_DECRYPT
	};
	int mode = MODE_DEFAULT;
	int v7 = 0;
	int numcodes = 0;
	int errors = 0;
	int ret;

	while ((ret = getopt_long(argc, argv, shortopts, longopts, NULL)) != -1) {
		switch (ret) {
		case '7':
			v7 = 1;
			break;
		case 'c':
			mode = MODE_CHECK;
			break;
		case 'd':
			mode = MODE_DECRYPT;
			break;
		default:
			fprintf(stderr, "usage: %s\n", usage);
			return 1;
		}
	}

	if (optind == argc) {
		fprintf(stderr, "usage: %s\n", usage);
		return 1;
	}
	if (v7 && mode == MODE_CHECK) {
		fprintf(stderr, "CBC v7 files don't have a signature\n");
		return 1;
	}

	while (optind < argc) {
		const char *filename = argv[optind++];
		uint8_t *buf;
		size_t buflen;

		if (read_file(&buf, &buflen, filename)) {
			fprintf(stderr, "%s: read error\n", filename);
			errors++;
			continue;
		}

		if (v7) {
			cbc7_hdr_t *hdr = (cbc7_hdr_t*)buf;
			int datalen = buflen - sizeof(cbc7_hdr_t);

			if (datalen < 0) {
				fprintf(stderr, "%s: not a CBC v7 file\n", filename);
				errors++;
				goto next_file;
			}

			CBCryptFileData(hdr->data, datalen);

			if (strcmp(hdr->gametitle, (char*)hdr->data)) {
				fprintf(stderr, "%s: invalid CBC v7 header\n", filename);
				errors++;
				goto next_file;
			}

			if (numcodes)
				printf("\n");
			numcodes += CbcExtractCheats(hdr->data, datalen, mode == MODE_DECRYPT);
		} else {
			cbc_hdr_t *hdr = (cbc_hdr_t*)buf;
			int datalen = buflen - hdr->dataoff;

			if (datalen < 0) {
				fprintf(stderr, "%s: not a CBC file\n", filename);
				errors++;
				goto next_file;
			}
			if (hdr->fileid != CBC_FILE_ID) {
				fprintf(stderr, "%s: invalid CBC file ID\n", filename);
				errors++;
				goto next_file;
			}

			if (mode == MODE_CHECK) {
				ret = CBVerifyFileSig(hdr->rsasig, buf + CBC_HASH_OFFSET,
						buflen - CBC_HASH_OFFSET, NULL, NULL);
				printf("%s: %s\n", filename, ret ? "FAILED" : "OK");
				errors += ret;
			} else {
				CBCryptFileData(buf + hdr->dataoff, datalen);
				if (numcodes)
					printf("\n");
				numcodes += CbcExtractCheats(buf + hdr->dataoff,
						datalen, mode == MODE_DECRYPT);
			}
		}
next_file:
		free(buf);
	}

	return errors ? 1 : 0;
}
