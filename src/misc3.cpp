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

static int spell_chance(int spell);

// Places a particular trap at location y, x -RAK-
void dungeonSetTrap(int y, int x, int sub_type_id) {
    int free_treasure_id = popt();
    cave[y][x].tptr = (uint8_t) free_treasure_id;
    inventoryItemCopyTo(OBJ_TRAP_LIST + sub_type_id, &treasure_list[free_treasure_id]);
}

// Places rubble at location y, x -RAK-
void dungeonPlaceRubble(int y, int x) {
    int free_treasure_id = popt();
    cave[y][x].tptr = (uint8_t) free_treasure_id;
    cave[y][x].fval = BLOCKED_FLOOR;
    inventoryItemCopyTo(OBJ_RUBBLE, &treasure_list[free_treasure_id]);
}

// Places a treasure (Gold or Gems) at given row, column -RAK-
void dungeonPlaceGold(int y, int x) {
    int free_treasure_id = popt();

    int gold_type_id = ((randomNumber(current_dungeon_level + 2) + 2) / 2) - 1;

    if (randomNumber(OBJ_GREAT) == 1) {
        gold_type_id += randomNumber(current_dungeon_level + 1);
    }

    if (gold_type_id >= MAX_GOLD) {
        gold_type_id = MAX_GOLD - 1;
    }

    cave[y][x].tptr = (uint8_t) free_treasure_id;
    inventoryItemCopyTo(OBJ_GOLD_LIST + gold_type_id, &treasure_list[free_treasure_id]);
    treasure_list[free_treasure_id].cost += (8L * (int32_t) randomNumber((int) treasure_list[free_treasure_id].cost)) + randomNumber(8);

    if (cave[y][x].cptr == 1) {
        printMessage("You feel something roll beneath your feet.");
    }
}

// Returns the array number of a random object -RAK-
int itemGetRandomObjectId(int level, bool must_be_small) {
    if (level == 0) {
        return randomNumber(treasure_levels[0]) - 1;
    }

    if (level >= MAX_OBJ_LEVEL) {
        level = MAX_OBJ_LEVEL;
    } else if (randomNumber(OBJ_GREAT) == 1) {
        level = level * MAX_OBJ_LEVEL / randomNumber(MAX_OBJ_LEVEL) + 1;
        if (level > MAX_OBJ_LEVEL) {
            level = MAX_OBJ_LEVEL;
        }
    }

    int object_id;

    // This code has been added to make it slightly more likely to get the
    // higher level objects.  Originally a uniform distribution over all
    // objects less than or equal to the dungeon level. This distribution
    // makes a level n objects occur approx 2/n% of the time on level n,
    // and 1/2n are 0th level.
    do {
        if (randomNumber(2) == 1) {
            object_id = randomNumber(treasure_levels[level]) - 1;
        } else {
            // Choose three objects, pick the highest level.
            object_id = randomNumber(treasure_levels[level]) - 1;

            int j = randomNumber(treasure_levels[level]) - 1;

            if (object_id < j) {
                object_id = j;
            }

            j = randomNumber(treasure_levels[level]) - 1;

            if (object_id < j) {
                object_id = j;
            }

            int foundLevel = game_objects[sorted_objects[object_id]].level;

            if (foundLevel == 0) {
                object_id = randomNumber(treasure_levels[0]) - 1;
            } else {
                object_id = randomNumber(treasure_levels[foundLevel] - treasure_levels[foundLevel - 1]) - 1 + treasure_levels[foundLevel - 1];
            }
        }
    } while (must_be_small && set_large(&game_objects[sorted_objects[object_id]]));

    return object_id;
}

// Places an object at given row, column co-ordinate -RAK-
void dungeonPlaceRandomObjectAt(int y, int x, bool must_be_small) {
    int free_treasure_id = popt();

    cave[y][x].tptr = (uint8_t) free_treasure_id;

    int object_id = itemGetRandomObjectId(current_dungeon_level, must_be_small);
    inventoryItemCopyTo(sorted_objects[object_id], &treasure_list[free_treasure_id]);

    magicTreasureMagicalAbility(free_treasure_id, current_dungeon_level);

    if (cave[y][x].cptr == 1) {
        printMessage("You feel something roll beneath your feet."); // -CJS-
    }
}

// Allocates an object for tunnels and rooms -RAK-
void dungeonAllocateAndPlaceObject(bool (*set_function)(int), int object_type, int number) {
    int y, x;

    for (int i = 0; i < number; i++) {
        // don't put an object beneath the player, this could cause
        // problems if player is standing under rubble, or on a trap.
        do {
            y = randomNumber(dungeon_height) - 1;
            x = randomNumber(dungeon_width) - 1;
        } while (!(*set_function)(cave[y][x].fval) || cave[y][x].tptr != 0 || (y == char_row && x == char_col));

        switch (object_type) {
            case 1:
                dungeonSetTrap(y, x, randomNumber(MAX_TRAP) - 1);
                break;
            case 2:
                // NOTE: object_type == 2 is no longer used - it used to be visible traps.
                // FIXME: there was no `break` here so `case 3` catches it? -MRC-
            case 3:
                dungeonPlaceRubble(y, x);
                break;
            case 4:
                dungeonPlaceGold(y, x);
                break;
            case 5:
                dungeonPlaceRandomObjectAt(y, x, false);
                break;
            default:
                break;
        }
    }
}

// Creates objects nearby the coordinates given -RAK-
void dungeonPlaceRandomObjectNear(int y, int x, int tries) {
    do {
        for (int i = 0; i <= 10; i++) {
            int j = y - 3 + randomNumber(5);
            int k = x - 4 + randomNumber(7);

            if (coordInBounds(j, k) && cave[j][k].fval <= MAX_CAVE_FLOOR && cave[j][k].tptr == 0) {
                if (randomNumber(100) < 75) {
                    dungeonPlaceRandomObjectAt(j, k, false);
                } else {
                    dungeonPlaceGold(j, k);
                }
                i = 9;
            }
        }

        tries--;
    } while (tries != 0);
}

// Converts stat num into string -RAK-
void statsAsString(uint8_t stat, char *stat_string) {
    if (stat <= 18) {
        (void) sprintf(stat_string, "%6d", stat);
        return;
    }

    int part1 = 18;
    int part2 = stat - 18;

    if (part2 == 100) {
        (void) strcpy(stat_string, "18/100");
        return;
    }

    (void) sprintf(stat_string, " %2d/%02d", part1, part2);
}

