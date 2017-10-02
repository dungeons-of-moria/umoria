// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Misc code, mainly to handle player commands

#include "headers.h"
#include "externs.h"

static void trapOpenPit(const Inventory_t &item, int dam) {
    printMessage("You fell into a pit!");

    if (py.flags.free_fall) {
        printMessage("You gently float down.");
        return;
    }

    obj_desc_t description = {'\0'};
    itemDescription(description, item, true);
    playerTakesHit(dam, description);
}

static void trapArrow(const Inventory_t &item, int dam) {
    if (playerTestBeingHit(125, 0, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
        obj_desc_t description = {'\0'};
        itemDescription(description, item, true);
        playerTakesHit(dam, description);

        printMessage("An arrow hits you.");
        return;
    }

    printMessage("An arrow barely misses you.");
}

static void trapCoveredPit(const Inventory_t &item, int dam, int y, int x) {
    printMessage("You fell into a covered pit.");

    if (py.flags.free_fall) {
        printMessage("You gently float down.");
    } else {
        obj_desc_t description = {'\0'};
        itemDescription(description, item, true);
        playerTakesHit(dam, description);
    }

    dungeonSetTrap(y, x, 0);
}

static void trapDoor(const Inventory_t &item, int dam) {
    generate_new_level = true;
    current_dungeon_level++;

    printMessage("You fell through a trap door!");

    if (py.flags.free_fall) {
        printMessage("You gently float down.");
    } else {
        obj_desc_t description = {'\0'};
        itemDescription(description, item, true);
        playerTakesHit(dam, description);
    }

    // Force the messages to display before starting to generate the next level.
    printMessage(CNIL);
}

static void trapSleepingGas() {
    if (py.flags.paralysis != 0) {
        return;
    }

    printMessage("A strange white mist surrounds you!");

    if (py.flags.free_action) {
        printMessage("You are unaffected.");
        return;
    }

    py.flags.paralysis += randomNumber(10) + 4;
    printMessage("You fall asleep.");
}

static void trapHiddenObject(int y, int x) {
    (void) dungeonDeleteObject(y, x);

    dungeonPlaceRandomObjectAt(y, x, false);

    printMessage("Hmmm, there was something under this rock.");
}

static void trapStrengthDart(const Inventory_t &item, int dam) {
    if (playerTestBeingHit(125, 0, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
        if (!py.flags.sustain_str) {
            (void) playerStatRandomDecrease(A_STR);

            obj_desc_t description = {'\0'};
            itemDescription(description, item, true);
            playerTakesHit(dam, description);

            printMessage("A small dart weakens you!");
        } else {
            printMessage("A small dart hits you.");
        }
    } else {
        printMessage("A small dart barely misses you.");
    }
}

static void trapTeleport(int y, int x) {
    teleport_player = true;

    printMessage("You hit a teleport trap!");

    // Light up the teleport trap, before we teleport away.
    dungeonMoveCharacterLight(y, x, y, x);
}

static void trapRockfall(int y, int x, int dam) {
    playerTakesHit(dam, "a falling rock");

    (void) dungeonDeleteObject(y, x);
    dungeonPlaceRubble(y, x);

    printMessage("You are hit by falling rock.");
}

static void trapCorrodeGas() {
    // Makes more sense to print the message first, then damage an object.
    printMessage("A strange red gas surrounds you.");

    damageCorrodingGas("corrosion gas");
}

static void trapSummonMonster(int y, int x) {
    // Rune disappears.
    (void) dungeonDeleteObject(y, x);

    int num = 2 + randomNumber(3);

    for (int i = 0; i < num; i++) {
        int ty = y;
        int tx = x;
        (void) monsterSummon(ty, tx, false);
    }
}

static void trapFire(int dam) {
    // Makes more sense to print the message first, then damage an object.
    printMessage("You are enveloped in flames!");

    damageFire(dam, "a fire trap");
}

static void trapAcid(int dam) {
    // Makes more sense to print the message first, then damage an object.
    printMessage("You are splashed with acid!");

    damageAcid(dam, "an acid trap");
}

static void trapPoisonGas(int dam) {
    // Makes more sense to print the message first, then damage an object.
    printMessage("A pungent green gas surrounds you!");

    damagePoisonedGas(dam, "a poison gas trap");
}

static void trapBlindGas() {
    printMessage("A black gas surrounds you!");

    py.flags.blind += randomNumber(50) + 50;
}

static void trapConfuseGas() {
    printMessage("A gas of scintillating colors surrounds you!");

    py.flags.confused += randomNumber(15) + 15;
}

static void trapSlowDart(const Inventory_t &item, int dam) {
    if (playerTestBeingHit(125, 0, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
        obj_desc_t description = {'\0'};
        itemDescription(description, item, true);
        playerTakesHit(dam, description);

        printMessage("A small dart hits you!");

        if (py.flags.free_action) {
            printMessage("You are unaffected.");
        } else {
            py.flags.slow += randomNumber(20) + 10;
        }
    } else {
        printMessage("A small dart barely misses you.");
    }
}

static void trapConstitutionDart(const Inventory_t &item, int dam) {
    if (playerTestBeingHit(125, 0, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
        if (!py.flags.sustain_con) {
            (void) playerStatRandomDecrease(A_CON);

            obj_desc_t description = {'\0'};
            itemDescription(description, item, true);
            playerTakesHit(dam, description);

            printMessage("A small dart saps your health!");
        } else {
            printMessage("A small dart hits you.");
        }
    } else {
        printMessage("A small dart barely misses you.");
    }
}

// Player hit a trap.  (Chuckle) -RAK-
static void playerStepsOnTrap(int y, int x) {
    playerEndRunning();
    dungeonChangeTrapVisibility(y, x);

    Inventory_t &item = treasure_list[cave[y][x].treasure_id];

    int damage = dicePlayerDamageRoll(item.damage);

    switch (item.sub_category_id) {
        case 1:
            // Open pit
            trapOpenPit(item, damage);
            break;
        case 2:
            // Arrow trap
            trapArrow(item, damage);
            break;
        case 3:
            // Covered pit
            trapCoveredPit(item, damage, y, x);
            break;
        case 4:
            // Trap door
            trapDoor(item, damage);
            break;
        case 5:
            // Sleep gas
            trapSleepingGas();
            break;
        case 6:
            // Hid Obj
            trapHiddenObject(y, x);
            break;
        case 7:
            // STR Dart
            trapStrengthDart(item, damage);
            break;
        case 8:
            // Teleport
            trapTeleport(y, x);
            break;
        case 9:
            // Rockfall
            trapRockfall(y, x, damage);
            break;
        case 10:
            // Corrode gas
            trapCorrodeGas();
            break;
        case 11:
            // Summon mon
            trapSummonMonster(y, x);
            break;
        case 12:
            // Fire trap
            trapFire(damage);
            break;
        case 13:
            // Acid trap
            trapAcid(damage);
            break;
        case 14:
            // Poison gas
            trapPoisonGas(damage);
            break;
        case 15:
            // Blind Gas
            trapBlindGas();
            break;
        case 16:
            // Confuse Gas
            trapConfuseGas();
            break;
        case 17:
            // Slow Dart
            trapSlowDart(item, damage);
            break;
        case 18:
            // CON Dart
            trapConstitutionDart(item, damage);
            break;
        case 19:
            // Secret Door
            break;
        case 99:
            // Scare Mon
            break;

            // Town level traps are special, the stores.
        case 101:
            // General
            storeEnter(0);
            break;
        case 102:
            // Armory
            storeEnter(1);
            break;
        case 103:
            // Weaponsmith
            storeEnter(2);
            break;
        case 104:
            // Temple
            storeEnter(3);
            break;
        case 105:
            // Alchemy
            storeEnter(4);
            break;
        case 106:
            // Magic-User
            storeEnter(5);
            break;

        default:
            printMessage("Unknown trap value.");
            break;
    }
}

// Return spell number and failure chance -RAK-
// returns -1 if no spells in book
// returns  1 if choose a spell in book to cast
// returns  0 if don't choose a spell, i.e. exit with an escape
int castSpellGetId(const char *prompt, int item_id, int &spell_id, int &spell_chance) {
    // NOTE: `flags` gets set again, since getAndClearFirstBit modified it
    uint32_t flags = inventory[item_id].flags;
    int first_spell = getAndClearFirstBit(flags);
    flags = inventory[item_id].flags & spells_learnt;

    // TODO(cook) move access to `magic_spells[]` directly to the for loop it's used in, below?
    Spell_t *spells = magic_spells[py.misc.class_id - 1];

    int spell_count = 0;
    int spell_list[31];

    while (flags != 0u) {
        int pos = getAndClearFirstBit(flags);

        if (spells[pos].level_required <= py.misc.level) {
            spell_list[spell_count] = pos;
            spell_count++;
        }
    }

    if (spell_count == 0) {
        return -1;
    }

    int result = 0;
    if (spellGetId(spell_list, spell_count, spell_id, spell_chance, prompt, first_spell)) {
        result = 1;
    }

    if ((result != 0) && magic_spells[py.misc.class_id - 1][spell_id].mana_required > py.misc.current_mana) {
        if (classes[py.misc.class_id].class_to_use_mage_spells == SPELL_TYPE_MAGE) {
            result = (int) getInputConfirmation("You summon your limited strength to cast this one! Confirm?");
        } else {
            result = (int) getInputConfirmation("The gods may think you presumptuous for this! Confirm?");
        }
    }

    return result;
}

// Player is on an object. Many things can happen based -RAK-
// on the TVAL of the object. Traps are set off, money and most objects
// are picked up. Some objects, such as open doors, just sit there.
static void carry(int y, int x, bool pickup) {
    Inventory_t &item = treasure_list[cave[y][x].treasure_id];

    int tileFlags = treasure_list[cave[y][x].treasure_id].category_id;

    if (tileFlags > TV_MAX_PICK_UP) {
        if (tileFlags == TV_INVIS_TRAP || tileFlags == TV_VIS_TRAP || tileFlags == TV_STORE_DOOR) {
            // OOPS!
            playerStepsOnTrap(y, x);
        }
        return;
    }

    obj_desc_t description = {'\0'};
    obj_desc_t msg = {'\0'};

    playerEndRunning();

    // There's GOLD in them thar hills!
    if (tileFlags == TV_GOLD) {
        py.misc.au += item.cost;

        itemDescription(description, item, true);
        (void) sprintf(msg, "You have found %d gold pieces worth of %s", item.cost, description);

        printCharacterGoldValue();
        (void) dungeonDeleteObject(y, x);

        printMessage(msg);

        return;
    }

    // Too many objects?
    if (inventoryCanCarryItemCount(item)) {
        // Okay,  pick it up
        if (pickup && config.prompt_to_pickup) {
            itemDescription(description, item, true);

            // change the period to a question mark
            description[strlen(description) - 1] = '?';
            (void) sprintf(msg, "Pick up %s", description);
            pickup = getInputConfirmation(msg);
        }

        // Check to see if it will change the players speed.
        if (pickup && !inventoryCanCarryItem(item)) {
            itemDescription(description, item, true);

            // change the period to a question mark
            description[strlen(description) - 1] = '?';
            (void) sprintf(msg, "Exceed your weight limit to pick up %s", description);
            pickup = getInputConfirmation(msg);
        }

        // Attempt to pick up an object.
        if (pickup) {
            int locn = inventoryCarryItem(item);

            itemDescription(description, inventory[locn], true);
            (void) sprintf(msg, "You have %s (%c)", description, locn + 'a');
            printMessage(msg);
            (void) dungeonDeleteObject(y, x);
        }
    } else {
        itemDescription(description, item, true);
        (void) sprintf(msg, "You can't carry %s", description);
        printMessage(msg);
    }
}

// Deletes a monster entry from the level -RAK-
void dungeonDeleteMonster(int id) {
    Monster_t &monster = monsters[id];

    cave[monster.y][monster.x].creature_id = 0;

    if (monster.lit) {
        dungeonLiteSpot((int) monster.y, (int) monster.x);
    }

    int last_id = next_free_monster_id - 1;

    if (id != last_id) {
        monster = monsters[last_id];
        cave[monster.y][monster.x].creature_id = (uint8_t) id;
        monsters[id] = monsters[last_id];
    }

    next_free_monster_id--;
    monsters[next_free_monster_id] = blank_monster;

    if (monster_multiply_total > 0) {
        monster_multiply_total--;
    }
}

// The following two procedures implement the same function as delete monster.
// However, they are used within updateMonsters(), because deleting a monster
// while scanning the monsters causes two problems, monsters might get two
// turns, and m_ptr/monptr might be invalid after the dungeonDeleteMonster.
// Hence the delete is done in two steps.
//
// dungeonDeleteMonsterFix1 does everything dungeonDeleteMonster does except delete
// the monster record and reduce next_free_monster_id, this is called in breathe, and
// a couple of places in creatures.c
void dungeonDeleteMonsterFix1(int id) {
    Monster_t &monster = monsters[id];

    // force the hp negative to ensure that the monster is dead, for example,
    // if the monster was just eaten by another, it will still have positive
    // hit points
    monster.hp = -1;

    cave[monster.y][monster.x].creature_id = 0;

    if (monster.lit) {
        dungeonLiteSpot((int) monster.y, (int) monster.x);
    }

    if (monster_multiply_total > 0) {
        monster_multiply_total--;
    }
}

// dungeonDeleteMonsterFix2 does everything in dungeonDeleteMonster that wasn't done
// by fix1_monster_delete above, this is only called in updateMonsters()
void dungeonDeleteMonsterFix2(int id) {
    int last_id = next_free_monster_id - 1;

    if (id != last_id) {
        int y = monsters[last_id].y;
        int x = monsters[last_id].x;
        cave[y][x].creature_id = (uint8_t) id;

        monsters[id] = monsters[last_id];
    }

    monsters[last_id] = blank_monster;
    next_free_monster_id--;
}

// Creates objects nearby the coordinates given -RAK-
static int dungeonSummonObject(int y, int x, int amount, int object_type) {
    int real_type;

    if (object_type == 1 || object_type == 5) {
        real_type = 1;   // object_type == 1 -> objects
    } else {
        real_type = 256; // object_type == 2 -> gold
    }

    int result = 0;

    do {
        for (int i = 0; i <= 20; i++) {
            int pos_y = y - 3 + randomNumber(5);
            int pos_x = x - 3 + randomNumber(5);

            if (coordInBounds(pos_y, pos_x) && los(y, x, pos_y, pos_x)) {
                if (cave[pos_y][pos_x].feature_id <= MAX_OPEN_SPACE && cave[pos_y][pos_x].treasure_id == 0) {
                    // object_type == 3 -> 50% objects, 50% gold
                    if (object_type == 3 || object_type == 7) {
                        if (randomNumber(100) < 50) {
                            real_type = 1;
                        } else {
                            real_type = 256;
                        }
                    }

                    if (real_type == 1) {
                        dungeonPlaceRandomObjectAt(pos_y, pos_x, (object_type >= 4));
                    } else {
                        dungeonPlaceGold(pos_y, pos_x);
                    }

                    dungeonLiteSpot(pos_y, pos_x);

                    if (caveTileVisible(pos_y, pos_x)) {
                        result += real_type;
                    }

                    i = 20;
                }
            }
        }

        amount--;
    } while (amount != 0);

    return result;
}

// Deletes object from given location -RAK-
bool dungeonDeleteObject(int y, int x) {
    Cave_t &tile = cave[y][x];

    if (tile.feature_id == TILE_BLOCKED_FLOOR) {
        tile.feature_id = TILE_CORR_FLOOR;
    }

    pusht(tile.treasure_id);

    tile.treasure_id = 0;
    tile.field_mark = false;

    dungeonLiteSpot(y, x);

    return caveTileVisible(y, x);
}

static int monsterDeathItemDropType(uint32_t flags) {
    int object;

    if ((flags & CM_CARRY_OBJ) != 0u) {
        object = 1;
    } else {
        object = 0;
    }

    if ((flags & CM_CARRY_GOLD) != 0u) {
        object += 2;
    }

    if ((flags & CM_SMALL_OBJ) != 0u) {
        object += 4;
    }

    return object;
}

static int monsterDeathItemDropCount(uint32_t flags) {
    int count = 0;

    if (((flags & CM_60_RANDOM) != 0u) && randomNumber(100) < 60) {
        count++;
    }

    if (((flags & CM_90_RANDOM) != 0u) && randomNumber(100) < 90) {
        count++;
    }

    if ((flags & CM_1D2_OBJ) != 0u) {
        count += randomNumber(2);
    }

    if ((flags & CM_2D2_OBJ) != 0u) {
        count += diceDamageRoll(2, 2);
    }

    if ((flags & CM_4D2_OBJ) != 0u) {
        count += diceDamageRoll(4, 2);
    }

    return count;
}

// Allocates objects upon a creatures death -RAK-
// Oh well,  another creature bites the dust. Reward the
// victor based on flags set in the main creature record.
//
// Returns a mask of bits from the given flags which indicates what the
// monster is seen to have dropped.  This may be added to monster memory.
uint32_t monsterDeath(int y, int x, uint32_t flags) {
    int item_type = monsterDeathItemDropType(flags);
    int item_count = monsterDeathItemDropCount(flags);

    uint32_t dropped_item_id = 0;

    if (item_count > 0) {
        dropped_item_id = (uint32_t) dungeonSummonObject(y, x, item_count, item_type);
    }

    // maybe the player died in mid-turn
    if (((flags & CM_WIN) != 0u) && !character_is_dead) {
        total_winner = true;

        printCharacterWinner();

        printMessage("*** CONGRATULATIONS *** You have won the game.");
        printMessage("You cannot save this game, but you may retire when ready.");
    }

    if (dropped_item_id == 0) {
        return 0;
    }

    uint32_t return_flags = 0;

    if ((dropped_item_id & 255) != 0u) {
        return_flags |= CM_CARRY_OBJ;

        if ((item_type & 0x04) != 0) {
            return_flags |= CM_SMALL_OBJ;
        }
    }

    if (dropped_item_id >= 256) {
        return_flags |= CM_CARRY_GOLD;
    }

    int number_of_items = (dropped_item_id % 256) + (dropped_item_id / 256);
    number_of_items = number_of_items << CM_TR_SHIFT;

    return return_flags | number_of_items;
}

static void playerGainKillExperience(const Creature_t &creature) {
    uint16_t exp = creature.kill_exp_value * creature.level;

    int32_t quotient = exp / py.misc.level;
    int32_t remainder = exp % py.misc.level;

    remainder *= 0x10000L;
    remainder /= py.misc.level;
    remainder += py.misc.exp_fraction;

    if (remainder >= 0x10000L) {
        quotient++;
        py.misc.exp_fraction = (uint16_t) (remainder - 0x10000L);
    } else {
        py.misc.exp_fraction = (uint16_t) remainder;
    }

    py.misc.exp += quotient;
}

// Decreases monsters hit points and deletes monster if needed.
// (Picking on my babies.) -RAK-
int monsterTakeHit(int monster_id, int damage) {
    Monster_t &monster = monsters[monster_id];
    const Creature_t &creature = creatures_list[monster.creature_id];

    monster.sleep_count = 0;
    monster.hp -= damage;

    if (monster.hp >= 0) {
        return -1;
    }

    uint32_t treasure_flags = monsterDeath((int) monster.y, (int) monster.x, creature.movement);

    Recall_t &memory = creature_recall[monster.creature_id];

    if ((py.flags.blind < 1 && monster.lit) || ((creature.movement & CM_WIN) != 0u)) {
        auto tmp = (uint32_t) ((memory.movement & CM_TREASURE) >> CM_TR_SHIFT);

        if (tmp > ((treasure_flags & CM_TREASURE) >> CM_TR_SHIFT)) {
            treasure_flags = (uint32_t) ((treasure_flags & ~CM_TREASURE) | (tmp << CM_TR_SHIFT));
        }

        memory.movement = (uint32_t) ((memory.movement & ~CM_TREASURE) | treasure_flags);

        if (memory.kills < MAX_SHORT) {
            memory.kills++;
        }
    }

    playerGainKillExperience(creature);

    // can't call displayCharacterExperience() here, as that would result in "new level"
    // message appearing before "monster dies" message.
    int m_take_hit = monster.creature_id;

    // in case this is called from within updateMonsters(), this is a horrible
    // hack, the monsters/updateMonsters() code needs to be rewritten.
    if (hack_monptr < monster_id) {
        dungeonDeleteMonster(monster_id);
    } else {
        dungeonDeleteMonsterFix1(monster_id);
    }

    return m_take_hit;
}

static void playerCalculateToHitBlows(int weapon_id, int weapon_weight, int &blows, int &total_to_hit) {
    if (weapon_id != TV_NOTHING) {
        // Proper weapon
        blows = playerAttackBlows(weapon_weight, total_to_hit);
    } else {
        // Bare hands?
        blows = 2;
        total_to_hit = -3;
    }

    // Fix for arrows
    if (weapon_id >= TV_SLING_AMMO && weapon_id <= TV_SPIKE) {
        blows = 1;
    }

    total_to_hit += py.misc.plusses_to_hit;
}

static int playerCalculateBaseToHit(bool creatureLit, int tot_tohit) {
    if (creatureLit) {
        return py.misc.bth;
    }

    // creature not lit, make it more difficult to hit
    int bth;

    bth = py.misc.bth / 2;
    bth -= tot_tohit * (BTH_PER_PLUS_TO_HIT_ADJUST - 1);
    bth -= py.misc.level * class_level_adj[py.misc.class_id][CLASS_BTH] / 2;

    return bth;
}

// Player attacks a (poor, defenseless) creature -RAK-
static void playerAttackMonster(int y, int x) {
    int creature_id = cave[y][x].creature_id;

    Monster_t &monster = monsters[creature_id];
    const Creature_t &creature = creatures_list[monster.creature_id];
    Inventory_t &item = inventory[EQUIPMENT_WIELD];

    monster.sleep_count = 0;

    // Does the player know what he's fighting?
    vtype_t name = {'\0'};
    if (!monster.lit) {
        (void) strcpy(name, "it");
    } else {
        (void) sprintf(name, "the %s", creature.name);
    }

    int blows, total_to_hit;
    playerCalculateToHitBlows(item.category_id, item.weight, blows, total_to_hit);

    int base_to_hit = playerCalculateBaseToHit(monster.lit, total_to_hit);

    int damage;
    vtype_t msg = {'\0'};

    // Loop for number of blows, trying to hit the critter.
    // Note: blows will always be greater than 0 at the start of the loop -MRC-
    for (int i = blows; i > 0; i--) {
        if (!playerTestBeingHit(base_to_hit, (int) py.misc.level, total_to_hit, (int) creature.ac, CLASS_BTH)) {
            (void) sprintf(msg, "You miss %s.", name);
            printMessage(msg);
            continue;
        }

        (void) sprintf(msg, "You hit %s.", name);
        printMessage(msg);

        if (item.category_id != TV_NOTHING) {
            damage = dicePlayerDamageRoll(item.damage);
            damage = itemMagicAbilityDamage(item, damage, monster.creature_id);
            damage = playerWeaponCriticalBlow((int) item.weight, total_to_hit, damage, CLASS_BTH);
        } else {
            // Bare hands!?
            damage = diceDamageRoll(1, 1);
            damage = playerWeaponCriticalBlow(1, 0, damage, CLASS_BTH);
        }

        damage += py.misc.plusses_to_damage;
        if (damage < 0) {
            damage = 0;
        }

        if (py.flags.confuse_monster) {
            py.flags.confuse_monster = false;

            printMessage("Your hands stop glowing.");

            if (((creature.defenses & CD_NO_SLEEP) != 0) || randomNumber(MON_MAX_LEVELS) < creature.level) {
                (void) sprintf(msg, "%s is unaffected.", name);
            } else {
                (void) sprintf(msg, "%s appears confused.", name);
                if (monster.confused_amount != 0u) {
                    monster.confused_amount += 3;
                } else {
                    monster.confused_amount = (uint8_t) (2 + randomNumber(16));
                }
            }
            printMessage(msg);

            if (monster.lit && randomNumber(4) == 1) {
                creature_recall[monster.creature_id].defenses |= creature.defenses & CD_NO_SLEEP;
            }
        }

        // See if we done it in.
        if (monsterTakeHit(creature_id, damage) >= 0) {
            (void) sprintf(msg, "You have slain %s.", name);
            printMessage(msg);
            displayCharacterExperience();

            return;
        }

        // Use missiles up
        if (item.category_id >= TV_SLING_AMMO && item.category_id <= TV_SPIKE) {
            item.items_count--;
            inventory_weight -= item.weight;
            py.flags.status |= PY_STR_WGT;

            if (item.items_count == 0) {
                equipment_count--;
                playerAdjustBonusesForItem(item, -1);
                inventoryItemCopyTo(OBJ_NOTHING, item);
                playerRecalculateBonuses();
            }
        }
    }
}

static bool playerRandomMovement(int dir) {
    // Never random if sitting
    if (dir == 5) {
        return false;
    }

    // 75% random movement
    bool playerRandomMove = randomNumber(4) > 1;

    bool playerIsConfused = py.flags.confused > 0;

    return playerIsConfused && playerRandomMove;
}

// Moves player from one space to another. -RAK-
void playerMove(int direction, bool do_pickup) {
    if (playerRandomMovement(direction)) {
        direction = randomNumber(9);
        playerEndRunning();
    }

    int y = char_row;
    int x = char_col;

    // Legal move?
    if (!playerMovePosition(direction, y, x)) {
        return;
    }

    const Cave_t &tile = cave[y][x];
    const Monster_t &monster = monsters[tile.creature_id];

    // if there is no creature, or an unlit creature in the walls then...
    // disallow attacks against unlit creatures in walls because moving into
    // a wall is a free turn normally, hence don't give player free turns
    // attacking each wall in an attempt to locate the invisible creature,
    // instead force player to tunnel into walls which always takes a turn
    if (tile.creature_id < 2 || (!monster.lit && tile.feature_id >= MIN_CLOSED_SPACE)) {
        // Open floor spot
        if (tile.feature_id <= MAX_OPEN_SPACE) {
            // Make final assignments of char coords
            int old_row = char_row;
            int old_col = char_col;

            char_row = (int16_t) y;
            char_col = (int16_t) x;

            // Move character record (-1)
            dungeonMoveCreatureRecord(old_row, old_col, char_row, char_col);

            // Check for new panel
            if (coordOutsidePanel(char_row, char_col, false)) {
                drawDungeonPanel();
            }

            // Check to see if he should stop
            if (running_counter != 0) {
                playerAreaAffect(direction, char_row, char_col);
            }

            // Check to see if he notices something
            // fos may be negative if have good rings of searching
            if (py.misc.fos <= 1 || randomNumber(py.misc.fos) == 1 || ((py.flags.status & PY_SEARCH) != 0u)) {
                dungeonSearch(char_row, char_col, py.misc.chance_in_search);
            }

            if (tile.feature_id == TILE_LIGHT_FLOOR) {
                // A room of light should be lit.

                if (!tile.permanent_light && (py.flags.blind == 0)) {
                    dungeonLightRoom(char_row, char_col);
                }
            } else if (tile.perma_lit_room && py.flags.blind < 1) {
                // In doorway of light-room?

                for (int row = (char_row - 1); row <= (char_row + 1); row++) {
                    for (int col = (char_col - 1); col <= (char_col + 1); col++) {
                        if (cave[row][col].feature_id == TILE_LIGHT_FLOOR && !cave[row][col].permanent_light) {
                            dungeonLightRoom(row, col);
                        }
                    }
                }
            }

            // Move the light source
            dungeonMoveCharacterLight(old_row, old_col, char_row, char_col);

            // An object is beneath him.
            if (tile.treasure_id != 0) {
                carry(char_row, char_col, do_pickup);

                // if stepped on falling rock trap, and space contains
                // rubble, then step back into a clear area
                if (treasure_list[tile.treasure_id].category_id == TV_RUBBLE) {
                    dungeonMoveCreatureRecord(char_row, char_col, old_row, old_col);
                    dungeonMoveCharacterLight(char_row, char_col, old_row, old_col);

                    char_row = (int16_t) old_row;
                    char_col = (int16_t) old_col;

                    // check to see if we have stepped back onto another trap, if so, set it off
                    uint8_t id = cave[char_row][char_col].treasure_id;
                    if (id != 0) {
                        int val = treasure_list[id].category_id;
                        if (val == TV_INVIS_TRAP || val == TV_VIS_TRAP || val == TV_STORE_DOOR) {
                            playerStepsOnTrap(char_row, char_col);
                        }
                    }
                }
            }
        } else {
            // Can't move onto floor space

            if ((running_counter == 0) && tile.treasure_id != 0) {
                if (treasure_list[tile.treasure_id].category_id == TV_RUBBLE) {
                    printMessage("There is rubble blocking your way.");
                } else if (treasure_list[tile.treasure_id].category_id == TV_CLOSED_DOOR) {
                    printMessage("There is a closed door blocking your way.");
                }
            } else {
                playerEndRunning();
            }
            player_free_turn = true;
        }
    } else {
        // Attacking a creature!

        int old_find_flag = running_counter;

        playerEndRunning();

        // if player can see monster, and was in find mode, then nothing
        if (monster.lit && (old_find_flag != 0)) {
            // did not do anything this turn
            player_free_turn = true;
        } else {
            playerAttackPosition(y, x);
        }
    }
}

static void chestLooseStrength() {
    printMessage("A small needle has pricked you!");

    if (py.flags.sustain_str) {
        printMessage("You are unaffected.");
        return;
    }

    (void) playerStatRandomDecrease(A_STR);

    playerTakesHit(diceDamageRoll(1, 4), "a poison needle");

    printMessage("You feel weakened!");
}

static void chestPoison() {
    printMessage("A small needle has pricked you!");

    playerTakesHit(diceDamageRoll(1, 6), "a poison needle");

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

    playerTakesHit(diceDamageRoll(5, 8), "an exploding chest");
}

// Chests have traps too. -RAK-
// Note: Chest traps are based on the FLAGS value
void chestTrap(int y, int x) {
    uint32_t flags = treasure_list[cave[y][x].treasure_id].flags;

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

static int16_t playerLockPickingSkill() {
    int16_t skill = py.misc.disarm;

    skill += 2;
    skill *= playerDisarmAdjustment();
    skill += playerStatAdjustmentWisdomIntelligence(A_INT);
    skill += class_level_adj[py.misc.class_id][CLASS_DISARM] * py.misc.level / 3;

    return skill;
}

static void openClosedDoor(int y, int x) {
    Cave_t &tile = cave[y][x];
    Inventory_t &item = treasure_list[tile.treasure_id];

    if (item.misc_use > 0) {
        // It's locked.

        if (py.flags.confused > 0) {
            printMessage("You are too confused to pick the lock.");
        } else if (playerLockPickingSkill() - item.misc_use > randomNumber(100)) {
            printMessage("You have picked the lock.");
            py.misc.exp++;
            displayCharacterExperience();
            item.misc_use = 0;
        } else {
            printMessageNoCommandInterrupt("You failed to pick the lock.");
        }
    } else if (item.misc_use < 0) {
        // It's stuck

        printMessage("It appears to be stuck.");
    }

    if (item.misc_use == 0) {
        inventoryItemCopyTo(OBJ_OPEN_DOOR, treasure_list[tile.treasure_id]);
        tile.feature_id = TILE_CORR_FLOOR;
        dungeonLiteSpot(y, x);
        command_count = 0;
    }
}

static void openClosedChest(int y, int x) {
    const Cave_t &tile = cave[y][x];
    Inventory_t &item = treasure_list[tile.treasure_id];

    bool success = false;

    if ((CH_LOCKED & item.flags) != 0u) {
        if (py.flags.confused > 0) {
            printMessage("You are too confused to pick the lock.");
        } else if (playerLockPickingSkill() - item.depth_first_found > randomNumber(100)) {
            printMessage("You have picked the lock.");

            py.misc.exp += item.depth_first_found;
            displayCharacterExperience();

            success = true;
        } else {
            printMessageNoCommandInterrupt("You failed to pick the lock.");
        }
    } else {
        success = true;
    }

    if (success) {
        item.flags &= ~CH_LOCKED;
        item.special_name_id = SN_EMPTY;
        spellItemIdentifyAndRemoveRandomInscription(item);
        item.cost = 0;
    }

    // Was chest still trapped?
    if ((CH_LOCKED & item.flags) != 0) {
        return;
    }

    // Oh, yes it was...   (Snicker)
    chestTrap(y, x);

    if (tile.treasure_id != 0) {
        // Chest treasure is allocated as if a creature had been killed.
        // clear the cursed chest/monster win flag, so that people
        // can not win by opening a cursed chest
        treasure_list[tile.treasure_id].flags &= ~TR_CURSED;

        (void) monsterDeath(y, x, treasure_list[tile.treasure_id].flags);

        treasure_list[tile.treasure_id].flags = 0;
    }
}

// Opens a closed door or closed chest. -RAK-
void objectOpen() {
    int dir;

    if (!getDirectionWithMemory(CNIL, dir)) {
        return;
    }

    int y = char_row;
    int x = char_col;
    (void) playerMovePosition(dir, y, x);

    bool no_object = false;

    const Cave_t &tile = cave[y][x];
    const Inventory_t &item = treasure_list[tile.treasure_id];

    if (tile.creature_id > 1 && tile.treasure_id != 0 && (item.category_id == TV_CLOSED_DOOR || item.category_id == TV_CHEST)) {
        objectBlockedByMonster(tile.creature_id);
    } else if (tile.treasure_id != 0) {
        if (item.category_id == TV_CLOSED_DOOR) {
            openClosedDoor(y, x);
        } else if (item.category_id == TV_CHEST) {
            openClosedChest(y, x);
        } else {
            no_object = true;
        }
    } else {
        no_object = true;
    }

    if (no_object) {
        player_free_turn = true;
        printMessage("I do not see anything you can open there.");
    }
}

// Closes an open door. -RAK-
void dungeonCloseDoor() {
    int dir;

    if (!getDirectionWithMemory(CNIL, dir)) {
        return;
    }

    int y = char_row;
    int x = char_col;
    (void) playerMovePosition(dir, y, x);

    Cave_t &tile = cave[y][x];
    Inventory_t &item = treasure_list[tile.treasure_id];

    bool no_object = false;

    if (tile.treasure_id != 0) {
        if (item.category_id == TV_OPEN_DOOR) {
            if (tile.creature_id == 0) {
                if (item.misc_use == 0) {
                    inventoryItemCopyTo(OBJ_CLOSED_DOOR, item);
                    tile.feature_id = TILE_BLOCKED_FLOOR;
                    dungeonLiteSpot(y, x);
                } else {
                    printMessage("The door appears to be broken.");
                }
            } else {
                objectBlockedByMonster(tile.creature_id);
            }
        } else {
            no_object = true;
        }
    } else {
        no_object = true;
    }

    if (no_object) {
        player_free_turn = true;
        printMessage("I do not see anything you can close there.");
    }
}

// Tunneling through real wall: 10, 11, 12 -RAK-
// Used by TUNNEL and WALL_TO_MUD
bool dungeonTunnelWall(int y, int x, int digging_ability, int digging_chance) {
    if (digging_ability <= digging_chance) {
        return false;
    }

    Cave_t &tile = cave[y][x];

    if (tile.perma_lit_room) {
        // Should become a room space, check to see whether
        // it should be TILE_LIGHT_FLOOR or TILE_DARK_FLOOR.
        bool found = false;

        for (int yy = y - 1; yy <= y + 1 && yy < MAX_HEIGHT; yy++) {
            for (int xx = x - 1; xx <= x + 1 && xx < MAX_WIDTH; xx++) {
                if (cave[yy][xx].feature_id <= MAX_CAVE_ROOM) {
                    tile.feature_id = cave[yy][xx].feature_id;
                    tile.permanent_light = cave[yy][xx].permanent_light;
                    found = true;
                    break;
                }
            }
        }

        if (!found) {
            tile.feature_id = TILE_CORR_FLOOR;
            tile.permanent_light = false;
        }
    } else {
        // should become a corridor space
        tile.feature_id = TILE_CORR_FLOOR;
        tile.permanent_light = false;
    }

    tile.field_mark = false;

    if (coordInsidePanel(y, x) && (tile.temporary_light || tile.permanent_light) && tile.treasure_id != 0) {
        printMessage("You have found something!");
    }

    dungeonLiteSpot(y, x);

    return true;
}

void objectBlockedByMonster(int monster_id) {
    vtype_t description = {'\0'};
    vtype_t msg = {'\0'};

    const Monster_t &monster = monsters[monster_id];
    const char *name = creatures_list[monster.creature_id].name;

    if (monster.lit) {
        (void) sprintf(description, "The %s", name);
    } else {
        (void) strcpy(description, "Something");
    }

    (void) sprintf(msg, "%s is in your way!", description);
    printMessage(msg);
}

// let the player attack the creature
void playerAttackPosition(int y, int x) {
    // Is a Coward?
    if (py.flags.afraid > 0) {
        printMessage("You are too afraid!");
        return;
    }

    playerAttackMonster(y, x);
}