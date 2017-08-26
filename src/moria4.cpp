// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Misc code, mainly to handle player commands

#include "headers.h"
#include "externs.h"

static bool lookRay(int y, int from, int to);
static bool lookSee(int x, int y, bool *transparent);

// Don't let the player tunnel somewhere illegal, this is necessary to
// prevent the player from getting a free attack by trying to tunnel
// somewhere where it has no effect.
static bool playerCanTunnel(int treasure_id, int tile_id) {
    if (tile_id < MIN_CAVE_WALL && (treasure_id == 0 || (treasure_list[treasure_id].category_id != TV_RUBBLE && treasure_list[treasure_id].category_id != TV_SECRET_DOOR))) {
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
    int diggingAbility = py.stats.used[A_STR];

    if ((weapon->flags & TR_TUNNEL) != 0u) {
        diggingAbility += 25 + weapon->misc_use * 50;
    } else {
        diggingAbility += (weapon->damage[0] * weapon->damage[1]) + weapon->to_hit + weapon->to_damage;

        // divide by two so that digging without shovel isn't too easy
        diggingAbility >>= 1;
    }

    // If this weapon is too heavy for the player to wield properly,
    // then also make it harder to dig with it.
    if (py.weapon_is_heavy) {
        diggingAbility += (py.stats.used[A_STR] * 15) - weapon->weight;

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
        case TILE_GRANITE_WALL:
            dungeonDigGraniteWall(y, x, digging_ability);
            break;
        case TILE_MAGMA_WALL:
            dungeonDigMagmaWall(y, x, digging_ability);
            break;
        case TILE_QUARTZ_WALL:
            dungeonDigQuartzWall(y, x, digging_ability);
            break;
        case TILE_BOUNDARY_WALL:
            printMessage("This seems to be permanent rock.");
            break;
        default:
            return false;
    }
    return true;
}

// Tunnels through rubble and walls -RAK-
// Must take into account: secret doors, special tools
void playerTunnel(int direction) {
    // Confused?                    75% random movement
    if (py.flags.confused > 0 && randomNumber(4) > 1) {
        direction = randomNumber(9);
    }

    int y = char_row;
    int x = char_col;
    (void) playerMovePosition(direction, &y, &x);

    Cave_t *tile = &cave[y][x];
    Inventory_t *item = &inventory[EQUIPMENT_WIELD];

    if (!playerCanTunnel(tile->treasure_id, tile->feature_id)) {
        return;
    }

    if (tile->creature_id > 1) {
        objectBlockedByMonster(tile->creature_id);
        playerAttackPosition(y, x);
        return;
    }

    if (item->category_id != TV_NOTHING) {
        int diggingAbility = playerDiggingAbility(item);

        if (!dungeonDigAtLocation(y, x, tile->feature_id, diggingAbility)) {
            // Is there an object in the way?  (Rubble and secret doors)
            if (tile->treasure_id != 0) {
                if (treasure_list[tile->treasure_id].category_id == TV_RUBBLE) {
                    dungeonDigRubble(y, x, diggingAbility);
                } else if (treasure_list[tile->treasure_id].category_id == TV_SECRET_DOOR) {
                    // Found secret door!
                    printMessageNoCommandInterrupt("You tunnel into the granite wall.");
                    dungeonSearch(char_row, char_col, py.misc.chance_in_search);
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
    ability += class_level_adj[py.misc.class_id][CLASS_DISARM] * py.misc.level / 3;

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

static void playerDisarmFloorTrap(int y, int x, int total, int level, int dir, int16_t misc_use) {
    int confused = py.flags.confused;

    if (total + 100 - level > randomNumber(100)) {
        printMessage("You have disarmed the trap.");
        py.misc.exp += misc_use;
        (void) dungeonDeleteObject(y, x);

        // make sure we move onto the trap even if confused
        py.flags.confused = 0;
        playerMove(dir, false);
        py.flags.confused = (int16_t) confused;

        displayCharacterExperience();
        return;
    }

    // avoid randomNumber(0) call
    if (total > 5 && randomNumber(total) > 5) {
        printMessageNoCommandInterrupt("You failed to disarm the trap.");
        return;
    }

    printMessage("You set the trap off!");

    // make sure we move onto the trap even if confused
    py.flags.confused = 0;
    playerMove(dir, false);
    py.flags.confused += confused;
}

static void playerDisarmChestTrap(int y, int x, int total, Inventory_t *item) {
    if (!spellItemIdentified(item)) {
        player_free_turn = true;
        printMessage("I don't see a trap.");

        return;
    }

    if ((item->flags & CH_TRAPPED) != 0u) {
        int level = item->depth_first_found;

        if ((total - level) > randomNumber(100)) {
            item->flags &= ~CH_TRAPPED;

            if ((item->flags & CH_LOCKED) != 0u) {
                item->special_name_id = SN_LOCKED;
            } else {
                item->special_name_id = SN_DISARMED;
            }

            printMessage("You have disarmed the chest.");

            spellItemIdentifyAndRemoveRandomInscription(item);
            py.misc.exp += level;

            displayCharacterExperience();
        } else if ((total > 5) && (randomNumber(total) > 5)) {
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
void playerDisarmTrap() {
    int dir;
    if (!getDirectionWithMemory(CNIL, &dir)) {
        return;
    }

    int y = char_row;
    int x = char_col;
    (void) playerMovePosition(dir, &y, &x);

    Cave_t *tile = &cave[y][x];

    bool no_disarm = false;

    if (tile->creature_id > 1 && tile->treasure_id != 0 && (treasure_list[tile->treasure_id].category_id == TV_VIS_TRAP || treasure_list[tile->treasure_id].category_id == TV_CHEST)) {
        objectBlockedByMonster(tile->creature_id);
    } else if (tile->treasure_id != 0) {
        int disarm_ability = playerTrapDisarmAbility();

        Inventory_t *item = &treasure_list[tile->treasure_id];

        if (item->category_id == TV_VIS_TRAP) {
            playerDisarmFloorTrap(y, x, disarm_ability, item->depth_first_found, dir, item->misc_use);
        } else if (item->category_id == TV_CHEST) {
            playerDisarmChestTrap(y, x, disarm_ability, item);
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

  dungeon y = char_row   + los_fyx * (ray x)  + los_fyy * (ray y)
  dungeon x = char_col   + los_fxx * (ray x)  + los_fxy * (ray y)
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
    if (!getAllDirections("Look which direction?", &dir)) {
        return;
    }

    los_num_places_seen = 0;
    los_rocks_and_objects = 0;

    // Have to set this up for the lookSee
    los_hack_no_query = false;

    bool dummy;
    if (lookSee(0, 0, &dummy)) {
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

    if (lookSee(x, y, &transparent)) {
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

            if (lookSee(x, y, &transparent)) {
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

            if (lookSee(x, y, &transparent)) {
                return true;
            }
        } while (transparent);
    }
}

static bool lookSee(int x, int y, bool *transparent) {
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

    int j = char_col + los_fxx * x + los_fxy * y;
    y = char_row + los_fyx * x + los_fyy * y;
    x = j;

    if (!coordInsidePanel(y, x)) {
        *transparent = false;
        return false;
    }

    Cave_t *tile = &cave[y][x];
    *transparent = tile->feature_id <= MAX_OPEN_SPACE;

    if (los_hack_no_query) {
        return false; // Don't look at a direct line of sight. A hack.
    }

    // This was uninitialized but the `query == ESCAPE` below was causing
    // a warning. Perhaps we can set it to `ESCAPE` here as default. -MRC-
    char query = ESCAPE;

    obj_desc_t msg = {'\0'};

    if (los_rocks_and_objects == 0 && tile->creature_id > 1 && monsters[tile->creature_id].lit) {
        j = monsters[tile->creature_id].creature_id;
        (void) sprintf(msg, "%s %s %s. [(r)ecall]", description, isVowel(creatures_list[j].name[0]) ? "an" : "a", creatures_list[j].name);
        description = "It is on";
        putStringClearToEOL(msg, 0, 0);

        moveCursorRelative(y, x);
        query = getKeyInput();

        if (query == 'r' || query == 'R') {
            terminalSaveScreen();
            query = (char) memoryRecall(j);
            terminalRestoreScreen();
        }
    }

    if (tile->temporary_light || tile->permanent_light || tile->field_mark) {
        const char *wall_description;

        if (tile->treasure_id != 0) {
            if (treasure_list[tile->treasure_id].category_id == TV_SECRET_DOOR) {
                goto granite;
            }

            if (los_rocks_and_objects == 0 && treasure_list[tile->treasure_id].category_id != TV_INVIS_TRAP) {
                obj_desc_t obj_string = {'\0'};
                itemDescription(obj_string, &treasure_list[tile->treasure_id], true);

                (void) sprintf(msg, "%s %s ---pause---", description, obj_string);
                description = "It is in";
                putStringClearToEOL(msg, 0, 0);

                moveCursorRelative(y, x);
                query = getKeyInput();
            }
        }

        if (((los_rocks_and_objects != 0) || (msg[0] != 0)) && tile->feature_id >= MIN_CLOSED_SPACE) {
            switch (tile->feature_id) {
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
                putStringClearToEOL(msg, 0, 0);
                moveCursorRelative(y, x);
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

static void inventoryThrow(int item_id, Inventory_t *treasure) {
    Inventory_t *item = &inventory[item_id];

    *treasure = *item;

    if (item->items_count > 1) {
        treasure->items_count = 1;
        item->items_count--;
        inventory_weight -= item->weight;
        py.flags.status |= PY_STR_WGT;
    } else {
        inventoryDestroyItem(item_id);
    }
}

// Obtain the hit and damage bonuses and the maximum distance for a thrown missile.
static void weaponMissileFacts(Inventory_t *item, int *tbth, int *tpth, int *tdam, int *tdis) {
    int weight = item->weight;
    if (weight < 1) {
        weight = 1;
    }

    // Throwing objects
    *tdam = dicePlayerDamageRoll(item->damage) + item->to_damage;
    *tbth = py.misc.bth_with_bows * 75 / 100;
    *tpth = py.misc.plusses_to_hit + item->to_hit;

    // Add this back later if the correct throwing device. -CJS-
    if (inventory[EQUIPMENT_WIELD].category_id != TV_NOTHING) {
        *tpth -= inventory[EQUIPMENT_WIELD].to_hit;
    }

    *tdis = (((py.stats.used[A_STR] + 20) * 10) / weight);
    if (*tdis > 10) {
        *tdis = 10;
    }

    // multiply damage bonuses instead of adding, when have proper
    // missile/weapon combo, this makes them much more useful

    // Using Bows, slings, or crossbows?
    if (inventory[EQUIPMENT_WIELD].category_id != TV_BOW) {
        return;
    }

    switch (inventory[EQUIPMENT_WIELD].misc_use) {
        case 1:
            if (item->category_id == TV_SLING_AMMO) { // Sling and ammo
                *tbth = py.misc.bth_with_bows;
                *tpth += 2 * inventory[EQUIPMENT_WIELD].to_hit;
                *tdam += inventory[EQUIPMENT_WIELD].to_damage;
                *tdam = *tdam * 2;
                *tdis = 20;
            }
            break;
        case 2:
            if (item->category_id == TV_ARROW) { // Short Bow and Arrow
                *tbth = py.misc.bth_with_bows;
                *tpth += 2 * inventory[EQUIPMENT_WIELD].to_hit;
                *tdam += inventory[EQUIPMENT_WIELD].to_damage;
                *tdam = *tdam * 2;
                *tdis = 25;
            }
            break;
        case 3:
            if (item->category_id == TV_ARROW) { // Long Bow and Arrow
                *tbth = py.misc.bth_with_bows;
                *tpth += 2 * inventory[EQUIPMENT_WIELD].to_hit;
                *tdam += inventory[EQUIPMENT_WIELD].to_damage;
                *tdam = *tdam * 3;
                *tdis = 30;
            }
            break;
        case 4:
            if (item->category_id == TV_ARROW) { // Composite Bow and Arrow
                *tbth = py.misc.bth_with_bows;
                *tpth += 2 * inventory[EQUIPMENT_WIELD].to_hit;
                *tdam += inventory[EQUIPMENT_WIELD].to_damage;
                *tdam = *tdam * 4;
                *tdis = 35;
            }
            break;
        case 5:
            if (item->category_id == TV_BOLT) { // Light Crossbow and Bolt
                *tbth = py.misc.bth_with_bows;
                *tpth += 2 * inventory[EQUIPMENT_WIELD].to_hit;
                *tdam += inventory[EQUIPMENT_WIELD].to_damage;
                *tdam = *tdam * 3;
                *tdis = 25;
            }
            break;
        case 6:
            if (item->category_id == TV_BOLT) { // Heavy Crossbow and Bolt
                *tbth = py.misc.bth_with_bows;
                *tpth += 2 * inventory[EQUIPMENT_WIELD].to_hit;
                *tdam += inventory[EQUIPMENT_WIELD].to_damage;
                *tdam = *tdam * 4;
                *tdis = 35;
            }
            break;
        default:
            // NOOP
            break;
    }
}

static void inventoryDropOrThrowItem(int y, int x, Inventory_t *item) {
    int pos_y = y;
    int pos_x = x;

    bool flag = false;

    if (randomNumber(10) > 1) {
        for (int k = 0; !flag && k <= 9;) {
            if (coordInBounds(pos_y, pos_x)) {
                if (cave[pos_y][pos_x].feature_id <= MAX_OPEN_SPACE && cave[pos_y][pos_x].treasure_id == 0) {
                    flag = true;
                }
            }

            if (!flag) {
                pos_y = y + randomNumber(3) - 2;
                pos_x = x + randomNumber(3) - 2;
                k++;
            }
        }
    }

    if (flag) {
        int cur_pos = popt();
        cave[pos_y][pos_x].treasure_id = (uint8_t) cur_pos;
        treasure_list[cur_pos] = *item;
        dungeonLiteSpot(pos_y, pos_x);
    } else {
        obj_desc_t description = {'\0'};
        obj_desc_t msg = {'\0'};
        itemDescription(description, item, false);

        (void) sprintf(msg, "The %s disappears.", description);
        printMessage(msg);
    }
}

// Throw an object across the dungeon. -RAK-
// Note: Flasks of oil do fire damage
// Note: Extra damage and chance of hitting when missiles are used
// with correct weapon. i.e. wield bow and throw arrow.
void playerThrowItem() {
    if (inventory_count == 0) {
        printMessage("But you are not carrying anything.");
        player_free_turn = true;
        return;
    }

    int item_id;
    if (!inventoryGetInputForItemId(&item_id, "Fire/Throw which one?", 0, inventory_count - 1, CNIL, CNIL)) {
        return;
    }

    int dir;
    if (!getDirectionWithMemory(CNIL, &dir)) {
        return;
    }

    itemTypeRemainingCountDescription(item_id);

    if (py.flags.confused > 0) {
        printMessage("You are confused.");
        dir = getRandomDirection();
    }

    Inventory_t thrown_item{};
    inventoryThrow(item_id, &thrown_item);

    int tbth, tpth, tdam, tdis;
    weaponMissileFacts(&thrown_item, &tbth, &tpth, &tdam, &tdis);

    char tile_char = thrown_item.sprite;
    bool visible;
    int y = char_row;
    int x = char_col;
    int old_y = char_row;
    int old_x = char_col;
    int current_distance = 0;

    Cave_t *tile;
    bool flag = false;

    while (!flag) {
        (void) playerMovePosition(dir, &y, &x);

        if (current_distance + 1 > tdis) {
            break;
        }

        current_distance++;
        dungeonLiteSpot(old_y, old_x);

        tile = &cave[y][x];

        if (tile->feature_id <= MAX_OPEN_SPACE && !flag) {
            if (tile->creature_id > 1) {
                flag = true;

                Monster_t *m_ptr = &monsters[tile->creature_id];

                tbth -= current_distance;

                // if monster not lit, make it much more difficult to hit, subtract
                // off most bonuses, and reduce bth_with_bows depending on distance.
                if (!m_ptr->lit) {
                    tbth /= current_distance + 2;
                    tbth -= py.misc.level * class_level_adj[py.misc.class_id][CLASS_BTHB] / 2;
                    tbth -= tpth * (BTH_PER_PLUS_TO_HIT_ADJUST - 1);
                }

                if (playerTestBeingHit(tbth, (int) py.misc.level, tpth, (int) creatures_list[m_ptr->creature_id].ac, CLASS_BTHB)) {
                    int damage = m_ptr->creature_id;

                    obj_desc_t description = {'\0'};
                    obj_desc_t msg = {'\0'};
                    itemDescription(description, &thrown_item, false);

                    // Does the player know what he's fighting?
                    if (!m_ptr->lit) {
                        (void) sprintf(msg, "You hear a cry as the %s finds a mark.", description);
                        visible = false;
                    } else {
                        (void) sprintf(msg, "The %s hits the %s.", description, creatures_list[damage].name);
                        visible = true;
                    }
                    printMessage(msg);

                    tdam = itemMagicAbilityDamage(&thrown_item, tdam, damage);
                    tdam = playerWeaponCriticalBlow((int) thrown_item.weight, tpth, tdam, CLASS_BTHB);

                    if (tdam < 0) {
                        tdam = 0;
                    }

                    damage = monsterTakeHit((int) tile->creature_id, tdam);

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
                    inventoryDropOrThrowItem(old_y, old_x, &thrown_item);
                }
            } else {
                // do not test tile->field_mark here

                if (coordInsidePanel(y, x) && py.flags.blind < 1 && (tile->temporary_light || tile->permanent_light)) {
                    putChar(tile_char, y, x);
                    putQIO(); // show object moving
                }
            }
        } else {
            flag = true;
            inventoryDropOrThrowItem(old_y, old_x, &thrown_item);
        }

        old_y = y;
        old_x = x;
    }
}

// Make a bash attack on someone. -CJS-
// Used to be part of bash above.
static void playerBashAttack(int y, int x) {
    int monster_id = cave[y][x].creature_id;
    Monster_t *monster = &monsters[monster_id];
    Creature_t *creature = &creatures_list[monster->creature_id];

    monster->sleep_count = 0;

    // Does the player know what he's fighting?
    vtype_t name = {'\0'};
    if (!monster->lit) {
        (void) strcpy(name, "it");
    } else {
        (void) sprintf(name, "the %s", creature->name);
    }

    int base_to_hit = py.stats.used[A_STR];
    base_to_hit += inventory[EQUIPMENT_ARM].weight / 2;
    base_to_hit += py.misc.weight / 10;

    if (!monster->lit) {
        base_to_hit /= 2;
        base_to_hit -= py.stats.used[A_DEX] * (BTH_PER_PLUS_TO_HIT_ADJUST - 1);
        base_to_hit -= py.misc.level * class_level_adj[py.misc.class_id][CLASS_BTH] / 2;
    }

    if (playerTestBeingHit(base_to_hit, (int) py.misc.level, (int) py.stats.used[A_DEX], (int) creature->ac, CLASS_BTH)) {
        vtype_t msg = {'\0'};
        (void) sprintf(msg, "You hit %s.", name);
        printMessage(msg);

        int damage = dicePlayerDamageRoll(inventory[EQUIPMENT_ARM].damage);
        damage = playerWeaponCriticalBlow(inventory[EQUIPMENT_ARM].weight / 4 + py.stats.used[A_STR], 0, damage, CLASS_BTH);
        damage += py.misc.weight / 60;
        damage += 3;

        if (damage < 0) {
            damage = 0;
        }

        // See if we done it in.
        if (monsterTakeHit(monster_id, damage) >= 0) {
            (void) sprintf(msg, "You have slain %s.", name);
            printMessage(msg);
            displayCharacterExperience();
        } else {
            name[0] = (char) toupper((int) name[0]); // Capitalize

            // Can not stun Balrog
            int avg_max_hp;
            if ((creature->defenses & CD_MAX_HP) != 0) {
                avg_max_hp = creature->hit_die[0] * creature->hit_die[1];
            } else {
                avg_max_hp = (creature->hit_die[0] * (creature->hit_die[1] + 1)) >> 1;
            }

            if (100 + randomNumber(400) + randomNumber(400) > monster->hp + avg_max_hp) {
                monster->stunned_amount += randomNumber(3) + 1;
                if (monster->stunned_amount > 24) {
                    monster->stunned_amount = 24;
                }

                (void) sprintf(msg, "%s appears stunned!", name);
            } else {
                (void) sprintf(msg, "%s ignores your bash!", name);
            }
            printMessage(msg);
        }
    } else {
        vtype_t msg = {'\0'};
        (void) sprintf(msg, "You miss %s.", name);
        printMessage(msg);
    }

    if (randomNumber(150) > py.stats.used[A_DEX]) {
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

    playerBashAttack(y, x);
}

static void playerBashClosedDoor(int y, int x, int dir, Cave_t *tile, Inventory_t *item) {
    printMessageNoCommandInterrupt("You smash into the door!");

    int chance = py.stats.used[A_STR] + py.misc.weight / 2;

    // Use (roughly) similar method as for monsters.
    auto abs_misc_use = (int) std::abs((std::intmax_t) item->misc_use);
    if (randomNumber(chance * (20 + abs_misc_use)) < 10 * (chance - abs_misc_use)) {
        printMessage("The door crashes open!");

        inventoryItemCopyTo(OBJ_OPEN_DOOR, &treasure_list[tile->treasure_id]);

        // 50% chance of breaking door
        item->misc_use = (int16_t) (1 - randomNumber(2));

        tile->feature_id = TILE_CORR_FLOOR;

        if (py.flags.confused == 0) {
            playerMove(dir, false);
        } else {
            dungeonLiteSpot(y, x);
        }

        return;
    }

    if (randomNumber(150) > py.stats.used[A_DEX]) {
        printMessage("You are off-balance.");
        py.flags.paralysis = (int16_t) (1 + randomNumber(2));
        return;
    }

    if (command_count == 0) {
        printMessage("The door holds firm.");
    }
}

static void playerBashClosedChest(Inventory_t *item) {
    if (randomNumber(10) == 1) {
        printMessage("You have destroyed the chest.");
        printMessage("and its contents!");

        item->id = OBJ_RUINED_CHEST;
        item->flags = 0;

        return;
    }

    if (((item->flags & CH_LOCKED) != 0u) && randomNumber(10) == 1) {
        printMessage("The lock breaks open!");

        item->flags &= ~CH_LOCKED;

        return;
    }

    printMessageNoCommandInterrupt("The chest holds firm.");
}

// Bash open a door or chest -RAK-
// Note: Affected by strength and weight of character
//
// For a closed door, `misc_use` is positive if locked; negative if stuck. A disarm spell
// unlocks and unjams doors!
//
// For an open door, `misc_use` is positive for a broken door.
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
void playerBash() {
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

    Cave_t *tile = &cave[y][x];

    if (tile->creature_id > 1) {
        playerBashPosition(y, x);
        return;
    }

    if (tile->treasure_id != 0) {
        Inventory_t *item = &treasure_list[tile->treasure_id];

        if (item->category_id == TV_CLOSED_DOOR) {
            playerBashClosedDoor(y, x, dir, tile, item);
        } else if (item->category_id == TV_CHEST) {
            playerBashClosedChest(item);
        } else {
            // Can't give free turn, or else player could try directions
            // until he found invisible creature
            printMessage("You bash it, but nothing interesting happens.");
        }
        return;
    }

    if (tile->feature_id < MIN_CAVE_WALL) {
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
