// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Code executed when player dies

#include "headers.h"
#include "externs.h"
#include "version.h"

static void date(char *day) {
    char *tmp;
    time_t clockvar = time((time_t *) 0);
    tmp = ctime(&clockvar);
    tmp[10] = '\0';
    (void) strcpy(day, tmp);
}

// Centers a string within a 31 character string -JWT-
static char *center_string(char *centered_str, const char *in_str) {
    int i = (int) strlen(in_str);
    int j = 15 - i / 2;
    (void) sprintf(centered_str, "%*s%s%*s", j, "", in_str, 31 - i - j, "");
    return centered_str;
}

void display_scores() {
    char string[100];

    if ((highscore_fp = fopen(MORIA_TOP, "rb")) == NULL) {
        sprintf(string, "Error opening score file \"%s\"\n", MORIA_TOP);
        msg_print(string);
        msg_print(CNIL);
        return;
    }

    (void) fseek(highscore_fp, (off_t) 0, SEEK_SET);

    // Read version numbers from the score file, and check for validity.
    uint8_t version_maj = (uint8_t) getc(highscore_fp);
    uint8_t version_min = (uint8_t) getc(highscore_fp);
    uint8_t patch_level = (uint8_t) getc(highscore_fp);

    // Support score files from 5.2.2 to present.
    if (feof(highscore_fp)) {
        ; // An empty score file.
    } else if (version_maj != CURRENT_VERSION_MAJOR ||
               version_min > CURRENT_VERSION_MINOR ||
               (version_min == CURRENT_VERSION_MINOR && patch_level > CURRENT_VERSION_PATCH) ||
               (version_min == 2 && patch_level < 2) ||
                version_min < 2
            ) {
        msg_print("Sorry. This score file is from a different version of umoria.");
        msg_print(CNIL);

        (void) fclose(highscore_fp);
        return;
    }

    // set the static fileptr in save.c to the high score file pointer
    set_fileptr(highscore_fp);

    HighScore_t score;
    rd_highscore(&score);

    char input;

    int i = 0;
    int rank = 1;

    while (!feof(highscore_fp)) {
        i = 1;
        clear_screen();
        // Put twenty scores on each page, on lines 2 through 21.
        while (!feof(highscore_fp) && i < 21) {
            (void) sprintf(string,
                           "%-4d%8d %-19.19s %c %-10.10s %-7.7s%3d %-22.22s",
                           rank, score.points, score.name, score.sex,
                           races[score.race].trace, classes[score.character_class].title,
                           score.lev, score.died_from);
            prt(string, ++i, 0);
            rank++;
            rd_highscore(&score);
        }
        prt("Rank  Points Name              Sex Race       Class  Lvl Killed By", 0, 0);
        erase_line(1, 0);
        prt("[Press any key to continue.]", 23, 23);
        input = inkey();
        if (input == ESCAPE) {
            break;
        }
    }

    (void) fclose(highscore_fp);
}

bool duplicate_character() {
    // Only check for duplicate characters under unix.
    return false;
}

