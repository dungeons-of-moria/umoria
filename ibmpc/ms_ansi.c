/* ibmpc/ms_ansi.c: a set of routines to provide either PCcurses or ANSI output

   Copyright (c) 1989-92 James E. Wilson, Don Kneller

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include <curses.h>
#include <stdio.h>

#if defined(MSDOS) && defined(ANSI)

#ifdef LINT_ARGS
static int curses_move(int, int);
static char *getent(char *,char * *,int );
static void initansistr(void);
static int ansi_initscr(void);
static int ansi_endwin(void);
static int ansi_addch(int );
static int ansi_mvaddstr(int ,int ,char *);
static int ansi_mvprintw(int ,int ,char *,int ,int ,int ,int );
static int ansi_move(int ,int );
static int ansi_move_tgoto(int ,int );
static int ansi_clrtobot(void);
static int ansi_clrtoeol(void);
static int ansi_mvaddch(int ,int ,char );
static int ansi_clear(void);
static int ansi_refresh(void);
static int ansi_noop(void);
int ansi_prep(int ,int, int);
#else
static int	ansi_addch(), ansi_mvaddstr(), ansi_mvprintw(), ansi_move(),
		ansi_move_tgoto(),
		ansi_clrtobot(), ansi_clrtoeol(), ansi_mvaddch(), ansi_endwin(),
		ansi_refresh(), ansi_clear(), ansi_noop(), ansi_initscr();
int ansi_prep();
#endif

extern char *tgetstr();
extern char *getenv();
extern char *tgoto();

/* Must supply a functional form of the PCcurses "move" routine */
static int
curses_move(y, x)
int	y, x;
{
	return wmove(stdscr, y, x);
}

/* Default is for curses to be used */
int	(*output_addch)() = addch;
int	(*output_mvaddstr)() = mvaddstr;
int	(*output_mvprintw)() = mvprintw;
int	(*output_move)() = curses_move;
int	(*output_endwin)() = endwin;
int	(*output_clrtobot)() = clrtobot;
int	(*output_clrtoeol)() = clrtoeol;
int	(*output_mvaddch)() = mvaddch;
int	(*output_initscr)() = initscr;
int	(*output_refresh)() = refresh;
int	(*output_clear)() = clear;

int	(*output_nocrmode)() = nocrmode;
int	(*output_crmode)() = crmode;
int	(*output_nonl)() = nonl;
int	(*output_nl)() = nl;
int	(*output_noecho)() = noecho;
int	(*output_echo)() = echo;
int	ansi;
int	LI;

#define LEFTFIELD	-10;
#define	NEED		1

static int	moveopt = 1;
static char	*CE, *CL, *CM, *DO, *LE, *ND, *TE, *TI, *UP;
static int	currow = 0;
static int	curcol = LEFTFIELD;

static char	*
getent(str, tbufp, need)
char	*str, **tbufp;
int	need;
{
	char	*value;

	if ((value = tgetstr(str, tbufp)) == NULL && need == NEED)
		error("termcap:  Moria needs %s\n", str);
	return value;
}

static void
initansistr()
{
	static	char tbuf[512];
	char	temp[1024], *tbufp, *term;

	if ((term = getenv("TERM")) == NULL)
		term = "ibmpc-mono";
	if (tgetent(temp, term) < 1)
		error("Unknown terminal type: %s.", term);
	tbufp = tbuf;
	LE = getent("le", &tbufp, NEED);
	CE = getent("ce", &tbufp, NEED);
	CL = getent("cl", &tbufp, NEED);
	CM = getent("cm", &tbufp, NEED);
	ND = getent("nd", &tbufp, NEED);
	TE = getent("te", &tbufp, !NEED);
	TI = getent("ti", &tbufp, !NEED);
	UP = getent("up", &tbufp, NEED);
	DO = getent("do", &tbufp, NEED);
	LI = tgetnum("li");
	if (LI <= 0)
		LI = 24;
}

