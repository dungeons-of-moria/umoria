// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Code for potions

#include "headers.h"
#include "externs.h"

static bool drinkPotion(uint32_t flags, uint8_t itemID) {
    bool identified = false;

    while (flags != 0) {
        int potionID = bit_pos(&flags) + 1;

        if (itemID == TV_POTION2) {
            potionID += 32;
        }

        // Potions
        switch (potionID) {
            case 1:
                if (inc_stat(A_STR)) {
                    msg_print("Wow!  What bulging muscles!");
                    identified = true;
                }
                break;
            case 2:
                lose_str();
                identified = true;
                break;
            case 3:
                if (res_stat(A_STR)) {
                    msg_print("You feel warm all over.");
                    identified = true;
                }
                break;
            case 4:
                if (inc_stat(A_INT)) {
                    msg_print("Aren't you brilliant!");
                    identified = true;
                }
                break;
            case 5:
                lose_int();
                identified = true;
                break;
            case 6:
                if (res_stat(A_INT)) {
                    msg_print("You have have a warm feeling.");
                    identified = true;
                }
                break;
            case 7:
                if (inc_stat(A_WIS)) {
                    msg_print("You suddenly have a profound thought!");
                    identified = true;
                }
                break;
            case 8:
                lose_wis();
                identified = true;
                break;
            case 9:
                if (res_stat(A_WIS)) {
                    msg_print("You feel your wisdom returning.");
                    identified = true;
                }
                break;
            case 10:
                if (inc_stat(A_CHR)) {
                    msg_print("Gee, ain't you cute!");
                    identified = true;
                }
                break;
            case 11:
                lose_chr();
                identified = true;
                break;
            case 12:
                if (res_stat(A_CHR)) {
                    msg_print("You feel your looks returning.");
                    identified = true;
                }
                break;
            case 13:
                identified = hp_player(damroll(2, 7));
                break;
            case 14:
                identified = hp_player(damroll(4, 7));
                break;
            case 15:
                identified = hp_player(damroll(6, 7));
                break;
            case 16:
                identified = hp_player(1000);
                break;
            case 17:
                if (inc_stat(A_CON)) {
                    msg_print("You feel tingly for a moment.");
                    identified = true;
                }
                break;
            case 18:
                if (py.misc.exp < MAX_EXP) {
                    uint32_t l = (uint32_t) ((py.misc.exp / 2) + 10);
                    if (l > 100000L) {
                        l = 100000L;
                    }
                    py.misc.exp += l;

                    msg_print("You feel more experienced.");
                    prt_experience();
                    identified = true;
                }
                break;
            case 19:
                if (!py.flags.free_act) {
                    // paralysis must == 0, otherwise could not drink potion
                    msg_print("You fall asleep.");
                    py.flags.paralysis += randint(4) + 4;
                    identified = true;
                }
                break;
            case 20:
                if (py.flags.blind == 0) {
                    msg_print("You are covered by a veil of darkness.");
                    identified = true;
                }
                py.flags.blind += randint(100) + 100;
                break;
            case 21:
                if (py.flags.confused == 0) {
                    msg_print("Hey!  This is good stuff!  * Hick! *");
                    identified = true;
                }
                py.flags.confused += randint(20) + 12;
                break;
            case 22:
                if (py.flags.poisoned == 0) {
                    msg_print("You feel very sick.");
                    identified = true;
                }
                py.flags.poisoned += randint(15) + 10;
                break;
            case 23:
                if (py.flags.fast == 0) {
                    identified = true;
                }
                py.flags.fast += randint(25) + 15;
                break;
            case 24:
                if (py.flags.slow == 0) {
                    identified = true;
                }
                py.flags.slow += randint(25) + 15;
                break;
            case 26:
                if (inc_stat(A_DEX)) {
                    msg_print("You feel more limber!");
                    identified = true;
                }
                break;
            case 27:
                if (res_stat(A_DEX)) {
                    msg_print("You feel less clumsy.");
                    identified = true;
                }
                break;
            case 28:
                if (res_stat(A_CON)) {
                    msg_print("You feel your health returning!");
                    identified = true;
                }
                break;
            case 29:
                identified = cure_blindness();
                break;
            case 30:
                identified = cure_confusion();
                break;
            case 31:
                identified = cure_poison();
                break;
                // case 33: break; // this is no longer useful, now that there is a 'G'ain magic spells command
            case 34:
                if (py.misc.exp > 0) {
                    msg_print("You feel your memories fade.");

                    // Lose between 1/5 and 2/5 of your experience
                    int32_t m = py.misc.exp / 5;

                    if (py.misc.exp > MAX_SHORT) {
                        int32_t scale = (int32_t) (MAX_LONG / py.misc.exp);
                        m += (randint((int) scale) * py.misc.exp) / (scale * 5);
                    } else {
                        m += randint((int) py.misc.exp) / 5;
                    }
                    lose_exp(m);
                    identified = true;
                }
                break;
            case 35:
                (void) cure_poison();
                if (py.flags.food > 150) {
                    py.flags.food = 150;
                }
                py.flags.paralysis = 4;

                msg_print("The potion makes you vomit!");
                identified = true;
                break;
            case 36:
                if (py.flags.invuln == 0) {
                    identified = true;
                }
                py.flags.invuln += randint(10) + 10;
                break;
            case 37:
                if (py.flags.hero == 0) {
                    identified = true;
                }
                py.flags.hero += randint(25) + 25;
                break;
            case 38:
                if (py.flags.shero == 0) {
                    identified = true;
                }
                py.flags.shero += randint(25) + 25;
                break;
            case 39:
                identified = remove_fear();
                break;
            case 40:
                identified = restore_level();
                break;
            case 41:
                if (py.flags.resist_heat == 0) {
                    identified = true;
                }
                py.flags.resist_heat += randint(10) + 10;
                break;
            case 42:
                if (py.flags.resist_cold == 0) {
                    identified = true;
                }
                py.flags.resist_cold += randint(10) + 10;
                break;
            case 43:
                if (py.flags.detect_inv == 0) {
                    identified = true;
                }
                detect_inv2(randint(12) + 12);
                break;
            case 44:
                identified = slow_poison();
                break;
            case 45:
                identified = cure_poison();
                break;
            case 46:
                if (py.misc.cmana < py.misc.mana) {
                    py.misc.cmana = py.misc.mana;
                    msg_print("Your feel your head clear.");
                    prt_cmana();
                    identified = true;
                }
                break;
            case 47:
                if (py.flags.tim_infra == 0) {
                    msg_print("Your eyes begin to tingle.");
                    identified = true;
                }
                py.flags.tim_infra += 100 + randint(100);
                break;
            default:
                msg_print("Internal error in potion()");
                break;
        }
    }

    return identified;
}

