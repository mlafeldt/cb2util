/*
 * cb2util/src/cb2util.c
 *
 * Main project file
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
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "mytypes.h"
#include "cb2util.h"
#include "cb2_crypto.h"
#include "cbc.h"
#include "pcb.h"


/* Text displayed for -h option */
#define HELP_TEXT \
	"Usage: "APP_NAME" [options] <input file> [output file]\n" \
	" Program to utilize different CodeBreaker PS2 file formats\n" \
	" Options are:\n" \
	"  -t/--type <type>\tDefine input file type; see below for type values\n" \
	"\n" \
	"  Code saves (V7) (type=cbc7)\n" \
	"   <no option>\t\tExtract cheats\n" \
	"   -d/--decrypt\t\tDecrypt extracted cheats\n" \
	"\n" \
	"  Code saves (V8+/Day1) (type=cbc, default)\n" \
	"   <no option>\t\tExtract cheats\n" \
	"   -d/--decrypt\t\tDecrypt extracted cheats\n" \
	"   -c/--check\t\tOnly check RSA signature\n" \
	"\n" \
	"  PCB files (type=pcb)\n" \
	"   <no option>\t\tEncrypt/decrypt to output file\n" \
	"   -s/--strip\t\tStrip RSA signature\n" \
	"   -c/--check\t\tOnly check RSA signature\n" \
	"   -e/--elf\t\tConvert into an ELF file\n" \
	"\n" \
	"  -h/--help\t\tDisplay this information\n" \
	"  -v/--version\t\tDisplay the version of "APP_NAME"\n"

/* Text displayed for -v option */
#define VERSION_TEXT \
	APP_NAME" "APP_VERSION" ("__DATE__")\n" \
	"Copyright (C) 2006-2007 misfire\n" \
	"This program is free software; you may redistribute it under the terms of\n" \
	"the GNU General Public License.  This program has absolutely no warranty.\n"

/* Short and long options accepted by getopt */
static const char *shortopts = "t:dcsehv";
static const struct option longopts[] = {
	{ "type", required_argument, NULL, 't' },
	{ "decrypt", no_argument, NULL, 'd' },
	{ "check", no_argument, NULL, 'c' },
	{ "strip", no_argument, NULL, 's' },
	{ "elf", no_argument, NULL, 'e' },
	{ "help", no_argument, NULL, 'h' },
	{ "version", no_argument, NULL, 'v' },
	{ NULL, 0, NULL, 0 }
};

/* Option flags */
enum {
	OPT_NO,			/* No option set */
	OPT_CBC_DECRYPT,	/* Decrypt extracted cheats (CBC) */
	OPT_CHECK,		/* Check RSA signature (CBC + PCB) */
	OPT_PCB_STRIP = 4,	/* Strip RSA signature (PCB) */
	OPT_PCB_ELF = 8		/* Create an ELF file (PCB) */
};

/* Number of supported file types */
#define NUM_FILE_TYPES		3

/* File types */
enum {
	FILE_TYPE_CBC,		/* CB Code Save (V8+/Day1) */
	FILE_TYPE_CBC_V7,	/* CB Code Save (V7) */
	FILE_TYPE_PCB		/* Pelican CB Binary File */
};

/* Arguments for -t option */
static const char *typeargs[NUM_FILE_TYPES] = {
	"cbc",
	"cbc7",
	"pcb"
};

/* Minimum size for each file type */
static const int minfilesizes[NUM_FILE_TYPES] = {
	CBC_MIN_FILE_SIZE,
	CBC_V7_MIN_FILE_SIZE,
	PCB_MIN_FILE_SIZE
};

/* Error codes */
enum {
	ERR_NO,			/* 0: No error */
	ERR_ARGS,		/* 1: Argument error */
	ERR_READ_FILE,		/* 2: Can't read from file */
	ERR_WRITE_FILE,		/* 3: Can't write to file */
	ERR_FILESIZE,		/* 4: Invalid file size */
	ERR_FILEID,		/* 5: Invalid file ID */
	ERR_FILESIG,		/* 6: Invalid file signature */
	ERR_MEMORY		/* 7: Memory allocation error */
};


/* Print help text */
static void show_help(void)
{
	printf(HELP_TEXT);
}

/* Print version info */
static void show_version(void)
{
	printf(VERSION_TEXT);
}

/* Print hash value */
static void print_hash(const u32 *hash)
{
	int i;

	for (i = 0; i < 5; i++)
		printf("%08X", hash[i]);
	printf("\n");
}

/* Check RSA signature */
static int check_sig(const u8 *sig, const u8 *data, int datasize)
{
	/* 160-bit hash is the concatenation of five 32-bit words */
	u32 sighash[5], calchash[5];

	if (!CBVerifyFileSig(sig, data, datasize, sighash, calchash)) {
		printf("RSA signature is valid\n");
		printf("SHA-1:\n");
		print_hash(sighash);
		return ERR_NO;
	} else {
		printf("RSA signature is invalid, hash mismatch\n");
		printf("SHA-1 in signature:\n");
		print_hash(sighash);
		printf("Calculated hash:\n");
		print_hash(calchash);
		return ERR_FILESIG;
	}
}

