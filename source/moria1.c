/* source/moria1.c: misc code, mainly handles player movement, inventory, etc

   Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#ifdef __TURBOC__
#include	<stdlib.h>
#endif

#include <stdio.h>
#include <ctype.h>

#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"

#ifdef USG
#ifndef ATARIST_MWC
#include <string.h>
#else
char *strcat();
int strlen();
#endif
#else
#include <strings.h>
#endif

#if defined(LINT_ARGS)
static void inven_screen(int);
static char map_roguedir(char);
static void sub1_move_light(int, int, int, int);
static void sub3_move_light(int, int, int, int);
#endif

#ifdef ATARIST_TC
/* Include this to get prototypes for standard library functions.  */
#include <stdlib.h>
#endif

/* Changes speed of monsters relative to player		-RAK-	*/
/* Note: When the player is sped up or slowed down, I simply	 */
/*	 change the speed of all the monsters.	This greatly	 */
/*	 simplified the logic.				       */
void change_speed(num)
register int num;
{
  register int i;
#ifdef ATARIST_MWC
  int32u holder;
#endif

  py.flags.speed += num;
#ifdef ATARIST_MWC
  py.flags.status |= (holder = PY_SPEED);
#else
  py.flags.status |= PY_SPEED;
#endif
  for (i = mfptr - 1; i >= MIN_MONIX; i--)
      m_list[i].cspeed += num;
}


/* Player bonuses					-RAK-	*/
/* When an item is worn or taken off, this re-adjusts the player */
/* bonuses.  Factor=1 : wear; Factor=-1 : removed		 */
/* Only calculates properties with cumulative effect.  Properties that
   depend on everything being worn are recalculated by calc_bonuses() -CJS- */
void py_bonuses(t_ptr, factor)
register inven_type *t_ptr;
register int factor;
{
  register int i, amount;
#ifdef ATARIST_MWC
  int32u holder;
#endif

  amount = t_ptr->p1 * factor;
  if (t_ptr->flags & TR_STATS)
    {
      for(i = 0; i < 6; i++)
	if ((1 << i) & t_ptr->flags)
	  bst_stat(i, amount);
    }
  if (TR_SEARCH & t_ptr->flags)
    {
      py.misc.srh += amount;
      py.misc.fos -= amount;
    }
  if (TR_STEALTH & t_ptr->flags)
    py.misc.stl += amount;
  if (TR_SPEED & t_ptr->flags)
    change_speed(-amount);
#ifdef ATARIST_MWC
  if (((holder = TR_BLIND) & t_ptr->flags) && (factor > 0))
    py.flags.blind += 1000;
  if (((holder = TR_TIMID) & t_ptr->flags) && (factor > 0))
    py.flags.afraid += 50;
  if ((holder = TR_INFRA) & t_ptr->flags)
    py.flags.see_infra += amount;
#else
  if ((TR_BLIND & t_ptr->flags) && (factor > 0))
    py.flags.blind += 1000;
  if ((TR_TIMID & t_ptr->flags) && (factor > 0))
    py.flags.afraid += 50;
  if (TR_INFRA & t_ptr->flags)
    py.flags.see_infra += amount;
#endif
}

