#include "constants.h"
#include "config.h"
#include "types.h"
#include "externs.h"


/* Wands for the aiming...				*/
aim()
{
  unsigned int i;
  int j, k, l, chance;
  int dir, item_val;
  int dumy, y_dumy, x_dumy;
  int redraw, ident;
  register treasure_type *i_ptr;
  register struct misc *m_ptr;

  redraw = FALSE;
  reset_flag = TRUE;
  if (inven_ctr > 0)
    {
      if (find_range(65, -1, &j, &k))
	{
	  if (get_item(&item_val, "Aim which wand?", &redraw, j, k))
	    {
	      i_ptr = &inventory[item_val];
	      if (redraw)  draw_cave();
	      reset_flag = FALSE;
	      redraw = FALSE;
	      y_dumy = char_row;
	      x_dumy = char_col;
	      if (get_dir("Which direction?", &dir, &dumy, &y_dumy, &x_dumy))
		{
		  if (py.flags.confused > 0)
		    {
		      msg_print("You are confused...");
		      do
			{
			  dir = randint(9);
			}
		      while (dir == 5);
		    }
		  i = i_ptr->flags;
		  ident = FALSE;
		  m_ptr = &py.misc;
		  chance = m_ptr->save + m_ptr->lev + int_adj() - i_ptr->level;
		  if (py.flags.confused > 0)
		    chance = chance / 2.0;
		  if (chance <= 0)  chance = 1;
		  if (randint(chance) < USE_DEVICE)
		    msg_print("You failed to use the wand properly.");
		  else if (i_ptr->p1 > 0)
		    {
		      (i_ptr->p1)--;
		      while (i != 0)
			{
			  j = bit_pos(&i) + 1;
			  k = char_row;
			  l = char_col;
			  /* Wands                       */
			  switch(j)
			    {
			    case 1:
			 msg_print("A line of blue shimmering light appears.");
			      light_line(dir, char_row, char_col);
			      ident = TRUE;
			      break;
			    case 2:
			      fire_bolt(1, dir, k, l, damroll("3d8"),
					"Lightning Bolt");
			      ident = TRUE;
			      break;
			    case 3:
			      fire_bolt(4, dir, k, l, damroll("4d8"),
					"Frost Bolt");
			      ident = TRUE;
			      break;
			    case 4:
			      fire_bolt(5, dir, k, l, damroll("6d8"),
					"Fire Bolt");
			      ident = TRUE;
			      break;
			    case 5:
			      ident = wall_to_mud(dir, k, l);
			      break;
			    case 6:
			      ident = poly_monster(dir, k, l);
			      break;
			    case 7:
			      ident = hp_monster(dir, k, l, -damroll("4d6"));
			      break;
			    case 8:
			      ident = speed_monster(dir, k, l, 1);
			      break;
			    case 9:
			      ident = speed_monster(dir, k, l, -1);
			      break;
			    case 10:
			      ident = confuse_monster(dir, k, l);
			      break;
			    case 11:
			      ident = sleep_monster(dir, k, l);
			      break;
			    case 12:
			      ident = drain_life(dir, k, l);
			      break;
			    case 13:
			      ident = td_destroy2(dir, k, l);
			      break;
			    case 14:
			      fire_bolt(0, dir, k, l, damroll("2d6"),
					"Magic Missile");
			      ident = TRUE;
			      break;
			    case 15:
			      ident = build_wall(dir, k, l);
			      break;
			    case 16:
			      ident = clone_monster(dir, k, l);
			      break;
			    case 17:
			      ident = teleport_monster(dir, k, l);
			      break;
			    case 18:
			      ident = disarm_all(dir, k, l);
			      break;
			    case 19:
			      fire_ball(1, dir, k, l, 24, "Lightning Ball");
			      ident = TRUE;
			      break;
			    case 20:
			      fire_ball(4, dir, k, l, 32, "Cold Ball");
			      ident = TRUE;
			      break;
			    case 21:
			      fire_ball(5, dir, k, l, 48, "Fire Ball");
			      ident = TRUE;
			      break;
			    case 22:
			      fire_ball(2, dir, k, l, 8, "Stinking Cloud");
			      ident = TRUE;
			      break;
			    case 23:
			      fire_ball(3, dir, k, l, 40, "Acid Ball");
			      ident = TRUE;
			      break;
			    case 24:
			      i = 2 << (randint(23) - 1);
			      break;
			    default:
			      break;
			    }
			  /* End of Wands...                  */
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
		    msg_print("The wand has no charges left.");
		}
	    }
	}
      else
	msg_print("You are not carrying any wands.");
    }
  else
    msg_print("But you are not carrying anything.");
  if (redraw)  draw_cave();
}
