#include <stdio.h>

#include "constants.h"
#include "config.h"
#include "types.h"
#include "externs.h"

#ifdef USG
#include <string.h>
#include <fcntl.h>
#else
#include <strings.h>
#include <sys/file.h>
#endif

#ifdef sun   /* correct SUN stupidity in the stdio.h file */
char *sprintf();
#endif

#if defined(ultrix) || defined(USG)
void exit();
#endif

/*
 *  init_scorefile
 *  Open the score file while we still have the setuid privileges.  Later
 *  when the score is being written out, you must be sure to flock the file
 *  so we don't have multiple people trying to write to it at the same time.
 *  Craig Norborg (doc)		Mon Aug 10 16:41:59 EST 1987
 */
init_scorefile()
{
  if (1 > (highscore_fd = open(MORIA_TOP, O_RDWR | O_CREAT, 0644)))
    {
      (void) fputs("Can't open score file!\n", stderr);
      exit(1);
    }
}

/* Attempt to open the intro file			-RAK-	 */
/* This routine also checks the hours file vs. what time it is	-Doc */
intro(finam)
char *finam;
{
  register int xpos, i;
  vtype in_line;
  FILE *file1;
  register char *string;

  /* Attempt to read hours.dat.  If it does not exist,     */
  /* inform the user so he can tell the wizard about it	 */
  if ((file1 = fopen(MORIA_HOU, "r")) != NULL)
    {
      while (fgets(in_line, 80, file1) != NULL)
	if (strlen(in_line) > 3)
	  {
	    if (!strncmp(in_line, "SUN:", 4))
	      (void) strcpy(days[0], in_line);
	    else if (!strncmp(in_line, "MON:", 4))
	      (void) strcpy(days[1], in_line);
	    else if (!strncmp(in_line, "TUE:", 4))
	      (void) strcpy(days[2], in_line);
	    else if (!strncmp(in_line, "WED:", 4))
	      (void) strcpy(days[3], in_line);
	    else if (!strncmp(in_line, "THU:", 4))
	      (void) strcpy(days[4], in_line);
	    else if (!strncmp(in_line, "FRI:", 4))
	      (void) strcpy(days[5], in_line);
	    else if (!strncmp(in_line, "SAT:", 4))
	      (void) strcpy(days[6], in_line);
	  }
      (void) fclose(file1);
    }
  else
    {
      (void) fprintf(stderr, "There is no hours file.\nPlease inform the wizard, %s, so he can correct this!\n", WIZARD);
      exit_game();
    }

  /* Check the hours, if closed  require password	 */
  string = index(finam, '^');
  if (string)
    xpos = strlen(finam) - strlen(string);
  else
    xpos = -1;
  if (xpos >= 0)
    if (check_pswd())
      insert_str(finam, "^", "");
  if (!check_time())
    {
      if (!wizard1)
	{
	  if ((file1 = fopen(MORIA_HOU, "r")) != NULL)
	    {
	      clear_screen(0, 0);
	      for (i = 0; fgets(in_line, 80, file1) != NULL; i++)
		prt(in_line, i, 0);
	      (void) fclose(file1);
	    }
	  exit_game();
	}
    }

  /* Print the introduction message, news, ect...		 */
  if ((file1 = fopen(MORIA_MOR, "r")) != NULL)
    {
      clear_screen(0, 0);
      for (i = 0; fgets(in_line, 80, file1) != NULL; i++)
	prt(in_line, i, 0);
      pause_line(23);
      (void) fclose(file1);
    }
}


