# Umoria Changelog


## 2016-10-03

- Remove support for discontinued computers and OS:
    * Atari ST
    * Amiga

    * MS DOS
    * "Classic" Mac OS (pre OS X)

    * VMS
    * System III
    * APOLLO
    * Pyramid
    * Ultrix

- Various other minor compile `#define`s reomved (e.g. Andrew FS)


## 2016-10-01, 2016-10-02

- Moves/merges all old document files into the `historical` directory.
- Deletes old computer and OS directories/files (`amiga`, `ibmpc`, `mac`, etc.)
- Format source code using `clang-tidy` and `clang-format`.
