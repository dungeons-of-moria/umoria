// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// The running algorithm: -CJS-

#include "headers.h"

// Overview: You keep moving until something interesting happens. If you are in
// an enclosed space, you follow corners. This is the usual corridor scheme. If
// you are in an open space, you go straight, but stop before entering enclosed
// space. This is analogous to reaching doorways. If you have enclosed space on
// one side only (that is, running along side a wall) stop if your wall opens
// out, or your open space closes in. Either case corresponds to a doorway.
//
// What happens depends on what you can really SEE. (i.e. if you have no light,
// then running along a dark corridor is JUST like running in a dark room.) The
// algorithm works equally well in corridors, rooms, mine tailings, earthquake
// rubble, etc, etc.
//
// These conditions are kept in static memory:
//      find_openarea  You are in the open on at least one
//                       side.
//      find_breakleft   You have a wall on the left, and will
//                       stop if it opens
//      find_breakright  You have a wall on the right, and will
//                       stop if it opens
//
// To initialize these conditions is the task of playerFindInitialize. If moving from the
// square marked @ to the square marked . (in the two diagrams below), then two
// adjacent squares on the left and the right (L and R) are considered. If either
// one is seen to be closed, then that side is considered to be closed. If both
// sides are closed, then it is an enclosed (corridor) run.
//
//       LL   L
//      @.         L.R
//       RR        @R
//
// Looking at more than just the immediate squares is significant. Consider the
// following case. A run along the corridor will stop just before entering the
// center point, because a choice is clearly established. Running in any of three
// available directions will be defined as a corridor run. Note that a minor hack
// is inserted to make the angled corridor entry (with one side blocked near and
// the other side blocked further away from the runner) work correctly. The
// runner moves diagonally, but then saves the previous direction as being
// straight into the gap. Otherwise, the tail end of the other entry would be
// perceived as an alternative on the next move.
//
//         #.#
//        ##.##
//        .@...
//        ##.##
//         #.#
//
// Likewise, a run along a wall, and then into a doorway (two runs) will work
// correctly. A single run rightwards from @ will stop at 1. Another run right
// and down will enter the corridor and make the corner, stopping at the 2.
//
//      #@    1
//      ########### ######
//      2     #
//      #############
//      #
//
// After any move, the function playerAreaAffect is called to determine the new
// surroundings, and the direction of subsequent moves. It takes a location (at
// which the runner has just arrived) and the previous direction (from which the
// runner is considered to have come). Moving one square in some direction places
// you adjacent to three or five new squares (for straight and diagonal moves) to
// which you were not previously adjacent.
//
//     ...!   ...        EG Moving from 1 to 2.
//     .12!   .1.!      . means previously adjacent
//     ...!   ..2!      ! means newly adjacent
//                 !!!
//
// You STOP if you can't even make the move in the chosen direction. You STOP if
// any of the new squares are interesting in any way: usually containing monsters
// or treasure. You STOP if any of the newly adjacent squares seem to be open,
// and you are also looking for a break on that side. (i.e. find_openarea AND
// find_break) You STOP if any of the newly adjacent squares do NOT seem to be
// open and you are in an open area, and that side was previously entirely open.
//
// Corners: If you are not in the open (i.e. you are in a corridor) and there is
// only one way to go in the new squares, then turn in that direction. If there
// are more than two new ways to go, STOP. If there are two ways to go, and those
// ways are separated by a square which does not seem to be open, then STOP.
//
// Otherwise, we have a potential corner. There are two new open squares, which
// are also adjacent. One of the new squares is diagonally located, the other is
// straight on (as in the diagram). We consider two more squares further out
// (marked below as ?).
//
//        .X
//       @.?
//        #?
//
// If they are both seen to be closed, then it is seen that no benefit is gained
// from moving straight. It is a known corner. To cut the corner, go diagonally,
// otherwise go straight, but pretend you stepped diagonally into that next
// location for a full view next time. Conversely, if one of the ? squares is not
// seen to be closed, then there is a potential choice. We check to see whether
// it is a potential corner or an intersection/room entrance. If the square two
// spaces straight ahead, and the space marked with 'X' are both blank, then it
// is a potential corner and enter if run_examine_corners is set, otherwise must stop
// because it is not a corner.

// The cycle lists the directions in anticlockwise order, for over two complete
// cycles. The chome array maps a direction on to its position in the cycle. -CJS-

