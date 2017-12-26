// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Code for mage spells

#include "headers.h"
#include "externs.h"

// names based on spell_names[62] in data_player.cpp
enum class MageSpellTypes {
    magic_missile = 1,
    detect_monsters,
    phase_door,
    light_area,
    cure_light_wounds,
    find_hidden_traps_doors,
    stinking_cloud,
    confusion,
    lightning_bolt,
    trap_door_destruction,
    sleep_i,
    cure_poison,
    teleport_self,
    remove_curse,
    frost_bolt,
    wall_to_mud,
    create_food,
    recharge_item_1,
    sleep_ii,
    polymorph_other,
    identify_item,
    sleep_iii,
    fire_bolt,
    speed_monster,
    frost_ball,
    recharge_item_ii,
    teleport_other,
    haste_self,
    fire_ball,
    word_of_destruction,
    genocide,
};

static bool canReadSpells() {
    if (py.flags.blind > 0) {
        printMessage("You can't see to read your spell book!");
        return false;
    }

    if (playerNoLight()) {
        printMessage("You have no light to read by.");
        return false;
    }

    if (py.flags.confused > 0) {
        printMessage("You are too confused.");
        return false;
    }

    if (classes[py.misc.class_id].class_to_use_mage_spells != SPELL_TYPE_MAGE) {
        printMessage("You can't cast spells!");
        return false;
    }

    return true;
}

static void castSpell(int spell_id) {
    int dir;

    switch ((MageSpellTypes) spell_id) {
        case MageSpellTypes::magic_missile :
            if (getDirectionWithMemory(CNIL, dir)) {
                spellFireBolt(py.row, py.col, dir, diceRoll(Dice_t{2, 6}), GF_MAGIC_MISSILE, spell_names[0]);
            }
            break;
        case MageSpellTypes::detect_monsters:
            (void) spellDetectMonsters();
            break;
        case MageSpellTypes::phase_door:
            playerTeleport(10);
            break;
        case MageSpellTypes::light_area:
            (void) spellLightArea(py.row, py.col);
            break;
        case MageSpellTypes::cure_light_wounds:
            (void) spellChangePlayerHitPoints(diceRoll(Dice_t{4, 4}));
            break;
        case MageSpellTypes::find_hidden_traps_doors:
            (void) spellDetectSecretDoorssWithinVicinity();
            (void) spellDetectTrapsWithinVicinity();
            break;
        case MageSpellTypes::stinking_cloud:
            if (getDirectionWithMemory(CNIL, dir)) {
                spellFireBall(py.row, py.col, dir, 12, GF_POISON_GAS, spell_names[6]);
            }
            break;
        case MageSpellTypes::confusion:
            if (getDirectionWithMemory(CNIL, dir)) {
                (void) spellConfuseMonster(py.row, py.col, dir);
            }
            break;
        case MageSpellTypes::lightning_bolt:
            if (getDirectionWithMemory(CNIL, dir)) {
                spellFireBolt(py.row, py.col, dir, diceRoll(Dice_t{4, 8}), GF_LIGHTNING, spell_names[8]);
            }
            break;
        case MageSpellTypes::trap_door_destruction:
            (void) spellDestroyAdjacentDoorsTraps();
            break;
        case MageSpellTypes::sleep_i:
            if (getDirectionWithMemory(CNIL, dir)) {
                (void) spellSleepMonster(py.row, py.col, dir);
            }
            break;
        case MageSpellTypes::cure_poison:
            (void) playerCurePoison();
            break;
        case MageSpellTypes::teleport_self:
            playerTeleport((py.misc.level * 5));
            break;
        case MageSpellTypes::remove_curse:
            for (int id = 22; id < PLAYER_INVENTORY_SIZE; id++) {
                inventory[id].flags = (uint32_t) (inventory[id].flags & ~TR_CURSED);
            }
            break;
        case MageSpellTypes::frost_bolt:
            if (getDirectionWithMemory(CNIL, dir)) {
                spellFireBolt(py.row, py.col, dir, diceRoll(Dice_t{6, 8}), GF_FROST, spell_names[14]);
            }
            break;
        case MageSpellTypes::wall_to_mud:
            if (getDirectionWithMemory(CNIL, dir)) {
                (void) spellWallToMud(py.row, py.col, dir);
            }
            break;
        case MageSpellTypes::create_food:
            spellCreateFood();
            break;
        case MageSpellTypes::recharge_item_1:
            (void) spellRechargeItem(20);
            break;
        case MageSpellTypes::sleep_ii:
            (void) monsterSleep(py.row, py.col);
            break;
        case MageSpellTypes::polymorph_other:
            if (getDirectionWithMemory(CNIL, dir)) {
                (void) spellPolymorphMonster(py.row, py.col, dir);
            }
            break;
        case MageSpellTypes::identify_item:
            (void) spellIdentifyItem();
            break;
        case MageSpellTypes::sleep_iii:
            (void) spellSleepAllMonsters();
            break;
        case MageSpellTypes::fire_bolt:
            if (getDirectionWithMemory(CNIL, dir)) {
                spellFireBolt(py.row, py.col, dir, diceRoll(Dice_t{9, 8}), GF_FIRE, spell_names[22]);
            }
            break;
        case MageSpellTypes::speed_monster:
            if (getDirectionWithMemory(CNIL, dir)) {
                (void) spellSpeedMonster(py.row, py.col, dir, -1);
            }
            break;
        case MageSpellTypes::frost_ball:
            if (getDirectionWithMemory(CNIL, dir)) {
                spellFireBall(py.row, py.col, dir, 48, GF_FROST, spell_names[24]);
            }
            break;
        case MageSpellTypes::recharge_item_ii:
            (void) spellRechargeItem(60);
            break;
        case MageSpellTypes::teleport_other:
            if (getDirectionWithMemory(CNIL, dir)) {
                (void) spellTeleportAwayMonsterInDirection(py.row, py.col, dir);
            }
            break;
        case MageSpellTypes::haste_self:
            py.flags.fast += randomNumber(20) + py.misc.level;
            break;
        case MageSpellTypes::fire_ball:
            if (getDirectionWithMemory(CNIL, dir)) {
                spellFireBall(py.row, py.col, dir, 72, GF_FIRE, spell_names[28]);
            }
            break;
        case MageSpellTypes::word_of_destruction:
            spellDestroyArea(py.row, py.col);
            break;
        case MageSpellTypes::genocide:
            (void) spellGenocide();
            break;
        default:
            break;
    }
}

