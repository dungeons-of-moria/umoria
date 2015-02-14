.\"This is a very modified version of the documentation
.\"for the original VMS game.  Changes were made to reflect differences
.\"found in the UNIX port by James E. Wilson.
.\"
.\" "tbl moria1.ms moria2.ms | nroff -ms"
.\"
.\" You might wanna change these two values:
.RP
.nr PO 1i
.nr LL 6.5i
.ds LH "The Dungeons of Moria
.ds CH "
.ds RH "Page %
.ds LF "
.ds CF "
.ds RF "
.TL
The Dungeons of Moria
.AU
Robert Alan Koeneke
James E. Wilson
David J. Grabiner
.AB no
Umoria is free software; you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
.LP
Umoria is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
.LP
You should have received a copy of the GNU General Public License 
along with Umoria.  If not, see <http://www.gnu.org/licenses/>. 
.AE

.NH 1
Introduction
.LP
The game of \fImoria\fP is a single player dungeon simulation.
A player may choose from a number of races and classes when
creating a character, and then `run' that character over a
period of days, weeks, even months, attempting to win the game by
defeating the Balrog which lurks in the deeper levels.
.LP
The player will begin his adventure on the town level where
he may acquire supplies, weapons, armor, and magical devices by
bartering with various shop owners.
After preparing for his adventure, the player can descend into the
dungeons of \fImoria\fP where fantastic adventures await his coming!
.LP
Before beginning your first adventure, you should read this
document carefully.
The game of \fImoria\fP is a complicated game, and
will require a dedicated player to win.

.NH 1
The Character
.LP
All characters have six main attributes which modify their basic abilities.
These six attributes, called \fIstats\fP, are
\fIstrength\fP, \fIintelligence\fP, \fIwisdom\fP, \fIdexterity\fP,
\fIconstitution\fP, and \fIcharisma\fP.
Stats may vary from a minimum of 3 to a maximum of 18.  At the highest level,
stats are further qualified by a number from zero to one hundred, so that
the highest value is actually 18/100.  A value of 18/100 can be thought
of as equivalent to 19, and 18/00 (not actually used) is equivalent to 18.
Because adventurers of interest tend to be better than
average characters, \fImoria\fP stats will average about 13, and are
further adjusted by race and class.
Some races are just naturally better at being certain classes, as will be
shown later.
.LP
In addition to the more visible stats, each character has certain abilities
which are mainly determined by his race, class, and level, but are
also modified by his stats.
The abilities are \fIfighting\fP, \fIthrowing/bows\fP, \fIsaving throw\fP,
\fIstealth\fP, \fIdisarming\fP, \fImagical devices\fP, \fIperception\fP,
\fIsearching\fP, and \fIinfravision\fP.
.LP
Characters will be assigned an early history, with money and a social class
based on that history.
Starting money is assigned based on history, charisma, and somewhat upon the
average of a character's stats.
A character with below average stats will receive extra money to help
him survive the first adventure.
.LP
Each character will also have physical attributes such as
race, height, weight, sex, and a physical description.
None of
these, except weight, play any part in the game other than to
give the player a \*Qfeeling\*U for his character.
Weight is used for computing carrying capacity and also for bashing.
.LP
Finally, each character is assigned \fIhit points\fP based on their race,
class, and constitution.
Spell casters will also receive \fImana\fP which is expended when casting
spells.
Mana is based on Wisdom for Priests and Intelligence for Mages.

.NH 2
Character Stats
.IP "Strength"
Strength is important in fighting with weapons and hand to hand combat.
A high strength can improve your chances of hitting, and the amount of
damage done with each hit.
Characters with low strengths may receive penalties.
Strength is also useful in tunneling, body and shield bashing, and in
carrying heavy items.
.IP "Intelligence"
Intelligence is the prime stat of a mage, or magician.
A high intelligence increases a mage's chances of learning spells, and
it also increases the amount of mana a mage has.
No spell may be learned by mages with intelligences under 8.
Intelligence also modifies a character's chance of disarming traps,
picking locks, and using magic devices.
.IP "Wisdom  "
Wisdom is the prime stat of a priest.
A high wisdom increases the chance of receiving new spells from a priest's
deity, and it also increases the amount of mana a priest has.
No spell may be learned by priests with wisdom under 8.
Wisdom also modifies a character's chance of resisting magical spells
cast upon his person.
.IP "Dexterity"
Dexterity is a combination of agility and quickness.
A high dexterity may allow a character to get multiple blows with lighter
weapons, thus greatly increasing his kill power, and may increase his
chances of hitting with any weapon and dodging blows from enemies.
Dexterity is also useful in picking locks, disarming traps, and
protecting yourself from pick pockets.
.IP "Constitution"
Constitution is a character's ability to resist damage to his body, and to
recover from damage received.
Therefore a character with a high constitution will receive more hit points,
and be more resistant to poisons.
.IP "Charisma"
Charisma represents a character's personality, as well as physical looks.
A character with a high charisma will receive better prices from store
owners, whereas a character with a very low charisma will be robbed blind.
A high charisma will also mean more starting money for the character.

.NH 2
Character Sex
.LP
You may choose to be either a male or a female character.
Only height and weight are affected by a character's sex.
Female characters tend to be somewhat smaller and lighter than their
male counterparts.
No adjustments to stats or abilities are made because of the sex of
a character.  Female characters start out with slightly more money than
male characters to help offset the weight penalty.

.NH 2
Character Abilities
.LP
Characters possess nine different abilities which can help
them to survive.  The starting abilities of a character are based
upon race and class.  Abilities may be adjusted by high or low
stats, and may increase with the level of the character.
.IP "Fighting"
Fighting is the ability to hit and do damage with weapons or
fists.  Normally a character gets a single blow from any weapon,
but if his dexterity and strength are high enough, he may receive
more blows per round with lighter weapons.  Strength and
dexterity both modify the ability to hit an opponent.
This skill increases with the level of the character.
.IP "Throwing/Bows"
Using ranged missile weapons and throwing objects is
included in this skill.  Different stats apply to different
weapons, but this ability may modify the distance an object is thrown/fired,
the amount of damage done, and the ability to hit a creature.
This skill increases with the level of the character.
.IP "Saving Throw"
A Saving Throw is the ability of a character to resist the
effects of a spell cast on him by another person/creature.
This does not include spells cast on the player by his own
stupidity, such as quaffing a nasty potion.  This ability
increases with the level of the character, but then most high
level creatures are better at casting spells, so it tends to even
out.  A high wisdom also increases this ability.
.IP "Stealth  "
The ability to move silently about is very useful.
Characters with good stealth can usually surprise their
opponents, gaining the first blow.  Also, creatures may fail to
notice a stealthy character entirely, allowing a player to avoid
certain fights.  This skill is based entirely upon race
and class, and will never improve unless magically enhanced.
.IP "Disarming"
Disarming is the ability to remove traps (safely), and
includes picking locks on traps and doors.
A successful disarming will gain the character some experience.
A trap must be found before it can be disarmed.
Dexterity and intelligence both modify the ability to disarm, and
this ability increases with the level of the character.
.IP "Using Magical Devices"
Using a magical device such as a wand or staff requires
experience and knowledge.  Spell users such as mages and priests
are therefore much better at using a magical device than say a
warrior.  This skill is modified by intelligence, and increases
with the level of the character.
.IP "Perception"
Perception is the ability to notice something without
actively seeking it out.  This skill is based entirely upon race
and class, and will never improve unless magically enhanced.
.IP "Searching"
To search is to actively look for secret doors, floor traps,
and traps on chests.  Rogues are the best at searching, but
mages, rangers, and priests are also good at it.
This skill is based entirely upon race
and class, and will never improve unless magically enhanced.
.IP "Infravision"
Infravision is the ability to see heat sources.  Since most
of the dungeon is cool or cold, infravision will not allow the
player to see walls and objects.  Infravision will allow a
character to see any warm-blooded creatures up to a certain
distance.  This ability works equally well with or without a
light source.  The majority of \fImoria\fP's creatures are
cold-blooded, and will not be detected unless lit up by a light
source.  All non-human races have innate infravision ability.
Humans can gain infravision only if it is magically enhanced.

.NH 2
Choosing A Race
.LP
There are eight different races that you can choose from in
\fImoria\fP.  Some races are restricted as to what profession they may
be, and each race has its own adjustments to a character's stats
and abilities.
.IP "Human  "
The human is the base character, all other races are
compared to him.  Humans can choose any class, and are average
at everything.  Humans tend to go up levels faster than any other
race, because of their shorter life spans.  No racial adjustments
occur to characters choosing human.
.IP "Half-Elf"
Half-elves tend to be smarter and faster than a human, but
not as strong.  Half-elves are slightly better at searching,
disarming, perception, stealth, and magic, but they are not as
good at hand weapons.  Half-elves may choose any class.
.IP "Elf       "
Elves are better magicians then humans, but not as good at
fighting.  They tend to be smarter and faster than either humans
or half-elves, and also have better wisdom.  Elves are better at
searching, disarming, perception, stealth, and magic, but they
are not as good at hand weapons.  Elves may choose any class
except Paladin.
.IP "Halfling"
Halflings, or Hobbits, are very good at bows, throwing, and
have good saving throws.  They also are very good at searching,
disarming, perception, and stealth; so they make excellent
thieves (but prefer to be called burglars...).  They will be much
weaker than humans, and no good at bashing.  Halflings have fair
infravision, so they can detect warm creatures at a distance.
Halflings can choose between being a warrior, mage, or rogue.
.IP "Gnome   "
Gnomes are smaller than dwarfs, but larger than halflings.
They, like the halflings, live in the earth in burrow-like homes.
Gnomes are practical jokers, so if they can kill something in a
humorous way, so much the better.  Gnomes make excellent mages,
and have very good saving throws.  They are good at searching,
disarming, perception, and stealth.  They have lower strength
than humans so they are not very good at
fighting with hand weapons.  Gnomes have fair infravision, so
they can detect warm creatures at a distance.  A gnome may choose
between being a warrior, mage, priest, or rogue.
.IP "Dwarf  "
Dwarves are the headstrong miners and fighters of legend.
Since dungeons are the natural home of a dwarf, they are
excellent choices for a warrior or priest.  Dwarves tend to be
stronger and have higher constitutions, but are slower and less
intelligent than humans.  Because they are so headstrong and are
somewhat wise, they resist spells which are cast on them.
Dwarves also have good infravision because they live
underground.  They do have one big drawback though.  Dwarves are
loudmouthed and proud, singing in loud voices, arguing with
themselves for no good reason, screaming out challenges at
imagined foes.  In other words, dwarves have a miserable stealth.
.IP "Half-Orc"
Half-Orcs make excellent warriors, and decent priests, but
are terrible at magic.  They are as bad as dwarves at stealth,
and horrible at searching, disarming, and perception.  Half-Orcs
are, let's face it, ugly.  They tend to pay more for goods in
town.  Half-Orcs do make good priests and rogues, for the simple
reason that Half-Orcs tend to have great constitutions and lots
of hit points.
.IP "Half-Troll"
Half-Trolls are incredibly strong, and have the highest hit
points of any character race.  They are also very stupid and
slow.  They will make great warriors and iffy priests.  They are
bad at searching, disarming, perception, and stealth.  They are
so ugly that a Half-Orc grimaces in their presence.  They also
happen to be fun to run...

.NH 3
Race Versus Skills and Stats
.LP
Stat, hit dice, and experience points per level modifications due
to race are listed in the following table.
.KS
.TS
tab(^) center;
l 2 n 2 n 2 n 2 n 2 n 2 n 2 n 2 n.
^Str^Int^Wis^Dex^Con^Chr^Hit Dice^Rqd Exp/level
.sp
Human^0^0^0^0^0^0^10^+0%
Half-Elf^\-1^+1^0^+1^\-1^+1^9^+10%
Elf^\-1^+2^+1^+1^\-2^+1^8^+20%
Halfling^\-2^+2^+1^+3^+1^+1^6^+10%
Gnome^\-1^+2^0^+2^+1^\-2^7^+25%
Dwarf^+2^\-3^+1^\-2^+2^\-3^9^+20%
Half-Orc^+2^\-1^0^0^+1^\-4^10^+10%
Half-Troll^+4^\-4^\-2^\-4^+3^\-6^12^+20%
.TE
.KE
Racial abilities as compared to each other, with 
1 the lowest, or worst, and 10 the highest, or best,
are listed in the following table.
.KS
.TS
tab(^) center;
l 1 n 1 n 1 n 1 n 1 n 1 n 1 n 1 l.
^Disarm^Search^Stealth^Percep^Fight^Bows^Save^Infra
.sp
Human^5^5^5^5^5^5^5^None
Half-Elf^6^7^7^6^4^6^6^20 feet
Elf^8^9^7^7^3^9^7^30 feet
Halfling^10^10^10^10^1^10^10^40 feet
Gnome^9^7^9^9^2^8^9^30 feet
Dwarf^6^8^3^5^9^5^8^50 feet
Half-Orc^3^5^3^2^8^3^3^30 feet
Half-Troll^1^1^1^1^10^1^1^30 feet
.TE
.KE

.NH 2
Choosing A Class
.LP
Once a race has been chosen, you will need to pick a class.
Some classes will not be available to certain races, for instance,
a Half-Troll cannot become a Paladin.  For the first few
adventures it is suggested that you run a warrior or rogue.
Spell casting generally requires a more experienced player that
is familiar with survival techniques.
.IP "Warrior"
A Warrior is a hack-and-slash character, who solves most of
his problems by cutting them to pieces, but will occasionally
fall back on the help of a magical device.  His prime stats are
Strength and Constitution, and a good Dexterity can really help
at times.  A Warrior will be good at Fighting and Throwing/Bows,
but bad at most other skills.
.IP "Mage    "
A Mage must live by his wits.  He cannot hope to simply hack
his way through the dungeon, and so must therefore use his magic
to defeat, deceive, confuse, and escape.  A mage is not
really complete without an assortment of magical devices to use in
addition to his spells.  He can master the higher level magical
devices far easier than anyone else, and has the best saving
throw to resist effects of spells cast at him.  Intelligence and
Dexterity are his primary stats.  There is no rule that says a
mage cannot become a good fighter, but spells are his true realm.
.IP "Priest    "
A Priest is a character of holy devotion.  He explores the
dungeon only to destroy the evil that lurks within, and if
treasure just happens to fall into his pack, well, so much
more to the glory of his temple!  A priest receives his spells
from a deity, and therefore does not choose which spells he will
learn.  He is familiar with magical devices, preferring to call
them instruments of god, but Is not as good as a mage in their
use.  Priests have good saving throws, and make decent fighters,
preferring blunt weapons over edged ones.  Wisdom and Charisma
are the priest's primary stats.
.IP "Rogue   "
A Rogue is a character that prefers to live by his cunning,
but is capable of fighting his way out of a tight spot.  He is
the master of traps and locks, no device being impossible for him
to overcome.  A rogue has a high stealth allowing him to sneak
around many creatures without having to fight, or sneak up and
get the first blow.  A rogue's perception is higher than any
other class, and many times he will notice a trap or secret door
before having to search.  A rogue is better than a warrior or
paladin with magical devices, but still can not rely on their
performance.  Rogues can also learn a few spells, but not the powerful
offensive spells mages can use.  A rogue's primary stats are
Intelligence and Dexterity.
.IP "Ranger  "
A Ranger is a warrior/mage.  He is a good fighter, and the
best of the classes with a missile weapon such as a bow.  The
ranger learns spells much more slowly than a mage, but is capable
of learning all but the most powerful spell.  Because a ranger is
really a dual class character, more experience is required for him
to advance.  A ranger has a good stealth, good perception, good
searching, a good saving throw, and is good with magical devices.
His primary stats are Intelligence and Dexterity.
.IP "Paladin  "
A Paladin is a warrior/priest.  He is a very good fighter,
second only to the warrior class, but not very good at missile
weapons.  He receives prayers at a slower pace then the priest,
but can eventually learn all the prayers.  Because a
paladin is really a dual class character, it requires more
experience to advance him.  A paladin lacks much in the way of
abilities.  He is poor at stealth, perception, searching, and
magical devices.  He has a decent saving throw due to his divine
alliance.  His primary stats are Strength and Charisma.

.NH 3
Race Versus Class
.LP
.KS
.TS
tab(^) center;
l 2 ce 2 ce 2 ce 2 ce 2 ce 2 ce.
^Warrior^Mage^Priest^Rogue^Ranger^Paladin
.sp
Human^Yes^Yes^Yes^Yes^Yes^Yes
Half-Elf^Yes^Yes^Yes^Yes^Yes^Yes
Elf^Yes^Yes^Yes^Yes^Yes^No
Halfling^Yes^Yes^No^Yes^No^No
Gnome^Yes^Yes^Yes^Yes^No^No
Dwarf^Yes^No^Yes^No^No^No
Half-Orc^Yes^No^Yes^Yes^No^No
Half-Troll^Yes^No^Yes^No^No^No
.TE
.KE

.NH 3
Class Versus Skills
.LP
Class abilities as compared to each other, with
1 as the lowest, or worst, and 10 as the highest, or best
are shown in the following table.
.KS
.TS
tab(^) center;
l 1 c 1 c 1 c 1 c 1 c 1 c 1 c 1 c 1 c
l c c c c c c c c c
l n n n n n n n n r.
^^^Save^Stea-^^Magic^^^Extra
^Fight^Bows^Throw^lth^Disarm^Device^Percep^Search^Exp/lev
.sp
Warrior^10^6^3^2^4^3^2^2^+0%
Mage^2^1^10^5^6^10^8^5^+30%
Priest^4^3^6^5^3^8^4^4^+20%
Rogue^8^9^7^10^10^6^10^10^+0%
Ranger^6^10^8^7^6^7^6^6^+40%
Paladin^9^5^4^2^2^4^2^2^+35%
.TE
.KE

.NH 1
Adventuring
.LP
After you have created your character, you will begin your
\fImoria\fP adventure.  Symbols appearing on your screen will represent
the dungeon's walls, floor, objects, features, and
creatures lurking about.  In order to direct your character
through his adventure, you will enter single character commands.
.LP
\fIMoria\fP symbols and commands each have a help section devoted
to them.  You should review these sections before attempting an
adventure.  Finally, a description of the town level and some
general help on adventuring are included.

.NH 1
Symbols On Your Map
.LP
Symbols on your map can be broken down into three
categories: Features of the dungeon such as walls, floor, doors,
and traps; objects which can be picked up such as treasure,
weapons, magical devices, etc; and creatures which may or may not
move about the dungeon, but are mostly harmful to your
character's well-being.
.LP
Some symbols can be in more than one category.
Also note that treasure may be embedded in a wall, and the wall
must be removed before the treasure can be picked up.
.LP
It will not be necessary to remember all of the symbols and
their meanings.  A simple command, the `/', will identify any
character appearing on your map.  See the section on commands for
further help.
.LP
.nf
.KS
.TS
tab(*) center;
c s s s
l l l l.
Features
.sp
\&.*A floor space, or hidden trap*1*Entrance to General Store
#*A wall*2*Entrance to Armory
'*An open door*3*Entrance to Weapon Smith
+*A closed door*4*Entrance to Temple
^*A trap*5*Entrance to Alchemy Shop
<*A staircase up*6*Entrance to Magic Shop
>*A staircase down*:*Obstructing rubble
;*A loose floor stone*\ *An open pit (Blank)
%*A mineral vein*@*The character
.TE
.KE
.sp
.KS
.TS
tab(^) center;
c s s s
l l l l.
Objects
.sp
!^A flask or potion^?^A scroll
"^An amulet^[^Hard armor
$^Money (Can be embedded)^\e^A hafted weapon
&^A chest^]^Misc. armor
(^Soft armor^\&_^A staff
)^A shield^{^Missile (arrow, bolt, pebble)
*^Gems (Can be embedded)^|^Sword or dagger
\-^A wand^}^Missile arm (Bow, X-bow, sling)
/^A pole-arm^~^Misc
\&=^A ring^,^Food
s^A skeleton^^
.TE
.KE
.sp
.KS
.TS
tab(^) center;
c s s s
l l l l.
Creatures
.sp
a^Giant Ant^A^Giant Ant Lion
b^Giant Bat^B^The Balrog
c^Giant Centipede^C^Gelatinous Cube
d^Dragon^D^Ancient Dragon
e^Floating Eye^E^Elemental
f^Giant Frog^F^Fly
g^Golem^G^Ghost
h^Harpy^H^Hobgoblin
i^Icky-Thing^I^
j^Jackal^J^Jelly
k^Kobold^K^Killer Beetle
l^Giant Louse^L^Lich
m^Mold^M^Mummy
n^Naga^N^
o^Orc or Ogre^O^Ooze
p^Human(oid)^P^Giant Human(oid)
q^Quasit^Q^Quylthulg
r^Rodent^R^Reptile
s^Skeleton^S^Scorpion
t^Giant Tick^T^Troll
u^^U^Umber Hulk
v^^V^Vampire
w^Worm or Worm Mass^W^Wight or Wraith
x^^X^Xorn
y^Yeek^Y^Yeti
z^Zombie^Z^
$^Creeping Coins^,^Mushroom Patch
.TE
.KE

