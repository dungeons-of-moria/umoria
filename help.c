#include "constants.h"
#include "config.h"
#include "types.h"
#include "externs.h"


ident_char()
{
  char command;

  if (get_com("Enter character to be identified :", &command))
    switch(command)
      {
      case ' ': prt("  - An open pit.", 0, 0); break;
      case '!': prt("! - A potion.", 0, 0); break;
      case '"': prt("\" - An amulet, periapt, or necklace.", 0, 0); break;
      case '#': prt("# - A stone wall.", 0, 0); break;
      case '$': prt("$ - Treasure.", 0, 0); break;
	/* case '%': prt("% - Not used.", 0, 0); break; */
      case '&': prt("& - Treasure chest.", 0, 0); break;
      case '\'': prt("' - An open door.", 0, 0); break;
      case '(': prt("( - Soft armor.", 0, 0); break;
      case ')': prt(") - A shield.", 0, 0); break;
      case '*': prt("* - Gems.", 0, 0); break;
      case '+': prt("+ - A closed door.", 0, 0); break;
      case ',': prt(", - Food or mushroom patch.", 0, 0); break;
      case '-': prt("- - A wand", 0, 0); break;
      case '.': prt(". - Floor.", 0, 0); break;
      case '/': prt("/ - A pole weapon.", 0, 0); break;
	/* case '0': prt("0 - Not used.", 0, 0); break; */
      case '1': prt("1 - Entrance to General Store.", 0, 0); break;
      case '2': prt("2 - Entrance to Armory.", 0, 0); break;
      case '3': prt("3 - Entrance to Weaponsmith.", 0, 0); break;
      case '4': prt("4 - Entrance to Temple.", 0, 0); break;
      case '5': prt("5 - Entrance to Alchemy shop.", 0, 0); break;
      case '6': prt("6 - Entrance to Magic-Users store.", 0, 0); break;
	/* case '7': prt("7 - Not used.", 0, 0); break; */
	/* case '8': prt("8 - Not used.", 0, 0); break; */
	/* case '9': prt("9 - Not used.", 0, 0);  break;*/
      case ':': prt(": - Rubble.", 0, 0); break;
      case ';': prt("; - A loose rock.", 0, 0); break;
      case '<': prt("< - An up staircase.", 0, 0); break;
      case '=': prt("= - A ring.", 0, 0); break;
      case '>': prt("> - A down staircase.", 0, 0); break;
      case '?': prt("? - A scroll.", 0, 0); break;
      case '@': prt(py.misc.name, 0, 0); break;
      case 'A': prt("A - Giant Ant Lion.", 0, 0); break;
      case 'B': prt("B - The Balrog.", 0, 0); break;
      case 'C': prt("C - Gelatinous Cube.", 0, 0); break;
      case 'D': prt("D - An Ancient Dragon (Beware).", 0, 0); break;
      case 'E': prt("E - Elemental.", 0, 0); break;
      case 'F': prt("F - Giant Fly.", 0, 0); break;
      case 'G': prt("G - Ghost.", 0, 0); break;
      case 'H': prt("H - Hobgoblin.", 0, 0); break;
      case 'I': prt("I - Invisible Stalker.", 0, 0); break;
      case 'J': prt("J - Jelly.", 0, 0); break;
      case 'K': prt("K - Killer Beetle.", 0, 0); break;
      case 'L': prt("L - Lich.", 0, 0); break;
      case 'M': prt("M - Mummy.", 0, 0); break;
	/* case 'N': prt("N - Not used.", 0, 0); break; */
      case 'O': prt("O - Ooze.", 0, 0); break;
      case 'P': prt("P - Giant humanoid.", 0, 0); break;
      case 'Q': prt("Q - Quylthulg (Pulsing Flesh Mound).", 0, 0); break;
      case 'R': prt("R - Reptile.", 0, 0); break;
      case 'S': prt("S - Giant Scorpion.", 0, 0); break;
      case 'T': prt("T - Troll.", 0, 0); break;
      case 'U': prt("U - Umber Hulk.", 0, 0); break;
      case 'V': prt("V - Vampire.", 0, 0); break;
      case 'W': prt("W - Wight or Wraith.", 0, 0); break;
      case 'X': prt("X - Xorn.", 0, 0); break;
      case 'Y': prt("Y - Yeti.", 0, 0); break;
	/* case 'Z': prt("Z - Not used.", 0, 0); break; */
      case '[': prt("[ - Hard armor.", 0, 0); break;
      case '\\': prt("\\ - A hafted weapon.", 0, 0); break;
      case ']': prt("] - Misc. armor.", 0, 0); break;
      case '^': prt("^ - A trap.", 0, 0); break;
      case '_': prt("_ - A staff.", 0, 0); break;
	/* case '`': prt("` - Not used.", 0, 0); break; */
      case 'a': prt("a - Giant Ant.", 0, 0); break;
      case 'b': prt("b - Giant Bat.", 0, 0); break;
      case 'c': prt("c - Giant Centipede.", 0, 0); break;
      case 'd': prt("d - Dragon.", 0, 0); break;
      case 'e': prt("e - Floating Eye.", 0, 0); break;
      case 'f': prt("f - Giant Frog", 0, 0); break;
      case 'g': prt("g - Golem.", 0, 0); break;
      case 'h': prt("h - Harpy.", 0, 0); break;
      case 'i': prt("i - Icky Thing.", 0, 0); break;
      case 'j': prt("j - Jackal.", 0, 0); break;
      case 'k': prt("k - Kobold.", 0, 0); break;
      case 'l': prt("l - Giant Louse.", 0, 0); break;
      case 'm': prt("m - Mold.", 0, 0); break;
      case 'n': prt("n - Naga.", 0, 0); break;
      case 'o': prt("o - Orc or Ogre.", 0, 0); break;
      case 'p': prt("p - Person (Humanoid).", 0, 0); break;
      case 'q': prt("q - Quasit.", 0, 0); break;
      case 'r': prt("r - Rodent.", 0, 0); break;
      case 's': prt("s - Skeleton.", 0, 0); break;
      case 't': prt("t - Giant Tick.", 0, 0); break;
	/* case 'u': prt("u - Not used.", 0, 0); break; */
	/* case 'v': prt("v - Not used.", 0, 0); break; */
      case 'w': prt("w - Worm or Worm Mass.", 0, 0); break;
	/* case 'x': prt("x - Not used.", 0, 0); break; */
      case 'y': prt("y - Yeek.", 0, 0); break;
      case 'z': prt("z - Zombie.", 0, 0); break;
      case '{': prt("{ - Arrow, bolt, or bullet.", 0, 0); break;
      case '|': prt("| - A sword or dagger.", 0, 0); break;
      case '}': prt("} - Bow, crossbow, or sling.", 0, 0); break;
      case '~': prt("~ - Miscellaneous item.", 0, 0); break;
      default:  prt("Not Used.", 0, 0); break;
      }
}

