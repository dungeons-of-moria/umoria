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

    int j, k;
    if (!find_range(TV_FOOD, TV_NEVER, &j, &k)) {
        printMessage("You are not carrying any food.");
        return;
    }

    int item_val;
    if (!get_item(&item_val, "Eat what?", j, k, CNIL, CNIL)) {
        return;
    }

    player_free_turn = false;

    bool identified = false;

    Inventory_t *i_ptr = &inventory[item_val];
    uint32_t itemFlags = i_ptr->flags;

    while (itemFlags != 0) {
        int foodID = getAndClearFirstBit(&itemFlags) + 1;

        // Foods
        switch (foodID) {
            case 1:
                py.flags.poisoned += randomNumber(10) + i_ptr->level;
                identified = true;
                break;
            case 2:
                py.flags.blind += randomNumber(250) + 10 * i_ptr->level + 100;
                drawCavePanel();
                printMessage("A veil of darkness surrounds you.");
                identified = true;
                break;
            case 3:
                py.flags.afraid += randomNumber(10) + i_ptr->level;
                printMessage("You feel terrified!");
                identified = true;
                break;
            case 4:
                py.flags.confused += randomNumber(10) + i_ptr->level;
                printMessage("You feel drugged.");
                identified = true;
                break;
            case 5:
                py.flags.image += randomNumber(200) + 25 * i_ptr->level + 200;
                printMessage("You feel drugged.");
                identified = true;
                break;
            case 6:
                identified = cure_poison();
                break;
            case 7:
                identified = cure_blindness();
                break;
            case 8:
                if (py.flags.afraid > 1) {
                    py.flags.afraid = 1;
                    identified = true;
                }
                break;
            case 9:
                identified = cure_confusion();
                break;
            case 10:
                lose_str();
                identified = true;
                break;
            case 11:
                lose_con();
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
                identified = hp_player(randomNumber(6));
                break;
            case 23:
                identified = hp_player(randomNumber(12));
                break;
            case 24:
                identified = hp_player(randomNumber(18));
                break;
#if 0 // 25 is no longer used
            case 25:
                identified = hp_player(damroll(3, 6));
                break;
#endif
            case 26:
                identified = hp_player(diceDamageRoll(3, 12));
                break;
            case 27:
                take_hit(randomNumber(18), "poisonous food.");
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
        if (!itemSetColorlessAsIdentifed(i_ptr)) {
            // use identified it, gain experience
            // round half-way case up
            py.misc.exp += (i_ptr->level + (py.misc.lev >> 1)) / py.misc.lev;

            prt_experience();

            itemIdentify(&item_val);
            i_ptr = &inventory[item_val];
        }
    } else if (!itemSetColorlessAsIdentifed(i_ptr)) {
        itemSetAsTried(i_ptr);
    }

    playerIngestFood(i_ptr->p1);

    py.flags.status &= ~(PY_WEAK | PY_HUNGRY);

    printCharacterHungerstatus();

    itemTypeRemainingCountDescription(item_val);
    inventoryDestroyItem(item_val);
}
