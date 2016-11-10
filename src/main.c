// src/main.c: initialization, main() function and main loop
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

static void char_inven_init();
static void init_m_level();
static void init_t_level();

#if (COST_ADJ != 100)
static void price_adjust();
#endif

// Initialize, restore, and get the ball rolling. -RAK-
int main(int argc, char *argv[]) {
    bool new_game = false;
    bool force_rogue_like = false;
    bool force_keys_to = false;

    // default command set defined in config.h file
    rogue_like_commands = ROGUE_LIKE;

    // call this routine to grab a file pointer to the highscore file
    // and prepare things to relinquish setuid privileges
    init_scorefile();

    // Check the user permissions -MRC-
    check_file_permissions();

    // use curses
    init_curses();

    uint32_t seed = 0; // let wizard specify rng seed
    // check for user interface option
    for (--argc, ++argv; argc > 0 && argv[0][0] == '-'; --argc, ++argv) {
        switch (argv[0][1]) {
        case 'N': case 'n':
            new_game = true;
            break;
        case 'O': case 'o':
            // rogue_like_commands may be set in get_char(),
            // so delay this until after read savefile if any.
            force_rogue_like = true;
            force_keys_to = false;
            break;
        case 'R': case 'r':
            force_rogue_like = true;
            force_keys_to = true;
            break;
        case 'S':
            display_scores(true);
            exit_game();
        case 's':
            display_scores(false);
            exit_game();
        case 'W': case 'w':
            to_be_wizard = true;

            if (isdigit((int)argv[0][2])) {
                seed = (uint32_t)atoi(&argv[0][2]);
            }
            break;
        default:
            (void)printf("Usage: moria [-norsw] [savefile]\n");
            exit_game();
        }
    }

    // Show intro text
    read_times();

    // Some necessary initializations
    // all made into constants or initialized in variables.c

    #if (COST_ADJ != 100)
        price_adjust();
    #endif

    // Grab a random seed from the clock
    init_seeds(seed);

    // Init monster and treasure levels for allocate
    init_m_level();
    init_t_level();

    // Init the store inventories
    store_init();

    // If -n is not passed, the calling routine will know
    // savefile name, hence, this code is not necessary.

    // Auto-restart of saved file
    char *p;
    if (argv[0] != CNIL) {
        (void)strcpy(savefile, argv[0]);
    } else if ((p = getenv("MORIA_SAV")) != CNIL) {
        (void)strcpy(savefile, p);
    } else {
        (void)strcpy(savefile, MORIA_SAV);
    }

    // This restoration of a saved character may get ONLY the monster memory. In
    // this case, get_char returns false. It may also resurrect a dead character
    // (if you are the wizard). In this case, it returns true, but also sets the
    // parameter "generate" to true, as it does not recover any cave details.

    bool result = false;
    bool generate = false;

    if ((new_game == false) && !access(savefile, 0) && get_char(&generate)) {
        result = true;
    }

    // enter wizard mode before showing the character display, but must wait
    // until after get_char in case it was just a resurrection
    if (to_be_wizard) {
        if (!enter_wiz_mode()) {
            exit_game();
        }
    }

    if (result) {
        change_name();

        // could be restoring a dead character after a signal or HANGUP
        if (py.misc.chp < 0) {
            death = true;
        }
    } else { // Create character
        create_character();

        birth_date = (int32_t)time((time_t *)0);

        char_inven_init();
        py.flags.food = 7500;
        py.flags.food_digested = 2;

        if (class[py.misc.pclass].spell == MAGE) {
            // Magic realm
            clear_screen(); // makes spell list easier to read
            calc_spells(A_INT);
            calc_mana(A_INT);
        } else if (class[py.misc.pclass].spell == PRIEST) {
            // Clerical realm
            calc_spells(A_WIS);
            clear_screen(); // force out the 'learn prayer' message
            calc_mana(A_WIS);
        }

        // prevent ^c quit from entering score into scoreboard,
        // and prevent signal from creating panic save until this
        // point, all info needed for save file is now valid.
        character_generated = true;
        generate = true;
    }

    if (force_rogue_like) {
        rogue_like_commands = force_keys_to;
    }

    magic_init();

    // Begin the game
    clear_screen();
    prt_stat_block();
    if (generate) {
        generate_cave();
    }

    // Loop till dead, or exit
    while (!death) {
        dungeon(); // Dungeon logic

        // check for eof here, see inkey() in io.c
        // eof can occur if the process gets a HANGUP signal
        if (eof_flag) {
            (void)strcpy(died_from, "(end of input: saved)");
            if (!save_char()) {
                (void)strcpy(died_from, "unexpected eof");
            }

            // should not reach here, but if we do, this guarantees exit
            death = true;
        }

        if (!death) {
            // New level
            generate_cave();
        }
    }

    exit_game(); // Character gets buried.

    // should never reach here, but just in case
    return 0;
}

