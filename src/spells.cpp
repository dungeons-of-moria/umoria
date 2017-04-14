// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Player/creature spells, breaths, wands, scrolls, etc. code

#include "headers.h"
#include "externs.h"

static void replace_spot(int, int, int);

// Following are spell procedure/functions -RAK-
// These routines are commonly used in the scroll, potion, wands, and
// staves routines, and are occasionally called from other areas.
// Now included are creature spells also.           -RAK

void monster_name(char *description, bool monsterLit, const char *monsterName) {
    if (!monsterLit) {
        (void) strcpy(description, "It");
    } else {
        (void) sprintf(description, "The %s", monsterName);
    }
}

void lower_monster_name(char *description, bool monsterLit, const char *monsterName) {
    if (!monsterLit) {
        (void) strcpy(description, "it");
    } else {
        (void) sprintf(description, "the %s", monsterName);
    }
}

// Sleep creatures adjacent to player -RAK-
bool sleep_monsters1(int y, int x) {
    bool asleep = false;

    for (int i = y - 1; i <= y + 1; i++) {
        for (int j = x - 1; j <= x + 1; j++) {
            if (cave[i][j].cptr <= 1) {
                continue;
            }

            monster_type *m_ptr = &m_list[cave[i][j].cptr];
            creature_type *r_ptr = &c_list[m_ptr->mptr];

            vtype m_name;
            monster_name(m_name, m_ptr->ml, r_ptr->name);

            if (randint(MAX_MONS_LEVEL) < r_ptr->level || (CD_NO_SLEEP & r_ptr->cdefense)) {
                if (m_ptr->ml && (r_ptr->cdefense & CD_NO_SLEEP)) {
                    c_recall[m_ptr->mptr].r_cdefense |= CD_NO_SLEEP;
                }

                vtype out_val;
                (void) sprintf(out_val, "%s is unaffected.", m_name);
                msg_print(out_val);
            } else {
                m_ptr->csleep = 500;
                asleep = true;

                vtype out_val;
                (void) sprintf(out_val, "%s falls asleep.", m_name);
                msg_print(out_val);
            }
        }
    }

    return asleep;
}

// Detect any treasure on the current panel -RAK-
bool detect_treasure() {
    bool detected = false;

    for (int i = panel_row_min; i <= panel_row_max; i++) {
        for (int j = panel_col_min; j <= panel_col_max; j++) {
            cave_type *c_ptr = &cave[i][j];

            if (c_ptr->tptr != 0 && t_list[c_ptr->tptr].tval == TV_GOLD && !test_light(i, j)) {
                c_ptr->fm = true;
                lite_spot(i, j);

                detected = true;
            }
        }
    }

    return detected;
}

// Detect all objects on the current panel -RAK-
bool detect_object() {
    bool detected = false;

    for (int i = panel_row_min; i <= panel_row_max; i++) {
        for (int j = panel_col_min; j <= panel_col_max; j++) {
            cave_type *c_ptr = &cave[i][j];

            if (c_ptr->tptr != 0 && t_list[c_ptr->tptr].tval < TV_MAX_OBJECT && !test_light(i, j)) {
                c_ptr->fm = true;
                lite_spot(i, j);

                detected = true;
            }
        }
    }

    return detected;
}

// Locates and displays traps on current panel -RAK-
bool detect_trap() {
    bool detected = false;

    for (int i = panel_row_min; i <= panel_row_max; i++) {
        for (int j = panel_col_min; j <= panel_col_max; j++) {
            cave_type *c_ptr = &cave[i][j];

            if (c_ptr->tptr == 0) {
                continue;
            }

            if (t_list[c_ptr->tptr].tval == TV_INVIS_TRAP) {
                c_ptr->fm = true;
                change_trap(i, j);

                detected = true;
            } else if (t_list[c_ptr->tptr].tval == TV_CHEST) {
                inven_type *t_ptr = &t_list[c_ptr->tptr];
                known2(t_ptr);
            }
        }
    }

    return detected;
}

// Locates and displays all secret doors on current panel -RAK-
bool detect_sdoor() {
    bool detected = false;

    for (int i = panel_row_min; i <= panel_row_max; i++) {
        for (int j = panel_col_min; j <= panel_col_max; j++) {
            cave_type *c_ptr = &cave[i][j];

            if (c_ptr->tptr == 0) {
                continue;
            }

            if (t_list[c_ptr->tptr].tval == TV_SECRET_DOOR) {
                // Secret doors

                c_ptr->fm = true;
                change_trap(i, j);

                detected = true;
            } else if ((t_list[c_ptr->tptr].tval == TV_UP_STAIR || t_list[c_ptr->tptr].tval == TV_DOWN_STAIR) && !c_ptr->fm) {
                // Staircases

                c_ptr->fm = true;
                lite_spot(i, j);

                detected = true;
            }
        }
    }

    return detected;
}

// Locates and displays all invisible creatures on current panel -RAK-
bool detect_invisible() {
    bool detected = false;

    for (int i = mfptr - 1; i >= MIN_MONIX; i--) {
        monster_type *m_ptr = &m_list[i];

        if (panel_contains((int) m_ptr->fy, (int) m_ptr->fx) && (CM_INVISIBLE & c_list[m_ptr->mptr].cmove)) {
            m_ptr->ml = true;

            // works correctly even if hallucinating
            print((char) c_list[m_ptr->mptr].cchar, (int) m_ptr->fy, (int) m_ptr->fx);

            detected = true;
        }
    }

    if (detected) {
        msg_print("You sense the presence of invisible creatures!");
        msg_print(CNIL);

        // must unlight every monster just lighted
        creatures(false);
    }

    return detected;
}

// Light an area: -RAK-
//     1.  If corridor  light immediate area
//     2.  If room      light entire room plus immediate area.
bool light_area(int y, int x) {
    if (py.flags.blind < 1) {
        msg_print("You are surrounded by a white light.");
    }

    // NOTE: this is not changed anywhere. A bug or correct? -MRC-
    bool lit = true;

    if (cave[y][x].lr && dun_level > 0) {
        light_room(y, x);
    }

    // Must always light immediate area, because one might be standing on
    // the edge of a room, or next to a destroyed area, etc.
    for (int i = y - 1; i <= y + 1; i++) {
        for (int j = x - 1; j <= x + 1; j++) {
            cave[i][j].pl = true;
            lite_spot(i, j);
        }
    }

    return lit;
}

// Darken an area, opposite of light area -RAK-
bool unlight_area(int y, int x) {
    bool darkened = false;

    if (cave[y][x].lr && dun_level > 0) {
        int tmp1 = (SCREEN_HEIGHT / 2);
        int tmp2 = (SCREEN_WIDTH / 2);
        int start_row = (y / tmp1) * tmp1 + 1;
        int start_col = (x / tmp2) * tmp2 + 1;
        int end_row = start_row + tmp1 - 1;
        int end_col = start_col + tmp2 - 1;

        for (int i = start_row; i <= end_row; i++) {
            for (int j = start_col; j <= end_col; j++) {
                cave_type *c_ptr = &cave[i][j];
                if (c_ptr->lr && c_ptr->fval <= MAX_CAVE_FLOOR) {
                    c_ptr->pl = false;
                    c_ptr->fval = DARK_FLOOR;
                    lite_spot(i, j);
                    if (!test_light(i, j)) {
                        darkened = true;
                    }
                }
            }
        }
    } else {
        for (int i = y - 1; i <= y + 1; i++) {
            for (int j = x - 1; j <= x + 1; j++) {
                cave_type *c_ptr = &cave[i][j];
                if (c_ptr->fval == CORR_FLOOR && c_ptr->pl) {
                    // pl could have been set by star-lite wand, etc
                    c_ptr->pl = false;
                    darkened = true;
                }
            }
        }
    }

    if (darkened && py.flags.blind < 1) {
        msg_print("Darkness surrounds you.");
    }

    return darkened;
}

// Map the current area plus some -RAK-
void map_area() {
    int rowMin = panel_row_min - randint(10);
    int rowMax = panel_row_max + randint(10);
    int colMin = panel_col_min - randint(20);
    int colMax = panel_col_max + randint(20);

    for (int row = rowMin; row <= rowMax; row++) {
        for (int col = colMin; col <= colMax; col++) {
            if (in_bounds(row, col) && cave[row][col].fval <= MAX_CAVE_FLOOR) {
                for (int y = row - 1; y <= row + 1; y++) {
                    for (int x = col - 1; x <= col + 1; x++) {
                        cave_type *c_ptr = &cave[y][x];

                        if (c_ptr->fval >= MIN_CAVE_WALL) {
                            c_ptr->pl = true;
                        } else if (c_ptr->tptr != 0 && t_list[c_ptr->tptr].tval >= TV_MIN_VISIBLE && t_list[c_ptr->tptr].tval <= TV_MAX_VISIBLE) {
                            c_ptr->fm = true;
                        }
                    }
                }
            }
        }
    }

    prt_map();
}

