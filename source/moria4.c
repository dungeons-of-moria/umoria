/* source/moria4.c: misc code, mainly to handle player commands

   Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#ifdef __TURBOC__
#include	<stdio.h>
#include	<stdlib.h>
#endif

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

#include <ctype.h>


#if defined(LINT_ARGS)
static int look_ray(int, int, int);
static int look_see(int, int, int *);
static void inven_throw(int, struct inven_type *);
static void facts(struct inven_type *, int *, int *, int *, int *);
static void drop_throw(int, int, struct inven_type *);
static void py_bash(int, int);
#else
static int look_ray();
static int look_see();
#endif


/* Tunnels through rubble and walls			-RAK-	*/
/* Must take into account: secret doors,  special tools		  */
void tunnel(dir)
int dir;
{
  register int i, tabil;
  register cave_type *c_ptr;
  register inven_type *i_ptr;
  int y, x;
  monster_type *m_ptr;
  vtype out_val, m_name;
#ifdef ATARIST_MWC
  int32u holder;
#endif

  if ((py.flags.confused > 0) &&    /* Confused?	     */
      (randint(4) > 1))		    /* 75% random movement   */
    dir = randint(9);
  y = char_row;
  x = char_col;
  (void) mmove(dir, &y, &x);

  c_ptr = &cave[y][x];
  /* Compute the digging ability of player; based on	   */
  /* strength, and type of tool used			   */
  tabil = py.stats.use_stat[A_STR];
  i_ptr = &inventory[INVEN_WIELD];

  /* Don't let the player tunnel somewhere illegal, this is necessary to
     prevent the player from getting a free attack by trying to tunnel
     somewhere where it has no effect.  */
  if (c_ptr->fval < MIN_CAVE_WALL
      && (c_ptr->tptr == 0 || (t_list[c_ptr->tptr].tval != TV_RUBBLE
			       && t_list[c_ptr->tptr].tval != TV_SECRET_DOOR)))
    {
      if (c_ptr->tptr == 0)
	{
	  msg_print ("Tunnel through what?  Empty air?!?");
	  free_turn_flag = TRUE;
	}
      else
	{
	  msg_print("You can't tunnel through that.");
	  free_turn_flag = TRUE;
	}
      return;
    }

  if (c_ptr->cptr > 1)
    {
      m_ptr = &m_list[c_ptr->cptr];
      if (m_ptr->ml)
	(void) sprintf (m_name, "The %s", c_list[m_ptr->mptr].name);
      else
	(void) strcpy (m_name, "Something");
      (void) sprintf(out_val, "%s is in your way!", m_name);
      msg_print(out_val);

      /* let the player attack the creature */
      if (py.flags.afraid < 1)
	py_attack(y, x);
      else
	msg_print("You are too afraid!");
    }
  else if (i_ptr->tval != TV_NOTHING)
    {
#ifdef ATARIST_MWC
      if ((holder = TR_TUNNEL) & i_ptr->flags)
#else
      if (TR_TUNNEL & i_ptr->flags)
#endif
	tabil += 25 + i_ptr->p1*50;
      else
	{
	  tabil += (i_ptr->damage[0]*i_ptr->damage[1]) + i_ptr->tohit
	    + i_ptr->todam;
	  /* divide by two so that digging without shovel isn't too easy */
	  tabil >>= 1;
	}

      /* If this weapon is too heavy for the player to wield properly, then
	 also make it harder to dig with it.  */

      if (weapon_heavy)
	{
	  tabil += (py.stats.use_stat[A_STR] * 15) - i_ptr->weight;
	  if (tabil < 0)
	    tabil = 0;
	}

      /* Regular walls; Granite, magma intrusion, quartz vein  */
      /* Don't forget the boundary walls, made of titanium (255)*/
      switch(c_ptr->fval)
	{
	case GRANITE_WALL:
	  i = randint(1200) + 80;
	  if (twall(y, x, tabil, i))
	    msg_print("You have finished the tunnel.");
	  else
	    count_msg_print("You tunnel into the granite wall.");
	  break;
	case MAGMA_WALL:
	  i = randint(600) + 10;
	  if (twall(y, x, tabil, i))
	    msg_print("You have finished the tunnel.");
	  else
	    count_msg_print("You tunnel into the magma intrusion.");
	  break;
	case QUARTZ_WALL:
	  i = randint(400) + 10;
	  if (twall(y, x, tabil, i))
	    msg_print("You have finished the tunnel.");
	  else
	    count_msg_print("You tunnel into the quartz vein.");
	  break;
	case BOUNDARY_WALL:
	  msg_print("This seems to be permanent rock.");
	  break;
	default:
	  /* Is there an object in the way?  (Rubble and secret doors)*/
	  if (c_ptr->tptr != 0)
	    {
	      /* Rubble.     */
	      if (t_list[c_ptr->tptr].tval == TV_RUBBLE)
		{
		  if (tabil > randint(180))
		    {
		      (void) delete_object(y, x);
		      msg_print("You have removed the rubble.");
		      if (randint(10) == 1)
			{
			  place_object(y, x);
			  if (test_light(y, x))
			    msg_print("You have found something!");
			}
		      lite_spot(y, x);
		    }
		  else
		    count_msg_print("You dig in the rubble.");
		}
	      /* Secret doors.*/
	      else if (t_list[c_ptr->tptr].tval == TV_SECRET_DOOR)
		{
		  count_msg_print("You tunnel into the granite wall.");
		  search(char_row, char_col, py.misc.srh);
		}
	      else
		abort ();
	    }
	  else
	    abort ();
	  break;
	}
    }
  else
    msg_print("You dig with your hands, making no progress.");
}