/* Prints dungeon map to external file			-RAK-	 */
print_map()
{
  register int i, j, m, n;
  register k, l;
  register i7, i8;
  char dun_line[MAX_WIDTH];
  char *dun_ptr;
  vtype filename1;
  char tmp_str[80];
  FILE *file1;
  int page_width = OUTPAGE_WIDTH;
  int page_height = OUTPAGE_HEIGHT;

  /* this allows us to strcat each character in the inner loop,
     instead of using the expensive sprintf */
  prt("File name: ", 0, 0);
  if (get_string(filename1, 0, 11, 64))
    {
      if (strlen(filename1) == 0)
	(void) strcpy(filename1, "MORIAMAP.DAT");
      if ((file1 = fopen(filename1, "w")) == NULL)
	{
	  (void) sprintf(dun_line, "Cannot open file %s", filename1);
	  prt(dun_line, 0, 0);
	  put_qio();
	  return;
	}
      (void) sprintf(tmp_str, "section width (default = %d char):", page_width);
      prt(tmp_str, 0, 0);
      (void) get_string(tmp_str, 0, strlen(tmp_str), 10);
      (void) sscanf(tmp_str, "%d", &page_width);
      if (page_width < 10)
	page_width = 10;

      (void) sprintf(tmp_str, "section height (default = %d lines):", page_height);
      prt(tmp_str, 0, 0);
      (void) get_string(tmp_str, 0, strlen(tmp_str), 10);
      (void) sscanf(tmp_str, "%d", &page_height);
      if (page_height < 10)
	page_height = 10;

      prt("Writing Moria Dungeon Map...", 0, 0);
      put_qio();

      i = 0;
      i7 = 0;
      do
	{
	  j = 0;
	  k = i + page_height - 1;
	  if (k >= cur_height)
	    k = cur_height - 1;
	  i7++;
	  i8 = 0;
	  do
	    {
	      l = j + page_width - 1;
	      if (l >= cur_width)
		l = cur_width - 1;
	      i8++;
	      (void) fprintf(file1, "%c\n", 12);
	      (void) fprintf(file1, "Section[%d,%d];     ", i7, i8);
	      (void) fprintf(file1, "Depth : %d (feet)\n\n   ",
			     (dun_level * 50));
	      for (m = j; m <= l; m++)
		{
		  n = (m / 100);
		  (void) fprintf(file1, "%d", n);
		}
	      (void) fputs("\n   ", file1);
	      for (m = j; m <= l; m++)
		{
		  n = (m / 10) - (m / 100) * 10;
		  (void) fprintf(file1, "%d", n);
		}
	      (void) fputs("\n   ", file1);
	      for (m = j; m <= l; m++)
		{
		  n = m - (m / 10) * 10;
		  (void) fprintf(file1, "%d", n);
		}
	      (void) fprintf(file1, "\n");
	      for (m = i; m <= k; m++)
		{
		  (void) sprintf(dun_line, "%2d ", m);
		  dun_ptr = &dun_line[3];
		  for (n = j; n <= l; n++)
		    {
		      if (test_light(m, n))
			loc_symbol(m, n, dun_ptr++);
		      else
			*dun_ptr++ = ' ';
		    }
		  *dun_ptr++ = '\n';
		  (void) fputs(dun_line, file1);
		}
	      j += page_width;
	    }
	  while (j < cur_width);
	  i += page_height;
	}
      while (i < cur_height);
      (void) fclose(file1);
      prt("Completed.", 0, 0);
    }
}


