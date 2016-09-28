# Umoria - a roguelike computer game

_The Dungeons of Moria_ is a single player dungeon simulation originally
written by Robert Alan Koeneke and first released in 1985.

The original version was written in VMS Pascal, before being ported to the
C language, and released as Umoria in 1988.

Over the years Moria has been ported to many platforms with code contributions
from 26 different people. These people were writing a great game, but they were
doing so _just for fun_, and this has unfortunely made the source code difficult
to nagivate and even more difficult to understand. It's also currently not
possible to compile the game on Windows or macOS.

This project is an attempt _clean up_ this decades old codebase; standardising
the coding style, and refactoring to be as platform independent as possible.


## Umoria Restoration / Development

Umoria has support for many different platforms which include; Linux, MS DOS,
"Classic" Mac OS (pre OS X), Amiga, and Atari ST. Back in the late 1980's and
early 1990's, all these were still useful systems to support.

Now, in the 2010's, only Linux is still a viable option - that means there is
no suport for Windows or macOS. However, most people these days are running on
Windows or macOS computers!

What to do about it?

With all those old systems no longer useful to anyone but the most dedicated
nerd, there is no point in clogging up the codebase with support for them.
That means we start work on removing all related code from the game sources.
This will clean up the source code considerably, making it much easier to
then implement proper Windows and macOS support.

Outline of the restoration goals:

  * Format source code using `clang-tidy` and `clang-format`.
  * Remove support for ancient computers/OS: DOS, Amiga, Atari ST, etc.
  * Compile against the C11 standard.
  * Update the code to to be as platform independent as possible.
    E.g. use _standard types_, such as changing `long` types to `int32`.
  * Add Windows and macOS support.


## Historial Documents and Information

All the old docs and changelog have been moved to the [historical](historical/)
directory. This folder contains various documents from the original release,
such as the game manual, FAQ, and even a CHANGELOG of all the changes made
between version 4.81 and 5.5.2 (1987-2008).