int main(int argc, char *argv[])
{
	FILE *fp;
	int filesize, filetype = FILE_TYPE_CBC; /* CBC is default */
	u8 *buf;	/* Buffer to hold file contents */
	cbc_hdr_t *hdr;
	u8 *data;	/* Pointer to data section */
	int datasize;	/* Size of data section */
	int optflags = OPT_NO;
	int option_index = 0; /* getopt_long() stores the option index here */
	int i, ret;

	setbuf(stdout, NULL);

	/* Check program options */
	while ((ret = getopt_long(argc, argv, shortopts, longopts, &option_index)) != -1) {
		switch (ret) {
			case 't':
				filetype = -1;
				for (i = 0; i < NUM_FILE_TYPES; i++) {
					if (!strcmp(optarg, typeargs[i])) {
						filetype = i;
						break;
					}
				}
				if (filetype == -1) {
					fprintf(stderr, APP_NAME": Invalid argument for -t option\n");
					return ERR_ARGS;
				}
				break;
			case 'd':
				optflags |= OPT_CBC_DECRYPT;
				break;
			case 'c':
				optflags |= OPT_CHECK;
				break;
			case 's':
				optflags |= OPT_PCB_STRIP;
				break;
			case 'e':
				optflags |= OPT_PCB_ELF;
				break;
			case 'h':
				show_help();
				return ERR_NO;
			case 'v':
				show_version();
				return ERR_NO;
			default:
				/* getopt_long() already printed an error message */
				return ERR_ARGS;
		}
	}

	/* Check remaining non-option arguments */
	if (!(argc - optind)) {
		fprintf(stderr, APP_NAME": You must specify an 'input file'\n");
		fprintf(stderr, "Try '%s --help' for more information.\n", APP_NAME);
		return ERR_ARGS;
	}

	/* Open input file */
	fp = fopen(argv[optind], "rb");
	if (fp == NULL) {
		fprintf(stderr, APP_NAME": Can't read from file '%s'\n", argv[optind]);
		return ERR_READ_FILE;
	}

	/* Get and check size of input file */
	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);
	if (!(filesize > minfilesizes[filetype])) {
		fprintf(stderr, APP_NAME": File too small (%d bytes)\n", filesize);
		fclose(fp);
		return ERR_FILESIZE;
	}

	/* Allocate memory for file contents */
	buf = (u8*)malloc(filesize);
	if (buf == NULL) {
		fprintf(stderr, APP_NAME": Unable to allocate %d bytes\n", filesize);
		fclose(fp);
		return ERR_MEMORY;
	}

	/* Read from input file into buffer */
	fseek(fp, 0, SEEK_SET);
	if (fread(buf, filesize, 1, fp) != 1) {
		fprintf(stderr, APP_NAME": Failed to read %d bytes from file\n", filesize);
		fclose(fp);
		return ERR_READ_FILE;
	}
	fclose(fp);

	/* Process file types */
	ret = ERR_NO;
	switch (filetype) {
		case FILE_TYPE_CBC:
			/* Check file ID */
			hdr = (cbc_hdr_t*)buf;
			if (hdr->fileid != CBC_FILE_ID) {
				fprintf(stderr, APP_NAME": Invalid CBC file ID\n");
				ret = ERR_FILEID;
				break;
			}

			/* Check signature */
			if (optflags & OPT_CHECK) {
				ret = check_sig(hdr->rsasig, &buf[CBC_HASH_DATA_OFFSET],
					filesize - CBC_HASH_DATA_OFFSET);
				break;
			}

			/* Extract cheats */
			data = &buf[sizeof(cbc_hdr_t)];
			datasize = filesize - sizeof(cbc_hdr_t);
			CBCryptFileData(data, datasize);
			CbcExtractCheats(data, datasize, optflags & OPT_CBC_DECRYPT);
			break;

		case FILE_TYPE_CBC_V7:
			/* There is no signature */
			if (optflags & OPT_CHECK) {
				fprintf(stderr, APP_NAME": CBC V7 files don't have a signature\n");
				ret = ERR_ARGS;
				break;
			}

			/* Decrypt data section */
			data = &buf[CBC_V7_HDR_SIZE];
			datasize = filesize - CBC_V7_HDR_SIZE;
			CBCryptFileData(data, datasize);

			/* Check header */
			if (!CbcIsV7Header(&buf[0], data)) {
				fprintf(stderr, APP_NAME": Invalid CBC V7 header\n");
				ret = ERR_FILEID;
				break;
			}

			/* Extract cheats */
			CbcExtractCheats(data, datasize, optflags & OPT_CBC_DECRYPT);
			break;

		case FILE_TYPE_PCB:
			/* Check signature */
			if (optflags & OPT_CHECK) {
				ret = check_sig(&buf[0], &buf[PCB_HASH_DATA_OFFSET],
					filesize - PCB_HASH_DATA_OFFSET);
				break;
			}

			/* Output file specified? */
			if (!(argc - ++optind)) {
				fprintf(stderr, APP_NAME": You must specify an 'output file'\n");
				fprintf(stderr, "Try '%s --help' for more information.\n", APP_NAME);
				ret = ERR_ARGS;
				break;
			}

			/* Open output file */
			fp = fopen(argv[optind], "wb");
			if (fp == NULL) {
				fprintf(stderr, APP_NAME": Can't write to file '%s'\n", argv[optind]);
				ret = ERR_WRITE_FILE;
				break;
			}

			/* Crypt data */
			data = &buf[PCB_HDR_SIZE]; /* Skip header */
			datasize = filesize - PCB_HDR_SIZE;
			CBCryptFileData(data, datasize);

			/* Write to output file */
			if (optflags & OPT_PCB_ELF)
				PcbWriteElf(fp, data, datasize); /* Create ELF */
			else if (optflags & OPT_PCB_STRIP)
				fwrite(data, datasize, 1, fp); /* Strip RSA signature */
			else
				fwrite(buf, filesize, 1, fp); /* Keep it */

			fclose(fp);
	}

	free(buf);

	return ret;
}
