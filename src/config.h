// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Basic Configuration

// Data files used by Umoria
// NOTE: use relative paths to the executable binary.
typedef struct {
    std::string splash_screen = "data/splash.txt";
    std::string welcome_screen = "data/welcome.txt";

    std::string license = "LICENSE";
    std::string versions_history = "data/versions.txt";

    std::string help = "data/help.txt";
    std::string help_wizard = "data/help_wizard.txt";
    std::string help_roguelike = "data/rl_help.txt";
    std::string help_roguelike_wizard = "data/rl_help_wizard.txt";

    std::string death_tomb = "data/death_tomb.txt";
    std::string death_royal = "data/death_royal.txt";

    std::string save_game = "game.sav";
    std::string scores = "scores.dat";
} Files_t;

// Game configuration
// TODO: We would like to store this data in external config files, but for now
// TODO: this works, and is good preparation for that change in the future.
typedef struct {
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

    Files_t files = Files_t{};
} Config_t;
