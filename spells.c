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

extern char cur_char2();

/* Following are spell procedure/functions			-RAK-	*/
/* These routines are commonly used in the scroll, potion, wands, and    */
/* staves routines, and are occasionally called from other areas.         */
/* Now included are creature spells also...                      -RAK    */

monster_name (m_name, m_ptr, r_ptr)
char *m_name;
monster_type *m_ptr;
creature_type *r_ptr;
{
  if ((!m_ptr->ml) || (py.flags.blind > 0) ||
      ((0x10000 & r_ptr->cmove) && (!py.flags.see_inv)))
    (void) strcpy (m_name, "It");
  else
    (void) sprintf (m_name, "The %s", r_ptr->name);
}

lower_monster_name (m_name, m_ptr, r_ptr)
char *m_name;
monster_type *m_ptr;
creature_type *r_ptr;
{
  if ((!m_ptr->ml) || (py.flags.blind > 0) ||
      ((0x10000 & r_ptr->cmove) && (!py.flags.see_inv)))
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
  vtype out_val;
  vtype m_name;

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
		(0x1000 & r_ptr->cdefense))
	      {
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
  register int i, j;
  register int detect;
  register cave_type *c_ptr;

  detect = FALSE;
  for (i = panel_row_min; i <= panel_row_max; i++)
    for (j = panel_col_min; j <= panel_col_max; j++)
      {
	c_ptr = &cave[i][j];
	if (c_ptr->tptr != 0)
	  if (t_list[c_ptr->tptr].tval == 100)
	    if (!test_light(i, j))
	      {
		lite_spot(i, j);
		c_ptr->tl = TRUE;
		detect = TRUE;
	      }
      }
  return(detect);
}


