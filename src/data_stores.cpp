// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// Store data

// clang-format off
#include "headers.h"

// Buying and selling adjustments for character race VS store owner race
uint8_t race_gold_adjustments[PLAYER_MAX_RACES][PLAYER_MAX_RACES] = {
    //Hum, HfE, Elf, Hal, Gno, Dwa, HfO, HfT
    { 100, 105, 105, 110, 113, 115, 120, 125 }, // Human
    { 110, 100, 100, 105, 110, 120, 125, 130 }, // Half-Elf
    { 110, 105, 100, 105, 110, 120, 125, 130 }, // Elf
    { 115, 110, 105,  95, 105, 110, 115, 130 }, // Halfling
    { 115, 115, 110, 105,  95, 110, 115, 130 }, // Gnome
    { 115, 120, 120, 110, 110,  95, 125, 135 }, // Dwarf
    { 115, 120, 125, 115, 115, 130, 110, 115 }, // Half-Orc
    { 110, 115, 115, 110, 110, 130, 110, 110 }, // Half-Troll
};

// game_objects[] index of objects that may appear in the store
uint16_t store_choices[MAX_STORES][STORE_MAX_ITEM_TYPES] = {
    // General Store
    {
        366, 365, 364,  84,  84, 365, 123, 366, 365, 350, 349, 348, 347,
        346, 346, 345, 345, 345, 344, 344, 344, 344, 344, 344, 344, 344,
    },
    // Armory
    {
        94,  95,  96, 109, 103, 104, 105, 106, 110, 111, 112, 114, 116,
        124, 125, 126, 127, 129, 103, 104, 124, 125, 91,  92,  95,  96,
    },
    // Weaponsmith
    {
        29, 30, 34, 37, 45, 49, 57, 58, 59, 65, 67, 68, 73,
        74, 75, 77, 79, 80, 81, 83, 29, 30, 80, 83, 80, 83,
    },
    // Temple
    {
        322, 323, 324, 325, 180, 180, 233, 237, 240, 241, 361, 362, 57,
        58,  59, 260, 358, 359, 265, 237, 237, 240, 240, 241, 323, 359,
    },
    // Alchemy shop
    {
        173, 174, 175, 351, 351, 352, 353, 354, 355, 356, 357, 206, 227,
        230, 236, 252, 253, 352, 353, 354, 355, 356, 359, 363, 359, 359,
    },
    // Magic-User store
    {
        318, 141, 142, 153, 164, 167, 168, 140, 319, 320, 320, 321, 269,
        270, 282, 286, 287, 292, 293, 294, 295, 308, 269, 290, 319, 282,
    },
};
