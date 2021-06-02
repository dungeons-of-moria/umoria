// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Sets up curses for the correct system.

// clang-format off

#ifdef _WIN32
  // this is defined in Windows and also in ncurses
  #undef KEY_EVENT
  #ifdef _MSVC_LANG
    // On Microsoft Visual Studio 2019, this constant also needs to
    // be undefined.  Also, we need to use the PDCurses library rather
    // than a system library, and it has a different include file name.
    #undef MOUSE_MOVED
    #include <curses.h>
  #else
    #include <ncurses/ncurses.h>
  #endif
#elif __NetBSD__
  #include <curses.h>
#else
  #include <ncurses.h>
#endif

