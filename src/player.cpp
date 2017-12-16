// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Player specific variable definitions

#include "headers.h"
#include "externs.h"

// Player record for most player related info
Player_t py = Player_t{};

bool playerIsMale() {
    return py.misc.gender;
}

void playerSetGender(bool is_male) {
    py.misc.gender = is_male;
}

const char *playerGetGenderLabel() {
    if (playerIsMale()) {
        return "Male";
    }
    return "Female";
}

// I don't really like this, but for now, I like it better than being a naked global -MRC-
void playerInitializeBaseExperienceLevels() {
    // TODO: load from external data file
    uint32_t levels[PLAYER_MAX_LEVEL] = {
            10,      25,     45,      70,       100,      140,      200,       280,
            380,     500,    650,     850,      1100,     1400,     1800,      2300,
            2900,    3600,   4400,    5400,      6800,     8400,    10200,     12500,
            17500,   25000,  35000L, 50000L,    75000L,  100000L,  150000L,   200000L,
            300000L, 400000L, 500000L, 750000L, 1500000L, 2500000L, 5000000L, 10000000L,
    };

    for (auto i = 0; i < PLAYER_MAX_LEVEL; i++) {
        py.base_exp_levels[i] = levels[i];
    }
}

// Calculate the players hit points
void playerCalculateHitPoints() {
    int hp = py.base_hp_levels[py.misc.level - 1] + (playerStatAdjustmentConstitution() * py.misc.level);

    // Always give at least one point per level + 1
    if (hp < (py.misc.level + 1)) {
        hp = py.misc.level + 1;
    }

    if ((py.flags.status & PY_HERO) != 0u) {
        hp += 10;
    }

    if ((py.flags.status & PY_SHERO) != 0u) {
        hp += 20;
    }

    // MHP can equal zero while character is being created
    if (hp != py.misc.max_hp && py.misc.max_hp != 0) {
        // Change current hit points proportionately to change of MHP,
        // divide first to avoid overflow, little loss of accuracy
        int32_t value = (((int32_t) py.misc.current_hp << 16) + py.misc.current_hp_fraction) / py.misc.max_hp * hp;
        py.misc.current_hp = (int16_t) (value >> 16);
        py.misc.current_hp_fraction = (uint16_t) (value & 0xFFFF);
        py.misc.max_hp = (int16_t) hp;

        // can't print hit points here, may be in store or inventory mode
        py.flags.status |= PY_HP;
    }
}

static int playerAttackBlowsDexterity(int dexterity) {
    int dex;

    if (dexterity < 10) {
        dex = 0;
    } else if (dexterity < 19) {
        dex = 1;
    } else if (dexterity < 68) {
        dex = 2;
    } else if (dexterity < 108) {
        dex = 3;
    } else if (dexterity < 118) {
        dex = 4;
    } else {
        dex = 5;
    }

    return dex;
}

static int playerAttackBlowsStrength(int strength, int weight) {
    int adj_weight = (strength * 10 / weight);

    int str;

    if (adj_weight < 2) {
        str = 0;
    } else if (adj_weight < 3) {
        str = 1;
    } else if (adj_weight < 4) {
        str = 2;
    } else if (adj_weight < 5) {
        str = 3;
    } else if (adj_weight < 7) {
        str = 4;
    } else if (adj_weight < 9) {
        str = 5;
    } else {
        str = 6;
    }

    return str;
}

// Weapon weight VS strength and dexterity -RAK-
int playerAttackBlows(int weight, int &weight_to_hit) {
    weight_to_hit = 0;

    int player_strength = py.stats.used[A_STR];

    if (player_strength * 15 < weight) {
        weight_to_hit = player_strength * 15 - weight;
        return 1;
    }

    int dexterity = playerAttackBlowsDexterity(py.stats.used[A_DEX]);
    int strength = playerAttackBlowsStrength(player_strength, weight);

    return (int) blows_table[strength][dexterity];
}

