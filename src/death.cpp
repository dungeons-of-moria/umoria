// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Code executed when player dies

#include "headers.h"
#include "externs.h"
#include "version.h"

void showScoresScreen() {
    char msg[100];

    if ((highscore_fp = fopen(MORIA_SCORES, "rb")) == nullptr) {
        sprintf(msg, "Error opening score file \"%s\"\n", MORIA_SCORES);
        printMessage(msg);
        printMessage(CNIL);
        return;
    }

    (void) fseek(highscore_fp, (off_t) 0, SEEK_SET);

    // Read version numbers from the score file, and check for validity.
    auto version_maj = (uint8_t) getc(highscore_fp);
    auto version_min = (uint8_t) getc(highscore_fp);
    auto patch_level = (uint8_t) getc(highscore_fp);

    // If score data present, check if a valid game version
    if (feof(highscore_fp) == 0 && !validGameVersion(version_maj, version_min, patch_level)) {
        printMessage("Sorry. This score file is from a different version of umoria.");
        printMessage(CNIL);
        (void) fclose(highscore_fp);
        return;
    }

    // set the static fileptr in save.c to the high score file pointer
    setFileptr(highscore_fp);

    HighScore_t score{};
    readHighScore(&score);

    char input;

    int i = 0;
    int rank = 1;

    while (feof(highscore_fp) == 0) {
        i = 1;
        clearScreen();
        // Put twenty scores on each page, on lines 2 through 21.
        while ((feof(highscore_fp) == 0) && i < 21) {
            (void) sprintf(
                msg,
                "%-4d%8d %-19.19s %c %-10.10s %-7.7s%3d %-22.22s",
                rank, score.points, score.name, score.gender,
                character_races[score.race].name, classes[score.character_class].title,
                score.level, score.died_from
            );
            putStringClearToEOL(msg, ++i, 0);
            rank++;
            readHighScore(&score);
        }
        putStringClearToEOL("Rank  Points Name              Sex Race       Class  Lvl Killed By", 0, 0);
        eraseLine(1, 0);
        putStringClearToEOL("[Press any key to continue.]", 23, 23);
        input = getKeyInput();
        if (input == ESCAPE) {
            break;
        }
    }

    (void) fclose(highscore_fp);
}

// Prints the gravestone of the character -RAK-
static void printTomb() {
    char *p = nullptr;
    vtype_t str = {'\0'};
    vtype_t tmp_str = {'\0'};

    clearScreen();
    putString("_______________________", 1, 15);
    putString("/", 2, 14);
    putString("\\         ___", 2, 38);
    putString("/", 3, 13);
    putString("\\ ___   /   \\      ___", 3, 39);
    putString("/            RIP            \\   \\  :   :     /   \\", 4, 12);
    putString("/", 5, 11);
    putString("\\  : _;,,,;_    :   :", 5, 41);
    (void) sprintf(str, "/%s\\,;_          _;,,,;_", centerString(tmp_str, py.misc.name));
    putString(str, 6, 10);
    putString("|               the               |   ___", 7, 9);
    if (!total_winner) {
        p = playerTitle();
    } else {
        p = (char *) "Magnificent";
    }
    (void) sprintf(str, "| %s |  /   \\", centerString(tmp_str, p));
    putString(str, 8, 9);
    putString("|", 9, 9);
    putString("|  :   :", 9, 43);
    if (!total_winner) {
        p = (char *) classes[py.misc.class_id].title;
    } else if (playerIsMale()) {
        p = (char *) "*King*";
    } else {
        p = (char *) "*Queen*";
    }
    (void) sprintf(str, "| %s | _;,,,;_   ____", centerString(tmp_str, p));
    putString(str, 10, 9);
    (void) sprintf(str, "Level : %d", (int) py.misc.level);
    (void) sprintf(str, "| %s |          /    \\", centerString(tmp_str, str));
    putString(str, 11, 9);
    (void) sprintf(str, "%d Exp", py.misc.exp);
    (void) sprintf(str, "| %s |          :    :", centerString(tmp_str, str));
    putString(str, 12, 9);
    (void) sprintf(str, "%d Au", py.misc.au);
    (void) sprintf(str, "| %s |          :    :", centerString(tmp_str, str));
    putString(str, 13, 9);
    (void) sprintf(str, "Died on Level : %d", current_dungeon_level);
    (void) sprintf(str, "| %s |         _;,,,,;_", centerString(tmp_str, str));
    putString(str, 14, 9);
    putString("|            killed by            |", 15, 9);
    p = character_died_from;

    auto len = (int) strlen(p);
    p[len] = '.'; // add a trailing period
    p[len + 1] = '\0';
    (void) sprintf(str, "| %s |", centerString(tmp_str, p));
    putString(str, 16, 9);
    p[len] = '\0'; // strip off the period

    char day[11];
    humanDateString(day);
    (void) sprintf(str, "| %s |", centerString(tmp_str, day));

    putString(str, 17, 9);
    putString("*|   *     *     *    *   *     *  | *", 18, 8);
    putString("________)/\\\\_)_/___(\\/___(//_\\)/_\\//__\\\\(/_|_)_______", 19, 0);

    retry:
    flushInputBuffer();

    putString("(ESC to abort, return to print on screen, or file name)", 23, 0);
    putString("Character record?", 22, 0);

    if (getStringInput(str, 22, 18, 60)) {
        for (auto &item : inventory) {
            itemSetAsIdentified(&item);
            spellItemIdentifyAndRemoveRandomInscription(&item);
        }

        playerRecalculateBonuses();

        if (str[0] != 0) {
            if (!outputPlayerCharacterToFile(str)) {
                goto retry;
            }
        } else {
            clearScreen();
            printCharacter();
            putString("Type ESC to skip the inventory:", 23, 0);
            if (getKeyInput() != ESCAPE) {
                clearScreen();
                printMessage("You are using:");
                (void) displayEquipment(true, 0);
                printMessage(CNIL);
                printMessage("You are carrying:");
                clearToBottom(1);
                (void) displayInventory(0, inventory_count - 1, true, 0, CNIL);
                printMessage(CNIL);
            }
        }
    }
}

