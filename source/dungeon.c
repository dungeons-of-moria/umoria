/* source/dungeon.c: the main command interpreter, updating player status

   Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#ifdef __TURBOC__
#include	<conio.h>
#include	<stdio.h>
#include	<stdlib.h>
#endif /* __TURBOC__ */
 
#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"

#ifdef USG
#ifndef ATARIST_MWC
#include <string.h>
#endif
#else
#include <strings.h>
#endif

#if defined(LINT_ARGS)
static char original_commands(char);
static void do_command(char);
static int valid_countcommand(char);
static void regenhp(int);
static void regenmana(int);
static int enchanted(inven_type *);
static void examine_book(void);
static void go_up(void);
static void go_down(void);
static void jamdoor(void);
static void refill_lamp(void);
#else
static char original_commands();
static void do_command();
static int valid_countcommand();
static void regenhp();
static void regenmana();
static int enchanted();
static void examine_book();
static void go_up();
static void go_down();
static void jamdoor();
static void refill_lamp();
#endif

#ifdef ATARIST_TC
/* Include this to get prototypes for standard library functions.  */
#include <stdlib.h>
#endif

/* Moria game module					-RAK-	*/
/* The code in this section has gone through many revisions, and */
/* some of it could stand some more hard work.	-RAK-	       */

/* It has had a bit more hard work.			-CJS- */

void dungeon()
{
  int find_count, i;
  int regen_amount;	    /* Regenerate hp and mana*/
  char command;		/* Last command		 */
  register struct misc *p_ptr;
  register inven_type *i_ptr;
  register struct flags *f_ptr;
#ifdef ATARIST_WMC
  int32u holder;
#endif

  /* Main procedure for dungeon.			-RAK-	*/
  /* Note: There is a lot of preliminary magic going on here at first*/

  /* init pointers. */
  f_ptr = &py.flags;
  p_ptr = &py.misc;

  /* Check light status for setup	   */
  i_ptr = &inventory[INVEN_LIGHT];
  if (i_ptr->p1 > 0)
    player_light = TRUE;
  else
    player_light = FALSE;
  /* Check for a maximum level		   */
  if (dun_level > p_ptr->max_dlv)
    p_ptr->max_dlv = dun_level;

  /* Reset flags and initialize variables  */
  command_count = 0;
  find_count = 0;
  new_level_flag    = FALSE;
  find_flag	= FALSE;
  teleport_flag = FALSE;
  mon_tot_mult	= 0;
  cave[char_row][char_col].cptr = 1;
  /* Ensure we display the panel. Used to do this with a global var. -CJS- */
  panel_row = panel_col = -1;
  /* Light up the area around character	   */
  check_view ();
  /* must do this after panel_row/col set to -1, because search_off() will
     call check_view(), and so the panel_* variables must be valid before
     search_off() is called */
  if (py.flags.status & PY_SEARCH)
    search_off();
  /* Light,  but do not move critters	    */
  creatures(FALSE);
  /* Print the depth			   */
  prt_depth();
#if 0
  /* This can't be right.  */
#ifdef ATARIST_MWC
  prt_map();
#endif
#endif

  /* Loop until dead,  or new level		*/
  do
    {
      /* Increment turn counter			*/
      turn++;
#ifndef MAC
      /* The Mac ignores the game hours file		*/
      /* Check for game hours			       */
      if (((turn % 250) == 1) && !check_time())
	if (closing_flag > 4)
	  {
	    msg_print("The gates to Moria are now closed.");
	    (void) strcpy (died_from, "(closing gate: saved)");
	    if (!save_char())
	      {
		(void) strcpy (died_from, "a slammed gate");
		death = TRUE;
	      }
	    exit_game();
	  }
	else
	  {
	    disturb (0, 0);
	    closing_flag++;
	    msg_print("The gates to Moria are closing.");
	    msg_print("Please finish up or save your game.");
	  }
#endif

      /* turn over the store contents every, say, 1000 turns */
      if ((dun_level != 0) && ((turn % 1000) == 0))
	store_maint();

      /* Check for creature generation		*/
      if (randint(MAX_MALLOC_CHANCE) == 1)
	alloc_monster(1, MAX_SIGHT, FALSE);
      /* Check light status			       */
      i_ptr = &inventory[INVEN_LIGHT];
      if (player_light)
	if (i_ptr->p1 > 0)
	  {
	    i_ptr->p1--;
	    if (i_ptr->p1 == 0)
	      {
		player_light = FALSE;
		msg_print("Your light has gone out!");
		disturb (0, 1);
		/* unlight creatures */
		creatures(FALSE);
	      }
	    else if ((i_ptr->p1 < 40) && (randint(5) == 1) &&
		     (py.flags.blind < 1))
	      {
		disturb (0, 0);
		msg_print("Your light is growing faint.");
	      }
	  }
	else
	  {
	    player_light = FALSE;
	    disturb (0, 1);
	    /* unlight creatures */
	    creatures(FALSE);
	  }
      else if (i_ptr->p1 > 0)
	{
	  i_ptr->p1--;
	  player_light = TRUE;
	  disturb (0, 1);
	  /* light creatures */
	  creatures(FALSE);
	}

      /* Update counters and messages			*/
      /* Check food status	       */
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
	      if ((PY_WEAK & f_ptr->status) == 0)
		{
		  f_ptr->status |= PY_WEAK;
		  msg_print("You are getting weak from hunger.");
		  disturb (0, 0);
		  prt_hunger();
		}
	      if ((f_ptr->food < PLAYER_FOOD_FAINT) && (randint(8) == 1))
		{
		  f_ptr->paralysis += randint(5);
		  msg_print("You faint from the lack of food.");
		  disturb (1, 0);
		}
	    }
	  else if ((PY_HUNGRY & f_ptr->status) == 0)
	    {
	      f_ptr->status |= PY_HUNGRY;
	      msg_print("You are getting hungry.");
	      disturb (0, 0);
	      prt_hunger();
	    }
	}
      /* Food consumption	*/
      /* Note: Speeded up characters really burn up the food!  */
      if (f_ptr->speed < 0)
	f_ptr->food -=	f_ptr->speed*f_ptr->speed;
      f_ptr->food -= f_ptr->food_digested;
      if (f_ptr->food < 0)
	{
	  take_hit (-f_ptr->food/16, "starvation");   /* -CJS- */
	  disturb(1, 0);
	}
      /* Regenerate	       */
      if (f_ptr->regenerate)  regen_amount = regen_amount * 3 / 2;
      if ((py.flags.status & PY_SEARCH) || f_ptr->rest != 0)
	regen_amount = regen_amount * 2;
      if ((py.flags.poisoned < 1) && (p_ptr->chp < p_ptr->mhp))
	regenhp(regen_amount);
      if (p_ptr->cmana < p_ptr->mana)
	regenmana(regen_amount);
      /* Blindness	       */
      if (f_ptr->blind > 0)
	{
	  if ((PY_BLIND & f_ptr->status) == 0)
	    {
	      f_ptr->status |= PY_BLIND;
	      prt_map();
	      prt_blind();
	      disturb (0, 1);
	      /* unlight creatures */
	      creatures (FALSE);
	    }
	  f_ptr->blind--;
	  if (f_ptr->blind == 0)
	    {
#ifdef ATARIST_MWC
	      f_ptr->status &= ~(holder = PY_BLIND);
#else
	      f_ptr->status &= ~PY_BLIND;
#endif
	      prt_blind();
	      prt_map();
	      /* light creatures */
	      disturb (0, 1);
	      creatures(FALSE);
	      msg_print("The veil of darkness lifts.");
	    }
	}
      /* Confusion	       */
      if (f_ptr->confused > 0)
	{
	  if ((PY_CONFUSED & f_ptr->status) == 0)
	    {
	      f_ptr->status |= PY_CONFUSED;
	      prt_confused();
	    }
	  f_ptr->confused--;
	  if (f_ptr->confused == 0)
	    {
#ifdef ATARIST_MWC
	      f_ptr->status &= ~(holder = PY_CONFUSED);
#else
	      f_ptr->status &= ~PY_CONFUSED;
#endif
	      prt_confused();
	      msg_print("You feel less confused now.");
	      if (py.flags.rest != 0)
		rest_off ();
	    }
	}
      /* Afraid		       */
      if (f_ptr->afraid > 0)
	{
	  if ((PY_FEAR & f_ptr->status) == 0)
	    {
	      if ((f_ptr->shero+f_ptr->hero) > 0)
		f_ptr->afraid = 0;
	      else
		{
		  f_ptr->status |= PY_FEAR;
		  prt_afraid();
		}
	    }
	  else if ((f_ptr->shero+f_ptr->hero) > 0)
	    f_ptr->afraid = 1;
	  f_ptr->afraid--;
	  if (f_ptr->afraid == 0)
	    {
#ifdef ATARIST_MWC
	      f_ptr->status &= ~(holder = PY_FEAR);
#else
	      f_ptr->status &= ~PY_FEAR;
#endif
	      prt_afraid();
	      msg_print("You feel bolder now.");
	      disturb (0, 0);
	    }
	}
      /* Poisoned	       */
      if (f_ptr->poisoned > 0)
	{
	  if ((PY_POISONED & f_ptr->status) == 0)
	    {
	      f_ptr->status |= PY_POISONED;
	      prt_poisoned();
	    }
	  f_ptr->poisoned--;
	  if (f_ptr->poisoned == 0)
	    {
#ifdef ATARIST_MWC
	      f_ptr->status &= ~(holder = PY_POISONED);
#else
	      f_ptr->status &= ~PY_POISONED;
#endif
	      prt_poisoned();
	      msg_print("You feel better.");
	      disturb (0, 0);
	    }
	  else
	    {
	      switch(con_adj())
		{
		case -4:  i = 4;  break;
		case -3:
		case -2:  i = 3;  break;
		case -1:  i = 2;  break;
		case 0:	  i = 1;  break;
		case 1: case 2: case 3:
		  i = ((turn % 2) == 0);
		  break;
		case 4: case 5:
		  i = ((turn % 3) == 0);
		  break;
		case 6:
		  i = ((turn % 4) == 0);
		  break;
		}
	      take_hit (i, "poison");
	      disturb (1, 0);
	    }
	}
      /* Fast		       */
      if (f_ptr->fast > 0)
	{
	  if ((PY_FAST & f_ptr->status) == 0)
	    {
	      f_ptr->status |= PY_FAST;
	      change_speed(-1);
	      msg_print("You feel yourself moving faster.");
	      disturb (0, 0);
	    }
	  f_ptr->fast--;
	  if (f_ptr->fast == 0)
	    {
#ifdef ATARIST_MWC
	      f_ptr->status &= ~(holder = PY_FAST);
#else
	      f_ptr->status &= ~PY_FAST;
#endif
	      change_speed(1);
	      msg_print("You feel yourself slow down.");
	      disturb (0, 0);
	    }
	}
      /* Slow		       */
      if (f_ptr->slow > 0)
	{
	  if ((PY_SLOW & f_ptr->status) == 0)
	    {
	      f_ptr->status |= PY_SLOW;
	      change_speed(1);
	      msg_print("You feel yourself moving slower.");
	      disturb (0, 0);
	    }
	  f_ptr->slow--;
	  if (f_ptr->slow == 0)
	    {
#ifdef ATARIST_MWC
	      f_ptr->status &= ~(holder = PY_SLOW);
#else
	      f_ptr->status &= ~PY_SLOW;
#endif
	      change_speed(-1);
	      msg_print("You feel yourself speed up.");
	      disturb (0, 0);
	    }
	}
      /* Resting is over?      */
      if (f_ptr->rest > 0)
	{
	  f_ptr->rest--;
	  if (f_ptr->rest == 0)		      /* Resting over	       */
	    rest_off();
	}
      else if (f_ptr->rest < 0)
	{
	  /* Rest until reach max mana and max hit points.  */
	  f_ptr->rest++;
	  if ((p_ptr->chp == p_ptr->mhp && p_ptr->cmana == p_ptr->mana)
	      || f_ptr->rest == 0)
	    rest_off();
	}

      /* Check for interrupts to find or rest. */
