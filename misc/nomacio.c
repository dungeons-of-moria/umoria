/* io.c: terminal I/O code, uses the curses package

   Copyright (c) 1989 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#ifndef ATARIST_MWC
#include <curses.h>
#else
#include "curses.h"
long wgetch();
#include <osbind.h>
char *getenv();
#endif

#if defined(MSDOS) && defined(ANSI)
#include "ms_ansi.h"
#else
#ifndef ATARIST_MWC
#ifndef VMS
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#else
#include <types.h>
#include <file.h>
#endif
#include <signal.h>
#endif
#endif
#include <ctype.h>
#include <sys/param.h>

#include "constant.h"
#include "config.h"
#include "types.h"
#include "externs.h"

#ifdef USG
#ifndef ATARIST_MWC
#include <string.h>
#endif
#include <termio.h>
#else
#ifndef VMS
#include <strings.h>
#include <sys/wait.h>
#endif
#endif

/* Fooling lint. Unfortunately, c defines all the TIO.	  -CJS-
   constants to be long, and lint expects them to be int. Also,
   ioctl is sometimes called with just two arguments. The
   following definition keeps lint happy. It may need to be
   reset for different systems.	 */
#ifdef lint
#ifdef Pyramid
/* Pyramid makes constants greater than 65535 into long! Gakk! -CJS-
/*ARGSUSED*/
/*VARARGS2*/
static Ioctl(i, l, p) long l; char *p; { return 0; }
#else
/*ARGSUSED*/
/*VARARGS2*/
static Ioctl(i, l, p) char *p; { return 0; }
#endif
#define ioctl	    Ioctl
/* This use_value hack is for curses macros which return a value,
   but don't shut up about it when you try to tell them (void).	 */
int Use_value;
#define use_value   Use_value +=
#else
#define use_value   (void)
#endif

char *getenv();

#ifdef USG
void exit();
unsigned sleep();
#endif
#ifdef ultrix
void exit();
void sleep();
#endif

#ifdef USG
static struct termio save_termio;
#else
#ifndef VMS
static struct ltchars save_special_chars;
static struct sgttyb save_ttyb;
static struct tchars save_tchars;
#endif
#endif

static int curses_on = FALSE;
static WINDOW *savescr;		/* Spare window for saving the screen. -CJS- */

#ifdef SIGTSTP
/* suspend()							   -CJS-
   Handle the stop and start signals. This ensures that the log
   is up to date, and that the terminal is fully reset and
   restored.  */
int suspend()
{
  struct sgttyb tbuf;
  struct ltchars lcbuf;
  struct tchars cbuf;
  long time();

  py.misc.male |= 2;
  upd_log();
  (void) ioctl(0, TIOCGETP, (char *)&tbuf);
  (void) ioctl(0, TIOCGETC, (char *)&cbuf);
  (void) ioctl(0, TIOCGLTC, (char *)&lcbuf);
  restore_term();
  (void) kill(0, SIGSTOP);
  curses_on = TRUE;
  (void) ioctl(0, TIOCSETP, (char *)&tbuf);
  (void) ioctl(0, TIOCSETC, (char *)&cbuf);
  (void) ioctl(0, TIOCSLTC, (char *)&lcbuf);
  (void) wrefresh(curscr);
  py.misc.male &= ~2;
  plog.log_out = time((long *)0);	/* Don't add in elapsed time. */
  upd_log();
  return 0;
}
#endif

/* initializes curses routines */
void init_curses()
{
  int i, y, x;

#ifndef USG
  (void) ioctl(0, TIOCGLTC, (char *)&save_special_chars);
  (void) ioctl(0, TIOCGETP, (char *)&save_ttyb);
  (void) ioctl(0, TIOCGETC, (char *)&save_tchars);
#else
#ifndef VMS
  (void) ioctl(0, TCGETA, (char *)&save_termio);
#endif
#endif

#ifdef ATARIST_MWC
  initscr();
  if (ERR)
#else
#if defined(USG) && !defined(PC_CURSES)	/* PC curses returns ERR */
  if (initscr() == NULL)
#else
  if (initscr() == ERR)
#endif
#endif
    {
      (void) printf("Error allocating screen in curses package.\n");
      exit(1);
    }
  if (LINES < 24 || COLS < 80)	 /* Check we have enough screen. -CJS- */
    {
      (void) printf("Screen too small for moria.\n");
      exit (1);
    }
#ifdef SIGTSTP
  (void) signal (SIGTSTP, suspend);
#endif
  if ((savescr = newwin (0, 0, 0, 0)) == NULL)
    {
      (void) printf ("Out of memory in starting up curses.\n");
      exit_game();
    }
  (void) clear();
  (void) refresh();
  moriaterm ();

  /* check tab settings, exit with error if they are not 8 spaces apart */
  move(0, 0);
  for (i = 1; i < 10; i++)
    {
      addch('\t');
      getyx(stdscr, y, x);
      if (y != 0 || x != i*8)
	break;
    }
  if (i != 10)
    {
      msg_print("Tabs must be set 8 spaces apart.");
      exit_game();
    }
}

