// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// The main command interpreter, updating player status

#include "headers.h"
#include "externs.h"

static char original_commands(char);
static void do_command(char);
static bool valid_countcommand(char);
static void regenhp(int);
static void regenmana(int);
static bool enchanted(inven_type *);
static void examine_book();
static void go_up();
static void go_down();
static void jamdoor();
static void refill_lamp();

// Moria game module -RAK-
// The code in this section has gone through many revisions, and
// some of it could stand some more hard work. -RAK-

// It has had a bit more hard work. -CJS-

// Check light status for dungeon setup
static void updatePlayerLightStatus() {
    player_light = (inventory[INVEN_LIGHT].p1 > 0);
}

// Check for a maximum level
static void updatePlayerMaximumDungeonLevel() {
    if (dun_level > py.misc.max_dlv) {
        py.misc.max_dlv = (uint16_t) dun_level;
    }
}

// Reset flags and initialize variables
static void resetDungeonFlags() {
    command_count = 0;
    new_level_flag = false;
    find_flag = 0;
    teleport_flag = false;
    mon_tot_mult = 0;
    cave[char_row][char_col].cptr = 1;
}

// Check light status
static void checkAndUpdatePlayerLightStatus() {
    inven_type *i_ptr = &inventory[INVEN_LIGHT];

    if (player_light) {
        if (i_ptr->p1 > 0) {
            i_ptr->p1--;
            if (i_ptr->p1 == 0) {
                player_light = false;
                msg_print("Your light has gone out!");
                disturb(0, 1);

                // unlight creatures
                creatures(false);
            } else if (i_ptr->p1 < 40 && randint(5) == 1 && py.flags.blind < 1) {
                disturb(0, 0);
                msg_print("Your light is growing faint.");
            }
        } else {
            player_light = false;
            disturb(0, 1);

            // unlight creatures
            creatures(false);
        }
    } else if (i_ptr->p1 > 0) {
        i_ptr->p1--;
        player_light = true;
        disturb(0, 1);

        // light creatures
        creatures(false);
    }
}

static void playerActivateHeroism() {
    py.flags.status |= PY_HERO;
    disturb(0, 0);
    py.misc.mhp += 10;
    py.misc.chp += 10;
    py.misc.bth += 12;
    py.misc.bthb += 12;
    msg_print("You feel like a HERO!");
    prt_mhp();
    prt_chp();
}

static void playerDisableHeroism() {
    py.flags.status &= ~PY_HERO;
    disturb(0, 0);
    py.misc.mhp -= 10;
    if (py.misc.chp > py.misc.mhp) {
        py.misc.chp = py.misc.mhp;
        py.misc.chp_frac = 0;
        prt_chp();
    }
    py.misc.bth -= 12;
    py.misc.bthb -= 12;
    msg_print("The heroism wears off.");
    prt_mhp();
}

static void playerActivateSuperHeroism() {
    py.flags.status |= PY_SHERO;
    disturb(0, 0);
    py.misc.mhp += 20;
    py.misc.chp += 20;
    py.misc.bth += 24;
    py.misc.bthb += 24;
    msg_print("You feel like a SUPER HERO!");
    prt_mhp();
    prt_chp();
}

static void playerDisableSuperHeroism() {
    py.flags.status &= ~PY_SHERO;
    disturb(0, 0);
    py.misc.mhp -= 20;
    if (py.misc.chp > py.misc.mhp) {
        py.misc.chp = py.misc.mhp;
        py.misc.chp_frac = 0;
        prt_chp();
    }
    py.misc.bth -= 24;
    py.misc.bthb -= 24;
    msg_print("The super heroism wears off.");
    prt_mhp();
}

static void playerUpdateHeroStatus() {
    // Heroism
    if (py.flags.hero > 0) {
        if ((PY_HERO & py.flags.status) == 0) {
            playerActivateHeroism();
        }

        py.flags.hero--;

        if (py.flags.hero == 0) {
            playerDisableHeroism();
        }
    }

    // Super Heroism
    if (py.flags.shero > 0) {
        if ((PY_SHERO & py.flags.status) == 0) {
            playerActivateSuperHeroism();
        }

        py.flags.shero--;

        if (py.flags.shero == 0) {
            playerDisableSuperHeroism();
        }
    }
}

static int playerFoodConsumption() {
    // Regenerate hp and mana
    int regen_amount = PLAYER_REGEN_NORMAL;

    if (py.flags.food < PLAYER_FOOD_ALERT) {
        if (py.flags.food < PLAYER_FOOD_WEAK) {
            if (py.flags.food < 0) {
                regen_amount = 0;
            } else if (py.flags.food < PLAYER_FOOD_FAINT) {
                regen_amount = PLAYER_REGEN_FAINT;
            } else if (py.flags.food < PLAYER_FOOD_WEAK) {
                regen_amount = PLAYER_REGEN_WEAK;
            }
            if ((PY_WEAK & py.flags.status) == 0) {
                py.flags.status |= PY_WEAK;
                msg_print("You are getting weak from hunger.");
                disturb(0, 0);
                prt_hunger();
            }
            if (py.flags.food < PLAYER_FOOD_FAINT && randint(8) == 1) {
                py.flags.paralysis += randint(5);
                msg_print("You faint from the lack of food.");
                disturb(1, 0);
            }
        } else if ((PY_HUNGRY & py.flags.status) == 0) {
            py.flags.status |= PY_HUNGRY;
            msg_print("You are getting hungry.");
            disturb(0, 0);
            prt_hunger();
        }
    }

    // Food consumption
    // Note: Sped up characters really burn up the food!
    if (py.flags.speed < 0) {
        py.flags.food -= py.flags.speed * py.flags.speed;
    }

    py.flags.food -= py.flags.food_digested;

    if (py.flags.food < 0) {
        take_hit(-py.flags.food / 16, "starvation"); // -CJS-
        disturb(1, 0);
    }

    return regen_amount;
}

static void playerUpdateRegeneration(int regen_amount) {
    if (py.flags.regenerate) {
        regen_amount = regen_amount * 3 / 2;
    }

    if ((py.flags.status & PY_SEARCH) || py.flags.rest != 0) {
        regen_amount = regen_amount * 2;
    }

    if (py.flags.poisoned < 1 && py.misc.chp < py.misc.mhp) {
        regenhp(regen_amount);
    }

    if (py.misc.cmana < py.misc.mana) {
        regenmana(regen_amount);
    }
}

static void playerUpdateBlindnessState() {
    if (py.flags.blind <= 0) {
        return;
    }

    if ((PY_BLIND & py.flags.status) == 0) {
        py.flags.status |= PY_BLIND;
        prt_map();
        prt_blind();
        disturb(0, 1);

        // unlight creatures
        creatures(false);
    }

    py.flags.blind--;

    if (py.flags.blind == 0) {
        py.flags.status &= ~PY_BLIND;
        prt_blind();
        prt_map();

        // light creatures
        disturb(0, 1);
        creatures(false);

        msg_print("The veil of darkness lifts.");
    }
}

