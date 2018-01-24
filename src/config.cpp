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
}
