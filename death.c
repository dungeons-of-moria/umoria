#include <stdio.h>
#include <pwd.h>
#include <time.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/file.h>

#include "constants.h"
#include "config.h"
#include "types.h"
#include "externs.h"

#ifdef USG
#include <string.h>
#include <fcntl.h>
#else
#include <strings.h>
#endif

#define MIN(a, b)	((a < b) ? a : b)

#ifdef sun   /* correct SUN stupidity in the stdio.h file */
char *sprintf();
#endif

#if defined(ultrix) || defined(sun) || defined(USG)
int getuid();
#else
uid_t getuid();
#endif

#if defined(sun) || defined(USG) || defined(ultrix)
long lseek();
#else
off_t lseek();
#endif
#ifdef USG
#define L_SET 0
#endif

#if defined(ultrix) || defined(USG)
void perror();
void exit();
#endif

#ifdef USG
struct passwd *getpwuid();
#endif

char *getlogin();
long time();

date(day)
char *day;
{
  register char *tmp;
  long clock;

  clock = time((long *) 0);
  tmp = ctime(&clock);
  tmp[11] = '\0';
  (void) strcpy(day, tmp);
}

/* Centers a string within a 31 character string		-JWT-	 */
char *fill_str(p1)
char *p1;
{
  vtype s1, s2;
  int i;

  s1[0] = '\0';
  s2[0] = '\0';
  i = (strlen(p1) / 2);
  (void) strcpy(s1, pad(s2, " ", 15 - i));
  (void) strcat(s1, pad(p1, " ", 31));
  s1[31] = '\0';
  return (s1);
}


/* Prints a line to the screen efficiently		-RAK-	 */
dprint(str, row)
char *str;
int row;
{
  register int i, j, nblanks, xpos;
  vtype prt_str;
  char tmp_str[2];

  tmp_str[1] = '\0';	/* yes, this is supposed to be a one */
  prt_str[0] = '\0';
  nblanks = 0;
  xpos = 0;
  for (i = 0; i < strlen(str); i++)
    {
      if (str[i] == ' ')
	{
	  if (xpos >= 0)
	    {
	      nblanks++;
	      if (nblanks > 5)
		{
		  nblanks = 0;
		  put_buffer(prt_str, row, xpos);
		  prt_str[0] = '\0';
		  xpos = -1;
		}
	    }
	}
      else
	{
	  if (xpos == -1)
	    xpos = i;
	  if (nblanks > 0)
	    {
	      for (j = 0; j < nblanks; j++)
		(void) strcat(prt_str, " ");
	      nblanks = 0;
	    }
	  tmp_str[0] = str[i];
	  (void) strcat(prt_str, tmp_str);
	}
    }
  if (xpos >= 0)
    put_buffer(prt_str, row, xpos);
}