static void playerUpdateConfusion() {
    if (py.flags.confused <= 0) {
        return;
    }

    if ((PY_CONFUSED & py.flags.status) == 0) {
        py.flags.status |= PY_CONFUSED;
        prt_confused();
    }

    py.flags.confused--;

    if (py.flags.confused == 0) {
        py.flags.status &= ~PY_CONFUSED;
        prt_confused();
        msg_print("You feel less confused now.");
        if (py.flags.rest != 0) {
            rest_off();
        }
    }
}

static void playerUpdateAfraidness() {
    if (py.flags.afraid <= 0) {
        return;
    }

    if ((PY_FEAR & py.flags.status) == 0) {
        if (py.flags.shero + py.flags.hero > 0) {
            py.flags.afraid = 0;
        } else {
            py.flags.status |= PY_FEAR;
            prt_afraid();
        }
    } else if (py.flags.shero + py.flags.hero > 0) {
        py.flags.afraid = 1;
    }

    py.flags.afraid--;

    if (py.flags.afraid == 0) {
        py.flags.status &= ~PY_FEAR;
        prt_afraid();
        msg_print("You feel bolder now.");
        disturb(0, 0);
    }
}

static void playerUpdatePoisonedState() {
    if (py.flags.poisoned <= 0) {
        return;
    }

    if ((PY_POISONED & py.flags.status) == 0) {
        py.flags.status |= PY_POISONED;
        prt_poisoned();
    }

    py.flags.poisoned--;

    if (py.flags.poisoned == 0) {
        py.flags.status &= ~PY_POISONED;
        prt_poisoned();

        msg_print("You feel better.");
        disturb(0, 0);

        return;
    }


    int damage = 0;

    switch (con_adj()) {
        case -4:
            damage = 4;
            break;
        case -3:
        case -2:
            damage = 3;
            break;
        case -1:
            damage = 2;
            break;
        case 0:
            damage = 1;
            break;
        case 1:
        case 2:
        case 3:
            damage = ((turn % 2) == 0);
            break;
        case 4:
        case 5:
            damage = ((turn % 3) == 0);
            break;
        case 6:
            damage = ((turn % 4) == 0);
            break;
    }

    take_hit(damage, "poison");
    disturb(1, 0);
}

static void playerUpdateFastness() {
    if (py.flags.fast <= 0) {
        return;
    }

    if ((PY_FAST & py.flags.status) == 0) {
        py.flags.status |= PY_FAST;
        change_speed(-1);
        msg_print("You feel yourself moving faster.");
        disturb(0, 0);
    }

    py.flags.fast--;

    if (py.flags.fast == 0) {
        py.flags.status &= ~PY_FAST;
        change_speed(1);
        msg_print("You feel yourself slow down.");
        disturb(0, 0);
    }
}

static void playerUpdateSlowness() {
    if (py.flags.slow <= 0) {
        return;
    }

    if ((PY_SLOW & py.flags.status) == 0) {
        py.flags.status |= PY_SLOW;
        change_speed(1);
        msg_print("You feel yourself moving slower.");
        disturb(0, 0);
    }

    py.flags.slow--;

    if (py.flags.slow == 0) {
        py.flags.status &= ~PY_SLOW;
        change_speed(-1);
        msg_print("You feel yourself speed up.");
        disturb(0, 0);
    }
}

// Resting is over?
static void playerUpdateRestingState() {
    if (py.flags.rest > 0) {
        py.flags.rest--;

        // Resting over
        if (py.flags.rest == 0) {
            rest_off();
        }
    } else if (py.flags.rest < 0) {
        // Rest until reach max mana and max hit points.
        py.flags.rest++;

        if ((py.misc.chp == py.misc.mhp && py.misc.cmana == py.misc.mana) || py.flags.rest == 0) {
            rest_off();
        }
    }
}

// Hallucinating?   (Random characters appear!)
static void playerUpdateHallucinationState() {
    if (py.flags.image <= 0) {
        return;
    }

    end_find();

    py.flags.image--;

    if (py.flags.image == 0) {
        // Used to draw entire screen! -CJS-
        prt_map();
    }
}

static void playerUpdateParalysis() {
    if (py.flags.paralysis <= 0) {
        return;
    }

    // when paralysis true, you can not see any movement that occurs
    py.flags.paralysis--;
    disturb(1, 0);
}

// Protection from evil counter
static void playerUpdateEvilProtection() {
    if (py.flags.protevil <= 0) {
        return;
    }

    py.flags.protevil--;

    if (py.flags.protevil == 0) {
        msg_print("You no longer feel safe from evil.");
    }
}

static void playerUpdateInvulnerability() {
    if (py.flags.invuln <= 0) {
        return;
    }

    if ((PY_INVULN & py.flags.status) == 0) {
        py.flags.status |= PY_INVULN;
        disturb(0, 0);
        py.misc.pac += 100;
        py.misc.dis_ac += 100;

        prt_pac();
        msg_print("Your skin turns into steel!");
    }

    py.flags.invuln--;

    if (py.flags.invuln == 0) {
        py.flags.status &= ~PY_INVULN;
        disturb(0, 0);
        py.misc.pac -= 100;
        py.misc.dis_ac -= 100;

        prt_pac();
        msg_print("Your skin returns to normal.");
    }

}

static void playerUpdateBlessedness() {
    if (py.flags.blessed <= 0) {
        return;
    }

    if ((PY_BLESSED & py.flags.status) == 0) {
        py.flags.status |= PY_BLESSED;
        disturb(0, 0);
        py.misc.bth += 5;
        py.misc.bthb += 5;
        py.misc.pac += 2;
        py.misc.dis_ac += 2;

        msg_print("You feel righteous!");
        prt_pac();
    }

    py.flags.blessed--;

    if (py.flags.blessed == 0) {
        py.flags.status &= ~PY_BLESSED;
        disturb(0, 0);
        py.misc.bth -= 5;
        py.misc.bthb -= 5;
        py.misc.pac -= 2;
        py.misc.dis_ac -= 2;

        msg_print("The prayer has expired.");
        prt_pac();
    }

}

// Resist Heat
static void playerUpdateHeatResistance() {
    if (py.flags.resist_heat <= 0) {
        return;
    }

    py.flags.resist_heat--;

    if (py.flags.resist_heat == 0) {
        msg_print("You no longer feel safe from flame.");
    }
}

static void playerUpdateColdResistance() {
    if (py.flags.resist_cold <= 0) {
        return;
    }

    py.flags.resist_cold--;

    if (py.flags.resist_cold == 0) {
        msg_print("You no longer feel safe from cold.");
    }
}

