/* source/spells.c: player/creature spells, breaths, wands, scrolls, etc. code

   Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#ifdef __TURBOC__
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
static void replace_spot(int, int, int);
#else
static void replace_spot();
#endif

/* Following are spell procedure/functions			-RAK-	*/
/* These routines are commonly used in the scroll, potion, wands, and	 */
/* staves routines, and are occasionally called from other areas.	  */
/* Now included are creature spells also.		       -RAK    */

void monster_name (m_name, m_ptr, r_ptr)
char *m_name;
monster_type *m_ptr;
creature_type *r_ptr;
{
  if (!m_ptr->ml)
    (void) strcpy (m_name, "It");
  else
    (void) sprintf (m_name, "The %s", r_ptr->name);
}

void lower_monster_name (m_name, m_ptr, r_ptr)
char *m_name;
monster_type *m_ptr;
creature_type *r_ptr;
{
  if (!m_ptr->ml)
    (void) strcpy (m_name, "it");
  else
    (void) sprintf (m_name, "the %s", r_ptr->name);
}

/* Sleep creatures adjacent to player			-RAK-	*/
int sleep_monsters1(y, x)
int y, x;
{
  register int i, j;
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  int sleep;
  vtype out_val, m_name;

  sleep = FALSE;
  for (i = y-1; i <= y+1; i++)
    for (j = x-1; j <= x+1; j++)
      {
	c_ptr = &cave[i][j];
	if (c_ptr->cptr > 1)
	  {
	    m_ptr = &m_list[c_ptr->cptr];
	    r_ptr = &c_list[m_ptr->mptr];

	    monster_name (m_name, m_ptr, r_ptr);
	    if ((randint(MAX_MONS_LEVEL) < r_ptr->level) ||
		(CD_NO_SLEEP & r_ptr->cdefense))
	      {
		if (m_ptr->ml && (r_ptr->cdefense & CD_NO_SLEEP))
		  c_recall[m_ptr->mptr].r_cdefense |= CD_NO_SLEEP;
		(void) sprintf(out_val, "%s is unaffected.", m_name);
		msg_print(out_val);
	      }
	    else
	      {
		sleep = TRUE;
		m_ptr->csleep = 500;
		(void) sprintf(out_val, "%s falls asleep.", m_name);
		msg_print(out_val);
	      }
	  }
      }
  return(sleep);
}

/* Detect any treasure on the current panel		-RAK-	*/
int detect_treasure()
{
  register int i, j, detect;
  register cave_type *c_ptr;

  detect = FALSE;
  for (i = panel_row_min; i <= panel_row_max; i++)
    for (j = panel_col_min; j <= panel_col_max; j++)
      {
	c_ptr = &cave[i][j];
	if ((c_ptr->tptr != 0) && (t_list[c_ptr->tptr].tval == TV_GOLD) &&
	    !test_light(i, j))
	  {
	    c_ptr->fm = TRUE;
	    lite_spot(i, j);
	    detect = TRUE;
	  }
      }
  return(detect);
}


/* Detect all objects on the current panel		-RAK-	*/
int detect_object()
{
  register int i, j, detect;
  register cave_type *c_ptr;

  detect = FALSE;
  for (i = panel_row_min; i <= panel_row_max; i++)
    for (j = panel_col_min; j <= panel_col_max; j++)
      {
	c_ptr = &cave[i][j];
	if ((c_ptr->tptr != 0) && (t_list[c_ptr->tptr].tval < TV_MAX_OBJECT)
	    && !test_light(i, j))
	  {
	    c_ptr->fm = TRUE;
	    lite_spot(i, j);
	    detect = TRUE;
	  }
      }
  return(detect);
}


/* Locates and displays traps on current panel		-RAK-	*/
int detect_trap()
{
  register int i, j;
  int detect;
  register cave_type *c_ptr;
  register inven_type *t_ptr;

  detect = FALSE;
  for (i = panel_row_min; i <= panel_row_max; i++)
    for (j = panel_col_min; j <= panel_col_max; j++)
      {
	c_ptr = &cave[i][j];
	if (c_ptr->tptr != 0)
	  if (t_list[c_ptr->tptr].tval == TV_INVIS_TRAP)
	    {
	      c_ptr->fm = TRUE;
	      change_trap(i, j);
	      detect = TRUE;
	    }
	  else if (t_list[c_ptr->tptr].tval == TV_CHEST)
	    {
	      t_ptr = &t_list[c_ptr->tptr];
	      known2(t_ptr);
	    }
      }
  return(detect);
}


/* Locates and displays all secret doors on current panel -RAK-	*/
int detect_sdoor()
{
  register int i, j, detect;
  register cave_type *c_ptr;

  detect = FALSE;
  for (i = panel_row_min; i <= panel_row_max; i++)
    for (j = panel_col_min; j <= panel_col_max; j++)
      {
	c_ptr = &cave[i][j];
	if (c_ptr->tptr != 0)
	  /* Secret doors  */
	  if (t_list[c_ptr->tptr].tval == TV_SECRET_DOOR)
	    {
	      c_ptr->fm = TRUE;
	      change_trap(i, j);
	      detect = TRUE;
	    }
	/* Staircases	 */
	  else if (((t_list[c_ptr->tptr].tval == TV_UP_STAIR) ||
		    (t_list[c_ptr->tptr].tval == TV_DOWN_STAIR)) &&
		   !c_ptr->fm)
	    {
	      c_ptr->fm = TRUE;
	      lite_spot(i, j);
	      detect = TRUE;
	    }
      }
  return(detect);
}


/* Locates and displays all invisible creatures on current panel -RAK-*/
int detect_invisible()
{
  register int i, flag;
  register monster_type *m_ptr;
#ifdef ATARIST_MWC
  int32u holder;
#endif

  flag = FALSE;
  for (i = mfptr - 1; i >= MIN_MONIX; i--)
    {
      m_ptr = &m_list[i];
      if (panel_contains((int)m_ptr->fy, (int)m_ptr->fx) &&
#ifdef ATARIST_MWC
	  ((holder = CM_INVISIBLE) & c_list[m_ptr->mptr].cmove))
#else
	  (CM_INVISIBLE & c_list[m_ptr->mptr].cmove))
#endif
	{
	  m_ptr->ml = TRUE;
	  /* works correctly even if hallucinating */
	  print((char)c_list[m_ptr->mptr].cchar, (int)m_ptr->fy,
		(int)m_ptr->fx);
	  flag = TRUE;
	}
    }
  if (flag)
    {
      msg_print("You sense the presence of invisible creatures!");
      msg_print(CNIL);
      /* must unlight every monster just lighted */
      creatures(FALSE);
    }
  return(flag);
}


/* Light an area: 1.  If corridor  light immediate area -RAK-*/
/*		  2.  If room  light entire room plus immediate area.     */
int light_area(y, x)
register int y, x;
{
  register int i, j, light;

  if (py.flags.blind < 1)
    msg_print("You are surrounded by a white light.");
  light = TRUE;
  if (cave[y][x].lr && (dun_level > 0))
    light_room(y, x);
  /* Must always light immediate area, because one might be standing on
     the edge of a room, or next to a destroyed area, etc.  */
  for (i = y-1; i <= y+1; i++)
    for (j = x-1; j <=  x+1; j++)
      {
	cave[i][j].pl = TRUE;
	lite_spot(i, j);
      }
  return(light);
}


/* Darken an area, opposite of light area		-RAK-	*/
int unlight_area(y, x)
int y, x;
{
  register int i, j;
  int tmp1, tmp2, unlight;
  int start_row, start_col, end_row, end_col;
  register cave_type *c_ptr;

  unlight = FALSE;
  if (cave[y][x].lr && (dun_level > 0))
    {
      tmp1 = (SCREEN_HEIGHT/2);
      tmp2 = (SCREEN_WIDTH /2);
      start_row = (y/tmp1)*tmp1 + 1;
      start_col = (x/tmp2)*tmp2 + 1;
      end_row = start_row + tmp1 - 1;
      end_col = start_col + tmp2 - 1;
      for (i = start_row; i <= end_row; i++)
	{
	  for (j = start_col; j <= end_col; j++)
	    {
	      c_ptr = &cave[i][j];
	      if (c_ptr->lr && c_ptr->fval <= MAX_CAVE_FLOOR)
		{
		  c_ptr->pl = FALSE;
		  c_ptr->fval = DARK_FLOOR;
		  lite_spot (i, j);
		  if (!test_light(i, j))
		    unlight = TRUE;
		}
	    }
	}
    }
  else
    for (i = y-1; i <= y+1; i++)
      for (j = x-1; j <= x+1; j++)
	{
	  c_ptr = &cave[i][j];
	  if ((c_ptr->fval == CORR_FLOOR) && c_ptr->pl)
	    {
	      /* pl could have been set by star-lite wand, etc */
	      c_ptr->pl = FALSE;
	      unlight = TRUE;
	    }
	}

  if (unlight && py.flags.blind <= 0)
    msg_print("Darkness surrounds you.");

  return(unlight);
}


