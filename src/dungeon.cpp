// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// The main command interpreter, updating player status

#include "headers.h"
#include "externs.h"

// The Dungeon global
// Yup, this initialization is ugly, we'll fix...eventually! -MRC-
Dungeon_t dg = Dungeon_t{0, 0, {}, -1, 0, true, {}};

// dungeonDisplayMap shrinks the dungeon to a single screen
void dungeonDisplayMap() {
    // Save the game screen
    terminalSaveScreen();
    clearScreen();

    int16_t priority[256] = {0};
    priority[60] = 5;    // char '<'
    priority[62] = 5;    // char '>'
    priority[64] = 10;   // char '@'
    priority[35] = -5;   // char '#'
    priority[46] = -10;  // char '.'
    priority[92] = -3;   // char '\'
    priority[32] = -15;  // char ' '

    // Display highest priority object in the RATIO, by RATIO area
    constexpr uint8_t RATIO = 3;
    uint8_t panel_width = MAX_WIDTH / RATIO;
    uint8_t panel_height = MAX_HEIGHT / RATIO;

    char map[MAX_WIDTH / RATIO + 1] = {'\0'};
    char line_buffer[80];

    // Add screen border
    addChar('+', Coord_t{0, 0});
    addChar('+', Coord_t{0, panel_width + 1});
    for (uint8_t i = 0; i < panel_width; i++) {
        addChar('-', Coord_t{0, i + 1});
        addChar('-', Coord_t{panel_height + 1, i + 1});
    }
    for (uint8_t i = 0; i < panel_height; i++) {
        addChar('|', Coord_t{i + 1, 0});
        addChar('|', Coord_t{i + 1, panel_width + 1});
    }
    addChar('+', Coord_t{panel_height + 1, 0});
    addChar('+', Coord_t{panel_height + 1, panel_width + 1});
    putString("Hit any key to continue", Coord_t{23, 23});

    int player_y = 0;
    int player_x = 0;
    int line = -1;

    // Shrink the dungeon!
    for (uint8_t y = 0; y < MAX_HEIGHT; y++) {
        uint8_t row = y / RATIO;
        if (row != line) {
            if (line >= 0) {
                sprintf(line_buffer, "|%s|", map);
                putString(line_buffer, Coord_t{line + 1, 0});
            }
            for (uint8_t j = 0; j < panel_width; j++) {
                map[j] = ' ';
            }
            line = row;
        }

        for (uint8_t x = 0; x < MAX_WIDTH; x++) {
            uint8_t col = x / RATIO;
            char cave_char = caveGetTileSymbol(Coord_t{y, x});
            if (priority[(uint8_t) map[col]] < priority[(uint8_t) cave_char]) {
                map[col] = cave_char;
            }
            if (map[col] == '@') {
                // +1 to account for border
                player_x = col + 1;
                player_y = row + 1;
            }
        }
    }

    if (line >= 0) {
        sprintf(line_buffer, "|%s|", map);
        putString(line_buffer, Coord_t{line + 1, 0});
    }

    // Move cursor onto player character
    moveCursor(Coord_t{player_y, player_x});

    // wait for any keypress
    (void) getKeyInput();

    // restore the game screen
    terminalRestoreScreen();
}

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

// Places a particular trap at location y, x -RAK-
void dungeonSetTrap(int y, int x, int sub_type_id) {
    int free_treasure_id = popt();
    dg.floor[y][x].treasure_id = (uint8_t) free_treasure_id;
    inventoryItemCopyTo(OBJ_TRAP_LIST + sub_type_id, treasure_list[free_treasure_id]);
}

// Places rubble at location y, x -RAK-
void dungeonPlaceRubble(int y, int x) {
    int free_treasure_id = popt();
    dg.floor[y][x].treasure_id = (uint8_t) free_treasure_id;
    dg.floor[y][x].feature_id = TILE_BLOCKED_FLOOR;
    inventoryItemCopyTo(OBJ_RUBBLE, treasure_list[free_treasure_id]);
}

