// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Handle monster movement and attacks

#include "headers.h"
#include "externs.h"

static bool checkMonsterIsVisible(Monster_t *m_ptr) {
    bool visible = false;

    Cave_t *c_ptr = &cave[m_ptr->fy][m_ptr->fx];
    Creature_t *r_ptr = &creatures_list[m_ptr->mptr];

    if (c_ptr->pl || c_ptr->tl || (running_counter && m_ptr->cdis < 2 && player_carrying_light)) {
        // Normal sight.
        if ((CM_INVISIBLE & r_ptr->cmove) == 0) {
            visible = true;
        } else if (py.flags.see_inv) {
            visible = true;
            creature_recall[m_ptr->mptr].r_cmove |= CM_INVISIBLE;
        }
    } else if (py.flags.see_infra > 0 && m_ptr->cdis <= py.flags.see_infra && (CD_INFRA & r_ptr->cdefense)) {
        // Infra vision.
        visible = true;
        creature_recall[m_ptr->mptr].r_cdefense |= CD_INFRA;
    }

    return visible;
}

// Updates screen when monsters move about -RAK-
void monsterUpdateVisibility(int monster_id) {
    bool visible = false;
    Monster_t *m_ptr = &monsters[monster_id];

    if (m_ptr->cdis <= MAX_SIGHT && !(py.flags.status & PY_BLIND) && coordInsidePanel((int) m_ptr->fy, (int) m_ptr->fx)) {
        if (wizard_mode) {
            // Wizard sight.
            visible = true;
        } else if (los(char_row, char_col, (int) m_ptr->fy, (int) m_ptr->fx)) {
            visible = checkMonsterIsVisible(m_ptr);
        }
    }

    if (visible) {
        // Light it up.
        if (!m_ptr->ml) {
            disturb(1, 0);
            m_ptr->ml = true;
            lite_spot((int) m_ptr->fy, (int) m_ptr->fx);

            // notify inven_command
            screen_has_changed = true;
        }
    } else if (m_ptr->ml) {
        // Turn it off.
        m_ptr->ml = false;
        lite_spot((int) m_ptr->fy, (int) m_ptr->fx);

        // notify inven_command
        screen_has_changed = true;
    }
}

// Given speed, returns number of moves this turn. -RAK-
// NOTE: Player must always move at least once per iteration,
// a slowed player is handled by moving monsters faster
static int movement_rate(int16_t speed) {
    if (speed > 0) {
        if (py.flags.rest != 0) {
            return 1;
        } else {
            return speed;
        }
    }

    // speed must be negative here
    return ((current_game_turn % (2 - speed)) == 0);
}

// Makes sure a new creature gets lit up. -CJS-
static bool check_mon_lite(int y, int x) {
    int monsterID = cave[y][x].cptr;
    if (monsterID <= 1) {
        return false;
    }

    monsterUpdateVisibility(monsterID);
    return monsters[monsterID].ml;
}

// Choose correct directions for monster movement -RAK-
static void get_moves(int monsterID, int *mm) {
    int ay, ax, movement;

    int y = monsters[monsterID].fy - char_row;
    int x = monsters[monsterID].fx - char_col;

    if (y < 0) {
        movement = 8;
        ay = -y;
    } else {
        movement = 0;
        ay = y;
    }
    if (x > 0) {
        movement += 4;
        ax = x;
    } else {
        ax = -x;
    }

    // this has the advantage of preventing the diamond maneuver, also faster
    if (ay > (ax << 1)) {
        movement += 2;
    } else if (ax > (ay << 1)) {
        movement++;
    }

    switch (movement) {
        case 0:
            mm[0] = 9;
            if (ay > ax) {
                mm[1] = 8;
                mm[2] = 6;
                mm[3] = 7;
                mm[4] = 3;
            } else {
                mm[1] = 6;
                mm[2] = 8;
                mm[3] = 3;
                mm[4] = 7;
            }
            break;
        case 1:
        case 9:
            mm[0] = 6;
            if (y < 0) {
                mm[1] = 3;

                mm[2] = 9;
                mm[3] = 2;
                mm[4] = 8;
            } else {
                mm[1] = 9;
                mm[2] = 3;
                mm[3] = 8;
                mm[4] = 2;
            }
            break;
        case 2:
        case 6:
            mm[0] = 8;
            if (x < 0) {
                mm[1] = 9;
                mm[2] = 7;
                mm[3] = 6;
                mm[4] = 4;
            } else {
                mm[1] = 7;
                mm[2] = 9;
                mm[3] = 4;
                mm[4] = 6;
            }
            break;
        case 4:
            mm[0] = 7;
            if (ay > ax) {
                mm[1] = 8;
                mm[2] = 4;
                mm[3] = 9;
                mm[4] = 1;
            } else {
                mm[1] = 4;
                mm[2] = 8;
                mm[3] = 1;
                mm[4] = 9;
            }
            break;
        case 5:
        case 13:
            mm[0] = 4;
            if (y < 0) {
                mm[1] = 1;
                mm[2] = 7;
                mm[3] = 2;
                mm[4] = 8;
            } else {
                mm[1] = 7;
                mm[2] = 1;
                mm[3] = 8;
                mm[4] = 2;
            }
            break;
        case 8:
            mm[0] = 3;
            if (ay > ax) {
                mm[1] = 2;
                mm[2] = 6;
                mm[3] = 1;
                mm[4] = 9;
            } else {
                mm[1] = 6;
                mm[2] = 2;
                mm[3] = 9;
                mm[4] = 1;
            }
            break;
        case 10:
        case 14:
            mm[0] = 2;
            if (x < 0) {
                mm[1] = 3;
                mm[2] = 1;
                mm[3] = 6;
                mm[4] = 4;
            } else {
                mm[1] = 1;
                mm[2] = 3;
                mm[3] = 4;
                mm[4] = 6;
            }
            break;
        case 12:
            mm[0] = 1;
            if (ay > ax) {
                mm[1] = 2;
                mm[2] = 4;
                mm[3] = 3;
                mm[4] = 7;
            } else {
                mm[1] = 4;
                mm[2] = 2;
                mm[3] = 7;
                mm[4] = 3;
            }
            break;
    }
}

// For "DIED_FROM" string
static void diedFromString(vtype_t *desc, const char *name, uint32_t move) {
    if (CM_WIN & move) {
        (void) sprintf(*desc, "The %s", name);
    } else if (isVowel(name[0])) {
        (void) sprintf(*desc, "an %s", name);
    } else {
        (void) sprintf(*desc, "a %s", name);
    }
}

