// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Misc code for maintaining the dungeon, printing player info

#include "headers.h"
#include "externs.h"

static const char *stat_names[] = {"STR : ", "INT : ", "WIS : ", "DEX : ", "CON : ", "CHR : ",};
#define BLANK_LENGTH 24
static char blank_string[] = "                        ";

// Places a particular trap at location y, x -RAK-
void place_trap(int y, int x, int subval) {
    int cur_pos = popt();
    cave[y][x].tptr = (uint8_t) cur_pos;
    invcopy(&t_list[cur_pos], OBJ_TRAP_LIST + subval);
}

// Places rubble at location y, x -RAK-
void place_rubble(int y, int x) {
    int cur_pos = popt();
    cave_type *cave_ptr = &cave[y][x];
    cave_ptr->tptr = (uint8_t) cur_pos;
    cave_ptr->fval = BLOCKED_FLOOR;
    invcopy(&t_list[cur_pos], OBJ_RUBBLE);
}

// Places a treasure (Gold or Gems) at given row, column -RAK-
void place_gold(int y, int x) {
    int cur_pos = popt();
    int i = ((randint(dun_level + 2) + 2) / 2) - 1;
    if (randint(OBJ_GREAT) == 1) {
        i += randint(dun_level + 1);
    }
    if (i >= MAX_GOLD) {
        i = MAX_GOLD - 1;
    }
    cave[y][x].tptr = (uint8_t) cur_pos;
    invcopy(&t_list[cur_pos], OBJ_GOLD_LIST + i);

    inven_type *t_ptr = &t_list[cur_pos];
    t_ptr->cost += (8L * (int32_t) randint((int) t_ptr->cost)) + randint(8);

    if (cave[y][x].cptr == 1) {
        msg_print("You feel something roll beneath your feet.");
    }
}

// Returns the array number of a random object -RAK-
int get_obj_num(int level, bool must_be_small) {
    if (level == 0) {
        return randint(t_level[0]) - 1;
    }

    if (level >= MAX_OBJ_LEVEL) {
        level = MAX_OBJ_LEVEL;
    } else if (randint(OBJ_GREAT) == 1) {
        level = level * MAX_OBJ_LEVEL / randint(MAX_OBJ_LEVEL) + 1;
        if (level > MAX_OBJ_LEVEL) {
            level = MAX_OBJ_LEVEL;
        }
    }

    int i;

    // This code has been added to make it slightly more likely to get the
    // higher level objects.  Originally a uniform distribution over all
    // objects less than or equal to the dungeon level. This distribution
    // makes a level n objects occur approx 2/n% of the time on level n,
    // and 1/2n are 0th level.
    do {
        if (randint(2) == 1) {
            i = randint(t_level[level]) - 1;
        } else {
            // Choose three objects, pick the highest level.

            i = randint(t_level[level]) - 1;
            int j = randint(t_level[level]) - 1;
            if (i < j) {
                i = j;
            }
            j = randint(t_level[level]) - 1;
            if (i < j) {
                i = j;
            }
            j = object_list[sorted_objects[i]].level;
            if (j == 0) {
                i = randint(t_level[0]) - 1;
            } else {
                i = randint(t_level[j] - t_level[j - 1]) - 1 + t_level[j - 1];
            }
        }
    } while ((must_be_small) && (set_large(&object_list[sorted_objects[i]])));

    return i;
}

// Places an object at given row, column co-ordinate -RAK-
void place_object(int y, int x, bool must_be_small) {
    int cur_pos = popt();
    cave[y][x].tptr = (uint8_t) cur_pos;

    // split this line up to avoid a reported compiler bug
    int tmp = get_obj_num(dun_level, must_be_small);
    invcopy(&t_list[cur_pos], sorted_objects[tmp]);
    magic_treasure(cur_pos, dun_level);
    if (cave[y][x].cptr == 1) {
        msg_print("You feel something roll beneath your feet."); // -CJS-
    }
}

// Allocates an object for tunnels and rooms -RAK-
void alloc_object(bool (*alloc_set)(int), int typ, int num) {
    for (int k = 0; k < num; k++) {
        int i, j;

        // don't put an object beneath the player, this could cause
        // problems if player is standing under rubble, or on a trap.
        do {
            i = randint(cur_height) - 1;
            j = randint(cur_width) - 1;
        } while (!(*alloc_set)(cave[i][j].fval) || cave[i][j].tptr != 0 || (i == char_row && j == char_col));

        // NOTE: typ == 2 is not used - used to be visible traps.
        if (typ < 4) {
            if (typ == 1) {
                // typ == 1
                place_trap(i, j, randint(MAX_TRAP) - 1);
            } else {
                // typ == 3
                place_rubble(i, j);
            }
        } else {
            if (typ == 4) {
                // typ == 4
                place_gold(i, j);
            } else {
                // typ == 5
                place_object(i, j, false);
            }
        }
    }
}

// Creates objects nearby the coordinates given -RAK-
void random_object(int y, int x, int num) {
    do {
        for (int i = 0; i <= 10; i++) {
            int j = y - 3 + randint(5);
            int k = x - 4 + randint(7);

            cave_type *cave_ptr = &cave[j][k];

            if (in_bounds(j, k) && cave_ptr->fval <= MAX_CAVE_FLOOR && cave_ptr->tptr == 0) {
                if (randint(100) < 75) {
                    place_object(j, k, false);
                } else {
                    place_gold(j, k);
                }
                i = 9;
            }
        }

        num--;
    } while (num != 0);
}

// Converts stat num into string -RAK-
void cnv_stat(uint8_t stat, char *out_val) {
    if (stat > 18) {
        int part1 = 18;
        int part2 = stat - 18;

        if (part2 == 100) {
            (void) strcpy(out_val, "18/100");
        } else {
            (void) sprintf(out_val, " %2d/%02d", part1, part2);
        }
    } else {
        (void) sprintf(out_val, "%6d", stat);
    }
}

// Print character stat in given row, column -RAK-
void prt_stat(int stat) {
    stat_type out_val1;
    cnv_stat(py.stats.use_stat[stat], out_val1);
    put_buffer(stat_names[stat], 6 + stat, STAT_COLUMN);
    put_buffer(out_val1, 6 + stat, STAT_COLUMN + 6);
}

// Print character info in given row, column -RAK-
// The longest title is 13 characters, so only pad to 13
void prt_field(const char *info, int row, int column) {
    put_buffer(&blank_string[BLANK_LENGTH - 13], row, column);
    put_buffer(info, row, column);
}

// Print long number with header at given row, column
static void prt_lnum(const char *header, int32_t num, int row, int column) {
    vtype out_val;
    (void) sprintf(out_val, "%s: %6d", header, num);
    put_buffer(out_val, row, column);
}

// Print long number (7 digits of space) with header at given row, column
static void prt_7lnum(const char *header, int32_t num, int row, int column) {
    vtype out_val;
    (void) sprintf(out_val, "%s: %7d", header, num);
    put_buffer(out_val, row, column);
}

// Print number with header at given row, column -RAK-
static void prt_num(const char *header, int num, int row, int column) {
    vtype out_val;
    (void) sprintf(out_val, "%s: %6d", header, num);
    put_buffer(out_val, row, column);
}

// Print long number at given row, column
static void prt_long(int32_t num, int row, int column) {
    vtype out_val;
    (void) sprintf(out_val, "%6d", num);
    put_buffer(out_val, row, column);
}

// Print number at given row, column -RAK-
static void prt_int(int num, int row, int column) {
    vtype out_val;
    (void) sprintf(out_val, "%6d", num);
    put_buffer(out_val, row, column);
}

// Adjustment for wisdom/intelligence -JWT-
int stat_adj(int stat) {
    int value = py.stats.use_stat[stat];

    if (value > 117) {
        return 7;
    } else if (value > 107) {
        return 6;
    } else if (value > 87) {
        return 5;
    } else if (value > 67) {
        return 4;
    } else if (value > 17) {
        return 3;
    } else if (value > 14) {
        return 2;
    } else if (value > 7) {
        return 1;
    } else {
        return 0;
    }
}