// Places a treasure (Gold or Gems) at given row, column -RAK-
void dungeonPlaceGold(int y, int x) {
    int free_treasure_id = popt();

    int gold_type_id = ((randomNumber(dg.current_level + 2) + 2) / 2) - 1;

    if (randomNumber(TREASURE_CHANCE_OF_GREAT_ITEM) == 1) {
        gold_type_id += randomNumber(dg.current_level + 1);
    }

    if (gold_type_id >= MAX_GOLD_TYPES) {
        gold_type_id = MAX_GOLD_TYPES - 1;
    }

    dg.floor[y][x].treasure_id = (uint8_t) free_treasure_id;
    inventoryItemCopyTo(OBJ_GOLD_LIST + gold_type_id, treasure_list[free_treasure_id]);
    treasure_list[free_treasure_id].cost += (8L * (int32_t) randomNumber((int) treasure_list[free_treasure_id].cost)) + randomNumber(8);

    if (dg.floor[y][x].creature_id == 1) {
        printMessage("You feel something roll beneath your feet.");
    }
}

// Places an object at given row, column co-ordinate -RAK-
void dungeonPlaceRandomObjectAt(int y, int x, bool must_be_small) {
    int free_treasure_id = popt();

    dg.floor[y][x].treasure_id = (uint8_t) free_treasure_id;

    int object_id = itemGetRandomObjectId(dg.current_level, must_be_small);
    inventoryItemCopyTo(sorted_objects[object_id], treasure_list[free_treasure_id]);

    magicTreasureMagicalAbility(free_treasure_id, dg.current_level);

    if (dg.floor[y][x].creature_id == 1) {
        printMessage("You feel something roll beneath your feet."); // -CJS-
    }
}

// Allocates an object for tunnels and rooms -RAK-
void dungeonAllocateAndPlaceObject(bool (*set_function)(int), int object_type, int number) {
    int y, x;

    for (int i = 0; i < number; i++) {
        // don't put an object beneath the player, this could cause
        // problems if player is standing under rubble, or on a trap.
        do {
            y = randomNumber(dg.height) - 1;
            x = randomNumber(dg.width) - 1;
        } while (!(*set_function)(dg.floor[y][x].feature_id) || dg.floor[y][x].treasure_id != 0 || (y == py.row && x == py.col));

        switch (object_type) {
            case 1:
                dungeonSetTrap(y, x, randomNumber(MAX_TRAPS) - 1);
                break;
            case 2:
                // NOTE: object_type == 2 is no longer used - it used to be visible traps.
                // FIXME: there was no `break` here so `case 3` catches it? -MRC-
            case 3:
                dungeonPlaceRubble(y, x);
                break;
            case 4:
                dungeonPlaceGold(y, x);
                break;
            case 5:
                dungeonPlaceRandomObjectAt(y, x, false);
                break;
            default:
                break;
        }
    }
}

// Creates objects nearby the coordinates given -RAK-
void dungeonPlaceRandomObjectNear(int y, int x, int tries) {
    do {
        for (int i = 0; i <= 10; i++) {
            int j = y - 3 + randomNumber(5);
            int k = x - 4 + randomNumber(7);

            if (coordInBounds(Coord_t{j, k}) && dg.floor[j][k].feature_id <= MAX_CAVE_FLOOR && dg.floor[j][k].treasure_id == 0) {
                if (randomNumber(100) < 75) {
                    dungeonPlaceRandomObjectAt(j, k, false);
                } else {
                    dungeonPlaceGold(j, k);
                }
                i = 9;
            }
        }

        tries--;
    } while (tries != 0);
}

// Moves creature record from one space to another -RAK-
// this always works correctly, even if y1==y2 and x1==x2
void dungeonMoveCreatureRecord(int y1, int x1, int y2, int x2) {
    int id = dg.floor[y1][x1].creature_id;
    dg.floor[y1][x1].creature_id = 0;
    dg.floor[y2][x2].creature_id = (uint8_t) id;
}

