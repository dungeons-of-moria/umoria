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
    cave[y][x].tptr = (uint8_t) cur_pos;
    cave[y][x].fval = BLOCKED_FLOOR;
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
    t_list[cur_pos].cost += (8L * (int32_t) randint((int) t_list[cur_pos].cost)) + randint(8);

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

    int objectID;

    // This code has been added to make it slightly more likely to get the
    // higher level objects.  Originally a uniform distribution over all
    // objects less than or equal to the dungeon level. This distribution
    // makes a level n objects occur approx 2/n% of the time on level n,
    // and 1/2n are 0th level.
    do {
        if (randint(2) == 1) {
            objectID = randint(t_level[level]) - 1;
        } else {
            // Choose three objects, pick the highest level.
            objectID = randint(t_level[level]) - 1;

            int j = randint(t_level[level]) - 1;

            if (objectID < j) {
                objectID = j;
            }

            j = randint(t_level[level]) - 1;

            if (objectID < j) {
                objectID = j;
            }

            int foundLevel = object_list[sorted_objects[objectID]].level;

            if (foundLevel == 0) {
                objectID = randint(t_level[0]) - 1;
            } else {
                objectID = randint(t_level[foundLevel] - t_level[foundLevel - 1]) - 1 + t_level[foundLevel - 1];
            }
        }
    } while (must_be_small && set_large(&object_list[sorted_objects[objectID]]));

    return objectID;
}

// Places an object at given row, column co-ordinate -RAK-
void place_object(int y, int x, bool must_be_small) {
    int cur_pos = popt();

    cave[y][x].tptr = (uint8_t) cur_pos;

    int objectID = get_obj_num(dun_level, must_be_small);
    invcopy(&t_list[cur_pos], sorted_objects[objectID]);

    magic_treasure(cur_pos, dun_level);

    if (cave[y][x].cptr == 1) {
        msg_print("You feel something roll beneath your feet."); // -CJS-
    }
}

// Allocates an object for tunnels and rooms -RAK-
void alloc_object(bool (*alloc_set)(int), int typ, int num) {
    int y, x;

    for (int k = 0; k < num; k++) {
        // don't put an object beneath the player, this could cause
        // problems if player is standing under rubble, or on a trap.
        do {
            y = randint(cur_height) - 1;
            x = randint(cur_width) - 1;
        } while (!(*alloc_set)(cave[y][x].fval) || cave[y][x].tptr != 0 || (y == char_row && x == char_col));

        switch (typ) {
            case 1:
                place_trap(y, x, randint(MAX_TRAP) - 1);
                break;
            case 2:
                // NOTE: typ == 2 is not used - used to be visible traps.
                // FIXME: no `break` here, is this correct?
                // FIXME: typ == 2 was not handled directly and was caught by typ == 3 -MRC-
            case 3:
                place_rubble(y, x);
                break;
            case 4:
                place_gold(y, x);
                break;
            case 5:
                place_object(y, x, false);
                break;
            default:
                break;
        }
    }
}

