/* source/save.c: save and restore games and monster memory info

   Copyright (C) 1989-2008 James E. Wilson, Robert A. Koeneke, 
                           David J. Grabiner

   This file is part of Umoria.

   Umoria is free software; you can redistribute it and/or modify 
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Umoria is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of 
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License 
   along with Umoria.  If not, see <http://www.gnu.org/licenses/>. */

/* For debugging the savefile code on systems with broken compilers.  */
#if 0
#define DEBUG(x)	x
#else
#define DEBUG(x)
#endif

#include <stdio.h>

#ifdef __TURBOC__
#include	<io.h>
#endif /* __TURBOC__ */
 
#include "config.h"
#include "constant.h"
#include "types.h"

#ifndef USG
/* stuff only needed for Berkeley UNIX */
#include <sys/types.h>
#include <sys/file.h>
#include <sys/param.h>
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
#else
#include "string.h"
#endif
#else
#include <strings.h>
#endif
#endif

/* This must be included after fcntl.h, which has a prototype for `open'
   on some systems.  Otherwise, the `open' prototype conflicts with the
   `topen' declaration.  */
#include "externs.h"

#ifdef ATARIST_TC
#include <time.h>
#endif

DEBUG(static FILE *logfile);

#if defined(LINT_ARGS)
static int sv_write(void);
static void wr_byte(int8u);
static void wr_short(int16u);
static void wr_long(int32u);
static void wr_bytes(int8u *, int);
static void wr_string(char *);
static void wr_shorts(int16u *, int);
static void wr_item(inven_type *);
static void wr_monster(monster_type *);
static void rd_byte(int8u *);
static void rd_short(int16u *);
static void rd_long(int32u *);
static void rd_bytes(int8u *, int);
static void rd_string(char *);
static void rd_shorts(int16u *, int);
static void rd_item(inven_type *);
static void rd_monster(monster_type *);
#else
static int sv_write();
static void wr_byte();
static void wr_short();
static void wr_long();
static void wr_bytes();
static void wr_string();
static void wr_shorts();
static void wr_item();
static void wr_monster();
static void rd_byte();
static void rd_short();
static void rd_long();
static void rd_bytes();
static void rd_string();
static void rd_shorts();
static void rd_item();
static void rd_monster();
#endif

#if !defined(ATARIST_MWC)
#ifdef MAC
#include <time.h>
#else
long time();
#endif
#else
char *malloc();
#endif

/* these are used for the save file, to avoid having to pass them to every
   procedure */
static FILE *fileptr;
static int8u xor_byte;
static int from_savefile;	/* can overwrite old savefile when save */
static int32u start_time;	/* time that play started */

/* This save package was brought to by			-JWT-
   and							-RAK-
   and has been completely rewritten for UNIX by	-JEW-  */
/* and has been completely rewritten again by	 -CJS-	*/
/* and completely rewritten again! for portability by -JEW- */