.NH 1
Commands
.LP
All commands are entered by pressing a single key.
Some commands are capital or control characters, which require
you to hold down the shift or control key while pressing another key.
As a special feature, control keys may be entered in a single stroke,
or in two strokes, with a `^' character first.
.LP
There are two command sets: the original command set which is
the default, and the rogue like command set.  The rogue like command
is generally more convenient, especially if you don't have a keypad.
.LP
The following tables summarize the two command sets.
Certain commands may be preceded by an optional count, and certain
commands must be followed by a direction.  These conditions
are indicated in the tables by `@' for an optional count,
and `~' for a direction.  If a particular command requires additional
key strokes, then they will be prompted for.
.KS
.TS
tab(^) center;
c 1 s 1 s 1 s 1 s 1 s 1 s.
Original command summary.
.T&
l l l l l l s.
^a^Aim and fire a wand^@^B ~^Bash (object/creature)
^b^Browse a book^^C^Change name
^c ~^Close a door^@^D ~^Disarm a trap/chest
^d^Drop an item^^E^Eat some food
^e^Equipment list^^F^Fill lamp with oil
^f^Fire/Throw an item^^G^Gain new magic spells
^i^Inventory list^^L^Locate with map
@^j ~^Jam a door with spike^^M^Map shown reduced size
^l ~^Look given direction^@^R^Rest for a period
^m^Magic spell casting^^S^Search Mode
@^o ~^Open a door/chest^@^T ~^Tunnel in a direction
^p^Pray^^V^View scoreboard
^q^Quaff a potion^^\&=^Set options
^r^Read a scroll^^?^Command quick reference
@^s^Search for trap or door^^{^Inscribe an object
^t^Take off an item^@^\- ~^Move without pickup
^u^Use a staff^^. ~^Run in direction
^v^Version, credits and manual^^/^Identify a character
^w^Wear/Wield an item^^CTRL-K^Quit the game
^x^Exchange weapon^@^CTRL-P^Repeat the last message
^<^Go up an up staircase^^CTRL-X^Save character and quit
^>^Go down a down staircase^@^~^for movement
.TE
.KE
.KS
.TS
tab(^) center;
c 1 s 1 s 1 s 1 s 1 s 1 s.
Rogue like command summary.
.T&
l l l l l l s.
^c ~^Close a door^^C^Character description
^d^Drop an item^@^D ~^Disarm a trap/chest
^e^Equipment list^^E^Eat some food
@^f ~^Force/bash item/monster^^F^Fill lamp with oil
^i^Inventory list^^G^Gain new magic spells
^m^magic spell casting^^M^Map shown reduced size
@^o ~^Open a door/chest^^P^Peruse a book
^p^Pray^^Q^Quit the game
^q^Quaff a potion^@^R^Rest for a period
^r^Read a scroll^@^S ~^Spike a door
@^s^Search for trap or door^^T^Take off an item
^t^Throw an item^^V^View scores
^v^Version, and manual^^W^Where: locate self
^w^Wear/Wield an item^^X^Exchange weapon
^x ~^Examine surroundings^^Z^Zap a staff
^z^Zap a wand^^#^Search Mode
^\&=^Set options^^<^Go up an up staircase
^/^Identify a character^^>^Go down a down stair
@^CTRL-P^Previous message review^^{^Inscribe an object
@^\- ~^Move without pickup^^?^Type this page
@^CTRL ~^Tunnel in a direction^^CTRL-X^Save game and exit
@^SHFT ~^Run in direction^@^~^for movement
.TE
.KE

.NH 2
Special keys.
.LP
Certain commands may be entered at any time input is accepted.
The special character control-R, entered as a single key stroke, will
always refresh the screen.  This may be used at any prompt for input,
and is otherwise ignored.
.LP
If you are playing on a UNIX or similar system, then there are some
additional special characters used by \fImoria\fP.
The special character control-C will interrupt \fImoria\fP, and ask if you
really want to die and quit the game.
If you choose not to die, \fImoria\fP merely continues as before, except that
resting, running, repeated commands, etc will be terminated.
You can suspend the game with control-Z, and return to the original
command shell.  In this case, \fImoria\fP is not terminated, and may be
restarted at any time from the shell.  Alternatively, the special
command `!' is available to run any normal shell command.  When it is
complete, \fImoria\fP will restart.
.LP
For many input requests or queries, the special character ESCAPE
will abort the command.  For the \*Q\-more\-\*U message prompts, any
of SPACE, ESCAPE, RETURN (control-m), or LINEFEED (control-j)
can be used to continue after pausing to read the displayed message.
.LP
It is possible to give control character commands in two key stroke,
by typing a `^' followed by the appropriate letter of the alphabet.
This is useful when running \fImoria\fP in circumstances where control
characters are intercepted by some external process, or by the
operating system.
.NH 2
Direction.
.LP
For the original style command set,
a direction is given by a digit which is in the appropriate
orientation on your keypad.  For the rogue like command set,
a direction is given by one of the letters `hykulnjb'.  Again,
the relative position of the keys on the keyboard gives a clue
as to the direction.
The digit `5' for the original commands, and the period `.'
for rogue like commands, is a null direction indicator.
This is only allowed in a movement command (to stay in one place)
or in a look command (to look in all directions).
.RS
.KS
.TS
tab(^);
c s s
n n n.
Original Directions
.sp
\0\\\\\&^\0|\&\0^\0\&/
7^8^9
.sp
\-\0 4^ ^6\&\0\0\-
.sp
1^2^3
\0/\0\&^\0|\&\0^\0\&\\\\\&
.TE
.KE
.KS
.TS
tab(^);
c s s
n n n.
Rogue-like Directions
.sp
\0\\\\\&^\0|\&\0^\0\&/
y\&^k\&^u\&
.sp
\-\0 h\&^ ^l\&\0\0\-
.sp
b\&^j\&^n\&
\0/\0\&^\0|\&\0^\0\&\\\\\&
.TE
.KE
.RE
.LP
Movement is accomplished by specifying a direction immediately.
Simply press the appropriate key and you character will move
one step in that direction.
You can only move onto
and through floor spots, and only if they contain no creatures or
obstructing objects such as a closed door.
.LP
Other commands that require a direction will prompt for it.
.LP
Moving your character one step at a time can be time
consuming and boring, so a faster method has been supplied.
For the original style command set, by
using the Run command `.', you may move in a direction until
something interesting happens.  For instance, by pressing the
period key `.' followed by the direction 8, your character would
continue to move up the screen, only coming to a stop after at
least one condition is satisfied.
For the rogue like command set, typing a shifted directional letter
will move you in that direction until something interesting happens.
The stopping conditions are described more completely in
the run command description below.

