/****************************************************************************/
/*                                                                          */
/*   CURSINC.H                    include file for the CURSES module itself */
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

/* Original author :  
    Rene van't Veen
    Debussystraat 27
    1817 GL Alkmaar
    The Netherlands

   Extensive modifications for speed and clarity by :
    Hildo Biersma
    Tollensstraat 138
    2513 GE Den Haag
    Holland

   The original copyright notices still apply - refer to CURSES.C */

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

#define mvwaddch(w,y,x,c)   { wmove(w,y,x); waddch(w,c); }
#define mvwaddstr(w,y,x,s)  { wmove(w,y,x); waddstr(w,s); }
#define clearok(w,f)        { w->_clear = (w->_flags & _FULLWIN) \
                              ? f : w->_clear; }
#define getyx(w,y,x)        { y = w->_cury; x = w->_curx; }

struct _win_st 
{
  int16 _cury, _curx;
  int16 _maxy, _maxx;
  int16 _begy, _begx;
  int16 _flags;
  bool  _clear;
  bool  _leave;
  bool  _scroll;
  int16 **_y;
  int16 *_firstch;
  int16 *_lastch;
  int16 *_yend;
};

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
int  curaddch( char c );
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
void cur_refresh( void );
void wrefresh ( WINDOW *w );
void mvcur ( int ly , int lx , int ny , int nx );
void _movcur ( int16 y , int16 x );
int  scroll ( WINDOW *w );
/* end of prototypes for curses.c */

/****************END OF SOURCE CURSINC.H************************************/
