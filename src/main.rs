mod crypto;

use crypto::cb1::encrypt_code;

fn main() {
    let code = encrypt_code(0x902db32c, 0x0c0baff1);
    println!("{:08x} {:08x}", code.0, code.1);
}
