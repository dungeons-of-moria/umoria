/* This file is part of Umoria.

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

/* This file was also released as part of an independent program, and
   may be redistributed and/or modified under the authors' original terms,
   which are given below. */

/* This is essentially a shareware program, its usage is subject to
the following conditions :

PUBLIC-DOMAIN       : don't take the credit for it.
Commercial purposes : Send 5 US-dollars or equivalent currency and return
                      postage to me to get a written consent to use it.

CURSES may be distributed to anyone, provided that the various
copyright notices remain intact.

Bug reports and suggestions are welcome, send me e-mail.

Rene van't Veen                  (...!mcvax!nikhefh!u13)
Debussystraat 27
1817 GL Alkmaar
The Netherlands

Extensive modifications for clarity and speed by :
Hildo Biersma
Tollensstraat 138
2513 GE Den Haag
Holland
The copyright remains with the original author */

/*************************************************************************/
/* curses: simple-minded implementation of the curses package            */
/* Introduction                                                          */
/*                                                                       */
/* This source and resulting object may be modified,used or distributed  */
/* by anyone who so desires under the following conditions :             */
/*                                                                       */
/*  1) This notice and the copyright notice shall not be removed or      */
/*     changed.                                                          */
/*  2) No credit shall be taken for this source and resulting objects    */
/*  3) This source or resulting objects is not to be traded, sold or     */
/*     used for personal gain or profit.                                 */
/*  4) Inclusion of this source or resulting objects in commercially     */
/*     available packages is forbidden without written consent of the    */
/*     author of this source.                                            */
/*                                                                       */
/*************************************************************************/

/**************************************************************************/
/*                                                                        */
/*  Some notes on the WINDOW data structure and the meaning of the flags. */
/*                                                                        */
/*  struct _win_st                                                        */
/*  {                                                                     */
/*    int16 _cury, _curx; : current cursor positions for this screen      */
/*    int16 _maxy, _maxx; : maximum x and y positions for this screen     */
/*    int16 _begy, _begx; : first x and y positions for this screen       */
/*    int16 _flags;       : a collection of flags for this screen         */
/*    bool  _clear;       : boolean - clear screen first at next update   */
/*    bool  _leave;       : boolean - leave cursor after printing a char  */
/*    bool  _scroll;      : boolean - scrolling allowed in this screen    */
/*    int16 **_y;         : pointer to the screen data (see below)        */
/*    int16 *_firstch;    : pointer to the first non-updated char         */
/*    int16 *_lastch;     : pointer to the last non-updated char          */
/*    int16 *_yend;       : pointer to the last char of _y                */
/*  };                                                                    */
/*                                                                        */
/*  The screen data is an array of 16-bit words. The lower 8 bits is the  */
/*  character; the next two bits, TOUCHED : 0x0200 and STANDOUT : 0x0100  */
/*  indicate whether this character is changed since the last screen and  */
/*  whether it should be printed in reverse. Note that, since curscr is   */
/*  a copy of the current screen, the TOUCHED flags have no meaning; they */
/*  may be set in a routine to keep it general. For the same reason, the  */
/*  _firstch and _lastch pointers may be set but have no real meaning.    */
/*  The _y pointer points to an array of pointers to words. Thus, _y[1]   */
/*  is a pointer to line number 1. This would indicate a practical way of */
/*  scrolling or erasing lines : just swap some pointers and don't copy   */
/*  the contents when unnecessary. However, this wreaks havoc with the    */
/*  _firstch and _lastch pointers; therefore this method is not used.     */
/*                                                                        */
/*  A subwindow is a screen that is a part of another screen and shares   */
/*  the character data with it. Any change to the window data is a change */
/*  to the subwindow and vice versa. The existence of subwindows is the   */
/*  reason for the data structure of _y described above; a subwindow only */
/*  has an array of pointers that point into the character data of the    */
/*  screen it is a subwindow of.                                          */
/*                                                                        */
/*  The _flags bit of the window may contain the following flags :        */
/*   _SUBWIN    : 01 - is this a suwindow                                 */
/*   _ENDLINE   : 02 - end of line of window is also end of screen        */
/*   _FULLWIN   : 04 - is this screen the full screen size                */
/*   _SCROLLWIN : 010 - may this window be scrolled                       */
/*   _STANDOUT  : 0200 - is reverse printing currently on for this window */
/*                                                                        */
/*  The _yend pointer in w is my own addition to this data structure. As  */
/*  a much-used operation was ptr = &(w->_y[w->_maxy][w->_maxx]), it was  */
/*  faster to do this at the creation of the window, and use it after.    */
/*  It is also useful as a sentinel, to see when a pointer goes beyond    */
/*  the character data.                                                   */
/*                                                                        */
/* I hope this makes all of the code somewhat more clear, especially the  */
/* setting or not setting of TOUCHED on curscr and the use of pointers.   */
/* Hildo Biersma (adress above) - please write if you have any problems.  */
/**************************************************************************/


/* include files */
#ifdef ATARIST_MWC
#include <osbind.h>	/* MWC */
#else
#include <tos.h>	/* TC */
#endif
#include <ext.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "cursinc.h"
/* end of include files */

/* terminal states : cooked, cbreak and raw */
/* Note that CBREAK mode has been disabled - Hildo Biersma */
#define COOKED 0
#define CBREAK 1
#define RAW    2

/* globals : definition of global variables */
/* the following global variables are defined as external in curses.h */
WINDOW      *curscr;
WINDOW      *stdscr;
char        *Def_term = "AtariST";
bool        My_term   = FALSE;
const char  *ttytype  = "Atari ST-series computer, VT52/CURSES mode";
int         LINES     = 25;
int         COLS      = 80;
/* end of global and externally available variables */

bool _doecho  = FALSE;
bool _nlmap   = FALSE;
bool _curini  = FALSE;
bool _modinp  = 0; /* set input to Un*x compatability, i.e only char's */
bool _csry    = 0;
bool _csrx    = 0;
/* end of global variable definitions */