// Adjustment for charisma -RAK-
// Percent decrease or increase in price of goods
int chr_adj() {
    int charisma = py.stats.use_stat[A_CHR];

    if (charisma > 117) {
        return 90;
    } else if (charisma > 107) {
        return 92;
    } else if (charisma > 87) {
        return 94;
    } else if (charisma > 67) {
        return 96;
    } else if (charisma > 18) {
        return 98;
    } else {
        switch (charisma) {
            case 18:
                return 100;
            case 17:
                return 101;
            case 16:
                return 102;
            case 15:
                return 103;
            case 14:
                return 104;
            case 13:
                return 106;
            case 12:
                return 108;
            case 11:
                return 110;
            case 10:
                return 112;
            case 9:
                return 114;
            case 8:
                return 116;
            case 7:
                return 118;
            case 6:
                return 120;
            case 5:
                return 122;
            case 4:
                return 125;
            case 3:
                return 130;
            default:
                return 100;
        }
    }
}

// Returns a character's adjustment to hit points -JWT-
int con_adj() {
    int con = py.stats.use_stat[A_CON];

    if (con < 7) {
        return (con - 7);
    } else if (con < 17) {
        return 0;
    } else if (con == 17) {
        return 1;
    } else if (con < 94) {
        return 2;
    } else if (con < 117) {
        return 3;
    } else {
        return 4;
    }
}

char *title_string() {
    const char *p;

    if (py.misc.lev < 1) {
        p = "Babe in arms";
    } else if (py.misc.lev <= MAX_PLAYER_LEVEL) {
        p = player_title[py.misc.pclass][py.misc.lev - 1];
    } else if (py.misc.male) {
        p = "**KING**";
    } else {
        p = "**QUEEN**";
    }

    return (char *) p;
}

// Prints title of character -RAK-
void prt_title() {
    prt_field(title_string(), 4, STAT_COLUMN);
}

// Prints level -RAK-
void prt_level() {
    prt_int((int) py.misc.lev, 13, STAT_COLUMN + 6);
}

// Prints players current mana points. -RAK-
void prt_cmana() {
    prt_int(py.misc.cmana, 15, STAT_COLUMN + 6);
}

// Prints Max hit points -RAK-
void prt_mhp() {
    prt_int(py.misc.mhp, 16, STAT_COLUMN + 6);
}

// Prints players current hit points -RAK-
void prt_chp() {
    prt_int(py.misc.chp, 17, STAT_COLUMN + 6);
}

// prints current AC -RAK-
void prt_pac() {
    prt_int(py.misc.dis_ac, 19, STAT_COLUMN + 6);
}

// Prints current gold -RAK-
void prt_gold() {
    prt_long(py.misc.au, 20, STAT_COLUMN + 6);
}

// Prints depth in stat area -RAK-
void prt_depth() {
    vtype depths;

    int depth = dun_level * 50;

    if (depth == 0) {
        (void) strcpy(depths, "Town level");
    } else {
        (void) sprintf(depths, "%d feet", depth);
    }

    prt(depths, 23, 65);
}

// Prints status of hunger -RAK-
void prt_hunger() {
    if (PY_WEAK & py.flags.status) {
        put_buffer("Weak  ", 23, 0);
    } else if (PY_HUNGRY & py.flags.status) {
        put_buffer("Hungry", 23, 0);
    } else {
        put_buffer(&blank_string[BLANK_LENGTH - 6], 23, 0);
    }
}

// Prints Blind status -RAK-
void prt_blind() {
    if (PY_BLIND & py.flags.status) {
        put_buffer("Blind", 23, 7);
    } else {
        put_buffer(&blank_string[BLANK_LENGTH - 5], 23, 7);
    }
}

// Prints Confusion status -RAK-
void prt_confused() {
    if (PY_CONFUSED & py.flags.status) {
        put_buffer("Confused", 23, 13);
    } else {
        put_buffer(&blank_string[BLANK_LENGTH - 8], 23, 13);
    }
}

// Prints Fear status -RAK-
void prt_afraid() {
    if (PY_FEAR & py.flags.status) {
        put_buffer("Afraid", 23, 22);
    } else {
        put_buffer(&blank_string[BLANK_LENGTH - 6], 23, 22);
    }
}

// Prints Poisoned status -RAK-
void prt_poisoned() {
    if (PY_POISONED & py.flags.status) {
        put_buffer("Poisoned", 23, 29);
    } else {
        put_buffer(&blank_string[BLANK_LENGTH - 8], 23, 29);
    }
}

// Prints Searching, Resting, Paralysis, or 'count' status -RAK-
void prt_state() {
    py.flags.status &= ~PY_REPEAT;

    if (py.flags.paralysis > 1) {
        put_buffer("Paralysed", 23, 38);
    } else if (PY_REST & py.flags.status) {
        char tmp[16];

        if (py.flags.rest < 0) {
            (void) strcpy(tmp, "Rest *");
        } else if (display_counts) {
            (void) sprintf(tmp, "Rest %-5d", py.flags.rest);
        } else {
            (void) strcpy(tmp, "Rest");
        }
        put_buffer(tmp, 23, 38);
    } else if (command_count > 0) {
        char tmp[16];

        if (display_counts) {
            (void) sprintf(tmp, "Repeat %-3d", command_count);
        } else {
            (void) strcpy(tmp, "Repeat");
        }

        py.flags.status |= PY_REPEAT;

        put_buffer(tmp, 23, 38);
        if (PY_SEARCH & py.flags.status) {
            put_buffer("Search", 23, 38);
        }
    } else if (PY_SEARCH & py.flags.status) {
        put_buffer("Searching", 23, 38);
    } else {
        // "repeat 999" is 10 characters
        put_buffer(&blank_string[BLANK_LENGTH - 10], 23, 38);
    }
}

// Prints the speed of a character. -CJS-
void prt_speed() {
    int i = py.flags.speed;

    // Search mode.
    if (PY_SEARCH & py.flags.status) {
        i--;
    }

    if (i > 1) {
        put_buffer("Very Slow", 23, 49);
    } else if (i == 1) {
        put_buffer("Slow     ", 23, 49);
    } else if (i == 0) {
        put_buffer(&blank_string[BLANK_LENGTH - 9], 23, 49);
    } else if (i == -1) {
        put_buffer("Fast     ", 23, 49);
    } else {
        put_buffer("Very Fast", 23, 49);
    }
}

void prt_study() {
    py.flags.status &= ~PY_STUDY;

    if (py.flags.new_spells == 0) {
        put_buffer(&blank_string[BLANK_LENGTH - 5], 23, 59);
    } else {
        put_buffer("Study", 23, 59);
    }
}

// Prints winner status on display -RAK-
void prt_winner() {
    if (noscore & 0x2) {
        if (wizard) {
            put_buffer("Is wizard  ", 22, 0);
        } else {
            put_buffer("Was wizard ", 22, 0);
        }
    } else if (noscore & 0x1) {
        put_buffer("Resurrected", 22, 0);
    } else if (noscore & 0x4) {
        put_buffer("Duplicate", 22, 0);
    } else if (total_winner) {
        put_buffer("*Winner*   ", 22, 0);
    }
}

uint8_t modify_stat(int stat, int16_t amount) {
    uint8_t tmp_stat = py.stats.cur_stat[stat];
    int loop = (amount < 0 ? -amount : amount);

    for (int i = 0; i < loop; i++) {
        if (amount > 0) {
            if (tmp_stat < 18) {
                tmp_stat++;
            } else if (tmp_stat < 108) {
                tmp_stat += 10;
            } else {
                tmp_stat = 118;
            }
        } else {
            if (tmp_stat > 27) {
                tmp_stat -= 10;
            } else if (tmp_stat > 18) {
                tmp_stat = 18;
            } else if (tmp_stat > 3) {
                tmp_stat--;
            }
        }
    }

    return tmp_stat;
}

// Set the value of the stat which is actually used. -CJS-
void set_use_stat(int stat) {
    py.stats.use_stat[stat] = modify_stat(stat, py.stats.mod_stat[stat]);

    if (stat == A_STR) {
        py.flags.status |= PY_STR_WGT;
        calc_bonuses();
    } else if (stat == A_DEX) {
        calc_bonuses();
    } else if (stat == A_INT && classes[py.misc.pclass].spell == MAGE) {
        calc_spells(A_INT);
        calc_mana(A_INT);
    } else if (stat == A_WIS && classes[py.misc.pclass].spell == PRIEST) {
        calc_spells(A_WIS);
        calc_mana(A_WIS);
    } else if (stat == A_CON) {
        calc_hitpoints();
    }
}

