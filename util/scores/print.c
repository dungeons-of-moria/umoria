/* util/scores/print.c: standalone program to print score file

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
  char string[100];
  int8u version_maj, version_min, patch_level;
  int16 player_uid;

  if (argc != 2)
    {
      printf ("Usage: print scorefile\n");
      exit (-2);
    }

  if ((highscore_fp = fopen (argv[1], "r")) == NULL)
    {
      printf ("Error opening score file \"%s\"\n", MORIA_TOP);
      exit (-1);
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

#ifdef unix
  player_uid = getuid ();
#else
#ifdef VMS
  player_uid = (getgid()*1000) + getuid();
#else
  player_uid = 0;
#endif
#endif

  /* set the static fileptr in save.c to the highscore file pointer */
  set_fileptr(highscore_fp);

  rank = 1;
  rd_highscore(&score);
  printf("Rank  Points Name              Sex Race       Class  Lvl Killed By\n");
  while (!feof(highscore_fp))
    {
      i = 1;
      /* Put twenty scores on each page, on lines 2 through 21. */
      while (!feof(highscore_fp) && i < 21)
	{
	  (void) sprintf(string,
			 "%-4d%8ld %-19.19s %c %-10.10s %-7.7s%3d %-22.22s\n",
			 rank, score.points, score.name, score.sex,
			 race[score.race].trace, class[score.class].title,
			 score.lev, score.died_from);
	  printf (string, ++i);
	  rank++;
	  rd_highscore(&score);
	}
    }

  /* Success.  */
  exit (0);
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