/* Map the current area plus some			-RAK-	*/
void map_area()
{
  register cave_type *c_ptr;
  register int i7, i8, n, m;
  int i, j, k, l;

  i = panel_row_min - randint(10);
  j = panel_row_max + randint(10);
  k = panel_col_min - randint(20);
  l = panel_col_max + randint(20);
  for (m = i; m <= j; m++)
    for (n = k; n <= l; n++)
      if (in_bounds(m, n) && (cave[m][n].fval <= MAX_CAVE_FLOOR))
	for (i7 = m-1; i7 <= m+1; i7++)
	  for (i8 = n-1; i8 <= n+1; i8++)
	    {
	      c_ptr = &cave[i7][i8];
	      if (c_ptr->fval >= MIN_CAVE_WALL)
		c_ptr->pl = TRUE;
	      else if ((c_ptr->tptr != 0) &&
		       (t_list[c_ptr->tptr].tval >= TV_MIN_VISIBLE) &&
		       (t_list[c_ptr->tptr].tval <= TV_MAX_VISIBLE))
		c_ptr->fm = TRUE;
	    }
  prt_map();
}


/* Identify an object					-RAK-	*/
int ident_spell()
{
  int item_val;
  bigvtype out_val, tmp_str;
  register int ident;
  register inven_type *i_ptr;

  ident = FALSE;
  if (get_item(&item_val, "Item you wish identified?", 0, INVEN_ARRAY_SIZE,
	       CNIL, CNIL))
    {
      ident = TRUE;
      identify(&item_val);
      i_ptr = &inventory[item_val];
      known2(i_ptr);
      objdes(tmp_str, i_ptr, TRUE);
      if (item_val >= INVEN_WIELD)
	{
	  calc_bonuses();
	  (void) sprintf (out_val, "%s: %s", describe_use(item_val), tmp_str);
	}
      else
	(void) sprintf(out_val, "%c %s", item_val+97, tmp_str);
      msg_print(out_val);
    }
  return(ident);
}


/* Get all the monsters on the level pissed off.	-RAK-	*/
int aggravate_monster (dis_affect)
int dis_affect;
{
  register int i, aggravate;
  register monster_type *m_ptr;

  aggravate = FALSE;
  for (i = mfptr - 1; i >= MIN_MONIX; i--)
    {
      m_ptr = &m_list[i];
      m_ptr->csleep = 0;
      if ((m_ptr->cdis <= dis_affect) && (m_ptr->cspeed < 2))
	{
	  m_ptr->cspeed++;
	  aggravate = TRUE;
	}
    }
  if (aggravate)
    msg_print ("You hear a sudden stirring in the distance!");
  return(aggravate);
}


/* Surround the fool with traps (chuckle)		-RAK-	*/
int trap_creation()
{
  register int i, j, trap;
  register cave_type *c_ptr;

  trap = TRUE;
  for (i = char_row-1; i <= char_row+1; i++)
    for (j = char_col-1; j <= char_col+1; j++)
      {
	/* Don't put a trap under the player, since this can lead to
	   strange situations, e.g. falling through a trap door while
	   trying to rest, setting off a falling rock trap and ending
	   up under the rock.  */
	if (i == char_row && j == char_col)
	  continue;
	c_ptr = &cave[i][j];
	if (c_ptr->fval <= MAX_CAVE_FLOOR)
	  {
	    if (c_ptr->tptr != 0)
	      (void) delete_object(i, j);
	    place_trap(i, j, randint(MAX_TRAP)-1);
	    /* don't let player gain exp from the newly created traps */
	    t_list[c_ptr->tptr].p1 = 0;
	    /* open pits are immediately visible, so call lite_spot */
	    lite_spot(i, j);
	  }
      }
  return(trap);
}


/* Surround the player with doors.			-RAK-	*/
int door_creation()
{
  register int i, j, door;
  int k;
  register cave_type *c_ptr;

  door = FALSE;
  for (i = char_row-1; i <= char_row+1; i++)
    for (j = char_col-1; j <=  char_col+1; j++)
      if ((i != char_row) || (j != char_col))
	{
	  c_ptr = &cave[i][j];
	  if (c_ptr->fval <= MAX_CAVE_FLOOR)
	    {
	      door = TRUE;
	      if (c_ptr->tptr != 0)
		(void) delete_object(i, j);
	      k = popt();
	      c_ptr->fval = BLOCKED_FLOOR;
	      c_ptr->tptr = k;
	      invcopy(&t_list[k], OBJ_CLOSED_DOOR);
	      lite_spot(i, j);
	    }
	}
  return(door);
}


/* Destroys any adjacent door(s)/trap(s)		-RAK-	*/
int td_destroy()
{
  register int i, j, destroy;
  register cave_type *c_ptr;

  destroy = FALSE;
  for (i = char_row-1; i <= char_row+1; i++)
    for (j = char_col-1; j <= char_col+1; j++)
      {
	c_ptr = &cave[i][j];
	if (c_ptr->tptr != 0)
	  {
	    if (((t_list[c_ptr->tptr].tval >= TV_INVIS_TRAP) &&
		 (t_list[c_ptr->tptr].tval <= TV_CLOSED_DOOR) &&
		 (t_list[c_ptr->tptr].tval != TV_RUBBLE)) ||
		(t_list[c_ptr->tptr].tval == TV_SECRET_DOOR))
	      {
		if (delete_object(i, j))
		  destroy = TRUE;
	      }
	    else if (t_list[c_ptr->tptr].tval == TV_CHEST)
	      {
		/* destroy traps on chest and unlock */
		t_list[c_ptr->tptr].flags &= ~(CH_TRAPPED|CH_LOCKED);
		t_list[c_ptr->tptr].name2 = SN_UNLOCKED;
		msg_print ("You have disarmed the chest.");
		known2(&t_list[c_ptr->tptr]);
		destroy = TRUE;
	      }
	  }
      }
  return(destroy);
}


/* Display all creatures on the current panel		-RAK-	*/
int detect_monsters()
{
  register int i, detect;
  register monster_type *m_ptr;
#ifdef ATARIST_MWC
  int32u holder;
#endif

  detect = FALSE;
  for (i = mfptr - 1; i >= MIN_MONIX; i--)
    {
      m_ptr = &m_list[i];
      if (panel_contains((int)m_ptr->fy, (int)m_ptr->fx) &&
#ifdef ATARIST_MWC
	  (((holder = CM_INVISIBLE) & c_list[m_ptr->mptr].cmove) == 0))
#else
	  ((CM_INVISIBLE & c_list[m_ptr->mptr].cmove) == 0))
#endif
	{
	  m_ptr->ml = TRUE;
	  /* works correctly even if hallucinating */
	  print((char)c_list[m_ptr->mptr].cchar, (int)m_ptr->fy,
		(int)m_ptr->fx);
	  detect = TRUE;
	}
    }
  if (detect)
    {
      msg_print("You sense the presence of monsters!");
      msg_print(CNIL);
      /* must unlight every monster just lighted */
      creatures(FALSE);
    }
  return(detect);
}


/* Leave a line of light in given dir, blue light can sometimes	*/
/* hurt creatures.				       -RAK-   */
void light_line(dir, y, x)
int dir, y, x;
{
  register int i;
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  int dist, flag;
  vtype out_val, m_name;

  dist = -1;
  flag = FALSE;
  do
    {
      /* put mmove at end because want to light up current spot */
      dist++;
      c_ptr = &cave[y][x];
      if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE)
	flag = TRUE;
      else
	{
	  if (!c_ptr->pl && !c_ptr->tl)
	    {
	      /* set pl so that lite_spot will work */
	      c_ptr->pl = TRUE;
	      if (c_ptr->fval == LIGHT_FLOOR)
		{
		  if (panel_contains(y, x))
		    light_room(y, x);
		}
	      else
		lite_spot(y, x);
	    }
	  /* set pl in case tl was true above */
	  c_ptr->pl = TRUE;
	  if (c_ptr->cptr > 1)
	    {
	      m_ptr = &m_list[c_ptr->cptr];
	      r_ptr = &c_list[m_ptr->mptr];
	      /* light up and draw monster */
	      update_mon ((int)c_ptr->cptr);
	      monster_name (m_name, m_ptr, r_ptr);
	      if (CD_LIGHT & r_ptr->cdefense)
		{
		  if (m_ptr->ml)
		    c_recall[m_ptr->mptr].r_cdefense |= CD_LIGHT;
		  i = mon_take_hit((int)c_ptr->cptr, damroll(2, 8));
		  if (i >= 0)
		    {
		      (void) sprintf(out_val,
			     "%s shrivels away in the light!", m_name);
		      msg_print(out_val);
		      prt_experience();
		    }
		  else
		    {
		      (void) sprintf(out_val, "%s cringes from the light!",
				     m_name);
		      msg_print (out_val);
		    }
		}
	    }
	}
      (void) mmove(dir, &y, &x);
    }
  while (!flag);
}


/* Light line in all directions				-RAK-	*/
void starlite(y, x)
register int y, x;
{
  register int i;

  if (py.flags.blind < 1)
    msg_print("The end of the staff bursts into a blue shimmering light.");
  for (i = 1; i <= 9; i++)
    if (i != 5)
      light_line(i, y, x);
}