/* Disarms a trap					-RAK-	*/
void disarm_trap()
{
  int y, x, level, tmp, dir, no_disarm;
  register int tot, i;
  register cave_type *c_ptr;
  register inven_type *i_ptr;
  monster_type *m_ptr;
  vtype m_name, out_val;

  y = char_row;
  x = char_col;
  if (get_dir(CNIL, &dir))
    {
      (void) mmove(dir, &y, &x);
      c_ptr = &cave[y][x];
      no_disarm = FALSE;
      if (c_ptr->cptr > 1 && c_ptr->tptr != 0 &&
	  (t_list[c_ptr->tptr].tval == TV_VIS_TRAP
	   || t_list[c_ptr->tptr].tval == TV_CHEST))
	{
	  m_ptr = &m_list[c_ptr->cptr];
	  if (m_ptr->ml)
	    (void) sprintf (m_name, "The %s", c_list[m_ptr->mptr].name);
	  else
	    (void) strcpy (m_name, "Something");
	  (void) sprintf(out_val, "%s is in your way!", m_name);
	  msg_print(out_val);
	}
      else if (c_ptr->tptr != 0)
	{
	  tot = py.misc.disarm + 2*todis_adj() + stat_adj(A_INT)
	    + (class_level_adj[py.misc.pclass][CLA_DISARM] * py.misc.lev / 3);
	  if ((py.flags.blind > 0) || (no_light()))
	    tot = tot / 10;
	  if (py.flags.confused > 0)
	    tot = tot / 10;
	  if (py.flags.image > 0)
	    tot = tot / 10;
	  i_ptr = &t_list[c_ptr->tptr];
	  i = i_ptr->tval;
	  level = i_ptr->level;
	  if (i == TV_VIS_TRAP)		    /* Floor trap    */
	    {
	      if ((tot + 100 - level) > randint(100))
		{
		  msg_print("You have disarmed the trap.");
		  py.misc.exp += i_ptr->p1;
		  (void) delete_object(y, x);
		  /* make sure we move onto the trap even if confused */
		  tmp = py.flags.confused;
		  py.flags.confused = 0;
		  move_char(dir, FALSE);
		  py.flags.confused = tmp;
		  prt_experience();
		}
	      /* avoid randint(0) call */
	      else if ((tot > 5) && (randint(tot) > 5))
		count_msg_print("You failed to disarm the trap.");
	      else
		{
		  msg_print("You set the trap off!");
		  /* make sure we move onto the trap even if confused */
		  tmp = py.flags.confused;
		  py.flags.confused = 0;
		  move_char(dir, FALSE);
		  py.flags.confused += tmp;
		}
	    }
	  else if (i == TV_CHEST)
	    {
	      if (!known2_p(i_ptr))
		{
		  msg_print("I don't see a trap.");
		  free_turn_flag = TRUE;
		}
	      else if (CH_TRAPPED & i_ptr->flags)
		{
		  if ((tot - level) > randint(100))
		    {
		      i_ptr->flags &= ~CH_TRAPPED;
		      if (CH_LOCKED & i_ptr->flags)
			i_ptr->name2 = SN_LOCKED;
		      else
			i_ptr->name2 = SN_DISARMED;
		      msg_print("You have disarmed the chest.");
		      known2(i_ptr);
		      py.misc.exp += level;
		      prt_experience();
		    }
		  else if ((tot > 5) && (randint(tot) > 5))
		    count_msg_print("You failed to disarm the chest.");
		  else
		    {
		      msg_print("You set a trap off!");
		      known2(i_ptr);
		      chest_trap(y, x);
		    }
		}
	      else
		{
		  msg_print("The chest was not trapped.");
		  free_turn_flag = TRUE;
		}
	    }
	  else
	    no_disarm = TRUE;
	}
      else
	no_disarm = TRUE;

      if (no_disarm)
	{
	  msg_print("I do not see anything to disarm there.");
	  free_turn_flag = TRUE;
	}
    }
}


