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

/* Generates character's stats				-JWT-	*/
int get_stat()
{
  register int i;

  i = randint(4) + randint(4) + randint(4) + 5;
  return(i);
}


/* Changes stats by given amount				-JWT-	*/
byteint change_stat(cur_stat, amount)
byteint cur_stat;
int amount;
{
  register int i;

  if (amount < 0)
    for (i = 0; i > amount; i--)
      cur_stat = de_statp(cur_stat);
  else
    for (i = 0; i < amount; i++)
      cur_stat = in_statp(cur_stat);
  return(cur_stat);
}


/* generate all stats and modify for race... needed in a separate module so
   looping of character selection would be allowed     -RGM- */
void get_stats ()
{
  register player_type *p_ptr;
  register race_type *r_ptr;

  p_ptr = &py;
  r_ptr = &race[p_ptr->misc.prace];
  p_ptr->stats.str    = get_stat();
  p_ptr->stats.intel  = get_stat();
  p_ptr->stats.wis    = get_stat();
  p_ptr->stats.dex    = get_stat();
  p_ptr->stats.con    = get_stat();
  p_ptr->stats.chr    = get_stat();
  p_ptr->stats.str    = change_stat(p_ptr->stats.str, r_ptr->str_adj);
  p_ptr->stats.intel  = change_stat(p_ptr->stats.intel, r_ptr->int_adj);
  p_ptr->stats.wis    = change_stat(p_ptr->stats.wis, r_ptr->wis_adj);
  p_ptr->stats.dex    = change_stat(p_ptr->stats.dex, r_ptr->dex_adj);
  p_ptr->stats.con    = change_stat(p_ptr->stats.con, r_ptr->con_adj);
  p_ptr->stats.chr    = change_stat(p_ptr->stats.chr, r_ptr->chr_adj);
  p_ptr->stats.cstr   = p_ptr->stats.str;
  p_ptr->stats.cint   = p_ptr->stats.intel;
  p_ptr->stats.cwis   = p_ptr->stats.wis;
  p_ptr->stats.cdex   = p_ptr->stats.dex;
  p_ptr->stats.ccon   = p_ptr->stats.con;
  p_ptr->stats.cchr   = p_ptr->stats.chr;
  p_ptr->misc.srh    = r_ptr->srh;
  p_ptr->misc.bth    = r_ptr->bth;
  p_ptr->misc.bthb   = r_ptr->bthb;
  p_ptr->misc.fos    = r_ptr->fos;
  p_ptr->misc.stl    = r_ptr->stl;
  p_ptr->misc.save   = r_ptr->bsav;
  p_ptr->misc.hitdie = r_ptr->bhitdie;
  p_ptr->misc.lev    = 1;
  p_ptr->misc.ptodam = todam_adj();
  p_ptr->misc.ptohit = tohit_adj();
  p_ptr->misc.ptoac  = 0;
  p_ptr->misc.pac    = toac_adj();
  p_ptr->misc.expfact = r_ptr->b_exp;
  p_ptr->flags.see_infra = r_ptr->infra;
}


/* Allows player to select a race			-JWT-	*/
int choose_race()
{
  register int j, k;
  int l, m;
  char s;
  int exit_flag;
  char tmp_str[80];
  register player_type *p_ptr;
  register race_type *r_ptr;
  int res;

  j = 0;
  k = 0;
  l = 2;
  m = 21;
  clear_screen(20, 0);
/*  help is unimplemented */
/*  prt("Choose a race (? for Help):", 20, 2); */
  prt("Choose a race:", 20, 2);
  do
    {
      (void) sprintf(tmp_str, "%c) %s", k+97, race[j].trace);
      put_buffer(tmp_str, m, l);
      k++;
      l += 15;
      if (l > 70)
	{
	  l = 2;
	  m++;
	}
      j++;
    }
  while (j < MAX_RACES);
  /* clear race string */
  py.misc.race[0] = '\0';
  move_cursor (20, 18);
  exit_flag = FALSE;
  do
    {
      inkey(&s);
      j = s - 97;
      if ((j < MAX_RACES) && (j >= 0))
	{
	  p_ptr = &py;
	  r_ptr = &race[j];
	  p_ptr->misc.prace  = j;
	  (void) strcpy(p_ptr->misc.race, r_ptr->trace);
	  get_stats();                  /* We don't need the code twice. */
	  exit_flag = TRUE;             /* so use function get_stats -RGM- */
	  res = TRUE;
	  put_buffer(py.misc.race, 3, 14);
	}
    }
  while (!exit_flag);
  return(res);
}


