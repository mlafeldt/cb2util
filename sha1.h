/*
 * Wrapper for SHA-1 implementation
 *
 * Copyright (C) 2006-2011 Mathias Lafeldt <misfire@debugon.org>
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

#ifndef _SHA1_H_
#define _SHA1_H_

#include "shs.h"

/* Some aliases */
#define sha1_ctx_t	SHS_INFO
#define sha1_init 	shsInit
#define sha1_update 	shsUpdate
#define sha1_final 	shsFinal
#define sha1_file	shsFile

#define SHA1_DATASIZE	SHS_DATASIZE
#define SHA1_DIGESTSIZE	SHS_DIGESTSIZE

#endif /* _SHA1_H_ */