static int cycle[] = {1, 2, 3, 6, 9, 8, 7, 4, 1, 2, 3, 6, 9, 8, 7, 4, 1};
static int chome[] = {-1, 8, 9, 10, 7, -1, 11, 6, 5, 4};
static bool find_openarea, find_breakright, find_breakleft;
static int find_prevdir;
static int find_direction; // Keep a record of which way we are going.

// Do we see a wall? Used in running. -CJS-
static bool playerCanSeeDungeonWall(int dir, int y, int x) {
    // check to see if movement there possible
    if (!playerMovePosition(dir, y, x)) {
        return true;
    }

    char c = caveGetTileSymbol(Coord_t{y, x});

    return c == '#' || c == '%';
}

// Do we see anything? Used in running. -CJS-
static bool playerSeeNothing(int dir, int y, int x) {
    // check to see if movement there possible
    return playerMovePosition(dir, y, x) && caveGetTileSymbol(Coord_t{y, x}) == ' ';
}

static void findRunningBreak(int dir, int row, int col) {
    bool deepLeft = false;
    bool deepRight = false;
    bool shortLeft = false;
    bool shortRight = false;

    int cycleIndex = chome[dir];

    if (playerCanSeeDungeonWall(cycle[cycleIndex + 1], py.row, py.col)) {
        find_breakleft = true;
        shortLeft = true;
    } else if (playerCanSeeDungeonWall(cycle[cycleIndex + 1], row, col)) {
        find_breakleft = true;
        deepLeft = true;
    }

    if (playerCanSeeDungeonWall(cycle[cycleIndex - 1], py.row, py.col)) {
        find_breakright = true;
        shortRight = true;
    } else if (playerCanSeeDungeonWall(cycle[cycleIndex - 1], row, col)) {
        find_breakright = true;
        deepRight = true;
    }

    if (find_breakleft && find_breakright) {
        find_openarea = false;

        // a hack to allow angled corridor entry
        if ((dir & 1) != 0) {
            if (deepLeft && !deepRight) {
                find_prevdir = cycle[cycleIndex - 1];
            } else if (deepRight && !deepLeft) {
                find_prevdir = cycle[cycleIndex + 1];
            }
        } else if (playerCanSeeDungeonWall(cycle[cycleIndex], row, col)) {
            // else if there is a wall two spaces ahead and seem to be in a
            // corridor, then force a turn into the side corridor, must
            // be moving straight into a corridor here

            if (shortLeft && !shortRight) {
                find_prevdir = cycle[cycleIndex - 2];
            } else if (shortRight && !shortLeft) {
                find_prevdir = cycle[cycleIndex + 2];
            }
        }
    } else {
        find_openarea = true;
    }
}

void playerFindInitialize(int direction) {
    int y = py.row;
    int x = py.col;

    if (!playerMovePosition(direction, y, x)) {
        py.running_tracker = 0;
    } else {
        py.running_tracker = 1;

        find_direction = direction;
        find_prevdir = direction;

        find_breakright = false;
        find_breakleft = false;

        if (py.flags.blind < 1) {
            findRunningBreak(direction, y, x);
        }
    }

    // We must erase the player symbol '@' here, because sub3_move_light()
    // does not erase the previous location of the player when in find mode
    // and when `run_print_self` is false.  The player symbol is not draw at all
    // in this case while moving, so the only problem is on the first turn
    // of find mode, when the initial position of the character must be erased.
    // Hence we must do the erasure here.
    if (!py.temporary_light_only && !config::options::run_print_self) {
        panelPutTile(caveGetTileSymbol(Coord_t{py.row, py.col}), Coord_t{py.row, py.col});
    }

    playerMove(direction, true);

    if (py.running_tracker == 0) {
        game.command_count = 0;
    }
}

void playerRunAndFind() {
    uint8_t tracker = py.running_tracker;

    py.running_tracker++;

    // prevent infinite loops in find mode, will stop after moving 100 times
    if (tracker > 100) {
        printMessage("You stop running to catch your breath.");
        playerEndRunning();
        return;
    }

    playerMove(find_direction, true);
}

// Switch off the run flag - and get the light correct. -CJS-
void playerEndRunning() {
    if (py.running_tracker == 0) {
        return;
    }

    py.running_tracker = 0;

    dungeonMoveCharacterLight(Coord_t{py.row, py.col}, Coord_t{py.row, py.col});
}

