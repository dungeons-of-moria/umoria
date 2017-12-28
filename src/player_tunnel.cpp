// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// Player throw functions

#include "headers.h"

// Don't let the player tunnel somewhere illegal, this is necessary to
// prevent the player from getting a free attack by trying to tunnel
// somewhere where it has no effect.
static bool playerCanTunnel(int treasure_id, int tile_id) {
    if (tile_id < MIN_CAVE_WALL && (treasure_id == 0 || (treasure_list[treasure_id].category_id != TV_RUBBLE && treasure_list[treasure_id].category_id != TV_SECRET_DOOR))) {
        game.player_free_turn = true;

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
static int playerDiggingAbility(const Inventory_t &weapon) {
    int diggingAbility = py.stats.used[A_STR];

    if ((weapon.flags & TR_TUNNEL) != 0u) {
        diggingAbility += 25 + weapon.misc_use * 50;
    } else {
        diggingAbility += maxDiceRoll(weapon.damage) + weapon.to_hit + weapon.to_damage;

        // divide by two so that digging without shovel isn't too easy
        diggingAbility >>= 1;
    }

    // If this weapon is too heavy for the player to wield properly,
    // then also make it harder to dig with it.
    if (py.weapon_is_heavy) {
        diggingAbility += (py.stats.used[A_STR] * 15) - weapon.weight;

        if (diggingAbility < 0) {
            diggingAbility = 0;
        }
    }

    return diggingAbility;
}

static void dungeonDigGraniteWall(int y, int x, int digging_ability) {
    int i = randomNumber(1200) + 80;

    if (playerTunnelWall(y, x, digging_ability, i)) {
        printMessage("You have finished the tunnel.");
    } else {
        printMessageNoCommandInterrupt("You tunnel into the granite wall.");
    }
}

static void dungeonDigMagmaWall(int y, int x, int digging_ability) {
    int i = randomNumber(600) + 10;

    if (playerTunnelWall(y, x, digging_ability, i)) {
        printMessage("You have finished the tunnel.");
    } else {
        printMessageNoCommandInterrupt("You tunnel into the magma intrusion.");
    }
}

static void dungeonDigQuartzWall(int y, int x, int digging_ability) {
    int i = randomNumber(400) + 10;

    if (playerTunnelWall(y, x, digging_ability, i)) {
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

            if (caveTileVisible(Coord_t{y, x})) {
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

    int y = py.row;
    int x = py.col;
    (void) playerMovePosition(direction, y, x);

    const Tile_t &tile = dg.floor[y][x];
    Inventory_t &item = inventory[EQUIPMENT_WIELD];

    if (!playerCanTunnel(tile.treasure_id, tile.feature_id)) {
        return;
    }

    if (tile.creature_id > 1) {
        objectBlockedByMonster(tile.creature_id);
        playerAttackPosition(y, x);
        return;
    }

    if (item.category_id != TV_NOTHING) {
        int diggingAbility = playerDiggingAbility(item);

        if (!dungeonDigAtLocation(y, x, tile.feature_id, diggingAbility)) {
            // Is there an object in the way?  (Rubble and secret doors)
            if (tile.treasure_id != 0) {
                if (treasure_list[tile.treasure_id].category_id == TV_RUBBLE) {
                    dungeonDigRubble(y, x, diggingAbility);
                } else if (treasure_list[tile.treasure_id].category_id == TV_SECRET_DOOR) {
                    // Found secret door!
                    printMessageNoCommandInterrupt("You tunnel into the granite wall.");
                    playerSearch(py.row, py.col, py.misc.chance_in_search);
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
