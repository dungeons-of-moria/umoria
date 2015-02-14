/* atari_st/st-stuff.c: Atari ST support code for GCC

   Copyright (c) 1990-94 Scott Kolodzieski, James E. Wilson

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
#include <time.h>

#include <osbind.h>
#include <curses.h>

/* this routine looks for an enviornment variable called MORIA_ROOT
   which may contain a sting specifing the base directory for the moria
   MORIA_ROOT/files subdirectory. this is more usefull on a version of
   unix-moria that runs on many personal computers, allowing the program
   run from any directory, no matter where it is based. also it allows
   the files directory to be anywhere on the system...


                                 scott kolodzieski
*/
char *prefix_file(name)
char *name;
{ char p_buff[80];
  char *p;
  p = (char *)p_buff;
  p = (char *)getenv("MORIA_ROOT");
  if (p == (char *)NULL)
    strcpy(extended_file_name,name);
  else
    { strcpy(extended_file_name,p);
      strcat(extended_file_name,name);
    }
   return(extended_file_name);
}


#if defined(atarist) && defined(__GNUC__) && defined(BAMMI_LIB)
int sigsetmask(x)
int x;
{ return(1);
}
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
