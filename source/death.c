/* death.c: code executed when player dies

   Copyright (c) 1989 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

/* some incorrectly define NULL as integer constant, so load this before
   local includes */
#include <stdio.h>

#include "constant.h"
#include "config.h"
#include "types.h"
#include "externs.h"

#ifdef Pyramid
#include <sys/time.h>
#else
#include <time.h>
#endif

#include <ctype.h>

#ifndef USG
/* only needed for Berkeley UNIX */
#include <sys/param.h>
#include <sys/types.h>
#include <sys/file.h>
#endif

#ifdef MSDOS
#include <io.h>
#else
#if !defined(ATARIST_MWC) && !defined(MAC)
#ifndef VMS
#include <pwd.h>
#else
#include <file.h>
#endif
#endif
#endif

#ifdef USG
#ifndef ATARIST_MWC
#include <string.h>
#include <fcntl.h>
#endif
#else
#include <strings.h>
#endif

#ifndef MIN
#define MIN(a, b)	((a < b) ? a : b)
#endif

#ifndef BSD4_3
long lseek();
#else
off_t lseek();
#endif

#if defined(USG) || defined(VMS)
#ifndef L_SET
#define L_SET 0
#endif
#endif

#ifndef VMS
#ifndef MAC
#if defined(ultrix) || defined(USG)
void perror();
void exit ();
#endif
#endif
#endif

#ifndef MAC
#ifdef SYS_V
struct passwd *getpwuid();
#endif
#endif

#if defined(LINT_ARGS)
static void date(char *);
static char *center_string(char *, char *);
static void print_tomb(void);
static void kingly(void);
#endif

#ifndef MAC
char *getlogin();
#ifndef ATARIST_MWC
long time();
#endif
#endif

static void date(day)
char *day;
{
  register char *tmp;
#ifdef MAC
  time_t clock;
#else
  long clock;
#endif

#ifdef MAC
  clock = time((time_t *) 0);
#else
  clock = time((long *) 0);
#endif
  tmp = ctime(&clock);
  tmp[10] = '\0';
  (void) strcpy(day, tmp);
}

/* Centers a string within a 31 character string		-JWT-	 */
static char *center_string(centered_str, in_str)
char *centered_str;
char *in_str;
{
  register int i, j;

  i = strlen(in_str);
  j = 15 - i/2;
  (void) sprintf (centered_str, "%*s%s%*s", j, "", in_str, 31 - i - j, "");
  return centered_str;
}


#if 0
/* Not touched for Mac port */
static void display_scores()
{
  register int i = 0, j;
  int fd;
  high_scores score;
  char list[20][128];
  char string[100];

#ifndef ATARIST_MWC
  if (1 > (fd = open(MORIA_TOP, O_RDONLY, 0644)))
#else
  if (1 > (fd = open(MORIA_TOP, 2)))
#endif
    {
      (void) sprintf (string, "Error opening score file \"%s\"\n", MORIA_TOP);
      prt(string, 0, 0);
      return ;
    }
#ifdef MSDOS
  (void) setmode(fd, O_BINARY);
#endif

  while (0 < read(fd, (char *)&score, sizeof(high_scores)))
    {
      (void) sprintf(list[i], "%-7ld%-15.15s%-10.10s%-10.10s%-5d%-25.25s%5d",
		    score.points, score.name,
		    race[score.prace].trace, class[score.pclass].title,
		    (int)score.lev, score.died_from, score.dun_level);
      i++;
    }

  controlz();
  put_buffer("Points Name           Race      Class     Lv   Killed By                Dun Lv", 0, 0);
  for (j = 0; j < i; j++)
    put_buffer(list[j], j + 1, 0);
  pause_line(23);
}
#endif

