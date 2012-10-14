/*
 * File I/O functions
 *
 * Copyright (C) 2006-2012 Mathias Lafeldt <mathias.lafeldt@gmail.com>
 *
 * This file is part of cb2util, the CodeBreaker PS2 File Utility.
 *
 * cb2util is licensed under the terms of the MIT License. See LICENSE file.
 */

#ifndef _FILEIO_H_
#define _FILEIO_H_

#include <stdint.h>

int read_file(uint8_t **buf, size_t *size, const char *path);
int write_file(const uint8_t *buf, size_t size, const char *path);

#endif /* _FILEIO_H_ */
