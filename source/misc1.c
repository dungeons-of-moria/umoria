/* misc1.c: misc utility and initialization code, magic objects code

   Copyright (c) 1989 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#ifdef Pyramid
#include <sys/time.h>
#else
#include <time.h>
#endif
#if !defined(GEMDOS) && !defined(MAC)
#ifndef VMS
#include <sys/types.h>
#else
#include <types.h>
#endif
#endif

#include "constant.h"
#include "config.h"
#include "types.h"
#include "externs.h"

#ifdef USG
#ifndef ATARIST_MWC
#include <string.h>
#else
#include "string.h"
#endif
#else
#include <strings.h>
#endif

#if defined(LINT_ARGS)
static void compact_monsters(void);
static void compact_objects(void);
#endif

#if !defined(ATARIST_MWC) && !defined(MAC)
long time();
#endif
struct tm *localtime();


/* gets a new random seed for the random number generator */
void init_seeds(seed)
int32u seed;
{
  register int32u clock;

  if (seed == 0)
#ifdef MAC
    clock = time((time_t *)0);
#else
    clock = time((long *)0);
#endif
  else
    clock = seed;
  randes_seed = (unsigned int)clock;

  clock += 8762;
  town_seed = (unsigned int)clock;

  clock += 113452L;
  set_rnd_seed(clock);
  /* make it a little more random */
  for (clock = randint(100); clock != 0; clock--)
    (void) rnd();
}

/* holds the previous rnd state */
static int32u old_seed;

/* change to different random number generator state */
void set_seed(seed)
int32u seed;
{
  old_seed = get_rnd_seed ();

  /* want reproducible state here */
  set_rnd_seed (seed);
}


/* restore the normal random generator state */
void reset_seed()
{
  set_rnd_seed (old_seed);
}


/* Check the day-time strings to see if open		-RAK-	*/
int check_time()
{
#ifdef MORIA_HOU
  long clock;
  register struct tm *tp;

#ifdef MAC
  clock = time((time_t *)0);
#else
  clock = time((long *)0);
#endif
  tp = localtime(&clock);
  if (days[tp->tm_wday][tp->tm_hour+4] == 'X')
    return TRUE;
  else
    return FALSE;
#else
  return TRUE;
#endif
}


/* Generates a random integer x where 1<=X<=MAXVAL	-RAK-	*/
int randint(maxval)
int maxval;
{
  register long randval;

  randval = rnd ();
  return ((randval % maxval) + 1);
}

/* Generates a random integer number of NORMAL distribution -RAK-*/
int randnor(mean, stand)
int mean, stand;
{
  register int tmp, offset, low, iindex, high;

#if 0
  /* alternate randnor code, slower but much smaller since no table */
  /* 2 per 1,000,000 will be > 4*SD, max is 5*SD */
  tmp = damroll(8, 99);	 /* mean 400, SD 81 */
  tmp = (tmp - 400) * stand / 81;
  return tmp + mean;
#endif

  tmp = randint(MAX_SHORT);

  /* off scale, assign random value between 4 and 5 times SD */
  if (tmp == MAX_SHORT)
    {
      offset = 4 * stand + randint(stand);

      /* one half are negative */
      if (randint(2) == 1)
	offset = -offset;

      return mean + offset;
    }

  /* binary search normal normal_table to get index that matches tmp */
  /* this takes up to 8 iterations */
  low = 0;
  iindex = NORMAL_TABLE_SIZE >> 1;
  high = NORMAL_TABLE_SIZE;
  while (TRUE)
    {
      if ((normal_table[iindex] == tmp) || (high == (low+1)))
	break;
      if (normal_table[iindex] > tmp)
	{
	  high = iindex;
	  iindex = low + ((iindex - low) >> 1);
	}
      else
	{
	  low = iindex;
	  iindex = iindex + ((high - iindex) >> 1);
	}
    }

  /* might end up one below target, check that here */
  if (normal_table[iindex] < tmp)
    iindex = iindex + 1;

  /* normal_table is based on SD of 64, so adjust the index value here,
     round the half way case up */
  offset = ((stand * iindex) + (NORMAL_TABLE_SD >> 1)) / NORMAL_TABLE_SD;

  /* one half should be negative */
  if (randint(2) == 1)
    offset = -offset;

  return mean + offset;
}


/* Returns position of first set bit			-RAK-	*/
/*     and clears that bit */
int bit_pos(test)
int32u *test;
{
  register int i;
  register int32u mask = 0x1;

  for (i = 0; i < sizeof(*test)*8; i++) {
    if (*test & mask) {
      *test &= ~mask;
      return(i);
    }
    mask <<= 1;
  }

  /* no one bits found */
  return(-1);
}

/* Checks a co-ordinate for in bounds status		-RAK-	*/
int in_bounds(y, x)
int y, x;
{
  if ((y > 0) && (y < cur_height-1) && (x > 0) && (x < cur_width-1))
    return(TRUE);
  else
    return(FALSE);
}


/* Calculates current boundaries				-RAK-	*/
void panel_bounds()
{
  panel_row_min = panel_row*(SCREEN_HEIGHT/2);
  panel_row_max = panel_row_min + SCREEN_HEIGHT - 1;
  panel_row_prt = panel_row_min - 1;
  panel_col_min = panel_col*(SCREEN_WIDTH/2);
  panel_col_max = panel_col_min + SCREEN_WIDTH - 1;
  panel_col_prt = panel_col_min - 13;
}


/* Given an row (y) and col (x), this routine detects  -RAK-	*/
/* when a move off the screen has occurred and figures new borders.
   Force forcses the panel bounds to be recalculated, useful for 'W'here. */
int get_panel(y, x, force)
int y, x, force;
{
  register int prow, pcol;
  register int panel;

  prow = panel_row;
  pcol = panel_col;
  if (force || (y < panel_row_min + 2) || (y > panel_row_max - 2))
    {
      prow = ((y - SCREEN_HEIGHT/4)/(SCREEN_HEIGHT/2));
      if (prow > max_panel_rows)
	prow = max_panel_rows;
      else if (prow < 0)
	prow = 0;
    }
  if (force || (x < panel_col_min + 3) || (x > panel_col_max - 3))
    {
      pcol = ((x - SCREEN_WIDTH/4)/(SCREEN_WIDTH/2));
      if (pcol > max_panel_cols)
	pcol = max_panel_cols;
      else if (pcol < 0)
	pcol = 0;
    }
  if ((prow != panel_row) || (pcol != panel_col))
    {
      panel_row = prow;
      panel_col = pcol;
      panel_bounds();
      panel = TRUE;
      /* stop movement if any */
      if (find_bound)
	end_find();
    }
  else
    panel = FALSE;
  return(panel);
}


/* Tests a given point to see if it is within the screen -RAK-	*/
/* boundaries.							  */
int panel_contains(y, x)
register int y, x;
{
  if ((y >= panel_row_min) && (y <= panel_row_max) &&
      (x >= panel_col_min) && (x <= panel_col_max))
    return (TRUE);
  else
    return (FALSE);
}


/* Distance between two points				-RAK-	*/
int distance(y1, x1, y2, x2)
int y1, x1, y2, x2;
{
  register int dy, dx;

  dy = y1 - y2;
  if (dy < 0)
    dy = -dy;
  dx = x1 - x2;
  if (dx < 0)
    dx = -dx;

  return ((((dy + dx) << 1) - (dy > dx ? dx : dy)) >> 1);
}

/* Checks points north, south, east, and west for a wall -RAK-	*/
/* note that y,x is always in_bounds(), i.e. 0 < y < cur_height-1, and
   0 < x < cur_width-1	*/
