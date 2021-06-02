// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "headers.h"

// generates damage for 2d6 style dice rolls
int diceRoll(Dice_t const &dice) {
    auto sum = 0;
    for (auto i = 0; i < dice.dice; i++) {
        sum += randomNumber(dice.sides);
    }
    return sum;
}

// Returns max dice roll value -RAK-
int maxDiceRoll(Dice_t const &dice) {
    return dice.dice * dice.sides;
}