// Identify an object -RAK-
bool ident_spell() {
    int item_val;
    if (!get_item(&item_val, "Item you wish identified?", 0, INVEN_ARRAY_SIZE, CNIL, CNIL)) {
        return false;
    }

    identify(&item_val);

    inven_type *i_ptr = &inventory[item_val];
    known2(i_ptr);

    bigvtype tmp_str;
    objdes(tmp_str, i_ptr, true);

    bigvtype out_val;
    if (item_val >= INVEN_WIELD) {
        calc_bonuses();
        (void) sprintf(out_val, "%s: %s", describe_use(item_val), tmp_str);
    } else {
        (void) sprintf(out_val, "%c %s", item_val + 97, tmp_str);
    }
    msg_print(out_val);

    return true;
}

// Get all the monsters on the level pissed off. -RAK-
bool aggravate_monster(int dis_affect) {
    bool aggravated = false;

    for (int i = mfptr - 1; i >= MIN_MONIX; i--) {
        monster_type *m_ptr = &m_list[i];
        m_ptr->csleep = 0;
        if (m_ptr->cdis <= dis_affect && m_ptr->cspeed < 2) {
            m_ptr->cspeed++;
            aggravated = true;
        }
    }

    if (aggravated) {
        msg_print("You hear a sudden stirring in the distance!");
    }

    return aggravated;
}

// Surround the fool with traps (chuckle) -RAK-
bool trap_creation() {
    // NOTE: this is not changed anywhere. A bug or correct? -MRC-
    bool trapped = true;

    for (int i = char_row - 1; i <= char_row + 1; i++) {
        for (int j = char_col - 1; j <= char_col + 1; j++) {
            // Don't put a trap under the player, since this can lead to
            // strange situations, e.g. falling through a trap door while
            // trying to rest, setting off a falling rock trap and ending
            // up under the rock.
            if (i == char_row && j == char_col) {
                continue;
            }

            cave_type *c_ptr = &cave[i][j];

            if (c_ptr->fval <= MAX_CAVE_FLOOR) {
                if (c_ptr->tptr != 0) {
                    (void) delete_object(i, j);
                }
                place_trap(i, j, randint(MAX_TRAP) - 1);

                // don't let player gain exp from the newly created traps
                t_list[c_ptr->tptr].p1 = 0;

                // open pits are immediately visible, so call lite_spot
                lite_spot(i, j);
            }
        }
    }

    return trapped;
}

// Surround the player with doors. -RAK-
bool door_creation() {
    bool created = false;

    for (int i = char_row - 1; i <= char_row + 1; i++) {
        for (int j = char_col - 1; j <= char_col + 1; j++) {
            // Don't put a door under the player!
            if (i == char_row && j == char_col) {
                continue;
            }

            cave_type *c_ptr = &cave[i][j];

            if (c_ptr->fval <= MAX_CAVE_FLOOR) {
                if (c_ptr->tptr != 0) {
                    (void) delete_object(i, j);
                }

                int k = popt();
                c_ptr->fval = BLOCKED_FLOOR;
                c_ptr->tptr = (uint8_t) k;
                invcopy(&t_list[k], OBJ_CLOSED_DOOR);
                lite_spot(i, j);

                created = true;
            }
        }
    }

    return created;
}

// Destroys any adjacent door(s)/trap(s) -RAK-
bool td_destroy() {
    bool destroyed = false;

    for (int i = char_row - 1; i <= char_row + 1; i++) {
        for (int j = char_col - 1; j <= char_col + 1; j++) {
            cave_type *c_ptr = &cave[i][j];

            if (c_ptr->tptr == 0) {
                continue;
            }

            if ((t_list[c_ptr->tptr].tval >= TV_INVIS_TRAP &&
                 t_list[c_ptr->tptr].tval <= TV_CLOSED_DOOR &&
                 t_list[c_ptr->tptr].tval != TV_RUBBLE) ||
                t_list[c_ptr->tptr].tval == TV_SECRET_DOOR) {
                if (delete_object(i, j)) {
                    destroyed = true;
                }
            } else if (t_list[c_ptr->tptr].tval == TV_CHEST && t_list[c_ptr->tptr].flags != 0) {
                // destroy traps on chest and unlock
                t_list[c_ptr->tptr].flags &= ~(CH_TRAPPED | CH_LOCKED);
                t_list[c_ptr->tptr].name2 = SN_UNLOCKED;

                destroyed = true;

                msg_print("You have disarmed the chest.");
                known2(&t_list[c_ptr->tptr]);
            }
        }
    }

    return destroyed;
}

// Display all creatures on the current panel -RAK-
bool detect_monsters() {
    bool detected = false;

    for (int i = mfptr - 1; i >= MIN_MONIX; i--) {
        monster_type *m_ptr = &m_list[i];

        if (panel_contains((int) m_ptr->fy, (int) m_ptr->fx) && (CM_INVISIBLE & c_list[m_ptr->mptr].cmove) == 0) {
            m_ptr->ml = true;
            detected = true;

            // works correctly even if hallucinating
            print((char) c_list[m_ptr->mptr].cchar, (int) m_ptr->fy, (int) m_ptr->fx);
        }
    }

    if (detected) {
        msg_print("You sense the presence of monsters!");
        msg_print(CNIL);

        // must unlight every monster just lighted
        creatures(false);
    }

    return detected;
}

// Leave a line of light in given dir, blue light can sometimes hurt creatures. -RAK-
void light_line(int dir, int y, int x) {
    int dist = -1;

    bool finished = false;
    while (!finished) {
        // put mmove at end because want to light up current spot
        dist++;

        cave_type *c_ptr = &cave[y][x];

        if (dist > OBJ_BOLT_RANGE || c_ptr->fval >= MIN_CLOSED_SPACE) {
            finished = true;
        } else {
            if (!c_ptr->pl && !c_ptr->tl) {
                // set pl so that lite_spot will work
                c_ptr->pl = true;

                if (c_ptr->fval == LIGHT_FLOOR) {
                    if (panel_contains(y, x)) {
                        light_room(y, x);
                    }
                } else {
                    lite_spot(y, x);
                }
            }

            // set pl in case tl was true above
            c_ptr->pl = true;

            if (c_ptr->cptr > 1) {
                monster_type *m_ptr = &m_list[c_ptr->cptr];
                creature_type *r_ptr = &c_list[m_ptr->mptr];

                // light up and draw monster
                update_mon((int) c_ptr->cptr);

                vtype m_name;
                monster_name(m_name, m_ptr->ml, r_ptr->name);

                if (CD_LIGHT & r_ptr->cdefense) {
                    if (m_ptr->ml) {
                        c_recall[m_ptr->mptr].r_cdefense |= CD_LIGHT;
                    }

                    vtype out_val;

                    int i = mon_take_hit((int) c_ptr->cptr, damroll(2, 8));
                    if (i >= 0) {
                        (void) sprintf(out_val, "%s shrivels away in the light!", m_name);
                        msg_print(out_val);
                        prt_experience();
                    } else {
                        (void) sprintf(out_val, "%s cringes from the light!", m_name);
                        msg_print(out_val);
                    }
                }
            }
        }
        (void) mmove(dir, &y, &x);
    }
}

// Light line in all directions -RAK-
void starlite(int y, int x) {
    if (py.flags.blind < 1) {
        msg_print("The end of the staff bursts into a blue shimmering light.");
    }

    for (int i = 1; i <= 9; i++) {
        if (i != 5) {
            light_line(i, y, x);
        }
    }
}

// Disarms all traps/chests in a given direction -RAK-
bool disarm_all(int dir, int y, int x) {
    bool disarmed = false;

    int dist = -1;

    cave_type *c_ptr;

    do {
        // put mmove at end, in case standing on a trap
        dist++;

        c_ptr = &cave[y][x];

        // note, must continue up to and including the first non open space,
        // because secret doors have fval greater than MAX_OPEN_SPACE
        if (c_ptr->tptr != 0) {
            inven_type *t_ptr = &t_list[c_ptr->tptr];

            if (t_ptr->tval == TV_INVIS_TRAP || t_ptr->tval == TV_VIS_TRAP) {
                if (delete_object(y, x)) {
                    disarmed = true;
                }
            } else if (t_ptr->tval == TV_CLOSED_DOOR) {
                // Locked or jammed doors become merely closed.
                t_ptr->p1 = 0;
            } else if (t_ptr->tval == TV_SECRET_DOOR) {
                c_ptr->fm = true;
                change_trap(y, x);

                disarmed = true;
            } else if (t_ptr->tval == TV_CHEST && t_ptr->flags != 0) {
                disarmed = true;

                msg_print("Click!");
                t_ptr->flags &= ~(CH_TRAPPED | CH_LOCKED);
                t_ptr->name2 = SN_UNLOCKED;
                known2(t_ptr);
            }
        }
        (void) mmove(dir, &y, &x);
    } while ((dist <= OBJ_BOLT_RANGE) && c_ptr->fval <= MAX_OPEN_SPACE);

    return disarmed;
}

