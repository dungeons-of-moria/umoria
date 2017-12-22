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
    putString(stat_names[stat], Coord_t{6 + stat, STAT_COLUMN});
    putString(text, Coord_t{6 + stat, STAT_COLUMN + 6});
}

// Print character info in given row, column -RAK-
// The longest title is 13 characters, so only pad to 13
static void printCharacterInfoInField(const char *info, int row, int column) {
    // blank out the current field space
    putString(&blank_string[BLANK_LENGTH - 13], Coord_t{row, column});

    putString(info, Coord_t{row, column});
}

// Print long number with header at given row, column
static void printHeaderLongNumber(const char *header, int32_t num, int row, int column) {
    vtype_t str = {'\0'};
    (void) sprintf(str, "%s: %6d", header, num);
    putString(str, Coord_t{row, column});
}

// Print long number (7 digits of space) with header at given row, column
static void printHeaderLongNumber7Spaces(const char *header, int32_t num, int row, int column) {
    vtype_t str = {'\0'};
    (void) sprintf(str, "%s: %7d", header, num);
    putString(str, Coord_t{row, column});
}

// Print number with header at given row, column -RAK-
static void printHeaderNumber(const char *header, int num, int row, int column) {
    vtype_t str = {'\0'};
    (void) sprintf(str, "%s: %6d", header, num);
    putString(str, Coord_t{row, column});
}

// Print long number at given row, column
static void printLongNumber(int32_t num, int row, int column) {
    vtype_t str = {'\0'};
    (void) sprintf(str, "%6d", num);
    putString(str, Coord_t{row, column});
}

// Print number at given row, column -RAK-
static void printNumber(int num, int row, int column) {
    vtype_t str = {'\0'};
    (void) sprintf(str, "%6d", num);
    putString(str, Coord_t{row, column});
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

    int depth = dg.current_level * 50;

    if (depth == 0) {
        (void) strcpy(depths, "Town level");
    } else {
        (void) sprintf(depths, "%d feet", depth);
    }

    putStringClearToEOL(depths, Coord_t{23, 65});
}

// Prints status of hunger -RAK-
void printCharacterHungerStatus() {
    if ((PY_WEAK & py.flags.status) != 0u) {
        putString("Weak  ", Coord_t{23, 0});
    } else if ((PY_HUNGRY & py.flags.status) != 0u) {
        putString("Hungry", Coord_t{23, 0});
    } else {
        putString(&blank_string[BLANK_LENGTH - 6], Coord_t{23, 0});
    }
}

// Prints Blind status -RAK-
void printCharacterBlindStatus() {
    if ((PY_BLIND & py.flags.status) != 0u) {
        putString("Blind", Coord_t{23, 7});
    } else {
        putString(&blank_string[BLANK_LENGTH - 5], Coord_t{23, 7});
    }
}

// Prints Confusion status -RAK-
void printCharacterConfusedState() {
    if ((PY_CONFUSED & py.flags.status) != 0u) {
        putString("Confused", Coord_t{23, 13});
    } else {
        putString(&blank_string[BLANK_LENGTH - 8], Coord_t{23, 13});
    }
}

// Prints Fear status -RAK-
void printCharacterFearState() {
    if ((PY_FEAR & py.flags.status) != 0u) {
        putString("Afraid", Coord_t{23, 22});
    } else {
        putString(&blank_string[BLANK_LENGTH - 6], Coord_t{23, 22});
    }
}

// Prints Poisoned status -RAK-
void printCharacterPoisonedState() {
    if ((PY_POISONED & py.flags.status) != 0u) {
        putString("Poisoned", Coord_t{23, 29});
    } else {
        putString(&blank_string[BLANK_LENGTH - 8], Coord_t{23, 29});
    }
}

