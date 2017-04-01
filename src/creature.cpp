// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Handle monster movement and attacks

#include "headers.h"
#include "externs.h"

// Updates screen when monsters move about -RAK-
void update_mon(int monptr) {
    cave_type *c_ptr;
    creature_type *r_ptr;

    bool flag = false;
    monster_type *m_ptr = &m_list[monptr];

    if ((m_ptr->cdis <= MAX_SIGHT) && !(py.flags.status & PY_BLIND) && (panel_contains((int) m_ptr->fy, (int) m_ptr->fx))) {
        if (wizard) {
            // Wizard sight.
            flag = true;
        } else if (los(char_row, char_col, (int) m_ptr->fy, (int) m_ptr->fx)) {
            // Normal sight.
            c_ptr = &cave[m_ptr->fy][m_ptr->fx];
            r_ptr = &c_list[m_ptr->mptr];
            if (c_ptr->pl || c_ptr->tl || (find_flag && m_ptr->cdis < 2 && player_light)) {
                if ((CM_INVISIBLE & r_ptr->cmove) == 0) {
                    flag = true;
                } else if (py.flags.see_inv) {
                    flag = true;
                    c_recall[m_ptr->mptr].r_cmove |= CM_INVISIBLE;
                }
            } else if ((py.flags.see_infra > 0) && (m_ptr->cdis <= py.flags.see_infra) && (CD_INFRA & r_ptr->cdefense)) {
                // Infra vision.

                flag = true;
                c_recall[m_ptr->mptr].r_cdefense |= CD_INFRA;
            }
        }
    }

    if (flag) {
        // Light it up.

        if (!m_ptr->ml) {
            disturb(1, 0);
            m_ptr->ml = true;
            lite_spot((int) m_ptr->fy, (int) m_ptr->fx);

            // notify inven_command
            screen_change = true;
        }
    } else if (m_ptr->ml) {
        // Turn it off.

        m_ptr->ml = false;
        lite_spot((int) m_ptr->fy, (int) m_ptr->fx);

        // notify inven_command
        screen_change = true;
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
    return ((turn % (2 - speed)) == 0);
}

// Makes sure a new creature gets lit up. -CJS-
static bool check_mon_lite(int y, int x) {
    int monptr = cave[y][x].cptr;

    if (monptr <= 1) {
        return false;
    }

    update_mon(monptr);
    return m_list[monptr].ml;
}

// Choose correct directions for monster movement -RAK-
static void get_moves(int monptr, int *mm) {
    int ay, ax, move_val;

    int y = m_list[monptr].fy - char_row;
    int x = m_list[monptr].fx - char_col;

    if (y < 0) {
        move_val = 8;
        ay = -y;
    } else {
        move_val = 0;
        ay = y;
    }
    if (x > 0) {
        move_val += 4;
        ax = x;
    } else {
        ax = -x;
    }

    // this has the advantage of preventing the diamond maneuver, also faster
    if (ay > (ax << 1)) {
        move_val += 2;
    } else if (ax > (ay << 1)) {
        move_val++;
    }

    switch (move_val) {
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

// Make an attack on the player (chuckle.) -RAK-
static void make_attack(int monptr) {
    // don't beat a dead body!
    if (death) {
        return;
    }

    monster_type *m_ptr = &m_list[monptr];
    creature_type *r_ptr = &c_list[m_ptr->mptr];

    vtype cdesc;
    if (!m_ptr->ml) {
        (void) strcpy(cdesc, "It ");
    } else {
        (void) sprintf(cdesc, "The %s ", r_ptr->name);
    }

    // For "DIED_FROM" string
    vtype ddesc;
    if (CM_WIN & r_ptr->cmove) {
        (void) sprintf(ddesc, "The %s", r_ptr->name);
    } else if (is_a_vowel(r_ptr->name[0])) {
        (void) sprintf(ddesc, "an %s", r_ptr->name);
    } else {
        (void) sprintf(ddesc, "a %s", r_ptr->name);
    }
    // End DIED_FROM

    int i = 0;
    int j, damage;
    int attype, adesc, adice, asides;
    int32_t gold;
    struct player_type::misc *p_ptr;
    struct player_type::flags *f_ptr;
    inven_type *i_ptr;
    vtype tmp_str;

    bool flag = false;
    int attackn = 0;
    uint8_t *attstr = r_ptr->damage;

    while ((*attstr != 0) && !death) {
        attype = monster_attacks[*attstr].attack_type;
        adesc = monster_attacks[*attstr].attack_desc;
        adice = monster_attacks[*attstr].attack_dice;
        asides = monster_attacks[*attstr].attack_sides;
        attstr++;
        flag = false;
        if ((py.flags.protevil > 0) && (r_ptr->cdefense & CD_EVIL) && ((py.misc.lev + 1) > r_ptr->level)) {
            if (m_ptr->ml) {
                c_recall[m_ptr->mptr].r_cdefense |= CD_EVIL;
            }
            attype = 99;
            adesc = 99;
        }

        p_ptr = &py.misc;

        switch (attype) {
            case 1: // Normal attack
                if (test_hit(60, (int) r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac, CLA_MISC_HIT)) {
                    flag = true;
                }
                break;
            case 2: // Lose Strength
                if (test_hit(-3, (int) r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac, CLA_MISC_HIT)) {
                    flag = true;
                }
                break;
            case 3: // Confusion attack
                if (test_hit(10, (int) r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac, CLA_MISC_HIT)) {
                    flag = true;
                }
                break;
            case 4: // Fear attack
                if (test_hit(10, (int) r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac, CLA_MISC_HIT)) {
                    flag = true;
                }
                break;
            case 5: // Fire attack
                if (test_hit(10, (int) r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac, CLA_MISC_HIT)) {
                    flag = true;
                }
                break;
            case 6: // Acid attack
                if (test_hit(0, (int) r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac, CLA_MISC_HIT)) {
                    flag = true;
                }
                break;
            case 7: // Cold attack
                if (test_hit(10, (int) r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac, CLA_MISC_HIT)) {
                    flag = true;
                }
                break;
            case 8: // Lightning attack
                if (test_hit(10, (int) r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac, CLA_MISC_HIT)) {
                    flag = true;
                }
                break;
            case 9: // Corrosion attack
                if (test_hit(0, (int) r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac, CLA_MISC_HIT)) {
                    flag = true;
                }
                break;
            case 10: // Blindness attack
                if (test_hit(2, (int) r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac, CLA_MISC_HIT)) {
                    flag = true;
                }
                break;
            case 11: // Paralysis attack
                if (test_hit(2, (int) r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac, CLA_MISC_HIT)) {
                    flag = true;
                }
                break;
            case 12: // Steal Money
                if ((test_hit(5, (int) r_ptr->level, 0, (int) py.misc.lev, CLA_MISC_HIT)) && (py.misc.au > 0)) {
                    flag = true;
                }
                break;
            case 13: // Steal Object
                if ((test_hit(2, (int) r_ptr->level, 0, (int) py.misc.lev, CLA_MISC_HIT)) && (inven_ctr > 0)) {
                    flag = true;
                }
                break;
            case 14: // Poison
                if (test_hit(5, (int) r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac, CLA_MISC_HIT)) {
                    flag = true;
                }
                break;
            case 15: // Lose dexterity
                if (test_hit(0, (int) r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac, CLA_MISC_HIT)) {
                    flag = true;
                }
                break;
            case 16: // Lose constitution
                if (test_hit(0, (int) r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac, CLA_MISC_HIT)) {
                    flag = true;
                }
                break;
            case 17: // Lose intelligence
                if (test_hit(2, (int) r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac, CLA_MISC_HIT)) {
                    flag = true;
                }
                break;
            case 18: // Lose wisdom
                if (test_hit(2, (int) r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac, CLA_MISC_HIT)) {
                    flag = true;
                }
                break;
            case 19: // Lose experience
                if (test_hit(5, (int) r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac, CLA_MISC_HIT)) {
                    flag = true;
                }
                break;
            case 20: // Aggravate monsters
                flag = true;
                break;
            case 21: // Disenchant
                if (test_hit(20, (int) r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac, CLA_MISC_HIT)) {
                    flag = true;
                }
                break;
            case 22: // Eat food
                if (test_hit(5, (int) r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac, CLA_MISC_HIT)) {
                    flag = true;
                }
                break;
            case 23: // Eat light
                if (test_hit(5, (int) r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac, CLA_MISC_HIT)) {
                    flag = true;
                }
                break;
            case 24: // Eat charges
                // check to make sure an object exists
                if ((test_hit(15, (int) r_ptr->level, 0, p_ptr->pac + p_ptr->ptoac, CLA_MISC_HIT)) && (inven_ctr > 0)) {
                    flag = true;
                }
                break;
            case 99:
                flag = true;
                break;
            default:
                break;
        }

        if (flag) {
            // can not strcat to cdesc because the creature may have multiple attacks.
            disturb(1, 0);
            (void) strcpy(tmp_str, cdesc);
            switch (adesc) {
                case 1:
                    msg_print(strcat(tmp_str, "hits you."));
                    break;
                case 2:
                    msg_print(strcat(tmp_str, "bites you."));
                    break;
                case 3:
                    msg_print(strcat(tmp_str, "claws you."));
                    break;
                case 4:
                    msg_print(strcat(tmp_str, "stings you."));
                    break;
                case 5:
                    msg_print(strcat(tmp_str, "touches you."));
                    break;
#if 0
                case 6:
                    msg_print(strcat(tmp_str, "kicks you."));
                    break;
#endif
                case 7:
                    msg_print(strcat(tmp_str, "gazes at you."));
                    break;
                case 8:
                    msg_print(strcat(tmp_str, "breathes on you."));
                    break;
                case 9:
                    msg_print(strcat(tmp_str, "spits on you."));
                    break;
                case 10:
                    msg_print(strcat(tmp_str, "makes a horrible wail."));
                    break;
#if 0
                case 11:
                    msg_print(strcat(tmp_str, "embraces you."));
                    break;
#endif
                case 12:
                    msg_print(strcat(tmp_str, "crawls on you."));
                    break;
                case 13:
                    msg_print(strcat(tmp_str, "releases a cloud of spores."));
                    break;
                case 14:
                    msg_print(strcat(tmp_str, "begs you for money."));
                    break;
                case 15:
                    msg_print("You've been slimed!");
                    break;
                case 16:
                    msg_print(strcat(tmp_str, "crushes you."));
                    break;
                case 17:
                    msg_print(strcat(tmp_str, "tramples you."));
                    break;
                case 18:
                    msg_print(strcat(tmp_str, "drools on you."));
                    break;
                case 19:
                    switch (randint(9)) {
                        case 1:
                            msg_print(strcat(tmp_str, "insults you!"));
                            break;
                        case 2:
                            msg_print(strcat(tmp_str, "insults your mother!"));
                            break;
                        case 3:
                            msg_print(strcat(tmp_str, "gives you the finger!"));
                            break;
                        case 4:
                            msg_print(strcat(tmp_str, "humiliates you!"));
                            break;
                        case 5:
                            msg_print(strcat(tmp_str, "wets on your leg!"));
                            break;
                        case 6:
                            msg_print(strcat(tmp_str, "defiles you!"));
                            break;
                        case 7:
                            msg_print(strcat(tmp_str, "dances around you!"));
                            break;
                        case 8:
                            msg_print(strcat(tmp_str, "makes obscene gestures!"));
                            break;
                        case 9:
                            msg_print(strcat(tmp_str, "moons you!!!"));
                            break;
                    }
                    break;
                case 99:
                    msg_print(strcat(tmp_str, "is repelled."));
                    break;
                default:
                    break;
            }

            bool notice = true;
            bool visible = true;

            // always fail to notice attack if creature invisible, set notice
            // and visible here since creature may be visible when attacking
            // and then teleport afterwards (becoming effectively invisible)
            if (!m_ptr->ml) {
                visible = false;
                notice = false;
            } else {
                visible = true;
            }

            damage = damroll(adice, asides);

            switch (attype) {
                case 1: // Normal attack
                    // round half-way case down
                    damage -= ((p_ptr->pac + p_ptr->ptoac) * damage) / 200;
                    take_hit(damage, ddesc);
                    break;
                case 2: // Lose Strength
                    take_hit(damage, ddesc);
                    if (py.flags.sustain_str) {
                        msg_print("You feel weaker for a moment, but it passes.");
                    } else if (randint(2) == 1) {
                        msg_print("You feel weaker.");
                        (void) dec_stat(A_STR);
                    } else {
                        notice = false;
                    }
                    break;
                case 3: // Confusion attack
                    f_ptr = &py.flags;
                    take_hit(damage, ddesc);
                    if (randint(2) == 1) {
                        if (f_ptr->confused < 1) {
                            msg_print("You feel confused.");
                            f_ptr->confused += randint((int) r_ptr->level);
                        } else {
                            notice = false;
                        }
                        f_ptr->confused += 3;
                    } else {
                        notice = false;
                    }
                    break;
                case 4: // Fear attack
                    f_ptr = &py.flags;
                    take_hit(damage, ddesc);
                    if (player_saves()) {
                        msg_print("You resist the effects!");
                    } else if (f_ptr->afraid < 1) {
                        msg_print("You are suddenly afraid!");
                        f_ptr->afraid += 3 + randint((int) r_ptr->level);
                    } else {
                        f_ptr->afraid += 3;
                        notice = false;
                    }
                    break;
                case 5: // Fire attack
                    msg_print("You are enveloped in flames!");
                    fire_dam(damage, ddesc);
                    break;
                case 6: // Acid attack
                    msg_print("You are covered in acid!");
                    acid_dam(damage, ddesc);
                    break;
                case 7: // Cold attack
                    msg_print("You are covered with frost!");
                    cold_dam(damage, ddesc);
                    break;
                case 8: // Lightning attack
                    msg_print("Lightning strikes you!");
                    light_dam(damage, ddesc);
                    break;
                case 9: // Corrosion attack
                    msg_print("A stinging red gas swirls about you.");
                    corrode_gas(ddesc);
                    take_hit(damage, ddesc);
                    break;
                case 10: // Blindness attack
                    f_ptr = &py.flags;
                    take_hit(damage, ddesc);
                    if (f_ptr->blind < 1) {
                        f_ptr->blind += 10 + randint((int) r_ptr->level);
                        msg_print("Your eyes begin to sting.");
                    } else {
                        f_ptr->blind += 5;
                        notice = false;
                    }
                    break;
                case 11: // Paralysis attack
                    f_ptr = &py.flags;
                    take_hit(damage, ddesc);
                    if (player_saves()) {
                        msg_print("You resist the effects!");
                    } else if (f_ptr->paralysis < 1) {
                        if (f_ptr->free_act) {
                            msg_print("You are unaffected.");
                        } else {
                            f_ptr->paralysis = (int16_t) (randint((int) r_ptr->level) + 3);
                            msg_print("You are paralyzed.");
                        }
                    } else {
                        notice = false;
                    }
                    break;
                case 12: // Steal Money
                    if ((py.flags.paralysis < 1) && (randint(124) < py.stats.use_stat[A_DEX])) {
                        msg_print("You quickly protect your money pouch!");
                    } else {
                        gold = (p_ptr->au / 10) + randint(25);
                        if (gold > p_ptr->au) {
                            p_ptr->au = 0;
                        } else {
                            p_ptr->au -= gold;
                        }
                        msg_print("Your purse feels lighter.");
                        prt_gold();
                    }
                    if (randint(2) == 1) {
                        msg_print("There is a puff of smoke!");
                        teleport_away(monptr, MAX_SIGHT);
                    }
                    break;
                case 13: // Steal Object
                    if ((py.flags.paralysis < 1) && (randint(124) < py.stats.use_stat[A_DEX])) {
                        msg_print("You grab hold of your backpack!");
                    } else {
                        i = randint(inven_ctr) - 1;
                        inven_destroy(i);
                        msg_print("Your backpack feels lighter.");
                    }
                    if (randint(2) == 1) {
                        msg_print("There is a puff of smoke!");
                        teleport_away(monptr, MAX_SIGHT);
                    }
                    break;
                case 14: // Poison
                    f_ptr = &py.flags;
                    take_hit(damage, ddesc);
                    msg_print("You feel very sick.");
                    f_ptr->poisoned += randint((int) r_ptr->level) + 5;
                    break;
                case 15: // Lose dexterity
                    f_ptr = &py.flags;
                    take_hit(damage, ddesc);
                    if (f_ptr->sustain_dex) {
                        msg_print("You feel clumsy for a moment, but it passes.");
                    } else {
                        msg_print("You feel more clumsy.");
                        (void) dec_stat(A_DEX);
                    }
                    break;
                case 16: // Lose constitution
                    f_ptr = &py.flags;
                    take_hit(damage, ddesc);
                    if (f_ptr->sustain_con) {
                        msg_print("Your body resists the effects of the disease.");
                    } else {
                        msg_print("Your health is damaged!");
                        (void) dec_stat(A_CON);
                    }
                    break;
                case 17: // Lose intelligence
                    f_ptr = &py.flags;
                    take_hit(damage, ddesc);
                    msg_print("You have trouble thinking clearly.");
                    if (f_ptr->sustain_int) {
                        msg_print("But your mind quickly clears.");
                    } else {
                        (void) dec_stat(A_INT);
                    }
                    break;
                case 18: // Lose wisdom
                    f_ptr = &py.flags;
                    take_hit(damage, ddesc);
                    if (f_ptr->sustain_wis) {
                        msg_print("Your wisdom is sustained.");
                    } else {
                        msg_print("Your wisdom is drained.");
                        (void) dec_stat(A_WIS);
                    }
                    break;
                case 19: // Lose experience
                    msg_print("You feel your life draining away!");
                    lose_exp(damage + (py.misc.exp / 100) * MON_DRAIN_LIFE);
                    break;
                case 20: // Aggravate monster
                    (void) aggravate_monster(20);
                    break;
                case 21: // Disenchant
                    flag = false;
                    switch (randint(7)) {
                        case 1:
                            i = INVEN_WIELD;
                            break;
                        case 2:
                            i = INVEN_BODY;
                            break;
                        case 3:
                            i = INVEN_ARM;
                            break;
                        case 4:
                            i = INVEN_OUTER;
                            break;
                        case 5:
                            i = INVEN_HANDS;
                            break;
                        case 6:
                            i = INVEN_HEAD;
                            break;
                        case 7:
                            i = INVEN_FEET;
                            break;
                    }
                    i_ptr = &inventory[i];

                    if (i_ptr->tohit > 0) {
                        i_ptr->tohit -= randint(2);

                        // don't send it below zero
                        if (i_ptr->tohit < 0) {
                            i_ptr->tohit = 0;
                        }
                        flag = true;
                    }
                    if (i_ptr->todam > 0) {
                        i_ptr->todam -= randint(2);

                        // don't send it below zero
                        if (i_ptr->todam < 0) {
                            i_ptr->todam = 0;
                        }
                        flag = true;
                    }
                    if (i_ptr->toac > 0) {
                        i_ptr->toac -= randint(2);

                        // don't send it below zero
                        if (i_ptr->toac < 0) {
                            i_ptr->toac = 0;
                        }
                        flag = true;
                    }
                    if (flag) {
                        msg_print("There is a static feeling in the air.");
                        calc_bonuses();
                    } else {
                        notice = false;
                    }
                    break;
                case 22: // Eat food
                    if (find_range(TV_FOOD, TV_NEVER, &i, &j)) {
                        inven_destroy(i);
                        msg_print("It got at your rations!");
                    } else {
                        notice = false;
                    }
                    break;
                case 23: // Eat light
                    i_ptr = &inventory[INVEN_LIGHT];
                    if (i_ptr->p1 > 0) {
                        i_ptr->p1 -= (250 + randint(250));
                        if (i_ptr->p1 < 1) {
                            i_ptr->p1 = 1;
                        }
                        if (py.flags.blind < 1) {
                            msg_print("Your light dims.");
                        } else {
                            notice = false;
                        }
                    } else {
                        notice = false;
                    }
                    break;
                case 24: // Eat charges
                    i = randint(inven_ctr) - 1;
                    j = r_ptr->level;
                    i_ptr = &inventory[i];
                    if (((i_ptr->tval == TV_STAFF) || (i_ptr->tval == TV_WAND)) && (i_ptr->p1 > 0)) {
                        m_ptr->hp += j * i_ptr->p1;
                        i_ptr->p1 = 0;
                        if (!known2_p(i_ptr)) {
                            add_inscribe(i_ptr, ID_EMPTY);
                        }
                        msg_print("Energy drains from your pack!");
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

            // Moved here from mon_move, so that monster only confused if it
            // actually hits. A monster that has been repelled has not hit
            // the player, so it should not be confused.
            if (py.flags.confuse_monster && adesc != 99) {
                msg_print("Your hands stop glowing.");
                py.flags.confuse_monster = false;
                if ((randint(MAX_MONS_LEVEL) < r_ptr->level) || (CD_NO_SLEEP & r_ptr->cdefense)) {
                    (void) sprintf(tmp_str, "%sis unaffected.", cdesc);
                } else {
                    (void) sprintf(tmp_str, "%sappears confused.", cdesc);
                    if (m_ptr->confused) {
                        m_ptr->confused += 3;
                    } else {
                        m_ptr->confused = (uint8_t) (2 + randint(16));
                    }
                }
                msg_print(tmp_str);
                if (visible && !death && randint(4) == 1) {
                    c_recall[m_ptr->mptr].r_cdefense |= r_ptr->cdefense & CD_NO_SLEEP;
                }
            }

            // increase number of attacks if notice true, or if visible and
            // had previously noticed the attack (in which case all this does
            // is help player learn damage), note that in the second case do
            // not increase attacks if creature repelled (no damage done)
            if ((notice || (visible && c_recall[m_ptr->mptr].r_attacks[attackn] != 0 && attype != 99)) && c_recall[m_ptr->mptr].r_attacks[attackn] < MAX_UCHAR) {
                c_recall[m_ptr->mptr].r_attacks[attackn]++;
            }
            if (death && c_recall[m_ptr->mptr].r_deaths < MAX_SHORT) {
                c_recall[m_ptr->mptr].r_deaths++;
            }
        } else {
            if ((adesc >= 1 && adesc <= 3) || (adesc == 6)) {
                disturb(1, 0);
                (void) strcpy(tmp_str, cdesc);
                msg_print(strcat(tmp_str, "misses you."));
            }
        }

        if (attackn < MAX_MON_NATTACK - 1) {
            attackn++;
        } else {
            break;
        }
    }
}

// Make the move if possible, five choices -RAK-
static void make_move(int monptr, int *mm, uint32_t *rcmove) {
    int newy, newx, stuck_door;
    cave_type *c_ptr;
    inven_type *t_ptr;

    bool do_turn = false;
    bool do_move = false;
    monster_type *m_ptr = &m_list[monptr];
    uint32_t movebits = c_list[m_ptr->mptr].cmove;

    // Up to 5 attempts at moving, give up.
    for (int i = 0; !do_turn && i < 5; i++) {
        // Get new position
        newy = m_ptr->fy;
        newx = m_ptr->fx;
        (void) mmove(mm[i], &newy, &newx);
        c_ptr = &cave[newy][newx];
        if (c_ptr->fval != BOUNDARY_WALL) {
            // Floor is open?
            if (c_ptr->fval <= MAX_OPEN_SPACE) {
                do_move = true;
            } else if (movebits & CM_PHASE) {
                // Creature moves through walls?

                do_move = true;
                *rcmove |= CM_PHASE;
            } else if (c_ptr->tptr != 0) {
                // Creature can open doors?

                t_ptr = &t_list[c_ptr->tptr];

                // Creature can open doors.
                if (movebits & CM_OPEN_DOOR) {
                    stuck_door = false;
                    if (t_ptr->tval == TV_CLOSED_DOOR) {
                        do_turn = true;

                        if (t_ptr->p1 == 0) {
                            // Closed doors

                            do_move = true;
                        } else if (t_ptr->p1 > 0) {
                            // Locked doors

                            if (randint((m_ptr->hp + 1) * (50 + t_ptr->p1)) < 40 * (m_ptr->hp - 10 - t_ptr->p1)) {
                                t_ptr->p1 = 0;
                            }
                        } else if (t_ptr->p1 < 0) {
                            // Stuck doors

                            if (randint((m_ptr->hp + 1) * (50 - t_ptr->p1)) < 40 * (m_ptr->hp - 10 + t_ptr->p1)) {
                                msg_print("You hear a door burst open!");
                                disturb(1, 0);
                                stuck_door = true;
                                do_move = true;
                            }
                        }
                    } else if (t_ptr->tval == TV_SECRET_DOOR) {
                        do_turn = true;
                        do_move = true;
                    }
                    if (do_move) {
                        invcopy(t_ptr, OBJ_OPEN_DOOR);

                        // 50% chance of breaking door
                        if (stuck_door) {
                            t_ptr->p1 = (int16_t) (1 - randint(2));
                        }
                        c_ptr->fval = CORR_FLOOR;
                        lite_spot(newy, newx);
                        *rcmove |= CM_OPEN_DOOR;
                        do_move = false;
                    }
                } else {
                    // Creature can not open doors, must bash them

                    if (t_ptr->tval == TV_CLOSED_DOOR) {
                        do_turn = true;
                        if (randint((m_ptr->hp + 1) * (80 + abs(t_ptr->p1))) < 40 * (m_ptr->hp - 20 - abs(t_ptr->p1))) {
                            invcopy(t_ptr, OBJ_OPEN_DOOR);

                            // 50% chance of breaking door
                            t_ptr->p1 = (int16_t) (1 - randint(2));
                            c_ptr->fval = CORR_FLOOR;
                            lite_spot(newy, newx);
                            msg_print("You hear a door burst open!");
                            disturb(1, 0);
                        }
                    }
                }
            }

            // Glyph of warding present?
            if (do_move && (c_ptr->tptr != 0) && (t_list[c_ptr->tptr].tval == TV_VIS_TRAP) && (t_list[c_ptr->tptr].subval == 99)) {
                if (randint(OBJ_RUNE_PROT) < c_list[m_ptr->mptr].level) {
                    if ((newy == char_row) && (newx == char_col)) {
                        msg_print("The rune of protection is broken!");
                    }
                    (void) delete_object(newy, newx);
                } else {
                    do_move = false;

                    // If the creature moves only to attack,
                    // don't let it move if the glyph prevents
                    // it from attacking
                    if (movebits & CM_ATTACK_ONLY) {
                        do_turn = true;
                    }
                }
            }

            // Creature has attempted to move on player?
            if (do_move) {
                if (c_ptr->cptr == 1) {
                    // if the monster is not lit, must call update_mon, it
                    // may be faster than character, and hence could have
                    // just moved next to character this same turn.
                    if (!m_ptr->ml) {
                        update_mon(monptr);
                    }
                    make_attack(monptr);
                    do_move = false;
                    do_turn = true;
                } else if ((c_ptr->cptr > 1) && ((newy != m_ptr->fy) || (newx != m_ptr->fx))) {
                    // Creature is attempting to move on other creature?

                    // Creature eats other creatures?
                    if ((movebits & CM_EATS_OTHER) && (c_list[m_ptr->mptr].mexp >= c_list[m_list[c_ptr->cptr].mptr].mexp)) {
                        if (m_list[c_ptr->cptr].ml) {
                            *rcmove |= CM_EATS_OTHER;
                        }

                        // It ate an already processed monster. Handle normally.
                        if (monptr < c_ptr->cptr) {
                            delete_monster((int) c_ptr->cptr);
                        } else {
                            // If it eats this monster, an already processed
                            // monster will take its place, causing all kinds
                            // of havoc. Delay the kill a bit.
                            fix1_delete_monster((int) c_ptr->cptr);
                        }
                    } else {
                        do_move = false;
                    }
                }
            }

            // Creature has been allowed move.
            if (do_move) {
                // Pick up or eat an object
                if (movebits & CM_PICKS_UP) {
                    c_ptr = &cave[newy][newx];

                    if ((c_ptr->tptr != 0) && (t_list[c_ptr->tptr].tval <= TV_MAX_OBJECT)) {
                        *rcmove |= CM_PICKS_UP;
                        (void) delete_object(newy, newx);
                    }
                }

                // Move creature record
                move_rec((int) m_ptr->fy, (int) m_ptr->fx, newy, newx);
                if (m_ptr->ml) {
                    m_ptr->ml = false;
                    lite_spot((int) m_ptr->fy, (int) m_ptr->fx);
                }
                m_ptr->fy = (uint8_t) newy;
                m_ptr->fx = (uint8_t) newx;
                m_ptr->cdis = (uint8_t) distance(char_row, char_col, newy, newx);
                do_turn = true;
            }
        }
    }
}

// Creatures can cast spells too.  (Dragon Breath) -RAK-
//   cast_spell = true if creature changes position
//   took_turn  = true if creature casts a spell
static void mon_cast_spell(int monptr, bool *took_turn) {
    if (death) {
        return;
    }

    monster_type *m_ptr = &m_list[monptr];
    creature_type *r_ptr = &c_list[m_ptr->mptr];
    int chance = (int) (r_ptr->spells & CS_FREQ);

    if (randint(chance) != 1) {
        // 1 in x chance of casting spell

        *took_turn = false;
    } else if (m_ptr->cdis > MAX_SPELL_DIS) {
        // Must be within certain range

        *took_turn = false;
    } else if (!los(char_row, char_col, (int) m_ptr->fy, (int) m_ptr->fx)) {
        // Must have unobstructed Line-Of-Sight

        *took_turn = false;
    } else {
        // Creature is going to cast a spell

        *took_turn = true;

        // Check to see if monster should be lit.
        update_mon(monptr);

        // Describe the attack
        vtype cdesc;
        if (m_ptr->ml) {
            (void) sprintf(cdesc, "The %s ", r_ptr->name);
        } else {
            (void) strcpy(cdesc, "It ");
        }

        // For "DIED_FROM" string
        vtype ddesc;
        if (CM_WIN & r_ptr->cmove) {
            (void) sprintf(ddesc, "The %s", r_ptr->name);
        } else if (is_a_vowel(r_ptr->name[0])) {
            (void) sprintf(ddesc, "an %s", r_ptr->name);
        } else {
            (void) sprintf(ddesc, "a %s", r_ptr->name);
        }
        // End DIED_FROM

        // Extract all possible spells into spell_choice
        int spell_choice[30];
        uint32_t i = (uint32_t) (r_ptr->spells & ~CS_FREQ);
        int k = 0;

        while (i != 0) {
            spell_choice[k] = bit_pos(&i);
            k++;
        }

        // Choose a spell to cast
        int thrown_spell = spell_choice[randint(k) - 1];
        thrown_spell++;

        // all except teleport_away() and drain mana spells always disturb
        if (thrown_spell > 6 && thrown_spell != 17) {
            disturb(1, 0);
        }

        // save some code/data space here, with a small time penalty
        if ((thrown_spell < 14 && thrown_spell > 6) || (thrown_spell == 16)) {
            (void) strcat(cdesc, "casts a spell.");
            msg_print(cdesc);
        }

        int y, x;

        // Cast the spell.
        switch (thrown_spell) {
            case 5: // Teleport Short
                teleport_away(monptr, 5);
                break;
            case 6: // Teleport Long
                teleport_away(monptr, MAX_SIGHT);
                break;
            case 7: // Teleport To
                teleport_to((int) m_ptr->fy, (int) m_ptr->fx);
                break;
            case 8: // Light Wound
                if (player_saves()) {
                    msg_print("You resist the effects of the spell.");
                } else {
                    take_hit(damroll(3, 8), ddesc);
                }
                break;
            case 9: // Serious Wound
                if (player_saves()) {
                    msg_print("You resist the effects of the spell.");
                } else {
                    take_hit(damroll(8, 8), ddesc);
                }
                break;
            case 10: // Hold Person
                if (py.flags.free_act) {
                    msg_print("You are unaffected.");
                } else if (player_saves()) {
                    msg_print("You resist the effects of the spell.");
                } else if (py.flags.paralysis > 0) {
                    py.flags.paralysis += 2;
                } else {
                    py.flags.paralysis = (int16_t) (randint(5) + 4);
                }
                break;
            case 11: // Cause Blindness
                if (player_saves()) {
                    msg_print("You resist the effects of the spell.");
                } else if (py.flags.blind > 0) {
                    py.flags.blind += 6;
                } else {
                    py.flags.blind += 12 + randint(3);
                }
                break;
            case 12: // Cause Confuse
                if (player_saves()) {
                    msg_print("You resist the effects of the spell.");
                } else if (py.flags.confused > 0) {
                    py.flags.confused += 2;
                } else {
                    py.flags.confused = (int16_t) (randint(5) + 3);
                }
                break;
            case 13: // Cause Fear
                if (player_saves()) {
                    msg_print("You resist the effects of the spell.");
                } else if (py.flags.afraid > 0) {
                    py.flags.afraid += 2;
                } else {
                    py.flags.afraid = (int16_t) (randint(5) + 3);
                }
                break;
            case 14: // Summon Monster
                (void) strcat(cdesc, "magically summons a monster!");
                msg_print(cdesc);
                y = char_row;
                x = char_col;

                // in case compact_monster() is called,it needs monptr
                hack_monptr = monptr;
                (void) summon_monster(&y, &x, false);
                hack_monptr = -1;
                update_mon((int) cave[y][x].cptr);
                break;
            case 15: // Summon Undead
                (void) strcat(cdesc, "magically summons an undead!");
                msg_print(cdesc);
                y = char_row;
                x = char_col;

                // in case compact_monster() is called,it needs monptr
                hack_monptr = monptr;
                (void) summon_undead(&y, &x);
                hack_monptr = -1;
                update_mon((int) cave[y][x].cptr);
                break;
            case 16: // Slow Person
                if (py.flags.free_act) {
                    msg_print("You are unaffected.");
                } else if (player_saves()) {
                    msg_print("You resist the effects of the spell.");
                } else if (py.flags.slow > 0) {
                    py.flags.slow += 2;
                } else {
                    py.flags.slow = (int16_t) (randint(5) + 3);
                }
                break;
            case 17: // Drain Mana
                if (py.misc.cmana > 0) {
                    vtype outval;

                    disturb(1, 0);
                    (void) sprintf(outval, "%sdraws psychic energy from you!", cdesc);
                    msg_print(outval);
                    if (m_ptr->ml) {
                        (void) sprintf(outval, "%sappears healthier.", cdesc);
                        msg_print(outval);
                    }

                    int r1 = (randint((int) r_ptr->level) >> 1) + 1;
                    if (r1 > py.misc.cmana) {
                        r1 = py.misc.cmana;
                        py.misc.cmana = 0;
                        py.misc.cmana_frac = 0;
                    } else {
                        py.misc.cmana -= r1;
                    }
                    prt_cmana();
                    m_ptr->hp += 6 * (r1);
                }
                break;
            case 20: // Breath Light
                (void) strcat(cdesc, "breathes lightning.");
                msg_print(cdesc);
                breath(GF_LIGHTNING, char_row, char_col, (m_ptr->hp / 4), ddesc, monptr);
                break;
            case 21: // Breath Gas
                (void) strcat(cdesc, "breathes gas.");
                msg_print(cdesc);
                breath(GF_POISON_GAS, char_row, char_col, (m_ptr->hp / 3), ddesc, monptr);
                break;
            case 22: // Breath Acid
                (void) strcat(cdesc, "breathes acid.");
                msg_print(cdesc);
                breath(GF_ACID, char_row, char_col, (m_ptr->hp / 3), ddesc, monptr);
                break;
            case 23: // Breath Frost
                (void) strcat(cdesc, "breathes frost.");
                msg_print(cdesc);
                breath(GF_FROST, char_row, char_col, (m_ptr->hp / 3), ddesc, monptr);
                break;
            case 24: // Breath Fire
                (void) strcat(cdesc, "breathes fire.");
                msg_print(cdesc);
                breath(GF_FIRE, char_row, char_col, (m_ptr->hp / 3), ddesc, monptr);
                break;
            default:
                (void) strcat(cdesc, "cast unknown spell.");
                msg_print(cdesc);
        }
        // End of spells

        if (m_ptr->ml) {
            c_recall[m_ptr->mptr].r_spells |= 1L << (thrown_spell - 1);
            if ((c_recall[m_ptr->mptr].r_spells & CS_FREQ) != CS_FREQ) {
                c_recall[m_ptr->mptr].r_spells++;
            }
            if (death && c_recall[m_ptr->mptr].r_deaths < MAX_SHORT) {
                c_recall[m_ptr->mptr].r_deaths++;
            }
        }
    }
}

// Places creature adjacent to given location -RAK-
// Rats and Flys are fun!
bool multiply_monster(int y, int x, int cr_index, int monptr) {
    cave_type *c_ptr;

    for (int i = 0; i <= 18; i++) {
        int j = y - 2 + randint(3);
        int k = x - 2 + randint(3);

        // don't create a new creature on top of the old one, that
        // causes invincible/invisible creatures to appear.
        if (in_bounds(j, k) && (j != y || k != x)) {
            c_ptr = &cave[j][k];
            if ((c_ptr->fval <= MAX_OPEN_SPACE) && (c_ptr->tptr == 0) && (c_ptr->cptr != 1)) {
                // Creature there already?
                if (c_ptr->cptr > 1) {
                    // Some critters are cannibalistic!
                    if ((c_list[cr_index].cmove & CM_EATS_OTHER)
                        // Check the experience level -CJS-
                        && c_list[cr_index].mexp >= c_list[m_list[c_ptr->cptr].mptr].mexp) {
                        // It ate an already processed monster.Handle * normally.
                        if (monptr < c_ptr->cptr) {
                            delete_monster((int) c_ptr->cptr);
                        } else {
                            // If it eats this monster, an already processed
                            // mosnter will take its place, causing all kinds
                            // of havoc. Delay the kill a bit.
                            fix1_delete_monster((int) c_ptr->cptr);
                        }

                        // in case compact_monster() is called,it needs monptr
                        hack_monptr = monptr;

                        // Place_monster() may fail if monster list full.
                        int result = place_monster(j, k, cr_index, false);
                        hack_monptr = -1;
                        if (!result) {
                            return false;
                        }
                        mon_tot_mult++;
                        return check_mon_lite(j, k);
                    }
                } else {
                    // All clear,  place a monster

                    // in case compact_monster() is called,it needs monptr
                    hack_monptr = monptr;

                    // Place_monster() may fail if monster list full.
                    int result = place_monster(j, k, cr_index, false);
                    hack_monptr = -1;
                    if (!result) {
                        return false;
                    }
                    mon_tot_mult++;
                    return check_mon_lite(j, k);
                }
            }
        }
    }

    return false;
}

// Move the critters about the dungeon -RAK-
static void mon_move(int monptr, uint32_t *rcmove) {
    int i, k;

    monster_type *m_ptr = &m_list[monptr];
    creature_type *r_ptr = &c_list[m_ptr->mptr];

    // Does the critter multiply?
    // rest could be negative, to be safe, only use mod with positive values.
    int rest_val = abs(py.flags.rest);

    if ((r_ptr->cmove & CM_MULTIPLY) && (MAX_MON_MULT >= mon_tot_mult) && ((rest_val % MON_MULT_ADJ) == 0)) {
        k = 0;
        for (i = m_ptr->fy - 1; i <= m_ptr->fy + 1; i++) {
            for (int j = m_ptr->fx - 1; j <= m_ptr->fx + 1; j++) {
                if (in_bounds(i, j) && (cave[i][j].cptr > 1)) {
                    k++;
                }
            }
        }

        // can't call randint with a value of zero, increment
        // counter to allow creature multiplication.
        if (k == 0) {
            k++;
        }
        if ((k < 4) && (randint(k * MON_MULT_ADJ) == 1)) {
            if (multiply_monster((int) m_ptr->fy, (int) m_ptr->fx, (int) m_ptr->mptr, monptr)) {
                *rcmove |= CM_MULTIPLY;
            }
        }
    }

    int mm[9];
    bool move_test = false;

    // if in wall, must immediately escape to a clear area
    if (!(r_ptr->cmove & CM_PHASE) && (cave[m_ptr->fy][m_ptr->fx].fval >= MIN_CAVE_WALL)) {
        // If the monster is already dead, don't kill it again!
        // This can happen for monsters moving faster than the player. They
        // will get multiple moves, but should not if they die on the first
        // move.  This is only a problem for monsters stuck in rock.
        if (m_ptr->hp < 0) {
            return;
        }

        k = 0;
        int dir = 1;

        // Note direction of for loops matches direction of keypad from 1 to 9
        // Do not allow attack against the player.
        // Must cast fy-1 to signed int, so that a negative value
        // of i will fail the comparison.
        for (i = m_ptr->fy + 1; i >= (m_ptr->fy - 1); i--) {
            for (int j = m_ptr->fx - 1; j <= m_ptr->fx + 1; j++) {
                if ((dir != 5) && (cave[i][j].fval <= MAX_OPEN_SPACE) && (cave[i][j].cptr != 1)) {
                    mm[k++] = dir;
                }
                dir++;
            }
        }

        if (k != 0) {
            // put a random direction first
            dir = randint(k) - 1;
            i = mm[0];
            mm[0] = mm[dir];
            mm[dir] = i;
            make_move(monptr, mm, rcmove);
            // this can only fail if mm[0] has a rune of protection
        }

        // if still in a wall, let it dig itself out, but also apply some more damage
        if (cave[m_ptr->fy][m_ptr->fx].fval >= MIN_CAVE_WALL) {
            // in case the monster dies, may need to callfix1_delete_monster()
            // instead of delete_monsters()
            hack_monptr = monptr;
            i = mon_take_hit(monptr, damroll(8, 8));
            hack_monptr = -1;
            if (i >= 0) {
                msg_print("You hear a scream muffled by rock!");
                prt_experience();
            } else {
                msg_print("A creature digs itself out from the rock!");
                (void) twall((int) m_ptr->fy, (int) m_ptr->fx, 1, 0);
            }
        }
        // monster movement finished
        return;
    } else if (m_ptr->confused) {
        // Creature is confused or undead turned?

        // Undead only get confused from turn undead, so they should flee
        if (r_ptr->cdefense & CD_UNDEAD) {
            get_moves(monptr, mm);
            mm[0] = 10 - mm[0];
            mm[1] = 10 - mm[1];
            mm[2] = 10 - mm[2];
            mm[3] = randint(9); // May attack only if cornered
            mm[4] = randint(9);
        } else {
            mm[0] = randint(9);
            mm[1] = randint(9);
            mm[2] = randint(9);
            mm[3] = randint(9);
            mm[4] = randint(9);
        }

        // don't move him if he is not supposed to move!
        if (!(r_ptr->cmove & CM_ATTACK_ONLY)) {
            make_move(monptr, mm, rcmove);
        }
        m_ptr->confused--;
        move_test = true;
    } else if (r_ptr->spells & CS_FREQ) {
        // Creature may cast a spell
        mon_cast_spell(monptr, &move_test);
    }

    if (!move_test) {
        if ((r_ptr->cmove & CM_75_RANDOM) && (randint(100) < 75)) {
            // 75% random movement
            mm[0] = randint(9);
            mm[1] = randint(9);
            mm[2] = randint(9);
            mm[3] = randint(9);
            mm[4] = randint(9);
            *rcmove |= CM_75_RANDOM;
            make_move(monptr, mm, rcmove);
        } else if ((r_ptr->cmove & CM_40_RANDOM) && (randint(100) < 40)) {
            // 40% random movement
            mm[0] = randint(9);
            mm[1] = randint(9);
            mm[2] = randint(9);
            mm[3] = randint(9);
            mm[4] = randint(9);
            *rcmove |= CM_40_RANDOM;
            make_move(monptr, mm, rcmove);
        } else if ((r_ptr->cmove & CM_20_RANDOM) && (randint(100) < 20)) {
            // 20% random movement
            mm[0] = randint(9);
            mm[1] = randint(9);
            mm[2] = randint(9);
            mm[3] = randint(9);
            mm[4] = randint(9);
            *rcmove |= CM_20_RANDOM;
            make_move(monptr, mm, rcmove);
        } else if (r_ptr->cmove & CM_MOVE_NORMAL) {
            // Normal movement
            if (randint(200) == 1) {
                mm[0] = randint(9);
                mm[1] = randint(9);
                mm[2] = randint(9);
                mm[3] = randint(9);
                mm[4] = randint(9);
            } else {
                get_moves(monptr, mm);
            }
            *rcmove |= CM_MOVE_NORMAL;
            make_move(monptr, mm, rcmove);
        } else if (r_ptr->cmove & CM_ATTACK_ONLY) {
            // Attack, but don't move
            if (m_ptr->cdis < 2) {
                get_moves(monptr, mm);
                make_move(monptr, mm, rcmove);
            } else {
                // Learn that the monster does does not move when
                // it should have moved, but didn't.
                *rcmove |= CM_ATTACK_ONLY;
            }
        } else if ((r_ptr->cmove & CM_ONLY_MAGIC) && (m_ptr->cdis < 2)) {
            // A little hack for Quylthulgs, so that one will eventually
            // notice that they have no physical attacks.
            if (c_recall[m_ptr->mptr].r_attacks[0] < MAX_UCHAR) {
                c_recall[m_ptr->mptr].r_attacks[0]++;
            }

            // Another little hack for Quylthulgs, so that one can
            // eventually learn their speed.
            if (c_recall[m_ptr->mptr].r_attacks[0] > 20) {
                c_recall[m_ptr->mptr].r_cmove |= CM_ONLY_MAGIC;
            }
        }
    }
}

// Creatures movement and attacking are done from here -RAK-
void creatures(int attack) {
    int k;
    int notice;
    uint32_t rcmove;
    bool wake, ignore;
    monster_type *m_ptr;
    recall_type *r_ptr;
    vtype cdesc;

    // Process the monsters
    for (int i = mfptr - 1; i >= MIN_MONIX && !death; i--) {
        m_ptr = &m_list[i];
        // Get rid of an eaten/breathed on monster.  Note: Be sure not to
        // process this monster. This is necessary because we can't delete
        // monsters while scanning the m_list here.
        if (m_ptr->hp < 0) {
            fix2_delete_monster(i);
            continue;
        }

        m_ptr->cdis = (uint8_t) distance(char_row, char_col, (int) m_ptr->fy, (int) m_ptr->fx);

        // Attack is argument passed to CREATURE
        if (attack) {
            k = movement_rate(m_ptr->cspeed);
            if (k <= 0) {
                update_mon(i);
            } else {
                while (k > 0) {
                    k--;
                    wake = false;
                    ignore = false;
                    rcmove = 0;

                    // Monsters trapped in rock must be given a turn also,
                    // so that they will die/dig out immediately.
                    if (m_ptr->ml || (m_ptr->cdis <= c_list[m_ptr->mptr].aaf) || ((!(c_list[m_ptr->mptr].cmove & CM_PHASE)) && cave[m_ptr->fy][m_ptr->fx].fval >= MIN_CAVE_WALL)) {
                        if (m_ptr->csleep > 0) {
                            if (py.flags.aggravate) {
                                m_ptr->csleep = 0;
                            } else if ((py.flags.rest == 0 && py.flags.paralysis < 1) || (randint(50) == 1)) {
                                notice = randint(1024);
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
                            if (randint(5000) < c_list[m_ptr->mptr].level * c_list[m_ptr->mptr].level) {
                                m_ptr->stunned = 0;
                            } else {
                                m_ptr->stunned--;
                            }
                            if (m_ptr->stunned == 0) {
                                if (m_ptr->ml) {
                                    (void) sprintf(cdesc, "The %s ", c_list[m_ptr->mptr].name);
                                    msg_print(strcat(cdesc, "recovers and glares at you."));
                                }
                            }
                        }
                        if ((m_ptr->csleep == 0) && (m_ptr->stunned == 0)) {
                            mon_move(i, &rcmove);
                        }
                    }

                    update_mon(i);
                    if (m_ptr->ml) {
                        r_ptr = &c_recall[m_ptr->mptr];
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
                }
            }
        } else {
            update_mon(i);
        }

        // Get rid of an eaten/breathed on monster. This is necessary because
        // we can't delete monsters while scanning the m_list here.
        // This monster may have been killed during mon_move().
        if (m_ptr->hp < 0) {
            fix2_delete_monster(i);
            continue;
        }
    }
    // End processing monsters
}