/* Prints a list of random objects to a file.  Note that -RAK-	 */
/* the objects produced is a sampling of objects which           */
/* be expected to appear on that level.                          */
print_objects()
{
  register int i;
  int nobj, j, level;
  vtype filename1, tmp_str;
  register FILE *file1;
  register treasure_type *i_ptr;

  prt("Produce objects on what level?: ", 0, 0);
  level = 0;
  if (get_string(tmp_str, 0, 32, 10))
    (void) sscanf(tmp_str, "%d", &level);
  prt("Produce how many objects?: ", 0, 0);
  nobj = 0;
  if (get_string(tmp_str, 0, 27, 10))
    (void) sscanf(tmp_str, "%d", &nobj);
  if ((nobj > 0) && (level > -1) && (level < 1201))
    {
      if (nobj > 9999)
	nobj = 9999;
      prt("File name: ", 0, 0);
      if (get_string(filename1, 0, 11, 64))
	{
	  if (strlen(filename1) == 0)
	    (void) strcpy(filename1, "MORIAOBJ.DAT");
	  if ((file1 = fopen(filename1, "w")) != NULL)
	    {
	      (void) sprintf(tmp_str, "%d", nobj);
	      prt(strcat(tmp_str, " random objects being produced..."), 0, 0);
	      put_qio();
	      (void) fprintf(file1, "*** Random Object Sampling:\n");
	      (void) fprintf(file1, "*** %d objects\n", nobj);
	      (void) fprintf(file1, "*** For Level %d\n", level);
	      (void) fprintf(file1, "\n");
	      (void) fprintf(file1, "\n");
	      popt(&j);
	      for (i = 0; i < nobj; i++)
		{
		  t_list[j] = object_list[get_obj_num(level)];
		  magic_treasure(j, level);
		  inventory[INVEN_MAX] = t_list[j];
		  i_ptr = &inventory[INVEN_MAX];
		  unquote(i_ptr->name);
		  known1(i_ptr->name);
		  known2(i_ptr->name);
		  objdes(tmp_str, INVEN_MAX, TRUE);
		  (void) fprintf(file1, "%s\n", tmp_str);
		}
	      pusht(j);
	      (void) fclose(file1);
	      prt("Completed.", 0, 0);
	    }
	  else
	    prt("File could not be opened.", 0, 0);
	}
    }
}


