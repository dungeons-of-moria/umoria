#include <stdio.h>

#include "constants.h"
#include "config.h"
#include "types.h"
#include "externs.h"

#ifdef USG
#include <string.h>
#else
#include <strings.h>
#endif

#ifdef sun   /* correct SUN stupidity in the stdio.h file */
char *sprintf();
#endif

#ifdef USG
unsigned sleep();
#endif

#ifdef ultrix
void sleep();
#endif

/* global flags */
int moria_flag;        /* Next level when true  */
int search_flag;       /* Player is searching   */
int teleport_flag;     /* Handle teleport traps  */
int player_light;      /* Player carrying light */
int cave_flag;         /* used in get_panel */
int light_flag;        /* used in move_light */

/* value of msg_flag at start of turn */
int save_msg_flag;

/* Moria game module					-RAK-	*/
/* The code in this section has gone through many revisions, and */
/* some of it could stand some more hard work...  -RAK-          */
dungeon()
{
  int old_chp, old_cmana;      /* Detect change         */
  double regen_amount;         /* Regenerate hp and mana*/
  char command;         /* Last command          */
  register struct misc *p_ptr;
  register treasure_type *i_ptr;
  register struct flags *f_ptr;
  int set_floor();

  /* Main procedure for dungeon... 			-RAK-	*/
  /* Note: There is a lot of preliminary magic going on here at first*/

  /* Check light status for setup          */
  i_ptr = &inventory[INVEN_LIGHT];
  if (i_ptr->p1 > 0)
    player_light = TRUE;
  else
    player_light = FALSE;
  /* Check for a maximum level             */
  p_ptr = &py.misc;
  if (dun_level > p_ptr->max_lev)  p_ptr->max_lev = dun_level;
  /* Set up the character co-ords          */
  if ((char_row == -1) || (char_col == -1))
    new_spot(&char_row, &char_col);
  /* Reset flags and initialize variables  */
  moria_flag    = FALSE;
  cave_flag     = FALSE;
  find_flag     = FALSE;
  search_flag   = FALSE;
  teleport_flag = FALSE;
  mon_tot_mult  = 0;
  cave[char_row][char_col].cptr = 1;
  old_chp   = (int)py.misc.chp;
  old_cmana = (int)py.misc.cmana;
  /* Light up the area around character    */
  move_char(5);
  /* Light,  but do not move critters       */
  creatures(FALSE);
  /* Print the depth                       */
  prt_depth();

  /* Loop until dead,  or new level 		*/
  do
    {
      /* Increment turn counter			*/
      turn++;
      /* Check for game hours                          */
      if (!wizard1)
	if ((turn % 250) == 1)
	  if (!check_time())
	    if (closing_flag > 4)
	      {
		if (search_flag)
		  search_off();
		if (py.flags.rest > 0)
		  rest_off();
		find_flag = FALSE;
		msg_print("The gates to Moria are now closed.");
		/* make sure player sees the message */
		msg_print(" ");
		do
		  {
		    save_char(TRUE, FALSE);
		  }
		while (TRUE);
	      }
	    else
	      {
		if (search_flag)
		  search_off();
		if (py.flags.rest > 0)
		  rest_off();
		find_flag = FALSE;
		move_light (char_row, char_col, char_row, char_col);
		closing_flag++;
		msg_print("The gates to Moria are closing...");
		msg_print("Please finish up or save your game.");
		/* make sure the player sees the message */
		msg_print(" ");
	      }

      /* turn over the store contents every, say, 1000 turns */
      if ((dun_level != 0) && ((turn % 1000) == 0))
	store_maint();

      /* Check for creature generation 		*/
      if (randint(MAX_MALLOC_CHANCE) == 1)
	alloc_monster(set_floor, 1, MAX_SIGHT, FALSE);
      /* Screen may need updating,  used mostly for stats*/
      if (print_stat != 0)
	{
	  if (0x0001 & print_stat)
	    prt_strength();
	  if (0x0002 & print_stat)
	    prt_dexterity();
	  if (0x0004 & print_stat)
	    prt_constitution();
	  if (0x0008 & print_stat)
	    prt_intelligence();
	  if (0x0010 & print_stat)
	    prt_wisdom();
	  if (0x0020 & print_stat)
	    prt_charisma();
	  if (0x0040 & print_stat)
	    prt_pac();
	  if (0x0100 & print_stat)
	    prt_mhp();
	  if (0x0200 & print_stat)
	    prt_title();
	  if (0x0400 & print_stat)
	    prt_level();
	}
      /* Check light status                            */
      i_ptr = &inventory[INVEN_LIGHT];
      if (player_light)
	if (i_ptr->p1 > 0)
	  {
	    i_ptr->p1--;
	    if (i_ptr->p1 == 0)
	      {
		player_light = FALSE;
		find_flag = FALSE;
		msg_print("Your light has gone out!");
		move_light(char_row, char_col, char_row, char_col);
	      }
	    else if (i_ptr->p1 < 40)
	      if (randint(5) == 1)
		{
		  if (find_flag)
		    {
		      find_flag = FALSE;
		      move_light(char_row, char_col, char_row, char_col);
		    }
		  msg_print("Your light is growing faint.");
		}
	  }
	else
	  {
	    player_light = FALSE;
	    find_flag = FALSE;
	    move_light(char_row, char_col, char_row, char_col);
	  }
      else if (i_ptr->p1 > 0)
	{
	  i_ptr->p1--;
	  player_light = TRUE;
	  move_light(char_row, char_col, char_row, char_col);
	}

      /* Update counters and messages			*/
      f_ptr = &py.flags;
      /* Check food status             */
      regen_amount = PLAYER_REGEN_NORMAL;
      if (f_ptr->food < PLAYER_FOOD_ALERT)
	{
	  if (f_ptr->food < PLAYER_FOOD_WEAK)
	    {
	      if (f_ptr->food < 0)
		regen_amount = 0;
	      else if (f_ptr->food < PLAYER_FOOD_FAINT)
		regen_amount = PLAYER_REGEN_FAINT;
	      else if (f_ptr->food < PLAYER_FOOD_WEAK)
		regen_amount = PLAYER_REGEN_WEAK;
	      if ((0x00000002 & f_ptr->status) == 0)
		{
		  f_ptr->status |= 0x00000003;
		  msg_print("You are getting weak from hunger.");
		  if (find_flag)
		    {
		      find_flag = FALSE;
		      move_light (char_row, char_col, char_row, char_col);
		    }
		  prt_hunger();
		}
	      if (f_ptr->food < PLAYER_FOOD_FAINT)
		if (randint(8) == 1)
		  {
		    f_ptr->paralysis += randint(5);
		    msg_print("You faint from the lack of food.");
		    if (find_flag)
		      {
			find_flag = FALSE;
			move_light (char_row, char_col, char_row, char_col);
		      }
		  }
	    }
	  else
	    {
	      if ((0x00000001 & f_ptr->status) == 0)
		{
		  f_ptr->status |= 0x00000001;
		  msg_print("You are getting hungry.");
		  if (find_flag)
		    {
		      find_flag = FALSE;
		      move_light (char_row, char_col, char_row, char_col);
		    }
		  prt_hunger();
		}
	    }
	}
      /* Food consumption       */
      /* Note: Speeded up characters really burn up the food!  */
      if (f_ptr->speed < 0)
	f_ptr->food +=  -(f_ptr->speed*f_ptr->speed) -
	  f_ptr->food_digested;
      else
	f_ptr->food -= f_ptr->food_digested;
      /* Regenerate            */
      p_ptr = &py.misc;
      if (f_ptr->regenerate)  regen_amount = regen_amount * 1.5;
      if (f_ptr->rest > 0)    regen_amount = regen_amount * 2;
      if (py.flags.poisoned < 1)
	if (p_ptr->chp < p_ptr->mhp)
	  regenhp(regen_amount);
      if (p_ptr->cmana < p_ptr->mana)
	regenmana(regen_amount);
      /* Blindness             */
      if (f_ptr->blind > 0)
	{
	  if ((0x00000004 & f_ptr->status) == 0)
	    {
	      f_ptr->status |= 0x00000004;
	      prt_map();
	      prt_blind();
	      if (search_flag)
		search_off();
	    }
	  f_ptr->blind--;
	  if (f_ptr->blind == 0)
	    {
	      f_ptr->status &= 0xFFFFFFFB;
	      prt_blind();
	      prt_map();
	      msg_print("The veil of darkness lifts.");
	      if (find_flag)
		{
		  find_flag = FALSE;
		}
	      /* turn light back on */
	      move_char(5);
	      /* light creatures */
	      creatures(FALSE);
	    }
	}
      /* Confusion             */
      if (f_ptr->confused > 0)
	{
	  if ((0x00000008 & f_ptr->status) == 0)
	    {
	      f_ptr->status |= 0x00000008;
	      prt_confused();
	    }
	  f_ptr->confused--;
	  if (f_ptr->confused == 0)
	    {
	      f_ptr->status &= 0xFFFFFFF7;
	      prt_confused();
	      msg_print("You feel less confused now.");
	      if (find_flag)
		{
		  find_flag = FALSE;
		  move_light (char_row, char_col, char_row, char_col);
		}
	    }
	}
      /* Afraid                */
      if (f_ptr->afraid > 0)
	{
	  if ((0x00000010 & f_ptr->status) == 0)
	    {
	      if ((f_ptr->shero+f_ptr->hero) > 0)
		f_ptr->afraid = 0;
	      else
		{
		  f_ptr->status |= 0x00000010;
		  prt_afraid();
		}
	    }
	  else if ((f_ptr->shero+f_ptr->hero) > 0)
	    f_ptr->afraid = 1;
	  f_ptr->afraid--;
	  if (f_ptr->afraid == 0)
	    {
	      f_ptr->status &= 0xFFFFFFEF;
	      prt_afraid();
	      msg_print("You feel bolder now.");
	      if (find_flag)
		{
		  find_flag = FALSE;
		  move_light (char_row, char_col, char_row, char_col);
		}
	    }
	}
      /* Poisoned              */
      if (f_ptr->poisoned > 0)
	{
	  if ((0x00000020 & f_ptr->status) == 0)
	    {
	      f_ptr->status |= 0x00000020;
	      prt_poisoned();
	    }
	  f_ptr->poisoned--;
	  if (f_ptr->poisoned == 0)
	    {
	      f_ptr->status &= 0xFFFFFFDF;
	      prt_poisoned();
	      msg_print("You feel better.");
	      if (find_flag)
		{
		  find_flag = FALSE;
		  move_light (char_row, char_col, char_row, char_col);
		}
	    }
	  else
	    {
	      switch(con_adj())
		{
		case -4:
		  take_hit(4, "poison.");
		  break;
		case -3: case  -2:
		  take_hit(3, "poison.");
		  break;
		case -1:
		  take_hit(2, "poison.");
		  break;
		case 0:
		  take_hit(1, "poison.");
		  break;
		case 1: case 2: case 3:
		  if ((turn % 2) == 0)
		    take_hit(1, "poison.");
		  break;
		case 4: case 5:
		  if ((turn % 3) == 0)
		    take_hit(1, "poison.");
		  break;
		case 6:
		  if ((turn % 4) == 0)
		    take_hit(1, "poison.");
		  break;
		}
	    }
	}
      /* Fast                  */
      if (f_ptr->fast > 0)
	{
	  if ((0x00000040 & f_ptr->status) == 0)
	    {
	      f_ptr->status |= 0x00000040;
	      change_speed(-1);
	      msg_print("You feel yourself moving faster.");
	      if (find_flag)
		{
		  find_flag = FALSE;
		  move_light (char_row, char_col, char_row, char_col);
		}
	    }
	  f_ptr->fast--;
	  if (f_ptr->fast == 0)
	    {
	      f_ptr->status &= 0xFFFFFFBF;
	      change_speed(1);
	      msg_print("You feel yourself slow down.");
	      if (find_flag)
		{
		  find_flag = FALSE;
		  move_light (char_row, char_col, char_row, char_col);
		}
	    }
	}
      /* Slow                  */
      if (f_ptr->slow > 0)
	{
	  if ((0x00000080 & f_ptr->status) == 0)
	    {
	      f_ptr->status |= 0x00000080;
	      change_speed(1);
	      msg_print("You feel yourself moving slower.");
	      if (find_flag)
		{
		  find_flag = FALSE;
		  move_light (char_row, char_col, char_row, char_col);
		}
	    }
	  f_ptr->slow--;
	  if (f_ptr->slow == 0)
	    {
	      f_ptr->status &= 0xFFFFFF7F;
	      change_speed(-1);
	      msg_print("You feel yourself speed up.");
	      if (find_flag)
		{
		  find_flag = FALSE;
		  move_light (char_row, char_col, char_row, char_col);
		}
	    }
	}
      /* Resting is over?      */
      if (f_ptr->rest > 0)
	{
#ifdef SLOW
	  /* Hibernate every 20 iterations so that process does  */
	  /* not eat up system...                                */
	  if ((f_ptr->rest % 20) == 1)  (void) sleep(1);
#endif
	  f_ptr->rest--;
	  /* do not need to refresh screen here, if any movement/hit occurs
	     update_mon/take_hit will turn off resting and screen refreshes */
	  /* put_qio(); */
	  if (f_ptr->rest == 0)               /* Resting over          */
	    rest_off();
	}
      /* Hallucinating?  (Random characters appear!)*/
      if (f_ptr->image > 0)
	{
	  f_ptr->image--;
	  if (f_ptr->image == 0)
	    draw_cave();
	}
      /* Paralysis             */
      if (f_ptr->paralysis > 0)
	{
	  /* when paralysis true, you can not see any movement that occurs */
	  f_ptr->paralysis--;
	  if (f_ptr->rest > 0)
	    rest_off();
	  if (search_flag)
	    search_off();
	}
      /* Protection from evil counter*/
      if (f_ptr->protevil > 0)  f_ptr->protevil--;
      /* Invulnerability        */
      if (f_ptr->invuln > 0)
	{
	  if ((0x00001000 & f_ptr->status) == 0)
	    {
	      f_ptr->status |= 0x00001000;
	      if (find_flag)
		{
		  find_flag = FALSE;
		  move_light (char_row, char_col, char_row, char_col);
		}
	      py.misc.pac += 100;
	      py.misc.dis_ac += 100;
	      prt_pac();
	      msg_print("Your skin turns into steel!");
	    }
	  f_ptr->invuln--;
	  if (f_ptr->invuln == 0)
	    {
	      f_ptr->status &= 0xFFFFEFFF;
	      if (find_flag)
		{
		  find_flag = FALSE;
		  move_light (char_row, char_col, char_row, char_col);
		}
	      py.misc.pac -= 100;
	      py.misc.dis_ac -= 100;
	      prt_pac();
	      msg_print("Your skin returns to normal...");
	    }
	}
      /* Heroism       */
      if (f_ptr->hero > 0)
	{
	  if ((0x00002000 & f_ptr->status) == 0)
	    {
	      f_ptr->status |= 0x00002000;
	      if (find_flag)
		{
		  find_flag = FALSE;
		  move_light (char_row, char_col, char_row, char_col);
		}
	      p_ptr = &py.misc;
	      p_ptr->mhp += 10;
	      p_ptr->chp += 10.0;
	      p_ptr->bth += 12;
	      p_ptr->bthb+= 12;
	      msg_print("You feel like a HERO!");
	      prt_mhp();
	    }
	  f_ptr->hero--;
	  if (f_ptr->hero == 0)
	    {
	      f_ptr->status &= 0xFFFFDFFF;
	      if (find_flag)
		{
		  find_flag = FALSE;
		  move_light (char_row, char_col, char_row, char_col);
		}
	      p_ptr = &py.misc;
	      p_ptr->mhp -= 10;
	      if (p_ptr->chp > p_ptr->mhp)  p_ptr->chp = p_ptr->mhp;
	      p_ptr->bth -= 12;
	      p_ptr->bthb-= 12;
	      msg_print("The heroism wears off.");
	      prt_mhp();
	    }
	}
      /* Super Heroism */
      if (f_ptr->shero > 0)
	{
	  if ((0x00004000 & f_ptr->status) == 0)
	    {
	      f_ptr->status |= 0x00004000;
	      if (find_flag)
		{
		  find_flag = FALSE;
		  move_light (char_row, char_col, char_row, char_col);
		}
	      p_ptr = &py.misc;
	      p_ptr->mhp += 20;
	      p_ptr->chp += 20.0;
	      p_ptr->bth += 24;
	      p_ptr->bthb+= 24;
	      msg_print("You feel like a SUPER HERO!");
	      prt_mhp();
	    }
	  f_ptr->shero--;
	  if (f_ptr->shero == 0)
	    {
	      f_ptr->status &= 0xFFFFBFFF;
	      if (find_flag)
		{
		  find_flag = FALSE;
		  move_light (char_row, char_col, char_row, char_col);
		}
	      p_ptr = &py.misc;
	      p_ptr->mhp -= 20;
	      if (p_ptr->chp > p_ptr->mhp)  p_ptr->chp = p_ptr->mhp;
	      p_ptr->bth -= 24;
	      p_ptr->bthb-= 24;
	      msg_print("The super heroism wears off.");
	      prt_mhp();
	    }
	}
      /* Blessed       */
      if (f_ptr->blessed > 0)
	{
	  if ((0x00008000 & f_ptr->status) == 0)
	    {
	      f_ptr->status |= 0x00008000;
	      if (find_flag)
		{
		  find_flag = FALSE;
		  move_light (char_row, char_col, char_row, char_col);
		}
	      p_ptr = &py.misc;
	      p_ptr->bth += 5;
	      p_ptr->bthb+= 5;
	      p_ptr->pac += 2;
	      p_ptr->dis_ac+= 2;
	      msg_print("You feel righteous!");
	      prt_pac();
	    }
	  f_ptr->blessed--;
	  if (f_ptr->blessed == 0)
	    {
	      f_ptr->status &= 0xFFFF7FFF;
	      if (find_flag)
		{
		  find_flag = FALSE;
		  move_light (char_row, char_col, char_row, char_col);
		}
	      p_ptr = &py.misc;
	      p_ptr->bth -= 5;
	      p_ptr->bthb-= 5;
	      p_ptr->pac -= 2;
	      p_ptr->dis_ac -= 2;
	      msg_print("The prayer has expired.");
	      prt_pac();
	    }
	}
      /* Resist Heat   */
      if (f_ptr->resist_heat > 0)  f_ptr->resist_heat--;
      /* Resist Cold   */
      if (f_ptr->resist_cold > 0)  f_ptr->resist_cold--;
      /* Detect Invisible      */
      if (f_ptr->detect_inv > 0)
	{
	  if ((0x00010000 & f_ptr->status) == 0)
	    {
	      f_ptr->status |= 0x00010000;
	      f_ptr->see_inv = TRUE;
	    }
	  f_ptr->detect_inv--;
	  if (f_ptr->detect_inv == 0)
	    {
	      f_ptr->status &= 0xFFFEFFFF;
	      f_ptr->see_inv = FALSE;
	      py_bonuses(blank_treasure, 0);
	    }
	}
      /* Timed infra-vision    */
      if (f_ptr->tim_infra > 0)
	{
	  if ((0x00020000 & f_ptr->status) == 0)
	    {
	      f_ptr->status |= 0x00020000;
	      f_ptr->see_infra++;
	    }
	  f_ptr->tim_infra--;
	  if (f_ptr->tim_infra == 0)
	    {
	      f_ptr->status &= 0xFFFDFFFF;
	      f_ptr->see_infra--;
	    }
	}
	/* Word-of-Recall  Note: Word-of-Recall is a delayed action      */
      if (f_ptr->word_recall > 0)
	if (f_ptr->word_recall == 1)
	  {
	    moria_flag = TRUE;
	    f_ptr->paralysis++;
	    f_ptr->word_recall = 0;
	    if (dun_level > 0)
	      {
		dun_level = 0;
		msg_print("You feel yourself yanked upwards!");
	      }
	    else if (py.misc.max_lev != 0)
	      {
		dun_level = py.misc.max_lev;
		msg_print("You feel yourself yanked downwards!");
	      }
	  }
	else
	  f_ptr->word_recall--;

      /* Check hit points for adjusting...			*/
      p_ptr = &py.misc;
      if (!find_flag)
	if (py.flags.rest < 1)
	  {
	    if (old_chp != (int)(p_ptr->chp))
	      {
		if (p_ptr->chp > p_ptr->mhp)
		  p_ptr->chp = (double)p_ptr->mhp;
		prt_chp();
		old_chp = (int)p_ptr->chp;
	      }
	    if (old_cmana != (int)(p_ptr->cmana))
	      {
		if (p_ptr->cmana > p_ptr->mana)
		  p_ptr->cmana = (double)p_ptr->mana;
		prt_cmana();
		old_cmana = (int)p_ptr->cmana;
	      }
	  }

      if ((py.flags.paralysis < 1) &&        /* Accept a command?     */
	  (py.flags.rest < 1) &&
	  (!death))
	/* Accept a command and execute it                               */
	{
	  do
	    {
	      print_stat = 0;
	      reset_flag = FALSE;
	      /* Random teleportation  */
	      if (py.flags.teleport)
		if (randint(100) == 1)
		  {
		    find_flag = FALSE; /* no need for move_char(5) */
		    teleport(40);
		  }
	      if (!find_flag)
		{
		  /* move the cursor to the players character */
		  print("", char_row, char_col);
		  save_msg_flag = msg_flag;
		  do
		    {
		      inkey(&command);
		    }
		  while (command == ' ');
		  if (save_msg_flag)  erase_line(MSG_LINE, 0);
		  global_com_val = (command);
		}
	      /* Commands are executed in following subroutines     */
	      if (key_bindings == ORIGINAL)
		original_commands(&global_com_val);
	      else
		rogue_like_commands(&global_com_val);
	      /* End of commands                                     */
	    }
	  while ((reset_flag) && (!moria_flag));
	}
      /* Teleport?                     */
      if (teleport_flag)  teleport(100);
      /* Move the creatures            */
      if (!moria_flag)  creatures(TRUE);
      /* Exit when moria_flag is set   */
    }
  while (!moria_flag);
  if (search_flag)  search_off();  /* Fixed "SLOW" bug; 06-11-86 RAK     */
}


