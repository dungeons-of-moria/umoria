// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Initialize/create a dungeon or town level

#include "headers.h"
#include "externs.h"

static Coord_t doors_tk[100];
static int door_index;

// Returns a Dark/Light floor tile based on dg.current_dungeon_level, and random number
static uint8_t dungeonFloorTileForLevel() {
    if (dg.current_dungeon_level <= randomNumber(25)) {
        return TILE_LIGHT_FLOOR;
    }
    return TILE_DARK_FLOOR;
}

// Always picks a correct direction
static void pickCorrectDirection(int &row_dir, int &col_dir, int y1, int x1, int y2, int x2) {
    if (y1 < y2) {
        row_dir = 1;
    } else if (y1 == y2) {
        row_dir = 0;
    } else {
        row_dir = -1;
    }

    if (x1 < x2) {
        col_dir = 1;
    } else if (x1 == x2) {
        col_dir = 0;
    } else {
        col_dir = -1;
    }

    if (row_dir != 0 && col_dir != 0) {
        if (randomNumber(2) == 1) {
            row_dir = 0;
        } else {
            col_dir = 0;
        }
    }
}

// Chance of wandering direction
static void chanceOfRandomDirection(int &y, int &x) {
    int direction = randomNumber(4);

    if (direction < 3) {
        x = 0;
        y = -3 + (direction << 1); // direction=1 -> y=-1; direction=2 -> y=1
    } else {
        y = 0;
        x = -7 + (direction << 1); // direction=3 -> x=-1; direction=4 -> x=1
    }
}

// Blanks out entire cave -RAK-
static void dungeonBlankEntireCave() {
    memset((char *) &dg.cave[0][0], 0, sizeof(dg.cave));
}

// Fills in empty spots with desired rock -RAK-
// Note: 9 is a temporary value.
static void dungeonFillEmptyTilesWith(uint8_t rock_type) {
    // no need to check the border of the cave
    for (int y = dg.height - 2; y > 0; y--) {
        int x = 1;

        for (int j = dg.width - 2; j > 0; j--) {
            if (dg.cave[y][x].feature_id == TILE_NULL_WALL || dg.cave[y][x].feature_id == TMP1_WALL || dg.cave[y][x].feature_id == TMP2_WALL) {
                dg.cave[y][x].feature_id = rock_type;
            }
            x++;
        }
    }
}

#ifdef DEBUG
#include <assert.h>
#endif

// Places indestructible rock around edges of dungeon -RAK-
static void dungeonPlaceBoundaryWalls() {
    Cave_t(*left_ptr)[MAX_WIDTH];
    Cave_t(*right_ptr)[MAX_WIDTH];

    // put permanent wall on leftmost row and rightmost row
    left_ptr = (Cave_t(*)[MAX_WIDTH]) &dg.cave[0][0];
    right_ptr = (Cave_t(*)[MAX_WIDTH]) &dg.cave[0][dg.width - 1];

    for (int i = 0; i < dg.height; i++) {
#ifdef DEBUG
        assert((Cave_t *)left_ptr == &cave[i][0]);
        assert((Cave_t *)right_ptr == &cave[i][dg.width - 1]);
#endif

        ((Cave_t *) left_ptr)->feature_id = TILE_BOUNDARY_WALL;
        left_ptr++;

        ((Cave_t *) right_ptr)->feature_id = TILE_BOUNDARY_WALL;
        right_ptr++;
    }

    // put permanent wall on top row and bottom row
    Cave_t *top_ptr = &dg.cave[0][0];
    Cave_t *bottom_ptr = &dg.cave[dg.height - 1][0];

    for (int i = 0; i < dg.width; i++) {
#ifdef DEBUG
        assert(top_ptr == &cave[0][i]);
        assert(bottom_ptr == &cave[dg.height - 1][i]);
#endif
        top_ptr->feature_id = TILE_BOUNDARY_WALL;
        top_ptr++;

        bottom_ptr->feature_id = TILE_BOUNDARY_WALL;
        bottom_ptr++;
    }
}

// Places "streamers" of rock through dungeon -RAK-
static void dungeonPlaceStreamerRock(uint8_t rock_type, int chance_of_treasure) {
    // Choose starting point and direction
    int pos_y = (dg.height / 2) + 11 - randomNumber(23);
    int pos_x = (dg.width / 2) + 16 - randomNumber(33);

    // Get random direction. Numbers 1-4, 6-9
    int dir = randomNumber(8);
    if (dir > 4) {
        dir += 1;
    }

    // Place streamer into dungeon
    int t1 = 2 * DUN_STREAMER_WIDTH + 1; // Constants
    int t2 = DUN_STREAMER_WIDTH + 1;

    do {
        for (int i = 0; i < DUN_STREAMER_DENSITY; i++) {
            int y = pos_y + randomNumber(t1) - t2;
            int x = pos_x + randomNumber(t1) - t2;

            if (coordInBounds(Coord_t{y, x})) {
                if (dg.cave[y][x].feature_id == TILE_GRANITE_WALL) {
                    dg.cave[y][x].feature_id = rock_type;

                    if (randomNumber(chance_of_treasure) == 1) {
                        dungeonPlaceGold(y, x);
                    }
                }
            }
        }
    } while (playerMovePosition(dir, pos_y, pos_x));
}

static void dungeonPlaceOpenDoor(int y, int x) {
    int cur_pos = popt();
    dg.cave[y][x].treasure_id = (uint8_t) cur_pos;
    inventoryItemCopyTo(OBJ_OPEN_DOOR, treasure_list[cur_pos]);
    dg.cave[y][x].feature_id = TILE_CORR_FLOOR;
}