/* An enhanced look, with peripheral vision. Looking all 8	-CJS-
   directions will see everything which ought to be visible. Can
   specify direction 5, which looks in all directions.

   For the purpose of hindering vision, each place is regarded as
   a diamond just touching its four immediate neighbours. A
   diamond is opaque if it is a wall, or shut door, or something
   like that. A place is visible if any part of its diamond is
   visible: i.e. there is a line from the view point to part of
   the diamond which does not pass through any opaque diamonds.

   Consider the following situation:

     @....			    X	X   X	X   X
     .##..			   / \ / \ / \ / \ / \
     .....			  X @ X . X . X 1 X . X
				   \ / \ / \ / \ / \ /
				    X	X   X	X   X
	   Expanded view, with	   / \ / \ / \ / \ / \
	   diamonds inscribed	  X . X # X # X 2 X . X
	   about each point,	   \ / \ / \ / \ / \ /
	   and some locations	    X	X   X	X   X
	   numbered.		   / \ / \ / \ / \ / \
				  X . X . X . X 3 X 4 X
				   \ / \ / \ / \ / \ /
				    X	X   X	X   X
	- Location 1 is fully visible.
	- Location 2 is visible, even though partially obscured.
	- Location 3 is invisible, but if either # were
	  transparent, it would be visible.
	- Location 4 is completely obscured by a single #.

   The function which does the work is look_ray. It sets up its
   own co-ordinate frame (global variables map back to the
   dungeon frame) and looks for everything between two angles
   specified from a central line. It is recursive, and each call
   looks at stuff visible along a line parallel to the center
   line, and a set distance away from it. A diagonal look uses
   more extreme peripheral vision from the closest horizontal and
   vertical directions; horizontal or vertical looks take a call
   for each side of the central line. */

/* Globally accessed variables: gl_nseen counts the number of places where
   something is seen. gl_rock indicates a look for rock or objects.

   The others map co-ords in the ray frame to dungeon co-ords.

   dungeon y = char_row	 + gl_fyx * (ray x)  + gl_fyy * (ray y)
   dungeon x = char_col	 + gl_fxx * (ray x)  + gl_fxy * (ray y) */
static int gl_fxx, gl_fxy, gl_fyx, gl_fyy;
static int gl_nseen, gl_noquery;
static int gl_rock;
/* Intended to be indexed by dir/2, since is only relevant to horizontal or
   vertical directions. */
static int set_fxy[] = { 0,  1,	 0,  0, -1 };
static int set_fxx[] = { 0,  0, -1,  1,	 0 };
static int set_fyy[] = { 0,  0,	 1, -1,	 0 };
static int set_fyx[] = { 0,  1,	 0,  0, -1 };
/* Map diagonal-dir/2 to a normal-dir/2. */
static int map_diag1[] = { 1, 3, 0, 2, 4 };
static int map_diag2[] = { 2, 1, 0, 4, 3 };

#define GRADF	10000	/* Any sufficiently big number will do */

/* Look at what we can see. This is a free move.

   Prompts for a direction, and then looks at every object in
   turn within a cone of vision in that direction. For each
   object, the cursor is moved over the object, a description is
   given, and we wait for the user to type something. Typing
   ESCAPE will abort the entire look.

   Looks first at real objects and monsters, and looks at rock
   types only after all other things have been seen.  Only looks at rock
   types if the highlight_seams option is set. */

void look()
{
  register int i, abort;
  int dir, dummy;

  if (py.flags.blind > 0)
    msg_print("You can't see a damn thing!");
  else if (py.flags.image > 0)
    msg_print("You can't believe what you are seeing! It's like a dream!");
  else if (get_alldir("Look which direction?", &dir))
    {
      abort = FALSE;
      gl_nseen = 0;
      gl_rock = 0;
      gl_noquery = FALSE;	/* Have to set this up for the look_see */
      if (look_see(0, 0, &dummy))
	abort = TRUE;
      else
	{
	  do
	    {
	      abort = FALSE;
	      if (dir == 5)
		{
		  for (i = 1; i <= 4; i++)
		    {
		      gl_fxx = set_fxx[i]; gl_fyx = set_fyx[i];
		      gl_fxy = set_fxy[i]; gl_fyy = set_fyy[i];
		      if (look_ray(0, 2*GRADF-1, 1))
			{
			  abort = TRUE;
			  break;
			}
		      gl_fxy = -gl_fxy;
		      gl_fyy = -gl_fyy;
		      if (look_ray(0, 2*GRADF, 2))
			{
			  abort = TRUE;
			  break;
			}
		    }
		}
	      else if ((dir & 1) == 0)	/* Straight directions */
		{
		  i = dir >> 1;
		  gl_fxx = set_fxx[i]; gl_fyx = set_fyx[i];
		  gl_fxy = set_fxy[i]; gl_fyy = set_fyy[i];
		  if (look_ray(0, GRADF, 1))
		    abort = TRUE;
		  else
		    {
		      gl_fxy = -gl_fxy;
		      gl_fyy = -gl_fyy;
		      abort = look_ray(0, GRADF, 2);
		    }
		}
	      else
		{
		  i = map_diag1[dir >> 1];
		  gl_fxx = set_fxx[i]; gl_fyx = set_fyx[i];
		  gl_fxy = -set_fxy[i]; gl_fyy = -set_fyy[i];
		  if (look_ray(1, 2*GRADF, GRADF))
		    abort = TRUE;
		  else
		    {
		      i = map_diag2[dir >> 1];
		      gl_fxx = set_fxx[i]; gl_fyx = set_fyx[i];
		      gl_fxy = set_fxy[i]; gl_fyy = set_fyy[i];
		      abort = look_ray(1, 2*GRADF-1, GRADF);
		    }
		}
	    }
	  while (abort == FALSE && highlight_seams && (++gl_rock < 2));
	  if (abort)
	    msg_print("--Aborting look--");
	  else
	    {
	      if (gl_nseen)
		{
		  if (dir == 5)
		    msg_print("That's all you see.");
		  else
		    msg_print("That's all you see in that direction.");
		}
	      else if (dir == 5)
		msg_print("You see nothing of interest.");
	      else
		msg_print("You see nothing of interest in that direction.");
	    }
	}
    }
}

