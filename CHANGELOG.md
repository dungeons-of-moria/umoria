# Umoria Changes

## HEAD

- Rename `typedef struct` names in `type.h` to the standard format of suffixing `_t`.
- Other minor refactoring - see commit history for changes.



## 5.7.3 (2017-06-21)

### Bug Fixes

- Various repeat commands (e.g tunnelling) were broken in the 5.7.1 release.
- Remove `curs_set(0)` as a visible cursor is needed in menus!

### Documentation

- Add a `CODE_OF_CONDUCT`.
- Add a `CONTRIBUTING` guide.

### Code

_Extract Method_ refactoring was the main focus of this release. Along the way
many other refactorings were made, including:

- Refactor `if/else` statements:
  * Add guard clauses / early returns where possible.
  * Remove `else` when an `if` returns.
  * Makes many easier to understand (e.g. less indentation).
- Refactor `do/while` statements, to be just `while` statements where possible.
- Remove unneeded braces for `if` statement expressions.
- Rename many variables to make their function more obvious, but there's more to do in this area.
- Generally lots of small refactoring.



## 5.7.2 (2017-03-12)

- Disable terminal control chars (e.g. CTRL-C)
- Make splash screen logo a little nicer.



## 5.7.1 (2017-01-27)

- Improved CLI: adds _help_ and _version_ information (`-h` and `-v`).
- Lots of spelling fixes, mostly source code comments, but a few in-game also.
- Updates to the manual, FAQ, and historical/errors documents.

- Compiles to C++, with all warnings fixed! (`-Wall`, `-Wextra`, `-Werror`, `-Wshadow`)
- Now uses CMake for Mac/Linux build - Windows/MinGW still uses a normal Makefile.



## 5.7.0 (2016-11-27)

Lots of code clean-up and standardization, along with removing support for
outdated systems. The main feature of this release is support for Windows
and macOS.

### Notable changes

- **Windows**, **macOS** and **Linux** support.
- Renaming binary from `moria` to `umoria`, save file to `game.sav`,
  and scores to `scores.dat`.
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