/* Help for available original commands					*/
original_help()
{
  clear_screen(0, 0);
  prt("B <Dir> Bash (object/creature)|  q        Quaff a potion.", 0, 0);
  prt("C       Display character.    |  r        Read a scroll.", 1, 0);
  prt("D <Dir> Disarm a trap/chest.  |  s        Search for hidden doors.",
      2, 0);
  prt("E       Eat some food.        |  t        Take off an item.", 3, 0);
  prt("F       Fill lamp with oil.   |  u        Use a staff.", 4, 0);
  prt("L       Current location.     |  v        Version and credits.", 5, 0);
  prt("P       Print map.            |  w        Wear/Wield an item.", 6, 0);
  prt("R       Rest for a period.    |  x        Exchange weapon.", 7, 0);
  prt("S       Search Mode.          |  /        Identify a character.", 8,0);
  prt("T <Dir> Tunnel.               |  ?        Display this panel.", 9, 0);
  prt("a       Aim and fire a wand.  |", 10, 0);
  prt("b       Browse a book.        |  ^P       Repeat the last message.",
      11, 0);
  prt("c <Dir> Close a door.         |  ^R       Redraw the screen.", 12, 0);
  prt("d       Drop an item.         |  ^K       Quit the game.", 13, 0);
  prt("e       Equipment list.       |  ^X       Save character and quit.",
      14, 0);
  prt("f       Fire/Throw an item.   |   $       Shell out of game.", 15, 0);
/*  prt("h       Help on key commands. |", 16, 0); */
  prt("i       Inventory list.       |  < Go up an up-staircase.", 17, 0);
  prt("j <Dir> Jam a door with spike.|  > Go down a down-staircase.", 18, 0);
  prt("l <Dir> Look given direction. |  . <Dir>  Move in direction.", 19, 0);
  prt("m       Cast a magic spell.   |  Movement: 7  8  9", 20, 0);
  prt("o <Dir> Open a door/chest.    |            4     6    5 == Rest",
      21, 0);
  prt("p       Read a prayer.        |            1  2  3", 22, 0);
  pause_line(23);
  draw_cave();
}


