// Copyright (c) 1989-94 James E. Wilson, Robert A. Koeneke
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

#pragma once

typedef struct {
    uint32_t magic_seed = 0;              // Seed for initializing magic items (Potions, Wands, Staves, Scrolls, etc.)
    uint32_t town_seed = 0;               // Seed for town generation

    bool character_generated = false;     // Don't save score until character generation is finished
    bool character_saved = false;         // Prevents save on kill after saving a character
    bool character_is_dead = false;       // `true` if character has died

    bool total_winner = false;            // Character beat the Balrog

    bool player_free_turn = false;        // Player has a free turn, so do not move creatures
    bool to_be_wizard = false;            // Player requests to be Wizard - used during startup, when -w option used
    bool wizard_mode = false;             // Character is a Wizard when true
    int16_t noscore = 0;                  // Don't save a score for this game. -CJS-

    bool use_last_direction = false;      // `true` when repeat commands should use last known direction
    char doing_inventory_command = 0;     // Track inventory commands -CJS-
    char last_command = ' ';              // Save of the previous player command

    int command_count = 0;                // How many times to repeat a specific command -CJS-

    vtype_t character_died_from = {'\0'}; // What the character died from: starvation, Bat, etc.
} Game_t;
