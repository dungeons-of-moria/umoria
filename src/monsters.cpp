// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Monster definitions
// clang-format off

#include "headers.h"

// Following are creature variables

Monster_t monsters[MON_TOTAL_ALLOCATIONS];
int16_t monster_levels[MON_MAX_LEVELS + 1];

// Values for a blank monster
Monster_t blank_monster = {0, 0, 0, 0, 0, 0, 0, false, 0, 0};

int16_t next_free_monster_id;    // ID for the next available monster ptr
int16_t monster_multiply_total;  // Total number of reproduction's of creatures