int next_to_walls(y, x)
register int y, x;
{
  register int i;
  register cave_type *c_ptr;

  i = 0;
  c_ptr = &cave[y-1][x];
  if (c_ptr->fval >= MIN_CAVE_WALL)
    i++;
  c_ptr = &cave[y+1][x];
  if (c_ptr->fval >= MIN_CAVE_WALL)
    i++;
  c_ptr = &cave[y][x-1];
  if (c_ptr->fval >= MIN_CAVE_WALL)
    i++;
  c_ptr = &cave[y][x+1];
  if (c_ptr->fval >= MIN_CAVE_WALL)
    i++;

  return(i);
}


/* Checks all adjacent spots for corridors		-RAK-	*/
/* note that y, x is always in_bounds(), hence no need to check that
   j, k are in_bounds(), even if they are 0 or cur_x-1 is still works */
int next_to_corr(y, x)
register int y, x;
{
  register int k, j, i;
  register cave_type *c_ptr;

  i = 0;
  for (j = y - 1; j <= (y + 1); j++)
    for (k = x - 1; k <= (x + 1); k++)
      {
	c_ptr = &cave[j][k];
	/* should fail if there is already a door present */
	if (c_ptr->fval == CORR_FLOOR
	    && (c_ptr->tptr == 0 || t_list[c_ptr->tptr].tval < TV_MIN_DOORS))
	  i++;
      }
  return(i);
}


/* generates damage for 2d6 style dice rolls */
int damroll(num, sides)
int num, sides;
{
  register int i, sum = 0;

  for (i = 0; i < num; i++)
    sum += randint(sides);
  return(sum);
}

int pdamroll(array)
int8u *array;
{
  return damroll((int)array[0], (int)array[1]);
}


/* A simple, fast, integer-based line-of-sight algorithm.  By Joseph Hall,
   4116 Brewster Drive, Raleigh NC 27606.  Email to jnh@ecemwl.ncsu.edu.

   Returns TRUE if a line of sight can be traced from x0, y0 to x1, y1.

   The LOS begins at the center of the tile [x0, y0] and ends at
   the center of the tile [x1, y1].  If los() is to return TRUE, all of
   the tiles this line passes through must be transparent, WITH THE
   EXCEPTIONS of the starting and ending tiles.

   We don't consider the line to be "passing through" a tile if
   it only passes across one corner of that tile. */

/* Because this function uses (short) ints for all calculations, overflow
   may occur if deltaX and deltaY exceed 90. */

int los(fromY, fromX, toY, toX)
int fromY, fromX, toY, toX;
{
  register int tmp, deltaX, deltaY;

  deltaX = toX - fromX;
  deltaY = toY - fromY;

  /* Adjacent? */
  if ((deltaX < 2) && (deltaX > -2) && (deltaY < 2) && (deltaY > -2))
    return TRUE;

  /* Handle the cases where deltaX or deltaY == 0. */
  if (deltaX == 0)
    {
      register int p_y;	/* y position -- loop variable	*/

      if (deltaY < 0)
	{
	  tmp = fromY;
	  fromY = toY;
	  toY = tmp;
	}
      for (p_y = fromY + 1; p_y < toY; p_y++)
	if (cave[p_y][fromX].fval >= MIN_CLOSED_SPACE)
	  return FALSE;
      return TRUE;
    }
  else if (deltaY == 0)
    {
      register int px;	/* x position -- loop variable	*/

      if (deltaX < 0)
	{
	  tmp = fromX;
	  fromX = toX;
	  toX = tmp;
	}
      for (px = fromX + 1; px < toX; px++)
	if (cave[fromY][px].fval >= MIN_CLOSED_SPACE)
	  return FALSE;
      return TRUE;
    }

  /* Now, we've eliminated all the degenerate cases.
     In the computations below, dy (or dx) and m are multiplied by a
     scale factor, scale = abs(deltaX * deltaY * 2), so that we can use
     integer arithmetic. */

  {
    register int px,	/* x position				*/
     p_y,		/* y position				*/
     scale2;		/* above scale factor / 2		*/
    int scale,		/* above scale factor			*/
     xSign,		/* sign of deltaX			*/
     ySign,		/* sign of deltaY			*/
     m;			/* slope or 1/slope of LOS		*/

    scale2 = abs(deltaX * deltaY);
    scale = scale2 << 1;
    xSign = (deltaX < 0) ? -1 : 1;
    ySign = (deltaY < 0) ? -1 : 1;

    /* Travel from one end of the line to the other, oriented along
       the longer axis. */

    if (abs(deltaX) >= abs(deltaY))
      {
	register int dy;		/* "fractional" y position	*/
	/* We start at the border between the first and second tiles,
	   where the y offset = .5 * slope.  Remember the scale
	   factor.  We have:

	   m = deltaY / deltaX * 2 * (deltaY * deltaX)
	     = 2 * deltaY * deltaY. */

	dy = deltaY * deltaY;
	m = dy << 1;
	px = fromX + xSign;

	/* Consider the special case where slope == 1. */
	if (dy == scale2)
	  {
	    p_y = fromY + ySign;
	    dy -= scale;
	  }
	else
	  p_y = fromY;

	while (toX - px)
	  {
	    if (cave[p_y][px].fval >= MIN_CLOSED_SPACE)
	      return FALSE;

	    dy += m;
	    if (dy < scale2)
	      px += xSign;
	    else if (dy > scale2)
	      {
		p_y += ySign;
		if (cave[p_y][px].fval >= MIN_CLOSED_SPACE)
		  return FALSE;
		px += xSign;
		dy -= scale;
	      }
	    else
	      {
		/* This is the case, dy == scale2, where the LOS
		   exactly meets the corner of a tile. */
		px += xSign;
		p_y += ySign;
		dy -= scale;
	      }
	  }
	return TRUE;
      }
    else
      {
	register int dx;		/* "fractional" x position	*/
	dx = deltaX * deltaX;
	m = dx << 1;

	p_y = fromY + ySign;
	if (dx == scale2)
	  {
	    px = fromX + xSign;
	    dx -= scale;
	  }
	else
	  px = fromX;

	while (toY - p_y)
	  {
	    if (cave[p_y][px].fval >= MIN_CLOSED_SPACE)
	      return FALSE;
	    dx += m;
	    if (dx < scale2)
	      p_y += ySign;
	    else if (dx > scale2)
	      {
		px += xSign;
		if (cave[p_y][px].fval >= MIN_CLOSED_SPACE)
		  return FALSE;
		p_y += ySign;
		dx -= scale;
	      }
	    else
	      {
		px += xSign;
		p_y += ySign;
		dx -= scale;
	      }
	  }
	return TRUE;
      }
  }
}


/* Returns symbol for given row, column			-RAK-	*/
unsigned char loc_symbol(y, x)
int y, x;
{
  register cave_type *cave_ptr;
  register struct flags *f_ptr;

  cave_ptr = &cave[y][x];
  f_ptr = &py.flags;

  if ((cave_ptr->cptr == 1) && (!find_flag || find_prself))
    return '@';
  else if (f_ptr->status & PY_BLIND)
    return ' ';
  else if ((f_ptr->image > 0) && (randint (12) == 1))
    return randint (95) + 31;
  else if ((cave_ptr->cptr > 1) && (m_list[cave_ptr->cptr].ml))
    return c_list[m_list[cave_ptr->cptr].mptr].cchar;
  else if (!cave_ptr->pl && !cave_ptr->tl && !cave_ptr->fm)
    return ' ';
  else if ((cave_ptr->tptr != 0)
	   && (t_list[cave_ptr->tptr].tval != TV_INVIS_TRAP))
    return t_list[cave_ptr->tptr].tchar;
  else if (cave_ptr->fval <= MAX_CAVE_FLOOR)
    {
#ifdef MSDOS
      return floorsym;
#else
      return '.';
#endif
    }
  else if (cave_ptr->fval == GRANITE_WALL || cave_ptr->fval == BOUNDARY_WALL
	   || highlight_seams == FALSE)
    {
#ifdef MSDOS
      return wallsym;
#else
#ifndef ATARIST_MWC
      return '#';
#else
      return (unsigned char)240;
#endif
#endif
    }
  else	/* Originally set highlight bit, but that is not portable, now use
	   the percent sign instead. */
    {
      return '%';
    }
}


