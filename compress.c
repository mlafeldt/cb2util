/*
 * zlib wrapper
 *
 * Copyright (C) 2006-2013 Mathias Lafeldt <mathias.lafeldt@gmail.com>
 *
 * This file is part of cb2util, the CodeBreaker PS2 File Utility.
 *
 * cb2util is licensed under the terms of the MIT License. See LICENSE file.
 */

#include <stdlib.h>
#include <zlib.h>

int zlib_compress(unsigned char **dst, unsigned long *dstlen,
		const unsigned char *src, unsigned long srclen)
{
	*dstlen = compressBound(srclen);
	*dst = malloc(*dstlen);
	if (*dst == NULL)
		return -1;
	return compress2(*dst, dstlen, src, srclen, Z_BEST_COMPRESSION);
}

int zlib_uncompress(unsigned char **dst, unsigned long *dstlen,
		const unsigned char *src, unsigned long srclen)
{
	int ret;

	*dst = NULL;
	*dstlen = srclen;
	do {
		/*
		 * enlarge destination buffer until it can store
		 * all uncompressed data
		 */
		*dstlen *= 2;
		*dst = realloc(*dst, *dstlen);
		if (*dst == NULL)
			return -1;
		ret = uncompress(*dst, dstlen, src, srclen);
	} while (ret == Z_BUF_ERROR);

	return ret;
}