/* Look at everything within a cone of vision between two ray
   lines emanating from the player, and y or more places away
   from the direct line of view. This is recursive.

   Rays are specified by gradients, y over x, multiplied by
   2*GRADF. This is ONLY called with gradients between 2*GRADF
   (45 degrees) and 1 (almost horizontal).

   (y axis)/ angle from
     ^	  /	    ___ angle to
     |	 /	 ___
  ...|../.....___.................... parameter y (look at things in the
     | /   ___			      cone, and on or above this line)
     |/ ___
     @-------------------->   direction in which you are looking. (x axis)
     |
     | */
static int look_ray(y, from, to)
int y, from, to;
{
  register int max_x, x;
  int transparent;

  /* from is the larger angle of the ray, since we scan towards the
     center line. If from is smaller, then the ray does not exist. */
  if (from <= to || y > MAX_SIGHT)
    return FALSE;
  /* Find first visible location along this line. Minimum x such
     that (2x-1)/x < from/GRADF <=> x > GRADF(2x-1)/from. This may
     be called with y=0 whence x will be set to 0. Thus we need a
     special fix. */
  x = (int)((long)GRADF * (2 * y - 1) / from + 1);
  if (x <= 0)
    x = 1;

  /* Find last visible location along this line.
     Maximum x such that (2x+1)/x > to/GRADF <=> x < GRADF(2x+1)/to */
  max_x = (int)(((long)GRADF * (2 * y + 1) - 1) / to);
  if (max_x > MAX_SIGHT)
    max_x = MAX_SIGHT;
  if (max_x < x)
    return FALSE;

  /* gl_noquery is a HACK to prevent doubling up on direct lines of
     sight. If 'to' is	greater than 1, we do not really look at
     stuff along the direct line of sight, but we do have to see
     what is opaque for the purposes of obscuring other objects. */
  if (y == 0 && to > 1 || y == x && from < GRADF*2)
    gl_noquery = TRUE;
  else
    gl_noquery = FALSE;
  if (look_see(x, y, &transparent))
    return TRUE;
  if (y == x)
    gl_noquery = FALSE;
  if (transparent)
    goto init_transparent;

  for (;;)
    {
      /* Look down the window we've found. */
      if (look_ray(y+1, from, (int)((2 * y + 1) * (long)GRADF / x)))
	return TRUE;
      /* Find the start of next window. */
      do
	{
	  if (x == max_x)
	    return FALSE;
	  /* See if this seals off the scan. (If y is zero, then it will.) */
	  from = (int)((2 * y - 1) * (long)GRADF / x);
	  if (from <= to)
	    return FALSE;
	  x++;
	  if (look_see(x, y, &transparent))
	    return TRUE;
	}
      while(!transparent);
    init_transparent:
      /* Find the end of this window of visibility. */
      do
	{
	  if (x == max_x)
	    /* The window is trimmed by an earlier limit. */
	    return look_ray(y+1, from, to);
	  x++;
	  if (look_see(x, y, &transparent))
	    return TRUE;
	}
      while(transparent);
    }
}