#ifdef MAC
      /* On Mac, checking for input is expensive, since it involves handling
	 events, so we only check in multiples of 16 turns. */
      if (!(turn & 0xF) && ((command_count > 0) || find_flag
			     || (f_ptr->rest != 0)))
	if (macgetkey(CNIL, TRUE))
	  disturb(0, 0);
#else
      if ((command_count > 0 || find_flag || f_ptr->rest != 0)
#if defined(MSDOS) || defined(VMS)
	  && kbhit()
#else
	  && (check_input (find_flag ? 0 : 10000))
#endif
	  )
	{
#ifdef MSDOS
	  (void) msdos_getch();
#endif
#ifdef VMS
	  /* Get and ignore the key used to interrupt resting/running.  */
	  (void) vms_getch ();
#endif
	  disturb (0, 0);
	}
#endif

      /* Hallucinating?	 (Random characters appear!)*/
      if (f_ptr->image > 0)
	{
	  end_find ();
	  f_ptr->image--;
	  if (f_ptr->image == 0)
	    prt_map ();	 /* Used to draw entire screen! -CJS- */
	}
      /* Paralysis	       */
      if (f_ptr->paralysis > 0)
	{
	  /* when paralysis true, you can not see any movement that occurs */
	  f_ptr->paralysis--;
	  disturb (1, 0);
	}
      /* Protection from evil counter*/
      if (f_ptr->protevil > 0)
	{
	  f_ptr->protevil--;
	  if (f_ptr->protevil == 0)
	    msg_print ("You no longer feel safe from evil.");
	}
      /* Invulnerability	*/
      if (f_ptr->invuln > 0)
	{
	  if ((PY_INVULN & f_ptr->status) == 0)
	    {
	      f_ptr->status |= PY_INVULN;
	      disturb (0, 0);
	      py.misc.pac += 100;
	      py.misc.dis_ac += 100;
	      prt_pac();
	      msg_print("Your skin turns into steel!");
	    }
	  f_ptr->invuln--;
	  if (f_ptr->invuln == 0)
	    {
#ifdef ATARIST_MWC
	      f_ptr->status &= ~(holder = PY_INVULN);
#else
	      f_ptr->status &= ~PY_INVULN;
#endif
	      disturb (0, 0);
	      py.misc.pac -= 100;
	      py.misc.dis_ac -= 100;
	      prt_pac();
	      msg_print("Your skin returns to normal.");
	    }
	}
      /* Heroism       */
      if (f_ptr->hero > 0)
	{
	  if ((PY_HERO & f_ptr->status) == 0)
	    {
	      f_ptr->status |= PY_HERO;
	      disturb (0, 0);
	      p_ptr->mhp += 10;
	      p_ptr->chp += 10;
	      p_ptr->bth += 12;
	      p_ptr->bthb+= 12;
	      msg_print("You feel like a HERO!");
	      prt_mhp();
	      prt_chp();
	    }
	  f_ptr->hero--;
	  if (f_ptr->hero == 0)
	    {
#ifdef ATARIST_MWC
	      f_ptr->status &= ~(holder = PY_HERO);
#else
	      f_ptr->status &= ~PY_HERO;
#endif
	      disturb (0, 0);
	      p_ptr->mhp -= 10;
	      if (p_ptr->chp > p_ptr->mhp)
		{
		  p_ptr->chp = p_ptr->mhp;
		  p_ptr->chp_frac = 0;
		  prt_chp();
		}
	      p_ptr->bth -= 12;
	      p_ptr->bthb-= 12;
	      msg_print("The heroism wears off.");
	      prt_mhp();
	    }
	}
      /* Super Heroism */
      if (f_ptr->shero > 0)
	{
	  if ((PY_SHERO & f_ptr->status) == 0)
	    {
	      f_ptr->status |= PY_SHERO;
	      disturb (0, 0);
	      p_ptr->mhp += 20;
	      p_ptr->chp += 20;
	      p_ptr->bth += 24;
	      p_ptr->bthb+= 24;
	      msg_print("You feel like a SUPER HERO!");
	      prt_mhp();
	      prt_chp();
	    }
	  f_ptr->shero--;
	  if (f_ptr->shero == 0)
	    {
#ifdef ATARIST_MWC
	      f_ptr->status &= ~(holder = PY_SHERO);
#else
	      f_ptr->status &= ~PY_SHERO;
#endif
	      disturb (0, 0);
	      p_ptr->mhp -= 20;
	      if (p_ptr->chp > p_ptr->mhp)
		{
		  p_ptr->chp = p_ptr->mhp;
		  p_ptr->chp_frac = 0;
		  prt_chp();
		}
	      p_ptr->bth -= 24;
	      p_ptr->bthb-= 24;
	      msg_print("The super heroism wears off.");
	      prt_mhp();
	    }
	}
      /* Blessed       */
      if (f_ptr->blessed > 0)
	{
	  if ((PY_BLESSED & f_ptr->status) == 0)
	    {
	      f_ptr->status |= PY_BLESSED;
	      disturb (0, 0);
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
#ifdef ATARIST_MWC
	      f_ptr->status &= ~(holder = PY_BLESSED);
#else
	      f_ptr->status &= ~PY_BLESSED;
#endif
	      disturb (0, 0);
	      p_ptr->bth -= 5;
	      p_ptr->bthb-= 5;
	      p_ptr->pac -= 2;
	      p_ptr->dis_ac -= 2;
	      msg_print("The prayer has expired.");
	      prt_pac();
	    }
	}
      /* Resist Heat   */
      if (f_ptr->resist_heat > 0)
	{
	  f_ptr->resist_heat--;
	  if (f_ptr->resist_heat == 0)
	    msg_print ("You no longer feel safe from flame.");
	}
      /* Resist Cold   */
      if (f_ptr->resist_cold > 0)
	{
	  f_ptr->resist_cold--;
	  if (f_ptr->resist_cold == 0)
	    msg_print ("You no longer feel safe from cold.");
	}
      /* Detect Invisible      */
      if (f_ptr->detect_inv > 0)
	{
#ifdef ATARIST_MWC
	  if (((holder = PY_DET_INV) & f_ptr->status) == 0)
#else
	  if ((PY_DET_INV & f_ptr->status) == 0)
#endif
	    {
#ifdef ATARIST_MWC
	      f_ptr->status |= holder;
#else
	      f_ptr->status |= PY_DET_INV;
#endif
	      f_ptr->see_inv = TRUE;
	      /* light but don't move creatures */
	      creatures (FALSE);
	    }
	  f_ptr->detect_inv--;
	  if (f_ptr->detect_inv == 0)
	    {
#ifdef ATARIST_MWC
	      f_ptr->status &= ~holder;
#else
	      f_ptr->status &= ~PY_DET_INV;
#endif
	      /* may still be able to see_inv if wearing magic item */
	      calc_bonuses();
	      /* unlight but don't move creatures */
	      creatures (FALSE);
	    }
	}
      /* Timed infra-vision    */
      if (f_ptr->tim_infra > 0)
	{
#ifdef ATARIST_MWC
	  if (((holder = PY_TIM_INFRA) & f_ptr->status) == 0)
#else
	  if ((PY_TIM_INFRA & f_ptr->status) == 0)
#endif
	    {
#ifdef ATARIST_MWC
	      f_ptr->status |= holder;
#else
	      f_ptr->status |= PY_TIM_INFRA;
#endif
	      f_ptr->see_infra++;
	      /* light but don't move creatures */
	      creatures (FALSE);
	    }
	  f_ptr->tim_infra--;
	  if (f_ptr->tim_infra == 0)
	    {
#ifdef ATARIST_MWC
	      f_ptr->status &= ~holder;
#else
	      f_ptr->status &= ~PY_TIM_INFRA;
#endif
	      f_ptr->see_infra--;
	      /* unlight but don't move creatures */
	      creatures (FALSE);
	    }
	}
	/* Word-of-Recall  Note: Word-of-Recall is a delayed action	 */
      if (f_ptr->word_recall > 0)
	if (f_ptr->word_recall == 1)
	  {
	    new_level_flag = TRUE;
	    f_ptr->paralysis++;
	    f_ptr->word_recall = 0;
	    if (dun_level > 0)
	      {
		dun_level = 0;
		msg_print("You feel yourself yanked upwards!");
	      }
	    else if (py.misc.max_dlv != 0)
	      {
		dun_level = py.misc.max_dlv;
		msg_print("You feel yourself yanked downwards!");
	      }
	  }
	else
	  f_ptr->word_recall--;

      /* Random teleportation  */
      if ((py.flags.teleport) && (randint(100) == 1))
	{
	  disturb (0, 0);
	  teleport(40);
	}

      /* See if we are too weak to handle the weapon or pack.  -CJS- */
