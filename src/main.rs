//! cb2util - CodeBreaker PS2 File Utility
//!
//! Supports:
//! - Code saves (v7 and v8+/Day1) - .cbc files
//! - "cheats" files (v7+) - internal code database
//! - PCB files - encrypted executables

mod cheats;
mod cmd_cbc;
mod cmd_cheats;
mod cmd_pcb;
mod crypto;

use clap::{Parser, Subcommand, ValueEnum};
use cmd_cbc::DecryptMode;
use cmd_pcb::PcbMode;
use std::process::ExitCode;

const VERSION: &str = env!("CARGO_PKG_VERSION");

#[derive(Parser)]
#[command(name = "cb2util")]
#[command(version = VERSION)]
#[command(about = "CodeBreaker PS2 File Utility")]
#[command(propagate_version = true)]
#[command(subcommand_required = true)]
#[command(arg_required_else_help = true)]
struct Cli {
    #[command(subcommand)]
    command: Option<Commands>,
}

#[derive(Subcommand)]
enum Commands {
    /// Process CBC files (code saves)
    Cbc {
        /// Files are in CBC v7 format
        #[arg(short = '7')]
        v7: bool,

        /// Custom banner for compiled CBC v8+ files
        #[arg(short, long)]
        banner: Option<String>,

        /// Compile text to CBC file
        #[arg(short, long)]
        compile: bool,

        /// Decrypt extracted cheats (auto or force)
        // require_equals: --decrypt=force syntax; num_args 0..=1: bare --decrypt allowed;
        // default_missing_value: bare --decrypt means auto
        #[arg(short, long, require_equals = true, num_args = 0..=1, default_missing_value = "auto")]
        decrypt: Option<DecryptArg>,

        /// Verify RSA signature
        #[arg(short, long)]
        verify: bool,

        /// Input files (or input/output pairs for compile)
        files: Vec<String>,
    },

    /// Process cheats files (internal code database)
    Cheats {
        /// Compile text to cheats file
        #[arg(short, long)]
        compile: bool,

        /// Decrypt extracted cheats (auto or force)
        // require_equals: --decrypt=force syntax; num_args 0..=1: bare --decrypt allowed;
        // default_missing_value: bare --decrypt means auto
        #[arg(short, long, require_equals = true, num_args = 0..=1, default_missing_value = "auto")]
        decrypt: Option<DecryptArg>,

        /// Input files (or input/output pairs for compile)
        files: Vec<String>,
    },

    /// Process PCB files (executables)
    Pcb {
        /// Strip RSA signature
        #[arg(short, long)]
        strip: bool,

        /// Convert into ELF file
        #[arg(short, long)]
        elf: bool,

        /// Verify RSA signature
        #[arg(short, long)]
        verify: bool,

        /// Input files (or input/output pairs)
        files: Vec<String>,
    },
}

#[derive(Clone, Copy, ValueEnum)]
enum DecryptArg {
    Auto,
    Force,
}

fn parse_decrypt_mode(opt: Option<DecryptArg>) -> DecryptMode {
    match opt {
        None => DecryptMode::Off,
        Some(DecryptArg::Auto) => DecryptMode::Auto,
        Some(DecryptArg::Force) => DecryptMode::Force,
    }
}

fn run_with_cli(cli: Cli) -> Result<(), String> {
    let command = cli.command.ok_or_else(|| "no command specified".to_string())?;

    match command {
        Commands::Cbc {
            v7,
            banner,
            compile,
            decrypt,
            verify,
            files,
        } => {
            if files.is_empty() {
                return Err("no input files".to_string());
            }

            if compile {
                if files.len() % 2 != 0 {
                    return Err("compile requires input/output file pairs".to_string());
                }
                if v7 && banner.is_some() {
                    return Err("CBC v7 files can't have a custom banner".to_string());
                }

                for pair in files.chunks(2) {
                    cmd_cbc::compile_cbc(&pair[0], &pair[1], v7, banner.as_deref())?;
                }
            } else {
                if v7 && verify {
                    return Err("CBC v7 files don't have a signature".to_string());
                }

                let decrypt_mode = parse_decrypt_mode(decrypt);
                let mut first = true;

                for file in &files {
                    if !first {
                        println!();
                    }
                    first = false;

                    if let Err(e) = cmd_cbc::process_cbc(file, v7, verify, decrypt_mode) {
                        if !e.is_empty() {
                            eprintln!("{}", e);
                        }
                        return Err(String::new());
                    }
                }
            }
        }

        Commands::Cheats {
            compile,
            decrypt,
            files,
        } => {
            if files.is_empty() {
                return Err("no input files".to_string());
            }

            if compile {
                if files.len() % 2 != 0 {
                    return Err("compile requires input/output file pairs".to_string());
                }

                for pair in files.chunks(2) {
                    cmd_cheats::compile_cheats_file(&pair[0], &pair[1])?;
                }
            } else {
                let decrypt_mode = parse_decrypt_mode(decrypt);
                let mut first = true;

                for file in &files {
                    if !first {
                        println!();
                    }
                    first = false;

                    if let Err(e) = cmd_cheats::process_cheats(file, decrypt_mode) {
                        eprintln!("{}", e);
                        return Err(String::new());
                    }
                }
            }
        }

        Commands::Pcb {
            strip,
            elf,
            verify,
            files,
        } => {
            if files.is_empty() {
                return Err("no input files".to_string());
            }

            let mode = if verify {
                PcbMode::Verify
            } else if elf {
                PcbMode::Elf
            } else if strip {
                PcbMode::CryptStrip
            } else {
                PcbMode::Crypt
            };

            if mode == PcbMode::Verify {
                for file in &files {
                    if let Err(e) = cmd_pcb::process_pcb(file, None, mode) {
                        if !e.is_empty() {
                            eprintln!("{}", e);
                        }
                        return Err(String::new());
                    }
                }
            } else {
                if files.len() % 2 != 0 {
                    return Err("requires input/output file pairs".to_string());
                }

                for pair in files.chunks(2) {
                    cmd_pcb::process_pcb(&pair[0], Some(&pair[1]), mode)?;
                }
            }
        }
    }

    Ok(())
}

fn main() -> ExitCode {
    match run_with_cli(Cli::parse()) {
        Ok(()) => ExitCode::SUCCESS,
        Err(e) => {
            if !e.is_empty() {
                eprintln!("error: {}", e);
            }
            ExitCode::FAILURE
        }
    }
}

