/* ibmpc/ms_ansi.h: ANSI definitions for MSDOS

   Copyright (c) 1989-92 James E. Wilson, Don Kneller

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

/* To allow the choice between PCcurses (which may not work on all machines)
 * and ANSI (which should work on all machines), indirect the output routines
 * through pointers and adjust the pointers to the correct routines at
 * initialization time.
 */
#ifdef LINT_ARGS
int	(*output_addch)(int);
int	(*output_mvaddstr)(int, int, char *);
int	(*output_mvprintw)(int, int, char *, ...);
int	(*output_move)(int, int);
int	(*output_endwin)(void);
int	(*output_clrtobot)(void);
int	(*output_clrtoeol)(void);
int	(*output_mvaddch)(int, int, char);
int	(*output_initscr)(void);
int	(*output_refresh)(void);
int	(*output_clear)(void);

int	(*output_nocrmode)(void);
int	(*output_crmode)(void);
int	(*output_nonl)(void);
int	(*output_nl)(void);
int	(*output_noecho)(void);
int	(*output_echo)(void);
int	ansi_prep (int, int, int);
#else
extern int	(*output_addch)();
extern int	(*output_mvaddstr)();
extern int	(*output_mvprintw)();
extern int	(*output_move)();
extern int	(*output_endwin)();
extern int	(*output_clrtobot)();
extern int	(*output_clrtoeol)();
extern int	(*output_mvaddch)();
extern int	(*output_initscr)();
extern int	(*output_refresh)();
extern int	(*output_clear)();

extern int	(*output_nocrmode)();
extern int	(*output_crmode)();
extern int	(*output_nonl)();
extern int	(*output_nl)();
extern int	(*output_noecho)();
extern int	(*output_echo)();
extern int	ansi_prep();
#endif

extern int	ansi;

#define addch		(*output_addch)
#define mvaddstr	(*output_mvaddstr)
#define mvprintw	(*output_mvprintw)
#undef move		/* from curses.h */
#define move		(*output_move)
#define endwin		(*output_endwin)
#define clrtobot	(*output_clrtobot)
#define clrtoeol	(*output_clrtoeol)
#define mvaddch		(*output_mvaddch)
#define initscr		(*output_initscr)
#define refresh		(*output_refresh)
#define clear		(*output_clear)
#define nocrmode	(*output_nocrmode)
#define crmode		(*output_crmode)
#define nonl		(*output_nonl)
#define nl		(*output_nl)
#define noecho		(*output_noecho)
#define echo		(*output_echo)