/* initscr : initialize the curses package */
WINDOW *initscr()
{
  if (_curini)
  /* Initialized before */
  {
    wrefresh(curscr);
    return(NULL);
  };
  _curini = TRUE;
  curscr = newwin(0,0,0,0);
  if (curscr == NULL)
    return(NULL);
  stdscr = newwin(0,0,0,0);
  if (stdscr == NULL)
    return(NULL);
  /* Clear screen and home cursor */
  Bconout(2,'\033');
  Bconout(2,'E');
  /* Set 'discard EOL mode' : CR + LF needed for new line */
  Bconout(2,'\033');
  Bconout(2,'w');
  /* Hide cursor */
  Bconout(2,'\033');
  Bconout(2,'f');
  _csry = 0;
  _csrx = 0;
  return(stdscr);
}
/* end of initscr */


/* endwin : end of curses package */
/* Does not seem to be used by Moria */
void endwin()
{
  if (!_curini)
    return;
  _curini = 0;
  delwin(stdscr);
  delwin(curscr);
  _movcur(LINES,0);
  /* Show cursor */
  Bconout(2,'\033');
  Bconout(2,'e');
}
/* end of endwin */


/* newwin : create a new window for the user */
/* l : lines; c : columns; by : begin-y; bx : begin-x */
WINDOW *newwin(l,c,by,bx)
int l,c,bx,by;
{
  register WINDOW *tmp;
  int16 i, nl, nc;
  register int16 *tmpptr1,*tmpptr2; /* temp pointers for loops */
  
  tmp = (WINDOW *) malloc(sizeof(WINDOW));
  if (tmp == NULL)
    return(NULL);
  tmp->_curx = 0;
  tmp->_cury = 0;
  tmp->_clear = FALSE;
  tmp->_leave = FALSE;
  tmp->_scroll = FALSE;
  if (l == 0)
    nl = LINES - by;
  else
    nl = l;
  if (c == 0)
    nc = COLS - bx;
  else
    nc = c;
  if (nl < 1 || nl > LINES || nc < 1 || nc > COLS)
  {
    free(tmp);
    return(NULL);
  };
  tmp->_maxy = nl - 1;
  tmp->_maxx = nc - 1;
  if (nl == LINES && nc == COLS)
    tmp->_flags = _FULLWIN;
  else
    tmp->_flags = 0;
  if (by < 0 || by >= LINES || bx < 0 || bx >= COLS)
  {
    free(tmp);
    return(NULL);
  };
  tmp->_begy = by;
  tmp->_begx = bx;
  /* Allocate an array of pointers to the line-arrays */
  tmp->_y = (int16 **) malloc(sizeof(int16 *) * (tmp->_maxy+1));
  if (tmp->_y == NULL)
  {
    free(tmp);
    return(NULL);
  };
  /* Allocate a block of memory to fit the whole screen */
  tmp->_y[0] = (int16 *)malloc(sizeof(int16) * (tmp->_maxy+1)
			       * (tmp->_maxx+1));
  if (tmp->_y[0] == NULL)
  {
    free(tmp->_y);
    free(tmp);
    return(NULL);
  };
  for (i = 1; i <= tmp->_maxy; i++)
    tmp->_y[i] = tmp->_y[0] + i * (tmp->_maxx + 1);
  tmpptr1 = tmp->_y[0];
  tmp->_firstch = tmpptr1;
  tmpptr2 = &(tmp->_y[tmp->_maxy][tmp->_maxx]);
  tmp->_yend = tmpptr2;
  tmp->_lastch = tmpptr2;
  for (; tmpptr1 <= tmpptr2; tmpptr1++)
      *tmpptr1 = (' ' | TOUCHED);
  return(tmp);
} /* end of newwin */


/* delwin : delete a window */
/* Does not seem to be used by Moria */
void delwin(w)
WINDOW *w;
{
  if (w == NULL)
    return;
  if (w->_flags & _SUBWIN)
    free(w->_y);
  else
  {
    free(w->_y[0]);
    free(w->_y);
  };
  free(w);
} /* end of delwin */


/* mvwin : move window */
/* Does not seem to be used by Moria */
int mvwin(w,y,x)
register WINDOW *w;
int y,x;
{
  if (y < 0 || x < 0 || (y + w->_maxy + 1) > LINES || (x + w->_maxx + 1) > COLS)
    return(ERR);
  w->_begy = y;
  w->_begx = x;
  touchwin(w);
  return(OK);
} /* end of mvwin */


/* touchwin : touch a window */
void touchwin(w)
WINDOW *w;
{
  register int16 j, **tmpptr1,*tmpptr2; /* temporary pointers for loops */

  w->_firstch = w->_y[0];
  w->_lastch = w->_yend;
  for (tmpptr1 = w->_y; *tmpptr1 <= w->_yend; tmpptr1++)
    for (j = 0,tmpptr2 = *tmpptr1; j <= w->_maxx; j++,tmpptr2++)
      *tmpptr2 |= TOUCHED;
} /* end of touchwin */


/* subwin : create a sub-window */
/* Does not seem to be used by Moria */
WINDOW *subwin(w,l,c,by,bx)
WINDOW *w;
int l,c,by,bx;
{
  WINDOW *tmp;
  int16  i, nl, nc;

  /* cannot take subwindows of curscr */
  if (w == curscr)
    return(NULL);
  tmp = (WINDOW *) malloc(sizeof(WINDOW));
  if (tmp == NULL)
    return(NULL);
  tmp->_curx = 0;
  tmp->_cury = 0;
  tmp->_clear = FALSE;
  tmp->_leave = FALSE;
  tmp->_scroll = FALSE;
  if (l == 0)
    nl = LINES - by;
  else
    nl = l;
  if (c == 0)
    nc = COLS - bx;
  else
    nc = c;
  if (l < 1 || l > (w->_maxy+1) || c < 1 || c > (w->_maxx+1))
  {
    free(tmp);
    return(NULL);
  };
  tmp->_maxy = nl - 1;
  tmp->_maxx = nc - 1;
  if (nl == LINES && nc == COLS)
    tmp->_flags = _FULLWIN | _SUBWIN;
  else
    tmp->_flags = _SUBWIN;
  if (by < w->_begy || by >= (w->_maxy+w->_begy) ||
      bx < w->_begx || bx >= (w->_maxx+w->_begx))
  {
    free(tmp);
    return(NULL);
  };
  tmp->_begy = by;
  tmp->_begx = bx;
  tmp->_y = (int16**) malloc(sizeof(int16*) * (tmp->_maxy+1));
  if (tmp->_y == NULL)
  {
    free(tmp);
    return(NULL);
  };
  tmp->_y[0] = w->_y[0] + (tmp->_begy-w->_begy) * (w->_maxx+2) + 
               (tmp->_maxx - w->_maxx);
  for (i = 1; i <= tmp->_maxy; i++)
    tmp->_y[i] = tmp->_y[0] + i * (w->_maxx + 2);
  tmp->_yend = &(tmp->_y[w->_maxy][w->_maxx]);
  /* make everything changed on first update of tmp       */
  touchwin(tmp);
  return(tmp);
} /* end of subwin */


