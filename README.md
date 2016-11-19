# Umoria

_The Dungeons of Moria_ is a single player dungeon simulation originally
written by Robert Alan Koeneke, with v1.0 released in 1983. The game was
originally written in VMS Pascal before being ported to the C language and
released as _Umoria_ in 1988. Moria has had many variants over the years, with
[_Angband_](http://rephial.org/) being the most well known, and was also an
inspiration for one the most commercially successful roguelike-like games,
_Diablo_!

Supported Platforms:

  - Windows
  - macOS
  - Linux* (Ubuntu/Debian, Fedora)

_* other Linux distros may work, but have not yet been tested._


## Umoria Restoration Release: v5.7.0

The main focus of the `v5.7.0` release was to provide support for Windows
and macOS users, while at the same time removing support for the many
outdated computer system such as, MS DOS, "Classic" Mac OS (pre OS X), Amiga,
and Atari ST.

_Note: there are no gameplay changes in this release._

During the process a great deal of _code restoration_ was undertaken to aid
future development of the game. This included tasks such as reformatting the
source code with the help of `clang-tidy` and `clang-format`, modernising the
code to use standard C types, and fixing all warnings while compiling against
recent versions of GCC. Details of all changes can be found in the
[CHANGELOG](CHANGELOG.md), and by browsing the commit history.

Due to its lack of Windows and Mac support, Moria was unplayable for many
people. Hopefully these changes will give more people a chance to play this
classic game.


## Notes on Compiling Umoria

At present Umoria has only been compiled against `GCC 6.2` and `ncurses 6.0`,
although some slightly earlier versions should also work fine. You will
require these along with the normal C build tools for your system.

### macOS and Debian Linux

- Copy the `makefile` to the `src` directory.
- `cd` into the `src` directory.
- Type `make && make install`

Currently the game will be installed to `~/umoria`.


### Windows

MinGW is used to provide GCC and Binutils for the Windows platform. This
can be installed via the [MSYS2 Installer](http://msys2.github.io/). Once
installed you will use `pacman` to install `GCC` and `ncurses`.

- Copy the `makefile.win` to the `src` directory.
- Rename `makefile.win` to `makefile`.
- `cd` into the `src` directory.
- Type `make && make install`

Currently the game will be installed to `C:\umoria`.


## Historical Documents

Most of the original documents included in the Umoria 5.6 sources have been
moved to the [historical](historical/) directory. You will even find the old
CHANGELOG which tracks all the code changes made between versions 4.81 and
5.5.2 (1987-2008).

If you'd like to learn more on the development history of Umoria, these can
make interesting reading.


## License Information

Umoria is released under the [General Public License version 2](LICENSE) (GPL v2).

In 2007 Ben Asselstine and Ben Shadwick started the
[_free-moria_](http://free-moria.sourceforge.net/) project to re-license
UMoria 5.5.2 under GPL v2 by obtaining permission from all the contributing
authors. Some eight years later, they finally succeeded in their goal and in
early 2015 the Moria maintainer, David Grabiner, released Umoria 5.6 under a
GPL v2 license.
