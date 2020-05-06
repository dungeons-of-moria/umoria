// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

#pragma once

constexpr uint8_t TREASURE_MAX_LEVELS = 50; // Maximum level of magic in dungeon

// Note that the following constants are all related, if you change one, you
// must also change all succeeding ones.
// Also, player_base_provisions[] and store_choices[] may also have to be changed.
constexpr uint16_t MAX_OBJECTS_IN_GAME = 420; // Number of objects for universe
constexpr uint16_t MAX_DUNGEON_OBJECTS = 344; // Number of dungeon objects
constexpr uint16_t OBJECT_IDENT_SIZE = 448;   // 7*64, see object_offset() in desc.cpp, could be MAX_OBJECTS o_o() rewritten

// With LEVEL_MAX_OBJECTS set to 150, it's possible to get compacting
// objects during level generation, although it is extremely rare.
constexpr uint8_t LEVEL_MAX_OBJECTS = 175; // Max objects per level

// definitions for the pseudo-normal distribution generation
constexpr uint16_t NORMAL_TABLE_SIZE = 256;
constexpr uint8_t NORMAL_TABLE_SD = 64; // the standard deviation for the table

typedef struct {
    uint32_t magic_seed = 0; // Seed for initializing magic items (Potions, Wands, Staves, Scrolls, etc.)
    uint32_t town_seed = 0;  // Seed for town generation

    bool character_generated = false; // Don't save score until character generation is finished
    bool character_saved = false;     // Prevents save on kill after saving a character
    bool character_is_dead = false;   // `true` if character has died

    bool total_winner = false; // Character beat the Balrog

    bool teleport_player = false;  // Handle teleport traps
    bool player_free_turn = false; // Player has a free turn, so do not move creatures

    bool to_be_wizard = false; // Player requests to be Wizard - used during startup, when -w option used
    bool wizard_mode = false;  // Character is a Wizard when true
    int16_t noscore = 0;       // Don't save a score for this game. -CJS-

    bool use_last_direction = false;  // `true` when repeat commands should use last known direction
    char doing_inventory_command = 0; // Track inventory commands -CJS-
    char last_command = ' ';          // Save of the previous player command
    int command_count = 0;            // How many times to repeat a specific command -CJS-

    vtype_t character_died_from = {'\0'}; // What the character died from: starvation, Bat, etc.

    struct {
        int16_t current_id = 0; // Current treasure heap ptr
        Inventory_t list[LEVEL_MAX_OBJECTS]{};
    } treasure;
} Game_t;

extern Game_t game;

extern int16_t sorted_objects[MAX_DUNGEON_OBJECTS];
extern uint16_t normal_table[NORMAL_TABLE_SIZE];
extern int16_t treasure_levels[TREASURE_MAX_LEVELS + 1];

void seedsInitialize(uint32_t seed);
void seedSet(uint32_t seed);
void seedResetToOldSeed();
int randomNumber(int max);
int randomNumberNormalDistribution(int mean, int standard);
void setGameOptions();
bool validGameVersion(uint8_t major, uint8_t minor, uint8_t patch);
bool isCurrentGameVersion(uint8_t major, uint8_t minor, uint8_t patch);

int getRandomDirection();
bool getDirectionWithMemory(char *prompt, int &direction);
bool getAllDirections(const char *prompt, int &direction);

void exitProgram();
void abortProgram(const char *msg);

// game object management
int popt();
void pusht(uint8_t treasure_id);
int itemGetRandomObjectId(int level, bool must_be_small);

// game files
bool initializeScoreFile();
void displaySplashScreen();
void displayTextHelpFile(const std::string &filename);
void displayDeathFile(const std::string &filename);
void outputRandomLevelObjectsToFile();
bool outputPlayerCharacterToFile(char *filename);

// game death
void endGame();

// save/load
bool saveGame();
bool loadGame(bool &generate);
void setFileptr(FILE *file);

// game_run.cpp
// (includes the playDungeon() main game loop)
void startMoria(int seed, bool start_new_game);
