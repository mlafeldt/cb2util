History
=======


v1.5 (Jul 17 2012)
------------------

* Add ability to compile code saves for CB v7 and v8+. Note that you need a
  hacked CB in order to use those files. (Requested by bfoos)

* Rename `--check` option to `--verify` so that the short option `-c` is
  available for the new compile feature.


v1.4 (Jul 16 2012)
------------------

* The "cheats" compiler no longer supports script-style `#` comments. The
  character `#` is often used in cheat descriptions to mean "number".
  Treating it as comment would mangle such lines. (Suggested by dlanor)

* Fix `make install` to create target folder.

* Fix return value of `big_int_hamming_distance()` in libbig_int.

* Run tests using Sharness v0.2.4.

* Add Travis CI config.

* Update documentation and convert it to markdown.

* Update license and copyright notices.


v1.3 (Jan 6 2011)
-----------------

* Decrypting extracted cheats now also discards "beefcodes" and succeeding
  000FFFFE/000FFFFF lines. Those codes are nothing more than CB bloat and
  useless on other cheat devices. (Requested by dlanor)

* Fix: Do not decrypt raw codes with leading beefcode. (Reported by dlanor)

* Disable code encryption in `cb2_crypto.c` - not needed by cb2util.


v1.2 (Jan 3 2011)
-----------------

* Added smart decryption of extracted cheats:
  The commands `cheats -d` and `cbc -d` tried to decrypt all codes, even if
  they were already in raw format. A mechanism has been added to detect which
  codes need to be decrypted and how. Now users can properly import "cheats"
  files with both encrypted and decrypted codes. (Suggested by dlanor)

* Improved Makefile:
  - auto-generate version info
  - quiet build

* Improved automated tests:
  - aggregate test results
  - create test area for each test
  - split up tests in multiple files
  - add more test data


v1.1 (Dec 21 2010)
------------------

* Added support for "cheats" files:
  - compile your own "cheats" file which can be copied to `mc0:/PCB/cheats`
  - extract (and decrypt) all cheats from those files

* Reworked command line interface (CLI).

* Added automated tests.

* Improved build system and added CMake listfile.


v1.0 (Apr 3 2007)
-----------------

* Initial public release
