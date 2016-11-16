# Umoria Change log

## HEAD 5.7.0-beta

Lots of code clean-up and standardization, along with removing support for
outdated systems. The main feature of this release is support for Windows
and macOS.

### Notable changes

- **Windows**, **macOS** and **Ubuntu** (Debian Linux) support.
- Use `clang-format`/`-tidy` to clean up the code formatting.
- Moves all standard library header includes into one file.
- Replaces custom types (e.g. `int8u`, `int16u`, etc.) with their equivalent
  C standard types.
- Introduce the `_Bool` type.
- Converts deprecated K&R style function declarations.
- Refactor all comments.
- Reorganise all old document files, create a `historical` directory.

### Deprecated

- Remove support for discontinued computers and OS: Atari ST, Amiga, MS DOS,
  "Classic" Mac OS (pre OS X), VMS, System III, etc., etc.



## 5.6.0 (2015-02)

Umoria is released under a new GPL v2 license. More information is available
on the [free-moria](http://free-moria.sourceforge.net/) website.

All previous changes can be found in the [historical/CHANGELOG](historical/CHANGELOG).