static int look_see(x, y, transparent)
register int x, y;
int *transparent;
{
  char *dstring, *string, query;
  register cave_type *c_ptr;
  register int j;
  bigvtype out_val, tmp_str;

  if (x < 0 || y < 0 || y > x)
    {
      (void) sprintf(tmp_str, "Illegal call to look_see(%d, %d)", x, y);
      msg_print(tmp_str);
    }
  if (x == 0 && y == 0)
    dstring = "You are on";
  else
    dstring = "You see";
  j = char_col + gl_fxx * x + gl_fxy * y;
  y = char_row + gl_fyx * x + gl_fyy * y;
  x = j;
  if (!panel_contains(y, x))
    {
      *transparent = FALSE;
      return FALSE;
    }
  c_ptr = &cave[y][x];
  *transparent = c_ptr->fval <= MAX_OPEN_SPACE;
  if (gl_noquery)
    return FALSE; /* Don't look at a direct line of sight. A hack. */
  out_val[0] = 0;
  if (gl_rock == 0 && c_ptr->cptr > 1 && m_list[c_ptr->cptr].ml)
    {
      j = m_list[c_ptr->cptr].mptr;
      (void) sprintf(out_val, "%s %s %s. [(r)ecall]",
		     dstring,
		     is_a_vowel( c_list[j].name[0] ) ? "an" : "a",
		     c_list[j].name);
      dstring = "It is on";
      prt(out_val, 0, 0);
      move_cursor_relative(y, x);
      query = inkey();
      if (query == 'r' || query == 'R')
	{
	  save_screen();
	  query = roff_recall(j);
	  restore_screen();
	}
    }
  if (c_ptr->tl || c_ptr->pl || c_ptr->fm)
    {
      if (c_ptr->tptr != 0)
	{
	  if (t_list[c_ptr->tptr].tval == TV_SECRET_DOOR)
	    goto granite;
	  if (gl_rock ==0 && t_list[c_ptr->tptr].tval != TV_INVIS_TRAP)
	    {
	      objdes(tmp_str, &t_list[c_ptr->tptr], TRUE);
	      (void) sprintf(out_val, "%s %s ---pause---", dstring, tmp_str);
	      dstring = "It is in";
	      prt(out_val, 0, 0);
	      move_cursor_relative(y, x);
	      query = inkey();
	    }
	}
      if ((gl_rock || out_val[0]) && c_ptr->fval >= MIN_CLOSED_SPACE)
	{
	  switch(c_ptr->fval)
	    {
	    case BOUNDARY_WALL:
	    case GRANITE_WALL:
	    granite:
	      /* Granite is only interesting if it contains something. */
	      if(out_val[0])
		string = "a granite wall";
	      else
		string = CNIL;	/* In case we jump here */
	      break;
	    case MAGMA_WALL: string = "some dark rock";
	      break;
	    case QUARTZ_WALL: string = "a quartz vein";
	      break;
	    default: string = CNIL;
	      break;
	    }
	  if (string)
	    {
	      (void) sprintf(out_val, "%s %s ---pause---", dstring, string);
	      prt(out_val, 0, 0);
	      move_cursor_relative(y, x);
	      query = inkey();
	    }
	}
    }
  if (out_val[0])
    {
      gl_nseen++;
      if (query == ESCAPE)
	return TRUE;
    }

  return FALSE;
}


static void inven_throw(item_val, t_ptr)
int item_val;
inven_type *t_ptr;
{
  register inven_type *i_ptr;
#ifdef ATARIST_MWC
  int32u holder;
#endif

  i_ptr = &inventory[item_val];
  *t_ptr = *i_ptr;
  if (i_ptr->number > 1)
    {
      t_ptr->number = 1;
      i_ptr->number--;
      inven_weight -= i_ptr->weight;
#ifdef ATARIST_MWC
      py.flags.status |= (holder = PY_STR_WGT);
#else
      py.flags.status |= PY_STR_WGT;
#endif
    }
  else
    inven_destroy(item_val);
}


/* Obtain the hit and damage bonuses and the maximum distance for a
   thrown missile. */
