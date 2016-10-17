/* source/spells.c: player/creature spells, breaths, wands, scrolls, etc. code
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

static void replace_spot();

/* Following are spell procedure/functions      -RAK- */
/* These routines are commonly used in the scroll, potion, wands, and */
/* staves routines, and are occasionally called from other areas. */
/* Now included are creature spells also.           -RAK */

void monster_name(char *m_name, monster_type *m_ptr, creature_type *r_ptr) {
    if (!m_ptr->ml) {
        (void)strcpy(m_name, "It");
    } else {
        (void)sprintf(m_name, "The %s", r_ptr->name);
    }
}

void lower_monster_name(char *m_name, monster_type *m_ptr, creature_type *r_ptr) {
    if (!m_ptr->ml) {
        (void)strcpy(m_name, "it");
    } else {
        (void)sprintf(m_name, "the %s", r_ptr->name);
    }
}

/* Sleep creatures adjacent to player      -RAK- */
int sleep_monsters1(int y, int x) {
    bool sleep = false;

    for (int i = y - 1; i <= y + 1; i++) {
        for (int j = x - 1; j <= x + 1; j++) {
            cave_type *c_ptr = &cave[i][j];
            if (c_ptr->cptr > 1) {
                monster_type *m_ptr = &m_list[c_ptr->cptr];
                creature_type *r_ptr = &c_list[m_ptr->mptr];

                vtype m_name;
                monster_name(m_name, m_ptr, r_ptr);

                if ((randint(MAX_MONS_LEVEL) < r_ptr->level) || (CD_NO_SLEEP & r_ptr->cdefense)) {
                    if (m_ptr->ml && (r_ptr->cdefense & CD_NO_SLEEP)) {
                        c_recall[m_ptr->mptr].r_cdefense |= CD_NO_SLEEP;
                    }

                    vtype out_val;
                    (void)sprintf(out_val, "%s is unaffected.", m_name);
                    msg_print(out_val);
                } else {
                    sleep = true;
                    m_ptr->csleep = 500;

                    vtype out_val;
                    (void)sprintf(out_val, "%s falls asleep.", m_name);
                    msg_print(out_val);
                }
            }
        }
    }

    return sleep;
}

/* Detect any treasure on the current panel    -RAK- */
int detect_treasure() {
    bool detect = false;

    for (int i = panel_row_min; i <= panel_row_max; i++) {
        for (int j = panel_col_min; j <= panel_col_max; j++) {
            cave_type *c_ptr = &cave[i][j];

            if ((c_ptr->tptr != 0) && (t_list[c_ptr->tptr].tval == TV_GOLD) &&
                !test_light(i, j)) {
                c_ptr->fm = true;
                lite_spot(i, j);
                detect = true;
            }
        }
    }

    return detect;
}

/* Detect all objects on the current panel    -RAK- */
int detect_object() {
    bool detect = false;

    for (int i = panel_row_min; i <= panel_row_max; i++) {
        for (int j = panel_col_min; j <= panel_col_max; j++) {
            cave_type *c_ptr = &cave[i][j];

            if ((c_ptr->tptr != 0) &&
                (t_list[c_ptr->tptr].tval < TV_MAX_OBJECT) &&
                !test_light(i, j)) {
                c_ptr->fm = true;
                lite_spot(i, j);
                detect = true;
            }
        }
    }

    return detect;
}

/* Locates and displays traps on current panel    -RAK- */
int detect_trap() {
    bool detect = false;

    for (int i = panel_row_min; i <= panel_row_max; i++) {
        for (int j = panel_col_min; j <= panel_col_max; j++) {
            cave_type *c_ptr = &cave[i][j];

            if (c_ptr->tptr != 0) {
                if (t_list[c_ptr->tptr].tval == TV_INVIS_TRAP) {
                    c_ptr->fm = true;
                    change_trap(i, j);
                    detect = true;
                } else if (t_list[c_ptr->tptr].tval == TV_CHEST) {
                    inven_type *t_ptr = &t_list[c_ptr->tptr];
                    known2(t_ptr);
                }
            }
        }
    }

    return detect;
}

/* Locates and displays all secret doors on current panel -RAK- */
int detect_sdoor() {
    bool detect = false;

    for (int i = panel_row_min; i <= panel_row_max; i++) {
        for (int j = panel_col_min; j <= panel_col_max; j++) {
            cave_type *c_ptr = &cave[i][j];

            if (c_ptr->tptr != 0) {
                /* Secret doors */
                if (t_list[c_ptr->tptr].tval == TV_SECRET_DOOR) {
                    c_ptr->fm = true;
                    change_trap(i, j);
                    detect = true;
                } else if (((t_list[c_ptr->tptr].tval == TV_UP_STAIR) ||
                            (t_list[c_ptr->tptr].tval == TV_DOWN_STAIR)) &&
                           !c_ptr->fm) { /* Staircases */
                    c_ptr->fm = true;
                    lite_spot(i, j);
                    detect = true;
                }
            }
        }
    }

    return detect;
}

/* Locates and displays all invisible creatures on current panel -RAK-*/
int detect_invisible() {
    bool flag = false;

    for (int i = mfptr - 1; i >= MIN_MONIX; i--) {
        monster_type *m_ptr = &m_list[i];

        if (panel_contains((int)m_ptr->fy, (int)m_ptr->fx) && (CM_INVISIBLE & c_list[m_ptr->mptr].cmove)) {
            m_ptr->ml = true;
            /* works correctly even if hallucinating */
            print((char)c_list[m_ptr->mptr].cchar, (int)m_ptr->fy, (int)m_ptr->fx);
            flag = true;
        }
    }

    if (flag) {
        msg_print("You sense the presence of invisible creatures!");
        msg_print(CNIL);

        /* must unlight every monster just lighted */
        creatures(false);
    }

    return flag;
}

/* Light an area:                           -RAK-
 *     1.  If corridor  light immediate area
 *     2.  If room      light entire room plus immediate area.
 */
int light_area(int y, int x) {
    if (py.flags.blind < 1) {
        msg_print("You are surrounded by a white light.");
    }

    bool light = true;

    if (cave[y][x].lr && (dun_level > 0)) {
        light_room(y, x);
    }

    /* Must always light immediate area, because one might be standing on
       the edge of a room, or next to a destroyed area, etc. */
    for (int i = y - 1; i <= y + 1; i++) {
        for (int j = x - 1; j <= x + 1; j++) {
            cave[i][j].pl = true;
            lite_spot(i, j);
        }
    }

    return light;
}

/* Darken an area, opposite of light area    -RAK- */
int unlight_area(int y, int x) {
    bool unlight = false;

    if (cave[y][x].lr && (dun_level > 0)) {
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
                        unlight = true;
                    }
                }
            }
        }
    } else {
        for (int i = y - 1; i <= y + 1; i++) {
            for (int j = x - 1; j <= x + 1; j++) {
                cave_type *c_ptr = &cave[i][j];
                if ((c_ptr->fval == CORR_FLOOR) && c_ptr->pl) {
                    /* pl could have been set by star-lite wand, etc */
                    c_ptr->pl = false;
                    unlight = true;
                }
            }
        }
    }

    if (unlight && py.flags.blind <= 0) {
        msg_print("Darkness surrounds you.");
    }

    return unlight;
}

