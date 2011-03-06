/*
 * zlib wrapper
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