// Increases a stat by one randomized level -RAK-
bool inc_stat(int stat) {
    int tmp_stat = py.stats.cur_stat[stat];
    if (tmp_stat < 118) {
        if (tmp_stat < 18) {
            tmp_stat++;
        } else if (tmp_stat < 116) {
            // stat increases by 1/6 to 1/3 of difference from max
            int gain = ((118 - tmp_stat) / 3 + 1) >> 1;
            tmp_stat += randint(gain) + gain;
        } else {
            tmp_stat++;
        }

        py.stats.cur_stat[stat] = (uint8_t) tmp_stat;

        if (tmp_stat > py.stats.max_stat[stat]) {
            py.stats.max_stat[stat] = (uint8_t) tmp_stat;
        }
        set_use_stat(stat);
        prt_stat(stat);
        return true;
    }

    return false;
}

// Decreases a stat by one randomized level -RAK-
bool dec_stat(int stat) {
    int tmp_stat = py.stats.cur_stat[stat];
    if (tmp_stat > 3) {
        if (tmp_stat < 19) {
            tmp_stat--;
        } else if (tmp_stat < 117) {
            int loss = (((118 - tmp_stat) >> 1) + 1) >> 1;
            tmp_stat += -randint(loss) - loss;
            if (tmp_stat < 18) {
                tmp_stat = 18;
            }
        } else {
            tmp_stat--;
        }

        py.stats.cur_stat[stat] = (uint8_t) tmp_stat;
        set_use_stat(stat);
        prt_stat(stat);
        return true;
    }

    return false;
}

// Restore a stat.  Return true only if this actually makes a difference.
bool res_stat(int stat) {
    int i = py.stats.max_stat[stat] - py.stats.cur_stat[stat];

    if (i) {
        py.stats.cur_stat[stat] += i;
        set_use_stat(stat);
        prt_stat(stat);
        return true;
    }

    return false;
}

// Boost a stat artificially (by wearing something). If the display
// argument is true, then increase is shown on the screen.
void bst_stat(int stat, int amount) {
    py.stats.mod_stat[stat] += amount;

    set_use_stat(stat);

    // can not call prt_stat() here, may be in store, may be in inven_command
    py.flags.status |= (PY_STR << stat);
}

// Returns a character's adjustment to hit. -JWT-
int tohit_adj() {
    int total;

    int stat = py.stats.use_stat[A_DEX];
    if (stat < 4) {
        total = -3;
    } else if (stat < 6) {
        total = -2;
    } else if (stat < 8) {
        total = -1;
    } else if (stat < 16) {
        total = 0;
    } else if (stat < 17) {
        total = 1;
    } else if (stat < 18) {
        total = 2;
    } else if (stat < 69) {
        total = 3;
    } else if (stat < 118) {
        total = 4;
    } else {
        total = 5;
    }

    stat = py.stats.use_stat[A_STR];
    if (stat < 4) {
        total -= 3;
    } else if (stat < 5) {
        total -= 2;
    } else if (stat < 7) {
        total -= 1;
    } else if (stat < 18) {
        total -= 0;
    } else if (stat < 94) {
        total += 1;
    } else if (stat < 109) {
        total += 2;
    } else if (stat < 117) {
        total += 3;
    } else {
        total += 4;
    }

    return total;
}

// Returns a character's adjustment to armor class -JWT-
int toac_adj() {
    int stat = py.stats.use_stat[A_DEX];

    if (stat < 4) {
        return -4;
    } else if (stat == 4) {
        return -3;
    } else if (stat == 5) {
        return -2;
    } else if (stat == 6) {
        return -1;
    } else if (stat < 15) {
        return 0;
    } else if (stat < 18) {
        return 1;
    } else if (stat < 59) {
        return 2;
    } else if (stat < 94) {
        return 3;
    } else if (stat < 117) {
        return 4;
    } else {
        return 5;
    }
}

// Returns a character's adjustment to disarm -RAK-
int todis_adj() {
    int stat = py.stats.use_stat[A_DEX];

    if (stat < 4) {
        return -8;
    } else if (stat == 4) {
        return -6;
    } else if (stat == 5) {
        return -4;
    } else if (stat == 6) {
        return -2;
    } else if (stat == 7) {
        return -1;
    } else if (stat < 13) {
        return 0;
    } else if (stat < 16) {
        return 1;
    } else if (stat < 18) {
        return 2;
    } else if (stat < 59) {
        return 4;
    } else if (stat < 94) {
        return 5;
    } else if (stat < 117) {
        return 6;
    } else {
        return 8;
    }
}

// Returns a character's adjustment to damage -JWT-
int todam_adj() {
    int stat = py.stats.use_stat[A_STR];

    if (stat < 4) {
        return -2;
    } else if (stat < 5) {
        return -1;
    } else if (stat < 16) {
        return 0;
    } else if (stat < 17) {
        return 1;
    } else if (stat < 18) {
        return 2;
    } else if (stat < 94) {
        return 3;
    } else if (stat < 109) {
        return 4;
    } else if (stat < 117) {
        return 5;
    } else {
        return 6;
    }
}

// Prints character-screen info -RAK-
void prt_stat_block() {
    struct player_type::misc *m_ptr = &py.misc;
    prt_field(race[py.misc.prace].trace, 2, STAT_COLUMN);
    prt_field(classes[py.misc.pclass].title, 3, STAT_COLUMN);
    prt_field(title_string(), 4, STAT_COLUMN);

    for (int i = 0; i < 6; i++) {
        prt_stat(i);
    }

    prt_num("LEV ", (int) m_ptr->lev, 13, STAT_COLUMN);
    prt_lnum("EXP ", m_ptr->exp, 14, STAT_COLUMN);
    prt_num("MANA", m_ptr->cmana, 15, STAT_COLUMN);
    prt_num("MHP ", m_ptr->mhp, 16, STAT_COLUMN);
    prt_num("CHP ", m_ptr->chp, 17, STAT_COLUMN);
    prt_num("AC  ", m_ptr->dis_ac, 19, STAT_COLUMN);
    prt_lnum("GOLD", m_ptr->au, 20, STAT_COLUMN);
    prt_winner();

    uint32_t status = py.flags.status;

    if ((PY_HUNGRY | PY_WEAK) & status) {
        prt_hunger();
    }
    if (PY_BLIND & status) {
        prt_blind();
    }
    if (PY_CONFUSED & status) {
        prt_confused();
    }
    if (PY_FEAR & status) {
        prt_afraid();
    }
    if (PY_POISONED & status) {
        prt_poisoned();
    }
    if ((PY_SEARCH | PY_REST) & status) {
        prt_state();
    }

    // if speed non zero, print it, modify speed if Searching
    if (py.flags.speed - ((PY_SEARCH & status) >> 8) != 0) {
        prt_speed();
    }

    // display the study field
    prt_study();
}

// Draws entire screen -RAK-
void draw_cave() {
    clear_screen();
    prt_stat_block();
    prt_map();
    prt_depth();
}

// Prints the following information on the screen. -JWT-
void put_character() {
    struct player_type::misc *m_ptr = &py.misc;

    clear_screen();

    put_buffer("Name        :", 2, 1);
    put_buffer("Race        :", 3, 1);
    put_buffer("Sex         :", 4, 1);
    put_buffer("Class       :", 5, 1);

    if (character_generated) {
        put_buffer(m_ptr->name, 2, 15);
        put_buffer(race[m_ptr->prace].trace, 3, 15);
        put_buffer((m_ptr->male ? "Male" : "Female"), 4, 15);
        put_buffer(classes[m_ptr->pclass].title, 5, 15);
    }
}