/* Map the current area plus some      -RAK- */
void map_area() {
    int i = panel_row_min - randint(10);
    int j = panel_row_max + randint(10);
    int k = panel_col_min - randint(20);
    int l = panel_col_max + randint(20);

    for (int m = i; m <= j; m++) {
        for (int n = k; n <= l; n++) {
            if (in_bounds(m, n) && (cave[m][n].fval <= MAX_CAVE_FLOOR)) {
                for (int i7 = m - 1; i7 <= m + 1; i7++) {
                    for (int i8 = n - 1; i8 <= n + 1; i8++) {
                        cave_type *c_ptr = &cave[i7][i8];

                        if (c_ptr->fval >= MIN_CAVE_WALL) {
                            c_ptr->pl = true;
                        } else if ((c_ptr->tptr != 0) && (t_list[c_ptr->tptr].tval >= TV_MIN_VISIBLE) && (t_list[c_ptr->tptr].tval <= TV_MAX_VISIBLE)) {
                            c_ptr->fm = true;
                        }
                    }
                }
            }
        }
    }

    prt_map();
}

/* Identify an object          -RAK- */
int ident_spell() {

    bool ident = false;

    int item_val;
    if (get_item(&item_val, "Item you wish identified?", 0, INVEN_ARRAY_SIZE, CNIL, CNIL)) {
        ident = true;
        identify(&item_val);

        inven_type *i_ptr = &inventory[item_val];
        known2(i_ptr);

        bigvtype tmp_str;
        objdes(tmp_str, i_ptr, true);

        bigvtype out_val;
        if (item_val >= INVEN_WIELD) {
            calc_bonuses();
            (void)sprintf(out_val, "%s: %s", describe_use(item_val), tmp_str);
        } else {
            (void)sprintf(out_val, "%c %s", item_val + 97, tmp_str);
        }
        msg_print(out_val);
    }

    return ident;
}

/* Get all the monsters on the level pissed off.  -RAK- */
int aggravate_monster(int dis_affect) {
    monster_type *m_ptr;

    bool aggravate = false;

    for (int i = mfptr - 1; i >= MIN_MONIX; i--) {
        m_ptr = &m_list[i];
        m_ptr->csleep = 0;
        if ((m_ptr->cdis <= dis_affect) && (m_ptr->cspeed < 2)) {
            m_ptr->cspeed++;
            aggravate = true;
        }
    }

    if (aggravate) {
        msg_print("You hear a sudden stirring in the distance!");
    }

    return aggravate;
}

/* Surround the fool with traps (chuckle)    -RAK- */
int trap_creation() {
    bool trap = true;

    for (int i = char_row - 1; i <= char_row + 1; i++) {
        for (int j = char_col - 1; j <= char_col + 1; j++) {
            /* Don't put a trap under the player, since this can lead to
               strange situations, e.g. falling through a trap door while
               trying to rest, setting off a falling rock trap and ending
               up under the rock. */
            if (i == char_row && j == char_col) {
                continue;
            }

            cave_type *c_ptr = &cave[i][j];

            if (c_ptr->fval <= MAX_CAVE_FLOOR) {
                if (c_ptr->tptr != 0) {
                    (void)delete_object(i, j);
                }
                place_trap(i, j, randint(MAX_TRAP) - 1);

                /* don't let player gain exp from the newly created traps */
                t_list[c_ptr->tptr].p1 = 0;

                /* open pits are immediately visible, so call lite_spot */
                lite_spot(i, j);
            }
        }
    }

    return trap;
}

/* Surround the player with doors.      -RAK- */
int door_creation() {
    bool door = false;

    for (int i = char_row - 1; i <= char_row + 1; i++) {
        for (int j = char_col - 1; j <= char_col + 1; j++) {
            if ((i != char_row) || (j != char_col)) {
                cave_type *c_ptr = &cave[i][j];

                if (c_ptr->fval <= MAX_CAVE_FLOOR) {
                    door = true;

                    if (c_ptr->tptr != 0) {
                        (void)delete_object(i, j);
                    }

                    int k = popt();
                    c_ptr->fval = BLOCKED_FLOOR;
                    c_ptr->tptr = k;
                    invcopy(&t_list[k], OBJ_CLOSED_DOOR);
                    lite_spot(i, j);
                }
            }
        }
    }

    return door;
}

/* Destroys any adjacent door(s)/trap(s)    -RAK- */
int td_destroy() {
    bool destroy = false;

    for (int i = char_row - 1; i <= char_row + 1; i++) {
        for (int j = char_col - 1; j <= char_col + 1; j++) {
            cave_type *c_ptr = &cave[i][j];
            if (c_ptr->tptr != 0) {
                if (((t_list[c_ptr->tptr].tval >= TV_INVIS_TRAP) &&
                     (t_list[c_ptr->tptr].tval <= TV_CLOSED_DOOR) &&
                     (t_list[c_ptr->tptr].tval != TV_RUBBLE)) ||
                    (t_list[c_ptr->tptr].tval == TV_SECRET_DOOR))
                {
                    if (delete_object(i, j)) {
                        destroy = true;
                    }
                } else if ((t_list[c_ptr->tptr].tval == TV_CHEST) &&
                           (t_list[c_ptr->tptr].flags != 0))
                {
                    /* destroy traps on chest and unlock */
                    t_list[c_ptr->tptr].flags &= ~(CH_TRAPPED | CH_LOCKED);
                    t_list[c_ptr->tptr].name2 = SN_UNLOCKED;
                    msg_print("You have disarmed the chest.");
                    known2(&t_list[c_ptr->tptr]);
                    destroy = true;
                }
            }
        }
    }

    return destroy;
}

/* Display all creatures on the current panel    -RAK- */
int detect_monsters() {
    bool detect = false;

    for (int i = mfptr - 1; i >= MIN_MONIX; i--) {
        monster_type *m_ptr = &m_list[i];

        if (panel_contains((int)m_ptr->fy, (int)m_ptr->fx) && ((CM_INVISIBLE & c_list[m_ptr->mptr].cmove) == 0)) {
            m_ptr->ml = true;
            /* works correctly even if hallucinating */
            print((char)c_list[m_ptr->mptr].cchar, (int)m_ptr->fy, (int)m_ptr->fx);
            detect = true;
        }
    }

    if (detect) {
        msg_print("You sense the presence of monsters!");
        msg_print(CNIL);

        /* must unlight every monster just lighted */
        creatures(false);
    }

    return detect;
}

/* Leave a line of light in given dir, blue light can sometimes */
/* hurt creatures.               -RAK- */
void light_line(int dir, int y, int x) {
    bool flag = false;
    int dist = -1;

    do {
        /* put mmove at end because want to light up current spot */
        dist++;

        cave_type *c_ptr = &cave[y][x];

        if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE) {
            flag = true;
        } else {
            if (!c_ptr->pl && !c_ptr->tl) {
                /* set pl so that lite_spot will work */
                c_ptr->pl = true;
                if (c_ptr->fval == LIGHT_FLOOR) {
                    if (panel_contains(y, x)) {
                        light_room(y, x);
                    }
                } else {
                    lite_spot(y, x);
                }
            }

            /* set pl in case tl was true above */
            c_ptr->pl = true;
            if (c_ptr->cptr > 1) {
                monster_type *m_ptr = &m_list[c_ptr->cptr];
                creature_type *r_ptr = &c_list[m_ptr->mptr];

                /* light up and draw monster */
                update_mon((int)c_ptr->cptr);

                vtype m_name;
                monster_name(m_name, m_ptr, r_ptr);

                if (CD_LIGHT & r_ptr->cdefense) {
                    if (m_ptr->ml) {
                        c_recall[m_ptr->mptr].r_cdefense |= CD_LIGHT;
                    }

                    vtype out_val;

                    int i = mon_take_hit((int)c_ptr->cptr, damroll(2, 8));
                    if (i >= 0) {
                        (void)sprintf(out_val, "%s shrivels away in the light!", m_name);
                        msg_print(out_val);
                        prt_experience();
                    } else {
                        (void)sprintf(out_val, "%s cringes from the light!", m_name);
                        msg_print(out_val);
                    }
                }
            }
        }
        (void)mmove(dir, &y, &x);
    } while (!flag);
}

