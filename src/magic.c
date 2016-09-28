/* source/magic.c: code for mage spells

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


/* Throw a magic spell					-RAK-	*/
void cast()
{
  int i, j, item_val, dir;
  int choice, chance, result;
  register struct flags *f_ptr;
  register struct misc *p_ptr;
  register inven_type *i_ptr;
  register spell_type *m_ptr;
#ifdef ATARIST_MWC
  int32u holder;
#endif

  free_turn_flag = TRUE;
  if (py.flags.blind > 0)
    msg_print("You can't see to read your spell book!");
  else if (no_light())
    msg_print("You have no light to read by.");
  else if (py.flags.confused > 0)
    msg_print("You are too confused.");
  else if (class[py.misc.pclass].spell != MAGE)
    msg_print("You can't cast spells!");
  else if (!find_range(TV_MAGIC_BOOK, TV_NEVER, &i, &j))
    msg_print("But you are not carrying any spell-books!");
  else if (get_item(&item_val, "Use which spell-book?", i, j, CNIL, CNIL))
    {
      result = cast_spell("Cast which spell?", item_val, &choice, &chance);
      if (result < 0)
	msg_print("You don't know any spells in that book.");
      else if (result > 0)
	{
	  m_ptr = &magic_spell[py.misc.pclass-1][choice];
	  free_turn_flag = FALSE;

	  if (randint(100) < chance)
	    msg_print("You failed to get the spell off!");
	  else
	    {
	      /* Spells.  */
	      switch(choice+1)
		{
		case 1:
		  if (get_dir(CNIL, &dir))
		    fire_bolt(GF_MAGIC_MISSILE, dir, char_row, char_col,
			      damroll(2, 6), spell_names[0]);
		  break;
		case 2:
		  (void) detect_monsters();
		  break;
		case 3:
		  teleport(10);
		  break;
		case 4:
		  (void) light_area(char_row, char_col);
		  break;
		case 5:
		  (void) hp_player(damroll(4, 4));
		  break;
		case 6:
		  (void) detect_sdoor();
		  (void) detect_trap();
		  break;
		case 7:
		  if (get_dir(CNIL, &dir))
		    fire_ball(GF_POISON_GAS, dir, char_row, char_col, 12,
			      spell_names[6]);
		  break;
		case 8:
		  if (get_dir(CNIL, &dir))
		    (void) confuse_monster(dir, char_row, char_col);
		  break;
		case 9:
		  if (get_dir(CNIL, &dir))
		    fire_bolt(GF_LIGHTNING, dir, char_row, char_col,
			      damroll(4, 8), spell_names[8]);
		  break;
		case 10:
		  (void) td_destroy();
		  break;
		case 11:
		  if (get_dir(CNIL, &dir))
		    (void) sleep_monster(dir, char_row, char_col);
		  break;
		case 12:
		  (void) cure_poison();
		  break;
		case 13:
		  teleport((int)(py.misc.lev*5));
		  break;
		case 14:
		  for (i = 22; i < INVEN_ARRAY_SIZE; i++)
		    {
		      i_ptr = &inventory[i];
#ifdef ATARIST_MWC
		      i_ptr->flags = (i_ptr->flags & ~(holder = TR_CURSED));
#else
		      i_ptr->flags = (i_ptr->flags & ~TR_CURSED);
#endif
		    }
		  break;
		case 15:
		  if (get_dir(CNIL, &dir))
		    fire_bolt(GF_FROST, dir, char_row, char_col,
			      damroll(6, 8), spell_names[14]);
		  break;
		case 16:
		  if (get_dir(CNIL, &dir))
		    (void) wall_to_mud(dir, char_row, char_col);
		  break;
		case 17:
		  create_food();
		  break;
		case 18:
		  (void) recharge(20);
		  break;
		case 19:
		  (void) sleep_monsters1(char_row, char_col);
		  break;
		case 20:
		  if (get_dir(CNIL, &dir))
		    (void) poly_monster(dir, char_row, char_col);
		  break;
		case 21:
		  (void) ident_spell();
		  break;
		case 22:
		  (void) sleep_monsters2();
		  break;
		case 23:
		  if (get_dir(CNIL, &dir))
		    fire_bolt(GF_FIRE, dir, char_row, char_col,
			      damroll(9, 8), spell_names[22]);
		  break;
		case 24:
		  if (get_dir(CNIL, &dir))
		    (void)speed_monster(dir, char_row, char_col, -1);
		  break;
		case 25:
		  if (get_dir(CNIL, &dir))
		    fire_ball(GF_FROST, dir, char_row, char_col, 48,
			      spell_names[24]);
		  break;
		case 26:
		  (void) recharge(60);
		  break;
		case 27:
		  if (get_dir(CNIL, &dir))
		    (void) teleport_monster(dir, char_row, char_col);
		  break;
		case 28:
		  f_ptr = &py.flags;
		  f_ptr->fast += randint(20) + py.misc.lev;
		  break;
		case 29:
		  if (get_dir(CNIL, &dir))
		    fire_ball(GF_FIRE, dir, char_row, char_col, 72,
			      spell_names[28]);
		  break;
		case 30:
		  destroy_area(char_row, char_col);
		  break;
		case 31:
		  (void) genocide();
		  break;
		default:
		  break;
		}
	      /* End of spells.				     */
	      if (!free_turn_flag)
		{
		  p_ptr = &py.misc;
		  if ((spell_worked & (1L << choice)) == 0)
		    {
		      p_ptr->exp += m_ptr->sexp << 2;
		      spell_worked |= (1L << choice);
		      prt_experience();
		    }
		}
	    }
	  p_ptr = &py.misc;
	  if (!free_turn_flag)
	    {
	      if (m_ptr->smana > p_ptr->cmana)
		{
		  msg_print("You faint from the effort!");
		  py.flags.paralysis =
		    randint((int)(5*(m_ptr->smana-p_ptr->cmana)));
		  p_ptr->cmana = 0;
		  p_ptr->cmana_frac = 0;
		  if (randint(3) == 1)
		    {
		      msg_print("You have damaged your health!");
		      (void) dec_stat (A_CON);
		    }
		}
	      else
		p_ptr->cmana -= m_ptr->smana;
	      prt_cmana();
	    }
	}
    }
}