/* leaveok : tell curses where to leave the cursor after updating */
/* Does not seem to be used by Moria */
void leaveok(w,f)
WINDOW *w;
bool f;
{
  w->_leave = f;
} /* end of leaveok */


/* scrollok : tell curses it is ok to scroll the window */
/* Does not seem to be used by Moria */
void scrollok(w,f)
WINDOW *w;
bool f;
{
  w->_scroll = f;
} /* end of scrollok */


/* nl : tell curses to map CR to CR,LF */
/* Does not seem to be used by Moria */
void nl()
{
  /* Set 'Wrap at EOL' mode */
  Bconout(2,'\033');
  Bconout(2,'v');
  _nlmap = 1;
} /* end of nl */


/* nonl : tell curses not to map CR to CR,LF */
/* Does not seem to be used by Moria */
void nonl()
{
  /* Set 'Discard EOL' mode */
  Bconout(2,'\033');
  Bconout(2,'w');
  _nlmap = 0;
} /* end of nonl */


/* longname : return the full name of the terminal */
/* always returns the contents of ttytype in name */
/* Does not seem to be used by Moria */
void longname(termbuf,name)
char *termbuf, *name;
{
  strcpy(name,ttytype);
} /* end of longname */


/* curaddch : add a character to the current screen */
int curaddch(c)
char c;
{
  int16 i, tpos, *tmpptr;

  switch(c)
  {
    case '\n' :
      wclrtoeol(curscr);
      if (_nlmap)
        wmove(curscr,curscr->_cury+1,0);
      else
        wmove(curscr,curscr->_cury + 1,curscr->_curx);
      break;
    case '\r' :
      wmove(curscr,curscr->_cury,0);
      break;
    case '\t' :
      tpos = (curscr->_curx + curscr->_begx) % 8;
      if ((curscr->_flags & _STANDOUT))
      {
        /* reverse on */
        Bconout(2,'\033');
        Bconout(2,'p');
      }
      for (i = 0; i < (8 - tpos); i++)
        Bconout(5,' ');
      if ((curscr->_flags & _STANDOUT))
      {
        /* reverse off */
        Bconout(2,'\033');
        Bconout(2,'q');
      }
      break;
    default :
      tmpptr = &(curscr->_y[curscr->_cury][curscr->_curx]);
      if ((curscr->_flags & _STANDOUT))
      {
        *tmpptr = c | STANDOUT;
        /* reverse on */
        Bconout(2,'\033');
        Bconout(2,'p');
        Bconout(5,c);
        /* reverse off */
        Bconout(2,'\033');
        Bconout(2,'q');
      }
      else
      {
        *tmpptr = c;
        Bconout(5,c);
      };
      _csrx++;
  } /* end of switch */
  return(OK);
} /* end of curaddch */


/* waddch : add a character to a window */
int waddch(w,c)
register WINDOW *w;
char   c;
{
  int16 i, tpos, *tmpptr;

  if (w == curscr)
    curaddch(c);
  else
  {
    switch(c)
    {
      case '\n' :
        wclrtoeol(w);
        if (_nlmap)
          wmove(w,w->_cury+1,0);
        else
          wmove(w,w->_cury+1,w->_curx);
        break;
      case '\r' :
        wmove(w,w->_cury,0);
        break;
      case '\t' :
        tpos = (w->_curx + w->_begx) % 8;
        for (i = 0; i < (8 - tpos); i++)
          waddch(w,' ');
        break;
      default :
        tmpptr = &(w->_y[w->_cury][w->_curx]);
        if ((w->_flags & _STANDOUT))
          *tmpptr = c | (STANDOUT | TOUCHED);
        else
          *tmpptr = c | TOUCHED;
        if (w->_firstch == NULL)
        {
          w->_firstch = tmpptr;
          w->_lastch = tmpptr;
        }
        else
        {
          if (w->_firstch > tmpptr)
            w->_firstch = tmpptr;
          else
          {
            if (w->_lastch < tmpptr)
              w->_lastch = tmpptr;
          };
        };
        wmove(w,w->_cury,w->_curx+1);
    } /* end of switch */
  } /* end of if (w == curscr) */
  return(OK);
} /* end of waddch */


/* waddstr : add a string of characters to a window */
int waddstr(w,s)
register WINDOW *w;
register char   *s;
{
  if (w == curscr)
    while (*s != '\0')
      curaddch(*s++);
  else
    while (*s != '\0')
      waddch(w,*s++);
  return(OK);
} /* end of waddstr */


/* box : draw a box around a window */
/* w : window nr; v : vertical char; h : horizontal char */
/* Does not seem to be used by Moria */
void box(w,v,h)
WINDOW *w;
char v,h;
{
  int16 i;

  for (i = 0; i <= w->_maxy; i++)
  {
    mvwaddch(w,i,0,v);
    mvwaddch(w,i,w->_maxx,v);
  };
  for (i = 1; i < w->_maxx; i++)
  {
    mvwaddch(w,0,i,h);
    mvwaddch(w,w->_maxy,i,h);
  };
} /* end of box */


/* wclear : clear a window */
void wclear(w)
WINDOW *w;
{
  werase(w);
  clearok(w,TRUE);
  w->_curx = 0;
  w->_cury = 0;
} /* end of wclear */


