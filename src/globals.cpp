// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Global variables with defaults
// clang-format off

#include "headers.h"

// A horrible hack, needed because compact_monster() is called from
// deep within creatures() via place_monster() and summon_monster()
int hack_monptr = -1;

vtype_t savegame_filename; // The save game filename -CJS-
FILE *highscore_fp;        // High score file pointer

// Game options as set on startup and with `=` set options command -CJS-
bool display_counts         = true;   // Display rest/repeat counts
bool find_bound             = false;  // Print yourself on a run (slower)
bool run_cut_corners        = true;   // Cut corners while running
bool run_examine_corners    = true;   // Check corners while running
bool run_ignore_doors       = false;  // Run through open doors
bool run_print_self         = false;  // Stop running when the map shifts
bool highlight_seams        = false;  // Highlight magma and quartz veins
bool prompt_to_pickup       = false;  // Prompt to pick something up
bool use_roguelike_keys     = false;  // Use classic Roguelike keys - set in config.h/main.c
bool show_inventory_weights = false;  // Display weights in inventory
bool error_beep_sound       = true;   // Beep for invalid characters

// Global flags
int16_t current_dungeon_level = 0;   // Current dungeon level
int32_t current_game_turn     = -1;  // Current turn of game

bool generate_new_level;           // True will generate a new level on next loop iteration
bool screen_has_changed  = false;  // Track screen changes for inventory commands

bool player_free_turn;             // Player has a free turn, so do not move creatures
int running_counter;               // Used in MORIA for .(dir)
bool teleport_player;              // Handle teleport traps

bool player_carrying_light;        // True when player is carrying light
bool weapon_is_heavy     = false;  // Weapon is too heavy -CJS-
int pack_heaviness       = 0;      // Heaviness of pack - used to calculate if pack is too heavy -CJS-

int32_t date_of_birth;             // Unix time for when the character was created
vtype_t died_from;                 // What the character died from: starvation, Bat, etc.
bool death               = false;  // True if character died

bool total_winner        = false;  // Character beat the Balrog
bool character_generated = false;  // Don't save score until character generation is finished
bool character_saved     = false;  // Prevents save on kill after saving a character

char doing_inven         = 0;      // Track inventory commands -CJS-
char last_command        = ' ';    // Save of the previous player command
int command_count;                 // How many times to repeat a specific command -CJS-
bool default_dir         = false;  // Direction to use for repeated commands

bool msg_flag;                     // Set with first message
vtype_t old_msgs[MAX_SAVE_MSG];    // Saved messages -CJS-
int16_t last_msg         = 0;      // Index of last message held in saved messages array

int16_t missile_ctr      = 0;      // Counter for missiles

uint32_t randes_seed;              // Seed for encoding colors / restarting randes_state
uint32_t town_seed;                // Seed for town generation

int eof_flag             = 0;      // Is used to signal EOF/HANGUP condition
bool panic_save          = false;  // True if playing from a panic save
int16_t noscore          = 0;      // Don't save a score for this game. -CJS-

bool to_be_wizard        = false;  // Player requests to be Wizard - used during startup, when -w option used
bool wizard              = false;  // Character is a Wizard when true

// Dungeon and display panel sizes
int16_t cur_height;  // Current dungeon height
int16_t cur_width;   // Current dungeon width
int16_t max_panel_rows, max_panel_cols;
int panel_row, panel_col;
int panel_row_min, panel_row_max;
int panel_col_min, panel_col_max;
int panel_col_prt, panel_row_prt;

// Floor definitions
Cave_t cave[MAX_HEIGHT][MAX_WIDTH];

// Player variables
bool light_flag             = false; // Track if temporary light about player.
int32_t character_max_score  = 0;    // Maximum score for a character

// Creature arrays and variables
Recall_t creature_recall[MAX_CREATURES]; // Monster memories
