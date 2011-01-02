/*
 * Implementation of the stream cipher ARCFOUR
 *
 * Copyright (C) 2006-2011 Mathias Lafeldt <misfire@debugon.org>
 * Copyright (C) 1996-2000 Whistle Communications, Inc.
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

/*
 * NOTE:
 * The ARCFOUR algorithm is believed to be fully interoperable with the RC4(TM)
 * algorithm.  RC4(TM) is trademark of RSA Data Security, Inc.
 */
#ifndef _ARCFOUR_H_
#define _ARCFOUR_H_

#include <stddef.h>
#include <stdint.h>

typedef struct {
	uint8_t	perm[256];
	uint8_t	index1;
	uint8_t	index2;
} arc4_ctx_t;

void arc4_init(arc4_ctx_t *ctx, const uint8_t *key, int keylen);
void arc4_crypt(arc4_ctx_t *ctx, uint8_t *buf, size_t buflen);

#endif /* _ARCFOUR_H_ */