static bool testAttackHits(int attackID, uint8_t level) {
    bool success = false;

    switch (attackID) {
        case 1: // Normal attack
            if (test_hit(60, (int) level, 0, py.misc.pac + py.misc.ptoac, CLA_MISC_HIT)) {
                success = true;
            }
            break;
        case 2: // Lose Strength
            if (test_hit(-3, (int) level, 0, py.misc.pac + py.misc.ptoac, CLA_MISC_HIT)) {
                success = true;
            }
            break;
        case 3: // Confusion attack
            if (test_hit(10, (int) level, 0, py.misc.pac + py.misc.ptoac, CLA_MISC_HIT)) {
                success = true;
            }
            break;
        case 4: // Fear attack
            if (test_hit(10, (int) level, 0, py.misc.pac + py.misc.ptoac, CLA_MISC_HIT)) {
                success = true;
            }
            break;
        case 5: // Fire attack
            if (test_hit(10, (int) level, 0, py.misc.pac + py.misc.ptoac, CLA_MISC_HIT)) {
                success = true;
            }
            break;
        case 6: // Acid attack
            if (test_hit(0, (int) level, 0, py.misc.pac + py.misc.ptoac, CLA_MISC_HIT)) {
                success = true;
            }
            break;
        case 7: // Cold attack
            if (test_hit(10, (int) level, 0, py.misc.pac + py.misc.ptoac, CLA_MISC_HIT)) {
                success = true;
            }
            break;
        case 8: // Lightning attack
            if (test_hit(10, (int) level, 0, py.misc.pac + py.misc.ptoac, CLA_MISC_HIT)) {
                success = true;
            }
            break;
        case 9: // Corrosion attack
            if (test_hit(0, (int) level, 0, py.misc.pac + py.misc.ptoac, CLA_MISC_HIT)) {
                success = true;
            }
            break;
        case 10: // Blindness attack
            if (test_hit(2, (int) level, 0, py.misc.pac + py.misc.ptoac, CLA_MISC_HIT)) {
                success = true;
            }
            break;
        case 11: // Paralysis attack
            if (test_hit(2, (int) level, 0, py.misc.pac + py.misc.ptoac, CLA_MISC_HIT)) {
                success = true;
            }
            break;
        case 12: // Steal Money
            if (test_hit(5, (int) level, 0, (int) py.misc.lev, CLA_MISC_HIT) && py.misc.au > 0) {
                success = true;
            }
            break;
        case 13: // Steal Object
            if (test_hit(2, (int) level, 0, (int) py.misc.lev, CLA_MISC_HIT) && inventory_count > 0) {
                success = true;
            }
            break;
        case 14: // Poison
            if (test_hit(5, (int) level, 0, py.misc.pac + py.misc.ptoac, CLA_MISC_HIT)) {
                success = true;
            }
            break;
        case 15: // Lose dexterity
            if (test_hit(0, (int) level, 0, py.misc.pac + py.misc.ptoac, CLA_MISC_HIT)) {
                success = true;
            }
            break;
        case 16: // Lose constitution
            if (test_hit(0, (int) level, 0, py.misc.pac + py.misc.ptoac, CLA_MISC_HIT)) {
                success = true;
            }
            break;
        case 17: // Lose intelligence
            if (test_hit(2, (int) level, 0, py.misc.pac + py.misc.ptoac, CLA_MISC_HIT)) {
                success = true;
            }
            break;
        case 18: // Lose wisdom
            if (test_hit(2, (int) level, 0, py.misc.pac + py.misc.ptoac, CLA_MISC_HIT)) {
                success = true;
            }
            break;
        case 19: // Lose experience
            if (test_hit(5, (int) level, 0, py.misc.pac + py.misc.ptoac, CLA_MISC_HIT)) {
                success = true;
            }
            break;
        case 20: // Aggravate monsters
            success = true;
            break;
        case 21: // Disenchant
            if (test_hit(20, (int) level, 0, py.misc.pac + py.misc.ptoac, CLA_MISC_HIT)) {
                success = true;
            }
            break;
        case 22: // Eat food
            if (test_hit(5, (int) level, 0, py.misc.pac + py.misc.ptoac, CLA_MISC_HIT)) {
                success = true;
            }
            break;
        case 23: // Eat light
            if (test_hit(5, (int) level, 0, py.misc.pac + py.misc.ptoac, CLA_MISC_HIT)) {
                success = true;
            }
            break;
        case 24: // Eat charges
            // check to make sure an object exists
            if (test_hit(15, (int) level, 0, py.misc.pac + py.misc.ptoac, CLA_MISC_HIT) && inventory_count > 0) {
                success = true;
            }
            break;
        case 99:
            success = true;
            break;
        default:
            break;
    }

    return success;
}

static void monsterPrintAttackDescription(char *msg, int attackID) {
    switch (attackID) {
        case 1:
            printMessage(strcat(msg, "hits you."));
            break;
        case 2:
            printMessage(strcat(msg, "bites you."));
            break;
        case 3:
            printMessage(strcat(msg, "claws you."));
            break;
        case 4:
            printMessage(strcat(msg, "stings you."));
            break;
        case 5:
            printMessage(strcat(msg, "touches you."));
            break;
#if 0
        case 6:
                    msg_print(strcat(msg, "kicks you."));
                    break;
#endif
        case 7:
            printMessage(strcat(msg, "gazes at you."));
            break;
        case 8:
            printMessage(strcat(msg, "breathes on you."));
            break;
        case 9:
            printMessage(strcat(msg, "spits on you."));
            break;
        case 10:
            printMessage(strcat(msg, "makes a horrible wail."));
            break;
#if 0
        case 11:
                    msg_print(strcat(msg, "embraces you."));
                    break;
#endif
        case 12:
            printMessage(strcat(msg, "crawls on you."));
            break;
        case 13:
            printMessage(strcat(msg, "releases a cloud of spores."));
            break;
        case 14:
            printMessage(strcat(msg, "begs you for money."));
            break;
        case 15:
            printMessage("You've been slimed!");
            break;
        case 16:
            printMessage(strcat(msg, "crushes you."));
            break;
        case 17:
            printMessage(strcat(msg, "tramples you."));
            break;
        case 18:
            printMessage(strcat(msg, "drools on you."));
            break;
        case 19:
            switch (randomNumber(9)) {
                case 1:
                    printMessage(strcat(msg, "insults you!"));
                    break;
                case 2:
                    printMessage(strcat(msg, "insults your mother!"));
                    break;
                case 3:
                    printMessage(strcat(msg, "gives you the finger!"));
                    break;
                case 4:
                    printMessage(strcat(msg, "humiliates you!"));
                    break;
                case 5:
                    printMessage(strcat(msg, "wets on your leg!"));
                    break;
                case 6:
                    printMessage(strcat(msg, "defiles you!"));
                    break;
                case 7:
                    printMessage(strcat(msg, "dances around you!"));
                    break;
                case 8:
                    printMessage(strcat(msg, "makes obscene gestures!"));
                    break;
                case 9:
                    printMessage(strcat(msg, "moons you!!!"));
                    break;
            }
            break;
        case 99:
            printMessage(strcat(msg, "is repelled."));
            break;
        default:
            break;
    }
}