/* Recalculate the effect of all the stuff we use.		  -CJS- */
void calc_bonuses()
{
  register int32u item_flags;
#if defined(ATARIST_MWC)
  int32u holder;		/* to avoid a compiler bug */
#endif
  int old_dis_ac;
  register struct flags *p_ptr;
  register struct misc *m_ptr;
  register inven_type *i_ptr;
  register int i;

  p_ptr = &py.flags;
  m_ptr = &py.misc;
  if (p_ptr->slow_digest)
    p_ptr->food_digested++;
  if (p_ptr->regenerate)
    p_ptr->food_digested -= 3;
  p_ptr->see_inv     = FALSE;
  p_ptr->teleport    = FALSE;
  p_ptr->free_act    = FALSE;
  p_ptr->slow_digest = FALSE;
  p_ptr->aggravate   = FALSE;
  p_ptr->sustain_str = FALSE;
  p_ptr->sustain_int = FALSE;
  p_ptr->sustain_wis = FALSE;
  p_ptr->sustain_con = FALSE;
  p_ptr->sustain_dex = FALSE;
  p_ptr->sustain_chr = FALSE;
  p_ptr->fire_resist = FALSE;
  p_ptr->acid_resist = FALSE;
  p_ptr->cold_resist = FALSE;
  p_ptr->regenerate  = FALSE;
  p_ptr->lght_resist = FALSE;
  p_ptr->ffall	     = FALSE;

  old_dis_ac = m_ptr->dis_ac;
  m_ptr->ptohit	 = tohit_adj();	      /* Real To Hit   */
  m_ptr->ptodam	 = todam_adj();	      /* Real To Dam   */
  m_ptr->ptoac	 = toac_adj();	      /* Real To AC    */
  m_ptr->pac	 = 0;		    /* Real AC	     */
  m_ptr->dis_th	 = m_ptr->ptohit;  /* Display To Hit	    */
  m_ptr->dis_td	 = m_ptr->ptodam;  /* Display To Dam	    */
  m_ptr->dis_ac	 = 0;		/* Display AC		 */
  m_ptr->dis_tac = m_ptr->ptoac;   /* Display To AC	    */
  for (i = INVEN_WIELD; i < INVEN_LIGHT; i++)
    {
      i_ptr = &inventory[i];
      if (i_ptr->tval != TV_NOTHING)
	{
	  m_ptr->ptohit += i_ptr->tohit;
	  if (i_ptr->tval != TV_BOW)		/* Bows can't damage. -CJS- */
	    m_ptr->ptodam += i_ptr->todam;
	  m_ptr->ptoac	+= i_ptr->toac;
	  m_ptr->pac += i_ptr->ac;
	  if (known2_p(i_ptr))
	    {
	      m_ptr->dis_th  += i_ptr->tohit;
	      if (i_ptr->tval != TV_BOW)
		m_ptr->dis_td  += i_ptr->todam;	/* Bows can't damage. -CJS- */
	      m_ptr->dis_tac += i_ptr->toac;
	      m_ptr->dis_ac += i_ptr->ac;
	    }
	  else if (! (TR_CURSED & i_ptr->flags))
	    /* Base AC values should always be visible, as long as the item
	       is not cursed.  */
	    m_ptr->dis_ac += i_ptr->ac;
	}
    }
  m_ptr->dis_ac += m_ptr->dis_tac;

  if (weapon_heavy)
    m_ptr->dis_th += (py.stats.use_stat[A_STR] * 15 -
		      inventory[INVEN_WIELD].weight);

  /* Add in temporary spell increases	*/
  if (p_ptr->invuln > 0)
    {
      m_ptr->pac += 100;
      m_ptr->dis_ac += 100;
    }
  if (p_ptr->blessed > 0)
    {
      m_ptr->pac    += 2;
      m_ptr->dis_ac += 2;
    }
  if (p_ptr->detect_inv > 0)
    p_ptr->see_inv = TRUE;

  /* can't print AC here because might be in a store */
  if (old_dis_ac != m_ptr->dis_ac)
#ifdef ATARIST_MWC
    p_ptr->status |= (holder = PY_ARMOR);
#else
    p_ptr->status |= PY_ARMOR;
#endif

  item_flags = 0;
  i_ptr = &inventory[INVEN_WIELD];
  for (i = INVEN_WIELD; i < INVEN_LIGHT; i++)
    {
      item_flags |= i_ptr->flags;
      i_ptr++;
    }
#if !defined(ATARIST_MWC)
  if (TR_SLOW_DIGEST & item_flags)
    p_ptr->slow_digest = TRUE;
  if (TR_AGGRAVATE & item_flags)
    p_ptr->aggravate = TRUE;
  if (TR_TELEPORT & item_flags)
    p_ptr->teleport = TRUE;
  if (TR_REGEN & item_flags)
    p_ptr->regenerate = TRUE;
  if (TR_RES_FIRE & item_flags)
    p_ptr->fire_resist = TRUE;
  if (TR_RES_ACID & item_flags)
    p_ptr->acid_resist = TRUE;
  if (TR_RES_COLD & item_flags)
    p_ptr->cold_resist = TRUE;
  if (TR_FREE_ACT & item_flags)
    p_ptr->free_act = TRUE;
  if (TR_SEE_INVIS & item_flags)
    p_ptr->see_inv = TRUE;
  if (TR_RES_LIGHT & item_flags)
    p_ptr->lght_resist = TRUE;
  if (TR_FFALL & item_flags)
    p_ptr->ffall = TRUE;
#else
  /* this avoids a bug in the Mark Williams C compiler for the Atari ST */
  holder = TR_SLOW_DIGEST;
  if (holder & item_flags)
    p_ptr->slow_digest = TRUE;
  holder = TR_AGGRAVATE;
  if (holder & item_flags)
    p_ptr->aggravate = TRUE;
  holder = TR_TELEPORT;
  if (holder & item_flags)
    p_ptr->teleport = TRUE;
  holder = TR_REGEN;
  if (holder & item_flags)
    p_ptr->regenerate = TRUE;
  holder = TR_RES_FIRE;
  if (holder & item_flags)
    p_ptr->fire_resist = TRUE;
  holder = TR_RES_ACID;
  if (holder & item_flags)
    p_ptr->acid_resist = TRUE;
  holder = TR_RES_COLD;
  if (holder & item_flags)
    p_ptr->cold_resist = TRUE;
  holder = TR_FREE_ACT;
  if (holder & item_flags)
    p_ptr->free_act = TRUE;
  holder = TR_SEE_INVIS;
  if (holder & item_flags)
    p_ptr->see_inv = TRUE;
  holder = TR_RES_LIGHT;
  if (holder & item_flags)
    p_ptr->lght_resist = TRUE;
  holder = TR_FFALL;
  if (holder & item_flags)
    p_ptr->ffall = TRUE;
#endif

  i_ptr = &inventory[INVEN_WIELD];
  for (i = INVEN_WIELD; i < INVEN_LIGHT; i++)
    {
#ifdef ATARIST_MWC
      if ((holder = TR_SUST_STAT) & i_ptr->flags)
#else
      if (TR_SUST_STAT & i_ptr->flags)
#endif
	switch(i_ptr->p1)
	  {
	  case 1: p_ptr->sustain_str = TRUE; break;
	  case 2: p_ptr->sustain_int = TRUE; break;
	  case 3: p_ptr->sustain_wis = TRUE; break;
	  case 4: p_ptr->sustain_con = TRUE; break;
	  case 5: p_ptr->sustain_dex = TRUE; break;
	  case 6: p_ptr->sustain_chr = TRUE; break;
	  default: break;
	  }
      i_ptr++;
    }

  if (p_ptr->slow_digest)
    p_ptr->food_digested--;
  if (p_ptr->regenerate)
    p_ptr->food_digested += 3;
}


/* Displays inventory items from r1 to r2	-RAK-	*/
/* Designed to keep the display as far to the right as possible.  The  -CJS-
   parameter col gives a column at which to start, but if the display does
   not fit, it may be moved left.  The return value is the left edge used. */
/* If mask is non-zero, then only display those items which have a non-zero
   entry in the mask array.  */
int show_inven(r1, r2, weight, col, mask)
register int r1, r2;
int weight, col;
char *mask;
{
  register int i;
  int total_weight, len, l, lim, current_line;
  bigvtype tmp_val;
  vtype out_val[23];

  len = 79 - col;
  if (weight)
    lim = 68;
  else
    lim = 76;

  for (i = r1; i <= r2; i++)		 /* Print the items	  */
    {
      if (mask == CNIL || mask[i])
	{
	  objdes(tmp_val, &inventory[i], TRUE);
	  tmp_val[lim] = 0;	 /* Truncate if too long. */
	  (void) sprintf(out_val[i], "  %c) %s", 'a'+i, tmp_val);
	  l = strlen(out_val[i]);
	  if (weight)
	    l += 9;
	  if (l > len)
	    len = l;
	}
    }

  col = 79 - len;
  if (col < 0)
    col = 0;

  current_line = 1;
  for (i = r1; i <= r2; i++)
    {
      if (mask == CNIL || mask[i])
	{
	  /* don't need first two spaces if in first column */
	  if (col == 0)
	    prt(&out_val[i][2], current_line, col);
	  else
	    prt(out_val[i], current_line, col);
	  if (weight)
	    {
	      total_weight = inventory[i].weight*inventory[i].number;
	      (void) sprintf (tmp_val, "%3d.%d lb",
			      (total_weight) / 10, (total_weight) % 10);
	      prt (tmp_val, current_line, 71);
	    }
	  current_line++;
	}
    }
  return col;
}


/* Return a string describing how a given equipment item is carried. -CJS- */
char *describe_use(i)
register int i;
{
  register char *p;

  switch(i)
    {
    case INVEN_WIELD:
      p = "wielding"; break;
    case INVEN_HEAD:
      p = "wearing on your head"; break;
    case INVEN_NECK:
      p = "wearing around your neck"; break;
    case INVEN_BODY:
      p = "wearing on your body"; break;
    case INVEN_ARM:
      p = "wearing on your arm"; break;
    case INVEN_HANDS:
      p = "wearing on your hands"; break;
    case INVEN_RIGHT:
      p = "wearing on your right hand"; break;
    case INVEN_LEFT:
      p = "wearing on your left hand"; break;
    case INVEN_FEET:
      p = "wearing on your feet"; break;
    case INVEN_OUTER:
      p = "wearing about your body"; break;
    case INVEN_LIGHT:
      p = "using to light the way"; break;
    case INVEN_AUX:
      p = "holding ready by your side"; break;
    default:
      p = "carrying in your pack"; break;
    }
  return p;
}


