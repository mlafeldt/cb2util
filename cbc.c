/*
 * CBC file handling
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
#include "cheats.h"
#include "fileio.h"

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

static const char *cbc_usage =
	"usage: cb2util cbc [-c|-d] <file>...\n"
	"   or: cb2util cbc -7 [-d] <file>...\n\n"
	"    no option         extract cheats\n\n"
	"    -7                files are in CBC v7 format\n"
	"    -c, --check       check RSA signature\n"
	"    -d, --decrypt     decrypt extracted cheats\n";

int cmd_cbc(int argc, char **argv)
{
	const char *shortopts = "7cd::h";
	const struct option longopts[] = {
		{ "check", no_argument, NULL, 'c' },
		{ "decrypt", optional_argument, NULL, 'd' },
		{ "help", no_argument, NULL, 'h' },
		{ NULL, 0, NULL, 0 }
	};
	enum {
		MODE_EXTRACT,
		MODE_CHECK
	};
	int mode = MODE_EXTRACT;
	int decrypt = DECRYPT_OFF;
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
			if (optarg != NULL) {
				if (!strcmp(optarg, "auto")) {
					decrypt = DECRYPT_AUTO;
				} else if (!strcmp(optarg, "force")) {
					decrypt = DECRYPT_FORCE;
				} else {
					fprintf(stderr, "%s\n", cbc_usage);
					return 1;
				}
			} else {
				decrypt = DECRYPT_AUTO;
			}
			break;
		case 'h':
			printf("%s\n", cbc_usage);
			return 0;
		default:
			fprintf(stderr, "%s\n", cbc_usage);
			return 1;
		}
	}

	if (optind == argc) {
		fprintf(stderr, "%s\n", cbc_usage);
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

			cb_crypt_data(hdr->data, datalen);

			if (strcmp(hdr->gametitle, (char*)hdr->data)) {
				fprintf(stderr, "%s: invalid CBC v7 header\n", filename);
				errors++;
				goto next_file;
			}

			if (numcodes)
				printf("\n");
			numcodes += extract_cheats(stdout, hdr->data, datalen, decrypt);
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
				ret = cb_verify_signature(hdr->rsasig, buf + CBC_HASH_OFFSET,
						buflen - CBC_HASH_OFFSET);
				printf("%s: %s\n", filename, ret ? "FAILED" : "OK");
				errors += ret;
			} else if (mode == MODE_EXTRACT) {
				cb_crypt_data(buf + hdr->dataoff, datalen);
				if (numcodes)
					printf("\n");
				numcodes += extract_cheats(stdout, buf + hdr->dataoff,
						datalen, decrypt);
			}
		}
next_file:
		free(buf);
	}

	return errors ? 1 : 0;
}
