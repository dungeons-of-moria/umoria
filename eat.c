#include "constants.h"
#include "config.h"
#include "types.h"
#include "externs.h"


/* Eat some food...					-RAK-	*/
eat()
{
  unsigned int i;
  int j, k, item_val;
  int redraw, ident;
  register struct flags *f_ptr;
  register struct misc *m_ptr;
  register struct stats *s_ptr;
  register treasure_type *i_ptr;

  reset_flag = TRUE;
  if (inven_ctr > 0)
    {
      if (find_range(80, -1, &j, &k))
	{
	  redraw = FALSE;
	  if (get_item(&item_val, "Eat what?", &redraw, j, k))
	    {
	      i_ptr = &inventory[item_val];
	      if (redraw)  draw_cave();
	      reset_flag = FALSE;
	      i = i_ptr->flags;
	      ident = FALSE;
	      while (i != 0)
		{
		  j = bit_pos(&i) + 1;
		  /* Foods					*/
		  switch(j)
		    {
		    case 1:
		      f_ptr = &py.flags;
		      f_ptr->poisoned += randint(10) + i_ptr->level;
		      ident = TRUE;
		      break;
		    case 2:
		      f_ptr = &py.flags;
		      f_ptr->blind += randint(250) + 10*i_ptr->level + 100;
		      draw_cave();
		      msg_print("A veil of darkness surrounds you.");
		      ident = TRUE;
		      break;
		    case 3:
		      f_ptr = &py.flags;
		      f_ptr->afraid += randint(10) + i_ptr->level;
		      msg_print("You feel terrified!");
		      ident = TRUE;
		      break;
		    case 4:
		      f_ptr = &py.flags;
		      f_ptr->confused += randint(10) + i_ptr->level;
		      msg_print("You feel drugged.");
		      ident = TRUE;
		      break;
		    case 5:
		      f_ptr = &py.flags;
		      f_ptr->image += randint(200) + 25*i_ptr->level + 200;
		      ident = TRUE;
		      break;
		    case 6:
		      ident = cure_poison();
		      break;
		    case 7:
		      ident = cure_blindness();
		      break;
		    case 8:
		      f_ptr = &py.flags;
		      if (f_ptr->afraid > 1)
			{
			  f_ptr->afraid = 1;
			  ident = TRUE;
			}
		      break;
		    case 9:
		      ident = cure_confusion();
		      break;
		    case 10:
		      ident = lose_str();
		      break;
		    case 11:
		      ident = lose_con();
		      break;
		    case 12:
		      ident = lose_int();
		      break;
		    case 13:
		      ident = lose_wis();
		      break;
		    case 14:
		      ident = lose_dex();
		      break;
		    case 15:
		      ident = lose_chr();
		      break;
		    case 16:
		      s_ptr = &py.stats;
		      if (s_ptr->str > s_ptr->cstr)
			{
			  s_ptr->cstr = s_ptr->str;
			  msg_print("You feel your strength returning.");
			  prt_strength();
			  ident = TRUE;
			  /* adjust misc stats */
			  py_bonuses(blank_treasure, 0);
			}
		      break;
		    case 17:
		      s_ptr = &py.stats;
		      if (s_ptr->con > s_ptr->ccon)
			{
			  s_ptr->ccon = s_ptr->con;
			  msg_print("You feel your health returning.");
			  prt_constitution();
			  ident = TRUE;
			}
		      break;
		    case 18:
		      s_ptr = &py.stats;
		      if (py.stats.intel > s_ptr->cint)
			{
			  s_ptr->cint = py.stats.intel;
			  msg_print("Your head spins a moment.");
			  prt_intelligence();
			  ident = TRUE;
			}
		      break;
		    case 19:
		      s_ptr = &py.stats;
		      if (s_ptr->wis > s_ptr->cwis)
			{
			  s_ptr->cwis = s_ptr->wis;
			  msg_print("You feel your wisdom returning.");
			  prt_wisdom();
			  ident = TRUE;
			}
		      break;
		    case 20:
		      s_ptr = &py.stats;
		      if (s_ptr->dex > s_ptr->cdex)
			{
			  s_ptr->cdex = s_ptr->dex;
			  msg_print("You feel more dextrous.");
			  prt_dexterity();
			  ident = TRUE;
			  /* adjust misc stats */
			  py_bonuses(blank_treasure, 0);
			}
		      break;
		    case 21:
		      s_ptr = &py.stats;
		      if (s_ptr->chr > s_ptr->cchr)
			{
			  s_ptr->cchr = s_ptr->chr;
			  msg_print("Your skin starts itching.");
			  prt_charisma();
			  ident = TRUE;
			}
		      break;
		    case 22:
		      ident = hp_player(randint(3), "poisonous food.");
		      break;
		    case 23:
		      ident = hp_player(randint(6), "poisonous food.");
		      break;
		    case 24:
		      ident = hp_player(randint(12), "poisonous food.");
		      break;
		    case 25:
		      ident = hp_player(damroll("3d6"), "poisonous food.");
		      break;
		    case 26:
		      ident = hp_player(damroll("3d12"), "poisonous food.");
		      break;
		    case 27:
		      ident = hp_player(-randint(4), "poisonous food.");
		      break;
		    case 28:
		      ident = hp_player(-randint(8), "poisonous food.");
		      break;
		    case 29:
		      ident = hp_player(-damroll("2d8"), "poisonous food.");
		      break;
		    case 30:
		      ident = hp_player(-damroll("3d8"), "poisonous food.");
		      break;
		    case 31:
		      m_ptr = &py.misc;
		      m_ptr->mhp--;
		      if (m_ptr->mhp < m_ptr->chp)
			m_ptr->chp = (double)m_ptr->mhp;
		      take_hit(1, "poisonous food.");
		      prt_mhp();
		      prt_chp();
		      ident = TRUE;
		      break;
		    default:
		      break;
		    }
		  /* End of food actions...				*/
		}
	      if (ident)
		identify(inventory[item_val]);
	      if (i_ptr->flags != 0)
		{
		  m_ptr = &py.misc;
		  m_ptr->exp += ((i_ptr->level/m_ptr->lev) + .5);
		  prt_experience();
		}
	      add_food(i_ptr->p1);
	      py.flags.status = (0xFFFFFFFC & py.flags.status);
	      prt_hunger();
	      desc_remain(item_val);
	      inven_destroy(item_val);
	    }
	  else
	    if (redraw)  draw_cave();
	}
      else
	msg_print("You are not carrying any food.");
    }
  else
    msg_print("But you are not carrying anything.");
}
