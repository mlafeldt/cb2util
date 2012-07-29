/*
 * cb2_crypto.h -- Functions for decrypting/encrypting cheat codes for
 * CodeBreaker PS2 (V1 + V7)
 *
 * Copyright (C) 2006 misfire
 * All rights reserved.
 *
 * This file is part of CB2crypt, the CodeBreaker PS2 Crypto Program.
 *
 * CB2crypt is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * CB2crypt is distributed in the hope that it will be useful,
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

#include "mytypes.h"

/* CB V1 encryption */
void CB1EncryptCode(u32 *addr, u32 *val);
void CB1DecryptCode(u32 *addr, u32 *val);

/* CB V7 encryption */
void CB7Beefcode(int init, u32 val);
void CB7EncryptCode(u32 *addr, u32 *val);
void CB7DecryptCode(u32 *addr, u32 *val);
int CB7SelfTest(void);

/* All versions */
void CBReset(void);
void CBEncryptCode(u32 *addr, u32 *val);
void CBDecryptCode(u32 *addr, u32 *val);

#endif /*_CB2_CRYPTO_H_*/