/* Displays equipment items from r1 to end	-RAK-	*/
/* Keep display as far right as possible. -CJS- */
int show_equip(weight, col)
int weight, col;
{
  register int i, line;
  int total_weight, l, len, lim;
  register char *prt1;
  bigvtype prt2;
  vtype out_val[INVEN_ARRAY_SIZE-INVEN_WIELD];
  register inven_type *i_ptr;

  line = 0;
  len = 79 - col;
  if (weight)
    lim = 52;
  else
    lim = 60;
  for (i = INVEN_WIELD; i < INVEN_ARRAY_SIZE; i++) /* Range of equipment */
    {
      i_ptr = &inventory[i];
      if (i_ptr->tval != TV_NOTHING)
	{
	  switch(i)	     /* Get position	      */
	    {
	    case INVEN_WIELD:
	      if (py.stats.use_stat[A_STR]*15 < i_ptr->weight)
		prt1 = "Just lifting";
	      else
		prt1 = "Wielding";
	      break;
	    case INVEN_HEAD:
	      prt1 = "On head"; break;
	    case INVEN_NECK:
	      prt1 = "Around neck"; break;
	    case INVEN_BODY:
	      prt1 = "On body"; break;
	    case INVEN_ARM:
	      prt1 = "On arm"; break;
	    case INVEN_HANDS:
	      prt1 = "On hands"; break;
	    case INVEN_RIGHT:
	      prt1 = "On right hand"; break;
	    case INVEN_LEFT:
	      prt1 = "On left hand"; break;
	    case INVEN_FEET:
	      prt1 = "On feet"; break;
	    case INVEN_OUTER:
	      prt1 = "About body"; break;
	    case INVEN_LIGHT:
	      prt1 = "Light source"; break;
	    case INVEN_AUX:
	      prt1 = "Spare weapon"; break;
	    default:
	      prt1 = "Unknown value"; break;
	    }
	  objdes(prt2, &inventory[i], TRUE);
	  prt2[lim] = 0; /* Truncate if necessary */
	  (void) sprintf(out_val[line], "  %c) %-14s: %s", line+'a',
			 prt1, prt2);
	  l = strlen(out_val[line]);
	  if (weight)
	    l += 9;
	  if (l > len)
	    len = l;
	  line++;
	}
    }
  col = 79 - len;
  if (col < 0)
    col = 0;

  line = 0;
  for (i = INVEN_WIELD; i < INVEN_ARRAY_SIZE; i++) /* Range of equipment */
    {
      i_ptr = &inventory[i];
      if (i_ptr->tval != TV_NOTHING)
	{
	  /* don't need first two spaces when using whole screen */
	  if (col == 0)
	    prt(&out_val[line][2], line+1, col);
	  else
	    prt(out_val[line], line+1, col);
	  if (weight)
	    {
	      total_weight = i_ptr->weight*i_ptr->number;
	      (void) sprintf(prt2, "%3d.%d lb",
			     (total_weight) / 10, (total_weight) % 10);
	      prt(prt2, line+1, 71);
	    }
	  line++;
	}
    }
  erase_line(line+1, col);
  return col;
}

/* Remove item from equipment list		-RAK-	*/
void takeoff(item_val, posn)
int item_val, posn;
{
  register char *p;
  bigvtype out_val, prt2;
  register inven_type *t_ptr;
#ifdef ATARIST_MWC
  int32u holder;
#endif

  equip_ctr--;
  t_ptr = &inventory[item_val];
  inven_weight -= t_ptr->weight*t_ptr->number;
#ifdef ATARIST_MWC
  py.flags.status |= (holder = PY_STR_WGT);
#else
  py.flags.status |= PY_STR_WGT;
#endif

  if (item_val == INVEN_WIELD || item_val == INVEN_AUX)
    p = "Was wielding ";
  else if (item_val == INVEN_LIGHT)
    p = "Light source was ";
  else
    p = "Was wearing ";

  objdes(prt2, t_ptr, TRUE);
  if (posn >= 0)
    (void) sprintf(out_val, "%s%s (%c)", p, prt2, 'a'+posn);
  else
    (void) sprintf(out_val, "%s%s", p, prt2);
  msg_print(out_val);
  if (item_val != INVEN_AUX)	  /* For secondary weapon  */
    py_bonuses(t_ptr, -1);
  invcopy(t_ptr, OBJ_NOTHING);
}


/* Used to verify if this really is the item we wish to	 -CJS-
   wear or read. */
int verify(prompt, item)
char *prompt;
int item;
{
  bigvtype out_str, object;

  objdes(object, &inventory[item], TRUE);
  object[strlen(object)-1] = '?'; /* change the period to a question mark */
  (void) sprintf(out_str, "%s %s", prompt, object);
  return get_check(out_str);
}


/* All inventory commands (wear, exchange, take off, drop, inventory and
   equipment) are handled in an alternative command input mode, which accepts
   any of the inventory commands.

   It is intended that this function be called several times in succession,
   as some commands take up a turn, and the rest of moria must proceed in the
   interim. A global variable is provided, doing_inven, which is normally
   zero; however if on return from inven_command it is expected that
   inven_command should be called *again*, (being still in inventory command
   input mode), then doing_inven is set to the inventory command character
   which should be used in the next call to inven_command.

   On return, the screen is restored, but not flushed. Provided no flush of
   the screen takes place before the next call to inven_command, the inventory
   command screen is silently redisplayed, and no actual output takes place at
   all. If the screen is flushed before a subsequent call, then the player is
   prompted to see if we should continue. This allows the player to see any
   changes that take place on the screen during inventory command input.

  The global variable, screen_change, is cleared by inven_command, and set
  when the screen is flushed. This is the means by which inven_command tell
  if the screen has been flushed.

  The display of inventory items is kept to the right of the screen to
  minimize the work done to restore the screen afterwards.		-CJS-*/

/* Inventory command screen states. */
#define BLANK_SCR	0
#define EQUIP_SCR	1
#define INVEN_SCR	2
#define WEAR_SCR	3
#define HELP_SCR	4
#define WRONG_SCR	5

/* Keep track of the state of the inventory screen. */
static int scr_state, scr_left, scr_base;
static int wear_low, wear_high;

/* Draw the inventory screen. */
static void inven_screen(new_scr)
int new_scr;
{
  register int line;

  if (new_scr != scr_state)
    {
      scr_state = new_scr;
      switch(new_scr)
	{
	case BLANK_SCR:
	  line = 0;
	  break;
	case HELP_SCR:
	  if (scr_left > 52)
	    scr_left = 52;
	  prt("  ESC: exit", 1, scr_left);
	  prt("  w  : wear or wield object", 2, scr_left);
	  prt("  t  : take off item", 3, scr_left);
	  prt("  d  : drop object", 4, scr_left);
	  prt("  x  : exchange weapons", 5, scr_left);
	  prt("  i  : inventory of pack", 6, scr_left);
	  prt("  e  : list used equipment", 7, scr_left);
	  line = 7;
	  break;
	case INVEN_SCR:
	  scr_left = show_inven(0, inven_ctr - 1, show_weight_flag, scr_left,
				CNIL);
	  line = inven_ctr;
	  break;
	case WEAR_SCR:
	  scr_left = show_inven(wear_low, wear_high, show_weight_flag,
				scr_left, CNIL);
	  line = wear_high - wear_low + 1;
	  break;
	case EQUIP_SCR:
	  scr_left = show_equip(show_weight_flag, scr_left);
	  line = equip_ctr;
	  break;
	}
      if (line >= scr_base)
	{
	  scr_base = line + 1;
	  erase_line(scr_base, scr_left);
	}
      else
	{
	  while (++line <= scr_base)
	    erase_line(line, scr_left);
	}
    }
}

