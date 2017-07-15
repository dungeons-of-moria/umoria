// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Initialize/create a dungeon or town level

#include "headers.h"
#include "externs.h"

typedef struct {
    int x, y;
} Coord_t;

static Coord_t doorstk[100];
static int doorindex;

// Returns a Dark/Light floor tile based on current_dungeon_level, and random number
static uint8_t floorTileForDungeonLevel() {
    if (current_dungeon_level <= randomNumber(25)) {
        return LIGHT_FLOOR;
    }
    return DARK_FLOOR;
}

// Always picks a correct direction
static void correct_dir(int *rowDir, int *colDir, int y1, int x1, int y2, int x2) {
    if (y1 < y2) {
        *rowDir = 1;
    } else if (y1 == y2) {
        *rowDir = 0;
    } else {
        *rowDir = -1;
    }

    if (x1 < x2) {
        *colDir = 1;
    } else if (x1 == x2) {
        *colDir = 0;
    } else {
        *colDir = -1;
    }

    if (*rowDir != 0 && *colDir != 0) {
        if (randomNumber(2) == 1) {
            *rowDir = 0;
        } else {
            *colDir = 0;
        }
    }
}

// Chance of wandering direction
static void rand_dir(int *rowDir, int *colDir) {
    int tmp = randomNumber(4);
    if (tmp < 3) {
        *colDir = 0;
        *rowDir = -3 + (tmp << 1); // tmp=1 -> *rdir=-1; tmp=2 -> *rdir=1
    } else {
        *rowDir = 0;
        *colDir = -7 + (tmp << 1); // tmp=3 -> *cdir=-1; tmp=4 -> *cdir=1
    }
}

// Blanks out entire cave -RAK-
static void blank_cave() {
    memset((char *) &cave[0][0], 0, sizeof(cave));
}

// Fills in empty spots with desired rock -RAK-
// Note: 9 is a temporary value.
static void fill_cave(uint8_t rockType) {
    // no need to check the border of the cave
    for (int y = dungeon_height - 2; y > 0; y--) {
        int x = 1;

        for (int j = dungeon_width - 2; j > 0; j--) {
            if (cave[y][x].fval == NULL_WALL || cave[y][x].fval == TMP1_WALL || cave[y][x].fval == TMP2_WALL) {
                cave[y][x].fval = rockType;
            }
            x++;
        }
    }
}

#ifdef DEBUG
#include <assert.h>
#endif

// Places indestructible rock around edges of dungeon -RAK-
static void place_boundary() {
    Cave_t(*left_ptr)[MAX_WIDTH];
    Cave_t(*right_ptr)[MAX_WIDTH];

    // put permanent wall on leftmost row and rightmost row
    left_ptr = (Cave_t(*)[MAX_WIDTH]) &cave[0][0];
    right_ptr = (Cave_t(*)[MAX_WIDTH]) &cave[0][dungeon_width - 1];

    for (int i = 0; i < dungeon_height; i++) {
#ifdef DEBUG
        assert((Cave_t *)left_ptr == &cave[i][0]);
        assert((Cave_t *)right_ptr == &cave[i][dungeon_width - 1]);
#endif

        ((Cave_t *) left_ptr)->fval = BOUNDARY_WALL;
        left_ptr++;

        ((Cave_t *) right_ptr)->fval = BOUNDARY_WALL;
        right_ptr++;
    }

    // put permanent wall on top row and bottom row
    Cave_t *top_ptr = &cave[0][0];
    Cave_t *bottom_ptr = &cave[dungeon_height - 1][0];

    for (int i = 0; i < dungeon_width; i++) {
#ifdef DEBUG
        assert(top_ptr == &cave[0][i]);
        assert(bottom_ptr == &cave[dungeon_height - 1][i]);
#endif
        top_ptr->fval = BOUNDARY_WALL;
        top_ptr++;

        bottom_ptr->fval = BOUNDARY_WALL;
        bottom_ptr++;
    }
}

// Places "streamers" of rock through dungeon -RAK-
static void place_streamer(uint8_t rockType, int treas_chance) {
    // Choose starting point and direction
    int y = (dungeon_height / 2) + 11 - randomNumber(23);
    int x = (dungeon_width / 2) + 16 - randomNumber(33);

    // Get random direction. Numbers 1-4, 6-9
    int dir = randomNumber(8);
    if (dir > 4) {
        dir += 1;
    }

    // Place streamer into dungeon
    int t1 = 2 * DUN_STR_RNG + 1; // Constants
    int t2 = DUN_STR_RNG + 1;

    do {
        for (int i = 0; i < DUN_STR_DEN; i++) {
            int ty = y + randomNumber(t1) - t2;
            int tx = x + randomNumber(t1) - t2;

            if (coordInBounds(ty, tx)) {
                if (cave[ty][tx].fval == GRANITE_WALL) {
                    cave[ty][tx].fval = rockType;

                    if (randomNumber(treas_chance) == 1) {
                        dungeonPlaceGold(ty, tx);
                    }
                }
            }
        }
    } while (playerMovePosition(dir, &y, &x));
}

