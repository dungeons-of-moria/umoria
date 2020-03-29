// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// Game object management

#include "headers.h"

int16_t sorted_objects[MAX_DUNGEON_OBJECTS];
int16_t treasure_levels[TREASURE_MAX_LEVELS + 1];

// If too many objects on floor level, delete some of them-RAK-
static void compactObjects() {
    printMessage("Compacting objects...");

    int counter = 0;
    int current_distance = 66;

    Coord_t coord = Coord_t{0, 0};

    while (counter <= 0) {
        for (coord.y = 0; coord.y < dg.height; coord.y++) {
            for (coord.x = 0; coord.x < dg.width; coord.x++) {
                if (dg.floor[coord.y][coord.x].treasure_id != 0 && coordDistanceBetween(coord, py.pos) > current_distance) {
                    int chance;

                    switch (game.treasure.list[dg.floor[coord.y][coord.x].treasure_id].category_id) {
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
                        (void) dungeonDeleteObject(coord);
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
    if (game.treasure.current_id == LEVEL_MAX_OBJECTS) {
        compactObjects();
    }

    return game.treasure.current_id++;
}

// Pushes a record back onto free space list -RAK-
// `dungeonDeleteObject()` should always be called instead, unless the object
// in question is not in the dungeon, e.g. in store1.c and files.c
void pusht(uint8_t treasure_id) {
    if (treasure_id != game.treasure.current_id - 1) {
        game.treasure.list[treasure_id] = game.treasure.list[game.treasure.current_id - 1];

        // must change the treasure_id in the cave of the object just moved
        for (int y = 0; y < dg.height; y++) {
            for (int x = 0; x < dg.width; x++) {
                if (dg.floor[y][x].treasure_id == game.treasure.current_id - 1) {
                    dg.floor[y][x].treasure_id = treasure_id;
                }
            }
        }
    }
    game.treasure.current_id--;

    inventoryItemCopyTo(config::dungeon::objects::OBJ_NOTHING, game.treasure.list[game.treasure.current_id]);
}

// Item too large to fit in chest? -DJG-
// Use a DungeonObject_t since the item has not yet been created
static bool itemBiggerThanChest(DungeonObject_t const &obj) {
    switch (obj.category_id) {
        case TV_CHEST:
        case TV_BOW:
        case TV_POLEARM:
        case TV_HARD_ARMOR:
        case TV_SOFT_ARMOR:
        case TV_STAFF:
            return true;
        case TV_HAFTED:
        case TV_SWORD:
        case TV_DIGGING:
            return (obj.weight > 150);
        default:
            return false;
    }
}

// Returns the array number of a random object -RAK-
int itemGetRandomObjectId(int level, bool must_be_small) {
    if (level == 0) {
        return randomNumber(treasure_levels[0]) - 1;
    }

    if (level >= TREASURE_MAX_LEVELS) {
        level = TREASURE_MAX_LEVELS;
    } else if (randomNumber(config::treasure::TREASURE_CHANCE_OF_GREAT_ITEM) == 1) {
        level = level * TREASURE_MAX_LEVELS / randomNumber(TREASURE_MAX_LEVELS) + 1;
        if (level > TREASURE_MAX_LEVELS) {
            level = TREASURE_MAX_LEVELS;
        }
    }

    int object_id;

    // This code has been added to make it slightly more likely to get the
    // higher level objects.  Originally a uniform distribution over all
    // objects less than or equal to the dungeon level. This distribution
    // makes a level n objects occur approx 2/n% of the time on level n,
    // and 1/2n are 0th level.
    do {
        if (randomNumber(2) == 1) {
            object_id = randomNumber(treasure_levels[level]) - 1;
        } else {
            // Choose three objects, pick the highest level.
            object_id = randomNumber(treasure_levels[level]) - 1;

            int j = randomNumber(treasure_levels[level]) - 1;

            if (object_id < j) {
                object_id = j;
            }

            j = randomNumber(treasure_levels[level]) - 1;

            if (object_id < j) {
                object_id = j;
            }

            int found_level = game_objects[sorted_objects[object_id]].depth_first_found;

            if (found_level == 0) {
                object_id = randomNumber(treasure_levels[0]) - 1;
            } else {
                object_id = randomNumber(treasure_levels[found_level] - treasure_levels[found_level - 1]) - 1 + treasure_levels[found_level - 1];
            }
        }
    } while (must_be_small && itemBiggerThanChest(game_objects[sorted_objects[object_id]]));

    return object_id;
}
