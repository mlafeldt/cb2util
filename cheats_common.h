/*
 * Extract and compile cheats
 *
 * Copyright (C) 2006-2012 Mathias Lafeldt <mathias.lafeldt@gmail.com>
 *
 * This file is part of cb2util, the CodeBreaker PS2 File Utility.
 *
 * cb2util is licensed under the terms of the MIT License. See LICENSE file.
 */

#ifndef _CHEATS_COMMON_H_
#define _CHEATS_COMMON_H_

#include <stdint.h>
#include <stdio.h>
#include <libcheats.h>

enum {
	DECRYPT_OFF,
	DECRYPT_FORCE,
	DECRYPT_AUTO
};

int extract_cheats(FILE *fp, const uint8_t *buf, int buflen, int decrypt);
int compile_cheats(uint8_t **dst, size_t *dstlen, const cheats_t *cheats);

#endif /* _CHEATS_COMMON_H_ */