// Prints the gravestone of the character -RAK-
static void print_tomb() {
    char *p;
    vtype_t str, tmp_str;

    clear_screen();
    put_buffer("_______________________", 1, 15);
    put_buffer("/", 2, 14);
    put_buffer("\\         ___", 2, 38);
    put_buffer("/", 3, 13);
    put_buffer("\\ ___   /   \\      ___", 3, 39);
    put_buffer("/            RIP            \\   \\  :   :     /   \\", 4, 12);
    put_buffer("/", 5, 11);
    put_buffer("\\  : _;,,,;_    :   :", 5, 41);
    (void) sprintf(str, "/%s\\,;_          _;,,,;_", center_string(tmp_str, py.misc.name));
    put_buffer(str, 6, 10);
    put_buffer("|               the               |   ___", 7, 9);
    if (!total_winner) {
        p = title_string();
    } else {
        p = (char *) "Magnificent";
    }
    (void) sprintf(str, "| %s |  /   \\", center_string(tmp_str, p));
    put_buffer(str, 8, 9);
    put_buffer("|", 9, 9);
    put_buffer("|  :   :", 9, 43);
    if (!total_winner) {
        p = (char *) classes[py.misc.pclass].title;
    } else if (py.misc.male) {
        p = (char *) "*King*";
    } else {
        p = (char *) "*Queen*";
    }
    (void) sprintf(str, "| %s | _;,,,;_   ____", center_string(tmp_str, p));
    put_buffer(str, 10, 9);
    (void) sprintf(str, "Level : %d", (int) py.misc.lev);
    (void) sprintf(str, "| %s |          /    \\", center_string(tmp_str, str));
    put_buffer(str, 11, 9);
    (void) sprintf(str, "%d Exp", py.misc.exp);
    (void) sprintf(str, "| %s |          :    :", center_string(tmp_str, str));
    put_buffer(str, 12, 9);
    (void) sprintf(str, "%d Au", py.misc.au);
    (void) sprintf(str, "| %s |          :    :", center_string(tmp_str, str));
    put_buffer(str, 13, 9);
    (void) sprintf(str, "Died on Level : %d", current_dungeon_level);
    (void) sprintf(str, "| %s |         _;,,,,;_", center_string(tmp_str, str));
    put_buffer(str, 14, 9);
    put_buffer("|            killed by            |", 15, 9);
    p = died_from;

    int len = (int) strlen(p);
    p[len] = '.'; // add a trailing period
    p[len + 1] = '\0';
    (void) sprintf(str, "| %s |", center_string(tmp_str, p));
    put_buffer(str, 16, 9);
    p[len] = '\0'; // strip off the period

    char day[11];
    date(day);
    (void) sprintf(str, "| %s |", center_string(tmp_str, day));

    put_buffer(str, 17, 9);
    put_buffer("*|   *     *     *    *   *     *  | *", 18, 8);
    put_buffer("________)/\\\\_)_/___(\\/___(//_\\)/_\\//__\\\\(/_|_)_______", 19, 0);

    retry:
    flush();

    put_buffer("(ESC to abort, return to print on screen, or file name)", 23, 0);
    put_buffer("Character record?", 22, 0);

    if (get_string(str, 22, 18, 60)) {
        for (int i = 0; i < INVEN_ARRAY_SIZE; i++) {
            known1(&inventory[i]);
            known2(&inventory[i]);
        }

        calc_bonuses();

        if (str[0]) {
            if (!file_character(str)) {
                goto retry;
            }
        } else {
            clear_screen();
            display_char();
            put_buffer("Type ESC to skip the inventory:", 23, 0);
            if (inkey() != ESCAPE) {
                clear_screen();
                msg_print("You are using:");
                (void) show_equip(true, 0);
                msg_print(CNIL);
                msg_print("You are carrying:");
                clear_from(1);
                (void) show_inven(0, inven_ctr - 1, true, 0, CNIL);
                msg_print(CNIL);
            }
        }
    }
}

// Calculates the total number of points earned -JWT-
int32_t total_points() {
    int32_t total = py.misc.max_exp + (100 * py.misc.max_dlv);
    total += py.misc.au / 100;

    for (int i = 0; i < INVEN_ARRAY_SIZE; i++) {
        total += item_value(&inventory[i]);
    }

    total += current_dungeon_level * 50;

    // Don't ever let the score decrease from one save to the next.
    if (character_max_score > total) {
        return character_max_score;
    }

    return total;
}

