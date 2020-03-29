// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// Initialize/create a dungeon or town level

#include "headers.h"

static Coord_t doors_tk[100];
static int door_index;

// Returns a Dark/Light floor tile based on dg.current_level, and random number
static uint8_t dungeonFloorTileForLevel() {
    if (dg.current_level <= randomNumber(25)) {
        return TILE_LIGHT_FLOOR;
    }
    return TILE_DARK_FLOOR;
}

// Always picks a correct direction
static void pickCorrectDirection(int &vertical, int &horizontal, Coord_t start, Coord_t end) {
    if (start.y < end.y) {
        vertical = 1;
    } else if (start.y == end.y) {
        vertical = 0;
    } else {
        vertical = -1;
    }

    if (start.x < end.x) {
        horizontal = 1;
    } else if (start.x == end.x) {
        horizontal = 0;
    } else {
        horizontal = -1;
    }

    if (vertical != 0 && horizontal != 0) {
        if (randomNumber(2) == 1) {
            vertical = 0;
        } else {
            horizontal = 0;
        }
    }
}

// Chance of wandering direction
static void chanceOfRandomDirection(int &vertical, int &horizontal) {
    int direction = randomNumber(4);

    if (direction < 3) {
        horizontal = 0;
        vertical = -3 + (direction << 1); // direction=1 -> y=-1; direction=2 -> y=1
    } else {
        vertical = 0;
        horizontal = -7 + (direction << 1); // direction=3 -> x=-1; direction=4 -> x=1
    }
}

// Blanks out entire cave -RAK-
static void dungeonBlankEntireCave() {
    memset((char *) &dg.floor[0][0], 0, sizeof(dg.floor));
}

