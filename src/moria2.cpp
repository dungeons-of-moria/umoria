// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Misc code, mainly handles player movement, inventory, etc

#include "headers.h"
#include "externs.h"

// Change a trap from invisible to visible -RAK-
// Note: Secret doors are handled here
void change_trap(int y, int x) {
    Inventory_t *t_ptr = &treasure_list[cave[y][x].tptr];

    if (t_ptr->tval == TV_INVIS_TRAP) {
        t_ptr->tval = TV_VIS_TRAP;
        lite_spot(y, x);
        return;
    }

    // change secret door to closed door
    if (t_ptr->tval == TV_SECRET_DOOR) {
        t_ptr->index = OBJ_CLOSED_DOOR;
        t_ptr->tval = object_list[OBJ_CLOSED_DOOR].tval;
        t_ptr->tchar = object_list[OBJ_CLOSED_DOOR].tchar;
        lite_spot(y, x);
    }
}

// Searches for hidden things. -RAK-
void search(int y, int x, int chance) {
    if (py.flags.confused > 0) {
        chance = chance / 10;
    }

    if (py.flags.blind > 0 || no_light()) {
        chance = chance / 10;
    }

    if (py.flags.image > 0) {
        chance = chance / 10;
    }

    for (int i = y - 1; i <= y + 1; i++) {
        for (int j = x - 1; j <= x + 1; j++) {
            // always in_bounds here
            if (randint(100) >= chance) {
                continue;
            }

            if (cave[i][j].tptr == 0) {
                continue;
            }

            // Search for hidden objects

            Inventory_t *t_ptr = &treasure_list[cave[i][j].tptr];

            if (t_ptr->tval == TV_INVIS_TRAP) {
                // Trap on floor?

                obj_desc_t description;
                objdes(description, t_ptr, true);

                obj_desc_t msg;
                (void) sprintf(msg, "You have found %s", description);
                msg_print(msg);

                change_trap(i, j);
                end_find();
            } else if (t_ptr->tval == TV_SECRET_DOOR) {
                // Secret door?

                msg_print("You have found a secret door.");

                change_trap(i, j);
                end_find();
            } else if (t_ptr->tval == TV_CHEST) {
                // Chest is trapped?

                // mask out the treasure bits
                if ((t_ptr->flags & CH_TRAPPED) > 1) {
                    if (!known2_p(t_ptr)) {
                        known2(t_ptr);
                        msg_print("You have discovered a trap on the chest!");
                    } else {
                        msg_print("The chest is trapped!");
                    }
                }
            }
        }
    }
}

// The running algorithm: -CJS-
//
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
// To initialize these conditions is the task of find_init. If moving from the
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
// After any move, the function area_affect is called to determine the new
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
static bool see_wall(int dir, int y, int x) {
    // check to see if movement there possible
    if (!mmove(dir, &y, &x)) {
        return true;
    }

    char c = loc_symbol(y, x);

    return c == '#' || c == '%';
}

// Do we see anything? Used in running. -CJS-
static bool see_nothing(int dir, int y, int x) {
    // check to see if movement there possible
    return mmove(dir, &y, &x) && loc_symbol(y, x) == ' ';
}