/* Set up the terminal into a suitable state for moria.	 -CJS- */
void moriaterm()
{
#ifdef USG
  struct termio tbuf;
#else
  struct ltchars lbuf;
  struct tchars buf;
#endif

  curses_on = TRUE;
#ifndef BSD4_3
  use_value crmode();
#else
  use_value cbreak();
#endif
  use_value noecho();
  /* can not use nonl(), because some curses do not handle it correctly */
#ifdef MSDOS
  msdos_raw();
#endif
#ifdef USG
  (void) ioctl(0, TCGETA, (char *)&tbuf);
  tbuf.c_cc[VINTR] = (char)3; /* control-C */
  tbuf.c_cc[VQUIT] = (char)-1;
  tbuf.c_cc[VERASE] = (char)-1;
  tbuf.c_cc[VKILL] = (char)-1;
  tbuf.c_cc[VEOF] = (char)-1;
  tbuf.c_cc[VEOL] = (char)-1;
  tbuf.c_cc[VEOL2] = (char)-1;
  (void) ioctl(0, TCSETA, (char *)&tbuf);
#else
#ifndef VMS
  /* disable all of the special characters except the suspend char, interrupt
     char, and the control flow start/stop characters */
  (void) ioctl(0, TIOCGLTC, (char *)&lbuf);
  lbuf.t_suspc = (char)26; /* control-Z */
  lbuf.t_dsuspc = (char)-1;
  lbuf.t_rprntc = (char)-1;
  lbuf.t_flushc = (char)-1;
  lbuf.t_werasc = (char)-1;
  lbuf.t_lnextc = (char)-1;
  (void) ioctl(0, TIOCSLTC, (char *)&lbuf);

  (void) ioctl (0, TIOCGETC, (char *)&buf);
  buf.t_intrc = (char)3; /* control-C */
  buf.t_quitc = (char)-1;
  buf.t_startc = (char)17; /* control-Q */
  buf.t_stopc = (char)19; /* control-S */
  buf.t_eofc = (char)-1;
  buf.t_brkc = (char)-1;
  (void) ioctl(0, TIOCSETC, (char *)&buf);
#endif
#endif
}


/* Dump IO to buffer					-RAK-	*/
void put_buffer(out_str, row, col)
char *out_str;
int row, col;
{
  vtype tmp_str;

  /* truncate the string, to make sure that it won't go past right edge of
     screen */
  if (col > 79)
    col = 79;
  (void) strncpy (tmp_str, out_str, 79 - col);
  tmp_str [79 - col] = '\0';

#ifndef ATARIST_MWC
  if (mvaddstr(row, col, tmp_str) == ERR)
#else
  mvaddstr(row, col, out_str);
  if (ERR)
#endif
    {
      (void) sprintf(tmp_str, "error in put_buffer, row = %d col = %d\n",
		     row, col);
      prt(tmp_str, 0, 0);
      bell();
      /* wait so user can see error */
      (void) sleep(2);
    }
}


/* Dump the IO buffer to terminal			-RAK-	*/
void put_qio()
{
  screen_change = TRUE;	    /* Let inven_command know something has changed. */
  (void) refresh();
}

