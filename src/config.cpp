// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

#include "headers.h"

namespace config {
    // Data files used by Umoria
    // NOTE: use relative paths to the executable binary.
    namespace files {
        const std::string splash_screen = "data/splash.txt";
        const std::string welcome_screen = "data/welcome.txt";
        const std::string license = "LICENSE";
        const std::string versions_history = "data/versions.txt";
        const std::string help = "data/help.txt";
        const std::string help_wizard = "data/help_wizard.txt";
        const std::string help_roguelike = "data/rl_help.txt";
        const std::string help_roguelike_wizard = "data/rl_help_wizard.txt";
        const std::string death_tomb = "data/death_tomb.txt";
        const std::string death_royal = "data/death_royal.txt";
        const std::string scores = "scores.dat";
        std::string save_game = "game.sav";
    }

    // Game options as set on startup and with `=` set options command -CJS-
    namespace options {
        bool display_counts = true;          // Display rest/repeat counts
        bool find_bound = false;             // Print yourself on a run (slower)
        bool run_cut_corners = true;         // Cut corners while running
        bool run_examine_corners = true;     // Check corners while running
        bool run_ignore_doors = false;       // Run through open doors
        bool run_print_self = false;         // Stop running when the map shifts
        bool highlight_seams = false;        // Highlight magma and quartz veins
        bool prompt_to_pickup = false;       // Prompt to pick something up
        bool use_roguelike_keys = false;     // Use classic Roguelike keys
        bool show_inventory_weights = false; // Display weights in inventory
        bool error_beep_sound = true;        // Beep for invalid characters
    }

    // Dungeon generation values
    // Note: The entire design of dungeon can be changed by only slight adjustments here.
    namespace dungeon {
        const uint8_t DUN_RANDOM_DIR = 9;       // 1/Chance of Random direction
        const uint8_t DUN_DIR_CHANGE = 70;      // Chance of changing direction (99 max)
        const uint8_t DUN_TUNNELING = 15;       // Chance of extra tunneling
        const uint8_t DUN_ROOMS_MEAN = 32;      // Mean of # of rooms, standard dev2
        const uint8_t DUN_ROOM_DOORS = 25;      // % chance of room doors
        const uint8_t DUN_TUNNEL_DOORS = 15;    // % chance of doors at tunnel junctions
        const uint8_t DUN_STREAMER_DENSITY = 5; // Density of streamers
        const uint8_t DUN_STREAMER_WIDTH = 2;   // Width of streamers
        const uint8_t DUN_MAGMA_STREAMER = 3;   // Number of magma streamers
        const uint8_t DUN_MAGMA_TREASURE = 90;  // 1/x chance of treasure per magma
        const uint8_t DUN_QUARTZ_STREAMER = 2;  // Number of quartz streamers
        const uint8_t DUN_QUARTZ_TREASURE = 40; // 1/x chance of treasure per quartz
        const uint16_t DUN_UNUSUAL_ROOMS = 300; // Level/x chance of unusual room
    }

    // Magic Treasure Generation constants
    // Note: Number of special objects, and degree of enchantments can be adjusted here.
    namespace treasure {
        const uint8_t LEVEL_STD_OBJECT_ADJUST = 125; // Adjust STD per level * 100
        const uint8_t LEVEL_MIN_OBJECT_STD = 7;      // Minimum STD
        const uint8_t LEVEL_TOWN_OBJECTS = 7;        // Town object generation level
        const uint8_t OBJECT_BASE_MAGIC = 15;        // Base amount of magic
        const uint8_t OBJECT_MAX_BASE_MAGIC = 70;    // Max amount of magic
        const uint8_t OBJECT_CHANCE_SPECIAL = 6;     // magic_chance/# special magic
        const uint8_t OBJECT_CHANCE_CURSED = 13;     // 10*magic_chance/# cursed items

        // Constants describing limits of certain objects
        const uint16_t OBJECT_LAMP_MAX_CAPACITY = 15000; // Maximum amount that lamp can be filled
        const uint8_t OBJECT_BOLTS_MAX_RANGE = 18;       // Maximum range of bolts and balls
        const uint16_t OBJECTS_RUNE_PROTECTION = 3000;   // Rune of protection resistance
    }

    namespace monsters {
        const uint8_t MON_CHANCE_OF_NEW = 160;            // 1/x chance of new monster each round
        const uint8_t MON_MAX_SIGHT = 20;                 // Maximum dis a creature can be seen
        const uint8_t MON_MAX_SPELL_CAST_DISTANCE = 20;   // Maximum dis creature spell can be cast
        const uint8_t MON_MAX_MULTIPLY_PER_LEVEL = 75;    // Maximum reproductions on a level
        const uint8_t MON_MULTIPLY_ADJUST = 7;            // High value slows multiplication
        const uint8_t MON_CHANCE_OF_NASTY = 50;           // 1/x chance of high level creature
        const uint8_t MON_MIN_PER_LEVEL = 14;             // Minimum number of monsters/level
        const uint8_t MON_MIN_TOWNSFOLK_DAY = 4;          // Number of people on town level (day)
        const uint8_t MON_MIN_TOWNSFOLK_NIGHT = 8;        // Number of people on town level (night)
        const uint8_t MON_ENDGAME_MONSTERS = 2;           // Total number of "win" creatures
        const uint8_t MON_ENDGAME_LEVEL = 50;             // Level where winning creatures begin
        const uint8_t MON_SUMMONED_LEVEL_ADJUST = 2;      // Adjust level of summoned creatures
        const uint8_t MON_PLAYER_EXP_DRAINED_PER_HIT = 2; // Percent of player exp drained per hit
        const uint8_t MON_MIN_INDEX_ID = 2;               // Minimum index in m_list (1 = py, 0 = no mon)
        const uint8_t SCARE_MONSTER = 99;
    }

    namespace player {
        const int32_t PLAYER_MAX_EXP = 9999999L;        // Maximum amount of experience -CJS-
        const uint8_t PLAYER_USE_DEVICE_DIFFICULTY = 3; // x> Harder devices x< Easier devices
        const uint16_t PLAYER_FOOD_FULL = 10000;        // Getting full
        const uint16_t PLAYER_FOOD_MAX = 15000;         // Maximum food value, beyond is wasted
        const uint16_t PLAYER_FOOD_FAINT = 300;         // Character begins fainting
        const uint16_t PLAYER_FOOD_WEAK = 1000;         // Warn player that they're getting weak
        const uint16_t PLAYER_FOOD_ALERT = 2000;        // Alert player that they're getting low on food
        const uint8_t PLAYER_REGEN_FAINT = 33;          // Regen factor*2^16 when fainting
        const uint8_t PLAYER_REGEN_WEAK = 98;           // Regen factor*2^16 when weak
        const uint8_t PLAYER_REGEN_NORMAL = 197;        // Regen factor*2^16 when full
        const uint16_t PLAYER_REGEN_HPBASE = 1442;      // Min amount hp regen*2^16
        const uint16_t PLAYER_REGEN_MNBASE = 524;       // Min amount mana regen*2^16
        const uint8_t PLAYER_WEIGHT_CAP = 130;          // "#"*(1/10 pounds) per strength point
    }
}
