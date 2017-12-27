// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Declarations for global variables and initialized data

extern int hack_monptr;

extern FILE *highscore_fp;

extern Config_t config;
extern Game_t game;
extern Dungeon_t dg;

extern bool screen_has_changed;

extern bool teleport_player;

extern bool message_ready_to_print;
extern vtype_t messages[MESSAGE_HISTORY_SIZE];
extern int16_t last_message_id;

// Converted to uint16_t when saving the game.
extern int16_t missiles_counter;

extern int eof_flag;
extern bool panic_save;

// Following are player variables
extern Player_t py;

extern ClassRankTitle_t class_rank_titles[PLAYER_MAX_CLASSES][PLAYER_MAX_LEVEL];
extern Race_t character_races[PLAYER_MAX_RACES];
extern Background_t character_backgrounds[PLAYER_MAX_BACKGROUNDS];

extern uint8_t race_gold_adjustments[PLAYER_MAX_RACES][PLAYER_MAX_RACES];

extern Class_t classes[PLAYER_MAX_CLASSES];
extern int16_t class_level_adj[PLAYER_MAX_CLASSES][CLASS_MAX_LEVEL_ADJUST];
extern uint16_t class_base_provisions[PLAYER_MAX_CLASSES][5];

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
extern DungeonObject_t game_objects[MAX_OBJECTS_IN_GAME];
extern uint8_t objects_identified[OBJECT_IDENT_SIZE];
extern int16_t treasure_levels[TREASURE_MAX_LEVELS + 1];
extern Inventory_t treasure_list[LEVEL_MAX_OBJECTS];
extern Inventory_t inventory[PLAYER_INVENTORY_SIZE];
extern char *special_item_names[SN_ARRAY_SIZE];
extern int16_t sorted_objects[MAX_DUNGEON_OBJECTS];

extern int16_t current_treasure_id;

// Following are creature arrays and variables
extern Creature_t creatures_list[MON_MAX_CREATURES];
extern Monster_t monsters[MON_TOTAL_ALLOCATIONS];
extern int16_t monster_levels[MON_MAX_LEVELS + 1];
extern MonsterAttack_t monster_attacks[MON_ATTACK_TYPES];
extern Recall_t creature_recall[MON_MAX_CREATURES]; // Monster memories. -CJS-
extern Monster_t blank_monster;
extern int16_t next_free_monster_id;
extern int16_t monster_multiply_total;
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

extern uint8_t blows_table[7][6];

extern uint16_t normal_table[NORMAL_TABLE_SIZE];


// Function return values
//
// Only extern functions declared here, static functions
// declared inside the file that defines them.

// create.c
void characterCreate();

// death.c
void exitGame();

// generate.c
void generateCave();

// los.cpp
bool los(int from_y, int from_x, int to_y, int to_x);
void look();

// moria.cpp - also includes the main dungeon game loop: playDungeon()
void startMoria(int seed, bool start_new_game, bool use_roguelike_keys);

// potions.c
void quaff();

// prayer.c
void pray();

// rng.cpp
uint32_t getRandomSeed();
void setRandomSeed(uint32_t seed);
int32_t rnd();

// save.c
bool saveGame();
bool loadGame(bool &generate);
void setFileptr(FILE *file);

// scores
void saveHighScore(const HighScore_t &score);
void readHighScore(HighScore_t &score);

// scrolls.c
void scrollRead();