static void dungeonPlaceBrokenDoor(int y, int x) {
    int cur_pos = popt();
    dg.cave[y][x].treasure_id = (uint8_t) cur_pos;
    inventoryItemCopyTo(OBJ_OPEN_DOOR, treasure_list[cur_pos]);
    dg.cave[y][x].feature_id = TILE_CORR_FLOOR;
    treasure_list[cur_pos].misc_use = 1;
}

static void dungeonPlaceClosedDoor(int y, int x) {
    int cur_pos = popt();
    dg.cave[y][x].treasure_id = (uint8_t) cur_pos;
    inventoryItemCopyTo(OBJ_CLOSED_DOOR, treasure_list[cur_pos]);
    dg.cave[y][x].feature_id = TILE_BLOCKED_FLOOR;
}

static void dungeonPlaceLockedDoor(int y, int x) {
    int cur_pos = popt();
    dg.cave[y][x].treasure_id = (uint8_t) cur_pos;
    inventoryItemCopyTo(OBJ_CLOSED_DOOR, treasure_list[cur_pos]);
    dg.cave[y][x].feature_id = TILE_BLOCKED_FLOOR;
    treasure_list[cur_pos].misc_use = (int16_t) (randomNumber(10) + 10);
}

static void dungeonPlaceStuckDoor(int y, int x) {
    int cur_pos = popt();
    dg.cave[y][x].treasure_id = (uint8_t) cur_pos;
    inventoryItemCopyTo(OBJ_CLOSED_DOOR, treasure_list[cur_pos]);
    dg.cave[y][x].feature_id = TILE_BLOCKED_FLOOR;
    treasure_list[cur_pos].misc_use = (int16_t) (-randomNumber(10) - 10);
}

static void dungeonPlaceSecretDoor(int y, int x) {
    int cur_pos = popt();
    dg.cave[y][x].treasure_id = (uint8_t) cur_pos;
    inventoryItemCopyTo(OBJ_SECRET_DOOR, treasure_list[cur_pos]);
    dg.cave[y][x].feature_id = TILE_BLOCKED_FLOOR;
}

static void dungeonPlaceDoor(int y, int x) {
    int door_type = randomNumber(3);

    if (door_type == 1) {
        if (randomNumber(4) == 1) {
            dungeonPlaceBrokenDoor(y, x);
        } else {
            dungeonPlaceOpenDoor(y, x);
        }
    } else if (door_type == 2) {
        door_type = randomNumber(12);

        if (door_type > 3) {
            dungeonPlaceClosedDoor(y, x);
        } else if (door_type == 3) {
            dungeonPlaceStuckDoor(y, x);
        } else {
            dungeonPlaceLockedDoor(y, x);
        }
    } else {
        dungeonPlaceSecretDoor(y, x);
    }
}

// Place an up staircase at given y, x -RAK-
static void dungeonPlaceUpStairs(int y, int x) {
    if (dg.cave[y][x].treasure_id != 0) {
        (void) dungeonDeleteObject(y, x);
    }

    int cur_pos = popt();
    dg.cave[y][x].treasure_id = (uint8_t) cur_pos;
    inventoryItemCopyTo(OBJ_UP_STAIR, treasure_list[cur_pos]);
}

// Place a down staircase at given y, x -RAK-
static void dungeonPlaceDownStairs(int y, int x) {
    if (dg.cave[y][x].treasure_id != 0) {
        (void) dungeonDeleteObject(y, x);
    }

    int cur_pos = popt();
    dg.cave[y][x].treasure_id = (uint8_t) cur_pos;
    inventoryItemCopyTo(OBJ_DOWN_STAIR, treasure_list[cur_pos]);
}

// Places a staircase 1=up, 2=down -RAK-
static void dungeonPlaceStairs(int stair_type, int number, int walls) {
    for (int i = 0; i < number; i++) {
        bool placed = false;

        while (!placed) {
            int j = 0;

            do {
                // Note:
                // don't let y1/x1 be zero,
                // don't let y2/x2 be equal to dg.height-1/dg.width-1,
                // these values are always BOUNDARY_ROCK.
                int y1 = randomNumber(dg.height - 14);
                int x1 = randomNumber(dg.width - 14);
                int y2 = y1 + 12;
                int x2 = x1 + 12;

                do {
                    do {
                        if (dg.cave[y1][x1].feature_id <= MAX_OPEN_SPACE && dg.cave[y1][x1].treasure_id == 0 && coordWallsNextTo(Coord_t{y1, x1}) >= walls) {
                            placed = true;
                            if (stair_type == 1) {
                                dungeonPlaceUpStairs(y1, x1);
                            } else {
                                dungeonPlaceDownStairs(y1, x1);
                            }
                        }
                        x1++;
                    } while ((x1 != x2) && (!placed));

                    x1 = x2 - 12;
                    y1++;
                } while ((y1 != y2) && (!placed));

                j++;
            } while ((!placed) && (j <= 30));

            walls--;
        }
    }
}

// Place a trap with a given displacement of point -RAK-
static void dungeonPlaceVaultTrap(int y, int x, int yd, int xd, int number) {
    for (int i = 0; i < number; i++) {
        bool placed = false;

        for (int count = 0; !placed && count <= 5; count++) {
            int y1 = y - yd - 1 + randomNumber(2 * yd + 1);
            int x1 = x - xd - 1 + randomNumber(2 * xd + 1);

            if (dg.cave[y1][x1].feature_id != TILE_NULL_WALL && dg.cave[y1][x1].feature_id <= MAX_CAVE_FLOOR && dg.cave[y1][x1].treasure_id == 0) {
                dungeonSetTrap(y1, x1, randomNumber(MAX_TRAPS) - 1);
                placed = true;
            }
        }
    }
}

