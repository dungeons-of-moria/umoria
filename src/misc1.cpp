// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Misc utility and initialization code, magic objects code

#include "headers.h"
#include "externs.h"

// generates damage for 2d6 style dice rolls
int diceDamageRoll(int dice, int sides) {
    int sum = 0;
    for (int i = 0; i < dice; i++) {
        sum += randomNumber(sides);
    }
    return sum;
}

int dicePlayerDamageRoll(uint8_t *notation_array) {
    return diceDamageRoll((int) notation_array[0], (int) notation_array[1]);
}

// If too many objects on floor level, delete some of them-RAK-
static void compactObjects() {
    printMessage("Compacting objects...");

    int counter = 0;
    int current_distance = 66;

    while (counter <= 0) {
        for (int y = 0; y < dg.height; y++) {
            for (int x = 0; x < dg.width; x++) {
                if (dg.floor[y][x].treasure_id != 0 && coordDistanceBetween(Coord_t{y, x}, Coord_t{py.row, py.col}) > current_distance) {
                    int chance;

                    switch (treasure_list[dg.floor[y][x].treasure_id].category_id) {
                        case TV_VIS_TRAP:
                            chance = 15;
                            break;
                        case TV_INVIS_TRAP:
                        case TV_RUBBLE:
                        case TV_OPEN_DOOR:
                        case TV_CLOSED_DOOR:
                            chance = 5;
                            break;
                        case TV_UP_STAIR:
                        case TV_DOWN_STAIR:
                        case TV_STORE_DOOR:
                            // Stairs, don't delete them.
                            // Shop doors, don't delete them.
                            chance = 0;
                            break;
                        case TV_SECRET_DOOR: // secret doors
                            chance = 3;
                            break;
                        default:
                            chance = 10;
                    }
                    if (randomNumber(100) <= chance) {
                        (void) dungeonDeleteObject(y, x);
                        counter++;
                    }
                }
            }
        }

        if (counter == 0) {
            current_distance -= 6;
        }
    }

    if (current_distance < 66) {
        drawDungeonPanel();
    }
}

// Gives pointer to next free space -RAK-
int popt() {
    if (current_treasure_id == LEVEL_MAX_OBJECTS) {
        compactObjects();
    }

    return current_treasure_id++;
}

// Pushes a record back onto free space list -RAK-
// `dungeonDeleteObject()` should always be called instead, unless the object
// in question is not in the dungeon, e.g. in store1.c and files.c
void pusht(uint8_t treasure_id) {
    if (treasure_id != current_treasure_id - 1) {
        treasure_list[treasure_id] = treasure_list[current_treasure_id - 1];

        // must change the treasure_id in the cave of the object just moved
        for (int y = 0; y < dg.height; y++) {
            for (int x = 0; x < dg.width; x++) {
                if (dg.floor[y][x].treasure_id == current_treasure_id - 1) {
                    dg.floor[y][x].treasure_id = treasure_id;
                }
            }
        }
    }
    current_treasure_id--;

    inventoryItemCopyTo(OBJ_NOTHING, treasure_list[current_treasure_id]);
}
