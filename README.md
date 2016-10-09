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

Over the years Moria has been ported to many platforms with code contributions
from 26 different people. Although these people were writing a great game, they
were also doing so _just for fun_, and this has unfortunately made the source
code difficult to navigate and even more difficult to understand.

Umoria 5.6 has support for many different platforms which include; Linux, MS DOS,
"Classic" Mac OS (pre OS X), Amiga, and Atari ST. Back in the late 1980's and
early 1990's, all these were still useful systems to support.

Now, 30 years on, the only viable option from those platforms is Linux.
This means there is no support for Windows or macOS. However, most people
these days are running on Windows and macOS computers!

What to do about it?

With all those old systems no longer useful to anyone but the most dedicated
nerd, there is no point in clogging up the code base with support for them.
That means we start work on removing all related code from the game sources.
This will clean up the source code considerably, making it much easier to
then implement proper Windows and macOS support.

This project is an attempt _clean up_ this decades old code base;
standardise the code against modern C, and refactor to be as platform
independent as possible.

I want to let everyone play Moria again!


## Umoria Development Road map

  * ~~Format source code using `clang-tidy` and `clang-format`.~~
  * ~~Remove support for ancient computers/OS: DOS, Amiga, Atari ST, etc.~~
  * ~~Compile against the `C11` standard.~~
  * Update the code to to be as platform independent as possible.
    E.g. use _standard types_, such as changing `long` types to `int32`.
  * Add Windows and macOS support.


## Historical Documents and Information

If you're interested in reading the original documentation that came with
Umoria 5.6, then take a look in the [historical](historical/) directory.

I've re-formatted the various documents from the original release, such as
the game manual, FAQ, and even a CHANGELOG of all the Umoria changes made
between version 4.81 and 5.5.2 (1987-2008).

