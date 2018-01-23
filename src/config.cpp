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
}
