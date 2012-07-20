/*
 * CBC file handling
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
#include "cheats_common.h"
#include "cb2_crypto.h"
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

/* when compiling CBC v8+ files, use this message as the signature by default */
#define CBC_BANNER	"Created with cb2util - http://mlafeldt.github.com/cb2util"

/* CBC v7 file header */
typedef struct {
	char		gametitle[64];
	uint8_t		data[0];
} cbc7_hdr_t;

static const char *cbc_usage =
	"usage: cb2util cbc [-d[mode] | -v] <file>...\n"
	"   or: cb2util cbc -7 [-d[mode]] <file>...\n"
	"   or: cb2util cbc [-b <banner>] -c <infile> <outfile>...\n"
	"   or: cb2util cbc -7 -c <infile> <outfile>...\n\n"
	"    no option\n"
	"        extract cheats\n\n"
	"    -d[mode], --decrypt[=mode]\n"
	"        decrypt extracted cheats\n"
	"        mode can be \"auto\" (default) or \"force\"\n\n"
	"    -v, --verify\n"
	"        verify RSA signature\n\n"
	"    -c, --compile\n"
	"        compile text to CBC file\n\n"
	"    -b, --banner <banner>\n"
	"        custom banner inserted into compiled CBC v8+ files\n\n"
	"    -7\n"
	"        files are in CBC v7 format\n";

int cmd_cbc(int argc, char **argv)
{
	const char *shortopts = "7b:cd::hv";
	const struct option longopts[] = {
		{ "banner", required_argument, NULL, 'b' },
		{ "compile", no_argument, NULL, 'c' },
		{ "decrypt", optional_argument, NULL, 'd' },
		{ "help", no_argument, NULL, 'h' },
		{ "verify", no_argument, NULL, 'v' },
		{ NULL, 0, NULL, 0 }
	};
	enum {
		MODE_COMPILE,
		MODE_EXTRACT,
		MODE_VERIFY
	};
	int mode = MODE_EXTRACT;
	int decrypt = DECRYPT_OFF;
	char *banner = NULL;
	int v7 = 0;
	int numcodes = 0;
	int errors = 0;
	int ret;

	while ((ret = getopt_long(argc, argv, shortopts, longopts, NULL)) != -1) {
		switch (ret) {
		case '7':
			v7 = 1;
			break;
		case 'b':
			banner = optarg;
			break;
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
					fprintf(stderr, "%s\n", cbc_usage);
					return 1;
				}
			} else {
				decrypt = DECRYPT_AUTO;
			}
			break;
		case 'v':
			mode = MODE_VERIFY;
			break;
		case 'h':
			printf("%s\n", cbc_usage);
			return 0;
		default:
			fprintf(stderr, "%s\n", cbc_usage);
			return 1;
		}
	}

	if ((optind == argc) || (mode == MODE_COMPILE && (argc - optind) & 1)) {
		fprintf(stderr, "%s\n", cbc_usage);
		return 1;
	}

	if (v7) {
		if (mode == MODE_VERIFY) {
			fprintf(stderr, "CBC v7 files don't have a signature\n");
			return 1;
		}
		if (mode == MODE_COMPILE && banner != NULL) {
			fprintf(stderr, "CBC v7 files can't have a custom banner\n");
			return 1;
		}
	}

	while (optind < argc) {
		const char *infile = argv[optind++];
		const char *outfile = mode == MODE_COMPILE ? argv[optind++] : NULL;
		uint8_t *buf = NULL;
		size_t buflen;

		if (mode == MODE_COMPILE) {
			FILE *fp = NULL;
			cheats_t cheats;

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

			fp = fopen(outfile, "wb");
			if (fp == NULL) {
				fprintf(stderr, "%s: write error\n", outfile);
				errors++;
				goto compile_end;
			}

			if (v7) {
				cbc7_hdr_t hdr;
				memset(&hdr, 0, sizeof(hdr));
				strncpy(hdr.gametitle, GAMES_FIRST(&cheats.games)->title, 64);
				fwrite(&hdr, sizeof(hdr), 1, fp);
			} else {
				cbc_hdr_t hdr;
				memset(&hdr, 0, sizeof(hdr));
				hdr.fileid = CBC_FILE_ID;
				strncpy((char*)hdr.rsasig, banner != NULL ? banner : CBC_BANNER, 256);
				hdr.cbvers = 0x0800;
				strncpy(hdr.gametitle, GAMES_FIRST(&cheats.games)->title, 72);
				hdr.dataoff = sizeof(hdr);
				fwrite(&hdr, sizeof(hdr), 1, fp);
			}

			cb_crypt_data(buf, buflen);
			fwrite(buf, buflen, 1, fp);
			fclose(fp);
compile_end:
			cheats_destroy(&cheats);
		} else {
			if (read_file(&buf, &buflen, infile)) {
				fprintf(stderr, "%s: read error\n", infile);
				errors++;
				continue;
			}

			if (v7) {
				cbc7_hdr_t *hdr = (cbc7_hdr_t*)buf;
				int datalen = buflen - sizeof(cbc7_hdr_t);

				if (datalen < 0) {
					fprintf(stderr, "%s: not a CBC v7 file\n", infile);
					errors++;
					goto next_file;
				}

				cb_crypt_data(hdr->data, datalen);

				if (strcmp(hdr->gametitle, (char*)hdr->data)) {
					fprintf(stderr, "%s: invalid CBC v7 header\n", infile);
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
					fprintf(stderr, "%s: not a CBC file\n", infile);
					errors++;
					goto next_file;
				}
				if (hdr->fileid != CBC_FILE_ID) {
					fprintf(stderr, "%s: invalid CBC file ID\n", infile);
					errors++;
					goto next_file;
				}

				if (mode == MODE_VERIFY) {
					ret = cb_verify_signature(hdr->rsasig, buf + CBC_HASH_OFFSET,
							buflen - CBC_HASH_OFFSET);
					printf("%s: %s\n", infile, ret ? "FAILED" : "OK");
					errors += ret;
				} else if (mode == MODE_EXTRACT) {
					cb_crypt_data(buf + hdr->dataoff, datalen);
					if (numcodes)
						printf("\n");
					numcodes += extract_cheats(stdout, buf + hdr->dataoff,
							datalen, decrypt);
				}
			}
		}
next_file:
		free(buf);
	}

	return errors ? 1 : 0;
}
