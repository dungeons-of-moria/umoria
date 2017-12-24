// Copyright (c) 1989-94 James E. Wilson, Robert A. Koeneke
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Game initialization and maintenance related functions

#include "headers.h"
#include "externs.h"
#include "version.h"

// holds the previous rnd state
static uint32_t old_seed;

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
int randomNumber(int max) {
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

    int tmp = randomNumber(MAX_SHORT);

    // off scale, assign random value between 4 and 5 times SD
    if (tmp == MAX_SHORT) {
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
} options[] = {{"Running: cut known corners",            &config.run_cut_corners},
               {"Running: examine potential corners",    &config.run_examine_corners},
               {"Running: print self during run",        &config.run_print_self},
               {"Running: stop when map sector changes", &config.find_bound},
               {"Running: run through open doors",       &config.run_ignore_doors},
               {"Prompt to pick up objects",             &config.prompt_to_pickup},
               {"Rogue like commands",                   &config.use_roguelike_keys},
               {"Show weights in inventory",             &config.show_inventory_weights},
               {"Highlight and notice mineral seams",    &config.highlight_seams},
               {"Beep for invalid character",            &config.error_beep_sound},
               {"Display rest/repeat counts",            &config.display_counts},
               {nullptr,                                 nullptr},
};

// Set or unset various boolean config.display_counts -CJS-
void setGameOptions() {
    putStringClearToEOL("  ESC when finished, y/n to set options, <return> or - to move cursor", Coord_t{0, 0});

    int max;
    for (max = 0; options[max].o_prompt != nullptr; max++) {
        vtype_t str = {'\0'};
        (void) sprintf(str, "%-38s: %s", options[max].o_prompt, (*options[max].o_var ? "yes" : "no "));
        putStringClearToEOL(str, Coord_t{max + 1, 0});
    }
    eraseLine(Coord_t{max + 1, 0});

    int optionID = 0;
    while (true) {
        moveCursor(Coord_t{optionID + 1, 40});

        switch (getKeyInput()) {
            case ESCAPE:
                return;
            case '-':
                if (optionID > 0) {
                    optionID--;
                } else {
                    optionID = max - 1;
                }
                break;
            case ' ':
            case '\n':
            case '\r':
                if (optionID + 1 < max) {
                    optionID++;
                } else {
                    optionID = 0;
                }
                break;
            case 'y':
            case 'Y':
                putString("yes", Coord_t{optionID + 1, 40});

                *options[optionID].o_var = true;

                if (optionID + 1 < max) {
                    optionID++;
                } else {
                    optionID = 0;
                }
                break;
            case 'n':
            case 'N':
                putString("no ", Coord_t{optionID + 1, 40});

                *options[optionID].o_var = false;

                if (optionID + 1 < max) {
                    optionID++;
                } else {
                    optionID = 0;
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