/* wclrtobot : clear a window to the bottom */
void wclrtobot(w)
WINDOW *w;
{
  register int16 j, **tmpptr1, *tmpptr2; /* temporary pointers for loops */

  tmpptr1 = &(w->_y[w->_cury]);
  for (j = w->_curx, tmpptr2 = tmpptr1[w->_curx];
       j <= w->_maxx; j++, tmpptr2++)
    *tmpptr2 = ' ' | TOUCHED;
  tmpptr1++;
  for (; *tmpptr1 <= w->_yend; tmpptr1++)
    for (j = 0,tmpptr2 = *tmpptr1; j <= w->_maxx; j++, tmpptr2++)
      *tmpptr2 = ' ' | TOUCHED;
  if (w == curscr)
  {
    /* Erase to end of page */
    Bconout(2,'\033');
    Bconout(2,'J');
  }
  else
  {
    tmpptr2 = &(w->_y[w->_cury][w->_curx]);
    if (w->_firstch == NULL)
    {
      w->_firstch = tmpptr2;
      w->_lastch = w->_yend;
    }
    else
    {
      if (w->_firstch > tmpptr2)
        w->_firstch = tmpptr2;
      w->_lastch = w->_yend;
    };
  };
} /* end of wclrtobot */


/* wclrtoeol : clear a window to the end of the line */
void wclrtoeol(w)
WINDOW *w;
{
  int16 i, *tmpptr2;
  register int16 *tmpptr1; /* Temporary pointer for loops */

  for (i = w->_curx, tmpptr1 = &(w->_y[w->_cury][w->_curx]);
       i <= w->_maxx; i++, tmpptr1++)
    *tmpptr1 = ' ' | TOUCHED;
  if (w == curscr)
  {
    /* Clear to end of line */
    Bconout(2,'\033');
    Bconout(2,'K');
  }
  else
  {
    tmpptr1 = &(w->_y[w->_cury][w->_curx]);
    tmpptr2 = &(w->_y[w->_cury][w->_maxx]);
    if (w->_firstch == NULL)
    {
      w->_firstch = tmpptr1;
      w->_lastch = tmpptr2;
    }
    else
    {
      if (w->_firstch > tmpptr1)
        w->_firstch = tmpptr1;
      if (w->_lastch < tmpptr2)
        w->_lastch = tmpptr2;
    };
  };
} /* end of wclrtoeo */


/* wdelch : delete a character on a window */
/* Does not seem to be used by Moria */
int wdelch(w)
WINDOW *w;
{
  int16 *tmpptr3;
  register int16 *tmpptr1, *tmpptr2; /* temporary pointers for loops */

  tmpptr3 = &(w->_y[w->_cury][w->_maxx]);
  for (tmpptr1 = &(w->_y[w->_cury][w->_curx]), tmpptr2 = tmpptr1 + 1;
       tmpptr1 < tmpptr3;
       tmpptr1++, tmpptr2++)
    *tmpptr1 = *tmpptr2 | TOUCHED;
  *tmpptr3 = ' ' | TOUCHED;
  tmpptr1 = &(w->_y[w->_cury][w->_curx]);
  if (w == curscr)
  {
    /* Clear to end of line */
    Bconout(2,'\033');
    Bconout(2,'K');
    for (; tmpptr1 <= tmpptr3; tmpptr1++)
    {
      Bconout(5,(*tmpptr1 & 0xff));
      _csrx++;
    };
    _movcur(w->_cury,w->_curx);
  }
  else
  {
    if (w->_firstch == NULL)
    {
      w->_firstch = tmpptr1;
      w->_lastch = tmpptr3;
    }
    else
    {
      if (w->_firstch > tmpptr1)
        w->_firstch = tmpptr1;
      if (w->_lastch < tmpptr3)
        w->_lastch = tmpptr3;
    };
  };
  return(OK);
} /* end of wdelch */


/* wdeleteln : delete a line from a window */
/* Does not seem to be used by Moria */
int wdeleteln(w)
WINDOW *w;
{
  int16 i,j;
  register int16 **tmpptr1, *tmpptr2, *tmpptr3; 
  /* temporary pointers for loops */

  for (i = w->_cury + 1, tmpptr1 = &(w->_y[w->_cury]);
       i <= w->_maxy;
       i++, tmpptr1++)
    for (j = 0, tmpptr2 = *tmpptr1, tmpptr3 = *(tmpptr1 + 1);
         j <= w->_maxx;
         j++, tmpptr2++, tmpptr3++)
      *tmpptr2 = *tmpptr3 | TOUCHED;
  for (tmpptr2 = w->_y[w->_maxy]; tmpptr2 <= w->_yend; tmpptr2++)
    *tmpptr2 = ' ' | TOUCHED;
  if (w == curscr)
  {
    /* Delete line */
    Bconout(2,'\033');
    Bconout(2,'M');
    _csrx = 0;
    _movcur(w->_cury,w->_curx);
  }
  else
  {
    tmpptr2 = w->_y[w->_cury];
    if (w->_firstch == NULL)
    {
      w->_firstch = tmpptr2;
      w->_lastch = w->_yend;
    }
    else
    {
      if (w->_firstch > tmpptr2)
        w->_firstch = tmpptr2;
      w->_lastch = w->_yend;
    };
  };
  return(OK);
} /* end of wdeleteln */


/* werase : erase a window */
void werase(w)
register WINDOW *w;
{
  register int16 j, **tmpptr1, *tmpptr2;

  for (tmpptr1 = w->_y; *tmpptr1 <= w->_yend; tmpptr1++)
    for (j = 0, tmpptr2 = *tmpptr1; j <= w->_maxx; j++, tmpptr2++)
      *tmpptr2 = (' ' | TOUCHED);
  if (w == curscr)
  {
    /* Clear screen and home cursor */
    Bconout(2,'\033');
    Bconout(2,'E');
    _csry = 0;
    _csrx = 0;
    _movcur(curscr->_cury,curscr->_curx);
  }
  else
  {
    w->_firstch = w->_y[0];
    w->_lastch = w->_yend;
  };
} /* end of werase */


