#include "constants.h"
#include "config.h"
#include "types.h"
#include "externs.h"


/* Potions for the quaffing				-RAK-	*/
quaff()
{
  unsigned int i;
  int j, k, l, item_val;
  int redraw, ident;
  register treasure_type *i_ptr;
  register struct misc *m_ptr;
  register struct flags *f_ptr;
  register struct stats *s_ptr;
  register class_type *c_ptr;

  reset_flag = TRUE;
  if (inven_ctr > 0)
    {
      if (find_range(75, 76, &j, &k))
	{
	  redraw = FALSE;
	  if (get_item(&item_val, "Quaff which potion?", &redraw, j, k))
	    {
	      i_ptr = &inventory[item_val];
	      if (redraw)  draw_cave();
	      reset_flag = FALSE;
	      i = i_ptr->flags;
	      ident = FALSE;
	      while (i != 0)
		{
		  j = bit_pos(&i) + 1;
		  if (i_ptr->tval == 76)
		    j += 31;
		  /* Potions						*/
		  switch(j)
		    {
		    case 1:
		      s_ptr = &py.stats;
		      s_ptr->cstr = in_statp(s_ptr->cstr);
		      if (s_ptr->cstr > s_ptr->str)
			s_ptr->str = s_ptr->cstr;
		      msg_print("Wow!  What bulging muscles!");
		      prt_strength();
		      ident = TRUE;
		      /* adjust misc stats */
		      py_bonuses(blank_treasure, 0);
		      break;
		    case 2:
		      ident = lose_str();
		      break;
		    case 3:
		      s_ptr = &py.stats;
		      s_ptr->cstr = s_ptr->str;
		      msg_print("You feel warm all over.");
		      prt_strength();
		      ident = TRUE;
		      /* adjust misc stats */
		      py_bonuses(blank_treasure, 0);
		      break;
		    case 4:
		      s_ptr = &py.stats;
		      s_ptr->cint = in_statp(s_ptr->cint);
		      if (s_ptr->cint > py.stats.intel)
			py.stats.intel = s_ptr->cint;
		      msg_print("Aren't you brilliant!");
		      prt_intelligence();
		      ident = TRUE;
		      break;
		    case 5:
		      msg_print("This potion tastes very dull.");
		      ident = lose_int();
		      break;
		    case 6:
		      s_ptr = &py.stats;
		      s_ptr->cint = py.stats.intel;
		      msg_print("You have have a warm feeling.");
		      prt_intelligence();
		      ident = TRUE;
		      break;
		    case 7:
		      s_ptr = &py.stats;
		      s_ptr->cwis = in_statp(s_ptr->cwis);
		      if (s_ptr->cwis > s_ptr->wis)
			s_ptr->wis = s_ptr->cwis;
		      msg_print("You suddenly have a profound thought!");
		      prt_wisdom();
		      ident = TRUE;
		      break;
		    case 8:
		      ident = lose_wis();
		      break;
		    case 9:
		      s_ptr = &py.stats;
		      if (s_ptr->cwis < s_ptr->wis)
			{
			  s_ptr->cwis = s_ptr->wis;
			  msg_print("You feel your wisdom returning.");
			  prt_wisdom();
			  ident = TRUE;
			}
		      break;
		    case 10:
		      s_ptr = &py.stats;
		      s_ptr->cchr = in_statp(s_ptr->cchr);
		      if (s_ptr->cchr > s_ptr->chr)
			s_ptr->chr = s_ptr->cchr;
		      msg_print("Gee,  ain't you cute!");
		      prt_charisma();
		      ident = TRUE;
		      break;
		    case 11:
		      ident = lose_chr();
		      break;
		    case 12:
		      s_ptr = &py.stats;
		      if (s_ptr->cchr < s_ptr->chr)
			{
			  s_ptr->cchr = s_ptr->chr;
			  msg_print("You feel your looks returning.");
			  prt_charisma();
			  ident = TRUE;
			}
		      break;
		    case 13:
		      ident = hp_player(damroll("2d7"), "a potion.");
		      break;
		    case 14:
		      ident = hp_player(damroll("4d7"), "a potion.");
		      break;
		    case 15:
		      ident = hp_player(damroll("6d7"), "a potion.");
		      break;
		    case 16:
		      ident = hp_player(1000, "a potion.");
		      break;
		    case 17:
		      m_ptr = &py.misc;
		      py.stats.ccon = in_statp(py.stats.ccon);
		      if (py.stats.ccon > py.stats.con)
			py.stats.con = py.stats.ccon;
		      m_ptr->mhp++;
		      m_ptr->chp += m_ptr->mhp;
		      msg_print("You feel tingly for a moment.");
		      prt_mhp();
		      prt_chp();
		      prt_constitution();
		      ident = TRUE;
		      break;
		    case 18:
		      m_ptr = &py.misc;
		      l = (m_ptr->exp / 2) + 10;
		      if (l > 100000)  l = 100000;
		      m_ptr->exp += l;
		      msg_print("You feel more experienced.");
		      prt_experience();
		      ident = TRUE;
		      break;
		    case 19:
		      f_ptr = &py.flags;
		      if (!f_ptr->free_act)
			{
			  msg_print("You fall asleep.");
			  f_ptr->paralysis += randint(4) + 4;
			  ident = TRUE;
			}
		      break;
		    case 20:
		      f_ptr = &py.flags;
		      msg_print("You are covered by a veil of darkness.");
		      f_ptr->blind += randint(100) + 100;
		      ident = TRUE;
		      break;
		    case 21:
		      f_ptr = &py.flags;
		      msg_print("Hey!  This is good stuff!  * Hick! *");
		      f_ptr->confused += randint(20) + 12;
		      ident = TRUE;
		      break;
		    case 22:
		      f_ptr = &py.flags;
		      msg_print("You feel very sick.");
		      f_ptr->poisoned += randint(15) + 10;
		      ident = TRUE;
		      break;
		    case 23:
		      py.flags.fast += randint(25) + 15;
		      ident = TRUE;
		      break;
		    case 24:
		      py.flags.slow += randint(25) + 15;
		      ident = TRUE;
		      break;
		    case 25:
		      ident = detect_monsters();
		      break;
		    case 26:
		      s_ptr = &py.stats;
		      s_ptr->cdex = in_statp(s_ptr->cdex);
		      if (s_ptr->cdex > s_ptr->dex)
			s_ptr->dex = s_ptr->cdex;
		      msg_print("You feel more limber!");
		      prt_dexterity();
		      ident = TRUE;
		      /* adjust misc stats */
		      py_bonuses(blank_treasure, 0);
		      break;
		    case 27:
		      s_ptr = &py.stats;
		      if (s_ptr->cdex < s_ptr->dex)
			{
			  s_ptr->cdex = s_ptr->dex;
			  msg_print("You feel less clumsy.");
			  prt_dexterity();
			  ident = TRUE;
			  /* adjust misc stats */
			  py_bonuses(blank_treasure, 0);
			}
		      break;
		    case 28:
		      s_ptr= &py.stats;
		      if (s_ptr->ccon < s_ptr->con)
			{
			  s_ptr->ccon = s_ptr->con;
			  msg_print("You feel your health returning!");
			  prt_constitution();
			  ident = TRUE;
			}
		      break;
		    case 29:
		      (void) cure_blindness();
		      break;
		    case 30:
		      (void) cure_confusion();
		      break;
		    case 31:
		      (void) cure_poison();
		      break;
		    case 32:
		      m_ptr = &py.misc;
		      c_ptr = &class[m_ptr->pclass];
		      if (c_ptr->mspell)
			{
			  ident = learn_spell(&redraw);
			  if (redraw)  draw_cave();
			}
		      else if (c_ptr->pspell)
			ident = learn_prayer();
		      break;
		    case 33:
		      msg_print("You feel your memories fade...");
		      /* avoid randint(0) call */
		      l = (py.misc.exp/5.0);
		      if (l == 0)
			lose_exp(1);
		      else
			lose_exp(randint(l)+l);
		      ident = TRUE;
		      break;
		    case 34:
		      f_ptr = &py.flags;
		      (void) cure_poison();
		      if (f_ptr->food > 150)  f_ptr->food = 150;
		      f_ptr->paralysis = 4;
		      msg_print("The potion makes you vomit!");
		      ident = TRUE;
		      break;
		    case 35:
		      py.flags.invuln += randint(10) + 10;
		      ident = TRUE;
		      break;
		    case 36:
		      py.flags.hero += randint(25) + 25;
		      ident = TRUE;
		      break;
		    case 37:
		      py.flags.shero += randint(25) + 25;
		      ident = TRUE;
		      break;
		    case 38:
		      ident = remove_fear();
		      break;
		    case 39:
		      ident = restore_level();
		      break;
		    case 40:
		      f_ptr = &py.flags;
		      f_ptr->resist_heat += randint(10) + 10;
		      break;
		    case 41:
		      f_ptr = &py.flags;
		      f_ptr->resist_cold += randint(10) + 10;
                      break;
		    case 42:
		      detect_inv2(randint(12)+12);
		      break;
		    case 43:
		      ident = slow_poison();
		      break;
		    case 44:
		      ident = cure_poison();
		      break;
		    case 45:
		      m_ptr = &py.misc;
		      if (m_ptr->cmana < m_ptr->mana)
			{
			  m_ptr->cmana = (double)m_ptr->mana;
			  ident = TRUE;
			  msg_print("Your feel your head clear...");
			}
		      break;
		    case 46:
		      f_ptr = &py.flags;
		      f_ptr->tim_infra += 100 + randint(100);
		      ident = TRUE;
		      msg_print("Your eyes begin to tingle.");
		      break;
		    case 47:
		      break;
		    case 48:
		      break;
		    case 49:
		      break;
		    case 50:
		      break;
		    case 51:
		      break;
		    case 52:
		      break;
		    case 53:
		      break;
		    case 54:
		      break;
		    case 55:
		      break;
		    case 56:
		      break;
		    case 57:
		      break;
		    case 58:
		      break;
		    case 59:
		      break;
		    case 60:
		      break;
		    case 61:
		      break;
		    case 62:
		      break;
		    default:
		      break;
		    }
		  /* End of Potions...					*/
		}
	      if (ident)
		identify(inventory[item_val]);
	      if (i_ptr->flags != 0)
		{
		  m_ptr = &py.misc;
		  m_ptr->exp += (int)(i_ptr->level/m_ptr->lev + 0.5);
		  prt_experience();
		}
	      add_food(i_ptr->p1);
	      desc_remain(item_val);
	      inven_destroy(item_val);
	    }
	  else
	    if (redraw)  draw_cave();
	}
      else
	msg_print("You are not carrying any potions.");
    }
  else
    msg_print("But you are not carrying anything.");
}
