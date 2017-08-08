// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Basic Configuration

// Data files used by Umoria - relative to the executable binary.
#define MORIA_GPL "LICENSE"
#define MORIA_TOP "scores.dat"
#define MORIA_SAV "game.sav"
#define MORIA_MOR "data/splash.hlp"
#define MORIA_HELP "data/roglcmds.hlp"
#define MORIA_ORIG_HELP "data/origcmds.hlp"
#define MORIA_WIZ_HELP "data/rwizcmds.hlp"
#define MORIA_OWIZ_HELP "data/owizcmds.hlp"
#define MORIA_WELCOME "data/welcome.hlp"
#define MORIA_VER "data/version.hlp"

// Game options as set on startup and with `=` set options command -CJS-
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
} Config_t;