.NH 2
Command counts.
.LP
Some commands can be executed a fixed number of times by preceding
them with a count.  Counted commands will execute until the count
expires, or until you type any character, or until something significant
happens, such as being attacked.  Thus, a counted command doesn't work
to attack another creature.  While the command is being repeated,
the number of times left to be repeated will flash by on the command
line at the bottom of the screen.
.LP
To give a count to a command in the rogue like mode, type the number
in digits, then the command.  A count of zero defaults to a count of
99.
.LP
To give a count to a command in the original mode, type a `#', followed
by the digits.  To count a movement command (which is itself a digit),
type a space after the number, and you will then be prompted for the
command.
.LP
Counted commands are very useful for searching or tunneling, as they
automatically terminate on success, or if you are attacked.
You may also terminate a counted command, or a Run command, by typing
any character.  This character is ignored, but it is safest to use
a SPACE or ESCAPE which are always ignored as commands.

.NH 2
Selection of objects.
.LP
Many commands will also prompt for a particular object to be used.
For example, the command to read a scroll will ask you which of the scrolls
that you are carrying that you wish to read.
In such cases, the selection is made by typing a letter of the alphabet;
if you are selecting from your pack, you may also type a digit to select
the item whose inscription is that digit.
The prompt will indicate the possible letters, and will also allow you
to type the key `*', which causes all of the available options to
be described.
.LP
The particular object may be selected by an upper case or a lower case
letter.  If lower case or a digit is used, the selection takes place
immediately.  If upper case is used, then the particular option is
described, and you are given the option of confirming or retracting that
choice.  Upper case selection is thus safer, but requires an extra key
stroke.

