/* source/signals.c: signal handlers

   Copyright (c) 1989-92 James E. Wilson, Christopher J. Stuart

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

/* This signal package was brought to you by		-JEW-  */
/* Completely rewritten by				-CJS- */

/* To find out what system we're on.  */

#include <stdio.h>

#include "config.h"
#include "constant.h"

/* Signals have no significance on the Mac */

#ifdef MAC

void nosignals()
{
}

void signals()
{
}

void init_signals()
{
}

#else /* a non-Mac system */

#ifdef ATARIST_MWC
/* need these for atari st, but for unix, must include signals.h first,
   or else suspend won't be properly declared */
#include "types.h"
#include "externs.h"
#endif

/* skip most of the file on an ATARI ST */
/* commented away most single handling for Atari ST TC too, as this
   doesn't work as it should.  */
#if !defined(ATARIST_MWC) && !defined(ATARIST_TC)

#if defined(SYS_V) && defined(lint)
/* for AIX, prevent hundreds of unnecessary lint errors, define before
   signal.h is included */
#define _h_IEEETRAP
typedef struct { int stuff; } fpvmach;
#endif

/* must include before externs.h, because that uses SIGTSTP */
#include <signal.h>

#include "types.h"
#include "externs.h"

#ifndef USG
/* only needed for Berkeley UNIX */
#include <sys/types.h>
#include <sys/param.h>
#endif

#ifdef USG
#ifndef ATARIST_MWC
#include <string.h>
#endif
#else
#include <strings.h>
#endif

#ifndef VMS
#ifdef USG
void exit();
#ifdef __TURBOC__
void sleep();
#else
unsigned sleep();
#endif
#endif
#endif

static int error_sig = -1;
static int signal_count = 0;

/*ARGSUSED*/
#ifndef USG
static int signal_handler(sig, code, scp)
int sig, code;
struct sigcontext *scp;
{
  int smask;

