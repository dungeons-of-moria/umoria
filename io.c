#include <curses.h>
#ifdef USG
#include <string.h>
#else
#include <strings.h>
#include <sgtty.h>
#include <sys/wait.h>
#endif
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/file.h>

#include "constants.h"
#include "types.h"
#include "externs.h"

#ifdef sun   /* correct SUN stupidity in the stdio.h file */
char *sprintf();
#endif

vtype pad_output;   /* static output string for the pad function */

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
#ifdef ultrix
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
      (void) sleep(2);
    }
  refresh();
}


/* Dump the IO buffer to terminal			-RAK-	*/
/* NOTE: Source is PUTQIO.MAR					*/
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

#ifndef BUGGY_CURSES
  nl();
#endif
#ifdef ultrix
  nocrmode();
#else
  nocbreak();
#endif
  echo();
  val = fork();
  if (val == 0)
    {
#ifdef USG
      /* no local special characters */
      resetterm();
#else
      (void) ioctl(0, TIOCSLTC, (char *)&save_special_chars);
#endif
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
  really_clear_screen();
#ifdef ultrix
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
#ifdef ultrix
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

  /* Now flush				*/
  refresh();
}


/* Flush buffer before input				-RAK-	*/
inkey_flush(x)
char *x;
{
  put_qio();	/* Dup the IO buffer	*/
  if (!wizard1)  flush();
  inkey(x);
}


/* Clears given line of text				-RAK-	*/
erase_line(row, col)
int row;
int col;
{
  move(row, col);
  clrtoeol();
  refresh();
}


/* Clears screen at given row, column				*/
clear_screen(row, col)
int row, col;
{
  int i;

  for (i = 1; i <= 23; i++)
    used_line[i] = FALSE;
  move(row, col);
  clrtobot();
  put_qio();	/* Dump the Clear Sequence	*/
  msg_flag = FALSE;
}


/* Clears entire screen, even if there are characters that curses
   does not know about */
really_clear_screen()
{
  int i;

  for (i = 1; i <= 23; i++)
    used_line[i] = FALSE;
  clear();
  put_qio();	/* Dump the Clear Sequence	*/
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


/* Outputs message to top line of screen				*/
msg_print(str_buff)
char *str_buff;
{
  int old_len;
  char in_char;

  if (msg_flag)
    {
      old_len = strlen(old_msg) + 1;
      put_buffer(" -more-", msg_line, old_len);
      do
	{
	  inkey(&in_char);
	}
      while ((in_char != ' ') && (in_char != '\033'));
    }
  move(msg_line, msg_line);
  clrtoeol();
  put_buffer(str_buff, msg_line, msg_line);
  (void) strcpy(old_msg, str_buff);
  msg_flag = TRUE;
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
    case 27:
      res = FALSE;
      break;
    default:
      res = TRUE;
      break;
    }
  erase_line(msg_line, msg_line);
  msg_flag = FALSE;
  return(res);
}


/* Gets a string terminated by <RETURN>				*/
/* Function returns false if <ESCAPE>, CNTL/(Y, C, Z) is input	*/
int get_string(in_str, row, column, slen)
char *in_str;
int row, column, slen;
{
  int start_col, end_col, i;
  char x;
  vtype tmp;
  int flag, abort;

  abort = FALSE;
  flag  = FALSE;
  in_str[0] = '\0';
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
	  (void) sprintf(tmp, "%c", x);
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

  prt("[Press any key to continue, or ESC to exit]", prt_line, 10);
  inkey(&dummy);
  switch(dummy)
    {
    case 27:
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
  int length;
  int i;

  (void) strcpy(pad_output, string);
  length = strlen(pad_output);
  for (i = length; i < filllength; i++)
    pad_output[i] = *fill;
  pad_output[i] = '\0';
  return(pad_output);
}
