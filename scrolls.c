#include <stdio.h>

#include "constants.h"
#include "types.h"
#include "externs.h"

#ifdef sun   /* correct SUN stupidity in the stdio.h file */
char *sprintf();
#endif

extern int moria_flag;


/* Scrolls for the reading				-RAK-	*/
read_scroll()
{
  unsigned int i;
  int j, k, l, item_val;
  int y, x;
  int tmp[5];
  vtype out_val, tmp_str;
  int redraw, ident, first, flag;
  treasure_type *i_ptr;
  struct misc *m_ptr;

  first = TRUE;
  reset_flag = TRUE;
  if (inven_ctr > 0)
    {
      if (find_range(70, 71, &j, &k))
	if (py.flags.blind > 0)
	  msg_print("You can't see to read the scroll.");
	else if (no_light())
	  msg_print("You have no light to read by.");
	else if (py.flags.confused > 0)
	  {
	    msg_print("The text seems to swim about the page!");
	    msg_print("You are too confused to scanf...");
	  }
	else
	  {
	    redraw = FALSE;
	    if (get_item(&item_val, "Read which scroll?", &redraw, j, k))
	      {
		i_ptr = &inventory[item_val];
		if (redraw)  draw_cave();
		reset_flag = FALSE;
		i = i_ptr->flags;
		ident = FALSE;

		while (i != 0)
		  {
		    j = bit_pos(&i) + 1;
		    if (i_ptr->tval == 71)
		      j += 31;
		    if (first)
		      if ((j != 4) && (j != 25))
			{
			  msg_print("As you read the scroll it vanishes.");
			  first = FALSE;
			}

		    /* Scrolls...			*/
		    switch(j)
		      {
		      case 1:
			i_ptr = &inventory[22];
			if (i_ptr->tval != 0)
			  {
			    objdes(tmp_str, 22, FALSE);
			    (void) sprintf(out_val, "Your %s glows faintly!",
					   tmp_str);
			    msg_print(out_val);
			    if (enchant(&i_ptr->tohit))
			      {
				i_ptr->flags &= 0x7FFFFFFF;
				py_bonuses(blank_treasure, 0);
			      }
			    else
			      msg_print("The enchantment fails...");
			  }
			ident = TRUE;
			break;
		      case 2:
			i_ptr = &inventory[22];
			if (i_ptr->tval != 0)
			  {
			    objdes(tmp_str, 22, FALSE);
			    (void) sprintf(out_val, "Your %s glows faintly!",
					   tmp_str);
			    msg_print(out_val);
			    if (enchant(&i_ptr->todam))
			      {
				i_ptr->flags &= 0x7FFFFFFF;
				py_bonuses(blank_treasure, 0);
			      }
			    else
			      msg_print("The enchantment fails...");
			  }
			ident = TRUE;
			break;
		      case 3:
			k = 0;
			l = 0;
			if (inventory[25].tval != 0)
			  {
			    k++;
			    tmp[k] = 25;
			  }
			if (inventory[26].tval != 0)
			  {
			    k++;
			    tmp[k] = 26;
			  }
			if (inventory[31].tval != 0)
			  {
			    k++;
			    tmp[k] = 31;
			  }
			if (inventory[27].tval != 0)
			  {
			    k++;
			    tmp[k] = 27;
			  }
			if (inventory[23].tval != 0)
			  {
			    k++;
			    tmp[k] = 23;
			  }
			if (k > 0)  l = tmp[randint(k)];
			if (0x80000000 & inventory[25].flags)
			  l = 25;
			else if (0x80000000 & inventory[26].flags)
			  l = 26;
			else if (0x80000000 & inventory[31].flags)
			  l = 31;
			else if (0x80000000 & inventory[23].flags)
			  l = 23;
			else if (0x80000000 & inventory[27].flags)
			  l = 27;
			if (l > 0)
			  {
			    i_ptr = &inventory[l];
			    objdes(tmp_str, l, FALSE);
			    (void) sprintf(out_val, "Your %s glows faintly!",
					   tmp_str);
			    if (enchant(&i_ptr->toac))
			      {
				i_ptr->flags &= 0x7FFFFFFF;
				py_bonuses(blank_treasure, 0);
			      }
			    else
			      msg_print("The enchantment fails...");
			  }
			ident = TRUE;
			break;
		      case 4:
			identify(inventory[item_val]);
			msg_print("This is an identify scroll");
			msg_print(" ");
			if (ident_spell())  first = FALSE;
			break;
		      case 5:
			if (remove_curse())
			  {
		     msg_print("You feel as if someone is watching over you.");
			    ident = TRUE;
			  }
			break;
		      case 6:
			ident = light_area(char_row, char_col);
			break;
		      case 7:
			ident = FALSE;
			for (k = 0; k < randint(3); k++)
			  {
			    y = char_row;
			    x = char_col;
			    ident |= summon_monster(&y, &x, FALSE);
			  }
			break;
		      case 8:
			teleport(10);
			ident = TRUE;
			break;
		      case 9:
			teleport(100);
			ident = TRUE;
			break;
		      case 10:
			dun_level += (-3) + 2*randint(2);
			if (dun_level < 1)
			  dun_level = 1;
			moria_flag = TRUE;
			ident = TRUE;
			break;
		      case 11:
			msg_print("Your hands begin to glow.");
			py.flags.confuse_monster = TRUE;
			ident = TRUE;
			break;
		      case 12:
			ident = map_area();
			break;
		      case 13:
			ident = sleep_monsters1(char_row, char_col);
			break;
		      case 14:
			ident = warding_glyph();
			break;
		      case 15:
			ident = detect_treasure();
			break;
		      case 16:
			ident = detect_object();
			break;
		      case 17:
			ident = detect_trap();
			break;
		      case 18:
			ident = detect_sdoor();
			break;
		      case 19:
			msg_print("This is a mass genocide scroll.");
			msg_print(" ");
			ident = mass_genocide();
			break;
		      case 20:
			ident = detect_invisible();
			break;
		      case 21:
			ident = aggravate_monster(20);
			msg_print("There is a high pitched humming noise");
			break;
		      case 22:
			ident = trap_creation();
			break;
		      case 23:
			ident = td_destroy();
			break;
		      case 24:
			ident = door_creation();
			break;
		      case 25:
			identify(inventory[item_val]);
			msg_print("This is a Recharge-Item scroll.");
			msg_print(" ");
			if (recharge(60))  first = FALSE;
			break;
		      case 26:
			msg_print("This is a genocide scroll.");
			msg_print(" ");
			ident = genocide();
			break;
		      case 27:
			ident = unlight_area(char_row, char_col);
			break;
		      case 28:
			ident = protect_evil();
			break;
		      case 29:
			ident = create_food();
			break;
		      case 30:
			ident = dispell_creature(0x0008, 60);
			break;
		      case 31:
			msg_print("That scroll appeared to be blank.");
			ident = TRUE;
			break;
		      case 32:
			i_ptr = &inventory[22];
			if (i_ptr->tval != 0)
			  {
			    objdes(tmp_str, 22, FALSE);
			    (void) sprintf(out_val, "Your %s glows brightly!",
				    tmp_str);
			    msg_print(out_val);
			    flag = FALSE;
			    for (k = 0; k < randint(2); k++)
			      if (enchant(&i_ptr->tohit))
				flag = TRUE;
			    for (k = 0; k < randint(2); k++)
			      if (enchant(&i_ptr->todam))
				flag = TRUE;
			    if (flag)
			      {
				i_ptr->flags &= 0x7FFFFFFF;
				py_bonuses(blank_treasure, 0);
			      }
			    else
			      msg_print("The enchantment fails...");
			  }
			ident = TRUE;
			break;
		      case 33:
			i_ptr = &inventory[22];
			if (i_ptr->tval != 0)
			  {
			    inventory[INVEN_MAX] = inventory[22];
			    objdes(tmp_str, 22, FALSE);
			    (void)sprintf(out_val,"Your %s glows black, fades",
				    tmp_str);
			    msg_print(out_val);
			    i_ptr->tohit = -randint(5) - randint(5);
			    i_ptr->todam = -randint(5) - randint(5);
			    i_ptr->flags = 0x80000000;
			    py_bonuses(inventory[INVEN_MAX], -1);
			    ident = TRUE;
			  }
			break;
		      case 34:
			if (0x80000000 & inventory[25].flags)
			  k = 25;
			else if (0x80000000 & inventory[26].flags)
			  k = 26;
			else if (0x80000000 & inventory[31].flags)
			  k = 31;
			else if (0x80000000 & inventory[23].flags)
			  k = 23;
			else if (0x80000000 & inventory[27].flags)
			  k = 27;
			else if (inventory[25].tval != 0)
			  k = 25;
			else if (inventory[26].tval !=0)
			  k = 26;
			else if (inventory[23].tval != 0)
			  k = 23;
			else if (inventory[27].tval != 0)
			  k = 27;
			else
			  k = 0;
			if (k > 0)
			  {
			    i_ptr = &inventory[k];
			    objdes(tmp_str, k, FALSE);
			    (void) sprintf(out_val,"Your %s glows brightly!",
					   tmp_str);
			    msg_print(out_val);
			    flag = FALSE;
			    for (k = 0; k < randint(2) + 1; k++)
			      if (enchant(&i_ptr->toac))
				flag = TRUE;
			    if (flag)
			      {
				i_ptr->flags &= 0x7FFFFFFF;
				py_bonuses(blank_treasure, 0);
			      }
			    else
			      msg_print("The enchantment fails...");
			  }
			ident = TRUE;
			break;
		      case 35:
			if ((inventory[25].tval != 0) && (randint(4) == 1))
			  k = 25;
			else if ((inventory[26].tval != 0) && (randint(3) ==1))
			  k = 26;
			else if ((inventory[31].tval != 0) && (randint(3) ==1))
			  k = 31;
			else if ((inventory[23].tval != 0) && (randint(3) ==1))
			  k = 23;
			else if ((inventory[27].tval != 0) && (randint(3) ==1))
			  k = 27;
			else if (inventory[25].tval != 0)
			  k = 25;
			else if (inventory[26].tval != 0)
			  k = 26;
			else if (inventory[31].tval != 0)
			  k = 31;
			else if (inventory[23].tval != 0)
			  k = 23;
			else if (inventory[27].tval != 0)
			  k = 27;
			else
			  k = 0;
			if (k > 0)
			  {
			    i_ptr = &inventory[k];
			    inventory[INVEN_MAX] = inventory[k];
			    objdes(tmp_str, k, FALSE);
			   (void)sprintf(out_val,"Your %s glows black, fades.",
				    tmp_str);
			    msg_print(out_val);
			    i_ptr->flags = 0x80000000;
			    i_ptr->toac = -randint(5) - randint(5);
			    py_bonuses(inventory[INVEN_MAX], -1);
			    ident = TRUE;
			  }
			break;
		      case 36:
			ident = FALSE;
			for (k = 0; k < randint(3); k++)
			  {
			    y = char_row;
			    x = char_col;
			    ident |= summon_undead(&y, &x);
			  }
			break;
		      case 37:
			ident = bless(randint(12)+6);
			break;
		      case 38:
			ident = bless(randint(24)+12);
			break;
		      case 39:
			ident = bless(randint(48)+24);
			break;
		      case 40:
			ident = TRUE;
			py.flags.word_recall = 25 + randint(30);
			msg_print("The air about you becomes charged...");
			break;
		      case 41:
			ident = destroy_area(char_row, char_col);
			break;
		      case 42:
			break;
		      case 43:
			break;
		      case 44:
			break;
		      case 45:
			break;
		      case 46:
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
		    /* End of Scrolls...                         */
		  }
		if (!reset_flag)
		  {
		    if (ident)
		      identify(inventory[item_val]);
		    if (!first)
		      {
			desc_remain(item_val);
			inven_destroy(item_val);
			if (i_ptr->flags != 0)
			  {
			    m_ptr = &py.misc;
			    m_ptr->exp += (i_ptr->level/m_ptr->lev);
			    prt_experience();
			  }
		      }
		  }
	      }
	    else
	      if (redraw)  draw_cave();
	  }
      else
	msg_print("You are not carrying any scrolls.");
    }
  else
    msg_print("But you are not carrying anything.");
}
