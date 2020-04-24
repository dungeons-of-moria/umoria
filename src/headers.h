// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// System specific headers

// clang-format off
#pragma once

#ifdef _WIN32
    #define _CRT_SECURE_NO_WARNINGS
    #define _CRT_NONSTDC_NO_DEPRECATE
    #define WIN32_LEAN_AND_MEAN

    #include <windows.h>

    #include <io.h>
    #include <sys/types.h>

#elif __APPLE__ ||  __linux__ || __NetBSD__

    #include <pwd.h>
    #include <unistd.h>
    #include <sys/param.h>

#else
#   error "Unknown compiler"
#endif


// Headers we can use on all supported systems!

#include <cctype>
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <limits>
#include <string>

#include <fcntl.h>
#include <sys/stat.h>


// General Umoria headers
#include "config.h"
#include "types.h"

#include "character.h"
#include "dice.h"
#include "ui.h"           // before dungeon.h
#include "inventory.h"    // before game.h
#include "game.h"         // before dungeon.h
#include "dungeon_tile.h"
#include "dungeon.h"
#include "helpers.h"
#include "identification.h"
#include "mage_spells.h"
#include "monster.h"
#include "player.h"
#include "recall.h"
#include "rng.h"
#include "scores.h"
#include "scrolls.h"
#include "spells.h"
#include "staves.h"
#include "store.h"
#include "treasure.h"
#include "wizard.h"
