//! CBC file handling (code saves for CB v7 and v8+/Day1)

use crate::cheats::CheatDb;
use crate::crypto;
use codebreaker::Codebreaker;
use std::io::Write;

const CBC_FILE_ID: u32 = 0x01554643; // "CFU", 1
const CBC_HASH_OFFSET: usize = 0x104;
const CBC_BANNER: &[u8] = b"Created with cb2util - https://github.com/mlafeldt/cb2util";
const CBC_HEADER_SIZE: usize = 348;
const CBC7_HEADER_SIZE: usize = 64;

/// Decrypt mode
#[derive(Clone, Copy, PartialEq, Eq)]
pub enum DecryptMode {
    Off,
    Auto,
    Force,
}

/// Extract cheats from CBC file data
pub fn extract_cheats<W: Write>(
    mut writer: W,
    data: &[u8],
    decrypt: DecryptMode,
) -> std::io::Result<usize> {
    let mut off = 0;
    let mut total_codes = 0;

    while off < data.len() {
        // Check for end marker
        if off + 2 <= data.len() {
            let marker = u16::from_le_bytes([data[off], data[off + 1]]);
            if marker == 0xFFFF {
                break;
            }
        }

        // Reset for new game
        let mut cb = Codebreaker::new();
        let mut beefcodf = false;
        let mut fix_beef = 0;

        if total_codes > 0 {
            writeln!(writer)?;
        }

        // Game title (null-terminated string)
        let title_end = data[off..].iter().position(|&b| b == 0).unwrap_or(0);
        let title = std::str::from_utf8(&data[off..off + title_end]).unwrap_or("");
        writeln!(writer, "\"{}\"", title)?;
        off += title_end + 1;

        if off + 2 > data.len() {
            break;
        }
        let num_desc = u16::from_le_bytes([data[off], data[off + 1]]) as usize;
        off += 2;

        // Process cheat descriptions
        for _ in 0..num_desc {
            if off >= data.len() {
                break;
            }

            // Cheat description (null-terminated string)
            let desc_end = data[off..].iter().position(|&b| b == 0).unwrap_or(0);
            let desc = std::str::from_utf8(&data[off..off + desc_end]).unwrap_or("");
            off += desc_end + 1;

            // Skip tag byte
            off += 1;

            if off + 2 > data.len() {
                break;
            }
            let num_lines = u16::from_le_bytes([data[off], data[off + 1]]) as usize;
            off += 2;

            writeln!(writer, "{}", desc)?;

            if num_lines > 0 {
                total_codes += 1;

                for _ in 0..num_lines {
                    if off + 8 > data.len() {
                        break;
                    }

                    let mut addr = u32::from_le_bytes([data[off], data[off + 1], data[off + 2], data[off + 3]]);
                    let mut val = u32::from_le_bytes([data[off + 4], data[off + 5], data[off + 6], data[off + 7]]);
                    off += 8;

                    // Decrypt code
                    match decrypt {
                        DecryptMode::Force => {
                            (addr, val) = cb.decrypt_code(addr, val);
                        }
                        DecryptMode::Auto => {
                            (addr, val) = cb.auto_decrypt_code(addr, val);
                        }
                        DecryptMode::Off => {}
                    }

                    // Discard beefcode and junk
                    if beefcodf {
                        beefcodf = false;
                        continue;
                    }
                    if fix_beef > 0
                        && (addr == 0x000FFFFE || addr == 0x000FFFFF)
                    {
                        fix_beef -= 1;
                        continue;
                    }
                    if (addr & 0xFFFFFFFE) == 0xBEEFC0DE {
                        beefcodf = (addr & 1) != 0;
                        fix_beef = 2;
                        continue;
                    } else {
                        beefcodf = false;
                        fix_beef = 0;
                    }

                    writeln!(writer, "{:08X} {:08X}", addr, val)?;
                }
            }
        }
    }

    Ok(total_codes)
}

/// Compile cheats to binary format
pub fn compile_cheats(cheats: &CheatDb) -> Vec<u8> {
    let mut buf = Vec::with_capacity(1024 * 1024);

    for game in &cheats.games {
        // Game title (null-terminated)
        buf.extend_from_slice(game.title.as_bytes());
        buf.push(0);

        // Number of descriptions
        let num_desc = game.cheats.len() as u16;
        buf.extend_from_slice(&num_desc.to_le_bytes());

        for cheat in &game.cheats {
            // Cheat description (null-terminated)
            buf.extend_from_slice(cheat.desc.as_bytes());
            buf.push(0);

            // Tag: 0 for cheats with codes, 4 for cheats without codes
            let tag = if cheat.codes.is_empty() { 4u8 } else { 0u8 };
            buf.push(tag);

            // Number of code lines
            let num_lines = cheat.codes.len() as u16;
            buf.extend_from_slice(&num_lines.to_le_bytes());

            // Code lines
            for code in &cheat.codes {
                buf.extend_from_slice(&code.addr.to_le_bytes());
                buf.extend_from_slice(&code.val.to_le_bytes());
            }
        }
    }

    // End marker
    buf.extend_from_slice(&0xFFFFu16.to_le_bytes());

    buf
}

