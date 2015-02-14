/* atari_st/atarist.c: Atari ST support code for MWC or TC

   Copyright (c) 1990-94 Stephen A. Jacobs, James E. Wilson

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

#include <stdio.h>
#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"

#if defined(GEMDOS) && (__STDC__ == 0) || defined(ATARIST_TC)
#include <time.h>
#ifdef ATARIST_TC
#include <tos.h>
#else	/* ATARIST_MWC */
#include <osbind.h>
#include <bios.h>
#endif

#include "curses.h"

#ifdef ATARIST_TC
/* Include this to get prototypes for standard library functions.  */
#include <stdlib.h>
#endif

/* check_input does a non blocking read (consuming the input if any) and
   returns 1 if there was input pending */
int check_input(microsec)
int microsec;
{
	time_t start;

	if(microsec != 0 && (turn & (unsigned long)0x3f) == 0){
		start = clock();
		while ((clock() <= (start + 100)));/*	half second pause */
	}
	if (Bconstat(2) != 0L)
	  {
	    (void) getch();
	    return 1;
	  }
	else
	  return 0;
}

void user_name(buf)
char *buf;
{
	extern char *getenv();
	register char *p;

	if(p=getenv("NAME")) strcpy(buf, p);
	else if(p=getenv("USER")) strcpy(buf, p);
	else strcpy(buf, "X");
}

#ifdef ATARIST_TC
int access(name, dum)
char *name;
int dum;
{
  int fd;

  fd = open(name, O_RDWR);
  if (fd < 0)
    return(-1);
  else
    {
      close(fd);
      return(0);
    }
}

void chmod(name, mode)
char *name;
int mode;
{
/* does not exist, dummy function */
}
#endif
#endif