/* Put the terminal in the original mode.			   -CJS- */
void restore_term()
{
  if (!curses_on)
    return;
  put_qio();  /* Dump any remaining buffer */
#ifdef MSDOS
  (void) sleep(2);   /* And let it be read. */
#endif
  /* this moves curses to bottom right corner */
  mvcur(curscr->_cury, curscr->_curx, LINES-1, 0);
#ifdef VMS
  pause_line(15);
#endif
  endwin();  /* exit curses */
  (void) fflush (stdout);
#ifdef MSDOS
  msdos_noraw();
  (void) clear();
#endif
  /* restore the saved values of the special chars */
#ifdef USG
  (void) ioctl(0, TCSETA, (char *)&save_termio);
#else
#ifndef VMS
  (void) ioctl(0, TIOCSLTC, (char *)&save_special_chars);
  (void) ioctl(0, TIOCSETP, (char *)&save_ttyb);
  (void) ioctl(0, TIOCSETC, (char *)&save_tchars);
#endif
#endif
  curses_on = FALSE;
}


void shell_out()
{
#ifdef USG
  struct termio tbuf;
#else
  struct sgttyb tbuf;
  struct ltchars lcbuf;
  struct tchars cbuf;
#endif
#ifdef MSDOS
#include <process.h>
  char	*comspec, key;
#else
#ifdef ATARIST_MWC
  char comstr[80];
  extern char **environ;
#else
  int val;
  char *str;
#endif
#endif

  save_screen();
  /* clear screen and print 'exit' message */
  clear_screen();
#ifndef ATARIST_MWC
  put_buffer("[Entering shell, type 'exit' to resume your game.]\n",0,0);
#else
  put_buffer("[Escaping to shell]\n",0,0);
#endif
  put_qio();

#ifdef USG
  (void) ioctl(0, TCGETA, (char *)&tbuf);
#else
#ifndef VMS
  (void) ioctl(0, TIOCGETP, (char *)&tbuf);
  (void) ioctl(0, TIOCGETC, (char *)&cbuf);
  (void) ioctl(0, TIOCGLTC, (char *)&lcbuf);
#endif
#endif
  /* would call nl() here if could use nl()/nonl(), see moriaterm() */
#ifndef BSD4_3
  use_value nocrmode();
#else
  use_value nocbreak();
#endif
#ifdef MSDOS
  use_value msdos_noraw();
#endif
  use_value echo();
  ignore_signals();
#ifdef MSDOS		/*{*/
  if ((comspec = getenv("COMSPEC")) == NULL
  ||  spawnl(P_WAIT, comspec, comspec, (char *) NULL) < 0) {
	clear_screen();	/* BOSS key if shell failed */
	put_buffer("M:\\> ", 0, 0);
	do {
	  key = inkey(key);
	} while (key != '!');
  }

#else		/* MSDOS }{*/
#ifndef ATARIST_MWC
  val = fork();
  if (val == 0)
    {
#endif
      default_signals();
#ifdef USG
      (void) ioctl(0, TCSETA, (char *)&save_termio);
#else
#ifndef VMS
      (void) ioctl(0, TIOCSLTC, (char *)&save_special_chars);
      (void) ioctl(0, TIOCSETP, (char *)&save_ttyb);
      (void) ioctl(0, TIOCSETC, (char *)&save_tchars);
#endif
#endif
#ifndef MSDOS
      /* close scoreboard descriptor */
      /* it is not open on MSDOS machines */
#if 0
      /* this file is not open now, see init_file() in files.c */
      (void) close(highscore_fd);
#endif
#endif
      if (str = getenv("SHELL"))
#ifndef ATARIST_MWC
	(void) execl(str, str, (char *) 0);
#else
	system(str);
#endif
      else
#ifndef ATARIST_MWC
	(void) execl("/bin/sh", "sh", (char *) 0);
#endif
      msg_print("Cannot execute shell.");
#ifndef ATARIST_MWC
      exit(1);
    }
  if (val == -1)
    {
      msg_print("Fork failed. Try again.");
      return;
    }
#ifdef USG
  (void) wait((int *) 0);
#else
  (void) wait((union wait *) 0);
#endif
#endif /* ATARIST_MWC */
#endif		 /* MSDOS }*/
  restore_signals();
  /* restore the cave to the screen */
  restore_screen();
#ifndef BSD4_3
  use_value crmode();
#else
  use_value cbreak();
#endif
  use_value noecho();
  /* would call nonl() here if could use nl()/nonl(), see moriaterm() */
#ifdef MSDOS
  msdos_raw();
#endif
  /* disable all of the local special characters except the suspend char */
  /* have to disable ^Y for tunneling */
#ifdef USG
  (void) ioctl(0, TCSETA, (char *)&tbuf);
#else
#ifndef VMS
  (void) ioctl(0, TIOCSLTC, (char *)&lcbuf);
  (void) ioctl(0, TIOCSETP, (char *)&tbuf);
  (void) ioctl(0, TIOCSETC, (char *)&cbuf);
#endif
#endif
  (void) wrefresh(curscr);
}