// Room is lit, make it appear -RAK-
void dungeonLightRoom(int pos_y, int pos_x) {
    int height_middle = (SCREEN_HEIGHT / 2);
    int width_middle = (SCREEN_WIDTH / 2);

    int top = (pos_y / height_middle) * height_middle;
    int left = (pos_x / width_middle) * width_middle;
    int bottom = top + height_middle - 1;
    int right = left + width_middle - 1;

    for (int y = top; y <= bottom; y++) {
        for (int x = left; x <= right; x++) {
            Tile_t &tile = dg.floor[y][x];

            if (tile.perma_lit_room && !tile.permanent_light) {
                tile.permanent_light = true;

                if (tile.feature_id == TILE_DARK_FLOOR) {
                    tile.feature_id = TILE_LIGHT_FLOOR;
                }
                if (!tile.field_mark && tile.treasure_id != 0) {
                    int treasure_id = treasure_list[tile.treasure_id].category_id;
                    if (treasure_id >= TV_MIN_VISIBLE && treasure_id <= TV_MAX_VISIBLE) {
                        tile.field_mark = true;
                    }
                }
                panelPutTile(caveGetTileSymbol(Coord_t{y, x}), Coord_t{y, x});
            }
        }
    }
}

// Lights up given location -RAK-
void dungeonLiteSpot(int y, int x) {
    if (!coordInsidePanel(Coord_t{y, x})) {
        return;
    }

    char symbol = caveGetTileSymbol(Coord_t{y, x});
    panelPutTile(symbol, Coord_t{y, x});
}

// Normal movement
// When FIND_FLAG,  light only permanent features
static void sub1_move_light(int y1, int x1, int y2, int x2) {
    if (py.temporary_light_only) {
        // Turn off lamp light
        for (int y = y1 - 1; y <= y1 + 1; y++) {
            for (int x = x1 - 1; x <= x1 + 1; x++) {
                dg.floor[y][x].temporary_light = false;
            }
        }
        if ((py.running_tracker != 0) && !config.run_print_self) {
            py.temporary_light_only = false;
        }
    } else if ((py.running_tracker == 0) || config.run_print_self) {
        py.temporary_light_only = true;
    }

    for (int y = y2 - 1; y <= y2 + 1; y++) {
        for (int x = x2 - 1; x <= x2 + 1; x++) {
            Tile_t &tile = dg.floor[y][x];

            // only light up if normal movement
            if (py.temporary_light_only) {
                tile.temporary_light = true;
            }

            if (tile.feature_id >= MIN_CAVE_WALL) {
                tile.permanent_light = true;
            } else if (!tile.field_mark && tile.treasure_id != 0) {
                int tval = treasure_list[tile.treasure_id].category_id;

                if (tval >= TV_MIN_VISIBLE && tval <= TV_MAX_VISIBLE) {
                    tile.field_mark = true;
                }
            }
        }
    }

    // From uppermost to bottom most lines player was on.
    int top, left, bottom, right;

    if (y1 < y2) {
        top = y1 - 1;
        bottom = y2 + 1;
    } else {
        top = y2 - 1;
        bottom = y1 + 1;
    }
    if (x1 < x2) {
        left = x1 - 1;
        right = x2 + 1;
    } else {
        left = x2 - 1;
        right = x1 + 1;
    }

    for (int y = top; y <= bottom; y++) {
        // Leftmost to rightmost do
        for (int x = left; x <= right; x++) {
            panelPutTile(caveGetTileSymbol(Coord_t{y, x}), Coord_t{y, x});
        }
    }
}

// When blinded,  move only the player symbol.
// With no light,  movement becomes involved.
static void sub3_move_light(int y1, int x1, int y2, int x2) {
    if (py.temporary_light_only) {
        for (int y = y1 - 1; y <= y1 + 1; y++) {
            for (int x = x1 - 1; x <= x1 + 1; x++) {
                dg.floor[y][x].temporary_light = false;
                panelPutTile(caveGetTileSymbol(Coord_t{y, x}), Coord_t{y, x});
            }
        }

        py.temporary_light_only = false;
    } else if ((py.running_tracker == 0) || config.run_print_self) {
        panelPutTile(caveGetTileSymbol(Coord_t{y1, x1}), Coord_t{y1, x1});
    }

    if ((py.running_tracker == 0) || config.run_print_self) {
        panelPutTile('@', Coord_t{y2, x2});
    }
}

// Package for moving the character's light about the screen
// Four cases : Normal, Finding, Blind, and No light -RAK-
void dungeonMoveCharacterLight(int y1, int x1, int y2, int x2) {
    if (py.flags.blind > 0 || !py.carrying_light) {
        sub3_move_light(y1, x1, y2, x2);
    } else {
        sub1_move_light(y1, x1, y2, x2);
    }
}

