// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Run the game: the main loop

#include "headers.h"
#include "externs.h"

static void initializeCharacterInventory();
static void initializeMonsterLevels();
static void initializeTreasureLevels();
static void priceAdjust();

void startMoria(int seed, bool start_new_game, bool use_roguelike_keys) {
    priceAdjust();

    // Show the game splash screen
    displaySplashScreen();

    // Some necessary initializations
    // all made into constants or initialized in `globals.cpp`

    // Grab a random seed from the clock
    seedsInitialize(static_cast<uint32_t>(seed));

    // Init monster and treasure levels for allocate
    initializeMonsterLevels();
    initializeTreasureLevels();

    // Init the store inventories
    storeInitializeOwners();

    // If -n is not passed, the calling routine will know
    // save file name, hence, this code is not necessary.

    // This restoration of a saved character may get ONLY the monster memory. In
    // this case, `loadGame()` returns false. It may also resurrect a dead character
    // (if you are the wizard). In this case, it returns true, but also sets the
    // parameter "generate" to true, as it does not recover any cave details.

    bool result = false;
    bool generate = false;

    if (!start_new_game && (access(config.save_game_filename, 0) == 0) && loadGame(&generate)) {
        result = true;
    }

    // Executing after game load to override saved game settings
    if (use_roguelike_keys) {
        config.use_roguelike_keys = true;
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
        if (py.misc.current_hp < 0) {
            character_is_dead = true;
        }
    } else {
        // Create character
        characterCreate();

        character_birth_date = (int32_t) time((time_t *) 0);

        initializeCharacterInventory();
        py.flags.food = 7500;
        py.flags.food_digested = 2;

        // Spell and Mana based on class: Mage or Clerical realm.
        if (classes[py.misc.class_id].class_to_use_mage_spells == SPELL_TYPE_MAGE) {
            clearScreen(); // makes spell list easier to read
            playerCalculateAllowedSpellsCount(A_INT);
            playerGainMana(A_INT);
        } else if (classes[py.misc.class_id].class_to_use_mage_spells == SPELL_TYPE_PRIEST) {
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
        if (eof_flag != 0) {
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
}

// Init players with some belongings -RAK-
static void initializeCharacterInventory() {
    Inventory_t item;

    // this is needed for bash to work right, it can't hurt anyway
    for (int i = 0; i < PLAYER_INVENTORY_SIZE; i++) {
        inventoryItemCopyTo(OBJ_NOTHING, &inventory[i]);
    }

    for (int i = 0; i < 5; i++) {
        inventoryItemCopyTo(class_base_provisions[py.misc.class_id][i], &item);

        // this makes it spellItemIdentifyAndRemoveRandomInscription and itemSetAsIdentified
        itemIdentifyAsStoreBought(&item);

        // must set this bit to display to_hit/to_damage for stiletto
        if (item.category_id == TV_SWORD) {
            item.identification |= ID_SHOW_HIT_DAM;
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
        treasure_levels[game_objects[i].depth_first_found]++;
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
        int level = game_objects[i].depth_first_found;
        int object_id = treasure_levels[level] - indexes[level];

        sorted_objects[object_id] = (int16_t) i;

        indexes[level]++;
    }
}

// Adjust prices of objects -RAK-
static void priceAdjust() {
#if (COST_ADJUSTMENT != 100)
    // round half-way cases up
    for (int i = 0; i < MAX_OBJECTS_IN_GAME; i++) {
        game_objects[i].cost = ((game_objects[i].cost * COST_ADJUSTMENT) + 50) / 100;
    }
#endif
}