/* Tests a spot for light or field mark status		-RAK-	*/
int test_light(y, x)
int y, x;
{
  register cave_type *cave_ptr;

  cave_ptr = &cave[y][x];
  if (cave_ptr->pl || cave_ptr->tl || cave_ptr->fm)
    return(TRUE);
  else
    return(FALSE);
}


/* Prints the map of the dungeon			-RAK-	*/
void prt_map()
{
  register int i, j, k;
  register unsigned char tmp_char;

  k = 0;
  for (i = panel_row_min; i <= panel_row_max; i++)  /* Top to bottom */
    {
      k++;
      erase_line (k, 13);
      for (j = panel_col_min; j <= panel_col_max; j++)	/* Left to right */
	{
	  tmp_char = loc_symbol(i, j);
	  if (tmp_char != ' ')
	    print(tmp_char, i, j);
	}
    }
}


/* Compact monsters					-RAK-	*/
static void compact_monsters()
{
  register int i;
  int cur_dis, delete_any;
  register monster_type *mon_ptr;

  msg_print("Compacting monsters...");

  cur_dis = 66;
  delete_any = FALSE;
  do
    {
      for (i = mfptr - 1; i >= MIN_MONIX; i--)
	{
	  mon_ptr = &m_list[i];
	  if ((cur_dis > mon_ptr->cdis) && (randint(3) == 1))
	    {
	      /* in case this is called from within creatures(), this is a
		 horrible hack, the m_list/creatures() code needs to be
		 rewritten */
	      if (hack_monptr < i)
		{
		  delete_monster(i);
		  delete_any = TRUE;
		}
	      else
		/* fix1_delete_monster() does not decrement mfptr, so
		   don't set delete_any if this was called */
		fix1_delete_monster(i);
	    }
	}
      if (!delete_any)
	{
	  cur_dis -= 6;
	  /* can't do anything else but abort, if can't delete any monsters */
	  if (cur_dis < 0)
	    abort();
	}
    }
  while (!delete_any);
}


/* Add to the players food time				-RAK-	*/
void add_food(num)
int num;
{
  register struct flags *p_ptr;

  p_ptr = &py.flags;
  if (p_ptr->food < 0)	p_ptr->food = 0;
  p_ptr->food += num;
  if (p_ptr->food > PLAYER_FOOD_MAX)
    {
      msg_print("You are bloated from overeating.");
      p_ptr->slow += (p_ptr->food - PLAYER_FOOD_MAX) / 50;
      p_ptr->food = PLAYER_FOOD_MAX + p_ptr->slow;
    }
  else if (p_ptr->food > PLAYER_FOOD_FULL)
    msg_print("You are full.");
}


/* Returns a pointer to next free space			-RAK-	*/
int popm()
{
  if (mfptr == MAX_MALLOC)
    compact_monsters();
  return (mfptr++);
}


/* Gives Max hit points					-RAK-	*/
int max_hp(array)
int8u *array;
{
  return(array[0] * array[1]);
}


/* Places a monster at given location			-RAK-	*/
void place_monster(y, x, z, slp)
register int y, x, z;
int slp;
{
  register int cur_pos;
  register monster_type *mon_ptr;

  cur_pos = popm();
  mon_ptr = &m_list[cur_pos];
  mon_ptr->fy = y;
  mon_ptr->fx = x;
  mon_ptr->mptr = z;
  if (c_list[z].cdefense & CD_MAX_HP)
    mon_ptr->hp = max_hp(c_list[z].hd);
  else
    mon_ptr->hp = pdamroll(c_list[z].hd);
  /* the c_list speed value is 10 greater, so that it can be a int8u */
  mon_ptr->cspeed = c_list[z].speed - 10 + py.flags.speed;
  mon_ptr->stunned = 0;
  mon_ptr->cdis = distance(char_row, char_col,y,x);
  mon_ptr->ml = FALSE;
  cave[y][x].cptr = cur_pos;
  if (slp)
    {
      if (c_list[z].sleep == 0)
	mon_ptr->csleep = 0;
      else
	mon_ptr->csleep = (c_list[z].sleep * 2) +
	  randint((int)c_list[z].sleep*10);
    }
  else
    mon_ptr->csleep = 0;
}


/* Places a monster at given location			-RAK-	*/
void place_win_monster()
{
  register int y, x, cur_pos;
  register monster_type *mon_ptr;

  if (!total_winner)
    {
      cur_pos = popm();
      mon_ptr = &m_list[cur_pos];
      do
	{
	  y = randint(cur_height-2);
	  x = randint(cur_width-2);
	}
      while ((cave[y][x].fval >= MIN_CLOSED_SPACE) || (cave[y][x].cptr != 0)
	     || (cave[y][x].tptr != 0) ||
	      (distance(y,x,char_row, char_col) <= MAX_SIGHT));
      mon_ptr->fy = y;
      mon_ptr->fx = x;
      mon_ptr->mptr = randint(WIN_MON_TOT) - 1 + m_level[MAX_MONS_LEVEL];
      if (c_list[mon_ptr->mptr].cdefense & CD_MAX_HP)
	mon_ptr->hp = max_hp(c_list[mon_ptr->mptr].hd);
      else
	mon_ptr->hp = pdamroll(c_list[mon_ptr->mptr].hd);
      /* the c_list speed value is 10 greater, so that it can be a int8u */
      mon_ptr->cspeed = c_list[mon_ptr->mptr].speed - 10 + py.flags.speed;
      mon_ptr->stunned = 0;
      mon_ptr->cdis = distance(char_row, char_col,y,x);
      cave[y][x].cptr = cur_pos;
      mon_ptr->csleep = 0;
    }
}


/* Return a monster suitable to be placed at a given level.  This makes
   high level monsters (up to the given level) slightly more common than
   low level monsters at any given level.   -CJS- */
int get_mons_num (level)
int level;
{
  register int i, j, num;

  if (level == 0)
    i = randint (m_level[0]) - 1;
  else
    {
      if (level > MAX_MONS_LEVEL)
	level = MAX_MONS_LEVEL;
      if (randint (MON_NASTY) == 1)
	{
	  i = randnor (0, 4);
	  level = level + abs(i) + 1;
	  if (level > MAX_MONS_LEVEL)
	    level = MAX_MONS_LEVEL;
	}
      else
	{
	  /* This code has been added to make it slightly more likely to
	     get the higher level monsters. Originally a uniform
	     distribution over all monsters of level less than or equal to the
	     dungeon level. This distribution makes a level n monster occur
	     approx 2/n% of the time on level n, and 1/n*n% are 1st level. */

	  num = m_level[level] - m_level[0];
	  i = randint (num) - 1;
	  j = randint (num) - 1;
	  if (j > i)
	    i = j;
	  level = c_list[i + m_level[0]].level;
	}
      i = randint(m_level[level]-m_level[level-1]) - 1 + m_level[level-1];
    }
  return i;
}


/* Allocates a random monster				-RAK-	*/
void alloc_monster(num, dis, slp)
int num, dis;
int slp;
{
  register int y, x, i;

  for (i = 0; i < num; i++)
    {
      do
	{
	  y = randint(cur_height-2);
	  x = randint(cur_width-2);
	}
      while (cave[y][x].fval >= MIN_CLOSED_SPACE || (cave[y][x].cptr != 0) ||
	     (distance(y, x, char_row, char_col) <= dis));
      place_monster(y, x, get_mons_num(dun_level), slp);
    }
}


