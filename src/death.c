// src/death.c: code executed when player dies
//
// Copyright (C) 1989-2008 James E. Wilson, Robert A. Koeneke,
//                         David J. Grabiner
//
// This file is part of Umoria.
//
// Umoria is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Umoria is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Umoria.  If not, see <http://www.gnu.org/licenses/>.

#include "standard_library.h"

#include "config.h"
#include "constant.h"
#include "types.h"

#include "externs.h"

#if defined(USG)
#ifndef L_SET
#define L_SET 0
#endif
#ifndef L_INCR
#define L_INCR 1
#endif
#endif

static void date(char *day) {
    char *tmp;
    long clockvar = time((time_t *)0);
    tmp = ctime(&clockvar);
    tmp[10] = '\0';
    (void)strcpy(day, tmp);
}

// Centers a string within a 31 character string -JWT-
static char *center_string(char *centered_str, char *in_str) {
    int i = (int)strlen(in_str);
    int j = 15 - i / 2;
    (void)sprintf(centered_str, "%*s%s%*s", j, "", in_str, 31 - i - j, "");
    return centered_str;
}

#if defined(USG)
// The following code is provided especially for systems which -CJS-
// have no flock system call. It has never been tested.

// DEBIAN_LINUX defined because fcntlbits.h defines EX and SH the other way. -RJW-
// The comment below indicates that they're not distinguished anyways, so
// this should be harmless, and this does seem to be the prevailing order
// (c.f. IRIX 6.5) but just in case, they've been ifdef'ed.
#ifdef DEBIAN_LINUX
#define LOCK_SH 1
#define LOCK_EX 2
#else // DEBIAN_LINUX
#define LOCK_EX 1
#define LOCK_SH 2
#endif // DEBIAN_LINUX
#define LOCK_NB 4
#define LOCK_UN 8

// An flock HACK.  LOCK_SH and LOCK_EX are not distinguished. DO NOT release
// a lock which you failed to set!  ALWAYS release a lock you set!
int flock(int f, int l) {
    struct stat sbuf;
    if (fstat(f, &sbuf) < 0) {
        return -1;
    }

    char lockname[80];
#ifdef __linux__
    (void)sprintf(lockname, "/tmp/moria.%ld", sbuf.st_ino);
#else
    (void)sprintf(lockname, "/tmp/moria.%d", sbuf.st_ino);
#endif

    if (l & LOCK_UN) {
        return unlink(lockname);
    }

    while (open(lockname, O_WRONLY | O_CREAT | O_EXCL, 0644) < 0) {
        if (errno != EEXIST) {
            return -1;
        }
        if (stat(lockname, &sbuf) < 0) {
            return -1;
        }
        // Locks which last more than 10 seconds get deleted.
        if (time((time_t *)0) - sbuf.st_mtime > 10) {
            if (unlink(lockname) < 0) {
                return -1;
            }
        } else if (l & LOCK_NB) {
            return -1;
        } else {
            (void)sleep(1);
        }
    }
    return 0;
}
#endif

void display_scores(int show_player) {
    (void)fseek(highscore_fp, (off_t)0, L_SET);

    // Read version numbers from the score file, and check for validity.
    uint8_t version_maj = getc(highscore_fp);
    uint8_t version_min = getc(highscore_fp);
    uint8_t patch_level = getc(highscore_fp);

    // Support score files from 5.2.2 to present.
    if (feof(highscore_fp)) {
        ; // An empty score file.
    } else if ((version_maj != CUR_VERSION_MAJ) ||
               (version_min > CUR_VERSION_MIN) ||
               (version_min == CUR_VERSION_MIN && patch_level > PATCH_LEVEL) ||
               (version_min == 2 && patch_level < 2) || (version_min < 2)) {
        msg_print("Sorry. This scorefile is from a different version of umoria.");
        msg_print(CNIL);

        return;
    }

    int16_t player_uid = getuid();

    // set the static fileptr in save.c to the highscore file pointer
    set_fileptr(highscore_fp);

    high_scores score;
    rd_highscore(&score);

    char input;
    char string[100];

    int i = 0;
    int rank = 1;

    while (!feof(highscore_fp)) {
        i = 1;
        clear_screen();
        // Put twenty scores on each page, on lines 2 through 21.
        while (!feof(highscore_fp) && i < 21) {
            // Only show the entry if show_player false, or

            // if the entry belongs to the current player.
            if (!show_player || score.uid == player_uid) {
                (void)sprintf(string,
                              "%-4d%8d %-19.19s %c %-10.10s %-7.7s%3d %-22.22s",
                              rank, score.points, score.name, score.sex,
                              race[score.race].trace, class[score.class].title,
                              score.lev, score.died_from);
                prt(string, ++i, 0);
            }
            rank++;
            rd_highscore(&score);
        }
        prt("Rank  Points Name              Sex Race       Class  Lvl Killed ""By", 0, 0);
        erase_line(1, 0);
        prt("[Press any key to continue.]", 23, 23);
        input = inkey();
        if (input == ESCAPE) {
            break;
        }
    }
}

bool duplicate_character() {
    // Only check for duplicate characters under unix.
    return false;
}