// Prints the following information on the screen. -JWT-
void put_stats() {
    struct player_type::misc *m_ptr = &py.misc;

    for (int i = 0; i < 6; i++) {
        vtype buf;

        cnv_stat(py.stats.use_stat[i], buf);
        put_buffer(stat_names[i], 2 + i, 61);
        put_buffer(buf, 2 + i, 66);
        if (py.stats.max_stat[i] > py.stats.cur_stat[i]) {
            cnv_stat(py.stats.max_stat[i], buf);
            put_buffer(buf, 2 + i, 73);
        }
    }

    prt_num("+ To Hit    ", m_ptr->dis_th, 9, 1);
    prt_num("+ To Damage ", m_ptr->dis_td, 10, 1);
    prt_num("+ To AC     ", m_ptr->dis_tac, 11, 1);
    prt_num("  Total AC  ", m_ptr->dis_ac, 12, 1);
}

// Returns a rating of x depending on y -JWT-
const char *likert(int x, int y) {
    switch ((x / y)) {
        case -3:
        case -2:
        case -1:
            return "Very Bad";
        case 0:
        case 1:
            return "Bad";
        case 2:
            return "Poor";
        case 3:
        case 4:
            return "Fair";
        case 5:
            return "Good";
        case 6:
            return "Very Good";
        case 7:
        case 8:
            return "Excellent";
        default:
            return "Superb";
    }
}

// Prints age, height, weight, and SC -JWT-
void put_misc1() {
    struct player_type::misc *m_ptr = &py.misc;

    prt_num("Age          ", (int) m_ptr->age, 2, 38);
    prt_num("Height       ", (int) m_ptr->ht, 3, 38);
    prt_num("Weight       ", (int) m_ptr->wt, 4, 38);
    prt_num("Social Class ", (int) m_ptr->sc, 5, 38);
}

// Prints the following information on the screen. -JWT-
void put_misc2() {
    struct player_type::misc *m_ptr = &py.misc;

    prt_7lnum("Level      ", (int32_t) m_ptr->lev, 9, 28);
    prt_7lnum("Experience ", m_ptr->exp, 10, 28);
    prt_7lnum("Max Exp    ", m_ptr->max_exp, 11, 28);

    if (m_ptr->lev >= MAX_PLAYER_LEVEL) {
        prt("Exp to Adv.: *******", 12, 28);
    } else {
        prt_7lnum("Exp to Adv.", (int32_t) (player_exp[m_ptr->lev - 1] * m_ptr->expfact / 100), 12, 28);
    }

    prt_7lnum("Gold       ", m_ptr->au, 13, 28);
    prt_num("Max Hit Points ", m_ptr->mhp, 9, 52);
    prt_num("Cur Hit Points ", m_ptr->chp, 10, 52);
    prt_num("Max Mana       ", m_ptr->mana, 11, 52);
    prt_num("Cur Mana       ", m_ptr->cmana, 12, 52);
}

// Prints ratings on certain abilities -RAK-
void put_misc3() {
    clear_from(14);

    struct player_type::misc *p_ptr = &py.misc;
    int xbth = p_ptr->bth + p_ptr->ptohit * BTH_PLUS_ADJ + (class_level_adj[p_ptr->pclass][CLA_BTH] * p_ptr->lev);
    int xbthb = p_ptr->bthb + p_ptr->ptohit * BTH_PLUS_ADJ + (class_level_adj[p_ptr->pclass][CLA_BTHB] * p_ptr->lev);

    // this results in a range from 0 to 29
    int xfos = 40 - p_ptr->fos;
    if (xfos < 0) {
        xfos = 0;
    }

    int xsrh = p_ptr->srh;

    // this results in a range from 0 to 9
    int xstl = p_ptr->stl + 1;
    int xdis = p_ptr->disarm + 2 * todis_adj() + stat_adj(A_INT) + (class_level_adj[p_ptr->pclass][CLA_DISARM] * p_ptr->lev / 3);
    int xsave = p_ptr->save + stat_adj(A_WIS) + (class_level_adj[p_ptr->pclass][CLA_SAVE] * p_ptr->lev / 3);
    int xdev = p_ptr->save + stat_adj(A_INT) + (class_level_adj[p_ptr->pclass][CLA_DEVICE] * p_ptr->lev / 3);

    vtype xinfra;
    (void) sprintf(xinfra, "%d feet", py.flags.see_infra * 10);

    put_buffer("(Miscellaneous Abilities)", 15, 25);
    put_buffer("Fighting    :", 16, 1);
    put_buffer(likert(xbth, 12), 16, 15);
    put_buffer("Bows/Throw  :", 17, 1);
    put_buffer(likert(xbthb, 12), 17, 15);
    put_buffer("Saving Throw:", 18, 1);
    put_buffer(likert(xsave, 6), 18, 15);

    put_buffer("Stealth     :", 16, 28);
    put_buffer(likert(xstl, 1), 16, 42);
    put_buffer("Disarming   :", 17, 28);
    put_buffer(likert(xdis, 8), 17, 42);
    put_buffer("Magic Device:", 18, 28);
    put_buffer(likert(xdev, 6), 18, 42);

    put_buffer("Perception  :", 16, 55);
    put_buffer(likert(xfos, 3), 16, 69);
    put_buffer("Searching   :", 17, 55);
    put_buffer(likert(xsrh, 6), 17, 69);
    put_buffer("Infra-Vision:", 18, 55);
    put_buffer(xinfra, 18, 69);
}

// Used to display the character on the screen. -RAK-
void display_char() {
    put_character();
    put_misc1();
    put_stats();
    put_misc2();
    put_misc3();
}

// Gets a name for the character -JWT-
void get_name() {
    prt("Enter your player's name  [press <RETURN> when finished]", 21, 2);
    put_buffer(&blank_string[BLANK_LENGTH - 23], 2, 15);

    if (!get_string(py.misc.name, 2, 15, 23) || py.misc.name[0] == 0) {
        user_name(py.misc.name);
        put_buffer(py.misc.name, 2, 15);
    }

    clear_from(20);
}

// Changes the name of the character -JWT-
void change_name() {
    vtype temp;

    display_char();

    bool flag = false;
    while (!flag) {
        prt("<f>ile character description. <c>hange character name.", 21, 2);
        char c = inkey();
        switch (c) {
            case 'c':
                get_name();
                flag = true;
                break;
            case 'f':
                prt("File name:", 0, 0);
                if (get_string(temp, 0, 10, 60) && temp[0]) {
                    if (file_character(temp)) {
                        flag = true;
                    }
                }
                break;
            case ESCAPE:
            case ' ':
            case '\n':
            case '\r':
                flag = true;
                break;
            default:
                bell();
                break;
        }
    }
}

// Destroy an item in the inventory -RAK-
void inven_destroy(int item_val) {
    inven_type *i_ptr = &inventory[item_val];

    if (i_ptr->number > 1 && i_ptr->subval <= ITEM_SINGLE_STACK_MAX) {
        i_ptr->number--;
        inven_weight -= i_ptr->weight;
    } else {
        inven_weight -= i_ptr->weight * i_ptr->number;
        for (int j = item_val; j < inven_ctr - 1; j++) {
            inventory[j] = inventory[j + 1];
        }
        invcopy(&inventory[inven_ctr - 1], OBJ_NOTHING);
        inven_ctr--;
    }
    py.flags.status |= PY_STR_WGT;
}

// Copies the object in the second argument over the first argument.
// However, the second always gets a number of one except for ammo etc.
void take_one_item(inven_type *s_ptr, inven_type *i_ptr) {
    *s_ptr = *i_ptr;
    if (s_ptr->number > 1 && s_ptr->subval >= ITEM_SINGLE_STACK_MIN && s_ptr->subval <= ITEM_SINGLE_STACK_MAX) {
        s_ptr->number = 1;
    }
}

// Drops an item from inventory to given location -RAK-
void inven_drop(int item_val, int drop_all) {
    if (cave[char_row][char_col].tptr != 0) {
        (void) delete_object(char_row, char_col);
    }

    int i = popt();
    inven_type *i_ptr = &inventory[item_val];
    t_list[i] = *i_ptr;
    cave[char_row][char_col].tptr = (uint8_t) i;

    if (item_val >= INVEN_WIELD) {
        takeoff(item_val, -1);
    } else {
        if (drop_all || i_ptr->number == 1) {
            inven_weight -= i_ptr->weight * i_ptr->number;
            inven_ctr--;
            while (item_val < inven_ctr) {
                inventory[item_val] = inventory[item_val + 1];
                item_val++;
            }
            invcopy(&inventory[inven_ctr], OBJ_NOTHING);
        } else {
            t_list[i].number = 1;
            inven_weight -= i_ptr->weight;
            i_ptr->number--;
        }

        bigvtype prt1, prt2;
        objdes(prt1, &t_list[i], true);
        (void) sprintf(prt2, "Dropped %s", prt1);
        msg_print(prt2);
    }
    py.flags.status |= PY_STR_WGT;
}