/* This does all the work. */
void inven_command(command)
char command;
{
  register int slot, item;
  int tmp, tmp2, selecting, from, to;
  char *prompt, *swap, *disp, *string;
  char which, query;
  bigvtype prt1, prt2;
  register inven_type *i_ptr;
  inven_type tmp_obj;
#ifdef ATARIST_MWC
  int32u holder;
#endif

  free_turn_flag = TRUE;
  save_screen();
  /* Take up where we left off after a previous inventory command. -CJS- */
  if (doing_inven)
    {
      /* If the screen has been flushed, we need to redraw. If the command is
	 a simple ' ' to recover the screen, just quit. Otherwise, check and
	 see what the user wants. */
      if (screen_change)
	{
	  if (command == ' ' || !get_check("Continuing with inventory command?"))
	    {
	      doing_inven = FALSE;
	      return;
	    }
	  scr_left = 50;
	  scr_base = 0;
	}
      tmp = scr_state;
      scr_state = WRONG_SCR;
      inven_screen(tmp);
    }
  else
    {
      scr_left = 50;
      scr_base = 0;
      /* this forces exit of inven_command() if selecting is not set true */
      scr_state = BLANK_SCR;
    }
  do
    {
      if (isupper((int)command))
	command = tolower((int)command);

      /* Simple command getting and screen selection. */
      selecting = FALSE;
      switch(command)
	{
	case 'i':	   /* Inventory	    */
	  if (inven_ctr == 0)
	    msg_print("You are not carrying anything.");
	  else
	    inven_screen(INVEN_SCR);
	  break;
	case 'e':	  /* Equipment	   */
	  if (equip_ctr == 0)
	    msg_print("You are not using any equipment.");
	  else
	    inven_screen(EQUIP_SCR);
	  break;
	case 't':	  /* Take off	   */
	  if (equip_ctr == 0)
	    msg_print("You are not using any equipment.");
	  /* don't print message restarting inven command after taking off
	     something, it is confusing */
	  else if (inven_ctr >= INVEN_WIELD && !doing_inven)
	    msg_print("You will have to drop something first.");
	  else
	    {
	      if (scr_state != BLANK_SCR)
		inven_screen(EQUIP_SCR);
	      selecting = TRUE;
	    }
	  break;
	case 'd':		/* Drop */
	  if (inven_ctr == 0 && equip_ctr == 0)
	    msg_print("But you're not carrying anything.");
	  else if (cave[char_row][char_col].tptr != 0)
	    msg_print("There's no room to drop anything here.");
	  else
	    {
	      selecting = TRUE;
	      if ((scr_state == EQUIP_SCR && equip_ctr > 0) || inven_ctr == 0)
		{
		  if (scr_state != BLANK_SCR)
		    inven_screen(EQUIP_SCR);
		  command = 'r';	/* Remove - or take off and drop. */
		}
	      else if (scr_state != BLANK_SCR)
		inven_screen(INVEN_SCR);
	    }
	  break;
	case 'w':	  /* Wear/wield	   */
	  for (wear_low = 0;
	       wear_low < inven_ctr && inventory[wear_low].tval > TV_MAX_WEAR;
	       wear_low++)
	    ;
	  for(wear_high = wear_low;
	      wear_high < inven_ctr && inventory[wear_high].tval >=TV_MIN_WEAR;
	      wear_high++)
	    ;
	  wear_high--;
	  if (wear_low > wear_high)
	    msg_print("You have nothing to wear or wield.");
	  else
	    {
	      if (scr_state != BLANK_SCR && scr_state != INVEN_SCR)
		inven_screen(WEAR_SCR);
	      selecting = TRUE;
	    }
	  break;
	case 'x':
	  if (inventory[INVEN_WIELD].tval == TV_NOTHING &&
	      inventory[INVEN_AUX].tval == TV_NOTHING)
	    msg_print("But you are wielding no weapons.");
#ifdef ATARIST_MWC
	  else if ((holder = TR_CURSED) & inventory[INVEN_WIELD].flags)
#else
	  else if (TR_CURSED & inventory[INVEN_WIELD].flags)
#endif
	    {
	      objdes(prt1, &inventory[INVEN_WIELD], FALSE);
	      (void) sprintf(prt2,
		     "The %s you are wielding appears to be cursed.", prt1);
	      msg_print(prt2);
	    }
	  else
	    {
	      free_turn_flag = FALSE;
	      tmp_obj = inventory[INVEN_AUX];
	      inventory[INVEN_AUX] = inventory[INVEN_WIELD];
	      inventory[INVEN_WIELD] = tmp_obj;
	      if (scr_state == EQUIP_SCR)
		scr_left = show_equip(show_weight_flag, scr_left);
	      py_bonuses(&inventory[INVEN_AUX], -1);	 /* Subtract bonuses */
	      py_bonuses(&inventory[INVEN_WIELD], 1);	   /* Add bonuses    */
	      if (inventory[INVEN_WIELD].tval != TV_NOTHING)
		{
		  (void) strcpy(prt1, "Primary weapon   : ");
		  objdes(prt2, &inventory[INVEN_WIELD], TRUE);
		  msg_print(strcat(prt1, prt2));
		}
	      else
		msg_print("No primary weapon.");
	      /* this is a new weapon, so clear the heavy flag */
	      weapon_heavy = FALSE;
	      check_strength();
	    }
	  break;
	case ' ':	/* Dummy command to return again to main prompt. */
	  break;
	case '?':
	  inven_screen(HELP_SCR);
	  break;
	default:
	  /* Nonsense command					   */
	  bell();
	  break;
	}

      /* Clear the doing_inven flag here, instead of at beginning, so that
	 can use it to control when messages above appear. */
      doing_inven = 0;

      /* Keep looking for objects to drop/wear/take off/throw off */
      which = 'z';
      while (selecting && free_turn_flag)
	{
	  swap = "";
	  if (command == 'w')
	    {
	      from = wear_low;
	      to = wear_high;
	      prompt = "Wear/Wield";
	    }
	  else
	    {
	      from = 0;
	      if (command == 'd')
		{
		  to = inven_ctr - 1;
		  prompt = "Drop";
		  if (equip_ctr > 0)
		    swap = ", / for Equip";
		}
	      else
		{
		  to = equip_ctr - 1;
		  if (command == 't')
		    prompt = "Take off";
		  else	/* command == 'r' */
		    {
		      prompt = "Throw off";
		      if (inven_ctr > 0)
			swap = ", / for Inven";
		    }
		}
	    }
	  if (from > to)
	    selecting = FALSE;
	  else
	    {
	      if (scr_state == BLANK_SCR)
		disp = ", * to list";
	      else
		disp = "";
	      (void) sprintf(prt1,
		      "(%c-%c%s%s, space to break, ESC to exit) %s which one?",
		      from+'a', to+'a', disp, swap, prompt);

	      /* Abort everything. */
	      if (!get_com(prt1, &which))
		{
		  selecting = FALSE;
		  which = ESCAPE;
		}
	      /* Draw the screen and maybe exit to main prompt. */
	      else if (which == ' ' || which == '*')
		{
		  if (command == 't' || command == 'r')
		    inven_screen(EQUIP_SCR);
		  else if (command == 'w' && scr_state != INVEN_SCR)
		    inven_screen(WEAR_SCR);
		  else
		    inven_screen(INVEN_SCR);
		  if (which == ' ')
		    selecting = FALSE;
		}
	      /* Swap screens (for drop) */
	      else if (which == '/' && swap[0])
		{
		  if (command == 'd')
		    command = 'r';
		  else
		    command = 'd';
		  if (scr_state == EQUIP_SCR)
		    inven_screen(INVEN_SCR);
		  else if (scr_state == INVEN_SCR)
		    inven_screen(EQUIP_SCR);
		}
	      else if ((which < from + 'a' || which > to + 'a')
		       && (which < from + 'A' || which > to + 'A'))
		bell();
	      else  /* Found an item! */
		{
		  if (isupper((int)which))
		    item = which - 'A';
		  else
		    item = which - 'a';
		  if (command == 'r' || command == 't')
		    {
		      /* Get its place in the equipment list. */
		      tmp = item;
		      item = 21;
		      do
			{
			  item++;
			  if (inventory[item].tval != TV_NOTHING)
			    tmp--;
			}
		      while (tmp >= 0);
		      if (isupper((int)which) && !verify(prompt, item))
			item = -1;
#ifdef ATARIST_MWC
		      else if ((holder = TR_CURSED) & inventory[item].flags)
#else
		      else if (TR_CURSED & inventory[item].flags)
#endif
			{
			  msg_print("Hmmm, it seems to be cursed.");
			  item = -1;
			}
		      else if (command == 't' &&
			       !inven_check_num(&inventory[item]))
			{
			  if (cave[char_row][char_col].tptr != 0)
			    {
			      msg_print("You can't carry it.");
			      item = -1;
			    }
			  else if (get_check("You can't carry it.  Drop it?"))
			    command = 'r';
			  else
			    item = -1;
			}
		      if (item >= 0)
			{
			  if (command == 'r')
			    {
			      inven_drop(item, TRUE);
			      /* As a safety measure, set the player's inven
				 weight to 0, when the last object is dropped*/
			      if (inven_ctr == 0 && equip_ctr == 0)
				inven_weight = 0;
			    }
			  else
			    {
			      slot = inven_carry(&inventory[item]);
			      takeoff(item, slot);
			    }
			  check_strength();
			  free_turn_flag = FALSE;
			  if (command == 'r')
			    selecting = FALSE;
			}
		    }
		  else if (command == 'w')
		    {
		      /* Wearing. Go to a bit of trouble over replacing
			 existing equipment. */
		      if (isupper((int)which) && !verify(prompt, item))
			item = -1;
		      else switch(inventory[item].tval)
			{ /* Slot for equipment	   */
			case TV_SLING_AMMO: case TV_BOLT: case TV_ARROW:
			case TV_BOW: case TV_HAFTED: case TV_POLEARM:
			case TV_SWORD: case TV_DIGGING: case TV_SPIKE:
			  slot = INVEN_WIELD; break;
			case TV_LIGHT: slot = INVEN_LIGHT; break;
			case TV_BOOTS: slot = INVEN_FEET; break;
			case TV_GLOVES: slot = INVEN_HANDS; break;
			case TV_CLOAK: slot = INVEN_OUTER; break;
			case TV_HELM: slot = INVEN_HEAD; break;
			case TV_SHIELD: slot = INVEN_ARM; break;
			case TV_HARD_ARMOR: case TV_SOFT_ARMOR:
			  slot = INVEN_BODY; break;
			case TV_AMULET: slot = INVEN_NECK; break;
			case TV_RING:
			  if (inventory[INVEN_RIGHT].tval == TV_NOTHING)
			    slot = INVEN_RIGHT;
			  else if (inventory[INVEN_LEFT].tval == TV_NOTHING)
			    slot = INVEN_LEFT;
			  else
			    {
			      slot = 0;
			      /* Rings. Give some choice over where they go. */
			      do
				{
				  if (!get_com(
			       "Put ring on which hand (l/r/L/R)?", &query))
				    {
				      item = -1;
				      slot = -1;
				    }
				  else if (query == 'l')
				    slot = INVEN_LEFT;
				  else if (query == 'r')
				    slot = INVEN_RIGHT;
				  else
				    {
				      if (query == 'L')
					slot = INVEN_LEFT;
				      else if (query == 'R')
					slot = INVEN_RIGHT;
				      else
					bell();
				      if (slot && !verify("Replace", slot))
					slot = 0;
				    }
				}
			      while(slot == 0);
			    }
			  break;
			default:
		  msg_print("IMPOSSIBLE: I don't see how you can use that.");
			  item = -1;
			  break;
			}
		      if (item >= 0 && inventory[slot].tval != TV_NOTHING)
			{
#ifdef ATARIST_MWC
			  if ((holder = TR_CURSED) & inventory[slot].flags)
#else
			  if (TR_CURSED & inventory[slot].flags)
#endif
			    {
			      objdes(prt1, &inventory[slot], FALSE);
			      (void) sprintf(prt2, "The %s you are ", prt1);
			      if (slot == INVEN_HEAD)
				(void) strcat(prt2, "wielding ");
			      else
				(void) strcat(prt2, "wearing ");
			      msg_print(strcat(prt2, "appears to be cursed."));
			      item = -1;
			    }
			  else if (inventory[item].subval == ITEM_GROUP_MIN &&
				   inventory[item].number > 1 &&
				   !inven_check_num(&inventory[slot]))
			    {
			      /* this can happen if try to wield a torch, and
				 have more than one in your inventory */
			   msg_print("You will have to drop something first.");
			      item = -1;
			    }
			}
		      if (item >= 0)
			{
			  /* OK. Wear it. */
			  free_turn_flag = FALSE;

			  /* first remove new item from inventory */
			  tmp_obj = inventory[item];
			  i_ptr = &tmp_obj;

			  wear_high--;
			  /* Fix for torches	   */
			  if (i_ptr->number > 1
			      && i_ptr->subval <= ITEM_SINGLE_STACK_MAX)
			    {
			      i_ptr->number = 1;
			      wear_high++;
			    }
			  inven_weight += i_ptr->weight*i_ptr->number;
			  inven_destroy(item);	/* Subtracts weight */

			  /* second, add old item to inv and remove from
			     equipment list, if necessary */
			  i_ptr = &inventory[slot];
			  if (i_ptr->tval != TV_NOTHING)
			    {
			      tmp2 = inven_ctr;
			      tmp = inven_carry(i_ptr);
			      /* if item removed did not stack with anything in
				 inventory, then increment wear_high */
			      if (inven_ctr != tmp2)
				wear_high++;
			      takeoff(slot, tmp);
			    }

			  /* third, wear new item */
			  *i_ptr = tmp_obj;
			  equip_ctr++;
			  py_bonuses(i_ptr, 1);
			  if (slot == INVEN_WIELD)
			    string = "You are wielding";
			  else if (slot == INVEN_LIGHT)
			    string = "Your light source is";
			  else
			    string = "You are wearing";
			  objdes(prt2, i_ptr, TRUE);
			  /* Get the right equipment letter. */
			  tmp = INVEN_WIELD;
			  item = 0;
			  while (tmp != slot)
			    if (inventory[tmp++].tval != TV_NOTHING)
			      item++;

			  (void) sprintf(prt1, "%s %s (%c)", string, prt2,
					 'a'+item);
			  msg_print(prt1);
			  /* this is a new weapon, so clear the heavy flag */
			  if (slot == INVEN_WIELD)
			    weapon_heavy = FALSE;
			  check_strength();
#ifdef ATARIST_MWC
			  if (i_ptr->flags & (holder = TR_CURSED))
#else
			  if (i_ptr->flags & TR_CURSED)
#endif
			    {
			      msg_print("Oops! It feels deathly cold!");
			      add_inscribe(i_ptr, ID_DAMD);
			      /* To force a cost of 0, even if unidentified. */
			      i_ptr->cost = -1;
			    }
			}
		    }
		  else /* command == 'd' */
		    {
		      if (inventory[item].number > 1)
			{
			  objdes(prt1, &inventory[item], TRUE);
			  prt1[strlen(prt1)-1] = '?';
			  (void) sprintf(prt2, "Drop all %s [y/n]", prt1);
			  prt1[strlen(prt1)-1] = '.';
			  prt(prt2, 0, 0);
			  query = inkey();
			  if (query != 'y' && query != 'n')
			    {
			      if (query != ESCAPE)
				bell();
			      erase_line(MSG_LINE, 0);
			      item = -1;
			    }
			}
		      else if (isupper((int)which) && !verify(prompt, item))
			item = -1;
		      else
			query = 'y';
		      if (item >= 0)
			{
			  free_turn_flag = FALSE;    /* Player turn   */
			  inven_drop(item, query == 'y');
			  check_strength();
			}
		      selecting = FALSE;
		      /* As a safety measure, set the player's inven weight
			 to 0, when the last object is dropped.  */
		      if (inven_ctr == 0 && equip_ctr == 0)
			inven_weight = 0;
		    }
		  if (free_turn_flag == FALSE && scr_state == BLANK_SCR)
		    selecting = FALSE;
		}
	    }
	}
      if (which == ESCAPE || scr_state == BLANK_SCR)
	command = ESCAPE;
      else if (!free_turn_flag)
	{
	  /* Save state for recovery if they want to call us again next turn.*/
	  if (selecting)
	    doing_inven = command;
	  else
	    doing_inven = ' ';	/* A dummy command to recover screen. */
	  /* flush last message before clearing screen_change and exiting */
	  msg_print(CNIL);
	  screen_change = FALSE;/* This lets us know if the world changes */
	  command = ESCAPE;
	}
      else
	{
	  /* Put an appropriate header. */
	  if (scr_state == INVEN_SCR)
	    {
	      if (! show_weight_flag || inven_ctr == 0)
		(void) sprintf(prt1,
		    "You are carrying %d.%d pounds. In your pack there is %s",
			       inven_weight / 10, inven_weight % 10,
			       (inven_ctr == 0 ? "nothing." : "-"));
	      else
		(void) sprintf (prt1,
	"You are carrying %d.%d pounds. Your capacity is %d.%d pounds. %s",
				inven_weight / 10, inven_weight % 10,
				weight_limit () / 10, weight_limit () % 10,
				"In your pack is -");
	      prt(prt1, 0, 0);
	    }
	  else if (scr_state == WEAR_SCR)
	    {
	      if (wear_high < wear_low)
		prt("You have nothing you could wield.", 0, 0);
	      else
		prt("You could wield -", 0, 0);
	    }
	  else if (scr_state == EQUIP_SCR)
	    {
	      if (equip_ctr == 0)
		prt("You are not using anything.", 0, 0);
	      else
		prt("You are using -", 0, 0);
	    }
	  else
	    prt("Allowed commands:", 0, 0);
	  erase_line(scr_base, scr_left);
	  put_buffer("e/i/t/w/x/d/?/ESC:", scr_base, 60);
	  command = inkey();
	  erase_line(scr_base, scr_left);
	}
    }
  while (command != ESCAPE);
  if (scr_state != BLANK_SCR)
    restore_screen();
  calc_bonuses();
}


