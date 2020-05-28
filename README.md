# Umoria

_The Dungeons of Moria_ is a single player dungeon simulation originally
written by Robert Alan Koeneke, with v1.0 released in 1983. The game was
originally developed in VMS Pascal before being ported to the C language and
released as _Umoria_ in 1988. Moria has had many variants over the years, with
[_Angband_](http://rephial.org/) being the most well known. Moria was also an
inspiration for one the most commercially successful action roguelike games,
_Diablo_!

Supported Platforms:

  - Windows
  - macOS
  - Linux (Ubuntu/Debian)

Compiling and limited testing done for:

  - NetBSD 8.1 (with gcc 5.5)
  - Fedora 32


## Umoria Restoration Release: v5.7

The main focus of the `v5.7` release is to provide support for the three main
operating systems: Windows, macOS, and Linux. Support for all other outdated
computer systems such as MS DOS, "Classic" Mac OS (pre OSX), Amiga, and
Atari ST has been removed.

_Note: there are no intentional game play changes in the 5.7.x releases._

A great deal of _code restoration_ has been undertaken in the hope of aiding
future development of the game. Examples of refactoring completed so far include
reformatting the source code with the help of `clang-tidy` and `clang-format`,
modernizing the code to use standard C types, and fixing all warnings while
compiling against recent versions of GCC and Clang.

Full details of all changes can be found in the [CHANGELOG](CHANGELOG.md), and
by browsing the commit history.

Due to its lack of Windows and Mac support, Moria was unplayable for many
people. Hopefully these changes will give more people a chance to play this
classic roguelike game.


## Notes on Compiling Umoria

At present Umoria has been tested against GCC `7.x`, and `8.1`, with
`ncurses 6.x`, although recent earlier versions should also work fine. You will
require these along with `CMake` and the C++ build tools for your system.


### macOS and Linux

Change to the `umoria` game directory and enter the following commands at the
terminal:

    $ cmake .
    $ make

To perform an out-of-source build, type the following:

    $ mkdir build && cd build
    $ cmake ..
    $ make

An `umoria` directory will be created in the current directory containing the
game binary and data files, which can then be moved to any other location, such
as the `home` directory.


### Windows

MinGW is used to provide GCC and GNU Binutils for compiling on the Windows platform.
The easiest solution to get set up is to use the [MSYS2 Installer](http://msys2.github.io/).
Once installed, `pacman` can be used to install `GCC`, `ncurses`, and the
`make`/`cmake` build tools.

At present an environment variable for the MinGW system being compiled on will
need to be specified. This will be either `mingw64` or `mingw32`.

At the command prompt type the following, being sure to add the correct label
to `MINGW=`:

    $ MINGW=mingw64 cmake .
    $ make

To perform an out-of-source build, type the following:

    $ mkdir build
    $ cd build
    $ MINGW=mingw64 cmake ..
    $ make

As with the macOS/Linux builds, the files will be installed into an `umoria` directory.


## Historical Documents

Most of the original documents included in the Umoria 5.6 sources have been
placed in the [historical](historical) directory. You will even find the old
CHANGELOG which tracks all the code changes made between versions 4.81 and
5.5.2 (1987-2008).

If you'd like to learn more on the development history of Umoria, these can
make for interesting reading.

There is also the original Moria Manual and FAQ. Although these are a little
outdated now, they are certainly worth reading as they contain a lot of
interesting and useful information.


## Code of Conduct and Contributions

See here for details on our [Code of Conduct](CODE_OF_CONDUCT.md).

For details on how to contribute to the Umoria project, please read our
[contributing](CONTRIBUTING.md) guide.


## License Information

Umoria is released under the [GNU General Public License v2.0](LICENSE).

In 2007 Ben Asselstine and Ben Shadwick started the
[_free-moria_](http://free-moria.sourceforge.net/) project to re-license
UMoria 5.5.2 under GPL v2 by obtaining permission from all the contributing
authors. A year later, they finally succeeded in their goal and in late 2008
the Moria maintainer, David Grabiner, released Umoria 5.6 under a GPL v2 license.
