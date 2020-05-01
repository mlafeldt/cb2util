pub mod cb1;
pub mod cb7;

extern crate libc;
use libc::{c_int, size_t};

// TODO: port all these functions to pure Rust
extern "C" {
    // CB V7 code encryption
    pub fn cb7_beefcode(init: c_int, val: u32);
    pub fn cb7_encrypt_code(addr: *mut u32, val: *mut u32);
    pub fn cb7_decrypt_code(addr: *mut u32, val: *mut u32);

    // All versions
    pub fn cb_reset();
    pub fn cb_set_common_v7();
    pub fn cb_encrypt_code(addr: *mut u32, val: *mut u32);
    pub fn cb_decrypt_code(addr: *mut u32, val: *mut u32);
    pub fn cb_decrypt_code2(addr: *mut u32, val: *mut u32);

    // CB file functions
    pub fn cb_verify_signature(sig: *const u8, buf: *const u8, buflen: size_t) -> c_int;
    pub fn cb_crypt_data(buf: *mut u8, buflen: size_t);
}

pub fn reset() {
    unsafe { cb_reset() }
}

pub fn set_common_v7() {
    unsafe { cb_set_common_v7() }
}

pub fn encrypt_code(addr: &mut u32, val: &mut u32) {
    unsafe { cb_encrypt_code(addr, val) }
}

pub fn decrypt_code(addr: &mut u32, val: &mut u32) {
    unsafe { cb_decrypt_code(addr, val) }
}

pub fn decrypt_code2(addr: &mut u32, val: &mut u32) {
    unsafe { cb_decrypt_code2(addr, val) }
}

pub fn verify_signature(sig: &[u8], buf: &[u8]) -> bool {
    unsafe { cb_verify_signature(sig.as_ptr(), buf.as_ptr(), buf.len() as size_t) == 0 }
}

pub fn crypt_data(buf: &mut [u8]) {
    unsafe { cb_crypt_data(buf.as_mut_ptr(), buf.len() as size_t) }
}