/* Get the ID of an item and return the CTR value of it	-RAK-	*/
int get_item(com_val, pmt, i, j, mask, message)
int *com_val;
char *pmt;
int i, j;
char *mask;
char *message;
{
  vtype out_val;
  char which;
  register int test_flag, item;
  int full, i_scr, redraw;

  item = FALSE;
  redraw = FALSE;
  *com_val = 0;
  i_scr = 1;
  if (j > INVEN_WIELD)
    {
      full = TRUE;
      if (inven_ctr == 0)
	{
	  i_scr = 0;
	  j = equip_ctr - 1;
	}
      else
	j = inven_ctr - 1;
    }
  else
    full = FALSE;

  if (inven_ctr > 0 || (full && equip_ctr > 0))
    {
      do
	{
	  if (redraw)
	    {
	      if (i_scr > 0)
		(void) show_inven (i, j, FALSE, 80, mask);
	      else
		(void) show_equip (FALSE, 80);
	    }
	  if (full)
	    (void) sprintf(out_val,
			   "(%s: %c-%c,%s / for %s, or ESC) %s",
			   (i_scr > 0 ? "Inven" : "Equip"), i+'a', j+'a',
			   (redraw ? "" : " * to see,"),
			   (i_scr > 0 ? "Equip" : "Inven"), pmt);
	  else
	    (void) sprintf(out_val,
			   "(Items %c-%c,%s ESC to exit) %s", i+'a', j+'a',
			   (redraw ? "" : " * for inventory list,"), pmt);
	  test_flag = FALSE;
	  prt(out_val, 0, 0);
	  do
	    {
	      which = inkey();
	      switch(which)
		{
		case ESCAPE:
		  test_flag = TRUE;
		  free_turn_flag = TRUE;
		  i_scr = -1;
		  break;
		case '/':
		  if (full)
		    {
		      if (i_scr > 0)
			{
			  if (equip_ctr == 0)
			    {
			      prt("But you're not using anything -more-",0,0);
			      (void) inkey();
			    }
			  else
			    {
			      i_scr = 0;
			      test_flag = TRUE;
			      if (redraw)
				{
				  j = equip_ctr;
				  while (j < inven_ctr)
				    {
				      j++;
				      erase_line(j, 0);
				    }
				}
			      j = equip_ctr - 1;
			    }
			  prt(out_val, 0, 0);
			}
		      else
			{
			  if (inven_ctr == 0)
			    {
			    prt("But you're not carrying anything -more-",0,0);
			      (void) inkey();
			    }
			  else
			    {
			      i_scr = 1;
			      test_flag = TRUE;
			      if (redraw)
				{
				  j = inven_ctr;
				  while (j < equip_ctr)
				    {
				      j++;
				      erase_line (j, 0);
				    }
				}
			      j = inven_ctr - 1;
			    }
			}
		    }
		  break;
		case '*':
		  if (!redraw)
		    {
		      test_flag = TRUE;
		      save_screen();
		      redraw = TRUE;
		    }
		  break;
		default:
		  if (isupper((int)which))
		    *com_val = which - 'A';
		  else
		    *com_val = which - 'a';
		  if ((*com_val >= i) && (*com_val <= j)
		      && (mask == CNIL || mask[*com_val]))
		    {
		      if (i_scr == 0)
			{
			  i = 21;
			  j = *com_val;
			  do
			    {
			      while (inventory[++i].tval == TV_NOTHING);
			      j--;
			    }
			  while (j >= 0);
			  *com_val = i;
			}
		      if (isupper((int)which) && !verify("Try", *com_val))
			{
			  test_flag = TRUE;
			  free_turn_flag = TRUE;
			  i_scr = -1;
			  break;
			}
		      test_flag = TRUE;
		      item = TRUE;
		      i_scr = -1;
		    }
		  else if (message)
		    {
		      msg_print (message);
		      /* Set test_flag to force redraw of the question.  */
		      test_flag = TRUE;
		    }
		  else
		    bell();
		  break;
		}
	    }
	  while (!test_flag);
	}
      while (i_scr >= 0);
      if (redraw)
	restore_screen();
      erase_line(MSG_LINE, 0);
    }
  else
    prt("You are not carrying anything.", 0, 0);
  return(item);
}

