// src/curses.h: imports the correct curses for the system.
//
// Copyright (c) 1989-94 James E. Wilson, Robert A. Koeneke
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

#ifdef _WIN32
    #include <curses.h>
#else
    #include <ncurses.h>
#endif