static void place_open_door(int y, int x) {
    int cur_pos = popt();
    cave[y][x].tptr = (uint8_t) cur_pos;
    inventoryItemCopyTo(OBJ_OPEN_DOOR, &treasure_list[cur_pos]);
    cave[y][x].fval = CORR_FLOOR;
}

static void place_broken_door(int y, int x) {
    int cur_pos = popt();
    cave[y][x].tptr = (uint8_t) cur_pos;
    inventoryItemCopyTo(OBJ_OPEN_DOOR, &treasure_list[cur_pos]);
    cave[y][x].fval = CORR_FLOOR;
    treasure_list[cur_pos].p1 = 1;
}

static void place_closed_door(int y, int x) {
    int cur_pos = popt();
    cave[y][x].tptr = (uint8_t) cur_pos;
    inventoryItemCopyTo(OBJ_CLOSED_DOOR, &treasure_list[cur_pos]);
    cave[y][x].fval = BLOCKED_FLOOR;
}

static void place_locked_door(int y, int x) {
    int cur_pos = popt();
    cave[y][x].tptr = (uint8_t) cur_pos;
    inventoryItemCopyTo(OBJ_CLOSED_DOOR, &treasure_list[cur_pos]);
    cave[y][x].fval = BLOCKED_FLOOR;
    treasure_list[cur_pos].p1 = (int16_t) (randomNumber(10) + 10);
}

static void place_stuck_door(int y, int x) {
    int cur_pos = popt();
    cave[y][x].tptr = (uint8_t) cur_pos;
    inventoryItemCopyTo(OBJ_CLOSED_DOOR, &treasure_list[cur_pos]);
    cave[y][x].fval = BLOCKED_FLOOR;
    treasure_list[cur_pos].p1 = (int16_t) (-randomNumber(10) - 10);
}

static void place_secret_door(int y, int x) {
    int cur_pos = popt();
    cave[y][x].tptr = (uint8_t) cur_pos;
    inventoryItemCopyTo(OBJ_SECRET_DOOR, &treasure_list[cur_pos]);
    cave[y][x].fval = BLOCKED_FLOOR;
}

static void place_door(int y, int x) {
    int doorType = randomNumber(3);

    if (doorType == 1) {
        if (randomNumber(4) == 1) {
            place_broken_door(y, x);
        } else {
            place_open_door(y, x);
        }
    } else if (doorType == 2) {
        doorType = randomNumber(12);

        if (doorType > 3) {
            place_closed_door(y, x);
        } else if (doorType == 3) {
            place_stuck_door(y, x);
        } else {
            place_locked_door(y, x);
        }
    } else {
        place_secret_door(y, x);
    }
}

// Place an up staircase at given y, x -RAK-
static void place_up_stairs(int y, int x) {
    if (cave[y][x].tptr != 0) {
        (void) dungeonDeleteObject(y, x);
    }

    int cur_pos = popt();
    cave[y][x].tptr = (uint8_t) cur_pos;
    inventoryItemCopyTo(OBJ_UP_STAIR, &treasure_list[cur_pos]);
}

// Place a down staircase at given y, x -RAK-
static void place_down_stairs(int y, int x) {
    if (cave[y][x].tptr != 0) {
        (void) dungeonDeleteObject(y, x);
    }

    int cur_pos = popt();
    cave[y][x].tptr = (uint8_t) cur_pos;
    inventoryItemCopyTo(OBJ_DOWN_STAIR, &treasure_list[cur_pos]);
}

// Places a staircase 1=up, 2=down -RAK-
static void place_stairs(int stairType, int num, int walls) {
    for (int i = 0; i < num; i++) {
        bool flag = false;

        while (!flag) {
            int j = 0;

            do {
                // Note:
                // don't let y1/x1 be zero,
                // don't let y2/x2 be equal to dungeon_height-1/dungeon_width-1,
                // these values are always BOUNDARY_ROCK.
                int y1 = randomNumber(dungeon_height - 14);
                int x1 = randomNumber(dungeon_width - 14);
                int y2 = y1 + 12;
                int x2 = x1 + 12;

                do {
                    do {
                        if (cave[y1][x1].fval <= MAX_OPEN_SPACE && cave[y1][x1].tptr == 0 && coordWallsNextTo(y1, x1) >= walls) {
                            flag = true;
                            if (stairType == 1) {
                                place_up_stairs(y1, x1);
                            } else {
                                place_down_stairs(y1, x1);
                            }
                        }
                        x1++;
                    } while ((x1 != x2) && (!flag));

                    x1 = x2 - 12;
                    y1++;
                } while ((y1 != y2) && (!flag));

                j++;
            } while ((!flag) && (j <= 30));

            walls--;
        }
    }
}

// Place a trap with a given displacement of point -RAK-
static void vault_trap(int y, int x, int yd, int xd, int num) {
    for (int i = 0; i < num; i++) {
        bool flag = false;

        for (int count = 0; !flag && count <= 5; count++) {
            int y1 = y - yd - 1 + randomNumber(2 * yd + 1);
            int x1 = x - xd - 1 + randomNumber(2 * xd + 1);

            if (cave[y1][x1].fval != NULL_WALL && cave[y1][x1].fval <= MAX_CAVE_FLOOR && cave[y1][x1].tptr == 0) {
                dungeonSetTrap(y1, x1, randomNumber(MAX_TRAP) - 1);
                flag = true;
            }
        }
    }
}

