// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Food code

#include "headers.h"
#include "externs.h"

// Eat some food. -RAK-
void playerEat() {
    player_free_turn = true;

    if (inventory_count == 0) {
        printMessage("But you are not carrying anything.");
        return;
    }

    int item_pos_start, item_pos_end;
    if (!inventoryFindRange(TV_FOOD, TV_NEVER, item_pos_start, item_pos_end)) {
        printMessage("You are not carrying any food.");
        return;
    }

    int item_id;
    if (!inventoryGetInputForItemId(item_id, "Eat what?", item_pos_start, item_pos_end, CNIL, CNIL)) {
        return;
    }

    player_free_turn = false;

    bool identified = false;

    Inventory_t *item = &inventory[item_id];
    uint32_t item_flags = item->flags;

    while (item_flags != 0) {
        int food_id = getAndClearFirstBit(item_flags) + 1;

        // Foods
        switch (food_id) {
            case 1:
                py.flags.poisoned += randomNumber(10) + item->depth_first_found;
                identified = true;
                break;
            case 2:
                py.flags.blind += randomNumber(250) + 10 * item->depth_first_found + 100;
                drawCavePanel();
                printMessage("A veil of darkness surrounds you.");
                identified = true;
                break;
            case 3:
                py.flags.afraid += randomNumber(10) + item->depth_first_found;
                printMessage("You feel terrified!");
                identified = true;
                break;
            case 4:
                py.flags.confused += randomNumber(10) + item->depth_first_found;
                printMessage("You feel drugged.");
                identified = true;
                break;
            case 5:
                py.flags.image += randomNumber(200) + 25 * item->depth_first_found + 200;
                printMessage("You feel drugged.");
                identified = true;
                break;
            case 6:
                identified = playerCurePoison();
                break;
            case 7:
                identified = playerCureBlindness();
                break;
            case 8:
                if (py.flags.afraid > 1) {
                    py.flags.afraid = 1;
                    identified = true;
                }
                break;
            case 9:
                identified = playerCureConfusion();
                break;
            case 10:
                spellLoseSTR();
                identified = true;
                break;
            case 11:
                spellLoseCON();
                identified = true;
                break;
#if 0 // 12 through 15 are no longer used
            case 12:
                lose_int();
                identified = true;
                break;
            case 13:
                lose_wis();
                identified = true;
                break;
            case 14:
                lose_dex();
                identified = true;
                break;
            case 15:
                lose_chr();
                identified = true;
                break;
#endif
            case 16:
                if (playerStatRestore(A_STR)) {
                    printMessage("You feel your strength returning.");
                    identified = true;
                }
                break;
            case 17:
                if (playerStatRestore(A_CON)) {
                    printMessage("You feel your health returning.");
                    identified = true;
                }
                break;
            case 18:
                if (playerStatRestore(A_INT)) {
                    printMessage("Your head spins a moment.");
                    identified = true;
                }
                break;
            case 19:
                if (playerStatRestore(A_WIS)) {
                    printMessage("You feel your wisdom returning.");
                    identified = true;
                }
                break;
            case 20:
                if (playerStatRestore(A_DEX)) {
                    printMessage("You feel more dexterous.");
                    identified = true;
                }
                break;
            case 21:
                if (playerStatRestore(A_CHR)) {
                    printMessage("Your skin stops itching.");
                    identified = true;
                }
                break;
            case 22:
                identified = spellChangePlayerHitPoints(randomNumber(6));
                break;
            case 23:
                identified = spellChangePlayerHitPoints(randomNumber(12));
                break;
            case 24:
                identified = spellChangePlayerHitPoints(randomNumber(18));
                break;
#if 0 // 25 is no longer used
            case 25:
                identified = hp_player(damroll(3, 6));
                break;
#endif
            case 26:
                identified = spellChangePlayerHitPoints(diceDamageRoll(3, 12));
                break;
            case 27:
                playerTakesHit(randomNumber(18), "poisonous food.");
                identified = true;
                break;
#if 0 // 28 through 30 are no longer used
            case 28:
                take_hit(randint(8), "poisonous food.");
                identified = true;
                break;
            case 29:
                take_hit(damroll(2, 8), "poisonous food.");
                identified = true;
                break;
            case 30:
                take_hit(damroll(3, 8), "poisonous food.");
                identified = true;
                break;
#endif
            default:
                printMessage("Internal error in playerEat()");
                break;
        }
    }

    if (identified) {
        if (!itemSetColorlessAsIdentified(item->category_id, item->sub_category_id, item->identification)) {
            // use identified it, gain experience
            // round half-way case up
            py.misc.exp += (item->depth_first_found + (py.misc.level >> 1)) / py.misc.level;

            displayCharacterExperience();

            itemIdentify(inventory[item_id], item_id);
            item = &inventory[item_id];
        }
    } else if (!itemSetColorlessAsIdentified(item->category_id, item->sub_category_id, item->identification)) {
        itemSetAsTried(*item);
    }

    playerIngestFood(item->misc_use);

    py.flags.status &= ~(PY_WEAK | PY_HUNGRY);

    printCharacterHungerStatus();

    itemTypeRemainingCountDescription(item_id);
    inventoryDestroyItem(item_id);
}
