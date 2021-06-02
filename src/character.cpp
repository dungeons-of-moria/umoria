// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Generate a new player character

#include "headers.h"

// Generates character's stats -JWT-
static void characterGenerateStats() {
    int total;
    int dice[18];

    do {
        total = 0;
        for (auto i = 0; i < 18; i++) {
            // Roll 3,4,5 sided dice once each
            dice[i] = randomNumber(3 + i % 3);
            total += dice[i];
        }
    } while (total <= 42 || total >= 54);

    for (auto i = 0; i < 6; i++) {
        py.stats.max[i] = uint8_t(5 + dice[3 * i] + dice[3 * i + 1] + dice[3 * i + 2]);
    }
}

static uint8_t decrementStat(int16_t const adjustment, uint8_t const current_stat) {
    auto stat = current_stat;
    for (auto i = 0; i > adjustment; i--) {
        if (stat > 108) {
            stat--;
        } else if (stat > 88) {
            stat += -randomNumber(6) - 2;
        } else if (stat > 18) {
            stat += -randomNumber(15) - 5;
            if (stat < 18) {
                stat = 18;
            }
        } else if (stat > 3) {
            stat--;
        }
    }
    return stat;
}

static uint8_t incrementStat(int16_t const adjustment, uint8_t const current_stat) {
    auto stat = current_stat;
    for (auto i = 0; i < adjustment; i++) {
        if (stat < 18) {
            stat++;
        } else if (stat < 88) {
            stat += randomNumber(15) + 5;
        } else if (stat < 108) {
            stat += randomNumber(6) + 2;
        } else if (stat < 118) {
            stat++;
        }
    }
    return stat;
}

// Changes stats by given amount -JWT-
// During character creation we adjust player stats based
// on their Race and Class...with a little randomness!
static uint8_t createModifyPlayerStat(uint8_t const stat, int16_t const adjustment) {
    if (adjustment < 0) {
        return decrementStat(adjustment, stat);
    }
    return incrementStat(adjustment, stat);
}

// generate all stats and modify for race. needed in a separate
// module so looping of character selection would be allowed -RGM-
static void characterGenerateStatsAndRace() {
    Race_t const &race = character_races[py.misc.race_id];

    characterGenerateStats();
    py.stats.max[PlayerAttr::A_STR] = createModifyPlayerStat(py.stats.max[PlayerAttr::A_STR], race.str_adjustment);
    py.stats.max[PlayerAttr::A_INT] = createModifyPlayerStat(py.stats.max[PlayerAttr::A_INT], race.int_adjustment);
    py.stats.max[PlayerAttr::A_WIS] = createModifyPlayerStat(py.stats.max[PlayerAttr::A_WIS], race.wis_adjustment);
    py.stats.max[PlayerAttr::A_DEX] = createModifyPlayerStat(py.stats.max[PlayerAttr::A_DEX], race.dex_adjustment);
    py.stats.max[PlayerAttr::A_CON] = createModifyPlayerStat(py.stats.max[PlayerAttr::A_CON], race.con_adjustment);
    py.stats.max[PlayerAttr::A_CHR] = createModifyPlayerStat(py.stats.max[PlayerAttr::A_CHR], race.chr_adjustment);

    py.misc.level = 1;

    for (auto i = 0; i < 6; i++) {
        py.stats.current[i] = py.stats.max[i];
        playerSetAndUseStat(i);
    }

    py.misc.chance_in_search = race.search_chance_base;
    py.misc.bth = race.base_to_hit;
    py.misc.bth_with_bows = race.base_to_hit_bows;
    py.misc.fos = race.fos;
    py.misc.stealth_factor = race.stealth;
    py.misc.saving_throw = race.saving_throw_base;
    py.misc.hit_die = race.hit_points_base;
    py.misc.plusses_to_damage = playerDamageAdjustment();
    py.misc.plusses_to_hit = playerToHitAdjustment();
    py.misc.magical_ac = 0;
    py.misc.ac = playerArmorClassAdjustment();
    py.misc.experience_factor = race.exp_factor_base;
    py.flags.see_infra = race.infra_vision;
}

// Prints a list of the available races: Human, Elf, etc.,
// shown during the character creation screens.
static void displayCharacterRaces() {
    clearToBottom(20);
    putString("Choose a race (? for Help):", Coord_t{20, 2});

    Coord_t coord = Coord_t{21, 2};

    for (auto i = 0; i < PLAYER_MAX_RACES; i++) {
        char description[80];

        (void) sprintf(description, "%c) %s", i + 'a', character_races[i].name);
        putString(description, coord);

        coord.x += 15;
        if (coord.x > 70) {
            coord.x = 2;
            coord.y++;
        }
    }
}

