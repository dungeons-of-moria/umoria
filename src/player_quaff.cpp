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
    strength = 1,
    weakness,
    restore_strength,
    intelligence,
    lose_intelligence,
    restore_intelligence,
    wisdom,
    lose_wisdom,
    restore_wisdom,
    charisma,
    ugliness,
    restore_charisma,
    cure_light_wounds,
    cure_serious_wounds,
    cure_critical_wounds,
    healing,
    constitution,
    gain_experience,
    sleep,
    blindness,
    confusion,
    poison,
    haste_self,
    slowness,
    // 25 not used
    dexterity = 26,
    restore_dexterity,
    restore_constitution,
    cure_blindness,
    cure_confusion,
    cure_poison,
    // 32 not used
    // 33 not used
    lose_experience = 34,
    salt_water,
    invulnerability,
    heroism,
    super_heroism,
    boldness,
    restore_life_levels,
    resist_heat,
    resist_cold,
    detect_invisible,
    slow_poison,
    neutralize_poison,
    restore_mana,
    infra_vision,
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
            case PotionSpellTypes::strength:
                if (playerStatRandomIncrease(PlayerAttr::STR)) {
                    printMessage("Wow!  What bulging muscles!");
                    identified = true;
                }
                break;
            case PotionSpellTypes::weakness:
                spellLoseSTR();
                identified = true;
                break;
            case PotionSpellTypes::restore_strength:
                if (playerStatRestore(PlayerAttr::STR)) {
                    printMessage("You feel warm all over.");
                    identified = true;
                }
                break;
            case PotionSpellTypes::intelligence:
                if (playerStatRandomIncrease(PlayerAttr::INT)) {
                    printMessage("Aren't you brilliant!");
                    identified = true;
                }
                break;
            case PotionSpellTypes::lose_intelligence:
                spellLoseINT();
                identified = true;
                break;
            case PotionSpellTypes::restore_intelligence:
                if (playerStatRestore(PlayerAttr::INT)) {
                    printMessage("You have have a warm feeling.");
                    identified = true;
                }
                break;
            case PotionSpellTypes::wisdom:
                if (playerStatRandomIncrease(PlayerAttr::WIS)) {
                    printMessage("You suddenly have a profound thought!");
                    identified = true;
                }
                break;
            case PotionSpellTypes::lose_wisdom:
                spellLoseWIS();
                identified = true;
                break;
            case PotionSpellTypes::restore_wisdom:
                if (playerStatRestore(PlayerAttr::WIS)) {
                    printMessage("You feel your wisdom returning.");
                    identified = true;
                }
                break;
            case PotionSpellTypes::charisma:
                if (playerStatRandomIncrease(PlayerAttr::CHR)) {
                    printMessage("Gee, ain't you cute!");
                    identified = true;
                }
                break;
            case PotionSpellTypes::ugliness:
                spellLoseCHR();
                identified = true;
                break;
            case PotionSpellTypes::restore_charisma:
                if (playerStatRestore(PlayerAttr::CHR)) {
                    printMessage("You feel your looks returning.");
                    identified = true;
                }
                break;
            case PotionSpellTypes::cure_light_wounds:
                identified = spellChangePlayerHitPoints(diceRoll(Dice_t{2, 7}));
                break;
            case PotionSpellTypes::cure_serious_wounds:
                identified = spellChangePlayerHitPoints(diceRoll(Dice_t{4, 7}));
                break;
            case PotionSpellTypes::cure_critical_wounds:
                identified = spellChangePlayerHitPoints(diceRoll(Dice_t{6, 7}));
                break;
            case PotionSpellTypes::healing:
                identified = spellChangePlayerHitPoints(1000);
                break;
            case PotionSpellTypes::constitution:
                if (playerStatRandomIncrease(PlayerAttr::CON)) {
                    printMessage("You feel tingly for a moment.");
                    identified = true;
                }
                break;
            case PotionSpellTypes::gain_experience:
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
            case PotionSpellTypes::sleep:
                if (!py.flags.free_action) {
                    // paralysis must == 0, otherwise could not drink potion
                    printMessage("You fall asleep.");
                    py.flags.paralysis += randomNumber(4) + 4;
                    identified = true;
                }
                break;
            case PotionSpellTypes::blindness:
                if (py.flags.blind == 0) {
                    printMessage("You are covered by a veil of darkness.");
                    identified = true;
                }
                py.flags.blind += randomNumber(100) + 100;
                break;
            case PotionSpellTypes::confusion:
                if (py.flags.confused == 0) {
                    printMessage("Hey!  This is good stuff!  * Hick! *");
                    identified = true;
                }
                py.flags.confused += randomNumber(20) + 12;
                break;
            case PotionSpellTypes::poison:
                if (py.flags.poisoned == 0) {
                    printMessage("You feel very sick.");
                    identified = true;
                }
                py.flags.poisoned += randomNumber(15) + 10;
                break;
            case PotionSpellTypes::haste_self:
                if (py.flags.fast == 0) {
                    identified = true;
                }
                py.flags.fast += randomNumber(25) + 15;
                break;
            case PotionSpellTypes::slowness:
                if (py.flags.slow == 0) {
                    identified = true;
                }
                py.flags.slow += randomNumber(25) + 15;
                break;
            case PotionSpellTypes::dexterity:
                if (playerStatRandomIncrease(PlayerAttr::DEX)) {
                    printMessage("You feel more limber!");
                    identified = true;
                }
                break;
            case PotionSpellTypes::restore_dexterity:
                if (playerStatRestore(PlayerAttr::DEX)) {
                    printMessage("You feel less clumsy.");
                    identified = true;
                }
                break;
            case PotionSpellTypes::restore_constitution:
                if (playerStatRestore(PlayerAttr::CON)) {
                    printMessage("You feel your health returning!");
                    identified = true;
                }
                break;
            case PotionSpellTypes::cure_blindness:
                identified = playerCureBlindness();
                break;
            case PotionSpellTypes::cure_confusion:
                identified = playerCureConfusion();
                break;
            case PotionSpellTypes::cure_poison:
                identified = playerCurePoison();
                break;
                // case 33: break; // this is no longer useful, now that there is a 'G'ain magic spells command
            case PotionSpellTypes::lose_experience:
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
            case PotionSpellTypes::salt_water:
                (void) playerCurePoison();
                if (py.flags.food > 150) {
                    py.flags.food = 150;
                }
                py.flags.paralysis = 4;

                printMessage("The potion makes you vomit!");
                identified = true;
                break;
            case PotionSpellTypes::invulnerability:
                if (py.flags.invulnerability == 0) {
                    identified = true;
                }
                py.flags.invulnerability += randomNumber(10) + 10;
                break;
            case PotionSpellTypes::heroism:
                if (py.flags.heroism == 0) {
                    identified = true;
                }
                py.flags.heroism += randomNumber(25) + 25;
                break;
            case PotionSpellTypes::super_heroism:
                if (py.flags.super_heroism == 0) {
                    identified = true;
                }
                py.flags.super_heroism += randomNumber(25) + 25;
                break;
            case PotionSpellTypes::boldness:
                identified = playerRemoveFear();
                break;
            case PotionSpellTypes::restore_life_levels:
                identified = spellRestorePlayerLevels();
                break;
            case PotionSpellTypes::resist_heat:
                if (py.flags.heat_resistance == 0) {
                    identified = true;
                }
                py.flags.heat_resistance += randomNumber(10) + 10;
                break;
            case PotionSpellTypes::resist_cold:
                if (py.flags.cold_resistance == 0) {
                    identified = true;
                }
                py.flags.cold_resistance += randomNumber(10) + 10;
                break;
            case PotionSpellTypes::detect_invisible:
                if (py.flags.detect_invisible == 0) {
                    identified = true;
                }
                playerDetectInvisible(randomNumber(12) + 12);
                break;
            case PotionSpellTypes::slow_poison:
                identified = spellSlowPoison();
                break;
            case PotionSpellTypes::neutralize_poison:
                identified = playerCurePoison();
                break;
            case PotionSpellTypes::restore_mana:
                if (py.misc.current_mana < py.misc.mana) {
                    py.misc.current_mana = py.misc.mana;
                    printMessage("Your feel your head clear.");
                    printCharacterCurrentMana();
                    identified = true;
                }
                break;
            case PotionSpellTypes::infra_vision:
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