// Return flags for given type area affect -RAK-
void get_flags(int typ, uint32_t *weapon_type, int *harm_type, bool (**destroy)(inven_type *)) {
    switch (typ) {
        case GF_MAGIC_MISSILE:
            *weapon_type = 0;
            *harm_type = 0;
            *destroy = set_null;
            break;
        case GF_LIGHTNING:
            *weapon_type = CS_BR_LIGHT;
            *harm_type = CD_LIGHT;
            *destroy = set_lightning_destroy;
            break;
        case GF_POISON_GAS:
            *weapon_type = CS_BR_GAS;
            *harm_type = CD_POISON;
            *destroy = set_null;
            break;
        case GF_ACID:
            *weapon_type = CS_BR_ACID;
            *harm_type = CD_ACID;
            *destroy = set_acid_destroy;
            break;
        case GF_FROST:
            *weapon_type = CS_BR_FROST;
            *harm_type = CD_FROST;
            *destroy = set_frost_destroy;
            break;
        case GF_FIRE:
            *weapon_type = CS_BR_FIRE;
            *harm_type = CD_FIRE;
            *destroy = set_fire_destroy;
            break;
        case GF_HOLY_ORB:
            *weapon_type = 0;
            *harm_type = CD_EVIL;
            *destroy = set_null;
            break;
        default:
            msg_print("ERROR in get_flags()\n");
    }
}

// Shoot a bolt in a given direction -RAK-
void fire_bolt(int typ, int dir, int y, int x, int dam, char *bolt_typ) {
    bool (*dummy)(inven_type *);
    int harm_type = 0;
    uint32_t weapon_type;
    get_flags(typ, &weapon_type, &harm_type, &dummy);

    int oldy = y;
    int oldx = x;
    int dist = 0;

    bool finished = false;
    while (!finished) {
        (void) mmove(dir, &y, &x);
        dist++;

        cave_type *c_ptr = &cave[y][x];

        lite_spot(oldy, oldx);
        if (dist > OBJ_BOLT_RANGE || c_ptr->fval >= MIN_CLOSED_SPACE) {
            finished = true;
        } else {
            if (c_ptr->cptr > 1) {
                finished = true;

                monster_type *m_ptr = &m_list[c_ptr->cptr];
                creature_type *r_ptr = &c_list[m_ptr->mptr];

                // light up monster and draw monster, temporarily set
                // pl so that update_mon() will work
                int i = c_ptr->pl;
                c_ptr->pl = true;
                update_mon((int) c_ptr->cptr);
                c_ptr->pl = i;

                // draw monster and clear previous bolt
                put_qio();

                vtype out_val, m_name;

                lower_monster_name(m_name, m_ptr->ml, r_ptr->name);
                (void) sprintf(out_val, "The %s strikes %s.", bolt_typ, m_name);
                msg_print(out_val);

                if (harm_type & r_ptr->cdefense) {
                    dam = dam * 2;
                    if (m_ptr->ml) {
                        c_recall[m_ptr->mptr].r_cdefense |= harm_type;
                    }
                } else if (weapon_type & r_ptr->spells) {
                    dam = dam / 4;
                    if (m_ptr->ml) {
                        c_recall[m_ptr->mptr].r_spells |= weapon_type;
                    }
                }

                monster_name(m_name, m_ptr->ml, r_ptr->name);
                i = mon_take_hit((int) c_ptr->cptr, dam);

                if (i >= 0) {
                    (void) sprintf(out_val, "%s dies in a fit of agony.", m_name);
                    msg_print(out_val);
                    prt_experience();
                } else if (dam > 0) {
                    (void) sprintf(out_val, "%s screams in agony.", m_name);
                    msg_print(out_val);
                }
            } else if (panel_contains(y, x) && py.flags.blind < 1) {
                print('*', y, x);

                // show the bolt
                put_qio();
            }
        }
        oldy = y;
        oldx = x;
    }
}

// Shoot a ball in a given direction.  Note that balls have an area affect. -RAK-
void fire_ball(int typ, int dir, int y, int x, int dam_hp, const char *descrip) {
    int thit = 0;
    int tkill = 0;
    int max_dis = 2;

    bool (*destroy)(inven_type *);
    int harm_type;
    uint32_t weapon_type;
    get_flags(typ, &weapon_type, &harm_type, &destroy);

    int oldy = y;
    int oldx = x;
    int dist = 0;

    bool finished = false;
    while (!finished) {
        (void) mmove(dir, &y, &x);
        dist++;

        lite_spot(oldy, oldx);

        if (dist > OBJ_BOLT_RANGE) {
            finished = true;
            continue;
        }

        cave_type *c_ptr = &cave[y][x];

        if (c_ptr->fval >= MIN_CLOSED_SPACE || c_ptr->cptr > 1) {
            finished = true;

            if (c_ptr->fval >= MIN_CLOSED_SPACE) {
                y = oldy;
                x = oldx;
            }

            // The ball hits and explodes.

            // The explosion.
            for (int i = y - max_dis; i <= y + max_dis; i++) {
                for (int j = x - max_dis; j <= x + max_dis; j++) {
                    if (in_bounds(i, j) && distance(y, x, i, j) <= max_dis && los(y, x, i, j)) {
                        c_ptr = &cave[i][j];

                        if (c_ptr->tptr != 0 && (*destroy)(&t_list[c_ptr->tptr])) {
                            (void) delete_object(i, j);
                        }

                        if (c_ptr->fval <= MAX_OPEN_SPACE) {
                            if (c_ptr->cptr > 1) {
                                monster_type *m_ptr = &m_list[c_ptr->cptr];
                                creature_type *r_ptr = &c_list[m_ptr->mptr];

                                // lite up creature if visible, temp set pl so that update_mon works
                                int tmp = c_ptr->pl;
                                c_ptr->pl = true;
                                update_mon((int) c_ptr->cptr);

                                thit++;
                                int dam = dam_hp;

                                if (harm_type & r_ptr->cdefense) {
                                    dam = dam * 2;
                                    if (m_ptr->ml) {
                                        c_recall[m_ptr->mptr].r_cdefense |= harm_type;
                                    }
                                } else if (weapon_type & r_ptr->spells) {
                                    dam = dam / 4;
                                    if (m_ptr->ml) {
                                        c_recall[m_ptr->mptr].r_spells |= weapon_type;
                                    }
                                }

                                dam = (dam / (distance(i, j, y, x) + 1));
                                int k = mon_take_hit((int) c_ptr->cptr, dam);

                                if (k >= 0) {
                                    tkill++;
                                }
                                c_ptr->pl = tmp;
                            } else if (panel_contains(i, j) && py.flags.blind < 1) {
                                print('*', i, j);
                            }
                        }
                    }
                }
            }

            // show ball of whatever
            put_qio();

            for (int i = (y - 2); i <= (y + 2); i++) {
                for (int j = (x - 2); j <= (x + 2); j++) {
                    if (in_bounds(i, j) && panel_contains(i, j) && distance(y, x, i, j) <= max_dis) {
                        lite_spot(i, j);
                    }
                }
            }
            // End  explosion.

            if (thit == 1) {
                vtype out_val;
                (void) sprintf(out_val, "The %s envelops a creature!", descrip);
                msg_print(out_val);
            } else if (thit > 1) {
                vtype out_val;
                (void) sprintf(out_val, "The %s envelops several creatures!", descrip);
                msg_print(out_val);
            }

            if (tkill == 1) {
                msg_print("There is a scream of agony!");
            } else if (tkill > 1) {
                msg_print("There are several screams of agony!");
            }

            if (tkill >= 0) {
                prt_experience();
            }
            // End ball hitting.
        } else if (panel_contains(y, x) && py.flags.blind < 1) {
            print('*', y, x);

            // show bolt
            put_qio();
        }
        oldy = y;
        oldx = x;
    }
}