// Place a trap with a given displacement of point -RAK-
static void vault_monster(int y, int x, int num) {
    int y1, x1;

    for (int i = 0; i < num; i++) {
        y1 = y;
        x1 = x;
        (void) monsterSummon(&y1, &x1, true);
    }
}

// Builds a room at a row, column coordinate -RAK-
static void build_room(int y, int x) {
    uint8_t floor = floorTileForDungeonLevel();

    int height = y - randomNumber(4);
    int depth = y + randomNumber(3);
    int left = x - randomNumber(11);
    int right = x + randomNumber(11);

    // the x dim of rooms tends to be much larger than the y dim,
    // so don't bother rewriting the y loop.

    for (int i = height; i <= depth; i++) {
        for (int j = left; j <= right; j++) {
            cave[i][j].fval = floor;
            cave[i][j].lr = true;
        }
    }

    for (int i = (height - 1); i <= (depth + 1); i++) {
        cave[i][left - 1].fval = GRANITE_WALL;
        cave[i][left - 1].lr = true;

        cave[i][right + 1].fval = GRANITE_WALL;
        cave[i][right + 1].lr = true;
    }

    for (int i = left; i <= right; i++) {
        cave[height - 1][i].fval = GRANITE_WALL;
        cave[height - 1][i].lr = true;

        cave[depth + 1][i].fval = GRANITE_WALL;
        cave[depth + 1][i].lr = true;
    }
}

// Builds a room at a row, column coordinate -RAK-
// Type 1 unusual rooms are several overlapping rectangular ones
static void build_type1(int y, int x) {
    uint8_t floor = floorTileForDungeonLevel();

    int limit = 1 + randomNumber(2);

    for (int i0 = 0; i0 < limit; i0++) {
        int height = y - randomNumber(4);
        int depth = y + randomNumber(3);
        int left = x - randomNumber(11);
        int right = x + randomNumber(11);

        // the x dim of rooms tends to be much larger than the y dim,
        // so don't bother rewriting the y loop.

        for (int i = height; i <= depth; i++) {
            for (int j = left; j <= right; j++) {
                cave[i][j].fval = floor;
                cave[i][j].lr = true;
            }
        }
        for (int i = (height - 1); i <= (depth + 1); i++) {
            if (cave[i][left - 1].fval != floor) {
                cave[i][left - 1].fval = GRANITE_WALL;
                cave[i][left - 1].lr = true;
            }

            if (cave[i][right + 1].fval != floor) {
                cave[i][right + 1].fval = GRANITE_WALL;
                cave[i][right + 1].lr = true;
            }
        }

        for (int i = left; i <= right; i++) {
            if (cave[height - 1][i].fval != floor) {
                cave[height - 1][i].fval = GRANITE_WALL;
                cave[height - 1][i].lr = true;
            }

            if (cave[depth + 1][i].fval != floor) {
                cave[depth + 1][i].fval = GRANITE_WALL;
                cave[depth + 1][i].lr = true;
            }
        }
    }
}

static void placeRandomSecretDoor(int y, int x, int depth, int height, int left, int right) {
    switch (randomNumber(4)) {
        case 1:
            place_secret_door(height - 1, x);
            break;
        case 2:
            place_secret_door(depth + 1, x);
            break;
        case 3:
            place_secret_door(y, left - 1);
            break;
        default:
            place_secret_door(y, right + 1);
            break;
    }
}

static void placeVault(int y, int x) {
    for (int i = y - 1; i <= y + 1; i++) {
        cave[i][x - 1].fval = TMP1_WALL;
        cave[i][x + 1].fval = TMP1_WALL;
    }

    cave[y - 1][x].fval = TMP1_WALL;
    cave[y + 1][x].fval = TMP1_WALL;
}

static void placeTreasureVault(int y, int x, int depth, int height, int left, int right) {
    placeRandomSecretDoor(y, x, depth, height, left, right);

    placeVault(y, x);

    // Place a locked door
    int offset = randomNumber(4);
    if (offset < 3) {
        // 1 -> y-1; 2 -> y+1
        place_locked_door(y - 3 + (offset << 1), x);
    } else {
        place_locked_door(y, x - 7 + (offset << 1));
    }
}

static void placeInnerPillars(int y, int x) {
    for (int i = y - 1; i <= y + 1; i++) {
        for (int j = x - 1; j <= x + 1; j++) {
            cave[i][j].fval = TMP1_WALL;
        }
    }

    if (randomNumber(2) != 1) {
        return;
    }

    int offset = randomNumber(2);

    for (int i = y - 1; i <= y + 1; i++) {
        for (int j = x - 5 - offset; j <= x - 3 - offset; j++) {
            cave[i][j].fval = TMP1_WALL;
        }
    }

    for (int i = y - 1; i <= y + 1; i++) {
        for (int j = x + 3 + offset; j <= x + 5 + offset; j++) {
            cave[i][j].fval = TMP1_WALL;
        }
    }
}