/* Light line in all directions        -RAK- */
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

/* Disarms all traps/chests in a given direction  -RAK- */
int disarm_all(int dir, int y, int x) {
    bool disarm = false;
    int dist = -1;

    cave_type *c_ptr;

    do {
        /* put mmove at end, in case standing on a trap */
        dist++;

        c_ptr = &cave[y][x];

        /* note, must continue upto and including the first non open space,
           because secret doors have fval greater than MAX_OPEN_SPACE */
        if (c_ptr->tptr != 0) {
            inven_type *t_ptr = &t_list[c_ptr->tptr];

            if ((t_ptr->tval == TV_INVIS_TRAP) ||
                (t_ptr->tval == TV_VIS_TRAP)) {
                if (delete_object(y, x)) {
                    disarm = true;
                }
            } else if (t_ptr->tval == TV_CLOSED_DOOR) {
                /* Locked or jammed doors become merely closed. */
                t_ptr->p1 = 0;
            } else if (t_ptr->tval == TV_SECRET_DOOR) {
                c_ptr->fm = true;
                change_trap(y, x);
                disarm = true;
            } else if ((t_ptr->tval == TV_CHEST) && (t_ptr->flags != 0)) {
                msg_print("Click!");
                t_ptr->flags &= ~(CH_TRAPPED | CH_LOCKED);
                disarm = true;
                t_ptr->name2 = SN_UNLOCKED;
                known2(t_ptr);
            }
        }
        (void)mmove(dir, &y, &x);
    } while ((dist <= OBJ_BOLT_RANGE) && c_ptr->fval <= MAX_OPEN_SPACE);

    return disarm;
}

/* Return flags for given type area affect    -RAK- */
void get_flags(int typ, uint32_t *weapon_type, int *harm_type, bool (**destroy)()) {
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

/* Shoot a bolt in a given direction      -RAK- */
void fire_bolt(int typ, int dir, int y, int x, int dam, char *bolt_typ) {
    bool flag = false;

    bool (*dummy)();
    int harm_type;
    uint32_t weapon_type;
    get_flags(typ, &weapon_type, &harm_type, &dummy);

    int oldy = y;
    int oldx = x;
    int dist = 0;

    do {
        (void)mmove(dir, &y, &x);
        dist++;

        cave_type *c_ptr = &cave[y][x];

        lite_spot(oldy, oldx);
        if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE) {
            flag = true;
        } else {
            if (c_ptr->cptr > 1) {
                flag = true;

                monster_type *m_ptr = &m_list[c_ptr->cptr];
                creature_type *r_ptr = &c_list[m_ptr->mptr];

                /* light up monster and draw monster, temporarily set
                   pl so that update_mon() will work */
                int i = c_ptr->pl;
                c_ptr->pl = true;
                update_mon((int)c_ptr->cptr);
                c_ptr->pl = i;

                /* draw monster and clear previous bolt */
                put_qio();

                vtype out_val, m_name;

                lower_monster_name(m_name, m_ptr, r_ptr);
                (void)sprintf(out_val, "The %s strikes %s.", bolt_typ, m_name);
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

                monster_name(m_name, m_ptr, r_ptr);
                i = mon_take_hit((int)c_ptr->cptr, dam);

                if (i >= 0) {
                    (void)sprintf(out_val, "%s dies in a fit of agony.", m_name);
                    msg_print(out_val);
                    prt_experience();
                } else if (dam > 0) {
                    (void)sprintf(out_val, "%s screams in agony.", m_name);
                    msg_print(out_val);
                }
            } else if (panel_contains(y, x) && (py.flags.blind < 1)) {
                print('*', y, x);

                /* show the bolt */
                put_qio();
            }
        }
        oldy = y;
        oldx = x;
    } while (!flag);
}

