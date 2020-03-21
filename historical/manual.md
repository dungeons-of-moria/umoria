# The Dungeons of Moria

_Robert A. Koeneke's classic roguelike dungeon crawler._


Contents

1. Introduction
2. Running The Game
3. The Character
4. Adventuring
5. Symbols On The Screen
6. Commands
7. The Town Level
8. Within The Dungeon
9. Attacking and Being Attacked
10. Objects Found In The Dungeon
11. Winning The Game
12. Upon Death and Dying
13. Wizards
14. Contributors
15. Umoria Licence


## 1. Introduction

The game of *moria* is a single player dungeon simulation. A player may
choose from a number of races and classes when creating a character, and
then *run* that character over a period of days, weeks, even months,
attempting to win the game by defeating the Balrog which lurks in the
deeper levels.

The character will begin the adventure on the town level where supplies,
weapons, armor, and magical devices can be acquired by bartering with
various shop owners. After preparing for this adventure, the character
can descend into the dungeons of *moria* where fantastic adventures
await!

Before beginning an adventure, this document should be read carefully.
The game of *moria* is a complicated game (but not as complicated as
other roguelikes), and will require a dedicated player to win.


## 2. Running The Game


    umoria [ -h ] [ -v ] [ -r ] [ -d ] [ -n ] [ -w ] [ -s ] [ SAVEGAME ]


By default, *moria* will save and restore games from a file called
`game.sav` in the directory where the game is located. The save file
can also be explicitly specified on the command line.

When the `-n` option is used, *moria* will create a new game, ignoring any
save file which may already exist. This works best when a save file name
is specified on the command line, as this will prevent *moria* from
trying to overwrite the default save file (if it exists) when saving a
game.

Movement in various directions is accomplished by pressing the numeric keypad
keys. When `-r` is specified, movement is accomplished in the same way as the
original _Rogue_ game (`hjkl`). This option will override defaults stored in the
save file.

When `-d` is specified, *moria* displays all of the scores in the score
file and exits.

When `-w` is specified, *moria* will start up in wizard mode. Dead
characters can be resurrected using this option when starting the game.
Resurrected characters are teleported to the town level and given zero
hit-points. Wizard mode is intended for debugging the game, and for
experimenting with new features.

*Using wizard mode to win the game is considered cheating. Games played with
wizard mode are not scored.*

To make random events happen in a predictable manner a `seed` number can be
given with the `-s` option (only for new games).

Use `-v` to show the current version of Umoria.

Use `-h` to show the help screen.


## 3. The Character

All characters have six main attributes which modify their basic
abilities. These six attributes, called *stats*, are *strength*,
*intelligence*, *wisdom*, *dexterity*, *constitution*, and *charisma*.
Stats may vary from a minimum of 3 to a maximum of 18. At the highest
level, stats are further qualified by a number from zero to one hundred,
so that the highest value is actually 18/100. A value of 18/100 can be
thought of as equivalent to 19, and 18/00 (not actually used) is
equivalent to 18. Because adventurers of interest tend to be better than
average characters, *moria* stats will average about 13, and are further
adjusted by race and class. Some races are just naturally better at
being certain classes, as will be shown later.

In addition to the more visible stats, each character has certain
abilities which are mainly determined by their race, class, and level,
but are also modified by their stats. The abilities are *fighting*,
*throwing/bows*, *saving throw*, *stealth*, *disarming*, *magical
devices*, *perception*, *searching*, and *infra-vision*.

Characters will be assigned an early history, with money and a social
class based on that history. Starting money is assigned based on
history, charisma, and somewhat upon the average of a character’s stats.
A character with below average stats will receive extra money to help
them survive the first adventure.

Each character will also have physical attributes such as race, height,
weight, sex, and a physical description. None of these, except weight,
play any part in the game other than to give the player a "feeling" for
their character. Weight is used for computing carrying capacity and also
for bashing.

Finally, each character is assigned *hit-points* based on their race,
class, and constitution. Spell casters will also receive *mana* which is
expended when casting spells. Mana is based on Wisdom for Priests and
Intelligence for Mages.


### 3.1 Character Stats

#### `STR`

Strength is important in fighting with weapons and hand to hand
combat. A high strength can improve the chances of hitting, and the
amount of damage done with each hit. Characters with low strengths
may receive penalties. Strength is also useful in tunneling, body
and shield bashing, and in carrying heavy items.

#### `INT`

Intelligence is the prime stat of a mage, or magician. A high
intelligence increases a mage’s chances of learning spells, and it
also increases the amount of mana a mage has. No spell may be
learned by mages with intelligences under 8. Intelligence also
modifies a character’s chance of disarming traps, picking locks, and
using magic devices.

#### `WIS`

Wisdom is the prime stat of a priest. A high wisdom increases the
chance of receiving new spells from a priest’s deity, and it also
increases the amount of mana a priest has. No spell may be learned
by priests with wisdom under 8. Wisdom also modifies a character’s
chance of resisting magical spells cast upon their person.

#### `DEX`

Dexterity is a combination of agility and quickness. A high
dexterity may allow a character to get multiple blows with lighter
weapons, thus greatly increasing kill power, and may increase the
chances of hitting with any weapon and dodging blows from enemies.
Dexterity is also useful in picking locks, disarming traps, and
protection from pick pockets.

#### `CON`

Constitution is a character’s ability to resist damage to the body,
and to recover from damage received. Therefore a character with a
high constitution will receive more hit points, and be more
resistant to poisons.

#### `CHR`

Charisma represents a character’s personality, as well as physical
looks. A character with a high charisma will receive better prices
from store owners, whereas a character with a very low charisma will
be robbed blind. A high charisma will also mean more starting money
for the character.


### 3.2 Character Sex

The player may choose to be either a male or a female character. Only
height and weight are affected by a character’s gender. Female
characters tend to be somewhat smaller and lighter than their male
counterparts. No adjustments to stats or abilities are made because of
the gender of a character. Female characters start out with slightly
more money than male characters to help offset the weight penalty.


### 3.3 Character Abilities

Characters possess nine different abilities which can help them to
survive. The starting abilities of a character are based upon race and
class. Abilities may be adjusted by high or low stats, and may increase
with the level of the character.

#### `Fighting`

Fighting is the ability to hit and do damage with weapons or fists.
Normally a character gets a single blow from any weapon, but if
their dexterity and strength are high enough, they may receive more
blows per round with lighter weapons. Strength and dexterity both
modify the ability to hit an opponent. This skill increases with the
level of the character.

#### `Throwing/Bows`

Using ranged missile weapons and throwing objects is included in
this skill. Different stats apply to different weapons, but this
ability may modify the distance an object is thrown/fired, the
amount of damage done, and the ability to hit a creature. This skill
increases with the level of the character.

#### `Saving Throw`

A Saving Throw is the ability of a character to resist the effects
of a spell cast on them by another person or creature. This does not
include spells cast on the character by their own stupidity, such as
quaffing a nasty potion. This ability increases with the level of
the character, but then most high level creatures are better at
casting spells, so it tends to even out. A high wisdom also
increases this ability.

#### `Stealth`

The ability to move silently about is very useful. Characters with
good stealth can usually surprise their opponents, gaining the first
blow. Also, creatures may fail to notice a stealthy character
entirely, allowing a character to avoid certain fights. This skill
is based entirely upon race and class, and will never improve unless
magically enhanced.

#### `Disarming`

Disarming is the ability to remove traps (safely), and includes
picking locks on traps and doors. A successful disarming will gain
the character some experience. A trap must be found before it can be
disarmed. Dexterity and intelligence both modify the ability to
disarm, and this ability increases with the level of the character.

#### `Using Magical Devices`

Using a magical device such as a wand or staff requires experience
and knowledge. Spell users such as mages and priests are therefore
much better at using a magical device than say a warrior. This skill
is modified by intelligence, and increases with the level of the
character.

#### `Perception`

Perception is the ability to notice something without actively
seeking it out. This skill is based entirely upon race and class,
and will never improve unless magically enhanced.

#### `Searching`

To search is to actively look for secret doors, floor traps, and
traps on chests. Rogues are the best at searching, but mages,
rangers, and priests are also good at it. This skill is based
entirely upon race and class, and will never improve unless
magically enhanced.

#### `Infra-Vision`

Infra-Vision is the ability to see heat sources. Since most of the
dungeon is cool or cold, infra-vision will not allow the character to
see walls and objects. Infra-Vision will allow a character to see any
warm-blooded creatures up to a certain distance. This ability works
equally well with or without a light source. The majority of
*moria*’s creatures are cold-blooded, and will not be detected
unless lit up by a light source. All non-human races have innate
infra-vision ability. Humans can gain infra-vision only if it is
magically enhanced.


### 3.4 Choosing A Race

There are eight different races that can be chosen from in *moria*. Some
races are restricted as to what profession they may be, and each race
has its own adjustments to a character’s stats and abilities.

#### `Human`

The human is the base character, all other races are compared to
this race. Humans can choose any class, and are average at
everything. Humans tend to go up levels faster than any other race,
because of their shorter life spans. No racial adjustments occur to
characters choosing human.

#### `Half-Elf`

Half-elves tend to be smarter and faster than a human, but not as
strong. Half-elves are slightly better at searching, disarming,
perception, stealth, and magic, but they are not as good at hand
weapons. Half-elves may choose any class.

#### `Elf`

Elves are better magicians then humans, but not as good at fighting.
They tend to be smarter and faster than either humans or half-elves,
and also have better wisdom. Elves are better at searching,
disarming, perception, stealth, and magic, but they are not as good
at hand weapons. Elves may choose any class except Paladin.

#### `Halfling`

Halflings, or Hobbits, are very good at bows, throwing, and have
good saving throws. They also are very good at searching, disarming,
perception, and stealth; so they make excellent thieves (but prefer
to be called burglars...). They will be much weaker than humans, and
no good at bashing. Halflings have fair infra-vision, so they can
detect warm creatures at a distance. Halflings can choose between
being a warrior, mage, or rogue.

#### `Gnome`

Gnomes are smaller than dwarfs, but larger than Halflings. They,
like the Halflings, live in the earth in burrow-like homes. Gnomes
are practical jokers, so if they can kill something in a humorous
way, so much the better. Gnomes make excellent mages, and have very
good saving throws. They are good at searching, disarming,
perception, and stealth. They have lower strength than humans so
they are not very good at fighting with hand weapons. Gnomes have
fair infra-vision, so they can detect warm creatures at a distance. A
gnome may choose between being a warrior, mage, priest, or rogue.

#### `Dwarf`

