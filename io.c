#include <curses.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/file.h>

#include "constants.h"
#include "config.h"
#include "types.h"
#include "externs.h"

#ifdef USG
#include <string.h>
#else
#include <strings.h>
#include <sgtty.h>
#include <sys/wait.h>
#endif

#ifdef sun   /* correct SUN stupidity in the stdio.h file */
char *sprintf();
#endif

vtype pad_output;   /* static output string for the pad function */

char *getenv();

vtype old_msg[SAVED_MSGS];      /* Last messages      */
int last_message = 0;           /* Number of last msg generated in old_msg */
int last_displayed_msg = 0;     /* Number of last msg printed (^M) */
int repeating_old_msg = 0;      /* flag which lets repeat_msg call msg_print */

/* value of msg flag at start of turn */
extern int save_msg_flag;

#ifdef USG
void exit();
unsigned sleep();
#endif
#ifdef ultrix
void exit();
void sleep();
#endif

#ifdef USG
/* no local special characters */
#else
struct ltchars save_special_chars;
#endif

/* initializes curses routines */
init_curses()
{
#ifdef USG
  /* no local special characters */
#else
  struct ltchars buf;
#endif

#ifdef USG
  if (initscr() == NULL)
#else
  if (initscr() == ERR)
#endif
    {
      (void) printf("error allocating screen in curses package\n");
      exit_game();
    }
  clear();
#ifdef USG
  saveterm();
#endif
#if defined(ultrix)
  crmode();
#else
  cbreak();
#endif
  noecho();
#ifndef BUGGY_CURSES
  nonl();
#endif
  /* save old settings of the local special characters */
#ifdef USG
  /* no local special characters */
#else
  (void) ioctl(0, TIOCGLTC, (char *)&save_special_chars);
  /* disable all of the local special characters except the suspend char */
  /* have to disable ^Y for tunneling */
  buf.t_suspc = (char)26;  /* control-Z */
  buf.t_dsuspc = (char)-1;
  buf.t_rprntc = (char)-1;
  buf.t_flushc = (char)-1;
  buf.t_werasc = (char)-1;
  buf.t_lnextc = (char)-1;
  (void) ioctl(0, TIOCSLTC, (char *)&buf);
#endif
}


/* Dump IO to buffer					-RAK-	*/
put_buffer(out_str, row, col)
char *out_str;
int row, col;
{
  vtype tmp_str;

  if (mvaddstr(row, col, out_str) == ERR)
    {
      (void) sprintf(tmp_str, "error row = %d col = %d\n", row, col);
      prt(tmp_str, 0, 0);
      /* wait so user can see error */
      (void) sleep(2);
    }
}


/* Dump the IO buffer to terminal			-RAK-	*/
put_qio()
{
  refresh();
}


