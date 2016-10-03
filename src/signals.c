/* source/signals.c: signal handlers
 *
 * Copyright (C) 1989-2008 James E. Wilson, Robert A. Koeneke,
 *                         David J. Grabiner
 *
 * This file is part of Umoria.
 *
 * Umoria is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Umoria is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Umoria.  If not, see <http://www.gnu.org/licenses/>.
 */

/* This signal package was brought to you by  -JEW- */
/* Completely rewritten by                    -CJS- */

/* To find out what system we're on. */

#include <stdio.h>

#include "config.h"
#include "constant.h"

/* Signals have no significance on the Mac */

#ifdef MAC

void nosignals() {
}

void signals() {
}

void init_signals() {
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
#include <sys/param.h>
#include <sys/types.h>
#endif

#ifdef USG
#include <string.h>
#else
#include <strings.h>
#endif

#ifdef USG
void exit();
unsigned sleep();
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
    static int signal_handler(sig)
    int sig;
{
#endif

    /* Ignore all second signals. */
    if (error_sig >= 0) {
        /* Be safe. We will die if persistent enough. */
        if (++signal_count > 10) {
            (void)MSIGNAL(sig, SIG_DFL);
        }
        return;
    }
    error_sig = sig;

    /* Allow player to think twice. Wizard may force a core dump. */
    if (sig == SIGINT || sig == SIGQUIT) {
        if (death) {
            /* Can't quit after death. */
            (void)MSIGNAL(sig, SIG_IGN);
        } else if (!character_saved && character_generated) {
            if (!get_check("Really commit *Suicide*?")) {
                if (turn > 0) {
                    disturb(1, 0);
                }
                erase_line(0, 0);
                put_qio();
                error_sig = -1;
#ifdef USG
                /* Have to restore handler. */
                (void)MSIGNAL(sig, signal_handler);
#else
                (void)sigsetmask(smask);
#endif
                /* in case control-c typed during msg_print */
                if (wait_for_more) {
                    put_buffer(" -more-", MSG_LINE, 0);
                }
                put_qio();

                /* OK. We don't quit. */
                return;
            }
            (void)strcpy(died_from, "Interrupting");
        } else {
            (void)strcpy(died_from, "Abortion");
        }
        prt("Interrupt!", 0, 0);
        death = TRUE;
        exit_game();
    }

    /* Die. */
    prt("OH NO!!!!!!  A gruesome software bug LEAPS out at you. There is NO ""defense!", 23, 0);
    if (!death && !character_saved && character_generated) {
        panic_save = 1;
        prt("Your guardian angel is trying to save you.", 0, 0);
        (void)sprintf(died_from, "(panic save %d)", sig);
        if (!save_char()) {
            (void)strcpy(died_from, "software bug");
            death = TRUE;
            turn = -1;
        }
    } else {
        death = TRUE;

        /* Quietly save the memory anyway. */
        (void)_save_char(savefile);
    }
    restore_term();

    /* always generate a core dump */
    (void)MSIGNAL(sig, SIG_DFL);
    (void)kill(getpid(), sig);
    (void)sleep(5);
    exit(1);
}

#ifndef USG
static int mask;
#endif

void nosignals() {
#ifdef SIGTSTP
    (void)MSIGNAL(SIGTSTP, SIG_IGN);
#ifndef USG
    mask = sigsetmask(0);
#endif
#endif
    if (error_sig < 0) {
        error_sig = 0;
    }
}

void signals() {
#ifdef SIGTSTP
#ifdef __386BSD__
    (void)MSIGNAL(SIGTSTP, (sig_t)suspend);
#else
    (void)MSIGNAL(SIGTSTP, suspend);
#endif
#ifndef USG
    (void)sigsetmask(mask);
#endif
#endif
    if (error_sig == 0) {
        error_sig = -1;
    }
}

void init_signals() {
    (void)MSIGNAL(SIGINT, signal_handler);
    (void)MSIGNAL(SIGINT, signal_handler);
    (void)MSIGNAL(SIGFPE, signal_handler);

    /* Ignore HANGUP, and let the EOF code take care of this case. */
    (void)MSIGNAL(SIGHUP, SIG_IGN);
    (void)MSIGNAL(SIGQUIT, signal_handler);
    (void)MSIGNAL(SIGILL, signal_handler);
    (void)MSIGNAL(SIGTRAP, signal_handler);
    (void)MSIGNAL(SIGIOT, signal_handler);
#ifdef SIGEMT /* in BSD systems */
    (void)MSIGNAL(SIGEMT, signal_handler);
#endif
#ifdef SIGDANGER /* in SYSV systems */
    (void)MSIGNAL(SIGDANGER, signal_handler);
#endif
    (void)MSIGNAL(SIGKILL, signal_handler);
    (void)MSIGNAL(SIGBUS, signal_handler);
    (void)MSIGNAL(SIGSEGV, signal_handler);
#ifdef SIGSYS
    (void)MSIGNAL(SIGSYS, signal_handler);
#endif
    (void)MSIGNAL(SIGTERM, signal_handler);
    (void)MSIGNAL(SIGPIPE, signal_handler);
#ifdef SIGXCPU /* BSD */
    (void)MSIGNAL(SIGXCPU, signal_handler);
#endif
#ifdef SIGPWR /* SYSV */
    (void)MSIGNAL(SIGPWR, signal_handler);
#endif
}

void ignore_signals() {
    (void)MSIGNAL(SIGINT, SIG_IGN);
#ifdef SIGQUIT
    (void)MSIGNAL(SIGQUIT, SIG_IGN);
#endif
}

void default_signals() {
    (void)MSIGNAL(SIGINT, SIG_DFL);
#ifdef SIGQUIT
    (void)MSIGNAL(SIGQUIT, SIG_DFL);
#endif
}

void restore_signals() {
    (void)MSIGNAL(SIGINT, signal_handler);
#ifdef SIGQUIT
    (void)MSIGNAL(SIGQUIT, signal_handler);
#endif
}

#endif /* big Mac conditional */
