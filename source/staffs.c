/* staffs.c: staff code

   Copyright (c) 1989 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include "constant.h"
#include "config.h"
#include "types.h"
#include "externs.h"

#ifdef USG
#include <string.h>
#else
#include <strings.h>
#endif


/* Use a staff.					-RAK-	*/
void use()
{
  int32u i;
  int j, k, item_val, chance, y, x;
  register int ident;
  register struct misc *m_ptr;
  register inven_type *i_ptr;

  free_turn_flag = TRUE;
  if (inven_ctr == 0)
    msg_print("But you are not carrying anything.");
  else if (!find_range(TV_STAFF, TV_NEVER, &j, &k))
    msg_print("You are not carrying any staffs.");
  else if (get_item(&item_val, "Use which staff?", j, k))
    {
      i_ptr = &inventory[item_val];
      free_turn_flag = FALSE;
      m_ptr = &py.misc;
      chance = m_ptr->save + stat_adj(A_INT) - (int)i_ptr->level - 5
	+ (class_level_adj[m_ptr->pclass][CLA_DEVICE] * m_ptr->lev / 3);
      if (py.flags.confused > 0)
	chance = chance / 2;
      if (chance <= 0)	chance = 1;
      if (randint(chance) < USE_DEVICE)
	msg_print("You failed to use the staff properly.");
      else if (i_ptr->p1 > 0)
	{
	  i = i_ptr->flags;
	  ident = FALSE;
	  (i_ptr->p1)--;
	  while (i != 0)
	    {
	      j = bit_pos(&i) + 1;
	      /* Staffs.				*/
	      switch(j)
		{
		case 1:
		  ident = light_area(char_row, char_col);
		  break;
		case 2:
		  ident = detect_sdoor();
		  break;
		case 3:
		  ident = detect_trap();
		  break;
		case 4:
		  ident = detect_treasure();
		  break;
		case 5:
		  ident = detect_object();
		  break;
		case 6:
		  teleport(100);
		  ident = TRUE;
		  break;
		case 7:
		  ident = TRUE;
		  earthquake();
		  break;
		case 8:
		  ident = FALSE;
		  for (k = 0; k < randint(4); k++)
		    {
		      y = char_row;
		      x = char_col;
		      ident |= summon_monster(&y, &x, FALSE);
		    }
		  break;
		case 10:
		  ident = TRUE;
		  destroy_area(char_row, char_col);
		  break;
		case 11:
		  ident = TRUE;
		  starlite(char_row, char_col);
		  break;
		case 12:
		  ident = speed_monsters(1);
		  break;
		case 13:
		  ident = speed_monsters(-1);
		  break;
		case 14:
		  ident = sleep_monsters2();
		  break;
		case 15:
		  ident = hp_player(randint(8));
		  break;
		case 16:
		  ident = detect_invisible();
		  break;
		case 17:
		  if (py.flags.fast == 0)
		    ident = TRUE;
		  py.flags.fast += randint(30) + 15;
		  break;
		case 18:
		  if (py.flags.slow == 0)
		    ident = TRUE;
		  py.flags.slow += randint(30) + 15;
		  break;
		case 19:
		  ident = mass_poly();
		  break;
		case 20:
		  if (remove_curse())
		    {
		      if (py.flags.blind < 1)
			msg_print("The staff glows blue for a moment..");
		      ident = TRUE;
		    }
		  break;
		case 21:
		  ident = detect_evil();
		  break;
		case 22:
		  if ((cure_blindness()) || (cure_poison()) ||
		      (cure_confusion()))
		    ident = TRUE;
		  break;
		case 23:
		  ident = dispel_creature(CD_EVIL, 60);
		  break;
		case 25:
		  ident = unlight_area(char_row, char_col);
		  break;
		case 32:
		  /* store bought flag */
		  break;
		default:
		  msg_print("Internal error in staffs()");
		  break;
		}
	      /* End of staff actions.		*/
	    }
	  if (ident)
	    {
	      if (!known1_p(i_ptr))
		{
		  m_ptr = &py.misc;
		  /* round half-way case up */
		  m_ptr->exp += (i_ptr->level + (m_ptr->lev >> 1)) /
		    m_ptr->lev;
		  prt_experience();

		  identify(&item_val);
		  i_ptr = &inventory[item_val];
		}
	    }
	  else if (!known1_p(i_ptr))
	    sample (i_ptr);
	  desc_charges(item_val);
	}
      else
	{
	  msg_print("The staff has no charges left.");
	  if (!known2_p(i_ptr))
	    add_inscribe(i_ptr, ID_EMPTY);
	}
    }
}
