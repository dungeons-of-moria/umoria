// src/unix.c: UNIX dependent code. -CJS-
//
// Copyright (C) 1989-1991 James E. Wilson, Robert A. Koeneke,
//
// This file is part of Umoria.
//
// Umoria is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Umoria is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Umoria.  If not, see <http://www.gnu.org/licenses/>.

#include "standard_library.h"
#include "curses.h"

#include "config.h"
#include "constant.h"
#include "types.h"

#include "externs.h"

// Provides for a timeout on input. Does a non-blocking read, consuming the data if
// any, and then returns 1 if data was read, zero otherwise.
//
// Porting:
//
// In systems without the select call, but with a sleep for fractional numbers of
// seconds, one could sleep for the time and then check for input.
//
// In systems which can only sleep for whole number of seconds, you might sleep by
// writing a lot of nulls to the terminal, and waiting for them to drain, or you
// might hack a static accumulation of times to wait. When the accumulation reaches
// a certain point, sleep for a second. There would need to be a way of resetting
// the count, with a call made for commands like run or rest.
bool check_input(int microsec) {
#ifdef _WIN32
    // Ugly non-blocking read...Ugh! -MRC-
    timeout(8);
    int result = getch();
    timeout(-1);

    if (result > 0) {
        return true;
    } else {
        return false;
    }
#else
    struct timeval tbuf;
    int ch;
    int smask;

    // Return true if a read on descriptor 1 will not block.
    tbuf.tv_sec = 0;
    tbuf.tv_usec = microsec;

    smask = 1; // i.e. (1 << 0)
    if (select(1, (fd_set *)&smask, (fd_set *)0, (fd_set *)0, &tbuf) == 1) {
        ch = getch();
        // check for EOF errors here, select sometimes works even when EOF
        if (ch == -1) {
            eof_flag++;
            return false;
        }
        return true;
    } else {
        return false;
    }
#endif
}

// Find a default user name from the system.
void user_name(char *buf) {
#ifdef _WIN32
    unsigned long bufCharCount = PLAYER_NAME_SIZE;

    if (!GetUserName(buf, &bufCharCount)) {
        (void)strcpy(buf, "X"); // Gotta have some name
    }
#else
    extern char *getlogin();

    char *p = getlogin();

    if (p && p[0]) {
        (void)strcpy(buf, p);
    } else {
        struct passwd *pwline = getpwuid((int)getuid());
        if (pwline) {
            (void)strcpy(buf, pwline->pw_name);
        }
    }
    if (!buf[0]) {
        (void)strcpy(buf, "X"); // Gotta have some name
    }
#endif
}

#ifndef _WIN32
// On unix based systems we should expand `~` to the users home path,
// otherwise on Windows we can ignore all of this. -MRC-

// undefine these so that tfopen and topen will work
#undef fopen
#undef open

// open a file just as does fopen, but allow a leading ~ to specify a home directory
FILE *tfopen(char *file, char *mode) {
    // extern int errno;

    char buf[1024];
    if (tilde(file, buf)) {
        return (fopen(buf, mode));
    }
    errno = ENOENT;
    return NULL;
}

// open a file just as does open, but expand a leading ~ into a home directory name
int topen(char *file, int flags, int mode) {
    // extern int errno;

    char buf[1024];
    if (tilde(file, buf)) {
        return (open(buf, flags, mode));
    }
    errno = ENOENT;
    return -1;
}

// expands a tilde at the beginning of a file name to a users home directory
int tilde(char *file, char *exp) {
    *exp = '\0';
    if (file) {
        if (*file == '~') {
            char user[128];
            struct passwd *pw = NULL;
            int i = 0;

            user[0] = '\0';
            file++;
            while (*file != '/' && i < sizeof(user)) {
                user[i++] = *file++;
            }
            user[i] = '\0';
            if (i == 0) {
                char *login = getlogin();

                if (login != NULL) {
                    (void)strcpy(user, login);
                } else if ((pw = getpwuid(getuid())) == NULL) {
                    return 0;
                }
            }
            if (pw == NULL && (pw = getpwnam(user)) == NULL) {
                return 0;
            }
            (void)strcpy(exp, pw->pw_dir);
        }
        (void)strcat(exp, file);
        return 1;
    }
    return 0;
}
#endif
