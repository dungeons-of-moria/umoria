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

static int spellChanceOfSuccess(int spell_id);

// Places a particular trap at location y, x -RAK-
void dungeonSetTrap(int y, int x, int sub_type_id) {
    int free_treasure_id = popt();
    cave[y][x].treasure_id = (uint8_t) free_treasure_id;
    inventoryItemCopyTo(OBJ_TRAP_LIST + sub_type_id, treasure_list[free_treasure_id]);
}

// Places rubble at location y, x -RAK-
void dungeonPlaceRubble(int y, int x) {
    int free_treasure_id = popt();
    cave[y][x].treasure_id = (uint8_t) free_treasure_id;
    cave[y][x].feature_id = TILE_BLOCKED_FLOOR;
    inventoryItemCopyTo(OBJ_RUBBLE, treasure_list[free_treasure_id]);
}

// Places a treasure (Gold or Gems) at given row, column -RAK-
void dungeonPlaceGold(int y, int x) {
    int free_treasure_id = popt();

    int gold_type_id = ((randomNumber(current_dungeon_level + 2) + 2) / 2) - 1;

    if (randomNumber(TREASURE_CHANCE_OF_GREAT_ITEM) == 1) {
        gold_type_id += randomNumber(current_dungeon_level + 1);
    }

    if (gold_type_id >= MAX_GOLD_TYPES) {
        gold_type_id = MAX_GOLD_TYPES - 1;
    }

    cave[y][x].treasure_id = (uint8_t) free_treasure_id;
    inventoryItemCopyTo(OBJ_GOLD_LIST + gold_type_id, treasure_list[free_treasure_id]);
    treasure_list[free_treasure_id].cost += (8L * (int32_t) randomNumber((int) treasure_list[free_treasure_id].cost)) + randomNumber(8);

    if (cave[y][x].creature_id == 1) {
        printMessage("You feel something roll beneath your feet.");
    }
}

// Returns the array number of a random object -RAK-
int itemGetRandomObjectId(int level, bool must_be_small) {
    if (level == 0) {
        return randomNumber(treasure_levels[0]) - 1;
    }

    if (level >= TREASURE_MAX_LEVELS) {
        level = TREASURE_MAX_LEVELS;
    } else if (randomNumber(TREASURE_CHANCE_OF_GREAT_ITEM) == 1) {
        level = level * TREASURE_MAX_LEVELS / randomNumber(TREASURE_MAX_LEVELS) + 1;
        if (level > TREASURE_MAX_LEVELS) {
            level = TREASURE_MAX_LEVELS;
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

            int foundLevel = game_objects[sorted_objects[object_id]].depth_first_found;

            if (foundLevel == 0) {
                object_id = randomNumber(treasure_levels[0]) - 1;
            } else {
                object_id = randomNumber(treasure_levels[foundLevel] - treasure_levels[foundLevel - 1]) - 1 + treasure_levels[foundLevel - 1];
            }
        }
    } while (must_be_small && setItemsLargerThanChests(&game_objects[sorted_objects[object_id]]));

    return object_id;
}

