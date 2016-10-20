# Umoria Restoration Project

_The Dungeons of Moria_ is a single player dungeon simulation originally written
by Robert Alan Koeneke and first released in 1985. The original version was
written in VMS Pascal, before being ported to the C language, and released as
_Umoria_ in 1988. Moria has inspired many other games including _Angband_ and
perhaps the most famous roguelike of all, _Diablo_!

Supported Platforms:

  - macOS
  - Ubuntu / Debian Linux

_Note: current platform support is still in Beta...things may go wrong!_


## What's the Moria Restoration Project?

Umoria 5.6 has support for many different platforms which include; Linux,
MS DOS, "Classic" Mac OS (pre OS X), Amiga, and Atari ST. Back in the late
1980's and early 1990's, all these were still useful systems to support.

Now, 30 years on, the only viable option from those platforms is Linux.
This means there is no support for Windows or macOS. However, most people
these days are running on Windows and macOS computers, and it is now far too
difficult for players on these modern systems to play Moria!

What to do about it?

This project is an attempt to _clean up_ this decades old code base.

With all those old systems no longer useful to anyone but the most dedicated
nerd, there is no point in clogging up the code base with support for them,
so the first task is to strip back the support to just Debian Linux. We can
then start work on modernising the code to be C99 or even C11 compliant, and
begin refactoring to be as platform independent as possible. From there it
should be much easier to add support for macOS, and then finally Windows.

I want to let everyone play Moria again!


## Umoria Development Road map

  * ~~Format source code using `clang-tidy` and `clang-format`.~~
  * ~~Remove support for ancient computers/OS: DOS, Amiga, Atari ST, etc.~~
  * ~~Compile against the `C11` standard.~~
  * ~~Use standard `bool` type instead of `int`s.~~
  * ~~Use only _standard types_ (`int32_t` instead of `long`) where possible.~~
  * ~~Clean up function prototypes/signatures.~~
  * General code clean up, fixing _all_ warnings.
  * Update the code to to be as platform independent as possible.
  * Add Windows and macOS support.


## Copyright / License Information

Umoria is released under the [General Public License version 2](LICENCE) (GPL v2).

In 2007 Ben Asselstine and Ben Shadwick started the
[_free-moria_](http://free-moria.sourceforge.net/) project to re-license
UMoria 5.5.2 under the `GPL v2` by obtaining permission from all the
contributing authors. Some eight years later, they finally succeeded in their
goal and in early 2015 David Grabiner, the official Moria maintainer, released
Umoria 5.6 under GPL v2.


## Historical Documents

If you're interested in reading the original documentation that came with
Umoria 5.6, then take a look in the [historical](historical/) directory.

I've re-formatted the various documents from the original release, such as
the game manual, FAQ, and even a CHANGELOG of all the Umoria changes made
between version 4.81 and 5.5.2 (1987-2008).