// Place a trap with a given displacement of point -RAK-
static void dungeonPlaceVaultMonster(int y, int x, int number) {
    int pos_y, pos_x;

    for (int i = 0; i < number; i++) {
        pos_y = y;
        pos_x = x;
        (void) monsterSummon(pos_y, pos_x, true);
    }
}

// Builds a room at a row, column coordinate -RAK-
static void dungeonBuildRoom(int y, int x) {
    uint8_t floor = dungeonFloorTileForLevel();

    int height = y - randomNumber(4);
    int depth = y + randomNumber(3);
    int left = x - randomNumber(11);
    int right = x + randomNumber(11);

    // the x dim of rooms tends to be much larger than the y dim,
    // so don't bother rewriting the y loop.

    for (int i = height; i <= depth; i++) {
        for (int j = left; j <= right; j++) {
            dg.cave[i][j].feature_id = floor;
            dg.cave[i][j].perma_lit_room = true;
        }
    }

    for (int i = height - 1; i <= depth + 1; i++) {
        dg.cave[i][left - 1].feature_id = TILE_GRANITE_WALL;
        dg.cave[i][left - 1].perma_lit_room = true;

        dg.cave[i][right + 1].feature_id = TILE_GRANITE_WALL;
        dg.cave[i][right + 1].perma_lit_room = true;
    }

    for (int i = left; i <= right; i++) {
        dg.cave[height - 1][i].feature_id = TILE_GRANITE_WALL;
        dg.cave[height - 1][i].perma_lit_room = true;

        dg.cave[depth + 1][i].feature_id = TILE_GRANITE_WALL;
        dg.cave[depth + 1][i].perma_lit_room = true;
    }
}

// Builds a room at a row, column coordinate -RAK-
// Type 1 unusual rooms are several overlapping rectangular ones
static void dungeonBuildRoomOverlappingRectangles(int y, int x) {
    uint8_t floor = dungeonFloorTileForLevel();

    int limit = 1 + randomNumber(2);

    for (int count = 0; count < limit; count++) {
        int height = y - randomNumber(4);
        int depth = y + randomNumber(3);
        int left = x - randomNumber(11);
        int right = x + randomNumber(11);

        // the x dim of rooms tends to be much larger than the y dim,
        // so don't bother rewriting the y loop.

        for (int i = height; i <= depth; i++) {
            for (int j = left; j <= right; j++) {
                dg.cave[i][j].feature_id = floor;
                dg.cave[i][j].perma_lit_room = true;
            }
        }
        for (int i = (height - 1); i <= (depth + 1); i++) {
            if (dg.cave[i][left - 1].feature_id != floor) {
                dg.cave[i][left - 1].feature_id = TILE_GRANITE_WALL;
                dg.cave[i][left - 1].perma_lit_room = true;
            }

            if (dg.cave[i][right + 1].feature_id != floor) {
                dg.cave[i][right + 1].feature_id = TILE_GRANITE_WALL;
                dg.cave[i][right + 1].perma_lit_room = true;
            }
        }

        for (int i = left; i <= right; i++) {
            if (dg.cave[height - 1][i].feature_id != floor) {
                dg.cave[height - 1][i].feature_id = TILE_GRANITE_WALL;
                dg.cave[height - 1][i].perma_lit_room = true;
            }

            if (dg.cave[depth + 1][i].feature_id != floor) {
                dg.cave[depth + 1][i].feature_id = TILE_GRANITE_WALL;
                dg.cave[depth + 1][i].perma_lit_room = true;
            }
        }
    }
}

static void dungeonPlaceRandomSecretDoor(int y, int x, int depth, int height, int left, int right) {
    switch (randomNumber(4)) {
        case 1:
            dungeonPlaceSecretDoor(height - 1, x);
            break;
        case 2:
            dungeonPlaceSecretDoor(depth + 1, x);
            break;
        case 3:
            dungeonPlaceSecretDoor(y, left - 1);
            break;
        default:
            dungeonPlaceSecretDoor(y, right + 1);
            break;
    }
}

static void dungeonPlaceVault(int y, int x) {
    for (int i = y - 1; i <= y + 1; i++) {
        dg.cave[i][x - 1].feature_id = TMP1_WALL;
        dg.cave[i][x + 1].feature_id = TMP1_WALL;
    }

    dg.cave[y - 1][x].feature_id = TMP1_WALL;
    dg.cave[y + 1][x].feature_id = TMP1_WALL;
}

static void dungeonPlaceTreasureVault(int y, int x, int depth, int height, int left, int right) {
    dungeonPlaceRandomSecretDoor(y, x, depth, height, left, right);
    dungeonPlaceVault(y, x);

    // Place a locked door
    int offset = randomNumber(4);
    if (offset < 3) {
        // 1 -> y-1; 2 -> y+1
        dungeonPlaceLockedDoor(y - 3 + (offset << 1), x);
    } else {
        dungeonPlaceLockedDoor(y, x - 7 + (offset << 1));
    }
}

static void dungeonPlaceInnerPillars(int y, int x) {
    for (int i = y - 1; i <= y + 1; i++) {
        for (int j = x - 1; j <= x + 1; j++) {
            dg.cave[i][j].feature_id = TMP1_WALL;
        }
    }

    if (randomNumber(2) != 1) {
        return;
    }

    int offset = randomNumber(2);

    for (int i = y - 1; i <= y + 1; i++) {
        for (int j = x - 5 - offset; j <= x - 3 - offset; j++) {
            dg.cave[i][j].feature_id = TMP1_WALL;
        }
    }

    for (int i = y - 1; i <= y + 1; i++) {
        for (int j = x + 3 + offset; j <= x + 5 + offset; j++) {
            dg.cave[i][j].feature_id = TMP1_WALL;
        }
    }
}

