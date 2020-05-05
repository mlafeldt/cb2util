pub mod cb1;
pub mod cb7;
mod rc4;

extern crate num_bigint;

use std::mem::size_of;
use std::slice;

#[derive(PartialEq)]
enum EncMode {
    RAW,
    V1,
    V7,
}

pub struct Codebreaker {
    enc_mode: EncMode,
    cb7: cb7::Context,
    v7_init: bool,
    code_lines: usize,
}

impl Codebreaker {
    pub fn new() -> Codebreaker {
        Codebreaker {
            enc_mode: EncMode::RAW,
            cb7: cb7::Context::new(),
            v7_init: false,
            code_lines: 0,
        }
    }

    // Resets the CB encryption.
    pub fn reset(&mut self) {
        *self = Codebreaker::new()
    }

    // Set common CB V7 encryption (B4336FA9 4DFEFB79) which is used by CMGSCCC.com
    pub fn set_common_v7(&mut self) {
        self.enc_mode = EncMode::V7;
        self.cb7.beefcode(true, 0);
        self.v7_init = true;
        self.cb7.beefcodf = false;
        self.code_lines = 0;
    }

    // Used to encrypt a list of CB codes (V1 + V7)
    pub fn encrypt_code(&mut self, addr: &mut u32, val: &mut u32) {
        let (oldaddr, oldval) = (*addr, *val);

        if self.enc_mode == EncMode::V7 {
            self.cb7.encrypt_code_mut(addr, val);
        } else {
            cb1::encrypt_code_mut(addr, val);
        }

        if is_beefcode(oldaddr) {
            self.cb7.beefcode(!self.v7_init, oldval);
            self.v7_init = true;
            self.enc_mode = EncMode::V7;
            self.cb7.beefcodf = oldaddr & 1 != 0;
        }
    }

    // Used to decrypt a list of CB codes (V1 + V7)
    pub fn decrypt_code(&mut self, addr: &mut u32, val: &mut u32) {
        if self.enc_mode == EncMode::V7 {
            self.cb7.decrypt_code_mut(addr, val);
        } else {
            cb1::decrypt_code_mut(addr, val);
        }

        if is_beefcode(*addr) {
            self.cb7.beefcode(!self.v7_init, *val);
            self.v7_init = true;
            self.enc_mode = EncMode::V7;
            self.cb7.beefcodf = *addr & 1 != 0;
        }
    }

    // Smart version of decrypt_code() that detects if a code needs to be decrypted and how
    pub fn decrypt_code2(&mut self, addr: &mut u32, val: &mut u32) {
        if self.enc_mode != EncMode::V7 {
            if self.code_lines == 0 {
                self.code_lines = num_code_lines(*addr);
                if (*addr >> 24) & 0x0e != 0 {
                    if is_beefcode(*addr) {
                        // ignore raw beefcode
                        self.code_lines -= 1;
                        return;
                    } else {
                        self.enc_mode = EncMode::V1;
                        self.code_lines -= 1;
                        cb1::decrypt_code_mut(addr, val);
                    }
                } else {
                    self.enc_mode = EncMode::RAW;
                    self.code_lines -= 1;
                }
            } else {
                self.code_lines -= 1;
                if self.enc_mode == EncMode::RAW {
                    return;
                }
                cb1::decrypt_code_mut(addr, val);
            }
        } else {
            self.cb7.decrypt_code_mut(addr, val);
            if self.code_lines == 0 {
                self.code_lines = num_code_lines(*addr);
                if self.code_lines == 1 && *addr == 0xffffffff {
                    // TODO: change encryption options
                    self.code_lines = 0;
                    return;
                }
            }
            self.code_lines -= 1;
        }

        if is_beefcode(*addr) {
            self.cb7.beefcode(!self.v7_init, *val);
            self.v7_init = true;
            self.enc_mode = EncMode::V7;
            self.cb7.beefcodf = *addr & 1 != 0;
            self.code_lines = 1;
        }
    }
}

#[inline(always)]
pub fn is_beefcode(val: u32) -> bool {
    val & 0xfffffffe == 0xbeefc0de
}

fn num_code_lines(addr: u32) -> usize {
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