// Destroys a type of item on a given percent chance -RAK-
int inven_damage(bool (*typ)(inven_type *), int perc) {
    int j = 0;

    for (int i = 0; i < inven_ctr; i++) {
        if ((*typ)(&inventory[i]) && randint(100) < perc) {
            inven_destroy(i);
            j++;
        }
    }

    return j;
}

// Computes current weight limit -RAK-
int weight_limit() {
    int weight_cap = py.stats.use_stat[A_STR] * PLAYER_WEIGHT_CAP + py.misc.wt;

    if (weight_cap > 3000) {
        weight_cap = 3000;
    }

    return weight_cap;
}

// this code must be identical to the inven_carry() code below
bool inven_check_num(inven_type *t_ptr) {
    if (inven_ctr < INVEN_WIELD) {
        return true;
    }

    if (t_ptr->subval >= ITEM_SINGLE_STACK_MIN) {
        for (int i = 0; i < inven_ctr; i++) {
            if (inventory[i].tval == t_ptr->tval && inventory[i].subval == t_ptr->subval &&
                // make sure the number field doesn't overflow
                ((int) inventory[i].number + (int) t_ptr->number) < 256 &&
                // they always stack (subval < 192), or else they have same p1
                (t_ptr->subval < ITEM_GROUP_MIN || inventory[i].p1 == t_ptr->p1) &&
                // only stack if both or neither are identified
                known1_p(&inventory[i]) == known1_p(t_ptr)) {
                return true;
            }
        }
    }

    return false;
}

// return false if picking up an object would change the players speed
bool inven_check_weight(inven_type *i_ptr) {
    int i = weight_limit();
    int new_inven_weight = i_ptr->number * i_ptr->weight + inven_weight;

    if (i < new_inven_weight) {
        i = new_inven_weight / (i + 1);
    } else {
        i = 0;
    }

    return (pack_heavy == i);
}

// Are we strong enough for the current pack and weapon? -CJS-
void check_strength() {
    inven_type *i_ptr = &inventory[INVEN_WIELD];

    if (i_ptr->tval != TV_NOTHING && py.stats.use_stat[A_STR] * 15 < i_ptr->weight) {
        if (!weapon_heavy) {
            msg_print("You have trouble wielding such a heavy weapon.");
            weapon_heavy = true;
            calc_bonuses();
        }
    } else if (weapon_heavy) {
        weapon_heavy = false;
        if (i_ptr->tval != TV_NOTHING) {
            msg_print("You are strong enough to wield your weapon.");
        }
        calc_bonuses();
    }

    int i = weight_limit();
    if (i < inven_weight) {
        i = inven_weight / (i + 1);
    } else {
        i = 0;
    }

    if (pack_heavy != i) {
        if (pack_heavy < i) {
            msg_print("Your pack is so heavy that it slows you down.");
        } else {
            msg_print("You move more easily under the weight of your pack.");
        }
        change_speed(i - pack_heavy);
        pack_heavy = i;
    }
    py.flags.status &= ~PY_STR_WGT;
}

// Add an item to players inventory.  Return the
// item position for a description if needed. -RAK-
// this code must be identical to the inven_check_num() code above
int inven_carry(inven_type *i_ptr) {
    int typ = i_ptr->tval;
    int subt = i_ptr->subval;
    bool known1p = known1_p(i_ptr);
    int always_known1p = (object_offset(i_ptr) == -1);

    int locn;

    // Now, check to see if player can carry object
    for (locn = 0;; locn++) {
        inven_type *t_ptr = &inventory[locn];

        if (typ == t_ptr->tval &&
            subt == t_ptr->subval && subt >= ITEM_SINGLE_STACK_MIN &&
            ((int) t_ptr->number + (int) i_ptr->number) < 256 &&
            (subt < ITEM_GROUP_MIN || t_ptr->p1 == i_ptr->p1) &&
            // only stack if both or neither are identified
            known1p == known1_p(t_ptr)) {
            t_ptr->number += i_ptr->number;
            break;
        } else if ((typ == t_ptr->tval && subt < t_ptr->subval && always_known1p) || typ > t_ptr->tval) {
            // For items which are always known1p, i.e. never have a 'color',
            // insert them into the inventory in sorted order.

            for (int i = inven_ctr - 1; i >= locn; i--) {
                inventory[i + 1] = inventory[i];
            }
            inventory[locn] = *i_ptr;
            inven_ctr++;
            break;
        }
    }

    inven_weight += i_ptr->number * i_ptr->weight;
    py.flags.status |= PY_STR_WGT;

    return locn;
}

// Returns spell chance of failure for spell -RAK-
int spell_chance(int spell) {
    spell_type *s_ptr = &magic_spell[py.misc.pclass - 1][spell];

    int stat;
    int chance = s_ptr->sfail - 3 * (py.misc.lev - s_ptr->slevel);

    if (classes[py.misc.pclass].spell == MAGE) {
        stat = A_INT;
    } else {
        stat = A_WIS;
    }

    chance -= 3 * (stat_adj(stat) - 1);
    if (s_ptr->smana > py.misc.cmana) {
        chance += 5 * (s_ptr->smana - py.misc.cmana);
    }

    if (chance > 95) {
        chance = 95;
    } else if (chance < 5) {
        chance = 5;
    }

    return chance;
}

// Print list of spells -RAK-
// if nonconsec is  -1: spells numbered consecutively from 'a' to 'a'+num
//                 >=0: spells numbered by offset from nonconsec
void print_spells(int *spell, int num, int comment, int nonconsec) {
    int col;
    if (comment) {
        col = 22;
    } else {
        col = 31;
    }

    int offset = (classes[py.misc.pclass].spell == MAGE ? SPELL_OFFSET : PRAYER_OFFSET);

    erase_line(1, col);
    put_buffer("Name", 1, col + 5);
    put_buffer("Lv Mana Fail", 1, col + 35);

    // only show the first 22 choices
    if (num > 22) {
        num = 22;
    }

    for (int i = 0; i < num; i++) {
        int j = spell[i];
        spell_type *s_ptr = &magic_spell[py.misc.pclass - 1][j];

        const char *p;
        if (comment == 0) {
            p = "";
        } else if ((spell_forgotten & (1L << j)) != 0) {
            p = " forgotten";
        } else if ((spell_learned & (1L << j)) == 0) {
            p = " unknown";
        } else if ((spell_worked & (1L << j)) == 0) {
            p = " untried";
        } else {
            p = "";
        }

        // determine whether or not to leave holes in character choices, nonconsec -1
        // when learning spells, consec offset>=0 when asking which spell to cast.
        char spell_char;
        if (nonconsec == -1) {
            spell_char = (char) ('a' + i);
        } else {
            spell_char = (char) ('a' + j - nonconsec);
        }

        vtype out_val;
        (void) sprintf(out_val, "  %c) %-30s%2d %4d %3d%%%s", spell_char, spell_names[j + offset], s_ptr->slevel, s_ptr->smana, spell_chance(j), p);
        prt(out_val, 2 + i, col);
    }
}

