#include "constants.h"
#include "config.h"
#include "types.h"
#include "externs.h"


/* Use a staff...					-RAK-	*/
use()
{
  unsigned int i;
  int j, k, item_val, chance;
  int y, x;
  int redraw, ident;
  register struct misc *m_ptr;
  register treasure_type *i_ptr;

  reset_flag = TRUE;
  if (inven_ctr > 0)
    {
      if (find_range(55, -1, &j, &k))
	{
	  redraw = FALSE;
	  if (get_item(&item_val, "Use which staff?", &redraw, j, k))
	    {
	      i_ptr = &inventory[item_val];
	      if (redraw)  draw_cave();
	      reset_flag = FALSE;
	      m_ptr = &py.misc;
	      chance = m_ptr->save + m_ptr->lev + int_adj() - i_ptr->level - 5;
	      if (py.flags.confused > 0)
		chance = chance / 2.0;
	      if (chance <= 0)  chance = 1;
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
		      /* Staffs...				*/
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
			  ident = earthquake();
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
			case 9:
			  ident = genocide();
			  break;
			case 10:
			  ident = destroy_area(char_row, char_col);
			  break;
			case 11:
			  ident = starlite(char_row, char_col);
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
			  ident = hp_player(randint(8), "a staff.");
			  break;
			case 16:
			  ident = detect_invisible();
			  break;
			case 17:
			  py.flags.fast += randint(30) + 15;
			  ident = TRUE;
			  break;
			case 18:
			  py.flags.slow += randint(30) + 15;
			  ident = TRUE;
			  break;
			case 19:
			  ident = mass_poly();
			  break;
			case 20:
			  if (remove_curse())
			    {
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
			  ident = dispell_creature(0x0004, 60);
			  break;
			case 24:
			  ident = mass_genocide();
			  break;
			case 25:
			  ident = unlight_area(char_row, char_col);
			  break;
			default:
			  break;
			}
		      /* End of staff actions...		*/
		    }
		  if (ident)
		    identify(inventory[item_val]);
		  if (i_ptr->flags != 0)
		    {
		      m_ptr = &py.misc;
		      m_ptr->exp += ((i_ptr->level/m_ptr->lev) + 0.5);
		      prt_experience();
		    }
		  desc_charges(item_val);
		}
	      else
		msg_print("The staff has no charges left.");
	    }
	  else
	    if (redraw)  draw_cave();
	}
      else
	msg_print("You are not carrying any staffs.");
    }
  else
    msg_print("But you are not carrying anything.");
}