ansi_prep(check_ansi, domoveopt, truetgoto)
int	check_ansi;
int	domoveopt;
int	truetgoto;
{
	moveopt = domoveopt;

	/* Check for ANSI.SYS */
	if (check_ansi) {
		/* Clear the input queue */
		while (kbhit())
			(void) getch();

		/* Send out the DSR string */
		fputs("\033[6n", stdout);
		fflush(stdout);

		/* Is there anything in the input?  If so ANSI responded. */
		if (kbhit()) {
			while (kbhit())
				(void) getch();
		}
		else {
			putchar('\n');
			error("ANSI.SYS not installed!  See MORIA.DOC for details!\n");
		}
	}

	/* get the ANSI strings */
	initansistr();

	/* change function pointers to ANSI versions */
	output_addch = ansi_addch;
	output_mvaddstr = ansi_mvaddstr;
	output_mvprintw = ansi_mvprintw;
	if (truetgoto)
		output_move = ansi_move_tgoto;
	else
		output_move = ansi_move;
	output_clrtobot = ansi_clrtobot;
	output_clrtoeol = ansi_clrtoeol;
	output_mvaddch = ansi_mvaddch;
	output_refresh = ansi_refresh;
	output_clear = ansi_clear;
	output_initscr = ansi_initscr;
	output_endwin = ansi_endwin;

	output_nocrmode =
	output_crmode =
	output_nonl =
	output_nl =
	output_noecho =
	output_echo = ansi_noop;

	ansi = 1;
}

static int
ansi_initscr()
{
	if (TI != NULL)
		fputs(TI, stdout);
	return OK;
}

static int
ansi_endwin()
{
	if (TI != NULL)
		fputs(TE, stdout);
	return OK;
}

static int
ansi_addch(ch)
int	ch;
{
	putc(ch, stdout);
	curcol++;
	return OK;
}

static int
ansi_mvaddstr(row, col, str)
int	row, col;
char	*str;
{
	(void) ansi_move(row, col);
	fputs(str, stdout);
	curcol = LEFTFIELD;
	return OK;
}

static int
ansi_mvprintw(row, col, fmt, a1, a2, a3, a4)
int	row, col;
char	*fmt;
int	a1, a2, a3, a4;	/* large enough for %c%s%c of files.c ! */
{
	(void) ansi_move(row, col);
	fprintf(stdout, fmt, a1, a2, a3, a4);
	curcol = LEFTFIELD;
	return OK;
}

#define abs(x)	((x) < 0 ? -(x) : (x))

/* For a bit more speed, don't use tgoto() from termcap */
static int
ansi_move(row, col)
int	row, col;
{
	if (moveopt && abs(currow - row) < 3 && abs(curcol - col) < 3) {
		while (row > currow)
			fputs(DO, stdout), currow++;
		while (row < currow)
			fputs(UP, stdout), currow--;
		while (col > curcol)
			fputs(ND, stdout), curcol++;
		while (col < curcol)
			fputs(LE, stdout), curcol--;
	}
	else
		fprintf(stdout, "\033[%d;%dH\0__cursor motion__", row+1,col+1);
	currow = row;
	curcol = col;
	return OK;
}

/* Use tgoto (which is rather slow) */
static int
ansi_move_tgoto(row, col)
int	row, col;
{
	if (moveopt && abs(currow - row) < 3 && abs(curcol - col) < 3) {
		while (row > currow)
			fputs(DO, stdout), currow++;
		while (row < currow)
			fputs(UP, stdout), currow--;
		while (col > curcol)
			fputs(ND, stdout), curcol++;
		while (col < curcol)
			fputs(LE, stdout), curcol--;
	}
	else {
		fputs(tgoto(CM, col, row), stdout);
	}
	currow = row;
	curcol = col;
	return OK;
}

static int
ansi_clrtobot()
{
	ansi_clrtoeol();
	ansi_move(++currow, 0);
	ansi_clrtoeol();
	for (; currow <= LI; currow++) {
		fputs(DO, stdout);
		ansi_clrtoeol();
	}
	curcol = LEFTFIELD;
	return OK;
}

static int
ansi_clrtoeol()
{
	fputs(CE, stdout);
	return OK;
}

static int
ansi_mvaddch(row, col, ch)
int	row, col;
char	ch;
{
	ansi_move(row, col);
	putchar(ch);
	curcol++;
	return OK;
}

static int
ansi_clear()
{
	fputs(CL, stdout);
	return OK;
}

static int
ansi_refresh()
{
	fflush(stdout);
	return OK;
}

static int
ansi_noop()
{
	return OK;
}
#endif
