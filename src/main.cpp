// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// Initialization, main() function and main loop

#include "headers.h"
#include "version.h"

static bool parseGameSeed(const char *argv, uint32_t &seed);

static const char *usage_instructions = R"(
Usage:
    umoria [OPTIONS] SAVEGAME

SAVEGAME is an optional save game filename (default: game.sav)

Options:
    -n           Force start of new game
    -d           Display high scores and exit
    -s NUMBER    Game Seed, as a decimal number (max: 2147483647)

    -v           Print version info and exit
    -h           Display this message
)";

// Initialize, restore, and get the ball rolling. -RAK-
int main(int argc, char *argv[]) {
    uint32_t seed = 0;
    bool new_game = false;

    // call this routine to grab a file pointer to the high score file
    // and prepare things to relinquish setuid privileges
    if (!initializeScoreFile()) {
        std::cerr << "Can't open score file '" << config::files::scores << "'\n";
        return 1;
    }

    // Make sure we have access to all files -MRC-
    if (!checkFilePermissions()) {
        return 1;
    }

    if (!terminalInitialize()) {
        return 1;
    }

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
            case 'd':
                showScoresScreen();
                exitProgram();
                break;
            case 's':
                // No NUMBER provided?
                if (argv[1] == nullptr) {
                    break;
                }

                // Move onto the NUMBER value
                --argc;
                ++argv;

                if (!parseGameSeed(argv[0], seed)) {
                    terminalRestore();
                    printf("Game seed must be a decimal number between 1 and 2147483647\n");
                    return -1;
                }

                break;
            case 'w':
                game.to_be_wizard = true;
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
        // (void) strcpy(config::files::save_game, argv[0]);
        config::files::save_game = argv[0];
    }

    startMoria(seed, new_game);

    return 0;
}

static bool parseGameSeed(const char *argv, uint32_t &seed) {
    int value;

    if (!stringToNumber(argv, value)) {
        return false;
    }
    if (value <= 0 || value > INT_MAX) {
        return false;
    }

    seed = (uint32_t) value;

    return true;
}