static bool executeDisenchantAttack() {
    bool success = false;
    int itemID;

    switch (randomNumber(7)) {
        case 1:
            itemID = INVEN_WIELD;
            break;
        case 2:
            itemID = INVEN_BODY;
            break;
        case 3:
            itemID = INVEN_ARM;
            break;
        case 4:
            itemID = INVEN_OUTER;
            break;
        case 5:
            itemID = INVEN_HANDS;
            break;
        case 6:
            itemID = INVEN_HEAD;
            break;
        case 7:
            itemID = INVEN_FEET;
            break;
    }

    Inventory_t *i_ptr = &inventory[itemID];

    if (i_ptr->tohit > 0) {
        i_ptr->tohit -= randomNumber(2);

        // don't send it below zero
        if (i_ptr->tohit < 0) {
            i_ptr->tohit = 0;
        }
        success = true;
    }
    if (i_ptr->todam > 0) {
        i_ptr->todam -= randomNumber(2);

        // don't send it below zero
        if (i_ptr->todam < 0) {
            i_ptr->todam = 0;
        }
        success = true;
    }
    if (i_ptr->toac > 0) {
        i_ptr->toac -= randomNumber(2);

        // don't send it below zero
        if (i_ptr->toac < 0) {
            i_ptr->toac = 0;
        }
        success = true;
    }

    return success;
}

static bool executeAttack(Creature_t *r_ptr, Monster_t *m_ptr, int monsterID, int attype, int damage, vtype_t deathDescription, bool notice) {
    int i;
    int j;
    int32_t gold;
    Inventory_t *i_ptr;

    switch (attype) {
        case 1: // Normal attack
            // round half-way case down
            damage -= ((py.misc.pac + py.misc.ptoac) * damage) / 200;
            take_hit(damage, deathDescription);
            break;
        case 2: // Lose Strength
            take_hit(damage, deathDescription);
            if (py.flags.sustain_str) {
                printMessage("You feel weaker for a moment, but it passes.");
            } else if (randomNumber(2) == 1) {
                printMessage("You feel weaker.");
                (void) playerStatRandomDecrease(A_STR);
            } else {
                notice = false;
            }
            break;
        case 3: // Confusion attack
            take_hit(damage, deathDescription);
            if (randomNumber(2) == 1) {
                if (py.flags.confused < 1) {
                    printMessage("You feel confused.");
                    py.flags.confused += randomNumber((int) r_ptr->level);
                } else {
                    notice = false;
                }
                py.flags.confused += 3;
            } else {
                notice = false;
            }
            break;
        case 4: // Fear attack
            take_hit(damage, deathDescription);
            if (player_saves()) {
                printMessage("You resist the effects!");
            } else if (py.flags.afraid < 1) {
                printMessage("You are suddenly afraid!");
                py.flags.afraid += 3 + randomNumber((int) r_ptr->level);
            } else {
                py.flags.afraid += 3;
                notice = false;
            }
            break;
        case 5: // Fire attack
            printMessage("You are enveloped in flames!");
            fire_dam(damage, deathDescription);
            break;
        case 6: // Acid attack
            printMessage("You are covered in acid!");
            acid_dam(damage, deathDescription);
            break;
        case 7: // Cold attack
            printMessage("You are covered with frost!");
            cold_dam(damage, deathDescription);
            break;
        case 8: // Lightning attack
            printMessage("Lightning strikes you!");
            light_dam(damage, deathDescription);
            break;
        case 9: // Corrosion attack
            printMessage("A stinging red gas swirls about you.");
            corrode_gas(deathDescription);
            take_hit(damage, deathDescription);
            break;
        case 10: // Blindness attack
            take_hit(damage, deathDescription);
            if (py.flags.blind < 1) {
                py.flags.blind += 10 + randomNumber((int) r_ptr->level);
                printMessage("Your eyes begin to sting.");
            } else {
                py.flags.blind += 5;
                notice = false;
            }
            break;
        case 11: // Paralysis attack
            take_hit(damage, deathDescription);
            if (player_saves()) {
                printMessage("You resist the effects!");
            } else if (py.flags.paralysis < 1) {
                if (py.flags.free_act) {
                    printMessage("You are unaffected.");
                } else {
                    py.flags.paralysis = (int16_t) (randomNumber((int) r_ptr->level) + 3);
                    printMessage("You are paralyzed.");
                }
            } else {
                notice = false;
            }
            break;
        case 12: // Steal Money
            if (py.flags.paralysis < 1 && randomNumber(124) < py.stats.use_stat[A_DEX]) {
                printMessage("You quickly protect your money pouch!");
            } else {
                gold = (py.misc.au / 10) + randomNumber(25);
                if (gold > py.misc.au) {
                    py.misc.au = 0;
                } else {
                    py.misc.au -= gold;
                }
                printMessage("Your purse feels lighter.");
                printCharacterGoldValue();
            }
            if (randomNumber(2) == 1) {
                printMessage("There is a puff of smoke!");
                teleport_away(monsterID, MAX_SIGHT);
            }
            break;
        case 13: // Steal Object
            if (py.flags.paralysis < 1 && randomNumber(124) < py.stats.use_stat[A_DEX]) {
                printMessage("You grab hold of your backpack!");
            } else {
                inventoryDestroyItem(randomNumber(inventory_count) - 1);
                printMessage("Your backpack feels lighter.");
            }
            if (randomNumber(2) == 1) {
                printMessage("There is a puff of smoke!");
                teleport_away(monsterID, MAX_SIGHT);
            }
            break;
        case 14: // Poison
            take_hit(damage, deathDescription);
            printMessage("You feel very sick.");
            py.flags.poisoned += randomNumber((int) r_ptr->level) + 5;
            break;
        case 15: // Lose dexterity
            take_hit(damage, deathDescription);
            if (py.flags.sustain_dex) {
                printMessage("You feel clumsy for a moment, but it passes.");
            } else {
                printMessage("You feel more clumsy.");
                (void) playerStatRandomDecrease(A_DEX);
            }
            break;
        case 16: // Lose constitution
            take_hit(damage, deathDescription);
            if (py.flags.sustain_con) {
                printMessage("Your body resists the effects of the disease.");
            } else {
                printMessage("Your health is damaged!");
                (void) playerStatRandomDecrease(A_CON);
            }
            break;
        case 17: // Lose intelligence
            take_hit(damage, deathDescription);
            printMessage("You have trouble thinking clearly.");
            if (py.flags.sustain_int) {
                printMessage("But your mind quickly clears.");
            } else {
                (void) playerStatRandomDecrease(A_INT);
            }
            break;
        case 18: // Lose wisdom
            take_hit(damage, deathDescription);
            if (py.flags.sustain_wis) {
                printMessage("Your wisdom is sustained.");
            } else {
                printMessage("Your wisdom is drained.");
                (void) playerStatRandomDecrease(A_WIS);
            }
            break;
        case 19: // Lose experience
            printMessage("You feel your life draining away!");
            lose_exp(damage + (py.misc.exp / 100) * MON_DRAIN_LIFE);
            break;
        case 20: // Aggravate monster
            (void) aggravate_monster(20);
            break;
        case 21: // Disenchant
            if (executeDisenchantAttack()) {
                printMessage("There is a static feeling in the air.");
                calc_bonuses();
            } else {
                notice = false;
            }
            break;
        case 22: // Eat food
            if (find_range(TV_FOOD, TV_NEVER, &i, &j)) {
                inventoryDestroyItem(i);
                printMessage("It got at your rations!");
            } else {
                notice = false;
            }
            break;
        case 23: // Eat light
            i_ptr = &inventory[INVEN_LIGHT];
            if (i_ptr->p1 > 0) {
                i_ptr->p1 -= (250 + randomNumber(250));
                if (i_ptr->p1 < 1) {
                    i_ptr->p1 = 1;
                }
                if (py.flags.blind < 1) {
                    printMessage("Your light dims.");
                } else {
                    notice = false;
                }
            } else {
                notice = false;
            }
            break;
        case 24: // Eat charges
            i_ptr = &inventory[randomNumber(inventory_count) - 1];
            if ((i_ptr->tval == TV_STAFF || i_ptr->tval == TV_WAND) && i_ptr->p1 > 0) {
                m_ptr->hp += r_ptr->level * i_ptr->p1;
                i_ptr->p1 = 0;
                if (!spellItemIdentified(i_ptr)) {
                    add_inscribe(i_ptr, ID_EMPTY);
                }
                printMessage("Energy drains from your pack!");
            } else {
                notice = false;
            }
            break;
        case 99:
            notice = false;
            break;
        default:
            notice = false;
            break;
    }

    return notice;
}

