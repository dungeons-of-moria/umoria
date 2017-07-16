// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Code for potions

#include "headers.h"
#include "externs.h"

static bool playerDrinkPotion(uint32_t flags, uint8_t item_id) {
    bool identified = false;

    while (flags != 0) {
        int potion_id = getAndClearFirstBit(&flags) + 1;

        if (item_id == TV_POTION2) {
            potion_id += 32;
        }

        // Potions
        switch (potion_id) {
            case 1:
                if (playerStatRandomIncrease(A_STR)) {
                    printMessage("Wow!  What bulging muscles!");
                    identified = true;
                }
                break;
            case 2:
                spellLoseSTR();
                identified = true;
                break;
            case 3:
                if (playerStatRestore(A_STR)) {
                    printMessage("You feel warm all over.");
                    identified = true;
                }
                break;
            case 4:
                if (playerStatRandomIncrease(A_INT)) {
                    printMessage("Aren't you brilliant!");
                    identified = true;
                }
                break;
            case 5:
                spellLoseINT();
                identified = true;
                break;
            case 6:
                if (playerStatRestore(A_INT)) {
                    printMessage("You have have a warm feeling.");
                    identified = true;
                }
                break;
            case 7:
                if (playerStatRandomIncrease(A_WIS)) {
                    printMessage("You suddenly have a profound thought!");
                    identified = true;
                }
                break;
            case 8:
                spellLoseWIS();
                identified = true;
                break;
            case 9:
                if (playerStatRestore(A_WIS)) {
                    printMessage("You feel your wisdom returning.");
                    identified = true;
                }
                break;
            case 10:
                if (playerStatRandomIncrease(A_CHR)) {
                    printMessage("Gee, ain't you cute!");
                    identified = true;
                }
                break;
            case 11:
                spellLoseCHR();
                identified = true;
                break;
            case 12:
                if (playerStatRestore(A_CHR)) {
                    printMessage("You feel your looks returning.");
                    identified = true;
                }
                break;
            case 13:
                identified = spellChangePlayerHitPoints(diceDamageRoll(2, 7));
                break;
            case 14:
                identified = spellChangePlayerHitPoints(diceDamageRoll(4, 7));
                break;
            case 15:
                identified = spellChangePlayerHitPoints(diceDamageRoll(6, 7));
                break;
            case 16:
                identified = spellChangePlayerHitPoints(1000);
                break;
            case 17:
                if (playerStatRandomIncrease(A_CON)) {
                    printMessage("You feel tingly for a moment.");
                    identified = true;
                }
                break;
            case 18:
                if (py.misc.exp < MAX_EXP) {
                    uint32_t exp = (uint32_t) ((py.misc.exp / 2) + 10);
                    if (exp > 100000L) {
                        exp = 100000L;
                    }
                    py.misc.exp += exp;

                    printMessage("You feel more experienced.");
                    displayCharacterExperience();
                    identified = true;
                }
                break;
            case 19:
                if (!py.flags.free_act) {
                    // paralysis must == 0, otherwise could not drink potion
                    printMessage("You fall asleep.");
                    py.flags.paralysis += randomNumber(4) + 4;
                    identified = true;
                }
                break;
            case 20:
                if (py.flags.blind == 0) {
                    printMessage("You are covered by a veil of darkness.");
                    identified = true;
                }
                py.flags.blind += randomNumber(100) + 100;
                break;
            case 21:
                if (py.flags.confused == 0) {
                    printMessage("Hey!  This is good stuff!  * Hick! *");
                    identified = true;
                }
                py.flags.confused += randomNumber(20) + 12;
                break;
            case 22:
                if (py.flags.poisoned == 0) {
                    printMessage("You feel very sick.");
                    identified = true;
                }
                py.flags.poisoned += randomNumber(15) + 10;
                break;
            case 23:
                if (py.flags.fast == 0) {
                    identified = true;
                }
                py.flags.fast += randomNumber(25) + 15;
                break;
            case 24:
                if (py.flags.slow == 0) {
                    identified = true;
                }
                py.flags.slow += randomNumber(25) + 15;
                break;
            case 26:
                if (playerStatRandomIncrease(A_DEX)) {
                    printMessage("You feel more limber!");
                    identified = true;
                }
                break;
            case 27:
                if (playerStatRestore(A_DEX)) {
                    printMessage("You feel less clumsy.");
                    identified = true;
                }
                break;
            case 28:
                if (playerStatRestore(A_CON)) {
                    printMessage("You feel your health returning!");
                    identified = true;
                }
                break;
            case 29:
                identified = playerCureBlindness();
                break;
            case 30:
                identified = playerCureConfusion();
                break;
            case 31:
                identified = playerCurePoison();
                break;
                // case 33: break; // this is no longer useful, now that there is a 'G'ain magic spells command
            case 34:
                if (py.misc.exp > 0) {
                    printMessage("You feel your memories fade.");

                    // Lose between 1/5 and 2/5 of your experience
                    int32_t exp = py.misc.exp / 5;

                    if (py.misc.exp > MAX_SHORT) {
                        int32_t scale = (int32_t) (MAX_LONG / py.misc.exp);
                        exp += (randomNumber((int) scale) * py.misc.exp) / (scale * 5);
                    } else {
                        exp += randomNumber((int) py.misc.exp) / 5;
                    }
                    spellLoseEXP(exp);
                    identified = true;
                }
                break;
            case 35:
                (void) playerCurePoison();
                if (py.flags.food > 150) {
                    py.flags.food = 150;
                }
                py.flags.paralysis = 4;

                printMessage("The potion makes you vomit!");
                identified = true;
                break;
            case 36:
                if (py.flags.invuln == 0) {
                    identified = true;
                }
                py.flags.invuln += randomNumber(10) + 10;
                break;
            case 37:
                if (py.flags.hero == 0) {
                    identified = true;
                }
                py.flags.hero += randomNumber(25) + 25;
                break;
            case 38:
                if (py.flags.shero == 0) {
                    identified = true;
                }
                py.flags.shero += randomNumber(25) + 25;
                break;
            case 39:
                identified = playerRemoveFear();
                break;
            case 40:
                identified = restore_level();
                break;
            case 41:
                if (py.flags.resist_heat == 0) {
                    identified = true;
                }
                py.flags.resist_heat += randomNumber(10) + 10;
                break;
            case 42:
                if (py.flags.resist_cold == 0) {
                    identified = true;
                }
                py.flags.resist_cold += randomNumber(10) + 10;
                break;
            case 43:
                if (py.flags.detect_inv == 0) {
                    identified = true;
                }
                detect_inv2(randomNumber(12) + 12);
                break;
            case 44:
                identified = spellSlowPoison();
                break;
            case 45:
                identified = playerCurePoison();
                break;
            case 46:
                if (py.misc.cmana < py.misc.mana) {
                    py.misc.cmana = py.misc.mana;
                    printMessage("Your feel your head clear.");
                    printCharacterCurrentMana();
                    identified = true;
                }
                break;
            case 47:
                if (py.flags.tim_infra == 0) {
                    printMessage("Your eyes begin to tingle.");
                    identified = true;
                }
                py.flags.tim_infra += 100 + randomNumber(100);
                break;
            default:
                printMessage("Internal error in potion()");
                break;
        }
    }

    return identified;
}

