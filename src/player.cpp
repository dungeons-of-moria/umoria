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
