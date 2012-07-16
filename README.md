cb2util - CodeBreaker PS2 File Utility
======================================

cb2util was made to utilize different file formats of CodeBreaker PS2.

It currently supports:

- Code saves (v7 and v8+/Day1)
- PCB files (upgrades/homebrew/etc)
- "cheats" files (v7+)

The features are:

- extract (and decrypt) all cheats from code saves and "cheats" files
- compile your own "cheats" files
- encrypt or decrypt PCB files
- convert PCB files into ELF files
- check digital signature on code saves and PCB files


Installation
------------

To build and install cb2util, simply run:

    $ make
    $ make install

CMake is supported too:

    $ mkdir build
    $ cd build/
    $ cmake ..
    $ make
    $ make install


Usage
-----

As cb2util is a command-line application, you have to pass arguments to it.

Using the `--help` option will display the following text:

    $ cb2util --help
    usage: cb2util [--version] [--help] <command> [<args>]

    The available commands are:
        cbc
        pcb
        cheats

    Try 'cb2util help <command>' for more information.

As written above, you must specify the command to use followed by its arguments.
There's one command for each of the supported file formats, e.g. `cbc` for code
saves. To learn how a command is supposed to work, run `cb2util help <command>`.

Below are some notes and examples concerning the different file formats.


### Code saves (v7 and v8+/Day1)

File extension: `*.cbc`

Code saves (also known as Day1 or CBC files) store cheat codes that can be added
to your CodeBreaker's code list. While older code saves for CB v7 are only
encrypted, newer files for CB v8+ and CB Day1 are digitally signed as well.

    usage: cb2util cbc [-c | -d[mode]] <file>...
       or: cb2util cbc -7 [-d[mode]] <file>...

        no option
            extract cheats

        -d[mode], --decrypt[=mode]
            decrypt extracted cheats
            mode can be "auto" (default) or "force"

        -c, --check
            check RSA signature

        -7
            files are in CBC v7 format

Examples:

Extract cheats from mgs3.cbc and print them to screen:

    $ cb2util cbc mgs3.cbc

Extract cheats, decrypt encrypted cheats, and print them to screen:

    $ cb2util cbc -d mgs3.cbc

Same as previous example, but force decryption of _all_ cheats:

    $ cb2util cbc -dforce mgs3.cbc

Check digital signature of re4.cbc:

    $ cb2util cbc -c re4.cbc

Extract and decrypt cheats from v7 code save re4.cbc, write them to re4.txt:

    $ cb2util cbc -7 -d re4.cbc > re4.txt


### PCB files

File extension: `*.bin`

PCB files are encrypted and digitally signed binaries that can be executed by
the CodeBreaker PS2. In theory, this could be any application; we've seen
upgrades and homebrew so far.

    usage: cb2util pcb [-s] <infile> <outfile>...
       or: cb2util pcb -e <infile> <outfile>...
       or: cb2util pcb -c <file>...

        no option
            encrypt/decrypt file

        -s, --strip
            strip RSA signature

        -e, --elf
            convert into ELF file

        -c, --check
            check RSA signature

Note: PCB files are encrypted with a symmetric cipher (RC4) and cb2util actually
doesn't care if it's encrypting or decrypting.

Examples:

Decrypt pelican.bin to pelican.raw:

    $ cb2util pcb pelican.bin pelican.raw

Decrypt pelican.bin to pelican.raw and strip RSA signature:

    $ cb2util pcb -s pelican.bin pelican.raw

Convert pelican.bin into the ELF file pelican.elf:

    $ cb2util pcb -e pelican.bin pelican.elf

Check RSA signature of pelican.bin:

    $ cb2util pcb -c pelican.bin


### "cheats" files

The "cheats" file is CodeBreaker's internal code database. It is usually saved
to memory card (`mc0:/PCB/cheats`) and updated everytime the code list is changed.

    usage: cb2util cheats [-d[mode]] <file>...
       or: cb2util cheats -c <infile> <outfile>...

        no option
            extract cheats

        -d[mode], --decrypt[=mode]
            decrypt extracted cheats
            mode can be "auto" (default) or "force"

        -c, --compile
            compile text to cheats file

Examples:

Extract all cheats from "cheats" file and print them to screen:

    $ cb2util cheats /path/to/cheats

Extract all cheats, decrypt encrypted cheats, and print them to screen:

    $ cb2util cheats -d /path/to/cheats

Same as previous example, but force decryption of _all_ cheats:

    $ cb2util cheats -dforce /path/to/cheats

Compile cheats in mygames.txt to "cheats" file:

    $ cb2util cheats -c mygames.txt /path/to/cheats

You can use your own "cheats" file with CodeBreaker in just a few steps:

1. create a text file containing your cheats; the format is described below
2. invoke cb2util to compile the text file to a "cheats" file
3. use your favorite method to transfer the file to `mc0:/PCB/cheats`
4. start CodeBreaker to see your cheats ready to be used

The format of the text file is quite simple:

    "Game title 1"
    Cheat description 1
    Cheat code 1
    Cheat code 2
    Cheat description 2
    Cheat code 1
    Cheat code 2

    "Game title 2"
    etc.

Also, C++-style comments are allowed; all text beginning with a `//` sequence to
the end of the line is ignored.

Sample:

    "TimeSplitters"
    // some senseless comment
    Mastercode
    902D51F8 0C0B95F6
    Invincible
    203C8728 00000001


### Game saves

File extension: `*.cbs`

Comming soon... ;)


Disclaimer
----------

THIS PROGRAM IS NOT LICENSED, ENDORSED, NOR SPONSORED BY SONY COMPUTER
ENTERTAINMENT, INC. NOR PELICAN ACCESSORIES, INC.
ALL TRADEMARKS ARE PROPERTY OF THEIR RESPECTIVE OWNERS.

cb2util comes with ABSOLUTELY NO WARRANTY. It is covered by the GNU General
Public License. Please see file [COPYING] for further information.


Special Thanks
--------------

- Thanks to the Free Software Foundation -- Open Source is a blessing!
- Alexander Valyalkin for his great BIG_INT library.
- Peter C. Gutmann and Paul Rubin for the fast implementation of SHA-1.
- Vector for making PS2 Save Builder and saving me some time. ;)
- Gtlcpimp who has awakened my interest in cb2util again and provided valuable
  information on the "cheats" compression.
- Greets to all the people that contribute to our code hacking community!


Contact
-------

* Web: <http://mlafeldt.github.com/cb2util>
* Mail: <mathias.lafeldt@gmail.com>
* Twitter: [@mlafeldt](https://twitter.com/mlafeldt)


[COPYING]: https://github.com/mlafeldt/cb2util/blob/master/COPYING