// Deletes a monster entry from the level -RAK-
void dungeonDeleteMonster(int id) {
    Monster_t *monster = &monsters[id];

    dg.floor[monster->y][monster->x].creature_id = 0;

    if (monster->lit) {
        dungeonLiteSpot((int) monster->y, (int) monster->x);
    }

    int last_id = next_free_monster_id - 1;

    if (id != last_id) {
        monster = &monsters[last_id];
        dg.floor[monster->y][monster->x].creature_id = (uint8_t) id;
        monsters[id] = monsters[last_id];
    }

    next_free_monster_id--;
    monsters[next_free_monster_id] = blank_monster;

    if (monster_multiply_total > 0) {
        monster_multiply_total--;
    }
}

// The following two procedures implement the same function as delete monster.
// However, they are used within updateMonsters(), because deleting a monster
// while scanning the monsters causes two problems, monsters might get two
// turns, and m_ptr/monptr might be invalid after the dungeonDeleteMonster.
// Hence the delete is done in two steps.
//
// dungeonDeleteMonsterFix1 does everything dungeonDeleteMonster does except delete
// the monster record and reduce next_free_monster_id, this is called in breathe, and
// a couple of places in creatures.c
void dungeonDeleteMonsterFix1(int id) {
    Monster_t &monster = monsters[id];

    // force the hp negative to ensure that the monster is dead, for example,
    // if the monster was just eaten by another, it will still have positive
    // hit points
    monster.hp = -1;

    dg.floor[monster.y][monster.x].creature_id = 0;

    if (monster.lit) {
        dungeonLiteSpot((int) monster.y, (int) monster.x);
    }

    if (monster_multiply_total > 0) {
        monster_multiply_total--;
    }
}

// dungeonDeleteMonsterFix2 does everything in dungeonDeleteMonster that wasn't done
// by fix1_monster_delete above, this is only called in updateMonsters()
void dungeonDeleteMonsterFix2(int id) {
    int last_id = next_free_monster_id - 1;

    if (id != last_id) {
        int y = monsters[last_id].y;
        int x = monsters[last_id].x;
        dg.floor[y][x].creature_id = (uint8_t) id;

        monsters[id] = monsters[last_id];
    }

    monsters[last_id] = blank_monster;
    next_free_monster_id--;
}

// Creates objects nearby the coordinates given -RAK-
int dungeonSummonObject(int y, int x, int amount, int object_type) {
    int real_type;

    if (object_type == 1 || object_type == 5) {
        real_type = 1;   // object_type == 1 -> objects
    } else {
        real_type = 256; // object_type == 2 -> gold
    }

    int result = 0;

    do {
        for (int i = 0; i <= 20; i++) {
            int pos_y = y - 3 + randomNumber(5);
            int pos_x = x - 3 + randomNumber(5);

            if (coordInBounds(Coord_t{pos_y, pos_x}) && los(y, x, pos_y, pos_x)) {
                if (dg.floor[pos_y][pos_x].feature_id <= MAX_OPEN_SPACE && dg.floor[pos_y][pos_x].treasure_id == 0) {
                    // object_type == 3 -> 50% objects, 50% gold
                    if (object_type == 3 || object_type == 7) {
                        if (randomNumber(100) < 50) {
                            real_type = 1;
                        } else {
                            real_type = 256;
                        }
                    }

                    if (real_type == 1) {
                        dungeonPlaceRandomObjectAt(pos_y, pos_x, (object_type >= 4));
                    } else {
                        dungeonPlaceGold(pos_y, pos_x);
                    }

                    dungeonLiteSpot(pos_y, pos_x);

                    if (caveTileVisible(Coord_t{pos_y, pos_x})) {
                        result += real_type;
                    }

                    i = 20;
                }
            }
        }

        amount--;
    } while (amount != 0);

    return result;
}

// Deletes object from given location -RAK-
bool dungeonDeleteObject(int y, int x) {
    Tile_t &tile = dg.floor[y][x];

    if (tile.feature_id == TILE_BLOCKED_FLOOR) {
        tile.feature_id = TILE_CORR_FLOOR;
    }

    pusht(tile.treasure_id);

    tile.treasure_id = 0;
    tile.field_mark = false;

    dungeonLiteSpot(y, x);

    return caveTileVisible(Coord_t{y, x});
}