static void confuseCreatureOnAttack(Creature_t *r_ptr, Monster_t *m_ptr, int adesc, vtype_t cdesc, bool visible) {
    if (py.flags.confuse_monster && adesc != 99) {
        printMessage("Your hands stop glowing.");
        py.flags.confuse_monster = false;

        vtype_t msg;

        if (randomNumber(MAX_MONS_LEVEL) < r_ptr->level || (CD_NO_SLEEP & r_ptr->cdefense)) {
            (void) sprintf(msg, "%sis unaffected.", cdesc);
        } else {
            (void) sprintf(msg, "%sappears confused.", cdesc);
            if (m_ptr->confused) {
                m_ptr->confused += 3;
            } else {
                m_ptr->confused = (uint8_t) (2 + randomNumber(16));
            }
        }

        printMessage(msg);

        if (visible && !character_is_dead && randomNumber(4) == 1) {
            creature_recall[m_ptr->mptr].r_cdefense |= r_ptr->cdefense & CD_NO_SLEEP;
        }
    }

}

// Make an attack on the player (chuckle.) -RAK-
static void make_attack(int monsterID) {
    // don't beat a dead body!
    if (character_is_dead) {
        return;
    }

    Monster_t *m_ptr = &monsters[monsterID];
    Creature_t *r_ptr = &creatures_list[m_ptr->mptr];

    vtype_t cdesc;
    if (!m_ptr->ml) {
        (void) strcpy(cdesc, "It ");
    } else {
        (void) sprintf(cdesc, "The %s ", r_ptr->name);
    }

    vtype_t deathDescription;
    diedFromString(&deathDescription, r_ptr->name, r_ptr->cmove);

    int attype, adesc, adice, asides;
    int attackn = 0;
    vtype_t tmp_str;

    uint8_t *attstr = r_ptr->damage;
    while ((*attstr != 0) && !character_is_dead) {
        attype = monster_attacks[*attstr].attack_type;
        adesc = monster_attacks[*attstr].attack_desc;
        adice = monster_attacks[*attstr].attack_dice;
        asides = monster_attacks[*attstr].attack_sides;
        attstr++;

        if (py.flags.protevil > 0 && (r_ptr->cdefense & CD_EVIL) && py.misc.lev + 1 > r_ptr->level) {
            if (m_ptr->ml) {
                creature_recall[m_ptr->mptr].r_cdefense |= CD_EVIL;
            }
            attype = 99;
            adesc = 99;
        }

        if (testAttackHits(attype, r_ptr->level)) {
            disturb(1, 0);

            // can not strcat to cdesc because the creature may have multiple attacks.
            (void) strcpy(tmp_str, cdesc);

            monsterPrintAttackDescription(tmp_str, adesc);

            // always fail to notice attack if creature invisible, set notice
            // and visible here since creature may be visible when attacking
            // and then teleport afterwards (becoming effectively invisible)
            bool notice = true;
            bool visible = true;
            if (!m_ptr->ml) {
                visible = false;
                notice = false;
            }

            int dam = diceDamageRoll(adice, asides);
            notice = executeAttack(r_ptr, m_ptr, monsterID, attype, dam, deathDescription, notice);

            // Moved here from mon_move, so that monster only confused if it
            // actually hits. A monster that has been repelled has not hit
            // the player, so it should not be confused.
            confuseCreatureOnAttack(r_ptr, m_ptr, adesc, cdesc, visible);

            // increase number of attacks if notice true, or if visible and
            // had previously noticed the attack (in which case all this does
            // is help player learn damage), note that in the second case do
            // not increase attacks if creature repelled (no damage done)
            if ((notice || (visible && creature_recall[m_ptr->mptr].r_attacks[attackn] != 0 && attype != 99)) && creature_recall[m_ptr->mptr].r_attacks[attackn] < MAX_UCHAR) {
                creature_recall[m_ptr->mptr].r_attacks[attackn]++;
            }
            if (character_is_dead && creature_recall[m_ptr->mptr].r_deaths < MAX_SHORT) {
                creature_recall[m_ptr->mptr].r_deaths++;
            }
        } else {
            if ((adesc >= 1 && adesc <= 3) || adesc == 6) {
                disturb(1, 0);
                (void) strcpy(tmp_str, cdesc);
                printMessage(strcat(tmp_str, "misses you."));
            }
        }

        if (attackn < MAX_MON_NATTACK - 1) {
            attackn++;
        } else {
            break;
        }
    }
}

static void creatureOpensDoor(Cave_t *c_ptr, int16_t monsterHP, uint32_t movebits, bool *do_turn, bool *do_move, uint32_t *rcmove, int y, int x) {
    Inventory_t *t_ptr = &treasure_list[c_ptr->tptr];

    // Creature can open doors.
    if (movebits & CM_OPEN_DOOR) {
        bool doorStuck = false;

        if (t_ptr->tval == TV_CLOSED_DOOR) {
            *do_turn = true;

            if (t_ptr->p1 == 0) {
                // Closed doors

                *do_move = true;
            } else if (t_ptr->p1 > 0) {
                // Locked doors

                if (randomNumber((monsterHP + 1) * (50 + t_ptr->p1)) < 40 * (monsterHP - 10 - t_ptr->p1)) {
                    t_ptr->p1 = 0;
                }
            } else if (t_ptr->p1 < 0) {
                // Stuck doors

                if (randomNumber((monsterHP + 1) * (50 - t_ptr->p1)) < 40 * (monsterHP - 10 + t_ptr->p1)) {
                    printMessage("You hear a door burst open!");
                    disturb(1, 0);
                    doorStuck = true;
                    *do_move = true;
                }
            }
        } else if (t_ptr->tval == TV_SECRET_DOOR) {
            *do_turn = true;
            *do_move = true;
        }

        if (*do_move) {
            inventoryItemCopyTo(OBJ_OPEN_DOOR, t_ptr);

            // 50% chance of breaking door
            if (doorStuck) {
                t_ptr->p1 = (int16_t) (1 - randomNumber(2));
            }
            c_ptr->fval = CORR_FLOOR;
            lite_spot(y, x);
            *rcmove |= CM_OPEN_DOOR;
            *do_move = false;
        }
    } else if (t_ptr->tval == TV_CLOSED_DOOR) {
        // Creature can not open doors, must bash them
        *do_turn = true;

        if (randomNumber((monsterHP + 1) * (80 + abs(t_ptr->p1))) < 40 * (monsterHP - 20 - abs(t_ptr->p1))) {
            inventoryItemCopyTo(OBJ_OPEN_DOOR, t_ptr);

            // 50% chance of breaking door
            t_ptr->p1 = (int16_t) (1 - randomNumber(2));
            c_ptr->fval = CORR_FLOOR;
            lite_spot(y, x);
            printMessage("You hear a door burst open!");
            disturb(1, 0);
        }
    }
}

