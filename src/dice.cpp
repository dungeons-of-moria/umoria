// Copyright (c) 1989-94 James E. Wilson, Robert A. Koeneke
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

#include "headers.h"

// generates damage for 2d6 style dice rolls
int diceRoll(Dice_t die) {
    auto sum = 0;
    for (auto i = 0; i < die.dice; i++) {
        sum += randomNumber(die.sides);
    }
    return sum;
}
