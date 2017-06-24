// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Code for priest spells

#include "headers.h"
#include "externs.h"

static bool canPray(int *itemPosBegin, int *itemPosEnd) {
    if (py.flags.blind > 0) {
        msg_print("You can't see to read your prayer!");
        return false;
    }

    if (no_light()) {
        msg_print("You have no light to read by.");
        return false;
    }

    if (py.flags.confused > 0) {
        msg_print("You are too confused.");
        return false;
    }

    if (classes[py.misc.pclass].spell != PRIEST) {
        msg_print("Pray hard enough and your prayers may be answered.");
        return false;
    }

    if (inven_ctr == 0) {
        msg_print("But you are not carrying anything!");
        return false;
    }

    if (!find_range(TV_PRAYER_BOOK, TV_NEVER, itemPosBegin, itemPosEnd)) {
        msg_print("You are not carrying any Holy Books!");
        return false;
    }

    return true;
}

// Recite a prayers.
static void recitePrayer(int prayerType) {
    int dir;

    switch (prayerType + 1) {
        case 1:
            (void) detect_evil();
            break;
        case 2:
            (void) hp_player(damroll(3, 3));
            break;
        case 3:
            bless(randint(12) + 12);
            break;
        case 4:
            (void) remove_fear();
            break;
        case 5:
            (void) light_area(char_row, char_col);
            break;
        case 6:
            (void) detect_trap();
            break;
        case 7:
            (void) detect_sdoor();
            break;
        case 8:
            (void) slow_poison();
            break;
        case 9:
            if (get_dir(CNIL, &dir)) {
                (void) confuse_monster(dir, char_row, char_col);
            }
            break;
        case 10:
            teleport((py.misc.lev * 3));
            break;
        case 11:
            (void) hp_player(damroll(4, 4));
            break;
        case 12:
            bless(randint(24) + 24);
            break;
        case 13:
            (void) sleep_monsters1(char_row, char_col);
            break;
        case 14:
            create_food();
            break;
        case 15:
            for (int i = 0; i < INVEN_ARRAY_SIZE; i++) {
                // only clear flag for items that are wielded or worn
                if (inventory[i].tval >= TV_MIN_WEAR && inventory[i].tval <= TV_MAX_WEAR) {
                    inventory[i].flags &= ~TR_CURSED;
                }
            }
            break;
        case 16:
            py.flags.resist_heat += randint(10) + 10;
            py.flags.resist_cold += randint(10) + 10;
            break;
        case 17:
            (void) cure_poison();
            break;
        case 18:
            if (get_dir(CNIL, &dir)) {
                fire_ball(GF_HOLY_ORB, dir, char_row, char_col, (damroll(3, 6) + py.misc.lev), "Black Sphere");
            }
            break;
        case 19:
            (void) hp_player(damroll(8, 4));
            break;
        case 20:
            detect_inv2(randint(24) + 24);
            break;
        case 21:
            (void) protect_evil();
            break;
        case 22:
            earthquake();
            break;
        case 23:
            map_area();
            break;
        case 24:
            (void) hp_player(damroll(16, 4));
            break;
        case 25:
            (void) turn_undead();
            break;
        case 26:
            bless(randint(48) + 48);
            break;
        case 27:
            (void) dispel_creature(CD_UNDEAD, (3 * py.misc.lev));
            break;
        case 28:
            (void) hp_player(200);
            break;
        case 29:
            (void) dispel_creature(CD_EVIL, (3 * py.misc.lev));
            break;
        case 30:
            warding_glyph();
            break;
        case 31:
            (void) remove_fear();
            (void) cure_poison();
            (void) hp_player(1000);

            for (int i = A_STR; i <= A_CHR; i++) {
                (void) res_stat(i);
            }

            (void) dispel_creature(CD_EVIL, (4 * py.misc.lev));
            (void) turn_undead();

            if (py.flags.invuln < 3) {
                py.flags.invuln = 3;
            } else {
                py.flags.invuln++;
            }
            break;
        default:
            break;
    }
}

// Pray like HELL. -RAK-
void pray() {
    free_turn_flag = true;

    int itemPosBegin, itemposEnd;
    if (!canPray(&itemPosBegin, &itemposEnd)) {
        return;
    }

    int item_val;
    if (!get_item(&item_val, "Use which Holy Book?", itemPosBegin, itemposEnd, CNIL, CNIL)) {
        return;
    }

    int choice, chance;
    int result = cast_spell("Recite which prayer?", item_val, &choice, &chance);
    if (result < 0) {
        msg_print("You don't know any prayers in that book.");
        return;
    } else if (result == 0) {
        return;
    }

    if (randint(100) < chance) {
        msg_print("You lost your concentration!");
        return;
    }

    Spell_t *s_ptr = &magic_spell[py.misc.pclass - 1][choice];

    // NOTE: at least one function called by `recitePrayer()` sets `free_turn_flag = true`,
    // e.g. `create_food()`, so this check is required. -MRC-
    free_turn_flag = false;
    recitePrayer(choice);
    if (!free_turn_flag) {
        if ((spell_worked & (1L << choice)) == 0) {
            py.misc.exp += s_ptr->sexp << 2;
            prt_experience();
            spell_worked |= (1L << choice);
        }
    }

    if (!free_turn_flag) {
        if (s_ptr->smana > py.misc.cmana) {
            msg_print("You faint from fatigue!");
            py.flags.paralysis = (int16_t) randint((5 * (s_ptr->smana - py.misc.cmana)));
            py.misc.cmana = 0;
            py.misc.cmana_frac = 0;
            if (randint(3) == 1) {
                msg_print("You have damaged your health!");
                (void) dec_stat(A_CON);
            }
        } else {
            py.misc.cmana -= s_ptr->smana;
        }

        prt_cmana();
    }
}
