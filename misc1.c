#include <time.h>
#include <math.h>
#include <stdio.h>
#include <sys/types.h>

#include "constants.h"
#include "config.h"
#include "types.h"
/* SUN4 has a variable called class in the include file <math.h>
   avoid a conflict by not defining my class in the file externs.h */
#define DONT_DEFINE_CLASS
#include "externs.h"

#ifdef USG
#include <string.h>
#else
#include <strings.h>
#endif

long time();
struct tm *localtime();
double sqrt();
double cos();
double fabs();
#if defined(ultrix) || defined(sun) || defined(USG)
int getuid();
int geteuid();
int getgid();
int getegid();
#else
uid_t getuid();
uid_t geteuid();
uid_t getgid();
uid_t getegid();
#endif

#ifdef USG
long lrand48();
void srand48();
unsigned short *seed48();
#else
long random();
char *initstate();
char *setstate();
#endif

#ifdef ultrix
void srandom();
#endif

/* gets a new random seed for the random number generator */
init_seeds()
{
  register long clock;
  register int euid;

  /* in case game is setuid root */
  if ((euid = geteuid()) == 0)
    euid = (int)time((long *)0);

  clock = time((long *)0);
  clock = clock * getpid() * euid;
#ifdef USG
  /* only uses randes_seed */
#else
  (void) initstate((unsigned int)clock, randes_state, STATE_SIZE);
#endif
  randes_seed = (unsigned int)clock;

  clock = time((long *)0);
  clock = clock * getpid() * euid;
#ifdef USG
  /* only uses town_seed */
#else
  (void) initstate((unsigned int)clock, town_state, STATE_SIZE);
#endif
  town_seed = (unsigned int)clock;

  clock = time((long *)0);
#if 0
  clock = clock * getpid() * euid * getuid();
#endif
#ifdef USG
  /* can't do this, so fake it */
  srand48(clock);
  /* make it a little more random */
  for (clock = randint(100); clock >= 0; clock--)
    (void) lrand48();
#else
  (void) initstate((unsigned int)clock, norm_state, STATE_SIZE);
  /* make it a little more random */
  for (clock = randint(100); clock >= 0; clock--)
    (void) random();
#endif
}

#ifdef USG
/* special array for restoring the SYS V number generator */
unsigned short oldseed[3];
#endif

/* change to different random number generator state */
/*ARGSUSED*/
set_seed(state, seed)
char *state;
int seed;
{
#ifdef USG
  register unsigned short *pointer;

  /* make phony call to get pointer to old value of seed */
  pointer = seed48(oldseed);
  /* copy old seed into oldseed */
  oldseed[0] = pointer[0];
  oldseed[1] = pointer[1];
  oldseed[2] = pointer[2];

  /* want reproducible state here, so call srand48 */
  srand48((long)seed);
#else
  (void) setstate(state);
  /* want reproducible state here, so call srandom */
  srandom(seed);
#endif
}


/* restore the normal random generator state */
reset_seed()
{
#ifdef USG
  (void) seed48(oldseed);
#if 0
  /* can't do this, so just call srand() with the current time */
  srand48((unsigned int)(time ((long *)0)));
#endif
#else
  (void) setstate(norm_state);
#endif
}


/* Returns the day number; 0==Sunday...6==Saturday 	-RAK-	*/
int day_num()
{
  long clock;
  register struct tm *time_struct;

  clock = time((long *)0);
  time_struct = localtime(&clock);
  return (time_struct->tm_wday);
}


/* Returns the hour number; 0==midnight...23==11 PM	-RAK-	*/
int hour_num()
{
  long clock;
  register struct tm *time_struct;

  clock = time((long *)0);
  time_struct = localtime(&clock);
  return (time_struct->tm_hour);
}


/* Check the day-time strings to see if open		-RAK-	*/
int check_time()
{
 switch ((int)days[day_num()][(hour_num()+4)]) {
   case '.':  return(FALSE);    /* Closed                */
   case 'X':  return(TRUE);     /* Normal hours          */
   default:   return(FALSE);    /* Other, assumed closed */
   }
}


/* Generates a random integer x where 1<==X<==MAXVAL	-RAK-	*/
int randint(maxval)
int maxval;
{
  register long randval;

#ifdef USG
  randval = lrand48();
#else
  randval = random();
#endif
  return ((randval % maxval) + 1);
}

/* For i := 1 to y do sum := sum + randint(x) */
int rand_rep(num, die)
int num;
int die;
{
  register int sum = 0;
  register int i;

  for (i = 0; i < num; i++)
    sum += randint(die);
  return(sum);
}

/* Generates a random integer number of NORMAL distribution -RAK-*/
int randnor(mean, stand)
int mean;
int stand;
{
  return ((int)((sqrt(-2.0*log(randint((int)9999999)/10000000.0))*
		 cos(6.283*(randint((int)9999999)/10000000.0))*stand) + mean));
}


