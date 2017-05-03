// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Food code

#include "headers.h"
#include "externs.h"

// Eat some food. -RAK-
void eat() {
    free_turn_flag = true;

    if (inven_ctr == 0) {
        msg_print("But you are not carrying anything.");
        return;
    }

    int j, k;
    if (!find_range(TV_FOOD, TV_NEVER, &j, &k)) {
        msg_print("You are not carrying any food.");
        return;
    }

    int item_val;
    if (!get_item(&item_val, "Eat what?", j, k, CNIL, CNIL)) {
        return;
    }

    free_turn_flag = false;

    bool identified = false;

    inven_type *i_ptr = &inventory[item_val];
    uint32_t itemFlags = i_ptr->flags;

    while (itemFlags != 0) {
        int foodID = bit_pos(&itemFlags) + 1;

        // Foods
        switch (foodID) {
            case 1:
                py.flags.poisoned += randint(10) + i_ptr->level;
                identified = true;
                break;
            case 2:
                py.flags.blind += randint(250) + 10 * i_ptr->level + 100;
                draw_cave();
                msg_print("A veil of darkness surrounds you.");
                identified = true;
                break;
            case 3:
                py.flags.afraid += randint(10) + i_ptr->level;
                msg_print("You feel terrified!");
                identified = true;
                break;
            case 4:
                py.flags.confused += randint(10) + i_ptr->level;
                msg_print("You feel drugged.");
                identified = true;
                break;
            case 5:
                py.flags.image += randint(200) + 25 * i_ptr->level + 200;
                msg_print("You feel drugged.");
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
                if (res_stat(A_STR)) {
                    msg_print("You feel your strength returning.");
                    identified = true;
                }
                break;
            case 17:
                if (res_stat(A_CON)) {
                    msg_print("You feel your health returning.");
                    identified = true;
                }
                break;
            case 18:
                if (res_stat(A_INT)) {
                    msg_print("Your head spins a moment.");
                    identified = true;
                }
                break;
            case 19:
                if (res_stat(A_WIS)) {
                    msg_print("You feel your wisdom returning.");
                    identified = true;
                }
                break;
            case 20:
                if (res_stat(A_DEX)) {
                    msg_print("You feel more dexterous.");
                    identified = true;
                }
                break;
            case 21:
                if (res_stat(A_CHR)) {
                    msg_print("Your skin stops itching.");
                    identified = true;
                }
                break;
            case 22:
                identified = hp_player(randint(6));
                break;
            case 23:
                identified = hp_player(randint(12));
                break;
            case 24:
                identified = hp_player(randint(18));
                break;
#if 0 // 25 is no longer used
            case 25:
                identified = hp_player(damroll(3, 6));
                break;
#endif
            case 26:
                identified = hp_player(damroll(3, 12));
                break;
            case 27:
                take_hit(randint(18), "poisonous food.");
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
                msg_print("Internal error in eat()");
                break;
        }
    }

    if (identified) {
        if (!known1_p(i_ptr)) {
            // use identified it, gain experience
            // round half-way case up
            py.misc.exp += (i_ptr->level + (py.misc.lev >> 1)) / py.misc.lev;

            prt_experience();

            identify(&item_val);
            i_ptr = &inventory[item_val];
        }
    } else if (!known1_p(i_ptr)) {
        sample(i_ptr);
    }

    add_food(i_ptr->p1);

    py.flags.status &= ~(PY_WEAK | PY_HUNGRY);

    prt_hunger();

    desc_remain(item_val);
    inven_destroy(item_val);
}