/* Shoot a ball in a given direction.  Note that balls have an */
/* area affect.                -RAK- */
void fire_ball(int typ, int dir, int y, int x, int dam_hp, char *descrip) {
    int thit = 0;
    int tkill = 0;
    int max_dis = 2;

    bool (*destroy)();
    int harm_type;
    uint32_t weapon_type;
    get_flags(typ, &weapon_type, &harm_type, &destroy);

    bool flag = false;

    int oldy = y;
    int oldx = x;
    int dist = 0;

    do {
        (void)mmove(dir, &y, &x);
        dist++;
        lite_spot(oldy, oldx);
        if (dist > OBJ_BOLT_RANGE) {
            flag = true;
        } else {
            cave_type *c_ptr = &cave[y][x];

            if ((c_ptr->fval >= MIN_CLOSED_SPACE) || (c_ptr->cptr > 1)) {
                flag = true;
                if (c_ptr->fval >= MIN_CLOSED_SPACE) {
                    y = oldy;
                    x = oldx;
                }
                /* The ball hits and explodes. */
                /* The explosion. */
                for (int i = y - max_dis; i <= y + max_dis; i++) {
                    for (int j = x - max_dis; j <= x + max_dis; j++) {
                        if (in_bounds(i, j) && (distance(y, x, i, j) <= max_dis) && los(y, x, i, j)) {
                            c_ptr = &cave[i][j];

                            if ((c_ptr->tptr != 0) && (*destroy)(&t_list[c_ptr->tptr])) {
                                (void)delete_object(i, j);
                            }

                            if (c_ptr->fval <= MAX_OPEN_SPACE) {
                                if (c_ptr->cptr > 1) {
                                    monster_type *m_ptr = &m_list[c_ptr->cptr];
                                    creature_type *r_ptr = &c_list[m_ptr->mptr];

                                    /* lite up creature if visible, temp set pl so that update_mon works */
                                    int tmp = c_ptr->pl;
                                    c_ptr->pl = true;
                                    update_mon((int)c_ptr->cptr);

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
                                    int k = mon_take_hit((int)c_ptr->cptr, dam);

                                    if (k >= 0) {
                                        tkill++;
                                    }
                                    c_ptr->pl = tmp;
                                } else if (panel_contains(i, j) && (py.flags.blind < 1)) {
                                    print('*', i, j);
                                }
                            }
                        }
                    }
                }

                /* show ball of whatever */
                put_qio();

                for (int i = (y - 2); i <= (y + 2); i++) {
                    for (int j = (x - 2); j <= (x + 2); j++) {
                        if (in_bounds(i, j) && panel_contains(i, j) && (distance(y, x, i, j) <= max_dis)) {
                            lite_spot(i, j);
                        }
                    }
                }
                /* End  explosion. */

                if (thit == 1) {
                    vtype out_val;
                    (void)sprintf(out_val, "The %s envelops a creature!", descrip);
                    msg_print(out_val);
                } else if (thit > 1) {
                    vtype out_val;
                    (void)sprintf(out_val, "The %s envelops several creatures!", descrip);
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
                /* End ball hitting. */
            } else if (panel_contains(y, x) && (py.flags.blind < 1)) {
                print('*', y, x);

                /* show bolt */
                put_qio();
            }
            oldy = y;
            oldx = x;
        }
    } while (!flag);
}

/* Breath weapon works like a fire_ball, but affects the player. */
/* Note the area affect.            -RAK- */
void breath(int typ, int y, int x, int dam_hp, char *ddesc, int monptr) {
    int max_dis = 2;

    bool (*destroy)();
    int harm_type;
    uint32_t weapon_type;
    get_flags(typ, &weapon_type, &harm_type, &destroy);

    int dam;

    for (int i = y - 2; i <= y + 2; i++) {
        for (int j = x - 2; j <= x + 2; j++) {
            if (in_bounds(i, j) && (distance(y, x, i, j) <= max_dis) && los(y, x, i, j)) {
                cave_type *c_ptr = &cave[i][j];

                if ((c_ptr->tptr != 0) && (*destroy)(&t_list[c_ptr->tptr])) {
                    (void)delete_object(i, j);
                }

                if (c_ptr->fval <= MAX_OPEN_SPACE) {
                    /* must test status bit, not py.flags.blind here, flag could have
                       been set by a previous monster, but the breath should still
                       be visible until the blindness takes effect */
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

                        /* can not call mon_take_hit here, since player does not
                           get experience for kill */
                        m_ptr->hp = m_ptr->hp - dam;
                        m_ptr->csleep = 0;

                        if (m_ptr->hp < 0) {
                            uint32_t treas = monster_death((int)m_ptr->fy, (int)m_ptr->fx, r_ptr->cmove);

                            if (m_ptr->ml) {
                                uint32_t tmp = (c_recall[m_ptr->mptr].r_cmove & CM_TREASURE) >> CM_TR_SHIFT;
                                if (tmp > ((treas & CM_TREASURE) >> CM_TR_SHIFT)) {
                                    treas = (treas & ~CM_TREASURE) | (tmp << CM_TR_SHIFT);
                                }
                                c_recall[m_ptr->mptr].r_cmove = treas | (c_recall[m_ptr->mptr].r_cmove & ~CM_TREASURE);
                            }

                            /* It ate an already processed monster.Handle
                             * normally.*/
                            if (monptr < c_ptr->cptr) {
                                delete_monster((int)c_ptr->cptr);
                            } else {
                                /* If it eats this monster, an already processed monster
                                   will take its place, causing all kinds of havoc.
                                   Delay the kill a bit. */
                                fix1_delete_monster((int)c_ptr->cptr);
                            }
                        }
                    } else if (c_ptr->cptr == 1) {
                        dam = (dam_hp / (distance(i, j, y, x) + 1));

                        /* let's do at least one point of damage */
                        /* prevents randint(0) problem with poison_gas, also */
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

    /* show the ball of gas */
    put_qio();

    for (int i = (y - 2); i <= (y + 2); i++) {
        for (int j = (x - 2); j <= (x + 2); j++) {
            if (in_bounds(i, j) && panel_contains(i, j) && (distance(y, x, i, j) <= max_dis)) {
                lite_spot(i, j);
            }
        }
    }
}

/* Recharge a wand, staff, or rod.  Sometimes the item breaks. -RAK-*/
int recharge(int num) {
    int i, j, item_val;

    bool res = false;

    if (!find_range(TV_STAFF, TV_WAND, &i, &j)) {
        msg_print("You have nothing to recharge.");
    } else if (get_item(&item_val, "Recharge which item?", i, j, CNIL, CNIL)) {
        inven_type *i_ptr = &inventory[item_val];

        res = true;

        /* recharge  I = recharge(20) = 1/6  failure for empty 10th level wand
         * recharge II = recharge(60) = 1/10 failure for empty 10th level wand
         *
         * make it harder to recharge high level, and highly charged wands, note
         * that i can be negative, so check its value before trying to call randint().
         */
        i = num + 50 - (int)i_ptr->level - i_ptr->p1;
        if (i < 19) {
            /* Automatic failure. */
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
    }
    return res;
}

/* Increase or decrease a creatures hit points    -RAK- */
int hp_monster(int dir, int y, int x, int dam) {
    bool monster = false;
    bool flag = false;
    int dist = 0;

    do {
        (void)mmove(dir, &y, &x);
        dist++;

        cave_type *c_ptr = &cave[y][x];

        if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE) {
            flag = true;
        } else if (c_ptr->cptr > 1) {
            flag = true;

            monster_type *m_ptr = &m_list[c_ptr->cptr];
            creature_type *r_ptr = &c_list[m_ptr->mptr];

            vtype m_name;
            monster_name(m_name, m_ptr, r_ptr);
            monster = true;
            int i = mon_take_hit((int)c_ptr->cptr, dam);

            if (i >= 0) {
                vtype out_val;
                (void)sprintf(out_val, "%s dies in a fit of agony.", m_name);
                msg_print(out_val);
                prt_experience();
            } else if (dam > 0) {
                vtype out_val;
                (void)sprintf(out_val, "%s screams in agony.", m_name);
                msg_print(out_val);
            }
        }
    } while (!flag);

    return monster;
}

/* Drains life; note it must be living.    -RAK- */
int drain_life(int dir, int y, int x) {
    bool drain = false;
    bool flag = false;
    int dist = 0;

    do {
        (void)mmove(dir, &y, &x);
        dist++;

        cave_type *c_ptr = &cave[y][x];

        if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE) {
            flag = true;
        } else if (c_ptr->cptr > 1) {
            flag = true;

            monster_type *m_ptr = &m_list[c_ptr->cptr];
            creature_type *r_ptr = &c_list[m_ptr->mptr];

            if ((r_ptr->cdefense & CD_UNDEAD) == 0) {
                drain = true;

                vtype m_name;
                monster_name(m_name, m_ptr, r_ptr);

                int i = mon_take_hit((int)c_ptr->cptr, 75);
                if (i >= 0) {
                    vtype out_val;
                    (void)sprintf(out_val, "%s dies in a fit of agony.", m_name);
                    msg_print(out_val);
                    prt_experience();
                } else {
                    vtype out_val;
                    (void)sprintf(out_val, "%s screams in agony.", m_name);
                    msg_print(out_val);
                }
            } else {
                c_recall[m_ptr->mptr].r_cdefense |= CD_UNDEAD;
            }
        }
    } while (!flag);

    return drain;
}

/* Increase or decrease a creatures speed    -RAK- */
/* NOTE: cannot slow a winning creature (BALROG) */
int speed_monster(int dir, int y, int x, int spd) {
    bool speed = false;
    bool flag = false;
    int dist = 0;

    do {
        (void)mmove(dir, &y, &x);
        dist++;

        cave_type *c_ptr = &cave[y][x];

        if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE) {
            flag = true;
        } else if (c_ptr->cptr > 1) {
            flag = true;

            monster_type *m_ptr = &m_list[c_ptr->cptr];
            creature_type *r_ptr = &c_list[m_ptr->mptr];

            vtype m_name;
            monster_name(m_name, m_ptr, r_ptr);

            if (spd > 0) {
                vtype out_val;
                m_ptr->cspeed += spd;
                m_ptr->csleep = 0;
                (void)sprintf(out_val, "%s starts moving faster.", m_name);
                msg_print(out_val);
                speed = true;
            } else if (randint(MAX_MONS_LEVEL) > r_ptr->level) {
                vtype out_val;
                m_ptr->cspeed += spd;
                m_ptr->csleep = 0;
                (void)sprintf(out_val, "%s starts moving slower.", m_name);
                msg_print(out_val);
                speed = true;
            } else {
                vtype out_val;
                m_ptr->csleep = 0;
                (void)sprintf(out_val, "%s is unaffected.", m_name);
                msg_print(out_val);
            }
        }
    } while (!flag);

    return speed;
}

/* Confuse a creature          -RAK- */
int confuse_monster(int dir, int y, int x) {
    bool confuse = false;
    bool flag = false;
    int dist = 0;

    do {
        (void)mmove(dir, &y, &x);
        dist++;
        cave_type *c_ptr = &cave[y][x];

        if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE) {
            flag = true;
        } else if (c_ptr->cptr > 1) {
            monster_type *m_ptr = &m_list[c_ptr->cptr];
            creature_type *r_ptr = &c_list[m_ptr->mptr];

            vtype m_name;
            monster_name(m_name, m_ptr, r_ptr);
            flag = true;
            if ((randint(MAX_MONS_LEVEL) < r_ptr->level) || (CD_NO_SLEEP & r_ptr->cdefense)) {
                if (m_ptr->ml && (r_ptr->cdefense & CD_NO_SLEEP)) {
                    c_recall[m_ptr->mptr].r_cdefense |= CD_NO_SLEEP;
                }

                /* Monsters which resisted the attack should wake up.
                   Monsters with innate resistence ignore the attack. */
                if (!(CD_NO_SLEEP & r_ptr->cdefense)) {
                    m_ptr->csleep = 0;
                }

                vtype out_val;
                (void)sprintf(out_val, "%s is unaffected.", m_name);
                msg_print(out_val);
            } else {
                if (m_ptr->confused) {
                    m_ptr->confused += 3;
                } else {
                    m_ptr->confused = 2 + randint(16);
                }
                confuse = true;
                m_ptr->csleep = 0;

                vtype out_val;
                (void)sprintf(out_val, "%s appears confused.", m_name);
                msg_print(out_val);
            }
        }
    } while (!flag);

    return confuse;
}

/* Sleep a creature.          -RAK- */
int sleep_monster(int dir, int y, int x) {
    bool sleep = false;
    bool flag = false;
    int dist = 0;

    do {
        (void)mmove(dir, &y, &x);
        dist++;

        cave_type *c_ptr = &cave[y][x];

        if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE) {
            flag = true;
        } else if (c_ptr->cptr > 1) {
            flag = true;

            monster_type *m_ptr = &m_list[c_ptr->cptr];
            creature_type *r_ptr = &c_list[m_ptr->mptr];

            vtype m_name;
            monster_name(m_name, m_ptr, r_ptr);

            if ((randint(MAX_MONS_LEVEL) < r_ptr->level) || (CD_NO_SLEEP & r_ptr->cdefense)) {
                if (m_ptr->ml && (r_ptr->cdefense & CD_NO_SLEEP)) {
                    c_recall[m_ptr->mptr].r_cdefense |= CD_NO_SLEEP;
                }

                vtype out_val;
                (void)sprintf(out_val, "%s is unaffected.", m_name);
                msg_print(out_val);
            } else {
                m_ptr->csleep = 500;
                sleep = true;

                vtype out_val;
                (void)sprintf(out_val, "%s falls asleep.", m_name);
                msg_print(out_val);
            }
        }
    } while (!flag);

    return sleep;
}

/* Turn stone to mud, delete wall.      -RAK- */
int wall_to_mud(int dir, int y, int x) {
    bool wall = false;
    bool flag = false;
    int dist = 0;

    do {
        (void)mmove(dir, &y, &x);
        dist++;

        cave_type *c_ptr = &cave[y][x];

        /* note, this ray can move through walls as it turns them to mud */
        if (dist == OBJ_BOLT_RANGE) {
            flag = true;
        }

        if ((c_ptr->fval >= MIN_CAVE_WALL) && (c_ptr->fval != BOUNDARY_WALL)) {
            flag = true;
            (void)twall(y, x, 1, 0);

            if (test_light(y, x)) {
                msg_print("The wall turns into mud.");
                wall = true;
            }
        } else if ((c_ptr->tptr != 0) && (c_ptr->fval >= MIN_CLOSED_SPACE)) {
            flag = true;
            if (panel_contains(y, x) && test_light(y, x)) {
                bigvtype out_val, tmp_str;
                objdes(tmp_str, &t_list[c_ptr->tptr], false);
                (void)sprintf(out_val, "The %s turns into mud.", tmp_str);
                msg_print(out_val);
                wall = true;
            }

            if (t_list[c_ptr->tptr].tval == TV_RUBBLE) {
                (void)delete_object(y, x);
                if (randint(10) == 1) {
                    place_object(y, x, false);
                    if (test_light(y, x)) {
                        msg_print("You have found something!");
                    }
                }
                lite_spot(y, x);
            } else {
                (void)delete_object(y, x);
            }
        }

        if (c_ptr->cptr > 1) {
            monster_type *m_ptr = &m_list[c_ptr->cptr];
            creature_type *r_ptr = &c_list[m_ptr->mptr];

            if (CD_STONE & r_ptr->cdefense) {
                vtype m_name;

                monster_name(m_name, m_ptr, r_ptr);
                int i = mon_take_hit((int)c_ptr->cptr, 100);

                /* Should get these messages even if the monster is not visible. */
                if (i >= 0) {
                    c_recall[i].r_cdefense |= CD_STONE;

                    bigvtype out_val;
                    (void)sprintf(out_val, "%s dissolves!", m_name);
                    msg_print(out_val);
                    prt_experience(); /* print msg before calling prt_exp */
                } else {
                    c_recall[m_ptr->mptr].r_cdefense |= CD_STONE;

                    bigvtype out_val;
                    (void)sprintf(out_val, "%s grunts in pain!", m_name);
                    msg_print(out_val);
                }
                flag = true;
            }
        }
    } while (!flag);

    return wall;
}

/* Destroy all traps and doors in a given direction  -RAK- */
int td_destroy2(int dir, int y, int x) {
    bool destroy2 = false;
    int dist = 0;

    cave_type *c_ptr;

    do {
        (void)mmove(dir, &y, &x);
        dist++;

        c_ptr = &cave[y][x];

        /* must move into first closed spot, as it might be a secret door */
        if (c_ptr->tptr != 0) {
            inven_type *t_ptr = &t_list[c_ptr->tptr];

            if ((t_ptr->tval == TV_INVIS_TRAP) ||
                (t_ptr->tval == TV_CLOSED_DOOR) ||
                (t_ptr->tval == TV_VIS_TRAP) ||
                (t_ptr->tval == TV_OPEN_DOOR) ||
                (t_ptr->tval == TV_SECRET_DOOR))
            {
                if (delete_object(y, x)) {
                    msg_print("There is a bright flash of light!");
                    destroy2 = true;
                }
            } else if ((t_ptr->tval == TV_CHEST) && (t_ptr->flags != 0)) {
                msg_print("Click!");
                t_ptr->flags &= ~(CH_TRAPPED | CH_LOCKED);
                destroy2 = true;
                t_ptr->name2 = SN_UNLOCKED;
                known2(t_ptr);
            }
        }
    } while ((dist <= OBJ_BOLT_RANGE) || c_ptr->fval <= MAX_OPEN_SPACE);

    return destroy2;
}

/* Polymorph a monster          -RAK- */
/* NOTE: cannot polymorph a winning creature (BALROG) */
int poly_monster(int dir, int y, int x) {
    bool poly = false;
    bool flag = false;
    int dist = 0;

    do {
        (void)mmove(dir, &y, &x);
        dist++;

        cave_type *c_ptr = &cave[y][x];

        if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE) {
            flag = true;
        } else if (c_ptr->cptr > 1) {
            monster_type *m_ptr = &m_list[c_ptr->cptr];
            creature_type *r_ptr = &c_list[m_ptr->mptr];

            if (randint(MAX_MONS_LEVEL) > r_ptr->level) {
                flag = true;
                delete_monster((int)c_ptr->cptr);

                /* Place_monster() should always return true here. */
                poly = place_monster(y, x, randint(m_level[MAX_MONS_LEVEL] - m_level[0]) - 1 + m_level[0], false);

                /* don't test c_ptr->fm here, only pl/tl */
                if (poly && panel_contains(y, x) && (c_ptr->tl || c_ptr->pl)) {
                    poly = true;
                }
            } else {
                vtype out_val, m_name;
                monster_name(m_name, m_ptr, r_ptr);
                (void)sprintf(out_val, "%s is unaffected.", m_name);
                msg_print(out_val);
            }
        }
    } while (!flag);

    return poly;
}

