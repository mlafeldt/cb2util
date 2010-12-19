#ifndef _COMPRESS_H_
#define _COMPRESS_H_

int zlib_compress(unsigned char **dst, unsigned long *dstlen,
		const unsigned char *src, unsigned long srclen);

int zlib_uncompress(unsigned char **dst, unsigned long *dstlen,
		const unsigned char *src, unsigned long srclen);

#endif /* _COMPRESS_H_ */
