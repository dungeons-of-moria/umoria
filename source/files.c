/* source/files.c: misc code to access files used by Moria

   Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include <stdio.h>

#ifndef STDIO_LOADED
#define STDIO_LOADED
#endif

#if 0
/* moved to externs.h to avoid VMS 'psect' problem */
#include <errno.h>
#endif

#ifdef __TURBOC__
#include	<io.h>
#include	<stdlib.h>
#endif /* __TURBOC__ */
 
#include "config.h"
#include "constant.h"
#include "types.h"

#if defined(GEMDOS) && (__STDC__ == 0) && !defined(ATARIST_TC)
#include <access.h>
char *strcat();
#endif

#ifdef VMS
#include <string.h>
#include <file.h>
#else
#ifdef USG
#ifndef ATARIST_MWC
#include <string.h>
#ifndef ATARIST_TC
#include <fcntl.h>
#endif
#endif
#else
#include <strings.h>
#include <sys/file.h>
#endif
#if defined(ultrix) || defined(USG)
void exit();
#endif
#endif

/* This must be included after fcntl.h, which has a prototype for `open'
   on some systems.  Otherwise, the `open' prototype conflicts with the
   `topen' declaration.  */
#include "externs.h"

#ifdef ATARIST_TC
/* Include this to get prototypes for standard library functions.  */
#include <stdlib.h>
#endif

#ifdef MAC
#include "ScrnMgr.h"
#define GNRL_ALRT	1024
#endif

/*
 *  init_scorefile
 *  Open the score file while we still have the setuid privileges.  Later
 *  when the score is being written out, you must be sure to flock the file
 *  so we don't have multiple people trying to write to it at the same time.
 *  Craig Norborg (doc)		Mon Aug 10 16:41:59 EST 1987
 */
void init_scorefile()
{
#ifdef MAC
  appldirectory ();
#endif

#if defined(atarist) || defined(ATARI_ST) || defined(MAC)
  highscore_fp = fopen(MORIA_TOP, "rb+");
#else
  highscore_fp = fopen(MORIA_TOP, "r+");
#endif

  if (highscore_fp == NULL)
    {
#ifdef MAC
      highscore_fp = fopen (MORIA_TOP, "wb");	/* Create it if not there.  */
      if (highscore_fp == NULL)
	{
	  ParamText ("\pCan't create score file!", NULL, NULL, NULL);
	  DoScreenALRT (GNRL_ALRT, akStop, fixHalf, fixThird);
	  ExitToShell ();
	}
      setfileinfo (MORIA_TOP, currentdirectory (), SCORE_FTYPE);
#else
      (void) fprintf (stderr, "Can't open score file \"%s\"\n", MORIA_TOP);
      exit(1);
#endif
    }
#if defined(MSDOS) || defined(VMS) || defined(MAC)
  /* can't leave it open, since this causes problems on networked PCs and VMS,
     we DO want to check to make sure we can open the file, though */
  fclose (highscore_fp);
#endif

#ifdef MAC
  restoredirectory ();
#endif
}

#ifndef MAC
/* Attempt to open the intro file			-RAK-	 */
/* This routine also checks the hours file vs. what time it is	-Doc */
void read_times()
{
  vtype in_line;
  register int i;
  FILE *file1;

#ifdef MORIA_HOU
  /* Attempt to read hours.dat.	 If it does not exist,	   */
  /* inform the user so he can tell the wizard about it	 */
  if ((file1 = fopen(MORIA_HOU, "r")) != NULL)
    {
      while (fgets(in_line, 80, file1) != CNIL)
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
      restore_term();
      (void) fprintf(stderr, "There is no hours file \"%s\".\n", MORIA_HOU);
      (void) fprintf(stderr, "Please inform the wizard, %s, so he ", WIZARD);
      (void) fprintf(stderr, "can correct this!\n");
      exit(1);
    }

  /* Check the hours, if closed	then exit. */
  if (!check_time())
    {
      if ((file1 = fopen(MORIA_HOU, "r")) != NULL)
	{
	  clear_screen();
#ifdef VMS
	  restore_screen();
#endif
	  for (i = 0; fgets(in_line, 80, file1) != CNIL; i++)
	    put_buffer(in_line, i, 0);
	  pause_line (23);
	  (void) fclose(file1);
	}
      exit_game();
    }
#endif

  /* Print the introduction message, news, etc.		 */
  if ((file1 = fopen(MORIA_MOR, "r")) != NULL)
    {
      clear_screen();
#ifdef VMS
      restore_screen();
#endif
      for (i = 0; fgets(in_line, 80, file1) != CNIL; i++)
	put_buffer(in_line, i, 0);
      pause_line(23);
      (void) fclose(file1);
    }
}
#endif

