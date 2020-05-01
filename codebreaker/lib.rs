pub mod cb1;
pub mod cb7;
mod rc4;

extern crate libc;
use libc::{c_int, size_t};

#[repr(C)]
#[derive(PartialEq)]
enum EncMode {
    RAW,
    V1,
    V7,
}

// TODO: port all these functions to pure Rust
extern "C" {
    pub static mut seeds: [[u8; 256]; 5];
    pub static mut key: [u32; 5];
    pub static mut oldkey: [u32; 5];
    static mut enc_mode: EncMode;
    static mut v7_init: libc::c_int;
    static mut beefcodf: libc::c_int;
    static mut code_lines: libc::c_int;

    // CB V7 code encryption
    pub fn rsa_crypt(addr: *mut u32, val: *mut u32, rsakey: u64);
    pub fn cb7_encrypt_code(addr: *mut u32, val: *mut u32);
    pub fn cb7_decrypt_code(addr: *mut u32, val: *mut u32);

    // CB file functions
    pub fn cb_verify_signature(sig: *const u8, buf: *const u8, buflen: size_t) -> c_int;
    pub fn cb_crypt_data(buf: *mut u8, buflen: size_t);
}

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
            cb7_encrypt_code(addr, val);
        } else {
            let code = cb1::encrypt_code(*addr, *val);
            *addr = code.0;
            *val = code.1;
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
            cb7_decrypt_code(addr, val);
        } else {
            let code = cb1::decrypt_code(*addr, *val);
            *addr = code.0;
            *val = code.1;
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
                        let code = cb1::decrypt_code(*addr, *val);
                        *addr = code.0;
                        *val = code.1;
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
                let code = cb1::decrypt_code(*addr, *val);
                *addr = code.0;
                *val = code.1;
            }
        } else {
            cb7_decrypt_code(addr, val);
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

// Verify digital signature on CB files
pub fn verify_signature(sig: &[u8], buf: &[u8]) -> bool {
    unsafe { cb_verify_signature(sig.as_ptr(), buf.as_ptr(), buf.len() as size_t) == 0 }
}

// Encrypt or decrypt CB file data
pub fn crypt_data(buf: &mut [u8]) {
    unsafe { cb_crypt_data(buf.as_mut_ptr(), buf.len() as size_t) }
}
