/*
 * cb2util/src/cbc.h
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

#ifndef _CBC_H_
#define _CBC_H_

#include "mytypes.h"

/* CBC file header */
typedef struct {
	u32	fileid;		/* File ID, must be CBC_FILE_ID */
	u8	rsasig[256];	/* 2048-bit RSA signature */
	u32	cbvers;		/* Required CB version */
	char	gametitle[72];	/* Game title */
	u32	dataoff;	/* File offset of data section */
	u32	zero;		/* Always 0 */
} cbc_hdr_t; /* 344 bytes total */

#define CBC_FILE_ID		0x01554643 /* "CFU", 1 */
#define CBC_MIN_FILE_SIZE	(sizeof(cbc_hdr_t))

/* Start offset of data that is hashed for RSA signature */
#define CBC_HASH_DATA_OFFSET	0x00000104 /* all data after signature */

/* Support for older CBC V7 files
   Header: Game title, 0, random padding junk (64 bytes total) */
#define CBC_V7_HDR_SIZE		64
#define CBC_V7_MIN_FILE_SIZE	CBC_V7_HDR_SIZE

void CbcPrintHeader(const cbc_hdr_t *hdr);
int CbcIsV7Header(const u8 *hdr, const u8 *data);
int CbcExtractCheats(const u8 *data, int datasize, int dodecrypt);

#endif /*_CBC_H_*/