// Creates objects nearby the coordinates given -RAK-
void random_object(int y, int x, int num) {
    do {
        for (int i = 0; i <= 10; i++) {
            int j = y - 3 + randint(5);
            int k = x - 4 + randint(7);

            if (in_bounds(j, k) && cave[j][k].fval <= MAX_CAVE_FLOOR && cave[j][k].tptr == 0) {
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
void cnv_stat(uint8_t stat, char *str) {
    if (stat <= 18) {
        (void) sprintf(str, "%6d", stat);
        return;
    }

    int part1 = 18;
    int part2 = stat - 18;

    if (part2 == 100) {
        (void) strcpy(str, "18/100");
        return;
    }

    (void) sprintf(str, " %2d/%02d", part1, part2);
}

// Print character stat in given row, column -RAK-
void prt_stat(int stat) {
    char text[7];
    cnv_stat(py.stats.use_stat[stat], text);
    put_buffer(stat_names[stat], 6 + stat, STAT_COLUMN);
    put_buffer(text, 6 + stat, STAT_COLUMN + 6);
}

// Print character info in given row, column -RAK-
// The longest title is 13 characters, so only pad to 13
void prt_field(const char *info, int row, int column) {
    put_buffer(&blank_string[BLANK_LENGTH - 13], row, column);
    put_buffer(info, row, column);
}

// Print long number with header at given row, column
static void prt_lnum(const char *header, int32_t num, int row, int column) {
    vtype_t str;
    (void) sprintf(str, "%s: %6d", header, num);
    put_buffer(str, row, column);
}

// Print long number (7 digits of space) with header at given row, column
static void prt_7lnum(const char *header, int32_t num, int row, int column) {
    vtype_t str;
    (void) sprintf(str, "%s: %7d", header, num);
    put_buffer(str, row, column);
}

// Print number with header at given row, column -RAK-
static void prt_num(const char *header, int num, int row, int column) {
    vtype_t str;
    (void) sprintf(str, "%s: %6d", header, num);
    put_buffer(str, row, column);
}

// Print long number at given row, column
static void prt_long(int32_t num, int row, int column) {
    vtype_t str;
    (void) sprintf(str, "%6d", num);
    put_buffer(str, row, column);
}

// Print number at given row, column -RAK-
static void prt_int(int num, int row, int column) {
    vtype_t str;
    (void) sprintf(str, "%6d", num);
    put_buffer(str, row, column);
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
    }

    if (charisma > 107) {
        return 92;
    }

    if (charisma > 87) {
        return 94;
    }

    if (charisma > 67) {
        return 96;
    }

    if (charisma > 18) {
        return 98;
    }

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

// Returns a character's adjustment to hit points -JWT-
int con_adj() {
    int con = py.stats.use_stat[A_CON];

    if (con < 7) {
        return (con - 7);
    }

    if (con < 17) {
        return 0;
    }

    if (con == 17) {
        return 1;
    }

    if (con < 94) {
        return 2;
    }

    if (con < 117) {
        return 3;
    }

    return 4;
}

char *title_string() {
    const char *p;

    if (py.misc.lev < 1) {
        p = "Babe in arms";
    } else if (py.misc.lev <= MAX_PLAYER_LEVEL) {
        p = player_titles[py.misc.pclass][py.misc.lev - 1];
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
    vtype_t depths;

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
        return;
    }

    if (PY_REST & py.flags.status) {
        char restString[16];

        if (py.flags.rest < 0) {
            (void) strcpy(restString, "Rest *");
        } else if (display_counts) {
            (void) sprintf(restString, "Rest %-5d", py.flags.rest);
        } else {
            (void) strcpy(restString, "Rest");
        }

        put_buffer(restString, 23, 38);

        return;
    }

    if (command_count > 0) {
        char repeatString[16];

        if (display_counts) {
            (void) sprintf(repeatString, "Repeat %-3d", command_count);
        } else {
            (void) strcpy(repeatString, "Repeat");
        }

        py.flags.status |= PY_REPEAT;

        put_buffer(repeatString, 23, 38);

        if (PY_SEARCH & py.flags.status) {
            put_buffer("Search", 23, 38);
        }

        return;
    }

    if (PY_SEARCH & py.flags.status) {
        put_buffer("Searching", 23, 38);
        return;
    }

    // "repeat 999" is 10 characters
    put_buffer(&blank_string[BLANK_LENGTH - 10], 23, 38);
}

// Prints the speed of a character. -CJS-
void prt_speed() {
    int speed = py.flags.speed;

    // Search mode.
    if (PY_SEARCH & py.flags.status) {
        speed--;
    }

    if (speed > 1) {
        put_buffer("Very Slow", 23, 49);
    } else if (speed == 1) {
        put_buffer("Slow     ", 23, 49);
    } else if (speed == 0) {
        put_buffer(&blank_string[BLANK_LENGTH - 9], 23, 49);
    } else if (speed == -1) {
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
    uint8_t newStat = py.stats.cur_stat[stat];

    int loop = (amount < 0 ? -amount : amount);

    for (int i = 0; i < loop; i++) {
        if (amount > 0) {
            if (newStat < 18) {
                newStat++;
            } else if (newStat < 108) {
                newStat += 10;
            } else {
                newStat = 118;
            }
        } else {
            if (newStat > 27) {
                newStat -= 10;
            } else if (newStat > 18) {
                newStat = 18;
            } else if (newStat > 3) {
                newStat--;
            }
        }
    }

    return newStat;
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
    int newStat = py.stats.cur_stat[stat];

    if (newStat >= 118) {
        return false;
    }

    if (newStat < 18) {
        newStat++;
    } else if (newStat < 116) {
        // stat increases by 1/6 to 1/3 of difference from max
        int gain = ((118 - newStat) / 3 + 1) >> 1;

        newStat += randint(gain) + gain;
    } else {
        newStat++;
    }

    py.stats.cur_stat[stat] = (uint8_t) newStat;

    if (newStat > py.stats.max_stat[stat]) {
        py.stats.max_stat[stat] = (uint8_t) newStat;
    }

    set_use_stat(stat);
    prt_stat(stat);

    return true;
}

// Decreases a stat by one randomized level -RAK-
bool dec_stat(int stat) {
    int newStat = py.stats.cur_stat[stat];

    if (newStat <= 3) {
        return false;
    }

    if (newStat < 19) {
        newStat--;
    } else if (newStat < 117) {
        int loss = (((118 - newStat) >> 1) + 1) >> 1;
        newStat += -randint(loss) - loss;

        if (newStat < 18) {
            newStat = 18;
        }
    } else {
        newStat--;
    }

    py.stats.cur_stat[stat] = (uint8_t) newStat;

    set_use_stat(stat);
    prt_stat(stat);

    return true;
}

// Restore a stat.  Return true only if this actually makes a difference.
bool res_stat(int stat) {
    int newStat = py.stats.max_stat[stat] - py.stats.cur_stat[stat];

    if (newStat == 0) {
        return false;
    }

    py.stats.cur_stat[stat] += newStat;

    set_use_stat(stat);
    prt_stat(stat);

    return true;
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

    int dexterity = py.stats.use_stat[A_DEX];
    if (dexterity < 4) {
        total = -3;
    } else if (dexterity < 6) {
        total = -2;
    } else if (dexterity < 8) {
        total = -1;
    } else if (dexterity < 16) {
        total = 0;
    } else if (dexterity < 17) {
        total = 1;
    } else if (dexterity < 18) {
        total = 2;
    } else if (dexterity < 69) {
        total = 3;
    } else if (dexterity < 118) {
        total = 4;
    } else {
        total = 5;
    }

    int strength = py.stats.use_stat[A_STR];
    if (strength < 4) {
        total -= 3;
    } else if (strength < 5) {
        total -= 2;
    } else if (strength < 7) {
        total -= 1;
    } else if (strength < 18) {
        total -= 0;
    } else if (strength < 94) {
        total += 1;
    } else if (strength < 109) {
        total += 2;
    } else if (strength < 117) {
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
    prt_field(races[py.misc.prace].trace, 2, STAT_COLUMN);
    prt_field(classes[py.misc.pclass].title, 3, STAT_COLUMN);
    prt_field(title_string(), 4, STAT_COLUMN);

    for (int i = 0; i < 6; i++) {
        prt_stat(i);
    }

    prt_num("LEV ", (int) py.misc.lev, 13, STAT_COLUMN);
    prt_lnum("EXP ", py.misc.exp, 14, STAT_COLUMN);
    prt_num("MANA", py.misc.cmana, 15, STAT_COLUMN);
    prt_num("MHP ", py.misc.mhp, 16, STAT_COLUMN);
    prt_num("CHP ", py.misc.chp, 17, STAT_COLUMN);
    prt_num("AC  ", py.misc.dis_ac, 19, STAT_COLUMN);
    prt_lnum("GOLD", py.misc.au, 20, STAT_COLUMN);
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
    int16_t speed = py.flags.speed - (int16_t) ((PY_SEARCH & status) >> 8);
    if (speed != 0) {
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
    clear_screen();

    put_buffer("Name        :", 2, 1);
    put_buffer("Race        :", 3, 1);
    put_buffer("Sex         :", 4, 1);
    put_buffer("Class       :", 5, 1);

    if (!character_generated) {
        return;
    }

    put_buffer(py.misc.name, 2, 15);
    put_buffer(races[py.misc.prace].trace, 3, 15);
    put_buffer((py.misc.male ? "Male" : "Female"), 4, 15);
    put_buffer(classes[py.misc.pclass].title, 5, 15);
}

// Prints the following information on the screen. -JWT-
void put_stats() {
    for (int i = 0; i < 6; i++) {
        vtype_t buf;

        cnv_stat(py.stats.use_stat[i], buf);
        put_buffer(stat_names[i], 2 + i, 61);
        put_buffer(buf, 2 + i, 66);

        if (py.stats.max_stat[i] > py.stats.cur_stat[i]) {
            cnv_stat(py.stats.max_stat[i], buf);
            put_buffer(buf, 2 + i, 73);
        }
    }

    prt_num("+ To Hit    ", py.misc.dis_th, 9, 1);
    prt_num("+ To Damage ", py.misc.dis_td, 10, 1);
    prt_num("+ To AC     ", py.misc.dis_tac, 11, 1);
    prt_num("  Total AC  ", py.misc.dis_ac, 12, 1);
}

// Returns a rating of x depending on y -JWT-
const char *likert(int x, int y) {
    switch (x / y) {
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
    prt_num("Age          ", (int) py.misc.age, 2, 38);
    prt_num("Height       ", (int) py.misc.ht, 3, 38);
    prt_num("Weight       ", (int) py.misc.wt, 4, 38);
    prt_num("Social Class ", (int) py.misc.sc, 5, 38);
}

// Prints the following information on the screen. -JWT-
void put_misc2() {
    prt_7lnum("Level      ", (int32_t) py.misc.lev, 9, 28);
    prt_7lnum("Experience ", py.misc.exp, 10, 28);
    prt_7lnum("Max Exp    ", py.misc.max_exp, 11, 28);

    if (py.misc.lev >= MAX_PLAYER_LEVEL) {
        prt("Exp to Adv.: *******", 12, 28);
    } else {
        prt_7lnum("Exp to Adv.", (int32_t) (player_exp[py.misc.lev - 1] * py.misc.expfact / 100), 12, 28);
    }

    prt_7lnum("Gold       ", py.misc.au, 13, 28);
    prt_num("Max Hit Points ", py.misc.mhp, 9, 52);
    prt_num("Cur Hit Points ", py.misc.chp, 10, 52);
    prt_num("Max Mana       ", py.misc.mana, 11, 52);
    prt_num("Cur Mana       ", py.misc.cmana, 12, 52);
}

// Prints ratings on certain abilities -RAK-
void put_misc3() {
    clear_from(14);

    int xbth = py.misc.bth + py.misc.ptohit * BTH_PLUS_ADJ + (class_level_adj[py.misc.pclass][CLA_BTH] * py.misc.lev);
    int xbthb = py.misc.bthb + py.misc.ptohit * BTH_PLUS_ADJ + (class_level_adj[py.misc.pclass][CLA_BTHB] * py.misc.lev);

    // this results in a range from 0 to 29
    int xfos = 40 - py.misc.fos;
    if (xfos < 0) {
        xfos = 0;
    }

    int xsrh = py.misc.srh;

    // this results in a range from 0 to 9
    int xstl = py.misc.stl + 1;
    int xdis = py.misc.disarm + 2 * todis_adj() + stat_adj(A_INT) + (class_level_adj[py.misc.pclass][CLA_DISARM] * py.misc.lev / 3);
    int xsave = py.misc.save + stat_adj(A_WIS) + (class_level_adj[py.misc.pclass][CLA_SAVE] * py.misc.lev / 3);
    int xdev = py.misc.save + stat_adj(A_INT) + (class_level_adj[py.misc.pclass][CLA_DEVICE] * py.misc.lev / 3);

    vtype_t xinfra;
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
    vtype_t temp;
    bool flag = false;

    display_char();

    while (!flag) {
        prt("<f>ile character description. <c>hange character name.", 21, 2);

        switch (inkey()) {
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
    Inventory_t *i_ptr = &inventory[item_val];

    if (i_ptr->number > 1 && i_ptr->subval <= ITEM_SINGLE_STACK_MAX) {
        i_ptr->number--;
        inven_weight -= i_ptr->weight;
    } else {
        inven_weight -= i_ptr->weight * i_ptr->number;

        for (int i = item_val; i < inven_ctr - 1; i++) {
            inventory[i] = inventory[i + 1];
        }

        invcopy(&inventory[inven_ctr - 1], OBJ_NOTHING);
        inven_ctr--;
    }

    py.flags.status |= PY_STR_WGT;
}

// Copies the object in the second argument over the first argument.
// However, the second always gets a number of one except for ammo etc.
void take_one_item(Inventory_t *s_ptr, Inventory_t *i_ptr) {
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

    int treasureID = popt();

    Inventory_t *i_ptr = &inventory[item_val];
    t_list[treasureID] = *i_ptr;

    cave[char_row][char_col].tptr = (uint8_t) treasureID;

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
            t_list[treasureID].number = 1;
            inven_weight -= i_ptr->weight;
            i_ptr->number--;
        }

        obj_desc_t prt1, prt2;
        objdes(prt1, &t_list[treasureID], true);
        (void) sprintf(prt2, "Dropped %s", prt1);
        msg_print(prt2);
    }

    py.flags.status |= PY_STR_WGT;
}

// Destroys a type of item on a given percent chance -RAK-
int inven_damage(bool (*typ)(Inventory_t *), int perc) {
    int damage = 0;

    for (int i = 0; i < inven_ctr; i++) {
        if ((*typ)(&inventory[i]) && randint(100) < perc) {
            inven_destroy(i);
            damage++;
        }
    }

    return damage;
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
bool inven_check_num(Inventory_t *t_ptr) {
    if (inven_ctr < INVEN_WIELD) {
        return true;
    }

    if (t_ptr->subval < ITEM_SINGLE_STACK_MIN) {
        return false;
    }

    for (int i = 0; i < inven_ctr; i++) {
        bool sameCharacter = inventory[i].tval == t_ptr->tval;
        bool sameCategory = inventory[i].subval == t_ptr->subval;

        // make sure the number field doesn't overflow
        bool sameNumber = inventory[i].number + t_ptr->number < 256;

        // they always stack (subval < 192), or else they have same p1
        bool sameGroup = t_ptr->subval < ITEM_GROUP_MIN || inventory[i].p1 == t_ptr->p1;

        // only stack if both or neither are identified
        bool identification = known1_p(&inventory[i]) == known1_p(t_ptr);

        if (sameCharacter && sameCategory && sameNumber && sameGroup && identification) {
            return true;
        }
    }

    return false;
}

// return false if picking up an object would change the players speed
bool inven_check_weight(Inventory_t *i_ptr) {
    int limit = weight_limit();
    int newWeight = i_ptr->number * i_ptr->weight + inven_weight;

    if (limit < newWeight) {
        limit = newWeight / (limit + 1);
    } else {
        limit = 0;
    }

    return pack_heavy == limit;
}

// Are we strong enough for the current pack and weapon? -CJS-
void check_strength() {
    Inventory_t *i_ptr = &inventory[INVEN_WIELD];

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

    int limit = weight_limit();

    if (limit < inven_weight) {
        limit = inven_weight / (limit + 1);
    } else {
        limit = 0;
    }

    if (pack_heavy != limit) {
        if (pack_heavy < limit) {
            msg_print("Your pack is so heavy that it slows you down.");
        } else {
            msg_print("You move more easily under the weight of your pack.");
        }
        change_speed(limit - pack_heavy);
        pack_heavy = limit;
    }

    py.flags.status &= ~PY_STR_WGT;
}

// Add an item to players inventory.  Return the
// item position for a description if needed. -RAK-
// this code must be identical to the inven_check_num() code above
int inven_carry(Inventory_t *i_ptr) {
    int typ = i_ptr->tval;
    int subt = i_ptr->subval;
    bool known1p = known1_p(i_ptr);
    int always_known1p = (object_offset(i_ptr) == -1);

    int locn;

    // Now, check to see if player can carry object
    for (locn = 0;; locn++) {
        Inventory_t *t_ptr = &inventory[locn];

        if (typ == t_ptr->tval && subt == t_ptr->subval && subt >= ITEM_SINGLE_STACK_MIN && ((int) t_ptr->number + (int) i_ptr->number) < 256 &&
            (subt < ITEM_GROUP_MIN || t_ptr->p1 == i_ptr->p1) &&
            // only stack if both or neither are identified
            known1p == known1_p(t_ptr)) {
            t_ptr->number += i_ptr->number;

            break;
        }

        if ((typ == t_ptr->tval && subt < t_ptr->subval && always_known1p) || typ > t_ptr->tval) {
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
    Spell_t *s_ptr = &magic_spell[py.misc.pclass - 1][spell];

    int chance = s_ptr->sfail - 3 * (py.misc.lev - s_ptr->slevel);

    int stat;
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
        int spellID = spell[i];
        Spell_t *s_ptr = &magic_spell[py.misc.pclass - 1][spellID];

        const char *p;
        if (comment == 0) {
            p = "";
        } else if ((spell_forgotten & (1L << spellID)) != 0) {
            p = " forgotten";
        } else if ((spell_learned & (1L << spellID)) == 0) {
            p = " unknown";
        } else if ((spell_worked & (1L << spellID)) == 0) {
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
            spell_char = (char) ('a' + spellID - nonconsec);
        }

        vtype_t out_val;
        (void) sprintf(out_val, "  %c) %-30s%2d %4d %3d%%%s", spell_char, spell_names[spellID + offset], s_ptr->slevel, s_ptr->smana, spell_chance(spellID), p);
        prt(out_val, 2 + i, col);
    }
}

// Returns spell pointer -RAK-
int get_spell(int *spell, int num, int *sn, int *sc, const char *prompt, int first_spell) {
    *sn = -1;

    vtype_t str;
    (void) sprintf(str, "(Spells %c-%c, *=List, <ESCAPE>=exit) %s", spell[0] + 'a' - first_spell, spell[num - 1] + 'a' - first_spell, prompt);

    bool flag = false;
    bool redraw = false;

    int offset = (classes[py.misc.pclass].spell == MAGE ? SPELL_OFFSET : PRAYER_OFFSET);

    char choice;

    while (!flag && get_com(str, &choice)) {
        if (isupper((int) choice)) {
            *sn = choice - 'A' + first_spell;

            // verify that this is in spell[], at most 22 entries in spell[]
            int spellID;
            for (spellID = 0; spellID < num; spellID++) {
                if (*sn == spell[spellID]) {
                    break;
                }
            }

            if (spellID == num) {
                *sn = -2;
            } else {
                Spell_t *s_ptr = &magic_spell[py.misc.pclass - 1][*sn];

                vtype_t tmp_str;
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
            int spellID;
            for (spellID = 0; spellID < num; spellID++) {
                if (*sn == spell[spellID]) {
                    break;
                }
            }

            if (spellID == num) {
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
            vtype_t tmp_str;
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

// check to see if know any spells greater than level, eliminate them
static void eliminateKnownSpellsGreaterThanLevel(Spell_t *msp_ptr, const char *p, int offset) {
    uint32_t mask = 0x80000000L;

    for (int i = 31; mask; mask >>= 1, i--) {
        if (mask & spell_learned) {
            if (msp_ptr[i].slevel > py.misc.lev) {
                spell_learned &= ~mask;
                spell_forgotten |= mask;

                vtype_t msg;
                (void) sprintf(msg, "You have forgotten the %s of %s.", p, spell_names[i + offset]);
                msg_print(msg);
            } else {
                break;
            }
        }
    }
}

static int numberOfSpellsAllowed(int stat) {
    int allowed = 0;

    int levels = py.misc.lev - classes[py.misc.pclass].first_spell_lev + 1;

    switch (stat_adj(stat)) {
        case 0:
            allowed = 0;
            break;
        case 1:
        case 2:
        case 3:
            allowed = 1 * levels;
            break;
        case 4:
        case 5:
            allowed = 3 * levels / 2;
            break;
        case 6:
            allowed = 2 * levels;
            break;
        case 7:
            allowed = 5 * levels / 2;
            break;
    }

    return allowed;
}

static int numberOfSpellsKnown() {
    int known = 0;

    for (uint32_t mask = 0x1; mask; mask <<= 1) {
        if (mask & spell_learned) {
            known++;
        }
    }

    return known;
}

// remember forgotten spells while forgotten spells exist of new_spells positive,
// remember the spells in the order that they were learned
static int rememberForgottenSpells(Spell_t *msp_ptr, int allowedSpells, int newSpells, const char *p, int offset) {
    uint32_t mask;

    for (int n = 0; (spell_forgotten && newSpells && (n < allowedSpells) && (n < 32)); n++) {
        // orderID is (i+1)th spell learned
        int orderID = spell_order[n];

        // shifting by amounts greater than number of bits in long gives
        // an undefined result, so don't shift for unknown spells
        if (orderID == 99) {
            mask = 0x0;
        } else {
            mask = (uint32_t) (1L << orderID);
        }

        if (mask & spell_forgotten) {
            if (msp_ptr[orderID].slevel <= py.misc.lev) {
                newSpells--;
                spell_forgotten &= ~mask;
                spell_learned |= mask;

                vtype_t msg;
                (void) sprintf(msg, "You have remembered the %s of %s.", p, spell_names[orderID + offset]);
                msg_print(msg);
            } else {
                allowedSpells++;
            }
        }
    }

    return newSpells;
}

// determine which spells player can learn must check all spells here,
// in gain_spell() we actually check if the books are present
static int learnableSpells(Spell_t *msp_ptr, int newSpells) {
    uint32_t spell_flag = (uint32_t) (0x7FFFFFFFL & ~spell_learned);

    int id = 0;
    uint32_t mask = 0x1;

    for (int i = 0; spell_flag; mask <<= 1, i++) {
        if (spell_flag & mask) {
            spell_flag &= ~mask;
            if (msp_ptr[i].slevel <= py.misc.lev) {
                id++;
            }
        }
    }

    if (newSpells > id) {
        newSpells = id;
    }

    return newSpells;
}

// forget spells until new_spells zero or no more spells know,
// spells are forgotten in the opposite order that they were learned
// NOTE: newSpells is always a negative value
static void forgetSpells(int newSpells, const char *p, int offset) {
    uint32_t mask;

    for (int i = 31; newSpells && spell_learned; i--) {
        // orderID is the (i+1)th spell learned
        int orderID = spell_order[i];

        // shifting by amounts greater than number of bits in long gives
        // an undefined result, so don't shift for unknown spells
        if (orderID == 99) {
            mask = 0x0;
        } else {
            mask = (uint32_t) (1L << orderID);
        }

        if (mask & spell_learned) {
            spell_learned &= ~mask;
            spell_forgotten |= mask;
            newSpells++;

            vtype_t msg;
            (void) sprintf(msg, "You have forgotten the %s of %s.", p, spell_names[orderID + offset]);
            msg_print(msg);
        }
    }
}

// calculate number of spells player should have, and
// learn forget spells until that number is met -JEW-
void calc_spells(int stat) {
    Spell_t *msp_ptr = &magic_spell[py.misc.pclass - 1][0];

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
    eliminateKnownSpellsGreaterThanLevel(msp_ptr, p, offset);

    // calc number of spells allowed
    int num_allowed = numberOfSpellsAllowed(stat);
    int num_known = numberOfSpellsKnown();
    int new_spells = num_allowed - num_known;

    if (new_spells > 0) {
        new_spells = rememberForgottenSpells(msp_ptr, num_allowed, new_spells, p, offset);

        // If new spells is still greater than zero
        if (new_spells > 0) {
            new_spells = learnableSpells(msp_ptr, new_spells);
        }
    } else if (new_spells < 0) {
        forgetSpells(new_spells, p, offset);
        new_spells = 0;
    }

    if (new_spells != py.flags.new_spells) {
        if (new_spells > 0 && py.flags.new_spells == 0) {
            vtype_t msg;
            (void) sprintf(msg, "You can learn some new %ss now.", p);
            msg_print(msg);
        }

        py.flags.new_spells = (uint8_t) new_spells;
        py.flags.status |= PY_STUDY;
    }
}

static bool playerCanRead() {
    if (py.flags.blind > 0) {
        msg_print("You can't see to read your spell book!");
        return false;
    }

    if (no_light()) {
        msg_print("You have no light to read by.");
        return false;
    }

    return true;
}

static int lastKnownSpell() {
    for (int last_known = 0; last_known < 32; last_known++) {
        if (spell_order[last_known] == 99) {
            return last_known;
        }
    }

    // We should never actually reach this, but just in case... -MRC-
    return 0;
}

static uint32_t playerDetermineLearnableSpells() {
    uint32_t spell_flag = 0;

    for (int i = 0; i < inven_ctr; i++) {
        if (inventory[i].tval == TV_MAGIC_BOOK) {
            spell_flag |= inventory[i].flags;
        }
    }

    return spell_flag;
}

// gain spells when player wants to    - jw
void gain_spells() {
    // Priests don't need light because they get spells from their god, so only
    // fail when can't see if player has MAGE spells. This check is done below.
    if (py.flags.confused > 0) {
        msg_print("You are too confused.");
        return;
    }

    int new_spells = py.flags.new_spells;
    int diff_spells = 0;

    Spell_t *msp_ptr = &magic_spell[py.misc.pclass - 1][0];

    int stat, offset;

    if (classes[py.misc.pclass].spell == MAGE) {
        // People with MAGE spells can't learn spells if they can't read their books.
        if (!playerCanRead()) {
            return;
        }
        stat = A_INT;
        offset = SPELL_OFFSET;
    } else {
        stat = A_WIS;
        offset = PRAYER_OFFSET;
    }

    int last_known = lastKnownSpell();

    if (!new_spells) {
        vtype_t tmp_str;
        (void) sprintf(tmp_str, "You can't learn any new %ss!", (stat == A_INT ? "spell" : "prayer"));
        msg_print(tmp_str);

        free_turn_flag = true;
        return;
    }

    uint32_t spell_flag;

    // determine which spells player can learn
    // mages need the book to learn a spell, priests do not need the book
    if (stat == A_INT) {
        spell_flag = playerDetermineLearnableSpells();
    } else {
        spell_flag = 0x7FFFFFFF;
    }

    // clear bits for spells already learned
    spell_flag &= ~spell_learned;

    int spellID = 0;
    int spells[31];
    uint32_t mask = 0x1;

    for (int i = 0; spell_flag; mask <<= 1, i++) {
        if (spell_flag & mask) {
            spell_flag &= ~mask;
            if (msp_ptr[i].slevel <= py.misc.lev) {
                spells[spellID] = i;
                spellID++;
            }
        }
    }

    if (new_spells > spellID) {
        msg_print("You seem to be missing a book.");

        diff_spells = new_spells - spellID;
        new_spells = spellID;
    }

    if (new_spells == 0) {
        // do nothing
    } else if (stat == A_INT) {
        // get to choose which mage spells will be learned
        save_screen();
        print_spells(spells, spellID, false, -1);

        char query;
        while (new_spells && get_com("Learn which spell?", &query)) {
            int c = query - 'a';

            // test j < 23 in case i is greater than 22, only 22 spells
            // are actually shown on the screen, so limit choice to those
            if (c >= 0 && c < spellID && c < 22) {
                new_spells--;

                spell_learned |= 1L << spells[c];
                spell_order[last_known++] = (uint8_t) spells[c];

                for (; c <= spellID - 1; c++) {
                    spells[c] = spells[c + 1];
                }

                spellID--;

                erase_line(c + 1, 31);
                print_spells(spells, spellID, false, -1);
            } else {
                bell();
            }
        }

        restore_screen();
    } else {
        // pick a prayer at random
        while (new_spells) {
            int s = randint(spellID) - 1;
            spell_learned |= 1L << spells[s];
            spell_order[last_known++] = (uint8_t) spells[s];

            vtype_t tmp_str;
            (void) sprintf(tmp_str, "You have learned the prayer of %s.", spell_names[spells[s] + offset]);
            msg_print(tmp_str);

            for (; s <= spellID - 1; s++) {
                spells[s] = spells[s + 1];
            }
            spellID--;
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

static int newMana(int stat) {
    int levels = py.misc.lev - classes[py.misc.pclass].first_spell_lev + 1;

    switch (stat_adj(stat)) {
        case 0:
            return 0;
        case 1:
        case 2:
            return 1 * levels;
        case 3:
            return 3 * levels / 2;
        case 4:
            return 2 * levels;
        case 5:
            return 5 * levels / 2;
        case 6:
            return 3 * levels;
        case 7:
            return 4 * levels;
        default:
            return 0;
    }
}

// Gain some mana if you know at least one spell -RAK-
void calc_mana(int stat) {
    if (spell_learned != 0) {
        int new_mana = newMana(stat);

        // increment mana by one, so that first level chars have 2 mana
        if (new_mana > 0) {
            new_mana++;
        }

        // mana can be zero when creating character
        if (py.misc.mana != new_mana) {
            if (py.misc.mana != 0) {
                // change current mana proportionately to change of max mana,
                // divide first to avoid overflow, little loss of accuracy
                int32_t value = (((int32_t) py.misc.cmana << 16) + py.misc.cmana_frac) / py.misc.mana * new_mana;
                py.misc.cmana = (int16_t) (value >> 16);
                py.misc.cmana_frac = (uint16_t) (value & 0xFFFF);
            } else {
                py.misc.cmana = (int16_t) new_mana;
                py.misc.cmana_frac = 0;
            }

            py.misc.mana = (int16_t) new_mana;

            // can't print mana here, may be in store or inventory mode
            py.flags.status |= PY_MANA;
        }
    } else if (py.misc.mana != 0) {
        py.misc.mana = 0;
        py.misc.cmana = 0;

        // can't print mana here, may be in store or inventory mode
        py.flags.status |= PY_MANA;
    }
}

// Increases hit points and level -RAK-
static void gain_level() {
    py.misc.lev++;

    vtype_t out_val;
    (void) sprintf(out_val, "Welcome to level %d.", (int) py.misc.lev);
    msg_print(out_val);

    calc_hitpoints();

    int32_t need_exp = player_exp[py.misc.lev - 1] * py.misc.expfact / 100;
    if (py.misc.exp > need_exp) {
        // lose some of the 'extra' exp when gaining several levels at once
        int32_t dif_exp = py.misc.exp - need_exp;
        py.misc.exp = need_exp + (dif_exp / 2);
    }

    prt_level();
    prt_title();

    Class_t *c_ptr = &classes[py.misc.pclass];

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
    if (py.misc.exp > MAX_EXP) {
        py.misc.exp = MAX_EXP;
    }

    while ((py.misc.lev < MAX_PLAYER_LEVEL) && (signed) (player_exp[py.misc.lev - 1] * py.misc.expfact / 100) <= py.misc.exp) {
        gain_level();
    }

    if (py.misc.exp > py.misc.max_exp) {
        py.misc.max_exp = py.misc.exp;
    }

    prt_long(py.misc.exp, 14, STAT_COLUMN + 6);
}

// Calculate the players hit points
void calc_hitpoints() {
    int hitpoints = player_hp[py.misc.lev - 1] + (con_adj() * py.misc.lev);

    // always give at least one point per level + 1
    if (hitpoints < (py.misc.lev + 1)) {
        hitpoints = py.misc.lev + 1;
    }

    if (py.flags.status & PY_HERO) {
        hitpoints += 10;
    }

    if (py.flags.status & PY_SHERO) {
        hitpoints += 20;
    }

    // mhp can equal zero while character is being created
    if (hitpoints != py.misc.mhp && py.misc.mhp != 0) {
        // change current hit points proportionately to change of mhp,
        // divide first to avoid overflow, little loss of accuracy
        int32_t value = (((int32_t) py.misc.chp << 16) + py.misc.chp_frac) / py.misc.mhp * hitpoints;
        py.misc.chp = (int16_t) (value >> 16);
        py.misc.chp_frac = (uint16_t) (value & 0xFFFF);
        py.misc.mhp = (int16_t) hitpoints;

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
        vtype_t str1, str2;

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
    if (s * 15 < weight) {
        *wtohit = s * 15 - weight;
        return 1;
    }

    *wtohit = 0;

    int d = py.stats.use_stat[A_DEX];
    int dexterity;

    if (d < 10) {
        dexterity = 0;
    } else if (d < 19) {
        dexterity = 1;
    } else if (d < 68) {
        dexterity = 2;
    } else if (d < 108) {
        dexterity = 3;
    } else if (d < 118) {
        dexterity = 4;
    } else {
        dexterity = 5;
    }

    int adj_weight = (s * 10 / weight);
    int strength;

    if (adj_weight < 2) {
        strength = 0;
    } else if (adj_weight < 3) {
        strength = 1;
    } else if (adj_weight < 4) {
        strength = 2;
    } else if (adj_weight < 5) {
        strength = 3;
    } else if (adj_weight < 7) {
        strength = 4;
    } else if (adj_weight < 9) {
        strength = 5;
    } else {
        strength = 6;
    }

    return (int) blows_table[strength][dexterity];
}

// Special damage due to magical abilities of object -RAK-
int tot_dam(Inventory_t *i_ptr, int tdam, int monster) {
    bool isProjectile = i_ptr->tval >= TV_SLING_AMMO && i_ptr->tval <= TV_ARROW;
    bool isHaftedSword = i_ptr->tval >= TV_HAFTED && i_ptr->tval <= TV_SWORD;

    if ((i_ptr->flags & TR_EGO_WEAPON) && (isProjectile || isHaftedSword || i_ptr->tval == TV_FLASK)) {
        Creature_t *m_ptr = &c_list[monster];
        Recall_t *r_ptr = &c_recall[monster];

        // Slay Dragon
        if ((m_ptr->cdefense & CD_DRAGON) && (i_ptr->flags & TR_SLAY_DRAGON)) {
            r_ptr->r_cdefense |= CD_DRAGON;
            return tdam * 4;
        }

        // Slay Undead
        if ((m_ptr->cdefense & CD_UNDEAD) && (i_ptr->flags & TR_SLAY_UNDEAD)) {
            r_ptr->r_cdefense |= CD_UNDEAD;
            return tdam * 3;
        }

        // Slay Animal
        if ((m_ptr->cdefense & CD_ANIMAL) && (i_ptr->flags & TR_SLAY_ANIMAL)) {
            r_ptr->r_cdefense |= CD_ANIMAL;
            return tdam * 2;
        }

        // Slay Evil
        if ((m_ptr->cdefense & CD_EVIL) && (i_ptr->flags & TR_SLAY_EVIL)) {
            r_ptr->r_cdefense |= CD_EVIL;
            return tdam * 2;
        }

        // Frost
        if ((m_ptr->cdefense & CD_FROST) && (i_ptr->flags & TR_FROST_BRAND)) {
            r_ptr->r_cdefense |= CD_FROST;
            return tdam * 3 / 2;
        }

        // Fire
        if ((m_ptr->cdefense & CD_FIRE) && (i_ptr->flags & TR_FLAME_TONGUE)) {
            r_ptr->r_cdefense |= CD_FIRE;
            return tdam * 3 / 2;
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
    int classLevelAdjustment = class_level_adj[py.misc.pclass][CLA_SAVE] * py.misc.lev / 3;

    int saving = py.misc.save + stat_adj(A_WIS) + classLevelAdjustment;

    return randint(100) <= saving;
}

// Finds range of item in inventory list -RAK-
int find_range(int item1, int item2, int *j, int *k) {
    *j = -1;
    *k = -1;

    bool flag = false;

    for (int i = 0; i < inven_ctr; i++) {
        int itemID = (int) inventory[i].tval;

        if (!flag) {
            if (itemID == item1 || itemID == item2) {
                flag = true;
                *j = i;
            }
        } else {
            if (itemID != item1 && itemID != item2) {
                *k = i - 1;
                break;
            }
        }
    }

    if (flag && *k == -1) {
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
            y += (char_row - y) / 2;
            x += (char_col - x) / 2;
        }
    } while (cave[y][x].fval >= MIN_CLOSED_SPACE || cave[y][x].cptr >= 2);

    move_rec(char_row, char_col, y, x);

    for (int yy = char_row - 1; yy <= char_row + 1; yy++) {
        for (int xx = char_col - 1; xx <= char_col + 1; xx++) {
            cave[yy][xx].tl = false;
            lite_spot(yy, xx);
        }
    }

    lite_spot(char_row, char_col);

    char_row = (int16_t) y;
    char_col = (int16_t) x;

    check_view();
    creatures(false);

    teleport_flag = false;
}