// Given direction "dir", returns new row, column location -RAK-
bool playerMovePosition(int dir, int &new_y, int &new_x) {
    int new_row;
    int new_col;

    switch (dir) {
        case 1:
            new_row = new_y + 1;
            new_col = new_x - 1;
            break;
        case 2:
            new_row = new_y + 1;
            new_col = new_x;
            break;
        case 3:
            new_row = new_y + 1;
            new_col = new_x + 1;
            break;
        case 4:
            new_row = new_y;
            new_col = new_x - 1;
            break;
        case 5:
            new_row = new_y;
            new_col = new_x;
            break;
        case 6:
            new_row = new_y;
            new_col = new_x + 1;
            break;
        case 7:
            new_row = new_y - 1;
            new_col = new_x - 1;
            break;
        case 8:
            new_row = new_y - 1;
            new_col = new_x;
            break;
        case 9:
            new_row = new_y - 1;
            new_col = new_x + 1;
            break;
        default:
            new_row = 0;
            new_col = 0;
            break;
    }

    bool can_move = false;

    if (new_row >= 0 && new_row < dg.height && new_col >= 0 && new_col < dg.width) {
        new_y = new_row;
        new_x = new_col;
        can_move = true;
    }

    return can_move;
}

// Teleport the player to a new location -RAK-
void playerTeleport(int new_distance) {
    int new_y, new_x;

    do {
        new_y = randomNumber(dg.height) - 1;
        new_x = randomNumber(dg.width) - 1;

        while (coordDistanceBetween(Coord_t{new_y, new_x}, Coord_t{py.row, py.col}) > new_distance) {
            new_y += (py.row - new_y) / 2;
            new_x += (py.col - new_x) / 2;
        }
    } while (dg.floor[new_y][new_x].feature_id >= MIN_CLOSED_SPACE || dg.floor[new_y][new_x].creature_id >= 2);

    dungeonMoveCreatureRecord(py.row, py.col, new_y, new_x);

    for (int y = py.row - 1; y <= py.row + 1; y++) {
        for (int x = py.col - 1; x <= py.col + 1; x++) {
            dg.floor[y][x].temporary_light = false;
            dungeonLiteSpot(y, x);
        }
    }

    dungeonLiteSpot(py.row, py.col);

    py.row = (int16_t) new_y;
    py.col = (int16_t) new_x;

    dungeonResetView();
    updateMonsters(false);

    teleport_player = false;
}

// Returns true if player has no light -RAK-
bool playerNoLight() {
    return !dg.floor[py.row][py.col].temporary_light && !dg.floor[py.row][py.col].permanent_light;
}

// Something happens to disturb the player. -CJS-
// The first arg indicates a major disturbance, which affects search.
// The second arg indicates a light change.
void playerDisturb(int major_disturbance, int light_disturbance) {
    game.command_count = 0;

    if ((major_disturbance != 0) && ((py.flags.status & PY_SEARCH) != 0u)) {
        playerSearchOff();
    }

    if (py.flags.rest != 0) {
        playerRestOff();
    }

    if ((light_disturbance != 0) || (py.running_tracker != 0)) {
        py.running_tracker = 0;
        dungeonResetView();
    }

    flushInputBuffer();
}

// Search Mode enhancement -RAK-
void playerSearchOn() {
    playerChangeSpeed(1);

    py.flags.status |= PY_SEARCH;

    printCharacterMovementState();
    printCharacterSpeed();

    py.flags.food_digested++;
}

void playerSearchOff() {
    dungeonResetView();
    playerChangeSpeed(-1);

    py.flags.status &= ~PY_SEARCH;

    printCharacterMovementState();
    printCharacterSpeed();
    py.flags.food_digested--;
}

