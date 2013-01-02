/*
 * Cryptographic functions for CodeBreaker PS2
 *
 * Copyright (C) 2006-2013 Mathias Lafeldt <mathias.lafeldt@gmail.com>
 *
 * This file is part of cb2util, the CodeBreaker PS2 File Utility.
 *
 * cb2util is licensed under the terms of the MIT License. See LICENSE file.
 */

#ifndef _CB2_CRYPTO_H_
#define _CB2_CRYPTO_H_

#include <stddef.h>
#include <stdint.h>

/* CB V1 code encryption */
#ifdef CODE_ENCRYPTION
void cb1_encrypt_code(uint32_t *addr, uint32_t *val);
#endif
void cb1_decrypt_code(uint32_t *addr, uint32_t *val);

/* CB V7 code encryption */
void cb7_beefcode(int init, uint32_t val);
#ifdef CODE_ENCRYPTION
void cb7_encrypt_code(uint32_t *addr, uint32_t *val);
#endif
void cb7_decrypt_code(uint32_t *addr, uint32_t *val);

/* All versions */
void cb_reset(void);
void cb_set_common_v7(void);
#ifdef CODE_ENCRYPTION
void cb_encrypt_code(uint32_t *addr, uint32_t *val);
#endif
void cb_decrypt_code(uint32_t *addr, uint32_t *val);
void cb_decrypt_code2(uint32_t *addr, uint32_t *val);

/* CB file functions */
int cb_verify_signature(const uint8_t *sig, const uint8_t *buf, size_t buflen);
void cb_crypt_data(uint8_t *buf, size_t buflen);

#endif /* _CB2_CRYPTO_H_ */