/* Create a wall.          -RAK- */
int build_wall(int dir, int y, int x) {
    bool build = false;
    int dist = 0;
    bool flag = false;

    int i = 0;
    do {
        (void)mmove(dir, &y, &x);
        dist++;

        cave_type *c_ptr = &cave[y][x];

        if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE) {
            flag = true;
        } else {
            if (c_ptr->tptr != 0) {
                (void)delete_object(y, x);
            }

            if (c_ptr->cptr > 1) {
                /* stop the wall building */
                flag = true;

                monster_type *m_ptr = &m_list[c_ptr->cptr];
                creature_type *r_ptr = &c_list[m_ptr->mptr];

                if (!(r_ptr->cmove & CM_PHASE)) {
                    int damage;

                    /* monster does not move, can't escape the wall */
                    if (r_ptr->cmove & CM_ATTACK_ONLY) {
                        /* this will kill everything */
                        damage = 3000;
                    } else {
                        damage = damroll(4, 8);
                    }

                    vtype m_name, out_val;
                    monster_name(m_name, m_ptr, r_ptr);
                    (void)sprintf(out_val, "%s wails out in pain!", m_name);
                    msg_print(out_val);
                    i = mon_take_hit((int)c_ptr->cptr, damage);

                    if (i >= 0) {
                        (void)sprintf(out_val, "%s is embedded in the rock.", m_name);
                        msg_print(out_val);
                        prt_experience();
                    }
                } else if (r_ptr->cchar == 'E' || r_ptr->cchar == 'X') {
                    /* must be an earth elemental or an earth spirit, or a Xorn
                       increase its hit points */
                    m_ptr->hp += damroll(4, 8);
                }
            }

            c_ptr->fval = MAGMA_WALL;
            c_ptr->fm = false;

            /* Permanently light this wall if it is lit by player's lamp. */
            c_ptr->pl = (c_ptr->tl || c_ptr->pl);
            lite_spot(y, x);
            i++;
            build = true;
        }
    } while (!flag);

    return build;
}

