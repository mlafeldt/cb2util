extern crate libc;

extern "C" {
    pub fn cb_reset();
    pub fn cb_decrypt_code(addr: *mut u32, val: *mut u32);
    pub fn cb_decrypt_code2(addr: *mut u32, val: *mut u32);
}

pub fn reset() {
    unsafe { cb_reset() }
}

pub fn decrypt_code(addr: &mut u32, val: &mut u32) {
    unsafe { cb_decrypt_code(addr, val) }
}

pub fn decrypt_code2(addr: &mut u32, val: &mut u32) {
    unsafe { cb_decrypt_code2(addr, val) }
}
