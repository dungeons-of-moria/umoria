/* mac/macio.c: terminal I/O code for the macintosh

   Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#ifdef THINK_C
#include "ScrnMgr.h"
#else
#include <scrnmgr.h>
#endif

#include <ctype.h>
#include <string.h>

#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"


void exit();
unsigned sleep();

/* Attributes of normal and hilighted characters */
#define ATTR_NORMAL	attrNormal
#define ATTR_HILITED	attrReversed

/* initializes curses routines */
void init_curses()
{
  /* Primary initialization is done in mac.c since game is restartable */
  /* Only need to clear the screen here */
  Rect scrn;

  scrn.left = scrn.top = 0;
  scrn.right = SCRN_COLS;
  scrn.bottom = SCRN_ROWS;
  EraseScreen(&scrn);
  UpdateScreen();
}

/* Set up the terminal into a suitable state for moria.	 -CJS- */
void moriaterm()
/* Nothing to do on Mac */
{
}

/* Dump IO to buffer					-RAK-	*/
void put_buffer(out_str, row, col)
char *out_str;
int row, col;
{
  /* The screen manager handles writes past the edge ok */
  DSetScreenCursor(col, row);
  DWriteScreenStringAttr(out_str, ATTR_NORMAL);
}

/* Dump the IO buffer to terminal			-RAK-	*/
void put_qio()
{
  screen_change = TRUE;	   /* Let inven_command know something has changed. */
  UpdateScreen();
}

/* Put the terminal in the original mode.			   -CJS- */
void restore_term()
/* Nothing to do on Mac */
{
}

void shell_out()
{
  alert_error("This command is not implemented on the Macintosh.");
}

/* Returns a single character input from the terminal.	This silently -CJS-
   consumes ^R to redraw the screen and reset the terminal, so that this
   operation can always be performed at any input prompt.  inkey() never
   returns ^R.	*/
char inkey()
/* The Mac does not need ^R, so it just consumes it */
/* This routine does nothing special with direction keys */
/* Just returns their keypad ascii value (e.g. '0'-'9') */
/* Compare with inkeydir() below */
{
  char ch;
  int dir;
  int shift_flag, ctrl_flag;

  put_qio();
  command_count = 0;

  do {
    macgetkey(&ch, FALSE);
  } while (ch == CTRL('R'));

  dir = extractdir(ch, &shift_flag, &ctrl_flag);
  if (dir != -1)
    ch = '0' + dir;

  return(ch);
}

char inkeydir()
/* The Mac does not need ^R, so it just consumes it */
/* This routine translates the direction keys in rogue-like mode */
{
  char ch;
  int dir;
  int shift_flag, ctrl_flag;
  static char tab[9] = {
	'b',		'j',		'n',
	'h',		'.',		'l',
	'y',		'k',		'u'
  };
  static char shifttab[9] = {
	'B',		'J',		'N',
	'H',		'.',		'L',
	'Y',		'K',		'U'
  };
  static char ctrltab[9] = {
	CTRL('B'),	CTRL('J'),	CTRL('N'),
	CTRL('H'),	'.',		CTRL('L'),
	CTRL('Y'),	CTRL('K'),	CTRL('U')
  };

  put_qio();
  command_count = 0;

  do {
    macgetkey(&ch, FALSE);
  } while (ch == CTRL('R'));

  dir = extractdir(ch, &shift_flag, &ctrl_flag);

  if (dir != -1) {
    if (!rogue_like_commands) {
      ch = '0' + dir;
    }
    else {
      if (ctrl_flag)
	ch = ctrltab[dir - 1];
      else if (shift_flag)
	ch = shifttab[dir - 1];
      else
	ch = tab[dir - 1];
    }
  }

  return(ch);
}

/* Flush the buffer					-RAK-	*/
void flush()
{
/* Removed put_qio() call.  Reduces flashing.  Doesn't seem to hurt. */
  FlushScreenKeys();
}

/* Clears given line of text				-RAK-	*/
void erase_line(row, col)
int row;
int col;
{
  Rect line;

  if (row == MSG_LINE && msg_flag)
    msg_print(NULL);

  line.left = col;
  line.top = row;
  line.right = SCRN_COLS;
  line.bottom = row + 1;
  DEraseScreen(&line);
}

/* Clears screen */
void clear_screen()
{
  Rect area;

  if (msg_flag)
    msg_print(NULL);

  area.left = area.top = 0;
  area.right = SCRN_COLS;
  area.bottom = SCRN_ROWS;
  DEraseScreen(&area);
}

void clear_from (row)
int row;
{
  Rect area;

  area.left = 0;
  area.top = row;
  area.right = SCRN_COLS;
  area.bottom = SCRN_ROWS;
  DEraseScreen(&area);
}

/* Outputs a char to a given interpolated y, x position	-RAK-	*/
/* sign bit of a character used to indicate standout mode. -CJS */
void print(ch, row, col)
char ch;
int row;
int col;
{
  char cnow, anow, cnew, anew;

  row -= panel_row_prt;/* Real co-ords convert to screen positions */
  col -= panel_col_prt;

  if (ch & 0x80) {
    cnew = ch & ~0x80;
    anew = ATTR_HILITED;
  }
  else {
    cnew = ch;
    anew = ATTR_NORMAL;
  }

  GetScreenCharAttr(&cnow, &anow, col, row);	/* Check current */

  if ((cnow != ch) || (anow != anew))	/* If char is already set, ignore op */
    DSetScreenCharAttr(cnew, anew, col, row);
}

