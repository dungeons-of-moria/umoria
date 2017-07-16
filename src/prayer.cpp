// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Code for priest spells

#include "headers.h"
#include "externs.h"

static bool playerCanPray(int *item_pos_begin, int *item_pos_end) {
    if (py.flags.blind > 0) {
        printMessage("You can't see to read your prayer!");
        return false;
    }

    if (playerNoLight()) {
        printMessage("You have no light to read by.");
        return false;
    }

    if (py.flags.confused > 0) {
        printMessage("You are too confused.");
        return false;
    }

    if (classes[py.misc.pclass].spell != PRIEST) {
        printMessage("Pray hard enough and your prayers may be answered.");
        return false;
    }

    if (inventory_count == 0) {
        printMessage("But you are not carrying anything!");
        return false;
    }

    if (!inventoryFindRange(TV_PRAYER_BOOK, TV_NEVER, item_pos_begin, item_pos_end)) {
        printMessage("You are not carrying any Holy Books!");
        return false;
    }

    return true;
}

// Recite a prayers.
static void playerRecitePrayer(int prayer_type) {
    int dir;

    switch (prayer_type + 1) {
        case 1:
            (void) detect_evil();
            break;
        case 2:
            (void) hp_player(diceDamageRoll(3, 3));
            break;
        case 3:
            bless(randomNumber(12) + 12);
            break;
        case 4:
            (void) remove_fear();
            break;
        case 5:
            (void) light_area(char_row, char_col);
            break;
        case 6:
            (void) dungeonDetectTrapOnPanel();
            break;
        case 7:
            (void) dungeonDetectSecretDoorsOnPanel();
            break;
        case 8:
            (void) slow_poison();
            break;
        case 9:
            if (getDirectionWithMemory(CNIL, &dir)) {
                (void) confuse_monster(char_row, char_col, dir);
            }
            break;
        case 10:
            playerTeleport((py.misc.lev * 3));
            break;
        case 11:
            (void) hp_player(diceDamageRoll(4, 4));
            break;
        case 12:
            bless(randomNumber(24) + 24);
            break;
        case 13:
            (void) monsterSleep(char_row, char_col);
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
            py.flags.resist_heat += randomNumber(10) + 10;
            py.flags.resist_cold += randomNumber(10) + 10;
            break;
        case 17:
            (void) cure_poison();
            break;
        case 18:
            if (getDirectionWithMemory(CNIL, &dir)) {
                fire_ball(char_row, char_col, dir, (diceDamageRoll(3, 6) + py.misc.lev), GF_HOLY_ORB, "Black Sphere");
            }
            break;
        case 19:
            (void) hp_player(diceDamageRoll(8, 4));
            break;
        case 20:
            detect_inv2(randomNumber(24) + 24);
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
            (void) hp_player(diceDamageRoll(16, 4));
            break;
        case 25:
            (void) turn_undead();
            break;
        case 26:
            bless(randomNumber(48) + 48);
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
                (void) playerStatRestore(i);
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
    player_free_turn = true;

    int item_pos_begin, item_pos_end;
    if (!playerCanPray(&item_pos_begin, &item_pos_end)) {
        return;
    }

    int item_id;
    if (!inventoryGetInputForItemId(&item_id, "Use which Holy Book?", item_pos_begin, item_pos_end, CNIL, CNIL)) {
        return;
    }

    int choice, chance;
    int result = castSpellGetId("Recite which prayer?", item_id, &choice, &chance);
    if (result < 0) {
        printMessage("You don't know any prayers in that book.");
        return;
    } else if (result == 0) {
        return;
    }

    if (randomNumber(100) < chance) {
        printMessage("You lost your concentration!");
        return;
    }

    Spell_t *spell = &magic_spells[py.misc.pclass - 1][choice];

    // NOTE: at least one function called by `playerRecitePrayer()` sets `player_free_turn = true`,
    // e.g. `create_food()`, so this check is required. -MRC-
    player_free_turn = false;
    playerRecitePrayer(choice);
    if (!player_free_turn) {
        if ((spells_worked & (1L << choice)) == 0) {
            py.misc.exp += spell->sexp << 2;
            displayCharacterExperience();
            spells_worked |= (1L << choice);
        }
    }

    if (!player_free_turn) {
        if (spell->smana > py.misc.cmana) {
            printMessage("You faint from fatigue!");
            py.flags.paralysis = (int16_t) randomNumber((5 * (spell->smana - py.misc.cmana)));
            py.misc.cmana = 0;
            py.misc.cmana_frac = 0;
            if (randomNumber(3) == 1) {
                printMessage("You have damaged your health!");
                (void) playerStatRandomDecrease(A_CON);
            }
        } else {
            py.misc.cmana -= spell->smana;
        }

        printCharacterCurrentMana();
    }
}
