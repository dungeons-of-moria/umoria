// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Player specific variable definitions

#include "headers.h"

// Player record for most player related info
Player_t py = Player_t{};

bool playerIsMale() {
    return py.misc.gender;
}

void playerSetGender(bool is_male) {
    py.misc.gender = is_male;
}

const char *playerGetGenderLabel() {
    if (playerIsMale()) {
        return "Male";
    }
    return "Female";
}

// I don't really like this, but for now, I like it better than being a naked global -MRC-
void playerInitializeBaseExperienceLevels() {
    // TODO: load from external data file
    uint32_t levels[PLAYER_MAX_LEVEL] = {
            10,      25,     45,      70,       100,      140,      200,       280,
            380,     500,    650,     850,      1100,     1400,     1800,      2300,
            2900,    3600,   4400,    5400,      6800,     8400,    10200,     12500,
            17500,   25000,  35000L, 50000L,    75000L,  100000L,  150000L,   200000L,
            300000L, 400000L, 500000L, 750000L, 1500000L, 2500000L, 5000000L, 10000000L,
    };

    for (auto i = 0; i < PLAYER_MAX_LEVEL; i++) {
        py.base_exp_levels[i] = levels[i];
    }
}