static int sv_write()
{
  int32u l;
  register int i, j;
  int count;
  int8u char_tmp, prev_char;
  register cave_type *c_ptr;
  register recall_type *r_ptr;
  struct stats *s_ptr;
  register struct flags *f_ptr;
  store_type *st_ptr;
  struct misc *m_ptr;
#if defined(MSDOS) || defined(ATARI_ST)
  inven_type *t_ptr;
#endif

  /* clear the death flag when creating a HANGUP save file, so that player
     can see tombstone when restart */
  if (eof_flag)
    death = FALSE;

  l = 0;
  if (find_cut)
    l |= 0x1;
  if (find_examine)
    l |= 0x2;
  if (find_prself)
    l |= 0x4;
  if (find_bound)
    l |= 0x8;
  if (prompt_carry_flag)
    l |= 0x10;
  if (rogue_like_commands)
    l |= 0x20;
  if (show_weight_flag)
    l |= 0x40;
  if (highlight_seams)
    l |= 0x80;
  if (find_ignore_doors)
    l |= 0x100;
  if (sound_beep_flag)
    l |= 0x200;
  if (display_counts)
    l |= 0x400;
  if (death)
    l |= 0x80000000L;	/* Sign bit */
  if (total_winner)
    l |= 0x40000000L;

  for (i = 0; i < MAX_CREATURES; i++)
    {
      r_ptr = &c_recall[i];
      if (r_ptr->r_cmove || r_ptr->r_cdefense || r_ptr->r_kills ||
	  r_ptr->r_spells || r_ptr->r_deaths || r_ptr->r_attacks[0] ||
	  r_ptr->r_attacks[1] || r_ptr->r_attacks[2] || r_ptr->r_attacks[3])
	{
	  wr_short((int16u)i);
	  wr_long(r_ptr->r_cmove);
	  wr_long(r_ptr->r_spells);
	  wr_short(r_ptr->r_kills);
	  wr_short(r_ptr->r_deaths);
	  wr_short(r_ptr->r_cdefense);
	  wr_byte(r_ptr->r_wake);
	  wr_byte(r_ptr->r_ignore);
	  wr_bytes(r_ptr->r_attacks, MAX_MON_NATTACK);
	}
    }
  wr_short((int16u)0xFFFF); /* sentinel to indicate no more monster info */

  wr_long(l);

  m_ptr = &py.misc;
  wr_string(m_ptr->name);
  wr_byte(m_ptr->male);
  wr_long((int32u)m_ptr->au);
  wr_long((int32u)m_ptr->max_exp);
  wr_long((int32u)m_ptr->exp);
  wr_short(m_ptr->exp_frac);
  wr_short(m_ptr->age);
  wr_short(m_ptr->ht);
  wr_short(m_ptr->wt);
  wr_short(m_ptr->lev);
  wr_short(m_ptr->max_dlv);
  wr_short((int16u)m_ptr->srh);
  wr_short((int16u)m_ptr->fos);
  wr_short((int16u)m_ptr->bth);
  wr_short((int16u)m_ptr->bthb);
  wr_short((int16u)m_ptr->mana);
  wr_short((int16u)m_ptr->mhp);
  wr_short((int16u)m_ptr->ptohit);
  wr_short((int16u)m_ptr->ptodam);
  wr_short((int16u)m_ptr->pac);
  wr_short((int16u)m_ptr->ptoac);
  wr_short((int16u)m_ptr->dis_th);
  wr_short((int16u)m_ptr->dis_td);
  wr_short((int16u)m_ptr->dis_ac);
  wr_short((int16u)m_ptr->dis_tac);
  wr_short((int16u)m_ptr->disarm);
  wr_short((int16u)m_ptr->save);
  wr_short((int16u)m_ptr->sc);
  wr_short((int16u)m_ptr->stl);
  wr_byte(m_ptr->pclass);
  wr_byte(m_ptr->prace);
  wr_byte(m_ptr->hitdie);
  wr_byte(m_ptr->expfact);
  wr_short((int16u)m_ptr->cmana);
  wr_short(m_ptr->cmana_frac);
  wr_short((int16u)m_ptr->chp);
  wr_short(m_ptr->chp_frac);
  for (i = 0; i < 4; i++)
    wr_string (m_ptr->history[i]);

  s_ptr = &py.stats;
  wr_bytes(s_ptr->max_stat, 6);
  wr_bytes(s_ptr->cur_stat, 6);
  wr_shorts((int16u *)s_ptr->mod_stat, 6);
  wr_bytes(s_ptr->use_stat, 6);

  f_ptr = &py.flags;
  wr_long(f_ptr->status);
  wr_short((int16u)f_ptr->rest);
  wr_short((int16u)f_ptr->blind);
  wr_short((int16u)f_ptr->paralysis);
  wr_short((int16u)f_ptr->confused);
  wr_short((int16u)f_ptr->food);
  wr_short((int16u)f_ptr->food_digested);
  wr_short((int16u)f_ptr->protection);
  wr_short((int16u)f_ptr->speed);
  wr_short((int16u)f_ptr->fast);
  wr_short((int16u)f_ptr->slow);
  wr_short((int16u)f_ptr->afraid);
  wr_short((int16u)f_ptr->poisoned);
  wr_short((int16u)f_ptr->image);
  wr_short((int16u)f_ptr->protevil);
  wr_short((int16u)f_ptr->invuln);
  wr_short((int16u)f_ptr->hero);
  wr_short((int16u)f_ptr->shero);
  wr_short((int16u)f_ptr->blessed);
  wr_short((int16u)f_ptr->resist_heat);
  wr_short((int16u)f_ptr->resist_cold);
  wr_short((int16u)f_ptr->detect_inv);
  wr_short((int16u)f_ptr->word_recall);
  wr_short((int16u)f_ptr->see_infra);
  wr_short((int16u)f_ptr->tim_infra);
  wr_byte(f_ptr->see_inv);
  wr_byte(f_ptr->teleport);
  wr_byte(f_ptr->free_act);
  wr_byte(f_ptr->slow_digest);
  wr_byte(f_ptr->aggravate);
  wr_byte(f_ptr->fire_resist);
  wr_byte(f_ptr->cold_resist);
  wr_byte(f_ptr->acid_resist);
  wr_byte(f_ptr->regenerate);
  wr_byte(f_ptr->lght_resist);
  wr_byte(f_ptr->ffall);
  wr_byte(f_ptr->sustain_str);
  wr_byte(f_ptr->sustain_int);
  wr_byte(f_ptr->sustain_wis);
  wr_byte(f_ptr->sustain_con);
  wr_byte(f_ptr->sustain_dex);
  wr_byte(f_ptr->sustain_chr);
  wr_byte(f_ptr->confuse_monster);
  wr_byte(f_ptr->new_spells);

  wr_short((int16u)missile_ctr);
  wr_long((int32u)turn);
  wr_short((int16u)inven_ctr);
  for (i = 0; i < inven_ctr; i++)
    wr_item(&inventory[i]);
  for (i = INVEN_WIELD; i < INVEN_ARRAY_SIZE; i++)
    wr_item(&inventory[i]);
  wr_short((int16u)inven_weight);
  wr_short((int16u)equip_ctr);
  wr_long(spell_learned);
  wr_long(spell_worked);
  wr_long(spell_forgotten);
  wr_bytes(spell_order, 32);
  wr_bytes(object_ident, OBJECT_IDENT_SIZE);
  wr_long(randes_seed);
  wr_long(town_seed);
  wr_short((int16u)last_msg);
  for (i = 0; i < MAX_SAVE_MSG; i++)
    wr_string(old_msg[i]);

  /* this indicates 'cheating' if it is a one */
  wr_short((int16u)panic_save);
  wr_short((int16u)total_winner);
  wr_short((int16u)noscore);
  wr_shorts(player_hp, MAX_PLAYER_LEVEL);

  for (i = 0; i < MAX_STORES; i++)
    {
      st_ptr = &store[i];
      wr_long((int32u)st_ptr->store_open);
      wr_short((int16u)st_ptr->insult_cur);
      wr_byte(st_ptr->owner);
      wr_byte(st_ptr->store_ctr);
      wr_short(st_ptr->good_buy);
      wr_short(st_ptr->bad_buy);
      for (j = 0; j < st_ptr->store_ctr; j++)
	{
	  wr_long((int32u)st_ptr->store_inven[j].scost);
	  wr_item(&st_ptr->store_inven[j].sitem);
	}
    }

  /* save the current time in the savefile */
#ifdef MAC
  l = time((time_t *)0);
#else
  l = time((long *)0);
#endif
  if (l < start_time)
    {
      /* someone is messing with the clock!, assume that we have been
	 playing for 1 day */
      l = start_time + 86400L;
    }
  wr_long(l);

  /* starting with 5.2, put died_from string in savefile */
  wr_string(died_from);

  /* starting with 5.2.2, put the max_score in the savefile */
  l = total_points ();
  wr_long (l);

  /* starting with 5.2.2, put the birth_date in the savefile */
  wr_long ((int32u) birth_date);

  /* only level specific info follows, this allows characters to be
     resurrected, the dungeon level info is not needed for a resurrection */
  if (death)
    {
      if (ferror(fileptr) || fflush(fileptr) == EOF)
	return FALSE;
      return TRUE;
    }

  wr_short((int16u)dun_level);
  wr_short((int16u)char_row);
  wr_short((int16u)char_col);
  wr_short((int16u)mon_tot_mult);
  wr_short((int16u)cur_height);
  wr_short((int16u)cur_width);
  wr_short((int16u)max_panel_rows);
  wr_short((int16u)max_panel_cols);

  for (i = 0; i < MAX_HEIGHT; i++)
    for (j = 0; j < MAX_WIDTH; j++)
      {
	c_ptr = &cave[i][j];
	if (c_ptr->cptr != 0)
	  {
	    wr_byte((int8u)i);
	    wr_byte((int8u)j);
	    wr_byte(c_ptr->cptr);
	  }
      }
  wr_byte((int8u)0xFF); /* marks end of cptr info */
  for (i = 0; i < MAX_HEIGHT; i++)
    for (j = 0; j < MAX_WIDTH; j++)
      {
	c_ptr = &cave[i][j];
	if (c_ptr->tptr != 0)
	  {
	    wr_byte((int8u)i);
	    wr_byte((int8u)j);
	    wr_byte(c_ptr->tptr);
	  }
      }
  wr_byte((int8u)0xFF); /* marks end of tptr info */
  /* must set counter to zero, note that code may write out two bytes
     unnecessarily */
  count = 0;
  prev_char = 0;
  for (i = 0; i < MAX_HEIGHT; i++)
    for (j = 0; j < MAX_WIDTH; j++)
      {
	c_ptr = &cave[i][j];
	char_tmp = c_ptr->fval | (c_ptr->lr << 4) | (c_ptr->fm << 5) |
	  (c_ptr->pl << 6) | (c_ptr->tl << 7);
	if (char_tmp != prev_char || count == MAX_UCHAR)
	  {
	    wr_byte((int8u)count);
	    wr_byte(prev_char);
	    prev_char = char_tmp;
	    count = 1;
	  }
	else
	  count++;
      }
  /* save last entry */
  wr_byte((int8u)count);
  wr_byte(prev_char);

#if defined(MSDOS) || defined(ATARI_ST)
  /* must change graphics symbols for walls and floors back to default chars,
     this is necessary so that if the user changes the graphics line, the
     program will be able change all existing walls and floors to the new
     symbol */
  /* Or if the user moves the savefile from one machine to another, we
     must have a consistent representation here.  */
  t_ptr = &t_list[tcptr - 1];
  for (i = tcptr - 1; i >= MIN_TRIX; i--)
    {
#ifdef MSDOS
      if (t_ptr->tchar == wallsym)
	t_ptr->tchar = '#';
#endif
#ifdef ATARI_ST
      if (t_ptr->tchar == (unsigned char)240)
	t_ptr->tchar = '#';
#endif
      t_ptr--;
    }
#endif
  wr_short((int16u)tcptr);
  for (i = MIN_TRIX; i < tcptr; i++)
    wr_item(&t_list[i]);
  wr_short((int16u)mfptr);
  for (i = MIN_MONIX; i < mfptr; i++)
    wr_monster(&m_list[i]);

  if (ferror(fileptr) || (fflush(fileptr) == EOF))
    return FALSE;
  return TRUE;
}