static void placeMazeInsideRoom(int depth, int height, int left, int right) {
    for (int y = height; y <= depth; y++) {
        for (int x = left; x <= right; x++) {
            if (0x1 & (x + y)) {
                cave[y][x].fval = TMP1_WALL;
            }
        }
    }
}

static void placeFourSmallRooms(int y, int x, int depth, int height, int left, int right) {
    for (int i = height; i <= depth; i++) {
        cave[i][x].fval = TMP1_WALL;
    }

    for (int i = left; i <= right; i++) {
        cave[y][i].fval = TMP1_WALL;
    }

    // place random secret door
    if (randomNumber(2) == 1) {
        int offsetX = randomNumber(10);
        place_secret_door(height - 1, x - offsetX);
        place_secret_door(height - 1, x + offsetX);
        place_secret_door(depth + 1, x - offsetX);
        place_secret_door(depth + 1, x + offsetX);
    } else {
        int offsetY = randomNumber(3);
        place_secret_door(y + offsetY, left - 1);
        place_secret_door(y - offsetY, left - 1);
        place_secret_door(y + offsetY, right + 1);
        place_secret_door(y - offsetY, right + 1);
    }
}

// Builds an unusual room at a row, column coordinate -RAK-
// Type 2 unusual rooms all have an inner room:
//   1 - Just an inner room with one door
//   2 - An inner room within an inner room
//   3 - An inner room with pillar(s)
//   4 - Inner room has a maze
//   5 - A set of four inner rooms
static void build_type2(int y, int x) {
    uint8_t floor = floorTileForDungeonLevel();

    int height = y - 4;
    int depth = y + 4;
    int left = x - 11;
    int right = x + 11;

    // the x dim of rooms tends to be much larger than the y dim,
    // so don't bother rewriting the y loop.

    for (int i = height; i <= depth; i++) {
        for (int j = left; j <= right; j++) {
            cave[i][j].fval = floor;
            cave[i][j].lr = true;
        }
    }

    for (int i = (height - 1); i <= (depth + 1); i++) {
        cave[i][left - 1].fval = GRANITE_WALL;
        cave[i][left - 1].lr = true;

        cave[i][right + 1].fval = GRANITE_WALL;
        cave[i][right + 1].lr = true;
    }

    for (int i = left; i <= right; i++) {
        cave[height - 1][i].fval = GRANITE_WALL;
        cave[height - 1][i].lr = true;

        cave[depth + 1][i].fval = GRANITE_WALL;
        cave[depth + 1][i].lr = true;
    }

    // The inner room
    height = height + 2;
    depth = depth - 2;
    left = left + 2;
    right = right - 2;

    for (int i = (height - 1); i <= (depth + 1); i++) {
        cave[i][left - 1].fval = TMP1_WALL;
        cave[i][right + 1].fval = TMP1_WALL;
    }

    for (int i = left; i <= right; i++) {
        cave[height - 1][i].fval = TMP1_WALL;
        cave[depth + 1][i].fval = TMP1_WALL;
    }

    // Inner room variations
    switch (randomNumber(5)) {
        case 1: // Just an inner room.
            placeRandomSecretDoor(y, x, depth, height, left, right);
            vault_monster(y, x, 1);
            break;
        case 2: // Treasure Vault
            placeTreasureVault(y, x, depth, height, left, right);

            // Guard the treasure well
            vault_monster(y, x, 2 + randomNumber(3));

            // If the monsters don't get 'em.
            vault_trap(y, x, 4, 10, 2 + randomNumber(3));
            break;
        case 3: // Inner pillar(s).
            placeRandomSecretDoor(y, x, depth, height, left, right);

            placeInnerPillars(y, x);

            if (randomNumber(3) != 1) {
                break;
            }

            // Inner rooms
            for (int i = x - 5; i <= x + 5; i++) {
                cave[y - 1][i].fval = TMP1_WALL;
                cave[y + 1][i].fval = TMP1_WALL;
            }
            cave[y][x - 5].fval = TMP1_WALL;
            cave[y][x + 5].fval = TMP1_WALL;

            place_secret_door(y - 3 + (randomNumber(2) << 1), x - 3);
            place_secret_door(y - 3 + (randomNumber(2) << 1), x + 3);

            if (randomNumber(3) == 1) {
                dungeonPlaceRandomObjectAt(y, x - 2, false);
            }

            if (randomNumber(3) == 1) {
                dungeonPlaceRandomObjectAt(y, x + 2, false);
            }

            vault_monster(y, x - 2, randomNumber(2));
            vault_monster(y, x + 2, randomNumber(2));
            break;
        case 4: // Maze inside.
            placeRandomSecretDoor(y, x, depth, height, left, right);

            placeMazeInsideRoom(depth, height, left, right);

            // Monsters just love mazes.
            vault_monster(y, x - 5, randomNumber(3));
            vault_monster(y, x + 5, randomNumber(3));

            // Traps make them entertaining.
            vault_trap(y, x - 3, 2, 8, randomNumber(3));
            vault_trap(y, x + 3, 2, 8, randomNumber(3));

            // Mazes should have some treasure too..
            for (int i = 0; i < 3; i++) {
                dungeonPlaceRandomObjectNear(y, x, 1);
            }
            break;
        case 5: // Four small rooms.
            placeFourSmallRooms(y, x, depth, height, left, right);

            // Treasure in each one.
            dungeonPlaceRandomObjectNear(y, x, 2 + randomNumber(2));

            // Gotta have some monsters.
            vault_monster(y + 2, x - 4, randomNumber(2));
            vault_monster(y + 2, x + 4, randomNumber(2));
            vault_monster(y - 2, x - 4, randomNumber(2));
            vault_monster(y - 2, x + 4, randomNumber(2));
            break;
    }
}

