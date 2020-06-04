// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// Code for mage spells

#include "headers.h"

// names based on spell_names[62] in data_player.cpp
enum class MageSpellId {
    MagicMissile = 1,
    DetectMonsters,
    PhaseDoor,
    LightArea,
    CureLightWounds,
    FindHiddenTrapsDoors,
    StinkingCloud,
    Confusion,
    LightningBolt,
    TrapDoorDestruction,
    Sleep1,
    CurePoison,
    TeleportSelf,
    RemoveCurse,
    FrostBolt,
    WallToMud,
    CreateFood,
    RechargeItem1,
    Sleep2,
    PolymorphOther,
    IdentifyItem,
    Sleep3,
    FireBolt,
    SpeedMonster,
    FrostBall,
    RechargeItem2,
    TeleportOther,
    HasteSelf,
    FireBall,
    WordOfDestruction,
    Genocide,
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

    if (classes[py.misc.class_id].class_to_use_mage_spells != config::spells::SPELL_TYPE_MAGE) {
        printMessage("You can't cast spells!");
        return false;
    }

    return true;
}

static void castSpell(int spell_id) {
    int dir;

    switch ((MageSpellId) spell_id) {
        case MageSpellId::MagicMissile:
            if (getDirectionWithMemory(CNIL, dir)) {
                spellFireBolt(py.pos, dir, diceRoll(Dice_t{2, 6}), MagicSpellFlags::MagicMissile, spell_names[0]);
            }
            break;
        case MageSpellId::DetectMonsters:
            (void) spellDetectMonsters();
            break;
        case MageSpellId::PhaseDoor:
            playerTeleport(10);
            break;
        case MageSpellId::LightArea:
            (void) spellLightArea(py.pos);
            break;
        case MageSpellId::CureLightWounds:
            (void) spellChangePlayerHitPoints(diceRoll(Dice_t{4, 4}));
            break;
        case MageSpellId::FindHiddenTrapsDoors:
            (void) spellDetectSecretDoorssWithinVicinity();
            (void) spellDetectTrapsWithinVicinity();
            break;
        case MageSpellId::StinkingCloud:
            if (getDirectionWithMemory(CNIL, dir)) {
                spellFireBall(py.pos, dir, 12, MagicSpellFlags::PoisonGas, spell_names[6]);
            }
            break;
        case MageSpellId::Confusion:
            if (getDirectionWithMemory(CNIL, dir)) {
                (void) spellConfuseMonster(py.pos, dir);
            }
            break;
        case MageSpellId::LightningBolt:
            if (getDirectionWithMemory(CNIL, dir)) {
                spellFireBolt(py.pos, dir, diceRoll(Dice_t{4, 8}), MagicSpellFlags::Lightning, spell_names[8]);
            }
            break;
        case MageSpellId::TrapDoorDestruction:
            (void) spellDestroyAdjacentDoorsTraps();
            break;
        case MageSpellId::Sleep1:
            if (getDirectionWithMemory(CNIL, dir)) {
                (void) spellSleepMonster(py.pos, dir);
            }
            break;
        case MageSpellId::CurePoison:
            (void) playerCurePoison();
            break;
        case MageSpellId::TeleportSelf:
            playerTeleport((py.misc.level * 5));
            break;
        case MageSpellId::RemoveCurse:
            for (int id = 22; id < PLAYER_INVENTORY_SIZE; id++) {
                py.inventory[id].flags = (uint32_t)(py.inventory[id].flags & ~config::treasure::flags::TR_CURSED);
            }
            break;
        case MageSpellId::FrostBolt:
            if (getDirectionWithMemory(CNIL, dir)) {
                spellFireBolt(py.pos, dir, diceRoll(Dice_t{6, 8}), MagicSpellFlags::Frost, spell_names[14]);
            }
            break;
        case MageSpellId::WallToMud:
            if (getDirectionWithMemory(CNIL, dir)) {
                (void) spellWallToMud(py.pos, dir);
            }
            break;
        case MageSpellId::CreateFood:
            spellCreateFood();
            break;
        case MageSpellId::RechargeItem1:
            (void) spellRechargeItem(20);
            break;
        case MageSpellId::Sleep2:
            (void) monsterSleep(py.pos);
            break;
        case MageSpellId::PolymorphOther:
            if (getDirectionWithMemory(CNIL, dir)) {
                (void) spellPolymorphMonster(py.pos, dir);
            }
            break;
        case MageSpellId::IdentifyItem:
            (void) spellIdentifyItem();
            break;
        case MageSpellId::Sleep3:
            (void) spellSleepAllMonsters();
            break;
        case MageSpellId::FireBolt:
            if (getDirectionWithMemory(CNIL, dir)) {
                spellFireBolt(py.pos, dir, diceRoll(Dice_t{9, 8}), MagicSpellFlags::Fire, spell_names[22]);
            }
            break;
        case MageSpellId::SpeedMonster:
            if (getDirectionWithMemory(CNIL, dir)) {
                (void) spellSpeedMonster(py.pos, dir, -1);
            }
            break;
        case MageSpellId::FrostBall:
            if (getDirectionWithMemory(CNIL, dir)) {
                spellFireBall(py.pos, dir, 48, MagicSpellFlags::Frost, spell_names[24]);
            }
            break;
        case MageSpellId::RechargeItem2:
            (void) spellRechargeItem(60);
            break;
        case MageSpellId::TeleportOther:
            if (getDirectionWithMemory(CNIL, dir)) {
                (void) spellTeleportAwayMonsterInDirection(py.pos, dir);
            }
            break;
        case MageSpellId::HasteSelf:
            py.flags.fast += randomNumber(20) + py.misc.level;
            break;
        case MageSpellId::FireBall:
            if (getDirectionWithMemory(CNIL, dir)) {
                spellFireBall(py.pos, dir, 72, MagicSpellFlags::Fire, spell_names[28]);
            }
            break;
        case MageSpellId::WordOfDestruction:
            spellDestroyArea(py.pos);
            break;
        case MageSpellId::Genocide:
            (void) spellGenocide();
            break;
        default:
            // All cases are handled, so this should never be reached!
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

    Spell_t const &magic_spell = magic_spells[py.misc.class_id - 1][choice];

    if (randomNumber(100) < chance) {
        printMessage("You failed to get the spell off!");
    } else {
        castSpell(choice + 1);

        if (!game.player_free_turn && (py.flags.spells_worked & (1L << choice)) == 0) {
            py.misc.exp += magic_spell.exp_gain_for_learning << 2;
            py.flags.spells_worked |= (1L << choice);

            displayCharacterExperience();
        }
    }

    if (game.player_free_turn) {
        return;
    }

    if (magic_spell.mana_required > py.misc.current_mana) {
        printMessage("You faint from the effort!");

        py.flags.paralysis = (int16_t) randomNumber((5 * (magic_spell.mana_required - py.misc.current_mana)));
        py.misc.current_mana = 0;
        py.misc.current_mana_fraction = 0;

        if (randomNumber(3) == 1) {
            printMessage("You have damaged your health!");
            (void) playerStatRandomDecrease(PlayerAttr::A_CON);
        }
    } else {
        py.misc.current_mana -= magic_spell.mana_required;
    }

    printCharacterCurrentMana();
}

// Returns spell chance of failure for class_to_use_mage_spells -RAK-
int spellChanceOfSuccess(int spell_id) {
    Spell_t const &spell = magic_spells[py.misc.class_id - 1][spell_id];

    int chance = spell.failure_chance - 3 * (py.misc.level - spell.level_required);

    int stat;
    if (classes[py.misc.class_id].class_to_use_mage_spells == config::spells::SPELL_TYPE_MAGE) {
        stat = PlayerAttr::A_INT;
    } else {
        stat = PlayerAttr::A_WIS;
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