#ifdef MAC

/* Set up prior to actual save, do the save, then clean up */
/* Notice that Mac version of this function takes a parameter */
/* To do a "save as" set always_ask */
/* To do a "save" clear always_ask */

int save_char(always_ask)
int always_ask;
{
  int rc, already_set, proceed;
  int16 vrefnum;

  /* cannot rely on _save_char to do this because we may put up a dialog */
  if (character_saved) return(TRUE);

  enablefilemenu(FALSE);

  already_set = getsavedefaults(savefile, &vrefnum);

  if (!already_set || always_ask)
    {
      /* Here if always_ask or user has not yet specified a save file */
      /* User specifies a save file when he restarts a previous one */
      sfposition(vrefnum);
      proceed = doputfile(death ? "Save memories as:" : "Save game as:",
			  savefile, &vrefnum);
    }
  else
    proceed = TRUE;

  if (proceed)
    {
      changedirectory(vrefnum);
      rc = _save_char(savefile);
      restoredirectory();
    }
  else
    rc = FALSE;

  if (rc)
    (void) setfileinfo(savefile, vrefnum, SAVE_FTYPE);

  enablefilemenu(TRUE);

  return(rc);
}

#else

/* The Mac has different logic here -- See above */

int save_char()
{
  int i;
  vtype temp;

#ifdef SECURE
  bePlayer();
#endif

  while (!_save_char(savefile))
    {
      (void) sprintf(temp, "Savefile '%s' fails.", savefile);
      msg_print(temp);
      i = 0;
      if (access(savefile, 0) < 0
	  || get_check("File exists. Delete old savefile?") == 0
	  || (i = unlink(savefile)) < 0)
	{
	  if (i < 0)
	    {
	      (void) sprintf(temp, "Can't delete '%s'", savefile);
	      msg_print(temp);
	    }
	  prt("New Savefile [ESC to give up]:", 0, 0);
	  if (!get_string(temp, 0, 31, 45))
	    return FALSE;
	  if (temp[0])
	    (void) strcpy(savefile, temp);
	}
      (void) sprintf(temp, "Saving with %s...", savefile);
      prt(temp, 0, 0);
    }
#ifdef SECURE
  beGames();
#endif
  return TRUE;
}
#endif

int _save_char(fnam)
char *fnam;
{
  vtype temp;
  register int ok, fd;
  int8u char_tmp;

  if (character_saved)
    return TRUE;	/* Nothing to save. */

  nosignals();
  put_qio();
  disturb (1, 0);		/* Turn off resting and searching. */
  change_speed(-pack_heavy);	/* Fix the speed */
  pack_heavy = 0;
  ok = FALSE;
  /* VMS files have version numbers, so don't worry about overwriting
     the old save file. */
#if !defined(ATARIST_MWC) && !defined(VMS)
  fd = -1;
  fileptr = NULL;		/* Do not assume it has been init'ed */
#if defined(MAC) || defined(AMIGA)
  /* The Mac version automatically overwrites */
  fd = open(fnam, O_RDWR|O_CREAT|O_TRUNC);
#ifdef MAC
  macbeginwait ();
#endif
#else
  fd = open(fnam, O_RDWR|O_CREAT|O_EXCL, 0600);
  if (fd < 0 && access(fnam, 0) >= 0 &&
      (from_savefile ||
       (wizard && get_check("Can't make new savefile. Overwrite old?"))))
    {
      (void) chmod(fnam, 0600);
      fd = open(fnam, O_RDWR|O_TRUNC, 0600);
    }
#endif
  if (fd >= 0)
    {
      (void) close(fd);
#endif /* !ATARIST_MWC && !VMS */
      /* GCC for atari st defines atarist */
#if defined(atarist) || defined(ATARI_ST) || defined(THINK_C) || defined(MSDOS)
      fileptr = fopen(savefile, "wb");
#else
      fileptr = fopen(savefile, "w");
#endif
#if !defined(ATARIST_MWC) && !defined(VMS)
    }
#endif
  DEBUG(logfile = fopen("IO_LOG", "a"));
  DEBUG(fprintf (logfile, "Saving data to %s\n", savefile));
  if (fileptr != NULL)
    {
      xor_byte = 0;
      wr_byte((int8u)CUR_VERSION_MAJ);
      xor_byte = 0;
      wr_byte((int8u)CUR_VERSION_MIN);
      xor_byte = 0;
      wr_byte((int8u)PATCH_LEVEL);
      xor_byte = 0;
      char_tmp = randint(256) - 1;
      wr_byte(char_tmp);
      /* Note that xor_byte is now equal to char_tmp */

      ok = sv_write();

      DEBUG(fclose (logfile));

      if (fclose(fileptr) == EOF)
	ok = FALSE;
    }

#ifdef MAC
  macendwait ();
#endif

  if (!ok)
    {
      if (fd >= 0)
	(void) unlink(fnam);
      signals();
      if (fd >= 0)
	(void) sprintf(temp, "Error writing to file %s", fnam);
      else
	(void) sprintf(temp, "Can't create new file %s", fnam);
      msg_print(temp);
      return FALSE;
    }
  else
    character_saved = 1;

  turn = -1;
  signals();

  return TRUE;
}


