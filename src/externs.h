// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// Declarations for global variables and initialized data

extern uint8_t race_gold_adjustments[PLAYER_MAX_RACES][PLAYER_MAX_RACES];

// Warriors don't have spells, so there is no entry for them.
extern Spell_t magic_spells[PLAYER_MAX_CLASSES - 1][31];
extern char *spell_names[62];

// Following are store definitions
extern Owner_t store_owners[MAX_OWNERS];
extern Store_t stores[MAX_STORES];
extern uint16_t store_choices[MAX_STORES][STORE_MAX_ITEM_TYPES];
extern bool (*store_buy[MAX_STORES])(int);
extern const char *speech_sale_accepted[14];
extern const char *speech_selling_haggle_final[3];
extern const char *speech_selling_haggle[16];
extern const char *speech_buying_haggle_final[3];
extern const char *speech_buying_haggle[15];
extern const char *speech_insulted_haggling_done[5];
extern const char *speech_get_out_of_my_store[5];
extern const char *speech_haggling_try_again[10];
extern const char *speech_sorry[5];

// Following are treasure arrays  and variables
extern uint8_t objects_identified[OBJECT_IDENT_SIZE];
extern int16_t treasure_levels[TREASURE_MAX_LEVELS + 1];
extern Inventory_t inventory[PLAYER_INVENTORY_SIZE];
extern char *special_item_names[SN_ARRAY_SIZE];

// Following are creature arrays and variables
extern Recall_t creature_recall[MON_MAX_CREATURES]; // Monster memories. -CJS-
extern const char *recall_description_attack_type[25];
extern const char *recall_description_attack_method[20];
extern const char *recall_description_how_much[8];
extern const char *recall_description_move[6];
extern const char *recall_description_spell[15];
extern const char *recall_description_breath[5];
extern const char *recall_description_weakness[6];

// Following are arrays for descriptive pieces
extern const char *colors[MAX_COLORS];
extern const char *mushrooms[MAX_MUSHROOMS];
extern const char *woods[MAX_WOODS];
extern const char *metals[MAX_METALS];
extern const char *rocks[MAX_ROCKS];
extern const char *amulets[MAX_AMULETS];
extern const char *syllables[MAX_SYLLABLES];
