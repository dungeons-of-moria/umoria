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
    if (tile_id < MIN_CAVE_WALL &&
        (treasure_id == 0 || (game.treasure.list[treasure_id].category_id != TV_RUBBLE && game.treasure.list[treasure_id].category_id != TV_SECRET_DOOR))) {
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
static int playerDiggingAbility(Inventory_t const &weapon) {
    int digging_ability = py.stats.used[PlayerAttr::A_STR];

    if ((weapon.flags & config::treasure::flags::TR_TUNNEL) != 0u) {
        digging_ability += 25 + weapon.misc_use * 50;
    } else {
        digging_ability += maxDiceRoll(weapon.damage) + weapon.to_hit + weapon.to_damage;

        // divide by two so that digging without shovel isn't too easy
        digging_ability >>= 1;
    }

    // If this weapon is too heavy for the player to wield properly,
    // then also make it harder to dig with it.
    if (py.weapon_is_heavy) {
        digging_ability += (py.stats.used[PlayerAttr::A_STR] * 15) - weapon.weight;

        if (digging_ability < 0) {
            digging_ability = 0;
        }
    }

    return digging_ability;
}

static void dungeonDigGraniteWall(Coord_t coord, int digging_ability) {
    int i = randomNumber(1200) + 80;

    if (playerTunnelWall(coord, digging_ability, i)) {
        printMessage("You have finished the tunnel.");
    } else {
        printMessageNoCommandInterrupt("You tunnel into the granite wall.");
    }
}

static void dungeonDigMagmaWall(Coord_t coord, int digging_ability) {
    int i = randomNumber(600) + 10;

    if (playerTunnelWall(coord, digging_ability, i)) {
        printMessage("You have finished the tunnel.");
    } else {
        printMessageNoCommandInterrupt("You tunnel into the magma intrusion.");
    }
}

static void dungeonDigQuartzWall(Coord_t coord, int digging_ability) {
    int i = randomNumber(400) + 10;

    if (playerTunnelWall(coord, digging_ability, i)) {
        printMessage("You have finished the tunnel.");
    } else {
        printMessageNoCommandInterrupt("You tunnel into the quartz vein.");
    }
}

static void dungeonDigRubble(Coord_t coord, int digging_ability) {
    if (digging_ability > randomNumber(180)) {
        (void) dungeonDeleteObject(coord);
        printMessage("You have removed the rubble.");

        if (randomNumber(10) == 1) {
            dungeonPlaceRandomObjectAt(coord, false);

            if (caveTileVisible(coord)) {
                printMessage("You have found something!");
            }
        }

        dungeonLiteSpot(coord);
    } else {
        printMessageNoCommandInterrupt("You dig in the rubble.");
    }
}

// Dig regular walls; Granite, magma intrusion, quartz vein
// Don't forget the boundary walls, made of titanium (255)
// Return `true` if a wall was dug at
static bool dungeonDigAtLocation(Coord_t coord, uint8_t wall_type, int digging_ability) {
    switch (wall_type) {
        case TILE_GRANITE_WALL:
            dungeonDigGraniteWall(coord, digging_ability);
            break;
        case TILE_MAGMA_WALL:
            dungeonDigMagmaWall(coord, digging_ability);
            break;
        case TILE_QUARTZ_WALL:
            dungeonDigQuartzWall(coord, digging_ability);
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

    Coord_t coord = py.pos;
    (void) playerMovePosition(direction, coord);

    Tile_t const &tile = dg.floor[coord.y][coord.x];
    Inventory_t &item = py.inventory[PlayerEquipment::Wield];

    if (!playerCanTunnel(tile.treasure_id, tile.feature_id)) {
        return;
    }

    if (tile.creature_id > 1) {
        objectBlockedByMonster(tile.creature_id);
        playerAttackPosition(coord);
        return;
    }

    if (item.category_id != TV_NOTHING) {
        int digging_ability = playerDiggingAbility(item);

        if (!dungeonDigAtLocation(coord, tile.feature_id, digging_ability)) {
            // Is there an object in the way?  (Rubble and secret doors)
            if (tile.treasure_id != 0) {
                if (game.treasure.list[tile.treasure_id].category_id == TV_RUBBLE) {
                    dungeonDigRubble(coord, digging_ability);
                } else if (game.treasure.list[tile.treasure_id].category_id == TV_SECRET_DOOR) {
                    // Found secret door!
                    printMessageNoCommandInterrupt("You tunnel into the granite wall.");
                    playerSearch(py.pos, py.misc.chance_in_search);
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