static void glyphOfWardingProtection(uint16_t creatureID, uint32_t movebits, bool *do_move, bool *do_turn, int y, int x) {
    if (randomNumber(OBJ_RUNE_PROT) < creatures_list[creatureID].level) {
        if (y == char_row && x == char_col) {
            printMessage("The rune of protection is broken!");
        }
        (void) delete_object(y, x);
        return;
    }

    *do_move = false;

    // If the creature moves only to attack,
    // don't let it move if the glyph prevents
    // it from attacking
    if (movebits & CM_ATTACK_ONLY) {
        *do_turn = true;
    }
}

static void creatureMovesOnPlayer(Monster_t *m_ptr, uint8_t creatureID, int monsterID, uint32_t movebits, bool *do_move, bool *do_turn, uint32_t *rcmove, int y, int x) {
    if (creatureID == 1) {
        // if the monster is not lit, must call monsterUpdateVisibility, it
        // may be faster than character, and hence could have
        // just moved next to character this same turn.
        if (!m_ptr->ml) {
            monsterUpdateVisibility(monsterID);
        }
        make_attack(monsterID);
        *do_move = false;
        *do_turn = true;
    } else if (creatureID > 1 && (y != m_ptr->fy || x != m_ptr->fx)) {
        // Creature is attempting to move on other creature?

        // Creature eats other creatures?
        if ((movebits & CM_EATS_OTHER) && creatures_list[m_ptr->mptr].mexp >= creatures_list[monsters[creatureID].mptr].mexp) {
            if (monsters[creatureID].ml) {
                *rcmove |= CM_EATS_OTHER;
            }

            // It ate an already processed monster. Handle normally.
            if (monsterID < creatureID) {
                delete_monster((int) creatureID);
            } else {
                // If it eats this monster, an already processed
                // monster will take its place, causing all kinds
                // of havoc. Delay the kill a bit.
                fix1_delete_monster((int) creatureID);
            }
        } else {
            *do_move = false;
        }
    }
}

static void creatureAllowedToMove(Monster_t *m_ptr, uint32_t movebits, bool *do_turn, uint32_t *rcmove, int y, int x) {
    // Pick up or eat an object
    if (movebits & CM_PICKS_UP) {
        uint8_t treasureID = cave[y][x].tptr;
        if (treasureID != 0 && treasure_list[treasureID].tval <= TV_MAX_OBJECT) {
            *rcmove |= CM_PICKS_UP;
            (void) delete_object(y, x);
        }
    }

    // Move creature record
    move_rec((int) m_ptr->fy, (int) m_ptr->fx, y, x);

    if (m_ptr->ml) {
        m_ptr->ml = false;
        lite_spot((int) m_ptr->fy, (int) m_ptr->fx);
    }

    m_ptr->fy = (uint8_t) y;
    m_ptr->fx = (uint8_t) x;
    m_ptr->cdis = (uint8_t) coordDistanceBetween(char_row, char_col, y, x);

    *do_turn = true;
}

// Make the move if possible, five choices -RAK-
static void make_move(int monsterID, int *mm, uint32_t *rcmove) {
    bool do_turn = false;
    bool do_move = false;

    Monster_t *m_ptr = &monsters[monsterID];
    uint32_t movebits = creatures_list[m_ptr->mptr].cmove;

    // Up to 5 attempts at moving, give up.
    for (int i = 0; !do_turn && i < 5; i++) {
        // Get new position
        int y = m_ptr->fy;
        int x = m_ptr->fx;
        (void) mmove(mm[i], &y, &x);

        Cave_t *c_ptr = &cave[y][x];

        if (c_ptr->fval == BOUNDARY_WALL) {
            continue;
        }

        // Floor is open?
        if (c_ptr->fval <= MAX_OPEN_SPACE) {
            do_move = true;
        } else if (movebits & CM_PHASE) {
            // Creature moves through walls?
            do_move = true;
            *rcmove |= CM_PHASE;
        } else if (c_ptr->tptr != 0) {
            // Creature can open doors?
            creatureOpensDoor(c_ptr, m_ptr->hp, movebits, &do_turn, &do_move, rcmove, y, x);
        }

        // Glyph of warding present?
        if (do_move && c_ptr->tptr != 0 && treasure_list[c_ptr->tptr].tval == TV_VIS_TRAP && treasure_list[c_ptr->tptr].subval == 99) {
            glyphOfWardingProtection(m_ptr->mptr, movebits, &do_move, &do_turn, y, x);
        }

        // Creature has attempted to move on player?
        if (do_move) {
            creatureMovesOnPlayer(m_ptr, c_ptr->cptr, monsterID, movebits, &do_move, &do_turn, rcmove, y, x);
        }

        // Creature has been allowed move.
        if (do_move) {
            creatureAllowedToMove(m_ptr, movebits, &do_turn, rcmove, y, x);
        }
    }
}

static bool canCreatureCastSpells(Monster_t *m_ptr, uint32_t spells) {
    // 1 in x chance of casting spell
    if (randomNumber((int) (spells & CS_FREQ)) != 1) {
        return false;
    }

    // Must be within certain range
    if (m_ptr->cdis > MAX_SPELL_DIS) {
        return false;
    }

    // Must have unobstructed Line-Of-Sight
    return los(char_row, char_col, (int) m_ptr->fy, (int) m_ptr->fx);
}