/* Disarms all traps/chests in a given direction	-RAK-	*/
int disarm_all(dir, y, x)
int dir, y, x;
{
  register cave_type *c_ptr;
  register inven_type *t_ptr;
  register int disarm, dist;

  disarm = FALSE;
  dist = -1;
  do
    {
      /* put mmove at end, in case standing on a trap */
      dist++;
      c_ptr = &cave[y][x];
      /* note, must continue upto and including the first non open space,
	 because secret doors have fval greater than MAX_OPEN_SPACE */
      if (c_ptr->tptr != 0)
	{
	  t_ptr = &t_list[c_ptr->tptr];
	  if ((t_ptr->tval == TV_INVIS_TRAP) || (t_ptr->tval == TV_VIS_TRAP))
	    {
	      if (delete_object(y, x))
		disarm = TRUE;
	    }
	  else if (t_ptr->tval == TV_CLOSED_DOOR)
	    t_ptr->p1 = 0;  /* Locked or jammed doors become merely closed. */
	  else if (t_ptr->tval == TV_SECRET_DOOR)
	    {
	      c_ptr->fm = TRUE;
	      change_trap(y, x);
	      disarm = TRUE;
	    }
	  else if ((t_ptr->tval == TV_CHEST) && (t_ptr->flags != 0))
	    {
	      msg_print("Click!");
	      t_ptr->flags &= ~(CH_TRAPPED|CH_LOCKED);
	      disarm = TRUE;
	      t_ptr->name2 = SN_UNLOCKED;
	      known2(t_ptr);
	    }
	}
      (void) mmove(dir, &y, &x);
    }
  while ((dist <= OBJ_BOLT_RANGE) && c_ptr->fval <= MAX_OPEN_SPACE);
  return(disarm);
}


/* Return flags for given type area affect		-RAK-	*/
void get_flags(typ, weapon_type, harm_type, destroy)
int typ;
int32u *weapon_type; int *harm_type;
int (**destroy)();
{
  switch(typ)
    {
    case GF_MAGIC_MISSILE:
      *weapon_type = 0;
      *harm_type   = 0;
      *destroy	   = set_null;
      break;
    case GF_LIGHTNING:
      *weapon_type = CS_BR_LIGHT;
      *harm_type   = CD_LIGHT;
      *destroy	   = set_lightning_destroy;
      break;
    case GF_POISON_GAS:
      *weapon_type = CS_BR_GAS;
      *harm_type   = CD_POISON;
      *destroy	   = set_null;
      break;
    case GF_ACID:
      *weapon_type = CS_BR_ACID;
      *harm_type   = CD_ACID;
      *destroy	   = set_acid_destroy;
      break;
    case GF_FROST:
      *weapon_type = CS_BR_FROST;
      *harm_type   = CD_FROST;
      *destroy	   = set_frost_destroy;
      break;
    case GF_FIRE:
      *weapon_type = CS_BR_FIRE;
      *harm_type   = CD_FIRE;
      *destroy	   = set_fire_destroy;
      break;
    case GF_HOLY_ORB:
      *weapon_type = 0;
      *harm_type   = CD_EVIL;
      *destroy	   = set_null;
      break;
    default:
      msg_print("ERROR in get_flags()\n");
    }
}


/* Shoot a bolt in a given direction			-RAK-	*/
void fire_bolt(typ, dir, y, x, dam, bolt_typ)
int typ, dir, y, x, dam;
char *bolt_typ;
{
  int i, oldy, oldx, dist, flag;
  int32u weapon_type; int harm_type;
  int (*dummy)();
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  vtype out_val, m_name;

  flag = FALSE;
  get_flags(typ, &weapon_type, &harm_type, &dummy);
  oldy = y;
  oldx = x;
  dist = 0;
  do
    {
      (void) mmove(dir, &y, &x);
      dist++;
      c_ptr = &cave[y][x];
      lite_spot(oldy, oldx);
      if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE)
	flag = TRUE;
      else
	{
	  if (c_ptr->cptr > 1)
	    {
	      flag = TRUE;
	      m_ptr = &m_list[c_ptr->cptr];
	      r_ptr = &c_list[m_ptr->mptr];

	      /* light up monster and draw monster, temporarily set
		 pl so that update_mon() will work */
	      i = c_ptr->pl;
	      c_ptr->pl = TRUE;
	      update_mon ((int)c_ptr->cptr);
	      c_ptr->pl = i;
	      /* draw monster and clear previous bolt */
	      put_qio();

	      lower_monster_name(m_name, m_ptr, r_ptr);
	      (void) sprintf(out_val, "The %s strikes %s.", bolt_typ, m_name);
	      msg_print(out_val);
	      if (harm_type & r_ptr->cdefense)
		{
		  dam = dam*2;
		  if (m_ptr->ml)
		    c_recall[m_ptr->mptr].r_cdefense |= harm_type;
		}
	      else if (weapon_type & r_ptr->spells)
		{
		  dam = dam / 4;
		  if (m_ptr->ml)
		    c_recall[m_ptr->mptr].r_spells |= weapon_type;
		}
	      monster_name(m_name, m_ptr, r_ptr);
	      i = mon_take_hit((int)c_ptr->cptr, dam);
	      if (i >= 0)
		{
		  (void) sprintf(out_val, "%s dies in a fit of agony.",
				 m_name);
		  msg_print(out_val);
		  prt_experience();
		}
	      else if (dam > 0)
		{
		  (void) sprintf (out_val, "%s screams in agony.", m_name);
		  msg_print (out_val);
		}
	    }
	  else if (panel_contains(y, x) && (py.flags.blind < 1))
	    {
	      print('*', y, x);
	      /* show the bolt */
	      put_qio();
	    }
	}
      oldy = y;
      oldx = x;
    }
  while (!flag);
}


/* Shoot a ball in a given direction.  Note that balls have an	*/
/* area affect.					      -RAK-   */
void fire_ball(typ, dir, y, x, dam_hp, descrip)
int typ, dir, y, x, dam_hp;
char *descrip;
{
  register int i, j;
  int dam, max_dis, thit, tkill, k, tmp;
  int oldy, oldx, dist, flag, harm_type;
  int32u weapon_type;
  int (*destroy)();
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  vtype out_val;

  thit	 = 0;
  tkill	 = 0;
  max_dis = 2;
  get_flags(typ, &weapon_type, &harm_type, &destroy);
  flag = FALSE;
  oldy = y;
  oldx = x;
  dist = 0;
  do
    {
      (void) mmove(dir, &y, &x);
      dist++;
      lite_spot(oldy, oldx);
      if (dist > OBJ_BOLT_RANGE)
	flag = TRUE;
      else
	{
	  c_ptr = &cave[y][x];
	  if ((c_ptr->fval >= MIN_CLOSED_SPACE) || (c_ptr->cptr > 1))
	    {
	      flag = TRUE;
	      if (c_ptr->fval >= MIN_CLOSED_SPACE)
		{
		  y = oldy;
		  x = oldx;
		}
	      /* The ball hits and explodes.		     */
	      /* The explosion.			     */
	      for (i = y-max_dis; i <= y+max_dis; i++)
		for (j = x-max_dis; j <= x+max_dis; j++)
		  if (in_bounds(i, j) && (distance(y, x, i, j) <= max_dis)
		      && los(y, x, i, j))
		    {
		      c_ptr = &cave[i][j];
		      if ((c_ptr->tptr != 0) &&
			  (*destroy)(&t_list[c_ptr->tptr]))
			(void) delete_object(i, j);
		      if (c_ptr->fval <= MAX_OPEN_SPACE)
			{
			  if (c_ptr->cptr > 1)
			    {
			      m_ptr = &m_list[c_ptr->cptr];
			      r_ptr = &c_list[m_ptr->mptr];

			      /* lite up creature if visible, temp
				 set pl so that update_mon works */
			      tmp = c_ptr->pl;
			      c_ptr->pl = TRUE;
			      update_mon((int)c_ptr->cptr);

			      thit++;
			      dam = dam_hp;
			      if (harm_type & r_ptr->cdefense)
				{
				  dam = dam*2;
				  if (m_ptr->ml)
				    c_recall[m_ptr->mptr].r_cdefense |=harm_type;
				}
			      else if (weapon_type & r_ptr->spells)
				{
				  dam = dam / 4;
				  if (m_ptr->ml)
				    c_recall[m_ptr->mptr].r_spells |=weapon_type;
				}
			      dam = (dam/(distance(i, j, y, x)+1));
			      k = mon_take_hit((int)c_ptr->cptr, dam);
			      if (k >= 0)
				tkill++;
			      c_ptr->pl = tmp;
			    }
			  else if (panel_contains(i, j) &&(py.flags.blind < 1))
			    print('*', i, j);
			}
		    }
	      /* show ball of whatever */
	      put_qio();

	      for (i = (y - 2); i <= (y + 2); i++)
		for (j = (x - 2); j <= (x + 2); j++)
		  if (in_bounds(i, j) && panel_contains(i, j) &&
		      (distance(y, x, i, j) <= max_dis))
		    lite_spot(i, j);

	      /* End  explosion.		     */
	      if (thit == 1)
		{
		  (void) sprintf(out_val,
				 "The %s envelops a creature!",
				 descrip);
		  msg_print(out_val);
		}
	      else if (thit > 1)
		{
		  (void) sprintf(out_val,
				 "The %s envelops several creatures!",
				 descrip);
		  msg_print(out_val);
		}
	      if (tkill == 1)
		msg_print("There is a scream of agony!");
	      else if (tkill > 1)
		msg_print("There are several screams of agony!");
	      if (tkill >= 0)
		prt_experience();
	      /* End ball hitting.		     */
	    }
	  else if (panel_contains(y, x) && (py.flags.blind < 1))
	    {
	      print('*', y, x);
	      /* show bolt */
	      put_qio();
	    }
	  oldy = y;
	  oldx = x;
	}
    }
  while (!flag);
}