// Returns spell pointer -RAK-
int get_spell(int *spell, int num, int *sn, int *sc, const char *prompt, int first_spell) {
    *sn = -1;

    vtype out_str;
    (void) sprintf(out_str, "(Spells %c-%c, *=List, <ESCAPE>=exit) %s", spell[0] + 'a' - first_spell, spell[num - 1] + 'a' - first_spell, prompt);

    bool flag = false;
    bool redraw = false;

    int offset = (classes[py.misc.pclass].spell == MAGE ? SPELL_OFFSET : PRAYER_OFFSET);

    char choice;
    while (!flag && get_com(out_str, &choice)) {
        if (isupper((int) choice)) {
            *sn = choice - 'A' + first_spell;

            // verify that this is in spell[], at most 22 entries in spell[]
            int i;
            for (i = 0; i < num; i++) {
                if (*sn == spell[i]) {
                    break;
                }
            }
            if (i == num) {
                *sn = -2;
            } else {
                spell_type *s_ptr = &magic_spell[py.misc.pclass - 1][*sn];

                vtype tmp_str;
                (void) sprintf(tmp_str, "Cast %s (%d mana, %d%% fail)?", spell_names[*sn + offset], s_ptr->smana, spell_chance(*sn));
                if (get_check(tmp_str)) {
                    flag = true;
                } else {
                    *sn = -1;
                }
            }
        } else if (islower((int) choice)) {
            *sn = choice - 'a' + first_spell;

            // verify that this is in spell[], at most 22 entries in spell[]
            int i;
            for (i = 0; i < num; i++) {
                if (*sn == spell[i]) {
                    break;
                }
            }
            if (i == num) {
                *sn = -2;
            } else {
                flag = true;
            }
        } else if (choice == '*') {
            // only do this drawing once
            if (!redraw) {
                save_screen();
                redraw = true;
                print_spells(spell, num, false, first_spell);
            }
        } else if (isalpha((int) choice)) {
            *sn = -2;
        } else {
            *sn = -1;
            bell();
        }
        if (*sn == -2) {
            vtype tmp_str;
            (void) sprintf(tmp_str, "You don't know that %s.", (offset == SPELL_OFFSET ? "spell" : "prayer"));
            msg_print(tmp_str);
        }
    }
    if (redraw) {
        restore_screen();
    }

    erase_line(MSG_LINE, 0);
    if (flag) {
        *sc = spell_chance(*sn);
    }

    return flag;
}

// calculate number of spells player should have, and
// learn forget spells until that number is met -JEW-
void calc_spells(int stat) {
    struct player_type::misc *p_ptr = &py.misc;
    spell_type *msp_ptr = &magic_spell[p_ptr->pclass - 1][0];

    const char *p;
    int offset;
    if (stat == A_INT) {
        p = "spell";
        offset = SPELL_OFFSET;
    } else {
        p = "prayer";
        offset = PRAYER_OFFSET;
    }

    // check to see if know any spells greater than level, eliminate them
    int i;
    uint32_t mask;
    for (i = 31, mask = 0x80000000L; mask; mask >>= 1, i--) {
        if (mask & spell_learned) {
            if (msp_ptr[i].slevel > p_ptr->lev) {
                spell_learned &= ~mask;
                spell_forgotten |= mask;

                vtype tmp_str;
                (void) sprintf(tmp_str, "You have forgotten the %s of %s.", p, spell_names[i + offset]);
                msg_print(tmp_str);
            } else {
                break;
            }
        }
    }

    // calc number of spells allowed
    int num_allowed = 0;
    int levels = p_ptr->lev - classes[p_ptr->pclass].first_spell_lev + 1;
    switch (stat_adj(stat)) {
        case 0:
            num_allowed = 0;
            break;
        case 1:
        case 2:
        case 3:
            num_allowed = 1 * levels;
            break;
        case 4:
        case 5:
            num_allowed = 3 * levels / 2;
            break;
        case 6:
            num_allowed = 2 * levels;
            break;
        case 7:
            num_allowed = 5 * levels / 2;
            break;
    }

    int num_known = 0;
    for (mask = 0x1; mask; mask <<= 1) {
        if (mask & spell_learned) {
            num_known++;
        }
    }
    int new_spells = num_allowed - num_known;

    if (new_spells > 0) {
        // remember forgotten spells while forgotten spells exist of new_spells
        // positive, remember the spells in the order that they were learned
        for (int n = 0; (spell_forgotten && new_spells && (n < num_allowed) && (n < 32)); n++) {
            // j is (i+1)th spell learned
            int j = spell_order[n];

            // shifting by amounts greater than number of bits in long gives
            // an undefined result, so don't shift for unknown spells
            if (j == 99) {
                mask = 0x0;
            } else {
                mask = (uint32_t) (1L << j);
            }
            if (mask & spell_forgotten) {
                if (msp_ptr[j].slevel <= p_ptr->lev) {
                    new_spells--;
                    spell_forgotten &= ~mask;
                    spell_learned |= mask;

                    vtype tmp_str;
                    (void) sprintf(tmp_str, "You have remembered the %s of %s.", p, spell_names[j + offset]);
                    msg_print(tmp_str);
                } else {
                    num_allowed++;
                }
            }
        }

        if (new_spells > 0) {
            // determine which spells player can learn must check all spells here,
            // in gain_spell() we actually check if the books are present
            uint32_t spell_flag = (uint32_t) (0x7FFFFFFFL & ~spell_learned);

            int j;
            int id = 0;
            for (j = 0, mask = 0x1; spell_flag; mask <<= 1, j++) {
                if (spell_flag & mask) {
                    spell_flag &= ~mask;
                    if (msp_ptr[j].slevel <= p_ptr->lev) {
                        id++;
                    }
                }
            }

            if (new_spells > id) {
                new_spells = id;
            }
        }
    } else if (new_spells < 0) {
        // forget spells until new_spells zero or no more spells know, spells
        // are forgotten in the opposite order that they were learned
        for (i = 31; new_spells && spell_learned; i--) {
            // j is the (i+1)th spell learned
            int j = spell_order[i];
            // shifting by amounts greater than number of bits in long gives
            // an undefined result, so don't shift for unknown spells
            if (j == 99) {
                mask = 0x0;
            } else {
                mask = (uint32_t) (1L << j);
            }
            if (mask & spell_learned) {
                spell_learned &= ~mask;
                spell_forgotten |= mask;
                new_spells++;

                vtype tmp_str;
                (void) sprintf(tmp_str, "You have forgotten the %s of %s.", p, spell_names[j + offset]);
                msg_print(tmp_str);
            }
        }

        new_spells = 0;
    }

    if (new_spells != py.flags.new_spells) {
        if (new_spells > 0 && py.flags.new_spells == 0) {
            vtype tmp_str;
            (void) sprintf(tmp_str, "You can learn some new %ss now.", p);
            msg_print(tmp_str);
        }

        py.flags.new_spells = (uint8_t) new_spells;
        py.flags.status |= PY_STUDY;
    }
}