static void dungeonPlaceMazeInsideRoom(int depth, int height, int left, int right) {
    for (int y = height; y <= depth; y++) {
        for (int x = left; x <= right; x++) {
            if ((0x1 & (x + y)) != 0) {
                dg.cave[y][x].feature_id = TMP1_WALL;
            }
        }
    }
}

static void dungeonPlaceFourSmallRooms(int y, int x, int depth, int height, int left, int right) {
    for (int i = height; i <= depth; i++) {
        dg.cave[i][x].feature_id = TMP1_WALL;
    }

    for (int i = left; i <= right; i++) {
        dg.cave[y][i].feature_id = TMP1_WALL;
    }

    // place random secret door
    if (randomNumber(2) == 1) {
        int offsetX = randomNumber(10);
        dungeonPlaceSecretDoor(height - 1, x - offsetX);
        dungeonPlaceSecretDoor(height - 1, x + offsetX);
        dungeonPlaceSecretDoor(depth + 1, x - offsetX);
        dungeonPlaceSecretDoor(depth + 1, x + offsetX);
    } else {
        int offsetY = randomNumber(3);
        dungeonPlaceSecretDoor(y + offsetY, left - 1);
        dungeonPlaceSecretDoor(y - offsetY, left - 1);
        dungeonPlaceSecretDoor(y + offsetY, right + 1);
        dungeonPlaceSecretDoor(y - offsetY, right + 1);
    }
}

// Type 2 unusual rooms all have an inner room:
//   1 - Just an inner room with one door
//   2 - An inner room within an inner room
//   3 - An inner room with pillar(s)
//   4 - Inner room has a maze
//   5 - A set of four inner rooms
enum class InnerRoomTypes {
    plain = 1,
    treasure_vault,
    pillars,
    maze,
    four_small_rooms,
};

// Builds a type 2 unusual room at a row, column coordinate -RAK-
static void dungeonBuildRoomWithInnerRooms(int y, int x) {
    uint8_t floor = dungeonFloorTileForLevel();

    int height = y - 4;
    int depth = y + 4;
    int left = x - 11;
    int right = x + 11;

    // the x dim of rooms tends to be much larger than the y dim,
    // so don't bother rewriting the y loop.

    for (int i = height; i <= depth; i++) {
        for (int j = left; j <= right; j++) {
            dg.cave[i][j].feature_id = floor;
            dg.cave[i][j].perma_lit_room = true;
        }
    }

    for (int i = (height - 1); i <= (depth + 1); i++) {
        dg.cave[i][left - 1].feature_id = TILE_GRANITE_WALL;
        dg.cave[i][left - 1].perma_lit_room = true;

        dg.cave[i][right + 1].feature_id = TILE_GRANITE_WALL;
        dg.cave[i][right + 1].perma_lit_room = true;
    }

    for (int i = left; i <= right; i++) {
        dg.cave[height - 1][i].feature_id = TILE_GRANITE_WALL;
        dg.cave[height - 1][i].perma_lit_room = true;

        dg.cave[depth + 1][i].feature_id = TILE_GRANITE_WALL;
        dg.cave[depth + 1][i].perma_lit_room = true;
    }

    // The inner room
    height = height + 2;
    depth = depth - 2;
    left = left + 2;
    right = right - 2;

    for (int i = (height - 1); i <= (depth + 1); i++) {
        dg.cave[i][left - 1].feature_id = TMP1_WALL;
        dg.cave[i][right + 1].feature_id = TMP1_WALL;
    }

    for (int i = left; i <= right; i++) {
        dg.cave[height - 1][i].feature_id = TMP1_WALL;
        dg.cave[depth + 1][i].feature_id = TMP1_WALL;
    }

    // Inner room variations
    switch ((InnerRoomTypes) randomNumber(5)) {
        case InnerRoomTypes::plain:
            dungeonPlaceRandomSecretDoor(y, x, depth, height, left, right);
            dungeonPlaceVaultMonster(y, x, 1);
            break;
        case InnerRoomTypes::treasure_vault:
            dungeonPlaceTreasureVault(y, x, depth, height, left, right);

            // Guard the treasure well
            dungeonPlaceVaultMonster(y, x, 2 + randomNumber(3));

            // If the monsters don't get 'em.
            dungeonPlaceVaultTrap(y, x, 4, 10, 2 + randomNumber(3));
            break;
        case InnerRoomTypes::pillars:
            dungeonPlaceRandomSecretDoor(y, x, depth, height, left, right);

            dungeonPlaceInnerPillars(y, x);

            if (randomNumber(3) != 1) {
                break;
            }

            // Inner rooms
            for (int i = x - 5; i <= x + 5; i++) {
                dg.cave[y - 1][i].feature_id = TMP1_WALL;
                dg.cave[y + 1][i].feature_id = TMP1_WALL;
            }
            dg.cave[y][x - 5].feature_id = TMP1_WALL;
            dg.cave[y][x + 5].feature_id = TMP1_WALL;

            dungeonPlaceSecretDoor(y - 3 + (randomNumber(2) << 1), x - 3);
            dungeonPlaceSecretDoor(y - 3 + (randomNumber(2) << 1), x + 3);

            if (randomNumber(3) == 1) {
                dungeonPlaceRandomObjectAt(y, x - 2, false);
            }

            if (randomNumber(3) == 1) {
                dungeonPlaceRandomObjectAt(y, x + 2, false);
            }

            dungeonPlaceVaultMonster(y, x - 2, randomNumber(2));
            dungeonPlaceVaultMonster(y, x + 2, randomNumber(2));
            break;
        case InnerRoomTypes::maze:
            dungeonPlaceRandomSecretDoor(y, x, depth, height, left, right);

            dungeonPlaceMazeInsideRoom(depth, height, left, right);

            // Monsters just love mazes.
            dungeonPlaceVaultMonster(y, x - 5, randomNumber(3));
            dungeonPlaceVaultMonster(y, x + 5, randomNumber(3));

            // Traps make them entertaining.
            dungeonPlaceVaultTrap(y, x - 3, 2, 8, randomNumber(3));
            dungeonPlaceVaultTrap(y, x + 3, 2, 8, randomNumber(3));

            // Mazes should have some treasure too..
            for (int i = 0; i < 3; i++) {
                dungeonPlaceRandomObjectNear(y, x, 1);
            }
            break;
        case InnerRoomTypes::four_small_rooms:
            dungeonPlaceFourSmallRooms(y, x, depth, height, left, right);

            // Treasure in each one.
            dungeonPlaceRandomObjectNear(y, x, 2 + randomNumber(2));

            // Gotta have some monsters.
            dungeonPlaceVaultMonster(y + 2, x - 4, randomNumber(2));
            dungeonPlaceVaultMonster(y + 2, x + 4, randomNumber(2));
            dungeonPlaceVaultMonster(y - 2, x - 4, randomNumber(2));
            dungeonPlaceVaultMonster(y - 2, x + 4, randomNumber(2));
            break;
        default:
            break;
    }
}