#ifdef MAC
/* Wrapper to set the appropriate directory */
int get_char(generate)
int *generate;
{
  int rc, exit_flag;
  int16 vrefnum;

  (void) getsavedefaults(savefile, &vrefnum);

  changedirectory(vrefnum);
  rc = _get_char(generate, &exit_flag);
  restoredirectory();

  if (exit_flag)
    exit_game();

  return(rc);
}
#endif

/* Certain checks are ommitted for the wizard. -CJS- */

#ifdef MAC
int _get_char(generate, exit_flag)
int *generate, *exit_flag;
#else
int get_char(generate)
int *generate;
#endif
{
  register int i, j;
  int fd, c, ok, total_count;
  int32u l, age, time_saved;
  vtype temp;
  int16u int16u_tmp;
  register cave_type *c_ptr;
  register recall_type *r_ptr;
  struct misc *m_ptr;
  struct stats *s_ptr;
  register struct flags *f_ptr;
  store_type *st_ptr;
  int8u char_tmp, ychar, xchar, count;
  int8u version_maj, version_min, patch_level;
#if defined(MSDOS) || defined(ATARI_ST)
  inven_type *t_ptr;
#endif

#ifdef MAC
  *exit_flag = FALSE;
#endif

  nosignals();
  *generate = TRUE;
  fd = -1;

#ifndef MAC
  /* Not required for Mac, because the file name is obtained through a dialog.
     There is no way for a non existnat file to be specified.  -BS-	*/
  if (access(savefile, 0) != 0)
    {
      signals();
      msg_print("Savefile does not exist.");
      return FALSE;	/* Don't bother with messages here. File absent. */
    }
#endif

  clear_screen();

  (void) sprintf(temp, "Savefile %s present. Attempting restore.", savefile);
  put_buffer(temp, 23, 0);

  if (turn >= 0)
    msg_print("IMPOSSIBLE! Attempt to restore while still alive!");

  /* Allow restoring a file belonging to someone else, if we can delete it. */
  /* Hence first try to read without doing a chmod. */

#if defined(MAC) || defined(AMIGA)
  else if ((fd = open(savefile, O_RDONLY)) < 0)
#else
#ifdef ATARI_ST
  else if (FALSE)
#else
  else if ((fd = open(savefile, O_RDONLY, 0)) < 0
	   && (chmod(savefile, 0400) < 0 ||
	       (fd = open(savefile, O_RDONLY, 0)) < 0))
#endif
#endif
    msg_print("Can't open file for reading.");
  else
    {
      turn = -1;
      ok = TRUE;

      (void) close(fd);
      fd = -1; /* Make sure it isn't closed again */
      /* GCC for atari st defines atarist */
#if defined(atarist) || defined(ATARI_ST) || defined(THINK_C) || defined(MSDOS)
      fileptr = fopen(savefile, "rb");
#else
      fileptr = fopen(savefile, "r");
#endif
      if (fileptr == NULL)
	goto error;

#ifdef MAC
      macbeginwait ();
#endif

      prt("Restoring Memory...", 0, 0);
      put_qio();

      DEBUG(logfile = fopen("IO_LOG", "a"));
      DEBUG(fprintf (logfile, "Reading data from %s\n", savefile));

      xor_byte = 0;
      rd_byte(&version_maj);
      xor_byte = 0;
      rd_byte(&version_min);
      xor_byte = 0;
      rd_byte(&patch_level);
      xor_byte = 0;
      rd_byte(&xor_byte);

      /* COMPAT support savefiles from 5.0.14 to 5.0.17 */
      /* support savefiles from 5.1.0 to present */
      if ((version_maj != CUR_VERSION_MAJ)
#if 0
	  /* As of version 5.4, accept savefiles even if they have higher
	     version numbers.  The savefile format was frozen as of version
	     5.2.2.  */
	  || (version_min > CUR_VERSION_MIN)
	  || (version_min == CUR_VERSION_MIN && patch_level > PATCH_LEVEL)
#endif
	  || (version_min == 0 && patch_level < 14))
	{
	  prt("Sorry. This savefile is from a different version of umoria.",
	      2, 0);
	  goto error;
	}

      rd_short(&int16u_tmp);
      while (int16u_tmp != 0xFFFF)
	{
	  if (int16u_tmp >= MAX_CREATURES)
	    goto error;
	  r_ptr = &c_recall[int16u_tmp];
	  rd_long(&r_ptr->r_cmove);
	  rd_long(&r_ptr->r_spells);
	  rd_short(&r_ptr->r_kills);
	  rd_short(&r_ptr->r_deaths);
	  rd_short(&r_ptr->r_cdefense);
	  rd_byte(&r_ptr->r_wake);
	  rd_byte(&r_ptr->r_ignore);
	  rd_bytes(r_ptr->r_attacks, MAX_MON_NATTACK);
	  rd_short(&int16u_tmp);
	}

      /* for save files before 5.2.2, read and ignore log_index (sic) */
      if ((version_min < 2) || (version_min == 2 && patch_level < 2))
	rd_short(&int16u_tmp);
      rd_long(&l);

      if (l & 0x1)
	find_cut = TRUE;
      else
	find_cut = FALSE;
      if (l & 0x2)
	find_examine = TRUE;
      else
	find_examine = FALSE;
      if (l & 0x4)
	find_prself = TRUE;
      else
	find_prself = FALSE;
      if (l & 0x8)
	find_bound = TRUE;
      else
	find_bound = FALSE;
      if (l & 0x10)
	prompt_carry_flag = TRUE;
      else
	prompt_carry_flag = FALSE;
      if (l & 0x20)
	rogue_like_commands = TRUE;
      else
	rogue_like_commands = FALSE;
      if (l & 0x40)
	show_weight_flag = TRUE;
      else
	show_weight_flag = FALSE;
      if (l & 0x80)
	highlight_seams = TRUE;
      else
	highlight_seams = FALSE;
      if (l & 0x100)
	find_ignore_doors = TRUE;
      else
	find_ignore_doors = FALSE;
      /* save files before 5.2.2 don't have sound_beep_flag, set it on
	 for compatibility */
      if ((version_min < 2) || (version_min == 2 && patch_level < 2))
	sound_beep_flag = TRUE;
      else if (l & 0x200)
	sound_beep_flag = TRUE;
      else
	sound_beep_flag = FALSE;
      /* save files before 5.2.2 don't have display_counts, set it on
	 for compatibility */
      if ((version_min < 2) || (version_min == 2 && patch_level < 2))
	display_counts = TRUE;
      else if (l & 0x400)
	display_counts = TRUE;
      else
	display_counts = FALSE;

      /* Don't allow resurrection of total_winner characters.  It causes
	 problems because the character level is out of the allowed range.  */
      if (to_be_wizard && (l & 0x40000000L))
	{
	  msg_print ("Sorry, this character is retired from moria.");
	  msg_print ("You can not resurrect a retired character.");
	}
      else if (to_be_wizard && (l & 0x80000000L)
	       && get_check("Resurrect a dead character?"))
	l &= ~0x80000000L;
      if ((l & 0x80000000L) == 0)
	{
	  m_ptr = &py.misc;
	  rd_string(m_ptr->name);
	  rd_byte(&m_ptr->male);
	  rd_long((int32u *)&m_ptr->au);
	  rd_long((int32u *)&m_ptr->max_exp);
	  rd_long((int32u *)&m_ptr->exp);
	  rd_short(&m_ptr->exp_frac);
	  rd_short(&m_ptr->age);
	  rd_short(&m_ptr->ht);
	  rd_short(&m_ptr->wt);
	  rd_short(&m_ptr->lev);
	  rd_short(&m_ptr->max_dlv);
	  rd_short((int16u *)&m_ptr->srh);
	  rd_short((int16u *)&m_ptr->fos);
	  rd_short((int16u *)&m_ptr->bth);
	  rd_short((int16u *)&m_ptr->bthb);
	  rd_short((int16u *)&m_ptr->mana);
	  rd_short((int16u *)&m_ptr->mhp);
	  rd_short((int16u *)&m_ptr->ptohit);
	  rd_short((int16u *)&m_ptr->ptodam);
	  rd_short((int16u *)&m_ptr->pac);
	  rd_short((int16u *)&m_ptr->ptoac);
	  rd_short((int16u *)&m_ptr->dis_th);
	  rd_short((int16u *)&m_ptr->dis_td);
	  rd_short((int16u *)&m_ptr->dis_ac);
	  rd_short((int16u *)&m_ptr->dis_tac);
	  rd_short((int16u *)&m_ptr->disarm);
	  rd_short((int16u *)&m_ptr->save);
	  rd_short((int16u *)&m_ptr->sc);
	  rd_short((int16u *)&m_ptr->stl);
	  rd_byte(&m_ptr->pclass);
	  rd_byte(&m_ptr->prace);
	  rd_byte(&m_ptr->hitdie);
	  rd_byte(&m_ptr->expfact);
	  rd_short((int16u *)&m_ptr->cmana);
	  rd_short(&m_ptr->cmana_frac);
	  rd_short((int16u *)&m_ptr->chp);
	  rd_short(&m_ptr->chp_frac);
	  for (i = 0; i < 4; i++)
	    rd_string (m_ptr->history[i]);

	  s_ptr = &py.stats;
	  rd_bytes(s_ptr->max_stat, 6);
	  rd_bytes(s_ptr->cur_stat, 6);
	  rd_shorts((int16u *)s_ptr->mod_stat, 6);
	  rd_bytes(s_ptr->use_stat, 6);

	  f_ptr = &py.flags;
	  rd_long(&f_ptr->status);
	  rd_short((int16u *)&f_ptr->rest);
	  rd_short((int16u *)&f_ptr->blind);
	  rd_short((int16u *)&f_ptr->paralysis);
	  rd_short((int16u *)&f_ptr->confused);
	  rd_short((int16u *)&f_ptr->food);
	  rd_short((int16u *)&f_ptr->food_digested);
	  rd_short((int16u *)&f_ptr->protection);
	  rd_short((int16u *)&f_ptr->speed);
	  rd_short((int16u *)&f_ptr->fast);
	  rd_short((int16u *)&f_ptr->slow);
	  rd_short((int16u *)&f_ptr->afraid);
	  rd_short((int16u *)&f_ptr->poisoned);
	  rd_short((int16u *)&f_ptr->image);
	  rd_short((int16u *)&f_ptr->protevil);
	  rd_short((int16u *)&f_ptr->invuln);
	  rd_short((int16u *)&f_ptr->hero);
	  rd_short((int16u *)&f_ptr->shero);
	  rd_short((int16u *)&f_ptr->blessed);
	  rd_short((int16u *)&f_ptr->resist_heat);
	  rd_short((int16u *)&f_ptr->resist_cold);
	  rd_short((int16u *)&f_ptr->detect_inv);
	  rd_short((int16u *)&f_ptr->word_recall);
	  rd_short((int16u *)&f_ptr->see_infra);
	  rd_short((int16u *)&f_ptr->tim_infra);
	  rd_byte(&f_ptr->see_inv);
	  rd_byte(&f_ptr->teleport);
	  rd_byte(&f_ptr->free_act);
	  rd_byte(&f_ptr->slow_digest);
	  rd_byte(&f_ptr->aggravate);
	  rd_byte(&f_ptr->fire_resist);
	  rd_byte(&f_ptr->cold_resist);
	  rd_byte(&f_ptr->acid_resist);
	  rd_byte(&f_ptr->regenerate);
	  rd_byte(&f_ptr->lght_resist);
	  rd_byte(&f_ptr->ffall);
	  rd_byte(&f_ptr->sustain_str);
	  rd_byte(&f_ptr->sustain_int);
	  rd_byte(&f_ptr->sustain_wis);
	  rd_byte(&f_ptr->sustain_con);
	  rd_byte(&f_ptr->sustain_dex);
	  rd_byte(&f_ptr->sustain_chr);
	  rd_byte(&f_ptr->confuse_monster);
	  rd_byte(&f_ptr->new_spells);

	  rd_short((int16u *)&missile_ctr);
	  rd_long((int32u *)&turn);
	  rd_short((int16u *)&inven_ctr);
	  if (inven_ctr > INVEN_WIELD)
	    goto error;
	  for (i = 0; i < inven_ctr; i++)
	    rd_item(&inventory[i]);
	  for (i = INVEN_WIELD; i < INVEN_ARRAY_SIZE; i++)
	    rd_item(&inventory[i]);
	  rd_short((int16u *)&inven_weight);
	  rd_short((int16u *)&equip_ctr);
	  rd_long(&spell_learned);
	  rd_long(&spell_worked);
	  rd_long(&spell_forgotten);
	  rd_bytes(spell_order, 32);
	  rd_bytes(object_ident, OBJECT_IDENT_SIZE);
	  rd_long(&randes_seed);
	  rd_long(&town_seed);
	  rd_short((int16u *)&last_msg);
	  for (i = 0; i < MAX_SAVE_MSG; i++)
	    rd_string(old_msg[i]);

	  rd_short((int16u *)&panic_save);
	  rd_short((int16u *)&total_winner);
	  rd_short((int16u *)&noscore);
	  rd_shorts(player_hp, MAX_PLAYER_LEVEL);

	  if ((version_min >= 2)
	      || (version_min == 1 && patch_level >= 3))
	    for (i = 0; i < MAX_STORES; i++)
	      {
		st_ptr = &store[i];
		rd_long((int32u *)&st_ptr->store_open);
		rd_short((int16u *)&st_ptr->insult_cur);
		rd_byte(&st_ptr->owner);
		rd_byte(&st_ptr->store_ctr);
		rd_short(&st_ptr->good_buy);
		rd_short(&st_ptr->bad_buy);
		if (st_ptr->store_ctr > STORE_INVEN_MAX)
		  goto error;
		for (j = 0; j < st_ptr->store_ctr; j++)
		  {
		    rd_long((int32u *)&st_ptr->store_inven[j].scost);
		    rd_item(&st_ptr->store_inven[j].sitem);
		  }
	      }

	  if ((version_min >= 2)
	      || (version_min == 1 && patch_level >= 3))
	    rd_long(&time_saved);

	  if (version_min >= 2)
	    rd_string(died_from);

	  if ((version_min >= 3)
	      || (version_min == 2 && patch_level >= 2))
	    rd_long ((int32u *)&max_score);
	  else
	    max_score = 0;

	  if ((version_min >= 3)
	      || (version_min == 2 && patch_level >= 2))
	    rd_long ((int32u *)&birth_date);
	  else
#ifdef MAC
	    birth_date = time((time_t *)0);
#else
	    birth_date = time((long *)0);
#endif
	}
      if ((c = getc(fileptr)) == EOF || (l & 0x80000000L))
	{
	  if ((l & 0x80000000L) == 0)
	    {
	      if (!to_be_wizard || turn < 0)
		goto error;
	      prt("Attempting a resurrection!", 0, 0);
	      if (py.misc.chp < 0)
		{
		  py.misc.chp =	 0;
		  py.misc.chp_frac = 0;
		}
	      /* don't let him starve to death immediately */
	      if (py.flags.food < 0)
		py.flags.food = 0;
	      /* don't let him die of poison again immediately */
	      if (py.flags.poisoned > 1)
		py.flags.poisoned = 1;
	      dun_level = 0; /* Resurrect on the town level. */
	      character_generated = 1;
	      /* set noscore to indicate a resurrection, and don't enter
		 wizard mode */
	      to_be_wizard = FALSE;
	      noscore |= 0x1;
	    }
	  else
	    {
	      /* Make sure that this message is seen, since it is a bit
		 more interesting than the other messages.  */
	      msg_print("Restoring Memory of a departed spirit...");
	      turn = -1;
	    }
	  put_qio();
	  goto closefiles;
	}
      if (ungetc(c, fileptr) == EOF)
	goto error;

      prt("Restoring Character...", 0, 0);
      put_qio();

      /* only level specific info should follow, not present for dead
         characters */

      rd_short((int16u *)&dun_level);
      rd_short((int16u *)&char_row);
      rd_short((int16u *)&char_col);
      rd_short((int16u *)&mon_tot_mult);
      rd_short((int16u *)&cur_height);
      rd_short((int16u *)&cur_width);
      rd_short((int16u *)&max_panel_rows);
      rd_short((int16u *)&max_panel_cols);

      /* read in the creature ptr info */
      rd_byte(&char_tmp);
      while (char_tmp != 0xFF)
	{
	  ychar = char_tmp;
	  rd_byte(&xchar);
	  rd_byte(&char_tmp);
	  if (xchar > MAX_WIDTH || ychar > MAX_HEIGHT)
	    goto error;
	  cave[ychar][xchar].cptr = char_tmp;
	  rd_byte(&char_tmp);
	}
      /* read in the treasure ptr info */
      rd_byte(&char_tmp);
      while (char_tmp != 0xFF)
	{
	  ychar = char_tmp;
	  rd_byte(&xchar);
	  rd_byte(&char_tmp);
	  if (xchar > MAX_WIDTH || ychar > MAX_HEIGHT)
	    goto error;
	  cave[ychar][xchar].tptr = char_tmp;
	  rd_byte(&char_tmp);
	}
      /* read in the rest of the cave info */
      c_ptr = &cave[0][0];
      total_count = 0;
      while (total_count != MAX_HEIGHT*MAX_WIDTH)
	{
	  rd_byte(&count);
	  rd_byte(&char_tmp);
	  for (i = count; i > 0; i--)
	    {
#ifndef ATARIST_MWC
	      if (c_ptr >= &cave[MAX_HEIGHT][0])
		goto error;
#endif
	      c_ptr->fval = char_tmp & 0xF;
	      c_ptr->lr = (char_tmp >> 4) & 0x1;
	      c_ptr->fm = (char_tmp >> 5) & 0x1;
	      c_ptr->pl = (char_tmp >> 6) & 0x1;
	      c_ptr->tl = (char_tmp >> 7) & 0x1;
	      c_ptr++;
	    }
	  total_count += count;
	}

      rd_short((int16u *)&tcptr);
      if (tcptr > MAX_TALLOC)
	goto error;
      for (i = MIN_TRIX; i < tcptr; i++)
	rd_item(&t_list[i]);
      rd_short((int16u *)&mfptr);
      if (mfptr > MAX_MALLOC)
	goto error;
      for (i = MIN_MONIX; i < mfptr; i++)
	rd_monster(&m_list[i]);

#if defined(MSDOS) || defined(ATARI_ST)
      /* change walls and floors to graphic symbols */
      t_ptr = &t_list[tcptr - 1];
      for (i = tcptr - 1; i >= MIN_TRIX; i--)
	{
#ifdef MSDOS
	  if (t_ptr->tchar == '#')
	    t_ptr->tchar = wallsym;
#endif
#ifdef ATARI_ST
	  if (t_ptr->tchar == '#')
	    t_ptr->tchar = (unsigned char) 240;
#endif
	  t_ptr--;
	}
#endif

      *generate = FALSE;  /* We have restored a cave - no need to generate. */

      if ((version_min == 1 && patch_level < 3)
	  || (version_min == 0))
	for (i = 0; i < MAX_STORES; i++)
	  {
	    st_ptr = &store[i];
	    rd_long((int32u *)&st_ptr->store_open);
	    rd_short((int16u *)&st_ptr->insult_cur);
	    rd_byte(&st_ptr->owner);
	    rd_byte(&st_ptr->store_ctr);
	    rd_short(&st_ptr->good_buy);
	    rd_short(&st_ptr->bad_buy);
	    if (st_ptr->store_ctr > STORE_INVEN_MAX)
	      goto error;
	    for (j = 0; j < st_ptr->store_ctr; j++)
	      {
		rd_long((int32u *)&st_ptr->store_inven[j].scost);
		rd_item(&st_ptr->store_inven[j].sitem);
	      }
	  }

      /* read the time that the file was saved */
      if (version_min == 0 && patch_level < 16)
	time_saved = 0; /* no time in file, clear to zero */
      else if (version_min == 1 && patch_level < 3)
	rd_long(&time_saved);

      if (ferror(fileptr))
	goto error;

      if (turn < 0)
      error:
	ok = FALSE;	/* Assume bad data. */
      else
	{
	  /* don't overwrite the killed by string if character is dead */
	  if (py.misc.chp >= 0)
	    (void) strcpy(died_from, "(alive and well)");
	  character_generated = 1;
	}

    closefiles:

      DEBUG(fclose (logfile));

      if (fileptr != NULL)
	{
	  if (fclose(fileptr) < 0)
	    ok = FALSE;
	}
      if (fd >= 0)
	(void) close(fd);

#ifdef MAC
      macendwait ();
#endif

      if (!ok)
	msg_print("Error during reading of file.");
      else
	{
	  /* let the user overwrite the old savefile when save/quit */
	  from_savefile = 1;

	  signals();

	  if (panic_save == 1)
	    {
	      (void) sprintf(temp, "This game is from a panic save.  \
Score will not be added to scoreboard.");
	      msg_print (temp);
	    }
	  else if ((!noscore & 0x04) && duplicate_character ())
	    {
	      (void) sprintf (temp, "This character is already on the \
scoreboard; it will not be scored again.");
	      msg_print (temp);
	      noscore |= 0x4;
	    }

	  if (turn >= 0)
	    {	/* Only if a full restoration. */
	      weapon_heavy = FALSE;
	      pack_heavy = 0;
	      check_strength();

	      /* rotate store inventory, depending on how old the save file */
	      /* is foreach day old (rounded up), call store_maint */
	      /* calculate age in seconds */
#ifdef MAC
	      start_time = time((time_t *)0);
#else
	      start_time = time((long *)0);
#endif
	      /* check for reasonable values of time here ... */
	      if (start_time < time_saved)
		age = 0;
	      else
		age = start_time - time_saved;

	      age = (age + 43200L) / 86400L;  /* age in days */
	      if (age > 10) age = 10; /* in case savefile is very old */
	      for (i = 0; i < age; i++)
		store_maint();
	    }

	  if (noscore)
	    msg_print("This save file cannot be used to get on the score board.");

	  if (version_maj != CUR_VERSION_MAJ
	      || version_min != CUR_VERSION_MIN)
	    {
	      (void) sprintf(temp,
			     "Save file version %d.%d %s on game version %d.%d.",
			     version_maj, version_min,
			     version_min <= CUR_VERSION_MIN
			     ? "accepted" : "risky" ,
			     CUR_VERSION_MAJ, CUR_VERSION_MIN);
	      msg_print(temp);
	    }

	  if (turn >= 0)
	    return TRUE;
	  else
	    return FALSE;	/* Only restored options and monster memory. */
	}
    }
  turn = -1;
  prt("Please try again without that savefile.", 1, 0);
  signals();
#ifdef MAC
  *exit_flag = TRUE;
#else
  exit_game();
#endif

  return FALSE;	/* not reached, unless on mac */
}