// Enters a players name on the top twenty list -JWT-
static void highscores() {
    clear_screen();

    if (noscore) {
        return;
    }

    if (panic_save) {
        msg_print("Sorry, scores for games restored from panic save files are not saved.");
        return;
    }

    HighScore_t new_entry;
    new_entry.points = total_points();
    new_entry.birth_date = birth_date;
    new_entry.uid = 0; // NOTE: do we not want to use `getuid()`? -MRC-
    new_entry.mhp = py.misc.mhp;
    new_entry.chp = py.misc.chp;
    new_entry.dun_level = (uint8_t) current_dungeon_level;
    new_entry.lev = (uint8_t) py.misc.lev;
    new_entry.max_dlv = (uint8_t) py.misc.max_dlv;
    new_entry.sex = (py.misc.male ? 'M' : 'F');
    new_entry.race = py.misc.prace;
    new_entry.character_class = py.misc.pclass;
    (void) strcpy(new_entry.name, py.misc.name);

    char *tmp = died_from;
    if ('a' == *tmp) {
        if ('n' == *(++tmp)) {
            tmp++;
        }
        while (isspace(*tmp)) {
            tmp++;
        }
    }
    (void) strcpy(new_entry.died_from, tmp);

    if ((highscore_fp = fopen(MORIA_TOP, "rb+")) == NULL) {
        char string[100];

        (void) sprintf(string, "Error opening score file \"%s\"\n", MORIA_TOP);
        msg_print(string);
        msg_print(CNIL);
        return;
    }

    // Search file to find where to insert this character, if uid != 0 and
    // find same uid/sex/race/class combo then exit without saving this score.
    // Seek to the beginning of the file just to be safe.
    (void) fseek(highscore_fp, (long) 0, SEEK_SET);

    // Read version numbers from the score file, and check for validity.
    uint8_t version_maj = (uint8_t) getc(highscore_fp);
    uint8_t version_min = (uint8_t) getc(highscore_fp);
    uint8_t patch_level = (uint8_t) getc(highscore_fp);

    // If this is a new score file, it should be empty.
    // Write the current version numbers to the score file.
    if (feof(highscore_fp)) {
        // Seek to the beginning of the file just to be safe.
        (void) fseek(highscore_fp, (long) 0, SEEK_SET);

        (void) putc(CURRENT_VERSION_MAJOR, highscore_fp);
        (void) putc(CURRENT_VERSION_MINOR, highscore_fp);
        (void) putc(CURRENT_VERSION_PATCH, highscore_fp);

        // must fseek() before can change read/write mode
        (void) fseek(highscore_fp, (long) 0, SEEK_CUR);
    } else if (
            version_maj != CURRENT_VERSION_MAJOR ||
            version_min > CURRENT_VERSION_MINOR ||
            (version_min == CURRENT_VERSION_MINOR && patch_level > CURRENT_VERSION_PATCH) ||
            (version_min == 2 && patch_level < 2) ||
            version_min < 2
            ) {
        // Support score files from 5.2.2 to present.
        // No need to print a message, a subsequent call to
        // display_scores() will print a message.
        (void) fclose(highscore_fp);
        return;
    }

    // set the static fileptr in save.c to the high score file pointer
    set_fileptr(highscore_fp);

    HighScore_t old_entry, entry;

    int i = 0;
    off_t curpos = ftell(highscore_fp);
    rd_highscore(&old_entry);

    while (!feof(highscore_fp)) {
        if (new_entry.points >= old_entry.points) {
            break;
        }

        // under unix, only allow one sex/race/class combo per person,
        // on single user system, allow any number of entries, but try to
        // prevent multiple entries per character by checking for case when
        // birthdate/sex/race/class are the same, and died_from of score file
        // entry is "(saved)"
        if (((new_entry.uid != 0 && new_entry.uid == old_entry.uid) ||
             (new_entry.uid == 0 && !strcmp(old_entry.died_from, "(saved)") && new_entry.birth_date == old_entry.birth_date)) &&
            new_entry.sex == old_entry.sex &&
            new_entry.race == old_entry.race &&
            new_entry.character_class == old_entry.character_class) {
            (void) fclose(highscore_fp);
            return;
        }

        // only allow one thousand scores in the score file
        if (++i >= SCOREFILE_SIZE) {
            (void) fclose(highscore_fp);
            return;
        }

        curpos = ftell(highscore_fp);
        rd_highscore(&old_entry);
    }

    if (feof(highscore_fp)) {
        // write out new_entry at end of file
        (void) fseek(highscore_fp, curpos, SEEK_SET);

        wr_highscore(&new_entry);
    } else {
        entry = new_entry;

        while (!feof(highscore_fp)) {
            (void) fseek(highscore_fp, -(long) sizeof(HighScore_t) - (long) sizeof(char), SEEK_CUR);

            wr_highscore(&entry);

            // under unix, only allow one sex/race/class combo per person,
            // on single user system, allow any number of entries, but try
            // to prevent multiple entries per character by checking for
            // case when birthdate/sex/race/class are the same, and died_from
            // of score file entry is "(saved)"
            if (((new_entry.uid != 0 && new_entry.uid == old_entry.uid) ||
                 (new_entry.uid == 0 && !strcmp(old_entry.died_from, "(saved)") && new_entry.birth_date == old_entry.birth_date)) &&
                new_entry.sex == old_entry.sex &&
                new_entry.race == old_entry.race &&
                new_entry.character_class == old_entry.character_class) {
                break;
            }
            entry = old_entry;

            // must fseek() before can change read/write mode
            (void) fseek(highscore_fp, (long) 0, SEEK_CUR);

            curpos = ftell(highscore_fp);
            rd_highscore(&old_entry);
        }
        if (feof(highscore_fp)) {
            (void) fseek(highscore_fp, curpos, SEEK_SET);

            wr_highscore(&entry);
        }
    }

    (void) fclose(highscore_fp);
}

