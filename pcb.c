/*
 * PCB file handling
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
#include <elf.h>
#include "cb2_crypto.h"
#include "fileio.h"

/* PCB file header */
typedef struct {
	uint8_t		rsasig[256];	/* 2048-bit RSA signature */
	uint8_t		data[0];
} pcb_hdr_t;

/* offset of data hashed for RSA signature */
#define PCB_HASH_OFFSET	(sizeof(pcb_hdr_t))

/* address at which the first byte of the PCB data resides in PS2 memory */
#define PCB_LOAD_ADDR	0x01000000

/* address to which the PS2 first transfers control */
#define PCB_ENTRY_ADDR	PCB_LOAD_ADDR

/* PS2 processor-specific flags:
   noreorder, MIPS R5900 CPU, eabi64, mips3
   (value for e_flags field of Elf32_Ehdr) */
#define _EF_PS2		0x20924001

static void gen_elf_header(uint8_t *hdr, int hdrlen, int datalen)
{
	Elf32_Ehdr *ehdr;
	Elf32_Phdr *phdr;
	static const char *note = "Created with cb2util";

	memset(hdr, 0, hdrlen);

	/* create ELF file header */
	ehdr = (Elf32_Ehdr*)hdr;
	memcpy(&ehdr->e_ident[EI_MAG0], ELFMAG, SELFMAG);
	ehdr->e_ident[EI_CLASS] = ELFCLASS32;
	ehdr->e_ident[EI_DATA] = ELFDATA2LSB;
	ehdr->e_ident[EI_VERSION] = EV_CURRENT;
	ehdr->e_ident[EI_OSABI] = ELFOSABI_NONE;
	ehdr->e_ident[EI_ABIVERSION] = 0;
	ehdr->e_type = ET_EXEC;
	ehdr->e_machine = EM_MIPS;
	ehdr->e_version = EV_CURRENT;
	ehdr->e_entry = PCB_ENTRY_ADDR;
	ehdr->e_phoff = sizeof(Elf32_Ehdr);
	ehdr->e_shoff = 0;
	ehdr->e_flags = _EF_PS2;
	ehdr->e_ehsize = sizeof(Elf32_Ehdr);
	ehdr->e_phentsize = sizeof(Elf32_Phdr);
	ehdr->e_phnum = 1;
	ehdr->e_shentsize = sizeof(Elf32_Shdr);
	ehdr->e_shnum = 0;
	ehdr->e_shstrndx = 0;

	/* create program segment header */
	phdr = (Elf32_Phdr*)&hdr[ehdr->e_phoff];
	phdr->p_type = PT_LOAD;
	phdr->p_offset = hdrlen;
	phdr->p_vaddr = PCB_LOAD_ADDR;
	phdr->p_paddr = PCB_LOAD_ADDR;
	phdr->p_filesz = datalen;
	phdr->p_memsz = datalen;
	phdr->p_flags = PF_X | PF_W | PF_R;
	phdr->p_align = 0; /* XXX set correct alignment */

	/* append note */
	strcpy((char*)&hdr[sizeof(Elf32_Ehdr) + sizeof(Elf32_Phdr)], note);
}

static const char *pcb_usage =
	"usage: cb2util pcb [-s] <infile> <outfile>...\n"
	"   or: cb2util pcb -e <infile> <outfile>...\n"
	"   or: cb2util pcb -v <file>...\n\n"
	"    no option\n"
	"        encrypt/decrypt file\n\n"
	"    -s, --strip\n"
	"        strip RSA signature\n\n"
	"    -e, --elf\n"
	"        convert into ELF file\n\n"
	"    -v, --verify\n"
	"        verify RSA signature\n";

int cmd_pcb(int argc, char **argv)
{
	const char *shortopts = "ehsv";
	const struct option longopts[] = {
		{ "elf", no_argument, NULL, 'e' },
		{ "help", no_argument, NULL, 'h' },
		{ "strip", no_argument, NULL, 's' },
		{ "verify", no_argument, NULL, 'v' },
		{ NULL, 0, NULL, 0 }
	};
	enum {
		MODE_CRYPT,
		MODE_ELF,
		MODE_STRIP,
		MODE_VERIFY
	};
	int mode = MODE_CRYPT;
	int errors = 0;
	int ret;

	while ((ret = getopt_long(argc, argv, shortopts, longopts, NULL)) != -1) {
		switch (ret) {
		case 'e':
			mode = MODE_ELF;
			break;
		case 'h':
			printf("%s\n", pcb_usage);
			return 0;
		case 's':
			mode = MODE_STRIP;
			break;
		case 'v':
			mode = MODE_VERIFY;
			break;
		default:
			fprintf(stderr, "%s\n", pcb_usage);
			return 1;
		}
	}

	if ((optind == argc) || (mode != MODE_VERIFY && (argc - optind) & 1)) {
		fprintf(stderr, "%s\n", pcb_usage);
		return 1;
	}

	while (optind < argc) {
		const char *infile = argv[optind++];
		const char *outfile = mode != MODE_VERIFY ? argv[optind++] : NULL;
		uint8_t *buf;
		size_t buflen;
		pcb_hdr_t *hdr;
		int datalen;

		if (read_file(&buf, &buflen, infile)) {
			fprintf(stderr, "%s: read error\n", infile);
			errors++;
			continue;
		}

		hdr = (pcb_hdr_t*)buf;
		datalen = buflen - sizeof(pcb_hdr_t);

		if (datalen < 0) {
			fprintf(stderr, "%s: not a PCB file\n", infile);
			errors++;
			goto next_file;
		}

		if (mode == MODE_VERIFY) {
			ret = cb_verify_signature(hdr->rsasig, hdr->data, datalen);
			printf("%s: %s\n", infile, ret ? "FAILED" : "OK");
			errors += ret;
		} else {
			cb_crypt_data(hdr->data, datalen);

			if (mode == MODE_ELF) {
				/* replace signature with ELF header */
				gen_elf_header(hdr->rsasig, sizeof(hdr->rsasig), datalen);
				ret = write_file(buf, buflen, outfile);
			} else if (mode == MODE_STRIP) {
				ret = write_file(hdr->data, datalen, outfile);
			} else if (mode == MODE_CRYPT) {
				ret = write_file(buf, buflen, outfile);
			}

			if (ret) {
				fprintf(stderr, "%s: write error\n", outfile);
				errors++;
			}
		}
next_file:
		free(buf);
	}

	return !!errors;
}
