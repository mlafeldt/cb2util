History
=======


v1.9 (Jul 20 2013)
-----------------

* Add **cb2serve**, the CodeBreaker PS2 File Server, to the `contrib` folder.


v1.8 (Jan 2 2013)
-----------------

* Switch to MIT license and get rid of the GPL virus.

* Remove unused code to generate version from CMake.

* Add another "cheats" command example to README.

* Let Travis only test master branch (and pull requests).

* Update copyright year to 2013.


v1.7 (Jul 29 2012)
------------------

* Add **cb2get** (the script I used to download all code saves, game saves, and
  firmware updates from CMGSCCC) and **CB2cheats** (a tool to extract all
  pre-loaded cheat codes from CB2 RAM dumps) to cb2util. See `contrib` folder.

* Include libbig_int as git submodule.

* Move common code to extract and compile cheats to separate file.

* Let Travis CI run tests against gcc and clang.

* With `make release`, rebuild everything and run tests before packaging.


v1.6 (Jul 18 2012)
------------------

* Allow to insert a custom banner text into compiled code saves (v8+ only).
  As we cannot sign those files ourselves, there are 256 unused bytes we might
  fill up with 2Pac lyrics or some other kind of credit message.

* Update install instructions.


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
