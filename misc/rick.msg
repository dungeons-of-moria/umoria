From 18862246%VUVAXCOM.BITNET@lilac.berkeley.edu Thu Aug 24 13:11:19 1989
Received: from lilac.Berkeley.EDU by ernie.Berkeley.EDU (5.61/1.36)
	id AA06320; Thu, 24 Aug 89 13:11:14 -0700
Received: by lilac.berkeley.edu (5.61.1/1.16.22)
	id AA05399; Thu, 24 Aug 89 12:47:05 PDT
Message-Id: <8908241947.AA05399@lilac.berkeley.edu>
Date:     Thu, 24 Aug 89 15:41 EST
From: "Computers and you, living life a bit at a time"
          <188622462%VUVAXCOM.bitnet@lilac.berkeley.edu>
Subject:  Here is the UPDATE.TXT file
To: wilson@ernie.Berkeley.EDU
Original_To:  BITNET%"wilson@ucbernie"
Status: RO


Here are the changes incorporated to date in the upgrade from V5.0 to V6.0:

    o The source code has been converted from a %INCLUDE format to modular
      programs.  Due to low system memory quotas set on student accounts at my
      university, compiling the %INCLUDE form was not possible.  By converting
      to modular form, anyone with enough disk space can compile the program,
      by compiling each module separately, and then linking all the files
      together.  Along with this, I also changed all the indentation in the
      code so that it is uniform throughout, something that was not there from
      having four or more different people work on it before.

    o Creation of a new high score list.  Early in Moria's time at my node,
      many people had found out the Wizard mode password (this was for version
      4.3 and then 4.8), so I have created a Wizard Mode Top Twenty list. Part
      of this has been creating 2 new fields in the player character record to
      enable Moria to remember if your character ever entered Wizard or God
      modes.  If you used Wizard mode at any point, you are eligible for the
      Wizard mode list, if you used God mode, you can't get on any list.  If
      you die and aren't allowed to go to either of the lists, the game does
      tell you what score you earned.

    o A second effect of everyone here knowing the Wizard password was the
      continual resurrection of characters by copying the save file and re-
      storing the character.  This lead to multiple copies of the same
      character appearing on the high score lists.  I have solved this problem
      by adding two more new fields to the character record, to keep track of
      the birthdate and birthtime of the character, taken from the system time
      at the same point that the stats are rolled.  When a character dies, his
      birthdate/time is recorded in the highscore file, and whenever that same
      userid kills another character, it checks to see if the birthdate/times
      match, if so, then it assumes a clone, and doesn't post the score to the
      list.  The game does tell you what you had earned in this case.

    o The game now saves the userid of the character's creator in the
      save file.  This prevents one person making a strong character, then
      giving it to someone else to use to finish off the Balrog.

    o The increase of stats via special items (ie: HA increasing strength) and
      via Gain potions has been re-worked.  Increase stat items (weapons,
      rings, crowns, amulets, armor) now affects only the current value of the
      stat, not the max value.  Gain potions affect the max and the current,
      based on the max value.  If you are close to 18/100 and the item would
      increase you past that, you will see a drop in the current value of the
      stat when you take it off, but a Restore potion will return you to what
      it was before you wielded/wore the item.

    o The changing of the cost of an item has been repaired where an increase
      to the tohit and todam values are concerned.  Also, changes have been
      implemented wherever items are damaged, so that the shops will know of
      their decreased value.

    o The Vampire Bites attack has been modified.  Originally, you lost the
      same amount (or close to it) of hit points as you did experience points
      (which could be from 18d8 to 32d8, depending on the type of Vampire).
      The Vampire Bite now takes off 1d6 hit points (logic: you bleed a bit).

    o The stats area (left side of screen) has been fixed in regards to field
      widths.  All fields, when re-printed, wipe out any extra digits from the
      previous value.  If you have more than 9,999,999 in gold, the string
      "$$$$$$$" is printed.  If more than 9,999,999 experience points are
      earned, the string "*******" is printed.  If you have more than 999 mana
      then the string "###/###" will be printed.

    o The God mode function ^C (Make Nice Character) now checks to see if the
      character is a Magic-using type (ie: not Warrior or Rogue) before upping
      the mana score.  The God function ^E (Change Character) does the same.

    o A new wizard function: Describe Object.  This will give you information
      about the item specified, including Damage, Tohit and ToDam adjustments,
      weight, and others.  This will not identify an unidentified item, only
      help you in figuring out if one item is better than another (ie: which
      sword does more damage?).  The hex flags for the item in question will
      be translated to show what special magic is inherit in the item.

    o A new God Mode function, ^X, Examine Character.  This prints out all the
      values of the Flags section of the player character record.  Useful in
      debugging purposes.

    o A bug with Search mode and going up/down stairs has been fixed.  When
      using stairs, Search mode is turned off (if it was on) and the relevant
      reduction in speed is removed.

    o All code referring to the "Consultant" classes and the "Extra" spells
      group have been removed, since a) they were written, but no character
      race was allowed to be one and b) they were simply slight modifications
      on the mage class.

    o In the source, there were two people who used their initials as parts of
      variable names.  These have all been changed to more descriptive names.

    o The Town generation code has been fixed so that any store could appear
      at any of the locations, instead of the Black Market always appearing in
      the upper right-hand corner.  The code there has also been fixed so that
      adding in additional stores should not be difficult.

    o The various spell books have been fixed so that multiple copies will be
      listed as "a) 2 Books of <whatever>" in the inventory list.

    o The running of the Black Market has been fixed up so that it works more
      like the other stores, except store inventory turn-around is still as
      as fast as it was before.  The inventory will now flucuate in size, just
      as the other stores do.

    o A bug involving the Wizard Mode Light Dungeon function and using a torch
      has been repaired so that the tunnel floors remain lit after passing
      over them.

    o Caps/Helms of Intelligence/Wisdom bought in the Black Market now
      correctly increase the relevant stat.

    o A bug involving certain area-affect spells (Light Area, any Ball-type
      spell, etc) and the right-hand and bottom edges of the town level has
      been fixed.

    o The magic translation case statements in SCROLLS.PAS and POTIONS.PAS
      has been fixed to allow a full set of both to be used.  Previously,
      each one was coming up short of the 64 actual possible slots that are
      available by using two Tval values.

    o The problem with the special Magic and Prayer books from the Black
      Market has been fixed so that they appear as separate items in your
      inventory and are not added with the normal books.

    o The bug concerning enchanting items bought from the Black Market and the
      new values never being seen was been repaired, so that the name string
      of the item will be displayed properly.

    o The Control-Y exit command has been changed so the user has to hit Q to
      exit instead of Y.  This is to make it more fool-proof and more accident
      proof (but not totally).

    o Three new monster attacks: Lose Mana (different from the monster spell),
      Slip, Trip, and Fall (you'll find out), and Eclipse attack.  A number of
      new attack descriptions have been added with this.  A new function was
      added, function DEX_ADJ, to determine the character's plus or minus to
      his dexterity saving throw.

    o New monsters have found their way into the Dungeons of Moria.  A list of
      their names can be found in the file MONSTERS.NEW, included in the
      distribution package (or from your local Moria Wizard).

    o The creatures "Mother & Baby" and "Small Girl" have been fixed to
      correctly utilize the CDEFENSE flag of Lose Experience for killing that
      creature.

    o The global constant store$turn_around has been changed to the global
      array variable store_turnaround, allowing each store to have a different
      inventory turn over rate.  This was done partially to work the Black
      Market in more properly.

    o A bug involving the running function ('.' and a direction) has been
      fixed so that if you are attacked in any form, either via a physical
      hit or a spell, you will stop running at that point.

    o New monster defense: cloning on death.  A new value has been added to
      the CDEFENSE flag so that certain monsters can try to clone themselves
      when they have been dealt a mortal blow.  There are four types of
      cloning, each giving a different base chance to clone.  The chance is
      reduced based on how badly killed the creature is (if you "neutronize"
      the critter, he isn't as likely to clone himself).

    o The \ function has been fixed up a bit, mainly in formatting control,
      with an added piece to watch for the end of the screen and page it out
      as needed.  Also, it has been renamed to ^U, and is now a Wizard mode
      function, just as creating the Monster Dictionary is.

    o The Create Food spell/scroll has been fixed.  If an item is under you
      when you cast/read this spell, it will not be deleted as it was doing
      before.  Make sure you have a open space of floor under or around you,
      or you may go hungry...

    o The Medusa has been fixed so that it's gaze causes Turn to Stone, not
      Vampire Bite.

    o Added a new flag to the monster's spell flags.  A creature can now be
      set up so that only a percentage of that type of creature will be spell
      casters, assuming the type has any spell ability at all.  For example,
      the Young Blue Dragon could be set up so that only 1 in 10 Young Blue
      Dragons can cast spells at all.  This does not effect Dragon Breath
      attacks, only spell ability.  If a zero or one is in this position,
      all members of that creature type can cast spells, if any are indicated.


From 18862246%VUVAXCOM.BITNET@lilac.berkeley.edu Thu Aug 31 06:10:59 1989
Received: from lilac.Berkeley.EDU by ernie.Berkeley.EDU (5.61/1.36)
	id AA14710; Thu, 31 Aug 89 06:10:54 -0700
Received: by lilac.berkeley.edu (5.61.1/1.16.22)
	id AA04291; Thu, 31 Aug 89 05:54:29 PDT
Message-Id: <8908311254.AA04291@lilac.berkeley.edu>
Date:     Thu, 31 Aug 89 08:51 EST
From: "Charon, the Ferryman on the River Styx"
          <188622462%VUVAXCOM.bitnet@lilac.berkeley.edu>
Subject:  what to put for a subject?  I don't know...
To: wilson@ernie.Berkeley.EDU
Original_To:  BITNET%"wilson@ucbernie"
Status: RO


Jim-

I see what you mean about the up/down and the search flag thing.  What did
you add to the main dungeon procedure to fix this?

At the end of this letter, I've appended the dictionary descriptions of the
"Mother & Baby" and "Small Girl".  I've also include the piece from the
mon_take_hit function that checks the appropriate bit in the monster cdefense
flag.  I believe this was something written in for the 5.0 UB version, but
I'm not certain, so if you need more to be able to put it into yours, I can
send whatever you need.

That bug you mention could well be the result of someone diddling with the
sources.  Off hand, I can think of one or two reasons they'd do something to
the monster-array parts, to save on memory usage, but the file-access goes
way up the minute you do anything other than the orginial form, so you lose
out in the end.  I'll keep my eye out, but I doubt if I'll ever see it happen
here.

Sounds like you and I, and the rest of the Usenet community, are in agreement
then.  We'll keep our two separate "schools" of Moria, and just share thoughts
back and forth.

Now if only the VM/CMS Moria porter would respond to my letter...

Catch you later,

Rick


---excerpt from function mon_take_hit.  Insert within the section of if
   statement that has determined the monster has died.
   EXP is the player's current experience points.  LEV is the player's level.
   I1 is the amount of experience the creature is worth (I think it's a
   calculated value)

                {Penalize player - subtract experience}
                if (uand (cdefense, %X'00000400') <> 0) then
                  begin
                    exp := exp - 10*lev;
                    if (exp < 0) then exp := 0;
                  end
                else
                  exp := exp + i1;


--------------------------------------------
  1  Mother and Baby                (p)
     Speed =  1  Level     =  0  Exp =     0
     AC    =  1  Eye-sight =  4  HD  =   1d1
     Creature harmed by cold.
     Creature harmed by fire.
     Creature harmed by poison.
     Creature harmed by acid.
     Creature harmed by stone-to-mud.
     Causes experience loss when killed
     Creature seen with Infra-Vision.
     Creature picks up objects.
--Spells/Dragon Breath =
--Movement =
     Moves and attacks normally.
     20% random movement.
     Can open doors.
--Creature attacks =
     Touches you for normal damage. (0d0)
--------------------------------------------
  2  Small Girl                     (p)
     Speed =  1  Level     =  0  Exp =     0
     AC    =  1  Eye-sight =  4  HD  =   1d1
     Creature harmed by cold.
     Creature harmed by fire.
     Creature harmed by poison.
     Creature harmed by acid.
     Creature harmed by stone-to-mud.
     Causes experience loss when killed
     Creature seen with Infra-Vision.
     Creature picks up objects.
--Spells/Dragon Breath =
--Movement =
     Moves and attacks normally.
     20% random movement.
     Can open doors.
--Creature attacks =
     Touches you for normal damage. (0d0)
--------------------------------------------