/* Breath weapon works like a fire_ball, but affects the player. */
/* Note the area affect.			      -RAK-   */
void breath(typ, y, x, dam_hp, ddesc, monptr)
int typ, y, x, dam_hp;
char *ddesc;
int monptr;
{
  register int i, j;
  int dam, max_dis, harm_type;
  int32u weapon_type;
  int32u tmp, treas;
  int (*destroy)();
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
#ifdef ATARIST_MWC
  int32u holder;
#endif

  max_dis = 2;
  get_flags(typ, &weapon_type, &harm_type, &destroy);
  for (i = y-2; i <= y+2; i++)
    for (j = x-2; j <= x+2; j++)
      if (in_bounds(i, j) && (distance(y, x, i, j) <= max_dis)
	  && los(y, x, i, j))
	{
	  c_ptr = &cave[i][j];
	  if ((c_ptr->tptr != 0) &&
	      (*destroy)(&t_list[c_ptr->tptr]))
	    (void) delete_object(i, j);
	  if (c_ptr->fval <= MAX_OPEN_SPACE)
	    {
	      /* must test status bit, not py.flags.blind here, flag could have
		 been set by a previous monster, but the breath should still
		 be visible until the blindness takes effect */
	      if (panel_contains(i, j) && !(py.flags.status & PY_BLIND))
		print('*', i, j);
	      if (c_ptr->cptr > 1)
		{
		  m_ptr = &m_list[c_ptr->cptr];
		  r_ptr = &c_list[m_ptr->mptr];
		  dam = dam_hp;
		  if (harm_type & r_ptr->cdefense)
		    dam = dam*2;
		  else if (weapon_type & r_ptr->spells)
		    dam = (dam / 4);
		  dam = (dam/(distance(i, j, y, x)+1));
		  /* can not call mon_take_hit here, since player does not
		     get experience for kill */
		  m_ptr->hp = m_ptr->hp - dam;
		  m_ptr->csleep = 0;
		  if (m_ptr->hp < 0)
		    {
		      treas = monster_death((int)m_ptr->fy, (int)m_ptr->fx,
					    r_ptr->cmove);
		      if (m_ptr->ml)
			{
#ifdef ATARIST_MWC
			  holder = CM_TREASURE;
			  tmp = (c_recall[m_ptr->mptr].r_cmove & holder)
			    >> CM_TR_SHIFT;
			  if (tmp > ((treas & holder) >> CM_TR_SHIFT))
			    treas = (treas & ~holder)|(tmp << CM_TR_SHIFT);
			  c_recall[m_ptr->mptr].r_cmove = treas |
			    (c_recall[m_ptr->mptr].r_cmove & ~holder);
#else
			  tmp = (c_recall[m_ptr->mptr].r_cmove & CM_TREASURE)
			    >> CM_TR_SHIFT;
			  if (tmp > ((treas & CM_TREASURE) >> CM_TR_SHIFT))
			    treas = (treas & ~CM_TREASURE)|(tmp<<CM_TR_SHIFT);
			  c_recall[m_ptr->mptr].r_cmove = treas |
			    (c_recall[m_ptr->mptr].r_cmove & ~CM_TREASURE);
#endif
			}

		      /* It ate an already processed monster.Handle normally.*/
		      if (monptr < c_ptr->cptr)
			delete_monster((int) c_ptr->cptr);
		      /* If it eats this monster, an already processed monster
			 will take its place, causing all kinds of havoc.
			 Delay the kill a bit. */
		      else
			fix1_delete_monster((int) c_ptr->cptr);
		    }
		}
	      else if (c_ptr->cptr == 1)
		{
		  dam = (dam_hp/(distance(i, j, y, x)+1));
		  /* let's do at least one point of damage */
		  /* prevents randint(0) problem with poison_gas, also */
		  if (dam == 0)
		    dam = 1;
		  switch(typ)
		    {
		    case GF_LIGHTNING: light_dam(dam, ddesc); break;
		    case GF_POISON_GAS: poison_gas(dam, ddesc); break;
		    case GF_ACID: acid_dam(dam, ddesc); break;
		    case GF_FROST: cold_dam(dam, ddesc); break;
		    case GF_FIRE: fire_dam(dam, ddesc); break;
		    }
		}
	    }
	}
  /* show the ball of gas */
  put_qio();

  for (i = (y - 2); i <= (y + 2); i++)
    for (j = (x - 2); j <= (x + 2); j++)
      if (in_bounds(i, j) && panel_contains(i, j) &&
	  (distance(y, x, i, j) <= max_dis))
	lite_spot(i, j);
}


/* Recharge a wand, staff, or rod.  Sometimes the item breaks. -RAK-*/
int recharge(num)
register int num;
{
  int i, j, item_val;
  register int res;
  register inven_type *i_ptr;

  res = FALSE;
  if (!find_range(TV_STAFF, TV_WAND, &i, &j))
    msg_print("You have nothing to recharge.");
  else if (get_item(&item_val, "Recharge which item?", i, j, CNIL, CNIL))
    {
      i_ptr = &inventory[item_val];
      res = TRUE;
      /* recharge I = recharge(20) = 1/6 failure for empty 10th level wand */
      /* recharge II = recharge(60) = 1/10 failure for empty 10th level wand*/
      /* make it harder to recharge high level, and highly charged wands, note
	 that i can be negative, so check its value before trying to call
	 randint().  */
      i = num + 50 - (int)i_ptr->level - i_ptr->p1;
      if (i < 19)
	i = 1;	/* Automatic failure.  */
      else
	i = randint (i/10);

      if (i == 1)
	{
	  msg_print("There is a bright flash of light.");
	  inven_destroy(item_val);
	}
      else
	{
	  num = (num/(i_ptr->level+2)) + 1;
	  i_ptr->p1 += 2 + randint(num);
	  if (known2_p(i_ptr))
	    clear_known2(i_ptr);
	  clear_empty(i_ptr);
	}
    }
  return(res);
}


/* Increase or decrease a creatures hit points		-RAK-	*/
int hp_monster(dir, y, x, dam)
int dir, y, x, dam;
{
  register int i;
  int flag, dist, monster;
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  vtype out_val, m_name;

  monster = FALSE;
  flag = FALSE;
  dist = 0;
  do
    {
      (void) mmove(dir, &y, &x);
      dist++;
      c_ptr = &cave[y][x];
      if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE)
	flag = TRUE;
      else if (c_ptr->cptr > 1)
	{
	  flag = TRUE;
	  m_ptr = &m_list[c_ptr->cptr];
	  r_ptr = &c_list[m_ptr->mptr];
	  monster_name (m_name, m_ptr, r_ptr);
	  monster = TRUE;
	  i = mon_take_hit((int)c_ptr->cptr, dam);
	  if (i >= 0)
	    {
	      (void) sprintf(out_val, "%s dies in a fit of agony.", m_name);
	      msg_print(out_val);
	      prt_experience();
	    }
	  else if (dam > 0)
	    {
	      (void) sprintf(out_val, "%s screams in agony.", m_name);
	      msg_print(out_val);
	    }
	}
    }
  while (!flag);
  return(monster);
}


/* Drains life; note it must be living.		-RAK-	*/
int drain_life(dir, y, x)
int dir, y, x;
{
  register int i;
  int flag, dist, drain;
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  vtype out_val, m_name;

  drain = FALSE;
  flag = FALSE;
  dist = 0;
  do
    {
      (void) mmove(dir, &y, &x);
      dist++;
      c_ptr = &cave[y][x];
      if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE)
	flag = TRUE;
      else if (c_ptr->cptr > 1)
	{
	  flag = TRUE;
	  m_ptr = &m_list[c_ptr->cptr];
	  r_ptr = &c_list[m_ptr->mptr];
	  if ((r_ptr->cdefense & CD_UNDEAD) == 0)
	    {
	      drain = TRUE;
	      monster_name (m_name, m_ptr, r_ptr);
	      i = mon_take_hit((int)c_ptr->cptr, 75);
	      if (i >= 0)
		{
		  (void) sprintf(out_val, "%s dies in a fit of agony.",m_name);
		  msg_print(out_val);
		  prt_experience();
		}
	      else
		{
		  (void) sprintf(out_val, "%s screams in agony.", m_name);
		  msg_print(out_val);
		}
	    }
	  else
	    c_recall[m_ptr->mptr].r_cdefense |= CD_UNDEAD;
	}
    }
  while (!flag);
  return(drain);
}


/* Increase or decrease a creatures speed		-RAK-	*/
/* NOTE: cannot slow a winning creature (BALROG)		 */
int speed_monster(dir, y, x, spd)
int dir, y, x, spd;
{
  int flag, dist, speed;
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  vtype out_val, m_name;

  speed = FALSE;
  flag = FALSE;
  dist = 0;
  do
    {
      (void) mmove(dir, &y, &x);
      dist++;
      c_ptr = &cave[y][x];
      if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE)
	flag = TRUE;
      else if (c_ptr->cptr > 1)
	{
	  flag = TRUE;
	  m_ptr = &m_list[c_ptr->cptr];
	  r_ptr = &c_list[m_ptr->mptr];
	  monster_name (m_name, m_ptr, r_ptr);
	  if (spd > 0)
	    {
	      m_ptr->cspeed += spd;
	      m_ptr->csleep = 0;
	      (void) sprintf (out_val, "%s starts moving faster.", m_name);
	      msg_print (out_val);
	      speed = TRUE;
	    }
	  else if (randint(MAX_MONS_LEVEL) > r_ptr->level)
	    {
	      m_ptr->cspeed += spd;
	      m_ptr->csleep = 0;
	      (void) sprintf (out_val, "%s starts moving slower.", m_name);
	      msg_print (out_val);
	      speed = TRUE;
	    }
	  else
	    {
	      m_ptr->csleep = 0;
	      (void) sprintf(out_val, "%s is unaffected.", m_name);
	      msg_print(out_val);
	    }
	}
    }
  while (!flag);
  return(speed);
}


