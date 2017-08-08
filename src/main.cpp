// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Initialization, main() function and main loop

#include "headers.h"
#include "externs.h"
#include "version.h"

static void initializeCharacterInventory();
static void initializeMonsterLevels();
static void initializeTreasureLevels();

#if (COST_ADJ != 100)
static void price_adjust();
#endif

// Initialize, restore, and get the ball rolling. -RAK-
int main(int argc, char *argv[]) {
    bool new_game = false;
    bool force_rogue_like = false;
    bool force_keys_to = false;

    // default command set defined in config.h file
    use_roguelike_keys = ROGUE_LIKE;

    // call this routine to grab a file pointer to the high score file
    // and prepare things to relinquish setuid privileges
    initializeScoreFile();

    // Make sure we have access to all files -MRC-
    checkFilePermissions();

    terminalInitialize();

    uint32_t seed = 0; // let wizard specify rng seed

    // check for user interface option
    for (--argc, ++argv; argc > 0 && argv[0][0] == '-'; --argc, ++argv) {
        switch (argv[0][1]) {
            case 'v':
                terminalRestore();
                printf("%d.%d.%d\n", CURRENT_VERSION_MAJOR, CURRENT_VERSION_MINOR, CURRENT_VERSION_PATCH);
                return 0;
            case 'n':
                new_game = true;
                break;
            case 'o':
                // use_roguelike_keys may be set in loadGame(),
                // so delay this until after read save file if any.
                force_rogue_like = true;
                force_keys_to = false;
                break;
            case 'r':
                force_rogue_like = true;
                force_keys_to = true;
                break;
            case 's':
                showScoresScreen();
                exitGame();
            case 'w':
                to_be_wizard = true;

                if (isdigit((int) argv[0][2])) {
                    seed = (uint32_t) atoi(&argv[0][2]);
                }
                break;
            default:
                terminalRestore();
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
    displaySplashScreen();

    // Some necessary initializations
    // all made into constants or initialized in variables.c

#if (COST_ADJ != 100)
    price_adjust();
#endif

    // Grab a random seed from the clock
    seedsInitialize(seed);

    // Init monster and treasure levels for allocate
    initializeMonsterLevels();
    initializeTreasureLevels();

    // Init the store inventories
    storeInitializeOwners();

    // If -n is not passed, the calling routine will know
    // save file name, hence, this code is not necessary.

    // Auto-restart of saved file
    char *p;
    if (argv[0] != CNIL) {
        (void) strcpy(savegame_filename, argv[0]);
    } else if ((p = getenv("MORIA_SAV")) != CNIL) {
        (void) strcpy(savegame_filename, p);
    } else {
        (void) strcpy(savegame_filename, MORIA_SAV);
    }

    // This restoration of a saved character may get ONLY the monster memory. In
    // this case, loadGame() returns false. It may also resurrect a dead character
    // (if you are the wizard). In this case, it returns true, but also sets the
    // parameter "generate" to true, as it does not recover any cave details.

    bool result = false;
    bool generate = false;

    if (!new_game && !access(savegame_filename, 0) && loadGame(&generate)) {
        result = true;
    }

    // enter wizard mode before showing the character display, but must wait
    // until after loadGame() in case it was just a resurrection
    if (to_be_wizard) {
        if (!enterWizardMode()) {
            exitGame();
        }
    }

    if (result) {
        changeCharacterName();

        // could be restoring a dead character after a signal or HANGUP
        if (py.misc.chp < 0) {
            character_is_dead = true;
        }
    } else { // Create character
        characterCreate();

        character_birth_date = (int32_t) time((time_t *) 0);

        initializeCharacterInventory();
        py.flags.food = 7500;
        py.flags.food_digested = 2;

        // Spell and Mana based on class: Mage or Clerical realm.
        if (classes[py.misc.pclass].spell == MAGE) {
            clearScreen(); // makes spell list easier to read
            playerCalculateAllowedSpellsCount(A_INT);
            playerGainMana(A_INT);
        } else if (classes[py.misc.pclass].spell == PRIEST) {
            playerCalculateAllowedSpellsCount(A_WIS);
            clearScreen(); // force out the 'learn prayer' message
            playerGainMana(A_WIS);
        }

        // prevent ^c quit from entering score into scoreboard,
        // and prevent signal from creating panic save until this
        // point, all info needed for save file is now valid.
        character_generated = true;
        generate = true;
    }

    if (force_rogue_like) {
        use_roguelike_keys = force_keys_to;
    }

    magicInitializeItemNames();

    //
    // Begin the game
    //
    clearScreen();
    printCharacterStatsBlock();

    if (generate) {
        generateCave();
    }

    // Loop till dead, or exit
    while (!character_is_dead) {
        // Dungeon logic
        playDungeon();

        // check for eof here, see getKeyInput() in io.c
        // eof can occur if the process gets a HANGUP signal
        if (eof_flag) {
            (void) strcpy(character_died_from, "(end of input: saved)");
            if (!saveGame()) {
                (void) strcpy(character_died_from, "unexpected eof");
            }

            // should not reach here, but if we do, this guarantees exit
            character_is_dead = true;
        }

        // New level if not dead
        if (!character_is_dead) {
            generateCave();
        }
    }

    // Character gets buried.
    exitGame();

    // should never reach here, but just in case
    return 0;
}

// Init players with some belongings -RAK-
static void initializeCharacterInventory() {
    Inventory_t item;

    // this is needed for bash to work right, it can't hurt anyway
    for (int i = 0; i < PLAYER_INVENTORY_SIZE; i++) {
        inventoryItemCopyTo(OBJ_NOTHING, &inventory[i]);
    }

    for (int i = 0; i < 5; i++) {
        inventoryItemCopyTo(class_base_provisions[py.misc.pclass][i], &item);

        // this makes it spellItemIdentifyAndRemoveRandomInscription and itemSetAsIdentified
        itemIdentifyAsStoreBought(&item);

        // must set this bit to display tohit/todam for stiletto
        if (item.tval == TV_SWORD) {
            item.ident |= ID_SHOW_HITDAM;
        }

        (void) inventoryCarryItem(&item);
    }

    // weird place for it, but why not?
    for (int i = 0; i < 32; i++) {
        spells_learned_order[i] = 99;
    }
}

// Initializes M_LEVEL array for use with PLACE_MONSTER -RAK-
static void initializeMonsterLevels() {
    for (int i = 0; i <= MON_MAX_LEVELS; i++) {
        monster_levels[i] = 0;
    }

    for (int i = 0; i < MON_MAX_CREATURES - MON_ENDGAME_MONSTERS; i++) {
        monster_levels[creatures_list[i].level]++;
    }

    for (int i = 1; i <= MON_MAX_LEVELS; i++) {
        monster_levels[i] += monster_levels[i - 1];
    }
}

// Initializes T_LEVEL array for use with PLACE_OBJECT -RAK-
static void initializeTreasureLevels() {
    for (int i = 0; i <= TREASURE_MAX_LEVELS; i++) {
        treasure_levels[i] = 0;
    }

    for (int i = 0; i < MAX_DUNGEON_OBJECTS; i++) {
        treasure_levels[game_objects[i].level]++;
    }

    for (int i = 1; i <= TREASURE_MAX_LEVELS; i++) {
        treasure_levels[i] += treasure_levels[i - 1];
    }

    // now produce an array with object indexes sorted by level,
    // by using the info in treasure_levels, this is an O(n) sort!
    // this is not a stable sort, but that does not matter
    int indexes[TREASURE_MAX_LEVELS + 1];
    for (int i = 0; i <= TREASURE_MAX_LEVELS; i++) {
        indexes[i] = 1;
    }

    for (int i = 0; i < MAX_DUNGEON_OBJECTS; i++) {
        int level = game_objects[i].level;
        int object_id = treasure_levels[level] - indexes[level];

        sorted_objects[object_id] = (int16_t) i;

        indexes[level]++;
    }
}

#if (COST_ADJ != 100)

// Adjust prices of objects -RAK-
static void price_adjust() {
    // round half-way cases up
    for (int i = 0; i < MAX_OBJECTS_IN_GAME; i++) {
        game_objects[i].cost = ((game_objects[i].cost * COST_ADJUSTMENT) + 50) / 100;
    }
}

#endif
