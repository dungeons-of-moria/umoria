// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// Handle reading, writing, and displaying of high scores.

#include "headers.h"
#include "version.h"

// High score file pointer
FILE *highscore_fp;

static uint8_t highScoreGenderLabel() {
    if (playerIsMale()) {
        return 'M';
    }
    return 'F';
}

// Enters a players name on the top twenty list -JWT-
void recordNewHighScore() {
    clearScreen();

    if (game.noscore != 0) {
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
    new_entry.dungeon_depth = (uint8_t) dg.current_level;
    new_entry.level = (uint8_t) py.misc.level;
    new_entry.deepest_dungeon_depth = (uint8_t) py.misc.max_dungeon_depth;
    new_entry.gender = highScoreGenderLabel();
    new_entry.race = py.misc.race_id;
    new_entry.character_class = py.misc.class_id;
    (void) strcpy(new_entry.name, py.misc.name);

    char *tmp = game.character_died_from;
    if ('a' == *tmp) {
        if ('n' == *(++tmp)) {
            tmp++;
        }
        while (isspace(*tmp) != 0) {
            tmp++;
        }
    }
    (void) strcpy(new_entry.died_from, tmp);

    if ((highscore_fp = fopen(config::files::scores.c_str(), "rb+")) == nullptr) {
        printMessage(("Error opening score file '" + config::files::scores + "'.").c_str());
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
    readHighScore(old_entry);

    while (feof(highscore_fp) == 0) {
        if (new_entry.points >= old_entry.points) {
            break;
        }

        // under unix, only allow one gender/race/class combo per person,
        // on single user system, allow any number of entries, but try to
        // prevent multiple entries per character by checking for case when
        // birthdate/gender/race/class are the same, and game.character_died_from
        // of score file entry is "(saved)"
        if (((new_entry.uid != 0 && new_entry.uid == old_entry.uid) ||
             (new_entry.uid == 0 && (strcmp(old_entry.died_from, "(saved)") == 0) && new_entry.birth_date == old_entry.birth_date)) &&
            new_entry.gender == old_entry.gender && new_entry.race == old_entry.race && new_entry.character_class == old_entry.character_class) {
            (void) fclose(highscore_fp);
            return;
        }

        // only allow one thousand scores in the score file
        i++;
        if (i >= MAX_HIGH_SCORE_ENTRIES) {
            (void) fclose(highscore_fp);
            return;
        }

        curpos = ftell(highscore_fp);
        readHighScore(old_entry);
    }

    if (feof(highscore_fp) != 0) {
        // write out new_entry at end of file
        (void) fseek(highscore_fp, curpos, SEEK_SET);

        saveHighScore(new_entry);
    } else {
        entry = new_entry;

        while (feof(highscore_fp) == 0) {
            (void) fseek(highscore_fp, -(long) sizeof(HighScore_t) - (long) sizeof(char), SEEK_CUR);

            saveHighScore(entry);

            // under unix, only allow one gender/race/class combo per person,
            // on single user system, allow any number of entries, but try to
            // prevent multiple entries per character by checking for case when
            // birth_date/gender/race/class are the same, and game.character_died_from
            // of score file entry is "(saved)"
            if (((new_entry.uid != 0 && new_entry.uid == old_entry.uid) ||
                 (new_entry.uid == 0 && (strcmp(old_entry.died_from, "(saved)") == 0) && new_entry.birth_date == old_entry.birth_date)) &&
                new_entry.gender == old_entry.gender && new_entry.race == old_entry.race && new_entry.character_class == old_entry.character_class) {
                break;
            }
            entry = old_entry;

            // must fseek() before can change read/write mode
            (void) fseek(highscore_fp, (long) 0, SEEK_CUR);

            curpos = ftell(highscore_fp);
            readHighScore(old_entry);
        }
        if (feof(highscore_fp) != 0) {
            (void) fseek(highscore_fp, curpos, SEEK_SET);

            saveHighScore(entry);
        }
    }

    (void) fclose(highscore_fp);
}

void showScoresScreen() {
    if ((highscore_fp = fopen(config::files::scores.c_str(), "rb")) == nullptr) {
        printMessage(("Error opening score file '" + config::files::scores + "'.").c_str());
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
    readHighScore(score);

    char input;
    char msg[100];

    int i = 0;
    int rank = 1;

    while (feof(highscore_fp) == 0) {
        i = 1;
        clearScreen();
        // Put twenty scores on each page, on lines 2 through 21.
        while ((feof(highscore_fp) == 0) && i < 21) {
            (void) sprintf(msg,                                               //
                           "%-4d%8d %-19.19s %c %-10.10s %-7.7s%3d %-22.22s", //
                           rank,                                              //
                           score.points,                                      //
                           score.name,                                        //
                           score.gender,                                      //
                           character_races[score.race].name,                  //
                           classes[score.character_class].title,              //
                           score.level,                                       //
                           score.died_from                                    //
            );
            i++;
            putStringClearToEOL(msg, Coord_t{i, 0});
            rank++;
            readHighScore(score);
        }
        putStringClearToEOL("Rank  Points Name              Sex Race       Class  Lvl Killed By", Coord_t{0, 0});
        eraseLine(Coord_t{1, 0});
        putStringClearToEOL("[ press any key to continue ]", Coord_t{23, 23});
        input = getKeyInput();
        if (input == ESCAPE) {
            break;
        }
    }

    (void) fclose(highscore_fp);
}

// Calculates the total number of points earned -JWT-
int32_t playerCalculateTotalPoints() {
    int32_t total = py.misc.max_exp + (100 * py.misc.max_dungeon_depth);
    total += py.misc.au / 100;

    for (auto &item : py.inventory) {
        total += storeItemValue(item);
    }

    total += dg.current_level * 50;

    // Don't ever let the score decrease from one save to the next.
    if (py.max_score > total) {
        return py.max_score;
    }

    return total;
}