// Prints the gravestone of the character -RAK-
static void print_tomb() {
    char *p;
    vtype str, tmp_str;

    clear_screen();
    put_buffer("_______________________", 1, 15);
    put_buffer("/", 2, 14);
    put_buffer("\\         ___", 2, 38);
    put_buffer("/", 3, 13);
    put_buffer("\\ ___   /   \\      ___", 3, 39);
    put_buffer("/            RIP            \\   \\  :   :     /   \\", 4, 12);
    put_buffer("/", 5, 11);
    put_buffer("\\  : _;,,,;_    :   :", 5, 41);
    (void)sprintf(str, "/%s\\,;_          _;,,,;_", center_string(tmp_str, py.misc.name));
    put_buffer(str, 6, 10);
    put_buffer("|               the               |   ___", 7, 9);
    if (!total_winner) {
        p = title_string();
    } else {
        p = "Magnificent";
    }
    (void)sprintf(str, "| %s |  /   \\", center_string(tmp_str, p));
    put_buffer(str, 8, 9);
    put_buffer("|", 9, 9);
    put_buffer("|  :   :", 9, 43);
    if (!total_winner) {
        p = class[py.misc.pclass].title;
    } else if (py.misc.male) {
        p = "*King*";
    } else {
        p = "*Queen*";
    }
    (void)sprintf(str, "| %s | _;,,,;_   ____", center_string(tmp_str, p));
    put_buffer(str, 10, 9);
    (void)sprintf(str, "Level : %d", (int)py.misc.lev);
    (void)sprintf(str, "| %s |          /    \\", center_string(tmp_str, str));
    put_buffer(str, 11, 9);
    (void)sprintf(str, "%d Exp", py.misc.exp);
    (void)sprintf(str, "| %s |          :    :", center_string(tmp_str, str));
    put_buffer(str, 12, 9);
    (void)sprintf(str, "%d Au", py.misc.au);
    (void)sprintf(str, "| %s |          :    :", center_string(tmp_str, str));
    put_buffer(str, 13, 9);
    (void)sprintf(str, "Died on Level : %d", dun_level);
    (void)sprintf(str, "| %s |         _;,,,,;_", center_string(tmp_str, str));
    put_buffer(str, 14, 9);
    put_buffer("|            killed by            |", 15, 9);
    p = died_from;

    int len = (int)strlen(p);
    p[len] = '.'; // add a trailing period
    p[len + 1] = '\0';
    (void)sprintf(str, "| %s |", center_string(tmp_str, p));
    put_buffer(str, 16, 9);
    p[len] = '\0'; // strip off the period

    char day[11];
    date(day);
    (void)sprintf(str, "| %s |", center_string(tmp_str, day));

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
                (void)show_equip(true, 0);
                msg_print(CNIL);
                msg_print("You are carrying:");
                clear_from(1);
                (void)show_inven(0, inven_ctr - 1, true, 0, CNIL);
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

    total += dun_level * 50;

    // Don't ever let the score decrease from one save to the next.
    if (max_score > total) {
        return max_score;
    }

    return total;
}

