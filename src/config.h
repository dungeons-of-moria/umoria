// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// Basic Configuration

namespace config {
    namespace files {
        extern const std::string splash_screen;
        extern const std::string welcome_screen;
        extern const std::string license;
        extern const std::string versions_history;
        extern const std::string help;
        extern const std::string help_wizard;
        extern const std::string help_roguelike;
        extern const std::string help_roguelike_wizard;
        extern const std::string death_tomb;
        extern const std::string death_royal;
        extern const std::string scores;
        extern std::string save_game;
    }

    namespace options {
        extern bool display_counts;
        extern bool find_bound;
        extern bool run_cut_corners;
        extern bool run_examine_corners;
        extern bool run_ignore_doors;
        extern bool run_print_self;
        extern bool highlight_seams;
        extern bool prompt_to_pickup;
        extern bool use_roguelike_keys;
        extern bool show_inventory_weights;
        extern bool error_beep_sound;
    }

    namespace dungeon {
        extern const uint8_t DUN_RANDOM_DIR;
        extern const uint8_t DUN_DIR_CHANGE;
        extern const uint8_t DUN_TUNNELING;
        extern const uint8_t DUN_ROOMS_MEAN;
        extern const uint8_t DUN_ROOM_DOORS;
        extern const uint8_t DUN_TUNNEL_DOORS;
        extern const uint8_t DUN_STREAMER_DENSITY;
        extern const uint8_t DUN_STREAMER_WIDTH;
        extern const uint8_t DUN_MAGMA_STREAMER;
        extern const uint8_t DUN_MAGMA_TREASURE;
        extern const uint8_t DUN_QUARTZ_STREAMER;
        extern const uint8_t DUN_QUARTZ_TREASURE;
        extern const uint16_t DUN_UNUSUAL_ROOMS;
    }

    namespace treasure {
        extern const uint8_t LEVEL_STD_OBJECT_ADJUST;
        extern const uint8_t LEVEL_MIN_OBJECT_STD;
        extern const uint8_t LEVEL_TOWN_OBJECTS;
        extern const uint8_t OBJECT_BASE_MAGIC;
        extern const uint8_t OBJECT_MAX_BASE_MAGIC;
        extern const uint8_t OBJECT_CHANCE_SPECIAL;
        extern const uint8_t OBJECT_CHANCE_CURSED;
        extern const uint16_t OBJECT_LAMP_MAX_CAPACITY;
        extern const uint8_t OBJECT_BOLTS_MAX_RANGE;
        extern const uint16_t OBJECTS_RUNE_PROTECTION;
    }

    namespace monsters {
        extern const uint8_t MON_CHANCE_OF_NEW;
        extern const uint8_t MON_MAX_SIGHT;
        extern const uint8_t MON_MAX_SPELL_CAST_DISTANCE;
        extern const uint8_t MON_MAX_MULTIPLY_PER_LEVEL;
        extern const uint8_t MON_MULTIPLY_ADJUST;
        extern const uint8_t MON_CHANCE_OF_NASTY;
        extern const uint8_t MON_MIN_PER_LEVEL;
        extern const uint8_t MON_MIN_TOWNSFOLK_DAY;
        extern const uint8_t MON_MIN_TOWNSFOLK_NIGHT;
        extern const uint8_t MON_ENDGAME_MONSTERS;
        extern const uint8_t MON_ENDGAME_LEVEL;
        extern const uint8_t MON_SUMMONED_LEVEL_ADJUST;
        extern const uint8_t MON_PLAYER_EXP_DRAINED_PER_HIT;
        extern const uint8_t MON_MIN_INDEX_ID;
        extern const uint8_t SCARE_MONSTER;
    }

    namespace player {
        extern const int32_t PLAYER_MAX_EXP;
        extern const uint8_t PLAYER_USE_DEVICE_DIFFICULTY;
        extern const uint16_t PLAYER_FOOD_FULL;
        extern const uint16_t PLAYER_FOOD_MAX;
        extern const uint16_t PLAYER_FOOD_FAINT;
        extern const uint16_t PLAYER_FOOD_WEAK;
        extern const uint16_t PLAYER_FOOD_ALERT;
        extern const uint8_t PLAYER_REGEN_FAINT;
        extern const uint8_t PLAYER_REGEN_WEAK;
        extern const uint8_t PLAYER_REGEN_NORMAL;
        extern const uint16_t PLAYER_REGEN_HPBASE;
        extern const uint16_t PLAYER_REGEN_MNBASE;
        extern const uint8_t PLAYER_WEIGHT_CAP;
    }
}
