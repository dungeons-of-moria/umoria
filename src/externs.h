// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// Declarations for global variables and initialized data

// Following are treasure arrays  and variables
extern int16_t treasure_levels[TREASURE_MAX_LEVELS + 1];
extern Inventory_t inventory[PLAYER_INVENTORY_SIZE];

// Following are creature arrays and variables
extern Recall_t creature_recall[MON_MAX_CREATURES]; // Monster memories. -CJS-
extern const char *recall_description_attack_type[25];
extern const char *recall_description_attack_method[20];
extern const char *recall_description_how_much[8];
extern const char *recall_description_move[6];
extern const char *recall_description_spell[15];
extern const char *recall_description_breath[5];
extern const char *recall_description_weakness[6];
