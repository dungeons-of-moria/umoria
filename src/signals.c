/* source/signals.c: signal handlers

   Copyright (C) 1989-2008 James E. Wilson, Robert A. Koeneke, 
                           David J. Grabiner

   This file is part of Umoria.

   Umoria is free software; you can redistribute it and/or modify 
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Umoria is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of 
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License 
   along with Umoria.  If not, see <http://www.gnu.org/licenses/>. */

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

/* Since libc6, linux (Debian, at least) defaults to BSD signal().  This */
/* expects SYSV.  Thus, DEBIAN_LINUX uses the sysv_signal call, everyone */
/* else uses just signal.  RJW 00_0528 */

#ifdef DEBIAN_LINUX
#define MSIGNAL sysv_signal
#else
#define MSIGNAL signal
#endif

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
#ifdef __386BSD__
static void signal_handler(sig, code, scp)
#else
static int signal_handler(sig, code, scp)
#endif
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
	(void) MSIGNAL(sig, SIG_DFL);
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
	(void) MSIGNAL(sig, SIG_IGN);		/* Can't quit after death. */
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
	      (void) MSIGNAL(sig, signal_handler);/* Have to restore handler. */
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
  (void) MSIGNAL(sig, SIG_DFL);
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
  (void) MSIGNAL(SIGTSTP, (__Sigfunc)SIG_IGN);
#else
  (void) MSIGNAL(SIGTSTP, SIG_IGN);
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
  (void) MSIGNAL(SIGTSTP, (__Sigfunc)suspend);
#else
#ifdef  __386BSD__
  (void) MSIGNAL(SIGTSTP, (sig_t)suspend);
#else
  (void) MSIGNAL(SIGTSTP, suspend);
#endif
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
  (void) MSIGNAL(SIGINT, signal_handler);

#if defined(atarist) && defined(__GNUC__)
  /* Atari ST compiled with GNUC has most signals, but we need a cast
     in every call to signal.  */
  (void) MSIGNAL(SIGINT, (__Sigfunc)signal_handler);
  (void) MSIGNAL(SIGQUIT, (__Sigfunc)signal_handler);
  (void) MSIGNAL(SIGTSTP,(__Sigfunc)SIG_IGN);
  (void) MSIGNAL(SIGILL, (__Sigfunc)signal_handler);
  (void) MSIGNAL(SIGHUP, (__Sigfunc)SIG_IGN);
  (void) MSIGNAL(SIGTRAP,(__Sigfunc)signal_handler);
  (void) MSIGNAL(SIGIOT, (__Sigfunc)signal_handler);
  (void) MSIGNAL(SIGEMT, (__Sigfunc)signal_handler);
  (void) MSIGNAL(SIGKILL, (__Sigfunc)signal_handler);
  (void) MSIGNAL(SIGBUS, (__Sigfunc)signal_handler);
  (void) MSIGNAL(SIGSEGV, (__Sigfunc)signal_handler);
  (void) MSIGNAL(SIGSYS, (__Sigfunc)signal_handler);
  (void) MSIGNAL(SIGTERM,(__Sigfunc)signal_handler);
  (void) MSIGNAL(SIGPIPE, (__Sigfunc)signal_handler);

#else
  /* Everybody except the atari st.  */
  (void) MSIGNAL(SIGINT, signal_handler);
  (void) MSIGNAL(SIGFPE, signal_handler);

#if defined(MSDOS)
  /* many fewer signals under MSDOS */
#else

#ifdef AMIGA
/*  (void) MSIGNAL(SIGINT, signal_handler); */
  (void) MSIGNAL(SIGTERM, signal_handler);
  (void) MSIGNAL(SIGABRT, signal_handler);
/*  (void) MSIGNAL(SIGFPE, signal_handler); */
  (void) MSIGNAL(SIGILL, signal_handler);
  (void) MSIGNAL(SIGSEGV, signal_handler);

#else

  /* Everybody except Atari, MSDOS, and Amiga.  */
  /* Ignore HANGUP, and let the EOF code take care of this case. */
  (void) MSIGNAL(SIGHUP, SIG_IGN);
  (void) MSIGNAL(SIGQUIT, signal_handler);
  (void) MSIGNAL(SIGILL, signal_handler);
  (void) MSIGNAL(SIGTRAP, signal_handler);
  (void) MSIGNAL(SIGIOT, signal_handler);
#ifdef SIGEMT  /* in BSD systems */
  (void) MSIGNAL(SIGEMT, signal_handler);
#endif
#ifdef SIGDANGER /* in SYSV systems */
  (void) MSIGNAL(SIGDANGER, signal_handler);
#endif
  (void) MSIGNAL(SIGKILL, signal_handler);
  (void) MSIGNAL(SIGBUS, signal_handler);
  (void) MSIGNAL(SIGSEGV, signal_handler);
#ifdef SIGSYS
  (void) MSIGNAL(SIGSYS, signal_handler);
#endif
  (void) MSIGNAL(SIGTERM, signal_handler);
  (void) MSIGNAL(SIGPIPE, signal_handler);
#ifdef SIGXCPU	/* BSD */
  (void) MSIGNAL(SIGXCPU, signal_handler);
#endif
#ifdef SIGPWR /* SYSV */
  (void) MSIGNAL(SIGPWR, signal_handler);
#endif
#endif
#endif
#endif
#endif
}

void ignore_signals()
{
#if !defined(ATARIST_MWC)
  (void) MSIGNAL(SIGINT, SIG_IGN);
#ifdef SIGQUIT
  (void) MSIGNAL(SIGQUIT, SIG_IGN);
#endif
#endif
}

void default_signals()
{
#if !defined(ATARIST_MWC)
  (void) MSIGNAL(SIGINT, SIG_DFL);
#ifdef SIGQUIT
  (void) MSIGNAL(SIGQUIT, SIG_DFL);
#endif
#endif
}

void restore_signals()
{
#if !defined(ATARIST_MWC)
#if defined(atarist) && defined(__GNUC__)
  (void) MSIGNAL(SIGINT, (__Sigfunc)signal_handler);
#else
  (void) MSIGNAL(SIGINT, signal_handler);
#endif
#ifdef SIGQUIT
#if defined(atarist) && defined(__GNUC__)
  (void) MSIGNAL(SIGQUIT, (__Sigfunc)signal_handler);
#else
  (void) MSIGNAL(SIGQUIT, signal_handler);
#endif
#endif
#endif
}

#endif /* big Mac conditional */
