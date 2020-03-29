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
static bool playerCanSeeDungeonWall(int dir, Coord_t coord) {
    // check to see if movement there possible
    if (!playerMovePosition(dir, coord)) {
        return true;
    }

    char c = caveGetTileSymbol(coord);

    return c == '#' || c == '%';
}

// Do we see anything? Used in running. -CJS-
static bool playerSeeNothing(int dir, Coord_t coord) {
    // check to see if movement there possible
    return playerMovePosition(dir, coord) && caveGetTileSymbol(coord) == ' ';
}

static void findRunningBreak(int dir, Coord_t coord) {
    bool deep_left = false;
    bool deep_right = false;
    bool short_left = false;
    bool short_right = false;

    int cycle_index = chome[dir];

    if (playerCanSeeDungeonWall(cycle[cycle_index + 1], py.pos)) {
        find_breakleft = true;
        short_left = true;
    } else if (playerCanSeeDungeonWall(cycle[cycle_index + 1], coord)) {
        find_breakleft = true;
        deep_left = true;
    }

    if (playerCanSeeDungeonWall(cycle[cycle_index - 1], py.pos)) {
        find_breakright = true;
        short_right = true;
    } else if (playerCanSeeDungeonWall(cycle[cycle_index - 1], coord)) {
        find_breakright = true;
        deep_right = true;
    }

    if (find_breakleft && find_breakright) {
        find_openarea = false;

        // a hack to allow angled corridor entry
        if ((dir & 1) != 0) {
            if (deep_left && !deep_right) {
                find_prevdir = cycle[cycle_index - 1];
            } else if (deep_right && !deep_left) {
                find_prevdir = cycle[cycle_index + 1];
            }
        } else if (playerCanSeeDungeonWall(cycle[cycle_index], coord)) {
            // else if there is a wall two spaces ahead and seem to be in a
            // corridor, then force a turn into the side corridor, must
            // be moving straight into a corridor here

            if (short_left && !short_right) {
                find_prevdir = cycle[cycle_index - 2];
            } else if (short_right && !short_left) {
                find_prevdir = cycle[cycle_index + 2];
            }
        }
    } else {
        find_openarea = true;
    }
}

void playerFindInitialize(int direction) {
    Coord_t coord = py.pos;

    if (!playerMovePosition(direction, coord)) {
        py.running_tracker = 0;
    } else {
        py.running_tracker = 1;

        find_direction = direction;
        find_prevdir = direction;

        find_breakright = false;
        find_breakleft = false;

        if (py.flags.blind < 1) {
            findRunningBreak(direction, coord);
        }
    }

    // We must erase the player symbol '@' here, because sub3_move_light()
    // does not erase the previous location of the player when in find mode
    // and when `run_print_self` is false.  The player symbol is not draw at all
    // in this case while moving, so the only problem is on the first turn
    // of find mode, when the initial position of the character must be erased.
    // Hence we must do the erasure here.
    if (!py.temporary_light_only && !config::options::run_print_self) {
        panelPutTile(caveGetTileSymbol(py.pos), py.pos);
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

    dungeonMoveCharacterLight(py.pos, py.pos);
}

static bool areaAffectStopLookingAtSquares(int i, int dir, int new_dir, Coord_t coord, int &check_dir, int &dir_a, int &dir_b) {
    Tile_t const &tile = dg.floor[coord.y][coord.x];

    // Default: Square unseen. Treat as open.
    bool invisible = true;

    if (py.carrying_light || tile.temporary_light || tile.permanent_light || tile.field_mark) {
        if (tile.treasure_id != 0) {
            int tile_id = game.treasure.list[tile.treasure_id].category_id;

            if (tile_id != TV_INVIS_TRAP && tile_id != TV_SECRET_DOOR && (tile_id != TV_OPEN_DOOR || !config::options::run_ignore_doors)) {
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
        } else if (dir_a == 0) {
            // The first new direction.
            dir_a = new_dir;
        } else if (dir_b != 0) {
            // Three new directions. STOP.
            playerEndRunning();
            return true;
        } else if (dir_a != cycle[chome[dir] + i - 1]) {
            // If not adjacent to prev, STOP
            playerEndRunning();
            return true;
        } else {
            // Two adjacent choices. Make dir_b the diagonal, and
            // remember the other diagonal adjacent to the first option.
            if ((new_dir & 1) == 1) {
                check_dir = cycle[chome[dir] + i - 2];
                dir_b = new_dir;
            } else {
                check_dir = cycle[chome[dir] + i + 1];
                dir_b = dir_a;
                dir_a = new_dir;
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
void playerAreaAffect(int direction, Coord_t coord) {
    if (py.flags.blind >= 1) {
        return;
    }

    int check_dir = 0;
    int dir_a = 0;
    int dir_b = 0;

    direction = find_prevdir;

    int max = (direction & 1) + 1;

    Coord_t spot = Coord_t{0, 0};

    // Look at every newly adjacent square.
    for (int i = -max; i <= max; i++) {
        int new_dir = cycle[chome[direction] + i];

        spot.y = coord.y;
        spot.x = coord.x;

        // Objects player can see (Including doors?) cause a stop.
        if (playerMovePosition(new_dir, spot)) {
            areaAffectStopLookingAtSquares(i, direction, new_dir, spot, check_dir, dir_a, dir_b);
        }
    }

    if (find_openarea) {
        return;
    }

    // choose a direction.

    if (dir_b == 0 || (config::options::run_examine_corners && !config::options::run_cut_corners)) {
        // There is only one option, or if two, then we always examine
        // potential corners and never cur known corners, so you step
        // into the straight option.
        if (dir_a != 0) {
            find_direction = dir_a;
        }

        if (dir_b == 0) {
            find_prevdir = dir_a;
        } else {
            find_prevdir = dir_b;
        }

        return;
    }

    // Two options!

    Coord_t location = Coord_t{coord.y, coord.x};
    (void) playerMovePosition(dir_a, location);

    if (!playerCanSeeDungeonWall(dir_a, location) || !playerCanSeeDungeonWall(check_dir, location)) {
        // Don't see that it is closed off.  This could be a
        // potential corner or an intersection.
        if (config::options::run_examine_corners && playerSeeNothing(dir_a, location) && playerSeeNothing(dir_b, location)) {
            // Can not see anything ahead and in the direction we are
            // turning, assume that it is a potential corner.
            find_direction = dir_a;
            find_prevdir = dir_b;
        } else {
            // STOP: we are next to an intersection or a room
            playerEndRunning();
        }
    } else if (config::options::run_cut_corners) {
        // This corner is seen to be enclosed; we cut the corner.
        find_direction = dir_b;
        find_prevdir = dir_b;
    } else {
        // This corner is seen to be enclosed, and we deliberately
        // go the long way.
        find_direction = dir_a;
        find_prevdir = dir_b;
    }
}
