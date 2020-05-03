#![allow(non_upper_case_globals)]

pub mod cb1;
pub mod cb7;
mod rc4;
mod rsa;

use std::mem::size_of;
use std::slice;

#[derive(PartialEq)]
enum EncMode {
    RAW,
    V1,
    V7,
}

// TODO: turn this into a struct
static mut seeds: [[u8; 256]; 5] = [[0; 256]; 5];
static mut key: [u32; 5] = [0; 5];
static mut oldkey: [u32; 5] = [0; 5];
static mut enc_mode: EncMode = EncMode::RAW;
static mut v7_init: i32 = 0;
static mut beefcodf: i32 = 0;
static mut code_lines: i32 = 0;

// Resets the CB encryption. Must be called before processing a code list using
// encrypt_code() or decrypt_code()!
pub fn reset() {
    unsafe {
        enc_mode = EncMode::RAW;
        v7_init = 0;
        beefcodf = 0;
        code_lines = 0;
    }
}

// Set common CB V7 encryption (B4336FA9 4DFEFB79) which is used by CMGSCCC.com
pub fn set_common_v7() {
    unsafe {
        enc_mode = EncMode::V7;
        cb7::beefcode(1, 0);
        v7_init = 1;
        beefcodf = 0;
        code_lines = 0;
    }
}

// Used to encrypt a list of CB codes (V1 + V7)
pub fn encrypt_code(addr: &mut u32, val: &mut u32) {
    unsafe {
        let (oldaddr, oldval) = (*addr, *val);

        if enc_mode == EncMode::V7 {
            cb7::encrypt_code_mut(addr, val);
        } else {
            cb1::encrypt_code_mut(addr, val);
        }

        if (oldaddr & 0xfffffffe) == 0xbeefc0de {
            if v7_init == 0 {
                cb7::beefcode(1, oldval);
                v7_init = 1;
            } else {
                cb7::beefcode(0, oldval);
            }
            enc_mode = EncMode::V7;
            beefcodf = (oldaddr & 1) as i32;
        }
    }
}

// Used to decrypt a list of CB codes (V1 + V7)
pub fn decrypt_code(addr: &mut u32, val: &mut u32) {
    unsafe {
        if enc_mode == EncMode::V7 {
            cb7::decrypt_code_mut(addr, val);
        } else {
            cb1::decrypt_code_mut(addr, val);
        }

        if (*addr & 0xfffffffe) == 0xbeefc0de {
            if v7_init == 0 {
                cb7::beefcode(1, *val);
                v7_init = 1;
            } else {
                cb7::beefcode(0, *val);
            }
            enc_mode = EncMode::V7;
            beefcodf = (*addr & 1) as i32;
        }
    }
}

// Smart version of decrypt_code() that detects if a code needs to be decrypted and how
pub fn decrypt_code2(addr: &mut u32, val: &mut u32) {
    unsafe {
        if enc_mode != EncMode::V7 {
            if code_lines == 0 {
                code_lines = num_code_lines(*addr);
                if (*addr >> 24) & 0x0e != 0 {
                    if (*addr & 0xfffffffe) == 0xbeefc0de {
                        // ignore raw beefcode
                        code_lines -= 1;
                        return;
                    } else {
                        enc_mode = EncMode::V1;
                        code_lines -= 1;
                        cb1::decrypt_code_mut(addr, val);
                    }
                } else {
                    enc_mode = EncMode::RAW;
                    code_lines -= 1;
                }
            } else {
                code_lines -= 1;
                if enc_mode == EncMode::RAW {
                    return;
                }
                cb1::decrypt_code_mut(addr, val);
            }
        } else {
            cb7::decrypt_code_mut(addr, val);
            if code_lines == 0 {
                code_lines = num_code_lines(*addr);
                if code_lines == 1 && *addr == 0xffffffff {
                    // TODO: change encryption options
                    code_lines = 0;
                    return;
                }
            }
            code_lines -= 1;
        }

        if (*addr & 0xfffffffe) == 0xbeefc0de {
            if v7_init == 0 {
                cb7::beefcode(1, *val);
                v7_init = 1;
            } else {
                cb7::beefcode(0, *val);
            }
            enc_mode = EncMode::V7;
            beefcodf = (*addr & 1) as i32;
            code_lines = 1;
        }
    }
}

fn num_code_lines(addr: u32) -> i32 {
    let cmd = addr >> 28;

    if cmd < 3 || cmd > 6 {
        1
    } else if cmd == 3 {
        if addr & 0x00400000 != 0 {
            2
        } else {
            1
        }
    } else {
        2
    }
}

// Source: https://github.com/BurntSushi/byteorder/blob/master/src/io.rs
unsafe fn slice_to_u8_mut<T: Copy>(slice: &mut [T]) -> &mut [u8] {
    let len = size_of::<T>() * slice.len();
    slice::from_raw_parts_mut(slice.as_mut_ptr() as *mut u8, len)
}

unsafe fn slice_to_u8<T: Copy>(slice: &[T]) -> &[u8] {
    let len = size_of::<T>() * slice.len();
    slice::from_raw_parts(slice.as_ptr() as *const u8, len)
}