display_scores()
{
  register int i = 0, j;
  int fd;
  high_scores score;
  char list[20][256];

  if (1 > (fd = open(MORIA_TOP, O_RDONLY, 0644)))
    {
      prt("Error opening top twenty file\n", 0, 0);
      return ;
    }

  while (0 < read(fd, (char *)&score, sizeof(high_scores)))
    {
      (void) sprintf(list[i], "%-7d%-15.15s%-10.10s%-10.10s%-5d%-25.25s%5d",
		    (int)score.points, score.name,
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

/* Prints the gravestone of the character		-RAK-	 */
print_tomb()
{
  vtype str1, str2, str3, str4, str5, str6, str7, str8;
  vtype dstr[20];
  vtype fnam;
  char command;
  FILE *f1;
  register int i;
  char day[11];
  int flag;
  char tmp_str[80];

  date(day);
  (void) strcpy(str1, fill_str(py.misc.name));
  (void) strcpy(str2, fill_str(py.misc.title));
  (void) strcpy(str3, fill_str(py.misc.tclass));
  (void) sprintf(str4, "Level : %d", (int)py.misc.lev);
  (void) strcpy(str4, fill_str(str4));
  (void) sprintf(str5, "%d Exp", py.misc.exp);
  (void) strcpy(str5, fill_str(str5));
  (void) sprintf(str6, "%d Au", py.misc.au);
  (void) strcpy(str6, fill_str(str6));
  (void) sprintf(str7, "Died on Level : %d", dun_level);
  (void) strcpy(str7, fill_str(str7));
  (void) strcpy(str8, fill_str(died_from));
  dstr[0][0] = '\0';
  (void) strcpy(dstr[1], "               _______________________");
  (void) strcpy(dstr[2], "              /                       \\         ___");
  (void) strcpy(dstr[3],
	"             /                         \\ ___   /   \\      ___");
  (void) strcpy(dstr[4],
	"            /            RIP            \\   \\  :   :     /   \\");
  (void) strcpy(dstr[5],
	"           /                             \\  : _;,,,;_    :   :");
  (void) sprintf(dstr[6], "          /%s\\,;_          _;,,,;_", str1);
  (void) strcpy(dstr[7], "         |               the               |   ___");
  (void) sprintf(dstr[8], "         | %s |  /   \\", str2);
  (void) strcpy(dstr[9], "         |                                 |  :   :");
  (void) sprintf(dstr[10], "         | %s | _;,,,;_   ____", str3);
  (void) sprintf(dstr[11], "         | %s |          /    \\", str4);
  (void) sprintf(dstr[12], "         | %s |          :    :", str5);
  (void) sprintf(dstr[13], "         | %s |          :    :", str6);
  (void) sprintf(dstr[14], "         | %s |         _;,,,,;_", str7);
  (void) strcpy(dstr[15], "         |            killed by            |");
  (void) sprintf(dstr[16], "         | %s |", str8);
  (void) sprintf(dstr[17], "         |           %s           |", day);
  (void) strcpy(dstr[18], "        *|   *     *     *    *   *     *  | *");
  (void) strcpy(dstr[19],
	      "________)/\\\\_)_/___(\\/___(//_\\)/_\\//__\\\\(/_|_)_______");
  clear_screen(0, 0);
  for (i = 0; i <= 19; i++)
    dprint(dstr[i], i);
  flush();
  if (get_com("Print to file? (Y/N)", &command))
    switch (command)
      {
      case 'y':
      case 'Y':
	prt("Enter Filename:", 0, 0);
	flag = FALSE;
	do
	  {
	    if (get_string(fnam, 0, 16, 60))
	      {
		if (strlen(fnam) == 0)
		  (void) strcpy(fnam, "MORIACHR.DIE");
		f1 = fopen(fnam, "w");
		if (f1 == NULL)
		  {
		    (void) sprintf(tmp_str, "Error creating> %s", fnam);
		    prt(tmp_str, 1, 0);
		  }
		else
		  {
		    flag = TRUE;
		    for (i = 0; i <= 19; i++)
		      (void) fprintf(f1, "%s\n", dstr[i]);
		  }
		(void) fclose(f1);
	      }
	    else
	      flag = TRUE;
	  }
	while (!flag);
	break;
      default:
	break;
      }
}


/* Calculates the total number of points earned		-JWT-	 */
int total_points()
{
  return (py.misc.max_exp + (100 * py.misc.max_lev));
}


/* Enters a players name on the top twenty list		-JWT-	 */
top_twenty()
{
  register int i, j, k;
  high_scores scores[20], myscore;
  char *tmp;

  clear_screen(0, 0);

  if (wizard1)
    exit_game();

  if (panic_save == 1)
    {
      msg_print("Sorry, scores for games restored from panic save files are not saved.");
      /* make sure player sees message before display_scores erases it */
      msg_print (" ");
      display_scores ();
      exit_game();
    }

  myscore.points = (long)total_points();
  myscore.dun_level = dun_level;
  myscore.lev = py.misc.lev;
  myscore.max_lev = py.misc.max_lev;
  myscore.mhp = py.misc.mhp;
  myscore.chp = py.misc.chp;
  myscore.uid = getuid();
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
  if (0 != flock(highscore_fd, LOCK_EX))
    {
      perror("Error gaining lock for score file");
      exit(1);
    }
#endif

  /*  Check to see if this score is a high one and where it goes */
  i = 0;
#if defined(sun) || defined(ultrix) || defined(USG)
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
#if defined(sun) || defined(ultrix) || defined(USG)
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
#if defined(sun) || defined(ultrix) || defined(USG)
	  (void) lseek(highscore_fd, (long)(k * sizeof(high_scores)), L_SET);
#else
	  (void) lseek(highscore_fd, (off_t)(k * sizeof(high_scores)), L_SET);
#endif
	  (void) write(highscore_fd, (char *)&scores[k - 1], sizeof(high_scores));
	}
      /* Write out your score */
#if defined(sun) || defined(ultrix) || defined(USG)
      (void) lseek(highscore_fd, (long)(j * sizeof(high_scores)), L_SET);
#else
      (void) lseek(highscore_fd, (off_t)(j * sizeof(high_scores)), L_SET);
#endif
      (void) write(highscore_fd, (char *)&myscore, sizeof(high_scores));
    }

#ifdef USG
  /* no flock sytem call, ignore the problem for now */
#else
  (void) flock(highscore_fd, LOCK_UN);
#endif
  (void) close(highscore_fd);
  display_scores();
}


/* Change the player into a King!			-RAK-	 */
kingly()
{
  register struct misc *p_ptr;

  /* Change the character attributes...		 */
  dun_level = 0;
  /* need dot on the end to be consistent with creature.c */
  (void) strcpy(died_from, "Ripe Old Age.");
  p_ptr = &py.misc;
  p_ptr->lev += MAX_PLAYER_LEVEL;
  if (p_ptr->sex[0] == 'M')
    {
      (void) strcpy(p_ptr->title, "Magnificent");
      (void) strcpy(p_ptr->tclass, "*King*");
    }
  else
    {
      (void) strcpy(p_ptr->title, "Beautiful");
      (void) strcpy(p_ptr->tclass, "*Queen*");
    }
  p_ptr->au += 250000;
  p_ptr->max_exp += 5000000;
  p_ptr->exp = p_ptr->max_exp;

  /* Let the player know that he did good...	 */
  clear_screen(0, 0);
  dprint("                                  #", 1);
  dprint("                                #####", 2);
  dprint("                                  #", 3);
  dprint("                            ,,,  $$$  ,,,", 4);
  dprint("                        ,,==$   \"$$$$$\"   $==,,", 5);
  dprint("                      ,$$        $$$        $$,", 6);
  dprint("                      *>         <*>         <*", 7);
  dprint("                      $$         $$$         $$", 8);
  dprint("                      \"$$        $$$        $$\"", 9);
  dprint("                       \"$$       $$$       $$\"", 10);
  dprint("                        *#########*#########*", 11);
  dprint("                        *#########*#########*", 12);
  dprint("                          Veni, Vidi, Vici!", 15);
  dprint("                     I came, I saw, I conquered!", 16);
  if (p_ptr->sex[0] == 'M')
    dprint("                      All Hail the Mighty King!", 17);
  else
    dprint("                      All Hail the Mighty Queen!", 17);
  flush();
  pause_line(23);
}


/* Handles the gravestone end top-twenty routines	-RAK-	 */
upon_death()
{
  /* What happens upon dying...				-RAK-	 */
  if (total_winner)
    kingly();
  print_tomb();
  top_twenty();
  exit_game();
}