/* Places creature adjacent to given location		-RAK-	*/
int summon_monster(y, x, slp)
int *y, *x;
int slp;
{
  register int i, j, k;
  int l, summon;
  register cave_type *cave_ptr;

  i = 0;
  summon = FALSE;
  l = get_mons_num (dun_level + MON_SUMMON_ADJ);
  do
    {
      j = *y - 2 + randint(3);
      k = *x - 2 + randint(3);
      if (in_bounds(j, k))
	{
	  cave_ptr = &cave[j][k];
	  if (cave_ptr->fval <= MAX_OPEN_SPACE && (cave_ptr->cptr == 0))
	    {
	      place_monster(j, k, l, slp);
	      summon = TRUE;
	      i = 9;
	      *y = j;
	      *x = k;
	    }
	}
      i++;
    }
  while (i <= 9);
  return(summon);
}


/* Places undead adjacent to given location		-RAK-	*/
int summon_undead(y, x)
int *y, *x;
{
  register int i, j, k;
  int l, m, ctr, summon;
  register cave_type *cave_ptr;

  i = 0;
  summon = FALSE;
  l = m_level[MAX_MONS_LEVEL];
  do
    {
      m = randint(l) - 1;
      ctr = 0;
      do
	{
	  if (c_list[m].cdefense & CD_UNDEAD)
	    {
	      ctr = 20;
	      l	 = 0;
	    }
	  else
	    {
	      m++;
	      if (m > l)
		ctr = 20;
	      else
		ctr++;
	    }
	}
      while (ctr <= 19);
    }
  while(l != 0);
  do
    {
      j = *y - 2 + randint(3);
      k = *x - 2 + randint(3);
      if (in_bounds(j, k))
	{
	  cave_ptr = &cave[j][k];
	  if (cave_ptr->fval <= MAX_OPEN_SPACE && (cave_ptr->cptr == 0))
	    {
	      place_monster(j, k, m, FALSE);
	      summon = TRUE;
	      i = 9;
	      *y = j;
	      *x = k;
	    }
	}
      i++;
    }
  while(i <= 9);
  return(summon);
}


/* If too many objects on floor level, delete some of them-RAK-	*/
static void compact_objects()
{
  register int i, j;
  int ctr, cur_dis, chance;
  register cave_type *cave_ptr;

  msg_print("Compacting objects...");

  ctr = 0;
  cur_dis = 66;
  do
    {
      for (i = 0; i < cur_height; i++)
	for (j = 0; j < cur_width; j++)
	  {
	    cave_ptr = &cave[i][j];
	    if ((cave_ptr->tptr != 0)
		&& (distance(i, j, char_row, char_col) > cur_dis))
	      {
		switch(t_list[cave_ptr->tptr].tval)
		  {
		  case TV_VIS_TRAP:
		    chance = 15;
		    break;
		  case TV_INVIS_TRAP:
		  case TV_RUBBLE:
		  case TV_OPEN_DOOR: case TV_CLOSED_DOOR:
		    chance = 5;
		    break;
		  case TV_UP_STAIR: case TV_DOWN_STAIR:
		  case TV_STORE_DOOR:
		    /* stairs, don't delete them */
		    /* shop doors, don't delete them */
		    chance = 0;
		    break;
		  case TV_SECRET_DOOR: /* secret doors */
		    chance = 3;
		    break;
		  default:
		    chance = 10;
		  }
		if (randint (100) <= chance)
		  {
		    (void) delete_object(i, j);
		    ctr++;
		  }
	      }
	  }
      if (ctr == 0)  cur_dis -= 6;
    }
  while (ctr <= 0);
  if (cur_dis < 66)  prt_map();
}

/* Gives pointer to next free space			-RAK-	*/
int popt()
{
  if (tcptr == MAX_TALLOC)
    compact_objects();
  return (tcptr++);
}


/* Pushs a record back onto free space list		-RAK-	*/
/* Delete_object() should always be called instead, unless the object in
   question is not in the dungeon, e.g. in store1.c and files.c */
void pusht(x)
register int8u x;
{
  register int i, j;

  if (x != tcptr - 1)
    {
      t_list[x] = t_list[tcptr - 1];

      /* must change the tptr in the cave of the object just moved */
      for (i = 0; i < cur_height; i++)
	for (j = 0; j < cur_width; j++)
	  if (cave[i][j].tptr == tcptr - 1)
	    cave[i][j].tptr = x;
    }
  tcptr--;
  invcopy(&t_list[tcptr], OBJ_NOTHING);
}


/* Boolean : is object enchanted	  -RAK- */
int magik(chance)
int chance;
{
  if (randint(100) <= chance)
    return(TRUE);
  else
    return(FALSE);
}


/* Enchant a bonus based on degree desired -RAK- */
int m_bonus(base, max_std, level)
int base, max_std, level;
{
  register int x, stand_dev, tmp;

  stand_dev = (OBJ_STD_ADJ * level / 100) + OBJ_STD_MIN;
  if (stand_dev > max_std)
    stand_dev = max_std;
  /* abs may be a macro, don't call it with randnor as a parameter */
  tmp = randnor(0, stand_dev);
  x = (abs(tmp) / 10) + base;
  if (x < base)
    return(base);
  else
    return(x);
}


