/* prayer.c: code for priest spells

   Copyright (c) 1989 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include "constant.h"
#include "config.h"
#include "types.h"
#include "externs.h"


/* Pray like HELL.					-RAK-	*/
void pray()
{
  int i, j, item_val, dir;
  int choice, chance, result;
  register spell_type *s_ptr;
  register struct misc *m_ptr;
  register struct flags *f_ptr;
  register inven_type *i_ptr;

  free_turn_flag = TRUE;
  if (py.flags.blind > 0)
    msg_print("You can't see to read your prayer!");
  else if (no_light())
    msg_print("You have no light to read by.");
  else if (py.flags.confused > 0)
    msg_print("You are too confused.");
  else if (class[py.misc.pclass].spell != PRIEST)
    msg_print("Pray hard enough and your prayers may be answered.");
  else if (inven_ctr == 0)
    msg_print ("But you are not carrying anything!");
  else if (!find_range(TV_PRAYER_BOOK, TV_NEVER, &i, &j))
    msg_print ("You are not carrying any Holy Books!");
  else if (get_item(&item_val, "Use which Holy Book?", i, j))
    {
      result = cast_spell("Recite which prayer?", item_val, &choice, &chance);
      if (result < 0)
	msg_print("You don't know any prayers in that book.");
      else if (result > 0)
	{
	  s_ptr = &magic_spell[py.misc.pclass-1][choice];
	  free_turn_flag = FALSE;

	  if (randint(100) < chance)
	    msg_print("You lost your concentration!");
	  else
	    {
	      /* Prayers.					*/
	      switch(choice+1)
		{
		case 1:
		  (void) detect_evil();
		  break;
		case 2:
		  (void) hp_player(damroll(3, 3));
		  break;
		case 3:
		  bless(randint(12)+12);
		  break;
		case 4:
		  (void) remove_fear();
		  break;
		case 5:
		  (void) light_area(char_row, char_col);
		  break;
		case 6:
		  (void) detect_trap();
		  break;
		case 7:
		  (void) detect_sdoor();
		  break;
		case 8:
		  (void) slow_poison();
		  break;
		case 9:
		  if (get_dir(NULL, &dir))
		    (void) confuse_monster(dir, char_row, char_col);
		  break;
		case 10:
		  teleport((int)(py.misc.lev*3));
		  break;
		case 11:
		  (void) hp_player(damroll(4, 4));
		  break;
		case 12:
		  bless(randint(24)+24);
		  break;
		case 13:
		  (void) sleep_monsters1(char_row, char_col);
		  break;
		case 14:
		  create_food();
		  break;
		case 15:
		  for (i = 0; i < INVEN_ARRAY_SIZE; i++)
		    {
		      i_ptr = &inventory[i];
		      /* only clear flag for items that are wielded or worn */
		      if (i_ptr->tval >= TV_MIN_WEAR
			  && i_ptr->tval <= TV_MAX_WEAR)
			i_ptr->flags &= ~TR_CURSED;
		    }
		  break;
		case 16:
		  f_ptr = &py.flags;
		  f_ptr->resist_heat += randint(10) + 10;
		  f_ptr->resist_cold += randint(10) + 10;
		  break;
		case 17:
		  (void) cure_poison();
		  break;
		case 18:
		  if (get_dir(NULL, &dir))
		    fire_ball(GF_HOLY_ORB, dir, char_row, char_col,
			      (int)(damroll(3, 6)+py.misc.lev),
			      "Black Sphere");
		  break;
		case 19:
		  (void) hp_player(damroll(8, 4));
		  break;
		case 20:
		  detect_inv2(randint(24)+24);
		  break;
		case 21:
		  (void) protect_evil();
		  break;
		case 22:
		  earthquake();
		  break;
		case 23:
		  map_area();
		  break;
		case 24:
		  (void) hp_player(damroll(16, 4));
		  break;
		case 25:
		  (void) turn_undead();
		  break;
		case 26:
		  bless(randint(48)+48);
		  break;
		case 27:
		  (void) dispel_creature(CD_UNDEAD, (int)(3*py.misc.lev));
		  break;
		case 28:
		  (void) hp_player(200);
		  break;
		case 29:
		  (void) dispel_creature(CD_EVIL, (int)(3*py.misc.lev));
		  break;
		case 30:
		  warding_glyph();
		  break;
		case 31:
		  (void) dispel_creature(CD_EVIL, (int)(4*py.misc.lev));
		  (void) remove_fear();
		  (void) cure_poison();
		  (void) hp_player(1000);
		  break;
		default:
		  break;
		}
	      /* End of prayers.				*/
	      if (!free_turn_flag)
		{
		  m_ptr = &py.misc;
		  if ((spell_worked & (1L << choice)) == 0)
		    {
		      m_ptr->exp += s_ptr->sexp << 2;
		      prt_experience();
		      spell_worked |= (1L << choice);
		    }
		}
	    }
	  m_ptr = &py.misc;
	  if (!free_turn_flag)
	    {
	      if (s_ptr->smana > m_ptr->cmana)
		{
		  msg_print("You faint from fatigue!");
		  py.flags.paralysis =
		    randint((int)(5 * (s_ptr->smana-m_ptr->cmana)));
		  m_ptr->cmana = 0;
		  m_ptr->cmana_frac = 0;
		  if (randint(3) == 1)
		    {
		      msg_print("You have damaged your health!");
		      (void) dec_stat (A_CON);
		    }
		}
	      else
		m_ptr->cmana -= s_ptr->smana;
	      prt_cmana();
	    }
	}
    }
}
