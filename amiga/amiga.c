/* amiga/amiga.c: Amiga support code

   Copyright (c) 1990-94 Corey Gehman, James E. Wilson
   SAS/C changes placed in the Public Domain by Ronald Cook.

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