/* Prints the gravestone of the character		-RAK-	 */
static void print_tomb()
{
  vtype str, tmp_str;
  register int i;
  char day[11];
  register char *p;
#ifdef MAC
  char func;
  int ok;
#endif

  clear_screen();
  put_buffer ("_______________________", 1, 15);
  put_buffer ("/", 2, 14);
  put_buffer ("\\         ___", 2, 38);
  put_buffer ("/", 3, 13);
  put_buffer ("\\ ___   /   \\      ___", 3, 39);
  put_buffer ("/            RIP            \\   \\  :   :     /   \\", 4, 12);
  put_buffer ("/", 5, 11);
  put_buffer ("\\  : _;,,,;_    :   :", 5, 41);
  (void) sprintf (str, "/%s\\,;_          _;,,,;_",
		  center_string (tmp_str, py.misc.name));
  put_buffer (str, 6, 10);
  put_buffer ("|               the               |   ___", 7, 9);
  if (!total_winner)
    p = title_string ();
  else
    p = "Magnificent";
  (void) sprintf (str, "| %s |  /   \\", center_string (tmp_str, p));
  put_buffer (str, 8, 9);
  put_buffer ("|", 9, 9);
  put_buffer ("|  :   :", 9, 43);
  if (!total_winner)
    p = class[py.misc.pclass].title;
  else if (py.misc.male)
    p = "*King*";
  else
    p = "*Queen*";
  (void) sprintf(str,"| %s | _;,,,;_   ____", center_string (tmp_str, p));
  put_buffer (str, 10, 9);
  (void) sprintf (str, "Level : %d", (int) py.misc.lev);
  (void) sprintf (str,"| %s |          /    \\", center_string (tmp_str, str));
  put_buffer (str, 11, 9);
  (void) sprintf(str, "%ld Exp", py.misc.exp);
  (void) sprintf(str,"| %s |          :    :", center_string (tmp_str, str));
  put_buffer (str, 12, 9);
  (void) sprintf(str, "%ld Au", py.misc.au);
  (void) sprintf(str,"| %s |          :    :", center_string (tmp_str, str));
  put_buffer (str, 13, 9);
  (void) sprintf(str, "Died on Level : %d", dun_level);
  (void) sprintf(str,"| %s |         _;,,,,;_", center_string (tmp_str, str));
  put_buffer (str, 14, 9);
  put_buffer ("|            killed by            |", 15, 9);
  p = died_from;
  i = strlen (p);
  p[i] = '.';  /* add a trailing period */
  p[i+1] = '\0';
  (void) sprintf(str, "| %s |", center_string (tmp_str, p));
  put_buffer (str, 16, 9);
  p[i] = '\0';	 /* strip off the period */
  date(day);
  (void) sprintf(str, "| %s |", center_string (tmp_str, day));
  put_buffer (str, 17, 9);
  put_buffer ("*|   *     *     *    *   *     *  | *", 18, 8);
  put_buffer ("________)/\\\\_)_/___(\\/___(//_\\)/_\\//__\\\\(/_|_)_______",
	      19, 0);

 retry:
  flush();
#ifdef MAC
  /* On Mac, file_character() gets file name via std file dialog */
  /* So, the prompt for character record cannot be made to do double duty */
  put_buffer ("('F' - Save record in file / 'Y' - Display record on screen / 'N' - Abort)", 23, 0);
  put_buffer ("Character record [F/Y/N]?", 22, 0);
  do
    {
      func = inkey();
      switch (func)
	{
	case 'f': case 'F':
	  func = 'F';
	  ok = TRUE;
	  break;
	case 'y': case 'Y':
	  func = 'Y';
	  ok = true;
	  break;
	case 'n': case 'N':
	  func = 'N';
	  ok = TRUE;
	  break;
	default:
	  bell();
	  ok = FALSE;
	  break;
	}
    }
  while (!ok);
  if (func != 'N')
#else
  put_buffer ("(ESC to abort, return to print on screen, or file name)", 23, 0);
  put_buffer ("Character record?", 22, 0);
  if (get_string (str, 22, 18, 60))
#endif
    {
      for (i = 0; i < INVEN_ARRAY_SIZE; i++)
	{
	  known1(&inventory[i]);
	  known2(&inventory[i]);
	}
      calc_bonuses ();
#ifdef MAC
      if (func == 'F')
	{
	  if (!file_character())
	    goto retry;
	}
#else
      if (str[0])
	{
	  if (!file_character (str))
	    goto retry;
	}
#endif
      else
	{
	  clear_screen ();
	  display_char ();
	  put_buffer ("Type ESC to skip the inventory:", 23, 0);
	  if (inkey() != ESCAPE)
	    {
	      clear_screen ();
	      msg_print ("You are using:");
	      (void) show_equip (TRUE, 0);
	      msg_print ("You are carrying:");
	      clear_from (1);
	      (void) show_inven (0, inven_ctr-1, TRUE, 0);
	      msg_print (NULL);
	    }
	}
    }
}