Dwarves are the headstrong miners and fighters of legend. Since
dungeons are the natural home of a dwarf, they are excellent choices
for a warrior or priest. Dwarves tend to be stronger and have higher
constitutions, but are slower and less intelligent than humans.
Because they are so headstrong and are somewhat wise, they resist
spells which are cast on them. Dwarves also have good infra-vision
because they live underground. They do have one big drawback though.
Dwarves are loudmouthed and proud, singing in loud voices, arguing
with themselves for no good reason, screaming out challenges at
imagined foes. In other words, Dwarves have a miserable stealth.

#### `Half-Orc`

Half-Orcs make excellent warriors, and decent priests, but are
terrible at magic. They are as bad as Dwarves at stealth, and
horrible at searching, disarming, and perception. Half-Orcs are,
let’s face it, ugly. They tend to pay more for goods in town.
Half-Orcs do make good priests and rogues, for the simple reason
that Half-Orcs tend to have great constitutions and lots of
hit-points.

#### `Half-Troll`

Half-Trolls are incredibly strong, and have the highest hit points
of any character race. They are also very stupid and slow. They will
make great warriors and iffy priests. They are bad at searching,
disarming, perception, and stealth. They are so ugly that a Half-Orc
grimaces in their presence. They also happen to be fun to run...


#### 3.4.1 Race versus Skills and Stats

Stat, hit dice, and experience points per level modifications due to
race are listed in the following table.


                Str  Int  Wis  Dex  Con  Chr  Hit Dice  Rqd Exp/level

    Human        0    0    0    0    0    0      10          +0%
    Half-Elf    -1   +1    0   +1   -1   +1       9         +10%
    Elf         -1   +2   +1   +1   -2   +1       8         +20%
    Halfling    -2   +2   +1   +3   +1   +1       6         +10%
    Gnome       -1   +2    0   +2   +1   -2       7         +25%
    Dwarf       +2   -3   +1   -2   +2   -3       9         +20%
    Half-Orc    +2   -1    0    0   +1   -4      10         +10%
    Half-Troll  +4   -4   -2   -4   +3   -6      12         +20%


Racial abilities as compared to each other, with 1 the lowest, or worst,
and 10 the highest, or best, are listed in the following table.


               Disarm Search Stealth Percep Fight Bows Save Infra

    Human         5      5      5       5     5     5    5  None
    Half-Elf      6      7      7       6     4     6    6  20 feet
    Elf           8      9      7       7     3     9    7  30 feet
    Halfling     10     10     10      10     1    10   10  40 feet
    Gnome         9      7      9       9     2     8    9  30 feet
    Dwarf         6      8      3       5     9     5    8  50 feet
    Half-Orc      3      5      3       2     8     3    3  30 feet
    Half-Troll    1      1      1       1    10     1    1  30 feet



### 3.5 Choosing A Class

Once a race has been chosen, the player will need to pick a class. Some
classes will not be available to certain races, for instance, a
Half-Troll cannot become a Paladin. Players who are new to the game
should run a warrior or rogue. Spell casting generally requires a more
experienced player that is familiar with survival techniques.

#### `Warrior`

Warriors are hack-and-slash characters, who solve most of their
problems by cutting them to pieces, but occasionally falls back on
the help of magical devices. Their prime stats are Strength and
Constitution, and a good Dexterity can really help at times. A
Warrior will be good at Fighting and Throwing/Bows, but bad at most
other skills.

#### `Mage`

Mages must live by their wits. They cannot hope to simply hack their
way through the dungeon, and so must therefore use their magic to
defeat, deceive, confuse, and escape. Mages are not really complete
without an assortment of magical devices to use in addition to their
spells. They can master the higher level magical devices far easier
than anyone else, and has the best saving throw to resist effects of
spells cast at them. Intelligence and Dexterity are their primary
stats. There is no rule that says a mages cannot become good
fighters, but spells are their true realm.

#### `Priest`

Priests are characters of holy devotion. They explore the dungeon
only to destroy the evil that lurks within, and if treasure just
happens to fall into their pack, well, so much more to the glory of
their temple! Priests receive their spells from a deity, and
therefore do not choose which spells are learned. They are familiar
with magical devices, preferring to call them instruments of god,
but is not as good as a mage in their use. Priests have good saving
throws, and make decent fighters, preferring blunt weapons over
edged ones. Wisdom and Charisma are the priest’s primary stats.

#### `Rogue`

Rogues are characters that prefer to live by their cunning, but is
capable of fighting their way out of a tight spot. They are the
master of traps and locks, no device being impossible to overcome. A
rogue has a high stealth allowing them to sneak around many
creatures without having to fight, or sneak up and get the first
blow. A rogue’s perception is higher than any other class, and many
times they will notice a trap or secret door before having to
search. A rogue is better than a warrior or paladin with magical
devices, but still cannot rely on their performance. Rogues can also
learn a few spells, but not the powerful offensive spells mages can
use. A rogue’s primary stats are Intelligence and Dexterity.

#### `Ranger`

A Ranger is a warrior/mage. They are good fighters, and the best of
the classes with a missile weapon such as a bow. Rangers learn
spells much more slowly than a mage, but is capable of learning all
but the most powerful spell. Because rangers are really a dual class
character, more experience is required for them to advance. Rangers
have good stealth, good perception, good searching, good saving
throw, and is good with magical devices. Their primary stats are
Intelligence and Dexterity.

#### `Paladin`

A Paladin is a warrior/priest. They are very good fighters, second
only to the warrior class, but are not very good at missile weapons.
They receive prayers at a slower pace then the priest, but can
eventually learn all the prayers. Because paladins are dual class
characters, it requires more experience to advance them. A paladin
lacks much in the way of abilities. They are poor at stealth,
perception, searching, and magical devices. They have a decent
saving throw due to their divine alliance. A paladin’s primary stats
are Strength and Charisma.


#### 3.5.1 Race Versus Class


                Warrior   Mage    Priest    Rogue   Ranger   Paladin

    Human         Yes      Yes      Yes      Yes      Yes      Yes
    Half-Elf      Yes      Yes      Yes      Yes      Yes      Yes
    Elf           Yes      Yes      Yes      Yes      Yes      No
    Halfling      Yes      Yes      No       Yes      No       No
    Gnome         Yes      Yes      Yes      Yes      No       No
    Dwarf         Yes      No       Yes      No       No       No
    Half-Orc      Yes      No       Yes      Yes      No       No
    Half-Troll    Yes      No       Yes      No       No       No



#### 3.5.2 Class Versus Skills

Class abilities as compared to each other, with 1 as the lowest, or
worst, and 10 as the highest, or best are shown in the following table.


                       Save  Stea-        Magic                 Extra
            Fight Bows Throw  lth  Disarm Device Percep Search Exp/lev

    Warrior  10     6    3     2      4      3      2      2       +0%
    Mage      2     1   10     5      6     10      8      5      +30%
    Priest    4     3    6     5      3      8      4      4      +20%
    Rogue     8     9    7    10     10      6     10     10       +0%
    Ranger    6    10    8     7      6      7      6      6      +40%
    Paladin   9     5    4     2      2      4      2      2      +35%



### 3.6 Experience

All characters receive experience during the game. Experience determines
the level, which determines hit-points, mana points, spells, abilities,
etc. The amount of experience required to advance a level increases as
the game is played, according to race and class.


#### 3.6.1 Getting Experience

There are many ways to gain experience. This list shows a few.

-   Defeating monsters
-   Disarming traps
-   Picking locks
-   Using a scroll, potion, staff, or wand, for the first time and
    discovering what it did
-   Casting a spell successfully for the first time
-   Drinking a potion of gain experience


## 4. Adventuring

After the character has been created, their dungeon adventure can begin.
Symbols appearing on the screen represent the dungeon walls, floor,
objects, features, and creatures lurking about. In order to direct the
character through their adventure, the player will enter single
character commands.

*Moria* symbols and commands each have a help section devoted to them.
The player should review these sections before attempting an adventure.
Finally, a description of the town level and some general help on
adventuring are included.


## 5. Symbols On The Screen

Symbols on the map can be broken down into three categories:

1.  Features of the dungeon such as walls, floor, doors, and traps.
2.  Objects which can be picked up such as treasure, weapons, armor,
    magical devices, etc.
3.  Creatures which may or may not move about the dungeon, but are
    mostly harmful to the character’s well-being.

Some symbols can be in more than one category. Also note that treasure
may be embedded in a wall, and the wall must be removed before the
treasure can be picked up.

It will not be necessary to remember all of the symbols and their
meanings. A simple command, the `/`, will identify any symbol appearing
on the map. See the section on commands for further help.

### Features


    .   A floor space, or hidden trap   1   Entrance to General Store
    #   A wall                          2   Entrance to Armory
    '   An open door                    3   Entrance to Weapon Smith
    +   A closed door                   4   Entrance to Temple
    ^   A trap                          5   Entrance to Alchemy Shop
    <   A staircase up                  6   Entrance to Magic Shop
    >   A staircase down                :   Obstructing rubble
    ;   A loose floor stone                 An open pit (Blank)
    %   A mineral vein                  @   The character



### Objects


    !   A flask or potion         ?   A scroll
    "   An amulet                 [   Hard armor
    $   Money (Can be embedded)   \   A hafted weapon
    &   A chest                   ]   Misc. armor
    (   Soft armor                _   A staff
    )   A shield                  {   Missile (arrow, bolt, pebble)
    *   Gems (Can be embedded)    |   Sword or dagger
    -   A wand                    }   Missile arm (Bow, X-bow, sling)
    /   A pole-arm                ~   Misc
    =   A ring                    ,   Food
    s   A skeleton



### Creatures


    a   Giant Ant           A   Giant Ant Lion
    b   Giant Bat           B   The Balrog
    c   Giant Centipede     C   Gelatinous Cube
    d   Dragon              D   Ancient Dragon
    e   Floating Eye        E   Elemental
    f   Giant Frog          F   Fly
    g   Golem               G   Ghost
    h   Harpy               H   Hobgoblin
    i   Icky-Thing          I
    j   Jackal              J   Jelly
    k   Kobold              K   Killer Beetle
    l   Giant Louse         L   Lich
    m   Mold                M   Mummy
    n   Naga                N
    o   Orc or Ogre         O   Ooze
    p   Human(oid)          P   Giant Human(oid)
    q   Quasit              Q   Quylthulg
    r   Rodent              R   Reptile
    s   Skeleton            S   Scorpion
    t   Giant Tick          T   Troll
    u                       U   Umber Hulk
    v                       V   Vampire
    w   Worm or Worm Mass   W   Wight or Wraith
    x                       X   Xorn
    y   Yeek                Y   Yeti
    z   Zombie              Z
    $   Creeping Coins      ,   Mushroom Patch



