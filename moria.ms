\"This is a slightly modified version of the documentation
\"for the original VMS game.  Changes were made to reflect differences
\"found in the UNIX port by James E. Wilson.
\" "tbl moria.doc | nroff -ms"
\" You might wanna change these two values:
.RP
.nr PO 1i
.nr LL 6.5i
.ds LH "The Dungeons of Moria
.ds CH "
.ds RH "Page %
.ds LF "Copyright
.ds CF "
.ds RF "Robert Alan Koeneke
.TL
The Dungeons of Moria
.AU
Robert Alan Koeneke
.AB no
Moria is intended for the Public Domain, and may not be sold or
marketed IN ANY FORM without the permission and written consent
from the author Robert Alan Koeneke. I retain all copyrights to
this program, in either the original or modified forms, and no
violation, deletion, or change of the copyright notice is
allowed. Furthermore, I will have no liability or responsibility
to any user with respect to loss or damage caused directly or
indirectly by this program.
.AE
.NH 1
Introduction
.LP
The game of \fImoria\fP is a single player dungeon simulation.
A player may choose from a number of races and classes when
creating their character, and then `run' that character over a
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
Stats may vary from 3 as a minimum to 18/100 as a maximum.
Because adventurers of interest tend to be better than
average characters, \fImoria\fP stats will average about 12.5, and are
further adjusted by race and class.
Some races are just naturally better at being certain classes, as will be
shown
later.
.LP
In addition to the more visible stats, each character has certain abilities
which are mainly determined by his race and class, but are also modified
by his stats.
The abilities are \fIfighting\fP, \fIthrowing/bows\fP, \fIsaving throw\fP,
\fIstealth\fP, \fIdisarming\fP, \fImagical devices\fP, \fIperception\fP,
\fIsearching\fP, and \fIinfra-vision\fP.
.LP
Characters will be assigned an early history, with money and a social class
based on that history.
Starting money is assigned based on history, charisma, and somewhat upon the
average of a character's stats.
A character with below average stats will receive extra money to help
him survive the first adventure.
.LP
Each character will also have physical attributes such as a
race, a height, weight, sex, and a physical description.
None of
these, except weight, play any part in the game other than to
give the player a \*Qfeeling\*U for his character.
Weight is used for computing carrying capacity and also for \fIbashing\fP.
.LP
Finally each character is assigned \fIhit points\fP based on their race,
class, and constitution.
Spell casters will also receive \fImana\fP which is expended when casting
spells.
Mana is based on Wisdom for Priests and Intelligence for Mages.

.NH 2
Character Stats
.IP "Strength"
Strength is important in fighting with weapons, or hand to hand.
A high strength can improve your chances of hitting, and the amount of
damage done with each hit.
Characters with low strengths may receive penalties.
Strength is also useful in tunneling, body and shield bashing, and in
carrying heavy items.
.IP "Intelligence"
Intelligence is the prime stat of a mage, or magician.
A high intelligence increases a mages chances of learning spells, and in
gaining mana.
No spell may be learned by mages with intelligences under 8.
Intelligence also modifies a character's chance of disarming traps and
picking locks.
.IP "Wisdom  "
Wisdom is the prime stat of a priest.
A high wisdom increases the chance of receiving new spells from a priest's
diety, and in the gaining of mana.
Wisdom also modifies a character's chance of resisting magical spells
cast upon his person.
.IP "Dexterity"
Dexterity is the combination of agility and quickness.
A high dexterity may allow a character to get multiple blows with lighter
weapons, thus greatly increasing his kill power, and may increase his
chances of hitting with any weapon.
Dexterity is also useful in picking locks and disarming traps.
.IP "Constitution"
Constitution is a characters ability to resist damage to their body, and to
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
Female characters tend to be somewhat smaller and lighter then their
male counterparts.
No adjustments to stats or abilities are made because of the sex of
a character.

.NH 2
Character Abilities
.LP
Characters possess nine different abilities which can help
them to survive. The starting abilities of a character are based
upon race and class. Abilities may be adjusted by high or low
stats, and may increase with the level of the character.
.IP "Fighting"
Fighting is the ability to hit and do damage with weapons or
fists. Normally a character gets a single blow from any weapon,
but if his dexterity and strength are high enough, he may receive
more blows per round with lighter weapons. Strength and
dexterity both modify the ability to hit an opponent. In
addition this skill increases with the level of the character.
.IP "Throwing/Bows"
Using stand-off missile weapons and throwing objects is
included in this skill. Different stats apply to different
weapons, but may modify the distance an object is thrown/fired,
the amount of damage done, and the ability to hit a creature.
This skill increases with the level of the character.
.IP "Saving Throw"
A Saving Throw is the ability of a character to resist the
effects of a spell cast on him by another person/creature. Note
that this does not include spells cast on the player by his own
stupidity, such as quaffing a nasty potion. This ability
increases with the level of the character, but then most high
level creatures are better at casting spells, so it tends to even
out.
.IP "Stealth"
The ability to move silently about is very useful.
Characters with good stealth can usually surprise their
opponents, gaining the first blow. Also, creatures may fail to
notice a stealthy character entirely, allowing a player to avoid
certain fights.
.IP "Disarming"
Disarming is the ability to remove traps (safely), and
includes picking locks on traps and doors. Note that a
successful disarming will gain the character some experience.
The character must have found a trap before it can be disarmed.
Dexterity and intelligence both modify the ability to disarm, and
this ability increases with the level of the character.
.IP "Using Magical Devices"
Using a magical device such as a wand or staff requires
experience and knowledge. Spell users such as mages and priests
are therefore much better at using a magical device than say a
fighter. This skill is modified by intelligence, and increases
with the level of the character.
.IP "Perception"
Perception is the ability to notice something without
actively seeking it out. This skill is based entirely upon race
and class, and will never improve unless magically enhanced.
.IP "Searching"
To search is to actively look for secret doors, floor traps,
and traps on chests. Rogues are the best at searching, but
mages, rangers, and priests are also good at it. Intelligence
modifies your searching ability, and your ability will also
increase with your level.
.IP "Infra Vision"
Infra-vision is the ability to see heat sources. Since most
of the dungeon is cool or cold, infra-vision will not allow the
player to see walls and objects. Infra-vision will allow a
character to see any warm-blooded creatures up to a certain
distance. This ability works equally well with or with out a
light source. Note that the majority of \fImoria\fP's creatures are
cold blooded, and will not be detected unless lit up by a light
source.

.NH 2
Choosing A Race
.LP
There are eight different races that you can choose from in
\fImoria\fP. Some races are restricted as to what profession they may
be, and each race has it's own adjustments to a character's stats
and abilities.
.IP "Human  "
The human is the base character, all other races are
compared to him. Human's can choose any class, and are average
at everything. Humans tend to go up levels faster than any other
race, because of their shorter life-spans. No racial adjustments
occur to character's choosing human.
.IP "Half-Elf"
Half-elves tend to be smarter and faster than a human, but
not as strong. Half-elves are slightly better at searching,
disarming, perception, stealth, and magic, but they are not as
good at hand weapons. Half-elves may choose any class.
.IP "Elf      "
Elves are better magicians then humans, but not as good at
fighting. They tend to be smarter and faster than either humans
or half-elves, and also have better wisdom. Elves are better at
searching, disarming, perception, stealth, and magic, but they
are not as good at hand weapons. Elves may choose any class
except Paladin.
.IP "Halfling"
Halflings, or Hobbits, are very good at bows, throwing, and
have good saving throws. They also are very good at searching,
disarming, perception, and stealth; so they make excellent
thieves (but prefer to be called burglars...). They will be much
weaker than humans, and no good at bashing. Halflings have fair
infra-vision, so can detect warm creatures at a distance.
Halflings can choose between being a fighter, mage, or rogue.
.IP "Gnome   "
Gnomes are smaller than dwarfs, but larger than halflings.
They, like the halflings, live in the earth in burrow-like homes.
Gnomes are practical jokers, so if they can kill something in a
humorous way, so much the better. Gnomes make excellent mages,
and have very good saving throws. They are good at searching,
disarming, perception, and stealth. They have lower strengths
and constitutions then humans so they are not very good at
fighting with hand weapons. Gnomes have fair infra-vision, so
can detect warm creatures at a distance. A gnome may choose
between being a fighter, mage, priest, or rogue.
.IP "Dwarf  "
Dwarves are the headstrong miners and fighters of legend.
Since dungeons are the natural home of a dwarf, they are
excellent choices for a warrior or priest. Dwarves tend to be
stronger, have higher constitutions, but are slower and less
intelligent than humans. Because they are so headstrong and are
somewhat wise, they resist spells which are cast on them.
Dwarves also have good infra-vision because they live
underground. They do have one big draw-back though. Dwarves are
loud-mouthed and proud, singing in loud voices, arguing with
themselves for no good reason, screaming out challenges at
imagined foes. In other words, dwarves have a miserable stealth.
.IP "Half-Orc"
Half-Orcs make excellent fighters, and decent priests, but
are terrible at magic. They are as bad as dwarves at stealth,
and horrible at searching, disarming, and perception. Half-Orcs
are, let's face it, ugly. They tend to pay more for goods in
town. Half-Orcs do make good priest's and rogues, for the simple
reason that Half-Orcs tend to have great constitutions and lots
of hit points.
.IP "Half-Troll"
Half-Trolls are incredibly strong, and have the highest hit
points of any character race. They are also very stupid and
slow. They will make great fighters and iffy priests. They are
bad at searching, disarming, perception, and stealth. They are
so ugly that a Half-Orc grimaces in their presence. They also
happen to be fun to run...

.NH 3
Race VS Skills And Stats
.LP
Stat Modifications due to race.
.KS
.TS
tab(^) center box;
l n n n n n n n n.
^^^^^^^Hit^Required
Race^Str^Int^Wis^Dex^Con^Chr^Dice^EXP/level
.sp
_
Human^0^0^0^0^0^0^10^+0%
Half-Elf^-1^+1^0^+1^-1^+1^9^+10%
Elf^-1^+2^+1^+1^-2^+1^8^+20%
Halfling^-2^+2^+1^+3^+1^+1^6^+10%
Gnome^-1^+2^0^+2^+1^-2^7^+25%
Dwarf^+2^-3^+1^-2^+2^-3^8^+20%
Half-Orc^+2^-1^0^0^+1^-4^10^+10%
Half-Troll^+4^-4^-2^-4^+3^-6^12^+20%
.TE
.KE
Abilities as compared to each other:
1 is lowest, or worst; 10 is highest, or best.
.KS
.TS
tab(^) center box;
l n n n n n n n l.
Race^Disarm^Search^Stealt^Percep^Fight^Bows^Save^Infra
_
Human^5^5^5^5^5^5^5^None
Half-Elf^6^6^7^6^5^6^6^None
Elf^8^8^7^7^4^9^7^None
Halfling^10^10^10^10^1^10^10^40 feet
Gnome^9^6^9^9^2^8^9^30 feet
Dwarf^6^7^5^5^9^5^8^50 feet
Half-Orc^2^5^3^2^8^2^2^30 feet
Half-Troll^1^1^1^1^10^1^1^30 feet
.TE
.KE

.NH 2
Choosing A Class
.LP
Once a race has been chosen, you will need to pick a class.
Some classes will not be available to certain races, for instance
a Half-Troll cannot become a Paladin. For the first few
adventures it is suggested that you run a warrior or rogue.
Spell casting generally requires a more experienced player that
is familiar with survival techniques.
.IP "Warrior"
A Warrior is a hack-and-slash character, who solves most of
his problems by cutting them to pieces, but will occasionally
fall back on the help of a magical device. His prime stats are
Strength and Constitution, and a good Dexterity can really help
at times. A Warrior will be good at Fighting and Throwing/Bows,
but bad at most other skills.
.IP "Mage    "
A Mage must live by his wits. He cannot hope to simply hack
his way through the dungeon, and so must therefore use his magic
to defeat, deceive, confuse, and to escape with. A mage is not
really complete without a golf-cart of magical devices to use in
addition to his spells. He can master the higher level magical
devices far easier than anyone else, and has the best saving
throw to resist effects of spells cast at him. Intelligence and
Dexterity are his primary stats. There is no rule that says a
mage cannot become a good fighter, but spells are his true realm.
.IP "Priest  "
A Priest is a character of holy devotion. They explore the
dungeon only to destroy the evil that lurks within, and if
treasure just happens to fall into their packs, well, so much
more to the glory of their church! Priests receive their spells
from a diety, and therefore do not choose which spells they will
learn. He is familiar with magical devices, preferring to call
them instruments of god, but is not as good as a mage in their
use. Priests have good saving throws, and make decent fighters,
preferring blunt weapons over edged ones. Wisdom and Charisma
are the priests primary stats.
.IP "Rogue   "
A Rogue is a character that prefers to live by his cunning,
but is capable of fighting his way out of a tight spot. He is
the master of traps and locks, no device being impossible for him
to over-come. A rogue has a high stealth allowing him to sneak
around many creatures without having to fight, or sneak up and
get the first blow. A rogue's perception is higher than any
other class, and many times he will notice a trap or secret door
before having to search. A rogue is better than warriors or
paladins with magical devices, but still can not rely on their
performance. A rogue's primary stats are Intelligence and
Dexterity.
.IP "Ranger  "
A Ranger is a fighter/mage. He is a good fighter, and the
best of the classes with a missile weapon such as a bow. The
ranger learns spells much more slowly than a mage, but is capable
of learning all but the most powerful spell. Because a ranger is
really a dual class character, it requires more experience to
advance him. A ranger has a good stealth, good perception, good
searching, a good saving throw, and is good with magical devices.
His primary stats are Intelligence and Dexterity.
.IP "Paladin"
A Paladin is a fighter/priest. He is a very good fighter,
second only to the warrior class, but not very good at missile
weapons. He receives prayers at a slower pace then the priest,
and can receive all but the most powerful prayer. Because a
paladin is really a dual class character, it requires more
experience to advance him. A paladin lacks much in the way of
abilities. He is poor at stealth, perception, searching, and
magical devices. He has a decent saving throw due to his divine
alliance. His primary stats are Strength and Charisma.

.NH 3
Race VS Class
.LP
.KS
.TS
tab(^) center box;
ce ce ce ce ce ce ce.
Race^Warrior^Mage^Priest^Rogue^Ranger^Paladin
_
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
Class VS Skills
.LP
Abilities as compared to each other:
1 is lowest, or worst; 10 is highest, or best.
.KS
.TS
tab(^) center box;
l c c c c c c c c c
l c c c c c c c c c
l n n n n n n n n r.
^^^Save^^^Magic^^^Required
Race^Fight^Bows^Throw^Stlth^Disarm^Device^Percep^Search^Exp.
_
Warrior^10^6^3^2^3^3^2^2^+0%
Mage^2^1^10^5^8^10^8^5^+30%
Priest^4^3^7^5^4^7^4^4^+10%
Rogue^8^9^7^10^10^7^10^10^+0%
Ranger^6^10^7^7^6^7^6^6^+40%
Paladin^10^5^4^2^2^4^4^2^+35%
.TE
.KE

.NH 1
Adventuring
.LP
After you have created your character, you will begin your
\fImoria\fP adventure. Symbols appearing on your screen will represent
the dungeon's walls and floor, objects and features, and
creatures lurking about. In order to direct your character
through his adventure, you will enter single character commands.
.LP
\fIMoria\fP symbols and commands each have a help section devoted
to them. You should review these sections before attempting an
adventure. Finally, a description of the town level and some
general help on adventuring are included.

.NH 2
Symbols On Your Map
.LP
Symbols on your map can be broken down into three
categories: Features of the dungeon such as walls, floor, doors,
and traps; Objects which can be picked up such as treasure,
weapons, magical devices, etc; and Monsters which may or may not
move about the dungeon, but are mostly harmful to your
character's well being.
.LP
Note that some symbols can be in more than one category.
Also note that treasure may be imbedded in a wall, and the wall
must be removed before the treasure can be picked up.
.LP
It will not be necessary to remember all of the symbols and
their meanings. A simple command, the '/', will identify any
character appearing on your map. See the section on commands for
further help.
.LP
.nf
.KS
.TS
box tab(%) center;
c s s s
l l l l.
Features
_
\&.%A floor space, or hidden trap%1%Entrance to General Store
#%A wall%2%Entrance to Armory
'%An open door%3%Entrance to Weapon Smith
+%A closed door%4%Entrance to Temple
^%A trap%5%Entrance to Alchemy Shop
<%A staircase up%6%Entrance to Magic Shop
>%A staircase down%:%Obstructing rubble
;%A loose floor stone%\ %An open pit (Blank)
.TE
.KE
.sp
.KS
.TS
box tab(^) center;
c s s s
l l l l.
Objects
_
!^A flask or potion^?^A scroll
"^An amulet^[^Hard armor
$^Money (Can be imbedded)^\e^A hafted weapon
&^A chest^]^Misc. armor
(^Soft armor^\&_^A staff
)^A shield^{^Missile (arrow, bolt, pebble)
*^Gems (Can be imbedded)^|^Sword or dagger
\-^A wand^}^Missile arm (Bow, X-bow, sling)
/^A pole-arm^~^Misc
\&=^A ring^,^Food
s^A skeleton^^
.TE
.KE
.sp
.KS
.TS
box tab(^) center;
c s s s
l l l l.
Monsters
_
a^Giant Ant^A^Giant Ant Lion
b^Giant Bat^B^The Balrog
c^Giant Centipede^C^Gelatinous Cube
d^Dragon^D^Ancient Dragon
e^Floating Eye^E^Elemental
f^Giant Frog^F^Fly
g^Golem^G^Ghost
h^Harpy^H^Hob-Goblin
i^Icky-Thing^I^Invisible Stalker
j^Jackal^J^Jelly
k^Kobold^K^Killer Beetle
l^Giant Lice^L^Lich
m^Mold^M^Mummy
n^Naga^N^
o^Orc or Ogre^O^Ooze
p^Human(iod)^P^Giant Human(iod)
q^Quasit^Q^Quythulg
r^Rodent^R^Reptile
s^Skeleton^S^Scorpion
t^Giant Tick^T^Troll
u^^U^Umber Hulk
v^^V^Vampire
w^Worm or Worm Mass^W^Wight or Wraith.
x^^X^Xorn
y^Yeek^Y^Yeti
z^Zombie^Z^
$^Creeping Coins^,^Mushroom Patch
.TE
.KE

.NH 2
Commands
.LP
All commands are entered by pressing a single key, or
control sequence (holding down the control key while pressing a
key). If a particular command requires additional action, it
will be prompted for, with one exception. When a direction is
required, no prompt is given unless and until an illegal response
is given.
.LP
For each command, the characters used by the original style option are
listed first, followed by the characters used by the rogue-like
option in braces, if different.

.NH 3
Note On <Dir>
.LP
For the original style option,
in the following instructions, the symbol <Dir> refers to a
numeric direction based on your keypad.
For the rogue-like option, the symbol <Dir> refers to one of the
following letters 'hykulnjb', or to a numeric direction based on
your keypad.
It is not valid to use
the number '5' in this context. One exception to this is with
movement, in which case '5' will rest the character for one turn.
For the rogue-like option, '.' will also rest the character for one
turn.
.LP
Commands which require a direction will not prompt you for
one, unless you input an illegal direction. Just enter a
direction after the entering the command.
.LP
.RS
.TS
tab(^);
c s s
n n n.
Original Directions
.sp
\0\\\\\&^\0|\&\0^\0\&/
7^8^9
.sp
\-\0 4^ ^6\&\0\-
.sp
1^2^3
\0/\0\&^\0|\&\0^\0\&\\\\
.TE
.TS
tab(^);
c s s
n n n.
Rogue-like Directions
.sp
\0\\\\\&^\0|\&\0^\0\&/
y^k^u
.sp
\-\0 h^ ^l\&\0\-
.sp
b^j^n
\0/\0\&^\0|\&\0^\0\&\\\\
.TE
.RE
For the original style option, movement is accomplished
by using your numeric keypad.
Simply press a number and your character will move one step in
that direction.
For the rogue-like option, movement is accomplished by using the
numeric keypad, or one of the directional characters mentioned above.
Pressing a '5' is equivalent to waiting for one
round (More efficient resting over long periods of time is
accomplished by using the Rest command). You can only move onto
and through floor spots, and only if they contain no creatures or
obstructing objects such as a closed door.
.LP
Moving your character one step at a time can be time
consuming and boring, so a faster method has been supplied.
For the original style option, by
using the Move command '.', you may move in a direction until
something interesting happens. For instance, by pressing the
period key '.' followed by the direction 8, your character would
continue to move up the screen, only coming to a stop after at
least one condition is satisfied.
For the rogue-like option, typing a shifted directional letter
will move you in that direction until something interesting happens.
These stopping conditions are:
.IP "[1]"
A creature appears upon the screen, or a creature already on
the screen moves.
.IP "[2]"
You move next to an object, or feature such as a door,
staircase, or trap.
.IP "[3]"
You come to a wall, and have more than one choice of
directions from which to continue, or are in a dead in
passage.
.IP "[4]"
You come to a junction of several passages.

.IP "B <Dir> - Bash {f - force}"
The Bash command includes breaking open doors and chests, or
bashing an opponent. Two main factors determine the ability of a
character to bash, their weight and their strength. In addition,
when bashing an opponent, you will either perform a body bash, or
if wielding a shield, perform a shield bash which is more
effective.
.sp
Bashing a door can throw the character off-balance, but this
will not generally be a problem. Doors that have been jammed
closed with spikes can only be opened by bashing. Locked doors
may also be bashed open. Note that bashing a door open will
permanently break it.
.sp
Bashing a creature has effects on both the player and his
opponent. Depending on a character's dexterity, he may or may
not be thrown off-balance allowing free moves to his opponents.
.sp
If the bash is successful, the opponent will be thrown
off-balance for 1 to 3 turns, thus allowing the character free
hits or a chance to run.
.sp
A player automatically performs a shield bash instead of a
body bash, if he is currently wearing a shield. A shield bash
adds the damage of a shield to that of the bash, so is more
effective. Note that size and material both affect the damage
that a shield will do.
.IP "C  - Print character (to screen or file)"
This command allows the player to either display his
character on the terminal screen, or to print an entire character
info listing to a file. If printed to a file, history, equipment
list, and an inventory list are included.
.IP "D <Dir> - Disarm a trap."
You can attempt to disarm floor traps, or trapped chests.
If you fail to disarm a trap, there is a chance that you blunder
and set it off. You can only disarm a trap on a chest after
finding it with the search command.
.IP "E  - Eat some food."
A character must eat occasionally to remain effective. As a
character grows hungry, a message will appear at the bottom of
the screen saying \*QHungry\*U. If a character remains hungry long
enough, he will become weak and eventually start fainting.
.IP "F  - Fill a lamp or lantern with oil."
If your character is currently using a lamp for light, and
if he has a flask of oil in inventory, he may refill the lamp by
using this command. A lamp is capable of a maximum of 5500 turns
of light, and each flask has 5000 turns of oil contained in it.
.IP "L  - Display map co-ordinates. {W - where}"
The Location command will display your character's current
co-ordinates as shown on a printed map (printed with the 'P'
command). Sectors contain up to 44 rows by 99 columns each. The
Location command will display the character's current row and
column map co-ordinates, as well as the sector number.
.IP "P  - Print map to file. {M - map}"
The Print command will write an entire map of the dungeon
floor explored to a file. Since the dungeon floor is large, the
map is broken up into sectors, each containing up to 44 rows by
99 columns.
.IP "R  - Rest for a number of turns."
You may rest one turn by pressing the '5' key. Resting for
longer periods of time is accomplished by using the Rest command,
followed by the number of turns you want to rest your character.
Resting will continue until the specified duration has expired,
or something to wake the character happens, such as a creature
wandering by. It is sometimes a good idea to rest a beat-up
character until he regains some of his hit points, but be sure to
have plenty of food if you rest often.
.sp
If you have accidently entered in a rest period too large,
or change your mind about the resting period, you may wake your
character up by typing control-C.
.IP "S  - Search mode toggle. {#}"
The Searching toggle will take the character into and out of
searching mode. When first pressed, the message \*QSearching\*U will
appear at the bottom of the screen. The character is now taking
two turns for each command, one for the command and one turn to
search about him. Note that this means he is taking twice the
time to move about the dungeon, and therefore twice the food. If
a creature should happen by or attack you, search mode will
automatically shut off. Otherwise you may turn off search mode
by again pressing the 'S' key.
.IP "T <Dir> - Tunnel through rock. {control-<Dir>}"
Tunneling (Mining) is a very useful art. There are four
kinds of rock present in the dungeons of \fImoria\fP: Permanent Rock,
Granite Rock, Magma Intrusion, and Quartz Veins. Permanent Rock
is exactly that, permanent. Granite is very hard, therefore hard
to dig through, and contains no valuable metals. Magma and
Quartz veins are softer and sometimes bare valuable metals and
gems, shown as a '$' or a '*' character. You can tell if the
metal or gems are embedded into the wall by trying to move onto
them. If you can't move over them, you'll have to dig them out.
.sp
Tunneling can be VERY difficult by hand, so when you dig be
sure to wield either a shovel or a pick. Magical shovels and
picks can be found which allow the wielder to dig much faster
than normal, and a good strength also helps.
.sp
It is sometimes possible to get a character trapped within
the dungeon by using various magical spells and items. So it is
a very good idea to always carry some kind of digging tool, even
when you are not planning on tunneling for treasure.
.IP "a <Dir> - Aim a wand. {z - zap}"
Wands must be aimed in a direction to be used. Wands are a
magical device and therefore use the Magical Devices ability of
the player. They will either affect the first object/creature
encountered, or affect anything in a given direction, depending
upon the wand. An obstruction such as door or wall will general
stop the effects of a wand from traveling further.
.IP "b  - Browse a book. {P - peruse}"
You can only read a book if you are of it's realm.
Therefore a magic user could read a magic book, but not a holy
book. Fighter's will not be able to read either kind of book.
When the browse command is used, all of the spells or prayers
contained in the book along with information about it, such as
it's level, the amount of mana used up in casting it, and whether
or not you know the spell or prayer, will be displayed. There
are a total of 31 different magical spells in four books, and 31
different prayers in four books.
.IP "c <Dir> - Close a door."
Non-intelligent and certain other creatures will not be able
to open a door. Therefore shutting doors can be a life saver.
You must be adjacent to an open door, and you cannot close broken
doors. Note that bashing a door open will break it.
.IP "d  - Drop an object from you inventory."
You can drop a single object onto the floor beneath you if
that floor spot does not already contain an object. Note that
doors and traps are considered objects in this sense. If you
drop an object such as a potion or scroll, a single one is
dropped onto the floor at a time. Group objects such as arrows
are all dropped at once onto the floor.
.IP "e  - Display a list of equipment being used."
Use the Equipment command to display a list of objects
currently being used by your character. Note that each object
has a specific place were it is placed, and that only one object
of each type may be used at any one time, excepting rings of
which two can be worn, one on each hand.
.IP "f <Dir> - Fire/Throw an object/Use a missile weapon. {t - throw}"
You may throw any object carried by your character.
Depending upon the weight of an object, it may travel across a
room or drop down beside you. If you throw an object such as an
arrow, only one will be used at a time.
.sp
If you throw at a creature, your chance of hitting the
creature is determined by your pluses to hit, your ability at
throwing, and the object's pluses to hit. Once the creature is
hit, the object may or may not do any actual damage to it.
Certain objects in the dungeon can do great amounts of damage
when thrown, but it's for you to figure out the obscure ones.
Oil flasks are considered to be lit before thrown, therefore they
will do fire damage to a creature if they hit it.
.sp
To use a bow with arrows, simply wield the bow and throw the
arrows. Extra pluses to damage and hitting are gained by
wielding the proper weapon and throwing the corresponding ammo.
A heavy crossbow with bolts for example, is a killer...
.IP "h  - Enter the \fImoria\fP help library"
A subprocess is spawned and the HELP utility entered. Help
on individual commands may be displayed without actually exiting
your game.
This is currently unimplemented.
.IP "i  - Display a list of objects being carried."
The Inventory command displays a list of all objects being
carried, but are not in current use. You may carry up to 22
different kinds of objects, not including those in your equipment
list. Depending upon your strength, you will be able carry many
identical objects before hitting your weight limit.
.IP "j <Dir> - Jam a door with an iron spike. {S - spike}"
Most humanoid and many intelligent creatures can simply open
a closed door, and can eventually get through a locked door.
Therefore you may spike a door in order to jam it. Each spike
used on a door will increase it's strength. It is very easy to
jam a door so much as to make it impossible for your character to
bash it down, so spike doors wisely. Note that the bigger a
creature is, the easier it can bash a door down. Therefore four
or more spikes might be necessary to slow down a dragon, where
one spike would slow down a kobold.
.IP "m  - Cast a magic spell."
First, a character must have learned a spell before he can
cast it. Next, when casting a spell, he must read the spell from
a book, so a book containing the spell must be in his inventory.
Each spell has a chance of failure which starts out fairly large
but decreases as a character gains levels. If a character does
not have the available mana, he increases his chance of failure,
and gambles on losing a point of constitution. Note that since a
character must read the spell from a book, he cannot be blind or
confused when casting a spell, and there must be some light
present.
.IP "l <Dir> - Look in a direction. {x - examine}"
The Look command is useful in identifying the exact type of
object or creature shown on the screen. Also, if a creature is
on top of an object, the look command will describe both. You
can see creatures and objects up to 200 feet away (20 units).
Note that you may freely use the Look command without the
creatures getting a move on you.
.IP "o <Dir> - Open a door, chest, or lock."
To open an object such as a door or chest you must use the
Open command. If the object is locked, the Open command will
attempt to pick the lock, based on your ability at disarming.
Note that if an object is trapped and you open it, the trap will
be set off.
.IP "p  - Read a prayer."
First, a character must have learned a prayer before he can
read it. Next, when reading a prayer, he must have the book
containing the prayer in his inventory. Each prayer has a chance
of failure which starts out fairly large but decreases as a
character gains levels. If a character does not have the
available mana, he increases his chance of failure, and gambles
on losing a point of constitution. Note that since a character
must read the prayer from a book, he cannot be blind or confused
and there must be some light present.
.IP "q  - Quaff a potion."
To drink a potion use the Quaff command. A potion affects
the player in some manner. The effects of the potion may be
immediately noticed, or they may be subtle and unnoticed.
.IP "r  - Read a scroll."
To read a scroll use the Read command. A scroll spell has
an area affect, except in a few cases such as identify scrolls
which act on other objects. Note that two scrolls, the identify
scroll and the recharge scroll, have titles which can be read
without setting them off, and by pressing <ESCAPE> can be saved
for future use.
.IP "s  - Search general area one turn."
The Search command can be used to locate hidden traps and
secret doors about the player. Note that more than a single turn
of searching will be required in most cases. You should always
search a chest before trying to open it because they are
generally trapped.
.IP "t  - Take off a piece of equipment. {T}"
Use the Take-Off command to remove an object from use, and
return it to your inventory. Occasionally you will run into a
cursed item which cannot be removed. Cursed items are always
bad, and can only be taken off after removing the curse.
.IP "u  - Use a staff. {Z - Zap}"
The Use command will activate a staff. Like scrolls, most
staffs have an area affect. Because staffs are generally more
powerful than most other items, they are also harder to use
correctly.
.IP "v  - Display current version of game."
The Version command displays the credits for the current
version of \fImoria\fP.
.IP "w  - Wear or wield an item being carried."
To wear or wield an object in your inventory, use the
Wear/Wield command. If an object is already in use for the same
function, it is automatically removed first. Note that an
objects bonuses cannot be gained until it is worn or wielded.
.IP "x  - Exchange primary and secondary weapons. {X}"
A secondary weapon is any weapon which may be needed often.
Instead of searching through your inventory, you may use the
exchange command to keep the weapon ready. For instance, if you
wanted to use your bow most of the time, but needed a sword for
close combat, you could wield your sword, use the exchange
command to make it the secondary weapon, then wield your bow. If
the sword was suddenly needed, simply use the exchange command to
switch between the bow and the sword.
.IP "/  - Identify a character shown on screen."
Use the identify command to find out what a character
displayed on the screen stands for. For instance, by
pressing '/.', you can find out that the '.' stands for a floor spot.
When used with a creature, the identify command will tell you
only what class of creature the symbol stands for, not the
specific creature, therefore use the look command for this
information.
.IP "?  - Display a list of commands."
The ? command displays a one page quick reference help page
on the screen.
.IP "^M  - Repeat last message. {^P}"
The <Control>-M (Carriage-Return or Enter key) command will
re-display the last message printed on the message line at the
top of your screen.
.IP "^R  - Redraw the screen."
To redraw the entire screen, use the <Control>-R command.
.IP "^K  - Quit the game without saving. {Q}"
To exit the game without saving your character (i.e. kill him/her)
use the
<Control>-K command. Once exited in this manner, your character
is non-recoverable.
.IP "^X  - Save your character and exit the game."
To save your game so that it can be restarted later, use the
<Control>-X command. The save file cannot be moved, or read.
Do not try to tamper with it.
Note that a copy of a saved character will
not work after that character has died.
.IP "$  - Shell out of game. {!}"
Use the Shell command '$' to temporarily exit the game to
execute UNIX commands. You may re-enter the game by typing exit
to end the spawned process.
.IP "<  - Go up an up-staircase."
If you move onto an up-staircase you may use the '<' command
to go up one level. There is always one staircase going up on
every level except for the town level (this does not mean it's
easy to find). Going up a staircase will always take you to a
new dungeon area except for the town level, which remains the
same for the duration of your character.
.IP ">  - Go down a down-staircase."
If you are on top of a down-staircase you may use the '>'
command to go down one level. There are always two or three
staircases going down on each level, except the town level which
has only one. Going down will always take you to a new dungeon
area.
.IP ". <Dir> - Move in direction. {shift<Dir>}"
The Move command '.' will move you in the indicated
direction until one of several conditions happen. These
conditions include, a creature appearing on the screen, a
creature already on the screen moving, an object or feature such
as a door, a staircase, or a trap is adjacent to the character,
character comes into a junction of passages, or character comes
to a wall with no choice or more than one choice of continuing
directions.

.NH 3
Quick Reference Page.
.LP
.KS
.TS
center tab(^) box;
c s s s
l l l l.
Original Commands
_
B <Dir>^Bash (object/creature)^q^Quaff a potion
C^Display character^r^Read a scroll
D <Dir>^Disarm a trap/chest^s^Search for hidden doors
E^Eat some food^t^Take off an item
F^Fill lamp with oil^u^Use a staff
L^Current location^v^Version and credits
P^Print map^w^Wear/Wield an item
R^Rest for a period^x^Exchange weapon
S^Search Mode^/^Identify an character
T <Dir>^Tunnel^?^Display this panel
a^Aim and fire a wand^^
b^Browse a book^CTRL/M^Repeat the last message
c <Dir>^Close a door^CTRL/R^Redraw the screen
d^Drop an item^CTRL/K^Quit the game
e^Equipment list^CTRL/X^Save character and quit
f^Fire/Throw an item^$^Shell out of game
h^\fImoria\fP Help^^
i^Inventory list^<^Go up an up-staircase
j <Dir>^Jam a door with spike^>^Go down a down-staircase
l <Dir>^Look given direction^. <Dir>^Move in direction
m^Cast a magic spell^Move:^7\08\09
o <Dir>^Open a door/chest^^4\0\0\06\0 (5 = Rest)
p^Read a prayer^^1\02\03
.TE
.KE

.LP
.KS
.TS
center tab(^) box;
c s s s
l l l l.
Rogue-like Commands
_
f <Dir>^Bash (object/creature)^q^Quaff a potion
C^Display character^r^Read a scroll
D <Dir>^Disarm a trap/chest^s^Search for hidden doors
E^Eat some food^T^Take off an item
F^Fill lamp with oil^Z^Use a staff
W^Current location^v^Version and credits
M^Print map^w^Wear/Wield an item
R^Rest for a period^X^Exchange weapon
#^Search Mode^/^Identify an character
CTRL<Dir>^Tunnel^?^Display this panel
z^Aim and fire a wand^^
P^Browse a book^CTRL/P^Repeat the last message
c <Dir>^Close a door^CTRL/R^Redraw the screen
d^Drop an item^Q^Quit the game
e^Equipment list^CTRL/X^Save character and quit
t^Fire/Throw an item^!^Shell out of game
h^\fImoria\fP Help^^
i^Inventory list^<^Go up an up-staircase
S <Dir>^Jam a door with spike^>^Go down a down-staircase
x <Dir>^Look given direction^shift<Dir>^Move in direction
m^Cast a magic spell^Move:^y\0k\0u
o <Dir>^Open a door/chest^^h\0\0\0l\0 (. = Rest)
p^Read a prayer^^b\0j\0n
.TE
.KE

.NH 2
The Town Level
.LP
The town level is where you will begin your adventure. The
town consists of six buildings each with an entrance, some
towns 'people', and a wall which surrounds the town. The first time
you are in town it will be daytime, but you may return to find
that darkness has fallen. (Note that some spells may act
differently in the town level.)

.NH 3
Townspeople
.LP
The town contains many different kinds of people. There are
the street urchins, young children who will mob an adventurer for
money, and seem to come out of the woodwork when excited.
Blubbering Idiots are a constant annoyance, but not harmful.
Public drunks wander about the town singing, and are of no
threat to anyone. Sneaky rogues hang about watching for a
likely victim to mug. And finally, what town would be complete
without a swarm of half drunk warriors, who take offense or
become annoyed just for the fun of it.
.LP
Most of the towns people should be avoided by the largest
possible distance when you wander from store to store. Fights
will break out though, so be prepared. Since your character grew
up in this world of intrigue, no experience is awarded for
killing on the town level.

.NH 3
Supplies
.LP
Your character will begin his adventure with some supplies
already on him. Use the Inventory 'i' command to check what
these supplies are. It will be necessary to buy other supplies
before continuing into the dungeon, however, so be sure to enter
each of the stores.

.NH 3
Town Buildings
.LP
You may enter any of the stores, if they are open, and
barter with the owner for items you can afford. But be warned
that the owners can easily be insulted, and may even throw you
out for a while if you insult them too often. To enter a store,
simply move onto the entrance represented by the numbers 1
through 6.
.LP
Once inside a store, his inventory will appear on the screen
along with a set of options for your character. You may browse
the store's inventory if it takes more than one page to display,
and you may sell or purchase items in his inventory. You can
execute your inventory and equipment commands to see what you are
carrying. Not shown with the options are the wear, take-off, and
exchange commands which will also work, but were excluded to keep
the options simple.
.LP
Stores do not always have everything in stock. As the game
progresses, they may get new items so check from time to time.
Also, if you sell them an item, it may get sold to a customer
while you are adventuring so don't always expect to be able to
get back anything you have sold.
.LP
Store owners will not buy harmful or useless items. If an
object is unidentified, they will pay you some base price for it.
Once they have bought it they will immediately identify the
object. If it is a good object, they will add it to their
inventory. If it was a bad bargain, they simply throw the item
away. In any case, you may receive some knowledge of the item if
another is encountered.
.IP "The General Store"
The General Store sells foods, drinks, some clothing,
torches, lamps, oil, and spikes. All of these items, and others,
can be sold back to the General store for money. The entrance to
the General Store is a '1'.
.IP "The Armory"
The Armory is where the town's armor is fashioned. All
sorts of protective gear may be bought and sold here. The
entrance to the Armory is a '2'.
.IP "The Weaponsmith\'s Shop"
The Weaponsmith's Shop is where the town's weapons are
fashioned. Hand and missile weapons may be purchased and sold
here, along with arrows, bolts, and shots. The entrance to the
Weaponsmiths is a '3'.
.IP "The Temple"
The Temple deals in healing and restoration potions, as well
as bless scrolls, word-of-recall scrolls, some approved priestly
weapons, etc. The entrance to the Temple is a '4'.
.IP "The Alchemy shop"
The Alchemy Shop deals in all manner of potions and scrolls.
The entrance to the Alchemy Shop is a '5'.
.IP "The Magic User\'s Shop"
The Magic User's Shop is the most expensive of all the
stores. It deals in all sorts of rings, wands, amulets, and
staves. The entrance to the Magic Shop is a '6'.

.NH 2
Within The Dungeon
.LP
Once your character is adequately supplied with food, light,
armor, and weapons, he is ready to enter the dungeon. Move on
top of the '>' symbol and use the down '>' command. Your
character enters a maze of interconnecting staircases and finally
passes through a one-way door. He is now on the first level of
the dungeon (50 feet), and must survive many horrible and
challenging encounters to find the treasure lying about.

.NH 3
Light
.LP
There are two sources for light once inside the dungeon.
Permanent light which has been magically placed within rooms, and
a light source carried by the player. If neither is present, the
character will be unable to map or see any attackers. Lack of
light will also affect searching, picking locks, and disarming.
.LP
A character must wield a torch or lamp in order to supply
his own light. Once a torch or lamp has only 50 or less turns
left before burning out, the message \*QYour light is getting low\*U
will be displayed at random intervals. Once a torch is burnt
out, it is useless and can be dropped. A lamp or lantern can be
refilled with oil by using the Fill 'F' command. You must of
course be carrying extra oil to refill a lantern.

.NH 3
Attacking And Being Attacked
.LP
Attacking is simple in \fImoria\fP. If you move into a creature,
you attack him. You can attack from a distance by firing a
missile, or by magical means such as aiming a wand. Creatures
attack in the same way, if they move into you, they attack you.
Some creatures can cast spells from a distance, and dragon type
creatures can breath from a distance, but these are the only
exceptions.
.LP
If you are wielding a weapon, the damage for the weapon is
used when you hit the creature. If you are wielding no weapons,
you get two fist strikes. Note that very strong creatures can do
a lot of damage with their fists... A character may have a
primary and secondary weapon. A secondary weapon is kept on the
belt, or on the shoulder for immediate use. You can switch
between your primary and secondary weapons by using the exchange
command. Be sure you are wielding the proper weapon when
fighting. Hitting a dragon over the head with a bow will simply
make him mad, and get you killed.
.LP
Missile weapons, such as bows, can be wielded, and then the
proper missile, in this case an arrow, can be fired across the
room into a target. Missiles can be used without the proper
missile weapon, but used together they have a greater range and
do far more damage.
.LP
Hits and misses are determined by ability to hit vs. armor
class. A miss doesn't necessarily mean you failed to hit the
target, but only that you failed to do any damage. Therefore
a 'hit' is a strike that does some damage. Higher armor classes
make it harder to do damage, therefore more misses.

.NH 4
Your Weapon
.LP
Carrying a weapon in your backpack does you no good. You
must wield a weapon before it can be used in a fight. Note that
a secondary weapon can be kept by wielding it and then using the
exchange command. A secondary weapon is not in use, simply
ready to be switched with the current weapon if needed.
.LP
Weapons have two main characteristics, their ability to hit
and their ability to do damage, expressed as '(+ ,+ )'. A normal
weapon would be '(+0,+0)'. Many weapons in \fImoria\fP have magical
bonuses to hit and/or do damage. Some weapons are cursed, and
have penalties that hurt the player. Note that cursed swords
cannot be un-wielded until the curse is lifted.
.LP
Missile weapons, such as bows, have only one major
characteristic which is to hit, expressed '(+ )'. This plus to
hit is added to that of the missile used, if the proper
weapon/missiles combination are used.
.LP
Although you receive any magical bonuses an unidentified
weapon may possess when you wield it, those bonuses will not be
added in to the displayed values of to-hit and to-dam on your
character sheet. You must identify the weapon before the
displayed values reflect the real values used.
.LP
Finally, some rare weapons have special abilities. These are
called ego weapons, and are feared by great and meek. An ego
sword must be wielded to receive benefit of its abilities.
.LP
Special weapons are denoted by the following abbreviations:
.IP "DF - Defender."
A magical weapon that actually helps the wielder defend
himself, thus increasing his armor class.
.IP "FB - Frost Brand."
A magical weapon of ice that delivers a cold critical to
heat based creatures.
.IP "FT - Flame Tongue."
A magical weapon of flame that delivers a heat critical
to cold based creatures.
.IP "HA - Holy Avenger."
A Holy Avenger is by far the most powerful of weapons.
Holy Avengers have been known to increase several of the
wielder's stats, to actually increase the wielder's armor
class (because of the terror the weapon spawns in it's foes),
and to actually help the wielder to fight more effectively.
.IP "SD - Slay Dragon."
A Slay Dragon weapon is a special purpose weapon whose
sole intent is to destroy dragon-kind. Therefore, when used
against a dragon, the amount of damage done is greatly
increased.
.IP "SE - Slay Evil."
A Slay Evil weapon is a special purpose weapon whose
sole intent is to destroy all forms of evil. When used
against an evil creature, either alive or undead, the damage
done is greatly increased.
.IP "SM - Slay Monster."
A Slay Monster weapon is a special purpose weapon whose
sole intent is to destroy all the vile monsters of the world.
A monster is any creature not natural to the world.
Therefore an orc would be a monster, but a giant snake would
not be.
.IP "SU - Slay Undead."
A Slay Undead weapon is a special purpose weapon whose
sole intent is to destroy all forms of undead. This weapon
is hated and feared by the intelligent undead, for a single
blow from this weapon is capable of destroying them.

.NH 4
Body And Shield Bashes
.LP
Weight is the primary factor in being able to bash
something, but strength plays a role too. After bashing, a
character may be off balance for several rounds depending upon
his Dexterity.
.LP
Doors can be broken down by bashing them. Once a door is
bashed open, it is forever useless and cannot be closed.
.LP
Chests too may be bashed open, but be warned that the
careless smashing of a chest often ruins the contents. Bashing
open a chest will not disarm any traps it may contain, but does
allow the strong and ignorant to see what is inside.
.LP
Finally, a creature may be bashed. If a shield is currently
being worn, the bash is a shield bash and will do more damage.
In either case, a bash may throw an opponent off balance for a
number of rounds, allowing a player to get in a free hit or more.
If the player is thrown off-balance, his opponent may get free
hits on him. This is a risky attack.

.NH 4
Your Armor Class
.LP
Armor class is a number that describes the amount and the
quality of armor begin worn. Armor class will generally run from
about 0 to 40, but could become negative or greater than 40 in
rare cases.
.LP
The larger your armor class, the more protective it is. A
negative armor class would actually help get you hit. Armor
protects you in three manners. One, it makes you harder to be
hit for damage. A hit for no damage is the same as a miss. Two,
good armor will absorb some of the damage that your character
would have taken. An armor class of 30 would absorb 30% of any
damage meant for him. Three, fire and acid damage are reduced by
wearing body armor. It is obvious that a high armor class is a
must for surviving the lower levels of \fImoria\fP.
.LP
Each piece of armor has an armor class adjustment, and a
magical bonus. Armor bought in town will have these values
displayed with their description. Armor that is found within the
dungeon must be identified before these values will be displayed.
Armor class values are always displayed between a set of
brackets '[ ,+ ]'. The first value is the armor class of the item. The
second number is the magical bonus of the item, and will always
have a sign preceding the value. There are a few cases where the
form '[+ ]' is used, meaning the object has no armor class, only
a magical armor bonus if worn.
.LP
Some pieces of armor will possess special abilities denoted
by the following abbreviations:
.IP "RA - Resist Acid."
This magical ability is usually enchanted into armor,
but may occasionally be found as an ability of a weapon. A
character using such an object will take only a quarter
damage from any acid thrown upon him. In addition, armor so
enchanted will resist the acid's effects and not be damaged
by it.
.IP "RC - Resist Cold."
This magical ability is also found in both weapons and
armor. A character using a resist cold object will take only
half damage from frost and cold.
.IP "RF - Resist Fire."
This magical ability is found in both weapons and armor.
A character using a resist fire object will take only one
quarter damage from heat and fire.
.IP "RL - Resist Lightning."
This magical ability is found in both weapons and armor.
A character using a resist lightning object will take only
one quarter damage from electrical attacks.
.IP "R - Resistance."
This magical ability is found only in armor. A
character wearing armor with this ability will have
resistance to Acid, Cold, Fire, and Lightning as explained in
each part above.

.NH 3
Objects Found In The Dungeon
.LP
The mines are full of objects just waiting to be picked up
and used. How did they get there? Well, the main source for
useful items are all the foolish adventurers that proceeded into
the dungeon before you. They get killed, and the helpful
monsters scatter the various treasure throughout the dungeon.
Most cursed items are placed there by the joyful evil sorcerers,
who enjoy a good joke when it gets you killed.
.LP
You pick up objects by moving on top of them. You can carry
up to 22 different items in your backpack while wearing and
wielding many others. Note that although you are limited to 22
different items, you may be carrying several of each item
restricted only by the amount of weight your character can carry.
Your character's weight limit is determined by his Strength.
Only one object may occupy any one given floor location, which
may or may not also contain one monster. Note that doors, traps,
and staircases are considered objects for this purpose.
.LP
Many objects found within the dungeon have special commands
for their use. Wands must be Aimed, staves must be Used, scrolls
must be Read, and potions must be Quaffed. In any case, you must
first be able to carry an object before you can use it. Some
objects, such as chests, are very complex. Chests contain other
objects and may be trapped, and/or locked. Read the list of
player commands carefully for a further understanding of chests.
.LP
One item in particular will be discussed here. The scroll
of \*QWord of Recall\*U can be found within the dungeon, or bought at
the temple in town. It acts in two manners, depending upon your
current location. If read within the dungeon, it will teleport
you back to town. If read in town, it will teleport you back
down to the deepest level of the dungeon one which your character
has previously been. This makes the scroll very useful for
getting back to the deeper levels of \fImoria\fP. Once the scroll has
been read it takes a while for the spell to act, so don't expect
it to save you in a crisis.
.LP
And lastly, a final warning. Not all objects are what they
seem. Skeletons lying peacefully about the dungeon have been
known to get up...

.NH 3
Cursed Objects
.LP
Some objects, mainly armor and weapons, have had curses laid
upon them. These horrible objects will look like any other
normal item, but will detract from your characters stats or
abilities if worn. They will also be impossible to remove until
a remove curse is done.
.LP
When a cursed item has been identified, an asterisk '*' will
appear next to the inventory letter of the item. If you should
wear a cursed item, you will immediately know it is cursed and
again the asterisk will appear.

.NH 3
Mining
.LP
Much of the treasure within the dungeon can be found only by
mining it out of the walls. Many rich strikes exist within each
level, but must be found and mined. Quartz veins are the
richest, yielding the most metals and gems, but magma veins will
have some hordes hidden within.
.LP
Mining is virtually impossible without a pick or shovel.
Picks and shovels have an additional magical ability expressed as '(+ )'.
The higher the number, the better the magical digging
ability of the tool. Note that a pick or shovel also has plusses
to hit and damage, and can be used as a weapon.
.LP
When a vein of quartz or magma is located, the character
should wield his pick or shovel and begin digging out a section.
When that section is removed, he should locate another section of
the vein, and begin the process again. Since granite rock is
much harder to dig through, it is much faster to follow the vein
exactly and dig around the granite.
.LP
If the character has a scroll or staff of treasure location,
he can immediately locate all strikes of treasure within a vein
shown on the screen. This makes mining much easier and more
profitable.

.NH 3
Staircases
.LP
Staircases are the manner in which you get deeper, or climb
out of the dungeon. The symbols for the up and down staircases
are the same as the commands to use them. A '<' represents an up
staircase and a '>' represents a down staircase. You must move
your character over the staircase before you can use them.
.LP
Each level has at least one up staircase, and at least two
down staircases. There are no exceptions to this rule. You may
have trouble finding some well hidden secret doors, but the
stairs are there.

.NH 3
Secret Doors, Passages, And Rooms
.LP
Many secret doors are used within the dungeon to confuse and
demoralize adventurers foolish enough to enter. But with some
luck, and lots of concentration, you can find these secret doors.
.LP
Secret doors will sometimes hide rooms or corridors, or even
entire sections of that level of the dungeon. Sometimes they
simply hide small empty closets or even dead ends.
.LP
Creatures in the dungeon will generally know and use this
secret doors. If they leave one open, you will be able to go
right through it. If they close it behind them you will have to
search for the catch first. Once a secret door has been
discovered by you, it is drawn as a known door and no more
searching will be required to use it.

.NH 3
Winning The Game
.LP
Once your character has progressed into killing dragons with
but a mean glance and snap of his fingers, he may be ready to
take on the Balrog. The Balrog will appear on every level after
level 49, so don't go down there until you are ready for him.
.LP
The Balrog cannot be killed in some of the easier methods
used on normal creatures. Because of the Balrog's cunning, he
will teleport away to another level if a spell such as genocide
is used upon him, and the Balrog cannot be polymorphed, slept, or
charmed. Magical spells like coldball are effective against him
as are weapons, but he is difficult to kill and if allowed to
escape for a time can heal himself.
.LP
If you should actually survive the attempt of killing the
Balrog, you will receive the status of WINNER. Since you have
defeated the toughest monster alive, your character is ready to
retire and cannot be saved. When you quit the game, your
character receives a surprise bonus score and is entered into the
toptwenty file.

.NH 3
Upon Death And Dying
.LP
If your character falls below 0 hit points, he has died and
cannot be restored. A tombstone showing information about your
character will be displayed with the option to print the
information to a file.
.LP
After the tombstone, the toptwenty list of heroes is
displayed. If your score beats any of the toptwenty, your
character will join this elite rank of heroes. Otherwise, well,
there is always next time...

.NH 3
Wizards
.LP
There are rumors of \fImoria\fP Wizards which, if asked nicely,
can explain details of the \fImoria\fP game that seem complicated to
beginners. In addition, they have special spells which can
restore characters lost by accident, such as power-outs and such.