#ifdef ATARIST_MWC
      if (py.flags.status & (holder = PY_STR_WGT))
#else
      if (py.flags.status & PY_STR_WGT)
#endif
	check_strength();
      if (py.flags.status & PY_STUDY)
	prt_study();
#ifdef ATARIST_MWC
      if (py.flags.status & (holder = PY_SPEED))
#else
      if (py.flags.status & PY_SPEED)
#endif
	{
#ifdef ATARIST_MWC
	  py.flags.status &= ~holder;
#else
	  py.flags.status &= ~PY_SPEED;
#endif
	  prt_speed();
	}
#ifdef ATARIST_MWC
      if ((py.flags.status & (holder = PY_PARALYSED))
	  && (py.flags.paralysis < 1))
#else
      if ((py.flags.status & PY_PARALYSED) && (py.flags.paralysis < 1))
#endif
	{
	  prt_state();
#ifdef ATARIST_MWC
	  py.flags.status &= ~holder;
#else
	  py.flags.status &= ~PY_PARALYSED;
#endif
	}
      else if (py.flags.paralysis > 0)
	{
	  prt_state();
#ifdef ATARIST_MWC
	  py.flags.status |= (holder = PY_PARALYSED);
#else
	  py.flags.status |= PY_PARALYSED;
#endif
	}
      else if (py.flags.rest != 0)
	prt_state();

#ifdef ATARIST_MWC
      if ((py.flags.status & (holder = PY_ARMOR)) != 0)
#else
      if ((py.flags.status & PY_ARMOR) != 0)
#endif
	{
	  prt_pac();
#ifdef ATARIST_MWC
	  py.flags.status &= ~holder;
#else
	  py.flags.status &= ~PY_ARMOR;
#endif
	}
#ifdef ATARIST_MWC
      if ((py.flags.status & (holder = PY_STATS)) != 0)
#else
      if ((py.flags.status & PY_STATS) != 0)
