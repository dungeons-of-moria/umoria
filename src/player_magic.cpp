// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Player magic functions

#include "headers.h"
#include "externs.h"

// Cure players confusion -RAK-
bool playerCureConfusion() {
    if (py.flags.confused > 1) {
        py.flags.confused = 1;
        return true;
    }
    return false;
}

// Cure players blindness -RAK-
bool playerCureBlindness() {
    if (py.flags.blind > 1) {
        py.flags.blind = 1;
        return true;
    }
    return false;
}

// Cure poisoning -RAK-
bool playerCurePoison() {
    if (py.flags.poisoned > 1) {
        py.flags.poisoned = 1;
        return true;
    }
    return false;
}

// Cure the players fear -RAK-
bool playerRemoveFear() {
    if (py.flags.afraid > 1) {
        py.flags.afraid = 1;
        return true;
    }
    return false;
}

// Evil creatures don't like this. -RAK-
bool playerProtectEvil() {
    bool is_protected = py.flags.protect_evil == 0;

    py.flags.protect_evil += randomNumber(25) + 3 * py.misc.level;

    return is_protected;
}

// Bless -RAK-
void playerBless(int adjustment) {
    py.flags.blessed += adjustment;
}

// Detect Invisible for period of time -RAK-
void playerDetectInvisible(int adjustment) {
    py.flags.detect_invisible += adjustment;
}