original_commands(com_val)
int *com_val;
{
  int y, x, dir_val;
  char command;
  int i;
  vtype out_val, tmp_str;
  register struct stats *s_ptr;
  register struct flags *f_ptr;
  int set_floor();

  switch(*com_val)
    {
    case 0: case 11:		/*^K == exit    */
      flush();
      if (get_com("Enter 'Q' to quit", &command))
	switch(command)
	  {
	  case 'q': case  'Q':
	    moria_flag = TRUE;
	    death      = TRUE;
            /* need dot on the end to be consistent with creature.c */
            (void) strcpy(died_from, "Quitting.");
	    break;
	  default:
	    break;
	  }
      reset_flag = TRUE;
      break;
    case 16:                               /*^P == repeat  */
      repeat_msg ();
      reset_flag = TRUE;
      break;
    case 23:
      if (wizard1)                        /*^W == password*/
	{
	  wizard1 = FALSE;
	  wizard2 = FALSE;
	  msg_print("Wizard mode off.");
	}
      else
	{
	  if (check_pswd())
	    {
	      msg_print("Wizard mode on.");
	    }
	}
      reset_flag = TRUE;
      break;
    case 18:                               /*^R == redraw  */
      really_clear_screen();
      draw_cave();
      reset_flag = TRUE;
      break;
    case 24:                               /*^X == save    */
      if (total_winner)
	{
	  msg_print("You are a Total Winner,  your character must be retired...");
	  msg_print("Use <Control>-K to when you are ready to quit.");
	}
      else
	{
	  if (search_flag)  search_off();
	  save_char(TRUE, FALSE);
	}
      reset_flag = TRUE;
      break;
    case 32:                                /*SPACE do nothing */
      reset_flag = TRUE;
      break;
    case '!':                                /*!  == Shell   */
      shell_out();
      reset_flag = TRUE;
      break;
    case 46:                                   /*. == find     */
      y = char_row;
      x = char_col;
      if (get_dir("Which direction?", &dir_val, com_val, &y, &x))
	{
	  find_flag = TRUE;
	  move_char(dir_val);
	}
      break;
    case 47:                          /* / == identify */
      ident_char();
      reset_flag = TRUE;
      break;
    case 49:
      move_char(*com_val - 48);        /* Move dir 1    */
      break;
    case 50:
      move_char(*com_val - 48);        /* Move dir 2    */
      break;
    case 51:
      move_char(*com_val - 48);        /* Move dir 3    */
      break;
    case 52:
      move_char(*com_val - 48);        /* Move dir 4    */
      break;
    case 53:                          /* Rest one turn */
      move_char(*com_val - 48);
#ifdef SLOW
      (void) sleep(0);     /* Sleep 1/10 a second*/
#endif
      flush();
      break;
    case 54:
      move_char(*com_val - 48);        /* Move dir 6    */
      break;
    case 55:
      move_char(*com_val - 48);        /* Move dir 7    */
      break;
    case 56:
      move_char(*com_val - 48);        /* Move dir 8    */
      break;
    case 57:
      move_char(*com_val - 48);        /* Move dir 9    */
      break;
    case 60:
      go_up();                                  /*< == go up    */
      break;
    case 62:
      go_down();                                /*> == go down  */
      break;
    case 63:                                  /*? == help     */
      original_help();
      reset_flag = TRUE;   /* Free move     */
      break;
    case 66:
      bash();                                   /*B == bash     */
      break;
    case 67:                                   /*C == character*/
      if (get_com("Print to file? (Y/N)", &command))
	switch(command)
	  {
	  case 'y': case 'Y':
	    file_character();
	    break;
	  case 'n': case 'N':
	    change_name();
	    draw_cave();
	    break;
	  default:
	    break;
	  }
      reset_flag = TRUE;   /* Free move     */
      break;
    case 68:
      disarm_trap();                            /*D == disarm   */
      break;
    case 69:
      eat();                                    /*E == eat      */
      break;
    case 70:
      refill_lamp();                            /*F == refill   */
      break;
    case 76:                                   /*L == location */
      if ((py.flags.blind > 0) || (no_light()))
	msg_print("You can't see your map.");
      else
	{
	  (void) sprintf(out_val,
			 "Section [%d,%d]; Location == [%d,%d]",
			 (((char_row-1)/OUTPAGE_HEIGHT)+1),
			 (((char_col-1)/OUTPAGE_WIDTH )+1),
			 char_row, char_col);
	  msg_print(out_val);
	}
      reset_flag = TRUE;   /* Free move     */
      break;
    case 80:                                   /*P == print map*/
      if ((py.flags.blind > 0) || (no_light()))
	msg_print("You can't see to draw a map.");
      else
	print_map();
      reset_flag = TRUE;   /* Free move     */
      break;
    case 82:
      rest();                                   /*R == rest     */
      break;
    case 83:
      if (search_flag)                    /*S == srh mode */
	{
	  search_off();
	  reset_flag = TRUE; /* Free move     */
	}
      else if (py.flags.blind > 0)
	msg_print("You are incapable of searching while blind.");
      else
	{
	  search_on();
	  reset_flag = TRUE; /* Free move     */
	}
      break;
    case 84:
      y = char_row;
      x = char_col;
      if (get_dir("Which direction?", &dir_val, com_val, &y, &x))
	{
	  tunnel(y, x);                /*T == tunnel   */
	}
      break;
    case 97:
      aim();                                    /*a == aim      */
      break;
    case 98:
      examine_book();                           /*b == browse   */
      reset_flag = TRUE;
      break;
    case 99:
      closeobject();                            /*c == close    */
      break;
    case 100:
      drop();                                   /*d == drop     */
      break;
    case 101:                                   /*e == equipment*/
      reset_flag = TRUE;   /* Free move     */
      if (inven_command('e', 0, 0))  draw_cave();
      break;
    case 102:
      throw_object();                           /*f == throw    */
      break;
#if 0
    case 104:                                 /*h == moria hlp */
      moria_help("");
      draw_cave();
      reset_flag = TRUE;   /* Free move   */
      break;
#endif
    case 105:                                /*i == inventory*/
      reset_flag = TRUE;   /* Free move     */
      if (inven_command('i', 0, 0))  draw_cave();
      break;
    case 106:
      jamdoor();                                /*j == jam      */
      break;
    case 108:                                 /*l == look     */
      look();
      reset_flag = TRUE;   /* Free move     */
      break;
    case 109:
      cast();                                   /*m == magick   */
      break;
    case 111:
      openobject();                             /*o == open     */
      break;
    case 112:
      pray();                                   /*p == pray     */
      break;
    case 113:
      quaff();                                  /*q == quaff    */
      break;
    case 114:
      read_scroll();                            /*r == read     */
      break;
    case 115:
      if (py.flags.blind > 0)             /*s == search   */
	msg_print("You are incapable of searching while blind.");
      else
	search(char_row, char_col, py.misc.srh);
      break;
    case 116:                              /*t == unwear   */
      reset_flag = TRUE;
      if (inven_command('t', 0, 0))  draw_cave();
      break;
    case 117:
      use();                                    /*u == use staff*/
      break;
    case 118:
      game_version();                           /*v == version  */
      reset_flag = TRUE;
      break;
    case 119:                                 /*w == wear     */
      reset_flag = TRUE;
      if (inven_command('w', 0, 0))  draw_cave();
      break;
    case 120:                                 /*x == exchange */
      reset_flag = TRUE;
      if (inven_command('x', 0, 0))  draw_cave();
      break;
    default:
      if (wizard1)
	{
	  reset_flag = TRUE; /* Wizard commands are free moves*/
	  switch(*com_val)
	    {
	    case 1:                            /*^A == Cure all*/
	      (void) remove_curse();
	      (void) cure_blindness();
	      (void) cure_confusion();
	      (void) cure_poison();
	      (void) remove_fear();
	      s_ptr = &py.stats;
	      s_ptr->cstr = s_ptr->str;
	      s_ptr->cint = s_ptr->intel;
	      s_ptr->cwis = s_ptr->wis;
	      s_ptr->cdex = s_ptr->dex;
	      s_ptr->ccon = s_ptr->con;
	      s_ptr->cchr = s_ptr->chr;
	      f_ptr = &py.flags;
	      if (f_ptr->slow > 1)
		f_ptr->slow = 1;
	      if (f_ptr->image > 1)
		f_ptr->image = 1;
	      /* put_qio(); */
	      /* adjust misc stats */
	      py_bonuses(blank_treasure, 0);
	      break;
	    case 2:
	      print_objects();                  /*^B == objects */
	      break;
	    case 4:                           /*^D == up/down */
	      prt("Go to which level (0 -1200) ? ", 0, 0);
	      i = -1;
	      if (get_string(tmp_str, 0, 30, 10))
		(void) sscanf(tmp_str, "%d", &i);
	      if (i > -1)
		{
		  dun_level = i;
		  if (dun_level > 1200)
		    dun_level = 1200;
		  moria_flag = TRUE;
		}
	      else
		erase_line(MSG_LINE, 0);
	      break;
	    case 8:
	      original_wizard_help();           /*^H == wizhelp */
	      break;
	    case 9:
	      (void) ident_spell();             /*^I == identify*/
	      break;
	    case 14:
	      print_monsters();                 /*^N == mon map */
	      break;
	    case 12:
	      wizard_light();                   /*^L == wizlight*/
	      break;
	    case 20:
	      teleport(100);                    /*^T == teleport*/
	      break;
	    case 22:
	      restore_char();                   /*^V == restore */
	      prt_stat_block();
	      moria_flag = TRUE;
	      break;
	    default:
	      if (wizard2)
		switch(*com_val)
		  {
		  case 5:
		    change_character();         /*^E == wizchar */
		    break;
		  case 6:
		    (void) mass_genocide();     /*^F == genocide*/
		    /* put_qio(); */
		    break;
		  case 7:                       /*^G == treasure*/
		    alloc_object(set_floor, 5, 10);
		    /* put_qio(); */
		    break;
		  case 10:                      /*^J == gain exp*/
		    if (py.misc.exp == 0)
		      py.misc.exp = 1;
		    else
		      py.misc.exp = py.misc.exp * 2;
		    prt_experience();
		    break;
		  case 21:                    /*^U == summon  */
		    y = char_row;
		    x = char_col;
		    (void) summon_monster(&y, &x, TRUE);
		    creatures(FALSE);
		    break;
		  case '@':
		    wizard_create();          /*^Q == create  */
		    break;
		  default:
		    prt("Type '?' or '^H' for help...", 0, 0);
		    break;
		  }
	      else
		prt("Type '?' or '^H' for help...", 0, 0);
	    }
	}
      else
	{
	  prt("Type '?' for help...", 0, 0);
	  reset_flag = TRUE;
	}
    }
}