// Print character stat in given row, column -RAK-
void displayCharacterStats(int stat) {
    char text[7];
    statsAsString(py.stats.use_stat[stat], text);
    putString(stat_names[stat], 6 + stat, STAT_COLUMN);
    putString(text, 6 + stat, STAT_COLUMN + 6);
}

// Print character info in given row, column -RAK-
// The longest title is 13 characters, so only pad to 13
static void printCharacterInfoInField(const char *info, int row, int column) {
    // blank out the current field space
    putString(&blank_string[BLANK_LENGTH - 13], row, column);

    putString(info, row, column);
}

// Print long number with header at given row, column
static void printHeaderLongNumber(const char *header, int32_t num, int row, int column) {
    vtype_t str;
    (void) sprintf(str, "%s: %6d", header, num);
    putString(str, row, column);
}

// Print long number (7 digits of space) with header at given row, column
static void printHeaderLongNumber7Spaces(const char *header, int32_t num, int row, int column) {
    vtype_t str;
    (void) sprintf(str, "%s: %7d", header, num);
    putString(str, row, column);
}

// Print number with header at given row, column -RAK-
static void printHeaderNumber(const char *header, int num, int row, int column) {
    vtype_t str;
    (void) sprintf(str, "%s: %6d", header, num);
    putString(str, row, column);
}

// Print long number at given row, column
static void printLongNumber(int32_t num, int row, int column) {
    vtype_t str;
    (void) sprintf(str, "%6d", num);
    putString(str, row, column);
}

// Print number at given row, column -RAK-
static void printNumber(int num, int row, int column) {
    vtype_t str;
    (void) sprintf(str, "%6d", num);
    putString(str, row, column);
}