/* Chance of treasure having magic abilities		-RAK-	*/
/* Chance increases with each dungeon level			 */
void magic_treasure(x, level)
int x, level;
{
  register inven_type *t_ptr;
  register int chance, special, cursed, i;
  int tmp;

  chance = OBJ_BASE_MAGIC + level;
  if (chance > OBJ_BASE_MAX)
    chance = OBJ_BASE_MAX;
  special = chance / OBJ_DIV_SPECIAL;
  cursed  = (10 * chance) / OBJ_DIV_CURSED;
  t_ptr = &t_list[x];

  /* some objects appear multiple times in the object_list with different
     levels, this is to make the object occur more often, however, for
     consistency, must set the level of these duplicates to be the same
     as the object with the lowest level */

  /* Depending on treasure type, it can have certain magical properties*/
  switch (t_ptr->tval)
    {
    case TV_SHIELD: case TV_HARD_ARMOR: case TV_SOFT_ARMOR:
      if (magik(chance))
	{
	  t_ptr->toac += m_bonus(1, 30, level);
	  if (magik(special))
	    switch(randint(9))
	      {
	      case 1:
		t_ptr->flags |= (TR_RES_LIGHT|TR_RES_COLD|TR_RES_ACID|
				 TR_RES_FIRE);
		t_ptr->name2 = SN_R;
		t_ptr->toac += 5;
		t_ptr->cost += 2500;
		break;
	      case 2:	 /* Resist Acid	  */
		t_ptr->flags |= TR_RES_ACID;
		t_ptr->name2 = SN_RA;
		t_ptr->cost += 1000;
		break;
	      case 3: case 4:	 /* Resist Fire	  */
		t_ptr->flags |= TR_RES_FIRE;
		t_ptr->name2 = SN_RF;
		t_ptr->cost += 600;
		break;
	      case 5: case 6:	/* Resist Cold	 */
		t_ptr->flags |= TR_RES_COLD;
		t_ptr->name2 = SN_RC;
		t_ptr->cost += 600;
		break;
	      case 7: case 8: case 9:  /* Resist Lightning*/
		t_ptr->flags |= TR_RES_LIGHT;
		t_ptr->name2 = SN_RL;
		t_ptr->cost += 500;
		break;
	      }
	}
      else if (magik(cursed))
	{
	  t_ptr->toac -= m_bonus(1, 40, level);
	  t_ptr->cost = 0;
	  t_ptr->flags |= TR_CURSED;
	}
      break;

    case TV_HAFTED: case TV_POLEARM: case TV_SWORD:
      /* always show tohit/todam values if identified */
      t_ptr->ident |= ID_SHOW_HITDAM;
      if (magik(chance))
	{
	  t_ptr->tohit += m_bonus(0, 40, level);
	  t_ptr->todam += m_bonus(0, 40, level);
	  /* the 3*special/2 is needed because weapons are not as common as
	     before change to treasure distribution, this helps keep same
	     number of ego weapons same as before, see also missiles */
	  if (magik(3*special/2))
	    switch(randint(16))
	      {
	      case 1:	/* Holy Avenger	 */
		t_ptr->flags |= (TR_SEE_INVIS|TR_SUST_STAT|TR_SLAY_UNDEAD|
				 TR_SLAY_EVIL|TR_STR);
		t_ptr->tohit += 5;
		t_ptr->todam += 5;
		t_ptr->toac  += randint(4);
		/* the value in p1 is used for strength increase */
		/* p1 is also used for sustain stat */
		t_ptr->p1    = randint(4);
		t_ptr->name2 = SN_HA;
		t_ptr->cost += t_ptr->p1*500;
		t_ptr->cost += 10000;
		break;
	      case 2:	/* Defender	 */
		t_ptr->flags |= (TR_FFALL|TR_RES_LIGHT|TR_SEE_INVIS|TR_FREE_ACT
				 |TR_RES_COLD|TR_RES_ACID|TR_RES_FIRE|
				 TR_REGEN|TR_STEALTH);
		t_ptr->tohit += 3;
		t_ptr->todam += 3;
		t_ptr->toac  += 5 + randint(5);
		t_ptr->name2 = SN_DF;
		/* the value in p1 is used for stealth */
		t_ptr->p1    = randint(3);
		t_ptr->cost += t_ptr->p1*500;
		t_ptr->cost += 7500;
		break;
	      case 3: case 4:	 /* Slay Animal  */
		t_ptr->flags |= TR_SLAY_ANIMAL;
		t_ptr->tohit += 3;
		t_ptr->todam += 3;
		t_ptr->name2 = SN_SA;
		t_ptr->cost += 5000;
		break;
	      case 5: case 6:	/* Slay Dragon	 */
		t_ptr->flags |= TR_SLAY_DRAGON;
		t_ptr->tohit += 3;
		t_ptr->todam += 3;
		t_ptr->name2 = SN_SD;
		t_ptr->cost += 4000;
		break;
	      case 7: case 8:	  /* Slay Evil	   */
		t_ptr->flags |= TR_SLAY_EVIL;
		t_ptr->tohit += 3;
		t_ptr->todam += 3;
		t_ptr->name2 = SN_SE;
		t_ptr->cost += 4000;
		break;
	      case 9: case 10:	 /* Slay Undead	  */
		t_ptr->flags |= (TR_SEE_INVIS|TR_SLAY_UNDEAD);
		t_ptr->tohit += 2;
		t_ptr->todam += 2;
		t_ptr->name2 = SN_SU;
		t_ptr->cost += 3000;
		break;
	      case 11: case 12: case 13:   /* Flame Tongue  */
		t_ptr->flags |= TR_FLAME_TONGUE;
		t_ptr->tohit++;
		t_ptr->todam += 3;
		t_ptr->name2 = SN_FT;
		t_ptr->cost += 2000;
		break;
	      case 14: case 15: case 16:   /* Frost Brand   */
		t_ptr->flags |= TR_FROST_BRAND;
		t_ptr->tohit++;
		t_ptr->todam++;
		t_ptr->name2 = SN_FB;
		t_ptr->cost += 1200;
		break;
	      }
	}
      else if (magik(cursed))
	{
	  t_ptr->tohit -= m_bonus(1, 55, level);
	  t_ptr->todam -= m_bonus(1, 55, level);
	  t_ptr->flags |= TR_CURSED;
	  t_ptr->cost = 0;
	}
      break;

    case TV_BOW:
      /* always show tohit/todam values if identified */
      t_ptr->ident |= ID_SHOW_HITDAM;
      if (magik(chance))
	{
	  t_ptr->tohit += m_bonus(1, 30, level);
	  t_ptr->todam += m_bonus(1, 20, level); /* add damage. -CJS- */
	}
      else if (magik(cursed))
	{
	  t_ptr->tohit -= m_bonus(1, 50, level);
	  t_ptr->todam -= m_bonus(1, 30, level); /* add damage. -CJS- */
	  t_ptr->flags |= TR_CURSED;
	  t_ptr->cost = 0;
	}
      break;

    case TV_DIGGING:
      /* always show tohit/todam values if identified */
      t_ptr->ident |= ID_SHOW_HITDAM;
      if (magik(chance))
	{
	  tmp = randint(3);
	  if (tmp < 3)
	    t_ptr->p1 += m_bonus(0, 25, level);
	  else
	    {
	      /* a cursed digging tool */
	      t_ptr->p1 = -m_bonus(1, 30, level);
	      t_ptr->cost = 0;
	      t_ptr->flags |= TR_CURSED;
	    }
	}
      break;

    case TV_GLOVES:
      if (magik(chance))
	{
	  t_ptr->toac += m_bonus(1, 20, level);
	  if (magik(special))
	    {
	      if (randint(2) == 1)
		{
		  t_ptr->flags |= TR_FREE_ACT;
		  t_ptr->name2 = SN_FREE_ACTION;
		  t_ptr->cost += 1000;
		}
	      else
		{
		  t_ptr->ident |= ID_SHOW_HITDAM;
		  t_ptr->tohit += 1 + randint(3);
		  t_ptr->todam += 1 + randint(3);
		  t_ptr->name2 = SN_SLAYING;
		  t_ptr->cost += (t_ptr->tohit+t_ptr->todam)*250;
		}
	    }
	}
      else if (magik(cursed))
	{
	  if (magik(special))
	    {
	      if (randint(2) == 1)
		{
		  t_ptr->flags |= TR_DEX;
		  t_ptr->name2 = SN_CLUMSINESS;
		}
	      else
		{
		  t_ptr->flags |= TR_STR;
		  t_ptr->name2 = SN_WEAKNESS;
		}
	      t_ptr->ident |= ID_SHOW_P1;
	      t_ptr->p1   = -m_bonus(1, 10, level);
	    }
	  t_ptr->toac -= m_bonus(1, 40, level);
	  t_ptr->flags |= TR_CURSED;
	  t_ptr->cost = 0;
	}
      break;

    case TV_BOOTS:
      if (magik(chance))
	{
	  t_ptr->toac += m_bonus(1, 20, level);
	  if (magik(special))
	    {
	      tmp = randint(12);
	      if (tmp > 5)
		{
		  t_ptr->flags |= TR_FFALL;
		  t_ptr->name2 = SN_SLOW_DESCENT;
		  t_ptr->cost += 250;
		}
	      else if (tmp == 1)
		{
		  t_ptr->flags |= TR_SPEED;
		  t_ptr->name2 = SN_SPEED;
		  t_ptr->ident |= ID_SHOW_P1;
		  t_ptr->p1 = 1;
		  t_ptr->cost += 5000;
		}
	      else /* 2 - 5 */
		{
		  t_ptr->flags |= TR_STEALTH;
		  t_ptr->ident |= ID_SHOW_P1;
		  t_ptr->p1 = randint(3);
		  t_ptr->name2 = SN_STEALTH;
		  t_ptr->cost += 500;
		}
	    }
	}
      else if (magik(cursed))
	{
	  tmp = randint(3);
	  if (tmp == 1)
	    {
	      t_ptr->flags |= TR_SPEED;
	      t_ptr->name2 = SN_SLOWNESS;
	      t_ptr->ident |= ID_SHOW_P1;
	      t_ptr->p1 = -1;
	    }
	  else if (tmp == 2)
	    {
	      t_ptr->flags |= TR_AGGRAVATE;
	      t_ptr->name2 = SN_NOISE;
	    }
	  else
	    {
	      t_ptr->name2 = SN_GREAT_MASS;
	      t_ptr->weight = t_ptr->weight * 5;
	    }
	  t_ptr->cost = 0;
	  t_ptr->toac -= m_bonus(2, 45, level);
	  t_ptr->flags |= TR_CURSED;
	}
      break;

    case TV_HELM:  /* Helms */
      if ((t_ptr->subval >= 6) && (t_ptr->subval <= 8))
	{
	  /* give crowns a higher chance for magic */
	  chance += t_ptr->cost / 100;
	  special += special;
	}
      if (magik(chance))
	{
	  t_ptr->toac += m_bonus(1, 20, level);
	  if (magik(special))
	    {
	      if (t_ptr->subval < 6)
		{
		  tmp = randint(3);
		  t_ptr->ident |= ID_SHOW_P1;
		  if (tmp == 1)
		    {
		      t_ptr->p1 = randint(2);
		      t_ptr->flags |= TR_INT;
		      t_ptr->name2 = SN_INTELLIGENCE;
		      t_ptr->cost += t_ptr->p1*500;
		    }
		  else if (tmp == 2)
		    {
		      t_ptr->p1 = randint(2);
		      t_ptr->flags |= TR_WIS;
		      t_ptr->name2 = SN_WISDOM;
		      t_ptr->cost += t_ptr->p1*500;
		    }
		  else
		    {
		      t_ptr->p1 = 1 + randint(4);
		      t_ptr->flags |= TR_INFRA;
		      t_ptr->name2 = SN_INFRAVISION;
		      t_ptr->cost += t_ptr->p1*250;
		    }
		}
	      else
		{
		  switch(randint(6))
		    {
		    case 1:
		      t_ptr->ident |= ID_SHOW_P1;
		      t_ptr->p1 = randint(3);
		      t_ptr->flags |= (TR_FREE_ACT|TR_CON|TR_DEX|TR_STR);
		      t_ptr->name2 = SN_MIGHT;
		      t_ptr->cost += 1000 + t_ptr->p1*500;
		      break;
		    case 2:
		      t_ptr->ident |= ID_SHOW_P1;
		      t_ptr->p1 = randint(3);
		      t_ptr->flags |= (TR_CHR|TR_WIS);
		      t_ptr->name2 = SN_LORDLINESS;
		      t_ptr->cost += 1000 + t_ptr->p1*500;
		      break;
		    case 3:
		      t_ptr->ident |= ID_SHOW_P1;
		      t_ptr->p1 = randint(3);
		      t_ptr->flags |= (TR_RES_LIGHT|TR_RES_COLD|TR_RES_ACID|
				       TR_RES_FIRE|TR_INT);
		      t_ptr->name2 = SN_MAGI;
		      t_ptr->cost += 3000 + t_ptr->p1*500;
		      break;
		    case 4:
		      t_ptr->ident |= ID_SHOW_P1;
		      t_ptr->p1 = randint(3);
		      t_ptr->flags |= TR_CHR;
		      t_ptr->name2 = SN_BEAUTY;
		      t_ptr->cost += 750;
		      break;
		    case 5:
		      t_ptr->ident |= ID_SHOW_P1;
		      t_ptr->p1 = 5*(1 + randint(4));
		      t_ptr->flags |= (TR_SEE_INVIS|TR_SEARCH);
		      t_ptr->name2 = SN_SEEING;
		      t_ptr->cost += 1000 + t_ptr->p1*100;
		      break;
		    case 6:
		      t_ptr->flags |= TR_REGEN;
		      t_ptr->name2 = SN_REGENERATION;
		      t_ptr->cost += 1500;
		      break;
		    }
		}
	    }
	}
      else if (magik(cursed))
	{
	  t_ptr->toac -= m_bonus(1, 45, level);
	  t_ptr->flags |= TR_CURSED;
	  t_ptr->cost = 0;
	  if (magik(special))
	    switch(randint(7))
	      {
	      case 1:
		t_ptr->ident |= ID_SHOW_P1;
		t_ptr->p1 = -randint (5);
		t_ptr->flags |= TR_INT;
		t_ptr->name2 = SN_STUPIDITY;
		break;
	      case 2:
		t_ptr->ident |= ID_SHOW_P1;
		t_ptr->p1 = -randint (5);
		t_ptr->flags |= TR_WIS;
		t_ptr->name2 = SN_DULLNESS;
		break;
	      case 3:
		t_ptr->flags |= TR_BLIND;
		t_ptr->name2 = SN_BLINDNESS;
		break;
	      case 4:
		t_ptr->flags |= TR_TIMID;
		t_ptr->name2 = SN_TIMIDNESS;
		break;
	      case 5:
		t_ptr->ident |= ID_SHOW_P1;
		t_ptr->p1 = -randint (5);
		t_ptr->flags |= TR_STR;
		t_ptr->name2 = SN_WEAKNESS;
		break;
	      case 6:
		t_ptr->flags |= TR_TELEPORT;
		t_ptr->name2 = SN_TELEPORTATION;
		break;
	      case 7:
		t_ptr->ident |= ID_SHOW_P1;
		t_ptr->p1 = -randint (5);
		t_ptr->flags |= TR_CHR;
		t_ptr->name2 = SN_UGLINESS;
		break;
	      }
	}
      break;

    case TV_RING: /* Rings	      */
      switch(t_ptr->subval)
	{
	case 0: case 1: case 2: case 3:
	  if (magik(cursed))
	    {
	      t_ptr->p1 = -m_bonus(1, 20, level);
	      t_ptr->flags |= TR_CURSED;
	      t_ptr->cost = -t_ptr->cost;
	    }
	  else
	    {
	      t_ptr->p1 = m_bonus(1, 10, level);
	      t_ptr->cost += t_ptr->p1*100;
	    }
	  break;
	case 4:
	  if (magik(cursed))
	    {
	      t_ptr->p1 = -randint(3);
	      t_ptr->flags |= TR_CURSED;
	      t_ptr->cost = -t_ptr->cost;
	    }
	  else
	    t_ptr->p1 = 1;
	  break;
	case 5:
	  t_ptr->p1 = 5 * m_bonus(1, 20, level);
	  t_ptr->cost += t_ptr->p1*100;
	  if (magik (cursed))
	    {
	      t_ptr->p1 = -t_ptr->p1;
	      t_ptr->flags |= TR_CURSED;
	      t_ptr->cost = -t_ptr->cost;
	    }
	  break;
	case 19:     /* Increase damage	      */
	  t_ptr->todam += m_bonus(1, 20, level);
	  t_ptr->cost += t_ptr->todam*100;
	  if (magik(cursed))
	    {
	      t_ptr->todam = -t_ptr->todam;
	      t_ptr->flags |= TR_CURSED;
	      t_ptr->cost = -t_ptr->cost;
	    }
	  break;
	case 20:     /* Increase To-Hit	      */
	  t_ptr->tohit += m_bonus(1, 20, level);
	  t_ptr->cost += t_ptr->tohit*100;
	  if (magik(cursed))
	    {
	      t_ptr->tohit = -t_ptr->tohit;
	      t_ptr->flags |= TR_CURSED;
	      t_ptr->cost = -t_ptr->cost;
	    }
	  break;
	case 21:     /* Protection	      */
	  t_ptr->toac += m_bonus(1, 20, level);
	  t_ptr->cost += t_ptr->toac*100;
	  if (magik(cursed))
	    {
	      t_ptr->toac = -t_ptr->toac;
	      t_ptr->flags |= TR_CURSED;
	      t_ptr->cost = -t_ptr->cost;
	    }
	  break;
	case 24: case 25: case 26:
	case 27: case 28: case 29:
	  t_ptr->ident |= ID_NOSHOW_P1;
	  break;
	case 30:     /* Slaying	      */
	  t_ptr->ident |= ID_SHOW_HITDAM;
	  t_ptr->todam += m_bonus(1, 25, level);
	  t_ptr->tohit += m_bonus(1, 25, level);
	  t_ptr->cost += (t_ptr->tohit+t_ptr->todam)*100;
	  if (magik(cursed))
	    {
	      t_ptr->tohit = -t_ptr->tohit;
	      t_ptr->todam = -t_ptr->todam;
	      t_ptr->flags |= TR_CURSED;
	      t_ptr->cost = -t_ptr->cost;
	    }
	  break;
	default:
	  break;
	}
      break;

    case TV_AMULET: /* Amulets	      */
      if (t_ptr->subval < 2)
	{
	  if (magik(cursed))
	    {
	      t_ptr->p1 = -m_bonus(1, 20, level);
	      t_ptr->flags |= TR_CURSED;
	      t_ptr->cost = -t_ptr->cost;
	    }
	  else
	    {
	      t_ptr->p1 = m_bonus(1, 10, level);
	      t_ptr->cost += t_ptr->p1*100;
	    }
	}
      else if (t_ptr->subval == 2)
	{
	  t_ptr->p1 = 5 * m_bonus(1, 25, level);
	  if (magik(cursed))
	    {
	      t_ptr->p1 = -t_ptr->p1;
	      t_ptr->cost = -t_ptr->cost;
	      t_ptr->flags |= TR_CURSED;
	    }
	  else
	    t_ptr->cost += 20*t_ptr->p1;
	}
      else if (t_ptr->subval == 8)
	{
	  /* amulet of the magi is never cursed */
	  t_ptr->p1 = 5 * m_bonus(1, 25, level);
	  t_ptr->cost += 20*t_ptr->p1;
	}
      break;

      /* Subval should be even for store, odd for dungeon*/
      /* Dungeon found ones will be partially charged	 */
    case TV_LIGHT:
      if ((t_ptr->subval % 2) == 1)
	{
	  t_ptr->p1 = randint(t_ptr->p1);
	  t_ptr->subval -= 1;
	}
      break;

    case TV_WAND:
      switch(t_ptr->subval)
	{
	case 0:	  t_ptr->p1 = randint(10) +	 6; break;
	case 1:	  t_ptr->p1 = randint(8)  +	 6; break;
	case 2:	  t_ptr->p1 = randint(5)  +	 6; break;
	case 3:	  t_ptr->p1 = randint(8)  +	 6; break;
	case 4:	  t_ptr->p1 = randint(4)  +	 3; break;
	case 5:	  t_ptr->p1 = randint(8)  +	 6; break;
	case 6:	  t_ptr->p1 = randint(20) +	 12; break;
	case 7:	  t_ptr->p1 = randint(20) +	 12; break;
	case 8:	  t_ptr->p1 = randint(10) +	 6; break;
	case 9:	  t_ptr->p1 = randint(12) +	 6; break;
	case 10:   t_ptr->p1 = randint(10) +	 12; break;
	case 11:   t_ptr->p1 = randint(3)  +	 3; break;
	case 12:   t_ptr->p1 = randint(8)  +	 6; break;
	case 13:   t_ptr->p1 = randint(10) +	 6; break;
	case 14:   t_ptr->p1 = randint(5)  +	 3; break;
	case 15:   t_ptr->p1 = randint(5)  +	 3; break;
	case 16:   t_ptr->p1 = randint(5)  +	 6; break;
	case 17:   t_ptr->p1 = randint(5)  +	 4; break;
	case 18:   t_ptr->p1 = randint(8)  +	 4; break;
	case 19:   t_ptr->p1 = randint(6)  +	 2; break;
	case 20:   t_ptr->p1 = randint(4)  +	 2; break;
	case 21:   t_ptr->p1 = randint(8)  +	 6; break;
	case 22:   t_ptr->p1 = randint(5)  +	 2; break;
	case 23:   t_ptr->p1 = randint(12) + 12; break;
	default:
	  break;
	}
      break;

    case TV_STAFF:
      switch(t_ptr->subval)
	{
	case 0:	  t_ptr->p1 = randint(20) +	 12; break;
	case 1:	  t_ptr->p1 = randint(8)  +	 6; break;
	case 2:	  t_ptr->p1 = randint(5)  +	 6; break;
	case 3:	  t_ptr->p1 = randint(20) +	 12; break;
	case 4:	  t_ptr->p1 = randint(15) +	 6; break;
	case 5:	  t_ptr->p1 = randint(4)  +	 5; break;
	case 6:	  t_ptr->p1 = randint(5)  +	 3; break;
	case 7:	  t_ptr->p1 = randint(3)  +	 1;
	  t_ptr->level = 10;
	  break;
	case 8:	  t_ptr->p1 = randint(3)  +	 1; break;
	case 9:	  t_ptr->p1 = randint(5)  +	 6; break;
	case 10:   t_ptr->p1 = randint(10) +	 12; break;
	case 11:   t_ptr->p1 = randint(5)  +	 6; break;
	case 12:   t_ptr->p1 = randint(5)  +	 6; break;
	case 13:   t_ptr->p1 = randint(5)  +	 6; break;
	case 14:   t_ptr->p1 = randint(10) +	 12; break;
	case 15:   t_ptr->p1 = randint(3)  +	 4; break;
	case 16:   t_ptr->p1 = randint(5)  +	 6; break;
	case 17:   t_ptr->p1 = randint(5)  +	 6; break;
	case 18:   t_ptr->p1 = randint(3)  +	 4; break;
	case 19:   t_ptr->p1 = randint(10) +	 12; break;
	case 20:   t_ptr->p1 = randint(3)  +	 4; break;
	case 21:   t_ptr->p1 = randint(3)  +	 4; break;
	case 22:   t_ptr->p1 = randint(10) + 6;
	  t_ptr->level = 5;
	  break;
	default:
	  break;
	}
      break;

    case TV_CLOAK:
      if (magik(chance))
	{
	  if (magik(special))
	    {
	      if (randint(2) == 1)
		{
		  t_ptr->name2 = SN_PROTECTION;
		  t_ptr->toac += m_bonus(2, 40, level);
		  t_ptr->cost += 250;
		}
	      else
		{
		  t_ptr->toac += m_bonus(1, 20, level);
		  t_ptr->ident |= ID_SHOW_P1;
		  t_ptr->p1 = randint(3);
		  t_ptr->flags |= TR_STEALTH;
		  t_ptr->name2 = SN_STEALTH;
		  t_ptr->cost += 500;
		}
	    }
	  else
	    t_ptr->toac += m_bonus(1, 20, level);
	}
      else if (magik(cursed))
	{
	  tmp = randint(3);
	  if (tmp == 1)
	    {
	      t_ptr->flags |= TR_AGGRAVATE;
	      t_ptr->name2 = SN_IRRITATION;
	      t_ptr->toac  -= m_bonus(1, 10, level);
	      t_ptr->ident |= ID_SHOW_HITDAM;
	      t_ptr->tohit -= m_bonus(1, 10, level);
	      t_ptr->todam -= m_bonus(1, 10, level);
	      t_ptr->cost =  0;
	    }
	  else if (tmp == 2)
	    {
	      t_ptr->name2 = SN_VULNERABILITY;
	      t_ptr->toac -= m_bonus(10, 100, level+50);
	      t_ptr->cost = 0;
	    }
	  else
	    {
	      t_ptr->name2 = SN_ENVELOPING;
	      t_ptr->toac  -= m_bonus(1, 10, level);
	      t_ptr->ident |= ID_SHOW_HITDAM;
	      t_ptr->tohit -= m_bonus(2, 40, level+10);
	      t_ptr->todam -= m_bonus(2, 40, level+10);
	      t_ptr->cost = 0;
	    }
	  t_ptr->flags |= TR_CURSED;
	}
      break;

    case TV_CHEST:
      switch(randint(level+4))
	{
	case 1:
	  t_ptr->flags = 0;
	  t_ptr->name2 = SN_EMPTY;
	  break;
	case 2:
	  t_ptr->flags |= CH_LOCKED;
	  t_ptr->name2 = SN_LOCKED;
	  break;
	case 3: case 4:
	  t_ptr->flags |= (CH_LOSE_STR|CH_LOCKED);
	  t_ptr->name2 = SN_POISON_NEEDLE;
	  break;
	case 5: case 6:
	  t_ptr->flags |= (CH_POISON|CH_LOCKED);
	  t_ptr->name2 = SN_POISON_NEEDLE;
	  break;
	case 7: case 8: case 9:
	  t_ptr->flags |= (CH_PARALYSED|CH_LOCKED);
	  t_ptr->name2 = SN_GAS_TRAP;
	  break;
	case 10: case 11:
	  t_ptr->flags |= (CH_EXPLODE|CH_LOCKED);
	  t_ptr->name2 = SN_EXPLOSION_DEVICE;
	  break;
	case 12: case 13: case 14:
	  t_ptr->flags |= (CH_SUMMON|CH_LOCKED);
	  t_ptr->name2 = SN_SUMMONING_RUNES;
	  break;
	case 15: case 16: case 17:
	  t_ptr->flags |= (CH_PARALYSED|CH_POISON|CH_LOSE_STR|CH_LOCKED);
	  t_ptr->name2 = SN_MULTIPLE_TRAPS;
	  break;
	default:
	  t_ptr->flags |= (CH_SUMMON|CH_EXPLODE|CH_LOCKED);
	  t_ptr->name2 = SN_MULTIPLE_TRAPS;
	  break;
	}
      break;

    case TV_SLING_AMMO: case TV_SPIKE:
    case TV_BOLT: case TV_ARROW:
      if (t_ptr->tval == TV_SLING_AMMO || t_ptr->tval == TV_BOLT
	  || t_ptr->tval == TV_ARROW)
	{
	  /* always show tohit/todam values if identified */
	  t_ptr->ident |= ID_SHOW_HITDAM;

	  if (magik(chance))
	    {
	      t_ptr->tohit += m_bonus(1, 35, level);
	      t_ptr->todam += m_bonus(1, 35, level);
	      /* see comment for weapons */
	      if (magik(3*special/2))
		switch(randint(10))
		  {
		  case 1: case 2: case 3:
		    t_ptr->name2 = SN_SLAYING;
		    t_ptr->tohit += 5;
		    t_ptr->todam += 5;
		    t_ptr->cost += 20;
		    break;
		  case 4: case 5:
		    t_ptr->flags |= TR_FLAME_TONGUE;
		    t_ptr->tohit += 2;
		    t_ptr->todam += 4;
		    t_ptr->name2 = SN_FIRE;
		    t_ptr->cost += 25;
		    break;
		  case 6: case 7:
		    t_ptr->flags |= TR_SLAY_EVIL;
		    t_ptr->tohit += 3;
		    t_ptr->todam += 3;
		    t_ptr->name2 = SN_SLAY_EVIL;
		    t_ptr->cost += 25;
		    break;
		  case 8: case 9:
		    t_ptr->flags |= TR_SLAY_ANIMAL;
		    t_ptr->tohit += 2;
		    t_ptr->todam += 2;
		    t_ptr->name2 = SN_SLAY_ANIMAL;
		    t_ptr->cost += 30;
		    break;
		  case 10:
		    t_ptr->flags |= TR_SLAY_DRAGON;
		    t_ptr->tohit += 3;
		    t_ptr->todam += 3;
		    t_ptr->name2 = SN_DRAGON_SLAYING;
		    t_ptr->cost += 35;
		    break;
		  }
	    }
	  else if (magik(cursed))
	    {
	      t_ptr->tohit -= m_bonus(5, 55, level);
	      t_ptr->todam -= m_bonus(5, 55, level);
	      t_ptr->flags |= TR_CURSED;
	      t_ptr->cost = 0;
	    }
	}

      t_ptr->number = 0;
      for (i = 0; i < 7; i++)
	t_ptr->number += randint(6);
      if (missile_ctr == MAX_SHORT)
	missile_ctr = -MAX_SHORT - 1;
      else
	missile_ctr++;
      t_ptr->p1 = missile_ctr;
      break;

    case TV_FOOD:
      /* make sure all food rations have the same level */
      if (t_ptr->subval == 90)
	t_ptr->level = 0;
      /* give all elvish waybread the same level */
      else if (t_ptr->subval == 92)
	t_ptr->level = 6;
      break;

    case TV_SCROLL1:
      /* give all identify scrolls the same level */
      if (t_ptr->subval == 67)
	t_ptr->level = 1;
      /* scroll of light */
      else if (t_ptr->subval == 69)
	t_ptr->level = 0;
      /* scroll of trap detection */
      else if (t_ptr->subval == 80)
	t_ptr->level = 5;
      /* scroll of door/stair location */
      else if (t_ptr->subval == 81)
	t_ptr->level = 5;
      break;

    case TV_POTION1:  /* potions */
      /* cure light */
      if (t_ptr->subval == 76)
	t_ptr->level = 0;
      break;

    default:
      break;
    }
}


