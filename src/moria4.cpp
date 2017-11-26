// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Misc code, mainly to handle player commands

#include "headers.h"
#include "externs.h"

static bool lookRay(int y, int from, int to);
static bool lookSee(int x, int y, bool &transparent);

/*
  An enhanced look, with peripheral vision. Looking all 8 -CJS- directions will
  see everything which ought to be visible. Can specify direction 5, which looks
  in all directions.

  For the purpose of hindering vision, each place is regarded as a diamond just
  touching its four immediate neighbours. A diamond is opaque if it is a wall,
  or shut door, or something like that. A place is visible if any part of its
  diamond is visible: i.e. there is a line from the view point to part of the
  diamond which does not pass through any opaque diamonds.

  Consider the following situation:

    @....                         X   X   X   X   X
    .##..                        / \ / \ / \ / \ / \
    .....                       X @ X . X . X 1 X . X
                                 \ / \ / \ / \ / \ /
                                  X   X   X   X   X
          Expanded view, with    / \ / \ / \ / \ / \
          diamonds inscribed    X . X # X # X 2 X . X
          about each point,      \ / \ / \ / \ / \ /
          and some locations      X   X   X   X   X
          numbered.              / \ / \ / \ / \ / \
                                X . X . X . X 3 X 4 X
                                 \ / \ / \ / \ / \ /
                                  X   X   X   X   X

       - Location 1 is fully visible.
       - Location 2 is visible, even though partially obscured.
       - Location 3 is invisible, but if either # were
         transparent, it would be visible.
       - Location 4 is completely obscured by a single #.

  The function which does the work is lookRay(). It sets up its own co-ordinate
  frame (global variables map back to the dungeon frame) and looks for
  everything between two angles specified from a central line. It is recursive,
  and each call looks at stuff visible along a line parallel to the center line,
  and a set distance away from it. A diagonal look uses more extreme peripheral
  vision from the closest horizontal and vertical directions; horizontal or
  vertical looks take a call for each side of the central line.

  Globally accessed variables: los_num_places_seen counts the number of places where
  something is seen. los_rocks_and_objects indicates a look for rock or objects.

  The others map coords in the ray frame to dungeon coords.

  dungeon y = py.row   + los_fyx * (ray x)  + los_fyy * (ray y)
  dungeon x = py.col   + los_fxx * (ray x)  + los_fxy * (ray y)
*/
static int los_fxx, los_fxy, los_fyx, los_fyy;
static int los_num_places_seen;
static bool los_hack_no_query;
static int los_rocks_and_objects;

// Intended to be indexed by dir/2, since is only
// relevant to horizontal or vertical directions.
static int los_dir_set_fxy[] = {0, 1, 0, 0, -1};
static int los_dir_set_fxx[] = {0, 0, -1, 1, 0};
static int los_dir_set_fyy[] = {0, 0, 1, -1, 0};
static int los_dir_set_fyx[] = {0, 1, 0, 0, -1};

// Map diagonal-dir/2 to a normal-dir/2.
static int los_map_diagonals1[] = {1, 3, 0, 2, 4};
static int los_map_diagonals2[] = {2, 1, 0, 4, 3};

#define GRADF 10000 // Any sufficiently big number will do