// Calculates the total number of points earned -JWT-
int32_t playerCalculateTotalPoints() {
    int32_t total = py.misc.max_exp + (100 * py.misc.max_dungeon_depth);
    total += py.misc.au / 100;

    for (auto &item : inventory) {
        total += storeItemValue(&item);
    }

    total += current_dungeon_level * 50;

    // Don't ever let the score decrease from one save to the next.
    if (character_max_score > total) {
        return character_max_score;
    }

    return total;
}

static uint8_t highScoreGenderLabel() {
    if (playerIsMale()) {
        return 'M';
    }
    return 'F';
}

// Enters a players name on the top twenty list -JWT-
static void highscores() {
    clearScreen();

    if (noscore != 0) {
        return;
    }

    if (panic_save) {
        printMessage("Sorry, scores for games restored from panic save files are not saved.");
        return;
    }

    HighScore_t new_entry{};
    new_entry.points = playerCalculateTotalPoints();
    new_entry.birth_date = py.misc.date_of_birth;
    new_entry.uid = 0; // NOTE: do we not want to use `getuid()`? -MRC-
    new_entry.mhp = py.misc.max_hp;
    new_entry.chp = py.misc.current_hp;
    new_entry.dungeon_depth = (uint8_t) current_dungeon_level;
    new_entry.level = (uint8_t) py.misc.level;
    new_entry.deepest_dungeon_depth = (uint8_t) py.misc.max_dungeon_depth;
    new_entry.gender = highScoreGenderLabel();
    new_entry.race = py.misc.race_id;
    new_entry.character_class = py.misc.class_id;
    (void) strcpy(new_entry.name, py.misc.name);

    char *tmp = character_died_from;
    if ('a' == *tmp) {
        if ('n' == *(++tmp)) {
            tmp++;
        }
        while (isspace(*tmp) != 0) {
            tmp++;
        }
    }
    (void) strcpy(new_entry.died_from, tmp);

    if ((highscore_fp = fopen(MORIA_SCORES, "rb+")) == nullptr) {
        char msg[100];

        (void) sprintf(msg, "Error opening score file \"%s\"\n", MORIA_SCORES);
        printMessage(msg);
        printMessage(CNIL);
        return;
    }

    // Search file to find where to insert this character, if uid != 0 and
    // find same uid/gender/race/class combo then exit without saving this score.
    // Seek to the beginning of the file just to be safe.
    (void) fseek(highscore_fp, (long) 0, SEEK_SET);

    // Read version numbers from the score file, and check for validity.
    auto version_maj = (uint8_t) getc(highscore_fp);
    auto version_min = (uint8_t) getc(highscore_fp);
    auto patch_level = (uint8_t) getc(highscore_fp);

    // If this is a new score file, it should be empty.
    // Write the current version numbers to the score file.
    if (feof(highscore_fp) != 0) {
        // Seek to the beginning of the file just to be safe.
        (void) fseek(highscore_fp, (long) 0, SEEK_SET);

        (void) putc(CURRENT_VERSION_MAJOR, highscore_fp);
        (void) putc(CURRENT_VERSION_MINOR, highscore_fp);
        (void) putc(CURRENT_VERSION_PATCH, highscore_fp);

        // must fseek() before can change read/write mode
        (void) fseek(highscore_fp, (long) 0, SEEK_CUR);
    } else if (!validGameVersion(version_maj, version_min, patch_level)) {
        // No need to print a message, a subsequent call to
        // showScoresScreen() will print a message.
        (void) fclose(highscore_fp);
        return;
    }

    // set the static fileptr in save.c to the high score file pointer
    setFileptr(highscore_fp);

    HighScore_t old_entry{};
    HighScore_t entry{};

    int i = 0;
    off_t curpos = ftell(highscore_fp);
    readHighScore(&old_entry);

    while (feof(highscore_fp) == 0) {
        if (new_entry.points >= old_entry.points) {
            break;
        }

        // under unix, only allow one gender/race/class combo per person,
        // on single user system, allow any number of entries, but try to
        // prevent multiple entries per character by checking for case when
        // birthdate/gender/race/class are the same, and character_died_from
        // of score file entry is "(saved)"
        if (((new_entry.uid != 0 && new_entry.uid == old_entry.uid) ||
             (new_entry.uid == 0 && (strcmp(old_entry.died_from, "(saved)") == 0) && new_entry.birth_date == old_entry.birth_date)) &&
            new_entry.gender == old_entry.gender &&
            new_entry.race == old_entry.race &&
            new_entry.character_class == old_entry.character_class) {
            (void) fclose(highscore_fp);
            return;
        }

        // only allow one thousand scores in the score file
        if (++i >= MAX_HIGH_SCORE_ENTRIES) {
            (void) fclose(highscore_fp);
            return;
        }

        curpos = ftell(highscore_fp);
        readHighScore(&old_entry);
    }

    if (feof(highscore_fp) != 0) {
        // write out new_entry at end of file
        (void) fseek(highscore_fp, curpos, SEEK_SET);

        saveHighScore(&new_entry);
    } else {
        entry = new_entry;

        while (feof(highscore_fp) == 0) {
            (void) fseek(highscore_fp, -(long) sizeof(HighScore_t) - (long) sizeof(char), SEEK_CUR);

            saveHighScore(&entry);

            // under unix, only allow one gender/race/class combo per person,
            // on single user system, allow any number of entries, but try to
            // prevent multiple entries per character by checking for case when
            // birth_date/gender/race/class are the same, and character_died_from
            // of score file entry is "(saved)"
            if (((new_entry.uid != 0 && new_entry.uid == old_entry.uid) ||
                 (new_entry.uid == 0 && (strcmp(old_entry.died_from, "(saved)") == 0) && new_entry.birth_date == old_entry.birth_date)) &&
                new_entry.gender == old_entry.gender &&
                new_entry.race == old_entry.race &&
                new_entry.character_class == old_entry.character_class) {
                break;
            }
            entry = old_entry;

            // must fseek() before can change read/write mode
            (void) fseek(highscore_fp, (long) 0, SEEK_CUR);

            curpos = ftell(highscore_fp);
            readHighScore(&old_entry);
        }
        if (feof(highscore_fp) != 0) {
            (void) fseek(highscore_fp, curpos, SEEK_SET);

            saveHighScore(&entry);
        }
    }

    (void) fclose(highscore_fp);
}

