// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Misc code, mainly handles player movement, inventory, etc

#include "headers.h"
#include "externs.h"

uint32_t inventoryCollectAllItemFlags() {
    uint32_t flags = 0;

    for (int i = EQUIPMENT_WIELD; i < EQUIPMENT_LIGHT; i++) {
        flags |= inventory[i].flags;
    }

    return flags;
}

// I may have written the town level code, but I'm not exactly
// proud of it.   Adding the stores required some real slucky
// hooks which I have not had time to re-think. -RAK-

// map roguelike direction commands into numbers
static char mapRoguelikeKeysToKeypad(char command) {
    switch (command) {
        case 'h':
            return '4';
        case 'y':
            return '7';
        case 'k':
            return '8';
        case 'u':
            return '9';
        case 'l':
            return '6';
        case 'n':
            return '3';
        case 'j':
            return '2';
        case 'b':
            return '1';
        case '.':
            return '5';
        default:
            return command;
    }
}

// Prompts for a direction -RAK-
// Direction memory added, for repeated commands.  -CJS
bool getDirectionWithMemory(char *prompt, int &direction) {
    static char prev_dir; // Direction memory. -CJS-

    // used in counted commands. -CJS-
    if (game.use_last_direction) {
        direction = prev_dir;
        return true;
    }

    if (prompt == CNIL) {
        prompt = (char *) "Which direction?";
    }

    char command;

    while (true) {
        // Don't end a counted command. -CJS-
        int save = game.command_count;

        if (!getCommand(prompt, command)) {
            game.player_free_turn = true;
            return false;
        }

        game.command_count = save;

        if (config.use_roguelike_keys) {
            command = mapRoguelikeKeysToKeypad(command);
        }

        if (command >= '1' && command <= '9' && command != '5') {
            prev_dir = command - '0';
            direction = prev_dir;
            return true;
        }

        terminalBellSound();
    }
}

// Similar to getDirectionWithMemory(), except that no memory exists,
// and it is allowed to enter the null direction. -CJS-
bool getAllDirections(const char *prompt, int &direction) {
    char command;

    while (true) {
        if (!getCommand(prompt, command)) {
            game.player_free_turn = true;
            return false;
        }

        if (config.use_roguelike_keys) {
            command = mapRoguelikeKeysToKeypad(command);
        }

        if (command >= '1' && command <= '9') {
            direction = command - '0';
            return true;
        }

        terminalBellSound();
    }
}