// Breath weapon works like a fire_ball, but affects the player.
// Note the area affect. -RAK-
void breath(int typ, int y, int x, int dam_hp, char *ddesc, int monptr) {
    int max_dis = 2;

    bool (*destroy)(inven_type *);
    int harm_type;
    uint32_t weapon_type;
    get_flags(typ, &weapon_type, &harm_type, &destroy);

    int dam;

    for (int i = y - 2; i <= y + 2; i++) {
        for (int j = x - 2; j <= x + 2; j++) {
            if (in_bounds(i, j) && distance(y, x, i, j) <= max_dis && los(y, x, i, j)) {
                cave_type *c_ptr = &cave[i][j];

                if (c_ptr->tptr != 0 && (*destroy)(&t_list[c_ptr->tptr])) {
                    (void) delete_object(i, j);
                }

                if (c_ptr->fval <= MAX_OPEN_SPACE) {
                    // must test status bit, not py.flags.blind here, flag could have
                    // been set by a previous monster, but the breath should still
                    // be visible until the blindness takes effect
                    if (panel_contains(i, j) && !(py.flags.status & PY_BLIND)) {
                        print('*', i, j);
                    }

                    if (c_ptr->cptr > 1) {
                        monster_type *m_ptr = &m_list[c_ptr->cptr];
                        creature_type *r_ptr = &c_list[m_ptr->mptr];

                        dam = dam_hp;

                        if (harm_type & r_ptr->cdefense) {
                            dam = dam * 2;
                        } else if (weapon_type & r_ptr->spells) {
                            dam = (dam / 4);
                        }

                        dam = (dam / (distance(i, j, y, x) + 1));

                        // can not call mon_take_hit here, since player does not
                        // get experience for kill
                        m_ptr->hp = (int16_t) (m_ptr->hp - dam);
                        m_ptr->csleep = 0;

                        if (m_ptr->hp < 0) {
                            uint32_t treas = monster_death((int) m_ptr->fy, (int) m_ptr->fx, r_ptr->cmove);

                            if (m_ptr->ml) {
                                uint32_t tmp = (uint32_t) ((c_recall[m_ptr->mptr].r_cmove & CM_TREASURE) >> CM_TR_SHIFT);
                                if (tmp > ((treas & CM_TREASURE) >> CM_TR_SHIFT)) {
                                    treas = (uint32_t) ((treas & ~CM_TREASURE) | (tmp << CM_TR_SHIFT));
                                }
                                c_recall[m_ptr->mptr].r_cmove = (uint32_t) (treas | (c_recall[m_ptr->mptr].r_cmove & ~CM_TREASURE));
                            }

                            // It ate an already processed monster.Handle normally.
                            if (monptr < c_ptr->cptr) {
                                delete_monster((int) c_ptr->cptr);
                            } else {
                                // If it eats this monster, an already processed monster
                                // will take its place, causing all kinds of havoc.
                                // Delay the kill a bit.
                                fix1_delete_monster((int) c_ptr->cptr);
                            }
                        }
                    } else if (c_ptr->cptr == 1) {
                        dam = (dam_hp / (distance(i, j, y, x) + 1));

                        // let's do at least one point of damage
                        // prevents randint(0) problem with poison_gas, also
                        if (dam == 0) {
                            dam = 1;
                        }

                        switch (typ) {
                            case GF_LIGHTNING:
                                light_dam(dam, ddesc);
                                break;
                            case GF_POISON_GAS:
                                poison_gas(dam, ddesc);
                                break;
                            case GF_ACID:
                                acid_dam(dam, ddesc);
                                break;
                            case GF_FROST:
                                cold_dam(dam, ddesc);
                                break;
                            case GF_FIRE:
                                fire_dam(dam, ddesc);
                                break;
                        }
                    }
                }
            }
        }
    }

    // show the ball of gas
    put_qio();

    for (int i = (y - 2); i <= (y + 2); i++) {
        for (int j = (x - 2); j <= (x + 2); j++) {
            if (in_bounds(i, j) && panel_contains(i, j) && distance(y, x, i, j) <= max_dis) {
                lite_spot(i, j);
            }
        }
    }
}

// Recharge a wand, staff, or rod.  Sometimes the item breaks. -RAK-
bool recharge(int num) {
    int i, j;
    if (!find_range(TV_STAFF, TV_WAND, &i, &j)) {
        msg_print("You have nothing to recharge.");
        return false;
    }

    int item_val;
    if (!get_item(&item_val, "Recharge which item?", i, j, CNIL, CNIL)) {
        return false;
    }

    inven_type *i_ptr = &inventory[item_val];

    // recharge  I = recharge(20) = 1/6  failure for empty 10th level wand
    // recharge II = recharge(60) = 1/10 failure for empty 10th level wand
    //
    // make it harder to recharge high level, and highly charged wands, note
    // that i can be negative, so check its value before trying to call randint().
    i = num + 50 - (int) i_ptr->level - i_ptr->p1;
    if (i < 19) {
        // Automatic failure.
        i = 1;
    } else {
        i = randint(i / 10);
    }

    if (i == 1) {
        msg_print("There is a bright flash of light.");
        inven_destroy(item_val);
    } else {
        num = (num / (i_ptr->level + 2)) + 1;
        i_ptr->p1 += 2 + randint(num);
        if (known2_p(i_ptr)) {
            clear_known2(i_ptr);
        }
        clear_empty(i_ptr);
    }

    return true;
}

// Increase or decrease a creatures hit points -RAK-
bool hp_monster(int dir, int y, int x, int dam) {
    bool changed = false;

    int dist = 0;

    bool finished = false;
    while (!finished) {
        (void) mmove(dir, &y, &x);
        dist++;

        cave_type *c_ptr = &cave[y][x];

        if (dist > OBJ_BOLT_RANGE || c_ptr->fval >= MIN_CLOSED_SPACE) {
            finished = true;
        } else if (c_ptr->cptr > 1) {
            finished = true;

            monster_type *m_ptr = &m_list[c_ptr->cptr];
            creature_type *r_ptr = &c_list[m_ptr->mptr];

            vtype m_name;
            monster_name(m_name, m_ptr->ml, r_ptr->name);

            if (mon_take_hit((int) c_ptr->cptr, dam) >= 0) {
                vtype out_val;
                (void) sprintf(out_val, "%s dies in a fit of agony.", m_name);
                msg_print(out_val);
                prt_experience();
            } else if (dam > 0) {
                vtype out_val;
                (void) sprintf(out_val, "%s screams in agony.", m_name);
                msg_print(out_val);
            }

            changed = true;
        }
    }

    return changed;
}

// Drains life; note it must be living. -RAK-
bool drain_life(int dir, int y, int x) {
    bool drained = false;

    int dist = 0;

    bool finished = false;
    while (!finished) {
        (void) mmove(dir, &y, &x);
        dist++;

        cave_type *c_ptr = &cave[y][x];

        if (dist > OBJ_BOLT_RANGE || c_ptr->fval >= MIN_CLOSED_SPACE) {
            finished = true;
        } else if (c_ptr->cptr > 1) {
            finished = true;

            monster_type *m_ptr = &m_list[c_ptr->cptr];
            creature_type *r_ptr = &c_list[m_ptr->mptr];

            if ((r_ptr->cdefense & CD_UNDEAD) == 0) {
                vtype m_name;
                monster_name(m_name, m_ptr->ml, r_ptr->name);

                if (mon_take_hit((int) c_ptr->cptr, 75) >= 0) {
                    vtype out_val;
                    (void) sprintf(out_val, "%s dies in a fit of agony.", m_name);
                    msg_print(out_val);
                    prt_experience();
                } else {
                    vtype out_val;
                    (void) sprintf(out_val, "%s screams in agony.", m_name);
                    msg_print(out_val);
                }

                drained = true;
            } else {
                c_recall[m_ptr->mptr].r_cdefense |= CD_UNDEAD;
            }
        }
    }

    return drained;
}