/* Confuse a creature					-RAK-	*/
int confuse_monster(dir, y, x)
int dir, y, x;
{
  int flag, dist, confuse;
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  vtype out_val, m_name;

  confuse = FALSE;
  flag = FALSE;
  dist = 0;
  do
    {
      (void) mmove(dir, &y, &x);
      dist++;
      c_ptr = &cave[y][x];
      if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE)
	flag = TRUE;
      else if (c_ptr->cptr > 1)
	{
	  m_ptr = &m_list[c_ptr->cptr];
	  r_ptr = &c_list[m_ptr->mptr];
	  monster_name (m_name, m_ptr, r_ptr);
	  flag = TRUE;
	  if ((randint(MAX_MONS_LEVEL) < r_ptr->level) ||
	      (CD_NO_SLEEP & r_ptr->cdefense))
	    {
	      if (m_ptr->ml && (r_ptr->cdefense & CD_NO_SLEEP))
		c_recall[m_ptr->mptr].r_cdefense |= CD_NO_SLEEP;
	      /* Monsters which resisted the attack should wake up.
		 Monsters with inane resistence ignore the attack.  */
	      if (! (CD_NO_SLEEP & r_ptr->cdefense))
		m_ptr->csleep = 0;
	      (void) sprintf(out_val, "%s is unaffected.", m_name);
	      msg_print(out_val);
	    }
	  else
	    {
	      m_ptr->confused = TRUE;
	      confuse = TRUE;
	      m_ptr->csleep = 0;
	      (void) sprintf(out_val, "%s appears confused.", m_name);
	      msg_print(out_val);
	    }
	}
    }
  while (!flag);
  return(confuse);
}


/* Sleep a creature.					-RAK-	*/
int sleep_monster(dir, y, x)
int dir, y, x;
{
  int flag, dist, sleep;
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  vtype out_val, m_name;

  sleep = FALSE;
  flag = FALSE;
  dist = 0;
  do
    {
      (void) mmove(dir, &y, &x);
      dist++;
      c_ptr = &cave[y][x];
      if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE)
	flag = TRUE;
      else if (c_ptr->cptr > 1)
	{
	  m_ptr = &m_list[c_ptr->cptr];
	  r_ptr = &c_list[m_ptr->mptr];
	  flag = TRUE;
	  monster_name (m_name, m_ptr, r_ptr);
	  if ((randint(MAX_MONS_LEVEL) < r_ptr->level) ||
	      (CD_NO_SLEEP & r_ptr->cdefense))
	    {
	      if (m_ptr->ml && (r_ptr->cdefense & CD_NO_SLEEP))
		c_recall[m_ptr->mptr].r_cdefense |= CD_NO_SLEEP;
	      (void) sprintf(out_val, "%s is unaffected.", m_name);
	      msg_print(out_val);
	    }
	  else
	    {
	      m_ptr->csleep = 500;
	      sleep = TRUE;
	      (void) sprintf(out_val, "%s falls asleep.", m_name);
	      msg_print(out_val);
	    }
	}
    }
  while (!flag);
  return(sleep);
}


/* Turn stone to mud, delete wall.			-RAK-	*/
int wall_to_mud(dir, y, x)
int dir, y, x;
{
  int i, wall, dist;
  bigvtype out_val, tmp_str;
  register int flag;
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  vtype m_name;

  wall = FALSE;
  flag = FALSE;
  dist = 0;
  do
    {
      (void) mmove(dir, &y, &x);
      dist++;
      c_ptr = &cave[y][x];
      /* note, this ray can move through walls as it turns them to mud */
      if (dist == OBJ_BOLT_RANGE)
	flag = TRUE;
      if ((c_ptr->fval >= MIN_CAVE_WALL) && (c_ptr->fval != BOUNDARY_WALL))
	{
	  flag = TRUE;
	  (void) twall(y, x, 1, 0);
	  if (test_light(y, x))
	    {
	      msg_print("The wall turns into mud.");
	      wall = TRUE;
	    }
	}
      else if ((c_ptr->tptr != 0) && (c_ptr->fval >= MIN_CLOSED_SPACE))
	{
	  flag = TRUE;
	  if (panel_contains(y, x) && test_light(y, x))
	    {
	      objdes(tmp_str, &t_list[c_ptr->tptr], FALSE);
	      (void) sprintf(out_val, "The %s turns into mud.", tmp_str);
	      msg_print(out_val);
	      wall = TRUE;
	    }
	  (void) delete_object(y, x);
	}
      if (c_ptr->cptr > 1)
	{
	  m_ptr = &m_list[c_ptr->cptr];
	  r_ptr = &c_list[m_ptr->mptr];
	  if (CD_STONE & r_ptr->cdefense)
	    {
	      monster_name (m_name, m_ptr, r_ptr);
	      i = mon_take_hit((int)c_ptr->cptr, 100);
	      /* Should get these messages even if the monster is not
		 visible.  */
	      if (i >= 0)
		{
		  c_recall[i].r_cdefense |= CD_STONE;
		  (void) sprintf(out_val, "%s dissolves!", m_name);
		  msg_print(out_val);
		  prt_experience(); /* print msg before calling prt_exp */
		}
	      else
		{
		  c_recall[m_ptr->mptr].r_cdefense |= CD_STONE;
		  (void) sprintf(out_val, "%s grunts in pain!",m_name);
		  msg_print(out_val);
		}
	      flag = TRUE;
	    }
	}
    }
  while (!flag);
  return(wall);
}


/* Destroy all traps and doors in a given direction	-RAK-	*/
int td_destroy2(dir, y, x)
int dir, y, x;
{
  register int destroy2, dist;
  register cave_type *c_ptr;
  register inven_type *t_ptr;

  destroy2 = FALSE;
  dist= 0;
  do
    {
      (void) mmove(dir, &y, &x);
      dist++;
      c_ptr = &cave[y][x];
      /* must move into first closed spot, as it might be a secret door */
      if (c_ptr->tptr != 0)
	{
	  t_ptr = &t_list[c_ptr->tptr];
	  if ((t_ptr->tval == TV_INVIS_TRAP) || (t_ptr->tval == TV_CLOSED_DOOR)
	      || (t_ptr->tval == TV_VIS_TRAP) || (t_ptr->tval == TV_OPEN_DOOR)
	      || (t_ptr->tval == TV_SECRET_DOOR))
	    {
	      if (delete_object(y, x))
		{
		  msg_print("There is a bright flash of light!");
		  destroy2 = TRUE;
		}
	    }
	  else if (t_ptr->tval == TV_CHEST)
	    {
	      msg_print("Click!");
	      t_ptr->flags &= ~(CH_TRAPPED|CH_LOCKED);
	      destroy2 = TRUE;
	      t_ptr->name2 = SN_UNLOCKED;
	      known2(t_ptr);
	    }
	}
    }
  while ((dist <= OBJ_BOLT_RANGE) || c_ptr->fval <= MAX_OPEN_SPACE);
  return(destroy2);
}


/* Polymorph a monster					-RAK-	*/
/* NOTE: cannot polymorph a winning creature (BALROG)		 */
int poly_monster(dir, y, x)
int dir, y, x;
{
  int dist, flag, poly;
  register cave_type *c_ptr;
  register creature_type *r_ptr;
  register monster_type *m_ptr;
  vtype out_val, m_name;

  poly = FALSE;
  flag = FALSE;
  dist = 0;
  do
    {
      (void) mmove(dir, &y, &x);
      dist++;
      c_ptr = &cave[y][x];
      if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE)
	flag = TRUE;
      else if (c_ptr->cptr > 1)
	{
	  m_ptr = &m_list[c_ptr->cptr];
	  r_ptr = &c_list[m_ptr->mptr];
	  if (randint(MAX_MONS_LEVEL) > r_ptr->level)
	    {
	      flag = TRUE;
	      delete_monster((int)c_ptr->cptr);
	      /* Place_monster() should always return TRUE here.  */
	      poly = place_monster(y, x,
				   randint(m_level[MAX_MONS_LEVEL]-m_level[0])
				   - 1 + m_level[0], FALSE);
	      /* don't test c_ptr->fm here, only pl/tl */
	      if (poly && panel_contains(y, x) && (c_ptr->tl || c_ptr->pl))
		poly = TRUE;
	    }
	  else
	    {
	      monster_name (m_name, m_ptr, r_ptr);
	      (void) sprintf(out_val, "%s is unaffected.", m_name);
	      msg_print(out_val);
	    }
	}
    }
  while (!flag);
  return(poly);
}


