/* (C) 1994 Paul Rubin
   Implementation of SHA-1, the fixed version of SHS/SHA-0 */

/* NIST proposed Secure Hash Standard.

   Written 2 September 1992, Peter C. Gutmann.
   This implementation placed in the public domain.

   Comments to pgut1@cs.aukuni.ac.nz */

#ifndef _SHS_DEFINED
#define _SHS_DEFINED

#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Some useful types */

typedef uint8_t		BYTE;
typedef uint16_t	WORD;
typedef uint32_t	LONG;

/* Exit status of functions. */

#define OK      0
#define ERROR   -1

/* Define the following to use the updated SHS implementation */

#define NEW_SHS

/* Define the following if running on a little-endian CPU */

#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN
#endif

/* The SHS block size and message digest sizes, in bytes */

#define SHS_DATASIZE    64
#define SHS_DIGESTSIZE  20

/* The structure for storing SHS info */

typedef struct {
               LONG digest[ 5 ];            /* Message digest */
               LONG countLo, countHi;       /* 64-bit bit count */
               LONG data[ 16 ];             /* SHS data buffer */
               } SHS_INFO;

/* Message digest functions */

void shsInit( SHS_INFO *shsInfo );
void shsUpdate( SHS_INFO *shsInfo, BYTE *buffer, int count );
void shsFinal( SHS_INFO *shsInfo );

#endif /* _SHS_DEFINED */
