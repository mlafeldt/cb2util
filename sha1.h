/*
 * Wrapper for SHA-1 implementation
 *
 * Copyright (C) 2006-2013 Mathias Lafeldt <mathias.lafeldt@gmail.com>
 *
 * This file is part of cb2util, the CodeBreaker PS2 File Utility.
 *
 * cb2util is licensed under the terms of the MIT License. See LICENSE file.
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