// Resting allows a player to safely restore his hp -RAK-
void playerRestOn() {
    int rest_num;

    if (game.command_count > 0) {
        rest_num = game.command_count;
        game.command_count = 0;
    } else {
        rest_num = 0;
        vtype_t rest_str = {'\0'};

        putStringClearToEOL("Rest for how long? ", Coord_t{0, 0});

        if (getStringInput(rest_str, Coord_t{0, 19}, 5)) {
            if (rest_str[0] == '*') {
                rest_num = -MAX_SHORT;
            } else {
                (void) stringToNumber(rest_str, rest_num);
            }
        }
    }

    // check for reasonable value, must be positive number
    // in range of a short, or must be -MAX_SHORT
    if (rest_num == -MAX_SHORT || (rest_num > 0 && rest_num <= MAX_SHORT)) {
        if ((py.flags.status & PY_SEARCH) != 0u) {
            playerSearchOff();
        }

        py.flags.rest = (int16_t) rest_num;
        py.flags.status |= PY_REST;
        printCharacterMovementState();
        py.flags.food_digested--;

        putStringClearToEOL("Press any key to stop resting...", Coord_t{0, 0});
        putQIO();

        return;
    }

    // Something went wrong
    if (rest_num != 0) {
        printMessage("Invalid rest count.");
    }
    messageLineClear();

    game.player_free_turn = true;
}

void playerRestOff() {
    py.flags.rest = 0;
    py.flags.status &= ~PY_REST;

    printCharacterMovementState();

    // flush last message, or delete "press any key" message
    printMessage(CNIL);

    py.flags.food_digested++;
}