/* File perusal.	    -CJS-
   primitive, but portable */
void helpfile(filename)
char *filename;
#ifdef MAC
{
  mac_helpfile(filename, TRUE);
}
#else
{
  bigvtype tmp_str;
  FILE *file;
  char input;
  int i;

  file = fopen(filename, "r");
  if (file == NULL)
    {
      (void) sprintf (tmp_str, "Can not find help file \"%s\".\n", filename);
      prt (tmp_str, 0, 0);
      return;
    }

  save_screen();

  while (!feof(file))
    {
      clear_screen();
      for (i = 0; i < 23; i++)
	if (fgets (tmp_str, BIGVTYPESIZ-1, file) != CNIL)
	  put_buffer (tmp_str, i, 0);
      prt("[Press any key to continue.]", 23, 23);
      input = inkey();
      if (input == ESCAPE)
	break;
    }

  (void) fclose(file);
  restore_screen();
}
#endif

/* Prints a list of random objects to a file.  Note that -RAK-	 */
/* the objects produced is a sampling of objects which		 */
/* be expected to appear on that level.				 */
void print_objects()
{
  register int i;
  int nobj, j, level;
  vtype filename1; bigvtype tmp_str;
  register FILE *file1;
  register inven_type *i_ptr;
#ifdef MAC
  short vrefnum;
#endif
#ifdef ATARIST_MWC
  int32u holder;
#endif

  prt("Produce objects on what level?: ", 0, 0);
  level = 0;
  if (!get_string(tmp_str, 0, 32, 10))
    return;
  level = atoi(tmp_str);
  prt("Produce how many objects?: ", 0, 0);
  nobj = 0;
  if (!get_string(tmp_str, 0, 27, 10))
    return;
  nobj = atoi(tmp_str);
  if ((nobj > 0) && (level > -1) && (level < 1201))
    {
      if (nobj > 10000)
	nobj = 10000;
#ifdef MAC
      (void) strcpy(filename1, "Objects");
      if (doputfile("Save objects in:", filename1, &vrefnum))
#else
      prt("File name: ", 0, 0);
      if (get_string(filename1, 0, 11, 64))
#endif
	{
	  if (strlen(filename1) == 0)
	    return;
#ifdef MAC
	  changedirectory(vrefnum);
#endif
	  if ((file1 = fopen(filename1, "w")) != NULL)
	    {
#ifdef MAC
	      macbeginwait ();
#endif

	      (void) sprintf(tmp_str, "%d", nobj);
	      prt(strcat(tmp_str, " random objects being produced..."), 0, 0);
	      put_qio();
	      (void) fprintf(file1, "*** Random Object Sampling:\n");
	      (void) fprintf(file1, "*** %d objects\n", nobj);
	      (void) fprintf(file1, "*** For Level %d\n", level);
	      (void) fprintf(file1, "\n");
	      (void) fprintf(file1, "\n");
	      j = popt();
	      for (i = 0; i < nobj; i++)
		{
		  invcopy(&t_list[j], sorted_objects[get_obj_num(level)]);
		  magic_treasure(j, level);
		  i_ptr = &t_list[j];
		  store_bought(i_ptr);
#ifdef ATARIST_MWC
		  if (i_ptr->flags & (holder = TR_CURSED))
#else
		  if (i_ptr->flags & TR_CURSED)
#endif
		    add_inscribe(i_ptr, ID_DAMD);
		  objdes(tmp_str, i_ptr, TRUE);
		  (void) fprintf(file1, "%d %s\n", i_ptr->level, tmp_str);
		}
	      pusht((int8u)j);
	      (void) fclose(file1);
#ifdef MAC
	      setfileinfo(filename1, vrefnum, INFO_FTYPE);
	      macendwait ();
#endif
	      prt("Completed.", 0, 0);
	    }
	  else
	    prt("File could not be opened.", 0, 0);
#ifdef MAC
	  restoredirectory();
#endif
	}
    }
  else
    prt ("Parameters no good.", 0, 0);
}


