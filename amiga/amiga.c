/* amiga/amiga.c: Amiga support code

   Copyright (c) 1990-92 Corey Gehman, James E. Wilson

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include <curses.h>

sleep(secs)
int secs;
{
   sendtimer(secs*1000000);
   waittimer();
}

check_input(microsecs)
int microsecs;
{
  int ch;

  sendtimer(microsecs);
  waittimer();

  nodelay(stdscr, TRUE);
  ch = getch ();
  nodelay(stdscr, FALSE);

  if (ch == -1)
    return 0;

  return 1;
}