/* winsch : insert a character */
/* Does not seem to be used by Moria */
int winsch(w,c)
WINDOW *w;
char c;
{
  int16 *tmpptr2, *tmpptr3;
  register int16 *tmpptr1; /* temporary pointer for loops */

  tmpptr2 = &(w->_y[w->_cury][w->_curx]);
  tmpptr3 = &(w->_y[w->_cury][w->_maxx]);
  for (tmpptr1 = tmpptr3; tmpptr1 > tmpptr2; tmpptr1--)
    *tmpptr1 = *(tmpptr1 - 1) | TOUCHED;
  if (w == curscr)
  {
    /* Clear to end of line */
    Bconout(2,'\033');
    Bconout(2,'K');
    for (tmpptr1 = tmpptr2; tmpptr1 <= tmpptr3; tmpptr1++)
    {
      Bconout(5,(*tmpptr1 & 0xff));
      _csrx++;
    };
    _movcur(w->_cury,w->_curx);
  }
  else
  {
    if (w->_firstch == NULL)
    {
      w->_firstch = tmpptr2;
      w->_lastch = tmpptr3;
    }
    else
    {
      if (w->_firstch > tmpptr2)
        w->_firstch = tmpptr2;
      if (w->_lastch < tmpptr3)
        w->_lastch = tmpptr3;
    };
  };
  mvwaddch(w,w->_cury,w->_curx,c);
  return(OK);
} /* end of winsch */


/* winsertln : insert a line in a window */
/* Does not seem to be used by Moria */
void winsertln(w)
WINDOW *w;
{
  int16 i, **tmpptr1, **tmpptr2;
  register int16 j, *tmpptr3, *tmpptr4;

  for (i = w->_maxy - 1, tmpptr1 = &(w->_y[w->_maxy]), 
       tmpptr2 = &(w->_y[w->_maxy - 1]); i >= w->_cury; 
       --i, --tmpptr1, --tmpptr2)
  {
    for (j = 0, tmpptr3 = *tmpptr1, tmpptr4 = *tmpptr2;
         j <= w->_maxx; j++, tmpptr3++, tmpptr4++)
      *tmpptr3 = *tmpptr4 | TOUCHED;
  };
  for (j = 0, tmpptr3 = w->_y[w->_cury]; j <= w->_maxx; j++, tmpptr3++)
    *tmpptr3 = ' ' | TOUCHED;
  if (w == curscr)
  {
    /* Insert line */
    Bconout(2,'\033');
    Bconout(2,'L');
    _csrx = 0;
    _movcur(w->_cury,w->_curx);
  }
  else
  {
    if (w->_firstch == NULL)
      w->_firstch = w->_y[w->_cury];
    else
    {
      if (w->_firstch > w->_y[w->_cury])
        w->_firstch = w->_y[w->_cury];
    };
    w->_lastch = w->_yend;
  };
} /* end of winsertln */


/* wmove : move the cursor of the window to a location */
int wmove(w,y,x)
WINDOW *w;
int y,x;
{
  int16 i;

  if (x < 0)
  {
    w->_curx = 0;
    return(ERR);
  }
  else
    if (x > w->_maxx)
    {
      w->_curx = w->_maxx;
      return(ERR);
    }
    else
      w->_curx = x;
  if (y < 0)
    {
      w->_cury = 0;
      return(ERR);
    }
  else
    if (y > w->_maxy)
    {
      if (w->_scroll)
      {
        for (i = w->_maxy; i < y; i++)
          scroll(w);
      };
      w->_cury = w->_maxy;
      return(ERR);
    }
    else
      w->_cury = y;
  if (w == curscr)
    _movcur(y,x);
  return(OK);
} /* end of wmove */


/* overlay: overlay two windows, ie : copy all non-space chars of v     */
/* onto the corresponding char of w.                                    */
/* Does not seem to be used by Moria */
void overlay(v,w)
WINDOW *v, *w;
{
  int16 i, j;
  /* temporary pointers for loops */
  register int16 **tmpptr1, **tmpptr2, *tmpptr3, *tmpptr4;

  for (i = 0, tmpptr1 = v->_y, tmpptr2 = w->_y;
       i <= v->_maxy && i <= w->_maxy;
       i++, tmpptr1++, tmpptr2++)
    for (j = 0, tmpptr3 = *tmpptr1, tmpptr4 = *tmpptr2;
         j <= v->_maxx && j <= w->_maxx;
         j++, tmpptr3++, tmpptr4++)
      if ((*tmpptr3 & 0xff) != 0x20)
        *tmpptr4 = *tmpptr3 | TOUCHED;
  if (w == curscr)
    wrefresh(curscr);
  else
  {
    w->_firstch = w->_y[0];
    w->_lastch = w->_yend;
  };
} /* end of overlay */


/* overwrite : overwrite two windows. */
void overwrite(v,w)
WINDOW *v, *w;
{
  int16 j;
  /* temporary pointers for loops */
  register int16 **tmpptr1, **tmpptr2, *tmpptr3, *tmpptr4;

  for (tmpptr1 = w->_y, tmpptr2 = v->_y;
       *tmpptr1 <= w->_yend && *tmpptr2 <= v->_yend;
       tmpptr1++, tmpptr2++)
    for (j = 0, tmpptr3 = *tmpptr1, tmpptr4 = *tmpptr2;
         j <= w->_maxx && j <= v->_maxx;
         j++, tmpptr3++, tmpptr4++)
      *tmpptr3 = *tmpptr4 | TOUCHED;
  if (w == curscr)
    wrefresh(curscr);
  else
  {
    w->_firstch = w->_y[0];
    w->_lastch = w->_yend;
  };
} /* end of overwrite */


/* wstandout : set the standout flag for a window */
/* In the real curses, this returns FALSE or a pointer to a capability flag.
   Right here, it is a void function. */
void wstandout(w)
WINDOW *w;
{
  w->_flags |= _STANDOUT;
} /* end of wstandout */


/* wstandend : end standout mode */
/* In the real curses, this returns FALSE or a pointer to a capability flag.
   Right here, it is a void function. */