// Prints Searching, Resting, Paralysis, or 'count' status -RAK-
void printCharacterMovementState() {
    py.flags.status &= ~PY_REPEAT;

    if (py.flags.paralysis > 1) {
        putString("Paralysed", Coord_t{23, 38});
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

        putString(restString, Coord_t{23, 38});

        return;
    }

    if (game.command_count > 0) {
        char repeatString[16];

        if (config.display_counts) {
            (void) sprintf(repeatString, "Repeat %-3d", game.command_count);
        } else {
            (void) strcpy(repeatString, "Repeat");
        }

        py.flags.status |= PY_REPEAT;

        putString(repeatString, Coord_t{23, 38});

        if ((PY_SEARCH & py.flags.status) != 0u) {
            putString("Search", Coord_t{23, 38});
        }

        return;
    }

    if ((PY_SEARCH & py.flags.status) != 0u) {
        putString("Searching", Coord_t{23, 38});
        return;
    }

    // "repeat 999" is 10 characters
    putString(&blank_string[BLANK_LENGTH - 10], Coord_t{23, 38});
}

// Prints the speed of a character. -CJS-
void printCharacterSpeed() {
    int speed = py.flags.speed;

    // Search mode.
    if ((PY_SEARCH & py.flags.status) != 0u) {
        speed--;
    }

    if (speed > 1) {
        putString("Very Slow", Coord_t{23, 49});
    } else if (speed == 1) {
        putString("Slow     ", Coord_t{23, 49});
    } else if (speed == 0) {
        putString(&blank_string[BLANK_LENGTH - 9], Coord_t{23, 49});
    } else if (speed == -1) {
        putString("Fast     ", Coord_t{23, 49});
    } else {
        putString("Very Fast", Coord_t{23, 49});
    }
}

void printCharacterStudyInstruction() {
    py.flags.status &= ~PY_STUDY;

    if (py.flags.new_spells_to_learn == 0) {
        putString(&blank_string[BLANK_LENGTH - 5], Coord_t{23, 59});
    } else {
        putString("Study", Coord_t{23, 59});
    }
}

