// Copyright (c) 2021 Jeffrey D. Hirschberg
#pragma once

#include <array>
#include <set>
#include <utility>
#include <stdint.h>
#include <emscripten.h>

#undef  ERR
#define ERR (-1)

#undef  OK
#define OK (0)

#undef  TRUE
#define TRUE 1

#undef  FALSE
#define FALSE 0

/* Must define LINES and COLS for initscr() */
#define LINES 24
#define COLS 80

#define SPACE 32

#define DEFAULT_TIMEOUT -1

#define UNUSED(x) (void)(x)

typedef uint32_t chtype;

struct WINDOW {
  int y, x; // Logical cursor position

  /* Assume origin is 0, 0 and lines = LINES, columns = COLS */
  std::array<std::array<chtype, COLS>, LINES> cells;
};

extern WINDOW *stdscr;
extern WINDOW *curscr;
extern int _timeout;

int addch(const chtype ch);
int addstr(const char *str);
inline int beep() { EM_ASM({IO.beep();},); return OK; };
int clear();
int clrtobot();
int clrtoeol ();
inline int endwin() { EM_ASM({IO.endwin();},); return OK; };
inline void flushinputbuffer() { EM_ASM({IO.flushInputBuffer();},); };
int getch();
void getyx(WINDOW *win, int &y, int &x);
int initscr();
inline int keypad(WINDOW *win, bool bf) { UNUSED(win); UNUSED(bf); return OK; };
int move(int y, int x);
int mvaddch (int y, int x, const chtype ch);
int mvaddstr(int y, int x, const char *str);
int mvcur(int oldrow, int oldcol, int newrow, int newcol);
WINDOW *newwin(int nlines, int ncols, int begin_y, int begin_x);
inline int noecho() { return OK; };
inline int nonl() { return OK; };
int overwrite(const WINDOW *srcwin, WINDOW *dstwin);
inline int raw() { return OK; };
int refresh();
inline void timeout(int delay) { _timeout = delay; };
inline int touchwin(WINDOW *win) { return OK; };
int waddch(WINDOW *win, const chtype ch);
int wmove(WINDOW *win, int y, int x);
int wrefresh(WINDOW *win);