// Allows player to select a race -JWT-
static void characterChooseRace() {
    displayCharacterRaces();

    int id;
    while (true) {
        moveCursor(Coord_t{20, 30});
        const char key = getKeyInput();

        id = key - 97; // ASCII `a`, setting id between 0 and 7
        if (id >= 0 && id < PLAYER_MAX_RACES) {
            break;
        } else if (key == '?') {
            displayTextHelpFile(config::files::welcome_screen);
        } else {
            terminalBellSound();
        }
    }
    py.misc.race_id = (uint8_t) id;

    putString(character_races[id].name, Coord_t{3, 15});
}

// Will print the history of a character -JWT-
static void displayCharacterHistory() {
    putString("Character Background", Coord_t{14, 27});

    for (int i = 0; i < 4; i++) {
        putStringClearToEOL(py.misc.history[i], Coord_t{i + 15, 10});
    }
}

// Clear the previous history strings
static void playerClearHistory() {
    for (auto &entry : py.misc.history) {
        entry[0] = '\0';
    }
}

// Get the racial history, determines social class -RAK-
//
// Assumptions:
//   - Each race has init history beginning at (race-1)*3+1
//   - All history parts are in ascending order
static void characterGetHistory() {
    auto history_id = py.misc.race_id * 3 + 1;
    auto social_class = randomNumber(4);

    char history_block[240];
    history_block[0] = '\0';

    auto background_id = 0;

    // Get a block of history text
    do {
        bool flag = false;
        while (!flag) {
            if (character_backgrounds[background_id].chart == history_id) {
                int test_roll = randomNumber(100);

                while (test_roll > character_backgrounds[background_id].roll) {
                    background_id++;
                }

                Background_t const &background = character_backgrounds[background_id];

                (void) strcat(history_block, background.info);
                social_class += background.bonus - 50;

                if (history_id > background.next) {
                    background_id = 0;
                }

                history_id = background.next;
                flag = true;
            } else {
                background_id++;
            }
        }
    } while (history_id >= 1);

    playerClearHistory();

    // Process block of history text for pretty output
    int cursor_start = 0;
    auto cursor_end = (int) strlen(history_block) - 1;
    while (history_block[cursor_end] == ' ') {
        cursor_end--;
    }

    int line_number = 0;
    int new_cursor_start = 0;
    int current_cursor_position = 0;

    bool flag = false;
    while (!flag) {
        while (history_block[cursor_start] == ' ') {
            cursor_start++;
        }

        current_cursor_position = cursor_end - cursor_start + 1;

        if (current_cursor_position > 60) {
            current_cursor_position = 60;

            while (history_block[cursor_start + current_cursor_position - 1] != ' ') {
                current_cursor_position--;
            }

            new_cursor_start = cursor_start + current_cursor_position;

            while (history_block[cursor_start + current_cursor_position - 1] == ' ') {
                current_cursor_position--;
            }
        } else {
            flag = true;
        }

        (void) strncpy(py.misc.history[line_number], &history_block[cursor_start], (size_t) current_cursor_position);
        py.misc.history[line_number][current_cursor_position] = '\0';

        line_number++;
        cursor_start = new_cursor_start;
    }

    // Compute social class for player
    if (social_class > 100) {
        social_class = 100;
    } else if (social_class < 1) {
        social_class = 1;
    }

    py.misc.social_class = (int16_t) social_class;
}

// Gets the character's gender -JWT-
static void characterSetGender() {
    clearToBottom(20);
    putString("Choose a sex (? for Help):", Coord_t{20, 2});
    putString("m) Male       f) Female", Coord_t{21, 2});

    while (true) {
        moveCursor(Coord_t{20, 29});
        const char key = getKeyInput();

        if (key == 'f' || key == 'F') {
            playerSetGender(false);
            putString("Female", Coord_t{4, 15});
            break;
        } else if (key == 'm' || key == 'M') {
            playerSetGender(true);
            putString("Male", Coord_t{4, 15});
            break;
        } else if (key == '?') {
            displayTextHelpFile(config::files::welcome_screen);
        } else {
            terminalBellSound();
        }
    }
}

