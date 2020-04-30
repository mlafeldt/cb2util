// Implementation of the stream cipher RC4
// Based on https://github.com/DaGenix/rust-crypto/blob/master/src/rc4.rs

struct Rc4 {
    i: u8,
    j: u8,
    state: [u8; 256],
}

impl Rc4 {
    pub fn new(key: &[u8]) -> Rc4 {
        assert!(key.len() >= 1 && key.len() <= 256);
        let mut state = [0; 256];
        for i in 0..256 {
            state[i] = i as u8
        }
        let mut j: u8 = 0;
        for i in 0..256 {
            j = j.wrapping_add(state[i]).wrapping_add(key[i % key.len()]);
            state.swap(i, j as usize);
        }
        Rc4 {
            i: 0,
            j: 0,
            state: state,
        }
    }

    pub fn crypt(&mut self, input: &[u8], output: &mut [u8]) {
        assert_eq!(input.len(), output.len());
        for (i, x) in input.iter().enumerate() {
            self.i = self.i.wrapping_add(1);
            self.j = self.j.wrapping_add(self.state[self.i as usize]);
            self.state.swap(self.i as usize, self.j as usize);
            let j = self.state[self.i as usize].wrapping_add(self.state[self.j as usize]);
            output[i] = x ^ self.state[j as usize];
        }
    }
}

#[cfg(test)]
mod tests {
    use super::Rc4;
    use std::iter::repeat;

    struct Test {
        key: &'static str,
        input: &'static str,
        output: Vec<u8>,
    }

    #[cfg_attr(rustfmt, rustfmt_skip)]
    fn tests() -> Vec<Test> {
        vec![
            Test {
                key: "Key",
                input: "Plaintext",
                output: vec![0xbb, 0xf3, 0x16, 0xe8, 0xd9, 0x40, 0xaf, 0x0a, 0xd3],
            },
            Test {
                key: "Wiki",
                input: "pedia",
                output: vec![0x10, 0x21, 0xbf, 0x04, 0x20],
            },
            Test {
                key: "Secret",
                input: "Attack at dawn",
                output: vec![0x45, 0xa0, 0x1f, 0x64, 0x5f, 0xc3, 0x5b,
                             0x38, 0x35, 0x52, 0x54, 0x4b, 0x9b, 0xf5,
                ],
            },
        ]
    }

    #[test]
    fn wikipedia_tests() {
        for t in tests().iter() {
            let mut rc4 = Rc4::new(t.key.as_bytes());
            let mut result: Vec<u8> = repeat(0).take(t.output.len()).collect();
            rc4.crypt(t.input.as_bytes(), &mut result);
            assert_eq!(t.output, result);
        }
    }
}