/* Will print the history of a character			-JWT-	*/
print_history()
{
  register int i;

  put_buffer("Character Background", 13, 27);
  for(i = 0; i < 5; i++)
    put_buffer(py.misc.history[i], i+14, 4);
}


/* Get the racial history, determines social class	-RAK-	*/
/* Assumptions:	Each race has init history beginning at 	*/
/*		(race-1)*3+1					*/
/*		All history parts are in ascending order	*/
get_history()
{
  int hist_ptr, cur_ptr, test_roll;
  register int start_pos, end_pos, cur_len;
  int line_ctr, new_start, social_class;
  char history_block[400];
  vtype tmp_str;
  int flag;
  register background_type *b_ptr;

  /* Get a block of history text				*/
  hist_ptr = py.misc.prace*3 + 1;
  history_block[0] = '\0';
  social_class = randint(4);
  cur_ptr = 0;
  do
    {
      flag = FALSE;
      do
	{
	  if (background[cur_ptr].chart == hist_ptr)
	    {
	      test_roll = randint(100);
	      while (test_roll > background[cur_ptr].roll)
		cur_ptr++;
	      b_ptr = &background[cur_ptr];
	      (void) strcat(history_block, b_ptr->info);
	      social_class += b_ptr->bonus;
	      if (hist_ptr > b_ptr->next)
		cur_ptr = 0;
	      hist_ptr = b_ptr->next;
	      flag = TRUE;
	    }
	  else
	    cur_ptr++;
	}
      while (!flag);
    }
  while (hist_ptr >= 1);

  /* Process block of history text for pretty output	*/
  start_pos = 0;
  end_pos   = strlen(history_block) - 1;
  line_ctr  = 0;
  flag = FALSE;
  while (history_block[end_pos] == ' ')
    end_pos--;
  do
    {
      while (history_block[start_pos] == ' ')
	start_pos++;
      cur_len = end_pos - start_pos + 1;
      if (cur_len > 70)
	{
	  cur_len = 70;
	  while (history_block[start_pos+cur_len-1] != ' ')
	    cur_len--;
	  new_start = start_pos + cur_len;
	  while (history_block[start_pos+cur_len-1] == ' ')
	    cur_len--;
	}
      else
	flag = TRUE;
      (void) strncpy(tmp_str, &history_block[start_pos], cur_len);
      tmp_str[cur_len] = '\0';
      (void) strcpy(py.misc.history[line_ctr], tmp_str);
      line_ctr++;
      start_pos = new_start;
    }
  while (!flag);

  /* Compute social class for player			*/
  if (social_class > 100)
    social_class = 100;
  else if (social_class < 1)
    social_class = 1;
  py.misc.sc = social_class;
}


/* Gets the character's sex				-JWT-	*/
int get_sex()
{
  char s;
  int exit_flag;
  int sex;

  py.misc.sex[0] = '\0';
  clear_screen(20, 0);
/* help is unimplemented */
/*  prt("Choose a sex (? for Help):", 20, 2); */
  prt("Choose a sex:", 20, 2);
  prt("m) Male       f) Female", 21, 2);
  move_cursor (20, 16);
  do
    {
      inkey(&s);
      switch(s)
	{
	case 'f': case 'F':
	  (void) strcpy(py.misc.sex, "Female");
	  prt(py.misc.sex, 4, 14);
	  exit_flag = TRUE;
	  sex = TRUE;
	  break;
	case 'm': case 'M':
	  (void) strcpy(py.misc.sex, "Male");
	  prt(py.misc.sex, 4, 14);
	  exit_flag = TRUE;
	  sex = TRUE;
	  break;
	default:
	  sex = FALSE;
	  exit_flag = FALSE;
	  break;
	}
    }
  while (!exit_flag);
  return(sex);
}