/* I may have written the town level code, but I'm not exactly	 */
/* proud of it.	 Adding the stores required some real slucky	 */
/* hooks which I have not had time to re-think.		 -RAK-	 */

/* Returns true if player has no light			-RAK-	*/
int no_light()
{
  register cave_type *c_ptr;

  c_ptr = &cave[char_row][char_col];
  if (!c_ptr->tl && !c_ptr->pl)
    return TRUE;
  return FALSE;
}


/* map rogue_like direction commands into numbers */
static char map_roguedir(comval)
register char comval;
{
  switch(comval)
    {
    case 'h':
      comval = '4';
      break;
    case 'y':
      comval = '7';
      break;
    case 'k':
      comval = '8';
      break;
    case 'u':
      comval = '9';
      break;
    case 'l':
      comval = '6';
      break;
    case 'n':
      comval = '3';
      break;
    case 'j':
      comval = '2';
      break;
    case 'b':
      comval = '1';
      break;
    case '.':
      comval = '5';
      break;
    }
  return(comval);
}


/* Prompts for a direction				-RAK-	*/
/* Direction memory added, for repeated commands.  -CJS */
int get_dir(prompt, dir)
char *prompt;
int *dir;
{
  char command;
  int save;
  static char prev_dir;		/* Direction memory. -CJS- */

  if (default_dir)	/* used in counted commands. -CJS- */
    {
      *dir = prev_dir;
      return TRUE;
    }
  if (prompt == CNIL)
    prompt = "Which direction?";
  for (;;)
    {
      save = command_count;	/* Don't end a counted command. -CJS- */
#ifdef MAC
      if (!get_comdir(prompt, &command))
#else
      if (!get_com(prompt, &command))
#endif
	{
	  free_turn_flag = TRUE;
	  return FALSE;
	}
      command_count = save;
      if (rogue_like_commands)
	command = map_roguedir(command);
      if (command >= '1' && command <= '9' && command != '5')
	{
	  prev_dir = command - '0';
	  *dir = prev_dir;
	  return TRUE;
	}
      bell();
    }
}



