/*
 * Implementation of the stream cipher ARCFOUR
 *
 * Copyright (C) 2006-2012 Mathias Lafeldt <mathias.lafeldt@gmail.com>
 *
 * This file is part of cb2util, the CodeBreaker PS2 File Utility.
 *
 * cb2util is licensed under the terms of the MIT License. See LICENSE file.
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