// Init players with some belongings -RAK-
static void char_inven_init() {
    int i, j;
    inven_type inven_init;

    // this is needed for bash to work right, it can't hurt anyway
    for (i = 0; i < INVEN_ARRAY_SIZE; i++) {
        invcopy(&inventory[i], OBJ_NOTHING);
    }

    for (i = 0; i < 5; i++) {
        j = player_init[py.misc.pclass][i];
        invcopy(&inven_init, j);
        // this makes it known2 and known1
        store_bought(&inven_init);
        // must set this bit to display tohit/todam for stiletto
        if (inven_init.tval == TV_SWORD) {
            inven_init.ident |= ID_SHOW_HITDAM;
        }
        (void)inven_carry(&inven_init);
    }

    // wierd place for it, but why not?
    for (i = 0; i < 32; i++) {
        spell_order[i] = 99;
    }
}

// Initializes M_LEVEL array for use with PLACE_MONSTER -RAK-
static void init_m_level() {
    for (int i = 0; i <= MAX_MONS_LEVEL; i++) {
        m_level[i] = 0;
    }

    int k = MAX_CREATURES - WIN_MON_TOT;
    for (int i = 0; i < k; i++) {
        m_level[c_list[i].level]++;
    }

    for (int i = 1; i <= MAX_MONS_LEVEL; i++) {
        m_level[i] += m_level[i - 1];
    }
}

// Initializes T_LEVEL array for use with PLACE_OBJECT -RAK-
static void init_t_level() {
    for (int i = 0; i <= MAX_OBJ_LEVEL; i++) {
        t_level[i] = 0;
    }

    for (int i = 0; i < MAX_DUNGEON_OBJ; i++) {
        t_level[object_list[i].level]++;
    }

    for (int i = 1; i <= MAX_OBJ_LEVEL; i++) {
        t_level[i] += t_level[i - 1];
    }

    // now produce an array with object indexes sorted by level,
    // by using the info in t_level, this is an O(n) sort!
    // this is not a stable sort, but that does not matter
    int tmp[MAX_OBJ_LEVEL + 1];

    for (int i = 0; i <= MAX_OBJ_LEVEL; i++) {
        tmp[i] = 1;
    }

    for (int i = 0; i < MAX_DUNGEON_OBJ; i++) {
        int l = object_list[i].level;
        sorted_objects[t_level[l] - tmp[l]] = i;
        tmp[l]++;
    }
}

#if (COST_ADJ != 100)
// Adjust prices of objects -RAK-
static void price_adjust() {
    // round half-way cases up
    for (int i = 0; i < MAX_OBJECTS; i++) {
        object_list[i].cost = ((object_list[i].cost * COST_ADJ) + 50) / 100;
    }
}
#endif

// Check user permissions on Unix based systems,
// or if on Windows just return. -MRC-
void check_file_permissions() {
#ifndef _WIN32
    if (0 != setuid(getuid())) {
        perror("Can't set permissions correctly!  Setuid call failed.\n");
        exit(0);
    }
    if (0 != setgid(getgid())) {
        perror("Can't set permissions correctly!  Setgid call failed.\n");
        exit(0);
    }
#endif
}