void wstandend(w)
WINDOW *w;
{
  w->_flags &= ~_STANDOUT;
} /* end of wstandend */


/* raw : set terminal in raw mode */
/* Does not seem to be used by Moria */
void raw()
{
  /*
  * raw mode means :
  * when getting a character from the keyboard, return everything
  * including keyboard shift state.
  */
  /* Note on RAW / COOKED / CBREAK : CBREAK mode has been cut out. */
  /* RAW means    : full return of getch() values.                 */
  /* COOKED means : return lower byte of getch() values only.      */
  
  _modinp = RAW;
} /* end of raw */


/* noraw : reset terminal from raw mode into cooked mode */
void noraw()
{
  /* See note above for RAW / COOKED / CBREAK */
  _modinp = COOKED;
} /* end of noraw */


/* crmode: set terminal in cbreak mode */
/* Note that CBREAK mode has been cut out - CBREAK now is the same as RAW */
/* This had been done to avoid having programs quit when pressing CTRL-C. */
void crmode()
{
  _modinp = CBREAK;
} /* end of crmode */


/* nocrmode : reset terminal from cbreak into cooked mode */
void nocrmode()
{
  _modinp = COOKED;
} /* end of nocrmode */


/* echo : set curses to echo characters on input */
void echo()
{
  _doecho = TRUE;
} /* end of echo */


/* noecho : set curses not to echo characters on input */
void noecho()
{
  _doecho = FALSE;
} /* end of noecho */


/* wgetch : get a character from the terminal */
/* WARNING : wgetch returns a 32-bit value, not a char although only */
/* the lowest 8 bits may actually be transmitted. */
/* Watch out for this bit of skulduggery : while getch is defined as */
/* wgetch in the main program, in this routine wgetch makes use of   */
/* getch. The difference between getch and wgetch is that wgetch     */
/* displays a cursor and echoes (if wished) the input character.     */
/* The cbreak mode has been cut out as I dislike being thrown out of */
/* a program if I accidentally press CTRL-C.                         */
long wgetch(w)
WINDOW *w;
{
  int32 retval;

  /* Show cursor */
  Bconout(2,'\033');
  Bconout(2,'e');
  retval = getch();
  if (_modinp == COOKED)
    retval &= 0x00ff;
  /* Hide cursor */
  Bconout(2,'\033');
  Bconout(2,'f');
  if (_doecho)
    waddch(w,(char)(0x00ff & retval));
  return(retval);
} /* end of wgetch */


/* wgetstr : get a string from the terminal */
/* I do not know whether this really works. The if's seem wrong to me - HB */
/* Does not seem to be used by Moria */
int wgetstr(w,s)
WINDOW *w;
char   *s;
{
  int16 ox, oy;
  bool reset, end;
  char c;
  int i;

  reset = FALSE;
  getyx(w,oy,ox);
  if (_modinp == COOKED && _doecho == TRUE)
  {
    reset = TRUE;
    _doecho = FALSE;
  };
  i = 0;
  for (end = FALSE; !end; i++)
  {
    switch (_modinp)
    {
      case COOKED:
        c = (char) wgetch(w);
        if (c != 0x0d && c != 0x0a && c != 0x04 && c != 0)
        {
          s[i] = '\0';
          end = TRUE;
          break;
        };
        /* receive a backspace */
        if (c == 0x08)
        {
          if (i != 0)
          {
            --i;
            s[i] = 0;
            if (reset)
            mvwaddstr(w,oy,ox,s);
          };
          break;
        };
        /* receive control U or line kill */
        if (c == 0x13)
        {
          i = 0;
          if (reset)
          wmove(w,oy,ox);
          break;
        };
        s[i] = c;
        if (reset)
          waddch(w,c);
        break;
      case CBREAK:
        c = (char) wgetch(w);
        if (c != 0x0d && c != 0x0a && c != 0x04 && c != 0)
        {
          s[i] = '\0';
          end = TRUE;
          break;
        };
        s[i] = c;
        break;
      case RAW:
        c = (char) wgetch(w);
        if (c != 0x0d && c != 0x0a && c != 0x04 && c != 0)
        {
          s[i] = '\0';
          end = TRUE;
          break;
        };
        s[i] = c;
        break;
    };
  };
  if (reset)
  _doecho = TRUE;
  return(OK);
} /* end of wgetstr */


/* cur_refresh : refresh the current screen only. This is a simple and */
/* relatively dumb routine that is called from wrefresh() only.        */
void cur_refresh()
{
  register int16 i, j, **tmpptr1, *tmpptr2;
  
  for (i = 0, tmpptr1 = curscr->_y; i < LINES; i++, tmpptr1++)
  {
    _movcur(i,0);
    for (j = 0, tmpptr2 = *tmpptr1; j < COLS; j++ ,tmpptr2++)
    {
      if ((*tmpptr2 & STANDOUT))
      {
        /* reverse on */
        Bconout(2,'\033');
        Bconout(2,'p');
        Bconout(5,(*tmpptr2 & 0xff));
        /* reverse off */
        Bconout(2,'\033');
        Bconout(2,'q');
      }
      else
        Bconout(5,(*tmpptr2 & 0xff));
      _csrx++;
    };
  };
  _movcur(curscr->_cury, curscr->_curx);
} /* end of cur_refresh */