static void placeLargeMiddlePillar(int y, int x) {
    for (int i = y - 1; i <= y + 1; i++) {
        for (int j = x - 1; j <= x + 1; j++) {
            cave[i][j].fval = TMP1_WALL;
        }
    }
}

// Builds a room at a row, column coordinate -RAK-
// Type 3 unusual rooms are cross shaped
static void build_type3(int y, int x) {
    uint8_t floor = floorTileForDungeonLevel();

    int randomOffset = 2 + randomNumber(2);

    int height = y - randomOffset;
    int depth = y + randomOffset;
    int left = x - 1;
    int right = x + 1;

    for (int i = height; i <= depth; i++) {
        for (int j = left; j <= right; j++) {
            cave[i][j].fval = floor;
            cave[i][j].lr = true;
        }
    }

    for (int i = height - 1; i <= depth + 1; i++) {
        cave[i][left - 1].fval = GRANITE_WALL;
        cave[i][left - 1].lr = true;

        cave[i][right + 1].fval = GRANITE_WALL;
        cave[i][right + 1].lr = true;
    }

    for (int i = left; i <= right; i++) {
        cave[height - 1][i].fval = GRANITE_WALL;
        cave[height - 1][i].lr = true;

        cave[depth + 1][i].fval = GRANITE_WALL;
        cave[depth + 1][i].lr = true;
    }

    randomOffset = 2 + randomNumber(9);

    height = y - 1;
    depth = y + 1;
    left = x - randomOffset;
    right = x + randomOffset;

    for (int i = height; i <= depth; i++) {
        for (int j = left; j <= right; j++) {
            cave[i][j].fval = floor;
            cave[i][j].lr = true;
        }
    }

    for (int i = height - 1; i <= depth + 1; i++) {
        if (cave[i][left - 1].fval != floor) {
            cave[i][left - 1].fval = GRANITE_WALL;
            cave[i][left - 1].lr = true;
        }

        if (cave[i][right + 1].fval != floor) {
            cave[i][right + 1].fval = GRANITE_WALL;
            cave[i][right + 1].lr = true;
        }
    }

    for (int i = left; i <= right; i++) {
        if (cave[height - 1][i].fval != floor) {
            cave[height - 1][i].fval = GRANITE_WALL;
            cave[height - 1][i].lr = true;
        }

        if (cave[depth + 1][i].fval != floor) {
            cave[depth + 1][i].fval = GRANITE_WALL;
            cave[depth + 1][i].lr = true;
        }
    }

    // Special features.
    switch (randomNumber(4)) {
        case 1: // Large middle pillar
            placeLargeMiddlePillar(y, x);
            break;
        case 2: // Inner treasure vault
            placeVault(y, x);

            // Place a secret door
            randomOffset = randomNumber(4);
            if (randomOffset < 3) {
                place_secret_door(y - 3 + (randomOffset << 1), x);
            } else {
                place_secret_door(y, x - 7 + (randomOffset << 1));
            }

            // Place a treasure in the vault
            dungeonPlaceRandomObjectAt(y, x, false);

            // Let's guard the treasure well.
            vault_monster(y, x, 2 + randomNumber(2));

            // Traps naturally
            vault_trap(y, x, 4, 4, 1 + randomNumber(3));
            break;
        case 3:
            if (randomNumber(3) == 1) {
                cave[y - 1][x - 2].fval = TMP1_WALL;
                cave[y + 1][x - 2].fval = TMP1_WALL;
                cave[y - 1][x + 2].fval = TMP1_WALL;
                cave[y + 1][x + 2].fval = TMP1_WALL;
                cave[y - 2][x - 1].fval = TMP1_WALL;
                cave[y - 2][x + 1].fval = TMP1_WALL;
                cave[y + 2][x - 1].fval = TMP1_WALL;
                cave[y + 2][x + 1].fval = TMP1_WALL;
                if (randomNumber(3) == 1) {
                    place_secret_door(y, x - 2);
                    place_secret_door(y, x + 2);
                    place_secret_door(y - 2, x);
                    place_secret_door(y + 2, x);
                }
            } else if (randomNumber(3) == 1) {
                cave[y][x].fval = TMP1_WALL;
                cave[y - 1][x].fval = TMP1_WALL;
                cave[y + 1][x].fval = TMP1_WALL;
                cave[y][x - 1].fval = TMP1_WALL;
                cave[y][x + 1].fval = TMP1_WALL;
            } else if (randomNumber(3) == 1) {
                cave[y][x].fval = TMP1_WALL;
            }
            break;
        case 4:
            // no special feature!
            break;
    }
}