// Change the player into a King! -RAK-
static void kingly() {
    const char *p = nullptr;

    // Change the character attributes.
    current_dungeon_level = 0;
    (void) strcpy(character_died_from, "Ripe Old Age");

    (void) spellRestorePlayerLevels();

    py.misc.level += PLAYER_MAX_LEVEL;
    py.misc.au += 250000L;
    py.misc.max_exp += 5000000L;
    py.misc.exp = py.misc.max_exp;

    // Let the player know that he did good.
    clearScreen();
    putString("#", 1, 34);
    putString("#####", 2, 32);
    putString("#", 3, 34);
    putString(",,,  $$$  ,,,", 4, 28);
    putString(",,=$   \"$$$$$\"   $=,,", 5, 24);
    putString(",$$        $$$        $$,", 6, 22);
    putString("*>         <*>         <*", 7, 22);
    putString("$$         $$$         $$", 8, 22);
    putString("\"$$        $$$        $$\"", 9, 22);
    putString("\"$$       $$$       $$\"", 10, 23);
    p = "*#########*#########*";
    putString(p, 11, 24);
    putString(p, 12, 24);
    putString("Veni, Vidi, Vici!", 15, 26);
    putString("I came, I saw, I conquered!", 16, 21);
    if (playerIsMale()) {
        putString("All Hail the Mighty King!", 17, 22);
    } else {
        putString("All Hail the Mighty Queen!", 17, 22);
    }
    flushInputBuffer();
    waitForContinueKey(23);
}

// What happens upon dying -RAK-
// Handles the gravestone and top-twenty routines -RAK-
[[noreturn]] void exitGame() {
    printMessage(CNIL);

    // flush all input
    flushInputBuffer();

    // If the game has been saved, then save sets turn back to -1,
    // which inhibits the printing of the tomb.
    if (current_game_turn >= 0) {
        if (total_winner) {
            kingly();
        }
        printTomb();
    }

    if (character_generated && !character_saved) {
        // Save the memory at least.
        (void) saveGame();
    }

    // add score to score file if applicable
    if (character_generated) {
        // Clear character_saved, strange thing to do, but it prevents
        // getKeyInput() from recursively calling exitGame() when there has
        // been an eof on stdin detected.
        character_saved = false;
        highscores();
        showScoresScreen();
    }
    eraseLine(23, 0);
    terminalRestore();

    exit(0);
}