// Increase or decrease a creatures speed -RAK-
// NOTE: cannot slow a winning creature (BALROG)
bool speed_monster(int dir, int y, int x, int spd) {
    bool changed = false;

    int dist = 0;

    bool finished = false;
    while (!finished) {
        (void) mmove(dir, &y, &x);
        dist++;

        cave_type *c_ptr = &cave[y][x];

        if (dist > OBJ_BOLT_RANGE || c_ptr->fval >= MIN_CLOSED_SPACE) {
            finished = true;
        } else if (c_ptr->cptr > 1) {
            finished = true;

            monster_type *m_ptr = &m_list[c_ptr->cptr];
            creature_type *r_ptr = &c_list[m_ptr->mptr];

            vtype m_name;
            monster_name(m_name, m_ptr->ml, r_ptr->name);

            if (spd > 0) {
                m_ptr->cspeed += spd;
                m_ptr->csleep = 0;

                changed = true;

                vtype out_val;
                (void) sprintf(out_val, "%s starts moving faster.", m_name);
                msg_print(out_val);
            } else if (randint(MAX_MONS_LEVEL) > r_ptr->level) {
                m_ptr->cspeed += spd;
                m_ptr->csleep = 0;

                changed = true;

                vtype out_val;
                (void) sprintf(out_val, "%s starts moving slower.", m_name);
                msg_print(out_val);
            } else {
                m_ptr->csleep = 0;

                vtype out_val;
                (void) sprintf(out_val, "%s is unaffected.", m_name);
                msg_print(out_val);
            }
        }
    }

    return changed;
}

// Confuse a creature -RAK-
bool confuse_monster(int dir, int y, int x) {
    bool confused = false;

    int dist = 0;

    bool finished = false;
    while (!finished) {
        (void) mmove(dir, &y, &x);
        dist++;

        cave_type *c_ptr = &cave[y][x];

        if (dist > OBJ_BOLT_RANGE || c_ptr->fval >= MIN_CLOSED_SPACE) {
            finished = true;
        } else if (c_ptr->cptr > 1) {
            finished = true;

            monster_type *m_ptr = &m_list[c_ptr->cptr];
            creature_type *r_ptr = &c_list[m_ptr->mptr];

            vtype m_name;
            monster_name(m_name, m_ptr->ml, r_ptr->name);

            if (randint(MAX_MONS_LEVEL) < r_ptr->level || (CD_NO_SLEEP & r_ptr->cdefense)) {
                if (m_ptr->ml && (r_ptr->cdefense & CD_NO_SLEEP)) {
                    c_recall[m_ptr->mptr].r_cdefense |= CD_NO_SLEEP;
                }

                // Monsters which resisted the attack should wake up.
                // Monsters with innate resistance ignore the attack.
                if (!(CD_NO_SLEEP & r_ptr->cdefense)) {
                    m_ptr->csleep = 0;
                }

                vtype out_val;
                (void) sprintf(out_val, "%s is unaffected.", m_name);
                msg_print(out_val);
            } else {
                if (m_ptr->confused) {
                    m_ptr->confused += 3;
                } else {
                    m_ptr->confused = (uint8_t) (2 + randint(16));
                }
                m_ptr->csleep = 0;

                confused = true;

                vtype out_val;
                (void) sprintf(out_val, "%s appears confused.", m_name);
                msg_print(out_val);
            }
        }
    }

    return confused;
}

// Sleep a creature. -RAK-
bool sleep_monster(int dir, int y, int x) {
    bool asleep = false;

    int dist = 0;

    bool finished = false;
    while (!finished) {
        (void) mmove(dir, &y, &x);
        dist++;

        cave_type *c_ptr = &cave[y][x];

        if (dist > OBJ_BOLT_RANGE || c_ptr->fval >= MIN_CLOSED_SPACE) {
            finished = true;
        } else if (c_ptr->cptr > 1) {
            finished = true;

            monster_type *m_ptr = &m_list[c_ptr->cptr];
            creature_type *r_ptr = &c_list[m_ptr->mptr];

            vtype m_name;
            monster_name(m_name, m_ptr->ml, r_ptr->name);

            if (randint(MAX_MONS_LEVEL) < r_ptr->level || (CD_NO_SLEEP & r_ptr->cdefense)) {
                if (m_ptr->ml && (r_ptr->cdefense & CD_NO_SLEEP)) {
                    c_recall[m_ptr->mptr].r_cdefense |= CD_NO_SLEEP;
                }

                vtype out_val;
                (void) sprintf(out_val, "%s is unaffected.", m_name);
                msg_print(out_val);
            } else {
                m_ptr->csleep = 500;

                asleep = true;

                vtype out_val;
                (void) sprintf(out_val, "%s falls asleep.", m_name);
                msg_print(out_val);
            }
        }
    }

    return asleep;
}

// Turn stone to mud, delete wall. -RAK-
bool wall_to_mud(int dir, int y, int x) {
    bool turned = false;

    int dist = 0;

    bool finished = false;
    while (!finished) {
        (void) mmove(dir, &y, &x);
        dist++;

        cave_type *c_ptr = &cave[y][x];

        // note, this ray can move through walls as it turns them to mud
        if (dist == OBJ_BOLT_RANGE) {
            finished = true;
        }

        if (c_ptr->fval >= MIN_CAVE_WALL && c_ptr->fval != BOUNDARY_WALL) {
            finished = true;

            (void) twall(y, x, 1, 0);

            if (test_light(y, x)) {
                turned = true;

                msg_print("The wall turns into mud.");
            }
        } else if (c_ptr->tptr != 0 && c_ptr->fval >= MIN_CLOSED_SPACE) {
            finished = true;

            if (panel_contains(y, x) && test_light(y, x)) {
                turned = true;

                bigvtype tmp_str;
                objdes(tmp_str, &t_list[c_ptr->tptr], false);

                bigvtype out_val;
                (void) sprintf(out_val, "The %s turns into mud.", tmp_str);
                msg_print(out_val);
            }

            if (t_list[c_ptr->tptr].tval == TV_RUBBLE) {
                (void) delete_object(y, x);
                if (randint(10) == 1) {
                    place_object(y, x, false);
                    if (test_light(y, x)) {
                        msg_print("You have found something!");
                    }
                }
                lite_spot(y, x);
            } else {
                (void) delete_object(y, x);
            }
        }

        if (c_ptr->cptr > 1) {
            monster_type *m_ptr = &m_list[c_ptr->cptr];
            creature_type *r_ptr = &c_list[m_ptr->mptr];

            if (CD_STONE & r_ptr->cdefense) {
                vtype m_name;
                monster_name(m_name, m_ptr->ml, r_ptr->name);

                // Should get these messages even if the monster is not visible.
                int i = mon_take_hit((int) c_ptr->cptr, 100);
                if (i >= 0) {
                    c_recall[i].r_cdefense |= CD_STONE;

                    bigvtype out_val;
                    (void) sprintf(out_val, "%s dissolves!", m_name);
                    msg_print(out_val);

                    prt_experience(); // print msg before calling prt_exp
                } else {
                    c_recall[m_ptr->mptr].r_cdefense |= CD_STONE;

                    bigvtype out_val;
                    (void) sprintf(out_val, "%s grunts in pain!", m_name);
                    msg_print(out_val);
                }
                finished = true;
            }
        }
    }

    return turned;
}

// Destroy all traps and doors in a given direction -RAK-
bool td_destroy2(int dir, int y, int x) {
    bool destroyed = false;

    int dist = 0;

    cave_type *c_ptr;

    do {
        (void) mmove(dir, &y, &x);
        dist++;

        c_ptr = &cave[y][x];

        // must move into first closed spot, as it might be a secret door
        if (c_ptr->tptr != 0) {
            inven_type *t_ptr = &t_list[c_ptr->tptr];

            if (t_ptr->tval == TV_INVIS_TRAP ||
                t_ptr->tval == TV_CLOSED_DOOR ||
                t_ptr->tval == TV_VIS_TRAP ||
                t_ptr->tval == TV_OPEN_DOOR ||
                t_ptr->tval == TV_SECRET_DOOR) {
                if (delete_object(y, x)) {
                    destroyed = true;

                    msg_print("There is a bright flash of light!");
                }
            } else if (t_ptr->tval == TV_CHEST && t_ptr->flags != 0) {
                destroyed = true;

                msg_print("Click!");
                t_ptr->flags &= ~(CH_TRAPPED | CH_LOCKED);
                t_ptr->name2 = SN_UNLOCKED;
                known2(t_ptr);
            }
        }
    } while ((dist <= OBJ_BOLT_RANGE) || c_ptr->fval <= MAX_OPEN_SPACE);

    return destroyed;
}