#endif
	{
	  for (i = 0; i < 6; i++)
#ifdef ATARIST_MWC
	    if (((holder = PY_STR) << i) & py.flags.status)
#else
	    if ((PY_STR << i) & py.flags.status)
#endif
	      prt_stat(i);
#ifdef ATARIST_MWC
	  py.flags.status &= ~(holder = PY_STATS);
#else
	  py.flags.status &= ~PY_STATS;
#endif
	}
#ifdef ATARIST_MWC
      if (py.flags.status & (holder = PY_HP))
#else
      if (py.flags.status & PY_HP)
#endif
	{
	  prt_mhp();
	  prt_chp();
#ifdef ATARIST_MWC
	  py.flags.status &= ~holder;
#else
	  py.flags.status &= ~PY_HP;
#endif
	}
#ifdef ATARIST_MWC
      if (py.flags.status & (holder = PY_MANA))
#else
      if (py.flags.status & PY_MANA)
#endif
	{
	  prt_cmana();
#ifdef ATARIST_MWC
	  py.flags.status &= ~holder;
#else
	  py.flags.status &= ~PY_MANA;
#endif
	}

      /* Allow for a slim chance of detect enchantment -CJS- */
      /* for 1st level char, check once every 2160 turns
	 for 40th level char, check once every 416 turns */
      if (((turn & 0xF) == 0) && (f_ptr->confused == 0)
	  && (randint((int)(10 + 750 / (5 + py.misc.lev))) == 1))
	{
	  vtype tmp_str;

	  for (i = 0; i < INVEN_ARRAY_SIZE; i++)
	    {
	      if (i == inven_ctr)
		i = 22;
	      i_ptr = &inventory[i];
	      /* if in inventory, succeed 1 out of 50 times,
		 if in equipment list, success 1 out of 10 times */
	      if ((i_ptr->tval != TV_NOTHING) && enchanted(i_ptr) &&
		  (randint(i < 22 ? 50 : 10) == 1))
		{
		  extern char *describe_use();

		  (void) sprintf(tmp_str,
				 "There's something about what you are %s...",
				 describe_use(i));
		  disturb(0, 0);
		  msg_print(tmp_str);
		  add_inscribe(i_ptr, ID_MAGIK);
		}
	    }
	}

      /* Check the state of the monster list, and delete some monsters if
	 the monster list is nearly full.  This helps to avoid problems in
	 creature.c when monsters try to multiply.  Compact_monsters() is
	 much more likely to succeed if called from here, than if called
	 from within creatures().  */
      if (MAX_MALLOC - mfptr < 10)
	(void) compact_monsters ();

      if ((py.flags.paralysis < 1) &&	     /* Accept a command?     */
	  (py.flags.rest == 0) &&
	  (!death))
	/* Accept a command and execute it				 */
	{
	  do
	    {
#ifdef ATARIST_MWC
	      if (py.flags.status & (holder = PY_REPEAT))
#else
	      if (py.flags.status & PY_REPEAT)
#endif
		prt_state ();
	      default_dir = FALSE;
	      free_turn_flag = FALSE;

	      if (find_flag)
		{
		  find_run();
		  find_count--;
		  if (find_count == 0)
		    end_find();
		  put_qio();
		}
	      else if (doing_inven)
		inven_command (doing_inven);
	      else
		{
		  /* move the cursor to the players character */
		  move_cursor_relative (char_row, char_col);
		  if (command_count > 0)
		    {
		      msg_flag = FALSE;
		      default_dir = TRUE;
		    }
		  else
		    {
		      msg_flag = FALSE;
#ifdef MAC
		      unloadsegments();
		      enablesavecmd(TRUE);
		      command = inkeydir();
		      enablesavecmd(FALSE);
#else
		      command = inkey();
#endif
		      i = 0;
		      /* Get a count for a command. */
		      if ((rogue_like_commands
			   && command >= '0' && command <= '9')
			  || (!rogue_like_commands && command == '#'))
			{
			  char tmp[8];

			  prt("Repeat count:", 0, 0);
			  if (command == '#')
			    command = '0';
			  i = 0;
			  while (TRUE)
			    {
			      if (command == DELETE || command == CTRL('H'))
				{
				  i = i / 10;
				  (void) sprintf(tmp, "%d", i);
				  prt (tmp, 0, 14);
				}
			      else if (command >= '0' && command <= '9')
				{
			          if (i > 99)
				    bell ();
				  else
				    {
				      i = i * 10 + command - '0';
				      (void) sprintf (tmp, "%d", i);
				      prt (tmp, 0, 14);
				    }
				}
			      else
				break;
#ifdef MAC
			      command = inkeydir();
#else
			      command = inkey();
#endif
			    }
			  if (i == 0)
			    {
			      i = 99;
			      (void) sprintf (tmp, "%d", i);
			      prt (tmp, 0, 14);
			    }
			  /* a special hack to allow numbers as commands */
			  if (command == ' ')
			    {
			      prt ("Command:", 0, 20);
#ifdef MAC
			      command = inkeydir();
#else
			      command = inkey();
#endif
			    }
			}
		      /* Another way of typing control codes -CJS- */
		      if (command == '^')
			{
			  if (command_count > 0)
			    prt_state();
			  if (get_com("Control-", &command))
			    {
			      if (command >= 'A' && command <= 'Z')
				command -= 'A' - 1;
			      else if (command >= 'a' && command <= 'z')
				command -= 'a' - 1;
			      else
				{
			       msg_print("Type ^ <letter> for a control char");
				  command = ' ';
				}
			    }
			  else
			    command = ' ';
			}
		      /* move cursor to player char again, in case it moved */
		      move_cursor_relative (char_row, char_col);
		      /* Commands are always converted to rogue form. -CJS- */
		      if (rogue_like_commands == FALSE)
			command = original_commands (command);
		      if (i > 0)
			{
			  if (!valid_countcommand(command))
			    {
			      free_turn_flag = TRUE;
			      msg_print ("Invalid command with a count.");
			      command = ' ';
			    }
			  else
			    {
			      command_count = i;
			      prt_state ();
			    }
			}
		    }
		  /* Flash the message line. */
		  erase_line(MSG_LINE, 0);
		  move_cursor_relative(char_row, char_col);
		  put_qio();

		  do_command (command);
		  /* Find is counted differently, as the command changes. */
		  if (find_flag)
		    {
		      find_count = command_count - 1;
		      command_count = 0;
		    }
		  else if (free_turn_flag)
		    command_count = 0;
		  else if (command_count)
		    command_count--;
		}
	      /* End of commands				     */
	    }
	  while (free_turn_flag && !new_level_flag && !eof_flag);
	}
      else
	{
	  /* if paralyzed, resting, or dead, flush output */
	  /* but first move the cursor onto the player, for aesthetics */
	  move_cursor_relative (char_row, char_col);
	  put_qio ();
	}

      /* Teleport?		       */
      if (teleport_flag)  teleport(100);
      /* Move the creatures	       */
      if (!new_level_flag)  creatures(TRUE);
      /* Exit when new_level_flag is set   */
    }
  while (!new_level_flag && !eof_flag);
}