/* Create a wall.					-RAK-	*/
int build_wall(dir, y, x)
int dir, y, x;
{
  register int i;
  int build, damage, dist, flag;
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  vtype m_name, out_val;
#ifdef ATARIST_MWC
  int32u holder;
#endif

  build = FALSE;
  dist = 0;
  flag = FALSE;
  do
    {
      (void) mmove(dir, &y, &x);
      dist++;
      c_ptr = &cave[y][x];
      if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE)
	flag = TRUE;
      else
	{
	  if (c_ptr->tptr != 0)
	    (void) delete_object(y, x);
	  if (c_ptr->cptr > 1)
	    {
	      /* stop the wall building */
	      flag = TRUE;
	      m_ptr = &m_list[c_ptr->cptr];
	      r_ptr = &c_list[m_ptr->mptr];

#ifdef ATARIST_MWC
	      if (!(r_ptr->cmove & (holder = CM_PHASE)))
#else
	      if (!(r_ptr->cmove & CM_PHASE))
#endif
		{
		  /* monster does not move, can't escape the wall */
		  if (r_ptr->cmove & CM_ATTACK_ONLY)
		    damage = 3000; /* this will kill everything */
		  else
		    damage = damroll (4, 8);

		  monster_name (m_name, m_ptr, r_ptr);
		  (void) sprintf (out_val, "%s wails out in pain!", m_name);
		  msg_print (out_val);
		  i = mon_take_hit((int)c_ptr->cptr, damage);
		  if (i >= 0)
		    {
		      (void) sprintf (out_val, "%s is embedded in the rock.",
				      m_name);
		      msg_print (out_val);
		      prt_experience();
		    }
		}
	      else if (r_ptr->cchar == 'E' || r_ptr->cchar == 'X')
		{
		  /* must be an earth elemental or an earth spirit, or a Xorn
		     increase its hit points */
		  m_ptr->hp += damroll(4, 8);
		}
	    }
	  c_ptr->fval  = MAGMA_WALL;
	  c_ptr->fm = FALSE;
	  /* Permanently light this wall if it is lit by player's lamp.  */
	  c_ptr->pl = (c_ptr->tl || c_ptr->pl);
	  lite_spot(y, x);
	  i++;
	  build = TRUE;
	}
    }
  while (!flag);
  return(build);
}


/* Replicate a creature					-RAK-	*/
int clone_monster(dir, y, x)
int dir, y, x;
{
  register cave_type *c_ptr;
  register int dist, flag;

  dist = 0;
  flag = FALSE;
  do
    {
      (void) mmove(dir, &y, &x);
      dist++;
      c_ptr = &cave[y][x];
      if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE)
	flag = TRUE;
      else if (c_ptr->cptr > 1)
	{
	  m_list[c_ptr->cptr].csleep = 0;
	  /* monptr of 0 is safe here, since can't reach here from creatures */
	  return multiply_monster(y, x, (int)m_list[c_ptr->cptr].mptr, 0);
	}
    }
  while (!flag);
  return(FALSE);
}


/* Move the creature record to a new location		-RAK-	*/
void teleport_away(monptr, dis)
int monptr, dis;
{
  register int yn, xn, ctr;
  register monster_type *m_ptr;

  m_ptr = &m_list[monptr];
  ctr = 0;
  do
    {
      do
	{
	  yn = m_ptr->fy + (randint(2*dis+1) - (dis + 1));
	  xn = m_ptr->fx + (randint(2*dis+1) - (dis + 1));
	}
      while (!in_bounds(yn, xn));
      ctr++;
      if (ctr > 9)
	{
	  ctr = 0;
	  dis += 5;
	}
    }
  while ((cave[yn][xn].fval >= MIN_CLOSED_SPACE) || (cave[yn][xn].cptr != 0));
  move_rec((int)m_ptr->fy, (int)m_ptr->fx, yn, xn);
  lite_spot((int)m_ptr->fy, (int)m_ptr->fx);
  m_ptr->fy = yn;
  m_ptr->fx = xn;
  /* this is necessary, because the creature is not currently visible
     in its new position */
  m_ptr->ml = FALSE;
  m_ptr->cdis = distance (char_row, char_col, yn, xn);
  update_mon (monptr);
}


/* Teleport player to spell casting creature		-RAK-	*/
void teleport_to(ny, nx)
int ny, nx;
{
  int dis, ctr, y, x;
  register int i, j;
  register cave_type *c_ptr;

  dis = 1;
  ctr = 0;
  do
    {
      y = ny + (randint(2*dis+1) - (dis + 1));
      x = nx + (randint(2*dis+1) - (dis + 1));
      ctr++;
      if (ctr > 9)
	{
	  ctr = 0;
	  dis++;
	}
    }
  while (!in_bounds(y, x) || (cave[y][x].fval >= MIN_CLOSED_SPACE)
	 || (cave[y][x].cptr >= 2));
  move_rec(char_row, char_col, y, x);
  for (i = char_row-1; i <= char_row+1; i++)
    for (j = char_col-1; j <= char_col+1; j++)
      {
	c_ptr = &cave[i][j];
	c_ptr->tl = FALSE;
	lite_spot(i, j);
      }
  lite_spot(char_row, char_col);
  char_row = y;
  char_col = x;
  check_view();
  /* light creatures */
  creatures(FALSE);
}


/* Teleport all creatures in a given direction away	-RAK-	*/
int teleport_monster(dir, y, x)
int dir, y, x;
{
  register int flag, result, dist;
  register cave_type *c_ptr;

  flag = FALSE;
  result = FALSE;
  dist = 0;
  do
    {
      (void) mmove(dir, &y, &x);
      dist++;
      c_ptr = &cave[y][x];
      if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE)
	flag = TRUE;
      else if (c_ptr->cptr > 1)
	{
	  m_list[c_ptr->cptr].csleep = 0; /* wake it up */
	  teleport_away((int)c_ptr->cptr, MAX_SIGHT);
	  result = TRUE;
	}
    }
  while (!flag);
  return(result);
}


/* Delete all creatures within max_sight distance	-RAK-	*/
/* NOTE : Winning creatures cannot be genocided			 */
int mass_genocide()
{
  register int i, result;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
#ifdef ATARIST_MWC
  int32u holder;
#endif

  result = FALSE;
  for (i = mfptr - 1; i >= MIN_MONIX; i--)
    {
      m_ptr = &m_list[i];
      r_ptr = &c_list[m_ptr->mptr];
#ifdef ATARIST_MWC
      if ((m_ptr->cdis <= MAX_SIGHT) &&
	  ((r_ptr->cmove & (holder = CM_WIN)) == 0))
#else
      if ((m_ptr->cdis <= MAX_SIGHT) && ((r_ptr->cmove & CM_WIN) == 0))
#endif
	{
	  delete_monster(i);
	  result = TRUE;
	}
    }
  return(result);
}

/* Delete all creatures of a given type from level.	-RAK-	*/
/* This does not keep creatures of type from appearing later.	 */
/* NOTE : Winning creatures can not be genocided. */
int genocide()
{
  register int i, killed;
  char typ;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  vtype out_val;
#ifdef ATARIST_MWC
  int32u holder;
#endif

  killed = FALSE;
  if (get_com("Which type of creature do you wish exterminated?", &typ))
    for (i = mfptr - 1; i >= MIN_MONIX; i--)
      {
	m_ptr = &m_list[i];
	r_ptr = &c_list[m_ptr->mptr];
	if (typ == c_list[m_ptr->mptr].cchar)
#ifdef ATARIST_MWC
	  if ((r_ptr->cmove & (holder = CM_WIN)) == 0)
#else
	  if ((r_ptr->cmove & CM_WIN) == 0)
#endif
	    {
	      delete_monster(i);
	      killed = TRUE;
	    }
	  else
	    {
	      /* genocide is a powerful spell, so we will let the player
		 know the names of the creatures he did not destroy,
		 this message makes no sense otherwise */
	      (void) sprintf(out_val, "The %s is unaffected.", r_ptr->name);
	      msg_print(out_val);
	    }
      }
  return(killed);
}


/* Change speed of any creature .			-RAK-	*/
/* NOTE: cannot slow a winning creature (BALROG)		 */
int speed_monsters(spd)
int spd;
{
  register int i, speed;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  vtype out_val, m_name;

  speed = FALSE;
  for (i = mfptr - 1; i >= MIN_MONIX; i--)
    {
      m_ptr = &m_list[i];
      r_ptr = &c_list[m_ptr->mptr];
      monster_name (m_name, m_ptr, r_ptr);

      if ((m_ptr->cdis > MAX_SIGHT) ||
	  !los(char_row, char_col, (int)m_ptr->fy, (int)m_ptr->fx))
	/* do nothing */
	;
      else if (spd > 0)
	{
	  m_ptr->cspeed += spd;
	  m_ptr->csleep = 0;
	  if (m_ptr->ml)
	    {
	      speed = TRUE;
	      (void) sprintf (out_val, "%s starts moving faster.", m_name);
	      msg_print (out_val);
	    }
	}
      else if (randint(MAX_MONS_LEVEL) > r_ptr->level)
	{
	  m_ptr->cspeed += spd;
	  m_ptr->csleep = 0;
	  if (m_ptr->ml)
	    {
	      (void) sprintf (out_val, "%s starts moving slower.", m_name);
	      msg_print (out_val);
	      speed = TRUE;
	    }
	}
      else if (m_ptr->ml)
	{
	  m_ptr->csleep = 0;
	  (void) sprintf(out_val, "%s is unaffected.", m_name);
	  msg_print(out_val);
	}
    }
  return(speed);
}