// Polymorph a monster -RAK-
// NOTE: cannot polymorph a winning creature (BALROG)
bool poly_monster(int dir, int y, int x) {
    bool morphed = false;

    int dist = 0;

    bool finished = false;
    while (!finished) {
        (void) mmove(dir, &y, &x);
        dist++;

        cave_type *c_ptr = &cave[y][x];

        if (dist > OBJ_BOLT_RANGE || c_ptr->fval >= MIN_CLOSED_SPACE) {
            finished = true;
        } else if (c_ptr->cptr > 1) {
            monster_type *m_ptr = &m_list[c_ptr->cptr];
            creature_type *r_ptr = &c_list[m_ptr->mptr];

            if (randint(MAX_MONS_LEVEL) > r_ptr->level) {
                finished = true;

                delete_monster((int) c_ptr->cptr);

                // Place_monster() should always return true here.
                morphed = place_monster(y, x, randint(m_level[MAX_MONS_LEVEL] - m_level[0]) - 1 + m_level[0], false);

                // don't test c_ptr->fm here, only pl/tl
                if (morphed && panel_contains(y, x) && (c_ptr->tl || c_ptr->pl)) {
                    morphed = true;
                }
            } else {
                vtype out_val, m_name;
                monster_name(m_name, m_ptr->ml, r_ptr->name);
                (void) sprintf(out_val, "%s is unaffected.", m_name);

                msg_print(out_val);
            }
        }
    }

    return morphed;
}

// Create a wall. -RAK-
bool build_wall(int dir, int y, int x) {
    bool built = false;

    int dist = 0;

    bool finished = false;
    while (!finished) {
        (void) mmove(dir, &y, &x);
        dist++;

        cave_type *c_ptr = &cave[y][x];

        if (dist > OBJ_BOLT_RANGE || c_ptr->fval >= MIN_CLOSED_SPACE) {
            finished = true;
        } else {
            if (c_ptr->tptr != 0) {
                (void) delete_object(y, x);
            }

            if (c_ptr->cptr > 1) {
                finished = true;

                monster_type *m_ptr = &m_list[c_ptr->cptr];
                creature_type *r_ptr = &c_list[m_ptr->mptr];

                if (!(r_ptr->cmove & CM_PHASE)) {
                    // monster does not move, can't escape the wall
                    int damage;
                    if (r_ptr->cmove & CM_ATTACK_ONLY) {
                        // this will kill everything
                        damage = 3000;
                    } else {
                        damage = damroll(4, 8);
                    }

                    vtype m_name, out_val;
                    monster_name(m_name, m_ptr->ml, r_ptr->name);
                    (void) sprintf(out_val, "%s wails out in pain!", m_name);
                    msg_print(out_val);

                    if (mon_take_hit((int) c_ptr->cptr, damage) >= 0) {
                        (void) sprintf(out_val, "%s is embedded in the rock.", m_name);
                        msg_print(out_val);
                        prt_experience();
                    }
                } else if (r_ptr->cchar == 'E' || r_ptr->cchar == 'X') {
                    // must be an earth elemental or an earth spirit, or a Xorn
                    // increase its hit points
                    m_ptr->hp += damroll(4, 8);
                }
            }

            c_ptr->fval = MAGMA_WALL;
            c_ptr->fm = false;

            // Permanently light this wall if it is lit by player's lamp.
            c_ptr->pl = (c_ptr->tl || c_ptr->pl);
            lite_spot(y, x);

            // NOTE: this is never used anywhere. Is that a bug or just obsolete code?
            // i++;

            built = true;
        }
    }

    return built;
}

// Replicate a creature -RAK-
bool clone_monster(int dir, int y, int x) {
    int dist = 0;

    bool finished = false;
    while (!finished) {
        (void) mmove(dir, &y, &x);
        dist++;

        cave_type *c_ptr = &cave[y][x];

        if (dist > OBJ_BOLT_RANGE || c_ptr->fval >= MIN_CLOSED_SPACE) {
            finished = true;
        } else if (c_ptr->cptr > 1) {
            m_list[c_ptr->cptr].csleep = 0;

            // monptr of 0 is safe here, since can't reach here from creatures
            return multiply_monster(y, x, (int) m_list[c_ptr->cptr].mptr, 0);
        }
    }

    return false;
}

// Move the creature record to a new location -RAK-
void teleport_away(int monptr, int dis) {
    int yn, xn;

    monster_type *m_ptr = &m_list[monptr];

    int ctr = 0;
    do {
        do {
            yn = m_ptr->fy + (randint(2 * dis + 1) - (dis + 1));
            xn = m_ptr->fx + (randint(2 * dis + 1) - (dis + 1));
        } while (!in_bounds(yn, xn));

        ctr++;
        if (ctr > 9) {
            ctr = 0;
            dis += 5;
        }
    } while ((cave[yn][xn].fval >= MIN_CLOSED_SPACE) || (cave[yn][xn].cptr != 0));

    move_rec((int) m_ptr->fy, (int) m_ptr->fx, yn, xn);
    lite_spot((int) m_ptr->fy, (int) m_ptr->fx);
    m_ptr->fy = (uint8_t) yn;
    m_ptr->fx = (uint8_t) xn;

    // this is necessary, because the creature is
    // not currently visible in its new position.
    m_ptr->ml = false;
    m_ptr->cdis = (uint8_t) distance(char_row, char_col, yn, xn);
    update_mon(monptr);
}

// Teleport player to spell casting creature -RAK-
void teleport_to(int ny, int nx) {
    int y, x;

    int dis = 1;
    int ctr = 0;
    do {
        y = ny + (randint(2 * dis + 1) - (dis + 1));
        x = nx + (randint(2 * dis + 1) - (dis + 1));
        ctr++;
        if (ctr > 9) {
            ctr = 0;
            dis++;
        }
    } while (!in_bounds(y, x) || (cave[y][x].fval >= MIN_CLOSED_SPACE) || (cave[y][x].cptr >= 2));

    move_rec(char_row, char_col, y, x);

    for (int i = char_row - 1; i <= char_row + 1; i++) {
        for (int j = char_col - 1; j <= char_col + 1; j++) {
            cave_type *c_ptr = &cave[i][j];
            c_ptr->tl = false;
            lite_spot(i, j);
        }
    }

    lite_spot(char_row, char_col);

    char_row = (int16_t) y;
    char_col = (int16_t) x;
    check_view();

    // light creatures
    creatures(false);
}

// Teleport all creatures in a given direction away -RAK-
bool teleport_monster(int dir, int y, int x) {
    bool teleported = false;

    int dist = 0;

    bool finished = false;
    while (!finished) {
        (void) mmove(dir, &y, &x);
        dist++;

        cave_type *c_ptr = &cave[y][x];

        if (dist > OBJ_BOLT_RANGE || c_ptr->fval >= MIN_CLOSED_SPACE) {
            finished = true;
        } else if (c_ptr->cptr > 1) {
            // wake it up
            m_list[c_ptr->cptr].csleep = 0;

            teleport_away((int) c_ptr->cptr, MAX_SIGHT);

            teleported = true;
        }
    }

    return teleported;
}

// Delete all creatures within max_sight distance -RAK-
// NOTE : Winning creatures cannot be killed by genocide.
bool mass_genocide() {
    bool killed = false;

    for (int i = mfptr - 1; i >= MIN_MONIX; i--) {
        monster_type *m_ptr = &m_list[i];
        creature_type *r_ptr = &c_list[m_ptr->mptr];

        if (m_ptr->cdis <= MAX_SIGHT && (r_ptr->cmove & CM_WIN) == 0) {
            delete_monster(i);

            killed = true;
        }
    }

    return killed;
}

// Delete all creatures of a given type from level. -RAK-
// This does not keep creatures of type from appearing later.
// NOTE : Winning creatures can not be killed by genocide.
bool genocide() {
    char typ;
    if (!get_com("Which type of creature do you wish exterminated?", &typ)) {
        return false;
    }

    bool killed = false;

    for (int i = mfptr - 1; i >= MIN_MONIX; i--) {
        monster_type *m_ptr = &m_list[i];
        creature_type *r_ptr = &c_list[m_ptr->mptr];

        if (typ == c_list[m_ptr->mptr].cchar) {
            if ((r_ptr->cmove & CM_WIN) == 0) {
                delete_monster(i);

                killed = true;
            } else {
                // genocide is a powerful spell, so we will let the player
                // know the names of the creatures he did not destroy,
                // this message makes no sense otherwise
                vtype out_val;
                (void) sprintf(out_val, "The %s is unaffected.", r_ptr->name);
                msg_print(out_val);
            }
        }
    }

    return killed;
}