// Fills in empty spots with desired rock -RAK-
// Note: 9 is a temporary value.
static void dungeonFillEmptyTilesWith(uint8_t rock_type) {
    // no need to check the border of the cave
    for (int y = dg.height - 2; y > 0; y--) {
        int x = 1;

        for (int j = dg.width - 2; j > 0; j--) {
            if (dg.floor[y][x].feature_id == TILE_NULL_WALL || dg.floor[y][x].feature_id == TMP1_WALL || dg.floor[y][x].feature_id == TMP2_WALL) {
                dg.floor[y][x].feature_id = rock_type;
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
    Tile_t(*left_ptr)[MAX_WIDTH];
    Tile_t(*right_ptr)[MAX_WIDTH];

    // put permanent wall on leftmost row and rightmost row
    left_ptr = (Tile_t(*)[MAX_WIDTH]) & dg.floor[0][0];
    right_ptr = (Tile_t(*)[MAX_WIDTH]) & dg.floor[0][dg.width - 1];

    for (int i = 0; i < dg.height; i++) {
#ifdef DEBUG
        assert((Tile_t *) left_ptr == &floor[i][0]);
        assert((Tile_t *) right_ptr == &floor[i][dg.width - 1]);
#endif

        ((Tile_t *) left_ptr)->feature_id = TILE_BOUNDARY_WALL;
        left_ptr++;

        ((Tile_t *) right_ptr)->feature_id = TILE_BOUNDARY_WALL;
        right_ptr++;
    }

    // put permanent wall on top row and bottom row
    Tile_t *top_ptr = &dg.floor[0][0];
    Tile_t *bottom_ptr = &dg.floor[dg.height - 1][0];

    for (int i = 0; i < dg.width; i++) {
#ifdef DEBUG
        assert(top_ptr == &floor[0][i]);
        assert(bottom_ptr == &floor[dg.height - 1][i]);
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
    Coord_t coord = Coord_t{
        (dg.height / 2) + 11 - randomNumber(23),
        (dg.width / 2) + 16 - randomNumber(33),
    };

    // Get random direction. Numbers 1-4, 6-9
    int dir = randomNumber(8);
    if (dir > 4) {
        dir += 1;
    }

    // Place streamer into dungeon
    int t1 = 2 * config::dungeon::DUN_STREAMER_WIDTH + 1; // Constants
    int t2 = config::dungeon::DUN_STREAMER_WIDTH + 1;

    do {
        for (int i = 0; i < config::dungeon::DUN_STREAMER_DENSITY; i++) {
            Coord_t spot = Coord_t{
                coord.y + randomNumber(t1) - t2,
                coord.x + randomNumber(t1) - t2,
            };

            if (coordInBounds(spot)) {
                if (dg.floor[spot.y][spot.x].feature_id == TILE_GRANITE_WALL) {
                    dg.floor[spot.y][spot.x].feature_id = rock_type;

                    if (randomNumber(chance_of_treasure) == 1) {
                        dungeonPlaceGold(spot);
                    }
                }
            }
        }
    } while (playerMovePosition(dir, coord));
}

static void dungeonPlaceOpenDoor(Coord_t coord) {
    int cur_pos = popt();
    dg.floor[coord.y][coord.x].treasure_id = (uint8_t) cur_pos;
    inventoryItemCopyTo(config::dungeon::objects::OBJ_OPEN_DOOR, game.treasure.list[cur_pos]);
    dg.floor[coord.y][coord.x].feature_id = TILE_CORR_FLOOR;
}

static void dungeonPlaceBrokenDoor(Coord_t coord) {
    int cur_pos = popt();
    dg.floor[coord.y][coord.x].treasure_id = (uint8_t) cur_pos;
    inventoryItemCopyTo(config::dungeon::objects::OBJ_OPEN_DOOR, game.treasure.list[cur_pos]);
    dg.floor[coord.y][coord.x].feature_id = TILE_CORR_FLOOR;
    game.treasure.list[cur_pos].misc_use = 1;
}

static void dungeonPlaceClosedDoor(Coord_t coord) {
    int cur_pos = popt();
    dg.floor[coord.y][coord.x].treasure_id = (uint8_t) cur_pos;
    inventoryItemCopyTo(config::dungeon::objects::OBJ_CLOSED_DOOR, game.treasure.list[cur_pos]);
    dg.floor[coord.y][coord.x].feature_id = TILE_BLOCKED_FLOOR;
}

static void dungeonPlaceLockedDoor(Coord_t coord) {
    int cur_pos = popt();
    dg.floor[coord.y][coord.x].treasure_id = (uint8_t) cur_pos;
    inventoryItemCopyTo(config::dungeon::objects::OBJ_CLOSED_DOOR, game.treasure.list[cur_pos]);
    dg.floor[coord.y][coord.x].feature_id = TILE_BLOCKED_FLOOR;
    game.treasure.list[cur_pos].misc_use = (int16_t)(randomNumber(10) + 10);
}

static void dungeonPlaceStuckDoor(Coord_t coord) {
    int cur_pos = popt();
    dg.floor[coord.y][coord.x].treasure_id = (uint8_t) cur_pos;
    inventoryItemCopyTo(config::dungeon::objects::OBJ_CLOSED_DOOR, game.treasure.list[cur_pos]);
    dg.floor[coord.y][coord.x].feature_id = TILE_BLOCKED_FLOOR;
    game.treasure.list[cur_pos].misc_use = (int16_t)(-randomNumber(10) - 10);
}

static void dungeonPlaceSecretDoor(Coord_t coord) {
    int cur_pos = popt();
    dg.floor[coord.y][coord.x].treasure_id = (uint8_t) cur_pos;
    inventoryItemCopyTo(config::dungeon::objects::OBJ_SECRET_DOOR, game.treasure.list[cur_pos]);
    dg.floor[coord.y][coord.x].feature_id = TILE_BLOCKED_FLOOR;
}

static void dungeonPlaceDoor(Coord_t coord) {
    int door_type = randomNumber(3);

    if (door_type == 1) {
        if (randomNumber(4) == 1) {
            dungeonPlaceBrokenDoor(coord);
        } else {
            dungeonPlaceOpenDoor(coord);
        }
    } else if (door_type == 2) {
        door_type = randomNumber(12);

        if (door_type > 3) {
            dungeonPlaceClosedDoor(coord);
        } else if (door_type == 3) {
            dungeonPlaceStuckDoor(coord);
        } else {
            dungeonPlaceLockedDoor(coord);
        }
    } else {
        dungeonPlaceSecretDoor(coord);
    }
}

// Place an up staircase at given y, x -RAK-
static void dungeonPlaceUpStairs(Coord_t coord) {
    if (dg.floor[coord.y][coord.x].treasure_id != 0) {
        (void) dungeonDeleteObject(coord);
    }

    int cur_pos = popt();
    dg.floor[coord.y][coord.x].treasure_id = (uint8_t) cur_pos;
    inventoryItemCopyTo(config::dungeon::objects::OBJ_UP_STAIR, game.treasure.list[cur_pos]);
}

// Place a down staircase at given y, x -RAK-
static void dungeonPlaceDownStairs(Coord_t coord) {
    if (dg.floor[coord.y][coord.x].treasure_id != 0) {
        (void) dungeonDeleteObject(coord);
    }

    int cur_pos = popt();
    dg.floor[coord.y][coord.x].treasure_id = (uint8_t) cur_pos;
    inventoryItemCopyTo(config::dungeon::objects::OBJ_DOWN_STAIR, game.treasure.list[cur_pos]);
}

// Places a staircase 1=up, 2=down -RAK-
static void dungeonPlaceStairs(int stair_type, int number, int walls) {
    Coord_t coord1 = Coord_t{0, 0};
    Coord_t coord2 = Coord_t{0, 0};

    for (int i = 0; i < number; i++) {
        bool placed = false;

        while (!placed) {
            int j = 0;

            do {
                // Note:
                // don't let y1/x1 be zero,
                // don't let y2/x2 be equal to dg.height-1/dg.width-1,
                // these values are always BOUNDARY_ROCK.
                coord1.y = randomNumber(dg.height - 14);
                coord1.x = randomNumber(dg.width - 14);
                coord2.y = coord1.y + 12;
                coord2.x = coord1.x + 12;

                do {
                    do {
                        if (dg.floor[coord1.y][coord1.x].feature_id <= MAX_OPEN_SPACE && dg.floor[coord1.y][coord1.x].treasure_id == 0 && coordWallsNextTo(coord1) >= walls) {
                            placed = true;
                            if (stair_type == 1) {
                                dungeonPlaceUpStairs(coord1);
                            } else {
                                dungeonPlaceDownStairs(coord1);
                            }
                        }
                        coord1.x++;
                    } while ((coord1.x != coord2.x) && (!placed));

                    coord1.x = coord2.x - 12;
                    coord1.y++;
                } while ((coord1.y != coord2.y) && (!placed));

                j++;
            } while ((!placed) && (j <= 30));

            walls--;
        }
    }
}

// Place a trap with a given displacement of point -RAK-
static void dungeonPlaceVaultTrap(Coord_t coord, Coord_t displacement, int number) {
    Coord_t spot = Coord_t{0, 0};

    for (int i = 0; i < number; i++) {
        bool placed = false;

        for (int count = 0; !placed && count <= 5; count++) {
            spot.y = coord.y - displacement.y - 1 + randomNumber(2 * displacement.y + 1);
            spot.x = coord.x - displacement.x - 1 + randomNumber(2 * displacement.x + 1);

            if (dg.floor[spot.y][spot.x].feature_id != TILE_NULL_WALL && dg.floor[spot.y][spot.x].feature_id <= MAX_CAVE_FLOOR && dg.floor[spot.y][spot.x].treasure_id == 0) {
                dungeonSetTrap(spot, randomNumber(config::dungeon::objects::MAX_TRAPS) - 1);
                placed = true;
            }
        }
    }
}

// Place a trap with a given displacement of point -RAK-
static void dungeonPlaceVaultMonster(Coord_t coord, int number) {
    Coord_t spot = Coord_t{0, 0};

    for (int i = 0; i < number; i++) {
        spot.y = coord.y;
        spot.x = coord.x;
        (void) monsterSummon(spot, true);
    }
}

// Builds a room at a row, column coordinate -RAK-
static void dungeonBuildRoom(Coord_t coord) {
    uint8_t floor = dungeonFloorTileForLevel();

    int height = coord.y - randomNumber(4);
    int depth = coord.y + randomNumber(3);
    int left = coord.x - randomNumber(11);
    int right = coord.x + randomNumber(11);

    // the x dim of rooms tends to be much larger than the y dim,
    // so don't bother rewriting the y loop.

    int y, x;

    for (y = height; y <= depth; y++) {
        for (x = left; x <= right; x++) {
            dg.floor[y][x].feature_id = floor;
            dg.floor[y][x].perma_lit_room = true;
        }
    }

    for (y = height - 1; y <= depth + 1; y++) {
        dg.floor[y][left - 1].feature_id = TILE_GRANITE_WALL;
        dg.floor[y][left - 1].perma_lit_room = true;

        dg.floor[y][right + 1].feature_id = TILE_GRANITE_WALL;
        dg.floor[y][right + 1].perma_lit_room = true;
    }

    for (x = left; x <= right; x++) {
        dg.floor[height - 1][x].feature_id = TILE_GRANITE_WALL;
        dg.floor[height - 1][x].perma_lit_room = true;

        dg.floor[depth + 1][x].feature_id = TILE_GRANITE_WALL;
        dg.floor[depth + 1][x].perma_lit_room = true;
    }
}

// Builds a room at a row, column coordinate -RAK-
// Type 1 unusual rooms are several overlapping rectangular ones
static void dungeonBuildRoomOverlappingRectangles(Coord_t coord) {
    uint8_t floor = dungeonFloorTileForLevel();

    int limit = 1 + randomNumber(2);

    for (int count = 0; count < limit; count++) {
        int height = coord.y - randomNumber(4);
        int depth = coord.y + randomNumber(3);
        int left = coord.x - randomNumber(11);
        int right = coord.x + randomNumber(11);

        // the x dim of rooms tends to be much larger than the y dim,
        // so don't bother rewriting the y loop.

        int y, x;

        for (y = height; y <= depth; y++) {
            for (x = left; x <= right; x++) {
                dg.floor[y][x].feature_id = floor;
                dg.floor[y][x].perma_lit_room = true;
            }
        }
        for (y = (height - 1); y <= (depth + 1); y++) {
            if (dg.floor[y][left - 1].feature_id != floor) {
                dg.floor[y][left - 1].feature_id = TILE_GRANITE_WALL;
                dg.floor[y][left - 1].perma_lit_room = true;
            }

            if (dg.floor[y][right + 1].feature_id != floor) {
                dg.floor[y][right + 1].feature_id = TILE_GRANITE_WALL;
                dg.floor[y][right + 1].perma_lit_room = true;
            }
        }

        for (x = left; x <= right; x++) {
            if (dg.floor[height - 1][x].feature_id != floor) {
                dg.floor[height - 1][x].feature_id = TILE_GRANITE_WALL;
                dg.floor[height - 1][x].perma_lit_room = true;
            }

            if (dg.floor[depth + 1][x].feature_id != floor) {
                dg.floor[depth + 1][x].feature_id = TILE_GRANITE_WALL;
                dg.floor[depth + 1][x].perma_lit_room = true;
            }
        }
    }
}

static void dungeonPlaceRandomSecretDoor(Coord_t coord, int depth, int height, int left, int right) {
    switch (randomNumber(4)) {
        case 1:
            dungeonPlaceSecretDoor(Coord_t{height - 1, coord.x});
            break;
        case 2:
            dungeonPlaceSecretDoor(Coord_t{depth + 1, coord.x});
            break;
        case 3:
            dungeonPlaceSecretDoor(Coord_t{coord.y, left - 1});
            break;
        default:
            dungeonPlaceSecretDoor(Coord_t{coord.y, right + 1});
            break;
    }
}

static void dungeonPlaceVault(Coord_t coord) {
    for (int y = coord.y - 1; y <= coord.y + 1; y++) {
        dg.floor[y][coord.x - 1].feature_id = TMP1_WALL;
        dg.floor[y][coord.x + 1].feature_id = TMP1_WALL;
    }

    dg.floor[coord.y - 1][coord.x].feature_id = TMP1_WALL;
    dg.floor[coord.y + 1][coord.x].feature_id = TMP1_WALL;
}

static void dungeonPlaceTreasureVault(Coord_t coord, int depth, int height, int left, int right) {
    dungeonPlaceRandomSecretDoor(coord, depth, height, left, right);
    dungeonPlaceVault(coord);

    // Place a locked door
    int offset = randomNumber(4);
    if (offset < 3) {
        // 1 -> y-1; 2 -> y+1
        dungeonPlaceLockedDoor(Coord_t{coord.y - 3 + (offset << 1), coord.x});
    } else {
        dungeonPlaceLockedDoor(Coord_t{coord.y, coord.x - 7 + (offset << 1)});
    }
}

static void dungeonPlaceInnerPillars(Coord_t coord) {
    int y, x;

    for (y = coord.y - 1; y <= coord.y + 1; y++) {
        for (x = coord.x - 1; x <= coord.x + 1; x++) {
            dg.floor[y][x].feature_id = TMP1_WALL;
        }
    }

    if (randomNumber(2) != 1) {
        return;
    }

    int offset = randomNumber(2);

    for (y = coord.y - 1; y <= coord.y + 1; y++) {
        for (x = coord.x - 5 - offset; x <= coord.x - 3 - offset; x++) {
            dg.floor[y][x].feature_id = TMP1_WALL;
        }
    }

    for (y = coord.y - 1; y <= coord.y + 1; y++) {
        for (x = coord.x + 3 + offset; x <= coord.x + 5 + offset; x++) {
            dg.floor[y][x].feature_id = TMP1_WALL;
        }
    }
}

static void dungeonPlaceMazeInsideRoom(int depth, int height, int left, int right) {
    for (int y = height; y <= depth; y++) {
        for (int x = left; x <= right; x++) {
            if ((0x1 & (x + y)) != 0) {
                dg.floor[y][x].feature_id = TMP1_WALL;
            }
        }
    }
}

static void dungeonPlaceFourSmallRooms(Coord_t coord, int depth, int height, int left, int right) {
    for (int y = height; y <= depth; y++) {
        dg.floor[y][coord.x].feature_id = TMP1_WALL;
    }

    for (int x = left; x <= right; x++) {
        dg.floor[coord.y][x].feature_id = TMP1_WALL;
    }

    // place random secret door
    if (randomNumber(2) == 1) {
        int offset = randomNumber(10);
        dungeonPlaceSecretDoor(Coord_t{height - 1, coord.x - offset});
        dungeonPlaceSecretDoor(Coord_t{height - 1, coord.x + offset});
        dungeonPlaceSecretDoor(Coord_t{depth + 1, coord.x - offset});
        dungeonPlaceSecretDoor(Coord_t{depth + 1, coord.x + offset});
    } else {
        int offset = randomNumber(3);
        dungeonPlaceSecretDoor(Coord_t{coord.y + offset, left - 1});
        dungeonPlaceSecretDoor(Coord_t{coord.y - offset, left - 1});
        dungeonPlaceSecretDoor(Coord_t{coord.y + offset, right + 1});
        dungeonPlaceSecretDoor(Coord_t{coord.y - offset, right + 1});
    }
}

// Type 2 unusual rooms all have an inner room:
//   1 - Just an inner room with one door
//   2 - An inner room within an inner room
//   3 - An inner room with pillar(s)
//   4 - Inner room has a maze
//   5 - A set of four inner rooms
enum class InnerRoomTypes {
    Plain = 1,
    TreasureVault,
    Pillars,
    Maze,
    FourSmallRooms,
};

// Builds a type 2 unusual room at a row, column coordinate -RAK-
static void dungeonBuildRoomWithInnerRooms(Coord_t coord) {
    uint8_t floor = dungeonFloorTileForLevel();

    int height = coord.y - 4;
    int depth = coord.y + 4;
    int left = coord.x - 11;
    int right = coord.x + 11;

    // the x dim of rooms tends to be much larger than the y dim,
    // so don't bother rewriting the y loop.

    for (int i = height; i <= depth; i++) {
        for (int j = left; j <= right; j++) {
            dg.floor[i][j].feature_id = floor;
            dg.floor[i][j].perma_lit_room = true;
        }
    }

    for (int i = (height - 1); i <= (depth + 1); i++) {
        dg.floor[i][left - 1].feature_id = TILE_GRANITE_WALL;
        dg.floor[i][left - 1].perma_lit_room = true;

        dg.floor[i][right + 1].feature_id = TILE_GRANITE_WALL;
        dg.floor[i][right + 1].perma_lit_room = true;
    }

    for (int i = left; i <= right; i++) {
        dg.floor[height - 1][i].feature_id = TILE_GRANITE_WALL;
        dg.floor[height - 1][i].perma_lit_room = true;

        dg.floor[depth + 1][i].feature_id = TILE_GRANITE_WALL;
        dg.floor[depth + 1][i].perma_lit_room = true;
    }

    // The inner room
    height = height + 2;
    depth = depth - 2;
    left = left + 2;
    right = right - 2;

    for (int i = (height - 1); i <= (depth + 1); i++) {
        dg.floor[i][left - 1].feature_id = TMP1_WALL;
        dg.floor[i][right + 1].feature_id = TMP1_WALL;
    }

    for (int i = left; i <= right; i++) {
        dg.floor[height - 1][i].feature_id = TMP1_WALL;
        dg.floor[depth + 1][i].feature_id = TMP1_WALL;
    }

    // Inner room variations
    switch ((InnerRoomTypes) randomNumber(5)) {
        case InnerRoomTypes::Plain:
            dungeonPlaceRandomSecretDoor(coord, depth, height, left, right);
            dungeonPlaceVaultMonster(coord, 1);
            break;
        case InnerRoomTypes::TreasureVault:
            dungeonPlaceTreasureVault(coord, depth, height, left, right);

            // Guard the treasure well
            dungeonPlaceVaultMonster(coord, 2 + randomNumber(3));

            // If the monsters don't get 'em.
            dungeonPlaceVaultTrap(coord, Coord_t{4, 10}, 2 + randomNumber(3));
            break;
        case InnerRoomTypes::Pillars:
            dungeonPlaceRandomSecretDoor(coord, depth, height, left, right);

            dungeonPlaceInnerPillars(coord);

            if (randomNumber(3) != 1) {
                break;
            }

            // Inner rooms
            for (int i = coord.x - 5; i <= coord.x + 5; i++) {
                dg.floor[coord.y - 1][i].feature_id = TMP1_WALL;
                dg.floor[coord.y + 1][i].feature_id = TMP1_WALL;
            }
            dg.floor[coord.y][coord.x - 5].feature_id = TMP1_WALL;
            dg.floor[coord.y][coord.x + 5].feature_id = TMP1_WALL;

            dungeonPlaceSecretDoor(Coord_t{coord.y - 3 + (randomNumber(2) << 1), coord.x - 3});
            dungeonPlaceSecretDoor(Coord_t{coord.y - 3 + (randomNumber(2) << 1), coord.x + 3});

            if (randomNumber(3) == 1) {
                dungeonPlaceRandomObjectAt(Coord_t{coord.y, coord.x - 2}, false);
            }

            if (randomNumber(3) == 1) {
                dungeonPlaceRandomObjectAt(Coord_t{coord.y, coord.x + 2}, false);
            }

            dungeonPlaceVaultMonster(Coord_t{coord.y, coord.x - 2}, randomNumber(2));
            dungeonPlaceVaultMonster(Coord_t{coord.y, coord.x + 2}, randomNumber(2));
            break;
        case InnerRoomTypes::Maze:
            dungeonPlaceRandomSecretDoor(coord, depth, height, left, right);

            dungeonPlaceMazeInsideRoom(depth, height, left, right);

            // Monsters just love mazes.
            dungeonPlaceVaultMonster(Coord_t{coord.y, coord.x - 5}, randomNumber(3));
            dungeonPlaceVaultMonster(Coord_t{coord.y, coord.x + 5}, randomNumber(3));

            // Traps make them entertaining.
            dungeonPlaceVaultTrap(Coord_t{coord.y, coord.x - 3}, Coord_t{2, 8}, randomNumber(3));
            dungeonPlaceVaultTrap(Coord_t{coord.y, coord.x + 3}, Coord_t{2, 8}, randomNumber(3));

            // Mazes should have some treasure too..
            for (int i = 0; i < 3; i++) {
                dungeonPlaceRandomObjectNear(coord, 1);
            }
            break;
        case InnerRoomTypes::FourSmallRooms:
            dungeonPlaceFourSmallRooms(coord, depth, height, left, right);

            // Treasure in each one.
            dungeonPlaceRandomObjectNear(coord, 2 + randomNumber(2));

            // Gotta have some monsters.
            dungeonPlaceVaultMonster(Coord_t{coord.y + 2, coord.x - 4}, randomNumber(2));
            dungeonPlaceVaultMonster(Coord_t{coord.y + 2, coord.x + 4}, randomNumber(2));
            dungeonPlaceVaultMonster(Coord_t{coord.y - 2, coord.x - 4}, randomNumber(2));
            dungeonPlaceVaultMonster(Coord_t{coord.y - 2, coord.x + 4}, randomNumber(2));
            break;
        default:
            // All cases are handled, so this should never be reached!
            break;
    }
}

static void dungeonPlaceLargeMiddlePillar(Coord_t coord) {
    for (int y = coord.y - 1; y <= coord.y + 1; y++) {
        for (int x = coord.x - 1; x <= coord.x + 1; x++) {
            dg.floor[y][x].feature_id = TMP1_WALL;
        }
    }
}

// Builds a room at a row, column coordinate -RAK-
// Type 3 unusual rooms are cross shaped
static void dungeonBuildRoomCrossShaped(Coord_t coord) {
    uint8_t floor = dungeonFloorTileForLevel();

    int random_offset = 2 + randomNumber(2);

    int height = coord.y - random_offset;
    int depth = coord.y + random_offset;
    int left = coord.x - 1;
    int right = coord.x + 1;

    for (int i = height; i <= depth; i++) {
        for (int j = left; j <= right; j++) {
            dg.floor[i][j].feature_id = floor;
            dg.floor[i][j].perma_lit_room = true;
        }
    }

    for (int i = height - 1; i <= depth + 1; i++) {
        dg.floor[i][left - 1].feature_id = TILE_GRANITE_WALL;
        dg.floor[i][left - 1].perma_lit_room = true;

        dg.floor[i][right + 1].feature_id = TILE_GRANITE_WALL;
        dg.floor[i][right + 1].perma_lit_room = true;
    }

    for (int i = left; i <= right; i++) {
        dg.floor[height - 1][i].feature_id = TILE_GRANITE_WALL;
        dg.floor[height - 1][i].perma_lit_room = true;

        dg.floor[depth + 1][i].feature_id = TILE_GRANITE_WALL;
        dg.floor[depth + 1][i].perma_lit_room = true;
    }

    random_offset = 2 + randomNumber(9);

    height = coord.y - 1;
    depth = coord.y + 1;
    left = coord.x - random_offset;
    right = coord.x + random_offset;

    for (int i = height; i <= depth; i++) {
        for (int j = left; j <= right; j++) {
            dg.floor[i][j].feature_id = floor;
            dg.floor[i][j].perma_lit_room = true;
        }
    }

    for (int i = height - 1; i <= depth + 1; i++) {
        if (dg.floor[i][left - 1].feature_id != floor) {
            dg.floor[i][left - 1].feature_id = TILE_GRANITE_WALL;
            dg.floor[i][left - 1].perma_lit_room = true;
        }

        if (dg.floor[i][right + 1].feature_id != floor) {
            dg.floor[i][right + 1].feature_id = TILE_GRANITE_WALL;
            dg.floor[i][right + 1].perma_lit_room = true;
        }
    }

    for (int i = left; i <= right; i++) {
        if (dg.floor[height - 1][i].feature_id != floor) {
            dg.floor[height - 1][i].feature_id = TILE_GRANITE_WALL;
            dg.floor[height - 1][i].perma_lit_room = true;
        }

        if (dg.floor[depth + 1][i].feature_id != floor) {
            dg.floor[depth + 1][i].feature_id = TILE_GRANITE_WALL;
            dg.floor[depth + 1][i].perma_lit_room = true;
        }
    }

    // Special features.
    switch (randomNumber(4)) {
        case 1: // Large middle pillar
            dungeonPlaceLargeMiddlePillar(coord);
            break;
        case 2: // Inner treasure vault
            dungeonPlaceVault(coord);

            // Place a secret door
            random_offset = randomNumber(4);
            if (random_offset < 3) {
                dungeonPlaceSecretDoor(Coord_t{coord.y - 3 + (random_offset << 1), coord.x});
            } else {
                dungeonPlaceSecretDoor(Coord_t{coord.y, coord.x - 7 + (random_offset << 1)});
            }

            // Place a treasure in the vault
            dungeonPlaceRandomObjectAt(coord, false);

            // Let's guard the treasure well.
            dungeonPlaceVaultMonster(coord, 2 + randomNumber(2));

            // Traps naturally
            dungeonPlaceVaultTrap(coord, Coord_t{4, 4}, 1 + randomNumber(3));
            break;
        case 3:
            if (randomNumber(3) == 1) {
                dg.floor[coord.y - 1][coord.x - 2].feature_id = TMP1_WALL;
                dg.floor[coord.y + 1][coord.x - 2].feature_id = TMP1_WALL;
                dg.floor[coord.y - 1][coord.x + 2].feature_id = TMP1_WALL;
                dg.floor[coord.y + 1][coord.x + 2].feature_id = TMP1_WALL;
                dg.floor[coord.y - 2][coord.x - 1].feature_id = TMP1_WALL;
                dg.floor[coord.y - 2][coord.x + 1].feature_id = TMP1_WALL;
                dg.floor[coord.y + 2][coord.x - 1].feature_id = TMP1_WALL;
                dg.floor[coord.y + 2][coord.x + 1].feature_id = TMP1_WALL;
                if (randomNumber(3) == 1) {
                    dungeonPlaceSecretDoor(Coord_t{coord.y, coord.x - 2});
                    dungeonPlaceSecretDoor(Coord_t{coord.y, coord.x + 2});
                    dungeonPlaceSecretDoor(Coord_t{coord.y - 2, coord.x});
                    dungeonPlaceSecretDoor(Coord_t{coord.y + 2, coord.x});
                }
            } else if (randomNumber(3) == 1) {
                dg.floor[coord.y][coord.x].feature_id = TMP1_WALL;
                dg.floor[coord.y - 1][coord.x].feature_id = TMP1_WALL;
                dg.floor[coord.y + 1][coord.x].feature_id = TMP1_WALL;
                dg.floor[coord.y][coord.x - 1].feature_id = TMP1_WALL;
                dg.floor[coord.y][coord.x + 1].feature_id = TMP1_WALL;
            } else if (randomNumber(3) == 1) {
                dg.floor[coord.y][coord.x].feature_id = TMP1_WALL;
            }
            break;
        // handled by the default case
        // case 4:
        //     // no special feature!
        //     break;
        default:
            break;
    }
}

// Constructs a tunnel between two points
static void dungeonBuildTunnel(Coord_t start, Coord_t end) {
    Coord_t tunnels_tk[1000], walls_tk[1000];

    // Main procedure for Tunnel
    // Note: 9 is a temporary value
    bool door_flag = false;
    bool stop_flag = false;
    int main_loop_count = 0;
    int start_row = start.y;
    int start_col = start.x;
    int tunnel_index = 0;
    int wall_index = 0;

    int y_direction, x_direction;
    pickCorrectDirection(y_direction, x_direction, start, end);

    do {
        // prevent infinite loops, just in case
        main_loop_count++;
        if (main_loop_count > 2000) {
            stop_flag = true;
        }

        if (randomNumber(100) > config::dungeon::DUN_DIR_CHANGE) {
            if (randomNumber(config::dungeon::DUN_RANDOM_DIR) == 1) {
                chanceOfRandomDirection(y_direction, x_direction);
            } else {
                pickCorrectDirection(y_direction, x_direction, start, end);
            }
        }

        int tmp_row = start.y + y_direction;
        int tmp_col = start.x + x_direction;

        while (!coordInBounds(Coord_t{tmp_row, tmp_col})) {
            if (randomNumber(config::dungeon::DUN_RANDOM_DIR) == 1) {
                chanceOfRandomDirection(y_direction, x_direction);
            } else {
                pickCorrectDirection(y_direction, x_direction, start, end);
            }
            tmp_row = start.y + y_direction;
            tmp_col = start.x + x_direction;
        }

        switch (dg.floor[tmp_row][tmp_col].feature_id) {
            case TILE_NULL_WALL:
                start.y = tmp_row;
                start.x = tmp_col;
                if (tunnel_index < 1000) {
                    tunnels_tk[tunnel_index].y = start.y;
                    tunnels_tk[tunnel_index].x = start.x;
                    tunnel_index++;
                }
                door_flag = false;
                break;
            case TMP2_WALL:
                // do nothing
                break;
            case TILE_GRANITE_WALL:
                start.y = tmp_row;
                start.x = tmp_col;

                if (wall_index < 1000) {
                    walls_tk[wall_index].y = start.y;
                    walls_tk[wall_index].x = start.x;
                    wall_index++;
                }

                for (int y = start.y - 1; y <= start.y + 1; y++) {
                    for (int x = start.x - 1; x <= start.x + 1; x++) {
                        if (coordInBounds(Coord_t{y, x})) {
                            // values 11 and 12 are impossible here, dungeonPlaceStreamerRock
                            // is never run before dungeonBuildTunnel
                            if (dg.floor[y][x].feature_id == TILE_GRANITE_WALL) {
                                dg.floor[y][x].feature_id = TMP2_WALL;
                            }
                        }
                    }
                }
                break;
            case TILE_CORR_FLOOR:
            case TILE_BLOCKED_FLOOR:
                start.y = tmp_row;
                start.x = tmp_col;

                if (!door_flag) {
                    if (door_index < 100) {
                        doors_tk[door_index].y = start.y;
                        doors_tk[door_index].x = start.x;
                        door_index++;
                    }
                    door_flag = true;
                }

                if (randomNumber(100) > config::dungeon::DUN_TUNNELING) {
                    // make sure that tunnel has gone a reasonable distance
                    // before stopping it, this helps prevent isolated rooms
                    tmp_row = start.y - start_row;
                    if (tmp_row < 0) {
                        tmp_row = -tmp_row;
                    }

                    tmp_col = start.x - start_col;
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
                start.y = tmp_row;
                start.x = tmp_col;
        }
    } while ((start.y != end.y || start.x != end.x) && !stop_flag);

    for (int i = 0; i < tunnel_index; i++) {
        dg.floor[tunnels_tk[i].y][tunnels_tk[i].x].feature_id = TILE_CORR_FLOOR;
    }

    for (int i = 0; i < wall_index; i++) {
        Tile_t &tile = dg.floor[walls_tk[i].y][walls_tk[i].x];

        if (tile.feature_id == TMP2_WALL) {
            if (randomNumber(100) < config::dungeon::DUN_ROOM_DOORS) {
                dungeonPlaceDoor(Coord_t{walls_tk[i].y, walls_tk[i].x});
            } else {
                // these have to be doorways to rooms
                tile.feature_id = TILE_CORR_FLOOR;
            }
        }
    }
}

static bool dungeonIsNextTo(Coord_t coord) {
    if (coordCorridorWallsNextTo(coord) > 2) {
        bool vertical = dg.floor[coord.y - 1][coord.x].feature_id >= MIN_CAVE_WALL && dg.floor[coord.y + 1][coord.x].feature_id >= MIN_CAVE_WALL;
        bool horizontal = dg.floor[coord.y][coord.x - 1].feature_id >= MIN_CAVE_WALL && dg.floor[coord.y][coord.x + 1].feature_id >= MIN_CAVE_WALL;

        return vertical || horizontal;
    }

    return false;
}

// Places door at y, x position if at least 2 walls found
static void dungeonPlaceDoorIfNextToTwoWalls(Coord_t coord) {
    if (dg.floor[coord.y][coord.x].feature_id == TILE_CORR_FLOOR && randomNumber(100) > config::dungeon::DUN_TUNNEL_DOORS && dungeonIsNextTo(coord)) {
        dungeonPlaceDoor(coord);
    }
}

// Returns random co-ordinates -RAK-
static void dungeonNewSpot(Coord_t &coord) {
    Tile_t *tile = nullptr;
    Coord_t position = Coord_t{0, 0};

    do {
        position.y = (int32_t) randomNumber(dg.height - 2);
        position.x = (int32_t) randomNumber(dg.width - 2);
        tile = &dg.floor[position.y][position.x];
    } while (tile->feature_id >= MIN_CLOSED_SPACE || tile->creature_id != 0 || tile->treasure_id != 0);

    coord.y = position.y;
    coord.x = position.x;
}

// Functions to emulate the original Pascal sets
static bool setRooms(int tile_id) {
    return (tile_id == TILE_DARK_FLOOR || tile_id == TILE_LIGHT_FLOOR);
}

static bool setCorridors(int tile_id) {
    return (tile_id == TILE_CORR_FLOOR || tile_id == TILE_BLOCKED_FLOOR);
}

static bool setFloors(int tile_id) {
    return (tile_id <= MAX_CAVE_FLOOR);
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

    int random_room_count = randomNumberNormalDistribution(config::dungeon::DUN_ROOMS_MEAN, 2);
    for (int i = 0; i < random_room_count; i++) {
        room_map[randomNumber(row_rooms) - 1][randomNumber(col_rooms) - 1] = true;
    }

    // Build rooms
    int location_id = 0;
    Coord_t locations[400];

    for (int row = 0; row < row_rooms; row++) {
        for (int col = 0; col < col_rooms; col++) {
            if (room_map[row][col]) {
                locations[location_id].y = (int32_t)(row * (SCREEN_HEIGHT >> 1) + QUART_HEIGHT);
                locations[location_id].x = (int32_t)(col * (SCREEN_WIDTH >> 1) + QUART_WIDTH);
                if (dg.current_level > randomNumber(config::dungeon::DUN_UNUSUAL_ROOMS)) {
                    int room_type = randomNumber(3);

                    if (room_type == 1) {
                        dungeonBuildRoomOverlappingRectangles(locations[location_id]);
                    } else if (room_type == 2) {
                        dungeonBuildRoomWithInnerRooms(locations[location_id]);
                    } else {
                        dungeonBuildRoomCrossShaped(locations[location_id]);
                    }
                } else {
                    dungeonBuildRoom(locations[location_id]);
                }
                location_id++;
            }
        }
    }

    for (int i = 0; i < location_id; i++) {
        int pick1 = randomNumber(location_id) - 1;
        int pick2 = randomNumber(location_id) - 1;

        int32_t y = locations[pick1].y;
        int32_t x = locations[pick1].x;
        locations[pick1].y = locations[pick2].y;
        locations[pick1].x = locations[pick2].x;
        locations[pick2].y = y;
        locations[pick2].x = x;
    }

    door_index = 0;

    // move zero entry to location_id, so that can call dungeonBuildTunnel all location_id times
    locations[location_id].y = locations[0].y;
    locations[location_id].x = locations[0].x;

    for (int i = 0; i < location_id; i++) {
        dungeonBuildTunnel(locations[i + 1], locations[i]);
    }

    // Generate walls and streamers
    dungeonFillEmptyTilesWith(TILE_GRANITE_WALL);
    for (int i = 0; i < config::dungeon::DUN_MAGMA_STREAMER; i++) {
        dungeonPlaceStreamerRock(TILE_MAGMA_WALL, config::dungeon::DUN_MAGMA_TREASURE);
    }
    for (int i = 0; i < config::dungeon::DUN_QUARTZ_STREAMER; i++) {
        dungeonPlaceStreamerRock(TILE_QUARTZ_WALL, config::dungeon::DUN_QUARTZ_TREASURE);
    }
    dungeonPlaceBoundaryWalls();

    // Place intersection doors
    for (int i = 0; i < door_index; i++) {
        dungeonPlaceDoorIfNextToTwoWalls(Coord_t{doors_tk[i].y, doors_tk[i].x - 1});
        dungeonPlaceDoorIfNextToTwoWalls(Coord_t{doors_tk[i].y, doors_tk[i].x + 1});
        dungeonPlaceDoorIfNextToTwoWalls(Coord_t{doors_tk[i].y - 1, doors_tk[i].x});
        dungeonPlaceDoorIfNextToTwoWalls(Coord_t{doors_tk[i].y + 1, doors_tk[i].x});
    }

    int alloc_level = (dg.current_level / 3);
    if (alloc_level < 2) {
        alloc_level = 2;
    } else if (alloc_level > 10) {
        alloc_level = 10;
    }

    dungeonPlaceStairs(2, randomNumber(2) + 2, 3);
    dungeonPlaceStairs(1, randomNumber(2), 3);

    // Set up the character coords, used by monsterPlaceNewWithinDistance, monsterPlaceWinning
    Coord_t coord = Coord_t{0, 0};
    dungeonNewSpot(coord);
    py.pos.y = coord.y;
    py.pos.x = coord.x;

    monsterPlaceNewWithinDistance((randomNumber(8) + config::monsters::MON_MIN_PER_LEVEL + alloc_level), 0, true);
    dungeonAllocateAndPlaceObject(setCorridors, 3, randomNumber(alloc_level));
    dungeonAllocateAndPlaceObject(setRooms, 5, randomNumberNormalDistribution(config::dungeon::objects::LEVEL_OBJECTS_PER_ROOM, 3));
    dungeonAllocateAndPlaceObject(setFloors, 5, randomNumberNormalDistribution(config::dungeon::objects::LEVEL_OBJECTS_PER_CORRIDOR, 3));
    dungeonAllocateAndPlaceObject(setFloors, 4, randomNumberNormalDistribution(config::dungeon::objects::LEVEL_TOTAL_GOLD_AND_GEMS, 3));
    dungeonAllocateAndPlaceObject(setFloors, 1, randomNumber(alloc_level));

    if (dg.current_level >= config::monsters::MON_ENDGAME_LEVEL) {
        monsterPlaceWinning();
    }
}

// Builds a store at a row, column coordinate
static void dungeonBuildStore(int store_id, Coord_t coord) {
    int yval = coord.y * 10 + 5;
    int xval = coord.x * 16 + 16;
    int height = yval - randomNumber(3);
    int depth = yval + randomNumber(4);
    int left = xval - randomNumber(6);
    int right = xval + randomNumber(6);

    int y, x;

    for (y = height; y <= depth; y++) {
        for (x = left; x <= right; x++) {
            dg.floor[y][x].feature_id = TILE_BOUNDARY_WALL;
        }
    }

    int tmp = randomNumber(4);
    if (tmp < 3) {
        y = randomNumber(depth - height) + height - 1;

        if (tmp == 1) {
            x = left;
        } else {
            x = right;
        }
    } else {
        x = randomNumber(right - left) + left - 1;

        if (tmp == 3) {
            y = depth;
        } else {
            y = height;
        }
    }

    dg.floor[y][x].feature_id = TILE_CORR_FLOOR;

    int cur_pos = popt();
    dg.floor[y][x].treasure_id = (uint8_t) cur_pos;

    inventoryItemCopyTo(config::dungeon::objects::OBJ_STORE_DOOR + store_id, game.treasure.list[cur_pos]);
}

// Link all free space in treasure list together
static void treasureLinker() {
    for (auto &item : game.treasure.list) {
        inventoryItemCopyTo(config::dungeon::objects::OBJ_NOTHING, item);
    }
    game.treasure.current_id = config::treasure::MIN_TREASURE_LIST_ID;
}

// Link all free space in monster list together
static void monsterLinker() {
    for (auto &monster : monsters) {
        monster = blank_monster;
    }
    next_free_monster_id = config::monsters::MON_MIN_INDEX_ID;
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
            dungeonBuildStore(rooms[room_id], Coord_t{y, x});

            for (int i = room_id; i < rooms_count - 1; i++) {
                rooms[i] = rooms[i + 1];
            }

            rooms_count--;
        }
    }
}

static bool isNighTime() {
    return (0x1 & (dg.game_turn / 5000)) != 0;
}

// Light town based on whether it is Night time, or day time.
static void lightTown() {
    if (isNighTime()) {
        for (int y = 0; y < dg.height; y++) {
            for (int x = 0; x < dg.width; x++) {
                if (dg.floor[y][x].feature_id != TILE_DARK_FLOOR) {
                    dg.floor[y][x].permanent_light = true;
                }
            }
        }
        monsterPlaceNewWithinDistance(config::monsters::MON_MIN_TOWNSFOLK_NIGHT, 3, true);
    } else {
        // ...it is day time
        for (int y = 0; y < dg.height; y++) {
            for (int x = 0; x < dg.width; x++) {
                dg.floor[y][x].permanent_light = true;
            }
        }
        monsterPlaceNewWithinDistance(config::monsters::MON_MIN_TOWNSFOLK_DAY, 3, true);
    }
}

// I may have written the town level code, but I'm not exactly
// proud of it.   Adding the stores required some real slucky
// hooks which I have not had time to re-think. -RAK-

// Town logic flow for generation of new town
static void townGeneration() {
    seedSet(game.town_seed);

    dungeonPlaceTownStores();

    dungeonFillEmptyTilesWith(TILE_DARK_FLOOR);

    // make stairs before seedResetToOldSeed, so that they don't move around
    dungeonPlaceBoundaryWalls();
    dungeonPlaceStairs(2, 1, 0);

    seedResetToOldSeed();

    // Set up the character coords, used by monsterPlaceNewWithinDistance below
    Coord_t coord = Coord_t{0, 0};
    dungeonNewSpot(coord);
    py.pos.y = coord.y;
    py.pos.x = coord.x;

    lightTown();

    storeMaintenance();
}

// Generates a random dungeon level -RAK-
void generateCave() {
    dg.panel.top = 0;
    dg.panel.bottom = 0;
    dg.panel.left = 0;
    dg.panel.right = 0;

    py.pos.y = -1;
    py.pos.x = -1;

    treasureLinker();
    monsterLinker();
    dungeonBlankEntireCave();

    // We're in the dungeon more than the town, so let's default to that -MRC-
    dg.height = MAX_HEIGHT;
    dg.width = MAX_WIDTH;

    if (dg.current_level == 0) {
        dg.height = SCREEN_HEIGHT;
        dg.width = SCREEN_WIDTH;
    }

    dg.panel.max_rows = (int16_t)((dg.height / SCREEN_HEIGHT) * 2 - 2);
    dg.panel.max_cols = (int16_t)((dg.width / SCREEN_WIDTH) * 2 - 2);

    dg.panel.row = dg.panel.max_rows;
    dg.panel.col = dg.panel.max_cols;

    if (dg.current_level == 0) {
        townGeneration();
    } else {
        dungeonGenerate();
    }
}
