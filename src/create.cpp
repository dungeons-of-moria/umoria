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
        py.stats.max_stat[i] = (uint8_t) (5 + dice[3 * i] + dice[3 * i + 1] + dice[3 * i + 2]);
    }
}

// Changes stats by given amount -JWT-
static void characterChangeStat(int stat, int16_t amount) {
    int new_stat = py.stats.max_stat[stat];

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
    py.stats.max_stat[stat] = (uint8_t) new_stat;
}

// generate all stats and modify for race. needed in a separate
// module so looping of character selection would be allowed -RGM-
static void characterGenerateStatsAndRace() {
    Race_t *race = &character_races[py.misc.prace];

    characterGenerateStats();
    characterChangeStat(A_STR, race->str_adj);
    characterChangeStat(A_INT, race->int_adj);
    characterChangeStat(A_WIS, race->wis_adj);
    characterChangeStat(A_DEX, race->dex_adj);
    characterChangeStat(A_CON, race->con_adj);
    characterChangeStat(A_CHR, race->chr_adj);

    py.misc.lev = 1;

    for (int i = 0; i < 6; i++) {
        py.stats.cur_stat[i] = py.stats.max_stat[i];
        playerSetAndUseStat(i);
    }

    py.misc.srh = race->srh;
    py.misc.bth = race->bth;
    py.misc.bthb = race->bthb;
    py.misc.fos = race->fos;
    py.misc.stl = race->stl;
    py.misc.save = race->bsav;
    py.misc.hitdie = race->bhitdie;
    py.misc.ptodam = (int16_t) playerDamageAdjustment();
    py.misc.ptohit = (int16_t) playerToHitAdjustment();
    py.misc.ptoac = 0;
    py.misc.pac = (int16_t) playerArmorClassAdjustment();
    py.misc.expfact = race->b_exp;
    py.flags.see_infra = race->infra;
}

// Prints a list of the available races: Human, Elf, etc.,
// shown during the character creation screens.
static void displayCharacterRaces(void) {
    clearToBottom(20);
    putString("Choose a race (? for Help):", 20, 2);

    int y = 2;
    int x = 21;

    for (int i = 0; i < MAX_RACES; i++) {
        char description[80];

        (void) sprintf(description, "%c) %s", i + 'a', character_races[i].trace);
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

        if (race_id < MAX_RACES && race_id >= 0) {
            break;
        } else if (input == '?') {
            displayTextHelpFile(MORIA_WELCOME);
        } else {
            terminalBellSound();
        }
    }

    py.misc.prace = (uint8_t) race_id;

    putString(character_races[race_id].trace, 3, 15);
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
    for (int i = 0; i < 4; i++) {
        py.misc.history[i][0] = '\0';
    }
}

// Get the racial history, determines social class -RAK-
//
// Assumptions:
//   - Each race has init history beginning at (race-1)*3+1
//   - All history parts are in ascending order
static void characterGetHistory() {
    int history_id = py.misc.prace * 3 + 1;
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

                Background_t *background = &character_backgrounds[background_id];

                (void) strcat(history_block, background->info);
                social_class += background->bonus - 50;

                if (history_id > background->next) {
                    background_id = 0;
                }

                history_id = background->next;
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

    py.misc.sc = (int16_t) social_class;
}

// Gets the character's sex -JWT-
static void get_sex() {
    clearToBottom(20);
    putString("Choose a sex (? for Help):", 20, 2);
    putString("m) Male       f) Female", 21, 2);

    char c;

    bool exit_flag = false;
    while (!exit_flag) {
        moveCursor(20, 29);

        // speed not important here
        c = getKeyInput();
        if (c == 'f' || c == 'F') {
            py.misc.male = false;
            putString("Female", 4, 15);
            exit_flag = true;
        } else if (c == 'm' || c == 'M') {
            py.misc.male = true;
            putString("Male", 4, 15);
            exit_flag = true;
        } else if (c == '?') {
            displayTextHelpFile(MORIA_WELCOME);
        } else {
            terminalBellSound();
        }
    }
}

