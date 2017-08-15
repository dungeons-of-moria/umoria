# A Brief History of Moria


## The Early Days of Moria by Robert Alan Koeneke

_posted to rec.games.roguelike.*, 21. February 1996_

I had some email show up asking about the origin of Moria, and its
relation to Rogue. So I thought I would just post some text on the early
days of Moria.

First of all, yes, I really am the Robert Koeneke who wrote the first
Moria. I had a lot of mail accusing me of pulling their leg and such. I
just recently connected to Internet (yes, I work for a company in the
dark ages where Internet is concerned) and was real surprised to find
Moria in the news groups... Angband was an even bigger surprise, since I
have never seen it. I probably spoke to its originator though... I have
given permission to lots of people through the years to enhance, modify,
or whatever as long as they freely distributed the results. I have
always been a proponent of sharing games, not selling them.

Anyway...

Around 1980 or 81 I was enrolled in engineering courses at the
University of Oklahoma. The engineering lab ran on a PDP 1170 under an
early version of UNIX. I was always good at computers, so it was natural
for me to get to know the system administrators. They invited me one
night to stay and play some games, an early startrek game, The Colossal
Cave Adventure (later just ’Adventure’), and late one night, a new
dungeon game called ’Rogue’.

So yes, I was exposed to Rogue before Moria was even a gleam in my eye.
In fact, Rogue was directly responsible for millions of hours of play
time wasted on Moria and its descendants...

Soon after playing Rogue (and man, was I HOOKED), I got a job in a
different department as a student assistant in computers. I worked on
one of the early VAX 11/780’s running VMS, and no games were available
for it at that time. The engineering lab got a real geek of an
administrator who thought the only purpose of a computer was WORK!
Imagine... Soooo, no more games, and no more rogue!

This was intolerable! So I decided to write my own rogue game, Moria
Beta 1.0. I had three languages available on my VMS system. Fortran IV,
PASCAL V1.?, and BASIC. Since most of the game was string manipulation,
I wrote the first attempt at Moria in VMS BASIC, and it looked a LOT
like Rogue, at least what I could remember of it. Then I began getting
ideas of how to improve it, how it should work differently, and I pretty
much didn’t touch it for about a year.

Around 1983, two things happened that caused Moria to be born in its
recognizable form. I was engaged to be married, and the only cure for
THAT is to work so hard you can’t think about it; and I was enrolled for
fall to take an operating systems class in PASCAL.

So, I investigated the new version of VMS PASCAL and found out it had a
new feature. Variable length strings! Wow...

That summer I finished Moria 1.0 in VMS PASCAL. I learned more about
data structures, optimization, and just plain programming that summer
then in all of my years in school. I soon drew a crowd of devoted Moria
players... All at OU.

I asked Jimmey Todd, a good friend of mine, to write a better character
generator for the game, and so the skills and history were born. Jimmey
helped out on many of the functions in the game as well. This would have
been about Moria 2.0

In the following two years, I listened a lot to my players and kept
making enhancements to the game to fix problems, to challenge them, and
to keep them going. If anyone managed to win, I immediately found out
how, and ’enhanced’ the game to make it harder. I once vowed it was
’unbeatable’, and a week later a friend of mine beat it! His character,
’Iggy’, was placed into the game as ’The Evil Iggy’, and immortalized...
And of course, I went in and plugged up the trick he used to win...

Around 1985 I started sending out source to other universities. Just
before a OU / Texas football clash, I was asked to send a copy to the
University of Texas... I couldn’t resist... I modified it so that the
beggar on the town level was ’An OU football fan’ and they moved at
maximum rate. They also multiplied at maximum rate... So the first step
you took and woke one up, it crossed the floor increasing to hundreds of
them and pounded you into oblivion... I soon received a call and
provided instructions on how to ’de-enhance’ the game!

Around 1986 - 87 I released Moria 4.7, my last official release. I was
working on a Moria 5.0 when I left OU to go to work for American
Airlines (and yes, I still work there). Moria 5.0 was a complete
rewrite, and contained many neat enhancements, features, you name it. It
had water, streams, lakes, pools, with water monsters. It had
’mysterious orbs’ which could be carried like torches for light but also
gave off magical aura’s (like protection from fire, or aggravate
monster...). It had new weapons and treasures... I left it with the
student assistants at OU to be finished, but I guess it soon died on the
vine. As far as I know, that source was lost...

I gave permission to anyone who asked to work on the game. Several
people asked if they could convert it to ’C’, and I said fine as long as
a complete credit history was maintained, and that it could NEVER be
sold, only given. So I guess one or more of them succeeded in their
efforts to rewrite it in ’C’.