// Enters a players name on the top twenty list -JWT-
static void highscores() {
    clear_screen();

    if (noscore) {
        return;
    }

    if (panic_save == true) {
        msg_print("Sorry, scores for games restored from panic save files are not saved.");
        return;
    }

    high_scores new_entry;
    new_entry.points = total_points();
    new_entry.birth_date = birth_date;

    new_entry.uid = getuid();

    new_entry.mhp = py.misc.mhp;
    new_entry.chp = py.misc.chp;
    new_entry.dun_level = dun_level;
    new_entry.lev = py.misc.lev;
    new_entry.max_dlv = py.misc.max_dlv;
    new_entry.sex = (py.misc.male ? 'M' : 'F');
    new_entry.race = py.misc.prace;
    new_entry.class = py.misc.pclass;
    (void)strcpy(new_entry.name, py.misc.name);

    char *tmp = died_from;
    if ('a' == *tmp) {
        if ('n' == *(++tmp)) {
            tmp++;
        }
        while (isspace(*tmp)) {
            tmp++;
        }
    }
    (void)strcpy(new_entry.died_from, tmp);

    // First, get a lock on the high score file so no-one else tries to write
    // to it while we are using it, on IBMPCs only one process can have the
    // file open at a time, so we just open it here
    if (0 != flock(fileno(highscore_fp), LOCK_EX)) {
        msg_print("Error gaining lock for score file");
        msg_print(CNIL);
        return;
    }

    // Search file to find where to insert this character, if uid != 0 and
    // find same uid/sex/race/class combo then exit without saving this score.
    // Seek to the beginning of the file just to be safe.
    (void)fseek(highscore_fp, (off_t)0, L_SET);

    // Read version numbers from the score file, and check for validity.
    uint8_t version_maj = getc(highscore_fp);
    uint8_t version_min = getc(highscore_fp);
    uint8_t patch_level = getc(highscore_fp);

    // If this is a new scorefile, it should be empty.
    // Write the current version numbers to the score file.
    if (feof(highscore_fp)) {
        // Seek to the beginning of the file just to be safe.
        (void)fseek(highscore_fp, (off_t)0, L_SET);

        (void)putc(CUR_VERSION_MAJ, highscore_fp);
        (void)putc(CUR_VERSION_MIN, highscore_fp);
        (void)putc(PATCH_LEVEL, highscore_fp);

        // must fseek() before can change read/write mode
        (void)fseek(highscore_fp, (off_t)0, L_INCR);
    } else if ((version_maj != CUR_VERSION_MAJ) ||
             (version_min > CUR_VERSION_MIN) ||
             (version_min == CUR_VERSION_MIN && patch_level > PATCH_LEVEL) ||
             (version_min == 2 && patch_level < 2) || (version_min < 2))
    { // Support score files from 5.2.2 to present.
        // No need to print a message, a subsequent call to
        // display_scores() will print a message.
        return;
    }

    // set the static fileptr in save.c to the highscore file pointer
    set_fileptr(highscore_fp);

    high_scores old_entry, entry;

    int i = 0;
    off_t curpos = ftell(highscore_fp);
    rd_highscore(&old_entry);

    while (!feof(highscore_fp)) {
        if (new_entry.points >= old_entry.points) {
            break;
            // under unix, only allow one sex/race/class combo per person,
            // on single user system, allow any number of entries, but try to
            // prevent multiple entries per character by checking for case when
            // birthdate/sex/race/class are the same, and died_from of scorefile
            // entry is "(saved)"
        } else if (((new_entry.uid != 0 && new_entry.uid == old_entry.uid) ||
                    (new_entry.uid == 0 &&
                     !strcmp(old_entry.died_from, "(saved)") &&
                     new_entry.birth_date == old_entry.birth_date)) &&
                   new_entry.sex == old_entry.sex &&
                   new_entry.race == old_entry.race &&
                   new_entry.class == old_entry.class)
        {
            return;
        } else if (++i >= SCOREFILE_SIZE) {
            // only allow one thousand scores in the score file
            return;
        }
        curpos = ftell(highscore_fp);
        rd_highscore(&old_entry);
    }

    if (feof(highscore_fp)) {
        // write out new_entry at end of file
        (void)fseek(highscore_fp, (off_t)curpos, L_SET);

        wr_highscore(&new_entry);
    } else {
        entry = new_entry;
        while (!feof(highscore_fp)) {
            (void)fseek(highscore_fp, -(off_t)sizeof(high_scores) - (off_t)sizeof(char), L_INCR);

            wr_highscore(&entry);
            // under unix, only allow one sex/race/class combo per person,
            // on single user system, allow any number of entries, but try
            // to prevent multiple entries per character by checking for
            // case when birthdate/sex/race/class are the same, and died_from
            // of scorefile entry is "(saved)"
            if (((new_entry.uid != 0 && new_entry.uid == old_entry.uid) ||
                 (new_entry.uid == 0 &&
                  !strcmp(old_entry.died_from, "(saved)") &&
                  new_entry.birth_date == old_entry.birth_date)) &&
                new_entry.sex == old_entry.sex &&
                new_entry.race == old_entry.race &&
                new_entry.class == old_entry.class) {
                break;
            }
            entry = old_entry;

            // must fseek() before can change read/write mode
            (void)fseek(highscore_fp, (off_t)0, L_INCR);

            curpos = ftell(highscore_fp);
            rd_highscore(&old_entry);
        }
        if (feof(highscore_fp)) {
            (void)fseek(highscore_fp, (off_t)curpos, L_SET);
            wr_highscore(&entry);
        }
    }

    (void)flock(fileno(highscore_fp), LOCK_UN);
}

// Change the player into a King! -RAK-
static void kingly() {
    char *p;

    // Change the character attributes.
    dun_level = 0;
    (void)strcpy(died_from, "Ripe Old Age");

    struct misc *p_ptr = &py.misc;

    (void)restore_level();

    p_ptr->lev += MAX_PLAYER_LEVEL;
    p_ptr->au += 250000L;
    p_ptr->max_exp += 5000000L;
    p_ptr->exp = p_ptr->max_exp;

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
    if (p_ptr->male) {
        put_buffer("All Hail the Mighty King!", 17, 22);
    } else {
        put_buffer("All Hail the Mighty Queen!", 17, 22);
    }
    flush();
    pause_line(23);
}

// Handles the gravestone end top-twenty routines -RAK-
void exit_game() {
    // What happens upon dying. -RAK-

    msg_print(CNIL);

    flush();     // flush all input

    // If the game has been saved, then save sets turn back to -1,
    // which inhibits the printing of the tomb.
    if (turn >= 0) {
        if (total_winner) {
            kingly();
        }
        print_tomb();
    }

    if (character_generated && !character_saved) {
        // Save the memory at least.
        (void)save_char();
    }

    // add score to scorefile if applicable
    if (character_generated) {
        // Clear character_saved, strange thing to do, but it prevents
        // inkey() from recursively calling exit_game() when there has
        // been an eof on stdin detected.
        character_saved = false;
        highscores();
        display_scores(true);
    }
    erase_line(23, 0);
    restore_term();

    exit(0);
}
