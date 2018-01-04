// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// Player functions related to traps

#include "headers.h"

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

static void playerDisarmChestTrap(int y, int x, int total, Inventory_t &item) {
    if (!spellItemIdentified(item)) {
        game.player_free_turn = true;
        printMessage("I don't see a trap.");

        return;
    }

    if ((item.flags & CH_TRAPPED) != 0u) {
        int level = item.depth_first_found;

        if ((total - level) > randomNumber(100)) {
            item.flags &= ~CH_TRAPPED;

            if ((item.flags & CH_LOCKED) != 0u) {
                item.special_name_id = SN_LOCKED;
            } else {
                item.special_name_id = SN_DISARMED;
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
    game.player_free_turn = true;
}

// Disarms a trap -RAK-
void playerDisarmTrap() {
    int dir;
    if (!getDirectionWithMemory(CNIL, dir)) {
        return;
    }

    int y = py.row;
    int x = py.col;
    (void) playerMovePosition(dir, y, x);

    Tile_t const &tile = dg.floor[y][x];

    bool no_disarm = false;

    if (tile.creature_id > 1 && tile.treasure_id != 0 && (treasure_list[tile.treasure_id].category_id == TV_VIS_TRAP || treasure_list[tile.treasure_id].category_id == TV_CHEST)) {
        objectBlockedByMonster(tile.creature_id);
    } else if (tile.treasure_id != 0) {
        int disarm_ability = playerTrapDisarmAbility();

        Inventory_t &item = treasure_list[tile.treasure_id];

        if (item.category_id == TV_VIS_TRAP) {
            playerDisarmFloorTrap(y, x, disarm_ability, item.depth_first_found, dir, item.misc_use);
        } else if (item.category_id == TV_CHEST) {
            playerDisarmChestTrap(y, x, disarm_ability, item);
        } else {
            no_disarm = true;
        }
    } else {
        no_disarm = true;
    }

    if (no_disarm) {
        printMessage("I do not see anything to disarm there.");
        game.player_free_turn = true;
    }
}

static void chestLooseStrength() {
    printMessage("A small needle has pricked you!");

    if (py.flags.sustain_str) {
        printMessage("You are unaffected.");
        return;
    }

    (void) playerStatRandomDecrease(A_STR);

    playerTakesHit(diceRoll(Dice_t{1, 4}), "a poison needle");

    printMessage("You feel weakened!");
}

static void chestPoison() {
    printMessage("A small needle has pricked you!");

    playerTakesHit(diceRoll(Dice_t{1, 6}), "a poison needle");

    py.flags.poisoned += 10 + randomNumber(20);
}

static void chestParalysed() {
    printMessage("A puff of yellow gas surrounds you!");

    if (py.flags.free_action) {
        printMessage("You are unaffected.");
        return;
    }

    printMessage("You choke and pass out.");
    py.flags.paralysis = (int16_t) (10 + randomNumber(20));
}

static void chestSummonMonster(int y, int x) {
    for (int i = 0; i < 3; i++) {
        int cy = y;
        int cx = x;
        (void) monsterSummon(cy, cx, false);
    }
}

static void chestExplode(int y, int x) {
    printMessage("There is a sudden explosion!");

    (void) dungeonDeleteObject(y, x);

    playerTakesHit(diceRoll(Dice_t{5, 8}), "an exploding chest");
}

// Chests have traps too. -RAK-
// Note: Chest traps are based on the FLAGS value
void chestTrap(int y, int x) {
    uint32_t flags = treasure_list[dg.floor[y][x].treasure_id].flags;

    if ((flags & CH_LOSE_STR) != 0u) {
        chestLooseStrength();
    }

    if ((flags & CH_POISON) != 0u) {
        chestPoison();
    }

    if ((flags & CH_PARALYSED) != 0u) {
        chestParalysed();
    }

    if ((flags & CH_SUMMON) != 0u) {
        chestSummonMonster(y, x);
    }

    if ((flags & CH_EXPLODE) != 0u) {
        chestExplode(y, x);
    }
}