shell_out()
{
  int val;
  char *str;
#ifdef USG
  /* no local special characters */
#else
  struct ltchars buf;
#endif

  /* clear screen and print 'exit' message */
  clear_screen(0, 0);
  prt("[Entering shell, type 'exit' to resume your game]\n",0,0);
  put_qio();

#ifndef BUGGY_CURSES
  nl();
#endif
#if defined(ultrix)
  nocrmode();
#else
  nocbreak();
#endif
  echo();
  ignore_signals();
  val = fork();
  if (val == 0)
    {
      default_signals();
#ifdef USG
      /* no local special characters */
      resetterm();
#else
      (void) ioctl(0, TIOCSLTC, (char *)&save_special_chars);
#endif
      /* close scoreboard descriptor */
      (void) close(highscore_fd);
      if (str = getenv("SHELL"))
	(void) execl(str, str, (char *) 0);
      else
	(void) execl("/bin/sh", "sh", (char *) 0);
      msg_print("Cannot execute shell");
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
  restore_signals();
  /* restore the cave to the screen */
  really_clear_screen();
  draw_cave();
#if defined(ultrix)
  crmode();
#else
  cbreak();
#endif
  noecho();
#ifndef BUGGY_CURSES
  nonl();
#endif
  /* disable all of the local special characters except the suspend char */
  /* have to disable ^Y for tunneling */
#ifdef USG
  /* no local special characters */
#else
  buf.t_suspc = (char)26;  /* control-Z */
  buf.t_dsuspc = (char)-1;
  buf.t_rprntc = (char)-1;
  buf.t_flushc = (char)-1;
  buf.t_werasc = (char)-1;
  buf.t_lnextc = (char)-1;
  (void) ioctl(0, TIOCSLTC, (char *)&buf);
#endif
}

exit_game()
{
  /* restore the saved values of the local special chars */
  put_qio();	/* Dump any remaining buffer	*/
  endwin();     /* exit curses */
  echo();
#ifndef BUGGY_CURSES
  nl();
#endif
#if defined(ultrix)
  nocrmode();
#else
  nocbreak();
#endif
#ifdef USG
  /* no local special characters */
  resetterm();
#else
  (void) ioctl(0, TIOCSLTC, (char *)&save_special_chars);
#endif
  exit(0);	/* exit from game		*/
}


/* Gets single character from keyboard and returns		*/
inkey(ch)
char *ch;
{
  put_qio();			/* Dump IO buffer		*/
  *ch = getch();
  msg_flag = FALSE;
}


/* Flush the buffer					-RAK-	*/
flush()
{
#ifdef USG
  (void) ioctl(0, TCFLSH, 0);  /* flush the input queue */
#else
  int arg;

  arg = FREAD;
  (void) ioctl(0, TIOCFLUSH, (char *)&arg);   /* flush all input */
#endif
}


#if 0
/* this is no longer used anywhere */
/* Flush buffer before input				-RAK-	*/
inkey_flush(x)
char *x;
{
  if (!wizard1)  flush();
  inkey(x);
}
#endif

/* Clears given line of text				-RAK-	*/
erase_line(row, col)
int row;
int col;
{
  move(row, col);
  clrtoeol();
  if (row == MSG_LINE)
    msg_flag = FALSE;
}


/* Clears screen at given row, column				*/
clear_screen(row, col)
int row, col;
{
  register int i;

  for (i = row; i < 23; i++)
    used_line[i] = FALSE;
  move(row, col);
  clrtobot();
  msg_flag = FALSE;
}


/* Clears entire screen, even if there are characters that curses
   does not know about */
really_clear_screen()
{
  register int i;

  for (i = 1; i < 23; i++)
    used_line[i] = FALSE;
  clear();
  msg_flag = FALSE;
}


/* Outputs a line to a given interpolated y, x position	-RAK-	*/
print(str_buff, row, col)
char *str_buff;
int row;
int col;
{
  row -= panel_row_prt;/* Real co-ords convert to screen positions */
  col -= panel_col_prt;
  used_line[row] = TRUE;
  put_buffer(str_buff, row, col);
}


/* Outputs a line to a given y, x position		-RAK-	*/
prt(str_buff, row, col)
char *str_buff;
int row;
int col;
{
  move(row, col);
  clrtoeol();
  put_buffer(str_buff, row, col);
}


/* move cursor to a given y, x position */
move_cursor(row, col)
int row, col;
{
  move (row, col);
}


/* Outputs message to top line of screen				*/
msg_print(str_buff)
char *str_buff;
{
  register int old_len;
  char in_char;
  register int do_flush = 0;

  /* stop the character if s/he is in a run */
  if (find_flag)
    {
      find_flag = FALSE;
      move_light (char_row, char_col, char_row, char_col);
    }

  if (msg_flag)
    {
      old_len = strlen(old_msg[last_message]) + 1;
      put_buffer(" -more-", MSG_LINE, old_len);
      /* let sigint handler know that we are waiting for a space */
      wait_for_more = 1;
      do
	{
	  inkey(&in_char);
	}
      while ((in_char != ' ') && (in_char != '\033'));
      wait_for_more = 0;
      do_flush = 1;
    }
  move(MSG_LINE, 0);
  clrtoeol();
  if (do_flush)
    put_qio();
  put_buffer(str_buff, MSG_LINE, 0);

  if (!repeating_old_msg)
    {
      /* increment last message pointer */
      last_message++;
      if (last_message == SAVED_MSGS)
	last_message = 0;
      last_displayed_msg = last_message;
      (void) strcpy(old_msg[last_message], str_buff);
    }
  msg_flag = TRUE;
}


/* repeat an old message */
repeat_msg ()
{
  repeating_old_msg = 1;
  /* if message still visible, decrement counter to display previous one */
  if (save_msg_flag)
    {
      if (last_displayed_msg == 0)
	last_displayed_msg = SAVED_MSGS;
      last_displayed_msg--;
      msg_flag = FALSE;
      msg_print (old_msg[last_displayed_msg]);
    }
  else  /* display current message */
    msg_print (old_msg[last_displayed_msg]);
  repeating_old_msg = 0;
}

/* Prompts (optional) and returns ord value of input char	*/
/* Function returns false if <ESCAPE> is input	*/
get_com(prompt, command)
char *prompt;
char *command;
{
  int com_val;
  int res;

  if (strlen(prompt) > 1)
    prt(prompt, 0, 0);
  inkey(command);
  com_val = (*command);
  switch(com_val)
    {
    case 0: case 27:
      res = FALSE;
      break;
    default:
      res = TRUE;
      break;
    }
  erase_line(MSG_LINE, 0);
  msg_flag = FALSE;
  return(res);
}


/* Gets a string terminated by <RETURN>				*/
/* Function returns false if <ESCAPE>, CNTL/(Y, C, Z) is input	*/
int get_string(in_str, row, column, slen)
char *in_str;
int row, column, slen;
{
  register int start_col, end_col, i;
  char x;
  char tmp[2];
  int flag, abort;

  abort = FALSE;
  flag  = FALSE;
  in_str[0] = '\0';
  tmp[1] = '\0';
  put_buffer(pad(in_str, " ", slen), row, column);
  put_buffer("\0", row, column);
  start_col = column;
  end_col = column + slen - 1;
  do
    {
      inkey(&x);
      switch(x)
	{
	case 27:
	  abort = TRUE;
	  break;
	case 10: case 13:
	  flag  = TRUE;
	  break;
	case 127: case 8:
	  if (column > start_col)
	    {
	      column--;
	      put_buffer(" \b", row, column);
	      in_str[strlen(in_str)-1] = '\0';
	    }
	  break;
	default:
	  tmp[0] = x;
	  put_buffer(tmp, row, column);
	  (void) strcat(in_str, tmp);
	  column++;
	  if (column > end_col)
	    flag = TRUE;
	  break;
	}
    }
  while ((!flag) && (!abort));
  if (abort)
    return(FALSE);
  else
    {			/* Remove trailing blanks	*/
      i = strlen(in_str);
      if (i > 0)
	{
	  while ((in_str[i] == ' ') && (i > 0))
	    i--;
	  in_str[i+1] = '\0';
	}
    }
  return(TRUE);
}


/* Return integer value of hex string			-RAK-	*/
int get_hex_value(row, col, slen)
int row, col, slen;
{
  vtype tmp_str;
  int hex_value;

  hex_value = 0;
  if (get_string(tmp_str, row, col, slen))
    if (strlen(tmp_str) <= 8)
      {
	(void) sscanf(tmp_str, "%x", &hex_value);
      }
  return(hex_value);
}


/* Pauses for user response before returning		-RAK-	*/
pause_line(prt_line)
int prt_line;
{
  char dummy;

  prt("[Press any key to continue]", prt_line, 23);
  inkey(&dummy);
  erase_line(23, 0);
}


/* Pauses for user response before returning		-RAK-	*/
/* NOTE: Delay is for players trying to roll up "perfect"	*/
/*	characters.  Make them wait a bit...			*/
pause_exit(prt_line, delay)
int prt_line;
int delay;
{
  char dummy;

  prt("[Press any key to continue, or Q to exit]", prt_line, 10);
  inkey(&dummy);
  switch(dummy)
    {
    case 'Q':
      erase_line(prt_line, 0);
      if (delay > 0)  (void) sleep((unsigned)delay);
      exit_game();
      break;
    default:
      break;
    }
  erase_line(prt_line, 0);
}


/* pad a string with fill characters to specified length */
char *pad(string, fill, filllength)
char *string;
char *fill;
int filllength;
{
  register int length, i;

  (void) strcpy(pad_output, string);
  length = strlen(pad_output);
  for (i = length; i < filllength; i++)
    pad_output[i] = *fill;
  pad_output[i] = '\0';
  return(pad_output);
}

int confirm()
{
  char command;

  if (get_com("Are you sure?", &command))
    switch(command)
      {
      case 'y': case 'Y':
	return TRUE;

      default:
	return FALSE;
      }
  return FALSE;
}