rogue_like_commands(com_val)
int *com_val;
{
  int y, x;
  char command;
  int i;
  vtype out_val, tmp_str;
  register struct stats *s_ptr;
  register struct flags *f_ptr;
  int set_floor();

  switch(*com_val)
    {
    case 0: case 'Q':		/*Q == exit    */
      flush();
      if (get_com("Do you really want to quit?", &command))
	switch(command)
	  {
	  case 'y': case  'Y':
	    moria_flag = TRUE;
	    death      = TRUE;
            /* need dot on the end to be consistent with creature.c */
            (void) strcpy(died_from, "Quitting.");
	    break;
	  default:
	    break;
	  }
      reset_flag = TRUE;
      break;
    case 16:                               /*^P == repeat  */
      repeat_msg ();
      reset_flag = TRUE;
      break;
    case 23:
      if (wizard1)                        /*^W == password*/
	{
	  wizard1 = FALSE;
	  wizard2 = FALSE;
	  msg_print("Wizard mode off.");
	}
      else
	{
	  if (check_pswd())
	    {
	      msg_print("Wizard mode on.");
	    }
	}
      reset_flag = TRUE;
      break;
    case 18:                               /*^R == redraw  */
      really_clear_screen();
      draw_cave();
      reset_flag = TRUE;
      break;
    case 24:                               /*^X == save    */
      if (total_winner)
	{
	  msg_print("You are a Total Winner,  your character must be retired...");
	  msg_print("Use 'Q' to when you are ready to quit.");
	}
      else
	{
	  if (search_flag)  search_off();
	  save_char(TRUE, FALSE);
	}
      reset_flag = TRUE;
      break;
    case ' ':                                /*SPACE do nothing */
      reset_flag = TRUE;
      break;
    case '!':                                /*!  == Shell   */
      shell_out();
      reset_flag = TRUE;
      break;
    case 'b':
      move_char(1);
      break;
    case 'j':
      move_char(2);
      break;
    case 'n':
      move_char(3);
      break;
    case 'h':
      move_char(4);
      break;
    case 'l':
      move_char(6);
      break;
    case 'y':
      move_char(7);
      break;
    case 'k':
      move_char(8);
      break;
    case 'u':
      move_char(9);
      break;
    case 'B':                                   /*. == find     */
      find_flag = TRUE;
      move_char(1);
      break;
    case 'J':                                   /*. == find     */
      find_flag = TRUE;
      move_char(2);
      break;
    case 'N':                                   /*. == find     */
      find_flag = TRUE;
      move_char(3);
      break;
    case 'H':                                   /*. == find     */
      find_flag = TRUE;
      move_char(4);
      break;
    case 'L':                                   /*. == find     */
      find_flag = TRUE;
      move_char(6);
      break;
    case 'Y':                                   /*. == find     */
      find_flag = TRUE;
      move_char(7);
      break;
    case 'K':                                   /*. == find     */
      find_flag = TRUE;
      move_char(8);
      break;
    case 'U':                                   /*. == find     */
      find_flag = TRUE;
      move_char(9);
      break;
    case '/':                          /* / == identify */
      ident_char();
      reset_flag = TRUE;
      break;
    case '1':
      move_char(1);        /* Move dir 1    */
      break;
    case '2':
      move_char(2);        /* Move dir 2    */
      break;
    case '3':
      move_char(3);        /* Move dir 3    */
      break;
    case '4':
      move_char(4);        /* Move dir 4    */
      break;
    case '5':                          /* Rest one turn */
    case '.':
      move_char(5);
#ifdef SLOW
      (void) sleep(0);     /* Sleep 1/10 a second*/
#endif
      flush();
      break;
    case '6':
      move_char(6);        /* Move dir 6    */
      break;
    case '7':
      move_char(7);        /* Move dir 7    */
      break;
    case '8':
      move_char(8);        /* Move dir 8    */
      break;
    case '9':
      move_char(9);        /* Move dir 9    */
      break;
    case '<':
      go_up();                                  /*< == go up    */
      break;
    case '>':
      go_down();                                /*> == go down  */
      break;
    case '?':                                  /*? == help     */
      rogue_like_help();
      reset_flag = TRUE;   /* Free move     */
      break;
    case 'f':
      bash();                                   /*f == bash     */
      break;
    case 'C':                                   /*C == character*/
      if (get_com("Print to file? (Y/N)", &command))
	switch(command)
	  {
	  case 'y': case 'Y':
	    file_character();
	    break;
	  case 'n': case 'N':
	    change_name();
	    draw_cave();
	    break;
	  default:
	    break;
	  }
      reset_flag = TRUE;   /* Free move     */
      break;
    case 'D':
      disarm_trap();                            /*D == disarm   */
      break;
    case 'E':
      eat();                                    /*E == eat      */
      break;
    case 'F':
      refill_lamp();                            /*F == refill   */
      break;
    case 'W':                                   /*W == location */
      if ((py.flags.blind > 0) || (no_light()))
	msg_print("You can't see your map.");
      else
	{
	  (void) sprintf(out_val,
			 "Section [%d,%d]; Location == [%d,%d]",
			 (((char_row-1)/OUTPAGE_HEIGHT)+1),
			 (((char_col-1)/OUTPAGE_WIDTH )+1),
			 char_row, char_col);
	  msg_print(out_val);
	}
      reset_flag = TRUE;   /* Free move     */
      break;
    case 'M':                                   /*M == print map*/
      if ((py.flags.blind > 0) || (no_light()))
	msg_print("You can't see to draw a map.");
      else
	print_map();
      reset_flag = TRUE;   /* Free move     */
      break;
    case 'R':
      rest();                                   /*R == rest     */
      break;
    case '#':
      if (search_flag)                    /*^S == srh mode */
	{
	  search_off();
	  reset_flag = TRUE; /* Free move     */
	}
      else if (py.flags.blind > 0)
	msg_print("You are incapable of searching while blind.");
      else
	{
	  search_on();
	  reset_flag = TRUE; /* Free move     */
	}
      break;
    case 25:  /* ^Y */
      x = char_row;
      y = char_col;
      (void) move(7, &x, &y);
      tunnel(x, y);
      break;
    case 11:  /* ^K */
      x = char_row;
      y = char_col;
      (void) move(8, &x, &y);
      tunnel(x, y);
      break;
    case 21: /* ^U */
      x = char_row;
      y = char_col;
      (void) move(9, &x, &y);
      tunnel(x, y);
      break;
    case 12:  /* ^L */
      x = char_row;
      y = char_col;
      (void) move(6, &x, &y);
      tunnel(x, y);
      break;
    case 14:  /* ^N */
      x = char_row;
      y = char_col;
      (void) move(3, &x, &y);
      tunnel(x, y);
      break;
    case 10:    /* ^J */
      x = char_row;
      y = char_col;
      (void) move(2, &x, &y);
      tunnel(x, y);
      break;
    case 2:  /* ^B */
      x = char_row;
      y = char_col;
      (void) move(1, &x, &y);
      tunnel(x, y);
      break;
    case 8:  /* ^H */
      x = char_row;
      y = char_col;
      (void) move(4, &x, &y);
      tunnel(x, y);
      break;
    case 'z':
     aim();                                    /*z == aim      */
      break;
    case 'P':
      examine_book();                           /*P == browse   */
      reset_flag = TRUE;
      break;
    case 'c':
      closeobject();                            /*c == close    */
      break;
    case 'd':
      drop();                                   /*d == drop     */
      break;
    case 'e':                                   /*e == equipment*/
      reset_flag = TRUE;   /* Free move     */
      if (inven_command('e', 0, 0))  draw_cave();
      break;
    case 't':
      throw_object();                           /*t == throw    */
      break;
#if 0
    case 104:                                 /*h == moria hlp */
      moria_help("");
      draw_cave();
      reset_flag = TRUE;   /* Free move    */
      break;
#endif
    case 'i':                                /*i == inventory*/
      reset_flag = TRUE;   /* Free move     */
      if (inven_command('i', 0, 0))  draw_cave();
      break;
    case 'S':
      jamdoor();                                /*S == jam      */
      break;
    case 'x':                                 /*l == look     */
      look();
      reset_flag = TRUE;   /* Free move     */
      break;
    case 'm':
      cast();                                   /*m == magick   */
      break;
    case 'o':
      openobject();                             /*o == open     */
      break;
    case 'p':
      pray();                                   /*p == pray     */
      break;
    case 'q':
      quaff();                                  /*q == quaff    */
      break;
    case 'r':
      read_scroll();                            /*r == read     */
      break;
    case 's':
      if (py.flags.blind > 0)             /*s == search   */
	msg_print("You are incapable of searching while blind.");
      else
	search(char_row, char_col, py.misc.srh);
      break;
    case 'T':                              /*T == unwear   */
      reset_flag = TRUE;
      if (inven_command('t', 0, 0))  draw_cave();
      break;
    case 'Z':
      use();                                    /*Z == use staff*/
      break;
    case 'v':
      game_version();                           /*v == version  */
      reset_flag = TRUE;
      break;
    case 'w':                                 /*w == wear     */
      reset_flag = TRUE;
      if (inven_command('w', 0, 0))  draw_cave();
      break;
    case 'X':                                 /*x == exchange */
      reset_flag = TRUE;
      if (inven_command('x', 0, 0))  draw_cave();
      break;
    default:
      if (wizard1)
	{
	  reset_flag = TRUE; /* Wizard commands are free moves*/
	  switch(*com_val)
	    {
	    case 1:                            /*^A == Cure all*/
	      (void) remove_curse();
	      (void) cure_blindness();
	      (void) cure_confusion();
	      (void) cure_poison();
	      (void) remove_fear();
	      s_ptr = &py.stats;
	      s_ptr->cstr = s_ptr->str;
	      s_ptr->cint = s_ptr->intel;
	      s_ptr->cwis = s_ptr->wis;
	      s_ptr->cdex = s_ptr->dex;
	      s_ptr->ccon = s_ptr->con;
	      s_ptr->cchr = s_ptr->chr;
	      f_ptr = &py.flags;
	      if (f_ptr->slow > 1)
		f_ptr->slow = 1;
	      if (f_ptr->image > 1)
		f_ptr->image = 1;
	      /* put_qio(); */
	      /* adjust misc stats */
	      py_bonuses(blank_treasure, 0);
	      break;
	    case 15:
	      print_objects();                  /*^O == objects */
	      break;
	    case 4:                           /*^D == up/down */
	      prt("Go to which level (0 -1200) ? ", 0, 0);
	      i = -1;
	      if (get_string(tmp_str, 0, 30, 10))
		(void) sscanf(tmp_str, "%d", &i);
	      if (i > -1)
		{
		  dun_level = i;
		  if (dun_level > 1200)
		    dun_level = 1200;
		  moria_flag = TRUE;
		}
	      else
		erase_line(MSG_LINE, 0);
	      break;
	    case 127: /* ^? DEL */
	      rogue_like_wizard_help();         /*DEL == wizhelp */
	      break;
	    case 9:
	      (void) ident_spell();             /*^I == identify*/
	      break;
	    case 13:
	      print_monsters();                 /*^M == mon map */
	      break;
	    case '*':
	      wizard_light();                   /*` == wizlight*/
	      break;
	    case 20:
	      teleport(100);                    /*^T == teleport*/
	      break;
	    case 22:
	      restore_char();                   /*^V == restore */
	      prt_stat_block();
	      moria_flag = TRUE;
	      break;
	    default:
	      if (wizard2)
		switch(*com_val)
		  {
		  case 5:
		    change_character();         /*^E == wizchar */
		    break;
		  case 6:
		    (void) mass_genocide();     /*^F == genocide*/
		    /* put_qio(); */
		    break;
		  case 7:                       /*^G == treasure*/
		    alloc_object(set_floor, 5, 10);
		    /* put_qio(); */
		    break;
		  case '+':                      /*+ == gain exp*/
		    if (py.misc.exp == 0)
		      py.misc.exp = 1;
		    else
		      py.misc.exp = py.misc.exp * 2;
		    prt_experience();
		    break;
		  case 19:                    /*^S == summon  */
		    y = char_row;
		    x = char_col;
		    (void) summon_monster(&y, &x, TRUE);
		    creatures(FALSE);
		    break;
		  case '@':
		    wizard_create();          /*^Q == create  */
		    break;
		  default:
		    prt("Type '?' or DELETE for help...", 0, 0);
		    break;
		  }
	      else
		prt("Type '?' or DELETE for help...", 0, 0);
	    }
	}
      else
	{
	  prt("Type '?' for help...", 0, 0);
	  reset_flag = TRUE;
	}
    }
}
