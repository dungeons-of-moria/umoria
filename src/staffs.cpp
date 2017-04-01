// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Staff code

#include "headers.h"
#include "externs.h"

// Use a staff. -RAK-
void use() {
    free_turn_flag = true;

    if (inven_ctr == 0) {
        msg_print("But you are not carrying anything.");
        return;
    }

    int j, k;
    if (!find_range(TV_STAFF, TV_NEVER, &j, &k)) {
        msg_print("You are not carrying any staffs.");
        return;
    }

    int item_val;
    if (!get_item(&item_val, "Use which staff?", j, k, CNIL, CNIL)) {
        return;
    }

    free_turn_flag = false;

    inven_type *i_ptr = &inventory[item_val];
    struct player_type::misc *m_ptr = &py.misc;

    int chance = m_ptr->save + stat_adj(A_INT) - (int) i_ptr->level - 5 + (class_level_adj[m_ptr->pclass][CLA_DEVICE] * m_ptr->lev / 3);

    if (py.flags.confused > 0) {
        chance = chance / 2;
    }

    if ((chance < USE_DEVICE) && (randint(USE_DEVICE - chance + 1) == 1)) {
        chance = USE_DEVICE; // Give everyone a slight chance
    }

    if (chance <= 0) {
        chance = 1;
    }

    if (randint(chance) < USE_DEVICE) {
        msg_print("You failed to use the staff properly.");
        return;
    }

    if (i_ptr->p1 < 1) {
        msg_print("The staff has no charges left.");
        if (!known2_p(i_ptr)) {
            add_inscribe(i_ptr, ID_EMPTY);
        }
        return;
    }

    uint32_t i = i_ptr->flags;
    (i_ptr->p1)--;

    int y, x;
    bool ident = false;

    while (i != 0) {
        int pos = bit_pos(&i) + 1;

        // Staffs.
        switch (pos) {
            case 1:
                ident = light_area(char_row, char_col);
                break;
            case 2:
                ident = detect_sdoor();
                break;
            case 3:
                ident = detect_trap();
                break;
            case 4:
                ident = detect_treasure();
                break;
            case 5:
                ident = detect_object();
                break;
            case 6:
                teleport(100);
                ident = true;
                break;
            case 7:
                ident = true;
                earthquake();
                break;
            case 8:
                ident = false;
                for (k = 0; k < randint(4); k++) {
                    y = char_row;
                    x = char_col;
                    ident |= summon_monster(&y, &x, false);
                }
                break;
            case 10:
                ident = true;
                destroy_area(char_row, char_col);
                break;
            case 11:
                ident = true;
                starlite(char_row, char_col);
                break;
            case 12:
                ident = speed_monsters(1);
                break;
            case 13:
                ident = speed_monsters(-1);
                break;
            case 14:
                ident = sleep_monsters2();
                break;
            case 15:
                ident = hp_player(randint(8));
                break;
            case 16:
                ident = detect_invisible();
                break;
            case 17:
                if (py.flags.fast == 0) {
                    ident = true;
                }
                py.flags.fast += randint(30) + 15;
                break;
            case 18:
                if (py.flags.slow == 0) {
                    ident = true;
                }
                py.flags.slow += randint(30) + 15;
                break;
            case 19:
                ident = mass_poly();
                break;
            case 20:
                if (remove_curse()) {
                    if (py.flags.blind < 1) {
                        msg_print("The staff glows blue for a moment..");
                    }
                    ident = true;
                }
                break;
            case 21:
                ident = detect_evil();
                break;
            case 22:
                if ((cure_blindness()) || (cure_poison()) || (cure_confusion())) {
                    ident = true;
                }
                break;
            case 23:
                ident = dispel_creature(CD_EVIL, 60);
                break;
            case 25:
                ident = unlight_area(char_row, char_col);
                break;
            case 32:
                // store bought flag
                break;
            default:
                msg_print("Internal error in staffs()");
                break;
        }
        // End of staff actions.
    }

    if (ident) {
        if (!known1_p(i_ptr)) {
            m_ptr = &py.misc;

            // round half-way case up
            m_ptr->exp += (i_ptr->level + (m_ptr->lev >> 1)) / m_ptr->lev;

            prt_experience();

            identify(&item_val);

            // NOTE: this is never read after this, so commenting out. -MRC-
            // i_ptr = &inventory[item_val];
        }
    } else if (!known1_p(i_ptr)) {
        sample(i_ptr);
    }

    desc_charges(item_val);
}