static void dungeonPlaceLargeMiddlePillar(int y, int x) {
    for (int i = y - 1; i <= y + 1; i++) {
        for (int j = x - 1; j <= x + 1; j++) {
            dg.cave[i][j].feature_id = TMP1_WALL;
        }
    }
}

// Builds a room at a row, column coordinate -RAK-
// Type 3 unusual rooms are cross shaped
static void dungeonBuildRoomCrossShaped(int y, int x) {
    uint8_t floor = dungeonFloorTileForLevel();

    int random_offset = 2 + randomNumber(2);

    int height = y - random_offset;
    int depth = y + random_offset;
    int left = x - 1;
    int right = x + 1;

    for (int i = height; i <= depth; i++) {
        for (int j = left; j <= right; j++) {
            dg.cave[i][j].feature_id = floor;
            dg.cave[i][j].perma_lit_room = true;
        }
    }

    for (int i = height - 1; i <= depth + 1; i++) {
        dg.cave[i][left - 1].feature_id = TILE_GRANITE_WALL;
        dg.cave[i][left - 1].perma_lit_room = true;

        dg.cave[i][right + 1].feature_id = TILE_GRANITE_WALL;
        dg.cave[i][right + 1].perma_lit_room = true;
    }

    for (int i = left; i <= right; i++) {
        dg.cave[height - 1][i].feature_id = TILE_GRANITE_WALL;
        dg.cave[height - 1][i].perma_lit_room = true;

        dg.cave[depth + 1][i].feature_id = TILE_GRANITE_WALL;
        dg.cave[depth + 1][i].perma_lit_room = true;
    }

    random_offset = 2 + randomNumber(9);

    height = y - 1;
    depth = y + 1;
    left = x - random_offset;
    right = x + random_offset;

    for (int i = height; i <= depth; i++) {
        for (int j = left; j <= right; j++) {
            dg.cave[i][j].feature_id = floor;
            dg.cave[i][j].perma_lit_room = true;
        }
    }

    for (int i = height - 1; i <= depth + 1; i++) {
        if (dg.cave[i][left - 1].feature_id != floor) {
            dg.cave[i][left - 1].feature_id = TILE_GRANITE_WALL;
            dg.cave[i][left - 1].perma_lit_room = true;
        }

        if (dg.cave[i][right + 1].feature_id != floor) {
            dg.cave[i][right + 1].feature_id = TILE_GRANITE_WALL;
            dg.cave[i][right + 1].perma_lit_room = true;
        }
    }

    for (int i = left; i <= right; i++) {
        if (dg.cave[height - 1][i].feature_id != floor) {
            dg.cave[height - 1][i].feature_id = TILE_GRANITE_WALL;
            dg.cave[height - 1][i].perma_lit_room = true;
        }

        if (dg.cave[depth + 1][i].feature_id != floor) {
            dg.cave[depth + 1][i].feature_id = TILE_GRANITE_WALL;
            dg.cave[depth + 1][i].perma_lit_room = true;
        }
    }

    // Special features.
    switch (randomNumber(4)) {
        case 1: // Large middle pillar
            dungeonPlaceLargeMiddlePillar(y, x);
            break;
        case 2: // Inner treasure vault
            dungeonPlaceVault(y, x);

            // Place a secret door
            random_offset = randomNumber(4);
            if (random_offset < 3) {
                dungeonPlaceSecretDoor(y - 3 + (random_offset << 1), x);
            } else {
                dungeonPlaceSecretDoor(y, x - 7 + (random_offset << 1));
            }

            // Place a treasure in the vault
            dungeonPlaceRandomObjectAt(y, x, false);

            // Let's guard the treasure well.
            dungeonPlaceVaultMonster(y, x, 2 + randomNumber(2));

            // Traps naturally
            dungeonPlaceVaultTrap(y, x, 4, 4, 1 + randomNumber(3));
            break;
        case 3:
            if (randomNumber(3) == 1) {
                dg.cave[y - 1][x - 2].feature_id = TMP1_WALL;
                dg.cave[y + 1][x - 2].feature_id = TMP1_WALL;
                dg.cave[y - 1][x + 2].feature_id = TMP1_WALL;
                dg.cave[y + 1][x + 2].feature_id = TMP1_WALL;
                dg.cave[y - 2][x - 1].feature_id = TMP1_WALL;
                dg.cave[y - 2][x + 1].feature_id = TMP1_WALL;
                dg.cave[y + 2][x - 1].feature_id = TMP1_WALL;
                dg.cave[y + 2][x + 1].feature_id = TMP1_WALL;
                if (randomNumber(3) == 1) {
                    dungeonPlaceSecretDoor(y, x - 2);
                    dungeonPlaceSecretDoor(y, x + 2);
                    dungeonPlaceSecretDoor(y - 2, x);
                    dungeonPlaceSecretDoor(y + 2, x);
                }
            } else if (randomNumber(3) == 1) {
                dg.cave[y][x].feature_id = TMP1_WALL;
                dg.cave[y - 1][x].feature_id = TMP1_WALL;
                dg.cave[y + 1][x].feature_id = TMP1_WALL;
                dg.cave[y][x - 1].feature_id = TMP1_WALL;
                dg.cave[y][x + 1].feature_id = TMP1_WALL;
            } else if (randomNumber(3) == 1) {
                dg.cave[y][x].feature_id = TMP1_WALL;
            }
            break;
        case 4:
            // no special feature!
            break;
        default:
            break;
    }
}

