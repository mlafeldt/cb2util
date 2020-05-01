extern crate libc;

extern "C" {
    pub fn cb_reset();
    pub fn cb_set_common_v7();
    pub fn cb_encrypt_code(addr: *mut u32, val: *mut u32);
    pub fn cb_decrypt_code(addr: *mut u32, val: *mut u32);
    pub fn cb_decrypt_code2(addr: *mut u32, val: *mut u32);
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