static void facts(i_ptr, tbth, tpth, tdam, tdis)
register inven_type *i_ptr;
int *tbth, *tpth, *tdam, *tdis;
{
  register int tmp_weight;

  if (i_ptr->weight < 1)
    tmp_weight = 1;
  else
    tmp_weight = i_ptr->weight;

  /* Throwing objects			*/
  *tdam = pdamroll(i_ptr->damage) + i_ptr->todam;
  *tbth = py.misc.bthb * 75 / 100;
  *tpth = py.misc.ptohit + i_ptr->tohit;

  /* Add this back later if the correct throwing device. -CJS- */
  if (inventory[INVEN_WIELD].tval != TV_NOTHING)
    *tpth -= inventory[INVEN_WIELD].tohit;

  *tdis = (((py.stats.use_stat[A_STR]+20)*10)/tmp_weight);
  if (*tdis > 10)  *tdis = 10;

  /* multiply damage bonuses instead of adding, when have proper
     missile/weapon combo, this makes them much more useful */

  /* Using Bows,  slings,  or crossbows	*/
  if (inventory[INVEN_WIELD].tval == TV_BOW)
    switch(inventory[INVEN_WIELD].p1)
      {
      case 1:
	if (i_ptr->tval == TV_SLING_AMMO) /* Sling and ammo */
	  {
	    *tbth = py.misc.bthb;
	    *tpth += 2 * inventory[INVEN_WIELD].tohit;
	    *tdam += inventory[INVEN_WIELD].todam;
	    *tdam = *tdam * 2;
	    *tdis = 20;
	  }
	break;
      case 2:
	if (i_ptr->tval == TV_ARROW)      /* Short Bow and Arrow	*/
	  {
	    *tbth = py.misc.bthb;
	    *tpth += 2 * inventory[INVEN_WIELD].tohit;
	    *tdam += inventory[INVEN_WIELD].todam;
	    *tdam = *tdam * 2;
	    *tdis = 25;
	  }
	break;
      case 3:
	if (i_ptr->tval == TV_ARROW)      /* Long Bow and Arrow	*/
	  {
	    *tbth = py.misc.bthb;
	    *tpth += 2 * inventory[INVEN_WIELD].tohit;
	    *tdam += inventory[INVEN_WIELD].todam;
	    *tdam = *tdam * 3;
	    *tdis = 30;
	  }
	break;
      case 4:
	if (i_ptr->tval == TV_ARROW)      /* Composite Bow and Arrow*/
	  {
	    *tbth = py.misc.bthb;
	    *tpth += 2 * inventory[INVEN_WIELD].tohit;
	    *tdam += inventory[INVEN_WIELD].todam;
	    *tdam = *tdam * 4;
	    *tdis = 35;
	  }
	break;
      case 5:
	if (i_ptr->tval == TV_BOLT)      /* Light Crossbow and Bolt*/
	  {
	    *tbth = py.misc.bthb;
	    *tpth += 2 * inventory[INVEN_WIELD].tohit;
	    *tdam += inventory[INVEN_WIELD].todam;
	    *tdam = *tdam * 3;
	    *tdis = 25;
	  }
	break;
      case 6:
	if (i_ptr->tval == TV_BOLT)      /* Heavy Crossbow and Bolt*/
	  {
	    *tbth = py.misc.bthb;
	    *tpth += 2 * inventory[INVEN_WIELD].tohit;
	    *tdam += inventory[INVEN_WIELD].todam;
	    *tdam = *tdam * 4;
	    *tdis = 35;
	  }
	break;
      }
}


static void drop_throw(y, x, t_ptr)
int y, x;
inven_type *t_ptr;
{
  register int i, j, k;
  int flag, cur_pos;
  bigvtype out_val, tmp_str;
  register cave_type *c_ptr;

  flag = FALSE;
  i = y;
  j = x;
  k = 0;
  if (randint(10) > 1)
    {
      do
	{
	  if (in_bounds(i, j))
	    {
	      c_ptr = &cave[i][j];
	      if (c_ptr->fval <= MAX_OPEN_SPACE && c_ptr->tptr == 0)
		flag = TRUE;
	    }
	  if (!flag)
	    {
	      i = y + randint(3) - 2;
	      j = x + randint(3) - 2;
	      k++;
	    }
	}
      while ((!flag) && (k <= 9));
    }
  if (flag)
    {
      cur_pos = popt();
      cave[i][j].tptr = cur_pos;
      t_list[cur_pos] = *t_ptr;
      lite_spot(i, j);
    }
  else
    {
      objdes(tmp_str, t_ptr, FALSE);
      (void) sprintf(out_val, "The %s disappears.", tmp_str);
      msg_print(out_val);
    }
}