/* Sleep any creature .		-RAK-	*/
int sleep_monsters2()
{
  register int i, sleep;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  vtype out_val, m_name;

  sleep = FALSE;
  for (i = mfptr - 1; i >= MIN_MONIX; i--)
    {
      m_ptr = &m_list[i];
      r_ptr = &c_list[m_ptr->mptr];
      monster_name (m_name, m_ptr, r_ptr);
      if ((m_ptr->cdis > MAX_SIGHT) || 
	  !los(char_row, char_col, (int)m_ptr->fy, (int)m_ptr->fx))
	/* do nothing */
	;
      else if ((randint(MAX_MONS_LEVEL) < r_ptr->level) ||
	  (CD_NO_SLEEP & r_ptr->cdefense))
	{
	  if (m_ptr->ml)
	    {
	      if (r_ptr->cdefense & CD_NO_SLEEP)
		c_recall[m_ptr->mptr].r_cdefense |= CD_NO_SLEEP;
	      (void) sprintf(out_val, "%s is unaffected.", m_name);
	      msg_print(out_val);
	    }
	}
      else
	{
	  m_ptr->csleep = 500;
	  if (m_ptr->ml)
	    {
	      (void) sprintf(out_val, "%s falls asleep.", m_name);
	      msg_print(out_val);
	      sleep = TRUE;
	    }
	}
    }
  return(sleep);
}


/* Polymorph any creature that player can see.	-RAK-	*/
/* NOTE: cannot polymorph a winning creature (BALROG)		 */
int mass_poly()
{
  register int i;
  int y, x, mass;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
#ifdef ATARIST_MWC
  int32u holder;
#endif

  mass = FALSE;
  for (i = mfptr - 1; i >= MIN_MONIX; i--)
    {
      m_ptr = &m_list[i];
      if (m_ptr->cdis <= MAX_SIGHT)
	{
	  r_ptr = &c_list[m_ptr->mptr];
#ifdef ATARIST_MWC
	  if ((r_ptr->cmove & (holder = CM_WIN)) == 0)
#else
	  if ((r_ptr->cmove & CM_WIN) == 0)
#endif
	    {
	      y = m_ptr->fy;
	      x = m_ptr->fx;
	      delete_monster(i);
	      /* Place_monster() should always return TRUE here.  */
	      mass = place_monster(y, x,
				   randint(m_level[MAX_MONS_LEVEL]-m_level[0])
				   - 1 + m_level[0], FALSE);
	    }
	}
    }
  return(mass);
}


/* Display evil creatures on current panel		-RAK-	*/
int detect_evil()
{
  register int i, flag;
  register monster_type *m_ptr;

  flag = FALSE;
  for (i = mfptr - 1; i >= MIN_MONIX; i--)
    {
      m_ptr = &m_list[i];
      if (panel_contains((int)m_ptr->fy, (int)m_ptr->fx) &&
	  (CD_EVIL & c_list[m_ptr->mptr].cdefense))
	{
	  m_ptr->ml = TRUE;
	  /* works correctly even if hallucinating */
	  print((char)c_list[m_ptr->mptr].cchar, (int)m_ptr->fy,
		(int)m_ptr->fx);
	  flag = TRUE;
	}
    }
  if (flag)
    {
      msg_print("You sense the presence of evil!");
      msg_print(CNIL);
      /* must unlight every monster just lighted */
      creatures(FALSE);
    }
  return(flag);
}


/* Change players hit points in some manner		-RAK-	*/
int hp_player(num)
int num;
{
  register int res;
  register struct misc *m_ptr;

  res = FALSE;
  m_ptr = &py.misc;
  if (m_ptr->chp < m_ptr->mhp)
    {
      m_ptr->chp += num;
      if (m_ptr->chp > m_ptr->mhp)
	{
	  m_ptr->chp = m_ptr->mhp;
	  m_ptr->chp_frac = 0;
	}
      prt_chp();

      num = num / 5;
      if (num < 3) {
	if (num == 0) msg_print("You feel a little better.");
	else	      msg_print("You feel better.");
      } else {
	if (num < 7) msg_print("You feel much better.");
	else	     msg_print("You feel very good.");
      }
      res = TRUE;
    }
  return(res);
}


/* Cure players confusion				-RAK-	*/
int cure_confusion()
{
  register int cure;
  register struct flags *f_ptr;

  cure = FALSE;
  f_ptr = &py.flags;
  if (f_ptr->confused > 1)
    {
      f_ptr->confused = 1;
      cure = TRUE;
    }
  return(cure);
}


/* Cure players blindness				-RAK-	*/
int cure_blindness()
{
  register int cure;
  register struct flags *f_ptr;

  cure = FALSE;
  f_ptr = &py.flags;
  if (f_ptr->blind > 1)
    {
      f_ptr->blind = 1;
      cure = TRUE;
    }
  return(cure);
}


/* Cure poisoning					-RAK-	*/
int cure_poison()
{
  register int cure;
  register struct flags *f_ptr;

  cure = FALSE;
  f_ptr = &py.flags;
  if (f_ptr->poisoned > 1)
    {
      f_ptr->poisoned = 1;
      cure = TRUE;
    }
  return(cure);
}


/* Cure the players fear				-RAK-	*/
int remove_fear()
{
  register int result;
  register struct flags *f_ptr;

  result = FALSE;
  f_ptr = &py.flags;
  if (f_ptr->afraid > 1)
    {
      f_ptr->afraid = 1;
      result = TRUE;
    }
  return(result);
}


/* This is a fun one.  In a given block, pick some walls and	*/
/* turn them into open spots.  Pick some open spots and turn	 */
/* them into walls.  An "Earthquake" effect.	       -RAK-   */
void earthquake()
{
  register int i, j;
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  int damage, tmp;
  vtype out_val, m_name;
#ifdef ATARIST_MWC
  int32u holder;
#endif

  for (i = char_row-8; i <= char_row+8; i++)
    for (j = char_col-8; j <= char_col+8; j++)
      if (((i != char_row) || (j != char_col)) &&
	  in_bounds(i, j) && (randint(8) == 1))
	{
	  c_ptr = &cave[i][j];
	  if (c_ptr->tptr != 0)
	    (void) delete_object(i, j);
	  if (c_ptr->cptr > 1)
	    {
	      m_ptr = &m_list[c_ptr->cptr];
	      r_ptr = &c_list[m_ptr->mptr];

#ifdef ATARIST_MWC
	      if (!(r_ptr->cmove & (holder = CM_PHASE)))
#else
	      if (!(r_ptr->cmove & CM_PHASE))
#endif
		{
		  if(r_ptr->cmove & CM_ATTACK_ONLY)
		    damage = 3000; /* this will kill everything */
		  else
		    damage = damroll (4, 8);

		  monster_name (m_name, m_ptr, r_ptr);
		  (void) sprintf (out_val, "%s wails out in pain!", m_name);
		  msg_print (out_val);
		  i = mon_take_hit((int)c_ptr->cptr, damage);
		  if (i >= 0)
		    {
		      (void) sprintf (out_val, "%s is embedded in the rock.",
				      m_name);
		      msg_print (out_val);
		      prt_experience();
		    }
		}
	      else if (r_ptr->cchar == 'E' || r_ptr->cchar == 'X')
		{
		  /* must be an earth elemental or an earth spirit, or a Xorn
		     increase its hit points */
		  m_ptr->hp += damroll(4, 8);
		}
	    }

	  if ((c_ptr->fval >= MIN_CAVE_WALL) && (c_ptr->fval != BOUNDARY_WALL))
	    {
	      c_ptr->fval  = CORR_FLOOR;
	      c_ptr->pl = FALSE;
	      c_ptr->fm = FALSE;
	    }
	  else if (c_ptr->fval <= MAX_CAVE_FLOOR)
	    {
	      tmp = randint(10);
	      if (tmp < 6)
		c_ptr->fval  = QUARTZ_WALL;
	      else if (tmp < 9)
		c_ptr->fval  = MAGMA_WALL;
	      else
		c_ptr->fval  = GRANITE_WALL;

	      c_ptr->fm = FALSE;
	    }
	  lite_spot(i, j);
	}
}


/* Evil creatures don't like this.		       -RAK-   */
int protect_evil()
{
  register int res;
  register struct flags *f_ptr;

  f_ptr = &py.flags;
  if (f_ptr->protevil == 0)
    res = TRUE;
  else
    res = FALSE;
  f_ptr->protevil += randint(25) + 3*py.misc.lev;
  return(res);
}


/* Create some high quality mush for the player.	-RAK-	*/
void create_food()
{
  register cave_type *c_ptr;

  c_ptr = &cave[char_row][char_col];
  if (c_ptr->tptr != 0)
    {
      /* take no action here, don't want to destroy object under player */
      msg_print ("There is already an object under you.");
      /* set free_turn_flag so that scroll/spell points won't be used */
      free_turn_flag = TRUE;
    }
  else
    {
      place_object(char_row, char_col);
      invcopy(&t_list[c_ptr->tptr], OBJ_MUSH);
    }
}