static void playerUpdateDetectInvisible() {
    if (py.flags.detect_inv <= 0) {
        return;
    }

    if ((PY_DET_INV & py.flags.status) == 0) {
        py.flags.status |= PY_DET_INV;
        py.flags.see_inv = true;

        // light but don't move creatures
        creatures(false);
    }

    py.flags.detect_inv--;

    if (py.flags.detect_inv == 0) {
        py.flags.status &= ~PY_DET_INV;

        // may still be able to see_inv if wearing magic item
        calc_bonuses();

        // unlight but don't move creatures
        creatures(false);
    }
}

// Timed infra-vision
static void playerUpdateInfraVision() {
    if (py.flags.tim_infra <= 0) {
        return;
    }

    if ((PY_TIM_INFRA & py.flags.status) == 0) {
        py.flags.status |= PY_TIM_INFRA;
        py.flags.see_infra++;

        // light but don't move creatures
        creatures(false);
    }

    py.flags.tim_infra--;

    if (py.flags.tim_infra == 0) {
        py.flags.status &= ~PY_TIM_INFRA;
        py.flags.see_infra--;

        // unlight but don't move creatures
        creatures(false);
    }
}

// Word-of-Recall  Note: Word-of-Recall is a delayed action
static void playerUpdateWordOfRecall() {
    if (py.flags.word_recall <= 0) {
        return;
    }

    if (py.flags.word_recall == 1) {
        new_level_flag = true;
        py.flags.paralysis++;
        py.flags.word_recall = 0;
        if (dun_level > 0) {
            dun_level = 0;
            msg_print("You feel yourself yanked upwards!");
        } else if (py.misc.max_dlv != 0) {
            dun_level = py.misc.max_dlv;
            msg_print("You feel yourself yanked downwards!");
        }
    } else {
        py.flags.word_recall--;
    }
}

static int getRepeatCommandCount(char *lastInputCharacter) {
    prt("Repeat count:", 0, 0);
    if (*lastInputCharacter == '#') {
        *lastInputCharacter = '0';
    }

    int count = 0;
    char countMsg[8];

    while (true) {
        if (*lastInputCharacter == DELETE || *lastInputCharacter == CTRL_KEY('H')) {
            count = count / 10;
            (void) sprintf(countMsg, "%d", count);
            prt(countMsg, 0, 14);
        } else if (*lastInputCharacter >= '0' && *lastInputCharacter <= '9') {
            if (count > 99) {
                bell();
            } else {
                count = count * 10 + *lastInputCharacter - '0';
                (void) sprintf(countMsg, "%d", count);
                prt(countMsg, 0, 14);
            }
        } else {
            break;
        }
        *lastInputCharacter = inkey();
    }

    if (count == 0) {
        count = 99;
        (void) sprintf(countMsg, "%d", count);
        prt(countMsg, 0, 14);
    }

    // a special hack to allow numbers as commands
    if (*lastInputCharacter == ' ') {
        prt("Command:", 0, 20);
        *lastInputCharacter = inkey();
    }

    return count;
}

// Another way of typing control codes -CJS-
static void processControlCharacterInput(char *lastInputCharacter) {
    if (*lastInputCharacter != '^') {
        return;
    }

    if (command_count > 0) {
        prt_state();
    }

    if (get_com("Control-", lastInputCharacter)) {
        if (*lastInputCharacter >= 'A' && *lastInputCharacter <= 'Z') {
            *lastInputCharacter -= 'A' - 1;
        } else if (*lastInputCharacter >= 'a' && *lastInputCharacter <= 'z') {
            *lastInputCharacter -= 'a' - 1;
        } else {
            msg_print("Type ^ <letter> for a control char");
            *lastInputCharacter = ' ';
        }
    } else {
        *lastInputCharacter = ' ';
    }
}

// Accept a command and execute it
static int doCommandInput(int find_count) {
    char lastInputCharacter;

    do {
        if (py.flags.status & PY_REPEAT) {
            prt_state();
        }

        default_dir = false;
        free_turn_flag = false;

        if (find_flag) {
            find_run();
            find_count--;
            if (find_count == 0) {
                end_find();
            }
            put_qio();
            continue;
        }

        if (doing_inven) {
            inven_command(doing_inven);
            continue;
        }

        // move the cursor to the players character
        move_cursor_relative(char_row, char_col);

        msg_flag = false;

        if (command_count > 0) {
            default_dir = true;
        } else {
            lastInputCharacter = inkey();

            int repeatCount = 0;

            // Get a count for a command.
            if ((rogue_like_commands && lastInputCharacter >= '0' && lastInputCharacter <= '9') || (!rogue_like_commands && lastInputCharacter == '#')) {
                repeatCount = getRepeatCommandCount(&lastInputCharacter);
            }

            // Another way of typing control codes -CJS-
            processControlCharacterInput(&lastInputCharacter);

            // move cursor to player char again, in case it moved
            move_cursor_relative(char_row, char_col);

            // Commands are always converted to rogue form. -CJS-
            if (!rogue_like_commands) {
                lastInputCharacter = original_commands(lastInputCharacter);
            }

            if (repeatCount > 0) {
                if (!valid_countcommand(lastInputCharacter)) {
                    free_turn_flag = true;
                    msg_print("Invalid command with a count.");
                    lastInputCharacter = ' ';
                } else {
                    command_count = repeatCount;
                    prt_state();
                }
            }
        }

        // Flash the message line.
        erase_line(MSG_LINE, 0);
        move_cursor_relative(char_row, char_col);
        put_qio();

        do_command(lastInputCharacter);

        // Find is counted differently, as the command changes.
        if (find_flag) {
            find_count = command_count - 1;
            command_count = 0;
        } else if (free_turn_flag) {
            command_count = 0;
        } else if (command_count) {
            command_count--;
        }
    } while (free_turn_flag && !new_level_flag && !eof_flag);

    return find_count;
}