/* Returns a single character input from the terminal.	This silently -CJS-
   consumes ^R to redraw the screen and reset the terminal, so that this
   operation can always be performed at any input prompt.  inkey() never
   returns ^R.	*/
char inkey()
{
  int i;

  put_qio();			/* Dump IO buffer		*/
  command_count = 0;  /* Just to be safe -CJS- */
  while (TRUE)
    {
#ifdef MSDOS
      i = msdos_getch();
#else
      i = getch();
#endif
      /* some machines may not sign extend. */
      if (((char)i == (char)EOF) && (feof (stdin)))
	{
	  prt ("Read end of file!!!!!", 22, 0);
	  (void) refresh ();
	  if (!character_generated || character_saved)
	    exit_game();
	  disturb(1, 0);
	  eof_flag++;
	  if (eof_flag > 100)
	    {
	      /* just in case, to make sure that the process eventually dies */
	      panic_save = 1;
	      (void) strcpy(plog.log_died_from, "(end of input: panic saved)");
	      if (!save_char())
		{
		  (void) strcpy(plog.log_died_from, "panic: unexpected eof");
		  death = TRUE;
		}
	      exit_game();
	    }
	  return ESCAPE;
	}
      if (i != CTRL('R'))
	return i;
      (void) wrefresh (curscr);
      moriaterm();
    }
}


/* Flush the buffer					-RAK-	*/
void flush()
{
#ifdef MSDOS
  while (kbhit())
	(void) getch();
#else
#ifdef TIOCDRAIN
  (void) ioctl (1, TIOCDRAIN); /* Wait for output to finish.  -CJS- */
#else
  /* This should wait for output to drain if the special ioctl isn't there. */
#ifndef USG
  { struct sgttyb sb;
    (void) ioctl(0, TIOCGETP, &sb);
    (void) ioctl(0, TIOCSETP, &sb);
  }
#else
#ifndef VMS
  { struct termio tio;
    (void) ioctl(0, TCGETA, &tio);
    (void) ioctl(0, TCSETAF, &tio); /* F means flush input, drain output */
  }
#endif
#endif
#endif

  /* now drain output */
  put_qio ();
}


/* Clears given line of text				-RAK-	*/
void erase_line(row, col)
int row;
int col;
{
  if (row == MSG_LINE && msg_flag)
    msg_print(NULL);
  (void) move(row, col);
  clrtoeol();
}


/* Clears screen */
void clear_screen()
{
  if (msg_flag)
    msg_print(NULL);
  (void) clear();
}

void clear_from (row)
int row;
{
  (void) move(row, 0);
  clrtobot();
}


/* Outputs a char to a given interpolated y, x position	-RAK-	*/
/* sign bit of a character used to indicate standout mode. -CJS */
void print(ch, row, col)
char ch;
int row;
int col;
{
  vtype tmp_str;

  row -= panel_row_prt;/* Real co-ords convert to screen positions */
  col -= panel_col_prt;
  if (mvaddch (row, col, ch) == ERR)
    {
      (void) sprintf(tmp_str, "error in print, row = %d col = %d\n", row, col);
      prt(tmp_str, 0, 0);
      bell ();
      /* wait so user can see error */
      (void) sleep(2);
    }
}


/* Moves the cursor to a given interpolated y, x position	-RAK-	*/
void move_cursor_relative(row, col)
int row;
int col;
{
  vtype tmp_str;

  row -= panel_row_prt;/* Real co-ords convert to screen positions */
  col -= panel_col_prt;
  if (move (row, col) == ERR)
    {
      (void) sprintf(tmp_str, "error in move_cursor_relative, row = %d col = %d\n", row, col);
      prt(tmp_str, 0, 0);
      bell();
      /* wait so user can see error */
      (void) sleep(2);
    }
}