/* Computes character's age, height, and weight		-JWT-	*/
get_ahw()
{
  register int i;

  i = py.misc.prace;
  py.misc.age = race[i].b_age + randint((int)race[i].m_age);
  switch(py.misc.sex[0])
    {
    case 'F': case 'f':
      py.misc.ht = randnor((int)race[i].f_b_ht, (int)race[i].f_m_ht);
      py.misc.wt = randnor((int)race[i].f_b_wt, (int)race[i].f_m_wt);
      break;
    case 'M': case 'm':
      py.misc.ht = randnor((int)race[i].m_b_ht, (int)race[i].m_m_ht);
      py.misc.wt = randnor((int)race[i].m_b_wt, (int)race[i].m_m_wt);
      break;
    }
  py.misc.disarm = race[i].b_dis + todis_adj();
}


/* Gets a character class				-JWT-	*/
int get_class()
{
  register int i, j;
  int k, l, m;
  int cl[MAX_CLASS];
  char s;
  int exit_flag;
  int res;
  register struct misc *m_ptr;
  register player_type *p_ptr;
  char tmp_str[80];

  for (j = 0; j < MAX_CLASS; j++)
    cl[j] = 0;
  i = py.misc.prace;
  j = 0;
  k = 0;
  l = 2;
  m = 21;
  clear_screen(20, 0);
/* help is unimplemented */
/* prt("Choose a class (? for Help):", 20, 2); */
  prt("Choose a class:", 20, 2);
  do
    {
      if (race[i].tclass & bit_array[j])
	{
	  (void) sprintf(tmp_str, "%c) %s", k+97, class[j].title);
	  put_buffer(tmp_str, m, l);
	  cl[k] = j;
	  l += 15;
	  if (l > 70)
	    {
	      l = 2;
	      m++;
	    }
	  k++;
	}
      j++;
    }
  while (j < MAX_CLASS);
  py.misc.pclass = 0;
  move_cursor (20, 19);
  exit_flag = FALSE;
  do
    {
      inkey(&s);
      j = s - 97;
      if ((j < k) && (j >= 0))
	{
	  (void) strcpy(py.misc.tclass, class[cl[j]].title);
	  py.misc.pclass = cl[j];
	  exit_flag = TRUE;
	  res = TRUE;
	  clear_screen(20, 0);
	  put_buffer(py.misc.tclass, 5, 14);

	  /* Adjust the stats for the class adjustment		-RAK-	*/
	  p_ptr = &py;
	  p_ptr->stats.str  = change_stat(p_ptr->stats.str,
					 class[p_ptr->misc.pclass].madj_str);
	  p_ptr->stats.intel= change_stat(p_ptr->stats.intel,
					 class[p_ptr->misc.pclass].madj_int);
	  p_ptr->stats.wis  = change_stat(p_ptr->stats.wis,
					 class[p_ptr->misc.pclass].madj_wis);
	  p_ptr->stats.dex  = change_stat(p_ptr->stats.dex,
					 class[p_ptr->misc.pclass].madj_dex);
	  p_ptr->stats.con  = change_stat(p_ptr->stats.con,
					 class[p_ptr->misc.pclass].madj_con);
	  p_ptr->stats.chr  = change_stat(p_ptr->stats.chr,
					 class[p_ptr->misc.pclass].madj_chr);
	  p_ptr->stats.cstr = p_ptr->stats.str;
	  p_ptr->stats.cint = p_ptr->stats.intel;
	  p_ptr->stats.cwis = p_ptr->stats.wis;
	  p_ptr->stats.cdex = p_ptr->stats.dex;
	  p_ptr->stats.ccon = p_ptr->stats.con;
	  p_ptr->stats.cchr = p_ptr->stats.chr;
	  p_ptr->misc.ptodam = todam_adj();	/* Real values		*/
	  p_ptr->misc.ptohit = tohit_adj();
	  p_ptr->misc.ptoac  = toac_adj();
	  p_ptr->misc.pac    = 0;
	  p_ptr->misc.dis_td = p_ptr->misc.ptodam; /* Displayed values	*/
	  p_ptr->misc.dis_th = p_ptr->misc.ptohit;
	  p_ptr->misc.dis_tac= p_ptr->misc.ptoac;
	  p_ptr->misc.dis_ac = p_ptr->misc.pac;

	  /* now set misc stats, do this after setting stats because
	     of con_adj() for hitpoints */
	  m_ptr = &py.misc;
	  m_ptr->hitdie += class[m_ptr->pclass].adj_hd;
	  m_ptr->mhp = con_adj() + m_ptr->hitdie;
	  m_ptr->chp = (double)m_ptr->mhp;
	  m_ptr->bth += class[m_ptr->pclass].mbth;
	  m_ptr->bthb += class[m_ptr->pclass].mbthb;	/*RAK*/
	  m_ptr->srh += class[m_ptr->pclass].msrh;
	  m_ptr->disarm += class[m_ptr->pclass].mdis;
	  m_ptr->fos += class[m_ptr->pclass].mfos;
	  m_ptr->stl += class[m_ptr->pclass].mstl;
	  m_ptr->save += class[m_ptr->pclass].msav;
	  (void) strcat(m_ptr->title, player_title[m_ptr->pclass][0]);
	  m_ptr->expfact += class[m_ptr->pclass].m_exp;

	}
    }
  while (!exit_flag);
  return(res);
}


