#include <curses.h>
#include <signal.h>

#include "constants.h"
#include "config.h"
#include "types.h"
#include "externs.h"

#ifdef USG
#include <string.h>
#else
#include <strings.h>
#endif

extern int total_winner;
extern int moria_flag;
extern int search_flag;

#ifdef USG
/* no local special characters */
#else
extern struct ltchars save_special_chars;
#endif

int error_sig, error_code;

/* This signal package was brought to you by           -JEW-  */

/* on some systems, signal and suspend_handler must be declared as 'void'
   instead of 'int', this varies even among different OS releases, so there
   is no good way to solve the problem with ifdefs */
int (*signal())();
int signal_save_core();
int signal_save_no_core();
int signal_ask_quit();

#ifdef USG
/* no suspend signal */
#else
int (*suspend_handler)();
#endif

/* only allow one signal handler to execute, otherwise can get multiple save
       files by sending multiple signals */
static int caught_signal = 0;

init_signals()
{
  (void) signal(SIGHUP, signal_save_no_core);
  (void) signal(SIGINT, signal_ask_quit);
  (void) signal(SIGQUIT, signal_save_core);
  (void) signal(SIGILL, signal_save_core);
  (void) signal(SIGTRAP, signal_save_core);
  (void) signal(SIGIOT, signal_save_core);
  (void) signal(SIGEMT, signal_save_core);
  (void) signal(SIGFPE, signal_save_core);
  (void) signal(SIGKILL, signal_save_core);
  (void) signal(SIGBUS, signal_save_core);
  (void) signal(SIGSEGV, signal_save_core);
  (void) signal(SIGSYS, signal_save_core);
  (void) signal(SIGTERM, signal_save_core);
}

/*ARGSUSED*/
#ifdef USG
signal_save_core(sig)
int sig;
{
  if (caught_signal)
    return;
  caught_signal = 1;
  panic_save = 1;

  error_sig = sig;
  error_code = 0;
  if (!character_saved)
    {
      prt("OH NO!!!!!!!!!!  Attempting panic save.", 23, 0);
      save_char(FALSE, FALSE);
    }
  (void) signal(SIGQUIT, SIG_DFL);
  /* restore terminal settings */
#ifndef BUGGY_CURSES
  nl();
#endif
#if defined(ultrix)
  nocrmode();
#else
  nocbreak();
#endif
  echo();
  resetterm();
  /* restore the saved values of the local special chars */
  /* no local special characters */
  /* allow QUIT signal */
  /* nothing needs to be done here */
  (void) kill(getpid(), 3);
  exit_game();
}
#else
signal_save_core(sig, code, scp)
int sig, code;
struct sigcontext *scp;
{
  if (caught_signal)
    return;
  caught_signal = 1;
  panic_save = 1;

  error_sig = sig;
  error_code = code;
  if (!character_saved)
    {
      prt("OH NO!!!!!!!!!!  Attempting panic save.", 23, 0);
      save_char(FALSE, FALSE);
    }
  (void) signal(SIGQUIT, SIG_DFL);
  /* restore terminal settings */
#ifndef BUGGY_CURSES
  nl();
#endif
#if defined(ultrix)
  nocrmode();
#else
  nocbreak();
#endif
  echo();
  /* restore the saved values of the local special chars */
  (void) ioctl(0, TIOCSLTC, (char *)&save_special_chars);
  /* allow QUIT signal */
  (void) sigsetmask(0);
  (void) kill(getpid(), 3);
  exit_game();
}
#endif

/*ARGSUSED*/
#ifdef USG
signal_save_no_core(sig)
int sig;
{
  if (caught_signal)
    return;
  caught_signal = 1;
  panic_save = 1;

  error_sig = sig;
  error_code = 0;
  if (!character_saved)
    save_char(FALSE, TRUE);
  exit_game();
}
#else
signal_save_no_core(sig, code, scp)
int sig, code;
struct sigcontext *scp;
{
  if (caught_signal)
    return;
  caught_signal = 1;
  panic_save = 1;

  error_sig = sig;
  error_code = code;
  if (!character_saved)
    save_char(FALSE, TRUE);
  exit_game();
}
#endif

/*ARGSUSED*/
#ifdef USG
signal_ask_quit(sig)
int sig;
{
  char command;

  /* reset signal handler */
  (void) signal(sig, signal_ask_quit);
  find_flag = FALSE;
  if (search_flag)
    search_off();
  if (py.flags.rest > 0)
    {
      rest_off();
      return;
    }
  if (get_com("Do you really want to quit?", &command))
    switch(command)
      {
      case 'y': case 'Y':
        if (character_generated)
	  {
	    (void) strcpy(died_from, "Quitting.");
	    upon_death();
	  }
        else
          exit_game();
	break;
      }
  erase_line(MSG_LINE, 0);
  /* in case control-c typed during msg_print */
  if (wait_for_more)
    {
      put_buffer(" -more-", MSG_LINE, 0);
      put_qio();
    }
}
#else
signal_ask_quit(sig, code, scp)
int sig, code;
struct sigcontext *scp;
{
  char command;

  /* no need to reset signal handler */
  find_flag = FALSE;
  if (search_flag)
    search_off();
  if (py.flags.rest > 0)
    {
      rest_off();
      return;
    }
  if (get_com("Do you really want to quit?", &command))
    switch(command)
      {
      case 'y': case 'Y':
	if (character_generated)
	  {
	    (void) strcpy(died_from, "Quitting.");
	    upon_death();
	  }
	else
	  exit_game();
	break;
      }
  erase_line(MSG_LINE, 0);
  /* in case control-c typed during msg_print */
  if (wait_for_more)
    {
      put_buffer(" -more-", MSG_LINE, 0);
      put_qio();
    }
}
#endif

no_controlz()
{
#ifdef USG
  /* no suspend signal */
#else
  suspend_handler = signal(SIGTSTP, SIG_IGN);
#endif
}

controlz()
{
#ifdef USG
  /* no suspend signal */
#else
  (void) signal(SIGTSTP, suspend_handler);
#endif
}

ignore_signals()
{
  (void) signal(SIGINT, SIG_IGN);
  (void) signal(SIGQUIT, SIG_IGN);
}

default_signals()
{
  (void) signal(SIGINT, SIG_DFL);
  (void) signal(SIGQUIT, SIG_DFL);
}

restore_signals()
{
  (void) signal(SIGINT, signal_ask_quit);
  (void) signal(SIGQUIT, signal_save_core);
}
