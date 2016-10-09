/* source/config.h: configuration definitions
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

#define CONFIG_H_INCLUDED
#ifdef CONSTANT_H_INCLUDED
Constant.h should always be included after config.h,
because it uses some of the system defines set up here.
#endif

/* NOTE: the wizard password and wizard uid are no longer used.
 *
 * Person to bother if something goes wrong.
 * Recompile files.c and misc2.c if this changes.
 */
#define WIZARD "David Grabiner <grabiner@alumni.princeton.edu>"

/*
 * System definitions.
 *
 * You must define one of these as appropriate for the system
 * you are compiling moria on.
 */

/* If compiling on Debian (also works on other versions of Linux), define this. */
#define DEBIAN_LINUX

/* If you are compiling on a SYS V version of UNIX, define this. */
/* #define SYS_V */

/* Files used by moria, set these to valid pathnames for your system. */
#define MORIA_SAV "moria-save"
#define MORIA_MOR "files/news"
#define MORIA_GPL "files/COPYING"
#define MORIA_TOP "files/scores"
#define MORIA_HELP "files/roglcmds.hlp"
#define MORIA_ORIG_HELP "files/origcmds.hlp"
#define MORIA_WIZ_HELP "files/rwizcmds.hlp"
#define MORIA_OWIZ_HELP "files/owizcmds.hlp"
#define MORIA_WELCOME "files/welcome.hlp"
#define MORIA_VER "files/version.hlp"

/* This sets the default user interface.
 * To use the original key bindings (keypad for movement) set ROGUE_LIKE to FALSE;
 * to use the rogue-like key bindings (vi style movement) set ROGUE_LIKE to TRUE.
 * If you change this, you only need to recompile main.c.
 */
#define ROGUE_LIKE FALSE

/*
 * System dependent defines follow.
 *
 * You should not need to change anything below.
 */

/* Linux supports System V */
#if defined(__linux__)
#define SYS_V
#endif

/* Define USG for many systems, this is basically to select SYS V style
 * system calls (as opposed to BSD style).
 */
#if defined(SYS_V)
#ifndef USG
#define USG
#endif
#endif
