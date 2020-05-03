// TODO: port to pure Rust

extern "C" {
    fn rsa_crypt(addr: *mut u32, val: *mut u32, rsakey: u64);
}

pub fn crypt(addr: *mut u32, val: *mut u32, rsakey: u64) {
    unsafe { rsa_crypt(addr, val, rsakey) }
}
