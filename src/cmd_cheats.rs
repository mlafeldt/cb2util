//! "cheats" file handling (CodeBreaker's internal code database)

use crate::cheats::CheatDb;
use crate::cmd_cbc::{compile_cheats, extract_cheats, DecryptMode};
use flate2::read::ZlibDecoder;
use flate2::write::ZlibEncoder;
use flate2::Compression;
use std::io::{Read, Write};

const CHEATS_FILE_ID: u32 = 0x00554643; // "CFU\0"

/// Process cheats file (extract)
pub fn process_cheats(path: &str, decrypt: DecryptMode) -> Result<(), String> {
    let buf = std::fs::read(path).map_err(|e| format!("{}: read error: {}", path, e))?;

    if buf.len() < 8 {
        return Err(format!("{}: not a cheats file", path));
    }

    let fileid = u32::from_le_bytes([buf[0], buf[1], buf[2], buf[3]]);
    if fileid != CHEATS_FILE_ID {
        return Err(format!("{}: invalid cheats file ID", path));
    }

    // Decompress data (skip 8-byte header)
    let compressed = &buf[8..];
    let mut decoder = ZlibDecoder::new(compressed);
    let mut data = Vec::new();
    decoder
        .read_to_end(&mut data)
        .map_err(|_| format!("{}: uncompress error", path))?;

    extract_cheats(std::io::stdout(), &data, decrypt)
        .map_err(|e| format!("{}: {}", path, e))?;

    Ok(())
}

/// Compile text file to cheats file
pub fn compile_cheats_file(infile: &str, outfile: &str) -> Result<(), String> {
    let mut cheats = CheatDb::new();
    cheats
        .read_file(infile)
        .map_err(|e| format!("{}: line: {}\nerror: {}", infile, e.line, e.message))?;

    let data = compile_cheats(&cheats);

    // Compress data
    let mut encoder = ZlibEncoder::new(Vec::new(), Compression::best());
    encoder
        .write_all(&data)
        .map_err(|e| format!("{}: compress error: {}", infile, e))?;
    let compressed = encoder
        .finish()
        .map_err(|e| format!("{}: compress error: {}", infile, e))?;

    // Write output file
    let mut file =
        std::fs::File::create(outfile).map_err(|e| format!("{}: write error: {}", outfile, e))?;

    // Header
    file.write_all(&CHEATS_FILE_ID.to_le_bytes())
        .map_err(|e| format!("{}: write error: {}", outfile, e))?;
    file.write_all(&0x00010000u32.to_le_bytes())
        .map_err(|e| format!("{}: write error: {}", outfile, e))?;

    // Compressed data
    file.write_all(&compressed)
        .map_err(|e| format!("{}: write error: {}", outfile, e))?;

    Ok(())
}