/* Prints a listing of monsters				-RAK-	 */
print_monsters()
{
  register int i;
  int j, xpos, attype, adesc;
  register FILE *file1;
  vtype out_val, filename1;
  vtype attstr, attx;
  dtype damstr;
  register creature_type *c_ptr;
  register char *string;

  prt("File name: ", 0, 0);
  if (get_string(filename1, 0, 11, 64))
    {
      if (strlen(filename1) == 0)
	(void) strcpy(filename1, "MORIAMON.DAT");
      if ((file1 = fopen(filename1, "w")) != NULL)
	{
	  prt("Writing Monster Dictionary...", 0, 0);
	  put_qio();
	  for (i = 0; i < MAX_CREATURES; i++)
	    {
	      c_ptr = &c_list[i];
	      /* Begin writing to file                                 */
	      (void) fprintf(file1, "--------------------------------------------\n");
	      (void) strcpy(out_val, c_ptr->name);
	      (void) strcat(out_val, "                              ");
	      (void) fprintf(file1, "%d  %s     (%c)\n", i, out_val, c_ptr->cchar);
	      (void) fprintf(file1, "     Speed ==%d  Level     ==%d  Exp ==%d\n",
		      c_ptr->speed, c_ptr->level, (int)c_ptr->mexp);
	      (void) fprintf(file1, "     AC    ==%d  Eye-sight ==%d  HD  ==%s\n",
		      c_ptr->ac, c_ptr->aaf, c_ptr->hd);
	      if (0x80000000 & c_ptr->cmove)
		(void) fprintf(file1, "     Creature is a ***Win Creature***\n");
	      if (0x00080000 & c_ptr->cmove)
		(void) fprintf(file1, "     Creature Eats/kills other creatures.\n");
	      if (0x0001 & c_ptr->cdefense)
		(void) fprintf(file1, "     Creature is a dragon.\n");
	      if (0x0002 & c_ptr->cdefense)
		(void) fprintf(file1, "     Creature is a monster.\n");
	      if (0x0004 & c_ptr->cdefense)
		(void) fprintf(file1, "     Creature is evil.\n");
	      if (0x0008 & c_ptr->cdefense)
		(void) fprintf(file1, "     Creature is undead.\n");
	      if (0x0010 & c_ptr->cdefense)
		(void) fprintf(file1, "     Creature harmed by cold.\n");
	      if (0x0020 & c_ptr->cdefense)
		(void) fprintf(file1, "     Creature harmed by fire.\n");
	      if (0x0040 & c_ptr->cdefense)
		(void) fprintf(file1, "     Creature harmed by poison.\n");
	      if (0x0080 & c_ptr->cdefense)
		(void) fprintf(file1, "     Creature harmed by acid.\n");
	      if (0x0100 & c_ptr->cdefense)
		(void) fprintf(file1, "     Creature harmed by blue light.\n");
	      if (0x0200 & c_ptr->cdefense)
		(void) fprintf(file1, "     Creature harmed by Stone-to-Mud.\n");
	      if (0x1000 & c_ptr->cdefense)
		(void) fprintf(file1, "     Creature cannot be charmed or slept.\n");
	      if (0x2000 & c_ptr->cdefense)
		(void) fprintf(file1, "     Creature seen with Infra-Vision.\n");
	      if (0x4000 & c_ptr->cdefense)
		(void) fprintf(file1, "     Creature has MAX hit points.\n");
	      if (0x00010000 & c_ptr->cmove)
		(void) fprintf(file1, "     Creature is invisible.\n");
	      if (0x00100000 & c_ptr->cmove)
		(void) fprintf(file1, "     Creature picks up objects.\n");
	      if (0x00200000 & c_ptr->cmove)
		(void) fprintf(file1, "     Creature multiplies.\n");
	      if (0x01000000 & c_ptr->cmove)
		(void) fprintf(file1, "     Carries object(s).\n");
	      if (0x02000000 & c_ptr->cmove)
		(void) fprintf(file1, "     Carries gold, gems, etc.\n");
	      if (0x04000000 & c_ptr->cmove)
		(void) fprintf(file1, "       Has object/gold 60%% of time.\n");
	      if (0x08000000 & c_ptr->cmove)
		(void) fprintf(file1, "       Has object/gold 90%% of time.\n");
	      if (0x10000000 & c_ptr->cmove)
		(void) fprintf(file1, "       Has 1d2 object(s)/gold.\n");
	      if (0x20000000 & c_ptr->cmove)
		(void) fprintf(file1, "       Has 2d2 object(s)/gold.\n");
	      if (0x40000000 & c_ptr->cmove)
		(void) fprintf(file1, "       Has 4d2 object(s)/gold.\n");
	      /*
	       * Creature casts spells / Breathes Dragon
	       * breath...
	       */
	      if (c_ptr->spells != 0)
		{
		  (void) fprintf(file1, "   --Spells/Dragon Breath ==\n");
		  (void) fprintf(file1, "       Casts spells 1 out of %d turns.\n",
			  (int)(0xF & c_ptr->spells));
		  if (0x00000010 & c_ptr->spells)
		    (void) fprintf(file1, "       Can teleport short.\n");
		  if (0x00000020 & c_ptr->spells)
		    (void) fprintf(file1, "       Can teleport long.\n");
		  if (0x00000040 & c_ptr->spells)
		    (void) fprintf(file1, "       Teleport player to itself.\n");
		  if (0x00000080 & c_ptr->spells)
		    (void) fprintf(file1, "       Cause light wounds.\n");
		  if (0x00000100 & c_ptr->spells)
		    (void) fprintf(file1, "       Cause serious wounds.\n");
		  if (0x00000200 & c_ptr->spells)
		    (void) fprintf(file1, "       Hold person.\n");
		  if (0x00000400 & c_ptr->spells)
		    (void) fprintf(file1, "       Cause blindness.\n");
		  if (0x00000800 & c_ptr->spells)
		    (void) fprintf(file1, "       Cause confusion.\n");
		  if (0x00001000 & c_ptr->spells)
		    (void) fprintf(file1, "       Cause fear.\n");
		  if (0x00002000 & c_ptr->spells)
		    (void) fprintf(file1, "       Summon a monster.\n");
		  if (0x00004000 & c_ptr->spells)
		    (void) fprintf(file1, "       Summon an undead.\n");
		  if (0x00008000 & c_ptr->spells)
		    (void) fprintf(file1, "       Slow person.\n");
		  if (0x00010000 & c_ptr->spells)
		    (void) fprintf(file1, "       Drains mana for healing.\n");
		  if (0x00020000 & c_ptr->spells)
		    (void) fprintf(file1, "       **Unknown spell value**\n");
		  if (0x00040000 & c_ptr->spells)
		    (void) fprintf(file1, "       **Unknown spell value**\n");
		  if (0x00080000 & c_ptr->spells)
		    (void) fprintf(file1, "       Breathes Lightning Dragon Breath.\n");
		  if (0x00100000 & c_ptr->spells)
		    (void) fprintf(file1, "       Breathes Gas Dragon Breath.\n");
		  if (0x00200000 & c_ptr->spells)
		    (void) fprintf(file1, "       Breathes Acid Dragon Breath.\n");
		  if (0x00400000 & c_ptr->spells)
		    (void) fprintf(file1, "       Breathes Frost Dragon Breath.\n");
		  if (0x00800000 & c_ptr->spells)
		    (void) fprintf(file1, "       Breathes Fire Dragon Breath.\n");
		}
	      /* Movement for creature                                 */
	      (void) fprintf(file1, "   --Movement ==\n");
	      if (0x00000001 & c_ptr->cmove)
		(void) fprintf(file1, "       Move only to attack.\n");
	      if (0x00000002 & c_ptr->cmove)
		(void) fprintf(file1, "       Move and attack normally.\n");
	      if (0x00000008 & c_ptr->cmove)
		(void) fprintf(file1, "       20%% random movement.\n");
	      if (0x00000010 & c_ptr->cmove)
		(void) fprintf(file1, "       40%% random movement.\n");
	      if (0x00000020 & c_ptr->cmove)
		(void) fprintf(file1, "       75%% random movement.\n");
	      if (0x00020000 & c_ptr->cmove)
		(void) fprintf(file1, "       Can open doors.\n");
	      if (0x00040000 & c_ptr->cmove)
		(void) fprintf(file1, "       Can phase through walls.\n");

	      (void) fprintf(file1, "   --Creature attacks ==\n");
	      (void) strcpy(attstr, c_ptr->damage);
	      while (strlen(attstr) > 0)
		{
		  string = index(attstr, '|');
		  if (string)
		    xpos = strlen(attstr) - strlen(string);
		  else
		    xpos = -1;
		  if (xpos >= 0)
		    {
		      (void) strncpy(attx, attstr, xpos);
		      attx[xpos] = '\0';
		      (void) strcpy(attstr, &attstr[xpos + 1]);
		    }
		  else
		    {
		      (void) strcpy(attx, attstr);
		      attstr[0] = '\0';
		    }
		  (void) sscanf(attx, "%d%d%s", &attype, &adesc, damstr);
		  out_val[0] = '\0';
		  switch (adesc)
		    {
		    case 1:
		      (void) strcpy(out_val, "       Hits for ");
		      break;
		    case 2:
		      (void) strcpy(out_val, "       Bites for ");
		      break;
		    case 3:
		      (void) strcpy(out_val, "       Claws for ");
		      break;
		    case 4:
		      (void) strcpy(out_val, "       Stings for ");
		      break;
		    case 5:
		      (void) strcpy(out_val, "       Touches for ");
		      break;
		    case 6:
		      (void) strcpy(out_val, "       Kicks for ");
		      break;
		    case 7:
		      (void) strcpy(out_val, "       Gazes for ");
		      break;
		    case 8:
		      (void) strcpy(out_val, "       Breathes for ");
		      break;
		    case 9:
		      (void) strcpy(out_val, "       Spits for ");
		      break;
		    case 10:
		      (void) strcpy(out_val, "       Wails for ");
		      break;
		    case 11:
		      (void) strcpy(out_val, "       Embraces for ");
		      break;
		    case 12:
		      (void) strcpy(out_val, "       Crawls on you for ");
		      break;
		    case 13:
		      (void) strcpy(out_val, "       Shoots spores for ");
		      break;
		    case 14:
		      (void) strcpy(out_val, "       Begs for money for ");
		      break;
		    case 15:
		      (void) strcpy(out_val, "       Slimes you for ");
		      break;
		    case 16:
		      (void) strcpy(out_val, "       Crushes you for ");
		      break;
		    case 17:
		      (void) strcpy(out_val, "       Tramples you for ");
		      break;
		    case 18:
		      (void) strcpy(out_val, "       Drools on you for ");
		      break;
		    case 19:
		      (void) strcpy(out_val, "       Insults you for ");
		      break;
		    case 99:
		      (void) strcpy(out_val, "       Is repelled...");
		      break;
		    default:
		      (void) strcpy(out_val, "     **Unknown value** ");
		      break;
		    }
		  switch (attype)
		    {
		    case 1:
		      (void) strcat(out_val, "normal damage.");
		      break;
		    case 2:
		      (void) strcat(out_val, "lowering strength.");
		      break;
		    case 3:
		      (void) strcat(out_val, "confusion.");
		      break;
		    case 4:
		      (void) strcat(out_val, "fear.");
		      break;
		    case 5:
		      (void) strcat(out_val, "fire damage.");
		      break;
		    case 6:
		      (void) strcat(out_val, "acid damage.");
		      break;
		    case 7:
		      (void) strcat(out_val, "cold damage.");
		      break;
		    case 8:
		      (void) strcat(out_val, "lightning damage.");
		      break;
		    case 9:
		      (void) strcat(out_val, "corrosion damage.");
		      break;
		    case 10:
		      (void) strcat(out_val, "blindness.");
		      break;
		    case 11:
		      (void) strcat(out_val, "paralyzation.");
		      break;
		    case 12:
		      (void) strcat(out_val, "stealing money.");
		      break;
		    case 13:
		      (void) strcat(out_val, "stealing object.");
		      break;
		    case 14:
		      (void) strcat(out_val, "poison damage.");
		      break;
		    case 15:
		      (void) strcat(out_val, "lose dexterity.");
		      break;
		    case 16:
		      (void) strcat(out_val, "lose constitution.");
		      break;
		    case 17:
		      (void) strcat(out_val, "lose intelligence.");
		      break;
		    case 18:
		      (void) strcat(out_val, "lose wisdom.");
		      break;
		    case 19:
		      (void) strcat(out_val, "lose experience.");
		      break;
		    case 20:
		      (void) strcat(out_val, "aggravates monsters.");
		      break;
		    case 21:
		      (void) strcat(out_val, "disenchants objects.");
		      break;
		    case 22:
		      (void) strcat(out_val, "eating food.");
		      break;
		    case 23:
		      (void) strcat(out_val, "eating light source.");
		      break;
		    case 24:
		      (void) strcat(out_val, "absorbing charges.");
		      break;
		    case 99:
		      (void) strcat(out_val, "blank message.");
		      break;
		    default:
		      (void) strcat(out_val, "**Unknown value**");
		      break;
		    }
		  (void) fprintf(file1, "%s (%s)\n", out_val, damstr);
		}
	      for (j = 0; j < 2; j++)
		(void) fprintf(file1, "\n");
	    }
	  /* End writing to file                                   */
	  (void) fclose(file1);
	  prt("Completed.", 0, 0);
	}
    }
}


