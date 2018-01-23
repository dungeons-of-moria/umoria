// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// Global variables with defaults
// clang-format off

#include "headers.h"

// A horrible hack, needed because compact_monster() is called from
// deep within updateMonsters() via monsterPlaceNew() and monsterSummon()
int hack_monptr = -1;

// High score file pointer
FILE *highscore_fp;

Game_t game = Game_t{};

// Track screen changes for inventory commands
bool screen_has_changed = false;

// Handle teleport traps
bool teleport_player;

bool message_ready_to_print;            // Set with first message
vtype_t messages[MESSAGE_HISTORY_SIZE]; // Saved message history -CJS-
int16_t last_message_id = 0;            // Index of last message held in saved messages array

int16_t missiles_counter = 0; // Counter for missiles
int eof_flag = 0;             // Is used to signal EOF/HANGUP condition
bool panic_save = false;      // True if playing from a panic save
