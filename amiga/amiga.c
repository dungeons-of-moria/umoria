/* amiga/amiga.c: Amiga support code

   Copyright (c) 1990-94 Corey Gehman, James E. Wilson
   SAS/C changes placed in the Public Domain by Ronald Cook.

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include <curses.h>

#ifdef LATTICE

\/***************************************************************************
 * curses function overwrite() - not present in Simon J. Raybould's curses
 * library, so write a dumb version capable of doing what we need.
 *
 * This version is based on what Niilo Paasivirta (paasivir@jyu.fi) sent
 * to me when I couldn't get this working. Turns out his is a lot SIMPLER
 * than my version - he leaves some things alone which I copied. Oh well.
 * - RC
 *
 * BUGS: Assumes that lines in both src & dst are COLS lines long & LINES
 *          lines high.
 *       Assumes that ATTRS is smaller than a char.
 */
int
overwrite(WINDOW *src, WINDOW *dst)
{
    int     i;

    /* Allocate new memory for the destination window. */
    for (i=0; i<LINES; i++) {
        dst->LnArry[i].Touched = TRUE; /* Not needed? */
        memcpy(dst->LnArry[i].Line, src->LnArry[i].Line, COLS);
        memcpy((char *)(dst->LnArry[i].ATTRS), (char *)(src->LnArry[i].ATTRS), COLS);
        dst->LnArry[i].StartCol = src->LnArry[i].StartCol;
        dst->LnArry[i].EndCol = src->LnArry[i].EndCol;
    }
    return 0;
}
#endif

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