// Prints winner status on display -RAK-
void printCharacterWinner() {
    if ((game.noscore & 0x2) != 0) {
        if (game.wizard_mode) {
            putString("Is wizard  ", Coord_t{22, 0});
        } else {
            putString("Was wizard ", Coord_t{22, 0});
        }
    } else if ((game.noscore & 0x1) != 0) {
        putString("Resurrected", Coord_t{22, 0});
    } else if ((game.noscore & 0x4) != 0) {
        putString("Duplicate", Coord_t{22, 0});
    } else if (game.total_winner) {
        putString("*Winner*   ", Coord_t{22, 0});
    }
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

// Prints the following information on the screen. -JWT-
void printCharacterInformation() {
    clearScreen();

    putString("Name        :", Coord_t{2, 1});
    putString("Race        :", Coord_t{3, 1});
    putString("Sex         :", Coord_t{4, 1});
    putString("Class       :", Coord_t{5, 1});

    if (!game.character_generated) {
        return;
    }

    putString(py.misc.name, Coord_t{2, 15});
    putString(character_races[py.misc.race_id].name, Coord_t{3, 15});
    putString((playerGetGenderLabel()), Coord_t{4, 15});
    putString(classes[py.misc.class_id].title, Coord_t{5, 15});
}

// Prints the following information on the screen. -JWT-
void printCharacterStats() {
    for (int i = 0; i < 6; i++) {
        vtype_t buf = {'\0'};

        statsAsString(py.stats.used[i], buf);
        putString(stat_names[i], Coord_t{2 + i, 61});
        putString(buf, Coord_t{2 + i, 66});

        if (py.stats.max[i] > py.stats.current[i]) {
            statsAsString(py.stats.max[i], buf);
            putString(buf, Coord_t{2 + i, 73});
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
        putStringClearToEOL("Exp to Adv.: *******", Coord_t{12, 28});
    } else {
        printHeaderLongNumber7Spaces("Exp to Adv.", (int32_t) (py.base_exp_levels[py.misc.level - 1] * py.misc.experience_factor / 100), 12, 28);
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
    int xdis =
            py.misc.disarm + 2 * playerDisarmAdjustment() + playerStatAdjustmentWisdomIntelligence(A_INT) + (class_level_adj[py.misc.class_id][CLASS_DISARM] * py.misc.level / 3);
    int xsave = py.misc.saving_throw + playerStatAdjustmentWisdomIntelligence(A_WIS) + (class_level_adj[py.misc.class_id][CLASS_SAVE] * py.misc.level / 3);
    int xdev = py.misc.saving_throw + playerStatAdjustmentWisdomIntelligence(A_INT) + (class_level_adj[py.misc.class_id][CLASS_DEVICE] * py.misc.level / 3);

    vtype_t xinfra = {'\0'};
    (void) sprintf(xinfra, "%d feet", py.flags.see_infra * 10);

    putString("(Miscellaneous Abilities)", Coord_t{15, 25});
    putString("Fighting    :", Coord_t{16, 1});
    putString(statRating(12, xbth), Coord_t{16, 15});
    putString("Bows/Throw  :", Coord_t{17, 1});
    putString(statRating(12, xbthb), Coord_t{17, 15});
    putString("Saving Throw:", Coord_t{18, 1});
    putString(statRating(6, xsave), Coord_t{18, 15});

    putString("Stealth     :", Coord_t{16, 28});
    putString(statRating(1, xstl), Coord_t{16, 42});
    putString("Disarming   :", Coord_t{17, 28});
    putString(statRating(8, xdis), Coord_t{17, 42});
    putString("Magic Device:", Coord_t{18, 28});
    putString(statRating(6, xdev), Coord_t{18, 42});

    putString("Perception  :", Coord_t{16, 55});
    putString(statRating(3, xfos), Coord_t{16, 69});
    putString("Searching   :", Coord_t{17, 55});
    putString(statRating(6, xsrh), Coord_t{17, 69});
    putString("Infra-Vision:", Coord_t{18, 55});
    putString(xinfra, Coord_t{18, 69});
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
    putStringClearToEOL("Enter your player's name  [press <RETURN> when finished]", Coord_t{21, 2});

    putString(&blank_string[BLANK_LENGTH - 23], Coord_t{2, 15});

    if (!getStringInput(py.misc.name, Coord_t{2, 15}, 23) || py.misc.name[0] == 0) {
        getDefaultPlayerName(py.misc.name);
        putString(py.misc.name, Coord_t{2, 15});
    }

    clearToBottom(20);
}

// Changes the name of the character -JWT-
void changeCharacterName() {
    vtype_t temp = {'\0'};
    bool flag = false;

    printCharacter();

    while (!flag) {
        putStringClearToEOL("<f>ile character description. <c>hange character name.", Coord_t{21, 2});

        switch (getKeyInput()) {
            case 'c':
                getCharacterName();
                flag = true;
                break;
            case 'f':
                putStringClearToEOL("File name:", Coord_t{0, 0});

                if (getStringInput(temp, Coord_t{0, 10}, 60) && (temp[0] != 0)) {
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

// Computes current weight limit -RAK-
int playerCarryingLoadLimit() {
    int weight_cap = py.stats.used[A_STR] * PLAYER_WEIGHT_CAP + py.misc.weight;

    if (weight_cap > 3000) {
        weight_cap = 3000;
    }

    return weight_cap;
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

    if (limit < py.inventory_weight) {
        limit = py.inventory_weight / (limit + 1);
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

    eraseLine(Coord_t{1, col});
    putString("Name", Coord_t{1, col + 5});
    putString("Lv Mana Fail", Coord_t{1, col + 35});

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
        } else if ((py.flags.spells_forgotten & (1L << spell_id)) != 0) {
            p = " forgotten";
        } else if ((py.flags.spells_learnt & (1L << spell_id)) == 0) {
            p = " unknown";
        } else if ((py.flags.spells_worked & (1L << spell_id)) == 0) {
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
        (void) sprintf(out_val, "  %c) %-30s%2d %4d %3d%%%s", spell_char, spell_names[spell_id + consecutive_offset], spell.level_required, spell.mana_required,
                       spellChanceOfSuccess(spell_id), p);
        putStringClearToEOL(out_val, Coord_t{2 + i, col});
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

    messageLineClear();

    if (spell_found) {
        spell_chance = spellChanceOfSuccess(spell_id);
    }

    return spell_found;
}

// check to see if know any spells greater than level, eliminate them
static void eliminateKnownSpellsGreaterThanLevel(Spell_t *msp_ptr, const char *p, int offset) {
    uint32_t mask = 0x80000000L;

    for (int i = 31; mask != 0u; mask >>= 1, i--) {
        if ((mask & py.flags.spells_learnt) != 0u) {
            if (msp_ptr[i].level_required > py.misc.level) {
                py.flags.spells_learnt &= ~mask;
                py.flags.spells_forgotten |= mask;

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
        if ((mask & py.flags.spells_learnt) != 0u) {
            known++;
        }
    }

    return known;
}

// remember forgotten spells while forgotten spells exist of new_spells_to_learn positive,
// remember the spells in the order that they were learned
static int rememberForgottenSpells(Spell_t *msp_ptr, int allowedSpells, int newSpells, const char *p, int offset) {
    uint32_t mask;

    for (int n = 0; ((py.flags.spells_forgotten != 0u) && (newSpells != 0) && (n < allowedSpells) && (n < 32)); n++) {
        // orderID is (i+1)th spell learned
        int orderID = py.flags.spells_learned_order[n];

        // shifting by amounts greater than number of bits in long gives
        // an undefined result, so don't shift for unknown spells
        if (orderID == 99) {
            mask = 0x0;
        } else {
            mask = (uint32_t) (1L << orderID);
        }

        if ((mask & py.flags.spells_forgotten) != 0u) {
            if (msp_ptr[orderID].level_required <= py.misc.level) {
                newSpells--;
                py.flags.spells_forgotten &= ~mask;
                py.flags.spells_learnt |= mask;

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
    auto spell_flag = (uint32_t) (0x7FFFFFFFL & ~py.flags.spells_learnt);

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

    for (int i = 31; (newSpells != 0) && (py.flags.spells_learnt != 0u); i--) {
        // orderID is the (i+1)th spell learned
        int orderID = py.flags.spells_learned_order[i];

        // shifting by amounts greater than number of bits in long gives
        // an undefined result, so don't shift for unknown spells
        if (orderID == 99) {
            mask = 0x0;
        } else {
            mask = (uint32_t) (1L << orderID);
        }

        if ((mask & py.flags.spells_learnt) != 0u) {
            py.flags.spells_learnt &= ~mask;
            py.flags.spells_forgotten |= mask;
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
        if (py.flags.spells_learned_order[last_known] == 99) {
            return last_known;
        }
    }

    // We should never actually reach this, but just in case... -MRC-
    return 0;
}

static uint32_t playerDetermineLearnableSpells() {
    uint32_t spell_flag = 0;

    for (int i = 0; i < py.unique_inventory_items; i++) {
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

        game.player_free_turn = true;
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
    spell_flag &= ~py.flags.spells_learnt;

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

                py.flags.spells_learnt |= 1L << spell_bank[c];
                py.flags.spells_learned_order[last_known++] = (uint8_t) spell_bank[c];

                for (; c <= spell_id - 1; c++) {
                    spell_bank[c] = spell_bank[c + 1];
                }

                spell_id--;

                eraseLine(Coord_t{c + 1, 31});
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
            py.flags.spells_learnt |= 1L << spell_bank[id];
            py.flags.spells_learned_order[last_known++] = (uint8_t) spell_bank[id];

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
    if (py.flags.spells_learnt != 0) {
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

    int32_t new_exp = py.base_exp_levels[py.misc.level - 1] * py.misc.experience_factor / 100;

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

    while ((py.misc.level < PLAYER_MAX_LEVEL) && (signed) (py.base_exp_levels[py.misc.level - 1] * py.misc.experience_factor / 100) <= py.misc.exp) {
        playerGainLevel();
    }

    if (py.misc.exp > py.misc.max_exp) {
        py.misc.max_exp = py.misc.exp;
    }

    printLongNumber(py.misc.exp, 14, STAT_COLUMN + 6);
}

// lets anyone enter wizard mode after a disclaimer... -JEW-
bool enterWizardMode() {
    bool answer = false;

    if (game.noscore == 0) {
        printMessage("Wizard mode is for debugging and experimenting.");
        answer = getInputConfirmation("The game will not be scored if you enter wizard mode. Are you sure?");
    }

    if ((game.noscore != 0) || answer) {
        game.noscore |= 0x2;
        game.wizard_mode = true;
        return true;
    }

    return false;
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

// Saving throws for player character. -RAK-
bool playerSavingThrow() {
    int class_level_adjustment = class_level_adj[py.misc.class_id][CLASS_SAVE] * py.misc.level / 3;

    int saving = py.misc.saving_throw + playerStatAdjustmentWisdomIntelligence(A_WIS) + class_level_adjustment;

    return randomNumber(100) <= saving;
}