// gain spells when player wants to    - jw
void gain_spells() {
    uint32_t spell_flag;

    // Priests don't need light because they get spells from their god, so only
    // fail when can't see if player has MAGE spells. This check is done below.
    if (py.flags.confused > 0) {
        msg_print("You are too confused.");
        return;
    }

    int new_spells = py.flags.new_spells;
    int diff_spells = 0;

    struct player_type::misc *p_ptr = &py.misc;
    spell_type *msp_ptr = &magic_spell[p_ptr->pclass - 1][0];

    int stat, offset;
    if (classes[p_ptr->pclass].spell == MAGE) {
        stat = A_INT;
        offset = SPELL_OFFSET;

        // People with MAGE spells can't learn spells if they can't read their books.
        if (py.flags.blind > 0) {
            msg_print("You can't see to read your spell book!");
            return;
        } else if (no_light()) {
            msg_print("You have no light to read by.");
            return;
        }
    } else {
        stat = A_WIS;
        offset = PRAYER_OFFSET;
    }

    int last_known;
    for (last_known = 0; last_known < 32; last_known++) {
        if (spell_order[last_known] == 99) {
            break;
        }
    }

    if (!new_spells) {
        vtype tmp_str;
        (void) sprintf(tmp_str, "You can't learn any new %ss!", (stat == A_INT ? "spell" : "prayer"));
        msg_print(tmp_str);
        free_turn_flag = true;
        return;
    }

    // determine which spells player can learn
    // mages need the book to learn a spell, priests do not need the book
    if (stat == A_INT) {
        spell_flag = 0;
        for (int i = 0; i < inven_ctr; i++) {
            if (inventory[i].tval == TV_MAGIC_BOOK) {
                spell_flag |= inventory[i].flags;
            }
        }
    } else {
        spell_flag = 0x7FFFFFFF;
    }

    // clear bits for spells already learned
    spell_flag &= ~spell_learned;

    int i = 0;
    int spells[31];

    int j;
    uint32_t mask;
    for (j = 0, mask = 0x1; spell_flag; mask <<= 1, j++) {
        if (spell_flag & mask) {
            spell_flag &= ~mask;
            if (msp_ptr[j].slevel <= p_ptr->lev) {
                spells[i] = j;
                i++;
            }
        }
    }

    if (new_spells > i) {
        msg_print("You seem to be missing a book.");
        diff_spells = new_spells - i;
        new_spells = i;
    }
    if (new_spells == 0) { ;
    } else if (stat == A_INT) {
        // get to choose which mage spells will be learned
        save_screen();
        print_spells(spells, i, false, -1);

        char query;
        while (new_spells && get_com("Learn which spell?", &query)) {
            int c = query - 'a';

            // test j < 23 in case i is greater than 22, only 22 spells
            // are actually shown on the screen, so limit choice to those
            if (c >= 0 && c < i && c < 22) {
                new_spells--;
                spell_learned |= 1L << spells[c];
                spell_order[last_known++] = (uint8_t) spells[c];
                for (; c <= i - 1; c++) {
                    spells[c] = spells[c + 1];
                }
                i--;
                erase_line(c + 1, 31);
                print_spells(spells, i, false, -1);
            } else {
                bell();
            }
        }
        restore_screen();
    } else {
        // pick a prayer at random
        while (new_spells) {
            int s = randint(i) - 1;
            spell_learned |= 1L << spells[s];
            spell_order[last_known++] = (uint8_t) spells[s];

            vtype tmp_str;
            (void) sprintf(tmp_str, "You have learned the prayer of %s.", spell_names[spells[s] + offset]);
            msg_print(tmp_str);

            for (; s <= i - 1; s++) {
                spells[s] = spells[s + 1];
            }
            i--;
            new_spells--;
        }
    }

    py.flags.new_spells = (uint8_t) (new_spells + diff_spells);
    if (py.flags.new_spells == 0) {
        py.flags.status |= PY_STUDY;
    }

    // set the mana for first level characters when they learn their first spell.
    if (py.misc.mana == 0) {
        calc_mana(stat);
    }
}

// Gain some mana if you know at least one spell -RAK-
void calc_mana(int stat) {
    struct player_type::misc *p_ptr = &py.misc;

    if (spell_learned != 0) {
        int new_mana = 0;
        int levels = p_ptr->lev - classes[p_ptr->pclass].first_spell_lev + 1;
        switch (stat_adj(stat)) {
            case 0:
                new_mana = 0;
                break;
            case 1:
            case 2:
                new_mana = 1 * levels;
                break;
            case 3:
                new_mana = 3 * levels / 2;
                break;
            case 4:
                new_mana = 2 * levels;
                break;
            case 5:
                new_mana = 5 * levels / 2;
                break;
            case 6:
                new_mana = 3 * levels;
                break;
            case 7:
                new_mana = 4 * levels;
                break;
        }

        // increment mana by one, so that first level chars have 2 mana
        if (new_mana > 0) {
            new_mana++;
        }

        // mana can be zero when creating character
        if (p_ptr->mana != new_mana) {
            if (p_ptr->mana != 0) {
                // change current mana proportionately to change of max mana,
                // divide first to avoid overflow, little loss of accuracy
                int32_t value = (((int32_t) p_ptr->cmana << 16) + p_ptr->cmana_frac) / p_ptr->mana * new_mana;
                p_ptr->cmana = (int16_t) (value >> 16);
                p_ptr->cmana_frac = (uint16_t) (value & 0xFFFF);
            } else {
                p_ptr->cmana = (int16_t) new_mana;
                p_ptr->cmana_frac = 0;
            }
            p_ptr->mana = (int16_t) new_mana;

            // can't print mana here, may be in store or inventory mode
            py.flags.status |= PY_MANA;
        }
    } else if (p_ptr->mana != 0) {
        p_ptr->mana = 0;
        p_ptr->cmana = 0;

        // can't print mana here, may be in store or inventory mode
        py.flags.status |= PY_MANA;
    }
}

// Increases hit points and level -RAK-
static void gain_level() {
    struct player_type::misc *p_ptr = &py.misc;
    p_ptr->lev++;

    vtype out_val;
    (void) sprintf(out_val, "Welcome to level %d.", (int) p_ptr->lev);
    msg_print(out_val);
    calc_hitpoints();

    int32_t need_exp = player_exp[p_ptr->lev - 1] * p_ptr->expfact / 100;
    if (p_ptr->exp > need_exp) {
        // lose some of the 'extra' exp when gaining several levels at once
        int32_t dif_exp = p_ptr->exp - need_exp;
        p_ptr->exp = need_exp + (dif_exp / 2);
    }

    prt_level();
    prt_title();

    class_type *c_ptr = &classes[p_ptr->pclass];

    if (c_ptr->spell == MAGE) {
        calc_spells(A_INT);
        calc_mana(A_INT);
    } else if (c_ptr->spell == PRIEST) {
        calc_spells(A_WIS);
        calc_mana(A_WIS);
    }
}

// Prints experience -RAK-
void prt_experience() {
    struct player_type::misc *p_ptr = &py.misc;

    if (p_ptr->exp > MAX_EXP) {
        p_ptr->exp = MAX_EXP;
    }

    while ((p_ptr->lev < MAX_PLAYER_LEVEL) && (signed) (player_exp[p_ptr->lev - 1] * p_ptr->expfact / 100) <= p_ptr->exp) {
        gain_level();
    }

    if (p_ptr->exp > p_ptr->max_exp) {
        p_ptr->max_exp = p_ptr->exp;
    }

    prt_long(p_ptr->exp, 14, STAT_COLUMN + 6);
}

// Calculate the players hit points
void calc_hitpoints() {
    struct player_type::misc *p_ptr = &py.misc;
    int hitpoints = player_hp[p_ptr->lev - 1] + (con_adj() * p_ptr->lev);

    // always give at least one point per level + 1
    if (hitpoints < (p_ptr->lev + 1)) {
        hitpoints = p_ptr->lev + 1;
    }

    if (py.flags.status & PY_HERO) {
        hitpoints += 10;
    }

    if (py.flags.status & PY_SHERO) {
        hitpoints += 20;
    }

    // mhp can equal zero while character is being created
    if (hitpoints != p_ptr->mhp && p_ptr->mhp != 0) {
        // change current hit points proportionately to change of mhp,
        // divide first to avoid overflow, little loss of accuracy
        int32_t value = (((int32_t) p_ptr->chp << 16) + p_ptr->chp_frac) / p_ptr->mhp * hitpoints;
        p_ptr->chp = (int16_t) (value >> 16);
        p_ptr->chp_frac = (uint16_t) (value & 0xFFFF);
        p_ptr->mhp = (int16_t) hitpoints;

        // can't print hit points here, may be in store or inventory mode
        py.flags.status |= PY_HP;
    }
}

// Inserts a string into a string
void insert_str(char *object_str, const char *mtc_str, const char *insert) {
    int mtc_len = (int) strlen(mtc_str);
    int obj_len = (int) strlen(object_str);
    char *bound = object_str + obj_len - mtc_len;

    char *pc;
    for (pc = object_str; pc <= bound; pc++) {
        char *temp_obj = pc;
        const char *temp_mtc = mtc_str;

        int i;
        for (i = 0; i < mtc_len; i++) {
            if (*temp_obj++ != *temp_mtc++) {
                break;
            }
        }
        if (i == mtc_len) {
            break;
        }
    }

    if (pc <= bound) {
        char out_val[80];

        (void) strncpy(out_val, object_str, (pc - object_str));
        // Turbo C needs int for array index.
        out_val[(int) (pc - object_str)] = '\0';
        if (insert) {
            (void) strcat(out_val, insert);
        }
        (void) strcat(out_val, (pc + mtc_len));
        (void) strcpy(object_str, out_val);
    }
}

