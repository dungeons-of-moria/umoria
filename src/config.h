// src/config.h: configuration definitions
//
// Copyright (C) 1989-2008 James E. Wilson, Robert A. Koeneke,
//                         David J. Grabiner
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

#define CONFIG_H_INCLUDED
#ifdef CONSTANT_H_INCLUDED
Constant.h should always be included after config.h,
because it uses some of the system defines set up here.
#endif

// NOTE: the wizard password and wizard uid are no longer used.
//
// Person to bother if something goes wrong.
// Recompile files.c and misc2.c if this changes.
#define WIZARD "David Grabiner <grabiner@alumni.princeton.edu>"

// Files used by moria, set these to valid pathnames for your system.
#define MORIA_GPL "LICENSE"
#define MORIA_TOP "scores.dat"
#define MORIA_SAV "savegame.dat"
#define MORIA_MOR "data/splash.hlp"
#define MORIA_HELP "data/roglcmds.hlp"
#define MORIA_ORIG_HELP "data/origcmds.hlp"
#define MORIA_WIZ_HELP "data/rwizcmds.hlp"
#define MORIA_OWIZ_HELP "data/owizcmds.hlp"
#define MORIA_WELCOME "data/welcome.hlp"
#define MORIA_VER "data/version.hlp"

// This sets the default user interface.
// To use the original key bindings (keypad for movement) set ROGUE_LIKE to false;
// to use the rogue-like key bindings (vi style movement) set ROGUE_LIKE to true.
// If you change this, you only need to recompile main.c.
#define ROGUE_LIKE false
