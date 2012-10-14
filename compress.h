/*
 * zlib wrapper
 *
 * Copyright (C) 2006-2012 Mathias Lafeldt <mathias.lafeldt@gmail.com>
 *
 * This file is part of cb2util, the CodeBreaker PS2 File Utility.
 *
 * cb2util is licensed under the terms of the MIT License. See LICENSE file.
 */

#ifndef _COMPRESS_H_
#define _COMPRESS_H_

int zlib_compress(unsigned char **dst, unsigned long *dstlen,
		const unsigned char *src, unsigned long srclen);

int zlib_uncompress(unsigned char **dst, unsigned long *dstlen,
		const unsigned char *src, unsigned long srclen);

#endif /* _COMPRESS_H_ */