// Places an object at given row, column co-ordinate -RAK-
void dungeonPlaceRandomObjectAt(int y, int x, bool must_be_small) {
    int free_treasure_id = popt();

    cave[y][x].treasure_id = (uint8_t) free_treasure_id;

    int object_id = itemGetRandomObjectId(current_dungeon_level, must_be_small);
    inventoryItemCopyTo(sorted_objects[object_id], treasure_list[free_treasure_id]);

    magicTreasureMagicalAbility(free_treasure_id, current_dungeon_level);

    if (cave[y][x].creature_id == 1) {
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
        } while (!(*set_function)(cave[y][x].feature_id) || cave[y][x].treasure_id != 0 || (y == char_row && x == char_col));

        switch (object_type) {
            case 1:
                dungeonSetTrap(y, x, randomNumber(MAX_TRAPS) - 1);
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

            if (coordInBounds(j, k) && cave[j][k].feature_id <= MAX_CAVE_FLOOR && cave[j][k].treasure_id == 0) {
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
    statsAsString(py.stats.used[stat], text);
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
    vtype_t str = {'\0'};
    (void) sprintf(str, "%s: %6d", header, num);
    putString(str, row, column);
}

// Print long number (7 digits of space) with header at given row, column
static void printHeaderLongNumber7Spaces(const char *header, int32_t num, int row, int column) {
    vtype_t str = {'\0'};
    (void) sprintf(str, "%s: %7d", header, num);
    putString(str, row, column);
}

// Print number with header at given row, column -RAK-
static void printHeaderNumber(const char *header, int num, int row, int column) {
    vtype_t str = {'\0'};
    (void) sprintf(str, "%s: %6d", header, num);
    putString(str, row, column);
}

// Print long number at given row, column
static void printLongNumber(int32_t num, int row, int column) {
    vtype_t str = {'\0'};
    (void) sprintf(str, "%6d", num);
    putString(str, row, column);
}

// Print number at given row, column -RAK-
static void printNumber(int num, int row, int column) {
    vtype_t str = {'\0'};
    (void) sprintf(str, "%6d", num);
    putString(str, row, column);
}

// Adjustment for wisdom/intelligence -JWT-
int playerStatAdjustmentWisdomIntelligence(int stat) {
    int value = py.stats.used[stat];

    int adjustment;

    if (value > 117) {
        adjustment = 7;
    } else if (value > 107) {
        adjustment = 6;
    } else if (value > 87) {
        adjustment = 5;
    } else if (value > 67) {
        adjustment = 4;
    } else if (value > 17) {
        adjustment = 3;
    } else if (value > 14) {
        adjustment = 2;
    } else if (value > 7) {
        adjustment = 1;
    } else {
        adjustment = 0;
    }

    return adjustment;
}

// Adjustment for charisma -RAK-
// Percent decrease or increase in price of goods
int playerStatAdjustmentCharisma() {
    int charisma = py.stats.used[A_CHR];

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
    int con = py.stats.used[A_CON];

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
    const char *p = nullptr;

    if (py.misc.level < 1) {
        p = "Babe in arms";
    } else if (py.misc.level <= PLAYER_MAX_LEVEL) {
        p = class_rank_titles[py.misc.class_id][py.misc.level - 1];
    } else if (playerIsMale()) {
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
    printNumber((int) py.misc.level, 13, STAT_COLUMN + 6);
}

// Prints players current mana points. -RAK-
void printCharacterCurrentMana() {
    printNumber(py.misc.current_mana, 15, STAT_COLUMN + 6);
}

// Prints Max hit points -RAK-
void printCharacterMaxHitPoints() {
    printNumber(py.misc.max_hp, 16, STAT_COLUMN + 6);
}

// Prints players current hit points -RAK-
void printCharacterCurrentHitPoints() {
    printNumber(py.misc.current_hp, 17, STAT_COLUMN + 6);
}

// prints current AC -RAK-
void printCharacterCurrentArmorClass() {
    printNumber(py.misc.display_ac, 19, STAT_COLUMN + 6);
}

// Prints current gold -RAK-
void printCharacterGoldValue() {
    printLongNumber(py.misc.au, 20, STAT_COLUMN + 6);
}

// Prints depth in stat area -RAK-
void printCharacterCurrentDepth() {
    vtype_t depths = {'\0'};

    int depth = current_dungeon_level * 50;

    if (depth == 0) {
        (void) strcpy(depths, "Town level");
    } else {
        (void) sprintf(depths, "%d feet", depth);
    }

    putStringClearToEOL(depths, 23, 65);
}

// Prints status of hunger -RAK-
void printCharacterHungerStatus() {
    if ((PY_WEAK & py.flags.status) != 0u) {
        putString("Weak  ", 23, 0);
    } else if ((PY_HUNGRY & py.flags.status) != 0u) {
        putString("Hungry", 23, 0);
    } else {
        putString(&blank_string[BLANK_LENGTH - 6], 23, 0);
    }
}

// Prints Blind status -RAK-
void printCharacterBlindStatus() {
    if ((PY_BLIND & py.flags.status) != 0u) {
        putString("Blind", 23, 7);
    } else {
        putString(&blank_string[BLANK_LENGTH - 5], 23, 7);
    }
}

// Prints Confusion status -RAK-
void printCharacterConfusedState() {
    if ((PY_CONFUSED & py.flags.status) != 0u) {
        putString("Confused", 23, 13);
    } else {
        putString(&blank_string[BLANK_LENGTH - 8], 23, 13);
    }
}

// Prints Fear status -RAK-
void printCharacterFearState() {
    if ((PY_FEAR & py.flags.status) != 0u) {
        putString("Afraid", 23, 22);
    } else {
        putString(&blank_string[BLANK_LENGTH - 6], 23, 22);
    }
}

// Prints Poisoned status -RAK-
void printCharacterPoisonedState() {
    if ((PY_POISONED & py.flags.status) != 0u) {
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

    if ((PY_REST & py.flags.status) != 0u) {
        char restString[16];

        if (py.flags.rest < 0) {
            (void) strcpy(restString, "Rest *");
        } else if (config.display_counts) {
            (void) sprintf(restString, "Rest %-5d", py.flags.rest);
        } else {
            (void) strcpy(restString, "Rest");
        }

        putString(restString, 23, 38);

        return;
    }

    if (command_count > 0) {
        char repeatString[16];

        if (config.display_counts) {
            (void) sprintf(repeatString, "Repeat %-3d", command_count);
        } else {
            (void) strcpy(repeatString, "Repeat");
        }

        py.flags.status |= PY_REPEAT;

        putString(repeatString, 23, 38);

        if ((PY_SEARCH & py.flags.status) != 0u) {
            putString("Search", 23, 38);
        }

        return;
    }

    if ((PY_SEARCH & py.flags.status) != 0u) {
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
    if ((PY_SEARCH & py.flags.status) != 0u) {
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

    if (py.flags.new_spells_to_learn == 0) {
        putString(&blank_string[BLANK_LENGTH - 5], 23, 59);
    } else {
        putString("Study", 23, 59);
    }
}

// Prints winner status on display -RAK-
void printCharacterWinner() {
    if ((noscore & 0x2) != 0) {
        if (wizard_mode) {
            putString("Is wizard  ", 22, 0);
        } else {
            putString("Was wizard ", 22, 0);
        }
    } else if ((noscore & 0x1) != 0) {
        putString("Resurrected", 22, 0);
    } else if ((noscore & 0x4) != 0) {
        putString("Duplicate", 22, 0);
    } else if (total_winner) {
        putString("*Winner*   ", 22, 0);
    }
}

static uint8_t playerModifyStat(int stat, int16_t amount) {
    uint8_t new_stat = py.stats.current[stat];

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
    py.stats.used[stat] = playerModifyStat(stat, py.stats.modified[stat]);

    if (stat == A_STR) {
        py.flags.status |= PY_STR_WGT;
        playerRecalculateBonuses();
    } else if (stat == A_DEX) {
        playerRecalculateBonuses();
    } else if (stat == A_INT && classes[py.misc.class_id].class_to_use_mage_spells == SPELL_TYPE_MAGE) {
        playerCalculateAllowedSpellsCount(A_INT);
        playerGainMana(A_INT);
    } else if (stat == A_WIS && classes[py.misc.class_id].class_to_use_mage_spells == SPELL_TYPE_PRIEST) {
        playerCalculateAllowedSpellsCount(A_WIS);
        playerGainMana(A_WIS);
    } else if (stat == A_CON) {
        playerCalculateHitPoints();
    }
}

// Increases a stat by one randomized level -RAK-
bool playerStatRandomIncrease(int stat) {
    int new_stat = py.stats.current[stat];

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

    py.stats.current[stat] = (uint8_t) new_stat;

    if (new_stat > py.stats.max[stat]) {
        py.stats.max[stat] = (uint8_t) new_stat;
    }

    playerSetAndUseStat(stat);
    displayCharacterStats(stat);

    return true;
}

// Decreases a stat by one randomized level -RAK-
bool playerStatRandomDecrease(int stat) {
    int new_stat = py.stats.current[stat];

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

    py.stats.current[stat] = (uint8_t) new_stat;

    playerSetAndUseStat(stat);
    displayCharacterStats(stat);

    return true;
}

// Restore a stat.  Return true only if this actually makes a difference.
bool playerStatRestore(int stat) {
    int new_stat = py.stats.max[stat] - py.stats.current[stat];

    if (new_stat == 0) {
        return false;
    }

    py.stats.current[stat] += new_stat;

    playerSetAndUseStat(stat);
    displayCharacterStats(stat);

    return true;
}

// Boost a stat artificially (by wearing something). If the display
// argument is true, then increase is shown on the screen.
void playerStatBoost(int stat, int amount) {
    py.stats.modified[stat] += amount;

    playerSetAndUseStat(stat);

    // can not call displayCharacterStats() here, may be in store, may be in inventoryExecuteCommand()
    py.flags.status |= (PY_STR << stat);
}

// Returns a character's adjustment to hit. -JWT-
int playerToHitAdjustment() {
    int total;

    int dexterity = py.stats.used[A_DEX];
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

    int strength = py.stats.used[A_STR];
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
int playerArmorClassAdjustment() {
    int stat = py.stats.used[A_DEX];

    int adjustment;

    if (stat < 4) {
        adjustment = -4;
    } else if (stat == 4) {
        adjustment = -3;
    } else if (stat == 5) {
        adjustment = -2;
    } else if (stat == 6) {
        adjustment = -1;
    } else if (stat < 15) {
        adjustment = 0;
    } else if (stat < 18) {
        adjustment = 1;
    } else if (stat < 59) {
        adjustment = 2;
    } else if (stat < 94) {
        adjustment = 3;
    } else if (stat < 117) {
        adjustment = 4;
    } else {
        adjustment = 5;
    }

    return adjustment;
}

// Returns a character's adjustment to disarm -RAK-
int playerDisarmAdjustment() {
    int stat = py.stats.used[A_DEX];

    int adjustment;

    if (stat < 4) {
        adjustment = -8;
    } else if (stat == 4) {
        adjustment = -6;
    } else if (stat == 5) {
        adjustment = -4;
    } else if (stat == 6) {
        adjustment = -2;
    } else if (stat == 7) {
        adjustment = -1;
    } else if (stat < 13) {
        adjustment = 0;
    } else if (stat < 16) {
        adjustment = 1;
    } else if (stat < 18) {
        adjustment = 2;
    } else if (stat < 59) {
        adjustment = 4;
    } else if (stat < 94) {
        adjustment = 5;
    } else if (stat < 117) {
        adjustment = 6;
    } else {
        adjustment = 8;
    }

    return adjustment;
}

// Returns a character's adjustment to damage -JWT-
int playerDamageAdjustment() {
    int stat = py.stats.used[A_STR];

    int adjustment;

    if (stat < 4) {
        adjustment = -2;
    } else if (stat < 5) {
        adjustment = -1;
    } else if (stat < 16) {
        adjustment = 0;
    } else if (stat < 17) {
        adjustment = 1;
    } else if (stat < 18) {
        adjustment = 2;
    } else if (stat < 94) {
        adjustment = 3;
    } else if (stat < 109) {
        adjustment = 4;
    } else if (stat < 117) {
        adjustment = 5;
    } else {
        adjustment = 6;
    }

    return adjustment;
}

// Prints character-screen info -RAK-
void printCharacterStatsBlock() {
    printCharacterInfoInField(character_races[py.misc.race_id].name, 2, STAT_COLUMN);
    printCharacterInfoInField(classes[py.misc.class_id].title, 3, STAT_COLUMN);
    printCharacterInfoInField(playerTitle(), 4, STAT_COLUMN);

    for (int i = 0; i < 6; i++) {
        displayCharacterStats(i);
    }

    printHeaderNumber("LEV ", (int) py.misc.level, 13, STAT_COLUMN);
    printHeaderLongNumber("EXP ", py.misc.exp, 14, STAT_COLUMN);
    printHeaderNumber("MANA", py.misc.current_mana, 15, STAT_COLUMN);
    printHeaderNumber("MHP ", py.misc.max_hp, 16, STAT_COLUMN);
    printHeaderNumber("CHP ", py.misc.current_hp, 17, STAT_COLUMN);
    printHeaderNumber("AC  ", py.misc.display_ac, 19, STAT_COLUMN);
    printHeaderLongNumber("GOLD", py.misc.au, 20, STAT_COLUMN);
    printCharacterWinner();

    uint32_t status = py.flags.status;

    if (((PY_HUNGRY | PY_WEAK) & status) != 0u) {
        printCharacterHungerStatus();
    }

    if ((PY_BLIND & status) != 0u) {
        printCharacterBlindStatus();
    }

    if ((PY_CONFUSED & status) != 0u) {
        printCharacterConfusedState();
    }

    if ((PY_FEAR & status) != 0u) {
        printCharacterFearState();
    }

    if ((PY_POISONED & status) != 0u) {
        printCharacterPoisonedState();
    }

    if (((PY_SEARCH | PY_REST) & status) != 0u) {
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
void drawCavePanel() {
    clearScreen();
    printCharacterStatsBlock();
    drawDungeonPanel();
    printCharacterCurrentDepth();
}

// Prints the following information on the screen. -JWT-
void printCharacterInformation() {
    clearScreen();

    putString("Name        :", 2, 1);
    putString("Race        :", 3, 1);
    putString("Sex         :", 4, 1);
    putString("Class       :", 5, 1);

    if (!character_generated) {
        return;
    }

    putString(py.misc.name, 2, 15);
    putString(character_races[py.misc.race_id].name, 3, 15);
    putString((playerGetGenderLabel()), 4, 15);
    putString(classes[py.misc.class_id].title, 5, 15);
}

// Prints the following information on the screen. -JWT-
void printCharacterStats() {
    for (int i = 0; i < 6; i++) {
        vtype_t buf = {'\0'};

        statsAsString(py.stats.used[i], buf);
        putString(stat_names[i], 2 + i, 61);
        putString(buf, 2 + i, 66);

        if (py.stats.max[i] > py.stats.current[i]) {
            statsAsString(py.stats.max[i], buf);
            putString(buf, 2 + i, 73);
        }
    }

    printHeaderNumber("+ To Hit    ", py.misc.display_to_hit, 9, 1);
    printHeaderNumber("+ To Damage ", py.misc.display_to_damage, 10, 1);
    printHeaderNumber("+ To AC     ", py.misc.display_to_ac, 11, 1);
    printHeaderNumber("  Total AC  ", py.misc.display_ac, 12, 1);
}

// Returns a rating of x depending on y -JWT-
const char *statRating(int y, int x) {
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
void printCharacterVitalStatistics() {
    printHeaderNumber("Age          ", (int) py.misc.age, 2, 38);
    printHeaderNumber("Height       ", (int) py.misc.height, 3, 38);
    printHeaderNumber("Weight       ", (int) py.misc.weight, 4, 38);
    printHeaderNumber("Social Class ", (int) py.misc.social_class, 5, 38);
}

// Prints the following information on the screen. -JWT-
void printCharacterLevelExperience() {
    printHeaderLongNumber7Spaces("Level      ", (int32_t) py.misc.level, 9, 28);
    printHeaderLongNumber7Spaces("Experience ", py.misc.exp, 10, 28);
    printHeaderLongNumber7Spaces("Max Exp    ", py.misc.max_exp, 11, 28);

    if (py.misc.level >= PLAYER_MAX_LEVEL) {
        putStringClearToEOL("Exp to Adv.: *******", 12, 28);
    } else {
        printHeaderLongNumber7Spaces("Exp to Adv.", (int32_t) (player_base_exp_levels[py.misc.level - 1] * py.misc.experience_factor / 100), 12, 28);
    }

    printHeaderLongNumber7Spaces("Gold       ", py.misc.au, 13, 28);
    printHeaderNumber("Max Hit Points ", py.misc.max_hp, 9, 52);
    printHeaderNumber("Cur Hit Points ", py.misc.current_hp, 10, 52);
    printHeaderNumber("Max Mana       ", py.misc.mana, 11, 52);
    printHeaderNumber("Cur Mana       ", py.misc.current_mana, 12, 52);
}

// Prints ratings on certain abilities -RAK-
void printCharacterAbilities() {
    clearToBottom(14);

    int xbth = py.misc.bth + py.misc.plusses_to_hit * BTH_PER_PLUS_TO_HIT_ADJUST + (class_level_adj[py.misc.class_id][CLASS_BTH] * py.misc.level);
    int xbthb = py.misc.bth_with_bows + py.misc.plusses_to_hit * BTH_PER_PLUS_TO_HIT_ADJUST + (class_level_adj[py.misc.class_id][CLASS_BTHB] * py.misc.level);

    // this results in a range from 0 to 29
    int xfos = 40 - py.misc.fos;
    if (xfos < 0) {
        xfos = 0;
    }

    int xsrh = py.misc.chance_in_search;

    // this results in a range from 0 to 9
    int xstl = py.misc.stealth_factor + 1;
    int xdis = py.misc.disarm + 2 * playerDisarmAdjustment() + playerStatAdjustmentWisdomIntelligence(A_INT) + (class_level_adj[py.misc.class_id][CLASS_DISARM] * py.misc.level / 3);
    int xsave = py.misc.saving_throw + playerStatAdjustmentWisdomIntelligence(A_WIS) + (class_level_adj[py.misc.class_id][CLASS_SAVE] * py.misc.level / 3);
    int xdev = py.misc.saving_throw + playerStatAdjustmentWisdomIntelligence(A_INT) + (class_level_adj[py.misc.class_id][CLASS_DEVICE] * py.misc.level / 3);

    vtype_t xinfra = {'\0'};
    (void) sprintf(xinfra, "%d feet", py.flags.see_infra * 10);

    putString("(Miscellaneous Abilities)", 15, 25);
    putString("Fighting    :", 16, 1);
    putString(statRating(12, xbth), 16, 15);
    putString("Bows/Throw  :", 17, 1);
    putString(statRating(12, xbthb), 17, 15);
    putString("Saving Throw:", 18, 1);
    putString(statRating(6, xsave), 18, 15);

    putString("Stealth     :", 16, 28);
    putString(statRating(1, xstl), 16, 42);
    putString("Disarming   :", 17, 28);
    putString(statRating(8, xdis), 17, 42);
    putString("Magic Device:", 18, 28);
    putString(statRating(6, xdev), 18, 42);

    putString("Perception  :", 16, 55);
    putString(statRating(3, xfos), 16, 69);
    putString("Searching   :", 17, 55);
    putString(statRating(6, xsrh), 17, 69);
    putString("Infra-Vision:", 18, 55);
    putString(xinfra, 18, 69);
}

// Used to display the character on the screen. -RAK-
void printCharacter() {
    printCharacterInformation();
    printCharacterVitalStatistics();
    printCharacterStats();
    printCharacterLevelExperience();
    printCharacterAbilities();
}

// Gets a name for the character -JWT-
void getCharacterName() {
    putStringClearToEOL("Enter your player's name  [press <RETURN> when finished]", 21, 2);

    putString(&blank_string[BLANK_LENGTH - 23], 2, 15);

    if (!getStringInput(py.misc.name, 2, 15, 23) || py.misc.name[0] == 0) {
        getDefaultPlayerName(py.misc.name);
        putString(py.misc.name, 2, 15);
    }

    clearToBottom(20);
}

// Changes the name of the character -JWT-
void changeCharacterName() {
    vtype_t temp = {'\0'};
    bool flag = false;

    printCharacter();

    while (!flag) {
        putStringClearToEOL("<f>ile character description. <c>hange character name.", 21, 2);

        switch (getKeyInput()) {
            case 'c':
                getCharacterName();
                flag = true;
                break;
            case 'f':
                putStringClearToEOL("File name:", 0, 0);

                if (getStringInput(temp, 0, 10, 60) && (temp[0] != 0)) {
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
void inventoryDestroyItem(int item_id) {
    Inventory_t &item = inventory[item_id];

    if (item.items_count > 1 && item.sub_category_id <= ITEM_SINGLE_STACK_MAX) {
        item.items_count--;
        inventory_weight -= item.weight;
    } else {
        inventory_weight -= item.weight * item.items_count;

        for (int i = item_id; i < inventory_count - 1; i++) {
            inventory[i] = inventory[i + 1];
        }

        inventoryItemCopyTo(OBJ_NOTHING, inventory[inventory_count - 1]);
        inventory_count--;
    }

    py.flags.status |= PY_STR_WGT;
}

// Copies the object in the second argument over the first argument.
// However, the second always gets a number of one except for ammo etc.
void inventoryTakeOneItem(Inventory_t *to_item, Inventory_t *from_item) {
    *to_item = *from_item;

    if (to_item->items_count > 1 && to_item->sub_category_id >= ITEM_SINGLE_STACK_MIN && to_item->sub_category_id <= ITEM_SINGLE_STACK_MAX) {
        to_item->items_count = 1;
    }
}

// Drops an item from inventory to given location -RAK-
void inventoryDropItem(int item_id, bool drop_all) {
    if (cave[char_row][char_col].treasure_id != 0) {
        (void) dungeonDeleteObject(char_row, char_col);
    }

    int treasureID = popt();

    Inventory_t &item = inventory[item_id];
    treasure_list[treasureID] = item;

    cave[char_row][char_col].treasure_id = (uint8_t) treasureID;

    if (item_id >= EQUIPMENT_WIELD) {
        playerTakeOff(item_id, -1);
    } else {
        if (drop_all || item.items_count == 1) {
            inventory_weight -= item.weight * item.items_count;
            inventory_count--;

            while (item_id < inventory_count) {
                inventory[item_id] = inventory[item_id + 1];
                item_id++;
            }

            inventoryItemCopyTo(OBJ_NOTHING, inventory[inventory_count]);
        } else {
            treasure_list[treasureID].items_count = 1;
            inventory_weight -= item.weight;
            item.items_count--;
        }

        obj_desc_t prt1 = {'\0'};
        obj_desc_t prt2 = {'\0'};
        itemDescription(prt1, treasure_list[treasureID], true);
        (void) sprintf(prt2, "Dropped %s", prt1);
        printMessage(prt2);
    }

    py.flags.status |= PY_STR_WGT;
}

// Destroys a type of item on a given percent chance -RAK-
int inventoryDamageItem(bool (*item_type)(Inventory_t *), int chance_percentage) {
    int damage = 0;

    for (int i = 0; i < inventory_count; i++) {
        if ((*item_type)(&inventory[i]) && randomNumber(100) < chance_percentage) {
            inventoryDestroyItem(i);
            damage++;
        }
    }

    return damage;
}

// Computes current weight limit -RAK-
int playerCarryingLoadLimit() {
    int weight_cap = py.stats.used[A_STR] * PLAYER_WEIGHT_CAP + py.misc.weight;

    if (weight_cap > 3000) {
        weight_cap = 3000;
    }

    return weight_cap;
}

// this code must be identical to the inventoryCarryItem() code below
bool inventoryCanCarryItemCount(const Inventory_t &item) {
    if (inventory_count < EQUIPMENT_WIELD) {
        return true;
    }

    if (item.sub_category_id < ITEM_SINGLE_STACK_MIN) {
        return false;
    }

    for (int i = 0; i < inventory_count; i++) {
        bool same_character = inventory[i].category_id == item.category_id;
        bool same_category = inventory[i].sub_category_id == item.sub_category_id;

        // make sure the number field doesn't overflow
        bool same_number = inventory[i].items_count + item.items_count < 256;

        // they always stack (sub_category_id < 192), or else they have same `misc_use`
        bool same_group = item.sub_category_id < ITEM_GROUP_MIN || inventory[i].misc_use == item.misc_use;

        // only stack if both or neither are identified
        // TODO(cook): is it correct that they should be equal to each other, regardless of true/false value?
        bool inventory_item_is_colorless = itemSetColorlessAsIdentified(inventory[i].category_id, inventory[i].sub_category_id, inventory[i].identification);
        bool item_is_colorless = itemSetColorlessAsIdentified(item.category_id, item.sub_category_id, item.identification);
        bool identification = inventory_item_is_colorless == item_is_colorless;

        if (same_character && same_category && same_number && same_group && identification) {
            return true;
        }
    }

    return false;
}

// return false if picking up an object would change the players speed
bool inventoryCanCarryItem(const Inventory_t &item) {
    int limit = playerCarryingLoadLimit();
    int newWeight = item.items_count * item.weight + inventory_weight;

    if (limit < newWeight) {
        limit = newWeight / (limit + 1);
    } else {
        limit = 0;
    }

    return py.pack_heaviness == limit;
}

// Are we strong enough for the current pack and weapon? -CJS-
void playerStrength() {
    const Inventory_t &item = inventory[EQUIPMENT_WIELD];

    if (item.category_id != TV_NOTHING && py.stats.used[A_STR] * 15 < item.weight) {
        if (!py.weapon_is_heavy) {
            printMessage("You have trouble wielding such a heavy weapon.");
            py.weapon_is_heavy = true;
            playerRecalculateBonuses();
        }
    } else if (py.weapon_is_heavy) {
        py.weapon_is_heavy = false;
        if (item.category_id != TV_NOTHING) {
            printMessage("You are strong enough to wield your weapon.");
        }
        playerRecalculateBonuses();
    }

    int limit = playerCarryingLoadLimit();

    if (limit < inventory_weight) {
        limit = inventory_weight / (limit + 1);
    } else {
        limit = 0;
    }

    if (py.pack_heaviness != limit) {
        if (py.pack_heaviness < limit) {
            printMessage("Your pack is so heavy that it slows you down.");
        } else {
            printMessage("You move more easily under the weight of your pack.");
        }
        playerChangeSpeed(limit - py.pack_heaviness);
        py.pack_heaviness = (int16_t) limit;
    }

    py.flags.status &= ~PY_STR_WGT;
}

// Add an item to players inventory.  Return the
// item position for a description if needed. -RAK-
// this code must be identical to the inventoryCanCarryItemCount() code above
int inventoryCarryItem(Inventory_t &new_item) {
    bool is_known = itemSetColorlessAsIdentified(new_item.category_id, new_item.sub_category_id, new_item.identification);
    bool is_always_known = objectPositionOffset(new_item.category_id, new_item.sub_category_id) == -1;

    int slot_id;

    // Now, check to see if player can carry object
    for (slot_id = 0; slot_id < PLAYER_INVENTORY_SIZE; slot_id++) {
        Inventory_t &item = inventory[slot_id];

        bool is_same_category = new_item.category_id == item.category_id && new_item.sub_category_id == item.sub_category_id;
        bool not_too_many_items = int(item.items_count + new_item.items_count) < 256;

        // only stack if both or neither are identified
        bool same_known_status = itemSetColorlessAsIdentified(item.category_id, item.sub_category_id, item.identification) == is_known;

        if (is_same_category && new_item.sub_category_id >= ITEM_SINGLE_STACK_MIN && not_too_many_items &&
            (new_item.sub_category_id < ITEM_GROUP_MIN || item.misc_use == new_item.misc_use) && same_known_status) {
            item.items_count += new_item.items_count;
            break;
        }

        if ((is_same_category && is_always_known) || new_item.category_id > item.category_id) {
            // For items which are always `is_known`, i.e. never have a 'color',
            // insert them into the inventory in sorted order.
            for (int i = inventory_count - 1; i >= slot_id; i--) {
                inventory[i + 1] = inventory[i];
            }
            inventory[slot_id] = new_item;
            inventory_count++;
            break;
        }
    }

    inventory_weight += new_item.items_count * new_item.weight;
    py.flags.status |= PY_STR_WGT;

    return slot_id;
}

// Returns spell chance of failure for class_to_use_mage_spells -RAK-
static int spellChanceOfSuccess(int spell_id) {
    const Spell_t &spell = magic_spells[py.misc.class_id - 1][spell_id];

    int chance = spell.failure_chance - 3 * (py.misc.level - spell.level_required);

    int stat;
    if (classes[py.misc.class_id].class_to_use_mage_spells == SPELL_TYPE_MAGE) {
        stat = A_INT;
    } else {
        stat = A_WIS;
    }

    chance -= 3 * (playerStatAdjustmentWisdomIntelligence(stat) - 1);

    if (spell.mana_required > py.misc.current_mana) {
        chance += 5 * (spell.mana_required - py.misc.current_mana);
    }

    if (chance > 95) {
        chance = 95;
    } else if (chance < 5) {
        chance = 5;
    }

    return chance;
}

// Print list of spells -RAK-
// if non_consecutive is  -1: spells numbered consecutively from 'a' to 'a'+num
//                       >=0: spells numbered by offset from non_consecutive
void displaySpellsList(const int *spell_ids, int number_of_choices, bool comment, int non_consecutive) {
    int col;
    if (comment) {
        col = 22;
    } else {
        col = 31;
    }

    int consecutive_offset = (classes[py.misc.class_id].class_to_use_mage_spells == SPELL_TYPE_MAGE ? NAME_OFFSET_SPELLS : NAME_OFFSET_PRAYERS);

    eraseLine(1, col);
    putString("Name", 1, col + 5);
    putString("Lv Mana Fail", 1, col + 35);

    // only show the first 22 choices
    if (number_of_choices > 22) {
        number_of_choices = 22;
    }

    for (int i = 0; i < number_of_choices; i++) {
        int spell_id = spell_ids[i];
        const Spell_t &spell = magic_spells[py.misc.class_id - 1][spell_id];

        const char *p = nullptr;
        if (!comment) {
            p = "";
        } else if ((spells_forgotten & (1L << spell_id)) != 0) {
            p = " forgotten";
        } else if ((spells_learnt & (1L << spell_id)) == 0) {
            p = " unknown";
        } else if ((spells_worked & (1L << spell_id)) == 0) {
            p = " untried";
        } else {
            p = "";
        }

        // determine whether or not to leave holes in character choices, non_consecutive -1
        // when learning spells, consecutive_offset>=0 when asking which spell to cast.
        char spell_char;
        if (non_consecutive == -1) {
            spell_char = (char) ('a' + i);
        } else {
            spell_char = (char) ('a' + spell_id - non_consecutive);
        }

        vtype_t out_val = {'\0'};
        (void) sprintf(out_val, "  %c) %-30s%2d %4d %3d%%%s", spell_char, spell_names[spell_id + consecutive_offset], spell.level_required, spell.mana_required, spellChanceOfSuccess(spell_id), p);
        putStringClearToEOL(out_val, 2 + i, col);
    }
}

// Returns spell pointer -RAK-
bool spellGetId(int *spell_ids, int number_of_choices, int &spell_id, int &spell_chance, const char *prompt, int first_spell) {
    spell_id = -1;

    vtype_t str = {'\0'};
    (void) sprintf(str, "(Spells %c-%c, *=List, <ESCAPE>=exit) %s", spell_ids[0] + 'a' - first_spell, spell_ids[number_of_choices - 1] + 'a' - first_spell, prompt);

    bool spell_found = false;
    bool redraw = false;

    int offset = (classes[py.misc.class_id].class_to_use_mage_spells == SPELL_TYPE_MAGE ? NAME_OFFSET_SPELLS : NAME_OFFSET_PRAYERS);

    char choice;

    while (!spell_found && getCommand(str, choice)) {
        if (isupper((int) choice) != 0) {
            spell_id = choice - 'A' + first_spell;

            // verify that this is in spells[], at most 22 entries in class_to_use_mage_spells[]
            int test_spell_id;
            for (test_spell_id = 0; test_spell_id < number_of_choices; test_spell_id++) {
                if (spell_id == spell_ids[test_spell_id]) {
                    break;
                }
            }

            if (test_spell_id == number_of_choices) {
                spell_id = -2;
            } else {
                const Spell_t &spell = magic_spells[py.misc.class_id - 1][spell_id];

                vtype_t tmp_str = {'\0'};
                (void) sprintf(tmp_str, "Cast %s (%d mana, %d%% fail)?", spell_names[spell_id + offset], spell.mana_required, spellChanceOfSuccess(spell_id));
                if (getInputConfirmation(tmp_str)) {
                    spell_found = true;
                } else {
                    spell_id = -1;
                }
            }
        } else if (islower((int) choice) != 0) {
            spell_id = choice - 'a' + first_spell;

            // verify that this is in spells[], at most 22 entries in class_to_use_mage_spells[]
            int test_spell_id;
            for (test_spell_id = 0; test_spell_id < number_of_choices; test_spell_id++) {
                if (spell_id == spell_ids[test_spell_id]) {
                    break;
                }
            }

            if (test_spell_id == number_of_choices) {
                spell_id = -2;
            } else {
                spell_found = true;
            }
        } else if (choice == '*') {
            // only do this drawing once
            if (!redraw) {
                terminalSaveScreen();
                redraw = true;
                displaySpellsList(spell_ids, number_of_choices, false, first_spell);
            }
        } else if (isalpha((int) choice) != 0) {
            spell_id = -2;
        } else {
            spell_id = -1;
            terminalBellSound();
        }

        if (spell_id == -2) {
            vtype_t tmp_str = {'\0'};
            (void) sprintf(tmp_str, "You don't know that %s.", (offset == NAME_OFFSET_SPELLS ? "spell" : "prayer"));
            printMessage(tmp_str);
        }
    }

    if (redraw) {
        terminalRestoreScreen();
    }

    eraseLine(MSG_LINE, 0);

    if (spell_found) {
        spell_chance = spellChanceOfSuccess(spell_id);
    }

    return spell_found;
}

// check to see if know any spells greater than level, eliminate them
static void eliminateKnownSpellsGreaterThanLevel(Spell_t *msp_ptr, const char *p, int offset) {
    uint32_t mask = 0x80000000L;

    for (int i = 31; mask != 0u; mask >>= 1, i--) {
        if ((mask & spells_learnt) != 0u) {
            if (msp_ptr[i].level_required > py.misc.level) {
                spells_learnt &= ~mask;
                spells_forgotten |= mask;

                vtype_t msg = {'\0'};
                (void) sprintf(msg, "You have forgotten the %s of %s.", p, spell_names[i + offset]);
                printMessage(msg);
            } else {
                break;
            }
        }
    }
}

static int numberOfSpellsAllowed(int stat) {
    int levels = py.misc.level - classes[py.misc.class_id].min_level_for_spell_casting + 1;

    int allowed;

    switch (playerStatAdjustmentWisdomIntelligence(stat)) {
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
        default:
            allowed = 0;
            break;
    }

    return allowed;
}

static int numberOfSpellsKnown() {
    int known = 0;

    for (uint32_t mask = 0x1; mask != 0u; mask <<= 1) {
        if ((mask & spells_learnt) != 0u) {
            known++;
        }
    }

    return known;
}

// remember forgotten spells while forgotten spells exist of new_spells_to_learn positive,
// remember the spells in the order that they were learned
static int rememberForgottenSpells(Spell_t *msp_ptr, int allowedSpells, int newSpells, const char *p, int offset) {
    uint32_t mask;

    for (int n = 0; ((spells_forgotten != 0u) && (newSpells != 0) && (n < allowedSpells) && (n < 32)); n++) {
        // orderID is (i+1)th spell learned
        int orderID = spells_learned_order[n];

        // shifting by amounts greater than number of bits in long gives
        // an undefined result, so don't shift for unknown spells
        if (orderID == 99) {
            mask = 0x0;
        } else {
            mask = (uint32_t) (1L << orderID);
        }

        if ((mask & spells_forgotten) != 0u) {
            if (msp_ptr[orderID].level_required <= py.misc.level) {
                newSpells--;
                spells_forgotten &= ~mask;
                spells_learnt |= mask;

                vtype_t msg = {'\0'};
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
    auto spell_flag = (uint32_t) (0x7FFFFFFFL & ~spells_learnt);

    int id = 0;
    uint32_t mask = 0x1;

    for (int i = 0; spell_flag != 0u; mask <<= 1, i++) {
        if ((spell_flag & mask) != 0u) {
            spell_flag &= ~mask;
            if (msp_ptr[i].level_required <= py.misc.level) {
                id++;
            }
        }
    }

    if (newSpells > id) {
        newSpells = id;
    }

    return newSpells;
}

// forget spells until new_spells_to_learn zero or no more spells know,
// spells are forgotten in the opposite order that they were learned
// NOTE: newSpells is always a negative value
static void forgetSpells(int newSpells, const char *p, int offset) {
    uint32_t mask;

    for (int i = 31; (newSpells != 0) && (spells_learnt != 0u); i--) {
        // orderID is the (i+1)th spell learned
        int orderID = spells_learned_order[i];

        // shifting by amounts greater than number of bits in long gives
        // an undefined result, so don't shift for unknown spells
        if (orderID == 99) {
            mask = 0x0;
        } else {
            mask = (uint32_t) (1L << orderID);
        }

        if ((mask & spells_learnt) != 0u) {
            spells_learnt &= ~mask;
            spells_forgotten |= mask;
            newSpells++;

            vtype_t msg = {'\0'};
            (void) sprintf(msg, "You have forgotten the %s of %s.", p, spell_names[orderID + offset]);
            printMessage(msg);
        }
    }
}

// calculate number of spells player should have, and
// learn forget spells until that number is met -JEW-
void playerCalculateAllowedSpellsCount(int stat) {
    Spell_t &spell = magic_spells[py.misc.class_id - 1][0];

    const char *magic_type_str = nullptr;
    int offset;

    if (stat == A_INT) {
        magic_type_str = "spell";
        offset = NAME_OFFSET_SPELLS;
    } else {
        magic_type_str = "prayer";
        offset = NAME_OFFSET_PRAYERS;
    }

    // check to see if know any spells greater than level, eliminate them
    eliminateKnownSpellsGreaterThanLevel(&spell, magic_type_str, offset);

    // calc number of spells allowed
    int num_allowed = numberOfSpellsAllowed(stat);
    int num_known = numberOfSpellsKnown();
    int new_spells = num_allowed - num_known;

    if (new_spells > 0) {
        new_spells = rememberForgottenSpells(&spell, num_allowed, new_spells, magic_type_str, offset);

        // If `new_spells_to_learn` is still greater than zero
        if (new_spells > 0) {
            new_spells = learnableSpells(&spell, new_spells);
        }
    } else if (new_spells < 0) {
        forgetSpells(new_spells, magic_type_str, offset);
        new_spells = 0;
    }

    if (new_spells != py.flags.new_spells_to_learn) {
        if (new_spells > 0 && py.flags.new_spells_to_learn == 0) {
            vtype_t msg = {'\0'};
            (void) sprintf(msg, "You can learn some new %ss now.", magic_type_str);
            printMessage(msg);
        }

        py.flags.new_spells_to_learn = (uint8_t) new_spells;
        py.flags.status |= PY_STUDY;
    }
}

static bool playerCanRead() {
    if (py.flags.blind > 0) {
        printMessage("You can't see to read your spell book!");
        return false;
    }

    if (playerNoLight()) {
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
        if (inventory[i].category_id == TV_MAGIC_BOOK) {
            spell_flag |= inventory[i].flags;
        }
    }

    return spell_flag;
}

// gain spells when player wants to -JW-
void playerGainSpells() {
    // Priests don't need light because they get spells from their god, so only
    // fail when can't see if player has SPELL_TYPE_MAGE spells. This check is done below.
    if (py.flags.confused > 0) {
        printMessage("You are too confused.");
        return;
    }

    int new_spells = py.flags.new_spells_to_learn;
    int diff_spells = 0;

    // TODO(cook) move access to `magic_spells[]` directly to the for loop it's used in, below?
    Spell_t *spells = &magic_spells[py.misc.class_id - 1][0];

    int stat, offset;

    if (classes[py.misc.class_id].class_to_use_mage_spells == SPELL_TYPE_MAGE) {
        // People with SPELL_TYPE_MAGE spells can't learn spell_bank if they can't read their books.
        if (!playerCanRead()) {
            return;
        }
        stat = A_INT;
        offset = NAME_OFFSET_SPELLS;
    } else {
        stat = A_WIS;
        offset = NAME_OFFSET_PRAYERS;
    }

    int last_known = lastKnownSpell();

    if (new_spells == 0) {
        vtype_t tmp_str = {'\0'};
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

    int spell_id = 0;
    int spell_bank[31];
    uint32_t mask = 0x1;

    for (int i = 0; spell_flag != 0u; mask <<= 1, i++) {
        if ((spell_flag & mask) != 0u) {
            spell_flag &= ~mask;
            if (spells[i].level_required <= py.misc.level) {
                spell_bank[spell_id] = i;
                spell_id++;
            }
        }
    }

    if (new_spells > spell_id) {
        printMessage("You seem to be missing a book.");

        diff_spells = new_spells - spell_id;
        new_spells = spell_id;
    }

    if (new_spells == 0) {
        // do nothing
    } else if (stat == A_INT) {
        // get to choose which mage spells will be learned
        terminalSaveScreen();
        displaySpellsList(spell_bank, spell_id, false, -1);

        char query;
        while ((new_spells != 0) && getCommand("Learn which spell?", query)) {
            int c = query - 'a';

            // test j < 23 in case i is greater than 22, only 22 spells
            // are actually shown on the screen, so limit choice to those
            if (c >= 0 && c < spell_id && c < 22) {
                new_spells--;

                spells_learnt |= 1L << spell_bank[c];
                spells_learned_order[last_known++] = (uint8_t) spell_bank[c];

                for (; c <= spell_id - 1; c++) {
                    spell_bank[c] = spell_bank[c + 1];
                }

                spell_id--;

                eraseLine(c + 1, 31);
                displaySpellsList(spell_bank, spell_id, false, -1);
            } else {
                terminalBellSound();
            }
        }

        terminalRestoreScreen();
    } else {
        // pick a prayer at random
        while (new_spells != 0) {
            int id = randomNumber(spell_id) - 1;
            spells_learnt |= 1L << spell_bank[id];
            spells_learned_order[last_known++] = (uint8_t) spell_bank[id];

            vtype_t tmp_str = {'\0'};
            (void) sprintf(tmp_str, "You have learned the prayer of %s.", spell_names[spell_bank[id] + offset]);
            printMessage(tmp_str);

            for (; id <= spell_id - 1; id++) {
                spell_bank[id] = spell_bank[id + 1];
            }

            spell_id--;
            new_spells--;
        }
    }

    py.flags.new_spells_to_learn = (uint8_t) (new_spells + diff_spells);

    if (py.flags.new_spells_to_learn == 0) {
        py.flags.status |= PY_STUDY;
    }

    // set the mana for first level characters when they learn their first spell.
    if (py.misc.mana == 0) {
        playerGainMana(stat);
    }
}

static int newMana(int stat) {
    int levels = py.misc.level - classes[py.misc.class_id].min_level_for_spell_casting + 1;

    switch (playerStatAdjustmentWisdomIntelligence(stat)) {
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
void playerGainMana(int stat) {
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
                int32_t value = (((int32_t) py.misc.current_mana << 16) + py.misc.current_mana_fraction) / py.misc.mana * new_mana;
                py.misc.current_mana = (int16_t) (value >> 16);
                py.misc.current_mana_fraction = (uint16_t) (value & 0xFFFF);
            } else {
                py.misc.current_mana = (int16_t) new_mana;
                py.misc.current_mana_fraction = 0;
            }

            py.misc.mana = (int16_t) new_mana;

            // can't print mana here, may be in store or inventory mode
            py.flags.status |= PY_MANA;
        }
    } else if (py.misc.mana != 0) {
        py.misc.mana = 0;
        py.misc.current_mana = 0;

        // can't print mana here, may be in store or inventory mode
        py.flags.status |= PY_MANA;
    }
}

// Increases hit points and level -RAK-
static void playerGainLevel() {
    py.misc.level++;

    vtype_t msg = {'\0'};
    (void) sprintf(msg, "Welcome to level %d.", (int) py.misc.level);
    printMessage(msg);

    playerCalculateHitPoints();

    int32_t new_exp = player_base_exp_levels[py.misc.level - 1] * py.misc.experience_factor / 100;

    if (py.misc.exp > new_exp) {
        // lose some of the 'extra' exp when gaining several levels at once
        int32_t dif_exp = py.misc.exp - new_exp;
        py.misc.exp = new_exp + (dif_exp / 2);
    }

    printCharacterLevel();
    printCharacterTitle();

    const Class_t &player_class = classes[py.misc.class_id];

    if (player_class.class_to_use_mage_spells == SPELL_TYPE_MAGE) {
        playerCalculateAllowedSpellsCount(A_INT);
        playerGainMana(A_INT);
    } else if (player_class.class_to_use_mage_spells == SPELL_TYPE_PRIEST) {
        playerCalculateAllowedSpellsCount(A_WIS);
        playerGainMana(A_WIS);
    }
}

// Prints experience -RAK-
void displayCharacterExperience() {
    if (py.misc.exp > PLAYER_MAX_EXP) {
        py.misc.exp = PLAYER_MAX_EXP;
    }

    while ((py.misc.level < PLAYER_MAX_LEVEL) && (signed) (player_base_exp_levels[py.misc.level - 1] * py.misc.experience_factor / 100) <= py.misc.exp) {
        playerGainLevel();
    }

    if (py.misc.exp > py.misc.max_exp) {
        py.misc.max_exp = py.misc.exp;
    }

    printLongNumber(py.misc.exp, 14, STAT_COLUMN + 6);
}

// Calculate the players hit points
void playerCalculateHitPoints() {
    int hp = player_base_hp_levels[py.misc.level - 1] + (playerStatAdjustmentConstitution() * py.misc.level);

    // Always give at least one point per level + 1
    if (hp < (py.misc.level + 1)) {
        hp = py.misc.level + 1;
    }

    if ((py.flags.status & PY_HERO) != 0u) {
        hp += 10;
    }

    if ((py.flags.status & PY_SHERO) != 0u) {
        hp += 20;
    }

    // MHP can equal zero while character is being created
    if (hp != py.misc.max_hp && py.misc.max_hp != 0) {
        // Change current hit points proportionately to change of MHP,
        // divide first to avoid overflow, little loss of accuracy
        int32_t value = (((int32_t) py.misc.current_hp << 16) + py.misc.current_hp_fraction) / py.misc.max_hp * hp;
        py.misc.current_hp = (int16_t) (value >> 16);
        py.misc.current_hp_fraction = (uint16_t) (value & 0xFFFF);
        py.misc.max_hp = (int16_t) hp;

        // can't print hit points here, may be in store or inventory mode
        py.flags.status |= PY_HP;
    }
}

// lets anyone enter wizard mode after a disclaimer... -JEW-
bool enterWizardMode() {
    bool answer = false;

    if (noscore == 0) {
        printMessage("Wizard mode is for debugging and experimenting.");
        answer = getInputConfirmation("The game will not be scored if you enter wizard mode. Are you sure?");
    }

    if ((noscore != 0) || answer) {
        noscore |= 0x2;
        wizard_mode = true;
        return true;
    }

    return false;
}

static int playerAttackBlowsDexterity(int dexterity) {
    int dex;

    if (dexterity < 10) {
        dex = 0;
    } else if (dexterity < 19) {
        dex = 1;
    } else if (dexterity < 68) {
        dex = 2;
    } else if (dexterity < 108) {
        dex = 3;
    } else if (dexterity < 118) {
        dex = 4;
    } else {
        dex = 5;
    }

    return dex;
}

static int playerAttackBlowsStrength(int strength, int weight) {
    int adj_weight = (strength * 10 / weight);

    int str;

    if (adj_weight < 2) {
        str = 0;
    } else if (adj_weight < 3) {
        str = 1;
    } else if (adj_weight < 4) {
        str = 2;
    } else if (adj_weight < 5) {
        str = 3;
    } else if (adj_weight < 7) {
        str = 4;
    } else if (adj_weight < 9) {
        str = 5;
    } else {
        str = 6;
    }

    return str;
}

// Weapon weight VS strength and dexterity -RAK-
int playerAttackBlows(int weight, int &weight_to_hit) {
    weight_to_hit = 0;

    int player_strength = py.stats.used[A_STR];

    if (player_strength * 15 < weight) {
        weight_to_hit = player_strength * 15 - weight;
        return 1;
    }

    int dexterity = playerAttackBlowsDexterity(py.stats.used[A_DEX]);
    int strength = playerAttackBlowsStrength(player_strength, weight);

    return (int) blows_table[strength][dexterity];
}

// Special damage due to magical abilities of object -RAK-
int itemMagicAbilityDamage(const Inventory_t &item, int total_damage, int monster_id) {
    bool is_ego_weapon = (item.flags & TR_EGO_WEAPON) != 0;
    bool is_projectile = item.category_id >= TV_SLING_AMMO && item.category_id <= TV_ARROW;
    bool is_hafted_sword = item.category_id >= TV_HAFTED && item.category_id <= TV_SWORD;
    bool is_flask = item.category_id == TV_FLASK;

    if (is_ego_weapon && (is_projectile || is_hafted_sword || is_flask)) {
        const Creature_t &creature = creatures_list[monster_id];
        Recall_t &memory = creature_recall[monster_id];

        // Slay Dragon
        if (((creature.defenses & CD_DRAGON) != 0) && ((item.flags & TR_SLAY_DRAGON) != 0u)) {
            memory.defenses |= CD_DRAGON;
            return total_damage * 4;
        }

        // Slay Undead
        if (((creature.defenses & CD_UNDEAD) != 0) && ((item.flags & TR_SLAY_UNDEAD) != 0u)) {
            memory.defenses |= CD_UNDEAD;
            return total_damage * 3;
        }

        // Slay Animal
        if (((creature.defenses & CD_ANIMAL) != 0) && ((item.flags & TR_SLAY_ANIMAL) != 0u)) {
            memory.defenses |= CD_ANIMAL;
            return total_damage * 2;
        }

        // Slay Evil
        if (((creature.defenses & CD_EVIL) != 0) && ((item.flags & TR_SLAY_EVIL) != 0u)) {
            memory.defenses |= CD_EVIL;
            return total_damage * 2;
        }

        // Frost
        if (((creature.defenses & CD_FROST) != 0) && ((item.flags & TR_FROST_BRAND) != 0u)) {
            memory.defenses |= CD_FROST;
            return total_damage * 3 / 2;
        }

        // Fire
        if (((creature.defenses & CD_FIRE) != 0) && ((item.flags & TR_FLAME_TONGUE) != 0u)) {
            memory.defenses |= CD_FIRE;
            return total_damage * 3 / 2;
        }
    }

    return total_damage;
}

// Critical hits, Nasty way to die. -RAK-
int playerWeaponCriticalBlow(int weapon_weight, int plus_to_hit, int damage, int attack_type_id) {
    int critical = damage;

    // Weight of weapon, plusses to hit, and character level all
    // contribute to the chance of a critical
    if (randomNumber(5000) <= weapon_weight + 5 * plus_to_hit + (class_level_adj[py.misc.class_id][attack_type_id] * py.misc.level)) {
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
bool playerMovePosition(int dir, int &new_y, int &new_x) {
    int new_row;
    int new_col;

    switch (dir) {
        case 1:
            new_row = new_y + 1;
            new_col = new_x - 1;
            break;
        case 2:
            new_row = new_y + 1;
            new_col = new_x;
            break;
        case 3:
            new_row = new_y + 1;
            new_col = new_x + 1;
            break;
        case 4:
            new_row = new_y;
            new_col = new_x - 1;
            break;
        case 5:
            new_row = new_y;
            new_col = new_x;
            break;
        case 6:
            new_row = new_y;
            new_col = new_x + 1;
            break;
        case 7:
            new_row = new_y - 1;
            new_col = new_x - 1;
            break;
        case 8:
            new_row = new_y - 1;
            new_col = new_x;
            break;
        case 9:
            new_row = new_y - 1;
            new_col = new_x + 1;
            break;
        default:
            new_row = 0;
            new_col = 0;
            break;
    }

    bool can_move = false;

    if (new_row >= 0 && new_row < dungeon_height && new_col >= 0 && new_col < dungeon_width) {
        new_y = new_row;
        new_x = new_col;
        can_move = true;
    }

    return can_move;
}

// Saving throws for player character. -RAK-
bool playerSavingThrow() {
    int class_level_adjustment = class_level_adj[py.misc.class_id][CLASS_SAVE] * py.misc.level / 3;

    int saving = py.misc.saving_throw + playerStatAdjustmentWisdomIntelligence(A_WIS) + class_level_adjustment;

    return randomNumber(100) <= saving;
}

// Finds range of item in inventory list -RAK-
bool inventoryFindRange(int item_id_start, int item_id_end, int &j, int &k) {
    j = -1;
    k = -1;

    bool at_end_of_range = false;

    for (int i = 0; i < inventory_count; i++) {
        auto item_id = (int) inventory[i].category_id;

        if (!at_end_of_range) {
            if (item_id == item_id_start || item_id == item_id_end) {
                at_end_of_range = true;
                j = i;
            }
        } else {
            if (item_id != item_id_start && item_id != item_id_end) {
                k = i - 1;
                break;
            }
        }
    }

    if (at_end_of_range && k == -1) {
        k = inventory_count - 1;
    }

    return at_end_of_range;
}

// Teleport the player to a new location -RAK-
void playerTeleport(int new_distance) {
    int new_y, new_x;

    do {
        new_y = randomNumber(dungeon_height) - 1;
        new_x = randomNumber(dungeon_width) - 1;

        while (coordDistanceBetween(new_y, new_x, char_row, char_col) > new_distance) {
            new_y += (char_row - new_y) / 2;
            new_x += (char_col - new_x) / 2;
        }
    } while (cave[new_y][new_x].feature_id >= MIN_CLOSED_SPACE || cave[new_y][new_x].creature_id >= 2);

    dungeonMoveCreatureRecord(char_row, char_col, new_y, new_x);

    for (int y = char_row - 1; y <= char_row + 1; y++) {
        for (int x = char_col - 1; x <= char_col + 1; x++) {
            cave[y][x].temporary_light = false;
            dungeonLiteSpot(y, x);
        }
    }

    dungeonLiteSpot(char_row, char_col);

    char_row = (int16_t) new_y;
    char_col = (int16_t) new_x;

    dungeonResetView();
    updateMonsters(false);

    teleport_player = false;
}
