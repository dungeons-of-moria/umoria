// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Misc utility and initialization code, magic objects code

#include "headers.h"
#include "externs.h"

static void panelBounds();
static int popm();
static int maxHitPoints(const uint8_t *array);
static int monsterGetOneSuitableForLevel(int level);

// Returns position of first set bit and clears that bit -RAK-
int getAndClearFirstBit(uint32_t &flag) {
    uint32_t mask = 0x1;

    for (int i = 0; i < (int) sizeof(flag) * 8; i++) {
        if ((flag & mask) != 0u) {
            flag &= ~mask;
            return i;
        }
        mask <<= 1;
    }

    // no one bits found
    return -1;
}

// Checks a co-ordinate for in bounds status -RAK-
bool coordInBounds(Coord_t coord) {
    bool valid_y = coord.y > 0 && coord.y < dg.height - 1;
    bool valid_x = coord.x > 0 && coord.x < dg.width - 1;

    return valid_y && valid_x;
}

// Calculates current boundaries -RAK-
static void panelBounds() {
    dg.panel.top = dg.panel.row * (SCREEN_HEIGHT / 2);
    dg.panel.bottom = dg.panel.top + SCREEN_HEIGHT - 1;
    dg.panel.row_prt = dg.panel.top - 1;
    dg.panel.left = dg.panel.col * (SCREEN_WIDTH / 2);
    dg.panel.right = dg.panel.left + SCREEN_WIDTH - 1;
    dg.panel.col_prt = dg.panel.left - 13;
}

// Given an row (y) and col (x), this routine detects -RAK-
// when a move off the screen has occurred and figures new borders.
// `force` forces the panel bounds to be recalculated, useful for 'W'here.
bool coordOutsidePanel(Coord_t coord, bool force) {
    int row = dg.panel.row;
    int col = dg.panel.col;

    if (force || coord.y < dg.panel.top + 2 || coord.y > dg.panel.bottom - 2) {
        row = (coord.y - SCREEN_HEIGHT / 4) / (SCREEN_HEIGHT / 2);

        if (row > dg.panel.max_rows) {
            row = dg.panel.max_rows;
        } else if (row < 0) {
            row = 0;
        }
    }

    if (force || coord.x < dg.panel.left + 3 || coord.x > dg.panel.right - 3) {
        col = ((coord.x - SCREEN_WIDTH / 4) / (SCREEN_WIDTH / 2));
        if (col > dg.panel.max_cols) {
            col = dg.panel.max_cols;
        } else if (col < 0) {
            col = 0;
        }
    }

    if (row != dg.panel.row || col != dg.panel.col) {
        dg.panel.row = row;
        dg.panel.col = col;
        panelBounds();

        // stop movement if any
        if (config.find_bound) {
            playerEndRunning();
        }

        // Yes, the coordinates are beyond the current panel boundary
        return true;
    }

    return false;
}