void creatureCastsSpell(Monster_t *m_ptr, int monsterID, int spellID, uint8_t level, vtype_t cdesc, vtype_t deathDescription) {
    int y, x;

    // Cast the spell.
    switch (spellID) {
        case 5: // Teleport Short
            teleport_away(monsterID, 5);
            break;
        case 6: // Teleport Long
            teleport_away(monsterID, MAX_SIGHT);
            break;
        case 7: // Teleport To
            teleport_to((int) m_ptr->fy, (int) m_ptr->fx);
            break;
        case 8: // Light Wound
            if (player_saves()) {
                printMessage("You resist the effects of the spell.");
            } else {
                take_hit(diceDamageRoll(3, 8), deathDescription);
            }
            break;
        case 9: // Serious Wound
            if (player_saves()) {
                printMessage("You resist the effects of the spell.");
            } else {
                take_hit(diceDamageRoll(8, 8), deathDescription);
            }
            break;
        case 10: // Hold Person
            if (py.flags.free_act) {
                printMessage("You are unaffected.");
            } else if (player_saves()) {
                printMessage("You resist the effects of the spell.");
            } else if (py.flags.paralysis > 0) {
                py.flags.paralysis += 2;
            } else {
                py.flags.paralysis = (int16_t) (randomNumber(5) + 4);
            }
            break;
        case 11: // Cause Blindness
            if (player_saves()) {
                printMessage("You resist the effects of the spell.");
            } else if (py.flags.blind > 0) {
                py.flags.blind += 6;
            } else {
                py.flags.blind += 12 + randomNumber(3);
            }
            break;
        case 12: // Cause Confuse
            if (player_saves()) {
                printMessage("You resist the effects of the spell.");
            } else if (py.flags.confused > 0) {
                py.flags.confused += 2;
            } else {
                py.flags.confused = (int16_t) (randomNumber(5) + 3);
            }
            break;
        case 13: // Cause Fear
            if (player_saves()) {
                printMessage("You resist the effects of the spell.");
            } else if (py.flags.afraid > 0) {
                py.flags.afraid += 2;
            } else {
                py.flags.afraid = (int16_t) (randomNumber(5) + 3);
            }
            break;
        case 14: // Summon Monster
            (void) strcat(cdesc, "magically summons a monster!");
            printMessage(cdesc);
            y = char_row;
            x = char_col;

            // in case compact_monster() is called,it needs monsterID
            hack_monptr = monsterID;
            (void) monsterSummon(&y, &x, false);
            hack_monptr = -1;
            monsterUpdateVisibility((int) cave[y][x].cptr);
            break;
        case 15: // Summon Undead
            (void) strcat(cdesc, "magically summons an undead!");
            printMessage(cdesc);
            y = char_row;
            x = char_col;

            // in case compact_monster() is called,it needs monsterID
            hack_monptr = monsterID;
            (void) monsterSummonUndead(&y, &x);
            hack_monptr = -1;
            monsterUpdateVisibility((int) cave[y][x].cptr);
            break;
        case 16: // Slow Person
            if (py.flags.free_act) {
                printMessage("You are unaffected.");
            } else if (player_saves()) {
                printMessage("You resist the effects of the spell.");
            } else if (py.flags.slow > 0) {
                py.flags.slow += 2;
            } else {
                py.flags.slow = (int16_t) (randomNumber(5) + 3);
            }
            break;
        case 17: // Drain Mana
            if (py.misc.cmana > 0) {
                disturb(1, 0);

                vtype_t outval;
                (void) sprintf(outval, "%sdraws psychic energy from you!", cdesc);
                printMessage(outval);

                if (m_ptr->ml) {
                    (void) sprintf(outval, "%sappears healthier.", cdesc);
                    printMessage(outval);
                }

                int r1 = (randomNumber((int) level) >> 1) + 1;
                if (r1 > py.misc.cmana) {
                    r1 = py.misc.cmana;
                    py.misc.cmana = 0;
                    py.misc.cmana_frac = 0;
                } else {
                    py.misc.cmana -= r1;
                }
                printCharacterCurrentMana();
                m_ptr->hp += 6 * (r1);
            }
            break;
        case 20: // Breath Light
            (void) strcat(cdesc, "breathes lightning.");
            printMessage(cdesc);
            breath(char_col, char_row, monsterID, (m_ptr->hp / 4), deathDescription, GF_LIGHTNING);
            break;
        case 21: // Breath Gas
            (void) strcat(cdesc, "breathes gas.");
            printMessage(cdesc);
            breath(char_col, char_row, monsterID, (m_ptr->hp / 3), deathDescription, GF_POISON_GAS);
            break;
        case 22: // Breath Acid
            (void) strcat(cdesc, "breathes acid.");
            printMessage(cdesc);
            breath(char_col, char_row, monsterID, (m_ptr->hp / 3), deathDescription, GF_ACID);
            break;
        case 23: // Breath Frost
            (void) strcat(cdesc, "breathes frost.");
            printMessage(cdesc);
            breath(char_col, char_row, monsterID, (m_ptr->hp / 3), deathDescription, GF_FROST);
            break;
        case 24: // Breath Fire
            (void) strcat(cdesc, "breathes fire.");
            printMessage(cdesc);
            breath(char_col, char_row, monsterID, (m_ptr->hp / 3), deathDescription, GF_FIRE);
            break;
        default:
            (void) strcat(cdesc, "cast unknown spell.");
            printMessage(cdesc);
    }
}

// Creatures can cast spells too.  (Dragon Breath) -RAK-
//   cast_spell = true if creature changes position
//   return true (took_turn) if creature casts a spell
static bool mon_cast_spell(int monsterID) {
    if (character_is_dead) {
        return false;
    }

    Monster_t *m_ptr = &monsters[monsterID];
    Creature_t *r_ptr = &creatures_list[m_ptr->mptr];

    if (!canCreatureCastSpells(m_ptr, r_ptr->spells)) {
        return false;
    }

    // Creature is going to cast a spell

    // Check to see if monster should be lit.
    monsterUpdateVisibility(monsterID);

    // Describe the attack
    vtype_t cdesc;
    if (m_ptr->ml) {
        (void) sprintf(cdesc, "The %s ", r_ptr->name);
    } else {
        (void) strcpy(cdesc, "It ");
    }

    vtype_t deathDescription;
    diedFromString(&deathDescription, r_ptr->name, r_ptr->cmove);

    // Extract all possible spells into spell_choice
    int spell_choice[30];
    uint32_t spellFlags = (uint32_t) (r_ptr->spells & ~CS_FREQ);

    int id = 0;
    while (spellFlags != 0) {
        spell_choice[id] = getAndClearFirstBit(&spellFlags);
        id++;
    }

    // Choose a spell to cast
    int thrown_spell = spell_choice[randomNumber(id) - 1];
    thrown_spell++;

    // all except teleport_away() and drain mana spells always disturb
    if (thrown_spell > 6 && thrown_spell != 17) {
        disturb(1, 0);
    }

    // save some code/data space here, with a small time penalty
    if ((thrown_spell < 14 && thrown_spell > 6) || thrown_spell == 16) {
        (void) strcat(cdesc, "casts a spell.");
        printMessage(cdesc);
    }

    creatureCastsSpell(m_ptr, monsterID, thrown_spell, r_ptr->level, cdesc, deathDescription);

    if (m_ptr->ml) {
        creature_recall[m_ptr->mptr].r_spells |= 1L << (thrown_spell - 1);
        if ((creature_recall[m_ptr->mptr].r_spells & CS_FREQ) != CS_FREQ) {
            creature_recall[m_ptr->mptr].r_spells++;
        }
        if (character_is_dead && creature_recall[m_ptr->mptr].r_deaths < MAX_SHORT) {
            creature_recall[m_ptr->mptr].r_deaths++;
        }
    }

    return true;
}