static void findRunningBreak(int dir, int row, int col) {
    bool deepLeft = false;
    bool deepRight = false;
    bool shortLeft = false;
    bool shortRight = false;

    int cycleIndex = chome[dir];

    if (see_wall(cycle[cycleIndex + 1], char_row, char_col)) {
        find_breakleft = true;
        shortLeft = true;
    } else if (see_wall(cycle[cycleIndex + 1], row, col)) {
        find_breakleft = true;
        deepLeft = true;
    }

    if (see_wall(cycle[cycleIndex - 1], char_row, char_col)) {
        find_breakright = true;
        shortRight = true;
    } else if (see_wall(cycle[cycleIndex - 1], row, col)) {
        find_breakright = true;
        deepRight = true;
    }

    if (find_breakleft && find_breakright) {
        find_openarea = false;

        // a hack to allow angled corridor entry
        if (dir & 1) {
            if (deepLeft && !deepRight) {
                find_prevdir = cycle[cycleIndex - 1];
            } else if (deepRight && !deepLeft) {
                find_prevdir = cycle[cycleIndex + 1];
            }
        } else if (see_wall(cycle[cycleIndex], row, col)) {
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

void find_init(int dir) {
    int row = char_row;
    int col = char_col;

    if (!mmove(dir, &row, &col)) {
        running_counter = 0;
    } else {
        running_counter = 1;

        find_direction = dir;
        find_prevdir = dir;

        find_breakright = false;
        find_breakleft = false;

        if (py.flags.blind < 1) {
            findRunningBreak(dir, row, col);
        }
    }

    // We must erase the player symbol '@' here, because sub3_move_light()
    // does not erase the previous location of the player when in find mode
    // and when run_print_self is false.  The player symbol is not draw at all
    // in this case while moving, so the only problem is on the first turn
    // of find mode, when the initial position of the character must be erased.
    // Hence we must do the erasure here.
    if (!light_flag && !run_print_self) {
        print(loc_symbol(char_row, char_col), char_row, char_col);
    }

    move_char(dir, true);

    if (running_counter == 0) {
        command_count = 0;
    }
}

void find_run() {
    // prevent infinite loops in find mode, will stop after moving 100 times
    if (running_counter++ > 100) {
        msg_print("You stop running to catch your breath.");
        end_find();
        return;
    }

    move_char(find_direction, true);
}

// Switch off the run flag - and get the light correct. -CJS-
void end_find() {
    if (!running_counter) {
        return;
    }

    running_counter = 0;

    move_light(char_row, char_col, char_row, char_col);
}

static bool areaAffectStopLookingAtSquares(int i, int dir, int newDir, int y, int x, int *check_dir, int *option, int *option2) {
    Cave_t *c_ptr = &cave[y][x];

    // Default: Square unseen. Treat as open.
    bool invisible = true;

    if (player_carrying_light || c_ptr->tl || c_ptr->pl || c_ptr->fm) {
        if (c_ptr->tptr != 0) {
            int tileID = treasure_list[c_ptr->tptr].tval;

            if (tileID != TV_INVIS_TRAP && tileID != TV_SECRET_DOOR && (tileID != TV_OPEN_DOOR || !run_ignore_doors)) {
                end_find();
                return true;
            }
        }

        // Also Creatures
        // The monster should be visible since update_mon() checks
        // for the special case of being in find mode
        if (c_ptr->cptr > 1 && monsters_list[c_ptr->cptr].ml) {
            end_find();
            return true;
        }

        invisible = false;
    }

    if (c_ptr->fval <= MAX_OPEN_SPACE || invisible) {
        if (find_openarea) {
            // Have we found a break?
            if (i < 0) {
                if (find_breakright) {
                    end_find();
                    return true;
                }
            } else if (i > 0) {
                if (find_breakleft) {
                    end_find();
                    return true;
                }
            }
        } else if (*option == 0) {
            // The first new direction.
            *option = newDir;
        } else if (*option2 != 0) {
            // Three new directions. STOP.
            end_find();
            return true;
        } else if (*option != cycle[chome[dir] + i - 1]) {
            // If not adjacent to prev, STOP
            end_find();
            return true;
        } else {
            // Two adjacent choices. Make option2 the diagonal, and
            // remember the other diagonal adjacent to the first option.
            if ((newDir & 1) == 1) {
                *check_dir = cycle[chome[dir] + i - 2];
                *option2 = newDir;
            } else {
                *check_dir = cycle[chome[dir] + i + 1];
                *option2 = *option;
                *option = newDir;
            }
        }
    } else if (find_openarea) {
        // We see an obstacle. In open area, STOP if on a side previously open.
        if (i < 0) {
            if (find_breakleft) {
                end_find();
                return true;
            }
            find_breakright = true;
        } else if (i > 0) {
            if (find_breakright) {
                end_find();
                return true;
            }
            find_breakleft = true;
        }
    }

    return false;
}

// Determine the next direction for a run, or if we should stop. -CJS-
void area_affect(int dir, int y, int x) {
    if (py.flags.blind >= 1) {
        return;
    }

    int check_dir = 0;
    int option = 0;
    int option2 = 0;

    dir = find_prevdir;

    int max = (dir & 1) + 1;

    // Look at every newly adjacent square.
    for (int i = -max; i <= max; i++) {
        int newdir = cycle[chome[dir] + i];

        int row = y;
        int col = x;

        // Objects player can see (Including doors?) cause a stop.
        if (mmove(newdir, &row, &col)) {
            areaAffectStopLookingAtSquares(i, dir, newdir, row, col, &check_dir, &option, &option2);
        }
    }

    if (find_openarea) {
        return;
    }

    // choose a direction.

    if (option2 == 0 || (run_examine_corners && !run_cut_corners)) {
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

    (void) mmove(option, &row, &col);

    if (!see_wall(option, row, col) || !see_wall(check_dir, row, col)) {
        // Don't see that it is closed off.  This could be a
        // potential corner or an intersection.
        if (run_examine_corners && see_nothing(option, row, col) && see_nothing(option2, row, col)) {
            // Can not see anything ahead and in the direction we are
            // turning, assume that it is a potential corner.
            find_direction = option;
            find_prevdir = option2;
        } else {
            // STOP: we are next to an intersection or a room
            end_find();
        }
    } else if (run_cut_corners) {
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

// AC gets worse -RAK-
// Note: This routine affects magical AC bonuses so
// that stores can detect the damage.
int minus_ac(uint32_t typ_dam) {
    int itemsCount = 0;
    int items[6];

    if (inventory[INVEN_BODY].tval != TV_NOTHING) {
        items[itemsCount] = INVEN_BODY;
        itemsCount++;
    }

    if (inventory[INVEN_ARM].tval != TV_NOTHING) {
        items[itemsCount] = INVEN_ARM;
        itemsCount++;
    }

    if (inventory[INVEN_OUTER].tval != TV_NOTHING) {
        items[itemsCount] = INVEN_OUTER;
        itemsCount++;
    }

    if (inventory[INVEN_HANDS].tval != TV_NOTHING) {
        items[itemsCount] = INVEN_HANDS;
        itemsCount++;
    }

    if (inventory[INVEN_HEAD].tval != TV_NOTHING) {
        items[itemsCount] = INVEN_HEAD;
        itemsCount++;
    }

    // also affect boots
    if (inventory[INVEN_FEET].tval != TV_NOTHING) {
        items[itemsCount] = INVEN_FEET;
        itemsCount++;
    }

    bool minus = false;

    if (itemsCount == 0) {
        return minus;
    }

    int itemID = items[randint(itemsCount) - 1];

    obj_desc_t description, msg;

    if (inventory[itemID].flags & typ_dam) {
        minus = true;

        objdes(description, &inventory[itemID], false);
        (void) sprintf(msg, "Your %s resists damage!", description);
        msg_print(msg);
    } else if (inventory[itemID].ac + inventory[itemID].toac > 0) {
        minus = true;

        objdes(description, &inventory[itemID], false);
        (void) sprintf(msg, "Your %s is damaged!", description);
        msg_print(msg);

        inventory[itemID].toac--;
        calc_bonuses();
    }

    return minus;
}

// Corrode the unsuspecting person's armor -RAK-
void corrode_gas(const char *kb_str) {
    if (!minus_ac((uint32_t) TR_RES_ACID)) {
        take_hit(randint(8), kb_str);
    }

    if (inven_damage(set_corrodes, 5) > 0) {
        msg_print("There is an acrid smell coming from your pack.");
    }
}

// Poison gas the idiot. -RAK-
void poison_gas(int dam, const char *kb_str) {
    take_hit(dam, kb_str);

    py.flags.poisoned += 12 + randint(dam);
}

// Burn the fool up. -RAK-
void fire_dam(int dam, const char *kb_str) {
    if (py.flags.fire_resist) {
        dam = dam / 3;
    }

    if (py.flags.resist_heat > 0) {
        dam = dam / 3;
    }

    take_hit(dam, kb_str);

    if (inven_damage(set_flammable, 3) > 0) {
        msg_print("There is smoke coming from your pack!");
    }
}

// Freeze him to death. -RAK-
void cold_dam(int dam, const char *kb_str) {
    if (py.flags.cold_resist) {
        dam = dam / 3;
    }

    if (py.flags.resist_cold > 0) {
        dam = dam / 3;
    }

    take_hit(dam, kb_str);

    if (inven_damage(set_frost_destroy, 5) > 0) {
        msg_print("Something shatters inside your pack!");
    }
}

// Lightning bolt the sucker away. -RAK-
void light_dam(int dam, const char *kb_str) {
    if (py.flags.lght_resist) {
        dam = dam / 3;
    }

    take_hit(dam, kb_str);

    if (inven_damage(set_lightning_destroy, 3) > 0) {
        msg_print("There are sparks coming from your pack!");
    }
}

// Throw acid on the hapless victim -RAK-
void acid_dam(int dam, const char *kb_str) {
    int flag = 0;

    if (minus_ac((uint32_t) TR_RES_ACID)) {
        flag = 1;
    }

    if (py.flags.acid_resist) {
        flag += 2;
    }

    take_hit(dam / (flag + 1), kb_str);

    if (inven_damage(set_acid_affect, 3) > 0) {
        msg_print("There is an acrid smell coming from your pack!");
    }
}
