// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Misc code, mainly to handle player commands

#include "headers.h"
#include "externs.h"

static bool look_ray(int y, int from, int to);
static bool look_see(int x, int y, bool *transparent);

// Don't let the player tunnel somewhere illegal, this is necessary to
// prevent the player from getting a free attack by trying to tunnel
// somewhere where it has no effect.
static bool playerCanTunnel(int treasure_id, int tile_id) {
    if (tile_id < MIN_CAVE_WALL && (treasure_id == 0 || (treasure_list[treasure_id].tval != TV_RUBBLE && treasure_list[treasure_id].tval != TV_SECRET_DOOR))) {
        player_free_turn = true;

        if (treasure_id == 0) {
            printMessage("Tunnel through what?  Empty air?!?");
        } else {
            printMessage("You can't tunnel through that.");
        }

        return false;
    }

    return true;
}

// Compute the digging ability of player; based on strength, and type of tool used
static int playerDiggingAbility(Inventory_t *weapon) {
    int diggingAbility = py.stats.use_stat[A_STR];

    if (weapon->flags & TR_TUNNEL) {
        diggingAbility += 25 + weapon->p1 * 50;
    } else {
        diggingAbility += (weapon->damage[0] * weapon->damage[1]) + weapon->tohit + weapon->todam;

        // divide by two so that digging without shovel isn't too easy
        diggingAbility >>= 1;
    }

    // If this weapon is too heavy for the player to wield properly,
    // then also make it harder to dig with it.
    if (weapon_is_heavy) {
        diggingAbility += (py.stats.use_stat[A_STR] * 15) - weapon->weight;

        if (diggingAbility < 0) {
            diggingAbility = 0;
        }
    }

    return diggingAbility;
}

static void dungeonDigGraniteWall(int y, int x, int digging_ability) {
    int i = randomNumber(1200) + 80;

    if (dungeonTunnelWall(y, x, digging_ability, i)) {
        printMessage("You have finished the tunnel.");
    } else {
        printMessageNoCommandInterrupt("You tunnel into the granite wall.");
    }
}

static void dungeonDigMagmaWall(int y, int x, int digging_ability) {
    int i = randomNumber(600) + 10;

    if (dungeonTunnelWall(y, x, digging_ability, i)) {
        printMessage("You have finished the tunnel.");
    } else {
        printMessageNoCommandInterrupt("You tunnel into the magma intrusion.");
    }
}

static void dungeonDigQuartzWall(int y, int x, int digging_ability) {
    int i = randomNumber(400) + 10;

    if (dungeonTunnelWall(y, x, digging_ability, i)) {
        printMessage("You have finished the tunnel.");
    } else {
        printMessageNoCommandInterrupt("You tunnel into the quartz vein.");
    }
}

static void dungeonDigRubble(int y, int x, int digging_ability) {
    if (digging_ability > randomNumber(180)) {
        (void) dungeonDeleteObject(y, x);
        printMessage("You have removed the rubble.");

        if (randomNumber(10) == 1) {
            dungeonPlaceRandomObjectAt(y, x, false);

            if (caveTileVisible(y, x)) {
                printMessage("You have found something!");
            }
        }

        dungeonLiteSpot(y, x);
    } else {
        printMessageNoCommandInterrupt("You dig in the rubble.");
    }
}

// Dig regular walls; Granite, magma intrusion, quartz vein
// Don't forget the boundary walls, made of titanium (255)
// Return `true` if a wall was dug at
static bool dungeonDigAtLocation(int y, int x, uint8_t wall_type, int digging_ability) {
    switch (wall_type) {
        case GRANITE_WALL:
            dungeonDigGraniteWall(y, x, digging_ability);
            break;
        case MAGMA_WALL:
            dungeonDigMagmaWall(y, x, digging_ability);
            break;
        case QUARTZ_WALL:
            dungeonDigQuartzWall(y, x, digging_ability);
            break;
        case BOUNDARY_WALL:
            printMessage("This seems to be permanent rock.");
            break;
        default:
            return false;
    }
    return true;
}