// Places creature adjacent to given location -RAK-
// Rats and Flys are fun!
bool monsterMultiply(int y, int x, int creatureID, int monsterID) {
    for (int i = 0; i <= 18; i++) {
        int row = y - 2 + randomNumber(3);
        int col = x - 2 + randomNumber(3);

        // don't create a new creature on top of the old one, that
        // causes invincible/invisible creatures to appear.
        if (coordInBounds(row, col) && (row != y || col != x)) {
            Cave_t *c_ptr = &cave[row][col];

            if (c_ptr->fval <= MAX_OPEN_SPACE && c_ptr->tptr == 0 && c_ptr->cptr != 1) {
                // Creature there already?
                if (c_ptr->cptr > 1) {
                    // Some critters are cannibalistic!
                    if ((creatures_list[creatureID].cmove & CM_EATS_OTHER)
                        // Check the experience level -CJS-
                        && creatures_list[creatureID].mexp >= creatures_list[monsters[c_ptr->cptr].mptr].mexp) {
                        // It ate an already processed monster.Handle * normally.
                        if (monsterID < c_ptr->cptr) {
                            delete_monster((int) c_ptr->cptr);
                        } else {
                            // If it eats this monster, an already processed
                            // monster will take its place, causing all kinds
                            // of havoc. Delay the kill a bit.
                            fix1_delete_monster((int) c_ptr->cptr);
                        }

                        // in case compact_monster() is called, it needs monsterID.
                        hack_monptr = monsterID;
                        // Place_monster() may fail if monster list full.
                        int result = monsterPlaceNew(row, col, creatureID, false);
                        hack_monptr = -1;

                        if (!result) {
                            return false;
                        }

                        monster_multiply_total++;
                        return check_mon_lite(row, col);
                    }
                } else {
                    // All clear,  place a monster

                    // in case compact_monster() is called,it needs monsterID
                    hack_monptr = monsterID;
                    // Place_monster() may fail if monster list full.
                    int result = monsterPlaceNew(row, col, creatureID, false);
                    hack_monptr = -1;

                    if (!result) {
                        return false;
                    }

                    monster_multiply_total++;
                    return check_mon_lite(row, col);
                }
            }
        }
    }

    return false;
}

static void multiplyCritter(Monster_t *m_ptr, int monsterID, uint32_t *rcmove) {
    int counter = 0;

    for (int y = m_ptr->fy - 1; y <= m_ptr->fy + 1; y++) {
        for (int x = m_ptr->fx - 1; x <= m_ptr->fx + 1; x++) {
            if (coordInBounds(y, x) && (cave[y][x].cptr > 1)) {
                counter++;
            }
        }
    }

    // can't call randomNumber with a value of zero, increment
    // counter to allow creature multiplication.
    if (counter == 0) {
        counter++;
    }

    if (counter < 4 && randomNumber(counter * MON_MULT_ADJ) == 1) {
        if (monsterMultiply((int) m_ptr->fy, (int) m_ptr->fx, (int) m_ptr->mptr, monsterID)) {
            *rcmove |= CM_MULTIPLY;
        }
    }
}

static void creatureMoveOutOfWall(Monster_t *m_ptr, int monsterID, uint32_t *rcmove) {
    // If the monster is already dead, don't kill it again!
    // This can happen for monsters moving faster than the player. They
    // will get multiple moves, but should not if they die on the first
    // move.  This is only a problem for monsters stuck in rock.
    if (m_ptr->hp < 0) {
        return;
    }

    int id = 0;
    int dir = 1;
    int mm[9];

    // Note direction of for loops matches direction of keypad from 1 to 9
    // Do not allow attack against the player.
    // Must cast fy-1 to signed int, so that a negative value
    // of i will fail the comparison.
    for (int y = m_ptr->fy + 1; y >= (m_ptr->fy - 1); y--) {
        for (int x = m_ptr->fx - 1; x <= m_ptr->fx + 1; x++) {
            if (dir != 5 && cave[y][x].fval <= MAX_OPEN_SPACE && cave[y][x].cptr != 1) {
                mm[id++] = dir;
            }
            dir++;
        }
    }

    if (id != 0) {
        // put a random direction first
        dir = randomNumber(id) - 1;
        int savedID = mm[0];
        mm[0] = mm[dir];
        mm[dir] = savedID;
        make_move(monsterID, mm, rcmove);
        // this can only fail if mm[0] has a rune of protection
    }

    // if still in a wall, let it dig itself out, but also apply some more damage
    if (cave[m_ptr->fy][m_ptr->fx].fval >= MIN_CAVE_WALL) {
        // in case the monster dies, may need to callfix1_delete_monster()
        // instead of delete_monsters()
        hack_monptr = monsterID;
        int i = mon_take_hit(monsterID, diceDamageRoll(8, 8));
        hack_monptr = -1;

        if (i >= 0) {
            printMessage("You hear a scream muffled by rock!");
            prt_experience();
        } else {
            printMessage("A creature digs itself out from the rock!");
            (void) twall((int) m_ptr->fy, (int) m_ptr->fx, 1, 0);
        }
    }
}

static void creatureMoveConfusedUndead(Monster_t *m_ptr, Creature_t *r_ptr, int monsterID, uint32_t *rcmove) {
    int mm[9];

    // Undead only get confused from turn undead, so they should flee
    if (r_ptr->cdefense & CD_UNDEAD) {
        get_moves(monsterID, mm);
        mm[0] = 10 - mm[0];
        mm[1] = 10 - mm[1];
        mm[2] = 10 - mm[2];
        mm[3] = randomNumber(9); // May attack only if cornered
        mm[4] = randomNumber(9);
    } else {
        mm[0] = randomNumber(9);
        mm[1] = randomNumber(9);
        mm[2] = randomNumber(9);
        mm[3] = randomNumber(9);
        mm[4] = randomNumber(9);
    }

    // don't move him if he is not supposed to move!
    if (!(r_ptr->cmove & CM_ATTACK_ONLY)) {
        make_move(monsterID, mm, rcmove);
    }

    m_ptr->confused--;
}