bool executeAttackOnPlayer(uint8_t creature_level, int16_t &monster_hp, int monster_id, int attack_type, int damage, vtype_t death_description, bool noticed) {
    int item_pos_start;
    int item_pos_end;
    int32_t gold;

    switch (attack_type) {
        case 1: // Normal attack
            // round half-way case down
            damage -= ((py.misc.ac + py.misc.magical_ac) * damage) / 200;
            playerTakesHit(damage, death_description);
            break;
        case 2: // Lose Strength
            playerTakesHit(damage, death_description);
            if (py.flags.sustain_str) {
                printMessage("You feel weaker for a moment, but it passes.");
            } else if (randomNumber(2) == 1) {
                printMessage("You feel weaker.");
                (void) playerStatRandomDecrease(A_STR);
            } else {
                noticed = false;
            }
            break;
        case 3: // Confusion attack
            playerTakesHit(damage, death_description);
            if (randomNumber(2) == 1) {
                if (py.flags.confused < 1) {
                    printMessage("You feel confused.");
                    py.flags.confused += randomNumber((int) creature_level);
                } else {
                    noticed = false;
                }
                py.flags.confused += 3;
            } else {
                noticed = false;
            }
            break;
        case 4: // Fear attack
            playerTakesHit(damage, death_description);
            if (playerSavingThrow()) {
                printMessage("You resist the effects!");
            } else if (py.flags.afraid < 1) {
                printMessage("You are suddenly afraid!");
                py.flags.afraid += 3 + randomNumber((int) creature_level);
            } else {
                py.flags.afraid += 3;
                noticed = false;
            }
            break;
        case 5: // Fire attack
            printMessage("You are enveloped in flames!");
            damageFire(damage, death_description);
            break;
        case 6: // Acid attack
            printMessage("You are covered in acid!");
            damageAcid(damage, death_description);
            break;
        case 7: // Cold attack
            printMessage("You are covered with frost!");
            damageCold(damage, death_description);
            break;
        case 8: // Lightning attack
            printMessage("Lightning strikes you!");
            damageLightningBolt(damage, death_description);
            break;
        case 9: // Corrosion attack
            printMessage("A stinging red gas swirls about you.");
            damageCorrodingGas(death_description);
            playerTakesHit(damage, death_description);
            break;
        case 10: // Blindness attack
            playerTakesHit(damage, death_description);
            if (py.flags.blind < 1) {
                py.flags.blind += 10 + randomNumber((int) creature_level);
                printMessage("Your eyes begin to sting.");
            } else {
                py.flags.blind += 5;
                noticed = false;
            }
            break;
        case 11: // Paralysis attack
            playerTakesHit(damage, death_description);
            if (playerSavingThrow()) {
                printMessage("You resist the effects!");
            } else if (py.flags.paralysis < 1) {
                if (py.flags.free_action) {
                    printMessage("You are unaffected.");
                } else {
                    py.flags.paralysis = (int16_t) (randomNumber((int) creature_level) + 3);
                    printMessage("You are paralyzed.");
                }
            } else {
                noticed = false;
            }
            break;
        case 12: // Steal Money
            if (py.flags.paralysis < 1 && randomNumber(124) < py.stats.used[A_DEX]) {
                printMessage("You quickly protect your money pouch!");
            } else {
                gold = (py.misc.au / 10) + randomNumber(25);
                if (gold > py.misc.au) {
                    py.misc.au = 0;
                } else {
                    py.misc.au -= gold;
                }
                printMessage("Your purse feels lighter.");
                printCharacterGoldValue();
            }
            if (randomNumber(2) == 1) {
                printMessage("There is a puff of smoke!");
                spellTeleportAwayMonster(monster_id, MON_MAX_SIGHT);
            }
            break;
        case 13: // Steal Object
            if (py.flags.paralysis < 1 && randomNumber(124) < py.stats.used[A_DEX]) {
                printMessage("You grab hold of your backpack!");
            } else {
                inventoryDestroyItem(randomNumber(py.unique_inventory_items) - 1);
                printMessage("Your backpack feels lighter.");
            }
            if (randomNumber(2) == 1) {
                printMessage("There is a puff of smoke!");
                spellTeleportAwayMonster(monster_id, MON_MAX_SIGHT);
            }
            break;
        case 14: // Poison
            playerTakesHit(damage, death_description);
            printMessage("You feel very sick.");
            py.flags.poisoned += randomNumber((int) creature_level) + 5;
            break;
        case 15: // Lose dexterity
            playerTakesHit(damage, death_description);
            if (py.flags.sustain_dex) {
                printMessage("You feel clumsy for a moment, but it passes.");
            } else {
                printMessage("You feel more clumsy.");
                (void) playerStatRandomDecrease(A_DEX);
            }
            break;
        case 16: // Lose constitution
            playerTakesHit(damage, death_description);
            if (py.flags.sustain_con) {
                printMessage("Your body resists the effects of the disease.");
            } else {
                printMessage("Your health is damaged!");
                (void) playerStatRandomDecrease(A_CON);
            }
            break;
        case 17: // Lose intelligence
            playerTakesHit(damage, death_description);
            printMessage("You have trouble thinking clearly.");
            if (py.flags.sustain_int) {
                printMessage("But your mind quickly clears.");
            } else {
                (void) playerStatRandomDecrease(A_INT);
            }
            break;
        case 18: // Lose wisdom
            playerTakesHit(damage, death_description);
            if (py.flags.sustain_wis) {
                printMessage("Your wisdom is sustained.");
            } else {
                printMessage("Your wisdom is drained.");
                (void) playerStatRandomDecrease(A_WIS);
            }
            break;
        case 19: // Lose experience
            printMessage("You feel your life draining away!");
            spellLoseEXP(damage + (py.misc.exp / 100) * MON_PLAYER_EXP_DRAINED_PER_HIT);
            break;
        case 20: // Aggravate monster
            (void) spellAggravateMonsters(20);
            break;
        case 21: // Disenchant
            if (executeDisenchantAttack()) {
                printMessage("There is a static feeling in the air.");
                playerRecalculateBonuses();
            } else {
                noticed = false;
            }
            break;
        case 22: // Eat food
            if (inventoryFindRange(TV_FOOD, TV_NEVER, item_pos_start, item_pos_end)) {
                inventoryDestroyItem(item_pos_start);
                printMessage("It got at your rations!");
            } else {
                noticed = false;
            }
            break;
        case 23: // Eat light
            noticed = inventoryDiminishLightAttack(noticed);
            break;
        case 24: // Eat charges
            noticed = inventoryDiminishChargesAttack(creature_level, monster_hp, noticed);
            break;
        case 99:
            noticed = false;
            break;
        default:
            noticed = false;
            break;
    }

    return noticed;
}