/* Returns position of first set bit			-RAK-	*/
/*     and clears that bit */
int bit_pos(test)
unsigned int *test;
{
  register int i;
  register int mask = 0x1;

  for (i = 0; i < sizeof(int)*8; i++) {
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


/* Distance between two points				-RAK-	*/
/* there is a bessel function names y1 in the math library, ignore warning */
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

  return( (2 * (dy + dx) - (dy > dx ? dx : dy)) / 2);
}

/* Checks points north, south, east, and west for a type -RAK-	*/
int next_to4(y, x, elem_a, elem_b, elem_c)
register int y, x;
int elem_a, elem_b, elem_c;
{
  register int i;

  i = 0;
  if (y > 0)
    if ((cave[y-1][x].fval == elem_a) || (cave[y-1][x].fval == elem_b) ||
	(cave[y-1][x].fval == elem_c))
      i++;
  if (y < cur_height-1)
    if ((cave[y+1][x].fval == elem_a) || (cave[y+1][x].fval == elem_b) ||
	(cave[y+1][x].fval == elem_c))
      i++;
  if (x > 0)
    if ((cave[y][x-1].fval == elem_a) || (cave[y][x-1].fval == elem_b) ||
	(cave[y][x-1].fval == elem_c))
      i++;
  if (x < cur_width-1)
    if ((cave[y][x+1].fval == elem_a) || (cave[y][x+1].fval == elem_b) ||
	(cave[y][x+1].fval == elem_c))
      i++;
  return(i);
}


/* Checks all adjacent spots for elements		-RAK-	*/
int next_to8(y, x, elem_a, elem_b, elem_c)
register int y, x;
int elem_a, elem_b, elem_c;
{
  register int k, j, i;

  i = 0;
  for (j = (y - 1); j <= (y + 1); j++)
    for (k = (x - 1); k <= (x + 1); k++)
      if (in_bounds(j, k))
	if ((cave[j][k].fval == elem_a) || (cave[j][k].fval == elem_b) ||
	    (cave[j][k].fval == elem_c))
	  i++;
  return(i);
}


/* Link all free space in treasure list together 		*/
tlink()
{
  register int i;

  for (i = 0; i < MAX_TALLOC; i++)
    {
      t_list[i] = blank_treasure;
      t_list[i].p1 = i - 1;
    }
  tcptr = MAX_TALLOC - 1;
}


/* Link all free space in monster list together			*/
mlink()
{
  register int i;

  for (i = 0; i < MAX_MALLOC; i++)
    {
      m_list[i] = blank_monster;
      m_list[i].nptr = i - 1;
    }
  m_list[1].nptr = 0;
  muptr = 0;
  mfptr = MAX_MALLOC - 1;
}


/* Initializes M_LEVEL array for use with PLACE_MONSTER	-RAK-	*/
init_m_level()
{
  register int i, j, k;

  i = 0;
  j = 0;
  k = MAX_CREATURES - WIN_MON_TOT;
  while (j <= MAX_MONS_LEVEL) {
    m_level[j] = 0;
    while ((i < k) && (c_list[i].level == j))
      {
	m_level[j]++;
	i++;
      }
    j++;
  }
  for (i = 2; i <= MAX_MONS_LEVEL; i++)
    m_level[i] += m_level[i-1];
}


/* Initializes T_LEVEL array for use with PLACE_OBJECT	-RAK-	*/
init_t_level()
{
  register int i, j;

  i = 0;
  j = 0;
  while ((j <= MAX_OBJ_LEVEL) && (i < MAX_OBJECTS))
    {
      while ((i < MAX_OBJECTS) && (object_list[i].level == j))
	{
	  t_level[j]++;
	  i++;
	}
      j++;
    }
  for (i = 1; i <= MAX_OBJ_LEVEL; i++)
    t_level[i] += t_level[i-1];
}


/* Adjust prices of objects				-RAK-	*/
price_adjust()
{
  register int i;

  for (i = 0; i < MAX_OBJECTS; i++)
    object_list[i].cost = object_list[i].cost*COST_ADJ + 0.99;
  for (i = 0; i < INVEN_INIT_MAX; i++)
    inventory_init[i].cost = inventory_init[i].cost*COST_ADJ + 0.99;
}


/* Converts input string into a dice roll		-RAK-	*/
/*       Normal input string will look like "2d6", "3d8"... etc. */
int damroll(dice)
char *dice;
{
  int num, sides;

  num = 0;
  sides = 0;
  (void) sscanf(dice, "%d d %d", &num, &sides);
  return(rand_rep(num, sides));
}


/* Returns true if no obstructions between two given points -RAK-*/
/* there is a bessel function names y1 in the math library, ignore warning */
int los(y1, x1, y2, x2)
int y1, x1, y2, x2;
{
  register int ty, tx, stepy, stepx;
  int aty, atx, p1, p2;
  double slp, tmp;
  int flag;

  ty = (y1 - y2);
  tx = (x1 - x2);
  flag = TRUE;
  if ((ty != 0) || (tx != 0))
    {
      if (ty < 0)
	{
	  stepy = -1;
	  aty = -ty;
	}
      else
	{
	  stepy = 1;
	  aty = ty;
	}
      if (tx < 0)
	{
	  stepx = -1;
	  atx = -tx;
	}
      else
	{
	  stepx = 1;
	  atx = tx;
	}
      if (ty == 0)
	{
	  do
	    {
	      x2 += stepx;
	      flag = cave[y2][x2].fopen;
	    }
	  while((x1 != x2) && (flag));
	}
      else if (tx == 0)
	{
	  do
	    {
	      y2 += stepy;
	      flag = cave[y2][x2].fopen;
	    }
	  while((y1 != y2) && (flag));
	}
      else if (aty > atx)
	{
	  slp = ((double)atx / (double)aty) * stepx;
	  tmp = x2;
	  do
	    {
	      y2 += stepy;
	      tmp += slp;
              /* round to nearest integer */
	      p1 = (int)floor(tmp - 0.1 + 0.5);
	      p2 = (int)floor(tmp + 0.1 + 0.5);
	      if ((!cave[y2][p1].fopen) && (!cave[y2][p2].fopen))
		  flag = FALSE;
	    }
	  while((y1 != y2) && (flag));
	}
      else
	{
	  slp = ((double)aty / (double)atx) * stepy;
	  tmp = y2;
	  do
	    {
	      x2 += stepx;
	      tmp += slp;
              /* round to nearest integer */
	      p1 = (int)floor(tmp - 0.1 + 0.5);
	      p2 = (int)floor(tmp + 0.1 + 0.5);
	      if ((!cave[p1][x2].fopen) && (!cave[p2][x2].fopen))
		flag = FALSE;
	    }
	  while((x1 != x2) && (flag));
	}
    }
  return(flag);
}


/* Returns symbol for given row, column			-RAK-	*/
loc_symbol(y, x, sym)
int y, x;
char *sym;
{
  register cave_type *cave_ptr;
  register monster_type *mon_ptr;

  cave_ptr = &cave[y][x];
  if ((cave_ptr->cptr == 1) && (!find_flag))
    *sym = '@';
  else if (py.flags.blind > 0)
    *sym = ' ';
  else
    {
      if (cave_ptr->cptr > 1)
	{
	  mon_ptr = &m_list[cave_ptr->cptr];
	  if ((mon_ptr->ml) &&
	      (((c_list[mon_ptr->mptr].cmove & 0x00010000) == 0) ||
	       (py.flags.see_inv)))
	    *sym = c_list[mon_ptr->mptr].cchar;
	  else if (cave_ptr->tptr != 0)
	    *sym = t_list[cave_ptr->tptr].tchar;
	  else if (cave_ptr->fval < 10)
	    *sym = '.';
	  else
	    *sym = '#';
	}
      else if (cave_ptr->tptr != 0)
	*sym = t_list[cave_ptr->tptr].tchar;
      else if (cave_ptr->fval < 10)
	*sym = '.';
      else
	*sym = '#';
    }
}


/* Tests a spot for light or field mark status		-RAK-	*/
int test_light(y, x)
int y, x;
{
  register cave_type *cave_ptr;

  cave_ptr = &cave[y][x];
  if ((cave_ptr->pl) || (cave_ptr->fm) || (cave_ptr->tl))
    return(TRUE);
  else
    return(FALSE);
}


/* Prints the map of the dungeon 			-RAK-	*/
prt_map()
{
  register int i, j, k;
  int l, m;
  int ypos, isp;
  /* this eliminates lint warning: xpos may be used before set */
  int xpos = 0;
  vtype floor_str;
  char tmp_char[2];
  int flag;
  register cave_type *cave_ptr;

  k = 0;                          /* Used for erasing dirty lines  */
  l = 13;                         /* Erasure starts in this column */
  for (i = panel_row_min; i <= panel_row_max; i++)  /* Top to bottom */
    {
      k++;                     /* Increment dirty line ctr      */
      if (used_line[k])        /* If line is dirty...           */
	{
	  erase_line(k, l);        /* erase it.                    */
	  used_line[k] = FALSE;   /* Now it's a clean line         */
	}
      floor_str[0] = '\0';              /* Floor_str is string to be printed*/
      ypos = i;                   /* Save row                      */
      flag = FALSE;                /* False until floor_str != ""   */
      isp = 0;                     /* Number of blanks encountered  */
      for (j = panel_col_min; j <= panel_col_max; j++)  /* Left to right */
	{
	  cave_ptr = &cave[i][j];    /* Get character for location    */
	  if (cave_ptr->pl || cave_ptr->fm || cave_ptr->tl)
	    loc_symbol(i, j, tmp_char);
	  else if ((cave_ptr->cptr == 1) && (!find_flag))
	    tmp_char[0] = '@';
	  else if (cave_ptr->cptr > 1)
	    if (m_list[cave_ptr->cptr].ml)
	      loc_symbol(i, j, tmp_char);
	    else
	      tmp_char[0] = ' ';
	  else
	    tmp_char[0] = ' ';
	  if (py.flags.image > 0)
	    if (randint(12) == 1)
	      tmp_char[0] = (randint(95) + 31);
	  if (tmp_char[0] == ' ') /* If blank...                   */
	    {
	      if (flag)       /* If floor_str != ""        */
		{
		  isp++;      /* Increment blank ctr           */
		  if (isp > 3)        /* Too many blanks, print*/
		    {                 /* floor_str and reset   */
		      print(floor_str, ypos, xpos);
		      flag = FALSE;
		      isp = 0;
		    }
		}
	    }
	  else
	    {
	      if (flag)       /* Floor_str != ""               */
		{
		  if (isp > 0)        /* Add on the blanks     */
		    {
		      for (m = 0; m < isp; m++)
			(void) strcat(floor_str, " ");
		      isp = 0;
		    }                  /* Add on the character  */
		  tmp_char[1] = '\0';
		  (void) strcat(floor_str, tmp_char);
		}
	      else
		{             /* Floor_str == ""                */
		  xpos = j;     /* Save column for printing      */
		  flag = TRUE;   /* Set flag to true              */
		  floor_str[0] = tmp_char[0];  /* Floor_str != ""       */
		  floor_str[1] = '\0';
		}
	    }
	}
      if (flag)                 /* Print remainder, if any       */
	print(floor_str, ypos, xpos);
    }
}


/* Compact monsters					-RAK-	*/
compact_monsters()
{
  register int i, j, k;
  int cur_dis;
  int delete_1, delete_any;
  register monster_type *mon_ptr;

  cur_dis = 66;
  delete_any = FALSE;
  do
    {
      i = muptr;
      j = 0;
      while (i > 0)
	{
	  delete_1 = FALSE;
	  k = m_list[i].nptr;
	  mon_ptr = &m_list[i];
	  if (cur_dis > mon_ptr->cdis)
	    if (randint(3) == 1)
	      {
		if (j == 0)
		  muptr = k;
		else
		  m_list[j].nptr = k;
		cave[mon_ptr->fy][mon_ptr->fx].cptr = 0;
		m_list[i] = blank_monster;
		m_list[i].nptr = mfptr;
		mfptr = i;
		delete_1 = TRUE;
		delete_any = TRUE;
	      }
	  if (!delete_1)
	    j = i;
	  i = k;
	}
      if (!delete_any)
	cur_dis -= 6;
    }
  while (!delete_any);
  if (cur_dis < 66)
    prt_map();
}


/* Returns a pointer to next free space			-RAK-	*/
popm(x)
register int *x;
{
  if (mfptr <= 1)
    compact_monsters();
  *x = mfptr;
  mfptr = m_list[*x].nptr;
}


/* Pushs a record back onto free space list		-RAK-	*/
pushm(x)
register int x;
{
  m_list[x] = blank_monster;
  m_list[x].nptr = mfptr;
  mfptr = x;
}


/* Gives Max hit points					-RAK-	*/
int max_hp(hp_str)
char *hp_str;
{
  int num, die;

  (void) sscanf(hp_str, "%d d %d", &num, &die);
  return(num*die);
}


/* Places a monster at given location			-RAK-	*/
place_monster(y, x, z, slp)
register int y, x, z;
int slp;
{
  int cur_pos;
  register monster_type *mon_ptr;

  popm(&cur_pos);
  mon_ptr = &m_list[cur_pos];
  mon_ptr->fy = y;
  mon_ptr->fx = x;
  mon_ptr->mptr = z;
  mon_ptr->nptr = muptr;
  muptr = cur_pos;
  if (c_list[z].cdefense & 0x4000)
    mon_ptr->hp = max_hp(c_list[z].hd);
  else
    mon_ptr->hp = damroll(c_list[z].hd);
  mon_ptr->cspeed = c_list[z].speed + py.flags.speed;
  mon_ptr->stunned = 0;
  mon_ptr->cdis = distance(char_row, char_col,y,x);
  cave[y][x].cptr = cur_pos;
  if (slp)
    {
      if (c_list[z].sleep == 0)
	mon_ptr->csleep = 0;
      else
	mon_ptr->csleep = (c_list[z].sleep/5.0) + randint(c_list[z].sleep);
    }
  else
    mon_ptr->csleep = 0;
}


/* Places a monster at given location			-RAK-	*/
place_win_monster()
{
  int cur_pos;
  register int y, x;
  register monster_type *mon_ptr;

  if (!total_winner)
    {
      popm(&cur_pos);
      mon_ptr = &m_list[cur_pos];
      do
	{
	  y = randint(cur_height-2);
	  x = randint(cur_width-2);
	}
      while (((cave[y][x].fval != 1) && (cave[y][x].fval != 2) &&
		(cave[y][x].fval != 4)) ||
	     (cave[y][x].cptr != 0) ||
	     (cave[y][x].tptr != 0) ||
	     (distance(y,x,char_row, char_col) <= MAX_SIGHT));
      mon_ptr->fy = y;
      mon_ptr->fx = x;
      mon_ptr->mptr = randint(WIN_MON_TOT) - 1 + m_level[MAX_MONS_LEVEL] +
	m_level[0];
      mon_ptr->nptr = muptr;
      muptr = cur_pos;
      if (c_list[mon_ptr->mptr].cdefense & 0x4000)
	mon_ptr->hp = max_hp(c_list[mon_ptr->mptr].hd);
      else
	mon_ptr->hp = damroll(c_list[mon_ptr->mptr].hd);
      mon_ptr->cspeed = c_list[mon_ptr->mptr].speed + py.flags.speed;
      mon_ptr->stunned = 0;
      mon_ptr->cdis = distance(char_row, char_col,y,x);
      cave[y][x].cptr = cur_pos;
      mon_ptr->csleep = 0;
    }
}

/* Allocates a random monster				-RAK-	*/
alloc_monster(alloc_set, num, dis, slp)
int (*alloc_set)();
int num, dis;
int slp;
{
  register int y, x, i, j, k;

  for (i = 0; i < num; i++)
    {
      do
	{
	  y = randint(cur_height-2);
	  x = randint(cur_width-2);
	}
      while ((!(*alloc_set)(cave[y][x].fval)) ||
		 (cave[y][x].cptr != 0) ||
		 (!cave[y][x].fopen) ||
		 (distance(y,x,char_row, char_col) <= dis));
      if (dun_level == 0)
	j = randint(m_level[0]) - 1;
      else if (dun_level >= MAX_MONS_LEVEL)
	j = randint(m_level[MAX_MONS_LEVEL]) - 1 + m_level[0];
      else if (randint(MON_NASTY) == 1)
	{
	  /* abs may be a macro, don't call it with randnor as a parameter */
	  k = randnor(0, 4);
	  j = dun_level + abs(k) + 1;
	  if (j > MAX_MONS_LEVEL)
	    j = MAX_MONS_LEVEL;
	  k = m_level[j] - m_level[j-1];
	  j = randint(k) - 1 + m_level[j-1];
	}
      else
	j = randint(m_level[dun_level]) - 1 + m_level[0];
      place_monster(y, x, j, slp);
    }
}


/* Places creature adjacent to given location		-RAK-	*/
int summon_monster(y, x, slp)
int *y, *x;
int slp;
{
  register int i, j, k;
  int l, m;
  register cave_type *cave_ptr;
  int summon;

  i = 0;
  m = dun_level + MON_SUMMON_ADJ;
  summon = FALSE;
  if (m > MAX_MONS_LEVEL)
    l = MAX_MONS_LEVEL;
  else
    l = m;
  if (dun_level == 0)
    l = randint(m_level[0]) - 1;
  else
    l = randint(m_level[l]) - 1 + m_level[0];
  do
    {
      j = *y - 2 + randint(3);
      k = *x - 2 + randint(3);
      if (in_bounds(j, k))
	{
	  cave_ptr = &cave[j][k];
	  if ((cave_ptr->fval == 1) || (cave_ptr->fval == 2) ||
	      (cave_ptr->fval == 4) || (cave_ptr->fval == 5))
	    if (cave_ptr->cptr == 0)
	      if (cave_ptr->fopen)
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
  int l, m, ctr;
  int summon;
  register cave_type *cave_ptr;

  i = 0;
  summon = FALSE;
  l = m_level[MAX_MONS_LEVEL] + m_level[0];
  do
    {
      m = randint(l) - 1;
      ctr = 0;
      do
	{
	  if (c_list[m].cdefense & 0x0008)
	    {
	      ctr = 20;
	      l  = 0;
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
	  if ((cave_ptr->fval == 1) || (cave_ptr->fval == 2) ||
	      (cave_ptr->fval == 4) || (cave_ptr->fval == 5))
	    if ((cave_ptr->cptr == 0) && (cave_ptr->fopen))
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
compact_objects()
{
  register int i, j;
  int ctr, cur_dis;
  int flag;
  register cave_type *cave_ptr;
  register treasure_type *t_ptr;

  ctr = 0;
  cur_dis = 66;
  do
    {
      for (i = 0; i < cur_height; i++)
	for (j = 0; j < cur_width; j++)
	  {
	    cave_ptr = &cave[i][j];
	    if (cave_ptr->tptr != 0)
	      if (distance(i, j, char_row, char_col) > cur_dis)
		{
		  flag = FALSE;
		  t_ptr = &t_list[cave_ptr->tptr];
		  switch(t_ptr->tval)
		    {
		    case 102:
		      if ((t_ptr->subval == 1) || (t_ptr->subval == 6) ||
			  (t_ptr->subval == 9))
			flag = TRUE;
		      else if (randint(4) == 1)
			flag = TRUE;
		      break;
		    case 103:
		      flag = TRUE;
		      break;
		    case 104: case 105:
		      /* doors */
		      if (randint(4) == 1)  flag = TRUE;
		      break;
		    case 107: case 108:
		      /* stairs, don't delete them */
		      break;
		    case 110:
		      /* shop doors, don't delete them */
		      break;
		    default:
		      if (randint(8) == 1)  flag = TRUE;
		    }
		  if (flag)
		    {
		      cave_ptr->fopen = TRUE;
		      t_list[cave_ptr->tptr] = blank_treasure;
		      t_list[cave_ptr->tptr].p1 = tcptr;
		      tcptr = cave_ptr->tptr;
		      cave_ptr->tptr = 0;
		      ctr++;
		    }
		}
	    if (ctr == 0)  cur_dis -= 6;
	  }
    }
  while (ctr <= 0);
  if (cur_dis < 66)  prt_map();
}



/* Gives pointer to next free space			-RAK-	*/
popt(x)
int *x;
{
  if (tcptr < 1)
    compact_objects();
  *x = tcptr;
  tcptr = t_list[*x].p1;
}


/* Pushs a record back onto free space list		-RAK-	*/
pusht(x)
register int x;
{
  t_list[x] = blank_treasure;
  t_list[x].p1 = tcptr;
  tcptr = x;
}


/* Order the treasure list by level			-RAK-	*/
sort_objects()
{
  register int i, j, k, gap;
  treasure_type tmp;

  gap = MAX_OBJECTS / 2;
  while (gap > 0)
    {
      for (i = gap; i < MAX_OBJECTS; i++)
	{
	  j = i - gap;
	  while (j >= 0)
	    {
	      k = j + gap;
	      if (object_list[j].level > object_list[k].level)
		{
		  tmp = object_list[j];
		  object_list[j] = object_list[k];
		  object_list[k] = tmp;
		}
	      else
		j = -1;
	      j -= gap;
	    }
	}
      gap = gap / 2;
    }
}


/* Boolean : is object enchanted 	  -RAK- */
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
  register int x, stand_dev;
  register int tmp;

  stand_dev = (OBJ_STD_ADJ*level) + OBJ_STD_MIN;
  if (stand_dev > max_std)
    stand_dev = max_std;
  /* abs may be a macro, don't call it with randnor as a parameter */
  tmp = randnor(0, stand_dev);
  x = (abs(tmp)/10.0) + base;
  if (x < base)
    return(base);
  else
    return(x);
}


/* Chance of treasure having magic abilities		-RAK-	*/
/* Chance increases with each dungeon level                      */
magic_treasure(x, level)
int x, level;
{
  register treasure_type *t_ptr;
  register int chance, special, cursed, i;

  chance = OBJ_BASE_MAGIC + level;
  if (chance > OBJ_BASE_MAX)
    chance = OBJ_BASE_MAX;
  special = (chance/OBJ_DIV_SPECIAL);
  cursed  = (chance/OBJ_DIV_CURSED);
  t_ptr = &t_list[x];
/*  I vehemently disagree with this!! */
/*  t_ptr->level = level;  */
  /* Depending on treasure type, it can have certain magical properties*/
  switch (t_ptr->tval)
    {
    case 34: case 35: case 36:  /* Armor and shields*/
      if (magik(chance))
	{
	  t_ptr->toac = m_bonus(1, 30, level);
	  if (magik(special))
	    switch(randint(9))
	      {
	      case 1:
		t_ptr->flags |= 0x02380000;
		(void) strcat(t_ptr->name, " (R)");
		t_ptr->toac += 5;
		t_ptr->cost += 2500;
		break;
	      case 2:    /* Resist Acid   */
		t_ptr->flags |= 0x00100000;
		(void) strcat(t_ptr->name, " (RA)");
		t_ptr->cost += 1000;
		break;
	      case 3: case 4:    /* Resist Fire   */
		t_ptr->flags |= 0x00080000;
		(void) strcat(t_ptr->name, " (RF)");
		t_ptr->cost += 600;
		break;
	      case 5: case 6:   /* Resist Cold   */
		t_ptr->flags |= 0x00200000;
		(void) strcat(t_ptr->name, " (RC)");
		t_ptr->cost += 600;
		break;
	      case 7: case 8: case 9:  /* Resist Lightning*/
		t_ptr->flags |= 0x02000000;
		(void) strcat(t_ptr->name, " (RL)");
		t_ptr->cost += 500;
		break;
	      }
	}
      else if (magik(cursed))
	{
	  t_ptr->toac = -m_bonus(1, 40, level);
	  t_ptr->cost = 0;
	  t_ptr->flags |= 0x80000000;
	}
      break;

    case 21: case 22: case 23:  /* Weapons       */
      if (magik(chance))
	{
	  t_ptr->tohit = m_bonus(0, 40, level);
	  t_ptr->todam = m_bonus(0, 40, level);
	  if (magik(special))
	    switch(randint(16))
	      {
		case 1:   /* Holy Avenger  */
		t_ptr->flags |= 0x01418001;
		t_ptr->tohit += 5;
		t_ptr->todam += 5;
		t_ptr->toac  = randint(4);
		/* the value in p1 is used for strength increase */
		t_ptr->p1    = randint(4);
		(void) strcat(t_ptr->name, " [%P4] (HA) (%P1 to STR)");
		t_ptr->cost += t_ptr->p1*500;
		t_ptr->cost += 10000;
		break;
	      case 2:   /* Defender      */
		t_ptr->flags |= 0x07B80900;
		t_ptr->tohit += 3;
		t_ptr->todam += 3;
		t_ptr->toac  = 5 + randint(5);
		(void) strcat(t_ptr->name, " [%P4] (DF)");
		/* note that the value in p1 is unused */
		t_ptr->p1    = randint(3);
		t_ptr->cost += t_ptr->p1*500;
		t_ptr->cost += 7500;
		break;
	      case 3: case 4:    /* Slay Monster  */
		t_ptr->flags |= 0x01004000;
		t_ptr->tohit += 3;
		t_ptr->todam += 3;
		(void) strcat(t_ptr->name, " (SM)");
		t_ptr->cost += 5000;
		break;
	      case 5: case 6:   /* Slay Dragon   */
		t_ptr->flags |= 0x00002000;
		t_ptr->tohit += 3;
		t_ptr->todam += 3;
		(void) strcat(t_ptr->name, " (SD)");
		t_ptr->cost += 4000;
		break;
	      case 7: case 8:     /* Slay Evil     */
		t_ptr->flags |= 0x00008000;
		t_ptr->tohit += 3;
		t_ptr->todam += 3;
		(void) strcat(t_ptr->name, " (SE)");
		t_ptr->cost += 4000;
		break;
	      case 9: case 10:   /* Slay Undead   */
		t_ptr->flags |= 0x00010000;
		t_ptr->tohit += 2;
		t_ptr->todam += 2;
		(void) strcat(t_ptr->name, " (SU)");
		t_ptr->cost += 3000;
		break;
	      case 11: case 12: case 13:   /* Flame Tongue  */
		t_ptr->flags |= 0x00040000;
		t_ptr->tohit++;
		t_ptr->todam += 3;
		(void) strcat(t_ptr->name, " (FT)");
		t_ptr->cost += 2000;
		break;
	      case 14: case 15: case 16:   /* Frost Brand   */
		t_ptr->flags |= 0x00020000;
		t_ptr->tohit++;
		t_ptr->todam++;
		(void) strcat(t_ptr->name, " (FB)");
		t_ptr->cost += 1200;
		break;
	      }
	}
      else if (magik(cursed))
	{
	  t_ptr->tohit = -m_bonus(1, 55, level);
	  t_ptr->todam = -m_bonus(1, 55, level);
	  t_ptr->flags |= 0x80000000;
	  t_ptr->cost = 0;
	}
      break;

    case 20:  /* Bows, crossbows, and slings   */
      if (magik(chance))
	t_ptr->tohit = m_bonus(1, 30, level);
      else if (magik(cursed))
	{
	  t_ptr->tohit = -m_bonus(1, 50, level);
	  t_ptr->flags |= 0x80000000;
	  t_ptr->cost = 0;
	}
      break;

    case 25:  /* Digging tools         */
      if (magik(chance))
	switch(randint(3))
	  {
	  case 1: case 2:
	    t_ptr->p1 = m_bonus(2, 25, level);
	    t_ptr->cost += t_ptr->p1*100;
	    break;
	  case 3:
	    t_ptr->p1 = -m_bonus(1, 30, level);
	    t_ptr->cost = 0;
	    t_ptr->flags |= 0x80000000;
	  }
      break;

    case 31:  /* Gloves and Gauntlets  */
      if (magik(chance))
	{
	  t_ptr->toac = m_bonus(1, 20, level);
	  if (magik(special))
	    switch(randint(2))
	      {
	      case 1:
		t_ptr->flags |= 0x00800000;
		(void) strcat(t_ptr->name, " of Free Action");
		t_ptr->cost += 1000;
		break;
	      case 2:
		t_ptr->tohit = 1 + randint(3);
		t_ptr->todam = 1 + randint(3);
		(void) strcat(t_ptr->name, " of Slaying (%P2,%P3)");
		t_ptr->cost += (t_ptr->tohit+t_ptr->todam)*250;
		break;
	      }
	}
      else if (magik(cursed))
	{
	  if (magik(special))
	    switch(randint(2))
	      {
	      case 1:
		t_ptr->flags |= 0x80000002;
		(void) strcat(t_ptr->name, " of Clumsiness");
		t_ptr->p1 = 1;
		break;
	      case 2:
		t_ptr->flags |= 0x80000001;
		(void) strcat(t_ptr->name, " of Weakness");
		t_ptr->p1 = 1;
		break;
	      }
	  t_ptr->toac = -m_bonus(1, 40, level);
	  t_ptr->p1   = -m_bonus(1, 10, level);
	  t_ptr->flags |= 0x80000000;
	  t_ptr->cost = 0;
	}
      break;

    case 30:  /* Boots */
      if (magik(chance))
	{
	  t_ptr->toac = m_bonus(1, 20, level);
	  if (magik(special))
	    switch(randint(12))
	      {
	      case 1:
		t_ptr->flags |= 0x00001000;
		(void) strcat(t_ptr->name, " of Speed");
		t_ptr->p1 = 1;
		t_ptr->cost += 5000;
		break;
	      case 2: case 3: case 4: case 5:
		t_ptr->flags |= 0x00000100;
		t_ptr->p1 = randint(3);
		(void) strcat(t_ptr->name, " of Stealth (%P1)");
		t_ptr->cost += 500;
		break;
	      default:
		t_ptr->flags |= 0x04000000;
		(void) strcat(t_ptr->name, " of Slow descent");
		t_ptr->cost += 250;
		break;
	      }
	}
      else if (magik(cursed))
	{
	switch(randint(3))
	  {
	    case 1:
	    t_ptr->flags |= 0x80001000;
	    (void) strcat(t_ptr->name, " of Slowness");
	    t_ptr->p1 = -1;
	    break;
	  case 2:
	    t_ptr->flags |= 0x80000200;
	    (void) strcat(t_ptr->name, " of Noise");
	    break;
	  case 3:
	    t_ptr->flags |= 0x80000000;
	    (void) strcat(t_ptr->name, " of Great Mass");
	    t_ptr->weight = t_ptr->weight * 5;
	    break;
	  }
	t_ptr->cost = 0;
	t_ptr->ac = -m_bonus(2, 45, level);
      }
      break;

    case 33:  /* Helms */
      if (magik(chance))
	{
	  t_ptr->toac = m_bonus(1, 20, level);
	  if (magik(special))
	    switch(t_ptr->subval)
	      {
	      case 1: case 2: case 3: case 4: case 5:
		switch(randint(3))
		  {
		  case 1:
		    t_ptr->p1 = randint(2);
		    t_ptr->flags |= 0x00000008;
		    (void) strcat(t_ptr->name, " of Intelligence (%P1)");
		    t_ptr->cost += t_ptr->p1*500;
		    break;
		  case 2:
		    t_ptr->p1 = randint(2);
		    t_ptr->flags |= 0x00000010;
		      (void) strcat(t_ptr->name, " of Wisdom (%P1)");
		    t_ptr->cost += t_ptr->p1*500;
		    break;
		  case 3:
		    t_ptr->p1 = 1 + randint(4);
		    t_ptr->flags |= 0x40000000;
		    (void) strcat(t_ptr->name, " of Infra-Vision (%P1)");
		    t_ptr->cost += t_ptr->p1*250;
		    break;
		  }
		break;
	      case 6: case 7: case 8:
		switch(randint(6))
		  {
		  case 1:
		    t_ptr->p1 = randint(3);
		    t_ptr->flags |= 0x00800007;
		    (void) strcat(t_ptr->name, " of Might (%P1)");
		    t_ptr->cost += 1000 + t_ptr->p1*500;
		    break;
		  case 2:
		    t_ptr->p1 = randint(3);
		    t_ptr->flags |= 0x00000030;
		    (void) strcat(t_ptr->name, " of Lordliness (%P1)");
		    t_ptr->cost += 1000 + t_ptr->p1*500;
		    break;
		  case 3:
		    t_ptr->p1 = randint(3);
		    t_ptr->flags |= 0x01380008;
		    (void) strcat(t_ptr->name, " of the Magi (%P1)");
		    t_ptr->cost += 3000 + t_ptr->p1*500;
		    break;
		  case 4:
		    t_ptr->p1 = randint(3);
		    t_ptr->flags |= 0x00000020;
		    (void) strcat(t_ptr->name, " of Beauty (%P1)");
		    t_ptr->cost += 750;
		    break;
		  case 5:
		    t_ptr->p1 = 1 + randint(4);
		    t_ptr->flags |= 0x01000040;
		    (void) strcat(t_ptr->name, " of Seeing (%P1)");
		    t_ptr->cost += 1000 + t_ptr->p1*100;
		    break;
		  case 6:
		    t_ptr->flags |= 0x00000800;
		    (void) strcat(t_ptr->name, " of Regeneration");
		    t_ptr->cost += 1500;
		    break;
		  }
		break;
	      }
	  else if (magik(cursed))
	    {
	      t_ptr->toac = -m_bonus(1, 45, level);
	      t_ptr->flags |= 0x80000000;
	      t_ptr->cost = 0;
	      if (magik(special))
	      switch(randint(7))
		{
		case 1:
		  t_ptr->p1 = -1;
		  t_ptr->flags |= 0x00000008;
		  (void) strcat(t_ptr->name, " of Stupidity");
		  break;
		case 2:
		  t_ptr->p1 = -1;
		  t_ptr->flags |= 0x00000010;
		  (void) strcat(t_ptr->name, " of Dullness");
		  break;
		case 3:
		  t_ptr->flags |= 0x08000000;
		  (void) strcat(t_ptr->name, " of Blindness");
		  break;
		case 4:
		  t_ptr->flags |= 0x10000000;
		  (void) strcat(t_ptr->name, " of Timidness");
		  break;
		case 5:
		  t_ptr->p1 = -1;
		  t_ptr->flags |= 0x00000001;
		  (void) strcat(t_ptr->name, " of Weakness");
		  break;
		case 6:
		  t_ptr->flags |= 0x00000400;
		  (void) strcat(t_ptr->name, " of Teleportation");
		  break;
		case 7:
		  t_ptr->p1 = -1;
		  t_ptr->flags |= 0x00000020;
		  (void) strcat(t_ptr->name, " of Ugliness");
		  break;
		}
	      t_ptr->p1 = t_ptr->p1 * randint (5);
	    }
	}
      break;

    case 45: /* Rings         */
      switch(t_ptr->subval)
	{
	case 1: case 2: case 3: case 4: case 5: case 6:
	  if (magik(cursed))
	    {
	      t_ptr->p1 = -m_bonus(1, 20, level);
	      t_ptr->flags |= 0x80000000;
	      t_ptr->cost = -t_ptr->cost;
	    }
	  else
	    {
	      t_ptr->p1 = m_bonus(1, 10, level);
	      t_ptr->cost += t_ptr->p1*100;
	    }
	  break;
	case 7:
	  if (magik(cursed))
	    {
	      t_ptr->p1 = -randint(3);
	      t_ptr->flags |= 0x80000000;
	      t_ptr->cost = -t_ptr->cost;
	    }
	  else
	    t_ptr->p1 = 1;
	  break;
	case 8:
	  t_ptr->p1 = 5*m_bonus(1, 20, level);
	  t_ptr->cost += t_ptr->p1*100;
	  break;
	case 22:     /* Increase damage       */
	  t_ptr->todam = m_bonus(1, 20, level);
	  t_ptr->cost += t_ptr->todam*100;
	  if (magik(cursed))
	    {
	      t_ptr->todam = -t_ptr->todam;
	      t_ptr->flags |= 0x80000000;
	      t_ptr->cost = -t_ptr->cost;
	    }
	  break;
	case 23:     /* Increase To-Hit       */
	  t_ptr->tohit = m_bonus(1, 20, level);
	  t_ptr->cost += t_ptr->tohit*100;
	  if (magik(cursed))
	    {
	      t_ptr->tohit = -t_ptr->tohit;
	      t_ptr->flags |= 0x80000000;
	      t_ptr->cost = -t_ptr->cost;
	    }
	  break;
	case 24:     /* Protection            */
	  t_ptr->toac = m_bonus(1, 20, level);
	  t_ptr->cost += t_ptr->toac*100;
	  if (magik(cursed))
	    {
	      t_ptr->toac = -t_ptr->toac;
	      t_ptr->flags |= 0x80000000;
	      t_ptr->cost = -t_ptr->cost;
	    }
	  break;
	case 33:     /* Slaying       */
	  t_ptr->todam = m_bonus(1, 25, level);
	  t_ptr->tohit = m_bonus(1, 25, level);
	  t_ptr->cost += (t_ptr->tohit+t_ptr->todam)*100;
	  if (magik(cursed))
	    {
	      t_ptr->tohit = -t_ptr->tohit;
	      t_ptr->todam = -t_ptr->todam;
	      t_ptr->flags |= 0x80000000;
	      t_ptr->cost = -t_ptr->cost;
	    }
	  break;
	default:
	  break;
	}
      break;

    case 40: /* Amulets       */
      switch(t_ptr->subval)
	{
	case 1: case 2: case 3: case 4: case 5: case 6:
	  if (magik(cursed))
	    {
	      t_ptr->p1 = -m_bonus(1, 20, level);
	      t_ptr->flags |= 0x80000000;
	      t_ptr->cost = -t_ptr->cost;
	    }
	  else
	    {
	      t_ptr->p1 = m_bonus(1, 10, level);
	      t_ptr->cost += t_ptr->p1*100;
	    }
	  break;
	case 7:
	  t_ptr->p1 = 5*m_bonus(1, 25, level);
	  if (magik(cursed))
	    {
	      t_ptr->p1 = -t_ptr->p1;
	      t_ptr->cost = -t_ptr->cost;
	      t_ptr->flags |= 0x80000000;
	    }
	  else
	    t_ptr->cost += 20*t_ptr->p1;
	  break;
	default:
	  break;
	}
      break;

      /* Subval should be even for store, odd for dungeon*/
      /* Dungeon found ones will be partially charged    */
    case 15: /* Lamps and torches*/
      if ((t_ptr->subval % 2) == 1)
	t_ptr->p1 = randint(t_ptr->p1);
      break;

    case 65: /* Wands         */
      switch(t_ptr->subval)
	{
	case 1:   t_ptr->p1 = randint(10) + 	 6; break;
	case 2:   t_ptr->p1 = randint(8)  + 	 6; break;
	case 3:   t_ptr->p1 = randint(5)  + 	 6; break;
	case 4:   t_ptr->p1 = randint(8)  + 	 6; break;
	case 5:   t_ptr->p1 = randint(4)  + 	 3; break;
	case 6:   t_ptr->p1 = randint(8)  + 	 6; break;
	case 7:   t_ptr->p1 = randint(20) + 	 12; break;
	case 8:   t_ptr->p1 = randint(20) + 	 12; break;
	case 9:   t_ptr->p1 = randint(10) + 	 6; break;
	case 10:   t_ptr->p1 = randint(12) + 	 6; break;
	case 11:   t_ptr->p1 = randint(10) + 	 12; break;
	case 12:   t_ptr->p1 = randint(3)  + 	 3; break;
	case 13:   t_ptr->p1 = randint(8)  + 	 6; break;
	case 14:   t_ptr->p1 = randint(10) + 	 6; break;
	case 15:   t_ptr->p1 = randint(5)  + 	 3; break;
	case 16:   t_ptr->p1 = randint(5)  + 	 3; break;
	case 17:   t_ptr->p1 = randint(5)  + 	 6; break;
	case 18:   t_ptr->p1 = randint(5)  + 	 4; break;
	case 19:   t_ptr->p1 = randint(8)  + 	 4; break;
	case 20:   t_ptr->p1 = randint(6)  + 	 2; break;
	case 21:   t_ptr->p1 = randint(4)  + 	 2; break;
	case 22:   t_ptr->p1 = randint(8)  + 	 6; break;
	case 23:   t_ptr->p1 = randint(5)  + 	 2; break;
	case 24:   t_ptr->p1 = randint(12) + 12; break;
	default:
	  break;
	}
      break;

    case 55: /* Staffs        */
      switch(t_ptr->subval)
	{
	case 1:   t_ptr->p1 = randint(20) + 	 12; break;
	case 2:   t_ptr->p1 = randint(8)  + 	 6; break;
	case 3:   t_ptr->p1 = randint(5)  + 	 6; break;
	case 4:   t_ptr->p1 = randint(20) + 	 12; break;
	case 5:   t_ptr->p1 = randint(15) + 	 6; break;
	case 6:   t_ptr->p1 = randint(4)  + 	 5; break;
	case 7:   t_ptr->p1 = randint(5)  + 	 3; break;
	case 8:   t_ptr->p1 = randint(3)  + 	 1; break;
	case 9:   t_ptr->p1 = randint(3)  + 	 1; break;
	case 10:   t_ptr->p1 = randint(3)  + 	 1; break;
	case 11:   t_ptr->p1 = randint(5)  + 	 6; break;
	case 12:   t_ptr->p1 = randint(10) + 	 12; break;
	case 13:   t_ptr->p1 = randint(5)  + 	 6; break;
	case 14:   t_ptr->p1 = randint(5)  + 	 6; break;
	case 15:   t_ptr->p1 = randint(5)  + 	 6; break;
	case 16:   t_ptr->p1 = randint(10) + 	 12; break;
	case 17:   t_ptr->p1 = randint(3)  + 	 4; break;
	case 18:   t_ptr->p1 = randint(5)  + 	 6; break;
	case 19:   t_ptr->p1 = randint(5)  + 	 6; break;
	case 20:   t_ptr->p1 = randint(3)  + 	 4; break;
	case 21:   t_ptr->p1 = randint(10) + 	 12; break;
	case 22:   t_ptr->p1 = randint(3)  + 	 4; break;
	case 23:   t_ptr->p1 = randint(3)  + 	 4; break;
	case 24:   t_ptr->p1 = randint(3)  + 	 1; break;
	case 25:   t_ptr->p1 = randint(10) + 6; break;
	default:
	  break;
	}
      break;

    case 32: /* Cloaks        */
      if (magik(chance))
	{
	  if (magik(special))
	    switch(randint(2))
	      {
	      case 1:
		(void) strcat(t_ptr->name, " of Protection");
		t_ptr->toac = m_bonus(2, 40, level);
		t_ptr->cost += 250;
		break;
	      case 2:
		t_ptr->toac = m_bonus(1, 20, level);
		t_ptr->p1 = randint(3);
		t_ptr->flags |= 0x00000100;
		(void) strcat(t_ptr->name, " of Stealth (%P1)");
		t_ptr->cost += 500;
		break;
	      }
	  else
	    t_ptr->toac = m_bonus(1, 20, level);
	}
      else if (magik(cursed))
	switch(randint(3))
	  {
	  case 1:
	    t_ptr->flags |= 0x80000200;
	    (void) strcat(t_ptr->name, " of Irritation");
	    t_ptr->ac   =  0;
	    t_ptr->toac  = -m_bonus(1, 10, level);
	    t_ptr->tohit = -m_bonus(1, 10, level);
	    t_ptr->todam = -m_bonus(1, 10, level);
	    t_ptr->cost =  0;
	    break;
	  case 2:
	    t_ptr->flags |= 0x80000000;
	    (void) strcat(t_ptr->name, " of Vulnerability");
	    t_ptr->ac   = 0;
	    t_ptr->toac = -m_bonus(10, 100, level+50);
	    t_ptr->cost = 0;
	    break;
	  case 3:
	    t_ptr->flags |= 0x80000000;
	    (void) strcat(t_ptr->name, " of Enveloping");
	    t_ptr->toac  = -m_bonus(1, 10, level);
	    t_ptr->tohit = -m_bonus(2, 40, level+10);
	    t_ptr->todam = -m_bonus(2, 40, level+10);
	    t_ptr->cost = 0;
	    break;
	  }
      break;

    case 2: /* Chests        */
      switch(randint(level+4))
	{
	case 1:
	  t_ptr->flags = 0;
	  (void) strcat(t_ptr->name, "^ (Empty)");
	  break;
	case 2:
	  t_ptr->flags |= 0x00000001;
	  (void) strcat(t_ptr->name, "^ (Locked)");
	  break;
	case 3: case 4:
	  t_ptr->flags |= 0x00000011;
	  (void) strcat(t_ptr->name, "^ (Poison Needle)");
	  break;
	case 5: case 6:
	  t_ptr->flags |= 0x00000021;
	  (void) strcat(t_ptr->name, "^ (Poison Needle)");
	  break;
	case 7: case 8: case 9:
	  t_ptr->flags |= 0x00000041;
	  (void) strcat(t_ptr->name, "^ (Gas Trap)");
	  break;
	case 10: case 11:
	  t_ptr->flags |= 0x00000081;
	  (void) strcat(t_ptr->name, "^ (Explosion Device)");
	  break;
	case 12: case 13: case 14:
	  t_ptr->flags |= 0x00000101;
	  (void) strcat(t_ptr->name, "^ (Summoning Runes)");
	  break;
	case 15: case 16: case 17:
	  t_ptr->flags |= 0x00000071;
	  (void) strcat(t_ptr->name, "^ (Multiple Traps)");
	  break;
	default:
	  t_ptr->flags |= 0x00000181;
	  (void) strcat(t_ptr->name, "^ (Multiple Traps)");
	  break;
	}
      break;

    case 10: case 11: case 12: case 13:  /* Arrows, bolts, ammo, and spikes */
      if ((t_ptr->tval == 11) || (t_ptr->tval == 12))
	if (magik(chance))
	  {
	    t_ptr->tohit = m_bonus(1, 35, level);
	    t_ptr->todam = m_bonus(1, 35, level);
	    if (magik(special))
	      switch(t_ptr->tval)  /*SWITCH 1*/
		{
		case 11: case 12:
		  switch(randint(10))   /*SWITCH 2*/
		    {
		    case 1: case 2: case 3:
		      (void) strcat(t_ptr->name, " of Slaying");
		      t_ptr->tohit += 5;
		      t_ptr->todam += 5;
		      t_ptr->cost += 20;
		      break;
		    case 4: case 5:
		      t_ptr->flags |= 0x00040000;
		      t_ptr->tohit += 2;
		      t_ptr->todam += 4;
		      (void) strcat(t_ptr->name, " of Fire");
		      t_ptr->cost += 25;
		      break;
		    case 6: case 7:
		      t_ptr->flags |= 0x00008000;
		      t_ptr->tohit += 3;
		      t_ptr->todam += 3;
		      (void) strcat(t_ptr->name, " of Slay Evil");
		      t_ptr->cost += 25;
		      break;
		    case 8: case 9:
		      t_ptr->flags |= 0x01004000;
		      t_ptr->tohit += 2;
		      t_ptr->todam += 2;
		      (void) strcat(t_ptr->name, " of Slay Monster");
		      t_ptr->cost += 30;
		      break;
		    case 10:
		      t_ptr->flags |= 0x00002000;
		      t_ptr->tohit += 10;
		      t_ptr->todam += 10;
		      (void) strcat(t_ptr->name, " of Dragon Slaying");
		      t_ptr->cost += 35;
		      break;
		    } /*SWITCH 2*/
		default:
		  break;
		}  /*SWITCH 1*/
	  }
	else if (magik(cursed))
	  {
	    t_ptr->tohit = -m_bonus(5, 55, level);
	    t_ptr->todam = -m_bonus(5, 55, level);
	    t_ptr->flags |= 0x80000000;
	    t_ptr->cost = 0;
	  }

      t_ptr->number = 0;
      for (i = 0; i < 7; i++)
	t_ptr->number += randint(6);
      missile_ctr++;
      if (missile_ctr > 65000)
	missile_ctr = 1;
      t_ptr->subval = missile_ctr + 512;
      break;

    default:
      break;
    }
}


