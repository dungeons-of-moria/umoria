/* potions.c: code for potions

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

/* Potions for the quaffing				-RAK-	*/
void quaff()
{
  int32u i, l;
  int j, k, item_val;
  int ident;
  register inven_type *i_ptr;
  register struct misc *m_ptr;
  register struct flags *f_ptr;
#if 0
  /* used by ifdefed out learn_spell potion */
  register class_type *c_ptr;
#endif

  free_turn_flag = TRUE;
  if (inven_ctr == 0)
    msg_print("But you are not carrying anything.");
  else if (!find_range(TV_POTION1, TV_POTION2, &j, &k))
    msg_print("You are not carrying any potions.");
  else if (get_item(&item_val, "Quaff which potion?", j, k))
    {
      i_ptr = &inventory[item_val];
      i = i_ptr->flags;
      free_turn_flag = FALSE;
      ident = FALSE;
      if (i == 0)
	{
	  msg_print ("You feel less thirsty.");
	  ident = TRUE;
	}
      else while (i != 0)
	{
	  j = bit_pos(&i) + 1;
	  if (i_ptr->tval == TV_POTION2)
	    j += 32;
	  /* Potions						*/
	  switch(j)
	    {
	    case 1:
	      if (inc_stat (A_STR))
		{
		  msg_print("Wow!  What bulging muscles!");
		  ident = TRUE;
		}
	      break;
	    case 2:
	      ident = TRUE;
	      lose_str();
	      break;
	    case 3:
	      if (res_stat (A_STR))
		{
		  msg_print("You feel warm all over.");
		  ident = TRUE;
		}
	      break;
	    case 4:
	      if (inc_stat (A_INT))
		{
		  msg_print("Aren't you brilliant!");
		  ident = TRUE;
		}
	      break;
	    case 5:
	      ident = TRUE;
	      lose_int();
	      break;
	    case 6:
	      if (res_stat (A_INT))
		{
		  msg_print("You have have a warm feeling.");
		  ident = TRUE;
		}
	      break;
	    case 7:
	      if (inc_stat (A_WIS))
		{
		  msg_print("You suddenly have a profound thought!");
		  ident = TRUE;
		}
	      break;
	    case 8:
	      ident = TRUE;
	      lose_wis();
	      break;
	    case 9:
	      if (res_stat (A_WIS))
		{
		  msg_print("You feel your wisdom returning.");
		  ident = TRUE;
		}
	      break;
	    case 10:
	      if (inc_stat (A_CHR))
		{
		  msg_print("Gee, ain't you cute!");
		  ident = TRUE;
		}
	      break;
	    case 11:
	      ident = TRUE;
	      lose_chr();
	      break;
	    case 12:
	      if (res_stat (A_CHR))
		{
		  msg_print("You feel your looks returning.");
		  ident = TRUE;
		}
	      break;
	    case 13:
	      ident = hp_player(damroll(2, 7));
	      break;
	    case 14:
	      ident = hp_player(damroll(4, 7));
	      break;
	    case 15:
	      ident = hp_player(damroll(6, 7));
	      break;
	    case 16:
	      ident = hp_player(1000);
	      break;
	    case 17:
	      if (inc_stat (A_CON))
		{
		  msg_print("You feel tingly for a moment.");
		  ident = TRUE;
		}
	      break;
	    case 18:
	      m_ptr = &py.misc;
	      if (m_ptr->exp < MAX_EXP)
		{
		  l = (m_ptr->exp / 2) + 10;
		  if (l > 100000L)  l = 100000L;
		  m_ptr->exp += l;
		  msg_print("You feel more experienced.");
		  prt_experience();
		  ident = TRUE;
		}
	      break;
	    case 19:
	      f_ptr = &py.flags;
	      if (!f_ptr->free_act)
		{
		  /* paralysis must == 0, otherwise could not drink potion */
		  msg_print("You fall asleep.");
		  f_ptr->paralysis += randint(4) + 4;
		  ident = TRUE;
		}
	      break;
	    case 20:
	      f_ptr = &py.flags;
	      if (f_ptr->blind == 0)
		{
		  msg_print("You are covered by a veil of darkness.");
		  ident = TRUE;
		}
	      f_ptr->blind += randint(100) + 100;
	      break;
	    case 21:
	      f_ptr = &py.flags;
	      if (f_ptr->confused == 0)
		{
		  msg_print("Hey!  This is good stuff!  * Hick! *");
		  ident = TRUE;
		}
	      f_ptr->confused += randint(20) + 12;
	      break;
	    case 22:
	      f_ptr = &py.flags;
	      if (f_ptr->poisoned == 0)
		{
		  msg_print("You feel very sick.");
		  ident = TRUE;
		}
	      f_ptr->poisoned += randint(15) + 10;
	      break;
	    case 23:
	      if (py.flags.fast == 0)
		ident = TRUE;
	      py.flags.fast += randint(25) + 15;
	      break;
	    case 24:
	      if (py.flags.slow == 0)
		ident = TRUE;
	      py.flags.slow += randint(25) + 15;
	      break;
	    case 26:
	      if (inc_stat (A_DEX))
		{
		  msg_print("You feel more limber!");
		  ident = TRUE;
		}
	      break;
	    case 27:
	      if (res_stat (A_DEX))
		{
		  msg_print("You feel less clumsy.");
		  ident = TRUE;
		}
	      break;
	    case 28:
	      if (res_stat (A_CON))
		{
		  msg_print("You feel your health returning!");
		  ident = TRUE;
		}
	      break;
	    case 29:
	      ident = cure_blindness();
	      break;
	    case 30:
	      ident = cure_confusion();
	      break;
	    case 31:
	      ident = cure_poison();
	      break;
#if 0
	    case 33:
	      /* this is no longer useful, now that there is a 'G'ain magic
		 spells command */
	      m_ptr = &py.misc;
	      c_ptr = &class[m_ptr->pclass];
	      if (c_ptr->spell == MAGE)
		{
		  calc_spells(A_INT);
		  calc_mana(A_INT);
		}
	      else if (c_ptr->spell == PRIEST)
		{
		  calc_spells(A_WIS);
		  calc_mana(A_WIS);
		}
	      else
		{
		  /* A warrior learns something about his equipment. -CJS- */
		  inven_type *w_ptr;
		  vtype tmp_str;
		  extern char *describe_use ();

		  for (k = 22; k < INVEN_ARRAY_SIZE; k++)
		    {
		      w_ptr = &inventory[k];
		      if (w_ptr->tval != TV_NOTHING && enchanted (w_ptr))
			{
			  (void) sprintf (tmp_str,
				  "There's something about what you are %s...",
					  describe_use(k));
			  msg_print (tmp_str);
			  add_inscribe(w_ptr, ID_MAGIK);
			  ident = TRUE;
			}
		    }
		}
	      break;
#endif
	    case 34:
	      if (py.misc.exp > 0)
		{
		  int32 m, scale;
		  msg_print("You feel your memories fade.");
		  /* Lose between 1/5 and 2/5 of your experience */
		  m = py.misc.exp / 5;
		  if (py.misc.exp > MAX_SHORT)
		    {
		      scale = MAX_LONG / py.misc.exp;
		      m += (randint((int)scale) * py.misc.exp) / (scale * 5);
		    }
		  else
		    m += randint((int)py.misc.exp) / 5;
		  lose_exp(m);
		  ident = TRUE;
		}
	      break;
	    case 35:
	      f_ptr = &py.flags;
	      (void) cure_poison();
	      if (f_ptr->food > 150)  f_ptr->food = 150;
	      f_ptr->paralysis = 4;
	      msg_print("The potion makes you vomit!");
	      ident = TRUE;
	      break;
	    case 36:
	      if (py.flags.invuln == 0)
		ident = TRUE;
	      py.flags.invuln += randint(10) + 10;
	      break;
	    case 37:
	      if (py.flags.hero == 0)
		ident = TRUE;
	      py.flags.hero += randint(25) + 25;
	      break;
	    case 38:
	      if (py.flags.shero == 0)
		ident = TRUE;
	      py.flags.shero += randint(25) + 25;
	      break;
	    case 39:
	      ident = remove_fear();
	      break;
	    case 40:
	      ident = restore_level();
	      break;
	    case 41:
	      f_ptr = &py.flags;
	      if (f_ptr->resist_heat == 0)
		ident = TRUE;
	      f_ptr->resist_heat += randint(10) + 10;
	      break;
	    case 42:
	      f_ptr = &py.flags;
	      if (f_ptr->resist_cold == 0)
		ident = TRUE;
	      f_ptr->resist_cold += randint(10) + 10;
	      break;
	    case 43:
	      if (py.flags.detect_inv == 0)
		ident = TRUE;
	      detect_inv2(randint(12)+12);
	      break;
	    case 44:
	      ident = slow_poison();
	      break;
	    case 45:
	      ident = cure_poison();
	      break;
	    case 46:
	      m_ptr = &py.misc;
	      if (m_ptr->cmana < m_ptr->mana)
		{
		  m_ptr->cmana = m_ptr->mana;
		  ident = TRUE;
		  msg_print("Your feel your head clear.");
		  prt_cmana();
		}
	      break;
	    case 47:
	      f_ptr = &py.flags;
	      if (f_ptr->tim_infra == 0)
		{
		  msg_print("Your eyes begin to tingle.");
		  ident = TRUE;
		}
	      f_ptr->tim_infra += 100 + randint(100);
	      break;
	    default:
	      msg_print ("Internal error in potion()");
	      break;
	    }
	  /* End of Potions.					*/
	}
      if (ident)
	{
	  if (!known1_p(i_ptr))
	    {
	      m_ptr = &py.misc;
	      /* round half-way case up */
	      m_ptr->exp += (i_ptr->level + (m_ptr->lev >> 1)) / m_ptr->lev;
	      prt_experience();

	      identify(&item_val);
	      i_ptr = &inventory[item_val];
	    }
	}
      else if (!known1_p(i_ptr))
	sample (i_ptr);

      add_food(i_ptr->p1);
      desc_remain(item_val);
      inven_destroy(item_val);
    }
}