// Main procedure for dungeon. -RAK-
void dungeon() {
    // Note: There is a lot of preliminary magic going on here at first
    updatePlayerLightStatus();
    updatePlayerMaximumDungeonLevel();
    resetDungeonFlags();

    int find_count = 0;

    // Ensure we display the panel. Used to do this with a global var. -CJS-
    panel_row = panel_col = -1;

    // Light up the area around character
    check_view();

    // must do this after panel_row/col set to -1, because search_off() will
    // call check_view(), and so the panel_* variables must be valid before
    // search_off() is called
    if (py.flags.status & PY_SEARCH) {
        search_off();
    }

    // Light,  but do not move critters
    creatures(false);

    // Print the depth
    prt_depth();

    // Loop until dead,  or new level
    // Exit when `new_level_flag` and `eof_flag` are both set
    do {
        // Increment turn counter
        turn++;

        // turn over the store contents every, say, 1000 turns
        if (dun_level != 0 && (turn % 1000) == 0) {
            store_maint();
        }

        // Check for creature generation
        if (randint(MAX_MALLOC_CHANCE) == 1) {
            alloc_monster(1, MAX_SIGHT, false);
        }

        checkAndUpdatePlayerLightStatus();

        //
        // Update counters and messages
        //

        // Heroism and Super Heroism must precede anything that can damage player
        playerUpdateHeroStatus();

        int regen_amount = playerFoodConsumption();
        playerUpdateRegeneration(regen_amount);

        playerUpdateBlindnessState();
        playerUpdateConfusion();
        playerUpdateAfraidness();
        playerUpdatePoisonedState();
        playerUpdateFastness();
        playerUpdateSlowness();
        playerUpdateRestingState();

        // Check for interrupts to find or rest.
        if ((command_count > 0 || find_flag || py.flags.rest != 0) && check_input(find_flag ? 0 : 10000)) {
            disturb(0, 0);
        }

        playerUpdateHallucinationState();
        playerUpdateParalysis();
        playerUpdateEvilProtection();
        playerUpdateInvulnerability();
        playerUpdateBlessedness();
        playerUpdateHeatResistance();
        playerUpdateColdResistance();
        playerUpdateDetectInvisible();
        playerUpdateInfraVision();
        playerUpdateWordOfRecall();

        // Random teleportation
        if (py.flags.teleport && randint(100) == 1) {
            disturb(0, 0);
            teleport(40);
        }

        // See if we are too weak to handle the weapon or pack. -CJS-
        if (py.flags.status & PY_STR_WGT) {
            check_strength();
        }

        if (py.flags.status & PY_STUDY) {
            prt_study();
        }

        if (py.flags.status & PY_SPEED) {
            py.flags.status &= ~PY_SPEED;
            prt_speed();
        }

        if ((py.flags.status & PY_PARALYSED) && py.flags.paralysis < 1) {
            prt_state();
            py.flags.status &= ~PY_PARALYSED;
        } else if (py.flags.paralysis > 0) {
            prt_state();
            py.flags.status |= PY_PARALYSED;
        } else if (py.flags.rest != 0) {
            prt_state();
        }

        if ((py.flags.status & PY_ARMOR) != 0) {
            prt_pac();
            py.flags.status &= ~PY_ARMOR;
        }

        if ((py.flags.status & PY_STATS) != 0) {
            for (int n = 0; n < 6; n++) {
                if ((PY_STR << n) & py.flags.status) {
                    prt_stat(n);
                }
            }

            py.flags.status &= ~PY_STATS;
        }

        if (py.flags.status & PY_HP) {
            prt_mhp();
            prt_chp();
            py.flags.status &= ~PY_HP;
        }

        if (py.flags.status & PY_MANA) {
            prt_cmana();
            py.flags.status &= ~PY_MANA;
        }

        // Allow for a slim chance of detect enchantment -CJS-
        // for 1st level char, check once every 2160 turns
        // for 40th level char, check once every 416 turns
        if ((turn & 0xF) == 0 && py.flags.confused == 0 && randint(10 + 750 / (5 + py.misc.lev)) == 1) {
            for (int i = 0; i < INVEN_ARRAY_SIZE; i++) {
                if (i == inven_ctr) {
                    i = 22;
                }

                inven_type *i_ptr = &inventory[i];

                // if in inventory, succeed 1 out of 50 times,
                // if in equipment list, success 1 out of 10 times
                if (i_ptr->tval != TV_NOTHING && enchanted(i_ptr) && randint(i < 22 ? 50 : 10) == 1) {
                    extern char *describe_use(int);

                    vtype tmp_str;
                    (void) sprintf(tmp_str, "There's something about what you are %s...", describe_use(i));
                    disturb(0, 0);
                    msg_print(tmp_str);
                    add_inscribe(i_ptr, ID_MAGIK);
                }
            }
        }

        // Check the state of the monster list, and delete some monsters if
        // the monster list is nearly full.  This helps to avoid problems in
        // creature.c when monsters try to multiply.  Compact_monsters() is
        // much more likely to succeed if called from here, than if called
        // from within creatures().
        if (MAX_MALLOC - mfptr < 10) {
            (void) compact_monsters();
        }

        // Accept a command?
        if (py.flags.paralysis < 1 && py.flags.rest == 0 && !death) {
            // Accept a command and execute it
            find_count = doCommandInput(find_count);
        } else {
            // if paralyzed, resting, or dead, flush output
            // but first move the cursor onto the player, for aesthetics
            move_cursor_relative(char_row, char_col);
            put_qio();
        }

        // Teleport?
        if (teleport_flag) {
            teleport(100);
        }

        // Move the creatures
        if (!new_level_flag) {
            creatures(true);
        }
    } while (!new_level_flag && !eof_flag);
}

static char original_commands(char com_val) {
    int dir_val;

    switch (com_val) {
        case CTRL_KEY('K'): // ^K = exit
            com_val = 'Q';
            break;
        case CTRL_KEY('J'):
        case CTRL_KEY('M'):
            com_val = '+';
            break;
        case CTRL_KEY('P'): // ^P = repeat
        case CTRL_KEY('W'): // ^W = password
        case CTRL_KEY('X'): // ^X = save
        case CTRL_KEY('V'): // ^V = view license
        case ' ':
        case '!':
        case '$':
            break;
        case '.':
            if (get_dir(CNIL, &dir_val)) {
                switch (dir_val) {
                    case 1:
                        com_val = 'B';
                        break;
                    case 2:
                        com_val = 'J';
                        break;
                    case 3:
                        com_val = 'N';
                        break;
                    case 4:
                        com_val = 'H';
                        break;
                    case 6:
                        com_val = 'L';
                        break;
                    case 7:
                        com_val = 'Y';
                        break;
                    case 8:
                        com_val = 'K';
                        break;
                    case 9:
                        com_val = 'U';
                        break;
                    default:
                        com_val = ' ';
                        break;
                }
            } else {
                com_val = ' ';
            }
            break;
        case '/':
        case '<':
        case '>':
        case '-':
        case '=':
        case '{':
        case '?':
        case 'A':
            break;
        case '1':
            com_val = 'b';
            break;
        case '2':
            com_val = 'j';
            break;
        case '3':
            com_val = 'n';
            break;
        case '4':
            com_val = 'h';
            break;
        case '5': // Rest one turn
            com_val = '.';
            break;
        case '6':
            com_val = 'l';
            break;
        case '7':
            com_val = 'y';
            break;
        case '8':
            com_val = 'k';
            break;
        case '9':
            com_val = 'u';
            break;
        case 'B':
            com_val = 'f';
            break;
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
            break;
        case 'L':
            com_val = 'W';
            break;
        case 'M':
            break;
        case 'R':
            break;
        case 'S':
            com_val = '#';
            break;
        case 'T':
            if (get_dir(CNIL, &dir_val)) {
                switch (dir_val) {
                    case 1:
                        com_val = CTRL_KEY('B');
                        break;
                    case 2:
                        com_val = CTRL_KEY('J');
                        break;
                    case 3:
                        com_val = CTRL_KEY('N');
                        break;
                    case 4:
                        com_val = CTRL_KEY('H');
                        break;
                    case 6:
                        com_val = CTRL_KEY('L');
                        break;
                    case 7:
                        com_val = CTRL_KEY('Y');
                        break;
                    case 8:
                        com_val = CTRL_KEY('K');
                        break;
                    case 9:
                        com_val = CTRL_KEY('U');
                        break;
                    default:
                        com_val = ' ';
                        break;
                }
            } else {
                com_val = ' ';
            }
            break;
        case 'V':
            break;
        case 'a':
            com_val = 'z';
            break;
        case 'b':
            com_val = 'P';
            break;
        case 'c':
        case 'd':
        case 'e':
            break;
        case 'f':
            com_val = 't';
            break;
        case 'h':
            com_val = '?';
            break;
        case 'i':
            break;
        case 'j':
            com_val = 'S';
            break;
        case 'l':
            com_val = 'x';
            break;
        case 'm':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
            break;
        case 't':
            com_val = 'T';
            break;
        case 'u':
            com_val = 'Z';
            break;
        case 'v':
        case 'w':
            break;
        case 'x':
            com_val = 'X';
            break;

            // wizard mode commands follow
        case CTRL_KEY('A'): // ^A = cure all
            break;
        case CTRL_KEY('B'): // ^B = objects
            com_val = CTRL_KEY('O');
            break;
        case CTRL_KEY('D'): // ^D = up/down
            break;
        case CTRL_KEY('H'): // ^H = wizhelp
            com_val = '\\';
            break;
        case CTRL_KEY('I'): // ^I = identify
            break;
        case CTRL_KEY('L'): // ^L = wizlight
            com_val = '*';
            break;
        case ':':
        case CTRL_KEY('T'): // ^T = teleport
        case CTRL_KEY('E'): // ^E = wizchar
        case CTRL_KEY('F'): // ^F = genocide
        case CTRL_KEY('G'): // ^G = treasure
        case '@':
        case '+':
            break;
        case CTRL_KEY('U'): // ^U = summon
            com_val = '&';
            break;
        default:
            com_val = '~'; // Anything illegal.
            break;
    }
    return com_val;
}