/* Print a message so as not to interrupt a counted command. -CJS- */
void count_msg_print(p)
char *p;
{
  int i;

  i = command_count;
  msg_print(p);
  command_count = i;
}


/* Outputs a line to a given y, x position		-RAK-	*/
void prt(str_buff, row, col)
char *str_buff;
int row;
int col;
{
  if (row == MSG_LINE && msg_flag)
    msg_print(NULL);
  (void) move(row, col);
  clrtoeol();
  put_buffer(str_buff, row, col);
}


/* move cursor to a given y, x position */
void move_cursor(row, col)
int row, col;
{
  (void) move (row, col);
}


/* Outputs message to top line of screen				*/
/* These messages are kept for later reference.	 */
void msg_print(str_buff)
char *str_buff;
{
  register int old_len;
  char in_char;

  if (msg_flag)
    {
      old_len = strlen(old_msg[last_msg]) + 1;
      /* ensure that the complete -more- message is visible. */
      if (old_len > 73)
	old_len = 73;
      put_buffer(" -more-", MSG_LINE, old_len);
      /* let sigint handler know that we are waiting for a space */
      wait_for_more = 1;
      do
	{
	  in_char = inkey();
	}
      while ((in_char != ' ') && (in_char != ESCAPE) && (in_char != '\n') &&
	     (in_char != '\r'));
      wait_for_more = 0;
    }
  (void) move(MSG_LINE, 0);
  clrtoeol();

  /* Make the null string a special case.  -CJS- */
  if (str_buff)
    {
      put_buffer(str_buff, MSG_LINE, 0);
      command_count = 0;
      last_msg++;
      if (last_msg >= MAX_SAVE_MSG)
	last_msg = 0;
      (void) strncpy(old_msg[last_msg], str_buff, VTYPESIZ);
      old_msg[last_msg][VTYPESIZ - 1] = '\0';
      msg_flag = TRUE;
    }
  else
    msg_flag = FALSE;
}


/* Used to verify a choice - user gets the chance to abort choice.  -CJS- */
int get_check(prompt)
char *prompt;
{
  int res;
  int y, x;

  prt(prompt, 0, 0);
  getyx(stdscr, y, x);
  if (x > 73)
    move(0, 73);
  (void) addstr(" [y/n]");
  do
    {
      res = inkey();
    }
  while(res == ' ');
  erase_line(0, 0);
  if (res == 'Y' || res == 'y')
    return TRUE;
  else
    return FALSE;
}

/* Prompts (optional) and returns ord value of input char	*/
/* Function returns false if <ESCAPE> is input	*/
int get_com(prompt, command)
char *prompt;
char *command;
{
  int res;

  if (prompt)
    prt(prompt, 0, 0);
  *command = inkey();
  if (*command == 0 || *command == ESCAPE)
    res = FALSE;
  else
    res = TRUE;
  erase_line(MSG_LINE, 0);
  return(res);
}


/* Gets a string terminated by <RETURN>		*/
/* Function returns false if <ESCAPE> is input	*/
int get_string(in_str, row, column, slen)
char *in_str;
int row, column, slen;
{
  register int start_col, end_col, i;
  char *p;
  int flag, abort;

  abort = FALSE;
  flag	= FALSE;
  (void) move(row, column);
  for (i = slen; i > 0; i--)
    (void) addch(' ');
  (void) move(row, column);
  start_col = column;
  end_col = column + slen - 1;
  if (end_col > 79)
    {
      slen = 80 - column;
      end_col = 79;
    }
  p = in_str;
  do
    {
      i = inkey();
      switch(i)
	{
	case ESCAPE:
	  abort = TRUE;
	  break;
	case CTRL('J'): case CTRL('M'):
	  flag	= TRUE;
	  break;
	case DELETE: case CTRL('H'):
	  if (column > start_col)
	    {
	      column--;
	      put_buffer(" ", row, column);
	      move_cursor(row, column);
	      *--p = '\0';
	    }
	  break;
	default:
	  if (!isprint(i) || column > end_col)
	    bell();
	  else
	    {
	      (void) mvaddch(row, column, (char)i);
	      *p++ = i;
	      column++;
	    }
	  break;
	}
    }
  while ((!flag) && (!abort));
  if (abort)
    return(FALSE);
  /* Remove trailing blanks	*/
  while (p > in_str && p[-1] == ' ')
    p--;
  *p = '\0';
  return(TRUE);
}


