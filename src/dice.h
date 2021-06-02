// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

typedef struct {
    uint8_t dice;
    uint8_t sides;
} Dice_t;

int diceRoll(Dice_t const &dice);
int maxDiceRoll(Dice_t const &dice);