I have since received thousands of letters from all over the world from
players telling about their exploits, and from administrators cursing
the day I was born... I received mail from behind the iron curtain
(while it was still standing) talking about the game on VAX’s (which
supposedly couldn’t be there due to export laws). I used to have a map
with pins for every letter I received, but I gave up on that!

I am very happy to learn my creation keeps on going... I plan to
download it and Angband and play them... Maybe something has been added
that will surprise me! That would be nice... I never got to play Moria
and be surprised...

Robert Alan Koeneke



## The Early Days of Umoria by James E Wilson

_posted to rec.games.roguelike.moria. January 2006_

I started work on Umoria in February 1987. I somehow acquired play
testers in April. I don’t recall exactly how, but I was at a university,
so maybe they saw me playing it on a public terminal and asked for a
copy. The game slowly spread around the Berkeley area. By November, the
game was in good enough shape that I could post it to
comp.sources.games. I think I was still calling it UNIX Moria then, to
distinguish it from the original, but the comp.sources.games moderator
shortened it to Umoria, and that name has stuck. After the
comp.sources.games posting, the game was widely available, and things
just grew from there. The Usenet rec.games.moria group was created
sometime around Jan 1, 1988, and was where most of the early discussions
about it occurred.

I originally got involved with Moria because I was tired of playing
other games. I spent a lot of time playing games such as rogue and hack,
and was looking for something different to try. There was a game called
UltraRogue that I enjoyed playing. Unfortunately, it had some very
frustrating bugs, and it was only available as a binary. I spent some
time investigating the possibilities of trying to decompile it and fix
it. Before I got very far, I chanced upon a copy of the VMS Pascal Moria
sources and quickly decided that porting the sources to Unix was a
better project than trying to fix UltraRogue. After I gained some fame
as the Umoria author, I did manage to get a copy of of the UltraRogue
sources, but by then it was too late, and I wasn’t sure if I had a legal
copy of the sources, so I never did anything with them.

The original game was written in VAX/VMS Pascal with some VAX/VMS
Assembler. I did not have access to a VMS machine, and both the Pascal
and Assembler code would not work with Unix because of many uses of VMS
extensions. Since C was a more common language for Unix anyways, I
decided to just convert the whole thing to C. Much of the initial syntax
conversion was done via emacs macros. Then it was just a matter of
tweaking the sources until they would compile. The hardest part was
finding and fixing all of the off-by-one errors, as Pascal array
indexing starts at 1, and C array indexing starts at 0. It took me years
to find and fix all of the off-by-one errors. I still can’t believe how
hard it was to find them all. At least, I hope I found them all.

Mostly what I remember about the early years of working on Umoria was
that it was a good introduction to software engineering principles. I
had to learn how to deal with bugs, bug fixes, release schedules, making
and distributing releases, etc. It was just a game, but I learned many
valuable lessons about real world software development while working on
it.

One of my favorite moments from my Umoria years is when I got mail from
one of the authors of rogue. I recognized the name right away, and had
to ask. He confirmed that he was one of the rogue authors, and that he
was a fan of Umoria. That made my day.

The credits in the PC/Windows game Diablo mention that it was inspired
in part by Umoria.




## A History of Moria: written by Jim Wilson

_first posted to rec.games.moria on November 27, 1990_

This history is somewhat biased towards the Umoria versions because
those are the versions that I am most familiar with; info for the
VMS versions, in particular, may be inaccurate.


### Moria 4.8:

The last version developed by the original authors, R.A. Koeneke,
etc., at the Univ Oklahoma.  This is written in VMS Pascal, and
is only available for VAX/VMS systems.  All other versions of moria
are based on this program.  (Sometimes referred to as VMS Moria	4.8.)


### Moria UB 5.0

This version was developed at Univ Buffalo, hence the UB in the name.
This is also written in VMS Pascal, and is only available for VAX/VMS
systems.  Some of the distinguishing features of this version are:
a Black Market where one can purchase highly enchanted magic items at
100 times normal value, monsters on the town level worth negative
experience (Mother and Baby, Small Child), *shadow* Balrogs start
appearing at level 50 with the *real* Balrog at level 1200 (this makes
it a bit difficult to win).  There are also some new items, and many
more new monsters.  (Sometimes referred to as VMS Moria 5.0 or
Moria 5.0.  Should not be confused with Umoria 5.x.)  This is based
on the Moria 4.8 sources.


### Moria UB 5.1

The latest version of UB Moria.  This version is not available outside
Univ Buffalo as yet, and I do not know what any of the new features
are.  Only available for VAX/VMS systems.


### VMS Moria 6.0

This version was under development at Villanova by Rick Greene.
However, it was never completed and was never released.  I believe
that it was abandoned because Rick lost computer and/or network access.
This was based on the Moria UB 5.0 sources.