/* Similar to get_dir, except that no memory exists, and it is		-CJS-
   allowed to enter the null direction. */
int get_alldir(prompt, dir)
char *prompt;
int *dir;
{
  char command;

  for(;;)
    {
#ifdef MAC
      if (!get_comdir(prompt, &command))
#else
      if (!get_com(prompt, &command))
#endif
	{
	  free_turn_flag = TRUE;
	  return FALSE;
	}
      if (rogue_like_commands)
	command = map_roguedir(command);
      if (command >= '1' && command <= '9')
	{
	  *dir = command - '0';
	  return TRUE;
	}
      bell();
    }
}


/* Moves creature record from one space to another	-RAK-	*/
void move_rec(y1, x1, y2, x2)
register int y1, x1, y2, x2;
{
  int tmp;

  /* this always works correctly, even if y1==y2 and x1==x2 */
  tmp = cave[y1][x1].cptr;
  cave[y1][x1].cptr = 0;
  cave[y2][x2].cptr = tmp;
}


/* Room is lit, make it appear				-RAK-	*/
void light_room(y, x)
int y, x;
{
  register int i, j, start_col, end_col;
  int tmp1, tmp2, start_row, end_row;
  register cave_type *c_ptr;
  int tval;

  tmp1 = (SCREEN_HEIGHT/2);
  tmp2 = (SCREEN_WIDTH /2);
  start_row = (y/tmp1)*tmp1;
  start_col = (x/tmp2)*tmp2;
  end_row = start_row + tmp1 - 1;
  end_col = start_col + tmp2 - 1;
  for (i = start_row; i <= end_row; i++)
    for (j = start_col; j <= end_col; j++)
      {
	c_ptr = &cave[i][j];
	if (c_ptr->lr && ! c_ptr->pl)
	  {
	    c_ptr->pl = TRUE;
	    if (c_ptr->fval == DARK_FLOOR)
	      c_ptr->fval = LIGHT_FLOOR;
	    if (! c_ptr->fm && c_ptr->tptr != 0)
	      {
		tval = t_list[c_ptr->tptr].tval;
		if (tval >= TV_MIN_VISIBLE && tval <= TV_MAX_VISIBLE)
		  c_ptr->fm = TRUE;
	      }
	    print(loc_symbol(i, j), i, j);
	  }
      }
}