// Constructs a tunnel between two points
static void build_tunnel(int row1, int col1, int row2, int col2) {
    Coord_t tunstk[1000], wallstk[1000];

    // Main procedure for Tunnel
    // Note: 9 is a temporary value
    bool door_flag = false;
    bool stop_flag = false;
    int main_loop_count = 0;
    int start_row = row1;
    int start_col = col1;
    int tunindex = 0;
    int wallindex = 0;

    int row_dir, col_dir;
    correct_dir(&row_dir, &col_dir, row1, col1, row2, col2);

    do {
        // prevent infinite loops, just in case
        main_loop_count++;
        if (main_loop_count > 2000) {
            stop_flag = true;
        }

        if (randomNumber(100) > DUN_TUN_CHG) {
            if (randomNumber(DUN_TUN_RND) == 1) {
                rand_dir(&row_dir, &col_dir);
            } else {
                correct_dir(&row_dir, &col_dir, row1, col1, row2, col2);
            }
        }

        int tmp_row = row1 + row_dir;
        int tmp_col = col1 + col_dir;

        while (!coordInBounds(tmp_row, tmp_col)) {
            if (randomNumber(DUN_TUN_RND) == 1) {
                rand_dir(&row_dir, &col_dir);
            } else {
                correct_dir(&row_dir, &col_dir, row1, col1, row2, col2);
            }
            tmp_row = row1 + row_dir;
            tmp_col = col1 + col_dir;
        }

        switch (cave[tmp_row][tmp_col].fval) {
            case NULL_WALL:
                row1 = tmp_row;
                col1 = tmp_col;
                if (tunindex < 1000) {
                    tunstk[tunindex].y = row1;
                    tunstk[tunindex].x = col1;
                    tunindex++;
                }
                door_flag = false;
                break;
            case TMP2_WALL:
                // do nothing
                break;
            case GRANITE_WALL:
                row1 = tmp_row;
                col1 = tmp_col;

                if (wallindex < 1000) {
                    wallstk[wallindex].y = row1;
                    wallstk[wallindex].x = col1;
                    wallindex++;
                }

                for (int y = row1 - 1; y <= row1 + 1; y++) {
                    for (int x = col1 - 1; x <= col1 + 1; x++) {
                        if (coordInBounds(y, x)) {
                            // values 11 and 12 are impossible here, place_streamer
                            // is never run before build_tunnel
                            if (cave[y][x].fval == GRANITE_WALL) {
                                cave[y][x].fval = TMP2_WALL;
                            }
                        }
                    }
                }
                break;
            case CORR_FLOOR:
            case BLOCKED_FLOOR:
                row1 = tmp_row;
                col1 = tmp_col;

                if (!door_flag) {
                    if (doorindex < 100) {
                        doorstk[doorindex].y = row1;
                        doorstk[doorindex].x = col1;
                        doorindex++;
                    }
                    door_flag = true;
                }

                if (randomNumber(100) > DUN_TUN_CON) {
                    // make sure that tunnel has gone a reasonable distance
                    // before stopping it, this helps prevent isolated rooms
                    tmp_row = row1 - start_row;
                    if (tmp_row < 0) {
                        tmp_row = -tmp_row;
                    }

                    tmp_col = col1 - start_col;
                    if (tmp_col < 0) {
                        tmp_col = -tmp_col;
                    }

                    if (tmp_row > 10 || tmp_col > 10) {
                        stop_flag = true;
                    }
                }
                break;
            default:
                // none of: NULL, TMP2, GRANITE, CORR
                row1 = tmp_row;
                col1 = tmp_col;
        }
    } while ((row1 != row2 || col1 != col2) && !stop_flag);

    for (int i = 0; i < tunindex; i++) {
        cave[tunstk[i].y][tunstk[i].x].fval = CORR_FLOOR;
    }

    for (int i = 0; i < wallindex; i++) {
        Cave_t *c_ptr = &cave[wallstk[i].y][wallstk[i].x];

        if (c_ptr->fval == TMP2_WALL) {
            if (randomNumber(100) < DUN_TUN_PEN) {
                place_door(wallstk[i].y, wallstk[i].x);
            } else {
                // these have to be doorways to rooms
                c_ptr->fval = CORR_FLOOR;
            }
        }
    }
}

static bool next_to(int y, int x) {
    if (coordCorridorWallsNextTo(y, x) > 2) {
        bool vertical = cave[y - 1][x].fval >= MIN_CAVE_WALL && cave[y + 1][x].fval >= MIN_CAVE_WALL;
        bool horizontal = cave[y][x - 1].fval >= MIN_CAVE_WALL && cave[y][x + 1].fval >= MIN_CAVE_WALL;

        return vertical || horizontal;
    }

    return false;
}

