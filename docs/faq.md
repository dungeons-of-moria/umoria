# Umoria: Frequently Asked Questions

_The most common questions, asked by both beginners and others..._

This FAQ relates to Umoria 5.7, released November 2016 and is split into
three sections:

1. General Information
2. Game Play - Non-Spoiler Questions
3. Game Play - Spoilers!


## 1. General Information

### How do I get the Umoria?

The best place to start is the [Umoria.org](http://umoria.org) website.

**The Game** - Umoria is available for Windows, macOS, and Linux.
The game files can be found on the
[Umoria download page](http://umoria.org/download).

**Source Code** - the main repository is currently hosted on Github at
[github.com/dungeons-of-moria/umoria](https://github.com/dungeons-of-moria/umoria)

If you are interested in Robert A. Koeneke's original VMS Pascal Moria,
developed 1980-87, then please visit the
[Dungeons of Moria](https://github.com/dungeons-of-moria) Github page, which
also includes a few other Pascal ports of Moria, including _Imoria_ and
_Moria UB_.


### How do I run the game?

Umoria 5.7 is a terminal/console game. Here is a very brief explanation on
running the game, however, for more detailed information please visit the
Umoria.org [download/installation page](http://umoria.org/download).

**macOS / Linux**

To run the game you must first open the
terminal application on your system, navigate to the `umoria` directory, and then
type the `./umoria` command.

**Windows**

Like with macOS, you can open the Windows console application (`cmd.exe` or PowerShell)
and launch the game from there. However, you can also navigate to the folder in
_Windows Explorer_ and double click on the `umoria.exe` file name to start the game.


### I think I've found a bug, what should I do?

When you are reporting a suspected bug, make sure to give the version number
and the system you are playing on, and provide as much information as possible.

Here are the preferred methods for reporting the bug:

* Email `info@umoria.org`
* [Submit an issue](https://github.com/dungeons-of-moria/umoria/issues) to the source code repository.

If you have a patch for the bug, please submit a _Pull Request_ to the
[source code repository on Github](https://github.com/dungeons-of-moria/umoria).



## 2. Game Play - Non-Spoiler Questions


### How does resistance work?  Are two items of resistance cumulative?

Resist heat/cold potions and spells give temporary resistance to heat or
cold. All other resistance items give permanent resistance. Two permanent
resistances _are not_ cumulative, and two temporary resistances _are_
cumulative only in duration.

Fire and cold do 1/3 damage if you have single resistance, 1/9 if you have
double. Acid does 1/2 damage if you have any armor to corrode, 1/3 if you have
resistance but no armor, and 1/4 if you have resistance and armor. Lightning
does 1/3 damage if you have resistance.

There is no resistance against poison gas.


### How does speed work? Do you get faster if you are already Very Fast and get another speed item?

`Very Fast` is the highest speed that can be displayed, but if you are fortunate
enough to find several speed items, you can get still faster. Permanent speed
items (rings and boots) are cumulative, and temporary speed (potions, spells,
and staffs) can add one more point to your speed. Multiple uses of temporary
speed are cumulative only in duration.



## 3. Game Play - Spoilers! (W.I.P.)

_**WARNING: the information given below contains many spoilers, read at your own peril.**_


### What are the special abilities of ego weapons?

_Crowns?  Amulet of the Magi?  Cloak of Protection?_

All version-dependent changes are marked in brackets.

    Amulet of the Magi      free action, see invisible, searching, +3 AC.

    Cloak of Protection     no special ability, just a larger bonus than usual.

    Ego weapons:

    (HA) Holy Avenger       +(1-4) str, +(1-4) AC, (SE), (SU),
                            sustain stat, see invisible.
    (DF) Defender           stealth, regeneration, free action, see invisible,
                            feather fall, RF, RC, RL, RA, +(6-10) to AC
    (SM) Slay Monster       Damage (x 2) vs. monsters, see invisible.
    (SA) Slay Animal        Damage (x 2) vs. animals, see invisible.
    (SD) Slay Dragon        Damage (x 4) vs. dragons.
    (SE) Slay Evil          Damage (x 2) vs. evil monsters.
    (SU) Slay Undead        Damage (x 3) vs. undead, see invisible.
    (FT) Flame Tongue       Damage (x 1.5) vs. monsters harmed by fire.
    (FB) Frost Brand        Damage (x 1.5) vs. monsters harmed by cold.

    A HA which is +1 to strength sustains strength; +2, intelligence; +3,
    wisdom; +4, constitution (not dexterity).

    Crown of the Magi       +(1-3) int, (RF), (RC), (RA), (RL)
    Crown of Lordliness     +(1-3) wis, chr.
    Crown of Might          +(1-3) str, dex, con, free action.
    Crown of Seeing         see invisible, +(10-25) searching, +(2-5) to search.
    Crown of Regeneration   Regeneration.
    Crown of Beauty         +(1-3) charisma.

Regeneration lets you recover mana and hit points at 1.5 times the normal rate,
but also makes you use up food much more quickly.

Free action prevents you from being slowed or paralyzed by monsters.


### How much damage do spells and wands do?

          Spell Name      | Damage
    ----------------------+-------
    Magic Missile            2d6
    Stinking Cloud           12
    Lightning Bolt           4d8
    Lightning Ball           32
    Frost Bolt               6d8
    Frost/Cold Ball          48
    Acid Ball                60
    Fire Bolt                9d8
    Fire Ball                72
    Wand of Drain Life       75

A wand of wall building will do `4d8` damage to any creature buried in the
wall (except one that moves through walls), and will kill any immobile
creature. On the creature's next turn, it will attempt to move out of the
wall, and if it is unable to do so, it will take `10d8` damage and dig its way
out.

    Wand of Light / Staff of Starlight  2d8 (if sensitive)
    Stone to Mud                        100 (if sensitive)
    Orb of Draining                     3d6 + caster's level, double to evil creatures
    Dispel Undead/Evil                  1-60 from scroll or staff; 1 up to triple
                                        caster's level from spell
    Holy Word                           Dispel evil for 1 up to quadruple caster's level



### What does spell Y do?

Non-obvious spell effects:

**Mage Spells**

- Phase Door: short-range teleport.
- Find Hidden Traps/Doors: also detects stairs.
- Sleep I: sleep one monster in a given direction.
- Recharge Item I: fewer charges than Recharge Item II, and more likely to fail.
- Sleep II: sleep all monsters adjacent to player.
- Sleep III: sleep all monsters with a line of sight to player
  (including invisible ones).
- Word of Destruction: obliterates everything within 15 spaces of the player;
  Balrog will teleport to another level.

**Priest Spells**

- Bless: +2 to AC, and +5 to chance to hit (equivalent to +1-2/3 bonus on
  weapon) for a short time.
- Blind Creature: blinded creatures wander around confused until they recover.
- Portal: medium-range teleport.
- Chant: double duration Bless.
- Sanctuary: sleep creatures adjacent to player.
- Protection from Evil: prevents any evil creature of the player's level or
  lower from attacking the player.
- Earthquake: causes random walls and ceilings in the area to collapse,
  possibly injuring anything nearby.
- Turn Undead: all undead of the player's level or lower, and some of higher
  level, will attempt to flee.
- Prayer: quadruple duration Bless.
- Dispel Undead/Evil: affects all undead/evil within line of sight,
  damage is from 1 up to 3x player's level, 1-60 from scroll or staff.
- _Glyph of Warding_: creates a glyph which monsters cannot enter, but have a
  small chance of breaking.
- Holy Word: heals player completely, cures poison and fear, and dispels evil
  for 1 to 4x player's level. Also restores all stats, and makes player
  invulnerable for 3 turns.



### On what level do you find X?

_Where important objects are found._

Just 1/12 of items are chosen as if you were on a deeper level, which has
(current level/50) chance of being level 50; this check is not made in town.
This affects only the type of item, not its enchantment.

Items become somewhat less common as you go deeper than the indicated levels;
however, if you can survive down there, this is compensated for by the fact
that there are more treasures on deeper levels.

     Item type                  | Level
    ----------------------------+-------------------------------------
    Ego weapons, special armor,   Progressively more common as you get
    boots, gloves, helmets        deeper, all the way to level 55

    Healing potion                12
    Gain stat potions             25
    Restore mana potion           25
    Invulnerability potion        40
    Gain experience potion        50
    Genocide scroll               35
    Destruction scroll            40
    Rune of Protection scroll     50
    Mass Genocide scroll          50
    Amulets of wisdom, charisma   20
    Gain str/int/dex/con rings    30
    Amulet of the magi            50
    Ring of speed                 50
    Staff of speed                40
    Staff of mass polymorph       46
    Staff of dispel evil          49
    Staff of destruction          50
    Wand of clone monster         15
    Wand of drain life            50




### How many attacks can I get with this weapon?

Here is the table for the number of blows for a given strength and dexterity.
If your strength or dexterity is `18+xx`, that is stored as `18/xx`; thus, for
example, you need an `18/90` strength to use the bottom row of the table with
a Katana (12 pounds).

If you don't know the weight of a weapon, set `Show weights in inventory`,
found in the game options menu.

    int8u blows_table[7][6] = {
      /* STR/W:    9  18  67 107 117 118   : DEX */
      /* <2 */  {  1,  1,  1,  1,  1,  1 },
      /* <3 */  {  1,  1,  1,  1,  2,  2 },
      /* <4 */  {  1,  1,  1,  2,  2,  3 },
      /* <5 */  {  1,  1,  2,  2,  3,  3 },
      /* <7 */  {  1,  2,  2,  3,  3,  4 },
      /* <9 */  {  1,  2,  2,  3,  4,  4 },
      /* >9 */  {  2,  2,  3,  3,  4,  4 }
    };


### How do you kill an Ancient Multi-Hued Dragon?

Usually, you don't want to try; one gas breath from a full-strength AMHD
does 693 damage, with no resistance.

If you can get to speed 3 (one permanent speed item, and either another
permanent speed item or a haste self spell or staff), you can try this
technique. First, create (or find in a maze room) a wall with one open
space on all four sides.

     .
    .#.
     .

Stand on one side, with the dragon on the other side. When the dragon moves
adjacent to you, attack it once, and then hide behind the pillar. The dragon
can't see you, so it won't breathe, and will instead chase you to another
side. Now attack once, and hide again, and so on until the dragon is finished.


### How do you kill an Emperor Lich?

You can kill an Emperor Lich if you can get to speed 2, which is its speed. A
Mage or Ranger can do this with the spell of haste self; anyone else needs a
staff of speed, potion of haste self, or permanent speed item. You will also
need about 10 cure critical wounds or cure serious wounds potions, and some
item giving you free action.

You also need some ranged spell attack. Liches take double damage from
lightning, so the spell of lightning bolt or wand of lightning balls is a good
choice. Rogues and Warriors will need several wands, with a total of about 30
charges to guarantee that they can kill the Lich with them. A Priest or
Paladin has _Orb of Draining_, which is even better.

Now, try to line up with the Lich while you are not adjacent to it, either in
a room or a corridor. This gives you a chance to cast your spell. The Lich
will get one action. If it cast a spell and you resisted, or the spell didn't
do anything harmful, you have another chance. If you were confused or blinded,
drink a cure wounds potion; the Lich isn't adjacent to you, so it can't hurt
you. If the Lich moved and is now adjacent to you, move back. Try to avoid
getting cornered, and phase door or portal away if you are. A priest can make
this easier by putting down a _Glyph of Warding_, but this must be done
**before** the Lich chases you across the glyph -- don't stand on the glyph; it
isn't foolproof.

If you run low on mana and don't have a wand, teleport out and come back later
to finish the job.

A priest with _Glyph of Warding_ can also set up the following configuration
(the exact length doesn't matter as long as you are within spell range):

    #L#####
    #^^...@
    #######

The Lich cannot cast spells from this position, because it cannot see you. As
long as it doesn't break the glyphs, you are safe, and can fire an _Orb of
Draining_ down the corridor; the Lich will take some damage each time. If the
Lich breaks either glyph, run or teleport out, and continue the battle
elsewhere.

I do not advise trying this technique against an AMHD; it will probably break
a glyph before the battle is over, and if your teleport spell fails, or if you
haven't hasted yourself, the AMHD gets a chance to breathe.

An Emperor Lich has 1520 hit points, plus anything additional that it gains by
draining mana (6 points per mana point drained) and charges (40 points per
charge). Never let it attack you in melee, because it can destroy your wands,
healing itself in the process, as well as draining your experience and
dexterity.

If you can get to speed 3, faster than the Lich, it is easy to kill; just
fight, move back, fight, move back, and so on. You will still need a lot of
cure wounds potions, unless you let it chase you around a pillar, as in the
AMHD technique.


## End Note

Much of this information was included in the original Umoria sources - last
updated in 2006 - and covered versions from 4.8 to 5.5 of the game. I've tried
to remove all information that is not relevant to the current game, however,
there may be incorrect/inconsistent information still present. If you find
something that needs changing, please let us know (`info@umoria.org`) so it
can be fixed.
