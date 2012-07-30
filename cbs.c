/*
 * CBS file handling
 *
 * Copyright (C) 2006-2012 Mathias Lafeldt <mathias.lafeldt@gmail.com>
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
#include "cb2_crypto.h"
#include "compress.h"
#include "fileio.h"

/* CBS file header */
typedef struct {
	uint32_t	fileid;		/* must be CBS_FILE_ID */
	uint32_t	unknown_8000;	/* always 8000 */
	uint32_t	dataoff;	/* file offset of data section */
	uint32_t	unpacklen;	/* decompressed size */
	uint32_t	packlen;	/* compressed size */
	char		root[32];	/* root/ID/folder name */
	uint8_t		unknown_40[40];
	char		gametitle[72];
	char		note[132];	/* game save description */
	uint8_t		data[0];
} cbs_hdr_t;

#define CBS_FILE_ID	0x00554643	/* "CFU\0" */

static const char *cbs_usage = "TODO";

int cmd_cbs(int argc, char **argv)
{
	const char *shortopts = "h";
	const struct option longopts[] = {
		{ "help", no_argument, NULL, 'h' },
		{ NULL, 0, NULL, 0 }
	};
	enum {
		MODE_EXTRACT,
		MODE_CHECK
	};
	int mode = MODE_EXTRACT;
	int errors = 0;
	int ret;

	while ((ret = getopt_long(argc, argv, shortopts, longopts, NULL)) != -1) {
		switch (ret) {
		case 'h':
			printf("%s\n", cbs_usage);
			return 0;
		default:
			fprintf(stderr, "%s\n", cbs_usage);
			return 1;
		}
	}

	if (optind == argc) {
		fprintf(stderr, "%s\n", cbs_usage);
		return 1;
	}

	while (optind < argc) {
		const char *filename = argv[optind++];
		uint8_t *buf = NULL;
		size_t buflen;

		if (read_file(&buf, &buflen, filename)) {
			fprintf(stderr, "%s: read error\n", filename);
			errors++;
			continue;
		}

		if (1) {
			cbs_hdr_t *hdr = (cbs_hdr_t*)buf;
			int datalen = buflen - hdr->dataoff;

			if (datalen < 0) {
				fprintf(stderr, "%s: not a CBS file\n", filename);
				errors++;
				goto next_file;
			}
			if (hdr->fileid != CBS_FILE_ID) {
				fprintf(stderr, "%s: invalid CBS file ID\n", filename);
				errors++;
				goto next_file;
			}

			if (mode == MODE_EXTRACT) {
				uint8_t *unpack;
				unsigned long unpacklen;

				cb_crypt_data(buf + hdr->dataoff, datalen);

				if (zlib_uncompress(&unpack, &unpacklen, hdr->data, datalen)) {
					fprintf(stderr, "%s: uncompress error\n", filename);
					errors++;
					goto next_file;
				}

				printf("%s\n", hdr->root);
				printf("%s\n", hdr->gametitle);
				printf("%s\n", hdr->note);
				write_file(unpack, unpacklen, "cbs.raw");
				free(unpack);
			}
		}
next_file:
		free(buf);
	}

	return !!errors;
}