static void do_command(char com_val) {
    int dir_val;
    bool do_pickup;
    int y, x, i, j;
    vtype out_val, tmp_str;
    struct player_type::flags *f_ptr;

    // hack for move without pickup.  Map '-' to a movement command.
    if (com_val == '-') {
        do_pickup = false;
        i = command_count;

        if (get_dir(CNIL, &dir_val)) {
            command_count = i;
            switch (dir_val) {
                case 1:
                    com_val = 'b';
                    break;
                case 2:
                    com_val = 'j';
                    break;
                case 3:
                    com_val = 'n';
                    break;
                case 4:
                    com_val = 'h';
                    break;
                case 6:
                    com_val = 'l';
                    break;
                case 7:
                    com_val = 'y';
                    break;
                case 8:
                    com_val = 'k';
                    break;
                case 9:
                    com_val = 'u';
                    break;
                default:
                    com_val = '~';
                    break;
            }
        } else {
            com_val = ' ';
        }
    } else {
        do_pickup = true;
    }

    switch (com_val) {
        case 'Q': // (Q)uit    (^K)ill
            flush();
            if (get_check("Do you really want to quit?")) {
                new_level_flag = true;
                death = true;
                (void) strcpy(died_from, "Quitting");
            }
            free_turn_flag = true;
            break;
        case CTRL_KEY('P'): // (^P)revious message.
            if (command_count > 0) {
                i = command_count;
                if (i > MAX_SAVE_MSG) {
                    i = MAX_SAVE_MSG;
                }
                command_count = 0;
            } else if (last_command != CTRL_KEY('P')) {
                i = 1;
            } else {
                i = MAX_SAVE_MSG;
            }

            j = last_msg;

            if (i > 1) {
                save_screen();
                x = i;

                while (i > 0) {
                    i--;
                    prt(old_msg[j], i, 0);
                    if (j == 0) {
                        j = MAX_SAVE_MSG - 1;
                    } else {
                        j--;
                    }
                }

                erase_line(x, 0);
                pause_line(x);
                restore_screen();
            } else {
                // Distinguish real and recovered messages with a '>'. -CJS-
                put_buffer(">", 0, 0);
                prt(old_msg[j], 0, 1);
            }

            free_turn_flag = true;
            break;
        case CTRL_KEY('V'): // (^V)iew license
            helpfile(MORIA_GPL);
            free_turn_flag = true;
            break;
        case CTRL_KEY('W'): // (^W)izard mode
            if (wizard) {
                wizard = false;
                msg_print("Wizard mode off.");
            } else if (enter_wiz_mode()) {
                msg_print("Wizard mode on.");
            }

            prt_winner();
            free_turn_flag = true;
            break;
        case CTRL_KEY('X'): // e(^X)it and save
            if (total_winner) {
                msg_print("You are a Total Winner,  your character must be retired.");
                if (rogue_like_commands) {
                    msg_print("Use 'Q' to when you are ready to quit.");
                } else {
                    msg_print("Use <Control>-K when you are ready to quit.");
                }
            } else {
                (void) strcpy(died_from, "(saved)");
                msg_print("Saving game...");

                if (save_char()) {
                    exit_game();
                }

                (void) strcpy(died_from, "(alive and well)");
            }

            free_turn_flag = true;
            break;
        case '=': // (=) set options
            save_screen();
            set_options();
            restore_screen();
            free_turn_flag = true;
            break;
        case '{': // ({) inscribe an object
            scribe_object();
            free_turn_flag = true;
            break;
        case '!': // (!) escape to the shell
        case '$':
            shell_out();
            free_turn_flag = true;
            break;
        case ESCAPE: // (ESC)   do nothing.
        case ' ':    // (space) do nothing.
            free_turn_flag = true;
            break;
        case 'b': // (b) down, left  (1)
            move_char(1, do_pickup);
            break;
        case 'j': // (j) down    (2)
            move_char(2, do_pickup);
            break;
        case 'n': // (n) down, right  (3)
            move_char(3, do_pickup);
            break;
        case 'h': // (h) left    (4)
            move_char(4, do_pickup);
            break;
        case 'l': // (l) right    (6)
            move_char(6, do_pickup);
            break;
        case 'y': // (y) up, left    (7)
            move_char(7, do_pickup);
            break;
        case 'k': // (k) up    (8)
            move_char(8, do_pickup);
            break;
        case 'u': // (u) up, right  (9)
            move_char(9, do_pickup);
            break;
        case 'B': // (B) run down, left  (. 1)
            find_init(1);
            break;
        case 'J': // (J) run down    (. 2)
            find_init(2);
            break;
        case 'N': // (N) run down, right  (. 3)
            find_init(3);
            break;
        case 'H': // (H) run left    (. 4)
            find_init(4);
            break;
        case 'L': // (L) run right  (. 6)
            find_init(6);
            break;
        case 'Y': // (Y) run up, left  (. 7)
            find_init(7);
            break;
        case 'K': // (K) run up    (. 8)
            find_init(8);
            break;
        case 'U': // (U) run up, right  (. 9)
            find_init(9);
            break;
        case '/': // (/) identify a symbol
            ident_char();
            free_turn_flag = true;
            break;
        case '.': // (.) stay in one place (5)
            move_char(5, do_pickup);
            if (command_count > 1) {
                command_count--;
                rest();
            }
            break;
        case '<': // (<) go down a staircase
            go_up();
            break;
        case '>': // (>) go up a staircase
            go_down();
            break;
        case '?': // (?) help with commands
            if (rogue_like_commands) {
                helpfile(MORIA_HELP);
            } else {
                helpfile(MORIA_ORIG_HELP);
            }
            free_turn_flag = true;
            break;
        case 'f': // (f)orce    (B)ash
            bash();
            break;
        case 'C': // (C)haracter description
            save_screen();
            change_name();
            restore_screen();
            free_turn_flag = true;
            break;
        case 'D': // (D)isarm trap
            disarm_trap();
            break;
        case 'E': // (E)at food
            eat();
            break;
        case 'F': // (F)ill lamp
            refill_lamp();
            break;
        case 'G': // (G)ain magic spells
            gain_spells();
            break;
        case 'V': // (V)iew scores
            save_screen();
            display_scores();
            restore_screen();
            free_turn_flag = true;
            break;
        case 'W': // (W)here are we on the map  (L)ocate on map
            if (py.flags.blind > 0 || no_light()) {
                msg_print("You can't see your map.");
            } else {
                int cy, cx, p_y, p_x;

                y = char_row;
                x = char_col;
                if (get_panel(y, x, true)) {
                    prt_map();
                }
                cy = panel_row;
                cx = panel_col;
                for (;;) {
                    p_y = panel_row;
                    p_x = panel_col;
                    if (p_y == cy && p_x == cx) {
                        tmp_str[0] = '\0';
                    } else {
                        (void) sprintf(tmp_str, "%s%s of", p_y < cy ? " North" : p_y > cy ? " South" : "", p_x < cx ? " West" : p_x > cx ? " East" : "");
                    }
                    (void) sprintf(out_val, "Map sector [%d,%d], which is%s your sector. Look which direction?", p_y, p_x, tmp_str);
                    if (!get_dir(out_val, &dir_val)) {
                        break;
                    }

                    // -CJS-
                    // Should really use the move function, but what the hell. This
                    // is nicer, as it moves exactly to the same place in another
                    // section. The direction calculation is not intuitive. Sorry.
                    for (;;) {
                        x += ((dir_val - 1) % 3 - 1) * SCREEN_WIDTH / 2;
                        y -= ((dir_val - 1) / 3 - 1) * SCREEN_HEIGHT / 2;
                        if (x < 0 || y < 0 || x >= cur_width || y >= cur_width) {
                            msg_print("You've gone past the end of your map.");
                            x -= ((dir_val - 1) % 3 - 1) * SCREEN_WIDTH / 2;
                            y += ((dir_val - 1) / 3 - 1) * SCREEN_HEIGHT / 2;
                            break;
                        }
                        if (get_panel(y, x, true)) {
                            prt_map();
                            break;
                        }
                    }
                }

                // Move to a new panel - but only if really necessary.
                if (get_panel(char_row, char_col, false)) {
                    prt_map();
                }
            }
            free_turn_flag = true;
            break;
        case 'R': // (R)est a while
            rest();
            break;
        case '#': // (#) search toggle  (S)earch toggle
            if (py.flags.status & PY_SEARCH) {
                search_off();
            } else {
                search_on();
            }
            free_turn_flag = true;
            break;
        case CTRL_KEY('B'): // (^B) tunnel down left  (T 1)
            tunnel(1);
            break;
        case CTRL_KEY('M'): // cr must be treated same as lf.
        case CTRL_KEY('J'): // (^J) tunnel down    (T 2)
            tunnel(2);
            break;
        case CTRL_KEY('N'): // (^N) tunnel down right  (T 3)
            tunnel(3);
            break;
        case CTRL_KEY('H'): // (^H) tunnel left    (T 4)
            tunnel(4);
            break;
        case CTRL_KEY('L'): // (^L) tunnel right    (T 6)
            tunnel(6);
            break;
        case CTRL_KEY('Y'): // (^Y) tunnel up left    (T 7)
            tunnel(7);
            break;
        case CTRL_KEY('K'): // (^K) tunnel up    (T 8)
            tunnel(8);
            break;
        case CTRL_KEY('U'): // (^U) tunnel up right    (T 9)
            tunnel(9);
            break;
        case 'z': // (z)ap a wand    (a)im a wand
            aim();
            break;
        case 'M':
            screen_map();
            free_turn_flag = true;
            break;
        case 'P': // (P)eruse a book  (B)rowse in a book
            examine_book();
            free_turn_flag = true;
            break;
        case 'c': // (c)lose an object
            closeobject();
            break;
        case 'd': // (d)rop something
            inven_command('d');
            break;
        case 'e': // (e)quipment list
            inven_command('e');
            break;
        case 't': // (t)hrow something  (f)ire something
            throw_object();
            break;
        case 'i': // (i)nventory list
            inven_command('i');
            break;
        case 'S': // (S)pike a door  (j)am a door
            jamdoor();
            break;
        case 'x': // e(x)amine surrounds  (l)ook about
            look();
            free_turn_flag = true;
            break;
        case 'm': // (m)agic spells
            cast();
            break;
        case 'o': // (o)pen something
            openobject();
            break;
        case 'p': // (p)ray
            pray();
            break;
        case 'q': // (q)uaff
            quaff();
            break;
        case 'r': // (r)ead
            read_scroll();
            break;
        case 's': // (s)earch for a turn
            search(char_row, char_col, py.misc.srh);
            break;
        case 'T': // (T)ake off something  (t)ake off
            inven_command('t');
            break;
        case 'Z': // (Z)ap a staff  (u)se a staff
            use();
            break;
        case 'v': // (v)ersion of game
            helpfile(MORIA_VER);
            free_turn_flag = true;
            break;
        case 'w': // (w)ear or wield
            inven_command('w');
            break;
        case 'X': // e(X)change weapons  e(x)change
            inven_command('x');
            break;
        default:
            if (wizard) {
                // Wizard commands are free moves
                free_turn_flag = true;

                switch (com_val) {
                    case CTRL_KEY('A'): // ^A = Cure all
                        (void) remove_curse();
                        (void) cure_blindness();
                        (void) cure_confusion();
                        (void) cure_poison();
                        (void) remove_fear();
                        (void) res_stat(A_STR);
                        (void) res_stat(A_INT);
                        (void) res_stat(A_WIS);
                        (void) res_stat(A_CON);
                        (void) res_stat(A_DEX);
                        (void) res_stat(A_CHR);
                        f_ptr = &py.flags;
                        if (f_ptr->slow > 1) {
                            f_ptr->slow = 1;
                        }
                        if (f_ptr->image > 1) {
                            f_ptr->image = 1;
                        }
                        break;
                    case CTRL_KEY('E'): // ^E = wizchar
                        change_character();
                        erase_line(MSG_LINE, 0);
                        break;
                    case CTRL_KEY('F'): // ^F = genocide
                        (void) mass_genocide();
                        break;
                    case CTRL_KEY('G'): // ^G = treasure
                        if (command_count > 0) {
                            i = command_count;
                            command_count = 0;
                        } else {
                            i = 1;
                        }
                        random_object(char_row, char_col, i);
                        prt_map();
                        break;
                    case CTRL_KEY('D'): // ^D = up/down
                        if (command_count > 0) {
                            if (command_count > 99) {
                                i = 0;
                            } else {
                                i = command_count;
                            }
                            command_count = 0;
                        } else {
                            prt("Go to which level (0-99) ? ", 0, 0);
                            i = -1;
                            if (get_string(tmp_str, 0, 27, 10)) {
                                i = atoi(tmp_str);
                            }
                        }
                        if (i > -1) {
                            dun_level = (int16_t) i;
                            if (dun_level > 99) {
                                dun_level = 99;
                            }
                            new_level_flag = true;
                        } else {
                            erase_line(MSG_LINE, 0);
                        }
                        break;
                    case CTRL_KEY('O'): // ^O = objects
                        print_objects();
                        break;
                    case '\\': // \ wizard help
                        if (rogue_like_commands) {
                            helpfile(MORIA_WIZ_HELP);
                        } else {
                            helpfile(MORIA_OWIZ_HELP);
                        }
                        break;
                    case CTRL_KEY('I'): // ^I = identify
                        (void) ident_spell();
                        break;
                    case '*':
                        wizard_light();
                        break;
                    case ':':
                        map_area();
                        break;
                    case CTRL_KEY('T'): // ^T = teleport
                        teleport(100);
                        break;
                    case '+':
                        if (command_count > 0) {
                            py.misc.exp = command_count;
                            command_count = 0;
                        } else if (py.misc.exp == 0) {
                            py.misc.exp = 1;
                        } else {
                            py.misc.exp = py.misc.exp * 2;
                        }
                        prt_experience();
                        break;
                    case '&': // & = summon
                        y = char_row;
                        x = char_col;
                        (void) summon_monster(&y, &x, true);
                        creatures(false);
                        break;
                    case '@':
                        wizard_create();
                        break;
                    default:
                        if (rogue_like_commands) {
                            prt("Type '?' or '\\' for help.", 0, 0);
                        } else {
                            prt("Type '?' or ^H for help.", 0, 0);
                        }
                }
            } else {
                prt("Type '?' for help.", 0, 0);
                free_turn_flag = true;
            }
    }
    last_command = com_val;
}

