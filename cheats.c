/*
 * "cheats" file handling
 *
 * Copyright (C) 2006-2013 Mathias Lafeldt <mathias.lafeldt@gmail.com>
 *
 * This file is part of cb2util, the CodeBreaker PS2 File Utility.
 *
 * cb2util is licensed under the terms of the MIT License. See LICENSE file.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "cheats_common.h"
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

	return !!errors;
}