void insert_lnum(char *object_str, const char *mtc_str, int32_t number, int show_sign) {
    size_t mlen = strlen(mtc_str);
    char *tmp_str = object_str;
    char *string;

    int flag = 1;
    while (flag != 0) {
        string = strchr(tmp_str, mtc_str[0]);
        if (string == 0) {
            flag = 0;
        } else {
            flag = strncmp(string, mtc_str, mlen);
            if (flag) {
                tmp_str = string + 1;
            }
        }
    }

    if (string) {
        vtype str1, str2;

        (void) strncpy(str1, object_str, string - object_str);
        str1[string - object_str] = '\0';
        (void) strcpy(str2, string + mlen);

        if (number >= 0 && show_sign) {
            (void) sprintf(object_str, "%s+%d%s", str1, number, str2);
        } else {
            (void) sprintf(object_str, "%s%d%s", str1, number, str2);
        }
    }
}

// lets anyone enter wizard mode after a disclaimer... -JEW-
bool enter_wiz_mode() {
    bool answer = false;

    if (!noscore) {
        msg_print("Wizard mode is for debugging and experimenting.");
        answer = get_check("The game will not be scored if you enter wizard mode. Are you sure?");
    }

    if (noscore || answer) {
        noscore |= 0x2;
        wizard = true;
        return true;
    }

    return false;
}

// Weapon weight VS strength and dexterity -RAK-
int attack_blows(int weight, int *wtohit) {
    int s = py.stats.use_stat[A_STR];
    int d = py.stats.use_stat[A_DEX];

    if (s * 15 < weight) {
        *wtohit = s * 15 - weight;
        return 1;
    }

    int str_index, dex_index;

    *wtohit = 0;
    if (d < 10) {
        dex_index = 0;
    } else if (d < 19) {
        dex_index = 1;
    } else if (d < 68) {
        dex_index = 2;
    } else if (d < 108) {
        dex_index = 3;
    } else if (d < 118) {
        dex_index = 4;
    } else {
        dex_index = 5;
    }

    int adj_weight = (s * 10 / weight);
    if (adj_weight < 2) {
        str_index = 0;
    } else if (adj_weight < 3) {
        str_index = 1;
    } else if (adj_weight < 4) {
        str_index = 2;
    } else if (adj_weight < 5) {
        str_index = 3;
    } else if (adj_weight < 7) {
        str_index = 4;
    } else if (adj_weight < 9) {
        str_index = 5;
    } else {
        str_index = 6;
    }

    return (int) blows_table[str_index][dex_index];
}

// Special damage due to magical abilities of object -RAK-
int tot_dam(inven_type *i_ptr, int tdam, int monster) {
    if ((i_ptr->flags & TR_EGO_WEAPON) &&
        (
                (i_ptr->tval >= TV_SLING_AMMO && i_ptr->tval <= TV_ARROW) ||
                (i_ptr->tval >= TV_HAFTED && i_ptr->tval <= TV_SWORD) ||
                i_ptr->tval == TV_FLASK
        )) {
        creature_type *m_ptr = &c_list[monster];
        recall_type *r_ptr = &c_recall[monster];

        if ((m_ptr->cdefense & CD_DRAGON) && (i_ptr->flags & TR_SLAY_DRAGON)) {
            // Slay Dragon

            tdam = tdam * 4;
            r_ptr->r_cdefense |= CD_DRAGON;
        } else if ((m_ptr->cdefense & CD_UNDEAD) && (i_ptr->flags & TR_SLAY_UNDEAD)) {
            // Slay Undead

            tdam = tdam * 3;
            r_ptr->r_cdefense |= CD_UNDEAD;
        } else if ((m_ptr->cdefense & CD_ANIMAL) && (i_ptr->flags & TR_SLAY_ANIMAL)) {
            // Slay Animal

            tdam = tdam * 2;
            r_ptr->r_cdefense |= CD_ANIMAL;
        } else if ((m_ptr->cdefense & CD_EVIL) && (i_ptr->flags & TR_SLAY_EVIL)) {
            // Slay Evil

            tdam = tdam * 2;
            r_ptr->r_cdefense |= CD_EVIL;
        } else if ((m_ptr->cdefense & CD_FROST) && (i_ptr->flags & TR_FROST_BRAND)) {
            // Frost

            tdam = tdam * 3 / 2;
            r_ptr->r_cdefense |= CD_FROST;
        } else if ((m_ptr->cdefense & CD_FIRE) && (i_ptr->flags & TR_FLAME_TONGUE)) {
            // Fire

            tdam = tdam * 3 / 2;
            r_ptr->r_cdefense |= CD_FIRE;
        }
    }

    return tdam;
}

// Critical hits, Nasty way to die. -RAK-
int critical_blow(int weight, int plus, int dam, int attack_type) {
    int critical = dam;

    // Weight of weapon, plusses to hit, and character level all
    // contribute to the chance of a critical
    if (randint(5000) <= weight + 5 * plus + (class_level_adj[py.misc.pclass][attack_type] * py.misc.lev)) {
        weight += randint(650);

        if (weight < 400) {
            critical = 2 * dam + 5;
            msg_print("It was a good hit! (x2 damage)");
        } else if (weight < 700) {
            critical = 3 * dam + 10;
            msg_print("It was an excellent hit! (x3 damage)");
        } else if (weight < 900) {
            critical = 4 * dam + 15;
            msg_print("It was a superb hit! (x4 damage)");
        } else {
            critical = 5 * dam + 20;
            msg_print("It was a *GREAT* hit! (x5 damage)");
        }
    }

    return critical;
}

// Given direction "dir", returns new row, column location -RAK-
bool mmove(int dir, int *y, int *x) {
    int new_row = 0;
    int new_col = 0;

    switch (dir) {
        case 1:
            new_row = *y + 1;
            new_col = *x - 1;
            break;
        case 2:
            new_row = *y + 1;
            new_col = *x;
            break;
        case 3:
            new_row = *y + 1;
            new_col = *x + 1;
            break;
        case 4:
            new_row = *y;
            new_col = *x - 1;
            break;
        case 5:
            new_row = *y;
            new_col = *x;
            break;
        case 6:
            new_row = *y;
            new_col = *x + 1;
            break;
        case 7:
            new_row = *y - 1;
            new_col = *x - 1;
            break;
        case 8:
            new_row = *y - 1;
            new_col = *x;
            break;
        case 9:
            new_row = *y - 1;
            new_col = *x + 1;
            break;
    }

    bool moved = false;

    if (new_row >= 0 && new_row < cur_height && new_col >= 0 && new_col < cur_width) {
        *y = new_row;
        *x = new_col;
        moved = true;
    }

    return moved;
}

// Saving throws for player character. -RAK-
bool player_saves() {
    // MPW C couldn't handle the expression, so split it into two parts
    int16_t temp = class_level_adj[py.misc.pclass][CLA_SAVE];

    return (randint(100) <= (py.misc.save + stat_adj(A_WIS) + (temp * py.misc.lev / 3)));
}

// Finds range of item in inventory list -RAK-
int find_range(int item1, int item2, int *j, int *k) {
    int i = 0;

    *j = -1;
    *k = -1;

    inven_type *i_ptr = &inventory[0];

    bool flag = false;

    while (i < inven_ctr) {
        if (!flag) {
            if (i_ptr->tval == item1 || i_ptr->tval == item2) {
                flag = true;
                *j = i;
            }
        } else {
            if (i_ptr->tval != item1 && i_ptr->tval != item2) {
                *k = i - 1;
                break;
            }
        }
        i++;
        i_ptr++;
    }

    if (flag && (*k == -1)) {
        *k = inven_ctr - 1;
    }

    return flag;
}

// Teleport the player to a new location -RAK-
void teleport(int dis) {
    int y, x;

    do {
        y = randint(cur_height) - 1;
        x = randint(cur_width) - 1;
        while (distance(y, x, char_row, char_col) > dis) {
            y += ((char_row - y) / 2);
            x += ((char_col - x) / 2);
        }
    } while ((cave[y][x].fval >= MIN_CLOSED_SPACE) || (cave[y][x].cptr >= 2));

    move_rec(char_row, char_col, y, x);

    for (int i = char_row - 1; i <= char_row + 1; i++) {
        for (int j = char_col - 1; j <= char_col + 1; j++) {
            cave[i][j].tl = false;
            lite_spot(i, j);
        }
    }

    lite_spot(char_row, char_col);
    char_row = (int16_t) y;
    char_col = (int16_t) x;
    check_view();
    creatures(false);
    teleport_flag = false;
}
