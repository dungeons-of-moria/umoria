/* util/scores/delete.c: standalone program to delete record from scorefile

   Copyright (c) 1991 James E. Wilson

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include <stdio.h>

#include "../../source/config.h"
#include "../../source/constant.h"
#include "../../source/types.h"
#include "../../source/externs.h"

#if defined(USG) || defined(VMS)
#ifndef L_SET
#define L_SET 0
#endif
#ifndef L_INCR
#define L_INCR 1
#endif
#endif

#undef fopen

#ifndef USG
/* only needed for Berkeley UNIX */
#include <sys/param.h>
#include <sys/types.h>
#include <sys/file.h>
#endif

extern race_type race[MAX_RACES];
extern class_type class[MAX_CLASS];

FILE *highscore_fp;
FILE *fileptr;
int8u xor_byte;

void set_fileptr();

main(argc, argv)
     int argc;
     char *argv[];
{
  register int i, rank;
  high_scores score;
  int8u version_maj, version_min, patch_level;
  int delete_number;

  if (argc != 3)
    {
      printf ("Usage: delete scorefile index > newscore\n");
      exit (-2);
    }

  if ((highscore_fp = fopen (argv[1], "r")) == NULL)
    {
      printf ("Error opening score file \"%s\"\n", MORIA_TOP);
      exit (-1);
    }

  if ((delete_number = atoi (argv[2])) <= 0)
    {
      printf ("Index must be a positive number.\n");
      printf ("Usage: delete scorefile index\n");
      exit (-2);
    }

#ifdef MSDOS
  (void) setmode (fileno(highscore_fp), O_BINARY);
#endif

#ifndef BSD4_3
  (void) fseek(highscore_fp, (long)0, L_SET);
#else
  (void) fseek(highscore_fp, (off_t)0, L_SET);
#endif

  /* Read version numbers from the score file, and check for validity.  */
  version_maj = getc (highscore_fp);
  version_min = getc (highscore_fp);
  patch_level = getc (highscore_fp);
  /* Support score files from 5.2.2 to present.  */
  if (feof (highscore_fp))
    {
      printf ("The scorefile is empty.\n");
      exit (-1);
    }
  else if ((version_maj != CUR_VERSION_MAJ)
      || (version_min > CUR_VERSION_MIN)
      || (version_min == CUR_VERSION_MIN && patch_level > PATCH_LEVEL)
      || (version_min == 2 && patch_level < 2)
      || (version_min < 2))
    {
      printf("Sorry. This scorefile is from a different version of umoria.\n");
      exit (-1);
    }

  (void) putc (version_maj, stdout);
  (void) putc (version_min, stdout);
  (void) putc (patch_level, stdout);

  /* set the static fileptr in save.c to the highscore file pointer */
  set_fileptr(highscore_fp);

  rank = 1;
  rd_highscore(&score);
  while (!feof(highscore_fp))
    {
      i = 1;
      /* Put twenty scores on each page, on lines 2 through 21. */
      while (!feof(highscore_fp) && i < 21)
	{
	  if (rank != delete_number)
	    {
	      set_fileptr (stdout);
	      wr_highscore (&score);
	      set_fileptr (highscore_fp);
	    }
	  rank++;
	  rd_highscore(&score);
	}
    }

  /* Success.  */
  exit (0);
}

static void wr_byte(c)
int8u c;
{
  xor_byte ^= c;
  (void) putc((int)xor_byte, fileptr);
}

static void wr_short(s)
int16u s;
{
  xor_byte ^= (s & 0xFF);
  (void) putc((int)xor_byte, fileptr);
  xor_byte ^= ((s >> 8) & 0xFF);
  (void) putc((int)xor_byte, fileptr);
}

static void wr_long(l)
register int32u l;
{
  xor_byte ^= (l & 0xFF);
  (void) putc((int)xor_byte, fileptr);
  xor_byte ^= ((l >> 8) & 0xFF);
  (void) putc((int)xor_byte, fileptr);
  xor_byte ^= ((l >> 16) & 0xFF);
  (void) putc((int)xor_byte, fileptr);
  xor_byte ^= ((l >> 24) & 0xFF);
  (void) putc((int)xor_byte, fileptr);
}

static void wr_bytes(c, count)
int8u *c;
register int count;
{
  register int i;
  register int8u *ptr;

  ptr = c;
  for (i = 0; i < count; i++)
    {
      xor_byte ^= *ptr++;
      (void) putc((int)xor_byte, fileptr);
    }
}

static void rd_byte(ptr)
int8u *ptr;
{
  int8u c;

  c = getc(fileptr) & 0xFF;
  *ptr = c ^ xor_byte;
  xor_byte = c;
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
  c = (getc(fileptr) & 0xFF);
  l |= (int32u)(c ^ xor_byte) << 16;
  xor_byte = (getc(fileptr) & 0xFF);
  l |= (int32u)(c ^ xor_byte) << 24;
  *ptr = l;
}

static void rd_bytes(ch_ptr, count)
int8u *ch_ptr;
register int count;
{
  register int i;
  register int8u *ptr;
  register int8u c;

  ptr = ch_ptr;
  for (i = 0; i < count; i++)
    {
      c = (getc(fileptr) & 0xFF);
      *ptr++ = c ^ xor_byte;
      xor_byte = c;
    }
}

/* set the local fileptr to the scorefile fileptr */
void set_fileptr(file)
FILE *file;
{
  fileptr = file;
}

void wr_highscore(score)
high_scores *score;
{
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
}

void rd_highscore(score)
high_scores *score;
{
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
}