Here is what the screen looks like after a character has been
adventuring in *moria* for a while:


                                     #            ' #
    Elf                              # ############'#
    Mage                             # #          # #
    Mage (2nd)                       # #
                                     # #
    STR :      9                     # #
    INT :  18/78                     # #
    WIS :     13                     # #
    DEX :     16                     # #
    CON :      8                     # #
    CHR :     15                     # #
                             ####+#### #
    LEV :     27             #........ #
    EXP :  38487             #.......###
    MANA:  66/66             #....@..#
    HP  :123/123             #........
                             #.......#
                             #.......#
    AC  :     55             ####.#'##
    GOLD:  23868


                                                               Study 1000 feet


The top line is where messages appear about what’s happening in the
game. The bottom line is a status line, that shows what dungeon depth
the character is at, and other things like if the character is hungry,
afraid, poisoned, or moving especially slow or fast. On the left side of
the screen the character’s stats can be seen. Everything else, is the
map of the dungeon.

This picture shows an elf mage character (`@`) in a lit room at 1000
feet below the surface. The walls of the room (`#`) can be seen, and the
empty lit tiles within it (`.`). The room has one closed door (`+`), and
one open door (`'`), and three other ways to leave. The character has
arrived to this room through a tunnel or hall coming from the top of the
screen. The tunnel is not lit like the room is. The tunnel will be lit
by the character’s light source as it travels through. There are no
monsters visible.

From the statistics on the left side of the screen, it can be seen that
the character is a `Mage (2nd)`, which is another way of saying that it
is at level 27. The reason that this character is at level 27 (`LEV`) is
because it has 38487 experience points (`EXP`).

The character has 123 of 123 hit-points available. `HP` shows the
current hit points and then the maximum hit-points. This spell-caster
has exactly 66 `MANA` (spell-casting) points to spend as spells are cast.
It can also be seen that this mage has some spells to learn. The word
`Study` in the bottom right shows this.

The character’s current levels of strength, intelligence, wisdom,
dexterity, constitution and charisma can also be seen. The character
doesn't have 18 of 78 intelligence, they have 18 78/100 intelligence,
which is more than just 18.

Finally, the character’s armor class (`AC`) is currently at 55, and has a
great number of gold pieces.


## 6. Commands

All commands are entered by pressing a single key. Some commands are
capital or control characters, which require the player to hold down the
shift or control key while pressing another key. As a special feature,
control keys may be entered in a single keystroke, or in two keystrokes,
with a `^` character first.

There are two command sets: the original command set which is the default
(e.g. the keypad based command set), and the roguelike command set. The
roguelike command is generally more convenient, especially if the keyboard
lacks a keypad.

The following tables summarize the two command sets. Certain commands
may be preceded by an optional count, and certain commands must be
followed by a direction. These conditions are indicated in the tables by
`@` for an optional count, and `~` for a direction. If a particular
command requires additional keystrokes, then they will be prompted for.