// Throw a magic spell -RAK-
void getAndCastMagicSpell() {
    game.player_free_turn = true;

    if (!canReadSpells()) {
        return;
    }

    int i, j;
    if (!inventoryFindRange(TV_MAGIC_BOOK, TV_NEVER, i, j)) {
        printMessage("But you are not carrying any spell-books!");
        return;
    }

    int item_val;
    if (!inventoryGetInputForItemId(item_val, "Use which spell-book?", i, j, CNIL, CNIL)) {
        return;
    }

    int choice, chance;
    int result = castSpellGetId("Cast which spell?", item_val, choice, chance);
    if (result < 0) {
        printMessage("You don't know any spells in that book.");
        return;
    }
    if (result == 0) {
        return;
    }

    game.player_free_turn = false;

    const Spell_t &magic_spell = magic_spells[py.misc.class_id - 1][choice];

    if (randomNumber(100) < chance) {
        printMessage("You failed to get the spell off!");
    } else {
        castSpell(choice + 1);

        if ((py.flags.spells_worked & (1L << choice)) == 0) {
            py.misc.exp += magic_spell.exp_gain_for_learning << 2;
            py.flags.spells_worked |= (1L << choice);

            displayCharacterExperience();
        }
    }

    if (magic_spell.mana_required > py.misc.current_mana) {
        printMessage("You faint from the effort!");

        py.flags.paralysis = (int16_t) randomNumber((5 * (magic_spell.mana_required - py.misc.current_mana)));
        py.misc.current_mana = 0;
        py.misc.current_mana_fraction = 0;

        if (randomNumber(3) == 1) {
            printMessage("You have damaged your health!");
            (void) playerStatRandomDecrease(A_CON);
        }
    } else {
        py.misc.current_mana -= magic_spell.mana_required;
    }

    printCharacterCurrentMana();
}

// Returns spell chance of failure for class_to_use_mage_spells -RAK-
int spellChanceOfSuccess(int spell_id) {
    const Spell_t &spell = magic_spells[py.misc.class_id - 1][spell_id];

    int chance = spell.failure_chance - 3 * (py.misc.level - spell.level_required);

    int stat;
    if (classes[py.misc.class_id].class_to_use_mage_spells == SPELL_TYPE_MAGE) {
        stat = A_INT;
    } else {
        stat = A_WIS;
    }

    chance -= 3 * (playerStatAdjustmentWisdomIntelligence(stat) - 1);

    if (spell.mana_required > py.misc.current_mana) {
        chance += 5 * (spell.mana_required - py.misc.current_mana);
    }

    if (chance > 95) {
        chance = 95;
    } else if (chance < 5) {
        chance = 5;
    }

    return chance;
}