// Is the given coordinate within the screen panel boundaries -RAK-
bool coordInsidePanel(Coord_t coord) {
    bool valid_y = coord.y >= dg.panel.top && coord.y <= dg.panel.bottom;
    bool valid_x = coord.x >= dg.panel.left && coord.x <= dg.panel.right;

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

// Prints the map of the dungeon -RAK-
void drawDungeonPanel() {
    int line = 1;

    // Top to bottom
    for (int y = dg.panel.top; y <= dg.panel.bottom; y++) {
        eraseLine(Coord_t{line++, 13});

        // Left to right
        for (int x = dg.panel.left; x <= dg.panel.right; x++) {
            char ch = caveGetTileSymbol(Coord_t{y, x});
            if (ch != ' ') {
                panelPutTile(ch, Coord_t{y, x});
            }
        }
    }
}

// Compact monsters -RAK-
// Return true if any monsters were deleted, false if could not delete any monsters.
bool compactMonsters() {
    printMessage("Compacting monsters...");

    int cur_dis = 66;

    bool delete_any = false;
    while (!delete_any) {
        for (int i = next_free_monster_id - 1; i >= MON_MIN_INDEX_ID; i--) {
            if (cur_dis < monsters[i].distance_from_player && randomNumber(3) == 1) {
                if ((creatures_list[monsters[i].creature_id].movement & CM_WIN) != 0u) {
                    // Never compact away the Balrog!!
                } else if (hack_monptr < i) {
                    // in case this is called from within updateMonsters(), this is a horrible
                    // hack, the monsters/updateMonsters() code needs to be rewritten.
                    dungeonDeleteMonster(i);
                    delete_any = true;
                } else {
                    // dungeonDeleteMonsterFix1() does not decrement next_free_monster_id,
                    // so don't set delete_any if this was called.
                    dungeonDeleteMonsterFix1(i);
                }
            }
        }

        if (!delete_any) {
            cur_dis -= 6;

            // Can't delete any monsters, return failure.
            if (cur_dis < 0) {
                return false;
            }
        }
    }

    return true;
}

// Returns a pointer to next free space -RAK-
// Returns -1 if could not allocate a monster.
static int popm() {
    if (next_free_monster_id == MON_TOTAL_ALLOCATIONS) {
        if (!compactMonsters()) {
            return -1;
        }
    }
    return next_free_monster_id++;
}

// Gives Max hit points -RAK-
static int maxHitPoints(const uint8_t *array) {
    return (array[0] * array[1]);
}

// Places a monster at given location -RAK-
bool monsterPlaceNew(int y, int x, int creature_id, bool sleeping) {
    int monster_id = popm();

    // Check for case where could not allocate space for the monster
    if (monster_id == -1) {
        return false;
    }

    Monster_t &monster = monsters[monster_id];

    monster.y = (uint8_t) y;
    monster.x = (uint8_t) x;
    monster.creature_id = (uint16_t) creature_id;

    if ((creatures_list[creature_id].defenses & CD_MAX_HP) != 0) {
        monster.hp = (int16_t) maxHitPoints(creatures_list[creature_id].hit_die);
    } else {
        monster.hp = (int16_t) dicePlayerDamageRoll(creatures_list[creature_id].hit_die);
    }

    // the creatures_list[] speed value is 10 greater, so that it can be a uint8_t
    monster.speed = (int16_t) (creatures_list[creature_id].speed - 10 + py.flags.speed);
    monster.stunned_amount = 0;
    monster.distance_from_player = (uint8_t) coordDistanceBetween(Coord_t{py.row, py.col}, Coord_t{y, x});
    monster.lit = false;

    dg.floor[y][x].creature_id = (uint8_t) monster_id;

    if (sleeping) {
        if (creatures_list[creature_id].sleep_counter == 0) {
            monster.sleep_count = 0;
        } else {
            monster.sleep_count = (int16_t) ((creatures_list[creature_id].sleep_counter * 2) + randomNumber((int) creatures_list[creature_id].sleep_counter * 10));
        }
    } else {
        monster.sleep_count = 0;
    }

    return true;
}

// Places a monster at given location -RAK-
void monsterPlaceWinning() {
    if (game.total_winner) {
        return;
    }

    int y, x;
    do {
        y = randomNumber(dg.height - 2);
        x = randomNumber(dg.width - 2);
    } while ((dg.floor[y][x].feature_id >= MIN_CLOSED_SPACE) || (dg.floor[y][x].creature_id != 0) || (dg.floor[y][x].treasure_id != 0) || (coordDistanceBetween(Coord_t{y, x}, Coord_t{py.row, py.col}) <= MON_MAX_SIGHT));

    int creature_id = randomNumber(MON_ENDGAME_MONSTERS) - 1 + monster_levels[MON_MAX_LEVELS];

    // FIXME: duplicate code -MRC-
    // The following code is now exactly the same as monsterPlaceNew() except here
    // we `abort()` on failed placement, and do not set `monster->lit = false`.
    // Perhaps we can find a way to call `monsterPlaceNew()` instead of
    // duplicating everything here.

    int monster_id = popm();

    // Check for case where could not allocate space for the win monster, this should never happen.
    if (monster_id == -1) {
        abort();
    }

    Monster_t &monster = monsters[monster_id];

    monster.y = (uint8_t) y;
    monster.x = (uint8_t) x;
    monster.creature_id = (uint16_t) creature_id;

    if ((creatures_list[creature_id].defenses & CD_MAX_HP) != 0) {
        monster.hp = (int16_t) maxHitPoints(creatures_list[creature_id].hit_die);
    } else {
        monster.hp = (int16_t) dicePlayerDamageRoll(creatures_list[creature_id].hit_die);
    }

    // the creatures_list speed value is 10 greater, so that it can be a uint8_t
    monster.speed = (int16_t) (creatures_list[creature_id].speed - 10 + py.flags.speed);
    monster.stunned_amount = 0;
    monster.distance_from_player = (uint8_t) coordDistanceBetween(Coord_t{py.row, py.col}, Coord_t{y, x});

    dg.floor[y][x].creature_id = (uint8_t) monster_id;

    monster.sleep_count = 0;
}

// Return a monster suitable to be placed at a given level. This
// makes high level monsters (up to the given level) slightly more
// common than low level monsters at any given level. -CJS-
static int monsterGetOneSuitableForLevel(int level) {
    if (level == 0) {
        return randomNumber(monster_levels[0]) - 1;
    }

    if (level > MON_MAX_LEVELS) {
        level = MON_MAX_LEVELS;
    }

    if (randomNumber(MON_CHANCE_OF_NASTY) == 1) {
        auto abs_distribution = (int) std::abs((std::intmax_t) randomNumberNormalDistribution(0, 4));
        level += abs_distribution + 1;
        if (level > MON_MAX_LEVELS) {
            level = MON_MAX_LEVELS;
        }
    } else {
        // This code has been added to make it slightly more likely to get
        // the higher level monsters. Originally a uniform distribution over
        // all monsters of level less than or equal to the dungeon level.
        // This distribution makes a level n monster occur approx 2/n% of the
        // time on level n, and 1/n*n% are 1st level.
        int num = monster_levels[level] - monster_levels[0];
        int i = randomNumber(num) - 1;
        int j = randomNumber(num) - 1;
        if (j > i) {
            i = j;
        }
        level = creatures_list[i + monster_levels[0]].level;
    }

    return randomNumber(monster_levels[level] - monster_levels[level - 1]) - 1 + monster_levels[level - 1];
}

// Allocates a random monster -RAK-
void monsterPlaceNewWithinDistance(int number, int distance_from_source, bool sleeping) {
    int y, x;

    for (int i = 0; i < number; i++) {
        do {
            y = randomNumber(dg.height - 2);
            x = randomNumber(dg.width - 2);
        } while (dg.floor[y][x].feature_id >= MIN_CLOSED_SPACE || dg.floor[y][x].creature_id != 0 || coordDistanceBetween(Coord_t{y, x}, Coord_t{py.row, py.col}) <= distance_from_source);

        int l = monsterGetOneSuitableForLevel(dg.current_level);

        // Dragons are always created sleeping here,
        // so as to give the player a sporting chance.
        if (creatures_list[l].sprite == 'd' || creatures_list[l].sprite == 'D') {
            sleeping = true;
        }

        // Place_monster() should always return true here.
        // It does not matter if it fails though.
        (void) monsterPlaceNew(y, x, l, sleeping);
    }
}

static bool placeMonsterAdjacentTo(int monsterID, int &y, int &x, bool slp) {
    bool placed = false;

    for (int i = 0; i <= 9; i++) {
        int yy = y - 2 + randomNumber(3);
        int xx = x - 2 + randomNumber(3);

        if (coordInBounds(Coord_t{yy, xx})) {
            if (dg.floor[yy][xx].feature_id <= MAX_OPEN_SPACE && dg.floor[yy][xx].creature_id == 0) {
                // Place_monster() should always return true here.
                if (!monsterPlaceNew(yy, xx, monsterID, slp)) {
                    return false;
                }

                y = yy;
                x = xx;

                placed = true;
                i = 9;
            }
        }
    }

    return placed;
}

// Places creature adjacent to given location -RAK-
bool monsterSummon(int &y, int &x, bool sleeping) {
    int monster_id = monsterGetOneSuitableForLevel(dg.current_level + MON_SUMMONED_LEVEL_ADJUST);
    return placeMonsterAdjacentTo(monster_id, y, x, sleeping);
}

// Places undead adjacent to given location -RAK-
bool monsterSummonUndead(int &y, int &x) {
    int monster_id;
    int max_levels = monster_levels[MON_MAX_LEVELS];

    do {
        monster_id = randomNumber(max_levels) - 1;
        for (int i = 0; i <= 19;) {
            if ((creatures_list[monster_id].defenses & CD_UNDEAD) != 0) {
                i = 20;
                max_levels = 0;
            } else {
                monster_id++;
                if (monster_id > max_levels) {
                    i = 20;
                } else {
                    i++;
                }
            }
        }
    } while (max_levels != 0);

    return placeMonsterAdjacentTo(monster_id, y, x, false);
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

// Pushs a record back onto free space list -RAK-
// Delete_object() should always be called instead, unless the object
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
