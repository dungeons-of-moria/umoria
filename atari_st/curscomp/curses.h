/****************************************************************************/
/*                                                                          */
/*   CURSES.H                      include file for programs using CURSES   */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/* This source and resulting object may be modified, used or distributed by */
/* anyone who so desires under the following conditions :                   */
/*                                                                          */
/*  1) This notice and the copyright notice shall not be removed or         */
/*     changed.                                                             */
/*  2) No credit shall be taken for this source and resulting objects       */
/*  3) This source or resulting objects is not to be traded, sold or        */
/*     used for personal gain or profit.                                    */
/*  4) Inclusion of this source or resulting objects in commercially        */
/*     available packages is forbidden without written consent of the       */
/*     author of this source.                                               */
/*                                                                          */
/****************************************************************************/

#include "stdio.h"

typedef unsigned char bool;
typedef short         int16;
typedef long          int32;

#define OK          1
#define ERR         0
#define TRUE        1
#define FALSE       0
#define _SUBWIN     01
#define _ENDLINE    02
#define _FULLWIN    04
#define _SCROLLWIN  010
#define _STANDOUT   0200
#define WINDOW      struct _win_st

#define TOUCHED     0x0200
#define STANDOUT    0x0100

/* prototypes of functions in curses.c */
WINDOW *initscr ( void );
void endwin ( void );
WINDOW *newwin ( int l , int c , int by , int bx );
void delwin ( WINDOW *w );
int mvwin ( WINDOW *w , int y , int x );
void touchwin ( WINDOW *w );
WINDOW *subwin ( WINDOW *w , int l , int c , int by , int bx );
void leaveok ( WINDOW *w , bool f );
void scrollok ( WINDOW *w , bool f );
void nl ( void );
void nonl ( void );
void longname ( char *termbuf , char *name );
int  waddch ( WINDOW *w , char c );
int  waddstr ( WINDOW *w , char *s );
void box ( WINDOW *w , char v , char h );
void wclear ( WINDOW *w );
void wclrtobot ( WINDOW *w );
void wclrtoeol ( WINDOW *w );
int  wdelch ( WINDOW *w );
int  wdeleteln ( WINDOW *w );
void werase ( WINDOW *w );
int  winsch ( WINDOW *w , char c );
void winsertln ( WINDOW *w );
int  wmove ( WINDOW *w , int y , int x );
void overlay ( WINDOW *v , WINDOW *w );
void overwrite ( WINDOW *v , WINDOW *w );
void wstandout ( WINDOW *w );
void wstandend ( WINDOW *w );
void raw ( void );
void noraw ( void );
void crmode ( void );
void nocrmode ( void );
void echo ( void );
void noecho ( void );
long wgetch ( WINDOW *w );
int  wgetstr ( WINDOW *w , char *s );
void wrefresh ( WINDOW *w );
void mvcur ( int ly , int lx , int ny , int nx );
void _movcur ( int16 y , int16 x );
int  scroll ( WINDOW *w );
/* end of prototypes for curses.c */

#define addch(c)            waddch(stdscr, c)
#if 0
/* The original code, does not return error codes properly.  */
#define mvaddch(y,x,c)      { wmove(stdscr,y,x) ; waddch(stdscr,c) ; }
#define mvwaddch(w,y,x,c)   { wmove(w,y,x) ; waddch(w,c) ; }
#else
#define mvaddch(y,x,c)      (wmove(stdscr,y,x) == ERR ? ERR : waddch(stdscr,c))
#define mvwaddch(w,y,x,c)   (wmove(w,y,x) == ERR ? ERR : waddch(w,c))
#endif
#define addstr(s)           waddstr(stdscr,s)
#if 0
/* The original code, does not return error codes properly.  */
#define mvaddstr(y,x,s)     { wmove(stdscr,y,x) ; waddstr(stdscr,s) ; }
#define mvwaddstr(w,y,x,s)  { wmove(w,y,x) ; waddstr(w,s) ; }
#else
#define mvaddstr(y,x,s)     (wmove(stdscr,y,x) == ERR ? ERR :waddstr(stdscr,s))
#define mvwaddstr(w,y,x,s)  (wmove(w,y,x) == ERR ? ERR : waddstr(w,s))
#endif
#define clear()             wclear(stdscr)
#define clearok(w,f)        { w->_clear = (w->_flags & _FULLWIN) ? f : \
                              w->_clear ; }
#define clrtobot()          wclrtobot(stdscr)
#define clrtoeol()          wclrtoeol(stdscr)
#define delch()             wdelch(stdscr)
#define mvdelch(y,x)        { wmove(stdscr,y,x) ; wdelch(stdscr) ; }
#define mvwdelch(w,y,x)     { wmove(w,y,x) ; wdelch(w) ; }
#define deleteln()          wdeleteln(stdscr)
#define mvdeleteln(y,x)     { wmove(stdscr,y,x) ; wdeleteln(stdscr) ; }
#define mvwdeleteln(w,y,x)  { wmove(w,y,x) ; wdeleteln(w) ; }
#define erase()             werase(stdscr)
#define insch(c)            winsch(stdscr,c)
#define mvinsch(y,x,c)      { wmove(stdscr,y,x) ; winsch(stdscr,c) ; }
#define mvwinsch(w,y,x,c)   { wmove(w,y,x) ; winsch(w,c) ; }
#define insertln()          winsertln(stdscr)
#define mvinsertln(y,x)     { wmove(stdscr,y,x) ; winsertln(stdscr) ; }
#define mvwinsertln(w,y,x)  { wmove(w,y,x) ; winsertln(w) ; }
#define move(y,x)           wmove(stdscr,y,x)
#define refresh()           wrefresh(stdscr)
#define standout()          wstandout(stdscr)
#define standend()          wstandend(stdscr)
#define getch()             wgetch(stdscr)
#define mvgetch(y,x)        ( wmove(stdscr,y,x) , wgetch(stdscr) )
#define mvwgetch(w,y,x)     ( wmove(w,y,x) , wgetch(w) )
#define getstr(s)           wgetstr(stdscr,s)
#define mvgetstr(y,x,s)     { wmove(stdscr,y,x) ; wgetstr(stdscr,s) ; }
#define mvwgetstr(w,y,x,s)  { wmove(w,y,x) ; wgetstr(w,s) ; }
#define getyx(w,y,x)        { y = w->_cury ; x = w->_curx ; }
#define inch()              ( stdscr->_y[stdscr->_cury][stdscr->_curx])
#define mvinch(y,x)         ( wmove(stdscr,y,x) , \
                              stdscr->_y[stdscr->_cury][stdscr->_curx])
#define mvwinch(w,y,x)      ( wmove(w,y,x) , w4->_y[w->_cury][w->_curx])
#define winch(w)            (w->_y[w->_cury][w->_curx])

struct _win_st 
{
  int16 _cury, _curx ;
  int16 _maxy, _maxx ;
  int16 _begy, _begx ;
  int16 _flags ;
  bool  _clear ;
  bool  _leave ;
  bool  _scroll ;
  int16 **_y ;
  int16 *_firstch ;
  int16 *_lastch ;
  int16 *_yend;
};

extern WINDOW *curscr;
extern WINDOW *stdscr;
extern char   *Def_term;
extern bool   My_term;
extern char   *ttytype;
extern int    LINES;
extern int    COLS;
/****************END OF SOURCE CURSES.H************************************/
