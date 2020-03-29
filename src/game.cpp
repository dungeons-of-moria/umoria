// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// Game initialization and maintenance related functions

#include "headers.h"
#include "version.h"

// holds the previous rnd state
static uint32_t old_seed;

Game_t game = Game_t{};

// gets a new random seed for the random number generator
void seedsInitialize(uint32_t seed) {
    uint32_t clock_var;

    if (seed == 0) {
        clock_var = getCurrentUnixTime();
    } else {
        clock_var = seed;
    }

    game.magic_seed = (int32_t) clock_var;

    clock_var += 8762;
    game.town_seed = (int32_t) clock_var;

    clock_var += 113452L;
    setRandomSeed(clock_var);

    // make it a little more random
    for (clock_var = (uint32_t) randomNumber(100); clock_var != 0; clock_var--) {
        (void) rnd();
    }
}

// change to different random number generator state
void seedSet(uint32_t seed) {
    old_seed = getRandomSeed();

    // want reproducible state here
    setRandomSeed(seed);
}

// restore the normal random generator state
void seedResetToOldSeed() {
    setRandomSeed(old_seed);
}

// Generates a random integer x where 1<=X<=MAXVAL -RAK-
int randomNumber(int const max) {
    return (rnd() % max) + 1;
}

// Generates a random integer number of NORMAL distribution -RAK-
int randomNumberNormalDistribution(int mean, int standard) {
    // alternate randomNumberNormalDistribution() code, slower but much smaller since no table
    // 2 per 1,000,000 will be > 4*SD, max is 5*SD
    //
    // tmp = diceRoll(8, 99);             // mean 400, SD 81
    // tmp = (tmp - 400) * standard / 81;
    // return tmp + mean;

    int tmp = randomNumber(SHRT_MAX);

    // off scale, assign random value between 4 and 5 times SD
    if (tmp == SHRT_MAX) {
        int offset = 4 * standard + randomNumber(standard);

        // one half are negative
        if (randomNumber(2) == 1) {
            offset = -offset;
        }

        return mean + offset;
    }

    // binary search normal normal_table to get index that
    // matches tmp this takes up to 8 iterations.
    int low = 0;
    int iindex = NORMAL_TABLE_SIZE >> 1;
    int high = NORMAL_TABLE_SIZE;

    while (true) {
        if (normal_table[iindex] == tmp || high == low + 1) {
            break;
        }

        if (normal_table[iindex] > tmp) {
            high = iindex;
            iindex = low + ((iindex - low) >> 1);
        } else {
            low = iindex;
            iindex = iindex + ((high - iindex) >> 1);
        }
    }

    // might end up one below target, check that here
    if (normal_table[iindex] < tmp) {
        iindex = iindex + 1;
    }

    // normal_table is based on SD of 64, so adjust the
    // index value here, round the half way case up.
    int offset = ((standard * iindex) + (NORMAL_TABLE_SD >> 1)) / NORMAL_TABLE_SD;

    // one half should be negative
    if (randomNumber(2) == 1) {
        offset = -offset;
    }

    return mean + offset;
}

static struct {
    const char *o_prompt;
    bool *o_var;
} game_options[] = {
    {"Running: cut known corners", &config::options::run_cut_corners},
    {"Running: examine potential corners", &config::options::run_examine_corners},
    {"Running: print self during run", &config::options::run_print_self},
    {"Running: stop when map sector changes", &config::options::find_bound},
    {"Running: run through open doors", &config::options::run_ignore_doors},
    {"Prompt to pick up objects", &config::options::prompt_to_pickup},
    {"Rogue like commands", &config::options::use_roguelike_keys},
    {"Show weights in inventory", &config::options::show_inventory_weights},
    {"Highlight and notice mineral seams", &config::options::highlight_seams},
    {"Beep for invalid character", &config::options::error_beep_sound},
    {"Display rest/repeat counts", &config::options::display_counts},
    {nullptr, nullptr},
};

// Set or unset various boolean config::options::display_counts -CJS-
void setGameOptions() {
    putStringClearToEOL("  ESC when finished, y/n to set options, <return> or - to move cursor", Coord_t{0, 0});

    int max;
    for (max = 0; game_options[max].o_prompt != nullptr; max++) {
        vtype_t str = {'\0'};
        (void) sprintf(str, "%-38s: %s", game_options[max].o_prompt, (*game_options[max].o_var ? "yes" : "no "));
        putStringClearToEOL(str, Coord_t{max + 1, 0});
    }
    eraseLine(Coord_t{max + 1, 0});

    int option_id = 0;
    while (true) {
        moveCursor(Coord_t{option_id + 1, 40});

        switch (getKeyInput()) {
            case ESCAPE:
                return;
            case '-':
                if (option_id > 0) {
                    option_id--;
                } else {
                    option_id = max - 1;
                }
                break;
            case ' ':
            case '\n':
            case '\r':
                if (option_id + 1 < max) {
                    option_id++;
                } else {
                    option_id = 0;
                }
                break;
            case 'y':
            case 'Y':
                putString("yes", Coord_t{option_id + 1, 40});

                *game_options[option_id].o_var = true;

                if (option_id + 1 < max) {
                    option_id++;
                } else {
                    option_id = 0;
                }
                break;
            case 'n':
            case 'N':
                putString("no ", Coord_t{option_id + 1, 40});

                *game_options[option_id].o_var = false;

                if (option_id + 1 < max) {
                    option_id++;
                } else {
                    option_id = 0;
                }
                break;
            default:
                terminalBellSound();
                break;
        }
    }
}

// Support for Umoria 5.2.2 up to 5.7.x.
// The save file format was frozen as of version 5.2.2.
bool validGameVersion(uint8_t major, uint8_t minor, uint8_t patch) {
    if (major != 5) {
        return false;
    }

    if (minor < 2) {
        return false;
    }

    if (minor == 2 && patch < 2) {
        return false;
    }

    return minor <= 7;
}

bool isCurrentGameVersion(uint8_t major, uint8_t minor, uint8_t patch) {
    return major == CURRENT_VERSION_MAJOR && minor == CURRENT_VERSION_MINOR && patch == CURRENT_VERSION_PATCH;
}

int getRandomDirection() {
    int dir;

    do {
        dir = randomNumber(9);
    } while (dir == 5);

    return dir;
}

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
    // used in counted commands. -CJS-
    if (game.use_last_direction) {
        direction = py.prev_dir;
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

        if (config::options::use_roguelike_keys) {
            command = mapRoguelikeKeysToKeypad(command);
        }

        if (command >= '1' && command <= '9' && command != '5') {
            py.prev_dir = command - '0';
            direction = py.prev_dir;
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

        if (config::options::use_roguelike_keys) {
            command = mapRoguelikeKeysToKeypad(command);
        }

        if (command >= '1' && command <= '9') {
            direction = command - '0';
            return true;
        }

        terminalBellSound();
    }
}

// Restore the terminal and exit
void exitProgram() {
    flushInputBuffer();
    terminalRestore();
    exit(0);
}

// Abort the program with a message displayed on the terminal.
void abortProgram(const char *msg) {
    flushInputBuffer();
    terminalRestore();

    printf("Program was manually aborted with the message:\n");
    printf("%s\n", msg);

    exit(0);
}