// Change speed of any creature . -RAK-
// NOTE: cannot slow a winning creature (BALROG)
bool speed_monsters(int spd) {
    bool speedy = false;

    for (int i = mfptr - 1; i >= MIN_MONIX; i--) {
        monster_type *m_ptr = &m_list[i];
        creature_type *r_ptr = &c_list[m_ptr->mptr];

        vtype m_name;
        monster_name(m_name, m_ptr->ml, r_ptr->name);

        if (m_ptr->cdis > MAX_SIGHT || !los(char_row, char_col, (int) m_ptr->fy, (int) m_ptr->fx)) {
            continue; // do nothing
        } else if (spd > 0) {
            m_ptr->cspeed += spd;
            m_ptr->csleep = 0;

            if (m_ptr->ml) {
                speedy = true;

                vtype out_val;
                (void) sprintf(out_val, "%s starts moving faster.", m_name);
                msg_print(out_val);
            }
        } else if (randint(MAX_MONS_LEVEL) > r_ptr->level) {
            m_ptr->cspeed += spd;
            m_ptr->csleep = 0;

            if (m_ptr->ml) {
                speedy = true;

                vtype out_val;
                (void) sprintf(out_val, "%s starts moving slower.", m_name);
                msg_print(out_val);
            }
        } else if (m_ptr->ml) {
            m_ptr->csleep = 0;

            vtype out_val;
            (void) sprintf(out_val, "%s is unaffected.", m_name);
            msg_print(out_val);
        }
    }

    return speedy;
}

// Sleep any creature . -RAK-
bool sleep_monsters2() {
    bool asleep = false;

    for (int i = mfptr - 1; i >= MIN_MONIX; i--) {
        monster_type *m_ptr = &m_list[i];
        creature_type *r_ptr = &c_list[m_ptr->mptr];

        vtype m_name;
        monster_name(m_name, m_ptr->ml, r_ptr->name);

        if (m_ptr->cdis > MAX_SIGHT || !los(char_row, char_col, (int) m_ptr->fy, (int) m_ptr->fx)) {
            continue; // do nothing
        } else if (randint(MAX_MONS_LEVEL) < r_ptr->level || (CD_NO_SLEEP & r_ptr->cdefense)) {
            if (m_ptr->ml) {
                if (r_ptr->cdefense & CD_NO_SLEEP) {
                    c_recall[m_ptr->mptr].r_cdefense |= CD_NO_SLEEP;
                }

                vtype out_val;
                (void) sprintf(out_val, "%s is unaffected.", m_name);
                msg_print(out_val);
            }
        } else {
            m_ptr->csleep = 500;
            if (m_ptr->ml) {
                asleep = true;

                vtype out_val;
                (void) sprintf(out_val, "%s falls asleep.", m_name);
                msg_print(out_val);
            }
        }
    }

    return asleep;
}

// Polymorph any creature that player can see. -RAK-
// NOTE: cannot polymorph a winning creature (BALROG)
bool mass_poly() {
    bool morphed = false;

    for (int i = mfptr - 1; i >= MIN_MONIX; i--) {
        monster_type *m_ptr = &m_list[i];

        if (m_ptr->cdis <= MAX_SIGHT) {
            creature_type *r_ptr = &c_list[m_ptr->mptr];

            if ((r_ptr->cmove & CM_WIN) == 0) {
                int y = m_ptr->fy;
                int x = m_ptr->fx;
                delete_monster(i);

                // Place_monster() should always return true here.
                morphed = place_monster(y, x, randint(m_level[MAX_MONS_LEVEL] - m_level[0]) - 1 + m_level[0], false);
            }
        }
    }

    return morphed;
}

// Display evil creatures on current panel -RAK-
bool detect_evil() {
    bool detected = false;

    for (int i = mfptr - 1; i >= MIN_MONIX; i--) {
        monster_type *m_ptr = &m_list[i];

        if (panel_contains((int) m_ptr->fy, (int) m_ptr->fx) && (CD_EVIL & c_list[m_ptr->mptr].cdefense)) {
            m_ptr->ml = true;

            detected = true;

            // works correctly even if hallucinating
            print((char) c_list[m_ptr->mptr].cchar, (int) m_ptr->fy, (int) m_ptr->fx);
        }
    }

    if (detected) {
        msg_print("You sense the presence of evil!");
        msg_print(CNIL);

        // must unlight every monster just lighted
        creatures(false);
    }

    return detected;
}

// Change players hit points in some manner -RAK-
bool hp_player(int num) {
    if (py.misc.chp >= py.misc.mhp) {
        return false;
    }

    py.misc.chp += num;
    if (py.misc.chp > py.misc.mhp) {
        py.misc.chp = py.misc.mhp;
        py.misc.chp_frac = 0;
    }
    prt_chp();

    num = num / 5;
    if (num < 3) {
        if (num == 0) {
            msg_print("You feel a little better.");
        } else {
            msg_print("You feel better.");
        }
    } else {
        if (num < 7) {
            msg_print("You feel much better.");
        } else {
            msg_print("You feel very good.");
        }
    }

    return true;
}

// Cure players confusion -RAK-
bool cure_confusion() {
    if (py.flags.confused > 1) {
        py.flags.confused = 1;
        return true;
    }
    return false;
}

// Cure players blindness -RAK-
bool cure_blindness() {
    if (py.flags.blind > 1) {
        py.flags.blind = 1;
        return true;
    }
    return false;
}

// Cure poisoning -RAK-
bool cure_poison() {
    if (py.flags.poisoned > 1) {
        py.flags.poisoned = 1;
        return true;
    }
    return false;
}

// Cure the players fear -RAK-
bool remove_fear() {
    if (py.flags.afraid > 1) {
        py.flags.afraid = 1;
        return true;
    }
    return false;
}

// This is a fun one.  In a given block, pick some walls and
// turn them into open spots.  Pick some open spots and turn
// them into walls.  An "Earthquake" effect. -RAK-
void earthquake() {
    for (int i = char_row - 8; i <= char_row + 8; i++) {
        for (int j = char_col - 8; j <= char_col + 8; j++) {
            if ((i != char_row || j != char_col) && in_bounds(i, j) && randint(8) == 1) {
                cave_type *c_ptr = &cave[i][j];

                if (c_ptr->tptr != 0) {
                    (void) delete_object(i, j);
                }

                if (c_ptr->cptr > 1) {
                    monster_type *m_ptr = &m_list[c_ptr->cptr];
                    creature_type *r_ptr = &c_list[m_ptr->mptr];

                    if (!(r_ptr->cmove & CM_PHASE)) {
                        int damage;
                        if (r_ptr->cmove & CM_ATTACK_ONLY) {
                            // this will kill everything
                            damage = 3000;
                        } else {
                            damage = damroll(4, 8);
                        }

                        vtype m_name;
                        monster_name(m_name, m_ptr->ml, r_ptr->name);

                        vtype out_val;
                        (void) sprintf(out_val, "%s wails out in pain!", m_name);
                        msg_print(out_val);

                        if (mon_take_hit((int) c_ptr->cptr, damage) >= 0) {
                            (void) sprintf(out_val, "%s is embedded in the rock.", m_name);
                            msg_print(out_val);
                            prt_experience();
                        }
                    } else if (r_ptr->cchar == 'E' || r_ptr->cchar == 'X') {
                        // must be an earth elemental or an earth spirit, or a
                        // Xorn increase its hit points
                        m_ptr->hp += damroll(4, 8);
                    }
                }

                if (c_ptr->fval >= MIN_CAVE_WALL && c_ptr->fval != BOUNDARY_WALL) {
                    c_ptr->fval = CORR_FLOOR;
                    c_ptr->pl = false;
                    c_ptr->fm = false;
                } else if (c_ptr->fval <= MAX_CAVE_FLOOR) {
                    int tmp = randint(10);

                    if (tmp < 6) {
                        c_ptr->fval = QUARTZ_WALL;
                    } else if (tmp < 9) {
                        c_ptr->fval = MAGMA_WALL;
                    } else {
                        c_ptr->fval = GRANITE_WALL;
                    }

                    c_ptr->fm = false;
                }
                lite_spot(i, j);
            }
        }
    }
}

// Evil creatures don't like this. -RAK-
bool protect_evil() {
    bool isProtected = py.flags.protevil == 0;

    py.flags.protevil += randint(25) + 3 * py.misc.lev;

    return isProtected;
}

// Create some high quality mush for the player. -RAK-
void create_food() {
    uint8_t treasureID = cave[char_row][char_col].tptr;

    // take no action here, don't want to destroy object under player
    if (treasureID != 0) {
        // set free_turn_flag so that scroll/spell points won't be used
        free_turn_flag = true;

        msg_print("There is already an object under you.");

        return;
    }

    place_object(char_row, char_col, false);
    invcopy(&t_list[treasureID], OBJ_MUSH);
}

