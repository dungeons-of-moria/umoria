# Umoria CHANGELOG - this file tracks changes since v5.6.0

## HEAD


### Code

Continuing the process of moving related functions to the same file.

- `types.h` now has just the two core `vtype_t` and `obj_desc_t` types.
- The numbered `misc` and `moria` files no longer exist!
- Moved `sets.cpp` functions elsewhere, allowing for most to become `static`.
- `externs.h` now contains only `externs`.


## 5.7.8 (2017-12-24)

- Improve _Wizard Mode_ help files.
- Easier item creation in _Wizard Mode_ (inspired by Bmoria).
- Change `[Press any key to continue.]` to `[ press any key to continue ]`,
  and any variations, as it looks clearer.

### Code

There are two main areas of focus for this release. The first was to create
more objects to move the numerous global variables on to. E.g. `Dungeon_t`.

The second area of focus has been to start grouping related functions and
variables in the same file. Ex. most player functions are moved to `player.cpp`,
or a new `player_xxx.cpp` file has been created (run, stats, tunnel, etc.).
The LOS and look functions are located in `los.cpp`.

Grouping globals and functions together like this should make their usage and
their relationships more obvious.

These locations are by no means final, but are a useful first pass.

- Grouping related functions together in the same file.
- Move more Player globals to the `Player_t` struct.
- Create a `Dungeon_t` and put all dungeon related globals here.
- Create a `Dice_t` instead of using an array.
- Started replacing `char *` with `std::string`.
- Simpler display of _death_ screens (using `death_tomb.txt`, `death_royal.txt` files).


## 5.7.7 (2017-11-07)

### Bug Fixes

- Game loading fix where shops data was being read twice, and the second time
  resulted in incorrect data. Oops! Broken during save game refactor:
  https://github.com/dungeons-of-moria/umoria/commit/ce2c756


## 5.7.6 (2017-11-05)

### Bug Fixes

- When compiling on Windows/MinGW, the OS was not being detected correctly.
  https://github.com/dungeons-of-moria/umoria/commit/3811bcd
- Now load cave tiles correctly (the `lit` status values), as a previous
  refactoring broke loading of pre-5.7 saves.
  https://github.com/dungeons-of-moria/umoria/commit/219f350

### Code

- _Pass by Value_ and _Pass by Reference_ major refactoring.
- Replace "magic numbers" with ENUMs, in `staffs.cpp`.
- Use `Coord_t` instead of `y`/`x` values in various coordinated related functions.


## 5.7.5 (2017-09-05)

### Notable changes

- CLI: Added a parameter to provide a custom game seed value, using `-s`.
- CLI: Display high scores flag now changed to `-d`, because  of the game seed.
- CLI: Remove `-o` and just use the one arg (`-r`) for specifying (forcing)
  the use of roguelike keys (`hjkl`).
- Renamed the data/help files. E.g. `owizcmds.hlp` > `help.txt`.
- Support only save/score files for Umoria `5.2.2` up to `5.7.x`.

### Bug Fixes

- `TV_NEVER` was an unsigned int, but should be signed as it's being given
  a `-1` value. [https://github.com/dungeons-of-moria/umoria/commit/8c3d1d2]
- The `monsterTakeHit()` check is now correct in `spellLightLineTouchesMonster()`.
  [https://github.com/dungeons-of-moria/umoria/commit/b26547d]
- When player was in _Run/Find_ mode, the `find_count` was not being
  dereferenced, so `playerEndRunning()` would not be called correctly.
  [https://github.com/dungeons-of-moria/umoria/commit/95dc308]

### Code

- Add an optimized "release" build, with an `-O2` optimization flag.
- Setting version number in version.h now updates data files automatically, via CMake.
- Release date in `data/versions.txt` is now set automatically via CMake.
- Rename field names for all structs in `types.h`.
- Rename many constants for better clarity on their purpose.
- Move the game options globals into a Config_t struct.
- Refactor `main.cpp` to contain [mostly] just CLI arg parsing, and some
  basic game initialization. All other game logic moved to `moria.cpp`.
- Lots of clang-tidy based refactoring.


## 5.7.4 (2017-08-06)

### Bug Fixes

- The create food spell was creating random items instead of just food.

### Code

The main focus of this release was to rename all functions, function variables,
and global variables. The old naming did not reveal their purpose very well,
which made understanding the code more difficult. These are not meant to be
final names, they have been chosen to aid future refactoring tasks.

Additionally:

* The `variable` file was renamed to `globals`.
* Many defines (mainly in `constant.h`) changed to a `constexpr`.



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



## 5.6.0 (2008-10-13)

Umoria is released under a new GPL v2 license. More information is available
on the [free-moria](http://free-moria.sourceforge.net/) website.

All previous changes can be found in the [historical/CHANGELOG](historical/CHANGELOG).
