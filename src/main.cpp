// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Initialization, main() function and main loop

#include "headers.h"
#include "externs.h"
#include "version.h"

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

    // call this routine to grab a file pointer to the high score file
    // and prepare things to relinquish setuid privileges
    init_scorefile();

    // Make sure we have access to all files -MRC-
    check_file_permissions();

    init_curses();

    uint32_t seed = 0; // let wizard specify rng seed

    // check for user interface option
    for (--argc, ++argv; argc > 0 && argv[0][0] == '-'; --argc, ++argv) {
        switch (argv[0][1]) {
            case 'v':
                restore_term();
                printf("%d.%d.%d\n", CURRENT_VERSION_MAJOR, CURRENT_VERSION_MINOR, CURRENT_VERSION_PATCH);
                return 0;
            case 'n':
                new_game = true;
                break;
            case 'o':
                // rogue_like_commands may be set in get_char(),
                // so delay this until after read save file if any.
                force_rogue_like = true;
                force_keys_to = false;
                break;
            case 'r':
                force_rogue_like = true;
                force_keys_to = true;
                break;
            case 's':
                display_scores();
                exit_game();
            case 'w':
                to_be_wizard = true;

                if (isdigit((int) argv[0][2])) {
                    seed = (uint32_t) atoi(&argv[0][2]);
                }
                break;
            default:
                restore_term();
                printf("Robert A. Koeneke's classic dungeon crawler.\n");
                printf("Umoria %d.%d.%d is released under a GPL v2 license.\n", CURRENT_VERSION_MAJOR, CURRENT_VERSION_MINOR, CURRENT_VERSION_PATCH);
                printf("\n");
                printf("Usage:\n");
                printf("    umoria [OPTIONS] SAVEGAME\n");
                printf("\n");
                printf("SAVEGAME is an optional save game filename (default: game.sav)\n");
                printf("\n");
                printf("Options:\n");
                printf("    -n    Force start of new game\n");
                printf("    -o    Use classic roguelike keys: hjkl (save game overrides)\n");
                printf("    -r    Force use of roguelike keys\n");
                printf("    -s    Display high scores and exit\n");
                printf("    -w    Enter Wizard mode!\n");
                printf("\n");
                printf("    -v    Print version info and exit\n");
                printf("    -h    Display this message\n");
                return 0;
        }
    }

    // Show the game splash screen
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
    // save file name, hence, this code is not necessary.

    // Auto-restart of saved file
    char *p;
    if (argv[0] != CNIL) {
        (void) strcpy(savefile, argv[0]);
    } else if ((p = getenv("MORIA_SAV")) != CNIL) {
        (void) strcpy(savefile, p);
    } else {
        (void) strcpy(savefile, MORIA_SAV);
    }

    // This restoration of a saved character may get ONLY the monster memory. In
    // this case, get_char returns false. It may also resurrect a dead character
    // (if you are the wizard). In this case, it returns true, but also sets the
    // parameter "generate" to true, as it does not recover any cave details.

    bool result = false;
    bool generate = false;

    if (!new_game && !access(savefile, 0) && get_char(&generate)) {
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

        birth_date = (int32_t) time((time_t *) 0);

        char_inven_init();
        py.flags.food = 7500;
        py.flags.food_digested = 2;

        // Spell and Mana based on class: Mage or Clerical realm.
        if (classes[py.misc.pclass].spell == MAGE) {
            clear_screen(); // makes spell list easier to read
            calc_spells(A_INT);
            calc_mana(A_INT);
        } else if (classes[py.misc.pclass].spell == PRIEST) {
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

    //
    // Begin the game
    //
    clear_screen();
    prt_stat_block();

    if (generate) {
        generate_cave();
    }

    // Loop till dead, or exit
    while (!death) {
        // Dungeon logic
        dungeon();

        // check for eof here, see inkey() in io.c
        // eof can occur if the process gets a HANGUP signal
        if (eof_flag) {
            (void) strcpy(died_from, "(end of input: saved)");
            if (!save_char()) {
                (void) strcpy(died_from, "unexpected eof");
            }

            // should not reach here, but if we do, this guarantees exit
            death = true;
        }

        // New level if not dead
        if (!death) {
            generate_cave();
        }
    }

    // Character gets buried.
    exit_game();

    // should never reach here, but just in case
    return 0;
}

// Init players with some belongings -RAK-
static void char_inven_init() {
    inven_type inven_init;

    // this is needed for bash to work right, it can't hurt anyway
    for (int i = 0; i < INVEN_ARRAY_SIZE; i++) {
        invcopy(&inventory[i], OBJ_NOTHING);
    }

    for (int i = 0; i < 5; i++) {
        invcopy(&inven_init, player_init[py.misc.pclass][i]);

        // this makes it known2 and known1
        store_bought(&inven_init);

        // must set this bit to display tohit/todam for stiletto
        if (inven_init.tval == TV_SWORD) {
            inven_init.ident |= ID_SHOW_HITDAM;
        }

        (void) inven_carry(&inven_init);
    }

    // weird place for it, but why not?
    for (int i = 0; i < 32; i++) {
        spell_order[i] = 99;
    }
}

// Initializes M_LEVEL array for use with PLACE_MONSTER -RAK-
static void init_m_level() {
    for (int i = 0; i <= MAX_MONS_LEVEL; i++) {
        m_level[i] = 0;
    }

    for (int i = 0; i < MAX_CREATURES - WIN_MON_TOT; i++) {
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
    int objectIndexes[MAX_OBJ_LEVEL + 1];
    for (int i = 0; i <= MAX_OBJ_LEVEL; i++) {
        objectIndexes[i] = 1;
    }

    for (int i = 0; i < MAX_DUNGEON_OBJ; i++) {
        int level = object_list[i].level;
        int objectID = t_level[level] - objectIndexes[level];
        sorted_objects[objectID] = (int16_t) i;
        objectIndexes[level]++;
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