Original command summary.

    --------- ----------------------------- ------------ -------------------------
        a      Aim and fire a wand            @ B ~       Bash (object/creature)
        b      Browse a book                    C         Change name
        c ~    Close a door                   @ D ~       Disarm a trap/chest
        d      Drop an item                     E         Eat some food
        e      Equipment list                   F         Fill lap with oil
        f      Fire/Throw an item               G         Gain new magic spells
        i      Inventory list                   L         Locate with map
      @ j ~    Jam a door with spike            M         Map shown reduced size
        l ~    Look given direction           @ R         Rest for a period
        m      Magic spell casting              S         Search Mode
      @ o ~    Open a door/chest              @ T ~       Tunnel in a direction
        p      Pray                             V         View scoreboard
        q      Quaff a potion                   =         Set options
        r      Read a scroll                    ?         Command quick reference
      @ s      Search for trap or door          {         Inscribe an object
        t      Take off an item               @ - ~       Move without pickup
        u      Use a staff                      . ~       Run in direction
        v      Version, credits and manual      /         Identify a character
        w      Wear/Wield an item               CTRL-K    Quit the game
        x      Exchange weapon                @ CTRL-P    Repeat the last message
        <      Go up an up staircase            CTRL-X    Save character and quit
        >      Go down a down staircase       @ ~         for movement
    --------- ----------------------------- ------------ -------------------------

Roguelike command summary.

    ------------ ------------------------- ------------ -----------------------
        c ~       Close a door                 C         Character description
        d         Drop an item               @ D ~       Disarm a trap/chest
        e         Equipment list               E         Eat some food
      @ f ~       Force/bash item/monster      F         Fill lamp with oil
        i         Inventory list               G         Gain new magic spells
      @ o ~       Open a door/chest            P         Peruse a book
        p         Pray                         Q         Quit the game
        q         Quaff a potion             @ R         Rest for a period
        r         Read a scroll              @ S ~       Spike a door
      @ s         Search for trap or door      T         Take off an item
        t         Throw an item                V         View scores
        v         Version, and manual          W         Where: locate self
        w         Wear/Wield an item           X         Exchange weapon
        x ~       Examine surroundings         Z         Zap a staff
        z         Zap a wand                   #         Search Mode
        =         Set options                  <         Go up an up staircase
        /         Identify a character         >         Go down a down stair
      @ CTRL-P    Previous message review      {         Inscribe an object
      @ - ~       Move without pickup          ?         Type this page
      @ CTRL ~    Tunnel in a direction        CTRL-X    Save game and exit
      @ SHFT ~    Run in direction           @ ~         for movement
    ------------ ------------------------- ------------ -----------------------


### 6.1 Special Keys

Certain commands may be entered at any time input is accepted. The
special character `CTRL+R`, entered as a single keystroke, will always
refresh the screen. This may be used at any prompt for input, and is
otherwise ignored.

When playing on a UNIX or similar system, then there are some additional
special characters used by *moria*. The special character `CTRL+C` will
interrupt *moria*, and asks the player if they really want the character
to die and quit the game. Should the player choose for the character not
to die, *moria* merely continues as before, except that resting,
running, repeated commands, etc will be terminated. The game can be
suspended with `CTRL+Z`, and return to the original command shell. In this
case, *moria* is not terminated, and may be restarted at any time from
the shell. Alternatively, the special command `!` is available to run
any normal shell command. When it is complete, *moria* will restart.

For many input requests or queries, the special character `ESCAPE` will
abort the command. For the "`-more-`" message prompts, any of `SPACE`,
`ESCAPE`, `RETURN` (`CTRL+Shift+M`), or `LINEFEED` (`CTRL+J`) can be used to
continue after pausing to read the displayed message.

It is possible to give control character commands in two keystrokes, by
pressing a `^` followed by the appropriate letter of the alphabet. This
is useful when running *moria* in circumstances where control characters
are intercepted by some external process, or by the operating system.


### 6.2 Direction

For the original style command set, a direction is given by a digit
which is in the appropriate orientation on the keypad. For the
roguelike command set, a direction is given by one of the letters
`hykulnjb`. Again, the relative position of the keys on the keyboard
gives a clue as to the direction. The digit `5` for the original
commands, and the period `.` for roguelike commands, is a null
direction indicator. This means to stay in one place, or when in a look
command to look in all directions.

    +---------------------------+---------------------------+
    |                           |                           |
    |    Original Directions    |   Roguelike Directions    |
    |                           |                           |
    |       \     |     /       |       \     |     /       |
    |        7    8    9        |        y    k    u        |
    |                           |                           |
    |     -  4         6  -     |     -  h         l  -     |
    |                           |                           |
    |        1    2    3        |        b    j    n        |
    |       /     |     \       |       /     |     \       |
    |                           |                           |
    +---------------------------+---------------------------+

Movement is accomplished by specifying a direction immediately. Simply
press the appropriate key and the character will move one step in that
direction. The character can only move onto and through floor spots, and
only if they contain no creatures or obstructing objects such as a
closed door.

Other commands that require a direction will prompt for it.

Moving the character one step at a time can be time consuming and
boring, so a faster method has been supplied. For the original keypad based
command set, by using the Run command `.`, the character may move in a
direction until something interesting happens. For instance, by pressing
the period key `.` followed by the direction 8, the character would
continue to move up the screen, only coming to a stop after at least one
condition is satisfied. For the roguelike command set, typing a shifted
directional letter will move the character in that direction until
something interesting happens. The stopping conditions are described
more completely in the run command description below.


### 6.3 Command Counts

Some commands can be executed a fixed number of times by preceding them
with a count. Counted commands will execute until the count expires, or
until any key is pressed, or until something significant happens, such
as being attacked. Thus, a counted command doesn't work to attack
another creature. While the command is being repeated, the number of
times left to be repeated will flash by on the command line at the
bottom of the screen.

To give a count to a command in the roguelike mode, type the number in
digits, then the command. A count of zero defaults to a count of 99.

To give a count to a command in the original mode, press `#`, followed
by the digits. To count a movement command (which is itself a digit),
type a space after the number, and then give the command.

Counted commands are very useful for searching or tunneling, as they
automatically terminate on success, or if the character is attacked. A
counted command or a Run can be terminated a counted command by pressing
any key. This character is ignored, but it is safest to use a `SPACE` or
`ESCAPE` which are always ignored as commands.


### 6.4 Selection of objects

Many commands will also prompt for a particular object to be used. For
example, the command to read a scroll will ask which of the scrolls that
the character is carrying is to be read. In such cases, the selection is
made by pressing a key for that letter of the alphabet; when selecting
from the backpack, the player may also type a digit to select the item
whose inscription is that digit. The prompt will indicate the possible
letters, and will also allow the `*` key to be pressed, which causes all
of the available options to be described (e.g. it lists all of the
scrolls that can be read).

The particular object may be selected by an upper case or a lower case
letter. If lower case or a digit is used, the selection takes place
immediately. If upper case is used, then the particular option is
described, and you are given the option of confirming or retracting that
choice. Upper case selection is thus safer, but requires an extra
keystroke.


### 6.5 Command descriptions

In the following command descriptions, the original style key is given.
If the roguelike key for that command is different, then it will be
shown following the original key.


#### `B <Dir> (f <Dir>) - Bash.`

The bash command includes breaking open doors and chests, or bashing
an opponent. The bashing ability increases with weight and strength.
In addition, when bashing an opponent, it will either perform a body
bash, or, when wielding a shield, perform a shield bash which is
more effective.

Bashing a door can throw the character off-balance, but this will
not generally be a problem. Doors that have been jammed closed with
spikes can only be opened by bashing. Locked doors may also be
bashed open. Bashing a door open will permanently break it.

Bashing a creature affects both the character and the opponent.
Depending on dexterity, the character may or may not be thrown
off-balance allowing free moves to the opponent. If the bash is
successful, the opponent may be thrown off-balance, thus giving the
character some free hits or a chance to flee. Huge creatures such as
ancient dragons will be difficult or even impossible to bash
successfully.

A character automatically performs a shield bash instead of a body
bash, when they are currently wearing a shield. A shield bash adds
the damage of a shield to that of the bash, so it is more effective.
Size and material both affect the damage that a shield will do.

Bashing can be done with a command-count, but if the character is
town off-balance, the count will be reset straight away.


#### `C - Display character (on screen or saving to a file.)`

This command allows the player to either display the character
details on the terminal screen, or to save an entire character info
listing to a file. The character’s history, equipment, and inventory
list are also included when saving to a file.


#### `D <Dir> - Disarm a trap.`

The character can attempt to disarm floor traps, or trapped chests.
When the character tries and fails to disarm a trap, there is a
chance that the blunder will set it off. Traps on chests can only be
disarmed after firstly finding the trap with the search command.
This command can have a count.


#### `E - Eat some food.`

A character must eat occasionally to remain effective. As a
character grows hungry, a message will appear at the bottom of the
screen saying "`Hungry`". When a character remains hungry for too
long, they will become weak, and eventually start fainting, and
finally die of starvation.


#### `F - Fill a lamp or lantern with oil.`

When the character is using a lamp for a light source, and has a
flask of oil in the pack, they may refill the lamp by using this
command. A lamp is capable of a maximum of 15000 turns of light, and
each flask has 7500 turns of oil contained in it.


#### `G - Gain new spells.`

This command causes the character to learn new spells. When the
character is able to learn some spells, the word "`Study`" appears
on the status line at the bottom of the screen. Mages, rogues, and
rangers must have the magic books containing new spells to be able
to learn them. Priests and Paladins are given their prayers by their
gods, and hence do not need a holy book before learning the prayers
in it. They do need the book in order to use the prayers.


#### `L (W) - Location on map.`

The location command allows the player to look at all parts of the
current dungeon level. The displayed view of the dungeon is shifted
to bring the character’s current position as close to the center as
possible. The map can then be shifted using any of the eight
possible directions. Each shift moves the view point by one half
screen. The top line displays a map section number, each map section
having a height and width one half that of the display, and
indicates the direction of the display from the character’s current
position. When this command is exited and the character is not on
the display, then the display is centered again.


#### `M - Map shown reduced size.`

This command will show the entire map, reduced by a factor of nine,
on the screen. Since nine places map into every character on the
screen, only the major dungeon features will be visible. This is
especially useful for finding where the stairs are in relation to
the character’s current position. It is also useful for identifying
unexplored areas.


#### `R - Rest for a number of turns.`

The character may rest one turn with the null movement command.
Resting for longer periods of time is accomplished by using the Rest
command, followed by the number of turns to rest. Resting will
continue until the specified duration has expired, or something to
wake the character happens, such as a creature wandering by, or
getting hungry, or some disability like blindness expiring. It is
sometimes a good idea to rest a beat-up character until they regain
some of their hit-points, but be sure to have plenty of food if you
rest often.

The character can be awakened by pressing any key. Space is best,
since if the rest ends just before the character is typed, the space
is ignored as a command.

It is also possible to rest by typing the count first, and using
either the Rest or the null movement command.

When `*` is given for the rest count, the character will rest until
both hit-points and mana reach their maximum values. As above, the
character will immediately be awakened if anything interesting
happens.


#### `S (#) - Search mode toggle.`

The Searching toggle will take the character into and out of search
mode. When first pressed, the message "`Searching`" will appear on
the status line at the bottom of the screen. The character is now
taking two turns for each command, one for the command and one turn
to search. This means that the character is taking twice the time to
move about the dungeon, and therefore twice the food. If a creature
should happen by or attack, search mode will automatically toggled
off. Search mode can also be turned off by again pressing the `S`
(or `#`) key.


#### `T <Dir> (CTRL+<Dir>) - Tunnel through rock.`

Tunneling (Mining) is a very useful art in the dungeons of *moria*.
There are four kinds of rock:

  1.  Permanent Rock
  2.  Granite Rock
  3.  Magma Intrusion
  4.  Quartz Veins

Permanent Rock is exactly that, permanent. Granite is very hard,
therefore hard to dig through, and contains no valuable metals.
Magma and Quartz veins are softer and sometimes bear valuable metals
and gems, shown as a `$` or a `*` symbol. When the character can't
move over these symbols, it means they are embedded in the rock, and
the tunnel command must be used to dig them out. There is a game
option which causes magma and quartz to be displayed differently
than other rock types.

Tunneling can be **VERY** difficult by hand, so when digging be sure to
wield either a shovel or a pick. Magical shovels and picks can be
found which allow the wielder to dig much faster than normal, and a
good strength also helps.

Tunneling can have a count.


#### `V - View scoreboard.`

This command will display the contents of the score board on the
screen. On a multi-user system, pressing `V` the first time will show
only those scores from the score board that belong to the current
user, and pressing `V` again will show all users’ scores.


#### `a <Dir> (z <Dir>) - Aim a wand.`

Wands must be aimed in a direction to be used. Wands are magical
devices and therefore use the Magical Devices ability of the
character. They will either affect the first object/creature
encountered, or affect anything in a given direction, depending upon
the wand. An obstruction such as a door or wall will generally stop
the effects of a wand from traveling further.


#### `b (P) - Browse a book.`

Books can only be read if the character is of its realm. Therefore a
magic user could read a magic book, but not a holy book. Warriors
will not be able to read either kind of book. When the browse
command is used, all of the spells or prayers contained therein are
displayed, along with information such as their level, the amount of
mana used up in casting them, and whether or not you know the spell
or prayer. There are a total of 31 different magical spells in four
books, and 31 different prayers in four books.


#### `c <Dir> - Close a door.`

Non-intelligent and certain other creatures will not be able to open
a door. Therefore shutting doors can be a life saver. To close a
door, the character must be adjacent to an open door, and broken
doors cannot be closed. Bashing a closed door will break it and
leave it permanently open.


#### `d - Drop an object from the inventory.`

Objects can be dropped onto the floor beneath the character if that
floor spot does not already contain an object. Doors and traps are
considered objects in this sense. If the character has several
objects of the same kind, the player will be prompted for dropping
one or all of them. It is possible to directly drop things which you
are wielding or wearing.


#### `e - Display a list of equipment being used.`

Use the Equipment command to display a list of objects currently
being worn or wielded by the character. Each object has a specific
place where it is placed, and that only one object of each type may
be used at any one time, excepting rings of which two can be worn,
one on each hand.


#### `f <Dir> (t <Dir>) - Fire/Throw an object/use a missile weapon.`

Any object carried by the character can be thrown. Depending upon
the weight of an object, it may travel across a room or drop down
beside the character. When a stackable object like an arrow is
thrown, only one will be thrown at a time.

When throwing at a creature, the chance of hitting the creature is
determined by the character’s pluses to hit, the ability at
throwing, and the object’s pluses to hit. Once the creature is hit,
the object may or may not do any actual damage to it. Certain
objects in the dungeon can do great amounts of damage when thrown,
but it’s for the player to figure out the obscure ones. Oil flasks
are considered to be lit before thrown; therefore, they will do fire
damage to a creature if they hit it.

To use a bow with arrows, simply wield the bow and throw the arrows.
Extra pluses to damage and hitting are gained by wielding the proper
weapon and throwing the corresponding ammo. A heavy crossbow with
bolts for example, is a killer...


#### `i - Display a list of objects being carried.`

This command displays an inventory of all objects being carried, but
not currently in use (e.g. being worn or wielded). The character may
carry up to 22 different kinds of objects in the pack. Depending
upon strength, the character will be able carry many identical
objects before hitting the weight limit. After the weight limit is
exceeded the character will move slower due to the encumbrance.


#### `j <Dir> (S <Dir>) - Jam a door with an iron spike.`

Most humanoid and many intelligent creatures can simply open a
closed door, and can eventually get through a locked door. Therefore
the character may also spike a door in order to jam it. Each spike
used on a door will increase its strength, although as more spikes
are jammed, the less effect each additional spike has. It is very
easy to jam a door so much as to make it impossible for the
character to bash it down, so spike doors wisely. The bigger a
creature is, the easier it can bash a door down. Therefore twenty or
more spikes might be necessary to slow down a dragon, where one
spike would slow down a Kobold. This command can be counted.


#### `l <Dir> (x <Dir>) - Look in a direction.`

The Look command is useful in identifying the exact type of object
or creature shown on the screen. Also, if a creature is on top of an
object, the look command will describe both. Creatures and objects
can be seen up to 200 feet away (20 spaces or tiles). The Look
command can be used freely without the creatures getting a turn to
move against the character.

Looking in a particular direction sees everything within a cone of
vision which just overlaps the cones of the two adjacent directions.
Looking with the null direction `5` (or `.`) sees everything which
there is to be seen.

The character is able to access monster memories with this command.
When the character sees a creature, the player has the option to
view a short paragraph of information about prior experiences with
that creature. See also the section on being attacked.


#### `m - Cast a magic spell.`

To cast a spell, a character must have previously learned it, and
must also have in the inventory a magical book from which the spell
may be read. Each spell has a chance of failure which starts out
fairly large but decreases as a character gains levels. If a
character does not have enough mana, the chance of failure is
greatly increased, and gambles on losing a point of constitution.
The player will be prompted for confirmation before trying to cast a
spell when there is insufficient mana. Since a character must read
the spell from a book, they cannot be blind or confused when casting
a spell, and there must be some light present.


#### `o <Dir> - Open a door, chest, or lock.`

To open an object such as a door or chest, the Open command can be
used. If the object is locked, the Open command will attempt to pick
the lock, based on the character’s disarm ability. If an object is
trapped and the character opens it, the trap will be set off. This
command can be counted, because the locked object may require
several tries to open.


#### `p - Read a prayer.`

To pray effectively, a character must have learned the prayer, and
must also have in the inventory a holy book from which the prayer
may be read. Each prayer has a chance of being ignored which starts
out fairly large but decreases as a character gains levels. If a
character does not have enough mana, the chance of failure is
greatly increased, and gambles on losing a point of constitution.
The player will be prompted for confirmation before trying to pray
when there is insufficient mana. Since a character must read the
prayer from a book, they cannot be blind or confused when praying,
and there must be some light present.


#### `q - Quaff a potion.`

To drink a potion use the Quaff command. A potion affects the
character in some manner. The effects of the potion may be
immediately noticed, or they may be subtle and unnoticed.


#### `r - Read a scroll.`

To read a scroll use the Read command. Most scroll spells either
affect the player or the area around the character; a few cases such
as identify scrolls act on other objects. Two scrolls, the identify
scroll and the recharge scroll, have titles which can be read
without setting them off, and by pressing ESCAPE can be saved for
future use.


#### `s - Search general area one turn.`

The Search command can be used to locate hidden traps and secret
doors about the character. More than a single turn of searching will
be required in most cases. The character should always search a
chest before trying to open it because they are generally trapped.
This command can be counted, and the counted search ends as soon as
anything is found.


#### `t (T) - Take off a piece of equipment.`

Use the Take Off command to remove an object from use, and return it
to the character’s pack, or inventory. Occasionally the character
will have a cursed item which cannot be removed. Cursed items are
always bad, and can only be taken off after removing the curse.


#### `u (Z) - Use/Zap a staff.`

The Use command will activate a staff. Like scrolls, most staves
have an area effect. Because staves are generally more powerful than
most other items, they are also harder to use correctly.


#### `v - Display current version of game.`

The Version command displays the credits for the current version of
*moria*.


#### `w - Wear or wield an item being carried.`

To wear or wield an object in the character’s inventory, use the
Wear/Wield command. If another object is already in use for the same
function, it is automatically removed first; if the character is
wearing two rings, the player is given a choice of which one to
remove. An object’s bonuses cannot be gained until it is worn or
wielded.


#### `x (X) - Exchange primary and secondary weapons.`

A secondary weapon is any weapon which may be needed often. Instead
of searching through the inventory, the character may use the
exchange command to keep the weapon ready (e.g. slung over the
character’s back). For instance, if the bow was going to be used
primarily, but needed a sword for close combat, the character could
wield the sword, use the exchange command to make it the secondary
weapon, then wield the bow. If the sword was suddenly needed, simply
use the exchange command to switch between the bow and the sword.


#### `/ - Identify a character shown on screen.`

Use the identify command to find out what a symbol displayed on the
screen stands for. For instance, by pressing `/.`, the player can
find out that the `.` stands for a floor spot, or tile. When used
with a creature, the identify command will only tell what class of
creature the symbol stands for, not the specific creature;
therefore, use the look command for this information.

When identifying a symbol for a creature in the character’s monster
memory, the player has the option to view a paragraph of information
on those creatures identified by the given character. Several
creatures may be identified in this way. Typing ESCAPE after the
paragraph for any creature will abort back to command level. See
also the section on being attacked.


#### `? - Display a list of commands.`

The `?` command displays a quick reference help page on the screen.


#### `- - Move without pickup.`

This is followed by a move command, and causes the character to move
over an object without automatically picking it up. This command can
be counted.


#### `= - Set options.`

This is a free move, to set various game options in *moria*. The
available options are:

1.  Cut known corners when running. This is on by default, and the
    only reason for switching it off would be if the character had
    the search mode on and wished to look for secret doors in the
    extremity of every corner.
2.  Examine potential corners when running. This is on by default,
    and allows the character to run along an unknown curving
    corridor. If however, the character is running from a creature,
    the player may wish to switch this option off, because the
    creature will cut the corner.
3.  Display self during a run. This is off by default, which gives
    faster screen updating.
4.  Stop when map sector changes. This is off by default, but can be
    switched on to stop running whenever a new part of the dungeon
    appears in view.
5.  Treat open doors as empty space while running. This is off by
    default, in which case the character stops whenever they run up
    to an open door.
6.  Prompt to pick up objects. This is off by default, in which case
    stepping over an object automatically causes the character to
    pick it up. With the option on, the player gets prompted in all
    such cases with a description of the object to see if it should
    be put into the character’s pack.
7.  Roguelike command set. This option controls the command set in
    use. It is off by default.
8.  Show weights in inventory. This is off by default: switching it
    on causes the inventory and equipment listings to include the
    weight of all objects. This may be useful to know if the
    character’s pack is getting too heavy.
9.  Highlight and notice mineral seams. This is off by default.
    Switching it on causes quartz and magma to be displayed as `%`
    instead of `#`; also, it causes the look command to treat them
    as interesting objects. This is handy when mining. Setting this
    option does not immediately highlight all minerals, but only
    those which are subsequently displayed. To display all minerals,
    just move the map around a bit with the *Where* (or *Locate*)
    command.
10. Beep for invalid character. This is on by default. When this
    option is on, the program will beep for most invalid characters,
    such as trying to choose a spell that hasn't been learned yet.
    When off, there are no such beeps.
11. Display rest/repeat counts. This is on by default. When on, the
    program will progressively display the remaining turns left
    while resting, and for repeated commands. For those trying to
    play over a very slow connection, or for those playing on very
    slow computers, turning this off will make resting and repeated
    commands work much faster.
12. Disable haggling. This is off by default. When on, all haggling
    will be disabled at the cost of a 10 percent tax on the final
    price on items you would otherwise have had to haggle for, and
    all prices displayed in the stores will be the actual prices you
    have to pay.

The setting of all these options persist in the save file, even after
the character dies.


#### `^P - Previous message.`

The Control-P command will redisplay the last message displayed on
the message line at the top of the screen. A second such command
will display all of the saved messages. This command can be given a
count to specify the number of previous messages to View. At
present, only 22 messages are saved.


#### `^K (^Q) - Kill the character.`

To exit the game without saving the character (i.e. killing the
character) use the Control-K command. Once exited in this manner,
the character is nonrecoverable.


#### `^X - Save the character and exit the game.`

To save the game so that it can be restarted later, use the
Control-X command. Save files will also be generated if the game
crashes due to a system error. When the character dies, a reduced
save file is produced containing only the monster memory, and the
option settings.


#### `{ - Inscribe an object.`

This command can be used to inscribe any short string on an object.
Inscriptions are limited to twelve characters. The inscription
applies only to the particular object, it is not automatically
transferred to all similar objects. Under certain circumstances,
*moria* will itself inscribe objects: if they have been discovered
to be cursed or enchanted, or if they have been sampled without
being identified. In this last case, *moria* does in fact carefully
inscribe every such item.

If the inscription on an item is a single digit, that digit can be
used to refer to it when using, wearing, or wielding an item from
inside the pack. For example, if a shovel is kept in the pack with
the inscription `1`, you can switch to the shovel by wielding item
`1` without checking the full inventory list to find out which item
the shovel is.


#### `! - Shell out of game.`

Use the Shell command `!` to temporarily exit the game to execute
UNIX or DOS commands. The game can be reentered by typing exit to
end the spawned process.


#### `< - Go up an up staircase.`

If the character moves onto an up staircase the `<` command will
take the character to go up one level (e.g. one level of dungeon
depth). There is always one staircase going up on every level except
for the town level (this does not mean it’s easy to find). Going up
a staircase will always take the character to a new dungeon area
except for the town level, which remains the same for the duration
of the current character’s game.


#### `> - Go down a down staircase.`

If the character is on top of a down staircase the `>` command may
be used to go down one level of dungeon depth. There are always two
or three staircases going down on each level, except the town level
which has only one. Going down will always take the character to a
new dungeon area.


#### `. <Dir> (shift <Dir>) - Move in direction.`

The Run command will move the character in the indicated direction
until either a choice needs to be made between two directions, or
something interesting happens. There are options which determine
behavior at corners, and at screen boundaries. More precisely, the
conditions which stop a run are as follows:

1.  A creature appears on the screen, one already on the screen
    moves, or a creature attacks or casts a spell at the character.
2.  The character moves next to an object, or a feature such as a
    door or trap.
3.  The character comes to the end of open space, or the end of a
    passage, or a junction of passages, or a hole in a wall.
4.  Anything typed during a run causes the run to stop. The keyboard
    key causing this to occur is ignored. It is best to use a space,
    which is ignored as a command, just in case the run stops just
    before the key is pressed.
5.  Various changes of state, such as recovery from fear or loss of
    heroism, will stop a run.

Corners are more complex. A corner allows a choice between adjacent
rectangular and diagonal directions. If the character can see walls
which ensure that the diagonal gives a faster traversal, then action
is determined by the "cut corners" options. If it is set, then the
character moves diagonally through the corner. This gives maximum
speed (as is nice when fleeing a hidden creature). On the other
hand, this option should not be set if more careful coverage is
desired (as when searching) so that the character take two moves
through the corner.

At a potential corner, where walls are not yet visible ahead of the
rectangular direction, the "examine corners" option is considered.
If set, the character will move straight into the corner, which will
light up all the corner and so determine where to go from there.
This allows the character to follow corners in new passages. If the
option is not set, the character stops. This allows highly cautious
running where the character stops at all potential choice points.

If the character moves off the screen while running, then a new
section of the dungeon is displayed and the run continues. However,
if the "stop when map changes" option is set, the character will
stop. Again, this is an option for nervous players; after all, there
may be a dragon on the new screen, and running into a dragon can be
bad for the character’s health.


## 7. The Town Level

The town level is where the character will begin the adventure. The town
consists of six buildings, each with an entrance, some townspeople, and
a wall which surrounds the town. At the start of the game it will be
daytime in the town, but later on the character may return to find that
darkness has fallen. (Note that some spells may act differently in the
town level.)


### 7.1 Townspeople

The town contains many different kinds of people. There are the street
urchins, young children who will mob an adventurer for money, and seem
to come out of the woodwork when excited. Blubbering Idiots are a
constant annoyance, but not harmful. Public drunks wander about the town
singing, and are of no threat to anyone. Sneaky rogues hang about
watching for a likely victim to mug. And finally, what town would be
complete without a swarm of half-drunk warriors, who take offense or
become annoyed just for the fun of it.

Most of the townspeople should be avoided by the largest possible
distance when wandering from store to store. Fights will break out
though, so be prepared. Since the character grew up in this world of
intrigue, no experience is awarded for killing on the town level.


### 7.2 Supplies

The character will begin the adventure with some supplies already in the
backpack. Use the Inventory `i` command to check what these supplies
are. It will be necessary to buy other supplies before continuing into
the dungeon, however, so be sure to enter each of the stores.


### 7.3 Town Buildings

The character may enter any of the stores, if they are open, and barter
with the owner for items that can be afforded. When bartering, the
player enters prices you will pay (or accept) for some object. An
absolute amount can be entered, or an amount preceded with a plus or
minus sign to give a positive or negative increment on the previous
offer. When an increment has been previously given, pressing RETURN will
use the previous increment. But be warned that the owners can easily be
insulted, and may even throw the character out for a while if you insult
them too often. To enter a store, simply move onto the entrance
represented by the numbers `1` through `6`.

If the character consistently bargains well in a store, that is, you
reach the final offer much more often than not, then the store owner
will eventually recognize that the character is a superb haggler, and
will go directly to the final offer instead of haggling with you. Items
which cost less than 10 gold pieces do not count, as haggling well with
these items is usually either very easy or almost impossible.

Once inside a store, the store inventory will appear on the screen along
with a set of options for the character. The store’s inventory can be
browsed if there is more than one page to View. Items can be sold to,
or purchased from the character’s backpack. Regular inventory and
equipment commands can be executed in a store. Although the wear, take
off, and exchange commands do not appear in the options, they will still
work, but were excluded to keep the options simple.

Stores do not always have everything in stock. As the game progresses,
they may get new items, so check from time to time. Also, if you sell
them an item, it may get sold to a customer while you are adventuring,
so don't always expect to be able to get back everything you have sold.

Store owners will not buy harmful or useless items. If an object is
unidentified, they will pay some base price for it. Once they have
bought it they will immediately identify the object. If it is a good
object, they will add it to their inventory. If it was a bad bargain,
they simply throw the item away. In any case, the player may receive
some knowledge of the item if another is encountered.


#### `The General Store`

The General Store sells foods, drinks, some clothing, torches,
lamps, oil, shovels, picks, and spikes. All of these items, and some
others, can be sold back to the General store for money. The
entrance to the General Store is a `1`.


#### `The Armory`

The Armory is where the town’s armor is fashioned. All sorts of
protective gear may be bought and sold here. The entrance to the
Armory is a `2`.


#### `The Weaponsmith's Shop`

The Weaponsmith’s Shop is where the town’s weapons are fashioned.
Hand and missile weapons may be purchased and sold here, along with
arrows, bolts, and shots. The entrance to the Weaponsmith’s is a `3`.


#### `The Temple`

The Temple deals in healing and restoration potions, as well as
bless scrolls, word of recall scrolls, some approved priestly
weapons, etc. The entrance to the Temple is a `4`.


#### `The Alchemy Shop`

The Alchemy Shop deals in all manner of potions and scrolls. The
entrance to the Alchemy Shop is a `5`.


#### `The Magic User's Shop`

The Magic User’s Shop is the most expensive of all the stores. It
deals in all sorts of rings, wands, amulets, and staves. The
entrance to the Magic Shop is a `6`.


## 8. Within The Dungeon

Once the character is adequately supplied with food, light, armor, and
weapons, it is time to enter the dungeon. Move on top of the `>` symbol
and use the down `>` command. The character enters a maze of
interconnecting staircases and finally passes through a one-way door.
The character is now on the first level of the dungeon (`50 feet`), and
must survive many horrible and challenging encounters to find the
treasure lying about.

There are two sources for light once inside the dungeon: permanent light
which has been magically placed within rooms, and a light source carried
by the character. If neither is present, the character will be unable to
map or see any attackers. Lack of light will also affect searching,
picking locks, disarming, and casting spells.

A character must wield a torch or lamp in order to supply their own
light. Once a torch or lamp has only 50 or less turns left before
burning out, the message "`Your light is growing faint`" will be
displayed at random intervals. Once a torch is burnt out, it is useless
and can be dropped. A lamp or lantern can be refilled with oil by using
the Fill `F` command, however the character must be carrying extra oil
to refill a lantern.


## 9. Attacking and Being Attacked

Attacking is simple in *moria*. When moving into a creature, the
character attacks it. The character can attack from a distance by firing
a missile, or by magical means such as aiming a wand. Creatures attack
in the same way; if they move into the character, they attack. Some
creatures can also cast spells from a distance, and others can breathe
fire or worse from a distance.

Creatures moving in walls cannot be attacked by wands and other magic
attacks normally stopped by walls. The character can attack a creature
in a wall normally though by trying to move into the wall space
containing the creature. However, in order to attack an invisible
creature in a wall, the character must tunnel into the wall containing
the creature. Trying to move into the wall will bump the character’s
head, and they will look quite silly.

If the character is wielding a weapon, the damage for the weapon is used
when a creature is hit. Otherwise, the character gets two fist strikes.
Very strong creatures can do a lot of damage with their fists... The
character may have a primary weapon, and a secondary weapon which is
kept on the belt or shoulder for immediate use. Weapons can be switched
with the exchange command. Be sure to wield the proper weapon when
fighting. Hitting a dragon over the head with a bow will simply make it
mad, and get the character killed.

Missile weapons, such as bows, can be wielded, and then the proper
missile, in this case an arrow, can be fired across the room into a
target. Missiles can be used without the proper missile weapon, but used
together they have a greater range and do far more damage.

Hits and misses are determined by ability to hit versus armor class. A
hit is a strike that does some damage; a miss may in fact reach a
target, but fails to do any damage. Higher armor classes make it harder
to do damage, and so lead to more misses.

In *Moria*, your character can wear any armor or wield any weapon – it
is not limited by race or class.


### 9.1 Monster Memories

There are hundreds of different creatures in the mines of *moria*, many
of which look the same on the screen. The exact species of a creature
can be discovered by looking at it. It is also very difficult to keep
track of the capabilities of various creatures. Rather than forcing the
player to keep notes, *moria* automatically keeps track of prior
experiences with a particular creature. This is called the *monster
memory*. The monster memory recalls the particular attacks of each
creature which the character has suffered, as well as recalling if the
character has observed them to multiply or move erratically, or drop
treasure, or many other attributes.

If the character has killed enough of a particular creature, or suffered
enough attacks, recalling the monster memory may also provide
information not otherwise available, such as a armor class or hit dice.
These are not explained, but may be useful to give the relative danger
of each creature. This memory can be passed on to a new character even
after the character dies, by means of a reduced save file.


### 9.2 The Character’s Weapon

Carrying a weapon in a backpack doesn't do much good. The character must
wield a weapon before it can be used in a fight. A secondary weapon can
be kept by wielding it and then using the exchange command. A secondary
weapon is not in use, simply ready to be switched with the current
weapon if needed.

Weapons have two main characteristics, their ability to hit and their
ability to do damage, expressed as `(+#,+#)`. A normal weapon would be
`(+0,+0)`. Many weapons in *moria* have magical bonuses to hit and/or do
damage. Some weapons are cursed, and have penalties that hurt the
character. Cursed weapons cannot be taken off until the curse is lifted.

*Moria* assumes that the youth of the character spent in the rough
environment near the dungeons has taught the relative merits of
different weapons, and displays as part of their description the damage
dice which define their capabilities. The ability to damage is added to
the dice roll for that weapon. The dice used for a given weapon is
displayed as `#d#`. The first number indicates how many dice to roll,
and the second indicates how many sides they have. A `2d6` weapon will
give damage from 2 to 12, plus any damage bonus. The weight of a weapon
is also a consideration. Heavy weapons may hit harder, but they are also
harder to use. Depending on the strength stat and the weight of the
weapon, the character may get several hits in a single turn.

Missile booster weapons, such as bows, have their characteristics added
to those of the missile used, if the proper weapon/missile combination
is used. Also, these weapons will multiply the base damage of the
missile by a number from 2 to 4, depending on the strength of the
weapon. This multiplier is displayed as `(x#)`.

Although the character receives any magical bonuses an unidentified
weapon may possess when you wield it, those bonuses will not be added in
to the displayed values of to-hit and to-dam on the character sheet.
Weapons must be identified before the displayed values reflect the real
values used.

Finally, some rare weapons have special abilities. These are called ego
weapons, and are feared by great and meek. An ego sword must be wielded
to receive benefit of its abilities.

Special weapons are denoted by the following abbreviations:


#### `DF - Defender.`

A magical weapon that helps wielders defend themselves, thus
increasing their armor class, and protecting them against damage
from fire, frost, acid, lightning, and falls. This weapon also will
increase stealth, let the character see invisible creatures, protect
from paralyzation attacks, and help regenerate hit-points and mana
faster. As a result of the regeneration ability, the character will
use up food faster than normal while wielding such a weapon.


#### `FB - Frost Brand.`

A magical weapon of ice that delivers a cold critical to heat-based
creatures. It will inflict one and a half times the normal damage
when used against a heat-based creature.


#### `FT - Flame Tongue.`

A magical weapon of flame that delivers a heat critical to
cold-based creatures. It will inflict one and a half times the
normal damage when used against cold-based or inflammable creatures.


#### `HA - Holy Avenger.`

A Holy Avenger is one of the most powerful of weapons. A Holy
Avenger will increase the strength and the armor class of the
wielder. This weapon will do extra damage when used against evil and
undead creatures, and will also give the ability to see invisible
creatures.


#### `SA - Slay Animal.`

A Slay Animal weapon is a special-purpose weapon whose sole intent
is to destroy all the dangerous animals in the world. An animal is
any creature natural to the world. Therefore an orc would not be an
animal, but a giant snake would be. This will inflict twice the
normal amount of damage when used against an animal.


#### `SD - Slay Dragon.`

A Slay Dragon weapon is a special-purpose weapon whose sole intent
is to destroy dragon-kind. Therefore, when used against a dragon,
the amount of damage done is four times the normal amount.


#### `SE - Slay Evil.`

A Slay Evil weapon is a special-purpose weapon whose sole intent is
to destroy all forms of evil. When used against an evil creature,
either alive or undead, the damage done twice the normal amount.


#### `SU - Slay Undead.`

A Slay Undead weapon is a special-purpose weapon whose sole intent
is to destroy all forms of undead. This weapon is hated and feared
by the intelligent undead, for a single blow from this weapon will
inflict three times the normal amount of damage. This weapon also
gives the ability to see invisible creatures, which is especially
useful against undead, since many of them are normally invisible.



### 9.3 Body and Shield Bashes

Weight is the primary factor in being able to bash something, but
strength plays a role too. After bashing, a character may be off-balance
for several rounds depending upon the dexterity stat.

Doors can be broken down by bashing them. Once a door is bashed open, it
is forever useless and cannot be closed.

Chests too may be bashed open, but be warned that the careless smashing
of a chest often ruins the contents. Bashing open a chest will not
disarm any traps it may contain, but does allow the strong and ignorant
to see what is inside.

Finally, a creature may be bashed. If a shield is currently being worn,
the bash is a shield bash and will do more damage. In either case, a
bash may throw an opponent off-balance for a number of rounds, allowing
the character to get in a free hit or more. If the character is thrown
off-balance, the opponent may get free hits. This is a risky attack.


### 9.4 The Character’s Armor Class

Armor class is a number that describes the amount and the quality of
armor being worn. Armor class will generally run from about 0 to 60, but
could become negative or greater than 60 in rare cases.

The larger the armor class, the more protective it is. A negative armor
class would actually help the character get hit. Armor protects in three
manners. One, it makes the character harder to be hit for damage. A hit
for no damage is the same as a miss. Two, good armor will absorb some of
the damage that the character would have taken. An armor class of 30
absorbs 15% of damage. Three, acid damage is reduced by wearing body
armor. It is obvious that a high armor class is a must for surviving the
lower levels of *moria*.

Each piece of armor has an armor class adjustment, and a magical bonus.
Armor bought in town will have these values displayed with its
description. Armor that is found within the dungeon must be identified
before these values will be displayed. All armor always has the base
armor class displayed, to which the bonus is added. It is always
possible to figure this out anyway, by watching the effect it has on the
displayed armor class.

Armor class values are always displayed between a set of brackets as
`[#]` or `[#,+#]`. The first value is the armor class of the item. The
second number is the magical bonus of the item which is only displayed
if known, and will always have a sign preceding the value. There are a
few cases where the form `[+#]` is used, meaning the object has no armor
class, only a magical armor bonus if worn. Body armor may also have a
`(-#)` displayed in parentheses; this is a penalty to hit, because the
bulk of the armor makes it more difficult to swing a weapon freely.

Some pieces of armor will possess special abilities denoted by the
following abbreviations:


#### `RA - Resist Acid.`

A character using such an object will take only one-third normal
damage from any acid thrown upon them. In addition, armor so
enchanted will resist the acid’s effects and not be damaged by it.

#### `RC - Resist Cold.`

A character using a resist cold object will take only one-third
damage from frost and cold.

#### `RF - Resist Fire.`

A character using a resist fire object will take only one-third
damage from heat and fire.

#### `RL - Resist Lightning.`

A character using a resist lightning object will take only one-third
damage from electrical attacks.

#### `R - Resistance.`

A character wearing armor with this ability will have resistance to
Acid, Cold, Fire, and Lightning as explained in each part above.



### 9.5 Crowns

Some crowns also have special magical abilities that improve chances in
a battle.

#### `Crown of Might`

This is the great crown of the warriors. This crown increases the
strength, dexterity, and constitution of the wearer. Any attempt to
paralyze or slow the character will fail.

#### `Crown of the Magi`

This is the great crown of the wizards. The wearer will have an
increased intelligence, and will also be given resistance against
fire, frost, acid, and lightning.

#### `Crown of Lordliness`

This is the great crown of the priests. The wearer will have an
increased wisdom and charisma.

#### `Crown of Seeing`

This is the great crown of the rogues. The wearer will be able to
see even invisible creatures, and will have an increased ability to
locate traps and secret doors.

#### `Crown of Regeneration`

This crown will help you regenerate hit-points and mana more quickly
than normal, allowing the character to fight longer before needing
to rest. The character will use food faster than normal while
wearing this crown because of the regenerative effects.

#### `Crown of Beauty`

This crown looks impressive, and will increase charisma, but is
otherwise not useful.



### 9.6 Magic Spells

All characters except warriors are able to learn some form of magic
spells. There are two kinds of magic in the game:

1.  Magic Spells
2.  Prayers

Magic spells can be casted by mages, rogues, and rangers. Prayers can be
prayed by priests and paladins. Each spell/prayer has a minimum level
required for learning it, a mana score, which is the mana points
required to cast it, and a failure percentage.

Mana points are determined by the character’s experience level, and the
level of a key stat. For mages, rogues, and rangers, the key stat is
intelligence, for priests and paladins the key stat is wisdom. The
effect of the key stat is shown by the following table.


             Stat Level                 Factor
                3-7                       0
                8-17                      1
               18-18/49                  3/2
            18/50-18/69                   2
            18/70-18/89                  5/2
            18/90-18/99                   3
               18/100                     4


The mana score is the factor times the experience level plus 1. If the
key stat is 7 or less, the mana score will be zero (not one) and the
character will not be able to use any spells. When attempting to cast a
spell that calls for more mana than the character has, the rate of
failure is much greater than normal. The character will faint for a few
turns afterward, and there is a chance at damaging health too.


#### 9.6.1 Priest Spells

Priest spells are received from the character’s deity. When the `G`
command is issued to learn new spells, spells are chosen randomly from
the spells that the character is able to cast. The character need not
have the book the spell is in to learn it, because the god gave it to
the character, but the book is needed to cast the spell. Failure
percentages and spell effectiveness are based on wisdom for priests and
paladins.


#### 9.6.1.1 Priest Spell Levels And Mana

This is a table of all the spells, with the mana and level of
achievement for Priests and Paladins.


                               Priest    Paladin
    (Beginner's Handbook)     Lv  Mana   Lv  Mana
    A  Detect Evil             1   1      1   1
    B  Cure Light Wounds       1   2      2   2
    C  Bless                   1   2      3   3
    D  Remove Fear             1   2      5   3
    E  Call Light              3   2      5   4
    F  Find Traps              3   3      7   5
    G  Detect Doors/Stairs     3   3      7   5
    H  Slow Poison             3   3      9   7


    (Words of Wisdom)
    A  Blind Creature          5   4      9   7
    B  Portal                  5   4      9   8
    C  Cure Medium Wounds      5   4     11   9
    D  Chant                   5   5     11  10
    E  Sanctuary               7   5     11  10
    F  Create Food             7   5     13  10
    G  Remove Curse            7   6     13  11
    H  Resist Heat and Cold    7   7     15  13


    (Chants and Blessings)
    A  Neutralize Poison       9   6     15  15
    B  Orb of Draining         9   7     17  15
    C  Cure Serious Wounds     9   7     17  15
    D  Sense Invisible        11   8     19  15
    E  Protection from Evil   11   8     19  15
    F  Earthquake             11   9     21  17
    G  Sense Surroundings     13  10     23  17
    H  Cure Critical Wounds   13  11     25  20
    I  Turn Undead            15  12     27  21


    (Exorcisms and Dispellings)
    A  Prayer                 15  14     29  22
    B  Dispel Undead          17  14     31  24
    C  Heal                   21  16     33  28
    D  Dispel Evil            25  20     35  32
    E  Resist Poison Gas      31  45     --  --
    F  Glyph of Warding       33  24     37  36
    G  Holy Word              39  32     39  38


#### 9.6.1.2 Priest Spell Descriptions

This is a short description of each of the spells, listed
alphabetically.

#### `Bless`

Improves armor class and fighting ability for a short period of time.

#### `Blind Creature`

Blinds a creature for a short period of time.

#### `Call Light`

Lights up an area.

#### `Chant`

Improves armor class and fighting ability for a medium period of time.

#### `Create Food`

Causes a food item to be dropped at the character`s feet.

#### `Cure Critical Wounds`

Cures a very large number of hit-points.

#### `Cure Light Wounds`

Cures a small number of hit-points.

#### `Cure Medium Wounds`

Cures a medium number of hit-points.

#### `Cure Serious Wounds`

Cures a large number of hit-points.

#### `Detect Doors/Stairs`

Finds all the doors and stairs on the screen.

#### `Detect Evil`

Finds all the evil creatures on the screen.

#### `Dispel Evil`

Attempts to destroy the evil creature.

#### `Dispel Undead`

Attempts to destroy the undead creature.

#### `Earthquake`

Randomly toggles corridors into walls and vice versa.

#### `Find Traps`

Locates all the traps on the screen.

#### `Glyph of Warding`

Leaves a *Glyph* that monsters won`t pass over. Appears as a trap.

#### `Heal`

Restores 200 Hit Points.

#### `Holy Word`

Dispels evil, removes fear, cures poison, restores 1000 hit-points,
restores all stats, and invulnerability for 3 turns.

#### `Neutralize Poison`

Cures you of poison.

#### `Orb of Draining`

Offensive spell that drains levels from monsters.

#### `Portal`

Teleports the character a short distance away.

#### `Prayer`

Improves armor class and fighting ability for a long period of time.

#### `Protection from Evil`

Causes evil creatures to do less damage.

#### `Remove Curse`

Removes {damned} objects that you are welding.

#### `Remove Fear`

Negates the fear placed on the character by an enemy.

#### `Resist Heat and Cold`

Reduce damage from heat or cold attacks.

#### `Resist Poison Gas`

Permanently reduces damage from poison gas attacks.

#### `Sanctuary`

Causes neighboring monsters to fall asleep for a while.

#### `Sense Invisible`

Finds all invisible creatures on the screen.

#### `Sense Surroundings`

Maps the dungeon appearing on the screen.

#### `Slow Poison`

Reduces the rate hit-points are lost due to poison.

#### `Turn Undead`

Attempts to cause undead creatures to flee.


#### 9.6.2 Mage Spells

Mage Spells are more powerful and offensive in nature than priest
spells. This offsets the fact that magicians are generally weaker than
any other class. Because mage spells are learned though study, the
correct magic book must be present in the character’s pack to learn and
cast a spell. Learning spells can be banked up. For example: a second
level mage who had learned Magic Missile and can learn one more spell.
If the mage does not wish to learn Detect Monsters, Phase Door or Light
Area, the mage can wait until the third level and learn both the Cure
Light Wounds and Stinking Cloud, both third level spells. Spell failure
and effectiveness is based on intelligence for mages, rangers, and
rogues. Rangers can learn all but the most powerful offensive spell.
Rogues cannot learn any offensive spell.


#### 9.6.2.1 Mage Spell Levels And Mana


                                      Mage       Ranger      Rogue
    (Beginners-Magick)               Lv  Mana    Lv  Mana    Lv  Mana
    A  Magic Missile                 1   1       3   1      --  --
    B  Detect Monsters               1   1       3   2       5   1
    C  Phase Door                    1   2       3   2       7   2
    D  Light Area                    1   2       5   3       9   3
    E  Cure Light Wounds             3   3       5   3      11   4
    F  Find Hidden Traps/Doors       3   3       5   4      13   5
    G  Stinking Cloud                3   3       7   5      --  --


    (Magik I)
    A  Confusion                     3   4       7   6      15   6
    B  Lightning Bolt                5   4       9   7      --  --
    C  Trap/Door Destruction         5   5       9   8      17   7
    D  Sleep I                       5   5      11   8      19   8
    E  Cure Poison                   5   5      11   9      21   9
    F  Teleport Self                 7   6      13  10      --  --
    G  Remove Curse                  7   6      13  11      23  10
    H  Frost Bolt                    7   6      15  12      --  --
    I  Turn Stone to Mud             9   7      15  13      --  --


    (Magik II)
    A  Create Food                   9   7      17  17      25  12
    B  Recharge Item I               9   7      17  17      27  15
    C  Sleep II                      9   7      21  17      --  --
    D  Polymorph Other              11   7      21  19      --  --
    E  Identify                     11   7      23  25      29  18
    F  Sleep III                    13   7      23  20      --  --
    G  Fire Bolt                    15   9      25  20      --  --
    H  Slow Monster                 17   9      25  21      --  --


    (Mages Guide to Power)
    A  Frost Ball                   19  12      27  21      --  --
    B  Recharge Item II             21  12      29  23      --  --
    C  Teleport Other               23  12      31  25      --  --
    D  Haste Self                   25  12      33  25      --  --
    E  Fire Ball                    29  18      35  25      --  --
    F  Resist Poison Gas            31  45      --  --      --  --
    G  Word of Destruction          33  21      37  30      --  --
    H  Genocide                     37  25      --  --      --  --


Note: Rangers don't get spells until 3rd level, Rogues 5th level.


#### 9.6.2.2 Mage Spell Descriptions

#### `Confusion`

Confuses a monster for a short time.

#### `Create Food`

Causes a food item to be dropped at the character`s feet.

#### `Cure Light Wounds`

Restores a small number of hit-points.

#### `Cure Poison`

Neutralizes the poison running through the character`s veins.

#### `Detect Monsters`

Displays all the monsters on the screen.

#### `Find Hidden Traps/Doors`

Locates all the secret traps and doors.

#### `Fire Ball`

Shoots a ball of flame toward a monster.

#### `Fire Bolt`

Shoots a bolt of flame toward a monster.

#### `Frost Ball`

Shoots a ball of frost toward a monster.

#### `Frost Bolt`

Shoots a bolt of frost toward a monster.

#### `Genocide`

Destroys a particular monster on the level.

#### `Haste Self`

Causes the character to move faster temporarily.

#### `Identify`

Identifies an unknown object in the pack.

#### `Light Area`

Illuminates the area with light.

#### `Lightning Bolt`

Shoots a bolt of lightning at the enemy.

#### `Magic Missile`

Traditional bolt of magic used to damage enemies.

#### `Phase Door`

Teleports the character a short distance.

#### `Polymorph Other`

Polymorphs a monster into a different creature.

#### `Recharge Item I and II`

Recharges a staff, or wand.

#### `Remove Curse`

Allows {damned} items to be taken off.

#### `Resist Poison Gas`

Permanently reduces damage from poison gas attacks.

#### `Sleep I`

Causes a specified monster to fall asleep.

#### `Sleep II`

Causes neighboring monsters to fall asleep.

#### `Sleep III`

Causes all monsters in range to fall asleep.

#### `Slow Monster`

Causes a monster to move slower.

#### `Stinking Cloud`

Shoots a ball of noxious vapors to do damage.

#### `Teleport Self`

Teleports the character to a new place on the level.

#### `Teleport Other`

Teleports an enemy to a new place on the level.

#### `Trap/Door Destruction`

Destroys all neighboring doors and traps.

#### `Turn Stone to Mud`

Causes a wall (or other stone object) to melt.

#### `Word of Destruction`

Destroys the entire screen.


For spells that come in numbered versions (Sleep I, II, III, etc), the
higher numbers have a higher effectiveness, but greater change of spell
failure and greater Mana cost.



#### 9.6.3 Using Offensive Spells Against Monsters

Monsters have a chance to save themselves from damage caused by
offensive spells, just like the character has a chance to be saved from
damage by monster spells. This chance is greater for higher level
monsters than for lower level monsters. Also, some spells will never
work against monsters whose level is higher than the character’s
experience level.

Many monsters are immune to certain kinds of attack, and will suffer
little or no damage from such attacks. For example, a fire-breathing
dragon will suffer little damage from a fire ball, but will suffer
greatly from a frost ball. Also, undead creatures will not be affected
by sleep spells, since they never sleep.


## 10. Objects Found In The Dungeon

The mines are full of objects just waiting to be picked up and used. How
did they get there? Well, the main source for useful items are all the
foolish adventurers that proceeded into the dungeon beforehand. They get
killed, and the helpful creatures scatter the various treasure
throughout the dungeon. Most cursed items are placed there by the joyful
evil sorcerers, who enjoy a good joke when it gets someone killed.

The character picks up objects by moving on top of them. Up to 22
different items can be carried in the backpack while wearing and
wielding many others. Although limited to 22 different items, several
items of each kind can be carried, restricted only by the amount of
weight the character can carry. The weight limit is determined by the
strength stat. Only one object may occupy a given floor location, which
may or may not also contain one creature. Doors, traps, and staircases
are considered objects for this purpose.

When carrying more weight than the character is able, they will move
more slowly than normal until the extra weight is dropped. If picking up
an object would take the character over their weight limit, then the
player will be asked whether or not the object should be picked up. It
is a good idea to leave the object alone when fleeing from a monster.

Many objects found within the dungeon have special commands for their
use. Wands must be Aimed, staves must be Used, scrolls must be Read, and
potions must be Quaffed. In any case, the character must first be able
to carry an object before it can be used. Some objects, such as chests,
are very complex. Chests contain other objects and may be trapped,
and/or locked. Read the list of commands carefully for a further
understanding of chests.

One item in particular will be discussed here. The scroll of
"Word-of-Recall" can be found within the dungeon, or bought at the
temple in town. It acts in two manners, depending upon the character’s
location. If read within the dungeon, it will teleport the character
back to town. If read in town, it will teleport the character back down
to the deepest level of the dungeon on which the character has
previously been. This makes the scroll very useful for getting back to
the deeper levels of *moria*. Once the scroll has been read, it takes a
while for the spell to act, so don't expect it to save the character in
a crisis.

The game provides some automatic inscriptions to help keep track of
possessions. Wands and staves which are known to be empty will be
inscribed with "`empty`". Objects which have been tried at least once,
but haven't been identified yet will be inscribed with "`tried`". Cursed
objects are inscribed with "`damned`". Also, occasionally the character
will notice that something in the pack or equipment list seems to be
magical. High level characters are much more likely to notice this than
beginning characters. When the character does notice this, the item in
question will be inscribed with "`magik`".

And lastly, a final warning: not all objects are what they seem.
Skeletons lying peacefully about the dungeon have been known to get
up...


### 10.1 Cursed Objects

Some objects, mainly armor and weapons, have had curses laid upon them.
These horrible objects will look like any other normal item, but will
detract from the character’s stats or abilities if worn. They will also
be impossible to remove until a remove curse is done.

When a cursed item is worn or wielded, the character will immediately
feel that something has gone wrong. The item will also be inscribed
"`damned`".


### 10.2 Mining

Much of the treasure within the dungeon can be found only by mining it
out of the walls. Many rich strikes exist within each level, but must be
found and mined. Quartz veins are the richest, yielding the most metals
and gems, but magma veins will have some hordes hidden within.

Mining is virtually impossible without a pick or shovel. Picks and
shovels have an additional magical ability expressed as `(+#)`. The
higher the number, the better the magical digging ability of the tool. A
pick or shovel also has pluses to hit and damage, and can be used as a
weapon.

When a vein of quartz or magma is located, the character should wield a
pick or shovel and begin digging out a section. When that section is
removed, another section of the vein can be located, and begin the
process again. Since granite rock is much harder to dig through, it is
much faster to follow the vein exactly and dig around the granite. There
is a game option for highlighting magma and quartz.

If the character has a scroll or staff of treasure location, all strikes
of treasure within a vein can be immediately shown on the screen. This
makes mining much easier and more profitable.

It is sometimes possible to get a character trapped within the dungeon
by using various magical spells and items. So it is a very good idea to
always carry some kind of digging tool, even when tunneling for treasure
is not a goal.


### 10.3 Staircases, Secret Doors, Passages and Rooms

Staircases are the manner in which the character gets deeper, or climbs
out of the dungeon. The symbols for the up and down staircases are the
same as the commands to use them. A `<` represents an up staircase and a
`>` represents a down staircase. The character must move on top of the
staircase before they can be used.

Each level has at least one up staircase, and at least two down
staircases. There are no exceptions to this rule. The character may have
trouble finding some well hidden secret doors, but the stairs are there.

Many secret doors are used within the dungeon to confuse and demoralize
adventurers foolish enough to enter. But with some luck, and lots of
concentration, these secret doors can be found. Secret doors will
sometimes hide rooms or corridors, or even entire sections of that level
of the dungeon. Sometimes they simply hide small empty closets or even
dead ends.

Creatures in the dungeon will generally know and use these secret doors.
If they leave one open, the character will be able to go right through
it. If they close it behind them the character will have to search for
the catch first. Once a secret door has been discovered, it is drawn as
a known door and no more searching will be required to use it.


## 11. Winning The Game

Once the character has progressed into killing dragons with but a mean
glance and snap of the fingers, they may be ready to take on the Balrog.
The Balrog will appear on most levels after level 49 (a depth of 2450
feet), so don't go down there until the character is ready to take on
the biggest and baddest creature in the game.

The Balrog cannot be killed in some of the easier methods used on normal
creatures. The Balrog will cunningly teleport to another level if a
spell such as destruction is cast. Also, the Balrog cannot be
polymorphed, slept, confused, or commit genocide. Magical spells like _Coldball_
are effective, as are weapons, but the Balrog is difficult to kill, and
if allowed to escape to another level it can heal itself.

If the character should actually survive the attempt of killing the
Balrog, they will receive the status of `WINNER`. Since the toughest
creature alive has been defeated, the character is ready to retire and
cannot be saved. When you quit the game, the character receives a
surprise bonus score.


## 12. Upon Death and Dying

If the character falls below 0 hit-points, they have died and cannot be
restored. A tombstone showing information about the character will be
displayed. A record of the character will be provided, along with all of
the equipment (identified) either on the screen or saved in a file.

The character will leave behind a reduced save file, which contains only
the monster memory and the game option choices. It may be restored, in
which case the new character is generated exactly as if the file was not
there, but the new character will find the monster memory containing all
the experience of past incarnations.


## 13. Wizards

There are rumors of *moria* Wizards which, if asked nicely, can explain
details of the *moria* game that seem complicated to beginners.


## 14. Contributors

The following people have contributed to *moria*:

-   Robert Alan Koeneke (Creator of Moria)
-   Jimmey Wayne Todd
    - added character generator
    - added skills
    - added history
-   James E. Wilson (Creator of Umoria 4.87)
    - ported moria from VMS to unix
-   David J. Grabiner (Maintainer of Umoria 5.5.2)
    - many bug fixes
-   Lars Hegelund
    - many bug fixes
-   Ben Asselstine
    - changed build system to autoconf
    - added manual in texinfo format
-   D. G. Kneller
    - reduced map display
    - 16 bit integers
-   Christopher J. Stuart
    - recall
    - options
    - inventory
    - running code
-   William Setzer
    - object naming code
-   Dan Bernstein
    - UNIX hangup signal fix
    - many bug fixes
-   Joseph Hall
    - line of sight code
-   Brian Johnson
    - linux support [Ed note: GNU+Linux support]
-   Eric Bazin
    - merge monster memories


## 15. Umoria Licence

```
Umoria is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Umoria is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Umoria.  If not, see <http://www.gnu.org/licenses/>.
```