static bool areaAffectStopLookingAtSquares(int i, int dir, int new_dir, int y, int x, int &check_dir, int &option1, int &option2) {
    Tile_t const &tile = dg.floor[y][x];

    // Default: Square unseen. Treat as open.
    bool invisible = true;

    if (py.carrying_light || tile.temporary_light || tile.permanent_light || tile.field_mark) {
        if (tile.treasure_id != 0) {
            int tileID = treasure_list[tile.treasure_id].category_id;

            if (tileID != TV_INVIS_TRAP && tileID != TV_SECRET_DOOR && (tileID != TV_OPEN_DOOR || !config::options::run_ignore_doors)) {
                playerEndRunning();
                return true;
            }
        }

        // Also Creatures
        // The monster should be visible since monsterUpdateVisibility() checks
        // for the special case of being in find mode
        if (tile.creature_id > 1 && monsters[tile.creature_id].lit) {
            playerEndRunning();
            return true;
        }

        invisible = false;
    }

    if (tile.feature_id <= MAX_OPEN_SPACE || invisible) {
        if (find_openarea) {
            // Have we found a break?
            if (i < 0) {
                if (find_breakright) {
                    playerEndRunning();
                    return true;
                }
            } else if (i > 0) {
                if (find_breakleft) {
                    playerEndRunning();
                    return true;
                }
            }
        } else if (option1 == 0) {
            // The first new direction.
            option1 = new_dir;
        } else if (option2 != 0) {
            // Three new directions. STOP.
            playerEndRunning();
            return true;
        } else if (option1 != cycle[chome[dir] + i - 1]) {
            // If not adjacent to prev, STOP
            playerEndRunning();
            return true;
        } else {
            // Two adjacent choices. Make option2 the diagonal, and
            // remember the other diagonal adjacent to the first option.
            if ((new_dir & 1) == 1) {
                check_dir = cycle[chome[dir] + i - 2];
                option2 = new_dir;
            } else {
                check_dir = cycle[chome[dir] + i + 1];
                option2 = option1;
                option1 = new_dir;
            }
        }
    } else if (find_openarea) {
        // We see an obstacle. In open area, STOP if on a side previously open.
        if (i < 0) {
            if (find_breakleft) {
                playerEndRunning();
                return true;
            }
            find_breakright = true;
        } else if (i > 0) {
            if (find_breakright) {
                playerEndRunning();
                return true;
            }
            find_breakleft = true;
        }
    }

    return false;
}

// Determine the next direction for a run, or if we should stop. -CJS-
void playerAreaAffect(int direction, int y, int x) {
    if (py.flags.blind >= 1) {
        return;
    }

    int check_dir = 0;
    int option = 0;
    int option2 = 0;

    direction = find_prevdir;

    int max = (direction & 1) + 1;

    // Look at every newly adjacent square.
    for (int i = -max; i <= max; i++) {
        int new_dir = cycle[chome[direction] + i];

        int row = y;
        int col = x;

        // Objects player can see (Including doors?) cause a stop.
        if (playerMovePosition(new_dir, row, col)) {
            areaAffectStopLookingAtSquares(i, direction, new_dir, row, col, check_dir, option, option2);
        }
    }

    if (find_openarea) {
        return;
    }

    // choose a direction.

    if (option2 == 0 || (config::options::run_examine_corners && !config::options::run_cut_corners)) {
        // There is only one option, or if two, then we always examine
        // potential corners and never cur known corners, so you step
        // into the straight option.
        if (option != 0) {
            find_direction = option;
        }

        if (option2 == 0) {
            find_prevdir = option;
        } else {
            find_prevdir = option2;
        }

        return;
    }

    // Two options!
    int row = y;
    int col = x;

    (void) playerMovePosition(option, row, col);

    if (!playerCanSeeDungeonWall(option, row, col) || !playerCanSeeDungeonWall(check_dir, row, col)) {
        // Don't see that it is closed off.  This could be a
        // potential corner or an intersection.
        if (config::options::run_examine_corners && playerSeeNothing(option, row, col) && playerSeeNothing(option2, row, col)) {
            // Can not see anything ahead and in the direction we are
            // turning, assume that it is a potential corner.
            find_direction = option;
            find_prevdir = option2;
        } else {
            // STOP: we are next to an intersection or a room
            playerEndRunning();
        }
    } else if (config::options::run_cut_corners) {
        // This corner is seen to be enclosed; we cut the corner.
        find_direction = option2;
        find_prevdir = option2;
    } else {
        // This corner is seen to be enclosed, and we deliberately
        // go the long way.
        find_direction = option;
        find_prevdir = option2;
    }
}