// Computes character's age, height, and weight -JWT-
static void get_ahw() {
    int i = py.misc.prace;
    py.misc.age = (uint16_t) (character_races[i].b_age + randomNumber((int) character_races[i].m_age));
    if (py.misc.male) {
        py.misc.ht = (uint16_t) randomNumberNormalDistribution((int) character_races[i].m_b_ht, (int) character_races[i].m_m_ht);
        py.misc.wt = (uint16_t) randomNumberNormalDistribution((int) character_races[i].m_b_wt, (int) character_races[i].m_m_wt);
    } else {
        py.misc.ht = (uint16_t) randomNumberNormalDistribution((int) character_races[i].f_b_ht, (int) character_races[i].f_m_ht);
        py.misc.wt = (uint16_t) randomNumberNormalDistribution((int) character_races[i].f_b_wt, (int) character_races[i].f_m_wt);
    }
    py.misc.disarm = (int16_t) (character_races[i].b_dis + playerDisarmAdjustment());
}

// Prints the classes for a given race: Rogue, Mage, Priest, etc.,
// shown during the character creation screens.
static int print_classes(int race_id, int *class_list) {
    int col = 2;
    int row = 21;

    int class_id = 0;

    char tmp_str[80];
    uint32_t mask = 0x1;

    clearToBottom(20);
    putString("Choose a class (? for Help):", 20, 2);

    for (int i = 0; i < MAX_CLASS; i++) {
        if (character_races[race_id].rtclass & mask) {
            (void) sprintf(tmp_str, "%c) %s", class_id + 'a', classes[i].title);
            putString(tmp_str, row, col);
            class_list[class_id] = i;

            col += 15;
            if (col > 70) {
                col = 2;
                row++;
            }
            class_id++;
        }
        mask <<= 1;
    }

    return class_id;
}

// Gets a character class -JWT-
static void get_class() {
    int class_list[MAX_CLASS];
    for (int cid = 0; cid < MAX_CLASS; cid++) {
        class_list[cid] = 0;
    }

    int class_count = print_classes(py.misc.prace, class_list);

    py.misc.pclass = 0;

    int min_value, max_value;

    bool exit_flag = false;
    while (!exit_flag) {
        moveCursor(20, 31);

        char s = getKeyInput();

        int cid = s - 'a';
        if (cid < class_count && cid >= 0) {
            exit_flag = true;

            py.misc.pclass = (uint8_t) class_list[cid];

            Class_t *c_ptr = &classes[py.misc.pclass];

            clearToBottom(20);
            putString(c_ptr->title, 5, 15);

            // Adjust the stats for the class adjustment -RAK-
            characterChangeStat(A_STR, c_ptr->madj_str);
            characterChangeStat(A_INT, c_ptr->madj_int);
            characterChangeStat(A_WIS, c_ptr->madj_wis);
            characterChangeStat(A_DEX, c_ptr->madj_dex);
            characterChangeStat(A_CON, c_ptr->madj_con);
            characterChangeStat(A_CHR, c_ptr->madj_chr);

            for (int i = 0; i < 6; i++) {
                py.stats.cur_stat[i] = py.stats.max_stat[i];
                playerSetAndUseStat(i);
            }

            py.misc.ptodam = (int16_t) playerDamageAdjustment(); // Real values
            py.misc.ptohit = (int16_t) playerToHitAdjustment();
            py.misc.ptoac = (int16_t) playerArmorClassAdjustment();
            py.misc.pac = 0;
            py.misc.dis_td = py.misc.ptodam; // Displayed values
            py.misc.dis_th = py.misc.ptohit;
            py.misc.dis_tac = py.misc.ptoac;
            py.misc.dis_ac = py.misc.pac + py.misc.dis_tac;

            // now set misc stats, do this after setting stats because of playerStatAdjustmentConstitution() for hit-points
            py.misc.hitdie += c_ptr->adj_hd;
            py.misc.mhp = (int16_t) (playerStatAdjustmentConstitution() + py.misc.hitdie);
            py.misc.chp = py.misc.mhp;
            py.misc.chp_frac = 0;

            // Initialize hit_points array.
            // Put bounds on total possible hp, only succeed
            // if it is within 1/8 of average value.
            min_value = (MAX_PLAYER_LEVEL * 3 / 8 * (py.misc.hitdie - 1)) + MAX_PLAYER_LEVEL;
            max_value = (MAX_PLAYER_LEVEL * 5 / 8 * (py.misc.hitdie - 1)) + MAX_PLAYER_LEVEL;
            player_base_hp_levels[0] = py.misc.hitdie;

            do {
                for (int i = 1; i < MAX_PLAYER_LEVEL; i++) {
                    player_base_hp_levels[i] = (uint16_t) randomNumber((int) py.misc.hitdie);
                    player_base_hp_levels[i] += player_base_hp_levels[i - 1];
                }
            } while (player_base_hp_levels[MAX_PLAYER_LEVEL - 1] < min_value || player_base_hp_levels[MAX_PLAYER_LEVEL - 1] > max_value);

            py.misc.bth += c_ptr->mbth;
            py.misc.bthb += c_ptr->mbthb; // RAK
            py.misc.srh += c_ptr->msrh;
            py.misc.disarm += c_ptr->mdis;
            py.misc.fos += c_ptr->mfos;
            py.misc.stl += c_ptr->mstl;
            py.misc.save += c_ptr->msav;
            py.misc.expfact += c_ptr->m_exp;
        } else if (s == '?') {
            displayTextHelpFile(MORIA_WELCOME);
        } else {
            terminalBellSound();
        }
    }
}