// Tunnels through rubble and walls -RAK-
// Must take into account: secret doors, special tools
void tunnel(int direction) {
    // Confused?                    75% random movement
    if (py.flags.confused > 0 && randomNumber(4) > 1) {
        direction = randomNumber(9);
    }

    int y = char_row;
    int x = char_col;
    (void) playerMovePosition(direction, &y, &x);

    Cave_t *c_ptr = &cave[y][x];
    Inventory_t *i_ptr = &inventory[INVEN_WIELD];

    if (!playerCanTunnel(c_ptr->tptr, c_ptr->fval)) {
        return;
    }

    if (c_ptr->cptr > 1) {
        objectBlockedByMonster(c_ptr->cptr);
        playerAttackPosition(y, x);
        return;
    }

    if (i_ptr->tval != TV_NOTHING) {
        int diggingAbility = playerDiggingAbility(i_ptr);

        if (!dungeonDigAtLocation(y, x, c_ptr->fval, diggingAbility)) {
            // Is there an object in the way?  (Rubble and secret doors)
            if (c_ptr->tptr != 0) {
                if (treasure_list[c_ptr->tptr].tval == TV_RUBBLE) {
                    dungeonDigRubble(y, x, diggingAbility);
                } else if (treasure_list[c_ptr->tptr].tval == TV_SECRET_DOOR) {
                    // Found secret door!
                    printMessageNoCommandInterrupt("You tunnel into the granite wall.");
                    dungeonSearch(char_row, char_col, py.misc.srh);
                } else {
                    abort();
                }
            } else {
                abort();
            }
        }

        return;
    }

    printMessage("You dig with your hands, making no progress.");
}

static int playerTrapDisarmAbility() {
    int ability = py.misc.disarm;
    ability += 2;
    ability *= playerDisarmAdjustment();
    ability += playerStatAdjustmentWisdomIntelligence(A_INT);
    ability += class_level_adj[py.misc.pclass][CLA_DISARM] * py.misc.lev / 3;

    if (py.flags.blind > 0 || playerNoLight()) {
        ability = ability / 10;
    }

    if (py.flags.confused > 0) {
        ability = ability / 10;
    }

    if (py.flags.image > 0) {
        ability = ability / 10;
    }

    return ability;
}

static void disarmFloorTrap(int y, int x, int tot, int level, int dir, int16_t p1) {
    int confused = py.flags.confused;

    if (tot + 100 - level > randomNumber(100)) {
        printMessage("You have disarmed the trap.");
        py.misc.exp += p1;
        (void) dungeonDeleteObject(y, x);

        // make sure we move onto the trap even if confused
        py.flags.confused = 0;
        playerMove(dir, false);
        py.flags.confused = (int16_t) confused;

        displayCharacterExperience();
        return;
    }

    // avoid randomNumber(0) call
    if (tot > 5 && randomNumber(tot) > 5) {
        printMessageNoCommandInterrupt("You failed to disarm the trap.");
        return;
    }

    printMessage("You set the trap off!");

    // make sure we move onto the trap even if confused
    py.flags.confused = 0;
    playerMove(dir, false);
    py.flags.confused += confused;
}

static void disarmChestTrap(int y, int x, int tot, Inventory_t *item) {
    if (!spellItemIdentified(item)) {
        player_free_turn = true;
        printMessage("I don't see a trap.");

        return;
    }

    if (item->flags & CH_TRAPPED) {
        int level = item->level;

        if ((tot - level) > randomNumber(100)) {
            item->flags &= ~CH_TRAPPED;

            if (item->flags & CH_LOCKED) {
                item->name2 = SN_LOCKED;
            } else {
                item->name2 = SN_DISARMED;
            }

            printMessage("You have disarmed the chest.");

            spellItemIdentifyAndRemoveRandomInscription(item);
            py.misc.exp += level;

            displayCharacterExperience();
        } else if ((tot > 5) && (randomNumber(tot) > 5)) {
            printMessageNoCommandInterrupt("You failed to disarm the chest.");
        } else {
            printMessage("You set a trap off!");
            spellItemIdentifyAndRemoveRandomInscription(item);
            chestTrap(y, x);
        }
        return;
    }

    printMessage("The chest was not trapped.");
    player_free_turn = true;
}