// Check whether this command will accept a count. -CJS-
static bool valid_countcommand(char c) {
    switch (c) {
        case 'Q':
        case CTRL_KEY('W'):
        case CTRL_KEY('X'):
        case '=':
        case '{':
        case '/':
        case '<':
        case '>':
        case '?':
        case 'C':
        case 'E':
        case 'F':
        case 'G':
        case 'V':
        case '#':
        case 'z':
        case 'P':
        case 'c':
        case 'd':
        case 'e':
        case 't':
        case 'i':
        case 'x':
        case 'm':
        case 'p':
        case 'q':
        case 'r':
        case 'T':
        case 'Z':
        case 'v':
        case 'w':
        case 'W':
        case 'X':
        case CTRL_KEY('A'):
        case '\\':
        case CTRL_KEY('I'):
        case '*':
        case ':':
        case CTRL_KEY('T'):
        case CTRL_KEY('E'):
        case CTRL_KEY('F'):
        case CTRL_KEY('S'):
        case CTRL_KEY('Q'):
            return false;
        case CTRL_KEY('P'):
        case ESCAPE:
        case ' ':
        case '-':
        case 'b':
        case 'f':
        case 'j':
        case 'n':
        case 'h':
        case 'l':
        case 'y':
        case 'k':
        case 'u':
        case '.':
        case 'B':
        case 'J':
        case 'N':
        case 'H':
        case 'L':
        case 'Y':
        case 'K':
        case 'U':
        case 'D':
        case 'R':
        case CTRL_KEY('Y'):
        case CTRL_KEY('K'):
        case CTRL_KEY('U'):
        case CTRL_KEY('L'):
        case CTRL_KEY('N'):
        case CTRL_KEY('J'):
        case CTRL_KEY('B'):
        case CTRL_KEY('H'):
        case 'S':
        case 'o':
        case 's':
        case CTRL_KEY('D'):
        case CTRL_KEY('G'):
        case '+':
            return true;
        default:
            return false;
    }
}

