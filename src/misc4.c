/* source/misc4.c: misc code for maintaining the dungeon, printing player info

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

#include	<stdio.h>

#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"

#ifndef USG
#include <sys/types.h>
#include <sys/param.h>
#endif

#ifdef USG
#ifndef ATARIST_MWC
#include <string.h>
#endif
#else
#include <strings.h>
#endif


/* Add a comment to an object description.		-CJS- */
void scribe_object()
{
  int item_val, j;
  bigvtype out_val, tmp_str;

  if (inven_ctr > 0 || equip_ctr > 0)
    {
      if (get_item(&item_val, "Which one? ", 0, INVEN_ARRAY_SIZE, CNIL, CNIL))
	{
	  objdes(tmp_str, &inventory[item_val], TRUE);
	  (void) sprintf(out_val, "Inscribing %s", tmp_str);
	  msg_print(out_val);
	  if (inventory[item_val].inscrip[0] != '\0')
	    (void) sprintf(out_val, "Replace %s New inscription:",
			   inventory[item_val].inscrip);
	  else
	    (void) strcpy(out_val, "Inscription: ");
	  j = 78 - strlen(tmp_str);
	  if (j > 12)
	    j = 12;
	  prt(out_val, 0, 0);
	  if (get_string(out_val, 0, (int)strlen(out_val), j))
	    inscribe(&inventory[item_val], out_val);
	}
    }
  else
    msg_print("You are not carrying anything to inscribe.");
}

/* Append an additional comment to an object description.	-CJS- */
void add_inscribe(i_ptr, type)
inven_type *i_ptr;
int8u type;
{
  i_ptr->ident |= type;
}

/* Replace any existing comment in an object description with a new one. CJS*/
void inscribe(i_ptr, str)
inven_type *i_ptr;
char *str;
{
  (void) strcpy(i_ptr->inscrip, str);
}


/* We need to reset the view of things.			-CJS- */
void check_view()
{
  register int i, j;
  register cave_type *c_ptr, *d_ptr;

  c_ptr = &cave[char_row][char_col];
  /* Check for new panel		   */
  if (get_panel(char_row, char_col, FALSE))
    prt_map();
  /* Move the light source		   */
  move_light(char_row, char_col, char_row, char_col);
  /* A room of light should be lit.	 */
  if (c_ptr->fval == LIGHT_FLOOR)
    {
      if ((py.flags.blind < 1) && !c_ptr->pl)
	light_room(char_row, char_col);
    }
  /* In doorway of light-room?		   */
  else if (c_ptr->lr && (py.flags.blind < 1))
    {
      for (i = (char_row - 1); i <= (char_row + 1); i++)
	for (j = (char_col - 1); j <= (char_col + 1); j++)
	  {
	    d_ptr = &cave[i][j];
	    if ((d_ptr->fval == LIGHT_FLOOR) && !d_ptr->pl)
	      light_room(i, j);
	  }
    }
}