// Disarms a trap -RAK-
void disarm_trap() {
    int dir;
    if (!getDirectionWithMemory(CNIL, &dir)) {
        return;
    }

    int y = char_row;
    int x = char_col;
    (void) playerMovePosition(dir, &y, &x);

    Cave_t *c_ptr = &cave[y][x];

    bool no_disarm = false;

    if (c_ptr->cptr > 1 && c_ptr->tptr != 0 && (treasure_list[c_ptr->tptr].tval == TV_VIS_TRAP || treasure_list[c_ptr->tptr].tval == TV_CHEST)) {
        objectBlockedByMonster(c_ptr->cptr);
    } else if (c_ptr->tptr != 0) {
        int disarmAbility = playerTrapDisarmAbility();

        Inventory_t *t_ptr = &treasure_list[c_ptr->tptr];

        if (t_ptr->tval == TV_VIS_TRAP) {
            disarmFloorTrap(y, x, disarmAbility, t_ptr->level, dir, t_ptr->p1);
        } else if (t_ptr->tval == TV_CHEST) {
            disarmChestTrap(y, x, disarmAbility, t_ptr);
        } else {
            no_disarm = true;
        }
    } else {
        no_disarm = true;
    }

    if (no_disarm) {
        printMessage("I do not see anything to disarm there.");
        player_free_turn = true;
    }
}

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

  The function which does the work is look_ray. It sets up its own co-ordinate
  frame (global variables map back to the dungeon frame) and looks for
  everything between two angles specified from a central line. It is recursive,
  and each call looks at stuff visible along a line parallel to the center line,
  and a set distance away from it. A diagonal look uses more extreme peripheral
  vision from the closest horizontal and vertical directions; horizontal or
  vertical looks take a call for each side of the central line.

  Globally accessed variables: gl_nseen counts the number of places where
  something is seen. gl_rock indicates a look for rock or objects.

  The others map coords in the ray frame to dungeon coords.

  dungeon y = char_row   + gl_fyx * (ray x)  + gl_fyy * (ray y)
  dungeon x = char_col   + gl_fxx * (ray x)  + gl_fxy * (ray y)
*/
static int gl_fxx, gl_fxy, gl_fyx, gl_fyy;
static int gl_nseen;
static bool gl_noquery;
static int gl_rock;

// Intended to be indexed by dir/2, since is only
// relevant to horizontal or vertical directions.
static int set_fxy[] = {0, 1, 0, 0, -1};
static int set_fxx[] = {0, 0, -1, 1, 0};
static int set_fyy[] = {0, 0, 1, -1, 0};
static int set_fyx[] = {0, 1, 0, 0, -1};

// Map diagonal-dir/2 to a normal-dir/2.
static int map_diag1[] = {1, 3, 0, 2, 4};
static int map_diag2[] = {2, 1, 0, 4, 3};

#define GRADF 10000 // Any sufficiently big number will do

