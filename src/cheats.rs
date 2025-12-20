//! Cheat code parsing and data structures (libcheats replacement)

use std::io::{BufRead, BufReader, Read};

/// Maximum title/description length
const MAX_TITLE_LEN: usize = 80;

/// A single cheat code (address + value pair)
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct Code {
    pub addr: u32,
    pub val: u32,
}

/// A cheat with description and codes
#[derive(Debug, Clone)]
pub struct Cheat {
    pub desc: String,
    pub codes: Vec<Code>,
}

/// A game with title and cheats
#[derive(Debug, Clone)]
pub struct Game {
    pub title: String,
    pub cheats: Vec<Cheat>,
}

/// Collection of games with their cheats
#[derive(Debug, Clone, Default)]
pub struct CheatDb {
    pub games: Vec<Game>,
}

/// Parse error with line number
#[derive(Debug)]
pub struct ParseError {
    pub line: usize,
    pub message: String,
}

impl std::fmt::Display for ParseError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "line {}: {}", self.line, self.message)
    }
}

impl std::error::Error for ParseError {}

/// Check if string is a game title (quoted string)
fn is_game_title(s: &str) -> bool {
    s.len() > 2 && s.starts_with('"') && s.ends_with('"')
}

/// Check if string is a cheat code (16 hex digits)
fn is_cheat_code(s: &str) -> bool {
    let hex_count: usize = s.chars().filter(|c| c.is_ascii_hexdigit()).count();
    let non_hex_non_space = s.chars().any(|c| !c.is_ascii_hexdigit() && !c.is_whitespace());
    hex_count == 16 && !non_hex_non_space
}

/// Parse a cheat code line into addr and val
fn parse_code(s: &str) -> Option<Code> {
    let digits: String = s.chars().filter(|c| c.is_ascii_hexdigit()).collect();
    if digits.len() != 16 {
        return None;
    }
    let addr = u32::from_str_radix(&digits[0..8], 16).ok()?;
    let val = u32::from_str_radix(&digits[8..16], 16).ok()?;
    Some(Code { addr, val })
}

/// Remove C++-style comments from a line
fn strip_comment(line: &str) -> &str {
    if let Some(pos) = line.find("//") {
        &line[..pos]
    } else {
        line
    }
}

impl CheatDb {
    /// Create an empty cheat database
    pub fn new() -> Self {
        Self { games: Vec::new() }
    }

    /// Parse cheats from a reader
    pub fn read<R: Read>(&mut self, reader: R) -> Result<(), ParseError> {
        self.games.clear();

        let reader = BufReader::new(reader);
        let mut current_game: Option<Game> = None;
        let mut current_cheat: Option<Cheat> = None;
        let mut expect_title = true;

        for (line_num, line_result) in reader.lines().enumerate() {
            let line_num = line_num + 1;
            let line = line_result.map_err(|e| ParseError {
                line: line_num,
                message: e.to_string(),
            })?;

            let line = strip_comment(&line).trim();
            if line.is_empty() {
                continue;
            }

            if is_game_title(line) {
                // Save previous cheat to previous game
                if let (Some(cheat), Some(game)) = (current_cheat.take(), current_game.as_mut()) {
                    game.cheats.push(cheat);
                }
                // Save previous game
                if let Some(game) = current_game.take() {
                    self.games.push(game);
                }

                let title = &line[1..line.len() - 1];
                let title = if title.len() > MAX_TITLE_LEN {
                    &title[..MAX_TITLE_LEN]
                } else {
                    title
                };
                current_game = Some(Game {
                    title: title.to_string(),
                    cheats: Vec::new(),
                });
                expect_title = false;
            } else if is_cheat_code(line) {
                if expect_title {
                    return Err(ParseError {
                        line: line_num,
                        message: "parse error: cheat code invalid here".to_string(),
                    });
                }
                let code = parse_code(line).ok_or_else(|| ParseError {
                    line: line_num,
                    message: "invalid cheat code format".to_string(),
                })?;

                if let Some(cheat) = current_cheat.as_mut() {
                    cheat.codes.push(code);
                } else {
                    return Err(ParseError {
                        line: line_num,
                        message: "parse error: cheat code invalid here".to_string(),
                    });
                }
            } else {
                // Cheat description
                if expect_title {
                    return Err(ParseError {
                        line: line_num,
                        message: "parse error: cheat description invalid here".to_string(),
                    });
                }

                // Save previous cheat
                if let (Some(cheat), Some(game)) = (current_cheat.take(), current_game.as_mut()) {
                    game.cheats.push(cheat);
                }

                let desc = if line.len() > MAX_TITLE_LEN {
                    &line[..MAX_TITLE_LEN]
                } else {
                    line
                };
                current_cheat = Some(Cheat {
                    desc: desc.to_string(),
                    codes: Vec::new(),
                });
            }
        }

        // Save final cheat and game
        if let (Some(cheat), Some(game)) = (current_cheat.take(), current_game.as_mut()) {
            game.cheats.push(cheat);
        }
        if let Some(game) = current_game.take() {
            self.games.push(game);
        }

        Ok(())
    }

    /// Parse cheats from a file
    pub fn read_file(&mut self, path: &str) -> Result<(), ParseError> {
        let file = std::fs::File::open(path).map_err(|e| ParseError {
            line: 0,
            message: format!("could not open input file {}: {}", path, e),
        })?;
        self.read(file)
    }

    /// Write cheats to a writer
    #[allow(dead_code)]
    pub fn write<W: std::io::Write>(&self, mut writer: W) -> std::io::Result<()> {
        for game in &self.games {
            writeln!(writer, "\"{}\"", game.title)?;
            for cheat in &game.cheats {
                writeln!(writer, "{}", cheat.desc)?;
                for code in &cheat.codes {
                    writeln!(writer, "{:08X} {:08X}", code.addr, code.val)?;
                }
            }
        }
        Ok(())
    }
}
