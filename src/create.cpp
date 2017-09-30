// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Create a player character

#include "headers.h"
#include "externs.h"

// Generates character's stats -JWT-
static void characterGenerateStats() {
    int total;
    int dice[18];

    do {
        total = 0;
        for (int i = 0; i < 18; i++) {
            // Roll 3,4,5 sided dice once each
            dice[i] = randomNumber(3 + i % 3);
            total += dice[i];
        }
    } while (total <= 42 || total >= 54);

    for (int i = 0; i < 6; i++) {
        py.stats.max[i] = (uint8_t) (5 + dice[3 * i] + dice[3 * i + 1] + dice[3 * i + 2]);
    }
}

// Changes stats by given amount -JWT-
static void characterChangeStat(int stat, int16_t amount) {
    int new_stat = py.stats.max[stat];

    if (amount < 0) {
        for (int i = 0; i > amount; i--) {
            if (new_stat > 108) {
                new_stat--;
            } else if (new_stat > 88) {
                new_stat += -randomNumber(6) - 2;
            } else if (new_stat > 18) {
                new_stat += -randomNumber(15) - 5;
                if (new_stat < 18) {
                    new_stat = 18;
                }
            } else if (new_stat > 3) {
                new_stat--;
            }
        }
    } else {
        for (int i = 0; i < amount; i++) {
            if (new_stat < 18) {
                new_stat++;
            } else if (new_stat < 88) {
                new_stat += randomNumber(15) + 5;
            } else if (new_stat < 108) {
                new_stat += randomNumber(6) + 2;
            } else if (new_stat < 118) {
                new_stat++;
            }
        }
    }
    py.stats.max[stat] = (uint8_t) new_stat;
}

