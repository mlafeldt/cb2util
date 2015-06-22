// Encrypt and decrypt codes using CB v7 scheme

pub fn encrypt_code(mut addr: u32, mut val: u32) -> (u32, u32) {
    // TODO
    (addr, val)
}

pub fn decrypt_code(mut addr: u32, mut val: u32) -> (u32, u32) {
    // TODO
    (addr, val)
}

#[cfg(test)]
mod tests {
    use super::{encrypt_code,decrypt_code};

    struct Test {
        decrypted: (u32, u32),
        encrypted: (u32, u32),
    }

    fn tests() -> Vec<Test> {
        vec![
            Test {
                decrypted: (0, 0),
                encrypted: (0, 0),
            },
       ]
    }

    #[test]
    fn test_encrypt_code() {
        for t in tests().iter() {
            let result = encrypt_code(t.decrypted.0, t.decrypted.1);
            assert_eq!(t.encrypted.0, result.0);
            assert_eq!(t.encrypted.1, result.1);
        }
    }

    #[test]
    fn test_decrypt_code() {
        for t in tests().iter() {
            let result = decrypt_code(t.encrypted.0, t.encrypted.1);
            assert_eq!(t.decrypted.0, result.0);
            assert_eq!(t.decrypted.1, result.1);
        }
    }
}