.NH 2
Command descriptions
.LP
In the following command descriptions, the original style key is
given.  If the rogue like key for that command is different, then it
will be shown inside the braces following the command name.
.IP "B <Dir> - Bash. {f - force}"
The bash command includes breaking open doors and chests, or
bashing an opponent.  Your bashing ability increases with
weight and strength.  In addition,
when bashing an opponent, you will either perform a body bash, or,
if wielding a shield, perform a shield bash which is more
effective.
.sp
Bashing a door can throw you off balance, but this
will not generally be a problem.  Doors that have been jammed
closed with spikes can only be opened by bashing.  Locked doors
may also be bashed open.  Bashing a door open will
permanently break it.
.sp
Bashing a creature affects both you and the opponent.
Depending on your dexterity, you may or may
not be thrown off balance allowing free moves to your opponent.
If the bash is successful, your opponent may be thrown
off balance, thus giving you some free
hits or a chance to run.  Huge creatures such as ancient dragons
will be difficult or impossible to bash successfully.
.sp
A player automatically performs a shield bash instead of a
body bash, if he is currently wearing a shield.  A shield bash
adds the damage of a shield to that of the bash, so it is more
effective.  Size and material both affect the damage
that a shield will do.
.sp
You can apply a count to this command, but if you are thrown off
balance, the count will be reset straight away.
.IP "C  - Print character (to screen or file)."
This command allows the player to either display his
character on the terminal screen, or to print an entire character
info listing to a file.  The character's history, equipment,
and inventory list are also included if you chose to print it to a file.
.IP "D <Dir> - Disarm a trap."
You can attempt to disarm floor traps, or trapped chests.
If you fail to disarm a trap, there is a chance that you blunder
and set it off.  You can only disarm a trap on a chest after
finding it with the search command.  This command can have a count.
.IP "E  - Eat some food."
A character must eat occasionally to remain effective.  As a
character grows hungry, a message will appear at the bottom of
the screen saying \*QHungry\*U.  If a character remains hungry long
enough, he will become weak, eventually start fainting, and finally die
of starvation.
.IP "F  - Fill a lamp or lantern with oil."
If your character is currently using a lamp for light, and
if he has a flask of oil in inventory, he may refill the lamp by
using this command.  A lamp is capable of a maximum of 15000 turns
of light, and each flask has 7500 turns of oil contained in it.
.IP "G  - Gain new spells."
To actually learn new spells, you must use this command.
When you are able to learn some spells, the word \*QStudy\*U
will appear on the status line at the bottom of the screen.
Mages, rogues, and rangers must have the magic books containing
new spells to be able to learn them.  Priests and Paladins are
given their prayers by their gods, and hence do not need a holy
book before learning the prayers in it.  They do need the book 
in order to use the prayers.
.IP "L  - Location on map. {W - where}"
The location command allows you to look at all parts of the 
current dungeon level.  The displayed view of the dungeon is
shifted to bring your current position as close to the center
as possible.  You may then shift the displayed map in any of the eight
possible directions.  Each shift moves your view point by one
half screen.  The top line displays a map section number, each map
section having a height and width one half that of the display, and
indicates the direction of the display from your current position.
If you exit this command while you are not on the display, then the
display is centered again.
.IP "M  - Map shown reduced size."
This command will show the entire map, reduced by a factor of nine,
on the screen.  Since nine places map into every character on the screen,
only the major dungeon features will be visible.  This is especially
useful for finding where the stairs are in relation to your current
position.  It is also useful for identifying unexplored areas.
.IP "R  - Rest for a number of turns."
You may rest one turn with the null movement command.  Resting for
longer periods of time is accomplished by using the Rest command,
followed by the number of turns you want to rest your character.
Resting will continue until the specified duration has expired,
or something to wake the character happens, such as a creature
wandering by, or getting hungry, or some disability like
blindness expiring.
It is sometimes a good idea to rest a beat-up
character until he regains some of his hit points, but be sure to
have plenty of food if you rest often.
.sp
If you have accidentally entered in a rest period too large,
or change your mind about the resting period, you may wake your
character up by typing any character.  Space is best, since if the
rest ends just before the character is typed, the space is ignored
as a command.
.sp
It is also possible to rest by typing the count first, and using
either the Rest or the null movement command.
.sp
If you type `*' for the rest count, your character will rest until both
hp and mana reach their maximum values.  As above, you will immediately
stop resting if anything interesting happens.
.IP "S  - Search mode toggle. {#}"
The Searching toggle will take you into and out of
search mode.  When first pressed, the message \*QSearching\*U will
appear at the bottom of the screen.  You are now taking
two turns for each command, one for the command and one turn to
search.  This means that you are taking twice the
time to move about the dungeon, and therefore twice the food.  If
a creature should happen by or attack you, search mode will
automatically shut off.  You may also turn off search mode
by again pressing the `S' {or #} key.
.IP "T <Dir> - Tunnel through rock. {control-<Dir>}"
Tunneling (Mining) is a very useful art.  There are four
kinds of rock present in the dungeons of \fImoria\fP: Permanent Rock,
Granite Rock, Magma Intrusion, and Quartz Veins.  Permanent Rock
is exactly that, permanent.  Granite is very hard, therefore hard
to dig through, and contains no valuable metals.  Magma and
Quartz veins are softer and sometimes bear valuable metals and
gems, shown as a `$' or a `*' character.  You can tell if the
metal or gems are embedded into the wall by trying to move onto
them.  If you can't move over them, you'll have to dig them out.
There is an option which causes magma and quartz to be displayed
differently than other rock types.
.sp
Tunneling can be VERY difficult by hand, so when you dig be
sure to wield either a shovel or a pick.  Magical shovels and
picks can be found which allow the wielder to dig much faster
than normal, and a good strength also helps.
.sp
Tunneling can have a count.
.IP "V  - View scoreboard."
This command will display the contents of the score board on the
screen.  On a multiuser system, typing `V' again will show only those
scores from the score board that are yours.
.IP "a <Dir> - Aim a wand. {z - zap}"
Wands must be aimed in a direction to be used.  Wands are
magical devices and therefore use the Magical Devices ability of
the player.  They will either affect the first object/creature
encountered, or affect anything in a given direction, depending
upon the wand.  An obstruction such as a door or wall will generally
stop the effects of a wand from traveling further.
.IP "b  - Browse a book. {P - peruse}"
You can only read a book if you are of its realm.
Therefore a magic user could read a magic book, but not a holy
book.  Warriors will not be able to read either kind of book.
When the browse command is used, all of the spells or prayers
contained therein are displayed, along with information such as
their level, the amount of mana used up in casting them, and whether
or not you know the spell or prayer.  There
are a total of 31 different magical spells in four books, and 31
different prayers in four books.
.IP "c <Dir> - Close a door."
Nonintelligent and certain other creatures will not be able
to open a door.  Therefore shutting doors can be a life saver.
You must be adjacent to an open door, and you cannot close broken
doors.  Bashing a door open will break it.
.IP "d  - Drop an object from your inventory."
You can drop an object onto the floor beneath you if
that floor spot does not already contain an object.
Doors and traps are considered objects in this sense.  If you
have several objects of the same kind, you will be prompted
for dropping one or all of them.  It is possible to directly drop things
which you are wielding or wearing.
.IP "e  - Display a list of equipment being used."
Use the Equipment command to display a list of objects
currently being used by your character.  Each object
has a specific place where it is placed, and that only one object
of each type may be used at any one time, excepting rings of
which two can be worn, one on each hand.
.IP "f <Dir> - Fire/Throw an object/use a missile weapon. {t - throw}"
You may throw any object carried by your character.
Depending upon the weight of an object, it may travel across a
room or drop down beside you.  If you throw an object such as an
arrow, only one will be used at a time.
.sp
If you throw at a creature, your chance of hitting the
creature is determined by your plusses to hit, your ability at
throwing, and the object's plusses to hit.  Once the creature is
hit, the object may or may not do any actual damage to it.
Certain objects in the dungeon can do great amounts of damage
when thrown, but it's for you to figure out the obscure ones.
Oil flasks are considered to be lit before thrown; therefore, they
will do fire damage to a creature if they hit it.
.sp
To use a bow with arrows, simply wield the bow and throw the
arrows.  Extra plusses to damage and hitting are gained by
wielding the proper weapon and throwing the corresponding ammo.
A heavy crossbow with bolts for example, is a killer...
.IP "i  - Display a list of objects being carried."
This command displays a list of all objects being
carried, but not currently in use.  You may carry up to 22
different kinds of objects, not including those in your equipment
list.  Depending upon your strength, you will be able carry many
identical objects before hitting your weight limit.
.IP "j <Dir> - Jam a door with an iron spike. {S - spike}"
Most humanoid and many intelligent creatures can simply open
a closed door, and can eventually get through a locked door.
Therefore you may spike a door in order to jam it.  Each spike
used on a door will increase its strength, although the more
spikes you add, the less effect each additional spike has.
It is very easy to
jam a door so much as to make it impossible for your character to
bash it down, so spike doors wisely.  The bigger a
creature is, the easier it can bash a door down.  Therefore twenty
or more spikes might be necessary to slow down a dragon, where
one spike would slow down a kobold.  This command can be counted.
.IP "l <Dir> - Look in a direction. {x - examine}"
The Look command is useful in identifying the exact type of
object or creature shown on the screen.  Also, if a creature is
on top of an object, the look command will describe both.  You
can see creatures and objects up to 200 feet away (20 spaces).
You may freely use the Look command without the
creatures getting a move on you.
.sp
Looking in a particular direction sees everything within a cone
of vision which just overlaps the cones of the two adjacent directions.
Looking with the null direction `5' (or `.') sees everything which there
is to be seen.
.sp
You are also able to access you monster memories with this command.
If you see a creature, you are prompted to ask if you wish to see
a short paragraph of information about your experiences with that
creature.  See also the section on being attacked.
.IP "m  - Cast a magic spell."
To cast a spell, a character must have previously learned it,
and must also have in the inventory a magical book from which the
spell may be read.
Each spell has a chance of failure which starts out fairly large
but decreases as a character gains levels.  If a character does
not have enough mana, the chance of failure is greatly increased,
and he gambles on losing a point of constitution.  You will be
prompted for confirmation before trying to cast a spell when you
don't have enough mana.  Since a
character must read the spell from a book, he cannot be blind or
confused when casting a spell, and there must be some light
present.
.IP "o <Dir> - Open a door, chest, or lock."
To open an object such as a door or chest, you must use the
Open command.  If the object is locked, the Open command will
attempt to pick the lock, based on your ability at disarming.
If an object is trapped and you open it, the trap will
be set off.
This command can be counted, because you may need several tries to 
get a locked door or chest open.
.IP "p  - Read a prayer."
To pay effectively, a character must have learned the prayer,
and must also have in the inventory a holy book from which
the prayer may be read.
Each prayer has a chance of being ignored which starts out fairly large
but decreases as a
character gains levels.  If a character does not have enough
mana, the chance of failure is greatly increased, and he
gambles on losing a point of constitution.  You will be prompted
for confirmation before trying to pray when you don't have enough mana.
Since a character
must read the prayer from a book, he cannot be blind or confused
when praying, and there must be some light present.
.IP "q  - Quaff a potion."
To drink a potion use the Quaff command.  A potion affects
the player in some manner.  The effects of the potion may be
immediately noticed, or they may be subtle and unnoticed.
.IP "r  - Read a scroll."
To read a scroll use the Read command.  Most scroll spells either
affect the player or the area around the player; a few cases such as
identify scrolls act on other objects.  Two scrolls, the identify
scroll and the recharge scroll, have titles which can be read
without setting them off, and by pressing ESCAPE can be saved
for future use.
.IP "s  - Search general area one turn."
The Search command can be used to locate hidden traps and
secret doors about the player.  More than a single turn
of searching will be required in most cases.  You should always
search a chest before trying to open it because they are
generally trapped.
This command can be counted, which is useful if you are really sure
of finding something eventually.  A counted search ends as soon as
anything is found.
.IP "t  - Take off a piece of equipment. {T}"
Use the Take Off command to remove an object from use, and
return it to your inventory.  Occasionally you will run into a
cursed item which cannot be removed.  Cursed items are always
bad, and can only be taken off after removing the curse.
.IP "u  - Use a staff. {Z - Zap}"
The Use command will activate a staff.  Like scrolls, most
staffs have an area effect.  Because staffs are generally more
powerful than most other items, they are also harder to use
correctly.
.IP "v  - Display current version of game."
The Version command displays the credits for the current
version of \fImoria\fP.
.IP "w  - Wear or wield an item being carried."
To wear or wield an object in your inventory, use the
Wear/Wield command.  If another object is already in use for the same
function, it is automatically removed first; if you are wearing two
rings, you are given a choice of which one to remove.  An
object's bonuses cannot be gained until it is worn or wielded.
.IP "x  - Exchange primary and secondary weapons. {X}"
A secondary weapon is any weapon which may be needed often.
Instead of searching through your inventory, you may use the
exchange command to keep the weapon ready.  For instance, if you
wanted to use your bow most of the time, but needed a sword for
close combat, you could wield your sword, use the exchange
command to make it the secondary weapon, then wield your bow.  If
the sword was suddenly needed, simply use the exchange command to
switch between the bow and the sword.
.IP "/  - Identify a character shown on screen."
Use the identify command to find out what a character
displayed on the screen stands for.  For instance, by
pressing `/.', you can find out that the `.' stands for a floor spot.
When used with a creature, the identify command will tell you
only what class of creature the symbol stands for, not the
specific creature; therefore, use the look command for this
information.
.sp
If you identify the character for a creature in your monster memory,
you are also prompted to ask if you wish to see a paragraph of
information on those creatures identified by the given character.
Several creatures may be identified in this way.
Typing ESCAPE after the paragraph for any creature will abort back to
command level.  See also the section on being attacked.
.IP "?  - Display a list of commands."
The ? command displays a quick reference help page
on the screen.
.IP "\-  - Move without pickup."
This is followed by a move command, and causes you to move over any
object without picking it up.
You can associate a count with this command.
.IP "=  - Set options."
This is a free move, to set various \fImoria\fP options.
The available options are:
.IP " (1)"
Cut known corners when running.  This is on by default, and the only reason
for switching it off would be if you had the search flag on and wished to
look for doors in the extremity of every corner.
.IP " (2)"
Examine potential corners when running.  This is on by default, and allows
you to run along an unknown curving corridor.  If, however, you are running
from a creature, you may wish to switch this option off, because the
creature will cut the corner.
.IP " (3)"
Print self during a run.  This is off by default, which gives faster screen
updating.
.IP " (4)"
Stop when map sector changes.  This is off by default, but can be switched
on if you wish to stop running whenever a new part of the dungeon appears
in view.
.IP " (5)"
Treat open doors as empty space while running.  This is off by default, in
which case you stop whenever you run up to an open door.
.IP " (6)"
Prompt to pick up objects.  This is off by default, in which case stepping
over an object automatically causes you to pick it up.  With the option
on, you get prompted in all such cases with a description of the object
to see if you really want to take it.
.IP " (7)"
Rogue like command set.  This option controls the command set in use.  It
is off by default.
.IP " (8)"
Show weights in inventory.  This is off by default: switching it on causes
the inventory and equipment listings to include the weight of all objects.
This may be useful to know if your pack is getting too heavy.
.IP " (9)"
Highlight and notice mineral seams.  This is off by default.  Switching
it on causes quartz and magma to be displayed as `%' instead of `#';
also, it causes the look command to treat them as interesting objects.
This is handy when mining.
Setting this option does not immediately highlight all minerals,
but only those which are subsequently displayed.  To display all minerals,
just move the map around a bit with the `Where' (or `Locate') command.
.IP " (10)"
Beep for invalid character.  This is on by default.  When on, the program
will beep for most invalid characters, such as trying to choose a spell
that you haven't learned yet.  When off, there are no such beeps.
.IP " (11)"
Display rest/repeat counts.  This is on by default.  When on, the program
will progressively display the remaining turns left while resting, and
for repeated commands.  For those trying to play over a 2400 bps or less
connection, or for those playing on very slow microcomputers, turning this
off will make resting and repeated commands work much faster.
.IP
The setting of all these options persist in your savefile, even after you
die.
.IP "^P  - Previous message."
The Control-P command will
redisplay the last message printed on the message line at the
top of your screen.
A second such command will display all of the saved messages.
You may also give this command a count to specify the number
of previous messages to display.  At present, only 22 messages are saved.
.IP "^K  - Quit the game without saving. {Q}"
To exit the game without saving your character (i.e. kill him/her)
use the
Control-K command.  Once exited in this manner, your character
is nonrecoverable.
.IP "^X  - Save your character and exit the game."
To save your game so that it can be restarted later, use the
Control-X command.  Save files will also be generated if the game
crashes due to a system error.  When you die, a reduced save file
is produced containing only your monster memory, and your option
settings.
.IP "{  - Inscribe an object."
This command can be used to inscribe any short string on an object.
Inscriptions are limited to twelve characters.
The inscription applies only to the particular object, it is not
automatically transferred to all similar objects.
Under certain circumstances, \fImoria\fP will itself inscribe objects: if they
have been discovered to be cursed or enchanted, or if they have been
sampled without being identified.
In this last case, \fImoria\fP does in fact carefully inscribe every such
item.
.sp
If the inscription on an item is a single digit, that digit can be used
to refer to it when using, wearing, or wielding an item from your pack.
For example, if you keep a pick in your pack with the inscription 1, you
can switch to the pick by wielding item 1 without checking your full
inventory list to find out which item the pick is.
.IP "!  - Shell out of game."
Use the Shell command `!' to temporarily exit the game to
execute UNIX or MSDOS commands.  You may reenter the game by typing exit
to end the spawned process.  This is not implemented in the 
Macintosh version.
.IP "<  - Go up an up staircase."
If you move onto an up staircase you may use the `<' command
to go up one level.  There is always one staircase going up on
every level except for the town level (this does not mean it's
easy to find).  Going up a staircase will always take you to a
new dungeon area except for the town level, which remains the
same for the duration of your character.
.IP ">  - Go down a down staircase."
If you are on top of a down staircase you may use the `>'
command to go down one level.  There are always two or three
staircases going down on each level, except the town level which
has only one.  Going down will always take you to a new dungeon
area.
.IP ". <Dir> - Move in direction. {shift<Dir>}"
The Run command will move you in the indicated
direction until either you have to make a choice as between two
directions, or something interesting happens.  There are options
which determine behavior at corners, and at screen boundaries.
More precisely, the conditions which stop a run are as follows:
.IP (1)
A creature appears on the screen, one already on the screen moves,
or a creature attacks you or casts a spell at you.
.IP (2)
You move next to an object, or a feature such as a door or trap.
.IP (3)
You come to the end of open space, or the end of a passage, or
a junction of passages, or a hole in a wall.
.IP (4)
Anything typed during a run causes the run to stop.  The character
causing this to occur is ignored.  It is best to use a space,
which is ignored as a command, just in case the run stops just before
you type the character.
.IP (5)
Various changes of state, such as recovery from fear or loss of heroism,
will stop a run.
.sp
Corners are more complex.  A corner allows a choice between adjacent
rectangular and diagonal directions.  If you can see walls which ensure
that the diagonal gives a faster traversal, then action is determined
by the \*Qcut corners\*U options.
If it is set, then you move diagonally through the corner.  This gives
you maximum speed (as is nice if you are fleeing a hidden creature).
On the other hand, this option should not be set if you want
more careful coverage (as when you are searching) so that you
take two moves through the corner.
.sp
At a potential corner, where walls are not yet visible ahead of the
rectangular direction, the \*Qexamine corners\*U option is considered.
If set, you will move straight into the corner, which will light up
all the corner and so determine where you can go from there.  This
allows you to follow corners in new passages.  If the option is not set,
you stop.  This allows highly cautious running where you want to stop
at all potential choice points.
.sp
If you move off the screen while running, then a new section of the dungeon
is displayed and the run continues.  However, if the \*Qstop when
map changes\*U option
is set, you will stop.  Again, this is an option for nervous players; after
all, there may be a dragon on the new screen.
