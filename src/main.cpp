// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Initialization, main() function and main loop

#include "headers.h"
#include "externs.h"
#include "version.h"

static const char *usage_instructions = R"(
Usage:
    umoria [OPTIONS] SAVEGAME

SAVEGAME is an optional save game filename (default: game.sav)

Options:
    -n    Force start of new game
    -r    Use classic roguelike keys: hjkl
    -s    Display high scores and exit
    -w    Enter Wizard mode!

    -v    Print version info and exit
    -h    Display this message
)";

// Initialize, restore, and get the ball rolling. -RAK-
int main(int argc, char *argv[]) {
    int seed = 0;
    bool new_game = false;
    bool roguelike_keys = false;

    // call this routine to grab a file pointer to the high score file
    // and prepare things to relinquish setuid privileges
    initializeScoreFile();

    // Make sure we have access to all files -MRC-
    checkFilePermissions();

    terminalInitialize();

    // check for user interface option
    for (--argc, ++argv; argc > 0 && argv[0][0] == '-'; --argc, ++argv) {
        switch (argv[0][1]) {
            case 'v':
                terminalRestore();
                printf("%d.%d.%d\n", CURRENT_VERSION_MAJOR, CURRENT_VERSION_MINOR, CURRENT_VERSION_PATCH);
                return 0;
            case 'n':
                new_game = true;
                break;
            case 'r':
                // This will force the use of roguelike keys,
                // ignoring the saved game file.
                roguelike_keys = true;
                break;
            case 's':
                showScoresScreen();
                exitGame();
                break;
            case 'w':
                to_be_wizard = true;

                if (isdigit((int) argv[0][2]) != 0) {
                    seed = atoi(&argv[0][2]);
                }
                break;
            default:
                terminalRestore();

                printf("Robert A. Koeneke's classic dungeon crawler.\n");
                printf("Umoria %d.%d.%d is released under a GPL v2 license.\n", CURRENT_VERSION_MAJOR, CURRENT_VERSION_MINOR, CURRENT_VERSION_PATCH);
                printf("%s", usage_instructions);
                return 0;
        }
    }

    // Auto-restart of saved file
    if (argv[0] != CNIL) {
        (void) strcpy(savegame_filename, argv[0]);
    } else {
        (void) strcpy(savegame_filename, MORIA_SAV);
    }

    startMoria(seed, new_game, roguelike_keys);

    return 0;
}
