
#include <stdio.h>

#include "constant.h"
#include "config.h"
#include "types.h"
#include "externs.h"

#if defined(GEMDOS) && (__STDC__ == 0)
#include <time.h>
#include <osbind.h>
#include <bios.h>
#include "curses.h"
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
#endif
