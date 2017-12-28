// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// The running algorithm: -CJS-

#include "headers.h"
#include "dice.h"

static void playerBashAttack(int y, int x);
static void playerBashPosition(int y, int x);
static void playerBashClosedDoor(int y, int x, int dir, Tile_t &tile, Inventory_t &item);
static void playerBashClosedChest(Inventory_t &item);

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
    if (!getDirectionWithMemory(CNIL, dir)) {
        return;
    }

    if (py.flags.confused > 0) {
        printMessage("You are confused.");
        dir = getRandomDirection();
    }

    int y = py.row;
    int x = py.col;
    (void) playerMovePosition(dir, y, x);

    Tile_t &tile = dg.floor[y][x];

    if (tile.creature_id > 1) {
        playerBashPosition(y, x);
        return;
    }

    if (tile.treasure_id != 0) {
        Inventory_t &item = treasure_list[tile.treasure_id];

        if (item.category_id == TV_CLOSED_DOOR) {
            playerBashClosedDoor(y, x, dir, tile, item);
        } else if (item.category_id == TV_CHEST) {
            playerBashClosedChest(item);
        } else {
            // Can't give free turn, or else player could try
            // directions until they find the invisible creature
            printMessage("You bash it, but nothing interesting happens.");
        }
        return;
    }

    if (tile.feature_id < MIN_CAVE_WALL) {
        printMessage("You bash at empty space.");
        return;
    }

    // same message for wall as for secret door
    printMessage("You bash it, but nothing interesting happens.");
}

// Make a bash attack on someone. -CJS-
// Used to be part of bash above.
static void playerBashAttack(int y, int x) {
    int monster_id = dg.floor[y][x].creature_id;

    Monster_t &monster = monsters[monster_id];
    const Creature_t &creature = creatures_list[monster.creature_id];

    monster.sleep_count = 0;

    // Does the player know what they're fighting?
    vtype_t name = {'\0'};
    if (!monster.lit) {
        (void) strcpy(name, "it");
    } else {
        (void) sprintf(name, "the %s", creature.name);
    }

    int base_to_hit = py.stats.used[A_STR];
    base_to_hit += inventory[EQUIPMENT_ARM].weight / 2;
    base_to_hit += py.misc.weight / 10;

    if (!monster.lit) {
        base_to_hit /= 2;
        base_to_hit -= py.stats.used[A_DEX] * (BTH_PER_PLUS_TO_HIT_ADJUST - 1);
        base_to_hit -= py.misc.level * class_level_adj[py.misc.class_id][CLASS_BTH] / 2;
    }

    if (playerTestBeingHit(base_to_hit, (int) py.misc.level, (int) py.stats.used[A_DEX], (int) creature.ac, CLASS_BTH)) {
        vtype_t msg = {'\0'};
        (void) sprintf(msg, "You hit %s.", name);
        printMessage(msg);

        int damage = diceRoll(inventory[EQUIPMENT_ARM].damage);
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
            if ((creature.defenses & CD_MAX_HP) != 0) {
                avg_max_hp = maxDiceRoll(creature.hit_die);
            } else {
                // TODO: use maxDiceRoll(), just be careful about the bit shift
                avg_max_hp = (creature.hit_die.dice * (creature.hit_die.sides + 1)) >> 1;
            }

            if (100 + randomNumber(400) + randomNumber(400) > monster.hp + avg_max_hp) {
                monster.stunned_amount += randomNumber(3) + 1;
                if (monster.stunned_amount > 24) {
                    monster.stunned_amount = 24;
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

static void playerBashClosedDoor(int y, int x, int dir, Tile_t &tile, Inventory_t &item) {
    printMessageNoCommandInterrupt("You smash into the door!");

    int chance = py.stats.used[A_STR] + py.misc.weight / 2;

    // Use (roughly) similar method as for monsters.
    auto abs_misc_use = (int) std::abs((std::intmax_t) item.misc_use);
    if (randomNumber(chance * (20 + abs_misc_use)) < 10 * (chance - abs_misc_use)) {
        printMessage("The door crashes open!");

        inventoryItemCopyTo(OBJ_OPEN_DOOR, treasure_list[tile.treasure_id]);

        // 50% chance of breaking door
        item.misc_use = (int16_t) (1 - randomNumber(2));

        tile.feature_id = TILE_CORR_FLOOR;

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

    if (game.command_count == 0) {
        printMessage("The door holds firm.");
    }
}

static void playerBashClosedChest(Inventory_t &item) {
    if (randomNumber(10) == 1) {
        printMessage("You have destroyed the chest.");
        printMessage("and its contents!");

        item.id = OBJ_RUINED_CHEST;
        item.flags = 0;

        return;
    }

    if (((item.flags & CH_LOCKED) != 0u) && randomNumber(10) == 1) {
        printMessage("The lock breaks open!");

        item.flags &= ~CH_LOCKED;

        return;
    }

    printMessageNoCommandInterrupt("The chest holds firm.");
}
