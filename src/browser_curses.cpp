// Copyright (c) 2021 Jeffrey D. Hirschberg
//
#ifdef __EMSCRIPTEN__
// Turn off check for $ in identifier names, as emscripten uses it
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdollar-in-identifier-extension"
#include "browser_curses.h"

WINDOW *stdscr;
WINDOW *curscr;
int _timeout = DEFAULT_TIMEOUT;

EM_ASYNC_JS(chtype, _getch, (int timeout), {
    return await IO.getch(timeout);
});

int addch(const chtype ch) { return waddch(stdscr, ch); }

int addstr(const char *str) {
  if (!str) {
    return ERR;
  }

  int code = OK;

  while ((*str != '\0') && (code == OK)) {
    code |= waddch(stdscr, *str++);
  }

  return code;
}

int clear(void) {
  stdscr->y = 0;
  stdscr->x = 0;

  int y = 0;
  int x = 0;

  while (y < LINES) {
    while (x < COLS) {
      addch(' ');
      x++;
    }
    x = 0;
    y++;
  }

  stdscr->y = 0;
  stdscr->x = 0;
    
  return OK;
}

int clrtobot() {
  int y, yOrig, x, xOrig;
  y = yOrig = stdscr->y;
  x = xOrig = stdscr->x;

  while (y < LINES) {
    while (x < COLS) {
      addch(' ');
      x++;
    }
    x = 0;
    y++;
  }

  stdscr->y = yOrig;
  stdscr->x = xOrig;
    
  return OK;
}

int clrtoeol() {
  int yOrig, x, xOrig;
  yOrig = stdscr->y;
  x = xOrig = stdscr->x;

  while (x < COLS) {
    addch(' ');
    x++;
  }

  stdscr->y = yOrig;
  stdscr->x = xOrig;

  return OK;
}

int getch() {
  return _getch(_timeout);
}

void getyx(WINDOW *win, int &y, int &x) {
  y = win->y;
  x = win->x;
}

int initscr(void) {
  stdscr = new WINDOW();
  curscr = new WINDOW();

  EM_ASM({IO.initscr($0, $1);}, COLS, LINES);

  return OK;
}

int move(int y, int x) {
  return wmove(stdscr, y, x);
}

int mvaddch (int y, int x, const chtype ch) {
  return ((move(y, x)) | (addch(ch)));
}

int mvaddstr(int y, int x, const char *str) {
  if (!str) {
    return ERR;
  }

  if (move(y, x)) {
    return ERR;
  }

  return addstr(str);
}

int mvcur(int oldrow, int oldcol, int newrow, int newcol) {
  UNUSED(oldrow);
  UNUSED(oldcol);
  if ((newrow < 0) || (newrow >= LINES) || (newcol < 0) || (newcol >= COLS)) {
    return ERR;
  }

  EM_ASM({IO.mvcur($0, $1);}, newrow, newcol);

  return OK;
}

WINDOW *newwin(int nlines, int ncols, int begin_y, int begin_col) {
  // Assume all windows are fullscreen
  UNUSED(nlines);
  UNUSED(ncols);
  UNUSED(begin_y);
  UNUSED(begin_col);
  return new WINDOW();
}

int overwrite(const WINDOW *srcwin, WINDOW *dstwin) {
  // Assuming that srcwin and dstwin have the same size and origin
  wmove(dstwin, 0, 0);
  
  for (int y = 0; y < LINES; y++) {
    for (int x = 0; x < COLS; x++) {
      waddch(dstwin, srcwin->cells.at(y).at(x));
    }
  }
  
  return OK;
}

int refresh(void) { return wrefresh(stdscr); }

int waddch(WINDOW *win, const chtype ch) {
  int y = win->y;
  int x = win->x;

  // Check for legal cursor position
  if ((y >= LINES) || (x >= COLS) || (y < 0) || (x < 0)) {
    return ERR;
  }

  win->cells.at(y).at(x) = ch;

  // Check if cursor is in bottom right corner.  If so, don't move
  // the cursor and return an error.
  if (((x + 1) == COLS) && ((y + 1) == LINES)) {
    return ERR;
  }

  // Move cursor one column to the right.  If the cursor goes beyond
  // the last column, wrap around to the start of the next row.
  x = ((++x) % COLS);
  
  if (x == 0) {
    y++;
  }

  win->y = y;;
  win->x = x;

  return OK;
}

int wmove(WINDOW *win, int y, int x) {
  if ((y < 0) || (x < 0) || (y >= LINES) || (x >= COLS)) {
    return ERR;
  }

  win->y = y;
  win->x = x;

  return OK;
}

int wrefresh(WINDOW *win) {
  for (std::size_t y = 0; y < win->cells.size(); ++y) {
    for (std::size_t x = 0; x < win->cells.at(0).size(); ++x) {
      if (win->cells.at(y).at(x) != curscr->cells.at(y).at(x)) {
	EM_ASM({IO.setCell($0, $1, $2);}, y, x, win->cells.at(y).at(x));
	curscr->cells.at(y).at(x) = win->cells.at(y).at(x);
      }
    }
  }

  // Update physical cursor to match win logical cursor
  mvcur(0, 0, win->y, win->x);

  // Update curscr logical cursor to match physical cursor
  wmove(curscr, win->y, win->x);

  EM_ASM({IO.refresh();},);
  
  return OK;
}
#pragma clang diagnostic pop
#endif
#endif
