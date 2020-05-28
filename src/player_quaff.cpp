// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// Code for potions

#include "headers.h"

enum class PotionSpellTypes {
    Strength = 1,
    Weakness,
    RestoreStrength,
    Intelligence,
    LoseIntelligence,
    RestoreIntelligence,
    Wisdom,
    LoseWisdom,
    RestoreWisdom,
    Charisma,
    Ugliness,
    RestoreCharisma,
    CureLightWounds,
    CureSeriousWounds,
    CureCriticalWounds,
    Healing,
    Constitution,
    GainExperience,
    Sleep,
    Blindness,
    Confusion,
    Poison,
    HasteSelf,
    Slowness,
    // 25 not used
    Dexterity = 26,
    RestoreDexterity,
    RestoreConstitution,
    CureBlindness,
    CureConfusion,
    CurePoison,
    // 32 not used
    // 33 not used
    LoseExperience = 34,
    SaltWater,
    Invulnerability,
    Heroism,
    SuperHeroism,
    Boldness,
    RestoreLifeLevels,
    ResistHeat,
    ResistCold,
    DetectInvisible,
    SlowPoison,
    NeutralizePoison,
    RestoreMana,
    InfraVision,
};

static bool playerDrinkPotion(uint32_t flags, uint8_t item_type) {
    bool identified = false;

    while (flags != 0) {
        int potion_id = getAndClearFirstBit(flags) + 1;

        if (item_type == TV_POTION2) {
            potion_id += 32;
        }

        // Potions
        switch ((PotionSpellTypes) potion_id) {
            case PotionSpellTypes::Strength:
                if (playerStatRandomIncrease(PlayerAttr::A_STR)) {
                    printMessage("Wow!  What bulging muscles!");
                    identified = true;
                }
                break;
            case PotionSpellTypes::Weakness:
                spellLoseSTR();
                identified = true;
                break;
            case PotionSpellTypes::RestoreStrength:
                if (playerStatRestore(PlayerAttr::A_STR)) {
                    printMessage("You feel warm all over.");
                    identified = true;
                }
                break;
            case PotionSpellTypes::Intelligence:
                if (playerStatRandomIncrease(PlayerAttr::A_INT)) {
                    printMessage("Aren't you brilliant!");
                    identified = true;
                }
                break;
            case PotionSpellTypes::LoseIntelligence:
                spellLoseINT();
                identified = true;
                break;
            case PotionSpellTypes::RestoreIntelligence:
                if (playerStatRestore(PlayerAttr::A_INT)) {
                    printMessage("You have have a warm feeling.");
                    identified = true;
                }
                break;
            case PotionSpellTypes::Wisdom:
                if (playerStatRandomIncrease(PlayerAttr::A_WIS)) {
                    printMessage("You suddenly have a profound thought!");
                    identified = true;
                }
                break;
            case PotionSpellTypes::LoseWisdom:
                spellLoseWIS();
                identified = true;
                break;
            case PotionSpellTypes::RestoreWisdom:
                if (playerStatRestore(PlayerAttr::A_WIS)) {
                    printMessage("You feel your wisdom returning.");
                    identified = true;
                }
                break;
            case PotionSpellTypes::Charisma:
                if (playerStatRandomIncrease(PlayerAttr::A_CHR)) {
                    printMessage("Gee, ain't you cute!");
                    identified = true;
                }
                break;
            case PotionSpellTypes::Ugliness:
                spellLoseCHR();
                identified = true;
                break;
            case PotionSpellTypes::RestoreCharisma:
                if (playerStatRestore(PlayerAttr::A_CHR)) {
                    printMessage("You feel your looks returning.");
                    identified = true;
                }
                break;
            case PotionSpellTypes::CureLightWounds:
                identified = spellChangePlayerHitPoints(diceRoll(Dice_t{2, 7}));
                break;
            case PotionSpellTypes::CureSeriousWounds:
                identified = spellChangePlayerHitPoints(diceRoll(Dice_t{4, 7}));
                break;
            case PotionSpellTypes::CureCriticalWounds:
                identified = spellChangePlayerHitPoints(diceRoll(Dice_t{6, 7}));
                break;
            case PotionSpellTypes::Healing:
                identified = spellChangePlayerHitPoints(1000);
                break;
            case PotionSpellTypes::Constitution:
                if (playerStatRandomIncrease(PlayerAttr::A_CON)) {
                    printMessage("You feel tingly for a moment.");
                    identified = true;
                }
                break;
            case PotionSpellTypes::GainExperience:
                if (py.misc.exp < config::player::PLAYER_MAX_EXP) {
                    auto exp = (uint32_t)((py.misc.exp / 2) + 10);
                    if (exp > 100000L) {
                        exp = 100000L;
                    }
                    py.misc.exp += exp;

                    printMessage("You feel more experienced.");
                    displayCharacterExperience();
                    identified = true;
                }
                break;
            case PotionSpellTypes::Sleep:
                if (!py.flags.free_action) {
                    // paralysis must == 0, otherwise could not drink potion
                    printMessage("You fall asleep.");
                    py.flags.paralysis += randomNumber(4) + 4;
                    identified = true;
                }
                break;
            case PotionSpellTypes::Blindness:
                if (py.flags.blind == 0) {
                    printMessage("You are covered by a veil of darkness.");
                    identified = true;
                }
                py.flags.blind += randomNumber(100) + 100;
                break;
            case PotionSpellTypes::Confusion:
                if (py.flags.confused == 0) {
                    printMessage("Hey!  This is good stuff!  * Hick! *");
                    identified = true;
                }
                py.flags.confused += randomNumber(20) + 12;
                break;
            case PotionSpellTypes::Poison:
                if (py.flags.poisoned == 0) {
                    printMessage("You feel very sick.");
                    identified = true;
                }
                py.flags.poisoned += randomNumber(15) + 10;
                break;
            case PotionSpellTypes::HasteSelf:
                if (py.flags.fast == 0) {
                    identified = true;
                }
                py.flags.fast += randomNumber(25) + 15;
                break;
            case PotionSpellTypes::Slowness:
                if (py.flags.slow == 0) {
                    identified = true;
                }
                py.flags.slow += randomNumber(25) + 15;
                break;
            case PotionSpellTypes::Dexterity:
                if (playerStatRandomIncrease(PlayerAttr::A_DEX)) {
                    printMessage("You feel more limber!");
                    identified = true;
                }
                break;
            case PotionSpellTypes::RestoreDexterity:
                if (playerStatRestore(PlayerAttr::A_DEX)) {
                    printMessage("You feel less clumsy.");
                    identified = true;
                }
                break;
            case PotionSpellTypes::RestoreConstitution:
                if (playerStatRestore(PlayerAttr::A_CON)) {
                    printMessage("You feel your health returning!");
                    identified = true;
                }
                break;
            case PotionSpellTypes::CureBlindness:
                identified = playerCureBlindness();
                break;
            case PotionSpellTypes::CureConfusion:
                identified = playerCureConfusion();
                break;
            case PotionSpellTypes::CurePoison:
                identified = playerCurePoison();
                break;
                // case 33: break; // this is no longer useful, now that there is a 'G'ain magic spells command
            case PotionSpellTypes::LoseExperience:
                if (py.misc.exp > 0) {
                    printMessage("You feel your memories fade.");

                    // Lose between 1/5 and 2/5 of your experience
                    int32_t exp = py.misc.exp / 5;

                    if (py.misc.exp > SHRT_MAX) {
                        auto scale = (int32_t)(INT_MAX / py.misc.exp);
                        exp += (randomNumber((int) scale) * py.misc.exp) / (scale * 5);
                    } else {
                        exp += randomNumber((int) py.misc.exp) / 5;
                    }
                    spellLoseEXP(exp);
                    identified = true;
                }
                break;
            case PotionSpellTypes::SaltWater:
                (void) playerCurePoison();
                if (py.flags.food > 150) {
                    py.flags.food = 150;
                }
                py.flags.paralysis = 4;

                printMessage("The potion makes you vomit!");
                identified = true;
                break;
            case PotionSpellTypes::Invulnerability:
                if (py.flags.invulnerability == 0) {
                    identified = true;
                }
                py.flags.invulnerability += randomNumber(10) + 10;
                break;
            case PotionSpellTypes::Heroism:
                if (py.flags.heroism == 0) {
                    identified = true;
                }
                py.flags.heroism += randomNumber(25) + 25;
                break;
            case PotionSpellTypes::SuperHeroism:
                if (py.flags.super_heroism == 0) {
                    identified = true;
                }
                py.flags.super_heroism += randomNumber(25) + 25;
                break;
            case PotionSpellTypes::Boldness:
                identified = playerRemoveFear();
                break;
            case PotionSpellTypes::RestoreLifeLevels:
                identified = spellRestorePlayerLevels();
                break;
            case PotionSpellTypes::ResistHeat:
                if (py.flags.heat_resistance == 0) {
                    identified = true;
                }
                py.flags.heat_resistance += randomNumber(10) + 10;
                break;
            case PotionSpellTypes::ResistCold:
                if (py.flags.cold_resistance == 0) {
                    identified = true;
                }
                py.flags.cold_resistance += randomNumber(10) + 10;
                break;
            case PotionSpellTypes::DetectInvisible:
                if (py.flags.detect_invisible == 0) {
                    identified = true;
                }
                playerDetectInvisible(randomNumber(12) + 12);
                break;
            case PotionSpellTypes::SlowPoison:
                identified = spellSlowPoison();
                break;
            case PotionSpellTypes::NeutralizePoison:
                identified = playerCurePoison();
                break;
            case PotionSpellTypes::RestoreMana:
                if (py.misc.current_mana < py.misc.mana) {
                    py.misc.current_mana = py.misc.mana;
                    printMessage("Your feel your head clear.");
                    printCharacterCurrentMana();
                    identified = true;
                }
                break;
            case PotionSpellTypes::InfraVision:
                if (py.flags.timed_infra == 0) {
                    printMessage("Your eyes begin to tingle.");
                    identified = true;
                }
                py.flags.timed_infra += 100 + randomNumber(100);
                break;
            default:
                // All cases are handled, so this should never be reached!
                printMessage("Internal error in potion()");
                break;
        }
    }

    return identified;
}