/* Attempts to destroy a type of creature.  Success depends on	*/
/* the creatures level VS. the player's level		 -RAK-	 */
int dispel_creature(cflag, damage)
int cflag;
int damage;
{
  register int i;
  int k, dispel;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  vtype out_val, m_name;

  dispel = FALSE;
  for (i = mfptr - 1; i >= MIN_MONIX; i--)
    {
      m_ptr = &m_list[i];
      if ((m_ptr->cdis <= MAX_SIGHT) &&
	  (cflag & c_list[m_ptr->mptr].cdefense) &&
	  los(char_row, char_col, (int)m_ptr->fy, (int)m_ptr->fx))
	{
	  r_ptr = &c_list[m_ptr->mptr];
	  c_recall[m_ptr->mptr].r_cdefense |= cflag;
	  monster_name (m_name, m_ptr, r_ptr);
	  k = mon_take_hit (i, randint(damage));
	  /* Should get these messages even if the monster is not
	     visible.  */
	  if (k >= 0)
	    (void) sprintf(out_val, "%s dissolves!", m_name);
	  else
	    (void) sprintf(out_val, "%s shudders.", m_name);
	  msg_print(out_val);
	  dispel = TRUE;
	  if (k >= 0)
	    prt_experience();
	}
    }
  return(dispel);
}


/* Attempt to turn (confuse) undead creatures.	-RAK-	*/
int turn_undead()
{
  register int i, turn_und;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  vtype out_val, m_name;

  turn_und = FALSE;
  for (i = mfptr - 1; i >= MIN_MONIX; i--)
    {
      m_ptr = &m_list[i];
      r_ptr = &c_list[m_ptr->mptr];
      if ((m_ptr->cdis <= MAX_SIGHT) &&
	  (CD_UNDEAD & r_ptr->cdefense) &&
	  (los(char_row, char_col, (int)m_ptr->fy, (int)m_ptr->fx)))
	{
	  monster_name (m_name, m_ptr, r_ptr);
	  if (((py.misc.lev+1) > r_ptr->level) ||
	      (randint(5) == 1))
	    {
	      if (m_ptr->ml)
		{
		  (void) sprintf(out_val, "%s runs frantically!", m_name);
		  msg_print(out_val);
		  turn_und = TRUE;
		  c_recall[m_ptr->mptr].r_cdefense |= CD_UNDEAD;
		}
	      m_ptr->confused = TRUE;
	    }
	  else if (m_ptr->ml)
	    {
	      (void) sprintf(out_val, "%s is unaffected.", m_name);
	      msg_print(out_val);
	    }
	}
    }
  return(turn_und);
}


/* Leave a glyph of warding. Creatures will not pass over! -RAK-*/
void warding_glyph()
{
  register int i;
  register cave_type *c_ptr;

  c_ptr = &cave[char_row][char_col];
  if (c_ptr->tptr == 0)
    {
      i = popt();
      c_ptr->tptr = i;
      invcopy(&t_list[i], OBJ_SCARE_MON);
    }
}


/* Lose a strength point.				-RAK-	*/
void lose_str()
{
  if (!py.flags.sustain_str)
    {
      (void) dec_stat (A_STR);
      msg_print("You feel very sick.");
    }
  else
    msg_print("You feel sick for a moment,  it passes.");
}


/* Lose an intelligence point.				-RAK-	*/
void lose_int()
{
  if (!py.flags.sustain_int)
    {
      (void) dec_stat(A_INT);
      msg_print("You become very dizzy.");
    }
  else
    msg_print("You become dizzy for a moment,  it passes.");
}


/* Lose a wisdom point.					-RAK-	*/
void lose_wis()
{
  if (!py.flags.sustain_wis)
    {
      (void) dec_stat(A_WIS);
      msg_print("You feel very naive.");
    }
  else
    msg_print("You feel naive for a moment,  it passes.");
}


/* Lose a dexterity point.				-RAK-	*/
void lose_dex()
{
  if (!py.flags.sustain_dex)
    {
      (void) dec_stat(A_DEX);
      msg_print("You feel very sore.");
    }
  else
    msg_print("You feel sore for a moment,  it passes.");
}


/* Lose a constitution point.				-RAK-	*/
void lose_con()
{
  if (!py.flags.sustain_con)
    {
      (void) dec_stat(A_CON);
      msg_print("You feel very sick.");
    }
  else
    msg_print("You feel sick for a moment,  it passes.");
}


/* Lose a charisma point.				-RAK-	*/
void lose_chr()
{
  if (!py.flags.sustain_chr)
    {
      (void) dec_stat(A_CHR);
      msg_print("Your skin starts to itch.");
    }
  else
    msg_print("Your skin starts to itch, but feels better now.");
}


/* Lose experience					-RAK-	*/
void lose_exp(amount)
int32 amount;
{
  register int i;
  register struct misc *m_ptr;
  register class_type *c_ptr;

  m_ptr = &py.misc;
  if (amount > m_ptr->exp)
    m_ptr->exp = 0;
  else
    m_ptr->exp -= amount;
  prt_experience();

  i = 0;
  while ((player_exp[i] * m_ptr->expfact / 100) <= m_ptr->exp)
    i++;
  /* increment i once more, because level 1 exp is stored in player_exp[0] */
  i++;

  if (m_ptr->lev != i)
    {
      m_ptr->lev = i;

      calc_hitpoints();
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
      prt_level();
      prt_title();
    }
}


/* Slow Poison						-RAK-	*/
int slow_poison()
{
  register int slow;
  register struct flags *f_ptr;

  slow = FALSE;
  f_ptr = &py.flags;
  if (f_ptr->poisoned > 0)
    {
      f_ptr->poisoned = f_ptr->poisoned / 2;
      if (f_ptr->poisoned < 1)	f_ptr->poisoned = 1;
      slow = TRUE;
      msg_print("The effect of the poison has been reduced.");
    }
  return(slow);
}


/* Bless						-RAK-	*/
void bless(amount)
int amount;
{
  py.flags.blessed += amount;
}


/* Detect Invisible for period of time			-RAK-	*/
void detect_inv2(amount)
int amount;
{
  py.flags.detect_inv += amount;
}


static void replace_spot(y, x, typ)
int y, x, typ;
{
  register cave_type *c_ptr;

  c_ptr = &cave[y][x];
  switch(typ)
    {
    case 1: case 2: case 3:
      c_ptr->fval  = CORR_FLOOR;
      break;
    case 4: case 7: case 10:
      c_ptr->fval  = GRANITE_WALL;
      break;
    case 5: case 8: case 11:
      c_ptr->fval  = MAGMA_WALL;
      break;
    case 6: case 9: case 12:
      c_ptr->fval  = QUARTZ_WALL;
      break;
    }
  c_ptr->pl = FALSE;
  c_ptr->fm = FALSE;
  c_ptr->lr = FALSE;  /* this is no longer part of a room */
  if (c_ptr->tptr != 0)
    (void) delete_object(y, x);
  if (c_ptr->cptr > 1)
    delete_monster((int)c_ptr->cptr);
}


/* The spell of destruction.				-RAK-	*/
/* NOTE : Winning creatures that are deleted will be considered	 */
/*	  as teleporting to another level.  This will NOT win the*/
/*	  game.						       */
void destroy_area(y, x)
register int y, x;
{
  register int i, j, k;

  if (dun_level > 0)
    {
      for (i = (y-15); i <= (y+15); i++)
	for (j = (x-15); j <= (x+15); j++)
	  if (in_bounds(i, j) && (cave[i][j].fval != BOUNDARY_WALL))
	    {
	      k = distance(i, j, y, x);
	      if (k == 0) /* clear player's spot, but don't put wall there */
		replace_spot(i, j, 1);
	      else if (k < 13)
		replace_spot(i, j, randint(6));
	      else if (k < 16)
		replace_spot(i, j, randint(9));
	    }
    }
  msg_print("There is a searing blast of light!");
  py.flags.blind += 10 + randint(10);
}


/* Enchants a plus onto an item.			-RAK-	*/
int enchant(plusses, limit)
int16 *plusses;
int16 limit; /* maximum bonus allowed; usually 10, but weapon's maximum damage
		when enchanting melee weapons to damage */
{
  register int chance, res;
  
  if (limit <= 0) /* avoid randint(0) call */
    return(FALSE);
  chance = 0;
  res = FALSE;
  if (*plusses > 0)
    {
      chance = *plusses;
      if (randint(100) == 1) /* very rarely allow enchantment over limit */
	chance = randint(chance) - 1;
    }
  if (randint(limit) > chance)
    {
      *plusses += 1;
      res = TRUE;
    }
  return(res);
}


/* Removes curses from items in inventory		-RAK-	*/
int remove_curse()
{
  register int i, result;
  register inven_type *i_ptr;
#ifdef ATARIST_MWC
  int32u holder = TR_CURSED;
#endif

  result = FALSE;
  for (i = INVEN_WIELD; i <= INVEN_OUTER; i++)
    {
      i_ptr = &inventory[i];
#ifdef ATARIST_MWC
      if (holder & i_ptr->flags)
#else
      if (TR_CURSED & i_ptr->flags)
#endif
	{
#ifdef ATARIST_MWC
	  i_ptr->flags &= ~holder;
#else
	  i_ptr->flags &= ~TR_CURSED;
#endif
	  calc_bonuses();
	  result = TRUE;
	}
    }
  return(result);
}


/* Restores any drained experience			-RAK-	*/
int restore_level()
{
  register int restore;
  register struct misc *m_ptr;

  restore = FALSE;
  m_ptr = &py.misc;
  if (m_ptr->max_exp > m_ptr->exp)
    {
      restore = TRUE;
      msg_print("You feel your life energies returning.");
      /* this while loop is not redundant, ptr_exp may reduce the exp level */
      while (m_ptr->exp < m_ptr->max_exp)
	{
	  m_ptr->exp = m_ptr->max_exp;
	  prt_experience();
	}
    }
  return(restore);
}