static struct opt_desc { char *o_prompt; int *o_var; } options[] = {
  { "Running: cut known corners",		&find_cut },
  { "Running: examine potential corners",	&find_examine },
  { "Running: print self during run",		&find_prself },
  { "Running: stop when map sector changes",	&find_bound },
  { "Running: run through open doors",		&find_ignore_doors },
  { "Prompt to pick up objects",		&prompt_carry_flag },
  { "Rogue like commands",			&rogue_like_commands },
  { "Show weights in inventory",		&show_weight_flag },
  { "Highlight and notice mineral seams",	&highlight_seams },
  { 0, 0 } };


/* Set or unset various boolean options.		-CJS- */
void set_options()
{
  register int i, max;
  vtype string;

  prt("  ESC when finished, y/n to set options, <return> or - to move cursor",
		0, 0);
  for (max = 0; options[max].o_prompt != 0; max++)
    {
      (void) sprintf(string, "%-38s: %s", options[max].o_prompt,
		     (*options[max].o_var ? "yes" : "no "));
      prt(string, max+1, 0);
    }
  erase_line(max+1, 0);
  i = 0;
  for(;;)
    {
      move_cursor(i+1, 40);
      switch(inkey())
	{
	case ESCAPE:
	  return;
	case '-':
	  if (i > 0)
	    i--;
	  else
	    i = max-1;
	  break;
	case ' ':
	case '\n':
	case '\r':
	  if (i+1 < max)
	    i++;
	  else
	    i = 0;
	  break;
	case 'y':
	case 'Y':
	  put_buffer("yes", i+1, 40);
	  *options[i].o_var = TRUE;
	  if (i+1 < max)
	    i++;
	  else
	    i = 0;
	  break;
	case 'n':
	case 'N':
	  put_buffer("no ", i+1, 40);
	  *options[i].o_var = FALSE;
	  if (i+1 < max)
	    i++;
	  else
	    i = 0;
	  break;
	default:
	  bell();
	  break;
	}
    }
}