/* Moves the cursor to a given interpolated y, x position	-RAK-	*/
void move_cursor_relative(row, col)
int row;
int col;
{
  row -= panel_row_prt;/* Real co-ords convert to screen positions */
  col -= panel_col_prt;

  DSetScreenCursor(col, row);
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
  Rect line;

  if (row == MSG_LINE && msg_flag)
    msg_print(NULL);

  line.left = col;
  line.top = row;
  line.right = SCRN_COLS;
  line.bottom = row + 1;
  DEraseScreen(&line);

  put_buffer(str_buff, row, col);
}

/* move cursor to a given y, x position */
void move_cursor(row, col)
int row, col;
{
  DSetScreenCursor(col, row);
}

/* Outputs message to top line of screen				*/
/* These messages are kept for later reference.	 */
void msg_print(str_buff)
char *str_buff;
{
  register int old_len;
  char in_char;
  Rect line;

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
  line.left = 0;
  line.top = MSG_LINE;
  line.right = SCRN_COLS;
  line.bottom = MSG_LINE+1;
  DEraseScreen(&line);

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
  long x, y;

  prt(prompt, 0, 0);
  GetScreenCursor(&x, &y);
  if (x > 73)
    DSetScreenCursor(73, y);
  DWriteScreenStringAttr(" [y/n]", ATTR_NORMAL);
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

/* Same as get_com(), but translates direction keys from keypad */
int get_comdir(prompt, command)
char *prompt;
char *command;
{
  int res;

  if (prompt)
    prt(prompt, 0, 0);
  *command = inkeydir();
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
  Rect area;

  abort = FALSE;
  flag	= FALSE;
  area.left = column;
  area.top = row;
  area.right = column + slen;
  area.bottom = row + 1;
  DEraseScreen(&area);
  DSetScreenCursor(column, row);
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
	      DSetScreenCursor(column, row);
	      DWriteScreenCharAttr((char) i, ATTR_NORMAL);
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
      if (delay > 0)  (void) sleep((unsigned)delay);
      exit_game();
    }
  erase_line(prt_line, 0);
}

void save_screen()
{
  mac_save_screen();
}

void restore_screen()
{
  mac_restore_screen();
}

void bell()
{
  put_qio();
  if (! sound_beep_flag)
    return;
  mac_beep();
}

/* definitions used by screen_map() */
/* index into border character array */
#define TL 0	/* top left */
#define TR 1
#define BL 2
#define BR 3
#define HE 4	/* horizontal edge */
#define VE 5

/* Display highest priority object in the RATIO by RATIO area */
#define	RATIO 3

void screen_map()
{
  register int	i, j;
  static int8u border[6] = {
    '+', '+', '+', '+', '-', '|'	/* normal chars */
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
  priority['#'] = -5;
  priority['.'] = -10;
  priority['\''] = -3;
  priority[' '] = -15;

  save_screen();
  clear_screen();
  DSetScreenCursor(0, 0);
  DWriteScreenCharAttr(border[TL], ATTR_NORMAL);
  for (i = 0; i < MAX_WIDTH / RATIO; i++)
    DWriteScreenCharAttr(border[HE], ATTR_NORMAL);
  DWriteScreenCharAttr(border[TR], ATTR_NORMAL);
  orow = -1;
  map[MAX_WIDTH / RATIO] = '\0';
  for (i = 0; i < MAX_HEIGHT; i++)
    {
      row = i / RATIO;
      if (row != orow)
	{
	  if (orow >= 0)
	    {
	      DSetScreenCursor(0, orow+1);
	      DWriteScreenCharAttr(border[VE], ATTR_NORMAL);
	      DWriteScreenString((char *)map);
	      DWriteScreenCharAttr(border[VE], ATTR_NORMAL);
	    }
	  for (j = 0; j < MAX_WIDTH / RATIO; j++)
	    map[j] = ' ';
	  orow = row;
	}
      for (j = 0; j < MAX_WIDTH; j++)
	{
	  col = j / RATIO;
	  tmp = loc_symbol(i, j);
	  /* Attributes are not handled correctly by DWriteScreenString */
	  /* Also, no special priority for the vein character */
	  if (tmp & 0x80)
	    tmp &= ~0x80;
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
    {
      DSetScreenCursor(0, orow+1);
      DWriteScreenCharAttr(border[VE], ATTR_NORMAL);
      DWriteScreenString((char *)map);
      DWriteScreenCharAttr(border[VE], ATTR_NORMAL);
    }
  DSetScreenCursor(0, orow + 2);
  DWriteScreenCharAttr(border[BL], ATTR_NORMAL);
  for (i = 0; i < MAX_WIDTH / RATIO; i++)
    DWriteScreenCharAttr(border[HE], ATTR_NORMAL);
  DWriteScreenCharAttr(border[BR], ATTR_NORMAL);
  DSetScreenCursor(23, 23);
  DWriteScreenStringAttr("Hit any key to continue", ATTR_NORMAL);
  if (mycol > 0)
    DSetScreenCursor(mycol, myrow);
  (void) inkey();
  restore_screen();
}
