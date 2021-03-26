// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

#pragma once

#include <cstdint>

typedef struct {
    uint8_t dice;
    uint8_t sides;
} Dice_t;

int diceRoll(Dice_t const &dice);
int maxDiceRoll(Dice_t const &dice);