  smask = sigsetmask(0) | (1 << sig);
#else
#if defined(__TURBOC__) || defined(AMIGA)
static void signal_handler(sig)
#else
static int signal_handler(sig)
#endif
int sig;
{
#endif

  if(error_sig >= 0)	/* Ignore all second signals. */
    {
      if(++signal_count > 10)	/* Be safe. We will die if persistent enough. */
	(void) signal(sig, SIG_DFL);
      return;
    }
  error_sig = sig;

  /* Allow player to think twice. Wizard may force a core dump. */
  if (sig == SIGINT
#if !defined(MSDOS) && !defined(AMIGA) && !defined(ATARIST_TC)
      || sig == SIGQUIT
#endif
      )
    {
      if (death)
	(void) signal(sig, SIG_IGN);		/* Can't quit after death. */
      else if (!character_saved && character_generated)
	{
	  if (!get_check("Really commit *Suicide*?"))
	    {
	      if (turn > 0)
		disturb(1, 0);
	      erase_line(0, 0);
	      put_qio();
	      error_sig = -1;
#ifdef USG
	      (void) signal(sig, signal_handler);/* Have to restore handler. */
#else
	      (void) sigsetmask(smask);
#endif
	      /* in case control-c typed during msg_print */
	      if (wait_for_more)
		put_buffer(" -more-", MSG_LINE, 0);
	      put_qio();
	      return;		/* OK. We don't quit. */
	    }
	  (void) strcpy(died_from, "Interrupting");
	}
      else
	(void) strcpy(died_from, "Abortion");
      prt("Interrupt!", 0, 0);
      death = TRUE;
      exit_game();
    }
  /* Die. */
  prt(
"OH NO!!!!!!  A gruesome software bug LEAPS out at you. There is NO defense!",
      23, 0);
  if (!death && !character_saved && character_generated)
    {
      panic_save = 1;
      prt("Your guardian angel is trying to save you.", 0, 0);
      (void) sprintf(died_from,"(panic save %d)",sig);
      if (!save_char())
	{
	  (void) strcpy(died_from, "software bug");
	  death = TRUE;
	  turn = -1;
	}
    }
  else
    {
      death = TRUE;
      (void) _save_char(savefile);	/* Quietly save the memory anyway. */
    }
  restore_term();
#if !defined(MSDOS) && !defined(AMIGA) && !defined(ATARIST_TC)
  /* always generate a core dump */
  (void) signal(sig, SIG_DFL);
  (void) kill(getpid(), sig);
  (void) sleep(5);
#endif
  exit(1);
}

#endif /* ATARIST_MWC, ATARIST_TC */

#ifndef USG
static int mask;
#endif

void nosignals()
{
#if !defined(ATARIST_MWC) && !defined(ATARIST_TC)
#ifdef SIGTSTP
#if defined(atarist) && defined(__GNUC__)
  (void) signal(SIGTSTP, (__Sigfunc)SIG_IGN);
#else
  (void) signal(SIGTSTP, SIG_IGN);
#endif
#ifndef USG
  mask = sigsetmask(0);
#endif
#endif
  if (error_sig < 0)
    error_sig = 0;
#endif
}

void signals()
{
#if !defined(ATARIST_MWC) && !defined(ATARIST_TC)
#ifdef SIGTSTP
#if defined(atarist) && defined(__GNUC__)
  (void) signal(SIGTSTP, (__Sigfunc)suspend);
#else
  (void) signal(SIGTSTP, suspend);
#endif
#ifndef USG
  (void) sigsetmask(mask);
#endif
#endif
  if (error_sig == 0)
    error_sig = -1;
#endif
}


void init_signals()
{
#if !defined(ATARIST_MWC) && !defined(ATARIST_TC)
  /* No signals for Atari ST compiled with MWC or TC.  */
  (void) signal(SIGINT, signal_handler);

#if defined(atarist) && defined(__GNUC__)
  /* Atari ST compiled with GNUC has most signals, but we need a cast
     in every call to signal.  */
  (void) signal(SIGINT, (__Sigfunc)signal_handler);
  (void) signal(SIGQUIT, (__Sigfunc)signal_handler);
  (void) signal(SIGTSTP,(__Sigfunc)SIG_IGN);
  (void) signal(SIGILL, (__Sigfunc)signal_handler);
  (void) signal(SIGHUP, (__Sigfunc)SIG_IGN);
  (void) signal(SIGTRAP,(__Sigfunc)signal_handler);
  (void) signal(SIGIOT, (__Sigfunc)signal_handler);
  (void) signal(SIGEMT, (__Sigfunc)signal_handler);
  (void) signal(SIGKILL, (__Sigfunc)signal_handler);
  (void) signal(SIGBUS, (__Sigfunc)signal_handler);
  (void) signal(SIGSEGV, (__Sigfunc)signal_handler);
  (void) signal(SIGSYS, (__Sigfunc)signal_handler);
  (void) signal(SIGTERM,(__Sigfunc)signal_handler);
  (void) signal(SIGPIPE, (__Sigfunc)signal_handler);

#else
  /* Everybody except the atari st.  */
  (void) signal(SIGINT, signal_handler);
  (void) signal(SIGFPE, signal_handler);

#if defined(MSDOS)
  /* many fewer signals under MSDOS */
#else

#ifdef AMIGA
/*  (void) signal(SIGINT, signal_handler); */
  (void) signal(SIGTERM, signal_handler);
  (void) signal(SIGABRT, signal_handler);
/*  (void) signal(SIGFPE, signal_handler); */
  (void) signal(SIGILL, signal_handler);
  (void) signal(SIGSEGV, signal_handler);

#else

  /* Everybody except Atari, MSDOS, and Amiga.  */
  /* Ignore HANGUP, and let the EOF code take care of this case. */
  (void) signal(SIGHUP, SIG_IGN);
  (void) signal(SIGQUIT, signal_handler);
  (void) signal(SIGILL, signal_handler);
  (void) signal(SIGTRAP, signal_handler);
  (void) signal(SIGIOT, signal_handler);
#ifdef SIGEMT  /* in BSD systems */
  (void) signal(SIGEMT, signal_handler);
#endif
#ifdef SIGDANGER /* in SYSV systems */
  (void) signal(SIGDANGER, signal_handler);
#endif
  (void) signal(SIGKILL, signal_handler);
  (void) signal(SIGBUS, signal_handler);
  (void) signal(SIGSEGV, signal_handler);
  (void) signal(SIGSYS, signal_handler);
  (void) signal(SIGTERM, signal_handler);
  (void) signal(SIGPIPE, signal_handler);
#ifdef SIGXCPU	/* BSD */
  (void) signal(SIGXCPU, signal_handler);
#endif
#ifdef SIGPWR /* SYSV */
  (void) signal(SIGPWR, signal_handler);
#endif
#endif
#endif
#endif
#endif
}

void ignore_signals()
{
#if !defined(ATARIST_MWC)
  (void) signal(SIGINT, SIG_IGN);
#ifdef SIGQUIT
  (void) signal(SIGQUIT, SIG_IGN);
#endif
#endif
}

void default_signals()
{
#if !defined(ATARIST_MWC)
  (void) signal(SIGINT, SIG_DFL);
#ifdef SIGQUIT
  (void) signal(SIGQUIT, SIG_DFL);
#endif
#endif
}

void restore_signals()
{
#if !defined(ATARIST_MWC)
#if defined(atarist) && defined(__GNUC__)
  (void) signal(SIGINT, (__Sigfunc)signal_handler);
#else
  (void) signal(SIGINT, signal_handler);
#endif
#ifdef SIGQUIT
#if defined(atarist) && defined(__GNUC__)
  (void) signal(SIGQUIT, (__Sigfunc)signal_handler);
#else
  (void) signal(SIGQUIT, signal_handler);
#endif
#endif
#endif
}

#endif /* big Mac conditional */
