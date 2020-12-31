// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// The main command interpreter, updating player status

#include "headers.h"

// The Dungeon global
// Yup, this initialization is ugly, we'll fix...eventually! -MRC-
Dungeon_t dg = Dungeon_t{0, 0, {}, -1, 0, true, {}};

// dungeonDisplayMap shrinks the dungeon to a single screen
void dungeonDisplayMap() {
    // Save the game screen
    terminalSaveScreen();
    clearScreen();

    int16_t priority[256] = {0};
    priority[60] = 5;   // char '<'
    priority[62] = 5;   // char '>'
    priority[64] = 10;  // char '@'
    priority[35] = -5;  // char '#'
    priority[46] = -10; // char '.'
    priority[92] = -3;  // char '\'
    priority[32] = -15; // char ' '

    // Display highest priority object in the RATIO, by RATIO area
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
bool coordInBounds(Coord_t const &coord) {
    bool y = coord.y > 0 && coord.y < dg.height - 1;
    bool x = coord.x > 0 && coord.x < dg.width - 1;

    return y && x;
}

// Distance between two points -RAK-
int coordDistanceBetween(Coord_t const &from, Coord_t const &to) {
    int dy = from.y - to.y;
    if (dy < 0) {
        dy = -dy;
    }

    int dx = from.x - to.x;
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
int coordWallsNextTo(Coord_t const &coord) {
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
int coordCorridorWallsNextTo(Coord_t const &coord) {
    int walls = 0;

    for (int y = coord.y - 1; y <= coord.y + 1; y++) {
        for (int x = coord.x - 1; x <= coord.x + 1; x++) {
            int tile_id = dg.floor[y][x].feature_id;
            int treasure_id = dg.floor[y][x].treasure_id;

            // should fail if there is already a door present
            if (tile_id == TILE_CORR_FLOOR && (treasure_id == 0 || game.treasure.list[treasure_id].category_id < TV_MIN_DOORS)) {
                walls++;
            }
        }
    }

    return walls;
}

// Returns symbol for given row, column -RAK-
char caveGetTileSymbol(Coord_t const &coord) {
    Tile_t const &tile = dg.floor[coord.y][coord.x];

    if (tile.creature_id == 1 && ((py.running_tracker == 0) || config::options::run_print_self)) {
        return '@';
    }

    if ((py.flags.status & config::player::status::PY_BLIND) != 0u) {
        return ' ';
    }

    if (py.flags.image > 0 && randomNumber(12) == 1) {
        return (uint8_t)(randomNumber(95) + 31);
    }

    if (tile.creature_id > 1 && monsters[tile.creature_id].lit) {
        return creatures_list[monsters[tile.creature_id].creature_id].sprite;
    }

    if (!tile.permanent_light && !tile.temporary_light && !tile.field_mark) {
        return ' ';
    }

    if (tile.treasure_id != 0 && game.treasure.list[tile.treasure_id].category_id != TV_INVIS_TRAP) {
        return game.treasure.list[tile.treasure_id].sprite;
    }

    if (tile.feature_id <= MAX_CAVE_FLOOR) {
        return '.';
    }

    if (tile.feature_id == TILE_GRANITE_WALL || tile.feature_id == TILE_BOUNDARY_WALL || !config::options::highlight_seams) {
        return '#';
    }

    // Originally set highlight bit, but that is not portable,
    // now use the percent sign instead.
    return '%';
}

// Tests a spot for light or field mark status -RAK-
bool caveTileVisible(Coord_t const &coord) {
    return dg.floor[coord.y][coord.x].permanent_light || dg.floor[coord.y][coord.x].temporary_light || dg.floor[coord.y][coord.x].field_mark;
}

// Places a particular trap at location y, x -RAK-
void dungeonSetTrap(Coord_t const &coord, int sub_type_id) {
    int free_treasure_id = popt();
    dg.floor[coord.y][coord.x].treasure_id = (uint8_t) free_treasure_id;
    inventoryItemCopyTo(config::dungeon::objects::OBJ_TRAP_LIST + sub_type_id, game.treasure.list[free_treasure_id]);
}

// Change a trap from invisible to visible -RAK-
// Note: Secret doors are handled here
void trapChangeVisibility(Coord_t const &coord) {
    uint8_t treasure_id = dg.floor[coord.y][coord.x].treasure_id;

    Inventory_t &item = game.treasure.list[treasure_id];

    if (item.category_id == TV_INVIS_TRAP) {
        item.category_id = TV_VIS_TRAP;
        dungeonLiteSpot(coord);
        return;
    }

    // change secret door to closed door
    if (item.category_id == TV_SECRET_DOOR) {
        item.id = config::dungeon::objects::OBJ_CLOSED_DOOR;
        item.category_id = game_objects[config::dungeon::objects::OBJ_CLOSED_DOOR].category_id;
        item.sprite = game_objects[config::dungeon::objects::OBJ_CLOSED_DOOR].sprite;
        dungeonLiteSpot(coord);
    }
}

// Places rubble at location y, x -RAK-
void dungeonPlaceRubble(Coord_t const &coord) {
    int free_treasure_id = popt();
    dg.floor[coord.y][coord.x].treasure_id = (uint8_t) free_treasure_id;
    dg.floor[coord.y][coord.x].feature_id = TILE_BLOCKED_FLOOR;
    inventoryItemCopyTo(config::dungeon::objects::OBJ_RUBBLE, game.treasure.list[free_treasure_id]);
}

// Places a treasure (Gold or Gems) at given row, column -RAK-
void dungeonPlaceGold(Coord_t const &coord) {
    int free_treasure_id = popt();

    int gold_type_id = ((randomNumber(dg.current_level + 2) + 2) / 2) - 1;

    if (randomNumber(config::treasure::TREASURE_CHANCE_OF_GREAT_ITEM) == 1) {
        gold_type_id += randomNumber(dg.current_level + 1);
    }

    if (gold_type_id >= config::dungeon::objects::MAX_GOLD_TYPES) {
        gold_type_id = config::dungeon::objects::MAX_GOLD_TYPES - 1;
    }

    dg.floor[coord.y][coord.x].treasure_id = (uint8_t) free_treasure_id;
    inventoryItemCopyTo(config::dungeon::objects::OBJ_GOLD_LIST + gold_type_id, game.treasure.list[free_treasure_id]);
    game.treasure.list[free_treasure_id].cost += (8L * (int32_t) randomNumber((int) game.treasure.list[free_treasure_id].cost)) + randomNumber(8);

    if (dg.floor[coord.y][coord.x].creature_id == 1) {
        printMessage("You feel something roll beneath your feet.");
    }
}

// Places an object at given row, column co-ordinate -RAK-
void dungeonPlaceRandomObjectAt(Coord_t const &coord, bool must_be_small) {
    int free_treasure_id = popt();

    dg.floor[coord.y][coord.x].treasure_id = (uint8_t) free_treasure_id;

    int object_id = itemGetRandomObjectId(dg.current_level, must_be_small);
    inventoryItemCopyTo(sorted_objects[object_id], game.treasure.list[free_treasure_id]);

    magicTreasureMagicalAbility(free_treasure_id, dg.current_level);

    if (dg.floor[coord.y][coord.x].creature_id == 1) {
        printMessage("You feel something roll beneath your feet."); // -CJS-
    }
}

// Allocates an object for tunnels and rooms -RAK-
void dungeonAllocateAndPlaceObject(bool (*set_function)(int), int object_type, int number) {
    Coord_t coord = Coord_t{0, 0};

    for (int i = 0; i < number; i++) {
        // don't put an object beneath the player, this could cause
        // problems if player is standing under rubble, or on a trap.
        do {
            coord.y = randomNumber(dg.height) - 1;
            coord.x = randomNumber(dg.width) - 1;
        } while (!(*set_function)(dg.floor[coord.y][coord.x].feature_id) || dg.floor[coord.y][coord.x].treasure_id != 0 || (coord.y == py.pos.y && coord.x == py.pos.x));

        switch (object_type) {
            case 1:
                dungeonSetTrap(coord, randomNumber(config::dungeon::objects::MAX_TRAPS) - 1);
                break;
            case 2:
                // NOTE: object_type == 2 is no longer used - it used to be visible traps.
                // FIXME: there was no `break` here so `case 3` catches it? -MRC-
            case 3:
                dungeonPlaceRubble(coord);
                break;
            case 4:
                dungeonPlaceGold(coord);
                break;
            case 5:
                dungeonPlaceRandomObjectAt(coord, false);
                break;
            default:
                break;
        }
    }
}

// Creates objects nearby the coordinates given -RAK-
void dungeonPlaceRandomObjectNear(Coord_t coord, int tries) {
    do {
        for (int i = 0; i <= 10; i++) {
            Coord_t at = Coord_t{
                coord.y - 3 + randomNumber(5),
                coord.x - 4 + randomNumber(7),
            };

            if (coordInBounds(at) && dg.floor[at.y][at.x].feature_id <= MAX_CAVE_FLOOR && dg.floor[at.y][at.x].treasure_id == 0) {
                if (randomNumber(100) < 75) {
                    dungeonPlaceRandomObjectAt(at, false);
                } else {
                    dungeonPlaceGold(at);
                }
                i = 9;
            }
        }

        tries--;
    } while (tries != 0);
}

// Moves creature record from one space to another -RAK-
// this always works correctly, even if y1==y2 and x1==x2
void dungeonMoveCreatureRecord(Coord_t const &from, Coord_t const &to) {
    int id = dg.floor[from.y][from.x].creature_id;
    dg.floor[from.y][from.x].creature_id = 0;
    dg.floor[to.y][to.x].creature_id = (uint8_t) id;
}

// Room is lit, make it appear -RAK-
void dungeonLightRoom(Coord_t const &coord) {
    int height_middle = (SCREEN_HEIGHT / 2);
    int width_middle = (SCREEN_WIDTH / 2);

    int top = (coord.y / height_middle) * height_middle;
    int left = (coord.x / width_middle) * width_middle;
    int bottom = top + height_middle - 1;
    int right = left + width_middle - 1;

    Coord_t location = Coord_t{0, 0};

    for (location.y = top; location.y <= bottom; location.y++) {
        for (location.x = left; location.x <= right; location.x++) {
            Tile_t &tile = dg.floor[location.y][location.x];

            if (tile.perma_lit_room && !tile.permanent_light) {
                tile.permanent_light = true;

                if (tile.feature_id == TILE_DARK_FLOOR) {
                    tile.feature_id = TILE_LIGHT_FLOOR;
                }
                if (!tile.field_mark && tile.treasure_id != 0) {
                    int treasure_id = game.treasure.list[tile.treasure_id].category_id;
                    if (treasure_id >= TV_MIN_VISIBLE && treasure_id <= TV_MAX_VISIBLE) {
                        tile.field_mark = true;
                    }
                }
                panelPutTile(caveGetTileSymbol(location), location);
            }
        }
    }
}

// Lights up given location -RAK-
void dungeonLiteSpot(Coord_t const &coord) {
    if (!coordInsidePanel(coord)) {
        return;
    }

    char symbol = caveGetTileSymbol(coord);
    panelPutTile(symbol, coord);
}

// Normal movement
// When FIND_FLAG,  light only permanent features
static void sub1MoveLight(Coord_t const &from, Coord_t const &to) {
    if (py.temporary_light_only) {
        // Turn off lamp light
        for (int y = from.y - 1; y <= from.y + 1; y++) {
            for (int x = from.x - 1; x <= from.x + 1; x++) {
                dg.floor[y][x].temporary_light = false;
            }
        }
        if ((py.running_tracker != 0) && !config::options::run_print_self) {
            py.temporary_light_only = false;
        }
    } else if ((py.running_tracker == 0) || config::options::run_print_self) {
        py.temporary_light_only = true;
    }

    for (int y = to.y - 1; y <= to.y + 1; y++) {
        for (int x = to.x - 1; x <= to.x + 1; x++) {
            Tile_t &tile = dg.floor[y][x];

            // only light up if normal movement
            if (py.temporary_light_only) {
                tile.temporary_light = true;
            }

            if (tile.feature_id >= MIN_CAVE_WALL) {
                tile.permanent_light = true;
            } else if (!tile.field_mark && tile.treasure_id != 0) {
                int tval = game.treasure.list[tile.treasure_id].category_id;

                if (tval >= TV_MIN_VISIBLE && tval <= TV_MAX_VISIBLE) {
                    tile.field_mark = true;
                }
            }
        }
    }

    // From uppermost to bottom most lines player was on.
    int top, left, bottom, right;

    if (from.y < to.y) {
        top = from.y - 1;
        bottom = to.y + 1;
    } else {
        top = to.y - 1;
        bottom = from.y + 1;
    }
    if (from.x < to.x) {
        left = from.x - 1;
        right = to.x + 1;
    } else {
        left = to.x - 1;
        right = from.x + 1;
    }

    Coord_t coord = Coord_t{0, 0};
    for (coord.y = top; coord.y <= bottom; coord.y++) {
        // Leftmost to rightmost do
        for (coord.x = left; coord.x <= right; coord.x++) {
            panelPutTile(caveGetTileSymbol(coord), coord);
        }
    }
}

// When blinded,  move only the player symbol.
// With no light,  movement becomes involved.
static void sub3MoveLight(Coord_t const &from, Coord_t const &to) {
    if (py.temporary_light_only) {
        Coord_t coord = Coord_t{0, 0};

        for (coord.y = from.y - 1; coord.y <= from.y + 1; coord.y++) {
            for (coord.x = from.x - 1; coord.x <= from.x + 1; coord.x++) {
                dg.floor[coord.y][coord.x].temporary_light = false;
                panelPutTile(caveGetTileSymbol(coord), coord);
            }
        }

        py.temporary_light_only = false;
    } else if ((py.running_tracker == 0) || config::options::run_print_self) {
        panelPutTile(caveGetTileSymbol(from), from);
    }

    if ((py.running_tracker == 0) || config::options::run_print_self) {
        panelPutTile('@', to);
    }
}

// Package for moving the character's light about the screen
// Four cases : Normal, Finding, Blind, and No light -RAK-
void dungeonMoveCharacterLight(Coord_t const &from, Coord_t const &to) {
    if (py.flags.blind > 0 || !py.carrying_light) {
        sub3MoveLight(from, to);
    } else {
        sub1MoveLight(from, to);
    }
}

// Deletes a monster entry from the level -RAK-
//
// If used within updateMonsters(), deleting a monster while scanning the
// monsters causes two problems;
//   1. monsters might get two turns
//   2. m_ptr/monptr might be invalid after running this function
// Hence a two step process is provided for updateMonsters().
void dungeonDeleteMonster(int id) {
    dungeonRemoveMonsterFromLevel(id);
    dungeonDeleteMonsterRecord(id);
}

// dungeonRemoveMonsterFromLevel ensures the monster has no HP before removing
// its ID from the dungeon level.
// This is called in breath(), and a couple of places in creatures.c.
void dungeonRemoveMonsterFromLevel(int id) {
    Monster_t &monster = monsters[id];

    // Force the HP negative to ensure that the monster is dead. For example, if the
    // monster was just eaten by another, it will still have positive hit points.
    monster.hp = -1;

    dg.floor[monster.pos.y][monster.pos.x].creature_id = 0;

    if (monster.lit) {
        dungeonLiteSpot(Coord_t{monster.pos.y, monster.pos.x});
    }

    if (monster_multiply_total > 0) {
        monster_multiply_total--;
    }
}

// dungeonDeleteMonsterRecord delete the monster record from the monsters list.
// Called by updateMonsters() and dungeonDeleteMonster() only.
void dungeonDeleteMonsterRecord(int id) {
    int last_id = next_free_monster_id - 1;
    Monster_t &monster = monsters[last_id];

    if (id != last_id) {
        dg.floor[monster.pos.y][monster.pos.x].creature_id = (uint8_t) id;
        monsters[id] = monsters[last_id];
    }

    monsters[last_id] = blank_monster;
    next_free_monster_id--;
}

// Creates objects nearby the coordinates given -RAK-
int dungeonSummonObject(Coord_t coord, int amount, int object_type) {
    int real_type;

    if (object_type == 1 || object_type == 5) {
        real_type = 1; // object_type == 1 -> objects
    } else {
        real_type = 256; // object_type == 2 -> gold
    }

    int result = 0;

    do {
        for (int tries = 0; tries <= 20; tries++) {
            Coord_t at = Coord_t{
                coord.y - 3 + randomNumber(5),
                coord.x - 3 + randomNumber(5),
            };

            if (coordInBounds(at) && los(coord, at)) {
                if (dg.floor[at.y][at.x].feature_id <= MAX_OPEN_SPACE && dg.floor[at.y][at.x].treasure_id == 0) {
                    // object_type == 3 -> 50% objects, 50% gold
                    if (object_type == 3 || object_type == 7) {
                        if (randomNumber(100) < 50) {
                            real_type = 1;
                        } else {
                            real_type = 256;
                        }
                    }

                    if (real_type == 1) {
                        dungeonPlaceRandomObjectAt(at, (object_type >= 4));
                    } else {
                        dungeonPlaceGold(at);
                    }

                    dungeonLiteSpot(at);

                    if (caveTileVisible(at)) {
                        result += real_type;
                    }

                    tries = 20;
                }
            }
        }

        amount--;
    } while (amount != 0);

    return result;
}

// Deletes object from given location -RAK-
bool dungeonDeleteObject(Coord_t const &coord) {
    Tile_t &tile = dg.floor[coord.y][coord.x];

    if (tile.feature_id == TILE_BLOCKED_FLOOR) {
        tile.feature_id = TILE_CORR_FLOOR;
    }

    pusht(tile.treasure_id);

    tile.treasure_id = 0;
    tile.field_mark = false;

    dungeonLiteSpot(coord);

    return caveTileVisible(coord);
}