// Adjustment for wisdom/intelligence -JWT-
int playerStatAdjustmentWisdomIntelligence(int stat) {
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
int playerStatAdjustmentCharisma() {
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
int playerStatAdjustmentConstitution() {
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

char *playerTitle() {
    const char *p;

    if (py.misc.lev < 1) {
        p = "Babe in arms";
    } else if (py.misc.lev <= MAX_PLAYER_LEVEL) {
        p = class_titles[py.misc.pclass][py.misc.lev - 1];
    } else if (py.misc.male) {
        p = "**KING**";
    } else {
        p = "**QUEEN**";
    }

    return (char *) p;
}

// Prints title of character -RAK-
void printCharacterTitle() {
    printCharacterInfoInField(playerTitle(), 4, STAT_COLUMN);
}

// Prints level -RAK-
void printCharacterLevel() {
    printNumber((int) py.misc.lev, 13, STAT_COLUMN + 6);
}

// Prints players current mana points. -RAK-
void printCharacterCurrentMana() {
    printNumber(py.misc.cmana, 15, STAT_COLUMN + 6);
}

// Prints Max hit points -RAK-
void printCharacterMaxHitPoints() {
    printNumber(py.misc.mhp, 16, STAT_COLUMN + 6);
}

// Prints players current hit points -RAK-
void printCharacterCurrentHitPoints() {
    printNumber(py.misc.chp, 17, STAT_COLUMN + 6);
}

// prints current AC -RAK-
void printCharacterCurrentArmorClass() {
    printNumber(py.misc.dis_ac, 19, STAT_COLUMN + 6);
}

// Prints current gold -RAK-
void printCharacterGoldValue() {
    printLongNumber(py.misc.au, 20, STAT_COLUMN + 6);
}

// Prints depth in stat area -RAK-
void printCharacterCurrentDepth() {
    vtype_t depths;

    int depth = current_dungeon_level * 50;

    if (depth == 0) {
        (void) strcpy(depths, "Town level");
    } else {
        (void) sprintf(depths, "%d feet", depth);
    }

    putStringClearToEOL(depths, 23, 65);
}

// Prints status of hunger -RAK-
void printCharacterHungerstatus() {
    if (PY_WEAK & py.flags.status) {
        putString("Weak  ", 23, 0);
    } else if (PY_HUNGRY & py.flags.status) {
        putString("Hungry", 23, 0);
    } else {
        putString(&blank_string[BLANK_LENGTH - 6], 23, 0);
    }
}

// Prints Blind status -RAK-
void printCharacterBlindStatus() {
    if (PY_BLIND & py.flags.status) {
        putString("Blind", 23, 7);
    } else {
        putString(&blank_string[BLANK_LENGTH - 5], 23, 7);
    }
}

// Prints Confusion status -RAK-
void printCharacterConfusedState() {
    if (PY_CONFUSED & py.flags.status) {
        putString("Confused", 23, 13);
    } else {
        putString(&blank_string[BLANK_LENGTH - 8], 23, 13);
    }
}

// Prints Fear status -RAK-
void printCharacterFearState() {
    if (PY_FEAR & py.flags.status) {
        putString("Afraid", 23, 22);
    } else {
        putString(&blank_string[BLANK_LENGTH - 6], 23, 22);
    }
}

// Prints Poisoned status -RAK-
void printCharacterPoisonedState() {
    if (PY_POISONED & py.flags.status) {
        putString("Poisoned", 23, 29);
    } else {
        putString(&blank_string[BLANK_LENGTH - 8], 23, 29);
    }
}

// Prints Searching, Resting, Paralysis, or 'count' status -RAK-
void printCharacterMovementState() {
    py.flags.status &= ~PY_REPEAT;

    if (py.flags.paralysis > 1) {
        putString("Paralysed", 23, 38);
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

        putString(restString, 23, 38);

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

        putString(repeatString, 23, 38);

        if (PY_SEARCH & py.flags.status) {
            putString("Search", 23, 38);
        }

        return;
    }

    if (PY_SEARCH & py.flags.status) {
        putString("Searching", 23, 38);
        return;
    }

    // "repeat 999" is 10 characters
    putString(&blank_string[BLANK_LENGTH - 10], 23, 38);
}

// Prints the speed of a character. -CJS-
void printCharacterSpeed() {
    int speed = py.flags.speed;

    // Search mode.
    if (PY_SEARCH & py.flags.status) {
        speed--;
    }

    if (speed > 1) {
        putString("Very Slow", 23, 49);
    } else if (speed == 1) {
        putString("Slow     ", 23, 49);
    } else if (speed == 0) {
        putString(&blank_string[BLANK_LENGTH - 9], 23, 49);
    } else if (speed == -1) {
        putString("Fast     ", 23, 49);
    } else {
        putString("Very Fast", 23, 49);
    }
}

void printCharacterStudyInstruction() {
    py.flags.status &= ~PY_STUDY;

    if (py.flags.new_spells == 0) {
        putString(&blank_string[BLANK_LENGTH - 5], 23, 59);
    } else {
        putString("Study", 23, 59);
    }
}

// Prints winner status on display -RAK-
void printCharacterWinner() {
    if (noscore & 0x2) {
        if (wizard_mode) {
            putString("Is wizard  ", 22, 0);
        } else {
            putString("Was wizard ", 22, 0);
        }
    } else if (noscore & 0x1) {
        putString("Resurrected", 22, 0);
    } else if (noscore & 0x4) {
        putString("Duplicate", 22, 0);
    } else if (total_winner) {
        putString("*Winner*   ", 22, 0);
    }
}

static uint8_t playerModifyStat(int stat, int16_t amount) {
    uint8_t new_stat = py.stats.cur_stat[stat];

    int loop = (amount < 0 ? -amount : amount);

    for (int i = 0; i < loop; i++) {
        if (amount > 0) {
            if (new_stat < 18) {
                new_stat++;
            } else if (new_stat < 108) {
                new_stat += 10;
            } else {
                new_stat = 118;
            }
        } else {
            if (new_stat > 27) {
                new_stat -= 10;
            } else if (new_stat > 18) {
                new_stat = 18;
            } else if (new_stat > 3) {
                new_stat--;
            }
        }
    }

    return new_stat;
}

// Set the value of the stat which is actually used. -CJS-
void playerSetAndUseStat(int stat) {
    py.stats.use_stat[stat] = playerModifyStat(stat, py.stats.mod_stat[stat]);

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
bool playerStatRandomIncrease(int stat) {
    int new_stat = py.stats.cur_stat[stat];

    if (new_stat >= 118) {
        return false;
    }

    if (new_stat < 18) {
        new_stat++;
    } else if (new_stat < 116) {
        // stat increases by 1/6 to 1/3 of difference from max
        int gain = ((118 - new_stat) / 3 + 1) >> 1;

        new_stat += randomNumber(gain) + gain;
    } else {
        new_stat++;
    }

    py.stats.cur_stat[stat] = (uint8_t) new_stat;

    if (new_stat > py.stats.max_stat[stat]) {
        py.stats.max_stat[stat] = (uint8_t) new_stat;
    }

    playerSetAndUseStat(stat);
    displayCharacterStats(stat);

    return true;
}

// Decreases a stat by one randomized level -RAK-
bool playerStatRandomDecrease(int stat) {
    int new_stat = py.stats.cur_stat[stat];

    if (new_stat <= 3) {
        return false;
    }

    if (new_stat < 19) {
        new_stat--;
    } else if (new_stat < 117) {
        int loss = (((118 - new_stat) >> 1) + 1) >> 1;
        new_stat += -randomNumber(loss) - loss;

        if (new_stat < 18) {
            new_stat = 18;
        }
    } else {
        new_stat--;
    }

    py.stats.cur_stat[stat] = (uint8_t) new_stat;

    playerSetAndUseStat(stat);
    displayCharacterStats(stat);

    return true;
}

// Restore a stat.  Return true only if this actually makes a difference.
bool playerStatRestore(int stat) {
    int new_stat = py.stats.max_stat[stat] - py.stats.cur_stat[stat];

    if (new_stat == 0) {
        return false;
    }

    py.stats.cur_stat[stat] += new_stat;

    playerSetAndUseStat(stat);
    displayCharacterStats(stat);

    return true;
}

// Boost a stat artificially (by wearing something). If the display
// argument is true, then increase is shown on the screen.
void playerStatBoost(int stat, int amount) {
    py.stats.mod_stat[stat] += amount;

    playerSetAndUseStat(stat);

    // can not call displayCharacterStats() here, may be in store, may be in inven_command
    py.flags.status |= (PY_STR << stat);
}

// Returns a character's adjustment to hit. -JWT-
int playerToHitAdjustment() {
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
    printCharacterInfoInField(character_races[py.misc.prace].trace, 2, STAT_COLUMN);
    printCharacterInfoInField(classes[py.misc.pclass].title, 3, STAT_COLUMN);
    printCharacterInfoInField(playerTitle(), 4, STAT_COLUMN);

    for (int i = 0; i < 6; i++) {
        displayCharacterStats(i);
    }

    printHeaderNumber("LEV ", (int) py.misc.lev, 13, STAT_COLUMN);
    printHeaderLongNumber("EXP ", py.misc.exp, 14, STAT_COLUMN);
    printHeaderNumber("MANA", py.misc.cmana, 15, STAT_COLUMN);
    printHeaderNumber("MHP ", py.misc.mhp, 16, STAT_COLUMN);
    printHeaderNumber("CHP ", py.misc.chp, 17, STAT_COLUMN);
    printHeaderNumber("AC  ", py.misc.dis_ac, 19, STAT_COLUMN);
    printHeaderLongNumber("GOLD", py.misc.au, 20, STAT_COLUMN);
    printCharacterWinner();

    uint32_t status = py.flags.status;

    if ((PY_HUNGRY | PY_WEAK) & status) {
        printCharacterHungerstatus();
    }

    if (PY_BLIND & status) {
        printCharacterBlindStatus();
    }

    if (PY_CONFUSED & status) {
        printCharacterConfusedState();
    }

    if (PY_FEAR & status) {
        printCharacterFearState();
    }

    if (PY_POISONED & status) {
        printCharacterPoisonedState();
    }

    if ((PY_SEARCH | PY_REST) & status) {
        printCharacterMovementState();
    }

    // if speed non zero, print it, modify speed if Searching
    int16_t speed = py.flags.speed - (int16_t) ((PY_SEARCH & status) >> 8);
    if (speed != 0) {
        printCharacterSpeed();
    }

    // display the study field
    printCharacterStudyInstruction();
}

// Draws entire screen -RAK-
void draw_cave() {
    clearScreen();
    prt_stat_block();
    drawDungeonPanel();
    printCharacterCurrentDepth();
}

// Prints the following information on the screen. -JWT-
void put_character() {
    clearScreen();

    putString("Name        :", 2, 1);
    putString("Race        :", 3, 1);
    putString("Sex         :", 4, 1);
    putString("Class       :", 5, 1);

    if (!character_generated) {
        return;
    }

    putString(py.misc.name, 2, 15);
    putString(character_races[py.misc.prace].trace, 3, 15);
    putString((py.misc.male ? "Male" : "Female"), 4, 15);
    putString(classes[py.misc.pclass].title, 5, 15);
}

// Prints the following information on the screen. -JWT-
void put_stats() {
    for (int i = 0; i < 6; i++) {
        vtype_t buf;

        statsAsString(py.stats.use_stat[i], buf);
        putString(stat_names[i], 2 + i, 61);
        putString(buf, 2 + i, 66);

        if (py.stats.max_stat[i] > py.stats.cur_stat[i]) {
            statsAsString(py.stats.max_stat[i], buf);
            putString(buf, 2 + i, 73);
        }
    }

    printHeaderNumber("+ To Hit    ", py.misc.dis_th, 9, 1);
    printHeaderNumber("+ To Damage ", py.misc.dis_td, 10, 1);
    printHeaderNumber("+ To AC     ", py.misc.dis_tac, 11, 1);
    printHeaderNumber("  Total AC  ", py.misc.dis_ac, 12, 1);
}

// Returns a rating of x depending on y -JWT-
const char *likert(int y, int x) {
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
    printHeaderNumber("Age          ", (int) py.misc.age, 2, 38);
    printHeaderNumber("Height       ", (int) py.misc.ht, 3, 38);
    printHeaderNumber("Weight       ", (int) py.misc.wt, 4, 38);
    printHeaderNumber("Social Class ", (int) py.misc.sc, 5, 38);
}

// Prints the following information on the screen. -JWT-
void put_misc2() {
    printHeaderLongNumber7Spaces("Level      ", (int32_t) py.misc.lev, 9, 28);
    printHeaderLongNumber7Spaces("Experience ", py.misc.exp, 10, 28);
    printHeaderLongNumber7Spaces("Max Exp    ", py.misc.max_exp, 11, 28);

    if (py.misc.lev >= MAX_PLAYER_LEVEL) {
        putStringClearToEOL("Exp to Adv.: *******", 12, 28);
    } else {
        printHeaderLongNumber7Spaces("Exp to Adv.", (int32_t) (player_base_exp_levels[py.misc.lev - 1] * py.misc.expfact / 100), 12, 28);
    }

    printHeaderLongNumber7Spaces("Gold       ", py.misc.au, 13, 28);
    printHeaderNumber("Max Hit Points ", py.misc.mhp, 9, 52);
    printHeaderNumber("Cur Hit Points ", py.misc.chp, 10, 52);
    printHeaderNumber("Max Mana       ", py.misc.mana, 11, 52);
    printHeaderNumber("Cur Mana       ", py.misc.cmana, 12, 52);
}

// Prints ratings on certain abilities -RAK-
void put_misc3() {
    clearToBottom(14);

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
    int xdis = py.misc.disarm + 2 * todis_adj() + playerStatAdjustmentWisdomIntelligence(A_INT) + (class_level_adj[py.misc.pclass][CLA_DISARM] * py.misc.lev / 3);
    int xsave = py.misc.save + playerStatAdjustmentWisdomIntelligence(A_WIS) + (class_level_adj[py.misc.pclass][CLA_SAVE] * py.misc.lev / 3);
    int xdev = py.misc.save + playerStatAdjustmentWisdomIntelligence(A_INT) + (class_level_adj[py.misc.pclass][CLA_DEVICE] * py.misc.lev / 3);

    vtype_t xinfra;
    (void) sprintf(xinfra, "%d feet", py.flags.see_infra * 10);

    putString("(Miscellaneous Abilities)", 15, 25);
    putString("Fighting    :", 16, 1);
    putString(likert(12, xbth), 16, 15);
    putString("Bows/Throw  :", 17, 1);
    putString(likert(12, xbthb), 17, 15);
    putString("Saving Throw:", 18, 1);
    putString(likert(6, xsave), 18, 15);

    putString("Stealth     :", 16, 28);
    putString(likert(1, xstl), 16, 42);
    putString("Disarming   :", 17, 28);
    putString(likert(8, xdis), 17, 42);
    putString("Magic Device:", 18, 28);
    putString(likert(6, xdev), 18, 42);

    putString("Perception  :", 16, 55);
    putString(likert(3, xfos), 16, 69);
    putString("Searching   :", 17, 55);
    putString(likert(6, xsrh), 17, 69);
    putString("Infra-Vision:", 18, 55);
    putString(xinfra, 18, 69);
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
    putStringClearToEOL("Enter your player's name  [press <RETURN> when finished]", 21, 2);

    putString(&blank_string[BLANK_LENGTH - 23], 2, 15);

    if (!getStringInput(py.misc.name, 2, 15, 23) || py.misc.name[0] == 0) {
        getDefaultPlayerName(py.misc.name);
        putString(py.misc.name, 2, 15);
    }

    clearToBottom(20);
}

// Changes the name of the character -JWT-
void change_name() {
    vtype_t temp;
    bool flag = false;

    display_char();

    while (!flag) {
        putStringClearToEOL("<f>ile character description. <c>hange character name.", 21, 2);

        switch (getKeyInput()) {
            case 'c':
                get_name();
                flag = true;
                break;
            case 'f':
                putStringClearToEOL("File name:", 0, 0);

                if (getStringInput(temp, 0, 10, 60) && temp[0]) {
                    if (outputPlayerCharacterToFile(temp)) {
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
                terminalBellSound();
                break;
        }
    }
}

// Destroy an item in the inventory -RAK-
void inven_destroy(int item_id) {
    Inventory_t *i_ptr = &inventory[item_id];

    if (i_ptr->number > 1 && i_ptr->subval <= ITEM_SINGLE_STACK_MAX) {
        i_ptr->number--;
        inventory_weight -= i_ptr->weight;
    } else {
        inventory_weight -= i_ptr->weight * i_ptr->number;

        for (int i = item_id; i < inventory_count - 1; i++) {
            inventory[i] = inventory[i + 1];
        }

        inventoryItemCopyTo(OBJ_NOTHING, &inventory[inventory_count - 1]);
        inventory_count--;
    }

    py.flags.status |= PY_STR_WGT;
}

// Copies the object in the second argument over the first argument.
// However, the second always gets a number of one except for ammo etc.
void take_one_item(Inventory_t *to_item, Inventory_t *from_item) {
    *to_item = *from_item;

    if (to_item->number > 1 && to_item->subval >= ITEM_SINGLE_STACK_MIN && to_item->subval <= ITEM_SINGLE_STACK_MAX) {
        to_item->number = 1;
    }
}

// Drops an item from inventory to given location -RAK-
void inven_drop(int item_id, bool drop_all) {
    if (cave[char_row][char_col].tptr != 0) {
        (void) delete_object(char_row, char_col);
    }

    int treasureID = popt();

    Inventory_t *i_ptr = &inventory[item_id];
    treasure_list[treasureID] = *i_ptr;

    cave[char_row][char_col].tptr = (uint8_t) treasureID;

    if (item_id >= INVEN_WIELD) {
        takeoff(item_id, -1);
    } else {
        if (drop_all || i_ptr->number == 1) {
            inventory_weight -= i_ptr->weight * i_ptr->number;
            inventory_count--;

            while (item_id < inventory_count) {
                inventory[item_id] = inventory[item_id + 1];
                item_id++;
            }

            inventoryItemCopyTo(OBJ_NOTHING, &inventory[inventory_count]);
        } else {
            treasure_list[treasureID].number = 1;
            inventory_weight -= i_ptr->weight;
            i_ptr->number--;
        }

        obj_desc_t prt1, prt2;
        itemDescription(prt1, &treasure_list[treasureID], true);
        (void) sprintf(prt2, "Dropped %s", prt1);
        printMessage(prt2);
    }

    py.flags.status |= PY_STR_WGT;
}

// Destroys a type of item on a given percent chance -RAK-
int inven_damage(bool (*item_type)(Inventory_t *), int chance_percentage) {
    int damage = 0;

    for (int i = 0; i < inventory_count; i++) {
        if ((*item_type)(&inventory[i]) && randomNumber(100) < chance_percentage) {
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
bool inven_check_num(Inventory_t *item) {
    if (inventory_count < INVEN_WIELD) {
        return true;
    }

    if (item->subval < ITEM_SINGLE_STACK_MIN) {
        return false;
    }

    for (int i = 0; i < inventory_count; i++) {
        bool sameCharacter = inventory[i].tval == item->tval;
        bool sameCategory = inventory[i].subval == item->subval;

        // make sure the number field doesn't overflow
        bool sameNumber = inventory[i].number + item->number < 256;

        // they always stack (subval < 192), or else they have same p1
        bool sameGroup = item->subval < ITEM_GROUP_MIN || inventory[i].p1 == item->p1;

        // only stack if both or neither are identified
        bool identification = itemSetColorlessAsIdentifed(&inventory[i]) == itemSetColorlessAsIdentifed(item);

        if (sameCharacter && sameCategory && sameNumber && sameGroup && identification) {
            return true;
        }
    }

    return false;
}

// return false if picking up an object would change the players speed
bool inven_check_weight(Inventory_t *item) {
    int limit = weight_limit();
    int newWeight = item->number * item->weight + inventory_weight;

    if (limit < newWeight) {
        limit = newWeight / (limit + 1);
    } else {
        limit = 0;
    }

    return pack_heaviness == limit;
}

// Are we strong enough for the current pack and weapon? -CJS-
void check_strength() {
    Inventory_t *i_ptr = &inventory[INVEN_WIELD];

    if (i_ptr->tval != TV_NOTHING && py.stats.use_stat[A_STR] * 15 < i_ptr->weight) {
        if (!weapon_is_heavy) {
            printMessage("You have trouble wielding such a heavy weapon.");
            weapon_is_heavy = true;
            calc_bonuses();
        }
    } else if (weapon_is_heavy) {
        weapon_is_heavy = false;
        if (i_ptr->tval != TV_NOTHING) {
            printMessage("You are strong enough to wield your weapon.");
        }
        calc_bonuses();
    }

    int limit = weight_limit();

    if (limit < inventory_weight) {
        limit = inventory_weight / (limit + 1);
    } else {
        limit = 0;
    }

    if (pack_heaviness != limit) {
        if (pack_heaviness < limit) {
            printMessage("Your pack is so heavy that it slows you down.");
        } else {
            printMessage("You move more easily under the weight of your pack.");
        }
        change_speed(limit - pack_heaviness);
        pack_heaviness = limit;
    }

    py.flags.status &= ~PY_STR_WGT;
}

// Add an item to players inventory.  Return the
// item position for a description if needed. -RAK-
// this code must be identical to the inven_check_num() code above
int inven_carry(Inventory_t *item) {
    int typ = item->tval;
    int subt = item->subval;
    bool known1p = itemSetColorlessAsIdentifed(item);
    int always_known1p = (objectPositionOffset(item) == -1);

    int locn;

    // Now, check to see if player can carry object
    for (locn = 0;; locn++) {
        Inventory_t *t_ptr = &inventory[locn];

        if (typ == t_ptr->tval && subt == t_ptr->subval && subt >= ITEM_SINGLE_STACK_MIN && ((int) t_ptr->number + (int) item->number) < 256 &&
            (subt < ITEM_GROUP_MIN || t_ptr->p1 == item->p1) &&
            // only stack if both or neither are identified
            known1p == itemSetColorlessAsIdentifed(t_ptr)) {
            t_ptr->number += item->number;

            break;
        }

        if ((typ == t_ptr->tval && subt < t_ptr->subval && always_known1p) || typ > t_ptr->tval) {
            // For items which are always known1p, i.e. never have a 'color',
            // insert them into the inventory in sorted order.
            for (int i = inventory_count - 1; i >= locn; i--) {
                inventory[i + 1] = inventory[i];
            }

            inventory[locn] = *item;
            inventory_count++;

            break;
        }
    }

    inventory_weight += item->number * item->weight;
    py.flags.status |= PY_STR_WGT;

    return locn;
}

// Returns spell chance of failure for spell -RAK-
static int spell_chance(int spell) {
    Spell_t *s_ptr = &magic_spells[py.misc.pclass - 1][spell];

    int chance = s_ptr->sfail - 3 * (py.misc.lev - s_ptr->slevel);

    int stat;
    if (classes[py.misc.pclass].spell == MAGE) {
        stat = A_INT;
    } else {
        stat = A_WIS;
    }

    chance -= 3 * (playerStatAdjustmentWisdomIntelligence(stat) - 1);

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
void print_spells(int *spell, int number_of_choices, int comment, int non_consecutive) {
    int col;
    if (comment) {
        col = 22;
    } else {
        col = 31;
    }

    int offset = (classes[py.misc.pclass].spell == MAGE ? SPELL_OFFSET : PRAYER_OFFSET);

    eraseLine(1, col);
    putString("Name", 1, col + 5);
    putString("Lv Mana Fail", 1, col + 35);

    // only show the first 22 choices
    if (number_of_choices > 22) {
        number_of_choices = 22;
    }

    for (int i = 0; i < number_of_choices; i++) {
        int spellID = spell[i];
        Spell_t *s_ptr = &magic_spells[py.misc.pclass - 1][spellID];

        const char *p;
        if (comment == 0) {
            p = "";
        } else if ((spells_forgotten & (1L << spellID)) != 0) {
            p = " forgotten";
        } else if ((spells_learnt & (1L << spellID)) == 0) {
            p = " unknown";
        } else if ((spells_worked & (1L << spellID)) == 0) {
            p = " untried";
        } else {
            p = "";
        }

        // determine whether or not to leave holes in character choices, non_consecutive -1
        // when learning spells, consec offset>=0 when asking which spell to cast.
        char spell_char;
        if (non_consecutive == -1) {
            spell_char = (char) ('a' + i);
        } else {
            spell_char = (char) ('a' + spellID - non_consecutive);
        }

        vtype_t out_val;
        (void) sprintf(out_val, "  %c) %-30s%2d %4d %3d%%%s", spell_char, spell_names[spellID + offset], s_ptr->slevel, s_ptr->smana, spell_chance(spellID), p);
        putStringClearToEOL(out_val, 2 + i, col);
    }
}

// Returns spell pointer -RAK-
int get_spell(int *spell, int number_of_choices, int *spell_id, int *spell_chances, const char *prompt, int first_spell) {
    *spell_id = -1;

    vtype_t str;
    (void) sprintf(str, "(Spells %c-%c, *=List, <ESCAPE>=exit) %s", spell[0] + 'a' - first_spell, spell[number_of_choices - 1] + 'a' - first_spell, prompt);

    bool flag = false;
    bool redraw = false;

    int offset = (classes[py.misc.pclass].spell == MAGE ? SPELL_OFFSET : PRAYER_OFFSET);

    char choice;

    while (!flag && getCommand(str, &choice)) {
        if (isupper((int) choice)) {
            *spell_id = choice - 'A' + first_spell;

            // verify that this is in spell[], at most 22 entries in spell[]
            int spellID;
            for (spellID = 0; spellID < number_of_choices; spellID++) {
                if (*spell_id == spell[spellID]) {
                    break;
                }
            }

            if (spellID == number_of_choices) {
                *spell_id = -2;
            } else {
                Spell_t *s_ptr = &magic_spells[py.misc.pclass - 1][*spell_id];

                vtype_t tmp_str;
                (void) sprintf(tmp_str, "Cast %s (%d mana, %d%% fail)?", spell_names[*spell_id + offset], s_ptr->smana, spell_chance(*spell_id));
                if (getInputConfirmation(tmp_str)) {
                    flag = true;
                } else {
                    *spell_id = -1;
                }
            }
        } else if (islower((int) choice)) {
            *spell_id = choice - 'a' + first_spell;

            // verify that this is in spell[], at most 22 entries in spell[]
            int spellID;
            for (spellID = 0; spellID < number_of_choices; spellID++) {
                if (*spell_id == spell[spellID]) {
                    break;
                }
            }

            if (spellID == number_of_choices) {
                *spell_id = -2;
            } else {
                flag = true;
            }
        } else if (choice == '*') {
            // only do this drawing once
            if (!redraw) {
                terminalSaveScreen();
                redraw = true;
                print_spells(spell, number_of_choices, false, first_spell);
            }
        } else if (isalpha((int) choice)) {
            *spell_id = -2;
        } else {
            *spell_id = -1;
            terminalBellSound();
        }

        if (*spell_id == -2) {
            vtype_t tmp_str;
            (void) sprintf(tmp_str, "You don't know that %s.", (offset == SPELL_OFFSET ? "spell" : "prayer"));
            printMessage(tmp_str);
        }
    }

    if (redraw) {
        terminalRestoreScreen();
    }

    eraseLine(MSG_LINE, 0);

    if (flag) {
        *spell_chances = spell_chance(*spell_id);
    }

    return flag;
}

// check to see if know any spells greater than level, eliminate them
static void eliminateKnownSpellsGreaterThanLevel(Spell_t *msp_ptr, const char *p, int offset) {
    uint32_t mask = 0x80000000L;

    for (int i = 31; mask; mask >>= 1, i--) {
        if (mask & spells_learnt) {
            if (msp_ptr[i].slevel > py.misc.lev) {
                spells_learnt &= ~mask;
                spells_forgotten |= mask;

                vtype_t msg;
                (void) sprintf(msg, "You have forgotten the %s of %s.", p, spell_names[i + offset]);
                printMessage(msg);
            } else {
                break;
            }
        }
    }
}

static int numberOfSpellsAllowed(int stat) {
    int allowed = 0;

    int levels = py.misc.lev - classes[py.misc.pclass].first_spell_lev + 1;

    switch (playerStatAdjustmentWisdomIntelligence(stat)) {
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
        if (mask & spells_learnt) {
            known++;
        }
    }

    return known;
}

// remember forgotten spells while forgotten spells exist of new_spells positive,
// remember the spells in the order that they were learned
static int rememberForgottenSpells(Spell_t *msp_ptr, int allowedSpells, int newSpells, const char *p, int offset) {
    uint32_t mask;

    for (int n = 0; (spells_forgotten && newSpells && (n < allowedSpells) && (n < 32)); n++) {
        // orderID is (i+1)th spell learned
        int orderID = spells_learned_order[n];

        // shifting by amounts greater than number of bits in long gives
        // an undefined result, so don't shift for unknown spells
        if (orderID == 99) {
            mask = 0x0;
        } else {
            mask = (uint32_t) (1L << orderID);
        }

        if (mask & spells_forgotten) {
            if (msp_ptr[orderID].slevel <= py.misc.lev) {
                newSpells--;
                spells_forgotten &= ~mask;
                spells_learnt |= mask;

                vtype_t msg;
                (void) sprintf(msg, "You have remembered the %s of %s.", p, spell_names[orderID + offset]);
                printMessage(msg);
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
    uint32_t spell_flag = (uint32_t) (0x7FFFFFFFL & ~spells_learnt);

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

    for (int i = 31; newSpells && spells_learnt; i--) {
        // orderID is the (i+1)th spell learned
        int orderID = spells_learned_order[i];

        // shifting by amounts greater than number of bits in long gives
        // an undefined result, so don't shift for unknown spells
        if (orderID == 99) {
            mask = 0x0;
        } else {
            mask = (uint32_t) (1L << orderID);
        }

        if (mask & spells_learnt) {
            spells_learnt &= ~mask;
            spells_forgotten |= mask;
            newSpells++;

            vtype_t msg;
            (void) sprintf(msg, "You have forgotten the %s of %s.", p, spell_names[orderID + offset]);
            printMessage(msg);
        }
    }
}

// calculate number of spells player should have, and
// learn forget spells until that number is met -JEW-
void calc_spells(int stat) {
    Spell_t *msp_ptr = &magic_spells[py.misc.pclass - 1][0];

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
            printMessage(msg);
        }

        py.flags.new_spells = (uint8_t) new_spells;
        py.flags.status |= PY_STUDY;
    }
}

static bool playerCanRead() {
    if (py.flags.blind > 0) {
        printMessage("You can't see to read your spell book!");
        return false;
    }

    if (no_light()) {
        printMessage("You have no light to read by.");
        return false;
    }

    return true;
}

static int lastKnownSpell() {
    for (int last_known = 0; last_known < 32; last_known++) {
        if (spells_learned_order[last_known] == 99) {
            return last_known;
        }
    }

    // We should never actually reach this, but just in case... -MRC-
    return 0;
}

static uint32_t playerDetermineLearnableSpells() {
    uint32_t spell_flag = 0;

    for (int i = 0; i < inventory_count; i++) {
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
        printMessage("You are too confused.");
        return;
    }

    int new_spells = py.flags.new_spells;
    int diff_spells = 0;

    Spell_t *msp_ptr = &magic_spells[py.misc.pclass - 1][0];

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
        printMessage(tmp_str);

        player_free_turn = true;
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
    spell_flag &= ~spells_learnt;

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
        printMessage("You seem to be missing a book.");

        diff_spells = new_spells - spellID;
        new_spells = spellID;
    }

    if (new_spells == 0) {
        // do nothing
    } else if (stat == A_INT) {
        // get to choose which mage spells will be learned
        terminalSaveScreen();
        print_spells(spells, spellID, false, -1);

        char query;
        while (new_spells && getCommand("Learn which spell?", &query)) {
            int c = query - 'a';

            // test j < 23 in case i is greater than 22, only 22 spells
            // are actually shown on the screen, so limit choice to those
            if (c >= 0 && c < spellID && c < 22) {
                new_spells--;

                spells_learnt |= 1L << spells[c];
                spells_learned_order[last_known++] = (uint8_t) spells[c];

                for (; c <= spellID - 1; c++) {
                    spells[c] = spells[c + 1];
                }

                spellID--;

                eraseLine(c + 1, 31);
                print_spells(spells, spellID, false, -1);
            } else {
                terminalBellSound();
            }
        }

        terminalRestoreScreen();
    } else {
        // pick a prayer at random
        while (new_spells) {
            int s = randomNumber(spellID) - 1;
            spells_learnt |= 1L << spells[s];
            spells_learned_order[last_known++] = (uint8_t) spells[s];

            vtype_t tmp_str;
            (void) sprintf(tmp_str, "You have learned the prayer of %s.", spell_names[spells[s] + offset]);
            printMessage(tmp_str);

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

    switch (playerStatAdjustmentWisdomIntelligence(stat)) {
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
    if (spells_learnt != 0) {
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
    printMessage(out_val);

    calc_hitpoints();

    int32_t need_exp = player_base_exp_levels[py.misc.lev - 1] * py.misc.expfact / 100;
    if (py.misc.exp > need_exp) {
        // lose some of the 'extra' exp when gaining several levels at once
        int32_t dif_exp = py.misc.exp - need_exp;
        py.misc.exp = need_exp + (dif_exp / 2);
    }

    printCharacterLevel();
    printCharacterTitle();

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

    while ((py.misc.lev < MAX_PLAYER_LEVEL) && (signed) (player_base_exp_levels[py.misc.lev - 1] * py.misc.expfact / 100) <= py.misc.exp) {
        gain_level();
    }

    if (py.misc.exp > py.misc.max_exp) {
        py.misc.max_exp = py.misc.exp;
    }

    printLongNumber(py.misc.exp, 14, STAT_COLUMN + 6);
}

// Calculate the players hit points
void calc_hitpoints() {
    int hitpoints = player_base_hp_levels[py.misc.lev - 1] + (playerStatAdjustmentConstitution() * py.misc.lev);

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
void insert_str(char *to_string, const char *from_string, const char *str_to_insert) {
    int mtc_len = (int) strlen(from_string);
    int obj_len = (int) strlen(to_string);
    char *bound = to_string + obj_len - mtc_len;

    char *pc;
    for (pc = to_string; pc <= bound; pc++) {
        char *temp_obj = pc;
        const char *temp_mtc = from_string;

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

        (void) strncpy(out_val, to_string, (pc - to_string));
        // Turbo C needs int for array index.
        out_val[(int) (pc - to_string)] = '\0';
        if (str_to_insert) {
            (void) strcat(out_val, str_to_insert);
        }
        (void) strcat(out_val, (pc + mtc_len));
        (void) strcpy(to_string, out_val);
    }
}

void insert_lnum(char *to_string, const char *from_string, int32_t number, bool show_sign) {
    size_t mlen = strlen(from_string);
    char *tmp_str = to_string;
    char *string;

    int flag = 1;
    while (flag != 0) {
        string = strchr(tmp_str, from_string[0]);
        if (string == 0) {
            flag = 0;
        } else {
            flag = strncmp(string, from_string, mlen);
            if (flag) {
                tmp_str = string + 1;
            }
        }
    }

    if (string) {
        vtype_t str1, str2;

        (void) strncpy(str1, to_string, string - to_string);
        str1[string - to_string] = '\0';
        (void) strcpy(str2, string + mlen);

        if (number >= 0 && show_sign) {
            (void) sprintf(to_string, "%s+%d%s", str1, number, str2);
        } else {
            (void) sprintf(to_string, "%s%d%s", str1, number, str2);
        }
    }
}

// lets anyone enter wizard mode after a disclaimer... -JEW-
bool enter_wiz_mode() {
    bool answer = false;

    if (!noscore) {
        printMessage("Wizard mode is for debugging and experimenting.");
        answer = getInputConfirmation("The game will not be scored if you enter wizard mode. Are you sure?");
    }

    if (noscore || answer) {
        noscore |= 0x2;
        wizard_mode = true;
        return true;
    }

    return false;
}

// Weapon weight VS strength and dexterity -RAK-
int attack_blows(int weight, int *weight_to_hit) {
    int s = py.stats.use_stat[A_STR];
    if (s * 15 < weight) {
        *weight_to_hit = s * 15 - weight;
        return 1;
    }

    *weight_to_hit = 0;

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
int tot_dam(Inventory_t *item, int total_damage, int monster_id) {
    bool isProjectile = item->tval >= TV_SLING_AMMO && item->tval <= TV_ARROW;
    bool isHaftedSword = item->tval >= TV_HAFTED && item->tval <= TV_SWORD;

    if ((item->flags & TR_EGO_WEAPON) && (isProjectile || isHaftedSword || item->tval == TV_FLASK)) {
        Creature_t *m_ptr = &creatures_list[monster_id];
        Recall_t *r_ptr = &creature_recall[monster_id];

        // Slay Dragon
        if ((m_ptr->cdefense & CD_DRAGON) && (item->flags & TR_SLAY_DRAGON)) {
            r_ptr->r_cdefense |= CD_DRAGON;
            return total_damage * 4;
        }

        // Slay Undead
        if ((m_ptr->cdefense & CD_UNDEAD) && (item->flags & TR_SLAY_UNDEAD)) {
            r_ptr->r_cdefense |= CD_UNDEAD;
            return total_damage * 3;
        }

        // Slay Animal
        if ((m_ptr->cdefense & CD_ANIMAL) && (item->flags & TR_SLAY_ANIMAL)) {
            r_ptr->r_cdefense |= CD_ANIMAL;
            return total_damage * 2;
        }

        // Slay Evil
        if ((m_ptr->cdefense & CD_EVIL) && (item->flags & TR_SLAY_EVIL)) {
            r_ptr->r_cdefense |= CD_EVIL;
            return total_damage * 2;
        }

        // Frost
        if ((m_ptr->cdefense & CD_FROST) && (item->flags & TR_FROST_BRAND)) {
            r_ptr->r_cdefense |= CD_FROST;
            return total_damage * 3 / 2;
        }

        // Fire
        if ((m_ptr->cdefense & CD_FIRE) && (item->flags & TR_FLAME_TONGUE)) {
            r_ptr->r_cdefense |= CD_FIRE;
            return total_damage * 3 / 2;
        }
    }

    return total_damage;
}

// Critical hits, Nasty way to die. -RAK-
int critical_blow(int weapon_weight, int plus_to_hit, int damage, int attack_type_id) {
    int critical = damage;

    // Weight of weapon, plusses to hit, and character level all
    // contribute to the chance of a critical
    if (randomNumber(5000) <= weapon_weight + 5 * plus_to_hit + (class_level_adj[py.misc.pclass][attack_type_id] * py.misc.lev)) {
        weapon_weight += randomNumber(650);

        if (weapon_weight < 400) {
            critical = 2 * damage + 5;
            printMessage("It was a good hit! (x2 damage)");
        } else if (weapon_weight < 700) {
            critical = 3 * damage + 10;
            printMessage("It was an excellent hit! (x3 damage)");
        } else if (weapon_weight < 900) {
            critical = 4 * damage + 15;
            printMessage("It was a superb hit! (x4 damage)");
        } else {
            critical = 5 * damage + 20;
            printMessage("It was a *GREAT* hit! (x5 damage)");
        }
    }

    return critical;
}

// Given direction "dir", returns new row, column location -RAK-
bool mmove(int dir, int *new_y, int *new_x) {
    int new_row = 0;
    int new_col = 0;

    switch (dir) {
        case 1:
            new_row = *new_y + 1;
            new_col = *new_x - 1;
            break;
        case 2:
            new_row = *new_y + 1;
            new_col = *new_x;
            break;
        case 3:
            new_row = *new_y + 1;
            new_col = *new_x + 1;
            break;
        case 4:
            new_row = *new_y;
            new_col = *new_x - 1;
            break;
        case 5:
            new_row = *new_y;
            new_col = *new_x;
            break;
        case 6:
            new_row = *new_y;
            new_col = *new_x + 1;
            break;
        case 7:
            new_row = *new_y - 1;
            new_col = *new_x - 1;
            break;
        case 8:
            new_row = *new_y - 1;
            new_col = *new_x;
            break;
        case 9:
            new_row = *new_y - 1;
            new_col = *new_x + 1;
            break;
    }

    bool moved = false;

    if (new_row >= 0 && new_row < dungeon_height && new_col >= 0 && new_col < dungeon_width) {
        *new_y = new_row;
        *new_x = new_col;
        moved = true;
    }

    return moved;
}

// Saving throws for player character. -RAK-
bool player_saves() {
    int classLevelAdjustment = class_level_adj[py.misc.pclass][CLA_SAVE] * py.misc.lev / 3;

    int saving = py.misc.save + playerStatAdjustmentWisdomIntelligence(A_WIS) + classLevelAdjustment;

    return randomNumber(100) <= saving;
}

// Finds range of item in inventory list -RAK-
int find_range(int item_id_start, int item_id_end, int *j, int *k) {
    *j = -1;
    *k = -1;

    bool flag = false;

    for (int i = 0; i < inventory_count; i++) {
        int itemID = (int) inventory[i].tval;

        if (!flag) {
            if (itemID == item_id_start || itemID == item_id_end) {
                flag = true;
                *j = i;
            }
        } else {
            if (itemID != item_id_start && itemID != item_id_end) {
                *k = i - 1;
                break;
            }
        }
    }

    if (flag && *k == -1) {
        *k = inventory_count - 1;
    }

    return flag;
}

// Teleport the player to a new location -RAK-
void teleport(int new_distance) {
    int y, x;

    do {
        y = randomNumber(dungeon_height) - 1;
        x = randomNumber(dungeon_width) - 1;

        while (coordDistanceBetween(y, x, char_row, char_col) > new_distance) {
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
    updateMonsters(false);

    teleport_player = false;
}