get_money()
{
  register int tmp;
  register struct stats *p_ptr;
  register struct misc *m_ptr;

  p_ptr = &py.stats;
  tmp = p_ptr->cstr + p_ptr->cint + p_ptr->cwis +
    p_ptr->cdex + p_ptr->ccon + p_ptr->cchr;
  m_ptr = &py.misc;
  m_ptr->au = m_ptr->sc*6 + randint(25) + 325;	/* Social Class adj	*/
  m_ptr->au = m_ptr->au - tmp;			/* Stat adj		*/
  m_ptr->au = m_ptr->au + p_ptr->cchr;		/* Charisma adj	*/
  if (m_ptr->au < 80)  m_ptr->au = 80;		/* Minimum		*/
}


/* ---------- M A I N  for Character Creation Routine ---------- */
/*							-JWT-	*/
create_character()
{
  char s;
  register int exit_flag = 1;

  put_character();
  (void) choose_race();
  (void) get_sex();

  /* here we start a loop giving a player a choice of characters -RGM- */
  get_stats ();
  get_history();
  get_ahw();
  put_character();
  print_history();
  put_misc1();
  put_stats();
  exit_flag = 1;
  do
    {
      prt("Hit space to reroll or ESC to accept characteristics: ", 20, 2);
      inkey(&s);
      switch (s)
	{
	case 27:
	  exit_flag = 0;
	  break;
	case ' ':
	  get_stats ();
	  get_history();
	  get_ahw();
	  put_character();
	  print_history();
	  put_misc1();
	  put_stats();
#ifdef SLOW
	  (void) sleep (0);
#endif
	  break;
	default:
	  break;
	}
    }               /* done with stats generation */
  while (exit_flag == 1);

  (void) get_class();
  get_money();
  put_stats();
  put_misc2();
  put_misc3();
  get_name();

  /* This delay may be reduced, but is recommended to keep players	*/
  /* from continuously rolling up characters, which can be VERY	*/
  /* expensive CPU wise.						*/
  pause_exit(23, PLAYER_EXIT_PAUSE);
}