// Look at what we can see. This is a free move.
//
// Prompts for a direction, and then looks at every object in turn within a cone of
// vision in that direction. For each object, the cursor is moved over the object,
// a description is given, and we wait for the user to type something. Typing
// ESCAPE will abort the entire look.
//
// Looks first at real objects and monsters, and looks at rock types only after all
// other things have been seen.  Only looks at rock types if the config.highlight_seams
// option is set.
void look() {
    if (py.flags.blind > 0) {
        printMessage("You can't see a damn thing!");
        return;
    }

    if (py.flags.image > 0) {
        printMessage("You can't believe what you are seeing! It's like a dream!");
        return;
    }

    int dir;
    if (!getAllDirections("Look which direction?", dir)) {
        return;
    }

    los_num_places_seen = 0;
    los_rocks_and_objects = 0;

    // Have to set this up for the lookSee
    los_hack_no_query = false;

    bool dummy;
    if (lookSee(0, 0, dummy)) {
        return;
    }

    bool abort;
    do {
        abort = false;
        if (dir == 5) {
            for (int i = 1; i <= 4; i++) {
                los_fxx = los_dir_set_fxx[i];
                los_fyx = los_dir_set_fyx[i];
                los_fxy = los_dir_set_fxy[i];
                los_fyy = los_dir_set_fyy[i];
                if (lookRay(0, 2 * GRADF - 1, 1)) {
                    abort = true;
                    break;
                }
                los_fxy = -los_fxy;
                los_fyy = -los_fyy;
                if (lookRay(0, 2 * GRADF, 2)) {
                    abort = true;
                    break;
                }
            }
        } else if ((dir & 1) == 0) {
            // Straight directions

            int i = dir >> 1;
            los_fxx = los_dir_set_fxx[i];
            los_fyx = los_dir_set_fyx[i];
            los_fxy = los_dir_set_fxy[i];
            los_fyy = los_dir_set_fyy[i];
            if (lookRay(0, GRADF, 1)) {
                abort = true;
            } else {
                los_fxy = -los_fxy;
                los_fyy = -los_fyy;
                abort = lookRay(0, GRADF, 2);
            }
        } else {
            int i = los_map_diagonals1[dir >> 1];
            los_fxx = los_dir_set_fxx[i];
            los_fyx = los_dir_set_fyx[i];
            los_fxy = -los_dir_set_fxy[i];
            los_fyy = -los_dir_set_fyy[i];
            if (lookRay(1, 2 * GRADF, GRADF)) {
                abort = true;
            } else {
                i = los_map_diagonals2[dir >> 1];
                los_fxx = los_dir_set_fxx[i];
                los_fyx = los_dir_set_fyx[i];
                los_fxy = los_dir_set_fxy[i];
                los_fyy = los_dir_set_fyy[i];
                abort = lookRay(1, 2 * GRADF - 1, GRADF);
            }
        }
    } while (!abort && config.highlight_seams && (++los_rocks_and_objects < 2));

    if (abort) {
        printMessage("--Aborting look--");
        return;
    }

    if (los_num_places_seen != 0) {
        if (dir == 5) {
            printMessage("That's all you see.");
        } else {
            printMessage("That's all you see in that direction.");
        }
    } else if (dir == 5) {
        printMessage("You see nothing of interest.");
    } else {
        printMessage("You see nothing of interest in that direction.");
    }
}

// Look at everything within a cone of vision between two ray lines emanating
// from  the player, and y or more places away from the direct line of view.
// This is recursive.
//
// Rays are specified by gradients, y over x, multiplied by 2*GRADF. This is ONLY
// called with gradients between 2*GRADF (45 degrees) and 1 (almost horizontal).
//
//   (y axis)/ angle from
//     ^    /      ___ angle to
//     |   /   ___
//  ...|../.....___.................... parameter y (look at things in the
//     | /   ___                        cone, and on or above this line)
//     |/ ___
//     @-------------------->   direction in which you are looking. (x axis)
//     |
//     |
static bool lookRay(int y, int from, int to) {
    // from is the larger angle of the ray, since we scan towards the
    // center line. If from is smaller, then the ray does not exist.
    if (from <= to || y > MON_MAX_SIGHT) {
        return false;
    }

    // Find first visible location along this line. Minimum x such
    // that (2x-1)/x < from/GRADF <=> x > GRADF(2x-1)/from. This may
    // be called with y=0 whence x will be set to 0. Thus we need a
    // special fix.
    auto x = (int) ((int32_t) GRADF * (2 * y - 1) / from + 1);
    if (x <= 0) {
        x = 1;
    }

    // Find last visible location along this line.
    // Maximum x such that (2x+1)/x > to/GRADF <=> x < GRADF(2x+1)/to
    auto max_x = (int) (((int32_t) GRADF * (2 * y + 1) - 1) / to);
    if (max_x > MON_MAX_SIGHT) {
        max_x = MON_MAX_SIGHT;
    }
    if (max_x < x) {
        return false;
    }

    // los_hack_no_query is a HACK to prevent doubling up on direct lines of
    // sight. If 'to' is  greater than 1, we do not really look at
    // stuff along the direct line of sight, but we do have to see
    // what is opaque for the purposes of obscuring other objects.
    los_hack_no_query = (y == 0 && to > 1) || (y == x && from < GRADF * 2);

    bool transparent;

    if (lookSee(x, y, transparent)) {
        return true;
    }

    if (y == x) {
        los_hack_no_query = false;
    }

    if (transparent) {
        goto init_transparent;
    }

    while (true) {
        // Look down the window we've found.
        if (lookRay(y + 1, from, ((2 * y + 1) * (int32_t) GRADF / x))) {
            return true;
        }

        // Find the start of next window.
        do {
            if (x == max_x) {
                return false;
            }

            // See if this seals off the scan. (If y is zero, then it will.)
            from = ((2 * y - 1) * (int32_t) GRADF / x);

            if (from <= to) {
                return false;
            }

            x++;

            if (lookSee(x, y, transparent)) {
                return true;
            }
        } while (!transparent);

        init_transparent:

        // Find the end of this window of visibility.
        do {
            if (x == max_x) {
                // The window is trimmed by an earlier limit.
                return lookRay(y + 1, from, to);
            }

            x++;

            if (lookSee(x, y, transparent)) {
                return true;
            }
        } while (transparent);
    }
}

