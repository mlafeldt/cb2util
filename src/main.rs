#[macro_use]
extern crate clap;
extern crate codebreaker;
extern crate flate2;

use clap::{App, AppSettings, Arg, SubCommand};

mod cli;
use cli::cheats;

fn main() {
    let m = App::new("cb2util")
        .version(crate_version!())
        .setting(AppSettings::SubcommandRequired)
        .subcommand(
            SubCommand::with_name("cheats")
                .about("Read or write CodeBreaker \"cheats\" files")
                .args(&[
                    Arg::from_usage("-d, --decrypt 'Decrypt extracted cheats'"),
                    Arg::from_usage("<INPUT> 'Path to \"cheats\" file to read'"),
                ]),
        )
        .get_matches();

    if let ("cheats", Some(m)) = m.subcommand() {
        cheats::run(m)
    }
}
