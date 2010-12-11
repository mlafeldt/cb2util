/*
 * cb2util/src/cb2_crypto.h
 *
 * Cryptographic functions for CodeBreaker PS2
 *
 * Copyright (C) 2006-2007 misfire
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

#ifndef _CB2_CRYPTO_H_
#define _CB2_CRYPTO_H_

#include <stdint.h>

/* CB V1 code encryption */
void CB1EncryptCode(uint32_t *addr, uint32_t *val);
void CB1DecryptCode(uint32_t *addr, uint32_t *val);

/* CB V7 code encryption */
void CB7Beefcode(int init, uint32_t val);
void CB7EncryptCode(uint32_t *addr, uint32_t *val);
void CB7DecryptCode(uint32_t *addr, uint32_t *val);
int CB7SelfTest(void);

/* All versions */
void CBReset(void);
void CBSetCommonV7(void);
void CBEncryptCode(uint32_t *addr, uint32_t *val);
void CBDecryptCode(uint32_t *addr, uint32_t *val);

/* CBC/PCB file functions */
int CBVerifyFileSig(const uint8_t *sig, const uint8_t *data, int datasize, uint32_t *sighash, uint32_t *calchash);
void CBCryptFileData(uint8_t *data, int datasize);

#endif /*_CB2_CRYPTO_H_*/