// Potions for the quaffing -RAK-
void quaff() {
    player_free_turn = true;

    if (inventory_count == 0) {
        printMessage("But you are not carrying anything.");
        return;
    }

    int item_pos_begin, item_pos_end;
    if (!inventoryFindRange(TV_POTION1, TV_POTION2, &item_pos_begin, &item_pos_end)) {
        printMessage("You are not carrying any potions.");
        return;
    }

    int item_id;
    if (!inventoryGetInputForItemId(&item_id, "Quaff which potion?", item_pos_begin, item_pos_end, CNIL, CNIL)) {
        return;
    }

    player_free_turn = false;

    bool identified;
    Inventory_t *i_ptr = &inventory[item_id];

    if (i_ptr->flags == 0) {
        printMessage("You feel less thirsty.");
        identified = true;
    } else {
        identified = playerDrinkPotion(i_ptr->flags, i_ptr->tval);
    }

    if (identified) {
        if (!itemSetColorlessAsIdentifed(i_ptr)) {
            // round half-way case up
            py.misc.exp += (i_ptr->level + (py.misc.lev >> 1)) / py.misc.lev;
            displayCharacterExperience();

            itemIdentify(&item_id);
            i_ptr = &inventory[item_id];
        }
    } else if (!itemSetColorlessAsIdentifed(i_ptr)) {
        itemSetAsTried(i_ptr);
    }

    playerIngestFood(i_ptr->p1);
    itemTypeRemainingCountDescription(item_id);
    inventoryDestroyItem(item_id);
}
