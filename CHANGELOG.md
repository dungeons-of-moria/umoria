# Umoria Changelog


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