### Imoria

This version was developed at the Univ. Washington.  It is written
in VMS Pascal, and is only available for VAX/VMX systems.

I know very little about this version, but have been told by people
at U Washington that it is far superior to any other Moria version
that currently exists.  It has many new monsters, new objects,
new races, new classes, new terrain types (like water), etc.
The sources are currently available from UBVMS.


### Imoria 4.9

The latest version of Imoria.


### PC-Moria 4.00+

This version is a port of the Moria 4.8 Pascal sources to the IBM-PC
by John W. DeBoskey.  This is a faithful port of the original game,
unfortunately, this version has quite a few bugs and hence is not
as good as the unrelated PC-Moria 4.87 version.


### Umoria (UNIX Moria) 4.83/4.85/4.87

This version was developed by Jim Wilson at UC Berkeley.
It is written in UNIX/C and is much more portable than the original
sources.  These sources, at one time or another, were ported to VMS,
IBM-PC, Atari ST, Amiga, Macintosh, Apple IIGS, VM/SP, Archimedes,
are probably others too.  This version fixes very many bugs, spelling
errors, and inconsistencies in the original Moria 4.8 sources.
This version has no help facility like the original program.  It has
character rerolling (in the later versions), but few other changes
from the original game.  (This version has many names, e.g.
Umoria 4.87, UNIX Moria 4.87, PC-Moria 4.83, PC-Moria 4.873, Mac Moria
4.87, etc.  Just about anything with a .83/.85/.87 in the name is
a version of this program.)  This is based on the Moria 4.8 sources.


### PC-Moria 4.83/4.873

This version was developed by Don Kneller, based on the Umoria sources.
These sources are identical except that they will compile on machines
with 16 bit integers, had many changes to reduce program size,
and introduced the reduced map View.  (Note: PC-Moria 4.83 is
extremely buggy, and is unplayable.  I strongly recommend that you
upgrade to a newer version if you are still playing it.)


### Amiga Moria v3.0

This version was written by Richard and Bryan Henderson of Kettering,
Ohio.  It is based on the Umoria 4.85 sources.  This version
has bitmapped graphics, unlike the ascii graphics of all other
versions.  It has weapons of Godly Might (which make one practically
invicible) and some other changes that make it far far easier than
all other Moria versions.  It also has several new monsters, such
as Lavender Leprechauns.  Sources for this version were never
released.


### BRUCE Moria

This version was developed by Christopher J. Stuart at Monash
University, Clayton, Victoria Australia.  This version has many
great new features: monster memories, look any direction code,
settable options, better run/find code, can center character on
screen, stat code rewritten to fix bugs, rudimentory help facility
added, plus many other enhancements.  This was considered an
experimental version, and source never publicly released.  This
was based on the Umoria 4.85 sources.


### UMoria 5.x

This version is under developement by Jim Wilson at Cygnus Support.
It has unified source for the UNIX/IBM-PC/Atari/Mac/VMS/Amiga ports
of Umoria 4.87.  It includes many new features borrowed from BRUCE
Moria, many more bug fixes, all floating point code eliminated,
many changes that affect play balance (hopefully for the better),
many type/structure changes to reduce game size and allow fixes for
pervasive bugs.  See the doc/FEATURES.NEW file for a list of most
user visible changes.  (Sometimes called Umoria 5.0, Moria 5.0,
Moria 5.x.  Should not be confused with Moria UB 5.0.)


### Umoria 5.6

The latest version of Umoria 5.x.  David Grabiner has taken over
Umoria 5.x development but has made only minor changes.  Umoria 5.6
is released under the GNU General Public License.


### Moria Genealogy

Vertical position indicates roughly when the versions were made available,
although the scale is not very accurate.


    1983                            Moria 1.0
                                       |
    1985                            Moria 4.8
                                      /  \
    1987            UMoria 4.83 ------    ----------------------\
                       /  \                       |              \
                      /    ------- PC-Moria 4.83  |              |
                UMoria 4.85            |          |              |
                  /  | \               |          |         Moria UB 5.0
    1988   /------   |  ------         |          |              |
          /          |    UMoria 4.87  |          |              |
         |       BRUCE Moria  |   \    |          |              |
     Amiga Moria     |        |   PC-Moria 4.873  |              |---\
     (surviving)     |     	  |      /            |              |    \
    1989             |   /----------/             |              |  VMS Moria 6.0
                     |  /                         |              |   (defunct)
    1990         Umoria 5.0	       	              |              |
                     |                       Imoria 4.9     Moria UB 5.1
                     |                            |              |
    1994         Umoria 5.5.1             (alive and well)  (alive and well)
                     |
    2008         Umoria 5.6
                     |
               (alive and well)