static char original_commands(com_val)
char com_val;
{
  int dir_val;

  switch(com_val)
    {
    case CTRL('K'):	/*^K = exit    */
      com_val = 'Q';
      break;
    case CTRL('J'):
    case CTRL('M'):
      com_val = '+';
      break;
    case CTRL('P'):	/*^P = repeat  */
    case CTRL('W'):	/*^W = password*/
    case CTRL('X'):	/*^X = save    */
    case ' ':
    case '!':
    case '$':
      break;
    case '.':
      if (get_dir(CNIL, &dir_val))
	switch (dir_val)
	  {
	  case 1:    com_val = 'B';    break;
	  case 2:    com_val = 'J';    break;
	  case 3:    com_val = 'N';    break;
	  case 4:    com_val = 'H';    break;
	  case 6:    com_val = 'L';    break;
	  case 7:    com_val = 'Y';    break;
	  case 8:    com_val = 'K';    break;
	  case 9:    com_val = 'U';    break;
	  default:   com_val = ' ';    break;
	  }
      else
	com_val = ' ';
      break;
    case '/':
    case '<':
    case '>':
    case '-':
    case '=':
    case '{':
    case '?':
    case 'A':
      break;
    case '1':
      com_val = 'b';
      break;
    case '2':
      com_val = 'j';
      break;
    case '3':
      com_val = 'n';
      break;
    case '4':
      com_val = 'h';
      break;
    case '5':	/* Rest one turn */
      com_val = '.';
      break;
    case '6':
      com_val = 'l';
      break;
    case '7':
      com_val = 'y';
      break;
    case '8':
      com_val = 'k';
      break;
    case '9':
      com_val = 'u';
      break;
    case 'B':
      com_val = 'f';
      break;
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
      break;
    case 'L':
      com_val = 'W';
      break;
    case 'M':
      break;
    case 'R':
      break;
    case 'S':
      com_val = '#';
      break;
    case 'T':
      if (get_dir(CNIL, &dir_val))
	switch (dir_val)
	  {
	  case 1:    com_val = CTRL('B');    break;
	  case 2:    com_val = CTRL('J');    break;
	  case 3:    com_val = CTRL('N');    break;
	  case 4:    com_val = CTRL('H');    break;
	  case 6:    com_val = CTRL('L');    break;
	  case 7:    com_val = CTRL('Y');    break;
	  case 8:    com_val = CTRL('K');    break;
	  case 9:    com_val = CTRL('U');    break;
	  default:   com_val = ' ';	     break;
	  }
      else
	com_val = ' ';
      break;
    case 'V':
      break;
    case 'a':
      com_val = 'z';
      break;
    case 'b':
      com_val = 'P';
      break;
    case 'c':
    case 'd':
    case 'e':
      break;
    case 'f':
      com_val = 't';
      break;
    case 'h':
      com_val = '?';
      break;
    case 'i':
      break;
    case 'j':
      com_val = 'S';
      break;
    case 'l':
      com_val = 'x';
      break;
    case 'm':
    case 'o':
    case 'p':
    case 'q':
    case 'r':
    case 's':
      break;
    case 't':
      com_val = 'T';
      break;
    case 'u':
      com_val = 'Z';
      break;
    case 'v':
    case 'w':
      break;
    case 'x':
      com_val = 'X';
      break;

      /* wizard mode commands follow */
    case CTRL('A'): /*^A = cure all */
      break;
    case CTRL('B'):	/*^B = objects */
      com_val = CTRL('O');
      break;
    case CTRL('D'):	/*^D = up/down */
      break;
    case CTRL('H'):	/*^H = wizhelp */
      com_val = '\\';
      break;
    case CTRL('I'):	/*^I = identify*/
      break;
    case CTRL('L'):	/*^L = wizlight*/
      com_val = '*';
      break;
    case ':':
    case CTRL('T'):	/*^T = teleport*/
    case CTRL('E'):	/*^E = wizchar */
    case CTRL('F'):	/*^F = genocide*/
    case CTRL('G'):	/*^G = treasure*/
    case '@':
    case '+':
      break;
    case CTRL('U'):	/*^U = summon  */
      com_val = '&';
      break;
    default:
      com_val = '~';  /* Anything illegal. */
      break;
    }
  return com_val;
}