/* Help for available rogue_like commands				*/
rogue_like_help()
{
  clear_screen(0, 0);
  prt("f <Dir> Bash (object/creature)|  q        Quaff a potion.", 0, 0);
  prt("C       Display character.    |  r        Read a scroll.", 1, 0);
  prt("D <Dir> Disarm a trap/chest.  |  s        Search for hidden doors.",
      2, 0);
  prt("E       Eat some food.        |  T        Take off an item.", 3, 0);
  prt("F       Fill lamp with oil.   |  Z        Use a staff.", 4, 0);
  prt("W       Current location.     |  v        Version and credits.", 5, 0);
  prt("M       Print map.            |  w        Wear/Wield an item.", 6, 0);
  prt("R       Rest for a period.    |  X        Exchange weapon.", 7, 0);
  prt("#       Search Mode.          |  /        Identify a character.", 8,0);
  prt("CTRL <dir> Tunnel             |  ?        Display this panel.", 9, 0);
  prt("z       Aim and fire a wand.  |", 10, 0);
  prt("P       Browse a book.        |  ^P       Repeat the last message.",
      11, 0);
  prt("c <Dir> Close a door.         |  ^R       Redraw the screen.", 12, 0);
  prt("d       Drop an item.         |  Q        Quit the game.", 13, 0);
  prt("e       Equipment list.       |  ^X       Save character and quit.",
      14, 0);
  prt("t       Fire/Throw an item.   |   !       Shell out of game.", 15, 0);
/*  prt("h       Help on key commands. |", 16, 0); */
  prt("i       Inventory list.       |  < Go up an up-staircase.", 17, 0);
  prt("S <Dir> Jam a door with spike.|  > Go down a down-staircase.", 18, 0);
  prt("x <Dir> Look given direction. |  SHIFT <Dir>  Move in direction.", 19, 0);
  prt("m       Cast a magic spell.   |  Movement: y  k  u", 20, 0);
  prt("o <Dir> Open a door/chest.    |            h     l    . == Rest",
      21, 0);
  prt("p       Read a prayer.        |            b  j  n", 22, 0);
  pause_line(23);
  draw_cave();
}


/* Help for available wizard commands				*/
original_wizard_help()
{
  clear_screen(0, 0);
  if (wizard2)
    {
      prt("^A - Remove Curse and Cure all maladies.", 0, 0);
      prt("^B - Print random objects sample.", 1, 0);
      prt("^D - Down/Up n levels.", 2, 0);
      prt("^E - Change character.", 3, 0);
      prt("^F - Delete monsters.", 4, 0);
      prt("^G - Allocate treasures.", 5, 0);
      prt("^H - Wizard Help.", 6, 0);
      prt("^I - Identify.", 7, 0);
      prt("^J - Gain experience.", 8, 0);
      prt("^U - Summon monster.", 9, 0);
      prt("^L - Wizard light.", 10, 0);
      prt("^N - Print monster dictionary.", 11, 0);
      prt("^W - Wizard password on/off.", 12, 0);
      prt("^T - Teleport self.", 13, 0);
      prt("^V - Restore lost character.", 14, 0);
      /* starting to run out of characters here!! */
      prt("@  - Create any object *CAN CAUSE FATAL ERROR*", 15, 0);
    }
  else
    {
      prt("^A - Remove Curse and Cure all maladies.", 0, 0);
      prt("^B - Print random objects sample.", 1, 0);
      prt("^D - Down/Up n levels.", 2, 0);
      prt("^H - Wizard Help.", 3, 0);
      prt("^I - Identify.", 4, 0);
      prt("^L - Wizard light.", 5, 0);
      prt("^N - Print monster dictionary.", 6, 0);
      prt("^W - Wizard password on/off.", 7, 0);
      prt("^T - Teleport self.", 8, 0);
      prt("^V - Restore lost character.", 9, 0);
    }
  pause_line(23);
  draw_cave();
}


/* Help for available wizard commands				*/
rogue_like_wizard_help()
{
  clear_screen(0, 0);
  if (wizard2)
    {
      prt("^A - Remove Curse and Cure all maladies.", 0, 0);
      prt("^O - Print random objects sample.", 1, 0);
      prt("^D - Down/Up n levels.", 2, 0);
      prt("^E - Change character.", 3, 0);
      prt("^F - Delete monsters.", 4, 0);
      prt("^G - Allocate treasures.", 5, 0);
      prt("^? - Wizard Help.", 6, 0);
      prt("^I - Identify.", 7, 0);
      prt("+  - Gain experience.", 8, 0);
      prt("^S - Summon monster.", 9, 0);
      prt("*  - Wizard light.", 10, 0);
      prt("^M - Print monster dictionary.", 11, 0);
      prt("^W - Wizard password on/off.", 12, 0);
      prt("^T - Teleport self.", 13, 0);
      prt("^V - Restore lost character.", 14, 0);
      /* starting to run out of characters here!! */
      prt("@  - Create any object *CAN CAUSE FATAL ERROR*", 15, 0);
    }
  else
    {
      prt("^A - Remove Curse and Cure all maladies.", 0, 0);
      prt("^O - Print random objects sample.", 1, 0);
      prt("^D - Down/Up n levels.", 2, 0);
      prt("^? - Wizard Help.", 3, 0);
      prt("^I - Identify.", 4, 0);
      prt("*  - Wizard light.", 5, 0);
      prt("^M - Print monster dictionary.", 6, 0);
      prt("^W - Wizard password on/off.", 7, 0);
      prt("^T - Teleport self.", 8, 0);
      prt("^V - Restore lost character.", 9, 0);
    }
  pause_line(23);
  draw_cave();
}