/* Lights up given location				-RAK-	*/
void lite_spot(y, x)
register int y, x;
{
  if (panel_contains(y, x))
    print(loc_symbol(y, x), y, x);
}


/* Normal movement					*/
/* When FIND_FLAG,  light only permanent features	*/
static void sub1_move_light(y1, x1, y2, x2)
register int x1, x2;
int y1, y2;
{
  register int i, j;
  register cave_type *c_ptr;
  int tval, top, left, bottom, right;

  if (light_flag)
    {
      for (i = y1-1; i <= y1+1; i++)	   /* Turn off lamp light	*/
	for (j = x1-1; j <= x1+1; j++)
	  cave[i][j].tl = FALSE;
      if (find_flag && !find_prself)
	light_flag = FALSE;
    }
  else if (!find_flag || find_prself)
    light_flag = TRUE;

  for (i = y2-1; i <= y2+1; i++)
    for (j = x2-1; j <= x2+1; j++)
      {
	c_ptr = &cave[i][j];
	/* only light up if normal movement */
	if (light_flag)
	  c_ptr->tl = TRUE;
	if (c_ptr->fval >= MIN_CAVE_WALL)
	  c_ptr->pl = TRUE;
	else if (!c_ptr->fm && c_ptr->tptr != 0)
	  {
	    tval = t_list[c_ptr->tptr].tval;
	    if ((tval >= TV_MIN_VISIBLE) && (tval <= TV_MAX_VISIBLE))
	      c_ptr->fm = TRUE;
	  }
      }

  /* From uppermost to bottom most lines player was on.	 */
  if (y1 < y2)
    {
      top = y1 - 1;
      bottom = y2 + 1;
    }
  else
    {
      top = y2 - 1;
      bottom = y1 + 1;
    }
  if (x1 < x2)
    {
      left = x1 - 1;
      right = x2 + 1;
    }
  else
    {
      left = x2 - 1;
      right = x1 + 1;
    }
  for (i = top; i <= bottom; i++)
    for (j = left; j <= right; j++)   /* Leftmost to rightmost do*/
      print(loc_symbol(i, j), i, j);
}


/* When blinded,  move only the player symbol.		*/
/* With no light,  movement becomes involved.		*/
static void sub3_move_light(y1, x1, y2, x2)
register int y1, x1;
int y2, x2;
{
  register int i, j;

  if (light_flag)
    {
      for (i = y1-1; i <= y1+1; i++)
	for (j = x1-1; j <= x1+1; j++)
	  {
	    cave[i][j].tl = FALSE;
	    print(loc_symbol(i, j), i, j);
	  }
      light_flag = FALSE;
    }
  else if (!find_flag || find_prself)
    print(loc_symbol(y1, x1), y1, x1);

  if (!find_flag || find_prself)
    print('@', y2, x2);
}


/* Package for moving the character's light about the screen	 */
/* Four cases : Normal, Finding, Blind, and Nolight	 -RAK-	 */
void move_light(y1, x1, y2, x2)
int y1, x1, y2, x2;
{
  if (py.flags.blind > 0 || !player_light)
    sub3_move_light(y1, x1, y2, x2);
  else
    sub1_move_light(y1, x1, y2, x2);
}


/* Something happens to disturb the player.		-CJS-
   The first arg indicates a major disturbance, which affects search.
   The second arg indicates a light change. */
void disturb(s, l)
int s, l;
{
  command_count = 0;
  if (s && (py.flags.status & PY_SEARCH))
    search_off();
  if (py.flags.rest != 0)
    rest_off();
  if (l || find_flag)
    {
      find_flag = FALSE;
      check_view();
    }
  flush();
}


/* Search Mode enhancement				-RAK-	*/
void search_on()
{
  change_speed(1);
  py.flags.status |= PY_SEARCH;
  prt_state();
  prt_speed();
  py.flags.food_digested++;
}

void search_off()
{
#ifdef ATARIST_MWC
  int32u holder;
#endif

  check_view();
  change_speed(-1);
#ifdef ATARIST_MWC
  py.flags.status &= ~(holder = PY_SEARCH);
#else
  py.flags.status &= ~PY_SEARCH;
#endif
  prt_state();
  prt_speed();
  py.flags.food_digested--;
}


/* Resting allows a player to safely restore his hp	-RAK-	*/
void rest()
{
  int rest_num;
  vtype rest_str;

  if (command_count > 0)
    {
      rest_num = command_count;
      command_count = 0;
    }
  else
    {
      prt("Rest for how long? ", 0, 0);
      rest_num = 0;
      if (get_string(rest_str, 0, 19, 5))
	{
	  if (rest_str[0] == '*')
	    rest_num = -MAX_SHORT;
	  else
	    rest_num = atoi(rest_str);
	}
    }
  /* check for reasonable value, must be positive number in range of a
     short, or must be -MAX_SHORT */
  if ((rest_num == -MAX_SHORT)
      || (rest_num > 0) && (rest_num < MAX_SHORT))
    {
      if (py.flags.status & PY_SEARCH)
	search_off();
      py.flags.rest = rest_num;
      py.flags.status |= PY_REST;
      prt_state();
      py.flags.food_digested--;
      prt ("Press any key to stop resting...", 0, 0);
      put_qio();
    }
  else
    {
      if (rest_num != 0)
	msg_print ("Invalid rest count.");
      erase_line(MSG_LINE, 0);
      free_turn_flag = TRUE;
    }
}

void rest_off()
{
#ifdef ATARIST_MWC
  int32u holder;
#endif

  py.flags.rest = 0;
#ifdef ATARIST_MWC
  py.flags.status &= ~(holder = PY_REST);
#else
  py.flags.status &= ~PY_REST;
#endif
  prt_state();
  msg_print(CNIL); /* flush last message, or delete "press any key" message */
  py.flags.food_digested++;
}


/* Attacker's level and plusses,  defender's AC		-RAK-	*/
int test_hit(bth, level, pth, ac, attack_type)
int bth, level, pth, ac, attack_type;
{
  register int i, die;

  disturb (1, 0);
  i = bth + pth * BTH_PLUS_ADJ
    + (level * class_level_adj[py.misc.pclass][attack_type]);
  /* pth could be less than 0 if player wielding weapon too heavy for him */
  /* always miss 1 out of 20, always hit 1 out of 20 */
  die = randint (20);
  if ((die != 1) && ((die == 20)
		     || ((i > 0) && (randint (i) > ac))))  /* normal hit */
    return TRUE;
  else
    return FALSE;
}


/* Decreases players hit points and sets death flag if necessary*/
/*							 -RAK-	 */
void take_hit(damage, hit_from)
int damage;
char *hit_from;
{
  if (py.flags.invuln > 0)  damage = 0;
  py.misc.chp -= damage;
  if (py.misc.chp < 0)
    {
      if (!death)
	{
	  death = TRUE;
	  (void) strcpy(died_from, hit_from);
	  total_winner = FALSE;
	}
      new_level_flag = TRUE;
    }
  else
    prt_chp();
}
