/*
 * cb2util/src/pcb.c
 *
 * PCB file handling
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
#include "mytypes.h"
#include "cb2util.h"
#include "ps2_elf.h"
#include "pcb.h"

/* Write ELF headers and data to file */
int PcbWriteElf(FILE *fp, const u8 *data, int datasize)
{
	u8 buf[0x80]; /* Buffer for ELF headers */
	Elf32_Ehdr *ehdr;
	Elf32_Phdr *phdr;
	static const char *note = "Created with "APP_NAME" "APP_VERSION;

	if ((fp == NULL) || ferror(fp))
		return -1;

	memset(buf, 0, sizeof(buf));

	/* Create ELF file header */
	ehdr = (Elf32_Ehdr*)&buf[0];
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

	/* Create program segment header */
	phdr = (Elf32_Phdr*)&buf[ehdr->e_phoff];
	phdr->p_type = PT_LOAD;
	phdr->p_offset = sizeof(buf);
	phdr->p_vaddr = PCB_LOAD_ADDR;
	phdr->p_paddr = PCB_LOAD_ADDR;
	phdr->p_filesz = datasize;
	phdr->p_memsz = datasize;
	phdr->p_flags = PF_X | PF_W | PF_R;
	phdr->p_align = sizeof(buf);

	/* Append note */
	strcpy((char*)&buf[sizeof(Elf32_Ehdr) + sizeof(Elf32_Phdr)], note);

	/* Write to file */
	fseek(fp, 0, SEEK_SET);
	fwrite(buf, sizeof(buf), 1, fp);
	fwrite(data, datasize, 1, fp);

	return 0;
}
