# Umoria Changelog

## HEAD - v5.7.0-beta

### Notable Changes

- Supported platforms: **Windows**, **macOS** and **Ubuntu** (Debian Linux).
- Remove support for all other platforms E.g. Atari ST, Amiga,
  MS DOS, "Classic" Mac OS (pre OS X), etc., etc.
- Use `clang-format`/`-tidy` to clean up the code formatting.


### All Changes


`2016-11-14`

- Updates and moves the game `manual.md` to the `docs` folder


`2016-11-10`

- Adds `.dat` extension to scores and save game.
- Renames `files` directory to `data`.


`2016-11-08`

- Removes remaining uneeded system dependent defines.
- Reorganise and clean up the various text `files`.
- Removes all remaining HOURS code.
- Delete files/COPYING and use LICENSE for instead.
- Open save/score files in binary mode for Windows support.


`2016-11-05`

- WIP Windows Support: Successfully compiles, although not yet playable.
- First try at a Windows/MinGW makefile.
- Fixes some minor Windows GCC compile errors.


`2016-10-24`

- Experiment on Windows non-blocking keyboard reads for `check_input()`.


`2016-10-23`

- Adds `sleep_in_seconds()` wrapper for cross platform sleeping.
- Replaces custom `fseek` _origin_ defines with standard `SEEK_SET` and `SEEK_CUR`.
- Refactor `death.c` to use standard `fopen`/`fclose`.


`2016-10-22`

- Initial work on Windows support
- Fix more implicit type conversions.


`2016-10-18`

- Converts to single-line comments, and Prettify them.
- Removes some unused values
- Fixes for "hides upper scope", redundant castings, "signedness mismatch",
  and other minor issues.


`2016-10-17`

- Fixes for signedness mismatch, redundant castings, hides upper scope issues
- Fixes numerous unread variables and other warnings.
- Fix numerous uninitialised variable warnings by initialising them!
- Fix numerous implicit type conversions when loss of precision occurs.
- Updates prototypes for static functions



`2016-10-16`

- Updates variable declarations: initialize at point of first use.


`2016-10-15`

- Converts deprecated K&R style function declarations.
- Cleans up function prototypes in `externs.h` and elsewhere.


`2016-10-14`

- Updates many function signatures/returns to use the `bool` type for booleans
- Removes unneeded braces around returns types


`2016-10-12`

- Change Globals in `variables.c`: use `bool` type for booleans.
- Update many local bool's to use an actual `bool` type.


`2016-10-10`

- Use the `_Bool` standard type in place of those `TRUE`/`FALSE`.
- Replaces custom types `int8u`, `int16`, `int16u`, `int32`, and `int32u`
  with their equivalent C _standard types_, such as `uint16_t`.
- Replace `unsigned char` with standard `uint8_t` type.


`2016-10-09`

- Default ncurses `ESCDELAY` on macOS is 1 second! Reduced to just 50ms.
- Removes play time restrictions (`MORIA_HOU`)
- Remove deprecated `index()` and use standard `strchr()`.
- Removes support for `HPUX`, `M_XENIX` systems, and the `unix` specific defines.


`2016-10-08`

- Compiles and runs on Mac OS X! [WIP]


`2016-10-07`

- Moves all standard library includes into one header file.


`2016-10-06`

- Compiles using Clang `gnu11`!


`2016-10-05`

- Replaces the `long` types with `int32`.
- Replaces the few `unsigned short` with `uint16_t`.
- Replaces deprecated `register` keyword on variable declarations.


`2016-10-03`

- Remove support for discontinued computers and OS: Atari ST, Amiga, MS DOS,
  "Classic" Mac OS (pre OS X), VMS, System III, APOLLO, Pyramid, Ultrix.
- Various other minor compile `#define`s removed (e.g. Andrew FS).


`2016-10-02`

- Moves/merges all old document files into the `historical` directory.
- Deletes old computer and OS directories/files (`amiga`, `ibmpc`, `mac`, etc.).
- Format source code using `clang-tidy` and `clang-format`.
- Fix some `sprintf`: format `%ld` error for `int` types.
- Initial Moria Restoration Commit.