// Look at what we can see. This is a free move.
//
// Prompts for a direction, and then looks at every object in turn within a cone of
// vision in that direction. For each object, the cursor is moved over the object,
// a description is given, and we wait for the user to type something. Typing
// ESCAPE will abort the entire look.
//
// Looks first at real objects and monsters, and looks at rock types only after all
// other things have been seen.  Only looks at rock types if the highlight_seams
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
    if (!getAllDirections("Look which direction?", &dir)) {
        return;
    }

    gl_nseen = 0;
    gl_rock = 0;

    // Have to set this up for the look_see
    gl_noquery = false;

    bool dummy;
    if (look_see(0, 0, &dummy)) {
        return;
    }

    bool abort;
    do {
        abort = false;
        if (dir == 5) {
            for (int i = 1; i <= 4; i++) {
                gl_fxx = set_fxx[i];
                gl_fyx = set_fyx[i];
                gl_fxy = set_fxy[i];
                gl_fyy = set_fyy[i];
                if (look_ray(0, 2 * GRADF - 1, 1)) {
                    abort = true;
                    break;
                }
                gl_fxy = -gl_fxy;
                gl_fyy = -gl_fyy;
                if (look_ray(0, 2 * GRADF, 2)) {
                    abort = true;
                    break;
                }
            }
        } else if ((dir & 1) == 0) {
            // Straight directions

            int i = dir >> 1;
            gl_fxx = set_fxx[i];
            gl_fyx = set_fyx[i];
            gl_fxy = set_fxy[i];
            gl_fyy = set_fyy[i];
            if (look_ray(0, GRADF, 1)) {
                abort = true;
            } else {
                gl_fxy = -gl_fxy;
                gl_fyy = -gl_fyy;
                abort = look_ray(0, GRADF, 2);
            }
        } else {
            int i = map_diag1[dir >> 1];
            gl_fxx = set_fxx[i];
            gl_fyx = set_fyx[i];
            gl_fxy = -set_fxy[i];
            gl_fyy = -set_fyy[i];
            if (look_ray(1, 2 * GRADF, GRADF)) {
                abort = true;
            } else {
                i = map_diag2[dir >> 1];
                gl_fxx = set_fxx[i];
                gl_fyx = set_fyx[i];
                gl_fxy = set_fxy[i];
                gl_fyy = set_fyy[i];
                abort = look_ray(1, 2 * GRADF - 1, GRADF);
            }
        }
    } while (!abort && highlight_seams && (++gl_rock < 2));

    if (abort) {
        printMessage("--Aborting look--");
        return;
    }

    if (gl_nseen) {
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
static bool look_ray(int y, int from, int to) {
    // from is the larger angle of the ray, since we scan towards the
    // center line. If from is smaller, then the ray does not exist.
    if (from <= to || y > MAX_SIGHT) {
        return false;
    }

    // Find first visible location along this line. Minimum x such
    // that (2x-1)/x < from/GRADF <=> x > GRADF(2x-1)/from. This may
    // be called with y=0 whence x will be set to 0. Thus we need a
    // special fix.
    int x = (int) ((int32_t) GRADF * (2 * y - 1) / from + 1);
    if (x <= 0) {
        x = 1;
    }

    // Find last visible location along this line.
    // Maximum x such that (2x+1)/x > to/GRADF <=> x < GRADF(2x+1)/to
    int max_x = (int) (((int32_t) GRADF * (2 * y + 1) - 1) / to);
    if (max_x > MAX_SIGHT) {
        max_x = MAX_SIGHT;
    }
    if (max_x < x) {
        return false;
    }

    // gl_noquery is a HACK to prevent doubling up on direct lines of
    // sight. If 'to' is  greater than 1, we do not really look at
    // stuff along the direct line of sight, but we do have to see
    // what is opaque for the purposes of obscuring other objects.
    gl_noquery = (y == 0 && to > 1) || (y == x && from < GRADF * 2);

    bool transparent;

    if (look_see(x, y, &transparent)) {
        return true;
    }

    if (y == x) {
        gl_noquery = false;
    }

    if (transparent) {
        goto init_transparent;
    }

    while (true) {
        // Look down the window we've found.
        if (look_ray(y + 1, from, ((2 * y + 1) * (int32_t) GRADF / x))) {
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

            if (look_see(x, y, &transparent)) {
                return true;
            }
        } while (!transparent);

        init_transparent:

        // Find the end of this window of visibility.
        do {
            if (x == max_x) {
                // The window is trimmed by an earlier limit.
                return look_ray(y + 1, from, to);
            }

            x++;

            if (look_see(x, y, &transparent)) {
                return true;
            }
        } while (transparent);
    }
}

static bool look_see(int x, int y, bool *transparent) {
    if (x < 0 || y < 0 || y > x) {
        obj_desc_t errorMessage;
        (void) sprintf(errorMessage, "Illegal call to look_see(%d, %d)", x, y);
        printMessage(errorMessage);
    }

    const char *description;
    if (x == 0 && y == 0) {
        description = "You are on";
    } else {
        description = "You see";
    }

    int j = char_col + gl_fxx * x + gl_fxy * y;
    y = char_row + gl_fyx * x + gl_fyy * y;
    x = j;

    if (!coordInsidePanel(y, x)) {
        *transparent = false;
        return false;
    }

    Cave_t *c_ptr = &cave[y][x];
    *transparent = c_ptr->fval <= MAX_OPEN_SPACE;

    if (gl_noquery) {
        return false; // Don't look at a direct line of sight. A hack.
    }

    // This was uninitialized but the `query == ESCAPE` below was causing
    // a warning. Perhaps we can set it to `ESCAPE` here as default. -MRC-
    char query = ESCAPE;

    obj_desc_t msg;
    msg[0] = 0;

    if (gl_rock == 0 && c_ptr->cptr > 1 && monsters[c_ptr->cptr].ml) {
        j = monsters[c_ptr->cptr].mptr;
        (void) sprintf(msg, "%s %s %s. [(r)ecall]", description, isVowel(creatures_list[j].name[0]) ? "an" : "a", creatures_list[j].name);
        description = "It is on";
        putStringClearToEOL(msg, 0, 0);

        moveCursorRelative(y, x);
        query = getKeyInput();

        if (query == 'r' || query == 'R') {
            terminalSaveScreen();
            query = (char) roff_recall(j);
            terminalRestoreScreen();
        }
    }

    if (c_ptr->tl || c_ptr->pl || c_ptr->fm) {
        if (c_ptr->tptr != 0) {
            if (treasure_list[c_ptr->tptr].tval == TV_SECRET_DOOR) {
                goto granite;
            }

            if (gl_rock == 0 && treasure_list[c_ptr->tptr].tval != TV_INVIS_TRAP) {
                obj_desc_t obj_string;
                itemDescription(obj_string, &treasure_list[c_ptr->tptr], true);

                (void) sprintf(msg, "%s %s ---pause---", description, obj_string);
                description = "It is in";
                putStringClearToEOL(msg, 0, 0);

                moveCursorRelative(y, x);
                query = getKeyInput();
            }
        }

        if ((gl_rock || msg[0]) && c_ptr->fval >= MIN_CLOSED_SPACE) {
            const char *wall_description;

            switch (c_ptr->fval) {
                case BOUNDARY_WALL:
                case GRANITE_WALL:
                granite:
                    // Granite is only interesting if it contains something.
                    if (msg[0]) {
                        wall_description = "a granite wall";
                    } else {
                        wall_description = CNIL; // In case we jump here
                    }
                    break;
                case MAGMA_WALL:
                    wall_description = "some dark rock";
                    break;
                case QUARTZ_WALL:
                    wall_description = "a quartz vein";
                    break;
                default:
                    wall_description = CNIL;
                    break;
            }

            if (wall_description) {
                (void) sprintf(msg, "%s %s ---pause---", description, wall_description);
                putStringClearToEOL(msg, 0, 0);
                moveCursorRelative(y, x);
                query = getKeyInput();
            }
        }
    }

    if (msg[0]) {
        gl_nseen++;
        if (query == ESCAPE) {
            return true;
        }
    }

    return false;
}

static void inven_throw(int itemID, Inventory_t *treasure) {
    Inventory_t *i_ptr = &inventory[itemID];

    *treasure = *i_ptr;

    if (i_ptr->number > 1) {
        treasure->number = 1;
        i_ptr->number--;
        inventory_weight -= i_ptr->weight;
        py.flags.status |= PY_STR_WGT;
    } else {
        inventoryDestroyItem(itemID);
    }
}

// Obtain the hit and damage bonuses and the maximum distance for a thrown missile.
static void facts(Inventory_t *i_ptr, int *tbth, int *tpth, int *tdam, int *tdis) {
    int weight = i_ptr->weight;
    if (weight < 1) {
        weight = 1;
    }

    // Throwing objects
    *tdam = dicePlayerDamageRoll(i_ptr->damage) + i_ptr->todam;
    *tbth = py.misc.bthb * 75 / 100;
    *tpth = py.misc.ptohit + i_ptr->tohit;

    // Add this back later if the correct throwing device. -CJS-
    if (inventory[INVEN_WIELD].tval != TV_NOTHING) {
        *tpth -= inventory[INVEN_WIELD].tohit;
    }

    *tdis = (((py.stats.use_stat[A_STR] + 20) * 10) / weight);
    if (*tdis > 10) {
        *tdis = 10;
    }

    // multiply damage bonuses instead of adding, when have proper
    // missile/weapon combo, this makes them much more useful

    // Using Bows, slings, or crossbows?
    if (inventory[INVEN_WIELD].tval != TV_BOW) {
        return;
    }

    switch (inventory[INVEN_WIELD].p1) {
        case 1:
            if (i_ptr->tval == TV_SLING_AMMO) { // Sling and ammo
                *tbth = py.misc.bthb;
                *tpth += 2 * inventory[INVEN_WIELD].tohit;
                *tdam += inventory[INVEN_WIELD].todam;
                *tdam = *tdam * 2;
                *tdis = 20;
            }
            break;
        case 2:
            if (i_ptr->tval == TV_ARROW) { // Short Bow and Arrow
                *tbth = py.misc.bthb;
                *tpth += 2 * inventory[INVEN_WIELD].tohit;
                *tdam += inventory[INVEN_WIELD].todam;
                *tdam = *tdam * 2;
                *tdis = 25;
            }
            break;
        case 3:
            if (i_ptr->tval == TV_ARROW) { // Long Bow and Arrow
                *tbth = py.misc.bthb;
                *tpth += 2 * inventory[INVEN_WIELD].tohit;
                *tdam += inventory[INVEN_WIELD].todam;
                *tdam = *tdam * 3;
                *tdis = 30;
            }
            break;
        case 4:
            if (i_ptr->tval == TV_ARROW) { // Composite Bow and Arrow
                *tbth = py.misc.bthb;
                *tpth += 2 * inventory[INVEN_WIELD].tohit;
                *tdam += inventory[INVEN_WIELD].todam;
                *tdam = *tdam * 4;
                *tdis = 35;
            }
            break;
        case 5:
            if (i_ptr->tval == TV_BOLT) { // Light Crossbow and Bolt
                *tbth = py.misc.bthb;
                *tpth += 2 * inventory[INVEN_WIELD].tohit;
                *tdam += inventory[INVEN_WIELD].todam;
                *tdam = *tdam * 3;
                *tdis = 25;
            }
            break;
        case 6:
            if (i_ptr->tval == TV_BOLT) { // Heavy Crossbow and Bolt
                *tbth = py.misc.bthb;
                *tpth += 2 * inventory[INVEN_WIELD].tohit;
                *tdam += inventory[INVEN_WIELD].todam;
                *tdam = *tdam * 4;
                *tdis = 35;
            }
            break;
        default:
            // NOOP
            break;
    }
}

static void drop_throw(int y, int x, Inventory_t *t_ptr) {
    int i = y;
    int j = x;

    bool flag = false;

    if (randomNumber(10) > 1) {
        for (int k = 0; !flag && k <= 9;) {
            if (coordInBounds(i, j)) {
                if (cave[i][j].fval <= MAX_OPEN_SPACE && cave[i][j].tptr == 0) {
                    flag = true;
                }
            }

            if (!flag) {
                i = y + randomNumber(3) - 2;
                j = x + randomNumber(3) - 2;
                k++;
            }
        }
    }

    if (flag) {
        int cur_pos = popt();
        cave[i][j].tptr = (uint8_t) cur_pos;
        treasure_list[cur_pos] = *t_ptr;
        dungeonLiteSpot(i, j);
    } else {
        obj_desc_t description, msg;
        itemDescription(description, t_ptr, false);

        (void) sprintf(msg, "The %s disappears.", description);
        printMessage(msg);
    }
}

// Throw an object across the dungeon. -RAK-
// Note: Flasks of oil do fire damage
// Note: Extra damage and chance of hitting when missiles are used
// with correct weapon. i.e. wield bow and throw arrow.
void throw_object() {
    if (inventory_count == 0) {
        printMessage("But you are not carrying anything.");
        player_free_turn = true;
        return;
    }

    int itemID;
    if (!inventoryGetInputForItemId(&itemID, "Fire/Throw which one?", 0, inventory_count - 1, CNIL, CNIL)) {
        return;
    }

    int dir;
    if (!getDirectionWithMemory(CNIL, &dir)) {
        return;
    }

    itemTypeRemainingCountDescription(itemID);

    if (py.flags.confused > 0) {
        printMessage("You are confused.");
        dir = getRandomDirection();
    }

    Inventory_t throw_obj;
    inven_throw(itemID, &throw_obj);

    int tbth, tpth, tdam, tdis;
    facts(&throw_obj, &tbth, &tpth, &tdam, &tdis);

    char tchar = throw_obj.tchar;
    bool visible;
    int y = char_row;
    int x = char_col;
    int oldy = char_row;
    int oldx = char_col;
    int cur_dis = 0;

    bool flag = false;

    while (!flag) {
        (void) playerMovePosition(dir, &y, &x);
        cur_dis++;
        dungeonLiteSpot(oldy, oldx);

        if (cur_dis > tdis) {
            flag = true;
        }

        Cave_t *c_ptr = &cave[y][x];

        if (c_ptr->fval <= MAX_OPEN_SPACE && !flag) {
            if (c_ptr->cptr > 1) {
                flag = true;

                Monster_t *m_ptr = &monsters[c_ptr->cptr];

                tbth -= cur_dis;

                // if monster not lit, make it much more difficult to hit, subtract
                // off most bonuses, and reduce bthb depending on distance.
                if (!m_ptr->ml) {
                    tbth /= cur_dis + 2;
                    tbth -= py.misc.lev * class_level_adj[py.misc.pclass][CLA_BTHB] / 2;
                    tbth -= tpth * (BTH_PLUS_ADJ - 1);
                }

                if (playerTestBeingHit(tbth, (int) py.misc.lev, tpth, (int) creatures_list[m_ptr->mptr].ac, CLA_BTHB)) {
                    int damage = m_ptr->mptr;

                    obj_desc_t description, msg;
                    itemDescription(description, &throw_obj, false);

                    // Does the player know what he's fighting?
                    if (!m_ptr->ml) {
                        (void) sprintf(msg, "You hear a cry as the %s finds a mark.", description);
                        visible = false;
                    } else {
                        (void) sprintf(msg, "The %s hits the %s.", description, creatures_list[damage].name);
                        visible = true;
                    }
                    printMessage(msg);

                    tdam = itemMagicAbilityDamage(&throw_obj, tdam, damage);
                    tdam = playerWeaponCriticalBlow((int) throw_obj.weight, tpth, tdam, CLA_BTHB);

                    if (tdam < 0) {
                        tdam = 0;
                    }

                    damage = monsterTakeHit((int) c_ptr->cptr, tdam);

                    if (damage >= 0) {
                        if (!visible) {
                            printMessage("You have killed something!");
                        } else {
                            (void) sprintf(msg, "You have killed the %s.", creatures_list[damage].name);
                            printMessage(msg);
                        }
                        displayCharacterExperience();
                    }
                } else {
                    drop_throw(oldy, oldx, &throw_obj);
                }
            } else {
                // do not test c_ptr->fm here

                if (coordInsidePanel(y, x) && py.flags.blind < 1 && (c_ptr->tl || c_ptr->pl)) {
                    putChar(tchar, y, x);
                    putQIO(); // show object moving
                }
            }
        } else {
            flag = true;
            drop_throw(oldy, oldx, &throw_obj);
        }

        oldy = y;
        oldx = x;
    }
}

// Make a bash attack on someone. -CJS-
// Used to be part of bash above.
static void py_bash(int y, int x) {
    int monsterID = cave[y][x].cptr;
    Monster_t *m_ptr = &monsters[monsterID];
    Creature_t *c_ptr = &creatures_list[m_ptr->mptr];

    m_ptr->csleep = 0;

    // Does the player know what he's fighting?
    vtype_t name;
    if (!m_ptr->ml) {
        (void) strcpy(name, "it");
    } else {
        (void) sprintf(name, "the %s", c_ptr->name);
    }

    int base_tohit = py.stats.use_stat[A_STR];
    base_tohit += inventory[INVEN_ARM].weight / 2;
    base_tohit += py.misc.wt / 10;

    if (!m_ptr->ml) {
        base_tohit /= 2;
        base_tohit -= py.stats.use_stat[A_DEX] * (BTH_PLUS_ADJ - 1);
        base_tohit -= py.misc.lev * class_level_adj[py.misc.pclass][CLA_BTH] / 2;
    }

    if (playerTestBeingHit(base_tohit, (int) py.misc.lev, (int) py.stats.use_stat[A_DEX], (int) c_ptr->ac, CLA_BTH)) {
        vtype_t msg;
        (void) sprintf(msg, "You hit %s.", name);
        printMessage(msg);

        int damage = dicePlayerDamageRoll(inventory[INVEN_ARM].damage);
        damage = playerWeaponCriticalBlow(inventory[INVEN_ARM].weight / 4 + py.stats.use_stat[A_STR], 0, damage, CLA_BTH);
        damage += py.misc.wt / 60;
        damage += 3;

        if (damage < 0) {
            damage = 0;
        }

        // See if we done it in.
        if (monsterTakeHit(monsterID, damage) >= 0) {
            (void) sprintf(msg, "You have slain %s.", name);
            printMessage(msg);
            displayCharacterExperience();
        } else {
            name[0] = (char) toupper((int) name[0]); // Capitalize

            // Can not stun Balrog
            int avg_max_hp;
            if (c_ptr->cdefense & CD_MAX_HP) {
                avg_max_hp = c_ptr->hd[0] * c_ptr->hd[1];
            } else {
                avg_max_hp = (c_ptr->hd[0] * (c_ptr->hd[1] + 1)) >> 1;
            }

            if (100 + randomNumber(400) + randomNumber(400) > m_ptr->hp + avg_max_hp) {
                m_ptr->stunned += randomNumber(3) + 1;
                if (m_ptr->stunned > 24) {
                    m_ptr->stunned = 24;
                }

                (void) sprintf(msg, "%s appears stunned!", name);
            } else {
                (void) sprintf(msg, "%s ignores your bash!", name);
            }
            printMessage(msg);
        }
    } else {
        vtype_t msg;
        (void) sprintf(msg, "You miss %s.", name);
        printMessage(msg);
    }

    if (randomNumber(150) > py.stats.use_stat[A_DEX]) {
        printMessage("You are off balance.");
        py.flags.paralysis = (int16_t) (1 + randomNumber(2));
    }
}

static void playerBashPosition(int y, int x) {
    // Is a Coward?
    if (py.flags.afraid > 0) {
        printMessage("You are afraid!");
        return;
    }

    py_bash(y, x);
}

static void bashClosedDoor(int y, int x, int dir, Cave_t *tile, Inventory_t *item) {
    printMessageNoCommandInterrupt("You smash into the door!");

    int chance = py.stats.use_stat[A_STR] + py.misc.wt / 2;

    // Use (roughly) similar method as for monsters.
    if (randomNumber(chance * (20 + abs(item->p1))) < 10 * (chance - abs(item->p1))) {
        printMessage("The door crashes open!");

        inventoryItemCopyTo(OBJ_OPEN_DOOR, &treasure_list[tile->tptr]);

        // 50% chance of breaking door
        item->p1 = (int16_t) (1 - randomNumber(2));

        tile->fval = CORR_FLOOR;

        if (py.flags.confused == 0) {
            playerMove(dir, false);
        } else {
            dungeonLiteSpot(y, x);
        }

        return;
    }

    if (randomNumber(150) > py.stats.use_stat[A_DEX]) {
        printMessage("You are off-balance.");
        py.flags.paralysis = (int16_t) (1 + randomNumber(2));
        return;
    }

    if (command_count == 0) {
        printMessage("The door holds firm.");
    }
}

static void bashClosedChest(Inventory_t *item) {
    if (randomNumber(10) == 1) {
        printMessage("You have destroyed the chest.");
        printMessage("and its contents!");

        item->index = OBJ_RUINED_CHEST;
        item->flags = 0;

        return;
    }

    if ((item->flags & CH_LOCKED) && randomNumber(10) == 1) {
        printMessage("The lock breaks open!");

        item->flags &= ~CH_LOCKED;

        return;
    }

    printMessageNoCommandInterrupt("The chest holds firm.");
}

// Bash open a door or chest -RAK-
// Note: Affected by strength and weight of character
//
// For a closed door, p1 is positive if locked; negative if stuck. A disarm spell
// unlocks and unjams doors!
//
// For an open door, p1 is positive for a broken door.
//
// A closed door can be opened - harder if locked. Any door might be bashed open
// (and thereby broken). Bashing a door is (potentially) faster! You move into the
// door way. To open a stuck door, it must be bashed. A closed door can be jammed
// (which makes it stuck if previously locked).
//
// Creatures can also open doors. A creature with open door ability will (if not
// in the line of sight) move though a closed or secret door with no changes. If
// in the line of sight, closed door are opened, & secret door revealed. Whether
// in the line of sight or not, such a creature may unlock or unstick a door.
//
// A creature with no such ability will attempt to bash a non-secret door.
void bash() {
    int dir;
    if (!getDirectionWithMemory(CNIL, &dir)) {
        return;
    }

    if (py.flags.confused > 0) {
        printMessage("You are confused.");
        dir = getRandomDirection();
    }

    int y = char_row;
    int x = char_col;
    (void) playerMovePosition(dir, &y, &x);

    Cave_t *c_ptr = &cave[y][x];

    if (c_ptr->cptr > 1) {
        playerBashPosition(y, x);
        return;
    }

    if (c_ptr->tptr != 0) {
        Inventory_t *t_ptr = &treasure_list[c_ptr->tptr];

        if (t_ptr->tval == TV_CLOSED_DOOR) {
            bashClosedDoor(y, x, dir, c_ptr, t_ptr);
        } else if (t_ptr->tval == TV_CHEST) {
            bashClosedChest(t_ptr);
        } else {
            // Can't give free turn, or else player could try directions
            // until he found invisible creature
            printMessage("You bash it, but nothing interesting happens.");
        }
        return;
    }

    if (c_ptr->fval < MIN_CAVE_WALL) {
        printMessage("You bash at empty space.");
        return;
    }

    // same message for wall as for secret door
    printMessage("You bash it, but nothing interesting happens.");
}

int getRandomDirection() {
    int dir;

    do {
        dir = randomNumber(9);
    } while (dir == 5);

    return dir;
}
