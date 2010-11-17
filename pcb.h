/*
 * cb2util/src/pcb.h
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

#ifndef _PCB_H_
#define _PCB_H_

#include <stdio.h>
#include "mytypes.h"

/* PCB header: 2048-bit RSA signature (256 bytes) */
#define PCB_HDR_SIZE		256
#define PCB_MIN_FILE_SIZE	PCB_HDR_SIZE

/* Start offset of data that is hashed for RSA signature */
#define PCB_HASH_DATA_OFFSET	PCB_HDR_SIZE /* all data after signature */

/* Address at which the first byte of the PCB data resides in PS2 memory */
#define PCB_LOAD_ADDR		0x01000000

/* Address to which the PS2 first transfers control */
#define PCB_ENTRY_ADDR		PCB_LOAD_ADDR

int PcbWriteElf(FILE *fp, const u8 *data, int datasize);

#endif /*_PCB_H_*/