// Change the player into a King! -RAK-
static void kingly() {
    const char *p;

    // Change the character attributes.
    current_dungeon_level = 0;
    (void) strcpy(died_from, "Ripe Old Age");

    (void) restore_level();

    py.misc.lev += MAX_PLAYER_LEVEL;
    py.misc.au += 250000L;
    py.misc.max_exp += 5000000L;
    py.misc.exp = py.misc.max_exp;

    // Let the player know that he did good.
    clear_screen();
    put_buffer("#", 1, 34);
    put_buffer("#####", 2, 32);
    put_buffer("#", 3, 34);
    put_buffer(",,,  $$$  ,,,", 4, 28);
    put_buffer(",,=$   \"$$$$$\"   $=,,", 5, 24);
    put_buffer(",$$        $$$        $$,", 6, 22);
    put_buffer("*>         <*>         <*", 7, 22);
    put_buffer("$$         $$$         $$", 8, 22);
    put_buffer("\"$$        $$$        $$\"", 9, 22);
    put_buffer("\"$$       $$$       $$\"", 10, 23);
    p = "*#########*#########*";
    put_buffer(p, 11, 24);
    put_buffer(p, 12, 24);
    put_buffer("Veni, Vidi, Vici!", 15, 26);
    put_buffer("I came, I saw, I conquered!", 16, 21);
    if (py.misc.male) {
        put_buffer("All Hail the Mighty King!", 17, 22);
    } else {
        put_buffer("All Hail the Mighty Queen!", 17, 22);
    }
    flush();
    pause_line(23);
}

// Handles the gravestone end top-twenty routines -RAK-
// What happens upon dying. -RAK-
void exit_game() {
    msg_print(CNIL);

    // flush all input
    flush();

    // If the game has been saved, then save sets turn back to -1,
    // which inhibits the printing of the tomb.
    if (current_game_turn >= 0) {
        if (total_winner) {
            kingly();
        }
        print_tomb();
    }

    if (character_generated && !character_saved) {
        // Save the memory at least.
        (void) save_char();
    }

    // add score to score file if applicable
    if (character_generated) {
        // Clear character_saved, strange thing to do, but it prevents
        // inkey() from recursively calling exit_game() when there has
        // been an eof on stdin detected.
        character_saved = false;
        highscores();
        display_scores();
    }
    erase_line(23, 0);
    restore_term();

    exit(0);
}