/* Detect all objects on the current panel		-RAK-	*/
int detect_object()
{
  register int i, j;
  register int detect;
  register cave_type *c_ptr;

  detect = FALSE;
  for (i = panel_row_min; i <= panel_row_max; i++)
    for (j = panel_col_min; j <= panel_col_max; j++)
      {
	c_ptr = &cave[i][j];
	if (c_ptr->tptr != 0)
	  if (t_list[c_ptr->tptr].tval < 100)
	    if (!test_light(i, j))
	      {
		lite_spot(i, j);
		c_ptr->tl = TRUE;
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
  register treasure_type *t_ptr;

  detect = FALSE;
  for (i = panel_row_min; i <= panel_row_max; i++)
    for (j = panel_col_min; j <= panel_col_max; j++)
      {
	c_ptr = &cave[i][j];
	if (c_ptr->tptr != 0)
	  if (t_list[c_ptr->tptr].tval == 101)
	    {
	      change_trap(i, j);
	      c_ptr->fm = TRUE;
	      detect = TRUE;
	    }
	  else if (t_list[c_ptr->tptr].tval == 2)
	    {
	      t_ptr = &t_list[c_ptr->tptr];
	      known2(t_ptr->name);
	    }
      }
  return(detect);
}


/* Locates and displays all secret doors on current panel -RAK-	*/
int detect_sdoor()
{
  register int i, j;
  register int detect;
  register cave_type *c_ptr;

  detect = FALSE;
  for (i = panel_row_min; i <= panel_row_max; i++)
    for (j = panel_col_min; j <= panel_col_max; j++)
      {
	c_ptr = &cave[i][j];
	if (c_ptr->tptr != 0)
	  /* Secret doors  */
	  if (t_list[c_ptr->tptr].tval == 109)
	    {
	      c_ptr->fval = corr_floor3.ftval;
	      change_trap(i, j);
	      c_ptr->fm = TRUE;
	      detect = TRUE;
	    }
	/* Staircases    */
	  else if ((t_list[c_ptr->tptr].tval == 107) ||
		   (t_list[c_ptr->tptr].tval == 108))
	    if (!c_ptr->fm)
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
  register int i;
  register int flag;
  char tmp_str[2];
  register monster_type *m_ptr;

  flag = FALSE;
  i = muptr;
  while (i > 0)
    {
      m_ptr = &m_list[i];
      if (panel_contains((int)m_ptr->fy, (int)m_ptr->fx))
	if (0x10000 & c_list[m_ptr->mptr].cmove)
	  {
	    m_ptr->ml = TRUE;
	    tmp_str[0] = c_list[m_ptr->mptr].cchar;
	    tmp_str[1] = '\0';
	    print(tmp_str, (int)m_ptr->fy, (int)m_ptr->fx);
	    flag = TRUE;
	  }
      i = m_list[i].nptr;
    }
  if (flag)
    {
      msg_print("You sense the presence of invisible creatures!");
      /* make sure player sees the message */
      msg_print(" ");
      msg_flag = FALSE;
    }
  return(flag);
}


/* Light an area: 1.  If corridor  light immediate area -RAK-*/
/*                2.  If room  light entire room.            */
int light_area(y, x)
register int y, x;
{
  register int i, j;
  register int light;

  msg_print("You are surrounded by a white light.");
  light = TRUE;
  if (((cave[y][x].fval == 1) || (cave[y][x].fval == 2)) && (dun_level > 0))
    light_room(y, x);
  else
    for (i = y-1; i <= y+1; i++)
      for (j = x-1; j <=  x+1; j++)
	if (in_bounds(i, j))
	  {
	    if (!test_light(i, j))
	      lite_spot(i, j);
	    cave[i][j].pl = TRUE;
	  }
  return(light);
}


/* Darken an area, opposite of light area		-RAK-	*/
int unlight_area(y, x)
int y, x;
{
  register int i, j, k;
  int tmp1, tmp2;
  int start_row, start_col;
  int end_row, end_col;
  int flag;
  int unlight;
  register cave_type *c_ptr;
  vtype out_val;

  flag = FALSE;
  if (((cave[y][x].fval == 1) || (cave[y][x].fval == 2)) && (dun_level > 0))
    {
      tmp1 = (SCREEN_HEIGHT/2);
      tmp2 = (SCREEN_WIDTH /2);
      start_row = (y/tmp1)*tmp1 + 1;
      start_col = (x/tmp2)*tmp2 + 1;
      end_row = start_row + tmp1 - 1;
      end_col = start_col + tmp2 - 1;
      for (i = start_row; i <= end_row; i++)
	{
	  out_val[0] = '\0';
	  k = 0;
	  for (j = start_col; j <= end_col; j++)
	    {
	      c_ptr = &cave[i][j];
	      if ((c_ptr->fval == 1) || (c_ptr->fval == 2))
		{
		  c_ptr->pl = FALSE;
		  c_ptr->fval = 1;
		  if (!test_light(i, j))
		    {
		      if (k == 0)
			k = j;
		      (void) strcat(out_val, " ");
		    }
		  else if (k > 0)
		    {
		      flag = TRUE;
		      print(out_val, i, k);
		      out_val[0] = '\0';
		      k = 0;
		    }
		}
	      else if (k > 0)
		{
		  flag = TRUE;
		  print(out_val, i, k);
		  out_val[0] = '\0';
		  k = 0;
		}
	      if (k > 0)
		{
		  flag = TRUE;
		  print(out_val, i, k);
		}
	    }
	}
    }
  else
    for (i = y-1; i <= y+1; i++)
      for (j = x-1; j <= x+1; j++)
	if (in_bounds(i, j))
	  {
	    c_ptr = &cave[i][j];
	    if ((c_ptr->fval == 4) || (c_ptr->fval == 5) || (c_ptr->fval ==6))
	      if (c_ptr->pl)
		{
		  c_ptr->pl = FALSE;
		  flag = TRUE;
		}
	    if (flag)
	      {
		msg_print("Darkness surrounds you...");
		unlight = TRUE;
	      }
	  }
	else
	  unlight = FALSE;
  return(unlight);
}


/* Map the current area plus some			-RAK-	*/
int map_area()
{
  register cave_type *c_ptr;
  register int i7, i8, n, m;
  int i, j, k, l;
  int map;

  map = TRUE;
  i = panel_row_min - randint(10);
  j = panel_row_max + randint(10);
  k = panel_col_min - randint(20);
  l = panel_col_max + randint(20);
  for (m = i; m <= j; m++)
    for (n = k; n <= l; n++)
      if (in_bounds(m, n))
	if (set_floor(cave[m][n].fval))
	  for (i7 = m-1; i7 <= m+1; i7++)
	    for (i8 = n-1; i8 <= n+1; i8++)
	      {
		c_ptr = &cave[i7][i8];
		if (((c_ptr->fval >= 10) && (c_ptr->fval <= 12)) ||
		    (c_ptr->fval == 15))
		  c_ptr->pl = TRUE;
		else if (c_ptr->tptr != 0)
		  if ((t_list[c_ptr->tptr].tval >= 102) &&
		      (t_list[c_ptr->tptr].tval <= 110) &&
		      (t_list[c_ptr->tptr].tval != 106))
		    c_ptr->fm = TRUE;
	      }
  prt_map();
  return(map);
}


/* Identify an object					-RAK-	*/
int ident_spell()
{
  int item_val;
  vtype out_val, tmp_str;
  int redraw;
  int ident;
  register treasure_type *i_ptr;

  ident = FALSE;
  redraw = FALSE;
  if (get_item(&item_val, "Item you wish identified?",
	       &redraw, 0, inven_ctr-1))
    {
      i_ptr = &inventory[item_val];
      ident = TRUE;
      identify(inventory[item_val]);
      known2(i_ptr->name);
      objdes(tmp_str, item_val, TRUE);
      (void) sprintf(out_val, "%c%c %s", item_val+97, cur_char2(item_val),
		     tmp_str);
      msg_print(out_val);
    }
  if (redraw)
    {
      /* make sure player sees message before draw cave erases it */
      msg_print(" ");
      draw_cave();
    }
  return(ident);
}


/* Get all the monsters on the level pissed off...	-RAK-	*/
int aggravate_monster  (dis_affect)
int dis_affect;
{
  register int i;
  int aggravate;
  register monster_type *m_ptr;

  aggravate = TRUE;
  i = muptr;
  while (i > 0)
    {
      m_ptr = &m_list[i];
      m_ptr->csleep = 0;
      if (m_ptr->cdis <= dis_affect)
	if (m_ptr->cspeed < 2)
	  m_ptr->cspeed++;
      i = m_list[i].nptr;
    }
  return(aggravate);
}


/* Surround the fool with traps (chuckle)		-RAK-	*/
int trap_creation()
{
  register int i, j;
  register int trap;
  register cave_type *c_ptr;

  trap = TRUE;
  for (i = char_row-1; i <= char_row+1; i++)
    for (j = char_col-1; j <= char_col+1; j++)
      {
	c_ptr = &cave[i][j];
	if (set_floor(c_ptr->fval))
	  {
	    if (c_ptr->tptr != 0)
	      (void) delete_object(i, j);
	    place_trap(i, j, 1, randint(MAX_TRAPA)-1);
	  }
      }
  return(trap);
}


/* Surround the player with doors...			-RAK-	*/
int door_creation()
{
  register int i, j;
  int k;
  register int door;
  register cave_type *c_ptr;

  door = TRUE;
  for (i = char_row-1; i <= char_row+1; i++)
    for (j = char_col-1; j <=  char_col+1; j++)
      if ((i != char_row) || (j != char_col))
	{
	  c_ptr = &cave[i][j];
	  if (set_floor(c_ptr->fval))
	    {
	      popt(&k);
	      if (c_ptr->tptr != 0)
		(void) delete_object(i, j);
	      c_ptr->fopen = FALSE;
	      c_ptr->tptr = k;
	      t_list[k] = door_list[1];
	      if (test_light(i, j))
		lite_spot(i, j);
	    }
	}
  return(door);
}


/* Destroys any adjacent door(s)/trap(s) 		-RAK-	*/
int td_destroy()
{
  register int i, j;
  register int destroy;
  register cave_type *c_ptr;

  destroy = FALSE;
  for (i = char_row-1; i <= char_row+1; i++)
    for (j = char_col-1; j <= char_col+1; j++)
      {
	c_ptr = &cave[i][j];
	if (c_ptr->tptr != 0)
	  {
	    if (((t_list[c_ptr->tptr].tval >= 101) &&
		 (t_list[c_ptr->tptr].tval <= 105) &&
		 (t_list[c_ptr->tptr].tval != 103)) ||
		(t_list[c_ptr->tptr].tval == 109))
	      {
		if (delete_object(i, j))
		  destroy = TRUE;
	      }
	    else if (t_list[c_ptr->tptr].tval == 2)
	      /* destroy traps on chest and unlock */
	      t_list[c_ptr->tptr].flags &= 0xFF000000;
	  }
      }
  return(destroy);
}


/* Display all creatures on the current panel		-RAK-	*/
int detect_monsters()
{
  register int i;
  register int flag, detect;
  char tmp_str[2];
  register monster_type *m_ptr;

  flag = FALSE;
  i = muptr;
  while (i > 0)
    {
      m_ptr = &m_list[i];
      if (panel_contains((int)m_ptr->fy, (int)m_ptr->fx))
	if ((0x10000 & c_list[m_ptr->mptr].cmove) == 0)
	  {
	    m_ptr->ml = TRUE;
	    tmp_str[0] = c_list[m_ptr->mptr].cchar;
	    tmp_str[1] = '\0';
	    print(tmp_str, (int)m_ptr->fy, (int)m_ptr->fx);
	    flag = TRUE;
	  }
      i = m_list[i].nptr;
    }
  if (flag)
    {
      msg_print("You sense the presence of monsters!");
      /* make sure player sees the message */
      msg_print(" ");
      msg_flag = FALSE;
      detect = TRUE;
    }
  detect = flag;
  return(detect);
}


/* Leave a line of light in given dir, blue light can sometimes	*/
/* hurt creatures...                                     -RAK-   */
light_line(dir, y, x)
int dir, y, x;
{
  register int i;
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  vtype out_val;
  vtype m_name;
  char tmp_str[2];

  while (cave[y][x].fopen){
      c_ptr = &cave[y][x];
      if (panel_contains(y, x))
	{
	  if ((!c_ptr->tl) && (!c_ptr->pl))
	    if (c_ptr->fval == 2)
	      light_room(y, x);
	    else
	      lite_spot(y, x);
	  if (c_ptr->cptr > 1)
	    {
	      m_ptr = &m_list[c_ptr->cptr];
	      r_ptr = &c_list[m_ptr->mptr];
	      m_ptr->ml = TRUE;
	      tmp_str[0] = r_ptr->cchar;
	      tmp_str[1] = '\0';
	      print(tmp_str, (int)m_ptr->fy, (int)m_ptr->fx);
	      monster_name (m_name, m_ptr, r_ptr);
	      if (0x0100 & r_ptr->cdefense)
		{
		  (void) sprintf(out_val, "%s wails out in pain!", m_name);
		  msg_print(out_val);
		  i = mon_take_hit((int)c_ptr->cptr, damroll("2d8"));
		  if (i >= 0)
		    {
		      (void) sprintf(out_val, "%s dies in a fit of agony.",
				     m_name);
		      msg_print(out_val);
		    }
		}
	    }
	  c_ptr->pl = TRUE;
	}
      (void) move(dir, &y, &x);
    }
}


/* Light line in all directions				-RAK-	*/
int starlite(y, x)
register int y, x;
{
  register int i;

  msg_print("The end of the staff bursts into a blue shimmering light.");
  for (i = 1; i <= 9; i++)
    if (i != 5)
      light_line(i, y, x);
  return(TRUE);
}


/* Disarms all traps/chests in a given direction 	-RAK-	*/
int disarm_all(dir, y, x)
int dir, y, x;
{
  register cave_type *c_ptr;
  register treasure_type *t_ptr;
  register int i, oldy, oldx;
  int disarm;
  char *string;

  disarm = FALSE;
  do
    {
      c_ptr = &cave[y][x];
      if (c_ptr->tptr != 0)
	{
	  t_ptr = &t_list[c_ptr->tptr];
	  if ((t_ptr->tval == 101) || (t_ptr->tval == 102))
	    {
	      if (delete_object(y, x))
		disarm = TRUE;
	    }
	  else if (t_ptr->tval == 105)
	    {
	      t_ptr->p1 = 0;
	    }
	  else if (t_ptr->tval == 109)
	    {
	      c_ptr->fval = corr_floor3.ftval;
	      change_trap(y, x);
	      c_ptr->fm = TRUE;
	      disarm = TRUE;
	    }
	  else if (t_ptr->tval == 2)
	    if (t_ptr->flags != 0)
	      {
		msg_print("Click!");
		t_ptr->flags = 0;
		disarm = TRUE;
		string = index(t_ptr->name, '(');
		if (string)
		  i = strlen(t_ptr->name) - strlen(string);
		else
		  i = -1;
		if (i >= 0)
		  t_ptr->name[i] = '\0';
		(void) strcat(t_ptr->name, " (Unlocked)");
		known2(t_ptr->name);
	      }
	}
      oldy = y;
      oldx = x;
      (void) move(dir, &y, &x);
    }
  while (cave[oldy][oldx].fopen);
  return(disarm);
}


/* Return flags for given type area affect		-RAK-	*/
get_flags(typ, weapon_type, harm_type, destroy)
int typ;
int *weapon_type, *harm_type;
int (**destroy)();
{
  int set_null(), set_fire_destroy(), set_frost_destroy();
  int set_acid_destroy(), set_lightning_destroy();

  switch(typ)
    {
    case 1:      /* Lightning     */
      *weapon_type = 0x00080000;
      *harm_type   = 0x0100;
      *destroy     = set_lightning_destroy;
      break;
    case 2:      /* Poison Gas    */
      *weapon_type = 0x00100000;
      *harm_type   = 0x0040;
      *destroy     = set_null;
      break;
    case 3:      /* Acid          */
      *weapon_type = 0x00200000;
      *harm_type   = 0x0080;
      *destroy     = set_acid_destroy;
      break;
    case 4:      /* Frost         */
      *weapon_type = 0x00400000;
      *harm_type   = 0x0010;
      *destroy     = set_frost_destroy;
      break;
    case 5:      /* Fire          */
      *weapon_type = 0x00800000;
      *harm_type   = 0x0020;
      *destroy     = set_fire_destroy;
      break;
    case 6:      /* Holy Orb      */
      *weapon_type = 0x00000000;
      *harm_type   = 0x0004;
      *destroy     = set_null;
      break;
    default:
      *weapon_type = 0;
      *harm_type   = 0;
      *destroy     = set_null;
    }
}


/* Shoot a bolt in a given direction			-RAK-	*/
int fire_bolt(typ, dir, y, x, dam, bolt_typ)
int typ, dir, y, x, dam;
ctype bolt_typ;
{
  int i, oldy, oldx, dist;
  int weapon_type, harm_type;
  int flag;
  int (*dummy)();
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  vtype out_val;
  vtype m_name;
  char tmp_str[2];

  flag = FALSE;
  get_flags(typ, &weapon_type, &harm_type, &dummy);
  oldy = y;
  oldx = x;
  dist = 0;
  do
    {
      (void) move(dir, &y, &x);
      if (test_light(oldy, oldx))
	lite_spot(oldy, oldx);
      else
	unlite_spot(oldy, oldx);
      dist++;
      if (dist > OBJ_BOLT_RANGE)
	flag = TRUE;
      else
	{
	  c_ptr = &cave[y][x];
	  if (c_ptr->fopen)
	    {
	      if (c_ptr->cptr > 1)
		{
		  flag = TRUE;
		  m_ptr = &m_list[c_ptr->cptr];
		  r_ptr = &c_list[m_ptr->mptr];
		  /* light it up first, then check to see if visible */
		  m_list[c_ptr->cptr].ml = TRUE;
		  lower_monster_name(m_name, m_ptr, r_ptr);
		  (void) sprintf(out_val, "The %s strikes %s.", bolt_typ,
			  m_name);
		  msg_print(out_val);
		  if (harm_type & r_ptr->cdefense)
		    dam = dam*2;
		  else if (weapon_type & r_ptr->spells)
		    dam = (dam/4.0);
		  i = mon_take_hit((int)c_ptr->cptr, dam);
		  monster_name(m_name, m_ptr, r_ptr);
		  if (i >= 0)
		    {
		      (void) sprintf(out_val, "%s dies in a fit of agony.",
			      m_name);
		      msg_print(out_val);
		    }
		  else
		    {
		      if (panel_contains(y, x))
			{
			  tmp_str[0] = c_list[m_ptr->mptr].cchar;
			  tmp_str[1] = '\0';
			  print(tmp_str, y, x);
			}
		      if (dam > 0)
			{
			  (void) sprintf (out_val, "%s screams in agony.",
					  m_name);
			  msg_print (out_val);
			}
		    }
		}
	      else if (panel_contains(y, x))
		{
		  print("*", y, x);
		  /* show the bolt */
		  put_qio();
		}
	    }
	  else
	    flag = TRUE;
	}
      oldy = y;
      oldx = x;
    }
  while (!flag);
}


/* Shoot a ball in a given direction.  Note that balls have an	*/
/* area affect....                                       -RAK-   */
int fire_ball(typ, dir, y, x, dam_hp, descrip)
int typ, dir, y, x, dam_hp;
ctype descrip;
{
  register int i, j;
  int dam, max_dis, thit, tkill, k;
  int oldy, oldx, dist;
  int weapon_type, harm_type;
  int flag;
  int (*destroy)();
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  vtype out_val;
  char tmp_str[2];

  thit   = 0;
  tkill  = 0;
  max_dis = 2;
  get_flags(typ, &weapon_type, &harm_type, &destroy);
  flag = FALSE;
  oldy = y;
  oldx = x;
  dist = 0;
  do
    {
      (void) move(dir, &y, &x);
      dist++;
      if (test_light(oldy, oldx))
	lite_spot(oldy, oldx);
      else
	unlite_spot(oldy, oldx);
      if (dist > OBJ_BOLT_RANGE)
	flag = TRUE;
      else
	{
	  c_ptr = &cave[y][x];
	  if ((!c_ptr->fopen) || (c_ptr->cptr > 1))
	    {
	      flag = TRUE;
	      if (!c_ptr->fopen)
		{
		  y = oldy;
		  x = oldx;
		}
	      /* The ball hits and explodes...                 */
	      /* The explosion...                      */
	      for (i = y-max_dis; i <= y+max_dis; i++)
		for (j = x-max_dis; j <= x+max_dis; j++)
		  if (in_bounds(i, j))
		    if (distance(y, x, i, j) <= max_dis)
		      if (los(y, x, i, j))     /* FIXED BUG V4.5        */
			{
			  c_ptr = &cave[i][j];
			  if (c_ptr->tptr != 0)
			    if ((*destroy)(t_list[c_ptr->tptr].tval))
			      (void) delete_object(i, j);
			  if (c_ptr->fopen)
			    {
			      if (panel_contains(i, j))  print("*", i, j);
			      if (c_ptr->cptr > 1)
				{
				  m_ptr = &m_list[c_ptr->cptr];
				  r_ptr = &c_list[m_ptr->mptr];
				  thit++;
				  dam = dam_hp;
				  if (harm_type & r_ptr->cdefense)
				    dam = dam*2;
				  else if (weapon_type & r_ptr->spells)
				    dam = dam / 4;
				  dam = (dam/(distance(i, j, y, x)+1));
				  k = mon_take_hit((int)c_ptr->cptr, dam);
				  if (k >= 0)
				    tkill++;
				  else
				    {
				      if (panel_contains(i, j))
					{
					  tmp_str[0] = r_ptr->cchar;
					  tmp_str[1] = '\0';
					  print(tmp_str, i, j);
					  m_ptr->ml = TRUE;
					}
				    }
				}
			    }
			}
	      /* show ball of whatever */
	      put_qio();

	      for (i = (y - 2); i <= (y + 2); i++)
		for (j = (x - 2); j <= (x + 2); j++)
		  if (in_bounds(i, j))
		    if (panel_contains(i, j))
		      if (distance(y, x, i, j) <= max_dis)
			{
			  c_ptr = &cave[i][j];
			  if (test_light(i, j))
			    lite_spot(i, j);
			  else if (c_ptr->cptr == 1)
			    lite_spot(i, j);
			  else if (c_ptr->cptr > 1)
			    if (m_list[c_ptr->cptr].ml)
			      lite_spot(i, j);
			    else
			      unlite_spot(i, j);
			  else
			    unlite_spot(i, j);
			}
	      /* End  explosion...                     */
	      if (thit == 1)
		{
		  (void) sprintf(out_val,
				 "The %s envelopes a creature!",
				 descrip);
		  msg_print(out_val);
		}
	      else if (thit > 1)
		{
		  (void) sprintf(out_val,
				 "The %s envelopes several creatures!",
				 descrip);
		  msg_print(out_val);
		}
	      if (tkill == 1)
		msg_print("There is a scream of agony!");
	      else if (tkill > 1)
		msg_print("There are several screams of agony!");
	      /* End ball hitting...                   */
	    }
	  else if (panel_contains(y, x))
	    {
	      print("*", y, x);
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
/* Note the area affect....                              -RAK-   */
breath(typ, y, x, dam_hp, ddesc)
int typ, y, x, dam_hp;
char *ddesc;
{
  register int i, j;
  int dam, max_dis;
  int weapon_type, harm_type;
  int (*destroy)();
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;

  max_dis = 2;
  get_flags(typ, &weapon_type, &harm_type, &destroy);
  for (i = y-2; i <= y+2; i++)
    for (j = x-2; j <= x+2; j++)
      if (in_bounds(i, j))
	if (distance(y, x, i, j) <= max_dis)
	  {
	    c_ptr = &cave[i][j];
	    if (c_ptr->tptr != 0)
	      if ((*destroy)(t_list[c_ptr->tptr].tval))
		(void) delete_object(i, j);
	    if (c_ptr->fopen)
	      {
		if (panel_contains(i, j))
		  print("*", i, j);
		if (c_ptr->cptr > 1)
		  {
		    m_ptr = &m_list[c_ptr->cptr];
		    r_ptr = &c_list[m_ptr->mptr];
		    dam = dam_hp;
		    if (harm_type & r_ptr->cdefense)
		      dam = dam*2;
		    else if (weapon_type & r_ptr->spells)
		      dam = (dam/4.0);
		    dam = (dam/(distance(i, j, y, x)+1));
		    m_ptr->hp = m_ptr->hp - dam;
		    m_ptr->csleep = 0;
		    if (m_ptr->hp < 0)
		      {
			monster_death((int)m_ptr->fy, (int)m_ptr->fx,
				      r_ptr->cmove);
			delete_monster((int)c_ptr->cptr);
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
		      case 1: light_dam(dam, ddesc); break;
		      case 2: poison_gas(dam, ddesc); break;
		      case 3: acid_dam(dam, ddesc); break;
		      case 4: cold_dam(dam, ddesc); break;
		      case 5: fire_dam(dam, ddesc); break;
		      }
		  }
	      }
	  }
  /* show the ball of gas */
  put_qio();

  for (i = (y - 2); i <= (y + 2); i++)
    for (j = (x - 2); j <= (x + 2); j++)
      if (in_bounds(i, j))
	if (panel_contains(i, j))
	  if (distance(y, x, i, j) <= max_dis)
	    {
	      c_ptr = &cave[i][j];
	      if (test_light(i, j))
		lite_spot(i, j);
	      else if (c_ptr->cptr == 1)
		lite_spot(i, j);
	      else if (c_ptr->cptr > 1)
		if (m_list[c_ptr->cptr].ml)
		  lite_spot(i, j);
		else
		  unlite_spot(i, j);
	      else
		unlite_spot(i, j);
	    }
}


/* Recharge a wand, staff, or rod.  Sometimes the item breaks. -RAK-*/
int recharge(num)
register int num;
{
  int item_val;
  int redraw;
  register int res;
  register treasure_type *i_ptr;

  res = FALSE;
  redraw = FALSE;
  if (get_item(&item_val, "Recharge which item?", &redraw, 0, inven_ctr-1))
    {
      i_ptr = &inventory[item_val];
      if ((i_ptr->tval == 55) || (i_ptr->tval == 60) || (i_ptr->tval == 65))
	/* recharge I = recharge(20) = 1/6 failure */
	/* recharge II = recharge(60) = 1/10 failure */
	if (randint((num + 40)/10) == 1)
	  {
	    res = TRUE;
	    msg_print("There is a bright flash of light...");
	    inven_destroy(item_val);
	  }
	else
	  {
	    res = TRUE;
	    num = (num/(i_ptr->level+2)) + 1;
	    i_ptr->p1 += 2 + randint(num);
	    if (index(i_ptr->name, '^') == 0)
	      insert_str(i_ptr->name, " (%P1", "^ (%P1");
	  }
    }
  if (redraw)
    {
      /* make sure player sees message before draw cave erases it */
      msg_print(" ");
      draw_cave();
    }
  return(res);
}


/* Increase or decrease a creatures hit points		-RAK-	*/
int hp_monster(dir, y, x, dam)
int dir, y, x, dam;
{
  register int i;
  int flag;
  int monster;
  cave_type *c_ptr;
  monster_type *m_ptr;
  creature_type *r_ptr;
  vtype out_val;
  vtype m_name;

  monster = FALSE;
  flag = FALSE;
  do
    {
      (void) move(dir, &y, &x);
      c_ptr = &cave[y][x];
      if (c_ptr->fopen)
	{
	  if (c_ptr->cptr > 1)
	    {
	      flag = TRUE;
	      m_ptr = &m_list[c_ptr->cptr];
	      r_ptr = &c_list[m_ptr->mptr];
	      monster_name (m_name, m_ptr, r_ptr);
	      monster = TRUE;
	      i = mon_take_hit((int)c_ptr->cptr, dam);
	      if (i >= 0)
		{
		  (void) sprintf(out_val, "%s dies in a fit of agony.",
			  m_name);
		  msg_print(out_val);
		}
	      else
		{
		  if (dam > 0)
		    {
		      (void) sprintf(out_val, "%s screams in agony.",
				     m_name);
		      msg_print(out_val);
		    }
		}
	    }
	}
      else
	flag = TRUE;
    }
  while (!flag);
  return(monster);
}


/* Drains life; note it must be living...		-RAK-	*/
int drain_life(dir, y, x)
int dir, y, x;
{
  register int i;
  int flag;
  int drain;
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  vtype out_val;
  vtype m_name;

  drain = FALSE;
  flag = FALSE;
  do
    {
      (void) move(dir, &y, &x);
      c_ptr = &cave[y][x];
      if (c_ptr->fopen)
	{
	  if (c_ptr->cptr > 1)
	    {
	      flag = TRUE;
	      m_ptr = &m_list[c_ptr->cptr];
	      r_ptr = &c_list[m_ptr->mptr];
	      if ((r_ptr->cdefense & 0x0008) == 0)
		{
		  drain = TRUE;
		  monster_name (m_name, m_ptr, r_ptr);
		  i = mon_take_hit((int)c_ptr->cptr, 50);
		  if (i >= 0)
		    {
		      (void) sprintf(out_val, "%s dies in a fit of agony.",
			      m_name);
		      msg_print(out_val);
		    }
		  else
		    {
		      (void) sprintf(out_val, "%s screams in agony.",
			      m_name);
		      msg_print(out_val);
		    }
		}
	    }
	  else
	    flag = TRUE;
	}
    }
  while (!flag);
  return(drain);
}


/* Increase or decrease a creatures speed		-RAK-	*/
/* NOTE: cannot slow a winning creature (BALROG)                 */
int speed_monster(dir, y, x, spd)
int dir, y, x, spd;
{
  int speed;
  int flag;
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  vtype out_val;
  vtype m_name;

  speed = FALSE;
  flag = FALSE;
  do
    {
      (void) move(dir, &y, &x);
      c_ptr = &cave[y][x];
      if (c_ptr->fopen)
	{
	  if (c_ptr->cptr > 1)
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
		  (void) sprintf(out_val, "%s is unaffected.", m_name);
		  msg_print(out_val);
		}
	    }
	}
      else
	flag = TRUE;
    }
  while (!flag);
  return(speed);
}


/* Confuse a creature					-RAK-	*/
int confuse_monster(dir, y, x)
int dir, y, x;
{
  int flag;
  int confuse;
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  vtype out_val;
  vtype m_name;

  confuse = FALSE;
  flag = FALSE;
  do
    {
      (void) move(dir, &y, &x);
      c_ptr = &cave[y][x];
      if (c_ptr->fopen)
	{
	  if (c_ptr->cptr > 1)
	    {
	      m_ptr = &m_list[c_ptr->cptr];
	      r_ptr = &c_list[m_ptr->mptr];
	      monster_name (m_name, m_ptr, r_ptr);
	      flag = TRUE;
	      if ((randint(MAX_MONS_LEVEL) < r_ptr->level) ||
		  (0x1000 & r_ptr->cdefense))
		{
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
      else
	flag = TRUE;
    }
  while (!flag);
  return(confuse);
}


/* Sleep a creature...					-RAK-	*/
int sleep_monster(dir, y, x)
int dir, y, x;
{
  int flag;
  int sleep;
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  vtype out_val;
  vtype m_name;

  sleep = FALSE;
  flag = FALSE;
  do
    {
      (void) move(dir, &y, &x);
      c_ptr = &cave[y][x];
      if (c_ptr->fopen)
	{
	  if (c_ptr->cptr > 1)
	    {
	      m_ptr = &m_list[c_ptr->cptr];
	      r_ptr = &c_list[m_ptr->mptr];
	      flag = TRUE;
	      monster_name (m_name, m_ptr, r_ptr);
	      if ((randint(MAX_MONS_LEVEL) < r_ptr->level) ||
		  (0x1000 & r_ptr->cdefense))
		{
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
      else
	flag = TRUE;
    }
  while (!flag);
  return(sleep);
}


/* Turn stone to mud, delete wall....			-RAK-	*/
int wall_to_mud(dir, y, x)
int dir, y, x;
{
  int i;
  vtype out_val, tmp_str;
  register int flag;
  int wall;
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  vtype m_name;

  wall = FALSE;
  flag = FALSE;
  do
    {
      (void) move(dir, &y, &x);
      c_ptr = &cave[y][x];
      if (in_bounds(y, x))
	{
	  if ((c_ptr->fval >= 10) && (c_ptr->fval <= 12))
	    {
	      flag = TRUE;
	      (void) twall(y, x, 1, 0);
	      if (test_light(y, x))
		{
		  msg_print("The wall turns into mud.");
		  wall = TRUE;
		}
	    }
	  else if ((c_ptr->tptr != 0) && (!c_ptr->fopen))
	    {
	      flag = TRUE;
	      if (panel_contains(y, x))
		if (test_light(y, x))
		  {
		    inventory[INVEN_MAX] = t_list[c_ptr->tptr];
		    objdes(tmp_str, INVEN_MAX, FALSE);
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
	      if (0x0200 & r_ptr->cdefense)
		{
		  monster_name (m_name, m_ptr, r_ptr);
		  i = mon_take_hit((int)c_ptr->cptr, 100);
		  flag = TRUE;
		  if (i >= 0)
		    {
		      (void) sprintf(out_val, "%s dies in a fit of agony.",
				     m_name);
		      msg_print(out_val);
		    }
		  else
		    {
		      (void) sprintf(out_val, "%s wails out in pain!", m_name);
		      msg_print(out_val);
		    }
		}
	    }
	}
      else
	flag = TRUE;
    }
  while (!flag);
  return(wall);
}


/* Destroy all traps and doors in a given direction	-RAK-	*/
int td_destroy2(dir, y, x)
int dir, y, x;
{
  register int destroy2;
  register cave_type *c_ptr;
  register treasure_type *t_ptr;

  destroy2 = FALSE;
  do
    {
      (void) move(dir, &y, &x);
      c_ptr = &cave[y][x];
      if (c_ptr->tptr != 0)
	{
	  t_ptr = &t_list[c_ptr->tptr];
	  if ((t_ptr->tval == 2) || (t_ptr->tval == 101) ||
	      (t_ptr->tval == 102) || (t_ptr->tval == 104) ||
	      (t_ptr->tval == 105) || (t_ptr->tval == 109))
	    {
	      if (delete_object(y, x))
		{
		  msg_print("There is a bright flash of light!");
		  c_ptr->fopen = TRUE;
		  destroy2 = TRUE;
		}
	    }
	}
    }
  while (cave[y][x].fopen);
  return(destroy2);
}


/* Polymorph a monster					-RAK-	*/
/* NOTE: cannot polymorph a winning creature (BALROG)            */
int poly_monster(dir, y, x)
int dir, y, x;
{
  int dist;
  int flag;
  int poly;
  register cave_type *c_ptr;
  register creature_type *r_ptr;
  register monster_type *m_ptr;
  vtype out_val;
  vtype m_name;

  poly = FALSE;
  flag = FALSE;
  dist = 0;
  do
    {
      (void) move(dir, &y, &x);
      dist++;
      if (dist <= OBJ_BOLT_RANGE)
	{
	  c_ptr = &cave[y][x];
	  if (c_ptr->fopen)
	    {
	      if (c_ptr->cptr > 1)
		{
		  m_ptr = &m_list[c_ptr->cptr];
		  r_ptr = &c_list[m_ptr->mptr];
		  if (randint(MAX_MONS_LEVEL) > r_ptr->level)
		    {
		      flag = TRUE;
		      delete_monster((int)c_ptr->cptr);
		      place_monster(y, x,
			  randint(m_level[MAX_MONS_LEVEL]) - 1 + m_level[0],
				    FALSE);
		      if (panel_contains(y, x))
			if (test_light(y, x))
			  poly = TRUE;
		    }
		  else
		    {
		      monster_name (m_name, m_ptr, r_ptr);
		      (void) sprintf(out_val, "%s is unaffected.", m_name);
		      msg_print(out_val);
		    }
		}
	      else
		flag = TRUE;
	    }
	}
      else
	flag = TRUE;
    }
  while (!flag);
  return(poly);
}


/* Create a wall...					-RAK-	*/
int build_wall(dir, y, x)
int dir, y, x;
{
  register int i;
  int build;
  register cave_type *c_ptr;

  build = FALSE;
  i = 0;
  (void) move(dir, &y, &x);
  while ((cave[y][x].fopen) && (i < 10))
    {
      c_ptr = &cave[y][x];
      if (c_ptr->tptr != 0)
	(void) delete_object(y, x);
      if (c_ptr->cptr > 1)
	/* what happens to this monster ? */
	(void) mon_take_hit((int)c_ptr->cptr, damroll("2d8"));
      c_ptr->fval  = rock_wall2.ftval;
      c_ptr->fopen = rock_wall2.ftopen;
      c_ptr->fm = FALSE;
      if (test_light(y, x))
	lite_spot(y, x);
      i++;
      build = TRUE;
      (void) move(dir, &y, &x);
    }
  return(build);
}


/* Replicate a creature					-RAK-	*/
int clone_monster(dir, y, x)
int dir, y, x;
{
  register int flag;
  register int clone;
  register cave_type *c_ptr;

  flag = FALSE;
  clone = FALSE;
  do
    {
      (void) move(dir, &y, &x);
      c_ptr = &cave[y][x];
      if (c_ptr->cptr > 1)
	{
	  multiply_monster(y, x, (int)m_list[c_ptr->cptr].mptr, FALSE);
	  if (panel_contains(y, x))
	    if (m_list[c_ptr->cptr].ml)
	      clone = TRUE;
	  flag = TRUE;
	}
    }
  while ((cave[y][x].fopen) && (!flag));
  return(clone);
}


/* Move the creature record to a new location		-RAK-	*/
teleport_away(monptr, dis)
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
  while ((!cave[yn][xn].fopen) || (cave[yn][xn].cptr != 0));
  move_rec((int)m_ptr->fy, (int)m_ptr->fx, yn, xn);
  if (test_light((int)m_ptr->fy, (int)m_ptr->fx))
    lite_spot((int)m_ptr->fy, (int)m_ptr->fx);
  m_ptr->fy = yn;
  m_ptr->fx = xn;
  m_ptr->ml = FALSE;
}


/* Teleport player to spell casting creature		-RAK-	*/
teleport_to(ny, nx)
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
  while ((!cave[y][x].fopen) || (cave[y][x].cptr >= 2));
  move_rec(char_row, char_col, y, x);
  for (i = char_row-1; i <= char_row+1; i++)
    for (j = char_col-1; j <= char_col+1; j++)
      {
	c_ptr = &cave[i][j];
	c_ptr->tl = FALSE;
	if (!test_light(i, j))
	  unlite_spot(i, j);
      }
  if (test_light(char_row, char_col))
    lite_spot(char_row, char_col);
  char_row = y;
  char_col = x;
  move_char(5);
  /* light creatures */
  creatures(FALSE);
}


/* Teleport all creatures in a given direction away	-RAK-	*/
int teleport_monster(dir, y, x)
int dir, y, x;
{
  register int flag;
  register int teleport;
  register cave_type *c_ptr;

  flag = FALSE;
  teleport = FALSE;
  do
    {
      (void) move(dir, &y, &x);
      c_ptr = &cave[y][x];
      if (c_ptr->cptr > 1)
	{
	  teleport_away((int)c_ptr->cptr, MAX_SIGHT);
	  teleport = TRUE;
	}
    }
  while ((cave[y][x].fopen) && (!flag));
  return(teleport);
}


/* Delete all creatures within max_sight distance	-RAK-	*/
/* NOTE : Winning creatures cannot be genocided                  */
int mass_genocide()
{
  register int i, j;
  int genocide;
  register monster_type *m_ptr;
  register creature_type *r_ptr;

  genocide = FALSE;
  i = muptr;
  while (i > 0)
    {
      m_ptr = &m_list[i];
      r_ptr = &c_list[m_ptr->mptr];
      j = m_ptr->nptr;
      if (m_ptr->cdis <= MAX_SIGHT)
	if ((r_ptr->cmove & 0x80000000) == 0)
	  {
	    delete_monster(i);
	    genocide = TRUE;
	  }
      i = j;
    }
  return(genocide);
}


/* Delete all creatures of a given type from level.	-RAK-	*/
/* This does not keep creatures of type from appearing later.    */
/* NOTE : Winning creatures can not be genocided. */
int genocide()
{
  register int i, j;
  char typ;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  vtype out_val;

  i = muptr;
  if (get_com("What kind of creature shall be exterminated?", &typ))
    while (i > 0)
      {
	m_ptr = &m_list[i];
	r_ptr = &c_list[m_ptr->mptr];
	j = m_ptr->nptr;
	if (typ == c_list[m_ptr->mptr].cchar)
	  if ((r_ptr->cmove & 0x80000000) == 0)
	    delete_monster(i);
	  else
	    {
	      /* genocide is a powerful spell, so we will let the player
		 know the names of the creatures he did not destroy,
		 this message makes no sense otherwise */
	      (void) sprintf(out_val, "The %s is unaffected.", r_ptr->name);
	      msg_print(out_val);
	    }
	i = j;
      }
  return(TRUE);
}


/* Change speed of any creature player can see....	-RAK-	*/
/* NOTE: cannot slow a winning creature (BALROG)                 */
int speed_monsters(spd)
int spd;
{
  register int i, j;
  int speed;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  vtype out_val;
  vtype m_name;

  i = muptr;
  speed = FALSE;
  while (i > 0)
    {
      m_ptr = &m_list[i];
      j = m_ptr->nptr;
      if (m_ptr->ml)
	{
	  r_ptr = &c_list[m_ptr->mptr];
	  monster_name (m_name, m_ptr, r_ptr);
	  if (spd > 0)
	    {
	      m_ptr->cspeed += spd;
	      m_ptr->csleep = 0;
	      speed = TRUE;
	      (void) sprintf (out_val, "%s starts moving faster.", m_name);
	      msg_print (out_val);
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
	      (void) sprintf(out_val, "%s is unaffected.", m_name);
	      msg_print(out_val);
	    }
	}
      i = j;
    }
  return(speed);
}


/* Sleep any creature that player can see		-RAK-	*/
int sleep_monsters2()
{
  register int i, j;
  int sleep;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  vtype out_val;
  vtype m_name;

  i = muptr;
  sleep = FALSE;
  while (i > 0)
    {
      m_ptr = &m_list[i];
      r_ptr = &c_list[m_ptr->mptr];
      monster_name (m_name, m_ptr, r_ptr);
      j = m_ptr->nptr;
      if (m_ptr->ml)
	{
	  if ((randint(MAX_MONS_LEVEL) < r_ptr->level) ||
	      (0x1000 & r_ptr->cdefense))
	    {
	      (void) sprintf(out_val, "%s is unaffected.", m_name);
	      msg_print(out_val);
	    }
	  else
	    {
	      m_ptr->csleep = 500;
	      (void) sprintf(out_val, "%s falls asleep.", m_name);
	      msg_print(out_val);
	      sleep = TRUE;
	    }
	}
      i = j;
    }
  return(sleep);
}


/* Polymorph any creature that player can see... 	-RAK-	*/
/* NOTE: cannot polymorph a winning creature (BALROG)            */
int mass_poly()
{
  register int i, j;
  int y, x;
  int mass;
  register monster_type *m_ptr;
  register creature_type *r_ptr;

  i = muptr;
  mass = FALSE;
  while (i > 0)
    {
      m_ptr = &m_list[i];
      j = m_ptr->nptr;
      if (m_ptr->cdis < MAX_SIGHT)
	{
	  r_ptr = &c_list[m_ptr->mptr];
	  if ((r_ptr->cmove & 0x80000000) == 0)
	    {
	      y = m_ptr->fy;
	      x = m_ptr->fx;
	      delete_monster(i);
	      place_monster(y, x, randint(m_level[MAX_MONS_LEVEL]) - 1
			    + m_level[0], FALSE);
	      mass = TRUE;
	    }
	}
      i = j;
    }
  return(mass);
}


/* Display evil creatures on current panel		-RAK-	*/
int detect_evil()
{
  register int i;
  int flag;
  register monster_type *m_ptr;
  char temp_str[2];

  flag = FALSE;
  i = muptr;
  while (i > 0)
    {
      m_ptr = &m_list[i];
      if (panel_contains((int)m_ptr->fy, (int)m_ptr->fx))
	if (0x0004 & c_list[m_ptr->mptr].cdefense)
	  {
	    m_ptr->ml = TRUE;
	    temp_str[0] = c_list[m_ptr->mptr].cchar;
	    temp_str[1] = '\0';
	    print(temp_str, (int)m_ptr->fy, (int)m_ptr->fx);
	    flag = TRUE;
	  }
      i = m_list[i].nptr;
    }
  if (flag)
    {
      msg_print("You sense the presence of evil!");
      /* make sure player sees the message */
      msg_print(" ");
      msg_flag = FALSE;
    }
  return(flag);
}


/* Change players hit points in some manner		-RAK-	*/
int hp_player(num, kind)
int num;
char *kind;
{
  register int res;
  register struct misc *m_ptr;

  res = FALSE;
  m_ptr = &py.misc;
  if (num < 0)
    {
      take_hit(num, kind);
      if (m_ptr->chp < 0)
	msg_print("You feel your life slipping away!");
      res = TRUE;
    }
  else if (m_ptr->chp < m_ptr->mhp)
    {
      m_ptr->chp += (double)num;
      if (m_ptr->chp > m_ptr->mhp)
	m_ptr->chp = (double)m_ptr->mhp;
      prt_chp();
      switch(num/5)
	{
	case 0:
	  msg_print("You feel a little better.");
	  break;
	case 1: case 2:
	  msg_print("You feel better.");
	  break;
	case 3: case 4: case 5: case 6:
	  msg_print("You feel much better.");
	  break;
	default:
	  msg_print("You feel very good.");
	  break;
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


/* Cure the players fear 				-RAK-	*/
int remove_fear()
{
  register int remove;
  register struct flags *f_ptr;

  remove = FALSE;
  f_ptr = &py.flags;
  if (f_ptr->afraid > 1)
    {
      f_ptr->afraid = 1;
      remove = TRUE;
    }
  return(remove);
}


/* This is a fun one.  In a given block, pick some walls and	*/
/* turn them into open spots.  Pick some open spots and turn     */
/* them into walls.  An "Earthquake" effect...           -RAK-   */
int earthquake()
{
  register int i, j;
  register cave_type *c_ptr;

  for (i = char_row-8; i <= char_row+8; i++)
    for (j = char_col-8; j <= char_col+8; j++)
      if ((i != char_row) || (j != char_col))
	if (in_bounds(i, j))
	  if (randint(8) == 1)
	    {
	      c_ptr = &cave[i][j];
	      if (c_ptr->tptr != 0)
		(void) delete_object(i, j);
	      if (c_ptr->cptr > 1)
		/* what happens to this monster ? */
		(void) mon_take_hit((int)c_ptr->cptr, damroll("2d8"));
	      if ((c_ptr->fval >= 10) && (c_ptr->fval <= 12))
		{
		  if (next_to4(i, j, 1, 2, -1) > 0)
		    {
		      c_ptr->fval  = corr_floor2.ftval;
		      c_ptr->fopen = corr_floor2.ftopen;
		    }
		  else
		    {
		      c_ptr->fval  = corr_floor1.ftval;
		      c_ptr->fopen = corr_floor1.ftopen;
		    }
		  if (test_light(i, j))
		    unlite_spot(i, j);
		  c_ptr->pl = FALSE;
		  c_ptr->fm = FALSE;
		  if (c_ptr->tl)
		    lite_spot(i, j);
		}
	      else if (set_floor(c_ptr->fval))
		{
		  switch(randint(10))
		    {
		    case 1: case 2: case 3: case 4: case 5:
		      c_ptr->fval  = rock_wall3.ftval;
		      c_ptr->fopen = rock_wall3.ftopen;
		      break;
		    case 6: case 7: case 8:
		      c_ptr->fval  = rock_wall2.ftval;
		      c_ptr->fopen = rock_wall2.ftopen;
		      break;
		    case 9: case 10:
		      c_ptr->fval  = rock_wall1.ftval;
		      c_ptr->fopen = rock_wall1.ftopen;
		      break;
		    }
		  c_ptr->fm = FALSE;
		}
	      if (test_light(i, j))
		lite_spot(i, j);
	    }
  return(TRUE);
}


/* Evil creatures don't like this...                     -RAK-   */
int protect_evil()
{
  register struct flags *f_ptr;

  f_ptr = &py.flags;
  f_ptr->protevil += randint(25) + 3*py.misc.lev;
  return(TRUE);
}


/* Create some high quality mush for the player. 	-RAK-	*/
int create_food()
{
  register cave_type *c_ptr;

  c_ptr = &cave[char_row][char_col];
  if (c_ptr->tptr != 0)
    {
      /* take no action here, don't want to destroy object under player */
      msg_print ("There is already an object under you.");
      /* set reset_flag so that scroll/spell points won't be used */
      reset_flag = TRUE;
    }
  else
    {
      place_object(char_row, char_col);
      t_list[c_ptr->tptr] = mush;
    }
  return (TRUE);
}


/* Attempts to destroy a type of creature.  Success depends on	*/
/* the creatures level VS. the player's level            -RAK-   */
int dispell_creature(cflag, damage)
int cflag;
int damage;
{
  register int i, m_next;
  vtype out_val;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  register struct misc *p_ptr;
  int dispel;
  vtype m_name;

  i = muptr;
  dispel = FALSE;
  while (i > 0)
    {
      m_next = m_list[i].nptr;
      m_ptr = &m_list[i];
      if (m_ptr->ml)
	if (cflag & c_list[m_ptr->mptr].cdefense)
	  {
	    m_ptr->hp -= randint(damage);
	    m_ptr->csleep = 0;
	    r_ptr = &c_list[m_ptr->mptr];
	    monster_name (m_name, m_ptr, r_ptr);
	    if (m_ptr->hp < 0)
	      {
		(void) sprintf(out_val, "%s dissolves!", m_name);
		msg_print(out_val);
		monster_death((int)m_ptr->fy, (int)m_ptr->fx,
			      c_list[m_ptr->mptr].cmove);
		p_ptr = &py.misc;
		p_ptr->exp += ((r_ptr->mexp*(r_ptr->level/p_ptr->lev)) + 0.5);
		delete_monster(i);
	      }
	    else
	      {
		(void) sprintf(out_val, "%s shudders.", m_name);
		msg_print(out_val);
	      }
	    dispel = TRUE;
	  }
      i = m_next;
    }
  return(dispel);
}


/* Attempt to turn (confuse) undead creatures... 	-RAK-	*/
int turn_undead()
{
  register int i;
  int turn_und;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  vtype out_val;
  vtype m_name;

  i = muptr;
  turn_und = FALSE;
  while (i > 0)
    {
      m_ptr = &m_list[i];
      r_ptr = &c_list[m_ptr->mptr];
      if (panel_contains((int)m_ptr->fy, (int)m_ptr->fx))
	if (m_ptr->ml)
	  if (0x0008 & r_ptr->cdefense)
	    {
	      monster_name (m_name, m_ptr, r_ptr);
	      if (((py.misc.lev+1) > r_ptr->level) ||
		  (randint(5) == 1))
		{
		  (void) sprintf(out_val, "%s runs frantically!", m_name);
		  msg_print(out_val);
		  m_ptr->confused = TRUE;
		  turn_und = TRUE;
		}
	      else
		{
		  (void) sprintf(out_val, "%s is unaffected.", m_name);
		  msg_print(out_val);
		}
	    }
      i = m_list[i].nptr;
    }
  return(turn_und);
}


/* Leave a glyph of warding... Creatures will not pass over! -RAK-*/
int warding_glyph()
{
  int i;
  register cave_type *c_ptr;

  c_ptr = &cave[char_row][char_col];
  if (c_ptr->tptr == 0)
    {
      popt(&i);
      c_ptr->tptr = i;
      t_list[i] = scare_monster;
    }
  return(TRUE);
}


/* Lose a strength point.				-RAK-	*/
int lose_str()
{
  if (!py.flags.sustain_str)
    {
      py.stats.cstr = de_statp(py.stats.cstr);
      msg_print("You feel very sick.");
      prt_strength();
      /* adjust misc stats */
      py_bonuses(blank_treasure, 0);
    }
  else
    msg_print("You feel sick for a moment,  it passes.");
  return(TRUE);
}


/* Lose an intelligence point.				-RAK-	*/
int lose_int()
{
  if (!py.flags.sustain_int)
    {
      py.stats.cint = de_statp(py.stats.cint);
      msg_print("You become very dizzy.");
      prt_intelligence();
    }
  else
    msg_print("You become dizzy for a moment,  it passes.");
  return(TRUE);
}


/* Lose a wisdom point.					-RAK-	*/
int lose_wis()
{
  if (!py.flags.sustain_wis)
    {
      py.stats.cwis = de_statp(py.stats.cwis);
      msg_print("You feel very naive.");
      prt_wisdom();
    }
  else
    msg_print("You feel naive for a moment,  it passes.");
  return(TRUE);
}


/* Lose a dexterity point.				-RAK-	*/
int lose_dex()
{
  if (!py.flags.sustain_dex)
    {
      py.stats.cdex = de_statp(py.stats.cdex);
      msg_print("You feel very sore.");
      prt_dexterity();
      /* adjust misc stats */
      py_bonuses(blank_treasure, 0);
    }
  else
    msg_print("You feel sore for a moment,  it passes.");
  return(TRUE);
}


/* Lose a constitution point.				-RAK-	*/
int lose_con()
{
  if (!py.flags.sustain_con)
    {
      py.stats.ccon = de_statp(py.stats.ccon);
      msg_print("You feel very sick.");
      prt_constitution();
    }
  else
    msg_print("You feel sick for a moment,  it passes.");
  return(TRUE);
}


/* Lose a charisma point.				-RAK-	*/
int lose_chr()
{
  if (!py.flags.sustain_chr)
    {
      py.stats.cchr = de_statp(py.stats.cchr);
      msg_print("Your skin starts to itch.");
      prt_charisma();
    }
  else
    msg_print("Your skin starts to itch, but feels better now.");
  return(TRUE);
}


/* Lose experience					-RAK-	*/
lose_exp(amount)
int amount;
{
  register int i, j;
  int av_hp, lose_hp;
  int av_mn, lose_mn;
  register struct misc *m_ptr;
  register class_type *c_ptr;
  int num_known, adjust, num_allowed, num_lose;
  double avg_spells;

  m_ptr = &py.misc;
  if (amount > m_ptr->exp)
    m_ptr->exp = 0;
  else
    m_ptr->exp -= amount;
  i = 1;
  while ((player_exp[i-1]*m_ptr->expfact) <= m_ptr->exp)
    i++;
  j = m_ptr->lev - i;
  while (j > 0)
    {
      av_hp = (int)((double)m_ptr->mhp/(double)m_ptr->lev + 0.5);
      av_mn = (int)((double)m_ptr->mana/(double)m_ptr->lev + 0.5);
      m_ptr->lev--;
      j--;
      lose_hp = randint(av_hp*2-1);
      lose_mn = randint(av_mn*2-1);
      m_ptr->mhp  -= lose_hp;
      m_ptr->mana -= lose_mn;
      if (m_ptr->mhp  < 1)  m_ptr->mhp  = 1;
      if (m_ptr->mana < 0)  m_ptr->mana = 0;

      /* perhaps lose some spells, depending on current int/wis and level */
      c_ptr = &class[m_ptr->pclass];
      if ((c_ptr->mspell) || (c_ptr->pspell))
	{
	  /* count spells known */
	  num_known = 0;
	  for (i = 0; i < 31; i++)
	    if (magic_spell[m_ptr->pclass][i].learned)
	      num_known++;

	  /* calculate number of spells allowed */
	  if (c_ptr->mspell)
	    adjust = int_adj();
	  else
	    adjust = wis_adj();
	  switch (adjust)
	    {
	    case 0: avg_spells = 0.0; break;
	    case 1: avg_spells = 1.0; break;
	    case 2: avg_spells = 1.0; break;
	    case 3: avg_spells = 1.0; break;
	    case 4: avg_spells = 1.5; break;
	    case 5: avg_spells = 1.5; break;
	    case 6: avg_spells = 2.0; break;
	    case 7: avg_spells = 2.5; break;
	    default: avg_spells = 1.0; break;
	    }
	  num_allowed = (int)(m_ptr->lev * avg_spells + 0.5);
	  num_lose = num_known - num_allowed;

	  /* forget spells until both:
	     number known is less than or equal to number allowed
	     and highest spell level is lower than or equal to player level */
	  i = 30;
	  while (((magic_spell[m_ptr->pclass][i].slevel > m_ptr->lev)
		  || (num_lose > 0))
		 && (i >= 0))
	    {
	      if (magic_spell[m_ptr->pclass][i].learned)
		{
		  magic_spell[m_ptr->pclass][i].learned = FALSE;
		  num_lose--;
		  if (c_ptr->mspell)
		    msg_print("You have forgotten a magic spell!");
		  else
		    msg_print("You have forgotten a prayer!");
		}
	      i--;
	    }
	}
    }
  if (m_ptr->chp > m_ptr->mhp)
    m_ptr->chp = (double)m_ptr->mhp;
  if (m_ptr->cmana > m_ptr->mana)
    m_ptr->cmana = (double)m_ptr->mana;
  (void) strcpy(m_ptr->title, player_title[m_ptr->pclass][m_ptr->lev-1]);
  prt_experience();
  prt_mhp();
  prt_chp();
  prt_cmana();
  prt_level();
  prt_title();
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
      f_ptr->poisoned = f_ptr->poisoned / 2.0;
      if (f_ptr->poisoned < 1)  f_ptr->poisoned = 1;
      slow = TRUE;
      msg_print("The effects of the poison has been reduced.");
    }
  return(slow);
}


/* Bless 						-RAK-	*/
int bless(amount)
int amount;
{
  py.flags.blessed += amount;
  return(TRUE);
}


/* Detect Invisible for period of time			-RAK-	*/
detect_inv2(amount)
int amount;
{
  py.flags.detect_inv += amount;
}


replace_spot(y, x, typ)
int y, x, typ;
{
  register cave_type *c_ptr;

  c_ptr = &cave[y][x];
  switch(typ)
    {
    case 1: case 2: case 3:
      c_ptr->fval  = corr_floor1.ftval;
      c_ptr->fopen = corr_floor1.ftopen;
      break;
    case 4: case 7: case 10:
      c_ptr->fval  = rock_wall1.ftval;
      c_ptr->fopen = rock_wall1.ftopen;
      break;
    case 5: case 8: case 11:
      c_ptr->fval  = rock_wall2.ftval;
      c_ptr->fopen = rock_wall2.ftopen;
      break;
    case 6: case 9: case 12:
      c_ptr->fval  = rock_wall3.ftval;
      c_ptr->fopen = rock_wall3.ftopen;
      break;
    }
  c_ptr->pl = FALSE;
  c_ptr->fm = FALSE;
  if (c_ptr->tptr != 0)
    (void) delete_object(y, x);
  if (c_ptr->cptr > 1)
    delete_monster((int)c_ptr->cptr);
}


/* The spell of destruction...				-RAK-	*/
/* NOTE : Winning creatures that are deleted will be considered  */
/*        as teleporting to another level.  This will NOT win the*/
/*        game...                                                */
int destroy_area(y, x)
register int y, x;
{
  register int i, j, k;

  if (dun_level > 0)
    {
      for (i = (y-15); i <= (y+15); i++)
	for (j = (x-15); j <= (x+15); j++)
	  if (in_bounds(i, j))
	    if (cave[i][j].fval != 15)
	      {
		k = distance(i, j, y, x);
		if (k < 13)
		  replace_spot(i, j, randint(6));
		else if (k < 16)
		  replace_spot(i, j, randint(9));
	      }
    }
  msg_print("There is a searing blast of light!");
  py.flags.blind += 10 + randint(10);
  return(TRUE);
}


/* Enchants a plus onto an item...			-RAK-	*/
int enchant(plusses)
worlint *plusses;
{
  register int chance;
  register int res;

  chance = 0;
  res = FALSE;
  if (*plusses > 0)
    switch(*plusses)
      {
      case 1:  chance = 040; break;
      case 2:  chance = 100; break;
      case 3:  chance = 200; break;
      case 4:  chance = 400; break;
      case 5:  chance = 600; break;
      case 6:  chance = 700; break;
      case 7:  chance = 800; break;
      case 8:  chance = 900; break;
      case 9:  chance = 950; break;
      default: chance = 995; break;
      }
  if (randint(1000) > chance)
    {
      *plusses += 1;
      res = TRUE;
    }
  return(res);
}


/* Removes curses from items in inventory		-RAK-	*/
int remove_curse()
{
  register int i;
  register int remove;
  register treasure_type *i_ptr;

  remove = FALSE;
  for (i = 22; i <= 31; i++)
    {
      i_ptr = &inventory[i];
      if (0x80000000 & i_ptr->flags)
	{
	  i_ptr->flags &= 0x7FFFFFFF;
	  py_bonuses(blank_treasure, 0);
	  remove = TRUE;
	}
    }
  return(remove);
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
      msg_print("You feel your life energies returning...");
      m_ptr->exp = m_ptr->max_exp;
      prt_experience();
    }
  return(restore);
}
