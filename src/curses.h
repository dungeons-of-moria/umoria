// Copyright (c) 1989-94 James E. Wilson, Robert A. Koeneke
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Sets up curses for the correct system.

#pragma once

#ifdef _WIN32
    // this is defined in Windows and also in ncurses
    #undef KEY_EVENT
#endif

#include <ncurses.h>