/* Replicate a creature          -RAK- */
bool clone_monster(int dir, int y, int x) {
    int dist = 0;
    bool flag = false;

    do {
        (void)mmove(dir, &y, &x);
        dist++;

        cave_type *c_ptr = &cave[y][x];

        if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE) {
            flag = true;
        } else if (c_ptr->cptr > 1) {
            m_list[c_ptr->cptr].csleep = 0;

            /* monptr of 0 is safe here, since can't reach here from creatures */
            return multiply_monster(y, x, (int)m_list[c_ptr->cptr].mptr, 0);
        }
    } while (!flag);

    return false;
}

/* Move the creature record to a new location    -RAK- */
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

    move_rec((int)m_ptr->fy, (int)m_ptr->fx, yn, xn);
    lite_spot((int)m_ptr->fy, (int)m_ptr->fx);
    m_ptr->fy = yn;
    m_ptr->fx = xn;

    /* this is necessary, because the creature is not currently visible in its new position */
    m_ptr->ml = false;
    m_ptr->cdis = distance(char_row, char_col, yn, xn);
    update_mon(monptr);
}

/* Teleport player to spell casting creature    -RAK- */
void teleport_to(int ny, int nx) {
    int dis = 1;
    int ctr = 0;

    int y, x;
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
    char_row = y;
    char_col = x;
    check_view();

    /* light creatures */
    creatures(false);
}

/* Teleport all creatures in a given direction away  -RAK- */
int teleport_monster(int dir, int y, int x) {
    bool flag = false;
    bool result = false;
    int dist = 0;

    do {
        (void)mmove(dir, &y, &x);
        dist++;

        cave_type *c_ptr = &cave[y][x];

        if ((dist > OBJ_BOLT_RANGE) || c_ptr->fval >= MIN_CLOSED_SPACE) {
            flag = true;
        } else if (c_ptr->cptr > 1) {
            m_list[c_ptr->cptr].csleep = 0; /* wake it up */
            teleport_away((int)c_ptr->cptr, MAX_SIGHT);
            result = true;
        }
    } while (!flag);

    return result;
}

/* Delete all creatures within max_sight distance  -RAK- */
/* NOTE : Winning creatures cannot be genocided */
int mass_genocide() {
    bool result = false;

    for (int i = mfptr - 1; i >= MIN_MONIX; i--) {
        monster_type *m_ptr = &m_list[i];
        creature_type *r_ptr = &c_list[m_ptr->mptr];

        if ((m_ptr->cdis <= MAX_SIGHT) && ((r_ptr->cmove & CM_WIN) == 0)) {
            delete_monster(i);
            result = true;
        }
    }

    return result;
}

/* Delete all creatures of a given type from level.  -RAK- */
/* This does not keep creatures of type from appearing later. */
/* NOTE : Winning creatures can not be genocided. */
int genocide() {
    bool killed = false;

    char typ;
    if (get_com("Which type of creature do you wish exterminated?", &typ)) {
        for (int i = mfptr - 1; i >= MIN_MONIX; i--) {
            monster_type *m_ptr = &m_list[i];
            creature_type *r_ptr = &c_list[m_ptr->mptr];
            if (typ == c_list[m_ptr->mptr].cchar) {
                if ((r_ptr->cmove & CM_WIN) == 0) {
                    delete_monster(i);
                    killed = true;
                } else {
                    /* genocide is a powerful spell, so we will let the player
                       know the names of the creatures he did not destroy,
                       this message makes no sense otherwise */
                    vtype out_val;
                    (void)sprintf(out_val, "The %s is unaffected.", r_ptr->name);
                    msg_print(out_val);
                }
            }
        }
    }

    return killed;
}

/* Change speed of any creature .      -RAK- */
/* NOTE: cannot slow a winning creature (BALROG) */
int speed_monsters(int spd) {
    bool speed = false;

    for (int i = mfptr - 1; i >= MIN_MONIX; i--) {
        monster_type *m_ptr = &m_list[i];
        creature_type *r_ptr = &c_list[m_ptr->mptr];

        vtype out_val, m_name;
        monster_name(m_name, m_ptr, r_ptr);

        if ((m_ptr->cdis > MAX_SIGHT) || !los(char_row, char_col, (int)m_ptr->fy, (int)m_ptr->fx)) {
            ; /* do nothing */
        } else if (spd > 0) {
            m_ptr->cspeed += spd;
            m_ptr->csleep = 0;

            if (m_ptr->ml) {
                speed = true;
                (void)sprintf(out_val, "%s starts moving faster.", m_name);
                msg_print(out_val);
            }
        } else if (randint(MAX_MONS_LEVEL) > r_ptr->level) {
            m_ptr->cspeed += spd;
            m_ptr->csleep = 0;

            if (m_ptr->ml) {
                (void)sprintf(out_val, "%s starts moving slower.", m_name);
                msg_print(out_val);
                speed = true;
            }
        } else if (m_ptr->ml) {
            m_ptr->csleep = 0;
            (void)sprintf(out_val, "%s is unaffected.", m_name);
            msg_print(out_val);
        }
    }

    return speed;
}

