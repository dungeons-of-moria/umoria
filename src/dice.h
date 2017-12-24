// Copyright (c) 1989-94 James E. Wilson, Robert A. Koeneke
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

#pragma once

typedef struct {
    uint8_t dice;
    uint8_t sides;
} Dice_t;

int diceRoll(Dice_t die);
int maxDiceRoll(const Dice_t &dice);