static void wr_byte(c)
int8u c;
{
  xor_byte ^= c;
  (void) putc((int)xor_byte, fileptr);
  DEBUG(fprintf (logfile, "BYTE:  %02X = %d\n", (int) xor_byte, (int) c));
}

static void wr_short(s)
int16u s;
{
  xor_byte ^= (s & 0xFF);
  (void) putc((int)xor_byte, fileptr);
  DEBUG(fprintf (logfile, "SHORT: %02X", (int) xor_byte));
  xor_byte ^= ((s >> 8) & 0xFF);
  (void) putc((int)xor_byte, fileptr);
  DEBUG(fprintf (logfile, " %02X = %d\n", (int) xor_byte, (int) s));
}

static void wr_long(l)
register int32u l;
{
  xor_byte ^= (l & 0xFF);
  (void) putc((int)xor_byte, fileptr);
  DEBUG(fprintf (logfile, "LONG:  %02X", (int) xor_byte));
  xor_byte ^= ((l >> 8) & 0xFF);
  (void) putc((int)xor_byte, fileptr);
  DEBUG(fprintf (logfile, " %02X", (int) xor_byte));
  xor_byte ^= ((l >> 16) & 0xFF);
  (void) putc((int)xor_byte, fileptr);
  DEBUG(fprintf (logfile, " %02X", (int) xor_byte));
  xor_byte ^= ((l >> 24) & 0xFF);
  (void) putc((int)xor_byte, fileptr);
  DEBUG(fprintf (logfile, " %02X = %ld\n", (int) xor_byte, (long) l));
}