/* Calculates the total number of points earned		-JWT-	 */
#if 0
/* Not touched for Mac port */
long total_points()
{
  return (py.misc.max_exp + (100 * py.misc.max_lev));
}
#endif


/* Enters a players name on the top twenty list		-JWT-	 */
#if 0
/* Not touched for mac port */
top_twenty()
{
  register int i, j, k;
  high_scores scores[20], myscore;
#ifdef MSDOS
  char string[100];
#endif
  char *tmp;

  clear_screen(0, 0);

  if (wizard)
    exit_game();

  if (panic_save == 1)
    {
      msg_print("Sorry, scores for games restored from panic save files are not saved.");
      display_scores ();
      exit_game();
    }

  myscore.points = total_points();
  myscore.dun_level = dun_level;
  myscore.lev = py.misc.lev;
  myscore.max_lev = py.misc.max_lev;
  myscore.mhp = py.misc.mhp;
  myscore.chp = py.misc.chp;
  myscore.uid = -1;
  /* First character of sex, lower case */
  myscore.sex = tolower(py.misc.sex[0]);
  myscore.prace = py.misc.prace;
  myscore.pclass = py.misc.pclass;
  (void) strcpy(myscore.name, py.misc.name);
  tmp = died_from;
  if ('a' == *tmp)
    {
      if ('n' == *(++tmp))
	{
	  tmp++;
	}
      while (isspace(*tmp))
	{
	  tmp++;
	}
    }
  (void) strncpy(myscore.died_from, tmp, strlen(tmp) - 1);
  myscore.died_from[strlen(tmp) - 1] = '\0';
  /* Get rid of '.' at end of death description */

  /*  First, get a lock on the high score file so no-one else tries */
  /*  to write to it while we are using it */
#ifdef USG
  /* no flock sytem call, ignore the problem for now */
#else
#ifndef VMS
  if (0 != flock(highscore_fd, LOCK_EX))
    {
      perror("Error gaining lock for score file");
      exit_game();
    }
#else
  if (1 > (highscore_fd = open(MORIA_TOP, O_RDWR | O_CREAT, 0644)))
    {
      (void) sprintf (string, "Error opening score file \"%s\"\n", MORIA_TOP);
      prt(string, 0, 0);
      return;
    }
#endif
#endif

#ifdef MSDOS
  /* open the scorefile here */
  if (1 > (highscore_fd = open(MORIA_TOP, O_RDWR | O_CREAT, 0644)))
    {
      (void) sprintf (string, "Error opening score file \"%s\"\n", MORIA_TOP);
      prt(string, 0, 0);
      return;
    }
  (void) setmode(highscore_fd, O_BINARY);
#endif

  /*  Check to see if this score is a high one and where it goes */
  i = 0;
#ifndef BSD4_3
  (void) lseek(highscore_fd, (long)0, L_SET);
#else
  (void) lseek(highscore_fd, (off_t)0, L_SET);
#endif
  while ((i < 20)
	&& (0 != read(highscore_fd, (char *)&scores[i], sizeof(high_scores))))
    {
      i++;
    }

  j = 0;
  while (j < i && (scores[j].points >= myscore.points))
    {
      j++;
    }
  /* i is now how many scores we have, and j is where we put this score */

  /* If its the first score, or it gets appended to the file */
  if (0 == i || (i == j && j < 20))
    {
#ifndef BSD4_3
      (void) lseek(highscore_fd, (long)(j * sizeof(high_scores)), L_SET);
#else
      (void) lseek(highscore_fd, (off_t)(j * sizeof(high_scores)), L_SET);
#endif
      (void) write(highscore_fd, (char *)&myscore, sizeof(high_scores));
    }
  else if (j < i)
    {
      /* If it gets inserted in the middle */
      /* Bump all the scores up one place */
      for (k = MIN(i, 19); k > j ; k--)
	{
#ifndef BSD4_3
	  (void) lseek(highscore_fd, (long)(k * sizeof(high_scores)), L_SET);
#else
	  (void) lseek(highscore_fd, (off_t)(k * sizeof(high_scores)), L_SET);
#endif
	  (void) write(highscore_fd, (char *)&scores[k - 1], sizeof(high_scores));
	}
      /* Write out your score */
#ifndef BSD4_3
      (void) lseek(highscore_fd, (long)(j * sizeof(high_scores)), L_SET);
#else
      (void) lseek(highscore_fd, (off_t)(j * sizeof(high_scores)), L_SET);
#endif
      (void) write(highscore_fd, (char *)&myscore, sizeof(high_scores));
    }

#ifdef USG
  /* no flock sytem call, ignore the problem for now */
#else
#ifndef VMS
  (void) flock(highscore_fd, LOCK_UN);
#endif
#endif
  (void) close(highscore_fd);
  display_scores();
}
#endif