// Computes character's age, height, and weight -JWT-
static void characterSetAgeHeightWeight() {
    Race_t const &race = character_races[py.misc.race_id];

    py.misc.age = uint16_t(race.base_age + randomNumber(race.max_age));

    int height_base, height_mod, weight_base, weight_mod;
    if (playerIsMale()) {
        height_base = race.male_height_base;
        height_mod = race.male_height_mod;
        weight_base = race.male_weight_base;
        weight_mod = race.male_weight_mod;
    } else {
        height_base = race.female_height_base;
        height_mod = race.female_height_mod;
        weight_base = race.female_weight_base;
        weight_mod = race.female_weight_mod;
    }

    py.misc.height = (uint16_t) randomNumberNormalDistribution(height_base, height_mod);
    py.misc.weight = (uint16_t) randomNumberNormalDistribution(weight_base, weight_mod);
    py.misc.disarm = race.disarm_chance_base + playerDisarmAdjustment();
}

// Prints the classes for a given race: Rogue, Mage, Priest, etc.,
// shown during the character creation screens.
static int displayRaceClasses(uint8_t const race_id, uint8_t *class_list) {
    Coord_t coord = Coord_t{21, 2};

    auto class_id = 0;

    char description[80];
    uint32_t mask = 0x1;

    clearToBottom(20);
    putString("Choose a class (? for Help):", Coord_t{20, 2});

    for (uint8_t i = 0; i < PLAYER_MAX_CLASSES; i++) {
        if ((character_races[race_id].classes_bit_field & mask) != 0u) {
            (void) sprintf(description, "%c) %s", class_id + 'a', classes[i].title);
            putString(description, coord);
            class_list[class_id] = i;

            coord.x += 15;
            if (coord.x > 70) {
                coord.x = 2;
                coord.y++;
            }
            class_id++;
        }
        mask <<= 1;
    }

    return class_id;
}

static void generateCharacterClass(uint8_t const class_id) {
    py.misc.class_id = class_id;

    Class_t const &klass = classes[py.misc.class_id];

    clearToBottom(20);
    putString(klass.title, Coord_t{5, 15});

    // Adjust the stats for the class adjustment -RAK-
    py.stats.max[PlayerAttr::A_STR] = createModifyPlayerStat(py.stats.max[PlayerAttr::A_STR], klass.strength);
    py.stats.max[PlayerAttr::A_INT] = createModifyPlayerStat(py.stats.max[PlayerAttr::A_INT], klass.intelligence);
    py.stats.max[PlayerAttr::A_WIS] = createModifyPlayerStat(py.stats.max[PlayerAttr::A_WIS], klass.wisdom);
    py.stats.max[PlayerAttr::A_DEX] = createModifyPlayerStat(py.stats.max[PlayerAttr::A_DEX], klass.dexterity);
    py.stats.max[PlayerAttr::A_CON] = createModifyPlayerStat(py.stats.max[PlayerAttr::A_CON], klass.constitution);
    py.stats.max[PlayerAttr::A_CHR] = createModifyPlayerStat(py.stats.max[PlayerAttr::A_CHR], klass.charisma);

    for (auto i = 0; i < 6; i++) {
        py.stats.current[i] = py.stats.max[i];
        playerSetAndUseStat(i);
    }

    // Real values
    py.misc.plusses_to_damage = playerDamageAdjustment();
    py.misc.plusses_to_hit = playerToHitAdjustment();
    py.misc.magical_ac = playerArmorClassAdjustment();
    py.misc.ac = 0;

    // Displayed values
    py.misc.display_to_damage = py.misc.plusses_to_damage;
    py.misc.display_to_hit = py.misc.plusses_to_hit;
    py.misc.display_to_ac = py.misc.magical_ac;
    py.misc.display_ac = py.misc.ac + py.misc.display_to_ac;

    // now set misc stats, do this after setting stats because of playerStatAdjustmentConstitution() for hit-points
    py.misc.hit_die += klass.hit_points;
    py.misc.max_hp = (int16_t) (playerStatAdjustmentConstitution() + py.misc.hit_die);
    py.misc.current_hp = py.misc.max_hp;
    py.misc.current_hp_fraction = 0;

    // Initialize hit_points array.
    // Put bounds on total possible hp, only succeed
    // if it is within 1/8 of average value.
    auto min_value = (PLAYER_MAX_LEVEL * 3 / 8 * (py.misc.hit_die - 1)) + PLAYER_MAX_LEVEL;
    auto max_value = (PLAYER_MAX_LEVEL * 5 / 8 * (py.misc.hit_die - 1)) + PLAYER_MAX_LEVEL;
    py.base_hp_levels[0] = py.misc.hit_die;

    do {
        for (auto i = 1; i < PLAYER_MAX_LEVEL; i++) {
            py.base_hp_levels[i] = (uint16_t) randomNumber(py.misc.hit_die);
            py.base_hp_levels[i] += py.base_hp_levels[i - 1];
        }
    } while (py.base_hp_levels[PLAYER_MAX_LEVEL - 1] < min_value || py.base_hp_levels[PLAYER_MAX_LEVEL - 1] > max_value);

    py.misc.bth += klass.base_to_hit;
    py.misc.bth_with_bows += klass.base_to_hit_with_bows; // RAK
    py.misc.chance_in_search += klass.searching;
    py.misc.disarm += klass.disarm_traps;
    py.misc.fos += klass.fos;
    py.misc.stealth_factor += klass.stealth;
    py.misc.saving_throw += klass.saving_throw;
    py.misc.experience_factor += klass.experience_factor;
}