static void wr_bytes(c, count)
int8u *c;
register int count;
{
  register int i;
  register int8u *ptr;

  DEBUG(fprintf (logfile, "%d BYTES:", count));
  ptr = c;
  for (i = 0; i < count; i++)
    {
      xor_byte ^= *ptr++;
      (void) putc((int)xor_byte, fileptr);
      DEBUG(fprintf (logfile, "  %02X = %d", (int) xor_byte,
		     (int) (ptr[-1])));
    }
  DEBUG(fprintf (logfile, "\n"));
}

static void wr_string(str)
register char *str;
{
  DEBUG(char *s = str);
  DEBUG(fprintf (logfile, "STRING:"));
  while (*str != '\0')
    {
      xor_byte ^= *str++;
      (void) putc((int)xor_byte, fileptr);
      DEBUG(fprintf (logfile, " %02X", (int) xor_byte));
    }
  xor_byte ^= *str;
  (void) putc((int)xor_byte, fileptr);
  DEBUG(fprintf (logfile, " %02X = \"%s\"\n", (int) xor_byte, s));
}

static void wr_shorts(s, count)
int16u *s;
register int count;
{
  register int i;
  register int16u *sptr;

  DEBUG(fprintf (logfile, "%d SHORTS:", count));
  sptr = s;
  for (i = 0; i < count; i++)
    {
      xor_byte ^= (*sptr & 0xFF);
      (void) putc((int)xor_byte, fileptr);
      DEBUG(fprintf (logfile, "  %02X", (int) xor_byte));
      xor_byte ^= ((*sptr++ >> 8) & 0xFF);
      (void) putc((int)xor_byte, fileptr);
      DEBUG(fprintf (logfile, " %02X = %d", (int) xor_byte, (int) sptr[-1]));
    }
  DEBUG(fprintf (logfile, "\n"));
}

