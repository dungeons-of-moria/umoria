# Umoria

_The Dungeons of Moria_ is a single player dungeon simulation originally
written by Robert Alan Koeneke, with v1.0 released in 1983. The game was
originally written in VMS Pascal before being ported to the C language and
released as _Umoria_ in 1988. Moria has had many variants over the years, with
[_Angband_](http://rephial.org/) being the most well known, and was also an
inspiration for one the most commercially successful action roguelike games,
_Diablo_!

Supported Platforms:

  - Windows
  - macOS
  - Linux* (Ubuntu, Debian, Fedora)

_* other Linux distros may work, but have not yet been tested._


## Umoria Restoration Release: v5.7

The main focus of the `v5.7` release is to provide support for the three
main systems: Windows, macOS, and Linux. Support for all other outdated
computer systems such as, MS DOS, "Classic" Mac OS (pre OS X), Amiga, and
Atari ST has been removed.

_Note: there are no gameplay changes in this release._

A great deal of _code restoration_ is also currently being undertaken in the
hope to aid future development of the game. Examples of tasks completed so far
includes, reformatting the source code with the help of `clang-tidy` and
`clang-format`, modernizing the code to use standard C types, and fixing all
warnings while compiling against recent versions of GCC.

Full details of all changes can be found in the [CHANGELOG](CHANGELOG.md), and
by browsing the commit history.

Due to its lack of Windows and Mac support, Moria was unplayable for many
people. Hopefully these changes will give more people a chance to play this
classic roguelike game.


## Notes on Compiling Umoria

At present Umoria has been tested against GCC `6.x`, `7.x`, and `8.1`, with
`ncurses 6.x`, although earlier versions should also work fine. You will
require these along with `CMake` and the C++ build tools for your system.


### macOS and Linux

You can do an in-source build by going to the umoria folder in a command prompt
and then type:

```
  $ cmake .
  $ make
```

If you would rather do an out-of-source build you can type:

```
  $ mkdir build && cd build
  $ cmake ..
  $ make
```

A `umoria` directory will be created in the current directory containing the
game binary and data files, which you can then move to your `home` directory,
or another location.


### Windows

MinGW is used to provide GCC and Binutils for compiling on the Windows platform.
The easiest solution to get set up is to use the [MSYS2 Installer](http://msys2.github.io/).
Once installed you will use `pacman` to install `GCC`, `ncurses`, and the
`make`/`cmake` build tools.

At present you will need to provide an environment variable for the MinGW system
you are compiling on. This will be either `mingw64` or `mingw32`.

At the command prompt type the following, being sure to add the correct label
to `MINGW=`:

```
  $ MINGW=mingw64 cmake .
  $ make
```

Or, if you want to do an out of source build, type

```
  $ mkdir build
  $ cd build
  $ MINGW=mingw64 cmake ..
  $ make
```

As with the macOS/Linux builds, the files will be installed into a `umoria`
directory, which you can then move to your games directory.


## Historical Documents

Most of the original documents included in the Umoria 5.6 sources have been
moved to the [historical](historical) directory. You will even find the old
CHANGELOG which tracks all the code changes made between versions 4.81 and
5.5.2 (1987-2008).

If you'd like to learn more on the development history of Umoria, these can
make for interesting reading.


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