/* Print the character to a file or device		-RAK-	 */
file_character()
{
  register int i;
  int j, xbth, xbthb, xfos, xsrh, xstl, xdis, xsave, xdev;
  vtype xinfra;
  register FILE *file1;
  vtype out_val, filename1, prt1, prt2;
  stat_type out_str, out_int, out_wis, out_dex, out_con, out_chr;
  register struct misc *p_ptr;
  register treasure_type *i_ptr;

  prt("File name: ", 0, 0);
  if (get_string(filename1, 0, 11, 64))
    {
      if (strlen(filename1) == 0)
	(void) strcpy(filename1, "MORIACHR.DAT");
      if ((file1 = fopen(filename1, "w")) != NULL)
	{
	  prt("Writing character sheet...", 0, 0);
	  put_qio();
	  (void) fprintf(file1, "%c", 12);
	  cnv_stat(py.stats.cstr, out_str);
	  cnv_stat(py.stats.cint, out_int);
	  cnv_stat(py.stats.cwis, out_wis);
	  cnv_stat(py.stats.cdex, out_dex);
	  cnv_stat(py.stats.ccon, out_con);
	  cnv_stat(py.stats.cchr, out_chr);
	  (void) fprintf(file1, "\n");
	  (void) fprintf(file1, "\n");
	  (void) fprintf(file1, "\n");
	  (void) fprintf(file1, "  Name  :%s", pad(py.misc.name, " ", 25));
	  (void) fprintf(file1, "  Age         :%4d", (int)py.misc.age);
	  (void) fprintf(file1, "     Strength     :%s\n", out_str);
	  (void) fprintf(file1, "  Race  :%s", pad(py.misc.race, " ", 25));
	  (void) fprintf(file1, "  Height      :%4d", (int)py.misc.ht);
	  (void) fprintf(file1, "     Intelligence :%s\n", out_int);
	  (void) fprintf(file1, "  Sex   :%s", pad(py.misc.sex, " ", 25));
	  (void) fprintf(file1, "  Weight      :%4d", (int)py.misc.wt);
	  (void) fprintf(file1, "     Wisdom       :%s\n", out_wis);
	  (void) fprintf(file1, "  Class :%s", pad(py.misc.tclass, " ", 25));
	  (void) fprintf(file1, "  Social Class:%4d", py.misc.sc);
	  (void) fprintf(file1, "     Dexterity    :%s\n", out_dex);
	  (void) fprintf(file1, "  Title :%s", pad(py.misc.title, " ", 25));
	  (void) fprintf(file1, "                   ");
	  (void) fprintf(file1, "     Constitution :%s\n", out_con);
	  (void) fprintf(file1, "                                  ");
	  (void) fprintf(file1, "                   ");
	  (void) fprintf(file1, "     Charisma     :%s\n", out_chr);
	  (void) fprintf(file1, "\n");
	  (void) fprintf(file1, "\n");
	  (void) fprintf(file1, "\n");
	  (void) fprintf(file1, "\n");

	  (void) fprintf(file1, "  + To Hit    :%6d", py.misc.dis_th);
	  (void) fprintf(file1, "     Level      :%6d", (int)py.misc.lev);
	  (void) fprintf(file1, "     Max Hit Points :%6d\n", py.misc.mhp);
	  (void) fprintf(file1, "  + To Damage :%6d", py.misc.dis_td);
	  (void) fprintf(file1, "     Experience :%6d", py.misc.exp);
	  (void) fprintf(file1, "     Cur Hit Points :%6d\n", (int) (py.misc.chp));
	  (void) fprintf(file1, "  + To AC     :%6d", py.misc.dis_tac);
	  (void) fprintf(file1, "     Gold       :%6d", py.misc.au);
	  (void) fprintf(file1, "     Max Mana       :%6d\n", py.misc.mana);
	  (void) fprintf(file1, "    Total AC  :%6d", py.misc.dis_ac);
	  (void) fprintf(file1, "                       ");
	  (void) fprintf(file1, "     Cur Mana       :%6d\n", (int) (py.misc.cmana));

	  (void) fprintf(file1, "\n");
	  (void) fprintf(file1, "\n");
	  p_ptr = &py.misc;
	  xbth = p_ptr->bth + p_ptr->lev * BTH_LEV_ADJ +
	    p_ptr->ptohit * BTH_PLUS_ADJ;
	  xbthb = p_ptr->bthb + p_ptr->lev * BTH_LEV_ADJ +
	    p_ptr->ptohit * BTH_PLUS_ADJ;
	  /* this results in a range from 0 to 29 */
	  xfos = 40 - p_ptr->fos;
	  if (xfos < 0)
	    xfos = 0;
	  xsrh = p_ptr->srh + int_adj();
	  /* this results in a range from 0 to 9 */
	  xstl = p_ptr->stl + 1;
	  xdis = p_ptr->disarm + p_ptr->lev + 2 * todis_adj() + int_adj();
	  xsave = p_ptr->save + p_ptr->lev + wis_adj();
	  xdev = p_ptr->save + p_ptr->lev + int_adj();
	  (void) sprintf(xinfra, "%d feet", py.flags.see_infra * 10);

	  (void) fprintf(file1, "(Miscellaneous Abilities)\n");
	  (void) fprintf(file1, "\n");
	  (void) fprintf(file1, "  Fighting    : %s", pad(likert(xbth, 12), " ", 10));
	  (void) fprintf(file1, "  Stealth     : %s", pad(likert(xstl, 1), " ", 10));
	  (void) fprintf(file1, "  Perception  : %s\n", pad(likert(xfos, 3), " ", 10));
	  (void) fprintf(file1, "  Throw/Bows  : %s", pad(likert(xbthb, 12), " ", 10));
	  (void) fprintf(file1, "  Disarming   : %s", pad(likert(xdis, 8), " ", 10));
	  (void) fprintf(file1, "  Searching   : %s\n", pad(likert(xsrh, 6), " ", 10));
	  (void) fprintf(file1, "  Saving Throw: %s", pad(likert(xsave, 6), " ", 10));
	  (void) fprintf(file1, "  Magic Device: %s", pad(likert(xdev, 6), " ", 10));
	  (void) fprintf(file1, "  Infra-Vision: %s\n", pad(xinfra, " ", 10));
	  /* Write out the character's history     */
	  (void) fprintf(file1, "\n");
	  (void) fprintf(file1, "\n");
	  (void) fprintf(file1, "Character Background\n");
	  for (i = 0; i < 5; i++)
	    (void) fprintf(file1, "%s\n", pad(py.misc.history[i], " ", 71));
	  /* Write out the equipment list...       */
	  j = 0;
	  (void) fprintf(file1, "\n");
	  (void) fprintf(file1, "\n");
	  (void) fprintf(file1, "  [Character's Equipment List]\n");
	  (void) fprintf(file1, "\n");
	  if (equip_ctr == 0)
	    (void) fprintf(file1, "  Character has no equipment in use.\n");
	  else
	    for (i = 22; i < INVEN_MAX; i++)
	      {
		i_ptr = &inventory[i];
		if (i_ptr->tval != 0)
		  {
		    switch (i)
		      {
		      case 22:
			(void) strcpy(prt1, ") You are wielding   : ");
			break;
		      case 23:
			(void) strcpy(prt1, ") Worn on head       : ");
			break;
		      case 24:
			(void) strcpy(prt1, ") Worn around neck   : ");
			break;
		      case 25:
			(void) strcpy(prt1, ") Worn on body       : ");
			break;
		      case 26:
			(void) strcpy(prt1, ") Worn on shield arm : ");
			break;
		      case 27:
			(void) strcpy(prt1, ") Worn on hands      : ");
			break;
		      case 28:
			(void) strcpy(prt1, ") Right ring finger  : ");
			break;
		      case 29:
			(void) strcpy(prt1, ") Left  ring finger  : ");
			break;
		      case 30:
			(void) strcpy(prt1, ") Worn on feet       : ");
			break;
		      case 31:
			(void) strcpy(prt1, ") Worn about body    : ");
			break;
		      case 32:
			(void) strcpy(prt1, ") Light source is    : ");
			break;
		      case 33:
			(void) strcpy(prt1, ") Secondary weapon   : ");
			break;
		      default:
			(void) strcpy(prt1, ") *Unknown value*    : ");
			break;
		      }
		    objdes(prt2, i, TRUE);
		    (void) sprintf(out_val, "  %c%s%s", j + 97, prt1, prt2);
		    (void) fprintf(file1, "%s\n", out_val);
		    j++;
		  }
	      }

	  /* Write out the character's inventory...        */
	  (void) fprintf(file1, "%c", 12);
	  (void) fprintf(file1, "\n");
	  (void) fprintf(file1, "\n");
	  (void) fprintf(file1, "\n");
	  (void) fprintf(file1, "  [General Inventory List]\n");
	  (void) fprintf(file1, "\n");
	  if (inven_ctr == 0)
	    (void) fprintf(file1, "  Character has no objects in inventory.\n");
	  else
	    {
	      for (i = 0; i < inven_ctr; i++)
		{
		  objdes(prt1, i, TRUE);
		  (void) sprintf(out_val, "%c) %s", i + 97, prt1);
		  (void) fprintf(file1, "%s\n", out_val);
		}
	    }
	  (void) fprintf(file1, "%c", 12);
	  (void) fclose(file1);
	  prt("Completed.", 0, 0);
	}
    }
}