/* Sleep any creature .    -RAK- */
int sleep_monsters2() {
    bool sleep = false;

    for (int i = mfptr - 1; i >= MIN_MONIX; i--) {
        monster_type *m_ptr = &m_list[i];
        creature_type *r_ptr = &c_list[m_ptr->mptr];

        vtype out_val, m_name;
        monster_name(m_name, m_ptr, r_ptr);

        if ((m_ptr->cdis > MAX_SIGHT) || !los(char_row, char_col, (int)m_ptr->fy, (int)m_ptr->fx)) {
            ; /* do nothing */
        } else if ((randint(MAX_MONS_LEVEL) < r_ptr->level) || (CD_NO_SLEEP & r_ptr->cdefense)) {
            if (m_ptr->ml) {
                if (r_ptr->cdefense & CD_NO_SLEEP) {
                    c_recall[m_ptr->mptr].r_cdefense |= CD_NO_SLEEP;
                }
                (void)sprintf(out_val, "%s is unaffected.", m_name);
                msg_print(out_val);
            }
        } else {
            m_ptr->csleep = 500;
            if (m_ptr->ml) {
                (void)sprintf(out_val, "%s falls asleep.", m_name);
                msg_print(out_val);
                sleep = true;
            }
        }
    }

    return sleep;
}

/* Polymorph any creature that player can see.  -RAK- */
/* NOTE: cannot polymorph a winning creature (BALROG) */
int mass_poly() {
    bool mass = false;

    for (int i = mfptr - 1; i >= MIN_MONIX; i--) {
        monster_type *m_ptr = &m_list[i];
        if (m_ptr->cdis <= MAX_SIGHT) {
            creature_type *r_ptr = &c_list[m_ptr->mptr];

            if ((r_ptr->cmove & CM_WIN) == 0) {
                int y = m_ptr->fy;
                int x = m_ptr->fx;
                delete_monster(i);

                /* Place_monster() should always return true here. */
                mass = place_monster(y, x, randint(m_level[MAX_MONS_LEVEL] - m_level[0]) - 1 + m_level[0], false);
            }
        }
    }

    return mass;
}

/* Display evil creatures on current panel    -RAK- */
int detect_evil() {
    bool flag = false;

    for (int i = mfptr - 1; i >= MIN_MONIX; i--) {
        monster_type *m_ptr = &m_list[i];
        if (panel_contains((int)m_ptr->fy, (int)m_ptr->fx) &&
            (CD_EVIL & c_list[m_ptr->mptr].cdefense)) {
            m_ptr->ml = true;

            /* works correctly even if hallucinating */
            print((char)c_list[m_ptr->mptr].cchar, (int)m_ptr->fy, (int)m_ptr->fx);
            flag = true;
        }
    }

    if (flag) {
        msg_print("You sense the presence of evil!");
        msg_print(CNIL);

        /* must unlight every monster just lighted */
        creatures(false);
    }

    return flag;
}

