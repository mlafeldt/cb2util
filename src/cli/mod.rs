// Based on http://is.gd/8TYNHp
macro_rules! abort {
    ($($arg:tt)*) => (
        {
            use ::std::io::prelude::*;
            write!(&mut ::std::io::stderr(), "error: {}\n", format_args!($($arg)*)).unwrap();
            ::std::process::exit(1);
        }
    )
}

pub mod cheats;