// generate all stats and modify for race. needed in a separate
// module so looping of character selection would be allowed -RGM-
static void characterGenerateStatsAndRace() {
    Race_t &race = character_races[py.misc.race_id];

    characterGenerateStats();
    characterChangeStat(A_STR, race.str_adjustment);
    characterChangeStat(A_INT, race.int_adjustment);
    characterChangeStat(A_WIS, race.wis_adjustment);
    characterChangeStat(A_DEX, race.dex_adjustment);
    characterChangeStat(A_CON, race.con_adjustment);
    characterChangeStat(A_CHR, race.chr_adjustment);

    py.misc.level = 1;

    for (int i = 0; i < 6; i++) {
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
    py.misc.plusses_to_damage = (int16_t) playerDamageAdjustment();
    py.misc.plusses_to_hit = (int16_t) playerToHitAdjustment();
    py.misc.magical_ac = 0;
    py.misc.ac = (int16_t) playerArmorClassAdjustment();
    py.misc.experience_factor = race.exp_factor_base;
    py.flags.see_infra = race.infra_vision;
}

// Prints a list of the available races: Human, Elf, etc.,
// shown during the character creation screens.
static void displayCharacterRaces() {
    clearToBottom(20);
    putString("Choose a race (? for Help):", 20, 2);

    int y = 2;
    int x = 21;

    for (int i = 0; i < PLAYER_MAX_RACES; i++) {
        char description[80];

        (void) sprintf(description, "%c) %s", i + 'a', character_races[i].name);
        putString(description, x, y);

        y += 15;
        if (y > 70) {
            y = 2;
            x++;
        }
    }
}

// Allows player to select a race -JWT-
static void characterChooseRace() {
    displayCharacterRaces();

    int race_id = 0;

    while (true) {
        moveCursor(20, 30);

        char input = getKeyInput();
        race_id = input - 'a';

        if (race_id < PLAYER_MAX_RACES && race_id >= 0) {
            break;
        }

        if (input == '?') {
            displayTextHelpFile(MORIA_WELCOME);
        } else {
            terminalBellSound();
        }
    }

    py.misc.race_id = (uint8_t) race_id;

    putString(character_races[race_id].name, 3, 15);
}

// Will print the history of a character -JWT-
static void displayCharacterHistory() {
    putString("Character Background", 14, 27);

    for (int i = 0; i < 4; i++) {
        putStringClearToEOL(py.misc.history[i], i + 15, 10);
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
    int history_id = py.misc.race_id * 3 + 1;
    int social_class = randomNumber(4);

    char history_block[240];
    history_block[0] = '\0';

    int background_id = 0;

    // Get a block of history text
    do {
        bool flag = false;
        while (!flag) {
            if (character_backgrounds[background_id].chart == history_id) {
                int test_roll = randomNumber(100);

                while (test_roll > character_backgrounds[background_id].roll) {
                    background_id++;
                }

                Background_t &background = character_backgrounds[background_id];

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
    int cursor_end = (int) strlen(history_block) - 1;
    while (history_block[cursor_end] == ' ') {
        cursor_end--;
    }

    int line_number = 0;
    int new_cursor_start = 0;
    int current_cursor_position;

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
    putString("Choose a sex (? for Help):", 20, 2);
    putString("m) Male       f) Female", 21, 2);

    char input;
    bool is_set = false;

    while (!is_set) {
        moveCursor(20, 29);

        // speed not important here
        input = getKeyInput();

        if (input == 'f' || input == 'F') {
            playerSetGender(false);
            putString("Female", 4, 15);
            is_set = true;
        } else if (input == 'm' || input == 'M') {
            playerSetGender(true);
            putString("Male", 4, 15);
            is_set = true;
        } else if (input == '?') {
            displayTextHelpFile(MORIA_WELCOME);
        } else {
            terminalBellSound();
        }
    }
}

// Computes character's age, height, and weight -JWT-
static void characterSetAgeHeightWeight() {
    int race_id = py.misc.race_id;
    py.misc.age = (uint16_t) (character_races[race_id].base_age + randomNumber((int) character_races[race_id].max_age));

    if (playerIsMale()) {
        py.misc.height = (uint16_t) randomNumberNormalDistribution((int) character_races[race_id].male_height_base, (int) character_races[race_id].male_height_mod);
        py.misc.weight = (uint16_t) randomNumberNormalDistribution((int) character_races[race_id].male_weight_base, (int) character_races[race_id].male_weight_mod);
    } else {
        py.misc.height = (uint16_t) randomNumberNormalDistribution((int) character_races[race_id].female_height_base, (int) character_races[race_id].female_height_mod);
        py.misc.weight = (uint16_t) randomNumberNormalDistribution((int) character_races[race_id].female_weight_base, (int) character_races[race_id].female_weight_mod);
    }

    py.misc.disarm = (int16_t) (character_races[race_id].disarm_chance_base + playerDisarmAdjustment());
}

// Prints the classes for a given race: Rogue, Mage, Priest, etc.,
// shown during the character creation screens.
static int displayRaceClasses(int race_id, int *class_list) {
    int y = 2;
    int x = 21;

    int class_id = 0;

    char description[80];
    uint32_t mask = 0x1;

    clearToBottom(20);
    putString("Choose a class (? for Help):", 20, 2);

    for (int i = 0; i < PLAYER_MAX_CLASSES; i++) {
        if ((character_races[race_id].classes_bit_field & mask) != 0u) {
            (void) sprintf(description, "%c) %s", class_id + 'a', classes[i].title);
            putString(description, x, y);
            class_list[class_id] = i;

            y += 15;
            if (y > 70) {
                y = 2;
                x++;
            }
            class_id++;
        }
        mask <<= 1;
    }

    return class_id;
}

// Gets a character class -JWT-
static void characterGetClass() {
    int class_list[PLAYER_MAX_CLASSES];
    for (int &entry : class_list) {
        entry = 0;
    }

    int class_count = displayRaceClasses(py.misc.race_id, class_list);

    py.misc.class_id = 0;

    int min_value, max_value;
    bool is_set = false;

    while (!is_set) {
        moveCursor(20, 31);

        char input = getKeyInput();
        int class_id = input - 'a';

        if (class_id < class_count && class_id >= 0) {
            is_set = true;

            py.misc.class_id = (uint8_t) class_list[class_id];

            Class_t &klass = classes[py.misc.class_id];

            clearToBottom(20);
            putString(klass.title, 5, 15);

            // Adjust the stats for the class adjustment -RAK-
            characterChangeStat(A_STR, klass.strength);
            characterChangeStat(A_INT, klass.intelligence);
            characterChangeStat(A_WIS, klass.wisdom);
            characterChangeStat(A_DEX, klass.dexterity);
            characterChangeStat(A_CON, klass.constitution);
            characterChangeStat(A_CHR, klass.charisma);

            for (int i = 0; i < 6; i++) {
                py.stats.current[i] = py.stats.max[i];
                playerSetAndUseStat(i);
            }

            // Real values
            py.misc.plusses_to_damage = (int16_t) playerDamageAdjustment();
            py.misc.plusses_to_hit = (int16_t) playerToHitAdjustment();
            py.misc.magical_ac = (int16_t) playerArmorClassAdjustment();
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
            min_value = (PLAYER_MAX_LEVEL * 3 / 8 * (py.misc.hit_die - 1)) + PLAYER_MAX_LEVEL;
            max_value = (PLAYER_MAX_LEVEL * 5 / 8 * (py.misc.hit_die - 1)) + PLAYER_MAX_LEVEL;
            player_base_hp_levels[0] = py.misc.hit_die;

            do {
                for (int i = 1; i < PLAYER_MAX_LEVEL; i++) {
                    player_base_hp_levels[i] = (uint16_t) randomNumber((int) py.misc.hit_die);
                    player_base_hp_levels[i] += player_base_hp_levels[i - 1];
                }
            } while (player_base_hp_levels[PLAYER_MAX_LEVEL - 1] < min_value || player_base_hp_levels[PLAYER_MAX_LEVEL - 1] > max_value);

            py.misc.bth += klass.base_to_hit;
            py.misc.bth_with_bows += klass.base_to_hit_with_bows; // RAK
            py.misc.chance_in_search += klass.searching;
            py.misc.disarm += klass.disarm_traps;
            py.misc.fos += klass.fos;
            py.misc.stealth_factor += klass.stealth;
            py.misc.saving_throw += klass.saving_throw;
            py.misc.experience_factor += klass.experience_factor;
        } else if (input == '?') {
            displayTextHelpFile(MORIA_WELCOME);
        } else {
            terminalBellSound();
        }
    }
}

// Given a stat value, return a monetary value,
// which affects the amount of gold a player has.
static int monetaryValueCalculatedFromStat(uint8_t stat) {
    return 5 * ((int) stat - 10);
}

static void playerCalculateStartGold() {
    int value = monetaryValueCalculatedFromStat(py.stats.max[A_STR]);
    value += monetaryValueCalculatedFromStat(py.stats.max[A_INT]);
    value += monetaryValueCalculatedFromStat(py.stats.max[A_WIS]);
    value += monetaryValueCalculatedFromStat(py.stats.max[A_CON]);
    value += monetaryValueCalculatedFromStat(py.stats.max[A_DEX]);

    // Social Class adjustment
    int new_gold = py.misc.social_class * 6 + randomNumber(25) + 325;

    // Stat adjustment
    new_gold -= value;

    // Charisma adjustment
    new_gold += monetaryValueCalculatedFromStat(py.stats.max[A_CHR]);

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
    characterGenerateStatsAndRace();
    characterGetHistory();
    characterSetAgeHeightWeight();
    displayCharacterHistory();
    printCharacterVitalStatistics();
    printCharacterStats();

    clearToBottom(20);
    putString("Hit space to re-roll or ESC to accept characteristics: ", 20, 2);

    bool exit_flag = true;
    while (exit_flag) {
        moveCursor(20, 56);

        char input = getKeyInput();

        if (input == ESCAPE) {
            exit_flag = false;
        } else if (input == ' ') {
            characterGenerateStatsAndRace();
            characterGetHistory();
            characterSetAgeHeightWeight();
            displayCharacterHistory();
            printCharacterVitalStatistics();
            printCharacterStats();
        } else {
            terminalBellSound();
        }
    }
    // done with stats generation

    characterGetClass();
    playerCalculateStartGold();
    printCharacterStats();
    printCharacterLevelExperience();
    printCharacterAbilities();
    getCharacterName();

    // This delay may be reduced, but is recommended to keep players from
    // continuously rolling up characters, which can be VERY expensive CPU wise.
    waitAndConfirmCharacterCreation(23, PLAYER_EXIT_PAUSE);
}