// Regenerate hit points -RAK-
static void regenhp(int percent) {
    struct player_type::misc *p_ptr = &py.misc;
    int old_chp = p_ptr->chp;
    int32_t new_chp = ((int32_t) p_ptr->mhp) * percent + PLAYER_REGEN_HPBASE;

    // div 65536
    p_ptr->chp += new_chp >> 16;

    // check for overflow
    if (p_ptr->chp < 0 && old_chp > 0) {
        p_ptr->chp = MAX_SHORT;
    }

    // mod 65536
    int32_t new_chp_frac = (new_chp & 0xFFFF) + p_ptr->chp_frac;

    if (new_chp_frac >= 0x10000L) {
        p_ptr->chp_frac = (uint16_t) (new_chp_frac - 0x10000L);
        p_ptr->chp++;
    } else {
        p_ptr->chp_frac = (uint16_t) new_chp_frac;
    }

    // must set frac to zero even if equal
    if (p_ptr->chp >= p_ptr->mhp) {
        p_ptr->chp = p_ptr->mhp;
        p_ptr->chp_frac = 0;
    }
    if (old_chp != p_ptr->chp) {
        prt_chp();
    }
}

// Regenerate mana points -RAK-
static void regenmana(int percent) {
    struct player_type::misc *p_ptr = &py.misc;
    int old_cmana = p_ptr->cmana;
    int32_t new_mana = ((int32_t) p_ptr->mana) * percent + PLAYER_REGEN_MNBASE;

    // div 65536
    p_ptr->cmana += new_mana >> 16;

    // check for overflow
    if (p_ptr->cmana < 0 && old_cmana > 0) {
        p_ptr->cmana = MAX_SHORT;
    }

    // mod 65536
    int32_t new_mana_frac = (new_mana & 0xFFFF) + p_ptr->cmana_frac;

    if (new_mana_frac >= 0x10000L) {
        p_ptr->cmana_frac = (uint16_t) (new_mana_frac - 0x10000L);
        p_ptr->cmana++;
    } else {
        p_ptr->cmana_frac = (uint16_t) new_mana_frac;
    }

    // must set frac to zero even if equal
    if (p_ptr->cmana >= p_ptr->mana) {
        p_ptr->cmana = p_ptr->mana;
        p_ptr->cmana_frac = 0;
    }
    if (old_cmana != p_ptr->cmana) {
        prt_cmana();
    }
}

