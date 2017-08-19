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

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <limits>
#include <string>

#include <sys/stat.h>


// General Umoria headers

#include "constant.h"
#include "types.h"
#include "config.h"