// Move the critters about the dungeon -RAK-
static void mon_move(int monsterID, uint32_t *rcmove) {
    Monster_t *m_ptr = &monsters[monsterID];
    Creature_t *r_ptr = &creatures_list[m_ptr->mptr];

    // Does the critter multiply?
    // rest could be negative, to be safe, only use mod with positive values.
    int restPeriod = abs(py.flags.rest);
    if ((r_ptr->cmove & CM_MULTIPLY) && MAX_MON_MULT >= monster_multiply_total && (restPeriod % MON_MULT_ADJ) == 0) {
        multiplyCritter(m_ptr, monsterID, rcmove);
    }

    // if in wall, must immediately escape to a clear area
    // then monster movement finished
    if (!(r_ptr->cmove & CM_PHASE) && cave[m_ptr->fy][m_ptr->fx].fval >= MIN_CAVE_WALL) {
        creatureMoveOutOfWall(m_ptr, monsterID, rcmove);
        return;
    }

    bool doMove = false;

    if (m_ptr->confused) {
        // Creature is confused or undead turned?
        creatureMoveConfusedUndead(m_ptr, r_ptr, monsterID, rcmove);
        doMove = true;
    } else if (r_ptr->spells & CS_FREQ) {
        // Creature may cast a spell
        doMove = mon_cast_spell(monsterID);
    }

    int mm[9];
    if (!doMove) {
        if ((r_ptr->cmove & CM_75_RANDOM) && randomNumber(100) < 75) {
            // 75% random movement
            mm[0] = randomNumber(9);
            mm[1] = randomNumber(9);
            mm[2] = randomNumber(9);
            mm[3] = randomNumber(9);
            mm[4] = randomNumber(9);
            *rcmove |= CM_75_RANDOM;
            make_move(monsterID, mm, rcmove);
        } else if ((r_ptr->cmove & CM_40_RANDOM) && randomNumber(100) < 40) {
            // 40% random movement
            mm[0] = randomNumber(9);
            mm[1] = randomNumber(9);
            mm[2] = randomNumber(9);
            mm[3] = randomNumber(9);
            mm[4] = randomNumber(9);
            *rcmove |= CM_40_RANDOM;
            make_move(monsterID, mm, rcmove);
        } else if ((r_ptr->cmove & CM_20_RANDOM) && randomNumber(100) < 20) {
            // 20% random movement
            mm[0] = randomNumber(9);
            mm[1] = randomNumber(9);
            mm[2] = randomNumber(9);
            mm[3] = randomNumber(9);
            mm[4] = randomNumber(9);
            *rcmove |= CM_20_RANDOM;
            make_move(monsterID, mm, rcmove);
        } else if (r_ptr->cmove & CM_MOVE_NORMAL) {
            // Normal movement
            if (randomNumber(200) == 1) {
                mm[0] = randomNumber(9);
                mm[1] = randomNumber(9);
                mm[2] = randomNumber(9);
                mm[3] = randomNumber(9);
                mm[4] = randomNumber(9);
            } else {
                get_moves(monsterID, mm);
            }
            *rcmove |= CM_MOVE_NORMAL;
            make_move(monsterID, mm, rcmove);
        } else if (r_ptr->cmove & CM_ATTACK_ONLY) {
            // Attack, but don't move
            if (m_ptr->cdis < 2) {
                get_moves(monsterID, mm);
                make_move(monsterID, mm, rcmove);
            } else {
                // Learn that the monster does does not move when
                // it should have moved, but didn't.
                *rcmove |= CM_ATTACK_ONLY;
            }
        } else if ((r_ptr->cmove & CM_ONLY_MAGIC) && m_ptr->cdis < 2) {
            // A little hack for Quylthulgs, so that one will eventually
            // notice that they have no physical attacks.
            if (creature_recall[m_ptr->mptr].r_attacks[0] < MAX_UCHAR) {
                creature_recall[m_ptr->mptr].r_attacks[0]++;
            }

            // Another little hack for Quylthulgs, so that one can
            // eventually learn their speed.
            if (creature_recall[m_ptr->mptr].r_attacks[0] > 20) {
                creature_recall[m_ptr->mptr].r_cmove |= CM_ONLY_MAGIC;
            }
        }
    }
}

static void updateRecall(Monster_t *m_ptr, bool wake, bool ignore, int rcmove) {
    if (!m_ptr->ml) {
        return;
    }

    Recall_t *r_ptr = &creature_recall[m_ptr->mptr];

    if (wake) {
        if (r_ptr->r_wake < MAX_UCHAR) {
            r_ptr->r_wake++;
        }
    } else if (ignore) {
        if (r_ptr->r_ignore < MAX_UCHAR) {
            r_ptr->r_ignore++;
        }
    }

    r_ptr->r_cmove |= rcmove;
}

static void creatureAttackingUpdate(Monster_t *m_ptr, int monsterID, int moves) {
    for (int i = moves; i > 0; i--) {
        bool wake = false;
        bool ignore = false;

        uint32_t rcmove = 0;

        // Monsters trapped in rock must be given a turn also,
        // so that they will die/dig out immediately.
        if (m_ptr->ml || m_ptr->cdis <= creatures_list[m_ptr->mptr].aaf || ((!(creatures_list[m_ptr->mptr].cmove & CM_PHASE)) && cave[m_ptr->fy][m_ptr->fx].fval >= MIN_CAVE_WALL)) {
            if (m_ptr->csleep > 0) {
                if (py.flags.aggravate) {
                    m_ptr->csleep = 0;
                } else if ((py.flags.rest == 0 && py.flags.paralysis < 1) || (randomNumber(50) == 1)) {
                    int notice = randomNumber(1024);

                    if (notice * notice * notice <= (1L << (29 - py.misc.stl))) {
                        m_ptr->csleep -= (100 / m_ptr->cdis);
                        if (m_ptr->csleep > 0) {
                            ignore = true;
                        } else {
                            wake = true;

                            // force it to be exactly zero
                            m_ptr->csleep = 0;
                        }
                    }
                }
            }

            if (m_ptr->stunned != 0) {
                // NOTE: Balrog = 100*100 = 10000, it always recovers instantly
                if (randomNumber(5000) < creatures_list[m_ptr->mptr].level * creatures_list[m_ptr->mptr].level) {
                    m_ptr->stunned = 0;
                } else {
                    m_ptr->stunned--;
                }

                if (m_ptr->stunned == 0) {
                    if (m_ptr->ml) {
                        vtype_t msg;
                        (void) sprintf(msg, "The %s ", creatures_list[m_ptr->mptr].name);
                        printMessage(strcat(msg, "recovers and glares at you."));
                    }
                }
            }
            if ((m_ptr->csleep == 0) && (m_ptr->stunned == 0)) {
                mon_move(monsterID, &rcmove);
            }
        }

        monsterUpdateVisibility(monsterID);

        updateRecall(m_ptr, wake, ignore, rcmove);
    }
}

// Creatures movement and attacking are done from here -RAK-
void updateMonsters(bool attack) {
    // Process the monsters
    for (int id = next_free_monster_id - 1; id >= MIN_MONIX && !character_is_dead; id--) {
        Monster_t *m_ptr = &monsters[id];

        // Get rid of an eaten/breathed on monster.  Note: Be sure not to
        // process this monster. This is necessary because we can't delete
        // monsters while scanning the monsters here.
        if (m_ptr->hp < 0) {
            fix2_delete_monster(id);
            continue;
        }

        m_ptr->cdis = (uint8_t) coordDistanceBetween(char_row, char_col, (int) m_ptr->fy, (int) m_ptr->fx);

        // Attack is argument passed to CREATURE
        if (attack) {
            int moves = movement_rate(m_ptr->cspeed);

            if (moves <= 0) {
                monsterUpdateVisibility(id);
            } else {
                creatureAttackingUpdate(m_ptr, id, moves);
            }
        } else {
            monsterUpdateVisibility(id);
        }

        // Get rid of an eaten/breathed on monster. This is necessary because
        // we can't delete monsters while scanning the monsters here.
        // This monster may have been killed during mon_move().
        if (m_ptr->hp < 0) {
            fix2_delete_monster(id);
            continue;
        }
    }
}