// Potions for the quaffing -RAK-
void quaff() {
    game.player_free_turn = true;

    if (py.pack.unique_items == 0) {
        printMessage("But you are not carrying anything.");
        return;
    }

    int item_pos_begin, item_pos_end;
    if (!inventoryFindRange(TV_POTION1, TV_POTION2, item_pos_begin, item_pos_end)) {
        printMessage("You are not carrying any potions.");
        return;
    }

    int item_id;
    if (!inventoryGetInputForItemId(item_id, "Quaff which potion?", item_pos_begin, item_pos_end, CNIL, CNIL)) {
        return;
    }

    game.player_free_turn = false;

    bool identified;
    Inventory_t *item = &py.inventory[item_id];

    if (item->flags == 0) {
        printMessage("You feel less thirsty.");
        identified = true;
    } else {
        identified = playerDrinkPotion(item->flags, item->category_id);
    }

    if (identified) {
        if (!itemSetColorlessAsIdentified(item->category_id, item->sub_category_id, item->identification)) {
            // round half-way case up
            py.misc.exp += (item->depth_first_found + (py.misc.level >> 1)) / py.misc.level;
            displayCharacterExperience();

            itemIdentify(py.inventory[item_id], item_id);
            item = &py.inventory[item_id];
        }
    } else if (!itemSetColorlessAsIdentified(item->category_id, item->sub_category_id, item->identification)) {
        itemSetAsTried(*item);
    }

    playerIngestFood(item->misc_use);
    itemTypeRemainingCountDescription(item_id);
    inventoryDestroyItem(item_id);
}