/* wrefresh (new version) : refresh a window on the screen            */
/* This should be an int function and return ERR or OK; but as Moria  */
/* expects to see the void version, that's what it is.                */
/* Sorry if you find this routine a bit hard to understand, what with */
/* the pointer stuff and all that. If you have any problems, please   */
/* read the comments on the data structure above. wrefresh() prints   */
/* all chars marked as TOUCHED and different from the current curscr. */
/* It also removes all TOUCHED marks from the window. If you like a   */
/* good bit of horror, then look below this routine : the original    */
/* source appears below (commented out). Enjoy the original code !    */
void wrefresh(w)
register WINDOW *w;
{
           int16 **winptr1, **curptr1, curline, c;
  register int16 *winptr2, *curptr2, curpos, linelength;

  if (w == curscr)
  {
    if (w->_clear)
    {
      werase(w);
      w->_clear = FALSE;
      /* next statement is return at end of routine */
    }
    else
    {
      cur_refresh(); /* dumb routine : refresh the current screen */
      /* next statement is return at end of routine */
    }
  }
  else /* ie : w != curscr */
  {
    if (curscr->_clear)
    {
      werase(curscr);
      curscr->_clear = FALSE;
      /* Note that both curscr and w might have _clear set; so reset */
      /* w->_clear too, or it might affect the next wrefresh() call. */
      w->_clear = FALSE;
    }
    else /* ie : w != curscr and !curscr->_clear */
    {
      if (w->_clear)
      {
        /* no need to actually clear the curscr; just touch window w. This */
        /* will set w->_firstch to the beginning of w, w->_lastch to the   */
        /* end of w, and make all characters touched. This will ensure all */
        /* characters of w that differ from curscr will be printed,        */
        /* whether window w is the size of curscr or not.                  */
        touchwin(w);
        w->_clear = FALSE;
      }
    }
    /* Copy all chars from _firstch to _lastch and print them.           */
    /* curline : current line in curscr - initial value is first line    */
    /* curpos : current char in curscr - initial value is first char     */
    /* winptr1, curptr1 : pointer to current line in window, curscr      */
    /* winptr2, curptr2 : pointer to current char in window, curscr      */
    if (w->_firstch == NULL) /* ie : no changes */
    {
      curscr->_cury = w->_cury + w->_begy;
      curscr->_curx = w->_curx + w->_begx;
      _movcur(curscr->_cury, curscr->_curx);
      /* next statement is return */
    }
    else /* w->_firstch != NULL : changes have appeared */
    {
      curline = (int16) ((w->_firstch - w->_y[0]) / (w->_maxx + 1));
      winptr1 = &(w->_y[curline]);
      winptr2 = w->_firstch;
      curline += w->_begy;
      curptr1 = &(curscr->_y[curline]);
      curpos = (winptr2 - *winptr1) + w->_begx;
      /* Sorry about the parentheses below - they are necessary. */
      /* curptr2 is the address of word nr curpos in the line that */
      /* curptr1 points to - ie curptr2 = curscr->_y[curline][curpos] */
      curptr2 = &((*curptr1)[curpos]);
      linelength = w->_maxx + w->_begx;
      if (linelength > COLS)
        linelength = COLS;
      while (winptr2 <= w->_lastch)
      {
        if (*winptr2 & TOUCHED)
        {
          /* logical AND *winptr2 with 0x01ff, which is ~TOUCHED plus the */
          /* the unused bits above TOUCHED that might accidentally be set */
          /* by the processor. Would AND with ~TOUCHED were it not that I */
          /* need to AND c with 0x01ff anyway - this way saves some time. */
          c = (*winptr2 &= 0x01ff);
          if ((*curptr2 & 0x01ff) != c)
          {
            *curptr2 = c;
            _movcur(curline,curpos);
            /* Note that _movcur sets _csrx to curpos and _csry to curline */
            if ((c & STANDOUT))
            {
              /* reverse on */
              Bconout(2,'\033');
              Bconout(2,'p');
              Bconout(5,(c & 0xff));
              /* reverse off */
              Bconout(2,'\033');
              Bconout(2,'q');
            }
            else
              Bconout(5,(c & 0xff));
            /* We just printed a char, so cursor pos. has changed - also */
            /* change _csrx. (_csry still is correct.)                   */
            _csrx++;
          }; /* end of if chars are different on window and curscr */
        }; /* end of if TOUCHED char */
        /* Now update all counters for next loop of while */
        if ((++curpos) <= linelength)
        {
          /* line not yet finished */
          winptr2++;
          curptr2++;
        }
        else
        {
          /* line finished - goto next line --> update all counters */
          curpos = w->_begx;
          winptr2 = *(++winptr1);
          curptr1++;
          /* curptr2 is the address of word nr curpos in the line that */
          /* the just increased curptr1 points to. */
          /* Sorry about the parentheses - they are necessary here. */
          curptr2 = &((*curptr1)[curpos]);
          curline++;
        }
      }; /* end of while */
      w->_firstch = NULL;
      w->_lastch = NULL;
      if (w->_leave)
      {
        w->_curx = _csrx - w->_begx;
        w->_cury = _csry - w->_begy;
        curscr->_cury = _csry;
        curscr->_curx = _csrx;
      }
      else
      {
        curscr->_cury = w->_cury + w->_begy;
        curscr->_curx = w->_curx + w->_begx;
        _movcur(curscr->_cury, curscr->_curx);
      };
    }
  } /* End of else : w != curscr */
  return;
} /* end of wrefresh : new version */