static void do_command(com_val)
char com_val;
{
  int dir_val, do_pickup;
  int y, x, i, j;
  vtype out_val, tmp_str;
  register struct flags *f_ptr;

  /* hack for move without pickup.  Map '-' to a movement command. */
  if (com_val == '-')
    {
      do_pickup = FALSE;
      i = command_count;
      if (get_dir(CNIL, &dir_val))
	{
	  command_count = i;
	  switch (dir_val)
	    {
	    case 1:    com_val = 'b';	 break;
	    case 2:    com_val = 'j';	 break;
	    case 3:    com_val = 'n';	 break;
	    case 4:    com_val = 'h';	 break;
	    case 6:    com_val = 'l';	 break;
	    case 7:    com_val = 'y';	 break;
	    case 8:    com_val = 'k';	 break;
	    case 9:    com_val = 'u';	 break;
	    default:   com_val = '~';	 break;
	    }
	}
      else
	com_val = ' ';
    }
  else
    do_pickup = TRUE;

  switch(com_val)
    {
    case 'Q':	/* (Q)uit		(^K)ill */
      flush();
      if (get_check("Do you really want to quit?"))
	{
	  new_level_flag = TRUE;
	  death = TRUE;
	  (void) strcpy(died_from, "Quitting");
	}
      free_turn_flag = TRUE;
      break;
    case CTRL('P'):	/* (^P)revious message. */
      if (command_count > 0)
	{
	  i = command_count;
	  if (i > MAX_SAVE_MSG)
	    i = MAX_SAVE_MSG;
	  command_count = 0;
	}
      else if (last_command != CTRL('P'))
	i = 1;
      else
	i = MAX_SAVE_MSG;
      j = last_msg;
      if (i > 1)
	{
	  save_screen();
	  x = i;
	  while (i > 0)
	    {
	      i--;
	      prt(old_msg[j], i, 0);
	      if (j == 0)
		j = MAX_SAVE_MSG-1;
	      else
		j--;
	    }
	  erase_line (x, 0);
	  pause_line(x);
	  restore_screen();
	}
      else
	{
	  /* Distinguish real and recovered messages with a '>'. -CJS- */
	  put_buffer(">", 0, 0);
	  prt(old_msg[j], 0, 1);
	}
      free_turn_flag = TRUE;
      break;
    case CTRL('W'):	/* (^W)izard mode */
      if (wizard)
	{
	  wizard = FALSE;
	  msg_print("Wizard mode off.");
	}
      else if (enter_wiz_mode())
	msg_print("Wizard mode on.");
      prt_winner();
      free_turn_flag = TRUE;
      break;
    case CTRL('X'):	/* e(^X)it and save */
      if (total_winner)
	{
       msg_print("You are a Total Winner,  your character must be retired.");
	  if (rogue_like_commands)
	    msg_print("Use 'Q' to when you are ready to quit.");
	  else
	    msg_print ("Use <Control>-K when you are ready to quit.");
	}
      else
	{
	  (void) strcpy (died_from, "(saved)");
	  msg_print ("Saving game...");
#ifdef MAC
	  if (save_char (TRUE))
	    exit_game();
#else
	  if (save_char ())
	    exit_game();
#endif
	  (void) strcpy (died_from, "(alive and well)");
	}
      free_turn_flag = TRUE;
      break;
    case '=':		/* (=) set options */
      save_screen();
      set_options();
      restore_screen();
      free_turn_flag = TRUE;
      break;
    case '{':		/* ({) inscribe an object    */
      scribe_object ();
      free_turn_flag = TRUE;
      break;
    case '!':		/* (!) escape to the shell */
    case '$':
#ifdef SECURE
      msg_print("Sorry, inferior shells are not allowed from Moria.");
#else
      shell_out();
#endif
      free_turn_flag = TRUE;
      break;
    case ESCAPE:	/* (ESC)   do nothing. */
    case ' ':		/* (space) do nothing. */
      free_turn_flag = TRUE;
      break;
    case 'b':		/* (b) down, left	(1) */
      move_char(1, do_pickup);
      break;
    case 'j':		/* (j) down		(2) */
      move_char(2, do_pickup);
      break;
    case 'n':		/* (n) down, right	(3) */
      move_char(3, do_pickup);
      break;
    case 'h':		/* (h) left		(4) */
      move_char(4, do_pickup);
      break;
    case 'l':		/* (l) right		(6) */
      move_char(6, do_pickup);
      break;
    case 'y':		/* (y) up, left		(7) */
      move_char(7, do_pickup);
      break;
    case 'k':		/* (k) up		(8) */
      move_char(8, do_pickup);
      break;
    case 'u':		/* (u) up, right	(9) */
      move_char(9, do_pickup);
      break;
    case 'B':		/* (B) run down, left	(. 1) */
      find_init(1);
      break;
    case 'J':		/* (J) run down		(. 2) */
      find_init(2);
      break;
    case 'N':		/* (N) run down, right	(. 3) */
      find_init(3);
      break;
    case 'H':		/* (H) run left		(. 4) */
      find_init(4);
      break;
    case 'L':		/* (L) run right	(. 6) */
      find_init(6);
      break;
    case 'Y':		/* (Y) run up, left	(. 7) */
      find_init(7);
      break;
    case 'K':		/* (K) run up		(. 8) */
      find_init(8);
      break;
    case 'U':		/* (U) run up, right	(. 9) */
      find_init(9);
      break;
    case '/':		/* (/) identify a symbol */
      ident_char();
      free_turn_flag = TRUE;
      break;
    case '.':		/* (.) stay in one place (5) */
      move_char (5, do_pickup);
      if (command_count > 1)
	{
	  command_count--;
	  rest();
	}
      break;
    case '<':		/* (<) go down a staircase */
      go_up();
      break;
    case '>':		/* (>) go up a staircase */
      go_down();
      break;
    case '?':		/* (?) help with commands */
      if (rogue_like_commands)
	helpfile(MORIA_HELP);
      else
	helpfile(MORIA_ORIG_HELP);
      free_turn_flag = TRUE;
      break;
    case 'f':		/* (f)orce		(B)ash */
      bash();
      break;
    case 'C':		/* (C)haracter description */
      save_screen();
      change_name();
      restore_screen();
      free_turn_flag = TRUE;
      break;
    case 'D':		/* (D)isarm trap */
      disarm_trap();
      break;
    case 'E':		/* (E)at food */
      eat();
      break;
    case 'F':		/* (F)ill lamp */
      refill_lamp();
      break;
    case 'G':		/* (G)ain magic spells */
      gain_spells();
      break;
    case 'V':		/* (V)iew scores */
      if (last_command != 'V')
	i = TRUE;
      else
	i = FALSE;
      save_screen();
      display_scores(i);
      restore_screen();
      free_turn_flag = TRUE;
      break;
    case 'W':		/* (W)here are we on the map	(L)ocate on map */
      if ((py.flags.blind > 0) || no_light())
	msg_print("You can't see your map.");
      else
	{
	  int cy, cx, p_y, p_x;

	  y = char_row;
	  x = char_col;
	  if (get_panel(y, x, TRUE))
	    prt_map();
	  cy = panel_row;
	  cx = panel_col;
	  for(;;)
	    {
	      p_y = panel_row;
	      p_x = panel_col;
	      if (p_y == cy && p_x == cx)
		tmp_str[0] = '\0';
	      else
		(void) sprintf(tmp_str, "%s%s of",
			       p_y < cy ? " North" : p_y > cy ? " South" : "",
			       p_x < cx ? " West" : p_x > cx ? " East" : "");
	      (void) sprintf(out_val,
	   "Map sector [%d,%d], which is%s your sector. Look which direction?",
			     p_y, p_x, tmp_str);
	      if (!get_dir(out_val, &dir_val))
		break;
/*								      -CJS-
// Should really use the move function, but what the hell. This
// is nicer, as it moves exactly to the same place in another
// section. The direction calculation is not intuitive. Sorry.
*/
	      for(;;){
		x += ((dir_val-1)%3 - 1) * SCREEN_WIDTH/2;
		y -= ((dir_val-1)/3 - 1) * SCREEN_HEIGHT/2;
		if (x < 0 || y < 0 || x >= cur_width || y >= cur_width)
		  {
		    msg_print("You've gone past the end of your map.");
		    x -= ((dir_val-1)%3 - 1) * SCREEN_WIDTH/2;
		    y += ((dir_val-1)/3 - 1) * SCREEN_HEIGHT/2;
		    break;
		  }
		if (get_panel(y, x, TRUE))
		  {
		    prt_map();
		    break;
		  }
	      }
	    }
	  /* Move to a new panel - but only if really necessary. */
	  if (get_panel(char_row, char_col, FALSE))
	    prt_map();
	}
      free_turn_flag = TRUE;
      break;
    case 'R':		/* (R)est a while */
      rest();
      break;
    case '#':		/* (#) search toggle	(S)earch toggle */
      if (py.flags.status & PY_SEARCH)
	search_off();
      else
	search_on();
      free_turn_flag = TRUE;
      break;
    case CTRL('B'):		/* (^B) tunnel down left	(T 1) */
      tunnel(1);
      break;
    case CTRL('M'):		/* cr must be treated same as lf. */
    case CTRL('J'):		/* (^J) tunnel down		(T 2) */
      tunnel(2);
      break;
    case CTRL('N'):		/* (^N) tunnel down right	(T 3) */
      tunnel(3);
      break;
    case CTRL('H'):		/* (^H) tunnel left		(T 4) */
      tunnel(4);
      break;
    case CTRL('L'):		/* (^L) tunnel right		(T 6) */
      tunnel(6);
      break;
    case CTRL('Y'):		/* (^Y) tunnel up left		(T 7) */
      tunnel(7);
      break;
    case CTRL('K'):		/* (^K) tunnel up		(T 8) */
      tunnel(8);
      break;
    case CTRL('U'):		/* (^U) tunnel up right		(T 9) */
      tunnel(9);
      break;
    case 'z':		/* (z)ap a wand		(a)im a wand */
      aim();
      break;
    case 'M':
      screen_map();
      free_turn_flag = TRUE;
      break;
    case 'P':		/* (P)eruse a book	(B)rowse in a book */
      examine_book();
      free_turn_flag = TRUE;
      break;
    case 'c':		/* (c)lose an object */
      closeobject();
      break;
    case 'd':		/* (d)rop something */
      inven_command('d');
      break;
    case 'e':		/* (e)quipment list */
      inven_command('e');
      break;
    case 't':		/* (t)hrow something	(f)ire something */
      throw_object();
      break;
    case 'i':		/* (i)nventory list */
      inven_command('i');
      break;
    case 'S':		/* (S)pike a door	(j)am a door */
      jamdoor();
      break;
    case 'x':		/* e(x)amine surrounds	(l)ook about */
      look();
      free_turn_flag = TRUE;
      break;
    case 'm':		/* (m)agic spells */
      cast();
      break;
    case 'o':		/* (o)pen something */
      openobject();
      break;
    case 'p':		/* (p)ray */
      pray();
      break;
    case 'q':		/* (q)uaff */
      quaff();
      break;
    case 'r':		/* (r)ead */
      read_scroll();
      break;
    case 's':		/* (s)earch for a turn */
      search(char_row, char_col, py.misc.srh);
      break;
    case 'T':		/* (T)ake off something	(t)ake off */
      inven_command('t');
      break;
    case 'Z':		/* (Z)ap a staff	(u)se a staff */
      use();
      break;
    case 'v':		/* (v)ersion of game */
      helpfile(MORIA_VER);
      free_turn_flag = TRUE;
      break;
    case 'w':		/* (w)ear or wield */
      inven_command('w');
      break;
    case 'X':		/* e(X)change weapons	e(x)change */
      inven_command('x');
      break;
    default:
      if (wizard)
	{
	  free_turn_flag = TRUE; /* Wizard commands are free moves*/
	  switch(com_val)
	    {
	    case CTRL('A'):	/*^A = Cure all*/
	      (void) remove_curse();
	      (void) cure_blindness();
	      (void) cure_confusion();
	      (void) cure_poison();
	      (void) remove_fear();
	      (void) res_stat(A_STR);
	      (void) res_stat(A_INT);
	      (void) res_stat(A_WIS);
	      (void) res_stat(A_CON);
	      (void) res_stat(A_DEX);
	      (void) res_stat(A_CHR);
	      f_ptr = &py.flags;
	      if (f_ptr->slow > 1)
		f_ptr->slow = 1;
	      if (f_ptr->image > 1)
		f_ptr->image = 1;
	      break;
	    case CTRL('E'):	/*^E = wizchar */
	      change_character();
	      erase_line(MSG_LINE, 0);
	      break;
	    case CTRL('F'):	/*^F = genocide*/
	      (void) mass_genocide();
	      break;
	    case CTRL('G'):	/*^G = treasure*/
	      if (command_count > 0)
		{
		  i = command_count;
		  command_count = 0;
		}
	      else
		i = 1;
	      random_object(char_row, char_col, i);
	      prt_map();
	      break;
	    case CTRL('D'):	/*^D = up/down */
	      if (command_count > 0)
		{
		  if (command_count > 99)
		    i = 0;
		  else
		    i = command_count;
		  command_count = 0;
		}
	      else
		{
		  prt("Go to which level (0-99) ? ", 0, 0);
		  i = -1;
		  if (get_string(tmp_str, 0, 27, 10))
		    i = atoi(tmp_str);
		}
	      if (i > -1)
		{
		  dun_level = i;
		  if (dun_level > 99)
		    dun_level = 99;
		  new_level_flag = TRUE;
		}
	      else
		erase_line(MSG_LINE, 0);
	      break;
	    case CTRL('O'):	/*^O = objects */
	      print_objects();
	      break;
	    case '\\': /* \ wizard help */
	      if (rogue_like_commands)
		helpfile(MORIA_WIZ_HELP);
	      else
		helpfile(MORIA_OWIZ_HELP);
	      break;
	    case CTRL('I'):	/*^I = identify*/
	      (void) ident_spell();
	      break;
	    case '*':
	      wizard_light();
	      break;
	    case ':':
	      map_area();
	      break;
	    case CTRL('T'):	/*^T = teleport*/
	      teleport(100);
	      break;
	    case '+':
	      if (command_count > 0)
		{
		  py.misc.exp = command_count;
		  command_count = 0;
		}
	      else if (py.misc.exp == 0)
		py.misc.exp = 1;
	      else
		py.misc.exp = py.misc.exp * 2;
	      prt_experience();
	      break;
	    case '&':	/*& = summon  */
	      y = char_row;
	      x = char_col;
	      (void) summon_monster(&y, &x, TRUE);
	      creatures(FALSE);
	      break;
	    case '@':
	      wizard_create();
	      break;
	    default:
	      if (rogue_like_commands)
		prt("Type '?' or '\\' for help.", 0, 0);
	      else
		prt("Type '?' or ^H for help.", 0, 0);
	    }
	}
      else
	{
	  prt("Type '?' for help.", 0, 0);
	  free_turn_flag = TRUE;
	}
    }
  last_command = com_val;
}

