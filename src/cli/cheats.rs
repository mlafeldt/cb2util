use std::fs::File;
use std::io::Read;
use std::path::Path;

use clap::ArgMatches;
use flate2::read::ZlibDecoder;

use codebreaker::Codebreaker;

pub fn run(m: &ArgMatches) {
    let path = Path::new(m.value_of("INPUT").unwrap());
    let mut file = match File::open(path) {
        Err(why) => abort!("couldn't open {}: {}", path.display(), why),
        Ok(file) => file,
    };

    let mut buf: Vec<u8> = Vec::new();
    file.read_to_end(&mut buf).unwrap();

    assert!(buf.len() > CHEATS_HEADER_SIZE);
    assert!(&buf[0..4] == CHEATS_FILE_ID);

    let mut decoder = ZlibDecoder::new(&buf[CHEATS_HEADER_SIZE..]);
    let mut unpack: Vec<u8> = Vec::new();
    decoder.read_to_end(&mut unpack).unwrap();

    let decrypt = m.is_present("decrypt");
    extract_cheats(&unpack[..], decrypt)
}

fn extract_cheats(buf: &[u8], decrypt: bool) {
    let mut i = 0;
    while i < buf.len() - 2 {
        // Check for end marker
        if read16(&buf[i..i + 2]) == 0xffff {
            break;
        }

        // Reset code encryption
        let mut cb = Codebreaker::new();
        let mut beefcodf = false;
        let mut fix_beef = 0;

        if i > 0 {
            println!("");
        }

        // Process game title
        // Example: "007 Agent Under Fire\0"
        let title = cstring(&buf[i..]);
        println!("\"{}\"", title);
        i += title.len() + 1;

        let numdesc = read16(&buf[i..i + 2]);
        i += 2;

        // Process code description(s)
        // Example: "Infinite Ammo\0"
        for _ in 0..numdesc {
            let desc = cstring(&buf[i..]);
            println!("{}", desc);
            i += desc.len() + 2;

            let numlines = read16(&buf[i..i + 2]);
            i += 2;

            // Process code line(s)
            // Example: 1A3EDED4 000003E7
            for _ in 0..numlines {
                let mut addr = read32(&buf[i..i + 4]);
                let mut val = read32(&buf[i + 4..i + 8]);
                i += 8;

                // Decrypt code
                if decrypt {
                    cb.auto_decrypt_code_mut(&mut addr, &mut val);
                }

                // Discard beefcode and other junk
                if beefcodf {
                    beefcodf = false;
                    continue;
                }
                if fix_beef != 0 && (addr == 0x000ffffe || addr == 0x000fffff) {
                    fix_beef -= 1;
                    continue;
                }
                if codebreaker::is_beefcode(addr) {
                    beefcodf = addr & 1 != 0;
                    fix_beef = 2;
                    continue;
                } else {
                    beefcodf = false;
                    fix_beef = 0;
                }

                println!("{:08X} {:08X}", addr, val);
            }
        }
    }
}

fn cstring(b: &[u8]) -> String {
    let eos = b.iter().position(|&x| x == 0).unwrap();
    String::from_utf8_lossy(&b[..eos]).into_owned()
}

fn read32(b: &[u8]) -> u32 {
    assert!(b.len() == 4);
    u32::from(b[0]) | (u32::from(b[1]) << 8) | (u32::from(b[2]) << 16) | (u32::from(b[3]) << 24)
}

fn read16(b: &[u8]) -> u16 {
    assert!(b.len() == 2);
    u16::from(b[0]) | (u16::from(b[1]) << 8)
}

const CHEATS_HEADER_SIZE: usize = 8;
const CHEATS_FILE_ID: &'static [u8; 4] = b"CFU\0";