// Attempts to destroy a type of creature.  Success depends on
// the creatures level VS. the player's level -RAK-
bool dispel_creature(int cflag, int damage) {
    bool dispelled = false;

    for (int i = mfptr - 1; i >= MIN_MONIX; i--) {
        monster_type *m_ptr = &m_list[i];

        if (m_ptr->cdis <= MAX_SIGHT && (cflag & c_list[m_ptr->mptr].cdefense) && los(char_row, char_col, (int) m_ptr->fy, (int) m_ptr->fx)) {
            creature_type *r_ptr = &c_list[m_ptr->mptr];

            c_recall[m_ptr->mptr].r_cdefense |= cflag;

            dispelled = true;

            vtype out_val, m_name;
            monster_name(m_name, m_ptr->ml, r_ptr->name);

            int k = mon_take_hit(i, randint(damage));

            // Should get these messages even if the monster is not visible.
            if (k >= 0) {
                (void) sprintf(out_val, "%s dissolves!", m_name);
            } else {
                (void) sprintf(out_val, "%s shudders.", m_name);
            }
            msg_print(out_val);

            if (k >= 0) {
                prt_experience();
            }
        }
    }

    return dispelled;
}

// Attempt to turn (confuse) undead creatures. -RAK-
bool turn_undead() {
    bool turned = false;

    for (int i = mfptr - 1; i >= MIN_MONIX; i--) {
        monster_type *m_ptr = &m_list[i];
        creature_type *r_ptr = &c_list[m_ptr->mptr];

        if (m_ptr->cdis <= MAX_SIGHT && (CD_UNDEAD & r_ptr->cdefense) && los(char_row, char_col, (int) m_ptr->fy, (int) m_ptr->fx)) {
            vtype m_name;
            monster_name(m_name, m_ptr->ml, r_ptr->name);

            if (py.misc.lev + 1 > r_ptr->level || randint(5) == 1) {
                if (m_ptr->ml) {
                    c_recall[m_ptr->mptr].r_cdefense |= CD_UNDEAD;

                    turned = true;

                    vtype out_val;
                    (void) sprintf(out_val, "%s runs frantically!", m_name);
                    msg_print(out_val);
                }

                m_ptr->confused = (uint8_t) py.misc.lev;
            } else if (m_ptr->ml) {
                vtype out_val;
                (void) sprintf(out_val, "%s is unaffected.", m_name);
                msg_print(out_val);
            }
        }
    }

    return turned;
}

// Leave a glyph of warding. Creatures will not pass over! -RAK-
void warding_glyph() {
    if (cave[char_row][char_col].tptr == 0) {
        int newID = popt();
        cave[char_row][char_col].tptr = (uint8_t) newID;
        invcopy(&t_list[newID], OBJ_SCARE_MON);
    }
}

// Lose a strength point. -RAK-
void lose_str() {
    if (!py.flags.sustain_str) {
        (void) dec_stat(A_STR);
        msg_print("You feel very sick.");
    } else {
        msg_print("You feel sick for a moment,  it passes.");
    }
}

// Lose an intelligence point. -RAK-
void lose_int() {
    if (!py.flags.sustain_int) {
        (void) dec_stat(A_INT);
        msg_print("You become very dizzy.");
    } else {
        msg_print("You become dizzy for a moment,  it passes.");
    }
}

// Lose a wisdom point. -RAK-
void lose_wis() {
    if (!py.flags.sustain_wis) {
        (void) dec_stat(A_WIS);
        msg_print("You feel very naive.");
    } else {
        msg_print("You feel naive for a moment,  it passes.");
    }
}

// Lose a dexterity point. -RAK-
void lose_dex() {
    if (!py.flags.sustain_dex) {
        (void) dec_stat(A_DEX);
        msg_print("You feel very sore.");
    } else {
        msg_print("You feel sore for a moment,  it passes.");
    }
}

// Lose a constitution point. -RAK-
void lose_con() {
    if (!py.flags.sustain_con) {
        (void) dec_stat(A_CON);
        msg_print("You feel very sick.");
    } else {
        msg_print("You feel sick for a moment,  it passes.");
    }
}

// Lose a charisma point. -RAK-
void lose_chr() {
    if (!py.flags.sustain_chr) {
        (void) dec_stat(A_CHR);
        msg_print("Your skin starts to itch.");
    } else {
        msg_print("Your skin starts to itch, but feels better now.");
    }
}

// Lose experience -RAK-
void lose_exp(int32_t amount) {
    if (amount > py.misc.exp) {
        py.misc.exp = 0;
    } else {
        py.misc.exp -= amount;
    }
    prt_experience();

    int exp = 0;
    while ((signed) (player_exp[exp] * py.misc.expfact / 100) <= py.misc.exp) {
        exp++;
    }

    // increment exp once more, because level 1 exp is stored in player_exp[0]
    exp++;

    if (py.misc.lev != exp) {
        py.misc.lev = (uint16_t) exp;

        calc_hitpoints();

        class_type *c_ptr = &classes[py.misc.pclass];

        if (c_ptr->spell == MAGE) {
            calc_spells(A_INT);
            calc_mana(A_INT);
        } else if (c_ptr->spell == PRIEST) {
            calc_spells(A_WIS);
            calc_mana(A_WIS);
        }
        prt_level();
        prt_title();
    }
}

// Slow Poison -RAK-
bool slow_poison() {
    if (py.flags.poisoned > 0) {
        py.flags.poisoned = (int16_t) (py.flags.poisoned / 2);
        if (py.flags.poisoned < 1) {
            py.flags.poisoned = 1;
        }
        msg_print("The effect of the poison has been reduced.");
        return true;
    }

    return false;
}

// Bless -RAK-
void bless(int amount) {
    py.flags.blessed += amount;
}

// Detect Invisible for period of time -RAK-
void detect_inv2(int amount) {
    py.flags.detect_inv += amount;
}

static void replace_spot(int y, int x, int typ) {
    cave_type *c_ptr = &cave[y][x];

    switch (typ) {
        case 1:
        case 2:
        case 3:
            c_ptr->fval = CORR_FLOOR;
            break;
        case 4:
        case 7:
        case 10:
            c_ptr->fval = GRANITE_WALL;
            break;
        case 5:
        case 8:
        case 11:
            c_ptr->fval = MAGMA_WALL;
            break;
        case 6:
        case 9:
        case 12:
            c_ptr->fval = QUARTZ_WALL;
            break;
    }

    c_ptr->pl = false;
    c_ptr->fm = false;
    c_ptr->lr = false; // this is no longer part of a room

    if (c_ptr->tptr != 0) {
        (void) delete_object(y, x);
    }

    if (c_ptr->cptr > 1) {
        delete_monster((int) c_ptr->cptr);
    }
}

// The spell of destruction. -RAK-
// NOTE : Winning creatures that are deleted will be considered
//        as teleporting to another level.  This will NOT win
//        the game.
void destroy_area(int y, int x) {
    if (dun_level > 0) {
        for (int i = (y - 15); i <= (y + 15); i++) {
            for (int j = (x - 15); j <= (x + 15); j++) {
                if (in_bounds(i, j) && cave[i][j].fval != BOUNDARY_WALL) {
                    int k = distance(i, j, y, x);

                    // clear player's spot, but don't put wall there
                    if (k == 0) {
                        replace_spot(i, j, 1);
                    } else if (k < 13) {
                        replace_spot(i, j, randint(6));
                    } else if (k < 16) {
                        replace_spot(i, j, randint(9));
                    }
                }
            }
        }
    }
    msg_print("There is a searing blast of light!");
    py.flags.blind += 10 + randint(10);
}

// Enchants a plus onto an item. -RAK-
// `limit` param is the maximum bonus allowed; usually 10,
// but weapon's maximum damage when enchanting melee weapons to damage.
bool enchant(int16_t *plusses, int16_t limit) {
    // avoid randint(0) call
    if (limit <= 0) {
        return false;
    }

    int chance = 0;

    if (*plusses > 0) {
        chance = *plusses;

        // very rarely allow enchantment over limit
        if (randint(100) == 1) {
            chance = randint(chance) - 1;
        }
    }

    if (randint(limit) > chance) {
        *plusses += 1;
        return true;
    }

    return false;
}

// Removes curses from items in inventory -RAK-
bool remove_curse() {
    bool removed = false;

    for (int i = INVEN_WIELD; i <= INVEN_OUTER; i++) {
        inven_type *i_ptr = &inventory[i];

        if (TR_CURSED & i_ptr->flags) {
            i_ptr->flags &= ~TR_CURSED;
            calc_bonuses();

            removed = true;
        }
    }

    return removed;
}

// Restores any drained experience -RAK-
bool restore_level() {
    if (py.misc.max_exp > py.misc.exp) {
        msg_print("You feel your life energies returning.");

        // this while loop is not redundant, ptr_exp may reduce the exp level
        while (py.misc.exp < py.misc.max_exp) {
            py.misc.exp = py.misc.max_exp;
            prt_experience();
        }

        return true;
    }

    return false;
}
