// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Basic Configuration

// Data files used by Umoria - relative to the executable binary.
#define MORIA_GPL "LICENSE"
#define MORIA_TOP "scores.dat"
#define MORIA_SAV "game.sav"
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
constexpr bool ROGUE_LIKE = false;