/* HORROR CODE STARTS HERE - NOT FOR SENSITIVE PROGRAMMERS OR YOUNGSTERS  */
/* As promised above, the original wrefresh() code. The only changes that */
/* have been made are in the tabs and { } placement. This code does OK on */
/* just one or two changed chars, but is far too slow on a whole screen.  */
/* #[wrefresh: refresh a window on the screen */
/* commenting out starts here
void wrefresh(w)
WINDOW *w ;
{
  WORD i,j,k,l ;
  WORD c, *ptr ;

  ERR = 0 ;
  OK = 1 ;
  if ( w != curscr && curscr->_clear )
  {
    Bconout(2,'\033') ;
    Bconout(2,'E') ;
    _csry = 0 ;
    _csrx = 0 ;
    for ( i = 0 ; i < LINES ; i++ )
    {
      for ( j = 0 ; j < COLS ; j++ )
        curscr->_y[i][j] = ' ' ;
    } ;
    curscr->_clear = 0 ;
  } ;
  if ( w->_clear )
  {
    if ( ( w->_flags & _FULLWIN ) )
    {
      Bconout(2,'\033') ;
      Bconout(2,'E') ;
      _csry = 0 ;
      _csrx = 0 ;
      for ( i = 0 ; i < LINES ; i++ )
      {
        for ( j = 0 ; j < COLS ; j++ )
          curscr->_y[i][j] = ' ' ;
      } ;
    } ;
    w->_firstch = w->_y[0] ;
    w->_lastch = &(w->_y[w->_maxy][w->_maxx]) ;
    w->_clear = 0 ;
  } ;
  if ( w != curscr )
  {
    if ( w->_firstch != 0 )
    {
      if ( w->_flags & _SUBWIN )
      {
        for ( i = 0 ; i <= w->_maxy ; i++ )
        {
          ptr = w->_y[i] ;
          if ( ptr >= w->_firstch && ptr <= w->_lastch )
          {
            for ( j = 0 ; j <= w->_maxx ; j++ )
            {
              c = ptr[j] ;
              k = i + w->_begy ;
              l = j + w->_begx ;
              if ( ( c & TOUCHED ) && ( k >= 0 && k < LINES && l >= 0 
                    && l < COLS ) )
              {
                ptr[j] = c & ~TOUCHED ;
                if ( ( curscr->_y[k][l] & 0x01ff ) != ( c & 0x01ff ) )
                {
                  curscr->_y[k][l] = c ;
                  _movcur(k,l) ;
                  if ( ( c & STANDOUT ) )
                  {
                    Bconout(2,'\033') ;
                    Bconout(2,'p') ;
                    Bconout(5,(c & 0xff)) ;
                    Bconout(2,'\033') ;
                    Bconout(2,'q') ;
                  }
                  else
                  {
                    Bconout(5,( c & 0xff )) ;
                  } ;
                  _csry = k ;
                  _csrx = l + 1 ;
                } ;
              } ;
            } ;
          } ;
        } ;
      }
      else
      {
        for ( ptr = w->_firstch ; ptr <= w->_lastch ; ptr++ )
        {
          c = *ptr ;
          if ( c & TOUCHED )
          {
            k = ( WORD ) ( ptr - w->_y[0] ) ;
            k = k / (  w->_maxx + 1 ) ;
            l = ( WORD ) ( ptr - w->_y[k] ) + w->_begx ;
            k = k + w->_begy ;
            if ( k >= 0 && k < LINES && l >= 0 && l < COLS )
            {
              *ptr = c & ~TOUCHED ;
              if ( ( curscr->_y[k][l] & 0x01ff ) != ( c & 0x01ff ) )
              {
                curscr->_y[k][l] = c ;
                _movcur(k,l) ;
                if ( ( c & STANDOUT ) )
                {
                  Bconout(2,'\033') ;
                  Bconout(2,'p') ;
                  Bconout(5,(c & 0xff)) ;
                  Bconout(2,'\033') ;
                  Bconout(2,'q') ;
                }
                else
                {
                  Bconout(5,( c & 0xff )) ;
                } ;
                _csry = k ;
                _csrx = l + 1 ;
              } ;
            } ;
          } ;
        } ;
      } ;
      w->_firstch = 0 ;
      w->_lastch = 0 ;
      if ( w->_leave )
      {
        w->_curx = _csrx - w->_begx ;
        w->_cury = _csry - w->_begy ;
        curscr->_cury = _csry ;
        curscr->_curx = _csrx ;
      }
      else
      {
        curscr->_cury = w->_cury + w->_begy ;
        curscr->_curx = w->_curx + w->_begx ;
        _movcur(curscr->_cury, curscr->_curx) ;
      } ;
    }
    else
    {
      curscr->_cury = w->_cury + w->_begy ;
      curscr->_curx = w->_curx + w->_begx ;
      _movcur(curscr->_cury, curscr->_curx) ;
    } ;
  }
  else
  {
    Bconout(2,'\033') ;
    Bconout(2,'H') ;
    _csry = 0 ;
    _csrx = 0 ;
    for ( i = 0 ; i < LINES ; i++ )
    {
      for ( j = 0 ; j < COLS ; j++ )
      {
        c = w->_y[i][j] ;
        if ( ( c & STANDOUT ) )
        {
          Bconout(2,'\033') ;
          Bconout(2,'p') ;
          Bconout(5,(c & 0xff)) ;
          Bconout(2,'\033') ;
          Bconout(2,'q') ;
        }
        else
        {
          Bconout(5,(c & 0xff)) ;
        } ;
        _csrx++ ;
      } ;
      _movcur(i+1,0) ;
    } ;
    _movcur( curscr->_cury, curscr->_curx) ;
  } ;
}
End of commenting out */
/* #]wrefresh: */
/* END OF HORROR CODE - CHILDREN CAN OPEN THEIR EYES NOW */


/* mvcur : move cursor in standard curses manner */
/* Does not seem to be used by Moria */
void mvcur(ly,lx,ny,nx)
int ly,lx,ny,nx;
{
  _movcur((int16) ny,(int16) nx);
} /* end of mvcur */


/* _movcur : move cursor */
void _movcur(y,x)
register int16 y,x;
{
  if (_csry == y && _csrx == x)
    return;
  /* Set cursor position */
  Bconout(2,'\033');
  Bconout(2,'Y');
  Bconout(2,y + ' ');
  Bconout(2,x + ' ');
  _csry = y;
  _csrx = x;
} /* end of _movcur */


/* scroll : scroll a window upward one line */
int scroll(w)
WINDOW *w;
{
           int16 **tmpptr1, **tmpptr2;
  register int16 j, *tmpptr3, *tmpptr4; /* temp pointers for loops */

  for (tmpptr1 = w->_y, tmpptr2 = &(w->_y[1]);
       *tmpptr2 < w->_yend; 
       tmpptr1++, tmpptr2++)
  {
    for (j = 0, tmpptr3 = *tmpptr1, tmpptr4 = *tmpptr2;
         j <= w->_maxx;
         j++, tmpptr3++, tmpptr4++)
    {
      *tmpptr3 = *tmpptr4;
    }
  }
  for (tmpptr3 = w->_y[w->_maxy]; tmpptr3 <= w->_yend; tmpptr3++)
    *tmpptr3 = ' ';
  if (w == curscr)
    wrefresh(curscr);
  else
  {
    w->_firstch = w->_y[0];
    w->_lastch = w->_yend;
  };
  return(OK);
} /* end of scroll */
/* end of the curses source */
