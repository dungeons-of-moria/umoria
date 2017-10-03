// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Global variables with defaults
// clang-format off

#include "headers.h"

// A horrible hack, needed because compact_monster() is called from
// deep within updateMonsters() via monsterPlaceNew() and monsterSummon()
int hack_monptr = -1;

FILE *highscore_fp;        // High score file pointer

// Game options as set on startup and with `=` set options command -CJS-
Config_t config = Config_t{};

int16_t current_dungeon_level = 0;      // Current dungeon level
int32_t current_game_turn     = -1;     // Current turn of game

bool generate_new_level;                // True will generate a new level on next loop iteration
bool screen_has_changed       = false;  // Track screen changes for inventory commands

bool player_free_turn;                  // Player has a free turn, so do not move creatures
bool teleport_player;                   // Handle teleport traps

bool total_winner             = false;  // Character beat the Balrog
bool character_generated      = false;  // Don't save score until character generation is finished
bool character_saved          = false;  // Prevents save on kill after saving a character
bool character_is_dead        = false;  // True if character has died
vtype_t character_died_from;            // What the character died from: starvation, Bat, etc.

char doing_inventory_command  = 0;      // Track inventory commands -CJS-
char last_command             = ' ';    // Save of the previous player command
int command_count;                      // How many times to repeat a specific command -CJS-
bool use_last_direction       = false;  // True when repeat commands should use last known direction

bool message_ready_to_print;            // Set with first message
vtype_t messages[MESSAGE_HISTORY_SIZE]; // Saved message history -CJS-
int16_t last_message_id       = 0;      // Index of last message held in saved messages array

int16_t missiles_counter      = 0;      // Counter for missiles

uint32_t magic_seed;                    // Seed for initializing magic items (Potions, Wands, Staves, Scrolls, etc.)
uint32_t town_seed;                     // Seed for town generation

int eof_flag                  = 0;      // Is used to signal EOF/HANGUP condition
bool panic_save               = false;  // True if playing from a panic save
int16_t noscore               = 0;      // Don't save a score for this game. -CJS-

bool to_be_wizard             = false;  // Player requests to be Wizard - used during startup, when -w option used
bool wizard_mode              = false;  // Character is a Wizard when true

// Dungeon size is either big enough for town level, or the dungeon itself
int16_t dungeon_height;
int16_t dungeon_width;

// Screen panels calculated from the dungeon/screen dimensions
int16_t max_panel_rows, max_panel_cols;
int panel_row, panel_col;
int panel_row_min, panel_row_max;
int panel_col_min, panel_col_max;
int panel_col_prt, panel_row_prt;

// Floor definitions
Cave_t cave[MAX_HEIGHT][MAX_WIDTH];

// Player variables
int32_t character_max_score  = 0;      // Maximum score for a character

// Creature arrays and variables
Recall_t creature_recall[MON_MAX_CREATURES]; // Monster memories
