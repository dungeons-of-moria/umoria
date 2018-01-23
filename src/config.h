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
}