// Constructs a tunnel between two points
static void dungeonBuildTunnel(int y_start, int x_start, int y_end, int x_end) {
    Coord_t tunnels_tk[1000], walls_tk[1000];

    // Main procedure for Tunnel
    // Note: 9 is a temporary value
    bool door_flag = false;
    bool stop_flag = false;
    int main_loop_count = 0;
    int start_row = y_start;
    int start_col = x_start;
    int tunnel_index = 0;
    int wall_index = 0;

    int row_dir, col_dir;
    pickCorrectDirection(row_dir, col_dir, y_start, x_start, y_end, x_end);

    do {
        // prevent infinite loops, just in case
        main_loop_count++;
        if (main_loop_count > 2000) {
            stop_flag = true;
        }

        if (randomNumber(100) > DUN_DIR_CHANGE) {
            if (randomNumber(DUN_RANDOM_DIR) == 1) {
                chanceOfRandomDirection(row_dir, col_dir);
            } else {
                pickCorrectDirection(row_dir, col_dir, y_start, x_start, y_end, x_end);
            }
        }

        int tmp_row = y_start + row_dir;
        int tmp_col = x_start + col_dir;

        while (!coordInBounds(Coord_t{tmp_row, tmp_col})) {
            if (randomNumber(DUN_RANDOM_DIR) == 1) {
                chanceOfRandomDirection(row_dir, col_dir);
            } else {
                pickCorrectDirection(row_dir, col_dir, y_start, x_start, y_end, x_end);
            }
            tmp_row = y_start + row_dir;
            tmp_col = x_start + col_dir;
        }

        switch (dg.cave[tmp_row][tmp_col].feature_id) {
            case TILE_NULL_WALL:
                y_start = tmp_row;
                x_start = tmp_col;
                if (tunnel_index < 1000) {
                    tunnels_tk[tunnel_index].y = y_start;
                    tunnels_tk[tunnel_index].x = x_start;
                    tunnel_index++;
                }
                door_flag = false;
                break;
            case TMP2_WALL:
                // do nothing
                break;
            case TILE_GRANITE_WALL:
                y_start = tmp_row;
                x_start = tmp_col;

                if (wall_index < 1000) {
                    walls_tk[wall_index].y = y_start;
                    walls_tk[wall_index].x = x_start;
                    wall_index++;
                }

                for (int y = y_start - 1; y <= y_start + 1; y++) {
                    for (int x = x_start - 1; x <= x_start + 1; x++) {
                        if (coordInBounds(Coord_t{y, x})) {
                            // values 11 and 12 are impossible here, dungeonPlaceStreamerRock
                            // is never run before dungeonBuildTunnel
                            if (dg.cave[y][x].feature_id == TILE_GRANITE_WALL) {
                                dg.cave[y][x].feature_id = TMP2_WALL;
                            }
                        }
                    }
                }
                break;
            case TILE_CORR_FLOOR:
            case TILE_BLOCKED_FLOOR:
                y_start = tmp_row;
                x_start = tmp_col;

                if (!door_flag) {
                    if (door_index < 100) {
                        doors_tk[door_index].y = y_start;
                        doors_tk[door_index].x = x_start;
                        door_index++;
                    }
                    door_flag = true;
                }

                if (randomNumber(100) > DUN_TUNNELING) {
                    // make sure that tunnel has gone a reasonable distance
                    // before stopping it, this helps prevent isolated rooms
                    tmp_row = y_start - start_row;
                    if (tmp_row < 0) {
                        tmp_row = -tmp_row;
                    }

                    tmp_col = x_start - start_col;
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
                y_start = tmp_row;
                x_start = tmp_col;
        }
    } while ((y_start != y_end || x_start != x_end) && !stop_flag);

    for (int i = 0; i < tunnel_index; i++) {
        dg.cave[tunnels_tk[i].y][tunnels_tk[i].x].feature_id = TILE_CORR_FLOOR;
    }

    for (int i = 0; i < wall_index; i++) {
        Cave_t &tile = dg.cave[walls_tk[i].y][walls_tk[i].x];

        if (tile.feature_id == TMP2_WALL) {
            if (randomNumber(100) < DUN_ROOM_DOORS) {
                dungeonPlaceDoor(walls_tk[i].y, walls_tk[i].x);
            } else {
                // these have to be doorways to rooms
                tile.feature_id = TILE_CORR_FLOOR;
            }
        }
    }
}

static bool dungeonIsNextTo(int y, int x) {
    if (coordCorridorWallsNextTo(Coord_t{y, x}) > 2) {
        bool vertical = dg.cave[y - 1][x].feature_id >= MIN_CAVE_WALL && dg.cave[y + 1][x].feature_id >= MIN_CAVE_WALL;
        bool horizontal = dg.cave[y][x - 1].feature_id >= MIN_CAVE_WALL && dg.cave[y][x + 1].feature_id >= MIN_CAVE_WALL;

        return vertical || horizontal;
    }

    return false;
}

// Places door at y, x position if at least 2 walls found
static void dungeonPlaceDoorIfNextToTwoWalls(int y, int x) {
    if (dg.cave[y][x].feature_id == TILE_CORR_FLOOR && randomNumber(100) > DUN_TUNNEL_DOORS && dungeonIsNextTo(y, x)) {
        dungeonPlaceDoor(y, x);
    }
}

// Returns random co-ordinates -RAK-
static void dungeonNewSpot(int16_t &y, int16_t &x) {
    int pos_y, pos_x;
    Cave_t *tile;

    do {
        pos_y = randomNumber(dg.height - 2);
        pos_x = randomNumber(dg.width - 2);
        tile = &dg.cave[pos_y][pos_x];
    } while (tile->feature_id >= MIN_CLOSED_SPACE || tile->creature_id != 0 || tile->treasure_id != 0);

    y = (int16_t) pos_y;
    x = (int16_t) pos_x;
}

// Cave logic flow for generation of new dungeon
static void dungeonGenerate() {
    // Room initialization
    int row_rooms = 2 * (dg.height / SCREEN_HEIGHT);
    int col_rooms = 2 * (dg.width / SCREEN_WIDTH);

    bool room_map[20][20];
    for (int row = 0; row < row_rooms; row++) {
        for (int col = 0; col < col_rooms; col++) {
            room_map[row][col] = false;
        }
    }

    int random_room_count = randomNumberNormalDistribution(DUN_ROOMS_MEAN, 2);
    for (int i = 0; i < random_room_count; i++) {
        room_map[randomNumber(row_rooms) - 1][randomNumber(col_rooms) - 1] = true;
    }

    // Build rooms
    int location_id = 0;
    int16_t y_locations[400], x_locations[400];

    for (int row = 0; row < row_rooms; row++) {
        for (int col = 0; col < col_rooms; col++) {
            if (room_map[row][col]) {
                y_locations[location_id] = (int16_t) (row * (SCREEN_HEIGHT >> 1) + QUART_HEIGHT);
                x_locations[location_id] = (int16_t) (col * (SCREEN_WIDTH >> 1) + QUART_WIDTH);
                if (dg.current_dungeon_level > randomNumber(DUN_UNUSUAL_ROOMS)) {
                    int room_type = randomNumber(3);

                    if (room_type == 1) {
                        dungeonBuildRoomOverlappingRectangles(y_locations[location_id], x_locations[location_id]);
                    } else if (room_type == 2) {
                        dungeonBuildRoomWithInnerRooms(y_locations[location_id], x_locations[location_id]);
                    } else {
                        dungeonBuildRoomCrossShaped(y_locations[location_id], x_locations[location_id]);
                    }
                } else {
                    dungeonBuildRoom(y_locations[location_id], x_locations[location_id]);
                }
                location_id++;
            }
        }
    }

    for (int i = 0; i < location_id; i++) {
        int pick1 = randomNumber(location_id) - 1;
        int pick2 = randomNumber(location_id) - 1;
        int y1 = y_locations[pick1];
        int x1 = x_locations[pick1];
        y_locations[pick1] = y_locations[pick2];
        x_locations[pick1] = x_locations[pick2];
        y_locations[pick2] = (int16_t) y1;
        x_locations[pick2] = (int16_t) x1;
    }

    door_index = 0;

    // move zero entry to location_id, so that can call dungeonBuildTunnel all location_id times
    y_locations[location_id] = y_locations[0];
    x_locations[location_id] = x_locations[0];

    for (int i = 0; i < location_id; i++) {
        int y1 = y_locations[i];
        int x1 = x_locations[i];
        int y2 = y_locations[i + 1];
        int x2 = x_locations[i + 1];
        dungeonBuildTunnel(y2, x2, y1, x1);
    }

    // Generate walls and streamers
    dungeonFillEmptyTilesWith(TILE_GRANITE_WALL);
    for (int i = 0; i < DUN_MAGMA_STREAMER; i++) {
        dungeonPlaceStreamerRock(TILE_MAGMA_WALL, DUN_MAGMA_TREASURE);
    }
    for (int i = 0; i < DUN_QUARTZ_STREAMER; i++) {
        dungeonPlaceStreamerRock(TILE_QUARTZ_WALL, DUN_QUARTZ_TREASURE);
    }
    dungeonPlaceBoundaryWalls();

    // Place intersection doors
    for (int i = 0; i < door_index; i++) {
        dungeonPlaceDoorIfNextToTwoWalls(doors_tk[i].y, doors_tk[i].x - 1);
        dungeonPlaceDoorIfNextToTwoWalls(doors_tk[i].y, doors_tk[i].x + 1);
        dungeonPlaceDoorIfNextToTwoWalls(doors_tk[i].y - 1, doors_tk[i].x);
        dungeonPlaceDoorIfNextToTwoWalls(doors_tk[i].y + 1, doors_tk[i].x);
    }

    int alloc_level = (dg.current_dungeon_level / 3);
    if (alloc_level < 2) {
        alloc_level = 2;
    } else if (alloc_level > 10) {
        alloc_level = 10;
    }

    dungeonPlaceStairs(2, randomNumber(2) + 2, 3);
    dungeonPlaceStairs(1, randomNumber(2), 3);

    // Set up the character coords, used by monsterPlaceNewWithinDistance, monsterPlaceWinning
    dungeonNewSpot(char_row, char_col);

    monsterPlaceNewWithinDistance((randomNumber(8) + MON_MIN_PER_LEVEL + alloc_level), 0, true);
    dungeonAllocateAndPlaceObject(setCorridors, 3, randomNumber(alloc_level));
    dungeonAllocateAndPlaceObject(setRooms, 5, randomNumberNormalDistribution(LEVEL_OBJECTS_PER_ROOM, 3));
    dungeonAllocateAndPlaceObject(setFloors, 5, randomNumberNormalDistribution(LEVEL_OBJECTS_PER_CORRIDOR, 3));
    dungeonAllocateAndPlaceObject(setFloors, 4, randomNumberNormalDistribution(LEVEL_TOTAL_GOLD_AND_GEMS, 3));
    dungeonAllocateAndPlaceObject(setFloors, 1, randomNumber(alloc_level));

    if (dg.current_dungeon_level >= MON_ENDGAME_LEVEL) {
        monsterPlaceWinning();
    }
}

// Builds a store at a row, column coordinate
static void dungeonBuildStore(int store_id, int y, int x) {
    int yval = y * 10 + 5;
    int xval = x * 16 + 16;
    int y_height = yval - randomNumber(3);
    int y_depth = yval + randomNumber(4);
    int x_left = xval - randomNumber(6);
    int x_right = xval + randomNumber(6);

    int pos_y, pos_x;

    for (pos_y = y_height; pos_y <= y_depth; pos_y++) {
        for (pos_x = x_left; pos_x <= x_right; pos_x++) {
            dg.cave[pos_y][pos_x].feature_id = TILE_BOUNDARY_WALL;
        }
    }

    int tmp = randomNumber(4);
    if (tmp < 3) {
        pos_y = randomNumber(y_depth - y_height) + y_height - 1;

        if (tmp == 1) {
            pos_x = x_left;
        } else {
            pos_x = x_right;
        }
    } else {
        pos_x = randomNumber(x_right - x_left) + x_left - 1;

        if (tmp == 3) {
            pos_y = y_depth;
        } else {
            pos_y = y_height;
        }
    }

    dg.cave[pos_y][pos_x].feature_id = TILE_CORR_FLOOR;

    int cur_pos = popt();
    dg.cave[pos_y][pos_x].treasure_id = (uint8_t) cur_pos;

    inventoryItemCopyTo(OBJ_STORE_DOOR + store_id, treasure_list[cur_pos]);
}

// Link all free space in treasure list together
static void treasureLinker() {
    for (auto &item : treasure_list) {
        inventoryItemCopyTo(OBJ_NOTHING, item);
    }
    current_treasure_id = MIN_TREASURE_LIST_ID;
}

// Link all free space in monster list together
static void monsterLinker() {
    for (auto &monster : monsters) {
        monster = blank_monster;
    }
    next_free_monster_id = MON_MIN_INDEX_ID;
}

static void dungeonPlaceTownStores() {
    int rooms[6];
    for (int i = 0; i < 6; i++) {
        rooms[i] = i;
    }

    int rooms_count = 6;

    for (int y = 0; y < 2; y++) {
        for (int x = 0; x < 3; x++) {
            int room_id = randomNumber(rooms_count) - 1;
            dungeonBuildStore(rooms[room_id], y, x);

            for (int i = room_id; i < rooms_count - 1; i++) {
                rooms[i] = rooms[i + 1];
            }

            rooms_count--;
        }
    }
}

static bool isNighTime() {
    return (0x1 & (dg.current_game_turn / 5000)) != 0;
}

// Light town based on whether it is Night time, or day time.
static void lightTown() {
    if (isNighTime()) {
        for (int y = 0; y < dg.height; y++) {
            for (int x = 0; x < dg.width; x++) {
                if (dg.cave[y][x].feature_id != TILE_DARK_FLOOR) {
                    dg.cave[y][x].permanent_light = true;
                }
            }
        }
        monsterPlaceNewWithinDistance(MON_MIN_TOWNSFOLK_NIGHT, 3, true);
    } else {
        // ...it is day time
        for (int y = 0; y < dg.height; y++) {
            for (int x = 0; x < dg.width; x++) {
                dg.cave[y][x].permanent_light = true;
            }
        }
        monsterPlaceNewWithinDistance(MON_MIN_TOWNSFOLK_DAY, 3, true);
    }
}

// Town logic flow for generation of new town
static void townGeneration() {
    seedSet(town_seed);

    dungeonPlaceTownStores();

    dungeonFillEmptyTilesWith(TILE_DARK_FLOOR);

    // make stairs before seedResetToOldSeed, so that they don't move around
    dungeonPlaceBoundaryWalls();
    dungeonPlaceStairs(2, 1, 0);

    seedResetToOldSeed();

    // Set up the character coords, used by monsterPlaceNewWithinDistance below
    dungeonNewSpot(char_row, char_col);

    lightTown();

    storeMaintenance();
}

// Generates a random dungeon level -RAK-
void generateCave() {
    dg.panel_row_min = 0;
    dg.panel_row_max = 0;
    dg.panel_col_min = 0;
    dg.panel_col_max = 0;

    char_row = -1;
    char_col = -1;

    treasureLinker();
    monsterLinker();
    dungeonBlankEntireCave();

    // We're in the dungeon more than the town, so let's default to that -MRC-
    dg.height = MAX_HEIGHT;
    dg.width = MAX_WIDTH;

    if (dg.current_dungeon_level == 0) {
        dg.height = SCREEN_HEIGHT;
        dg.width = SCREEN_WIDTH;
    }

    dg.max_panel_rows = (int16_t) ((dg.height / SCREEN_HEIGHT) * 2 - 2);
    dg.max_panel_cols = (int16_t) ((dg.width / SCREEN_WIDTH) * 2 - 2);

    dg.panel_row = dg.max_panel_rows;
    dg.panel_col = dg.max_panel_cols;

    if (dg.current_dungeon_level == 0) {
        townGeneration();
    } else {
        dungeonGenerate();
    }
}