/* Check whether this command will accept a count.     -CJS-  */
static int valid_countcommand(c)
char c;
{
  switch(c)
    {
    case 'Q':
    case CTRL('W'):
    case CTRL('X'):
    case '=':
    case '{':
    case '/':
    case '<':
    case '>':
    case '?':
    case 'C':
    case 'E':
    case 'F':
    case 'G':
    case 'V':
    case '#':
    case 'z':
    case 'P':
    case 'c':
    case 'd':
    case 'e':
    case 't':
    case 'i':
    case 'x':
    case 'm':
    case 'p':
    case 'q':
    case 'r':
    case 'T':
    case 'Z':
    case 'v':
    case 'w':
    case 'W':
    case 'X':
    case CTRL('A'):
    case '\\':
    case CTRL('I'):
    case '*':
    case ':':
    case CTRL('T'):
    case CTRL('E'):
    case CTRL('F'):
    case CTRL('S'):
    case CTRL('Q'):
      return FALSE;
    case CTRL('P'):
    case ESCAPE:
    case ' ':
    case '-':
    case 'b':
    case 'f':
    case 'j':
    case 'n':
    case 'h':
    case 'l':
    case 'y':
    case 'k':
    case 'u':
    case '.':
    case 'B':
    case 'J':
    case 'N':
    case 'H':
    case 'L':
    case 'Y':
    case 'K':
    case 'U':
    case 'D':
    case 'R':
    case CTRL('Y'):
    case CTRL('K'):
    case CTRL('U'):
    case CTRL('L'):
    case CTRL('N'):
    case CTRL('J'):
    case CTRL('B'):
    case CTRL('H'):
    case 'S':
    case 'o':
    case 's':
    case CTRL('D'):
    case CTRL('G'):
    case '+':
      return TRUE;
    default:
      return FALSE;
    }
}


/* Regenerate hit points				-RAK-	*/
static void regenhp(percent)
int percent;
{
  register struct misc *p_ptr;
  register int32 new_chp, new_chp_frac;
  int old_chp;

  p_ptr = &py.misc;
  old_chp = p_ptr->chp;
  new_chp = ((long)p_ptr->mhp) * percent + PLAYER_REGEN_HPBASE;
  p_ptr->chp += new_chp >> 16;	/* div 65536 */
  /* check for overflow */
  if (p_ptr->chp < 0 && old_chp > 0)
    p_ptr->chp = MAX_SHORT;
  new_chp_frac = (new_chp & 0xFFFF) + p_ptr->chp_frac; /* mod 65536 */
  if (new_chp_frac >= 0x10000L)
    {
      p_ptr->chp_frac = new_chp_frac - 0x10000L;
      p_ptr->chp++;
    }
  else
    p_ptr->chp_frac = new_chp_frac;

  /* must set frac to zero even if equal */
  if (p_ptr->chp >= p_ptr->mhp)
    {
      p_ptr->chp = p_ptr->mhp;
      p_ptr->chp_frac = 0;
    }
  if (old_chp != p_ptr->chp)
    prt_chp();
}


/* Regenerate mana points				-RAK-	*/
static void regenmana(percent)
int percent;
{
  register struct misc *p_ptr;
  register int32 new_mana, new_mana_frac;
  int old_cmana;

  p_ptr = &py.misc;
  old_cmana = p_ptr->cmana;
  new_mana = ((long)p_ptr->mana) * percent + PLAYER_REGEN_MNBASE;
  p_ptr->cmana += new_mana >> 16;  /* div 65536 */
  /* check for overflow */
  if (p_ptr->cmana < 0 && old_cmana > 0)
    p_ptr->cmana = MAX_SHORT;
  new_mana_frac = (new_mana & 0xFFFF) + p_ptr->cmana_frac; /* mod 65536 */
  if (new_mana_frac >= 0x10000L)
    {
      p_ptr->cmana_frac = new_mana_frac - 0x10000L;
      p_ptr->cmana++;
    }
  else
    p_ptr->cmana_frac = new_mana_frac;

  /* must set frac to zero even if equal */
  if (p_ptr->cmana >= p_ptr->mana)
    {
      p_ptr->cmana = p_ptr->mana;
      p_ptr->cmana_frac = 0;
    }
  if (old_cmana != p_ptr->cmana)
    prt_cmana();
}


