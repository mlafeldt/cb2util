//! PCB file handling (CodeBreaker PS2 executables)

use crate::crypto;
use std::io::Write;

/// PCB header size (RSA signature)
const PCB_HDR_SIZE: usize = 256;

/// Address at which PCB data is loaded in PS2 memory
const PCB_LOAD_ADDR: u32 = 0x01000000;

/// PS2 processor-specific ELF flags
const EF_PS2: u32 = 0x20924001;

/// ELF constants
const ELFMAG: [u8; 4] = [0x7f, b'E', b'L', b'F'];
const ELFCLASS32: u8 = 1;
const ELFDATA2LSB: u8 = 1;
const EV_CURRENT: u8 = 1;
const ET_EXEC: u16 = 2;
const EM_MIPS: u16 = 8;
const PT_LOAD: u32 = 1;
const PF_X: u32 = 1;
const PF_W: u32 = 2;
const PF_R: u32 = 4;

/// Generate ELF header for PCB to ELF conversion
fn gen_elf_header(data_len: usize) -> [u8; 256] {
    let mut hdr = [0u8; 256];
    let note = b"Created with cb2util";

    // ELF header (52 bytes)
    hdr[0..4].copy_from_slice(&ELFMAG);
    hdr[4] = ELFCLASS32; // e_ident[EI_CLASS]
    hdr[5] = ELFDATA2LSB; // e_ident[EI_DATA]
    hdr[6] = EV_CURRENT; // e_ident[EI_VERSION]

    hdr[16..18].copy_from_slice(&ET_EXEC.to_le_bytes()); // e_type
    hdr[18..20].copy_from_slice(&EM_MIPS.to_le_bytes()); // e_machine
    hdr[20..24].copy_from_slice(&1u32.to_le_bytes()); // e_version
    hdr[24..28].copy_from_slice(&PCB_LOAD_ADDR.to_le_bytes()); // e_entry
    hdr[28..32].copy_from_slice(&52u32.to_le_bytes()); // e_phoff
    hdr[32..36].copy_from_slice(&0u32.to_le_bytes()); // e_shoff
    hdr[36..40].copy_from_slice(&EF_PS2.to_le_bytes()); // e_flags
    hdr[40..42].copy_from_slice(&52u16.to_le_bytes()); // e_ehsize
    hdr[42..44].copy_from_slice(&32u16.to_le_bytes()); // e_phentsize
    hdr[44..46].copy_from_slice(&1u16.to_le_bytes()); // e_phnum
    hdr[46..48].copy_from_slice(&40u16.to_le_bytes()); // e_shentsize
    hdr[48..50].copy_from_slice(&0u16.to_le_bytes()); // e_shnum
    hdr[50..52].copy_from_slice(&0u16.to_le_bytes()); // e_shstrndx

    // Program header (32 bytes, starting at offset 52)
    hdr[52..56].copy_from_slice(&PT_LOAD.to_le_bytes()); // p_type
    hdr[56..60].copy_from_slice(&256u32.to_le_bytes()); // p_offset (header size)
    hdr[60..64].copy_from_slice(&PCB_LOAD_ADDR.to_le_bytes()); // p_vaddr
    hdr[64..68].copy_from_slice(&PCB_LOAD_ADDR.to_le_bytes()); // p_paddr
    hdr[68..72].copy_from_slice(&(data_len as u32).to_le_bytes()); // p_filesz
    hdr[72..76].copy_from_slice(&(data_len as u32).to_le_bytes()); // p_memsz
    hdr[76..80].copy_from_slice(&(PF_X | PF_W | PF_R).to_le_bytes()); // p_flags
    hdr[80..84].copy_from_slice(&0u32.to_le_bytes()); // p_align

    // Append note after program header
    let note_start = 84;
    let note_len = note.len().min(256 - note_start);
    hdr[note_start..note_start + note_len].copy_from_slice(&note[..note_len]);

    hdr
}

/// PCB operation mode
#[derive(Clone, Copy, PartialEq, Eq)]
pub enum PcbMode {
    Crypt,
    CryptStrip,
    Elf,
    Verify,
}

/// Process PCB file
pub fn process_pcb(infile: &str, outfile: Option<&str>, mode: PcbMode) -> Result<(), String> {
    let buf = std::fs::read(infile).map_err(|e| format!("{}: read error: {}", infile, e))?;

    if buf.len() < PCB_HDR_SIZE {
        return Err(format!("{}: not a PCB file", infile));
    }

    let sig: [u8; 256] = buf[..PCB_HDR_SIZE].try_into().unwrap();
    let data = &buf[PCB_HDR_SIZE..];

    match mode {
        PcbMode::Verify => {
            let valid = crypto::verify_signature(&sig, data);
            println!("{}: {}", infile, if valid { "OK" } else { "FAILED" });
            if !valid {
                return Err(String::new());
            }
        }
        _ => {
            let outfile = outfile.ok_or_else(|| "output file required".to_string())?;

            // Decrypt/encrypt data
            let mut decrypted = data.to_vec();
            crypto::crypt_data(&mut decrypted);

            let mut file = std::fs::File::create(outfile)
                .map_err(|e| format!("{}: write error: {}", outfile, e))?;

            match mode {
                PcbMode::Elf => {
                    // Replace signature with ELF header
                    let elf_hdr = gen_elf_header(decrypted.len());
                    file.write_all(&elf_hdr)
                        .map_err(|e| format!("{}: write error: {}", outfile, e))?;
                    file.write_all(&decrypted)
                        .map_err(|e| format!("{}: write error: {}", outfile, e))?;
                }
                PcbMode::CryptStrip => {
                    // Write only data (no signature)
                    file.write_all(&decrypted)
                        .map_err(|e| format!("{}: write error: {}", outfile, e))?;
                }
                PcbMode::Crypt => {
                    // Write full file with signature
                    file.write_all(&sig)
                        .map_err(|e| format!("{}: write error: {}", outfile, e))?;
                    file.write_all(&decrypted)
                        .map_err(|e| format!("{}: write error: {}", outfile, e))?;
                }
                PcbMode::Verify => unreachable!(),
            }
        }
    }

    Ok(())
}