// For "DIED_FROM" string
void playerDiedFromString(vtype_t *description, const char *monster_name, uint32_t move) {
    if ((move & CM_WIN) != 0u) {
        (void) sprintf(*description, "The %s", monster_name);
    } else if (isVowel(monster_name[0])) {
        (void) sprintf(*description, "an %s", monster_name);
    } else {
        (void) sprintf(*description, "a %s", monster_name);
    }
}

bool playerTestAttackHits(int attack_id, uint8_t level) {
    bool success = false;

    switch (attack_id) {
        case 1: // Normal attack
            if (playerTestBeingHit(60, (int) level, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
                success = true;
            }
            break;
        case 2: // Lose Strength
            if (playerTestBeingHit(-3, (int) level, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
                success = true;
            }
            break;
        case 3: // Confusion attack
            if (playerTestBeingHit(10, (int) level, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
                success = true;
            }
            break;
        case 4: // Fear attack
            if (playerTestBeingHit(10, (int) level, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
                success = true;
            }
            break;
        case 5: // Fire attack
            if (playerTestBeingHit(10, (int) level, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
                success = true;
            }
            break;
        case 6: // Acid attack
            if (playerTestBeingHit(0, (int) level, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
                success = true;
            }
            break;
        case 7: // Cold attack
            if (playerTestBeingHit(10, (int) level, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
                success = true;
            }
            break;
        case 8: // Lightning attack
            if (playerTestBeingHit(10, (int) level, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
                success = true;
            }
            break;
        case 9: // Corrosion attack
            if (playerTestBeingHit(0, (int) level, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
                success = true;
            }
            break;
        case 10: // Blindness attack
            if (playerTestBeingHit(2, (int) level, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
                success = true;
            }
            break;
        case 11: // Paralysis attack
            if (playerTestBeingHit(2, (int) level, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
                success = true;
            }
            break;
        case 12: // Steal Money
            if (playerTestBeingHit(5, (int) level, 0, (int) py.misc.level, CLASS_MISC_HIT) && py.misc.au > 0) {
                success = true;
            }
            break;
        case 13: // Steal Object
            if (playerTestBeingHit(2, (int) level, 0, (int) py.misc.level, CLASS_MISC_HIT) && py.unique_inventory_items > 0) {
                success = true;
            }
            break;
        case 14: // Poison
            if (playerTestBeingHit(5, (int) level, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
                success = true;
            }
            break;
        case 15: // Lose dexterity
            if (playerTestBeingHit(0, (int) level, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
                success = true;
            }
            break;
        case 16: // Lose constitution
            if (playerTestBeingHit(0, (int) level, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
                success = true;
            }
            break;
        case 17: // Lose intelligence
            if (playerTestBeingHit(2, (int) level, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
                success = true;
            }
            break;
        case 18: // Lose wisdom
            if (playerTestBeingHit(2, (int) level, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
                success = true;
            }
            break;
        case 19: // Lose experience
            if (playerTestBeingHit(5, (int) level, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
                success = true;
            }
            break;
        case 20: // Aggravate monsters
            success = true;
            break;
        case 21: // Disenchant
            if (playerTestBeingHit(20, (int) level, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
                success = true;
            }
            break;
        case 22: // Eat food
            if (playerTestBeingHit(5, (int) level, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
                success = true;
            }
            break;
        case 23: // Eat light
            if (playerTestBeingHit(5, (int) level, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
                success = true;
            }
            break;
        case 24: // Eat charges
            // check to make sure an object exists
            if (playerTestBeingHit(15, (int) level, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT) && py.unique_inventory_items > 0) {
                success = true;
            }
            break;
        case 99:
            success = true;
            break;
        default:
            break;
    }

    return success;
}