static void wr_item(item)
register inven_type *item;
{
  DEBUG(fprintf (logfile, "ITEM:\n"));
  wr_short(item->index);
  wr_byte(item->name2);
  wr_string(item->inscrip);
  wr_long(item->flags);
  wr_byte(item->tval);
  wr_byte(item->tchar);
  wr_short((int16u)item->p1);
  wr_long((int32u)item->cost);
  wr_byte(item->subval);
  wr_byte(item->number);
  wr_short(item->weight);
  wr_short((int16u)item->tohit);
  wr_short((int16u)item->todam);
  wr_short((int16u)item->ac);
  wr_short((int16u)item->toac);
  wr_bytes(item->damage, 2);
  wr_byte(item->level);
  wr_byte(item->ident);
}

static void wr_monster(mon)
register monster_type *mon;
{
  DEBUG(fprintf (logfile, "MONSTER:\n"));
  wr_short((int16u)mon->hp);
  wr_short((int16u)mon->csleep);
  wr_short((int16u)mon->cspeed);
  wr_short(mon->mptr);
  wr_byte(mon->fy);
  wr_byte(mon->fx);
  wr_byte(mon->cdis);
  wr_byte(mon->ml);
  wr_byte(mon->stunned);
  wr_byte(mon->confused);
}

static void rd_byte(ptr)
int8u *ptr;
{
  int8u c;

  c = getc(fileptr) & 0xFF;
  *ptr = c ^ xor_byte;
  xor_byte = c;
  DEBUG(fprintf (logfile, "BYTE:  %02X = %d\n", (int) c, (int) *ptr));
}