/* Throw an object across the dungeon.		-RAK-	*/
/* Note: Flasks of oil do fire damage				 */
/* Note: Extra damage and chance of hitting when missiles are used*/
/*	 with correct weapon.  I.E.  wield bow and throw arrow.	 */
void throw_object()
{
  int item_val, tbth, tpth, tdam, tdis;
  int y, x, oldy, oldx, cur_dis, dir;
  int flag, visible;
  bigvtype out_val, tmp_str;
  inven_type throw_obj;
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register int i;
  char tchar;

  if (inven_ctr == 0)
    {
      msg_print("But you are not carrying anything.");
      free_turn_flag = TRUE;
    }
  else if (get_item(&item_val, "Fire/Throw which one?", 0, inven_ctr-1, CNIL,
		    CNIL))
    {
      if (get_dir(CNIL, &dir))
	{
	  desc_remain(item_val);
	  if (py.flags.confused > 0)
	    {
	      msg_print("You are confused.");
	      do
		{
		  dir = randint(9);
		}
	      while (dir == 5);
	    }
	  inven_throw(item_val, &throw_obj);
	  facts(&throw_obj, &tbth, &tpth, &tdam, &tdis);
	  tchar = throw_obj.tchar;
	  flag = FALSE;
	  y = char_row;
	  x = char_col;
	  oldy = char_row;
	  oldx = char_col;
	  cur_dis = 0;
	  do
	    {
	      (void) mmove(dir, &y, &x);
	      cur_dis++;
	      lite_spot(oldy, oldx);
	      if (cur_dis > tdis)  flag = TRUE;
	      c_ptr = &cave[y][x];
	      if ((c_ptr->fval <= MAX_OPEN_SPACE) && (!flag))
		{
		  if (c_ptr->cptr > 1)
		    {
		      flag = TRUE;
		      m_ptr = &m_list[c_ptr->cptr];
		      tbth = tbth - cur_dis;
		      /* if monster not lit, make it much more difficult to
			 hit, subtract off most bonuses, and reduce bthb
			 depending on distance */
		      if (!m_ptr->ml)
			tbth = (tbth / (cur_dis+2))
			  - (py.misc.lev *
			     class_level_adj[py.misc.pclass][CLA_BTHB] / 2)
			    - (tpth * (BTH_PLUS_ADJ-1));
		      if (test_hit(tbth, (int)py.misc.lev, tpth,
				   (int)c_list[m_ptr->mptr].ac, CLA_BTHB))
			{
			  i = m_ptr->mptr;
			  objdes(tmp_str, &throw_obj, FALSE);
			  /* Does the player know what he's fighting?	   */
			  if (!m_ptr->ml)
			    {
			      (void) sprintf(out_val,
			   "You hear a cry as the %s finds a mark.", tmp_str);
			      visible = FALSE;
			    }
			  else
			    {
			      (void) sprintf(out_val, "The %s hits the %s.",
					     tmp_str, c_list[i].name);
			      visible = TRUE;
			    }
			  msg_print(out_val);
			  tdam = tot_dam(&throw_obj, tdam, i);
			  tdam = critical_blow((int)throw_obj.weight,
					       tpth, tdam, CLA_BTHB);
			  if (tdam < 0) tdam = 0;
			  i = mon_take_hit((int)c_ptr->cptr, tdam);
			  if (i >= 0)
			    {
			      if (!visible)
				msg_print("You have killed something!");
			      else
				{
			  (void) sprintf(out_val,"You have killed the %s.",
						 c_list[i].name);
				  msg_print(out_val);
				}
			      prt_experience();
			    }
			}
		      else
			drop_throw(oldy, oldx, &throw_obj);
		    }
		  else
		    {	/* do not test c_ptr->fm here */
		      if (panel_contains(y, x) && (py.flags.blind < 1)
			  && (c_ptr->tl || c_ptr->pl))
			{
			  print(tchar, y, x);
			  put_qio(); /* show object moving */
			}
		    }
		}
	      else
		{
		  flag = TRUE;
		  drop_throw(oldy, oldx, &throw_obj);
		}
	      oldy = y;
	      oldx = x;
	    }
	  while (!flag);
	}
    }
}


/* Make a bash attack on someone.				-CJS-
   Used to be part of bash above. */
static void py_bash(y, x)
int y, x;
{
  int monster, k, avg_max_hp, base_tohit;
  register creature_type *c_ptr;
  register monster_type *m_ptr;
  vtype m_name, out_val;

  monster = cave[y][x].cptr;
  m_ptr = &m_list[monster];
  c_ptr = &c_list[m_ptr->mptr];
  m_ptr->csleep = 0;
  /* Does the player know what he's fighting?	   */
  if (!m_ptr->ml)
    (void) strcpy(m_name, "it");
  else
    (void) sprintf(m_name, "the %s", c_ptr->name);
  base_tohit = py.stats.use_stat[A_STR] + inventory[INVEN_ARM].weight/2
    + py.misc.wt/10;
  if (!m_ptr->ml)
    base_tohit = (base_tohit / 2)
      - (py.stats.use_stat[A_DEX]*(BTH_PLUS_ADJ-1))
	- (py.misc.lev * class_level_adj[py.misc.pclass][CLA_BTH] / 2);

  if (test_hit(base_tohit, (int)py.misc.lev,
	       (int)py.stats.use_stat[A_DEX], (int)c_ptr->ac, CLA_BTH))
    {
      (void) sprintf(out_val, "You hit %s.", m_name);
      msg_print(out_val);
      k = pdamroll(inventory[INVEN_ARM].damage);
      k = critical_blow((int)(inventory[INVEN_ARM].weight / 4
			      + py.stats.use_stat[A_STR]), 0, k, CLA_BTH);
      k += py.misc.wt/60 + 3;
      if (k < 0) k = 0;

      /* See if we done it in.				     */
      if (mon_take_hit(monster, k) >= 0)
	{
	  (void) sprintf(out_val, "You have slain %s.", m_name);
	  msg_print(out_val);
	  prt_experience();
	}
      else
	{
	  m_name[0] = toupper((int)m_name[0]); /* Capitalize */
	  /* Can not stun Balrog */
	  avg_max_hp = (c_ptr->cdefense & CD_MAX_HP ?
			c_ptr->hd[0] * c_ptr->hd[1] :
			(c_ptr->hd[0] * (c_ptr->hd[1] + 1)) >> 1);
	  if ((100 + randint(400) + randint(400))
	      > (m_ptr->hp + avg_max_hp))
	    {
	      m_ptr->stunned += randint(3) + 1;
	      if (m_ptr->stunned > 24)	m_ptr->stunned = 24;
	      (void) sprintf(out_val, "%s appears stunned!", m_name);
	    }
	  else
	    (void) sprintf(out_val, "%s ignores your bash!", m_name);
	  msg_print(out_val);
	}
    }
  else
    {
      (void) sprintf(out_val, "You miss %s.", m_name);
      msg_print(out_val);
    }
  if (randint(150) > py.stats.use_stat[A_DEX])
    {
      msg_print("You are off balance.");
      py.flags.paralysis = 1 + randint(2);
    }
}


