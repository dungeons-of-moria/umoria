/* source/help.c: identify a symbol

   Copyright (C) 1989-2008 James E. Wilson, Robert A. Koeneke, 
                           David J. Grabiner

   This file is part of Umoria.

   Umoria is free software; you can redistribute it and/or modify 
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Umoria is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of 
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License 
   along with Umoria.  If not, see <http://www.gnu.org/licenses/>. */

#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"


void ident_char()
{
  char command, query;
  register int i, n;

  if (get_com("Enter character to be identified :", &command))
    switch(command)
      {
	/* every printing ASCII character is listed here, in the order in which
	   they appear in the ASCII character set */
      case ' ': prt("  - An open pit.", 0, 0); break;
      case '!': prt("! - A potion.", 0, 0); break;
      case '"': prt("\" - An amulet, periapt, or necklace.", 0, 0); break;
      case '#': prt("# - A stone wall.", 0, 0); break;
      case '$': prt("$ - Treasure.", 0, 0); break;
      case '%':
	if (highlight_seams == TRUE)
	  prt("% - A magma or quartz vein.", 0, 0);
	else
	  prt("% - Not used.", 0, 0);
	break;
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
	/* case 'I': prt("I - Invisible Stalker.", 0, 0); break; */
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
      case 'f': prt("f - Giant Frog.", 0, 0); break;
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
      default:	prt("Not Used.", 0, 0); break;
      }

  /* Allow access to monster memory. -CJS- */
  n = 0;
  for (i = MAX_CREATURES-1; i >= 0; i--)
    if ((c_list[i].cchar == command) && bool_roff_recall (i))
      {
	if (n == 0)
	  {
	    put_buffer ("You recall those details? [y/n]", 0, 40);
	    query = inkey();
	    if (query != 'y' && query != 'Y')
	      break;
	    erase_line (0, 40);
	    save_screen ();
	  }
	n++;
	query = roff_recall (i);
	restore_screen ();
	if (query == ESCAPE)
	  break;
      }
}