static void rd_short(ptr)
int16u *ptr;
{
  int8u c;
  int16u s;

  c = (getc(fileptr) & 0xFF);
  s = c ^ xor_byte;
  xor_byte = (getc(fileptr) & 0xFF);
  s |= (int16u)(c ^ xor_byte) << 8;
  *ptr = s;
  DEBUG(fprintf (logfile, "SHORT: %02X %02X = %d\n", (int) c, (int) xor_byte,\
		 (int) s));
}

static void rd_long(ptr)
int32u *ptr;
{
  register int32u l;
  register int8u c;

  c = (getc(fileptr) & 0xFF);
  l = c ^ xor_byte;
  xor_byte = (getc(fileptr) & 0xFF);
  l |= (int32u)(c ^ xor_byte) << 8;
  DEBUG(fprintf (logfile, "LONG:  %02X %02X ", (int) c, (int) xor_byte));
  c = (getc(fileptr) & 0xFF);
  l |= (int32u)(c ^ xor_byte) << 16;
  xor_byte = (getc(fileptr) & 0xFF);
  l |= (int32u)(c ^ xor_byte) << 24;
  *ptr = l;
  DEBUG(fprintf (logfile, "%02X %02X = %ld\n", (int) c, (int) xor_byte,\
		 (long) l));
}

static void rd_bytes(ch_ptr, count)
int8u *ch_ptr;
register int count;
{
  register int i;
  register int8u *ptr;
  register int8u c;

  DEBUG(fprintf (logfile, "%d BYTES:", count));
  ptr = ch_ptr;
  for (i = 0; i < count; i++)
    {
      c = (getc(fileptr) & 0xFF);
      *ptr++ = c ^ xor_byte;
      xor_byte = c;
      DEBUG(fprintf (logfile, "  %02X = %d", (int) c, (int) ptr[-1]));
    }
  DEBUG(fprintf (logfile, "\n"));
}

static void rd_string(str)
char *str;
{
  register int8u c;

  DEBUG(char *s = str);
  DEBUG(fprintf (logfile, "STRING: "));
  do
    {
      c = (getc(fileptr) & 0xFF);
      *str = c ^ xor_byte;
      xor_byte = c;
      DEBUG(fprintf (logfile, "%02X ", (int) c));
    }
  while (*str++ != '\0');
  DEBUG(fprintf (logfile, "= \"%s\"\n", s));
}

static void rd_shorts(ptr, count)
int16u *ptr;
register int count;
{
  register int i;
  register int16u *sptr;
  register int16u s;
  int8u c;

  DEBUG(fprintf (logfile, "%d SHORTS:", count));
  sptr = ptr;
  for (i = 0; i < count; i++)
    {
      c = (getc(fileptr) & 0xFF);
      s = c ^ xor_byte;
      xor_byte = (getc(fileptr) & 0xFF);
      s |= (int16u)(c ^ xor_byte) << 8;
      *sptr++ = s;
      DEBUG(fprintf (logfile, "  %02X %02X = %d", (int) c, (int) xor_byte,\
		     (int) s));
    }
  DEBUG(fprintf (logfile, "\n"));
}

static void rd_item(item)
register inven_type *item;
{
  DEBUG(fprintf (logfile, "ITEM:\n"));
  rd_short(&item->index);
  rd_byte(&item->name2);
  rd_string(item->inscrip);
  rd_long(&item->flags);
  rd_byte(&item->tval);
  rd_byte(&item->tchar);
  rd_short((int16u *)&item->p1);
  rd_long((int32u *)&item->cost);
  rd_byte(&item->subval);
  rd_byte(&item->number);
  rd_short(&item->weight);
  rd_short((int16u *)&item->tohit);
  rd_short((int16u *)&item->todam);
  rd_short((int16u *)&item->ac);
  rd_short((int16u *)&item->toac);
  rd_bytes(item->damage, 2);
  rd_byte(&item->level);
  rd_byte(&item->ident);
}

static void rd_monster(mon)
register monster_type *mon;
{
  DEBUG(fprintf (logfile, "MONSTER:\n"));
  rd_short((int16u *)&mon->hp);
  rd_short((int16u *)&mon->csleep);
  rd_short((int16u *)&mon->cspeed);
  rd_short(&mon->mptr);
  rd_byte(&mon->fy);
  rd_byte(&mon->fx);
  rd_byte(&mon->cdis);
  rd_byte(&mon->ml);
  rd_byte(&mon->stunned);
  rd_byte(&mon->confused);
}

/* functions called from death.c to implement the score file */

/* set the local fileptr to the scorefile fileptr */
void set_fileptr(file)
FILE *file;
{
  fileptr = file;
}

void wr_highscore(score)
high_scores *score;
{
  DEBUG(logfile = fopen ("IO_LOG", "a"));
  DEBUG(fprintf (logfile, "Saving score:\n"));
  /* Save the encryption byte for robustness.  */
  wr_byte(xor_byte);

  wr_long((int32u) score->points);
  wr_long((int32u) score->birth_date);
  wr_short((int16u) score->uid);
  wr_short((int16u) score->mhp);
  wr_short((int16u) score->chp);
  wr_byte(score->dun_level);
  wr_byte(score->lev);
  wr_byte(score->max_dlv);
  wr_byte(score->sex);
  wr_byte(score->race);
  wr_byte(score->class);
  wr_bytes((int8u *)score->name, PLAYER_NAME_SIZE);
  wr_bytes((int8u *)score->died_from, 25);
  DEBUG(fclose (logfile));
}

void rd_highscore(score)
high_scores *score;
{
  DEBUG(logfile = fopen ("IO_LOG", "a"));
  DEBUG(fprintf (logfile, "Reading score:\n"));
  /* Read the encryption byte.  */
  rd_byte (&xor_byte);

  rd_long((int32u *)&score->points);
  rd_long((int32u *)&score->birth_date);
  rd_short((int16u *)&score->uid);
  rd_short((int16u *)&score->mhp);
  rd_short((int16u *)&score->chp);
  rd_byte(&score->dun_level);
  rd_byte(&score->lev);
  rd_byte(&score->max_dlv);
  rd_byte(&score->sex);
  rd_byte(&score->race);
  rd_byte(&score->class);
  rd_bytes((int8u *)score->name, PLAYER_NAME_SIZE);
  rd_bytes((int8u *)score->died_from, 25);
  DEBUG(fclose (logfile));
}
