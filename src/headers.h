// Copyright (c) 1989-94 James E. Wilson, Robert A. Koeneke
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

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

#elif __APPLE__ ||  __linux__

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

#include "constant.h"
#include "dice.h"
#include "helpers.h"
#include "inventory.h"

#include "types.h"

#include "config.h"
#include "game.h"
#include "ui.h"
#include "dungeon.h"

#include "mage_spells.h"
#include "monster.h"
#include "player.h"
#include "scores.h"
#include "store.h"