/* Pauses for user response before returning		-RAK-	*/
void pause_line(prt_line)
int prt_line;
{
  prt("[Press any key to continue.]", prt_line, 23);
  (void) inkey();
  erase_line(prt_line, 0);
}


/* Pauses for user response before returning		-RAK-	*/
/* NOTE: Delay is for players trying to roll up "perfect"	*/
/*	characters.  Make them wait a bit.			*/
void pause_exit(prt_line, delay)
int prt_line;
int delay;
{
  char dummy;

  prt("[Press any key to continue, or Q to exit.]", prt_line, 10);
  dummy = inkey();
  if (dummy == 'Q')
    {
      erase_line(prt_line, 0);
#ifndef MSDOS		/* PCs are slow enough as is  -dgk */
      if (delay > 0)  (void) sleep((unsigned)delay);
#endif
      exit_game();
    }
  erase_line(prt_line, 0);
}

void save_screen()
{
  overwrite(stdscr, savescr);
}

void restore_screen()
{
  overwrite(savescr, stdscr);
  touchwin(stdscr);
}

void bell()
{
  put_qio();
  (void) write(1, "\007", 1);
}

/* definitions used by screen_map() */
/* index into border character array */
#define TL 0	/* top left */
#define TR 1
#define BL 2
#define BR 3
#define HE 4	/* horizontal edge */
#define VE 5

/* character set to use */
#ifdef MSDOS
# ifdef ANSI
#   define CH(x)	(ansi ? border[0][x] : border[1][x])
# else
#   define CH(x)	(border[1][x])
# endif
#else
#   define CH(x)	(border[0][x])
#endif

  /* Display highest priority object in the RATIO by RATIO area */
#define	RATIO 3

void screen_map()
{
  register int	i, j;
  static int8u border[2][6] = {
    {'+', '+', '+', '+', '-', '|'},	/* normal chars */
    {201, 187, 200, 188, 205, 186}	/* graphics chars */
  };
  int8u map[MAX_WIDTH / RATIO + 1];
  int8u tmp;
  int priority[256];
  int row, orow, col, myrow, mycol = 0;

  for (i = 0; i < 256; i++)
    priority[i] = 0;
  priority['<'] = 5;
  priority['>'] = 5;
  priority['@'] = 10;
#ifdef MSDOS
  priority[wallsym] = -5;
  priority[floorsym] = -10;
#else
  priority['#'] = -5;
  priority['.'] = -10;
#endif
  priority['\''] = -3;
  priority[' '] = -15;

  save_screen();
  clear_screen();
  (void) mvaddch(0, 0, CH(TL));
  for (i = 0; i < MAX_WIDTH / RATIO; i++)
    (void) addch(CH(HE));
  (void) addch(CH(TR));
  orow = -1;
  map[MAX_WIDTH / RATIO] = '\0';
  for (i = 0; i < MAX_HEIGHT; i++)
    {
      row = i / RATIO;
      if (row != orow)
	{
	  if (orow >= 0)
	    (void) mvprintw(orow+1, 0, "%c%s%c", CH(VE), map, CH(VE));
	  for (j = 0; j < MAX_WIDTH / RATIO; j++)
	    map[j] = ' ';
	  orow = row;
	}
      for (j = 0; j < MAX_WIDTH; j++)
	{
	  col = j / RATIO;
	  tmp = loc_symbol(i, j);
	  if (priority[map[col]] < priority[tmp])
	    map[col] = tmp;
	  if (map[col] == '@')
	    {
	      mycol = col + 1; /* account for border */
	      myrow = row + 1;
	    }
	}
    }
  if (orow >= 0)
    (void) mvprintw(orow + 1, 0, "%c%s%c", CH(VE), map, CH(VE));
  (void) mvaddch(orow + 2, 0, CH(BL));
  for (i = 0; i < MAX_WIDTH / RATIO; i++)
    (void) addch(CH(HE));
  (void) addch(CH(BR));
  (void) mvaddstr(23, 23, "Hit any key to continue");
  if (mycol > 0)
    (void) move(myrow, mycol);
  (void) inkey();
  restore_screen();
}
