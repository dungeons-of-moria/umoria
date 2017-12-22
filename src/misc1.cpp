// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Misc utility and initialization code, magic objects code

#include "headers.h"
#include "externs.h"

// Checks a co-ordinate for in bounds status -RAK-
bool coordInBounds(Coord_t coord) {
    bool valid_y = coord.y > 0 && coord.y < dg.height - 1;
    bool valid_x = coord.x > 0 && coord.x < dg.width - 1;

    return valid_y && valid_x;
}

// Distance between two points -RAK-
int coordDistanceBetween(Coord_t coord_a, Coord_t coord_b) {
    int dy = coord_a.y - coord_b.y;
    if (dy < 0) {
        dy = -dy;
    }

    int dx = coord_a.x - coord_b.x;
    if (dx < 0) {
        dx = -dx;
    }

    int a = (dy + dx) << 1;
    int b = dy > dx ? dx : dy;

    return ((a - b) >> 1);
}

// Checks points north, south, east, and west for a wall -RAK-
// note that y,x is always coordInBounds(), i.e. 0 < y < dg.height-1,
// and 0 < x < dg.width-1
int coordWallsNextTo(Coord_t coord) {
    int walls = 0;

    if (dg.floor[coord.y - 1][coord.x].feature_id >= MIN_CAVE_WALL) {
        walls++;
    }

    if (dg.floor[coord.y + 1][coord.x].feature_id >= MIN_CAVE_WALL) {
        walls++;
    }

    if (dg.floor[coord.y][coord.x - 1].feature_id >= MIN_CAVE_WALL) {
        walls++;
    }

    if (dg.floor[coord.y][coord.x + 1].feature_id >= MIN_CAVE_WALL) {
        walls++;
    }

    return walls;
}

// Checks all adjacent spots for corridors -RAK-
// note that y, x is always coordInBounds(), hence no need to check that
// j, k are coordInBounds(), even if they are 0 or cur_x-1 is still works
int coordCorridorWallsNextTo(Coord_t coord) {
    int walls = 0;

    for (int yy = coord.y - 1; yy <= coord.y + 1; yy++) {
        for (int xx = coord.x - 1; xx <= coord.x + 1; xx++) {
            int tile_id = dg.floor[yy][xx].feature_id;
            int treasure_id = dg.floor[yy][xx].treasure_id;

            // should fail if there is already a door present
            if (tile_id == TILE_CORR_FLOOR && (treasure_id == 0 || treasure_list[treasure_id].category_id < TV_MIN_DOORS)) {
                walls++;
            }
        }
    }

    return walls;
}

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

// Returns symbol for given row, column -RAK-
char caveGetTileSymbol(Coord_t coord) {
    const Tile_t &tile = dg.floor[coord.y][coord.x];

    if (tile.creature_id == 1 && ((py.running_tracker == 0) || config.run_print_self)) {
        return '@';
    }

    if ((py.flags.status & PY_BLIND) != 0u) {
        return ' ';
    }

    if (py.flags.image > 0 && randomNumber(12) == 1) {
        return (uint8_t) (randomNumber(95) + 31);
    }

    if (tile.creature_id > 1 && monsters[tile.creature_id].lit) {
        return creatures_list[monsters[tile.creature_id].creature_id].sprite;
    }

    if (!tile.permanent_light && !tile.temporary_light && !tile.field_mark) {
        return ' ';
    }

    if (tile.treasure_id != 0 && treasure_list[tile.treasure_id].category_id != TV_INVIS_TRAP) {
        return treasure_list[tile.treasure_id].sprite;
    }

    if (tile.feature_id <= MAX_CAVE_FLOOR) {
        return '.';
    }

    if (tile.feature_id == TILE_GRANITE_WALL || tile.feature_id == TILE_BOUNDARY_WALL || !config.highlight_seams) {
        return '#';
    }

    // Originally set highlight bit, but that is not portable,
    // now use the percent sign instead.
    return '%';
}

// Tests a spot for light or field mark status -RAK-
bool caveTileVisible(Coord_t coord) {
    return dg.floor[coord.y][coord.x].permanent_light || dg.floor[coord.y][coord.x].temporary_light || dg.floor[coord.y][coord.x].field_mark;
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