/* Is an item an enchanted weapon or armor and we don't know?  -CJS- */
/* only returns true if it is a good enchantment */
static int enchanted (t_ptr)
register inven_type *t_ptr;
{
#ifdef ATARIST_MWC
  int32u holder;
#endif

  if (t_ptr->tval < TV_MIN_ENCHANT || t_ptr->tval > TV_MAX_ENCHANT
#ifdef ATARIST_MWC
      || t_ptr->flags & (holder = TR_CURSED))
#else
      || t_ptr->flags & TR_CURSED)
#endif
    return FALSE;
  if (known2_p(t_ptr))
    return FALSE;
  if (t_ptr->ident & ID_MAGIK)
    return FALSE;
  if (t_ptr->tohit > 0 || t_ptr->todam > 0 || t_ptr->toac > 0)
    return TRUE;
  if ((0x4000107fL & t_ptr->flags) && t_ptr->p1 > 0)
    return TRUE;
  if (0x07ffe980L & t_ptr->flags)
    return TRUE;

  return FALSE;
}


/* Examine a Book					-RAK-	*/
static void examine_book()
{
  int32u j;
  int i, k, item_val, flag;
  int spell_index[31];
  register inven_type *i_ptr;
  register spell_type *s_ptr;

  if (!find_range(TV_MAGIC_BOOK, TV_PRAYER_BOOK, &i, &k))
    msg_print("You are not carrying any books.");
  else if (py.flags.blind > 0)
    msg_print("You can't see to read your spell book!");
  else if (no_light())
    msg_print("You have no light to read by.");
  else if (py.flags.confused > 0)
    msg_print("You are too confused.");
  else if (get_item(&item_val, "Which Book?", i, k, CNIL, CNIL))
    {
      flag = TRUE;
      i_ptr = &inventory[item_val];
      if (class[py.misc.pclass].spell == MAGE)
	{
	  if (i_ptr->tval != TV_MAGIC_BOOK)
	    flag = FALSE;
	}
      else if (class[py.misc.pclass].spell == PRIEST)
	{
	  if (i_ptr->tval != TV_PRAYER_BOOK)
	    flag = FALSE;
	}
      else
	flag = FALSE;

      if (!flag)
	msg_print("You do not understand the language.");
      else
	{
	  i = 0;
	  j = inventory[item_val].flags;
	  while (j)
	    {
	      k = bit_pos(&j);
	      s_ptr = &magic_spell[py.misc.pclass-1][k];
	      if (s_ptr->slevel < 99)
		{
		  spell_index[i] = k;
		  i++;
		}
	    }
	  save_screen();
	  print_spells(spell_index, i, TRUE, -1);
	  pause_line(0);
	  restore_screen();
	}
    }
}


/* Go up one level					-RAK-	*/
static void go_up()
{
  register cave_type *c_ptr;
  register int no_stairs = FALSE;

  c_ptr = &cave[char_row][char_col];
  if (c_ptr->tptr != 0)
    if (t_list[c_ptr->tptr].tval == TV_UP_STAIR)
      {
	dun_level--;
	new_level_flag = TRUE;
	msg_print("You enter a maze of up staircases.");
	msg_print("You pass through a one-way door.");
      }
    else
      no_stairs = TRUE;
  else
    no_stairs = TRUE;

  if (no_stairs)
    {
      msg_print("I see no up staircase here.");
      free_turn_flag = TRUE;
    }
}


/* Go down one level					-RAK-	*/
static void go_down()
{
  register cave_type *c_ptr;
  register int no_stairs = FALSE;

  c_ptr = &cave[char_row][char_col];
  if (c_ptr->tptr != 0)
    if (t_list[c_ptr->tptr].tval == TV_DOWN_STAIR)
      {
	dun_level++;
	new_level_flag = TRUE;
	msg_print("You enter a maze of down staircases.");
	msg_print("You pass through a one-way door.");
      }
    else
      no_stairs = TRUE;
  else
    no_stairs = TRUE;

  if (no_stairs)
    {
      msg_print("I see no down staircase here.");
      free_turn_flag = TRUE;
    }
}


/* Jam a closed door					-RAK-	*/
static void jamdoor()
{
  int y, x, dir, i, j;
  register cave_type *c_ptr;
  register inven_type *t_ptr, *i_ptr;
  char tmp_str[80];

  free_turn_flag = TRUE;
  y = char_row;
  x = char_col;
  if (get_dir(CNIL, &dir))
    {
      (void) mmove(dir, &y, &x);
      c_ptr = &cave[y][x];
      if (c_ptr->tptr != 0)
	{
	  t_ptr = &t_list[c_ptr->tptr];
	  if (t_ptr->tval == TV_CLOSED_DOOR)
	    if (c_ptr->cptr == 0)
	      {
		if (find_range(TV_SPIKE, TV_NEVER, &i, &j))
		  {
		    free_turn_flag = FALSE;
		    count_msg_print("You jam the door with a spike.");
		    if (t_ptr->p1 > 0)
		      t_ptr->p1 = -t_ptr->p1;	/* Make locked to stuck. */
		    /* Successive spikes have a progressively smaller effect.
		       Series is: 0 20 30 37 43 48 52 56 60 64 67 70 ... */
		    t_ptr->p1 -= 1 + 190 / (10 - t_ptr->p1);
		    i_ptr = &inventory[i];
		    if (i_ptr->number > 1)
		      {
			i_ptr->number--;
			inven_weight -= i_ptr->weight;
		      }
		    else
		      inven_destroy(i);
		  }
		else
		  msg_print("But you have no spikes.");
	      }
	    else
	      {
		free_turn_flag = FALSE;
		(void) sprintf(tmp_str, "The %s is in your way!",
			       c_list[m_list[c_ptr->cptr].mptr].name);
		msg_print(tmp_str);
	      }
	  else if (t_ptr->tval == TV_OPEN_DOOR)
	    msg_print("The door must be closed first.");
	  else
	    msg_print("That isn't a door!");
	}
      else
	msg_print("That isn't a door!");
    }
}


/* Refill the players lamp				-RAK-	*/
static void refill_lamp()
{
  int i, j;
  register int k;
  register inven_type *i_ptr;

  free_turn_flag = TRUE;
  k = inventory[INVEN_LIGHT].subval;
  if (k != 0)
    msg_print("But you are not using a lamp.");
  else if (!find_range(TV_FLASK, TV_NEVER, &i, &j))
    msg_print("You have no oil.");
  else
    {
      free_turn_flag = FALSE;
      i_ptr = &inventory[INVEN_LIGHT];
      i_ptr->p1 += inventory[i].p1;
      if (i_ptr->p1 > OBJ_LAMP_MAX)
	{
	  i_ptr->p1 = OBJ_LAMP_MAX;
	  msg_print ("Your lamp overflows, spilling oil on the ground.");
	  msg_print("Your lamp is full.");
	}
      else if (i_ptr->p1 > OBJ_LAMP_MAX/2)
	msg_print ("Your lamp is more than half full.");
      else if (i_ptr->p1 == OBJ_LAMP_MAX/2)
	msg_print ("Your lamp is half full.");
      else
	msg_print ("Your lamp is less than half full.");
      desc_remain(i);
      inven_destroy(i);
    }
}