/* Print the character to a file or device		-RAK-	 */
#ifdef MAC
int file_character()
#else
int file_character(filename1)
char *filename1;
#endif
{
  register int i;
  int j, xbth, xbthb, xfos, xsrh, xstl, xdis, xsave, xdev;
  vtype xinfra;
  int fd;
  register FILE *file1;
  bigvtype prt2;
  register struct misc *p_ptr;
  register inven_type *i_ptr;
  vtype out_val, prt1;
  char *p, *colon, *blank;
#ifdef MAC
  vtype filename1;
  short vrefnum;
#endif

#ifdef MAC
  (void) makefilename(filename1, "Stats", TRUE);
  if (!doputfile("Save character description in:", filename1, &vrefnum))
    return (FALSE);
#endif

#ifndef VMS
  /* VMS creates a new version of a file, so no need to check for rewrite. */
#ifdef MAC
  changedirectory(vrefnum);
  fd = open (filename1, O_WRONLY|O_CREAT|O_TRUNC);
  restoredirectory();
  macbeginwait ();
#else
#if defined(GEMDOS) && (__STDC__ == 0) && !defined(ATARIST_TC)
  if (!access(filename1, AREAD))
    {
      (void) sprintf(out_val, "Replace existing file %s?", filename1);
      if (get_check(out_val))
	fd = creat(filename1, 1);
    }
  else
    fd = creat (filename1, 1);
#else
  fd = open (filename1, O_WRONLY|O_CREAT|O_EXCL, 0644);
  if (fd < 0 && errno == EEXIST)
    {
      (void) sprintf(out_val, "Replace existing file %s?", filename1);
      if (get_check(out_val))
	fd = open(filename1, O_WRONLY, 0644);
    }
#endif
#endif
  if (fd >= 0)
    {
      /* on some non-unix machines, fdopen() is not reliable, hence must call
	 close() and then fopen() */
      (void) close(fd);
      file1 = fopen(filename1, "w");
    }
  else
    file1 = NULL;
#else /* VMS */
  fd = -1;
  file1 = fopen (filename1, "w");
#endif

  if (file1 != NULL)
    {
      prt("Writing character sheet...", 0, 0);
      put_qio();
      colon = ":";
      blank = " ";
#ifdef MAC
      (void) fprintf(file1, "\n\n");
#else
      (void) fprintf(file1, "%c\n\n", CTRL('L'));
#endif
      (void) fprintf(file1, " Name%9s %-23s", colon, py.misc.name);
      (void) fprintf(file1, " Age%11s %6d", colon, (int)py.misc.age);
      cnv_stat(py.stats.use_stat[A_STR], prt1);
      (void) fprintf(file1, "   STR : %s\n", prt1);
      (void) fprintf(file1, " Race%9s %-23s", colon,
		     race[py.misc.prace].trace);
      (void) fprintf(file1, " Height%8s %6d", colon, (int)py.misc.ht);
      cnv_stat(py.stats.use_stat[A_INT], prt1);
      (void) fprintf(file1, "   INT : %s\n", prt1);
      (void) fprintf(file1, " Sex%10s %-23s", colon,
		     (py.misc.male ? "Male" : "Female"));
      (void) fprintf(file1, " Weight%8s %6d", colon, (int)py.misc.wt);
      cnv_stat(py.stats.use_stat[A_WIS], prt1);
      (void) fprintf(file1, "   WIS : %s\n", prt1);
      (void) fprintf(file1, " Class%8s %-23s", colon,
		     class[py.misc.pclass].title);
      (void) fprintf(file1, " Social Class : %6d", py.misc.sc);
      cnv_stat(py.stats.use_stat[A_DEX], prt1);
      (void) fprintf(file1, "   DEX : %s\n", prt1);
      (void) fprintf(file1, " Title%8s %-23s", colon, title_string());
      (void) fprintf(file1, "%22s", blank);
      cnv_stat(py.stats.use_stat[A_CON], prt1);
      (void) fprintf(file1, "   CON : %s\n", prt1);
      (void) fprintf(file1, "%34s", blank);
      (void) fprintf(file1, "%26s", blank);
      cnv_stat(py.stats.use_stat[A_CHR], prt1);
      (void) fprintf(file1, "   CHR : %s\n\n", prt1);

      (void) fprintf(file1, " + To Hit    : %6d", py.misc.dis_th);
      (void) fprintf(file1, "%8sLevel      : %6d", blank, (int)py.misc.lev);
      (void) fprintf(file1, "    Max Hit Points : %6d\n", py.misc.mhp);
      (void) fprintf(file1, " + To Damage : %6d", py.misc.dis_td);
      (void) fprintf(file1, "%8sExperience : %6ld", blank, py.misc.exp);
      (void) fprintf(file1, "    Cur Hit Points : %6d\n", py.misc.chp);
      (void) fprintf(file1, " + To AC     : %6d", py.misc.dis_tac);
      (void) fprintf(file1, "%8sMax Exp    : %6ld", blank, py.misc.max_exp);
      (void) fprintf(file1, "    Max Mana%8s %6d\n", colon, py.misc.mana);
      (void) fprintf(file1, "   Total AC  : %6d", py.misc.dis_ac);
      if (py.misc.lev == MAX_PLAYER_LEVEL)
	(void) fprintf (file1, "%8sExp to Adv : ******", blank);
      else
	(void) fprintf(file1, "%8sExp to Adv : %6ld", blank,
		       (int32)(player_exp[py.misc.lev-1]
			       * py.misc.expfact / 100));
      (void) fprintf(file1, "    Cur Mana%8s %6d\n", colon, py.misc.cmana);
      (void) fprintf(file1, "%29sGold%8s %6ld\n\n", blank, colon,
		     py.misc.au);

      p_ptr = &py.misc;
      xbth = p_ptr->bth + p_ptr->ptohit * BTH_PLUS_ADJ
	+ (class_level_adj[p_ptr->pclass][CLA_BTH] * p_ptr->lev);
      xbthb = p_ptr->bthb + p_ptr->ptohit * BTH_PLUS_ADJ
	+ (class_level_adj[p_ptr->pclass][CLA_BTHB] * p_ptr->lev);
      /* this results in a range from 0 to 29 */
      xfos = 40 - p_ptr->fos;
      if (xfos < 0)
	xfos = 0;
      xsrh = p_ptr->srh;
      /* this results in a range from 0 to 9 */
      xstl = p_ptr->stl + 1;
      xdis = p_ptr->disarm + 2 * todis_adj() + stat_adj(A_INT)
	+ (class_level_adj[p_ptr->pclass][CLA_DISARM] * p_ptr->lev / 3);
      xsave = p_ptr->save + stat_adj(A_WIS)
	+ (class_level_adj[p_ptr->pclass][CLA_SAVE] * p_ptr->lev / 3);
      xdev = p_ptr->save + stat_adj(A_INT)
	+ (class_level_adj[p_ptr->pclass][CLA_DEVICE] * p_ptr->lev / 3);

      (void) sprintf(xinfra, "%d feet", py.flags.see_infra * 10);

      (void) fprintf(file1, "(Miscellaneous Abilities)\n\n");
      (void) fprintf(file1, " Fighting    : %-10s", likert(xbth, 12));
      (void) fprintf(file1, "   Stealth     : %-10s", likert(xstl, 1));
      (void) fprintf(file1, "   Perception  : %s\n", likert(xfos, 3));
      (void) fprintf(file1, " Bows/Throw  : %-10s", likert(xbthb, 12));
      (void) fprintf(file1, "   Disarming   : %-10s", likert(xdis, 8));
      (void) fprintf(file1, "   Searching   : %s\n", likert(xsrh, 6));
      (void) fprintf(file1, " Saving Throw: %-10s", likert(xsave, 6));
      (void) fprintf(file1, "   Magic Device: %-10s", likert(xdev, 6));
      (void) fprintf(file1, "   Infra-Vision: %s\n\n", xinfra);
      /* Write out the character's history     */
      (void) fprintf(file1, "Character Background\n");
      for (i = 0; i < 4; i++)
	(void) fprintf(file1, " %s\n", py.misc.history[i]);
      /* Write out the equipment list.	     */
      j = 0;
      (void) fprintf(file1, "\n  [Character's Equipment List]\n\n");
      if (equip_ctr == 0)
	(void) fprintf(file1, "  Character has no equipment in use.\n");
      else
	for (i = INVEN_WIELD; i < INVEN_ARRAY_SIZE; i++)
	  {
	    i_ptr = &inventory[i];
	    if (i_ptr->tval != TV_NOTHING)
	      {
		switch (i)
		  {
		  case INVEN_WIELD:	p = "You are wielding";	break;
		  case INVEN_HEAD:	p = "Worn on head";	break;
		  case INVEN_NECK:	p = "Worn around neck";	break;
		  case INVEN_BODY:	p = "Worn on body";	break;
		  case INVEN_ARM:	p = "Worn on shield arm";break;
		  case INVEN_HANDS:	p = "Worn on hands";	break;
		  case INVEN_RIGHT:	p = "Right ring finger";break;
		  case INVEN_LEFT:	p = "Left  ring finger";break;
		  case INVEN_FEET:	p = "Worn on feet";	break;
		  case INVEN_OUTER:	p = "Worn about body";	break;
		  case INVEN_LIGHT:	p = "Light source is";	break;
		  case INVEN_AUX:	p = "Secondary weapon";	break;
		  default: p = "*Unknown value*";     break;
		  }
		objdes(prt2, &inventory[i], TRUE);
		(void) fprintf(file1, "  %c) %-19s: %s\n", j+'a', p, prt2);
		j++;
	      }
	  }

      /* Write out the character's inventory.	     */
#ifdef MAC
      (void) fprintf(file1, "\n\n");
#else
      (void) fprintf(file1, "%c\n\n", CTRL('L'));
#endif
      (void) fprintf(file1, "  [General Inventory List]\n\n");
      if (inven_ctr == 0)
	(void) fprintf(file1, "  Character has no objects in inventory.\n");
      else
	{
	  for (i = 0; i < inven_ctr; i++)
	    {
	      objdes(prt2, &inventory[i], TRUE);
	      (void) fprintf(file1, "%c) %s\n", i+'a', prt2);
	    }
	}
#ifndef MAC
      (void) fprintf(file1, "%c", CTRL('L'));
#endif
      (void) fclose(file1);
#ifdef MAC
      setfileinfo(filename1, vrefnum, INFO_FTYPE);
      macendwait ();
#endif
      prt("Completed.", 0, 0);
      return TRUE;
    }
  else
    {
      if (fd >= 0)
	(void) close (fd);
      (void) sprintf (out_val, "Can't open file %s:", filename1);
      msg_print(out_val);
      return FALSE;
    }
}