// Potions for the quaffing -RAK-
void quaff() {
    free_turn_flag = true;

    if (inven_ctr == 0) {
        msg_print("But you are not carrying anything.");
        return;
    }

    int itemPosBegin, itemPosEnd;
    if (!find_range(TV_POTION1, TV_POTION2, &itemPosBegin, &itemPosEnd)) {
        msg_print("You are not carrying any potions.");
        return;
    }

    int itemID;
    if (!get_item(&itemID, "Quaff which potion?", itemPosBegin, itemPosEnd, CNIL, CNIL)) {
        return;
    }

    free_turn_flag = false;

    bool identified;
    Inventory_t *i_ptr = &inventory[itemID];

    if (i_ptr->flags == 0) {
        msg_print("You feel less thirsty.");
        identified = true;
    } else {
        identified = drinkPotion(i_ptr->flags, i_ptr->tval);
    }

    if (identified) {
        if (!known1_p(i_ptr)) {
            // round half-way case up
            py.misc.exp += (i_ptr->level + (py.misc.lev >> 1)) / py.misc.lev;
            prt_experience();

            identify(&itemID);
            i_ptr = &inventory[itemID];
        }
    } else if (!known1_p(i_ptr)) {
        sample(i_ptr);
    }

    add_food(i_ptr->p1);
    desc_remain(itemID);
    inven_destroy(itemID);
}
