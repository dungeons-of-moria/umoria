/* source/potions.c: code for potions
 *
 * Copyright (C) 1989-2008 James E. Wilson, Robert A. Koeneke,
 *                         David J. Grabiner
 *
 * This file is part of Umoria.
 *
 * Umoria is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Umoria is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Umoria.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "standard_library.h"

#include "config.h"
#include "constant.h"
#include "types.h"

#include "externs.h"

/* Potions for the quaffing        -RAK- */
void quaff() {
    uint32_t i, l;
    int j, k, item_val;
    int ident;
    inven_type *i_ptr;
    struct misc *m_ptr;
    struct flags *f_ptr;

    free_turn_flag = true;
    if (inven_ctr == 0) {
        msg_print("But you are not carrying anything.");
    } else if (!find_range(TV_POTION1, TV_POTION2, &j, &k)) {
        msg_print("You are not carrying any potions.");
    } else if (get_item(&item_val, "Quaff which potion?", j, k, CNIL, CNIL)) {
        i_ptr = &inventory[item_val];
        i = i_ptr->flags;
        free_turn_flag = false;
        ident = false;
        if (i == 0) {
            msg_print("You feel less thirsty.");
            ident = true;
        } else {
            while (i != 0) {
                j = bit_pos(&i) + 1;
                if (i_ptr->tval == TV_POTION2) {
                    j += 32;
                }
                /* Potions */
                switch (j) {
                case 1:
                    if (inc_stat(A_STR)) {
                        msg_print("Wow!  What bulging muscles!");
                        ident = true;
                    }
                    break;
                case 2:
                    ident = true;
                    lose_str();
                    break;
                case 3:
                    if (res_stat(A_STR)) {
                        msg_print("You feel warm all over.");
                        ident = true;
                    }
                    break;
                case 4:
                    if (inc_stat(A_INT)) {
                        msg_print("Aren't you brilliant!");
                        ident = true;
                    }
                    break;
                case 5:
                    ident = true;
                    lose_int();
                    break;
                case 6:
                    if (res_stat(A_INT)) {
                        msg_print("You have have a warm feeling.");
                        ident = true;
                    }
                    break;
                case 7:
                    if (inc_stat(A_WIS)) {
                        msg_print("You suddenly have a profound thought!");
                        ident = true;
                    }
                    break;
                case 8:
                    ident = true;
                    lose_wis();
                    break;
                case 9:
                    if (res_stat(A_WIS)) {
                        msg_print("You feel your wisdom returning.");
                        ident = true;
                    }
                    break;
                case 10:
                    if (inc_stat(A_CHR)) {
                        msg_print("Gee, ain't you cute!");
                        ident = true;
                    }
                    break;
                case 11:
                    ident = true;
                    lose_chr();
                    break;
                case 12:
                    if (res_stat(A_CHR)) {
                        msg_print("You feel your looks returning.");
                        ident = true;
                    }
                    break;
                case 13:
                    ident = hp_player(damroll(2, 7));
                    break;
                case 14:
                    ident = hp_player(damroll(4, 7));
                    break;
                case 15:
                    ident = hp_player(damroll(6, 7));
                    break;
                case 16:
                    ident = hp_player(1000);
                    break;
                case 17:
                    if (inc_stat(A_CON)) {
                        msg_print("You feel tingly for a moment.");
                        ident = true;
                    }
                    break;
                case 18:
                    m_ptr = &py.misc;
                    if (m_ptr->exp < MAX_EXP) {
                        l = (m_ptr->exp / 2) + 10;
                        if (l > 100000L) {
                            l = 100000L;
                        }
                        m_ptr->exp += l;
                        msg_print("You feel more experienced.");
                        prt_experience();
                        ident = true;
                    }
                    break;
                case 19:
                    f_ptr = &py.flags;
                    if (!f_ptr->free_act) {
                        /* paralysis must == 0, otherwise could not drink potion */
                        msg_print("You fall asleep.");
                        f_ptr->paralysis += randint(4) + 4;
                        ident = true;
                    }
                    break;
                case 20:
                    f_ptr = &py.flags;
                    if (f_ptr->blind == 0) {
                        msg_print("You are covered by a veil of darkness.");
                        ident = true;
                    }
                    f_ptr->blind += randint(100) + 100;
                    break;
                case 21:
                    f_ptr = &py.flags;
                    if (f_ptr->confused == 0) {
                        msg_print("Hey!  This is good stuff!  * Hick! *");
                        ident = true;
                    }
                    f_ptr->confused += randint(20) + 12;
                    break;
                case 22:
                    f_ptr = &py.flags;
                    if (f_ptr->poisoned == 0) {
                        msg_print("You feel very sick.");
                        ident = true;
                    }
                    f_ptr->poisoned += randint(15) + 10;
                    break;
                case 23:
                    if (py.flags.fast == 0) {
                        ident = true;
                    }
                    py.flags.fast += randint(25) + 15;
                    break;
                case 24:
                    if (py.flags.slow == 0) {
                        ident = true;
                    }
                    py.flags.slow += randint(25) + 15;
                    break;
                case 26:
                    if (inc_stat(A_DEX)) {
                        msg_print("You feel more limber!");
                        ident = true;
                    }
                    break;
                case 27:
                    if (res_stat(A_DEX)) {
                        msg_print("You feel less clumsy.");
                        ident = true;
                    }
                    break;
                case 28:
                    if (res_stat(A_CON)) {
                        msg_print("You feel your health returning!");
                        ident = true;
                    }
                    break;
                case 29:
                    ident = cure_blindness();
                    break;
                case 30:
                    ident = cure_confusion();
                    break;
                case 31:
                    ident = cure_poison();
                    break;
                // case 33: break; // this is no longer useful, now that there is a 'G'ain magic spells command
                case 34:
                    if (py.misc.exp > 0) {
                        int32_t m, scale;
                        msg_print("You feel your memories fade.");
                        /* Lose between 1/5 and 2/5 of your experience */
                        m = py.misc.exp / 5;
                        if (py.misc.exp > MAX_SHORT) {
                            scale = MAX_LONG / py.misc.exp;
                            m += (randint((int)scale) * py.misc.exp) /
                                 (scale * 5);
                        } else {
                            m += randint((int)py.misc.exp) / 5;
                        }
                        lose_exp(m);
                        ident = true;
                    }
                    break;
                case 35:
                    f_ptr = &py.flags;
                    (void)cure_poison();
                    if (f_ptr->food > 150) {
                        f_ptr->food = 150;
                    }
                    f_ptr->paralysis = 4;
                    msg_print("The potion makes you vomit!");
                    ident = true;
                    break;
                case 36:
                    if (py.flags.invuln == 0) {
                        ident = true;
                    }
                    py.flags.invuln += randint(10) + 10;
                    break;
                case 37:
                    if (py.flags.hero == 0) {
                        ident = true;
                    }
                    py.flags.hero += randint(25) + 25;
                    break;
                case 38:
                    if (py.flags.shero == 0) {
                        ident = true;
                    }
                    py.flags.shero += randint(25) + 25;
                    break;
                case 39:
                    ident = remove_fear();
                    break;
                case 40:
                    ident = restore_level();
                    break;
                case 41:
                    f_ptr = &py.flags;
                    if (f_ptr->resist_heat == 0) {
                        ident = true;
                    }
                    f_ptr->resist_heat += randint(10) + 10;
                    break;
                case 42:
                    f_ptr = &py.flags;
                    if (f_ptr->resist_cold == 0) {
                        ident = true;
                    }
                    f_ptr->resist_cold += randint(10) + 10;
                    break;
                case 43:
                    if (py.flags.detect_inv == 0) {
                        ident = true;
                    }
                    detect_inv2(randint(12) + 12);
                    break;
                case 44:
                    ident = slow_poison();
                    break;
                case 45:
                    ident = cure_poison();
                    break;
                case 46:
                    m_ptr = &py.misc;
                    if (m_ptr->cmana < m_ptr->mana) {
                        m_ptr->cmana = m_ptr->mana;
                        ident = true;
                        msg_print("Your feel your head clear.");
                        prt_cmana();
                    }
                    break;
                case 47:
                    f_ptr = &py.flags;
                    if (f_ptr->tim_infra == 0) {
                        msg_print("Your eyes begin to tingle.");
                        ident = true;
                    }
                    f_ptr->tim_infra += 100 + randint(100);
                    break;
                default:
                    msg_print("Internal error in potion()");
                    break;
                }
                /* End of Potions. */
            }
        }
        if (ident) {
            if (!known1_p(i_ptr)) {
                m_ptr = &py.misc;
                /* round half-way case up */
                m_ptr->exp += (i_ptr->level + (m_ptr->lev >> 1)) / m_ptr->lev;
                prt_experience();

                identify(&item_val);
                i_ptr = &inventory[item_val];
            }
        } else if (!known1_p(i_ptr)) {
            sample(i_ptr);
        }

        add_food(i_ptr->p1);
        desc_remain(item_val);
        inven_destroy(item_val);
    }
}