/// Process CBC file (extract or verify)
pub fn process_cbc(
    path: &str,
    v7: bool,
    verify: bool,
    decrypt: DecryptMode,
) -> Result<(), String> {
    let buf = std::fs::read(path).map_err(|e| format!("{}: read error: {}", path, e))?;

    if v7 {
        if buf.len() < CBC7_HEADER_SIZE {
            return Err(format!("{}: not a CBC v7 file", path));
        }

        let mut data = buf[CBC7_HEADER_SIZE..].to_vec();
        crypto::crypt_data(&mut data);

        // Verify header by checking if title matches first string in data
        // Title in header is null-terminated within the 64-byte field
        let title_end = buf[..CBC7_HEADER_SIZE]
            .iter()
            .position(|&b| b == 0)
            .unwrap_or(CBC7_HEADER_SIZE);
        let title = std::str::from_utf8(&buf[..title_end]).unwrap_or("");

        let data_title_end = data.iter().position(|&b| b == 0).unwrap_or(0);
        let data_title = std::str::from_utf8(&data[..data_title_end]).unwrap_or("");

        if title != data_title {
            return Err(format!("{}: invalid CBC v7 header", path));
        }

        extract_cheats(std::io::stdout(), &data, decrypt)
            .map_err(|e| format!("{}: {}", path, e))?;
    } else {
        if buf.len() < CBC_HEADER_SIZE {
            return Err(format!("{}: not a CBC file", path));
        }

        let fileid = u32::from_le_bytes([buf[0], buf[1], buf[2], buf[3]]);
        if fileid != CBC_FILE_ID {
            return Err(format!("{}: invalid CBC file ID", path));
        }

        let dataoff = u32::from_le_bytes([buf[336], buf[337], buf[338], buf[339]]) as usize;
        if dataoff > buf.len() {
            return Err(format!("{}: invalid CBC data offset", path));
        }

        if verify {
            let sig: [u8; 256] = buf[4..260].try_into().unwrap();
            let valid = crypto::verify_signature(&sig, &buf[CBC_HASH_OFFSET..]);
            println!("{}: {}", path, if valid { "OK" } else { "FAILED" });
            if !valid {
                return Err(String::new());
            }
        } else {
            let mut data = buf[dataoff..].to_vec();
            crypto::crypt_data(&mut data);

            extract_cheats(std::io::stdout(), &data, decrypt)
                .map_err(|e| format!("{}: {}", path, e))?;
        }
    }

    Ok(())
}

/// Compile text file to CBC file
pub fn compile_cbc(
    infile: &str,
    outfile: &str,
    v7: bool,
    banner: Option<&str>,
) -> Result<(), String> {
    let mut cheats = CheatDb::new();
    cheats
        .read_file(infile)
        .map_err(|e| format!("{}: line: {}\nerror: {}", infile, e.line, e.message))?;

    if cheats.games.is_empty() {
        return Err(format!("{}: no games found", infile));
    }

    let mut data = compile_cheats(&cheats);
    crypto::crypt_data(&mut data);

    let mut file =
        std::fs::File::create(outfile).map_err(|e| format!("{}: write error: {}", outfile, e))?;

    if v7 {
        // CBC v7 header
        let mut header = [0u8; 64];
        let title = cheats.games[0].title.as_bytes();
        let len = title.len().min(64);
        header[..len].copy_from_slice(&title[..len]);
        std::io::Write::write_all(&mut file, &header)
            .map_err(|e| format!("{}: write error: {}", outfile, e))?;
    } else {
        // CBC v8+ header
        let mut header = [0u8; 348];

        // File ID
        header[0..4].copy_from_slice(&CBC_FILE_ID.to_le_bytes());

        // RSA signature field (used as banner)
        let banner_bytes = banner.map(|s| s.as_bytes()).unwrap_or(CBC_BANNER);
        let len = banner_bytes.len().min(256);
        header[4..4 + len].copy_from_slice(&banner_bytes[..len]);

        // CB version
        header[260..264].copy_from_slice(&0x0800u32.to_le_bytes());

        // Game title
        let title = cheats.games[0].title.as_bytes();
        let len = title.len().min(72);
        header[264..264 + len].copy_from_slice(&title[..len]);

        // Data offset
        header[336..340].copy_from_slice(&348u32.to_le_bytes());

        std::io::Write::write_all(&mut file, &header)
            .map_err(|e| format!("{}: write error: {}", outfile, e))?;
    }

    std::io::Write::write_all(&mut file, &data)
        .map_err(|e| format!("{}: write error: {}", outfile, e))?;

    Ok(())
}