/* Change players hit points in some manner    -RAK- */
int hp_player(int num) {
    bool res = false;

    struct misc *m_ptr = &py.misc;

    if (m_ptr->chp < m_ptr->mhp) {
        m_ptr->chp += num;

        if (m_ptr->chp > m_ptr->mhp) {
            m_ptr->chp = m_ptr->mhp;
            m_ptr->chp_frac = 0;
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
        res = true;
    }

    return res;
}

/* Cure players confusion        -RAK- */
int cure_confusion() {
    bool cure = false;

    struct flags *f_ptr = &py.flags;

    if (f_ptr->confused > 1) {
        f_ptr->confused = 1;
        cure = true;
    }
    return cure;
}

/* Cure players blindness        -RAK- */
int cure_blindness() {
    bool cure = false;

    struct flags *f_ptr = &py.flags;

    if (f_ptr->blind > 1) {
        f_ptr->blind = 1;
        cure = true;
    }
    return cure;
}

/* Cure poisoning          -RAK- */
int cure_poison() {
    bool cure = false;

    struct flags *f_ptr = &py.flags;

    if (f_ptr->poisoned > 1) {
        f_ptr->poisoned = 1;
        cure = true;
    }
    return cure;
}

/* Cure the players fear        -RAK- */
int remove_fear() {
    bool result = false;

    struct flags *f_ptr = &py.flags;

    if (f_ptr->afraid > 1) {
        f_ptr->afraid = 1;
        result = true;
    }
    return result;
}

/* This is a fun one.  In a given block, pick some walls and */
/* turn them into open spots.  Pick some open spots and turn */
/* them into walls.  An "Earthquake" effect.         -RAK- */
void earthquake() {
    for (int i = char_row - 8; i <= char_row + 8; i++) {
        for (int j = char_col - 8; j <= char_col + 8; j++) {
            if (((i != char_row) || (j != char_col)) && in_bounds(i, j) && (randint(8) == 1)) {
                cave_type *c_ptr = &cave[i][j];

                if (c_ptr->tptr != 0) {
                    (void)delete_object(i, j);
                }

                if (c_ptr->cptr > 1) {
                    monster_type *m_ptr = &m_list[c_ptr->cptr];
                    creature_type *r_ptr = &c_list[m_ptr->mptr];

                    if (!(r_ptr->cmove & CM_PHASE)) {
                        int damage;

                        if (r_ptr->cmove & CM_ATTACK_ONLY) {
                            /* this will kill everything */
                            damage = 3000;
                        } else {
                            damage = damroll(4, 8);
                        }

                        vtype out_val, m_name;
                        monster_name(m_name, m_ptr, r_ptr);
                        (void)sprintf(out_val, "%s wails out in pain!", m_name);
                        msg_print(out_val);
                        i = mon_take_hit((int)c_ptr->cptr, damage);

                        if (i >= 0) {
                            (void)sprintf(out_val, "%s is embedded in the rock.", m_name);
                            msg_print(out_val);
                            prt_experience();
                        }
                    } else if (r_ptr->cchar == 'E' || r_ptr->cchar == 'X') {
                        /* must be an earth elemental or an earth spirit, or a
                           Xorn increase its hit points */
                        m_ptr->hp += damroll(4, 8);
                    }
                }

                if ((c_ptr->fval >= MIN_CAVE_WALL) && (c_ptr->fval != BOUNDARY_WALL)) {
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

/* Evil creatures don't like this.           -RAK- */
int protect_evil() {
    bool res;

    struct flags *f_ptr = &py.flags;

    if (f_ptr->protevil == 0) {
        res = true;
    } else {
        res = false;
    }
    f_ptr->protevil += randint(25) + 3 * py.misc.lev;

    return res;
}

/* Create some high quality mush for the player.  -RAK- */
void create_food() {
    cave_type *c_ptr = &cave[char_row][char_col];

    if (c_ptr->tptr != 0) {
        /* take no action here, don't want to destroy object under player */
        msg_print("There is already an object under you.");

        /* set free_turn_flag so that scroll/spell points won't be used */
        free_turn_flag = true;
    } else {
        place_object(char_row, char_col, false);
        invcopy(&t_list[c_ptr->tptr], OBJ_MUSH);
    }
}

/* Attempts to destroy a type of creature.  Success depends on */
/* the creatures level VS. the player's level     -RAK- */
int dispel_creature(int cflag, int damage) {
    bool dispel = false;

    for (int i = mfptr - 1; i >= MIN_MONIX; i--) {
        monster_type *m_ptr = &m_list[i];

        if ((m_ptr->cdis <= MAX_SIGHT) && (cflag & c_list[m_ptr->mptr].cdefense) && los(char_row, char_col, (int)m_ptr->fy, (int)m_ptr->fx)) {
            creature_type *r_ptr = &c_list[m_ptr->mptr];

            c_recall[m_ptr->mptr].r_cdefense |= cflag;

            vtype out_val, m_name;
            monster_name(m_name, m_ptr, r_ptr);
            int k = mon_take_hit(i, randint(damage));

            /* Should get these messages even if the monster is not visible. */
            if (k >= 0) {
                (void)sprintf(out_val, "%s dissolves!", m_name);
            } else {
                (void)sprintf(out_val, "%s shudders.", m_name);
            }
            msg_print(out_val);

            dispel = true;

            if (k >= 0) {
                prt_experience();
            }
        }
    }

    return dispel;
}

/* Attempt to turn (confuse) undead creatures.  -RAK- */
int turn_undead() {
    bool turn_und = false;

    for (int i = mfptr - 1; i >= MIN_MONIX; i--) {
        monster_type *m_ptr = &m_list[i];
        creature_type *r_ptr = &c_list[m_ptr->mptr];

        if ((m_ptr->cdis <= MAX_SIGHT) && (CD_UNDEAD & r_ptr->cdefense) && (los(char_row, char_col, (int)m_ptr->fy, (int)m_ptr->fx))) {
            vtype out_val, m_name;
            monster_name(m_name, m_ptr, r_ptr);

            if (((py.misc.lev + 1) > r_ptr->level) || (randint(5) == 1)) {
                if (m_ptr->ml) {
                    (void)sprintf(out_val, "%s runs frantically!", m_name);
                    msg_print(out_val);
                    turn_und = true;
                    c_recall[m_ptr->mptr].r_cdefense |= CD_UNDEAD;
                }
                m_ptr->confused = py.misc.lev;
            } else if (m_ptr->ml) {
                (void)sprintf(out_val, "%s is unaffected.", m_name);
                msg_print(out_val);
            }
        }
    }

    return turn_und;
}

/* Leave a glyph of warding. Creatures will not pass over! -RAK-*/
void warding_glyph() {
    cave_type *c_ptr = &cave[char_row][char_col];

    if (c_ptr->tptr == 0) {
        int i = popt();
        c_ptr->tptr = i;
        invcopy(&t_list[i], OBJ_SCARE_MON);
    }
}

/* Lose a strength point.        -RAK- */
void lose_str() {
    if (!py.flags.sustain_str) {
        (void)dec_stat(A_STR);
        msg_print("You feel very sick.");
    } else {
        msg_print("You feel sick for a moment,  it passes.");
    }
}

/* Lose an intelligence point.        -RAK- */
void lose_int() {
    if (!py.flags.sustain_int) {
        (void)dec_stat(A_INT);
        msg_print("You become very dizzy.");
    } else {
        msg_print("You become dizzy for a moment,  it passes.");
    }
}

/* Lose a wisdom point.          -RAK- */
void lose_wis() {
    if (!py.flags.sustain_wis) {
        (void)dec_stat(A_WIS);
        msg_print("You feel very naive.");
    } else {
        msg_print("You feel naive for a moment,  it passes.");
    }
}

/* Lose a dexterity point.        -RAK- */
void lose_dex() {
    if (!py.flags.sustain_dex) {
        (void)dec_stat(A_DEX);
        msg_print("You feel very sore.");
    } else {
        msg_print("You feel sore for a moment,  it passes.");
    }
}

/* Lose a constitution point.        -RAK- */
void lose_con() {
    if (!py.flags.sustain_con) {
        (void)dec_stat(A_CON);
        msg_print("You feel very sick.");
    } else {
        msg_print("You feel sick for a moment,  it passes.");
    }
}

/* Lose a charisma point.        -RAK- */
void lose_chr() {
    if (!py.flags.sustain_chr) {
        (void)dec_stat(A_CHR);
        msg_print("Your skin starts to itch.");
    } else {
        msg_print("Your skin starts to itch, but feels better now.");
    }
}

/* Lose experience          -RAK- */
void lose_exp(int32_t amount) {
    struct misc *m_ptr = &py.misc;

    if (amount > m_ptr->exp) {
        m_ptr->exp = 0;
    } else {
        m_ptr->exp -= amount;
    }

    prt_experience();

    int i = 0;
    while ((player_exp[i] * m_ptr->expfact / 100) <= m_ptr->exp) {
        i++;
    }

    /* increment i once more, because level 1 exp is stored in player_exp[0] */
    i++;

    if (m_ptr->lev != i) {
        m_ptr->lev = i;

        calc_hitpoints();

        class_type *c_ptr = &class[m_ptr->pclass];

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

/* Slow Poison            -RAK- */
int slow_poison() {
    bool slow = false;

    struct flags *f_ptr = &py.flags;

    if (f_ptr->poisoned > 0) {
        f_ptr->poisoned = f_ptr->poisoned / 2;
        if (f_ptr->poisoned < 1) {
            f_ptr->poisoned = 1;
        }
        slow = true;
        msg_print("The effect of the poison has been reduced.");
    }

    return slow;
}

/* Bless            -RAK- */
void bless(int amount) {
    py.flags.blessed += amount;
}

/* Detect Invisible for period of time      -RAK- */
void detect_inv2(int amount) {
    py.flags.detect_inv += amount;
}

static void replace_spot(int y, int x, int typ) {
    cave_type *c_ptr = &cave[y][x];

    switch (typ) {
    case 1: case 2: case 3:
        c_ptr->fval = CORR_FLOOR;
        break;
    case 4: case 7: case 10:
        c_ptr->fval = GRANITE_WALL;
        break;
    case 5: case 8: case 11:
        c_ptr->fval = MAGMA_WALL;
        break;
    case 6: case 9: case 12:
        c_ptr->fval = QUARTZ_WALL;
        break;
    }

    c_ptr->pl = false;
    c_ptr->fm = false;
    c_ptr->lr = false; /* this is no longer part of a room */

    if (c_ptr->tptr != 0) {
        (void)delete_object(y, x);
    }

    if (c_ptr->cptr > 1) {
        delete_monster((int)c_ptr->cptr);
    }
}

/* The spell of destruction.        -RAK-
 * NOTE : Winning creatures that are deleted will be considered
 *        as teleporting to another level.  This will NOT win
 *        the game.
 */
void destroy_area(int y, int x) {
    if (dun_level > 0) {
        for (int i = (y - 15); i <= (y + 15); i++) {
            for (int j = (x - 15); j <= (x + 15); j++) {
                if (in_bounds(i, j) && (cave[i][j].fval != BOUNDARY_WALL)) {
                    int k = distance(i, j, y, x);

                    /* clear player's spot, but don't put wall there */
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

/* Enchants a plus onto an item.      -RAK-
 * `limit` param is the maximum bonus allowed; usually 10,
 * but weapon's maximum damage when enchanting melee weapons to damage.
 */
bool enchant(int16_t *plusses, int16_t limit) {
    /* avoid randint(0) call */
    if (limit <= 0) {
        return false;
    }

    int chance = 0;
    bool res = false;

    if (*plusses > 0) {
        chance = *plusses;

        /* very rarely allow enchantment over limit */
        if (randint(100) == 1) {
            chance = randint(chance) - 1;
        }
    }

    if (randint(limit) > chance) {
        *plusses += 1;
        res = true;
    }

    return res;
}

/* Removes curses from items in inventory    -RAK- */
int remove_curse() {
    bool result = false;

    for (int i = INVEN_WIELD; i <= INVEN_OUTER; i++) {
        inven_type *i_ptr = &inventory[i];

        if (TR_CURSED & i_ptr->flags) {
            i_ptr->flags &= ~TR_CURSED;
            calc_bonuses();
            result = true;
        }
    }

    return result;
}

/* Restores any drained experience      -RAK- */
int restore_level() {
    bool restore = false;

    struct misc *m_ptr = &py.misc;

    if (m_ptr->max_exp > m_ptr->exp) {
        restore = true;
        msg_print("You feel your life energies returning.");

        /* this while loop is not redundant, ptr_exp may reduce the exp level */
        while (m_ptr->exp < m_ptr->max_exp) {
            m_ptr->exp = m_ptr->max_exp;
            prt_experience();
        }
    }

    return restore;
}
