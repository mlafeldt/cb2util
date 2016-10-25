use std::fs::File;
use std::io::Read;
use std::path::Path;

extern crate flate2;
use flate2::read::ZlibDecoder;

extern crate getopts;
use getopts::Options;

extern crate codebreaker;
pub use self::codebreaker::*;

// Based on http://is.gd/8TYNHp
macro_rules! abort {
    ($($arg:tt)*) => (
        {
            use std::io::prelude::*;
            write!(&mut ::std::io::stderr(), "error: {}\n", format_args!($($arg)*)).unwrap();
            std::process::exit(1);
        }
    )
}

fn main() {
    let args: Vec<_> = std::env::args().skip(1).collect();
    assert!(!args.is_empty());
    assert!(args[0] == "cheats");

    let mut opts = Options::new();
    opts.optflag("d", "decrypt", "decrypt extracted cheats");
    let matches = match opts.parse(&args[1..]) {
        Err(e) => abort!("{}", e),
        Ok(m) => m,
    };
    let path = if !matches.free.is_empty() {
        Path::new(&matches.free[0])
    } else {
        abort!("missing input file")
    };
    let decrypt = matches.opt_present("decrypt");

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

    extract_cheats(&unpack[..], decrypt)
}

fn extract_cheats(buf: &[u8], decrypt: bool) {
    let mut i = 0;
    while i < buf.len() && read16(&buf[i..i + 2]) != 0xffff {
        // Reset code encryption
        codebreaker::reset();
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
                    codebreaker::decrypt_code2(&mut addr, &mut val);
                }

                // Discard beefcode and other junk
                if beefcodf {
                    beefcodf = false;
                    continue;
                }
                if fix_beef != 0 {
                    if addr == 0x000ffffe || addr == 0x000fffff {
                        fix_beef -= 1;
                        continue;
                    } else {
                        fix_beef = 0;
                    }
                }
                if (addr & 0xfffffffe) == 0xbeefc0de {
                    beefcodf = (addr & 1) != 0;
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
    let b0 = b[0] as u32;
    let b1 = b[1] as u32;
    let b2 = b[2] as u32;
    let b3 = b[3] as u32;

    b0 | (b1 << 8) | (b2 << 16) | (b3 << 24)
}

fn read16(b: &[u8]) -> u16 {
    let b0 = b[0] as u16;
    let b1 = b[1] as u16;

    b0 | (b1 << 8)
}

const CHEATS_HEADER_SIZE: usize = 8;
const CHEATS_FILE_ID: &'static [u8; 4] = b"CFU\0";
