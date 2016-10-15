# Umoria Changelog

## HEAD - v5.7.0-beta

### Notable Changes

- Supported platforms: **macOS** and **Ubuntu** (Debian Linux).
- Remove support for all other platforms E.g. Atari ST, Amiga,
  MS DOS, "Classic" Mac OS (pre OS X), etc., etc.


### All Changes


`2016-10-15`

- Cleans up function prototypes in externs.h and elsewhere.


`2016-10-14`

- Updates many func signatures/returns to use the bool type for booleans
- Removes unneeded braces around returns types


`2016-10-12`

- Changes Globals in `variables.c`: use `bool` type for booleans.
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
- Replaces deprecated `register` keyword on var declarations.


`2016-10-03`

- Remove support for discontinued computers and OS: Atari ST, Amiga, MS DOS,
  "Classic" Mac OS (pre OS X), VMS, System III, APOLLO, Pyramid, Ultrix.
- Various other minor compile `#define`s reomved (e.g. Andrew FS).


`2016-10-02`

- Moves/merges all old document files into the `historical` directory.
- Deletes old computer and OS directories/files (`amiga`, `ibmpc`, `mac`, etc.).
- Format source code using `clang-tidy` and `clang-format`.
- Fix some sprintf: format ‘%ld’ error for int types.
- Initial Moria Restoration Commit.
