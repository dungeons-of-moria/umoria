// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Code for mage spells

#include "headers.h"
#include "externs.h"

static bool canReadSpells() {
    if (py.flags.blind > 0) {
        printMessage("You can't see to read your spell book!");
        return false;
    }

    if (no_light()) {
        printMessage("You have no light to read by.");
        return false;
    }

    if (py.flags.confused > 0) {
        printMessage("You are too confused.");
        return false;
    }

    if (classes[py.misc.pclass].spell != MAGE) {
        printMessage("You can't cast spells!");
        return false;
    }

    return true;
}

static void castSpell(int spellID) {
    int dir;

    switch (spellID) {
        case 1:
            if (get_dir(CNIL, &dir)) {
                fire_bolt(char_row, char_col, dir, diceDamageRoll(2, 6), GF_MAGIC_MISSILE, spell_names[0]);
            }
            break;
        case 2:
            (void) detect_monsters();
            break;
        case 3:
            teleport(10);
            break;
        case 4:
            (void) light_area(char_row, char_col);
            break;
        case 5:
            (void) hp_player(diceDamageRoll(4, 4));
            break;
        case 6:
            (void) detect_sdoor();
            (void) detect_trap();
            break;
        case 7:
            if (get_dir(CNIL, &dir)) {
                fire_ball(char_row, char_col, dir, 12, GF_POISON_GAS, spell_names[6]);
            }
            break;
        case 8:
            if (get_dir(CNIL, &dir)) {
                (void) confuse_monster(char_row, char_col, dir);
            }
            break;
        case 9:
            if (get_dir(CNIL, &dir)) {
                fire_bolt(char_row, char_col, dir, diceDamageRoll(4, 8), GF_LIGHTNING, spell_names[8]);
            }
            break;
        case 10:
            (void) td_destroy();
            break;
        case 11:
            if (get_dir(CNIL, &dir)) {
                (void) sleep_monster(char_row, char_col, dir);
            }
            break;
        case 12:
            (void) cure_poison();
            break;
        case 13:
            teleport((py.misc.lev * 5));
            break;
        case 14:
            for (int id = 22; id < INVEN_ARRAY_SIZE; id++) {
                inventory[id].flags = (uint32_t) (inventory[id].flags & ~TR_CURSED);
            }
            break;
        case 15:
            if (get_dir(CNIL, &dir)) {
                fire_bolt(char_row, char_col, dir, diceDamageRoll(6, 8), GF_FROST, spell_names[14]);
            }
            break;
        case 16:
            if (get_dir(CNIL, &dir)) {
                (void) wall_to_mud(char_row, char_col, dir);
            }
            break;
        case 17:
            create_food();
            break;
        case 18:
            (void) recharge(20);
            break;
        case 19:
            (void) sleep_monsters1(char_row, char_col);
            break;
        case 20:
            if (get_dir(CNIL, &dir)) {
                (void) poly_monster(char_row, char_col, dir);
            }
            break;
        case 21:
            (void) ident_spell();
            break;
        case 22:
            (void) sleep_monsters2();
            break;
        case 23:
            if (get_dir(CNIL, &dir)) {
                fire_bolt(char_row, char_col, dir, diceDamageRoll(9, 8), GF_FIRE, spell_names[22]);
            }
            break;
        case 24:
            if (get_dir(CNIL, &dir)) {
                (void) speed_monster(char_row, char_col, dir, -1);
            }
            break;
        case 25:
            if (get_dir(CNIL, &dir)) {
                fire_ball(char_row, char_col, dir, 48, GF_FROST, spell_names[24]);
            }
            break;
        case 26:
            (void) recharge(60);
            break;
        case 27:
            if (get_dir(CNIL, &dir)) {
                (void) teleport_monster(char_row, char_col, dir);
            }
            break;
        case 28:
            py.flags.fast += randomNumber(20) + py.misc.lev;
            break;
        case 29:
            if (get_dir(CNIL, &dir)) {
                fire_ball(char_row, char_col, dir, 72, GF_FIRE, spell_names[28]);
            }
            break;
        case 30:
            destroy_area(char_row, char_col);
            break;
        case 31:
            (void) genocide();
            break;
        default:
            break;
    }
}

// Throw a magic spell -RAK-
void getAndCastMagicSpell() {
    player_free_turn = true;

    if (!canReadSpells()) {
        return;
    }

    int i, j;
    if (!find_range(TV_MAGIC_BOOK, TV_NEVER, &i, &j)) {
        printMessage("But you are not carrying any spell-books!");
        return;
    }

    int item_val;
    if (!get_item(&item_val, "Use which spell-book?", i, j, CNIL, CNIL)) {
        return;
    }

    int choice, chance;
    int result = cast_spell("Cast which spell?", item_val, &choice, &chance);
    if (result < 0) {
        printMessage("You don't know any spells in that book.");
        return;
    } else if (result == 0) {
        return;
    }

    player_free_turn = false;

    Spell_t *m_ptr = &magic_spells[py.misc.pclass - 1][choice];

    if (randomNumber(100) < chance) {
        printMessage("You failed to get the spell off!");
    } else {
        castSpell(choice + 1);

        if ((spells_worked & (1L << choice)) == 0) {
            py.misc.exp += m_ptr->sexp << 2;
            spells_worked |= (1L << choice);

            prt_experience();
        }
    }

    if (m_ptr->smana > py.misc.cmana) {
        printMessage("You faint from the effort!");

        py.flags.paralysis = (int16_t) randomNumber((5 * (m_ptr->smana - py.misc.cmana)));
        py.misc.cmana = 0;
        py.misc.cmana_frac = 0;

        if (randomNumber(3) == 1) {
            printMessage("You have damaged your health!");
            (void) dec_stat(A_CON);
        }
    } else {
        py.misc.cmana -= m_ptr->smana;
    }

    prt_cmana();
}