/* Bash open a door or chest				-RAK-	*/
/* Note: Affected by strength and weight of character

   For a closed door, p1 is positive if locked; negative if
   stuck. A disarm spell unlocks and unjams doors!

   For an open door, p1 is positive for a broken door.

   A closed door can be opened - harder if locked. Any door might
   be bashed open (and thereby broken). Bashing a door is
   (potentially) faster! You move into the door way. To open a
   stuck door, it must be bashed. A closed door can be jammed
   (which makes it stuck if previously locked).

   Creatures can also open doors. A creature with open door
   ability will (if not in the line of sight) move though a
   closed or secret door with no changes. If in the line of
   sight, closed door are openned, & secret door revealed.
   Whether in the line of sight or not, such a creature may
   unlock or unstick a door.

   A creature with no such ability will attempt to bash a
   non-secret door. */
void bash()
{
  int y, x, dir, tmp;
  register cave_type *c_ptr;
  register inven_type *t_ptr;

  y = char_row;
  x = char_col;
  if (get_dir(CNIL, &dir))
    {
      if (py.flags.confused > 0)
	{
	  msg_print("You are confused.");
	  do
	    {
	      dir = randint(9);
	    }
	  while (dir == 5);
	}
      (void) mmove(dir, &y, &x);
      c_ptr = &cave[y][x];
      if (c_ptr->cptr > 1)
	{
	  if (py.flags.afraid > 0)
	    msg_print("You are afraid!");
	  else
	    py_bash(y, x);
	}
      else if (c_ptr->tptr != 0)
	{
	  t_ptr = &t_list[c_ptr->tptr];
	  if (t_ptr->tval == TV_CLOSED_DOOR)
	    {
	      count_msg_print("You smash into the door!");
	      tmp = py.stats.use_stat[A_STR] + py.misc.wt / 2;
	      /* Use (roughly) similar method as for monsters. */
	      if (randint(tmp*(20+abs(t_ptr->p1))) < 10*(tmp-abs(t_ptr->p1)))
		{
		  msg_print("The door crashes open!");
		  invcopy(&t_list[c_ptr->tptr], OBJ_OPEN_DOOR);
		  t_ptr->p1 = 1 - randint(2); /* 50% chance of breaking door */
		  c_ptr->fval = CORR_FLOOR;
		  if (py.flags.confused == 0)
		    move_char(dir, FALSE);
		  else
		    lite_spot(y, x);
		}
	      else if (randint(150) > py.stats.use_stat[A_DEX])
		{
		  msg_print("You are off-balance.");
		  py.flags.paralysis = 1 + randint(2);
		}
	      else if (command_count == 0)
		msg_print("The door holds firm.");
	    }
	  else if (t_ptr->tval == TV_CHEST)
	    {
	      if (randint(10) == 1)
		{
		  msg_print("You have destroyed the chest.");
		  msg_print("and its contents!");
		  t_ptr->index = OBJ_RUINED_CHEST;
		  t_ptr->flags = 0;
		}
	      else if ((CH_LOCKED & t_ptr->flags) && (randint(10) == 1))
		{
		  msg_print("The lock breaks open!");
		  t_ptr->flags &= ~CH_LOCKED;
		}
	      else
		count_msg_print("The chest holds firm.");
	    }
	  else 
	    /* Can't give free turn, or else player could try directions
	       until he found invisible creature */
	    msg_print("You bash it, but nothing interesting happens.");
	}
      else
	{
	  if (c_ptr->fval < MIN_CAVE_WALL)
	    msg_print("You bash at empty space.");
	  else
	    /* same message for wall as for secret door */
	    msg_print("You bash it, but nothing interesting happens.");
	}
    }
}