// Gets a character class -JWT-
static void characterGetClass() {
    uint8_t class_list[PLAYER_MAX_CLASSES];
    for (auto &entry : class_list) {
        entry = 0;
    }
    auto class_count = displayRaceClasses(py.misc.race_id, class_list);

    // Reset the class ID
    py.misc.class_id = 0;

    while (true) {
        moveCursor(Coord_t{20, 31});
        const char key = getKeyInput();

        int id = key - 97; // ASCII `a`, setting id to 0-5
        if (id >= 0 && id < class_count) {
            generateCharacterClass(class_list[id]);
            break;
        } else if (key == '?') {
            displayTextHelpFile(config::files::welcome_screen);
        } else {
            terminalBellSound();
        }
    }
}

// Given a stat value, return a monetary value,
// which affects the amount of gold a player has.
static int monetaryValueCalculatedFromStat(uint8_t const stat) {
    return 5 * (stat - 10);
}

static void playerCalculateStartGold() {
    auto value = monetaryValueCalculatedFromStat(py.stats.max[PlayerAttr::A_STR]);
    value += monetaryValueCalculatedFromStat(py.stats.max[PlayerAttr::A_INT]);
    value += monetaryValueCalculatedFromStat(py.stats.max[PlayerAttr::A_WIS]);
    value += monetaryValueCalculatedFromStat(py.stats.max[PlayerAttr::A_CON]);
    value += monetaryValueCalculatedFromStat(py.stats.max[PlayerAttr::A_DEX]);

    // Social Class adjustment
    auto new_gold = py.misc.social_class * 6 + randomNumber(25) + 325;

    // Stat adjustment
    new_gold -= value;

    // Charisma adjustment
    new_gold += monetaryValueCalculatedFromStat(py.stats.max[PlayerAttr::A_CHR]);

    // She charmed the banker into it! -CJS-
    if (!playerIsMale()) {
        new_gold += 50;
    }

    // Minimum
    if (new_gold < 80) {
        new_gold = 80;
    }

    py.misc.au = new_gold;
}

// Main Character Creation Routine -JWT-
void characterCreate() {
    printCharacterInformation();
    characterChooseRace();
    characterSetGender();

    // here we start a loop giving a player a choice of characters -RGM-
    bool done = false;
    while (!done) {
        characterGenerateStatsAndRace();
        characterGetHistory();
        characterSetAgeHeightWeight();
        displayCharacterHistory();
        printCharacterVitalStatistics();
        printCharacterStats();

        clearToBottom(20);
        putString("Hit space to re-roll or ESC to accept characteristics: ", Coord_t{20, 2});

        while (true) {
            const char key = getKeyInput();
            if (key == ESCAPE) {
                done = true;
                break;
            } else if (key == ' ') {
                break;
            } else {
                terminalBellSound();
            }
        }
    }

    characterGetClass();
    playerCalculateStartGold();
    printCharacterStats();
    printCharacterLevelExperience();
    printCharacterAbilities();
    getCharacterName();

    putStringClearToEOL("[ press any key to continue, or Q to exit ]", Coord_t{23, 17});
    if (getKeyInput() == 'Q') {
        exitProgram();
    }
    eraseLine(Coord_t{23, 0});
}