// Is an item an enchanted weapon or armor and we don't know? -CJS-
// only returns true if it is a good enchantment
static bool enchanted(inven_type *t_ptr) {
    if (t_ptr->tval < TV_MIN_ENCHANT || t_ptr->tval > TV_MAX_ENCHANT || (t_ptr->flags & TR_CURSED)) {
        return false;
    } else if (known2_p(t_ptr)) {
        return false;
    } else if (t_ptr->ident & ID_MAGIK) {
        return false;
    } else if (t_ptr->tohit > 0 || t_ptr->todam > 0 || t_ptr->toac > 0) {
        return true;
    } else if ((0x4000107fL & t_ptr->flags) && t_ptr->p1 > 0) {
        return true;
    } else if (0x07ffe980L & t_ptr->flags) {
        return true;
    }
    return false;
}

// Examine a Book -RAK-
static void examine_book() {
    int i, k, item_val;

    if (!find_range(TV_MAGIC_BOOK, TV_PRAYER_BOOK, &i, &k)) {
        msg_print("You are not carrying any books.");
    } else if (py.flags.blind > 0) {
        msg_print("You can't see to read your spell book!");
    } else if (no_light()) {
        msg_print("You have no light to read by.");
    } else if (py.flags.confused > 0) {
        msg_print("You are too confused.");
    } else if (get_item(&item_val, "Which Book?", i, k, CNIL, CNIL)) {
        int spell_index[31];
        spell_type *s_ptr;

        bool flag = true;
        inven_type *i_ptr = &inventory[item_val];

        if (classes[py.misc.pclass].spell == MAGE) {
            if (i_ptr->tval != TV_MAGIC_BOOK) {
                flag = false;
            }
        } else if (classes[py.misc.pclass].spell == PRIEST) {
            if (i_ptr->tval != TV_PRAYER_BOOK) {
                flag = false;
            }
        } else {
            flag = false;
        }

        if (!flag) {
            msg_print("You do not understand the language.");
        } else {
            i = 0;
            uint32_t j = inventory[item_val].flags;

            while (j) {
                k = bit_pos(&j);
                s_ptr = &magic_spell[py.misc.pclass - 1][k];
                if (s_ptr->slevel < 99) {
                    spell_index[i] = k;
                    i++;
                }
            }

            save_screen();
            print_spells(spell_index, i, true, -1);
            pause_line(0);
            restore_screen();
        }
    }
}

// Go up one level -RAK-
static void go_up() {
    bool no_stairs = false;
    cave_type *c_ptr = &cave[char_row][char_col];

    if (c_ptr->tptr != 0) {
        if (t_list[c_ptr->tptr].tval == TV_UP_STAIR) {
            dun_level--;
            new_level_flag = true;
            msg_print("You enter a maze of up staircases.");
            msg_print("You pass through a one-way door.");
        } else {
            no_stairs = true;
        }
    } else {
        no_stairs = true;
    }

    if (no_stairs) {
        msg_print("I see no up staircase here.");
        free_turn_flag = true;
    }
}

// Go down one level -RAK-
static void go_down() {
    bool no_stairs = false;
    cave_type *c_ptr = &cave[char_row][char_col];

    if (c_ptr->tptr != 0) {
        if (t_list[c_ptr->tptr].tval == TV_DOWN_STAIR) {
            dun_level++;
            new_level_flag = true;
            msg_print("You enter a maze of down staircases.");
            msg_print("You pass through a one-way door.");
        } else {
            no_stairs = true;
        }
    } else {
        no_stairs = true;
    }

    if (no_stairs) {
        msg_print("I see no down staircase here.");
        free_turn_flag = true;
    }
}

// Jam a closed door -RAK-
static void jamdoor() {
    free_turn_flag = true;

    int y = char_row;
    int x = char_col;

    int dir;
    if (get_dir(CNIL, &dir)) {
        (void) mmove(dir, &y, &x);
        cave_type *c_ptr = &cave[y][x];

        if (c_ptr->tptr != 0) {
            inven_type *t_ptr = &t_list[c_ptr->tptr];
            if (t_ptr->tval == TV_CLOSED_DOOR) {
                if (c_ptr->cptr == 0) {
                    int i, j;

                    if (find_range(TV_SPIKE, TV_NEVER, &i, &j)) {
                        free_turn_flag = false;
                        count_msg_print("You jam the door with a spike.");

                        if (t_ptr->p1 > 0) {
                            // Make locked to stuck.
                            t_ptr->p1 = -t_ptr->p1;
                        }

                        // Successive spikes have a progressively smaller effect.
                        // Series is: 0 20 30 37 43 48 52 56 60 64 67 70 ...
                        t_ptr->p1 -= 1 + 190 / (10 - t_ptr->p1);

                        inven_type *i_ptr = &inventory[i];
                        if (i_ptr->number > 1) {
                            i_ptr->number--;
                            inven_weight -= i_ptr->weight;
                        } else {
                            inven_destroy(i);
                        }
                    } else {
                        msg_print("But you have no spikes.");
                    }
                } else {
                    free_turn_flag = false;

                    char tmp_str[80];
                    (void) sprintf(tmp_str, "The %s is in your way!", c_list[m_list[c_ptr->cptr].mptr].name);
                    msg_print(tmp_str);
                }
            } else if (t_ptr->tval == TV_OPEN_DOOR) {
                msg_print("The door must be closed first.");
            } else {
                msg_print("That isn't a door!");
            }
        } else {
            msg_print("That isn't a door!");
        }
    }
}

// Refill the players lamp -RAK-
static void refill_lamp() {
    free_turn_flag = true;

    if (inventory[INVEN_LIGHT].subval != 0) {
        msg_print("But you are not using a lamp.");
        return;
    }

    int i, j;
    if (!find_range(TV_FLASK, TV_NEVER, &i, &j)) {
        msg_print("You have no oil.");
        return;
    }

    free_turn_flag = false;

    inven_type *i_ptr = &inventory[INVEN_LIGHT];
    i_ptr->p1 += inventory[i].p1;
    if (i_ptr->p1 > OBJ_LAMP_MAX) {
        i_ptr->p1 = OBJ_LAMP_MAX;
        msg_print("Your lamp overflows, spilling oil on the ground.");
        msg_print("Your lamp is full.");
    } else if (i_ptr->p1 > OBJ_LAMP_MAX / 2) {
        msg_print("Your lamp is more than half full.");
    } else if (i_ptr->p1 == OBJ_LAMP_MAX / 2) {
        msg_print("Your lamp is half full.");
    } else {
        msg_print("Your lamp is less than half full.");
    }
    desc_remain(i);
    inven_destroy(i);
}