// Places door at y, x position if at least 2 walls found
static void try_door(int y, int x) {
    if (cave[y][x].fval == CORR_FLOOR && randomNumber(100) > DUN_TUN_JCT && next_to(y, x)) {
        place_door(y, x);
    }
}

// Returns random co-ordinates -RAK-
static void new_spot(int16_t *y, int16_t *x) {
    int yy, xx;
    Cave_t *c_ptr;

    do {
        yy = randomNumber(dungeon_height - 2);
        xx = randomNumber(dungeon_width - 2);
        c_ptr = &cave[yy][xx];
    } while (c_ptr->fval >= MIN_CLOSED_SPACE || c_ptr->cptr != 0 || c_ptr->tptr != 0);

    *y = (int16_t) yy;
    *x = (int16_t) xx;
}

// Cave logic flow for generation of new dungeon
static void cave_gen() {
    // Room initialization
    int row_rooms = 2 * (dungeon_height / SCREEN_HEIGHT);
    int col_rooms = 2 * (dungeon_width / SCREEN_WIDTH);

    bool room_map[20][20];
    for (int row = 0; row < row_rooms; row++) {
        for (int col = 0; col < col_rooms; col++) {
            room_map[row][col] = false;
        }
    }

    int randRoomCounter = randomNumberNormalDistribution(DUN_ROO_MEA, 2);
    for (int i = 0; i < randRoomCounter; i++) {
        room_map[randomNumber(row_rooms) - 1][randomNumber(col_rooms) - 1] = true;
    }

    // Build rooms
    int locationID = 0;
    int16_t yloc[400], xloc[400];

    for (int row = 0; row < row_rooms; row++) {
        for (int col = 0; col < col_rooms; col++) {
            if (room_map[row][col]) {
                yloc[locationID] = (int16_t) (row * (SCREEN_HEIGHT >> 1) + QUART_HEIGHT);
                xloc[locationID] = (int16_t) (col * (SCREEN_WIDTH >> 1) + QUART_WIDTH);
                if (current_dungeon_level > randomNumber(DUN_UNUSUAL)) {
                    int buildType = randomNumber(3);

                    if (buildType == 1) {
                        build_type1(yloc[locationID], xloc[locationID]);
                    } else if (buildType == 2) {
                        build_type2(yloc[locationID], xloc[locationID]);
                    } else {
                        build_type3(yloc[locationID], xloc[locationID]);
                    }
                } else {
                    build_room(yloc[locationID], xloc[locationID]);
                }
                locationID++;
            }
        }
    }

    for (int i = 0; i < locationID; i++) {
        int pick1 = randomNumber(locationID) - 1;
        int pick2 = randomNumber(locationID) - 1;
        int y1 = yloc[pick1];
        int x1 = xloc[pick1];
        yloc[pick1] = yloc[pick2];
        xloc[pick1] = xloc[pick2];
        yloc[pick2] = (int16_t) y1;
        xloc[pick2] = (int16_t) x1;
    }

    doorindex = 0;

    // move zero entry to locationID, so that can call build_tunnel all locationID times
    yloc[locationID] = yloc[0];
    xloc[locationID] = xloc[0];

    for (int i = 0; i < locationID; i++) {
        int y1 = yloc[i];
        int x1 = xloc[i];
        int y2 = yloc[i + 1];
        int x2 = xloc[i + 1];
        build_tunnel(y2, x2, y1, x1);
    }

    // Generate walls and streamers
    fill_cave(GRANITE_WALL);
    for (int i = 0; i < DUN_STR_MAG; i++) {
        place_streamer(MAGMA_WALL, DUN_STR_MC);
    }
    for (int i = 0; i < DUN_STR_QUA; i++) {
        place_streamer(QUARTZ_WALL, DUN_STR_QC);
    }
    place_boundary();

    // Place intersection doors
    for (int i = 0; i < doorindex; i++) {
        try_door(doorstk[i].y, doorstk[i].x - 1);
        try_door(doorstk[i].y, doorstk[i].x + 1);
        try_door(doorstk[i].y - 1, doorstk[i].x);
        try_door(doorstk[i].y + 1, doorstk[i].x);
    }

    int alloc_level = (current_dungeon_level / 3);
    if (alloc_level < 2) {
        alloc_level = 2;
    } else if (alloc_level > 10) {
        alloc_level = 10;
    }

    place_stairs(2, randomNumber(2) + 2, 3);
    place_stairs(1, randomNumber(2), 3);

    // Set up the character coords, used by monsterPlaceNewWithinDistance, monsterPlaceWinning
    new_spot(&char_row, &char_col);

    monsterPlaceNewWithinDistance((randomNumber(8) + MIN_MALLOC_LEVEL + alloc_level), 0, true);
    dungeonAllocateAndPlaceObject(set_corr, 3, randomNumber(alloc_level));
    dungeonAllocateAndPlaceObject(set_room, 5, randomNumberNormalDistribution(TREAS_ROOM_ALLOC, 3));
    dungeonAllocateAndPlaceObject(set_floor, 5, randomNumberNormalDistribution(TREAS_ANY_ALLOC, 3));
    dungeonAllocateAndPlaceObject(set_floor, 4, randomNumberNormalDistribution(TREAS_GOLD_ALLOC, 3));
    dungeonAllocateAndPlaceObject(set_floor, 1, randomNumber(alloc_level));

    if (current_dungeon_level >= WIN_MON_APPEAR) {
        monsterPlaceWinning();
    }
}