// Given a stat value, return a monetary value,
// which affects the amount of gold a player has.
static int monval(uint8_t i) {
    return 5 * ((int) i - 10);
}

static void get_money() {
    uint8_t *a_ptr = py.stats.max_stat;
    int tmp = monval(a_ptr[A_STR]) + monval(a_ptr[A_INT]) + monval(a_ptr[A_WIS]) + monval(a_ptr[A_CON]) + monval(a_ptr[A_DEX]);

    int gold = py.misc.sc * 6 + randomNumber(25) + 325; // Social Class adj
    gold -= tmp;                                   // Stat adj
    gold += monval(a_ptr[A_CHR]);                  // Charisma adj

    // She charmed the banker into it! -CJS-
    if (!py.misc.male) {
        gold += 50;
    }

    // Minimum
    if (gold < 80) {
        gold = 80;
    }

    py.misc.au = gold;
}

// Main Character Creation Routine -JWT-
void createCharacter() {
    printCharacterInformation();
    characterChooseRace();
    get_sex();

    // here we start a loop giving a player a choice of characters -RGM-
    characterGenerateStatsAndRace();
    characterGetHistory();
    get_ahw();
    displayCharacterHistory();
    printCharacterVitalStatistics();
    printCharacterStats();

    clearToBottom(20);
    putString("Hit space to re-roll or ESC to accept characteristics: ", 20, 2);

    bool exit_flag = true;
    while (exit_flag) {
        moveCursor(20, 56);
        char c = getKeyInput();
        if (c == ESCAPE) {
            exit_flag = false;
        } else if (c == ' ') {
            characterGenerateStatsAndRace();
            characterGetHistory();
            get_ahw();
            displayCharacterHistory();
            printCharacterVitalStatistics();
            printCharacterStats();
        } else {
            terminalBellSound();
        }
    }
    // done with stats generation

    get_class();
    get_money();
    printCharacterStats();
    printCharacterLevelExperience();
    printCharacterAbilities();
    getCharacterName();

    // This delay may be reduced, but is recommended to keep players from
    // continuously rolling up characters, which can be VERY expensive CPU wise.
    waitAndConfirmCharacterCreation(23, PLAYER_EXIT_PAUSE);
}