/* Change the player into a King!			-RAK-	 */
static void kingly()
{
  register struct misc *p_ptr;
  register char *p;

  /* Change the character attributes.		 */
  dun_level = 0;
  (void) strcpy(died_from, "Ripe Old Age");
  p_ptr = &py.misc;
  (void) restore_level ();
  p_ptr->lev += MAX_PLAYER_LEVEL;
  p_ptr->au += 250000L;
  p_ptr->max_exp += 5000000L;
  p_ptr->exp = p_ptr->max_exp;

  /* Let the player know that he did good.	 */
  clear_screen();
  put_buffer("#", 1, 34);
  put_buffer("#####", 2, 32);
  put_buffer("#", 3, 34);
  put_buffer(",,,  $$$  ,,,", 4, 28);
  put_buffer(",,=$   \"$$$$$\"   $=,,", 5, 24);
  put_buffer(",$$        $$$        $$,", 6, 22);
  put_buffer("*>         <*>         <*", 7, 22);
  put_buffer("$$         $$$         $$", 8, 22);
  put_buffer("\"$$        $$$        $$\"", 9, 22);
  put_buffer("\"$$       $$$       $$\"", 10, 23);
  p = "*#########*#########*";
  put_buffer(p, 11, 24);
  put_buffer(p, 12, 24);
  put_buffer("Veni, Vidi, Vici!", 15, 26);
  put_buffer("I came, I saw, I conquered!", 16, 21);
  if (p_ptr->male)
    put_buffer("All Hail the Mighty King!", 17, 22);
  else
    put_buffer("All Hail the Mighty Queen!", 17, 22);
  flush();
  pause_line(23);
}


/* Handles the gravestone end top-twenty routines	-RAK-	 */
void exit_game ()
{
  register int i;

#ifdef MAC
  /* Prevent strange things from happening */
  enablefilemenu(FALSE);
#endif

  /* What happens upon dying.				-RAK-	 */
  msg_print(NULL);
  flush ();  /* flush all input */
  nosignals ();	 /* Can't interrupt or suspend. */
  /* If the game has been saved, then save sets turn back to -1, which
     inhibits the printing of the tomb.	 Save also finishes up the log. */
  /* Don't log anyone who has not even started. */
  if (turn >= 0)
    {
      if (total_winner)
	kingly();
      print_tomb();
    }
  i = log_index;
#ifdef MAC
  (void) save_char (TRUE);	/* Save the memory at least. */
#else
  (void) save_char ();		/* Save the memory at least. */
#endif
  if (i > 0)
    display_scores (i, TRUE);
  erase_line (23, 0);
  restore_term ();
#ifdef MAC
  /* Undo what has been done */
  enablefilemenu(TRUE);
  /* Long jump back into the Mac wrapper, in lieu of exit () */
  goback();
#else
  exit (0);
#endif
}