// Builds a store at a row, column coordinate
static void build_store(int store_num, int y, int x) {
    int yval = y * 10 + 5;
    int xval = x * 16 + 16;
    int y_height = yval - randomNumber(3);
    int y_depth = yval + randomNumber(4);
    int x_left = xval - randomNumber(6);
    int x_right = xval + randomNumber(6);

    int yy, xx;

    for (yy = y_height; yy <= y_depth; yy++) {
        for (xx = x_left; xx <= x_right; xx++) {
            cave[yy][xx].fval = BOUNDARY_WALL;
        }
    }

    int tmp = randomNumber(4);
    if (tmp < 3) {
        yy = randomNumber(y_depth - y_height) + y_height - 1;

        if (tmp == 1) {
            xx = x_left;
        } else {
            xx = x_right;
        }
    } else {
        xx = randomNumber(x_right - x_left) + x_left - 1;

        if (tmp == 3) {
            yy = y_depth;
        } else {
            yy = y_height;
        }
    }

    cave[yy][xx].fval = CORR_FLOOR;

    int cur_pos = popt();
    cave[yy][xx].tptr = (uint8_t) cur_pos;

    inventoryItemCopyTo(OBJ_STORE_DOOR + store_num, &treasure_list[cur_pos]);
}

// Link all free space in treasure list together
static void tlink() {
    for (int i = 0; i < MAX_TALLOC; i++) {
        inventoryItemCopyTo(OBJ_NOTHING, &treasure_list[i]);
    }
    current_treasure_id = MIN_TRIX;
}

// Link all free space in monster list together
static void mlink() {
    for (int i = 0; i < MAX_MALLOC; i++) {
        monsters[i] = blank_monster;
    }
    next_free_monster_id = MIN_MONIX;
}

static void placeTownStores() {
    int rooms[6];
    for (int i = 0; i < 6; i++) {
        rooms[i] = i;
    }

    int roomsCounter = 6;

    for (int y = 0; y < 2; y++) {
        for (int x = 0; x < 3; x++) {
            int randRoomID = randomNumber(roomsCounter) - 1;
            build_store(rooms[randRoomID], y, x);

            for (int i = randRoomID; i < roomsCounter - 1; i++) {
                rooms[i] = rooms[i + 1];
            }

            roomsCounter--;
        }
    }
}

static bool isNighTime() {
    return (0x1 & (current_game_turn / 5000)) != 0;
}

// Light town based on whether it is Night time, or day time.
static void lightTown() {
    if (isNighTime()) {
        for (int y = 0; y < dungeon_height; y++) {
            for (int x = 0; x < dungeon_width; x++) {
                if (cave[y][x].fval != DARK_FLOOR) {
                    cave[y][x].pl = true;
                }
            }
        }
        monsterPlaceNewWithinDistance(MIN_MALLOC_TN, 3, true);
    } else {
        // ...it is day time
        for (int y = 0; y < dungeon_height; y++) {
            for (int x = 0; x < dungeon_width; x++) {
                cave[y][x].pl = true;
            }
        }
        monsterPlaceNewWithinDistance(MIN_MALLOC_TD, 3, true);
    }
}

// Town logic flow for generation of new town
static void town_gen() {
    seedSet(town_seed);

    placeTownStores();

    fill_cave(DARK_FLOOR);

    // make stairs before seedResetToOldSeed, so that they don't move around
    place_boundary();
    place_stairs(2, 1, 0);

    seedResetToOldSeed();

    // Set up the character coords, used by monsterPlaceNewWithinDistance below
    new_spot(&char_row, &char_col);

    lightTown();

    store_maint();
}

// Generates a random dungeon level -RAK-
void generateCave() {
    panel_row_min = 0;
    panel_row_max = 0;
    panel_col_min = 0;
    panel_col_max = 0;

    char_row = -1;
    char_col = -1;

    tlink();
    mlink();
    blank_cave();

    // We're in the dungeon more than the town, so let's default to that -MRC-
    dungeon_height = MAX_HEIGHT;
    dungeon_width = MAX_WIDTH;

    if (current_dungeon_level == 0) {
        dungeon_height = SCREEN_HEIGHT;
        dungeon_width = SCREEN_WIDTH;
    }

    max_panel_rows = (int16_t) ((dungeon_height / SCREEN_HEIGHT) * 2 - 2);
    max_panel_cols = (int16_t) ((dungeon_width / SCREEN_WIDTH) * 2 - 2);

    panel_row = max_panel_rows;
    panel_col = max_panel_cols;

    if (current_dungeon_level == 0) {
        town_gen();
    } else {
        cave_gen();
    }
}