static bool lookSee(int x, int y, bool &transparent) {
    if (x < 0 || y < 0 || y > x) {
        obj_desc_t errorMessage = {'\0'};
        (void) sprintf(errorMessage, "Illegal call to lookSee(%d, %d)", x, y);
        printMessage(errorMessage);
    }

    const char *description = nullptr;
    if (x == 0 && y == 0) {
        description = "You are on";
    } else {
        description = "You see";
    }

    int j = py.col + los_fxx * x + los_fxy * y;
    y = py.row + los_fyx * x + los_fyy * y;
    x = j;

    if (!coordInsidePanel(Coord_t{y, x})) {
        transparent = false;
        return false;
    }

    const Tile_t &tile = dg.floor[y][x];
    transparent = tile.feature_id <= MAX_OPEN_SPACE;

    if (los_hack_no_query) {
        return false; // Don't look at a direct line of sight. A hack.
    }

    // This was uninitialized but the `query == ESCAPE` below was causing
    // a warning. Perhaps we can set it to `ESCAPE` here as default. -MRC-
    char query = ESCAPE;

    obj_desc_t msg = {'\0'};

    if (los_rocks_and_objects == 0 && tile.creature_id > 1 && monsters[tile.creature_id].lit) {
        j = monsters[tile.creature_id].creature_id;
        (void) sprintf(msg, "%s %s %s. [(r)ecall]", description, isVowel(creatures_list[j].name[0]) ? "an" : "a", creatures_list[j].name);
        description = "It is on";
        putStringClearToEOL(msg, Coord_t{0, 0});

        panelMoveCursor(Coord_t{y, x});
        query = getKeyInput();

        if (query == 'r' || query == 'R') {
            terminalSaveScreen();
            query = (char) memoryRecall(j);
            terminalRestoreScreen();
        }
    }

    if (tile.temporary_light || tile.permanent_light || tile.field_mark) {
        const char *wall_description;

        if (tile.treasure_id != 0) {
            if (treasure_list[tile.treasure_id].category_id == TV_SECRET_DOOR) {
                goto granite;
            }

            if (los_rocks_and_objects == 0 && treasure_list[tile.treasure_id].category_id != TV_INVIS_TRAP) {
                obj_desc_t obj_string = {'\0'};
                itemDescription(obj_string, treasure_list[tile.treasure_id], true);

                (void) sprintf(msg, "%s %s ---pause---", description, obj_string);
                description = "It is in";
                putStringClearToEOL(msg, Coord_t{0, 0});

                panelMoveCursor(Coord_t{y, x});
                query = getKeyInput();
            }
        }

        if (((los_rocks_and_objects != 0) || (msg[0] != 0)) && tile.feature_id >= MIN_CLOSED_SPACE) {
            switch (tile.feature_id) {
                case TILE_BOUNDARY_WALL:
                case TILE_GRANITE_WALL:
                granite:
                    // Granite is only interesting if it contains something.
                    if (msg[0] != 0) {
                        wall_description = "a granite wall";
                    } else {
                        wall_description = CNIL; // In case we jump here
                    }
                    break;
                case TILE_MAGMA_WALL:
                    wall_description = "some dark rock";
                    break;
                case TILE_QUARTZ_WALL:
                    wall_description = "a quartz vein";
                    break;
                default:
                    wall_description = CNIL;
                    break;
            }

            if (wall_description != nullptr) {
                (void) sprintf(msg, "%s %s ---pause---", description, wall_description);
                putStringClearToEOL(msg, Coord_t{0, 0});
                panelMoveCursor(Coord_t{y, x});
                query = getKeyInput();
            }
        }
    }

    if (msg[0] != 0) {
        los_num_places_seen++;
        if (query == ESCAPE) {
            return true;
        }
    }

    return false;
}

int getRandomDirection() {
    int dir;

    do {
        dir = randomNumber(9);
    } while (dir == 5);

    return dir;
}
