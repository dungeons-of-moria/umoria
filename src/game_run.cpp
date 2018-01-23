// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// Run the game: the main loop

#include "headers.h"

static void playDungeon();

static void initializeCharacterInventory();
static void initializeMonsterLevels();
static void initializeTreasureLevels();
static void priceAdjust();
static char originalCommands(char command);
static void doCommand(char command);
static bool validCountCommand(char command);
static void playerRegenerateHitPoints(int percent);
static void playerRegenerateMana(int percent);
static bool itemEnchanted(Inventory_t const &item);
static void examineBook();
static void dungeonGoUpLevel();
static void dungeonGoDownLevel();
static void dungeonJamDoor();
static void inventoryRefillLamp();

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

    // NOTE: base exp levels need initializing before loading a game
    playerInitializeBaseExperienceLevels();

    // initialize some player fields - may or may not be needed -MRC-
    py.flags.spells_learnt = 0;
    py.flags.spells_worked = 0;
    py.flags.spells_forgotten = 0;

    // If -n is not passed, the calling routine will know
    // save file name, hence, this code is not necessary.

    // This restoration of a saved character may get ONLY the monster memory. In
    // this case, `loadGame()` returns false. It may also resurrect a dead character
    // (if you are the wizard). In this case, it returns true, but also sets the
    // parameter "generate" to true, as it does not recover any cave details.

    bool result = false;
    bool generate = false;

    if (!start_new_game && (access(config.files.save_game.c_str(), 0) == 0) && loadGame(generate)) {
        result = true;
    }

    // Executing after game load to override saved game settings
    if (use_roguelike_keys) {
        config.use_roguelike_keys = true;
    }

    // enter wizard mode before showing the character display, but must wait
    // until after loadGame() in case it was just a resurrection
    if (game.to_be_wizard) {
        if (!enterWizardMode()) {
            endGame();
        }
    }

    if (result) {
        changeCharacterName();

        // could be restoring a dead character after a signal or HANGUP
        if (py.misc.current_hp < 0) {
            game.character_is_dead = true;
        }
    } else {
        // Create character
        characterCreate();

        py.misc.date_of_birth = getCurrentUnixTime();

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

        // Set some default values -MRC-
        py.temporary_light_only = false;
        py.weapon_is_heavy = false;
        py.pack_heaviness = 0;

        // prevent ^c quit from entering score into scoreboard,
        // and prevent signal from creating panic save until this
        // point, all info needed for save file is now valid.
        game.character_generated = true;
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
    while (!game.character_is_dead) {
        // Dungeon logic
        playDungeon();

        // check for eof here, see getKeyInput() in io.c
        // eof can occur if the process gets a HANGUP signal
        if (eof_flag != 0) {
            (void) strcpy(game.character_died_from, "(end of input: saved)");
            if (!saveGame()) {
                (void) strcpy(game.character_died_from, "unexpected eof");
            }

            // should not reach here, but if we do, this guarantees exit
            game.character_is_dead = true;
        }

        // New level if not dead
        if (!game.character_is_dead) {
            generateCave();
        }
    }

    // Character gets buried.
    endGame();
}

// Init players with some belongings -RAK-
static void initializeCharacterInventory() {
    Inventory_t item{};

    // this is needed for bash to work right, it can't hurt anyway
    for (auto &entry : inventory) {
        inventoryItemCopyTo(OBJ_NOTHING, entry);
    }

    for (auto item_id : class_base_provisions[py.misc.class_id]) {
        inventoryItemCopyTo(item_id, item);

        // this makes it spellItemIdentifyAndRemoveRandomInscription and itemSetAsIdentified
        itemIdentifyAsStoreBought(item);

        // must set this bit to display to_hit/to_damage for stiletto
        if (item.category_id == TV_SWORD) {
            item.identification |= ID_SHOW_HIT_DAM;
        }

        (void) inventoryCarryItem(item);
    }

    // weird place for it, but why not?
    for (uint8_t &id : py.flags.spells_learned_order) {
        id = 99;
    }
}

// Initializes M_LEVEL array for use with PLACE_MONSTER -RAK-
static void initializeMonsterLevels() {
    for (auto &level : monster_levels) {
        level = 0;
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
    for (auto &level : treasure_levels) {
        level = 0;
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
    int indexes[TREASURE_MAX_LEVELS + 1] = {};
    for (auto &i : indexes) {
        i = 1;
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
    for (auto &item : game_objects) {
        item.cost = ((item.cost * COST_ADJUSTMENT) + 50) / 100;
    }
#endif
}

// Moria game module -RAK-
// The code in this section has gone through many revisions, and
// some of it could stand some more hard work. -RAK-

// It has had a bit more hard work. -CJS-

// Reset flags and initialize variables
static void resetDungeonFlags() {
    game.command_count = 0;
    dg.generate_new_level = false;
    py.running_tracker = 0;
    teleport_player = false;
    monster_multiply_total = 0;
    dg.floor[py.row][py.col].creature_id = 1;
}

// Check light status for dungeon setup
static void playerInitializePlayerLight() {
    py.carrying_light = (inventory[EQUIPMENT_LIGHT].misc_use > 0);
}

// Check for a maximum level
static void playerUpdateMaxDungeonDepth() {
    if (dg.current_level > py.misc.max_dungeon_depth) {
        py.misc.max_dungeon_depth = (uint16_t) dg.current_level;
    }
}

// Check light status
static void playerUpdateLightStatus() {
    Inventory_t &item = inventory[EQUIPMENT_LIGHT];

    if (py.carrying_light) {
        if (item.misc_use > 0) {
            item.misc_use--;

            if (item.misc_use == 0) {
                py.carrying_light = false;
                printMessage("Your light has gone out!");
                playerDisturb(0, 1);

                // unlight creatures
                updateMonsters(false);
            } else if (item.misc_use < 40 && randomNumber(5) == 1 && py.flags.blind < 1) {
                playerDisturb(0, 0);
                printMessage("Your light is growing faint.");
            }
        } else {
            py.carrying_light = false;
            playerDisturb(0, 1);

            // unlight creatures
            updateMonsters(false);
        }
    } else if (item.misc_use > 0) {
        item.misc_use--;
        py.carrying_light = true;
        playerDisturb(0, 1);

        // light creatures
        updateMonsters(false);
    }
}

static void playerActivateHeroism() {
    py.flags.status |= PY_HERO;
    playerDisturb(0, 0);

    py.misc.max_hp += 10;
    py.misc.current_hp += 10;
    py.misc.bth += 12;
    py.misc.bth_with_bows += 12;

    printMessage("You feel like a HERO!");
    printCharacterMaxHitPoints();
    printCharacterCurrentHitPoints();
}

static void playerDisableHeroism() {
    py.flags.status &= ~PY_HERO;
    playerDisturb(0, 0);

    py.misc.max_hp -= 10;
    if (py.misc.current_hp > py.misc.max_hp) {
        py.misc.current_hp = py.misc.max_hp;
        py.misc.current_hp_fraction = 0;
        printCharacterCurrentHitPoints();
    }
    py.misc.bth -= 12;
    py.misc.bth_with_bows -= 12;

    printMessage("The heroism wears off.");
    printCharacterMaxHitPoints();
}

static void playerActivateSuperHeroism() {
    py.flags.status |= PY_SHERO;
    playerDisturb(0, 0);

    py.misc.max_hp += 20;
    py.misc.current_hp += 20;
    py.misc.bth += 24;
    py.misc.bth_with_bows += 24;

    printMessage("You feel like a SUPER HERO!");
    printCharacterMaxHitPoints();
    printCharacterCurrentHitPoints();
}

static void playerDisableSuperHeroism() {
    py.flags.status &= ~PY_SHERO;
    playerDisturb(0, 0);

    py.misc.max_hp -= 20;
    if (py.misc.current_hp > py.misc.max_hp) {
        py.misc.current_hp = py.misc.max_hp;
        py.misc.current_hp_fraction = 0;
        printCharacterCurrentHitPoints();
    }
    py.misc.bth -= 24;
    py.misc.bth_with_bows -= 24;

    printMessage("The super heroism wears off.");
    printCharacterMaxHitPoints();
}

static void playerUpdateHeroStatus() {
    // Heroism
    if (py.flags.heroism > 0) {
        if ((py.flags.status & PY_HERO) == 0) {
            playerActivateHeroism();
        }

        py.flags.heroism--;

        if (py.flags.heroism == 0) {
            playerDisableHeroism();
        }
    }

    // Super Heroism
    if (py.flags.super_heroism > 0) {
        if ((py.flags.status & PY_SHERO) == 0) {
            playerActivateSuperHeroism();
        }

        py.flags.super_heroism--;

        if (py.flags.super_heroism == 0) {
            playerDisableSuperHeroism();
        }
    }
}

static int playerFoodConsumption() {
    // Regenerate hp and mana
    int regen_amount = PLAYER_REGEN_NORMAL;

    if (py.flags.food < PLAYER_FOOD_ALERT) {
        if (py.flags.food < PLAYER_FOOD_WEAK) {
            if (py.flags.food < 0) {
                regen_amount = 0;
            } else if (py.flags.food < PLAYER_FOOD_FAINT) {
                regen_amount = PLAYER_REGEN_FAINT;
            } else if (py.flags.food < PLAYER_FOOD_WEAK) {
                regen_amount = PLAYER_REGEN_WEAK;
            }

            if ((py.flags.status & PY_WEAK) == 0) {
                py.flags.status |= PY_WEAK;
                printMessage("You are getting weak from hunger.");
                playerDisturb(0, 0);
                printCharacterHungerStatus();
            }

            if (py.flags.food < PLAYER_FOOD_FAINT && randomNumber(8) == 1) {
                py.flags.paralysis += randomNumber(5);
                printMessage("You faint from the lack of food.");
                playerDisturb(1, 0);
            }
        } else if ((py.flags.status & PY_HUNGRY) == 0) {
            py.flags.status |= PY_HUNGRY;
            printMessage("You are getting hungry.");
            playerDisturb(0, 0);
            printCharacterHungerStatus();
        }
    }

    // Food consumption
    // Note: Sped up characters really burn up the food!
    if (py.flags.speed < 0) {
        py.flags.food -= py.flags.speed * py.flags.speed;
    }

    py.flags.food -= py.flags.food_digested;

    if (py.flags.food < 0) {
        playerTakesHit(-py.flags.food / 16, "starvation"); // -CJS-
        playerDisturb(1, 0);
    }

    return regen_amount;
}

static void playerUpdateRegeneration(int amount) {
    if (py.flags.regenerate_hp) {
        amount = amount * 3 / 2;
    }

    if (((py.flags.status & PY_SEARCH) != 0u) || py.flags.rest != 0) {
        amount = amount * 2;
    }

    if (py.flags.poisoned < 1 && py.misc.current_hp < py.misc.max_hp) {
        playerRegenerateHitPoints(amount);
    }

    if (py.misc.current_mana < py.misc.mana) {
        playerRegenerateMana(amount);
    }
}

static void playerUpdateBlindness() {
    if (py.flags.blind <= 0) {
        return;
    }

    if ((py.flags.status & PY_BLIND) == 0) {
        py.flags.status |= PY_BLIND;

        drawDungeonPanel();
        printCharacterBlindStatus();
        playerDisturb(0, 1);

        // unlight creatures
        updateMonsters(false);
    }

    py.flags.blind--;

    if (py.flags.blind == 0) {
        py.flags.status &= ~PY_BLIND;

        printCharacterBlindStatus();
        drawDungeonPanel();
        playerDisturb(0, 1);

        // light creatures
        updateMonsters(false);

        printMessage("The veil of darkness lifts.");
    }
}

static void playerUpdateConfusion() {
    if (py.flags.confused <= 0) {
        return;
    }

    if ((py.flags.status & PY_CONFUSED) == 0) {
        py.flags.status |= PY_CONFUSED;
        printCharacterConfusedState();
    }

    py.flags.confused--;

    if (py.flags.confused == 0) {
        py.flags.status &= ~PY_CONFUSED;

        printCharacterConfusedState();
        printMessage("You feel less confused now.");

        if (py.flags.rest != 0) {
            playerRestOff();
        }
    }
}

static void playerUpdateFearState() {
    if (py.flags.afraid <= 0) {
        return;
    }

    if ((py.flags.status & PY_FEAR) == 0) {
        if (py.flags.super_heroism + py.flags.heroism > 0) {
            py.flags.afraid = 0;
        } else {
            py.flags.status |= PY_FEAR;
            printCharacterFearState();
        }
    } else if (py.flags.super_heroism + py.flags.heroism > 0) {
        py.flags.afraid = 1;
    }

    py.flags.afraid--;

    if (py.flags.afraid == 0) {
        py.flags.status &= ~PY_FEAR;

        printCharacterFearState();
        printMessage("You feel bolder now.");
        playerDisturb(0, 0);
    }
}

static void playerUpdatePoisonedState() {
    if (py.flags.poisoned <= 0) {
        return;
    }

    if ((py.flags.status & PY_POISONED) == 0) {
        py.flags.status |= PY_POISONED;
        printCharacterPoisonedState();
    }

    py.flags.poisoned--;

    if (py.flags.poisoned == 0) {
        py.flags.status &= ~PY_POISONED;

        printCharacterPoisonedState();
        printMessage("You feel better.");
        playerDisturb(0, 0);

        return;
    }

    int damage;

    switch (playerStatAdjustmentConstitution()) {
        case -4:
            damage = 4;
            break;
        case -3:
        case -2:
            damage = 3;
            break;
        case -1:
            damage = 2;
            break;
        case 0:
            damage = 1;
            break;
        case 1:
        case 2:
        case 3:
            damage = ((dg.game_turn % 2) == 0 ? 1 : 0);
            break;
        case 4:
        case 5:
            damage = ((dg.game_turn % 3) == 0 ? 1 : 0);
            break;
        case 6:
            damage = ((dg.game_turn % 4) == 0 ? 1 : 0);
            break;
        default:
            damage = 0;
            break;
    }

    playerTakesHit(damage, "poison");
    playerDisturb(1, 0);
}

static void playerUpdateFastness() {
    if (py.flags.fast <= 0) {
        return;
    }

    if ((py.flags.status & PY_FAST) == 0) {
        py.flags.status |= PY_FAST;
        playerChangeSpeed(-1);

        printMessage("You feel yourself moving faster.");
        playerDisturb(0, 0);
    }

    py.flags.fast--;

    if (py.flags.fast == 0) {
        py.flags.status &= ~PY_FAST;
        playerChangeSpeed(1);

        printMessage("You feel yourself slow down.");
        playerDisturb(0, 0);
    }
}

static void playerUpdateSlowness() {
    if (py.flags.slow <= 0) {
        return;
    }

    if ((py.flags.status & PY_SLOW) == 0) {
        py.flags.status |= PY_SLOW;
        playerChangeSpeed(1);

        printMessage("You feel yourself moving slower.");
        playerDisturb(0, 0);
    }

    py.flags.slow--;

    if (py.flags.slow == 0) {
        py.flags.status &= ~PY_SLOW;
        playerChangeSpeed(-1);

        printMessage("You feel yourself speed up.");
        playerDisturb(0, 0);
    }
}

static void playerUpdateSpeed() {
    playerUpdateFastness();
    playerUpdateSlowness();
}

// Resting is over?
static void playerUpdateRestingState() {
    if (py.flags.rest > 0) {
        py.flags.rest--;

        // Resting over
        if (py.flags.rest == 0) {
            playerRestOff();
        }
    } else if (py.flags.rest < 0) {
        // Rest until reach max mana and max hit points.
        py.flags.rest++;

        if ((py.misc.current_hp == py.misc.max_hp && py.misc.current_mana == py.misc.mana) || py.flags.rest == 0) {
            playerRestOff();
        }
    }
}

// Hallucinating?   (Random characters appear!)
static void playerUpdateHallucination() {
    if (py.flags.image <= 0) {
        return;
    }

    playerEndRunning();

    py.flags.image--;

    if (py.flags.image == 0) {
        // Used to draw entire screen! -CJS-
        drawDungeonPanel();
    }
}

static void playerUpdateParalysis() {
    if (py.flags.paralysis <= 0) {
        return;
    }

    // when paralysis true, you can not see any movement that occurs
    py.flags.paralysis--;

    playerDisturb(1, 0);
}

// Protection from evil counter
static void playerUpdateEvilProtection() {
    if (py.flags.protect_evil <= 0) {
        return;
    }

    py.flags.protect_evil--;

    if (py.flags.protect_evil == 0) {
        printMessage("You no longer feel safe from evil.");
    }
}

static void playerUpdateInvulnerability() {
    if (py.flags.invulnerability <= 0) {
        return;
    }

    if ((py.flags.status & PY_INVULN) == 0) {
        py.flags.status |= PY_INVULN;
        playerDisturb(0, 0);

        py.misc.ac += 100;
        py.misc.display_ac += 100;

        printCharacterCurrentArmorClass();
        printMessage("Your skin turns into steel!");
    }

    py.flags.invulnerability--;

    if (py.flags.invulnerability == 0) {
        py.flags.status &= ~PY_INVULN;
        playerDisturb(0, 0);

        py.misc.ac -= 100;
        py.misc.display_ac -= 100;

        printCharacterCurrentArmorClass();
        printMessage("Your skin returns to normal.");
    }
}

static void playerUpdateBlessedness() {
    if (py.flags.blessed <= 0) {
        return;
    }

    if ((py.flags.status & PY_BLESSED) == 0) {
        py.flags.status |= PY_BLESSED;
        playerDisturb(0, 0);

        py.misc.bth += 5;
        py.misc.bth_with_bows += 5;
        py.misc.ac += 2;
        py.misc.display_ac += 2;

        printMessage("You feel righteous!");
        printCharacterCurrentArmorClass();
    }

    py.flags.blessed--;

    if (py.flags.blessed == 0) {
        py.flags.status &= ~PY_BLESSED;
        playerDisturb(0, 0);

        py.misc.bth -= 5;
        py.misc.bth_with_bows -= 5;
        py.misc.ac -= 2;
        py.misc.display_ac -= 2;

        printMessage("The prayer has expired.");
        printCharacterCurrentArmorClass();
    }
}

// Resist Heat
static void playerUpdateHeatResistance() {
    if (py.flags.heat_resistance <= 0) {
        return;
    }

    py.flags.heat_resistance--;

    if (py.flags.heat_resistance == 0) {
        printMessage("You no longer feel safe from flame.");
    }
}

static void playerUpdateColdResistance() {
    if (py.flags.cold_resistance <= 0) {
        return;
    }

    py.flags.cold_resistance--;

    if (py.flags.cold_resistance == 0) {
        printMessage("You no longer feel safe from cold.");
    }
}

static void playerUpdateDetectInvisible() {
    if (py.flags.detect_invisible <= 0) {
        return;
    }

    if ((py.flags.status & PY_DET_INV) == 0) {
        py.flags.status |= PY_DET_INV;
        py.flags.see_invisible = true;

        // light but don't move creatures
        updateMonsters(false);
    }

    py.flags.detect_invisible--;

    if (py.flags.detect_invisible == 0) {
        py.flags.status &= ~PY_DET_INV;

        // may still be able to see_invisible if wearing magic item
        playerRecalculateBonuses();

        // unlight but don't move creatures
        updateMonsters(false);
    }
}

// Timed infra-vision
static void playerUpdateInfraVision() {
    if (py.flags.timed_infra <= 0) {
        return;
    }

    if ((py.flags.status & PY_TIM_INFRA) == 0) {
        py.flags.status |= PY_TIM_INFRA;
        py.flags.see_infra++;

        // light but don't move creatures
        updateMonsters(false);
    }

    py.flags.timed_infra--;

    if (py.flags.timed_infra == 0) {
        py.flags.status &= ~PY_TIM_INFRA;
        py.flags.see_infra--;

        // unlight but don't move creatures
        updateMonsters(false);
    }
}

// Word-of-Recall  Note: Word-of-Recall is a delayed action
static void playerUpdateWordOfRecall() {
    if (py.flags.word_of_recall <= 0) {
        return;
    }

    if (py.flags.word_of_recall == 1) {
        dg.generate_new_level = true;

        py.flags.paralysis++;
        py.flags.word_of_recall = 0;

        if (dg.current_level > 0) {
            dg.current_level = 0;
            printMessage("You feel yourself yanked upwards!");
        } else if (py.misc.max_dungeon_depth != 0) {
            dg.current_level = py.misc.max_dungeon_depth;
            printMessage("You feel yourself yanked downwards!");
        }
    } else {
        py.flags.word_of_recall--;
    }
}

static void playerUpdateStatusFlags() {
    if ((py.flags.status & PY_SPEED) != 0u) {
        py.flags.status &= ~PY_SPEED;
        printCharacterSpeed();
    }

    if (((py.flags.status & PY_PARALYSED) != 0u) && py.flags.paralysis < 1) {
        printCharacterMovementState();
        py.flags.status &= ~PY_PARALYSED;
    } else if (py.flags.paralysis > 0) {
        printCharacterMovementState();
        py.flags.status |= PY_PARALYSED;
    } else if (py.flags.rest != 0) {
        printCharacterMovementState();
    }

    if ((py.flags.status & PY_ARMOR) != 0) {
        printCharacterCurrentArmorClass();
        py.flags.status &= ~PY_ARMOR;
    }

    if ((py.flags.status & PY_STATS) != 0) {
        for (int n = 0; n < 6; n++) {
            if (((PY_STR << n) & py.flags.status) != 0u) {
                displayCharacterStats(n);
            }
        }

        py.flags.status &= ~PY_STATS;
    }

    if ((py.flags.status & PY_HP) != 0u) {
        printCharacterMaxHitPoints();
        printCharacterCurrentHitPoints();
        py.flags.status &= ~PY_HP;
    }

    if ((py.flags.status & PY_MANA) != 0u) {
        printCharacterCurrentMana();
        py.flags.status &= ~PY_MANA;
    }
}

// Allow for a slim chance of detect enchantment -CJS-
static void playerDetectEnchantment() {
    for (int i = 0; i < PLAYER_INVENTORY_SIZE; i++) {
        if (i == py.unique_inventory_items) {
            i = 22;
        }

        Inventory_t &item = inventory[i];

        // if in inventory, succeed 1 out of 50 times,
        // if in equipment list, success 1 out of 10 times
        int chance = (i < 22 ? 50 : 10);

        if (item.category_id != TV_NOTHING && itemEnchanted(item) && randomNumber(chance) == 1) {
            // extern const char *describe_use(int); // FIXME: Why here? We have it in externs.

            vtype_t tmp_str = {'\0'};
            (void) sprintf(tmp_str, "There's something about what you are %s...", playerItemWearingDescription(i));
            playerDisturb(0, 0);
            printMessage(tmp_str);
            itemAppendToInscription(item, ID_MAGIK);
        }
    }
}

static int getCommandRepeatCount(char &lastInputCommand) {
    putStringClearToEOL("Repeat count:", Coord_t{0, 0});

    if (lastInputCommand == '#') {
        lastInputCommand = '0';
    }

    char text_buffer[8];
    int repeat_count = 0;

    while (true) {
        if (lastInputCommand == DELETE || lastInputCommand == CTRL_KEY('H')) {
            repeat_count /= 10;
            (void) sprintf(text_buffer, "%d", (int16_t) repeat_count);
            putStringClearToEOL(text_buffer, Coord_t{0, 14});
        } else if (lastInputCommand >= '0' && lastInputCommand <= '9') {
            if (repeat_count > 99) {
                terminalBellSound();
            } else {
                repeat_count = repeat_count * 10 + lastInputCommand - '0';
                (void) sprintf(text_buffer, "%d", repeat_count);
                putStringClearToEOL(text_buffer, Coord_t{0, 14});
            }
        } else {
            break;
        }
        lastInputCommand = getKeyInput();
    }

    if (repeat_count == 0) {
        repeat_count = 99;
        (void) sprintf(text_buffer, "%d", repeat_count);
        putStringClearToEOL(text_buffer, Coord_t{0, 14});
    }

    // a special hack to allow numbers as commands
    if (lastInputCommand == ' ') {
        putStringClearToEOL("Command:", Coord_t{0, 20});
        lastInputCommand = getKeyInput();
    }

    return repeat_count;
}

static char parseAlternateCtrlInput(char lastInputCommand) {
    if (game.command_count > 0) {
        printCharacterMovementState();
    }

    if (getCommand("Control-", lastInputCommand)) {
        if (lastInputCommand >= 'A' && lastInputCommand <= 'Z') {
            lastInputCommand -= 'A' - 1;
        } else if (lastInputCommand >= 'a' && lastInputCommand <= 'z') {
            lastInputCommand -= 'a' - 1;
        } else {
            lastInputCommand = ' ';
            printMessage("Type ^ <letter> for a control char");
        }
    } else {
        lastInputCommand = ' ';
    }

    return lastInputCommand;
}

// Accept a command and execute it
static void executeInputCommands(char &command, int &find_count) {
    char lastInputCommand = command;

    // Accept a command and execute it
    do {
        if ((py.flags.status & PY_REPEAT) != 0u) {
            printCharacterMovementState();
        }

        game.use_last_direction = false;
        game.player_free_turn = false;

        if (py.running_tracker != 0) {
            playerRunAndFind();
            find_count -= 1;

            if (find_count == 0) {
                playerEndRunning();
            }

            putQIO();
            continue;
        }

        if (game.doing_inventory_command != 0) {
            inventoryExecuteCommand(game.doing_inventory_command);
            continue;
        }

        // move the cursor to the players character
        panelMoveCursor(Coord_t{py.row, py.col});

        message_ready_to_print = false;

        if (game.command_count > 0) {
            game.use_last_direction = true;
        } else {
            lastInputCommand = getKeyInput();

            // Get a count for a command.
            int repeat_count = 0;
            if ((config.use_roguelike_keys && lastInputCommand >= '0' && lastInputCommand <= '9') || (!config.use_roguelike_keys && lastInputCommand == '#')) {
                repeat_count = getCommandRepeatCount(lastInputCommand);
            }

            // Another way of typing control codes -CJS-
            if (lastInputCommand == '^') {
                lastInputCommand = parseAlternateCtrlInput(lastInputCommand);
            }

            // move cursor to player char again, in case it moved
            panelMoveCursor(Coord_t{py.row, py.col});

            // Commands are always converted to rogue form. -CJS-
            if (!config.use_roguelike_keys) {
                lastInputCommand = originalCommands(lastInputCommand);
            }

            if (repeat_count > 0) {
                if (!validCountCommand(lastInputCommand)) {
                    game.player_free_turn = true;
                    lastInputCommand = ' ';
                    printMessage("Invalid command with a count.");
                } else {
                    game.command_count = repeat_count;
                    printCharacterMovementState();
                }
            }
        }

        // Flash the message line.
        messageLineClear();
        panelMoveCursor(Coord_t{py.row, py.col});
        putQIO();

        doCommand(lastInputCommand);

        // Find is counted differently, as the command changes.
        if (py.running_tracker != 0) {
            find_count = game.command_count - 1;
            game.command_count = 0;
        } else if (game.player_free_turn) {
            game.command_count = 0;
        } else if (game.command_count != 0) {
            game.command_count--;
        }
    } while (game.player_free_turn && !dg.generate_new_level && (eof_flag == 0));

    command = lastInputCommand;
}

static char originalCommands(char command) {
    int direction;

    switch (command) {
        case CTRL_KEY('K'): // ^K = exit
            command = 'Q';
            break;
        case CTRL_KEY('J'):
        case CTRL_KEY('M'):
            command = '+';
            break;
        case CTRL_KEY('P'): // ^P = repeat
        case CTRL_KEY('W'): // ^W = password
        case CTRL_KEY('X'): // ^X = save
        case CTRL_KEY('V'): // ^V = view license
        case ' ':
        case '!':
        case '$':
            break;
        case '.':
            if (getDirectionWithMemory(CNIL, direction)) {
                switch (direction) {
                    case 1:
                        command = 'B';
                        break;
                    case 2:
                        command = 'J';
                        break;
                    case 3:
                        command = 'N';
                        break;
                    case 4:
                        command = 'H';
                        break;
                    case 6:
                        command = 'L';
                        break;
                    case 7:
                        command = 'Y';
                        break;
                    case 8:
                        command = 'K';
                        break;
                    case 9:
                        command = 'U';
                        break;
                    default:
                        command = ' ';
                        break;
                }
            } else {
                command = ' ';
            }
            break;
        case '/':
        case '<':
        case '>':
        case '-':
        case '=':
        case '{':
        case '?':
        case 'A':
            break;
        case '1':
            command = 'b';
            break;
        case '2':
            command = 'j';
            break;
        case '3':
            command = 'n';
            break;
        case '4':
            command = 'h';
            break;
        case '5': // Rest one turn
            command = '.';
            break;
        case '6':
            command = 'l';
            break;
        case '7':
            command = 'y';
            break;
        case '8':
            command = 'k';
            break;
        case '9':
            command = 'u';
            break;
        case 'B':
            command = 'f';
            break;
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
            break;
        case 'L':
            command = 'W';
            break;
        case 'M':
            break;
        case 'R':
            break;
        case 'S':
            command = '#';
            break;
        case 'T':
            if (getDirectionWithMemory(CNIL, direction)) {
                switch (direction) {
                    case 1:
                        command = CTRL_KEY('B');
                        break;
                    case 2:
                        command = CTRL_KEY('J');
                        break;
                    case 3:
                        command = CTRL_KEY('N');
                        break;
                    case 4:
                        command = CTRL_KEY('H');
                        break;
                    case 6:
                        command = CTRL_KEY('L');
                        break;
                    case 7:
                        command = CTRL_KEY('Y');
                        break;
                    case 8:
                        command = CTRL_KEY('K');
                        break;
                    case 9:
                        command = CTRL_KEY('U');
                        break;
                    default:
                        command = ' ';
                        break;
                }
            } else {
                command = ' ';
            }
            break;
        case 'V':
            break;
        case 'a':
            command = 'z';
            break;
        case 'b':
            command = 'P';
            break;
        case 'c':
        case 'd':
        case 'e':
            break;
        case 'f':
            command = 't';
            break;
        case 'h':
            command = '?';
            break;
        case 'i':
            break;
        case 'j':
            command = 'S';
            break;
        case 'l':
            command = 'x';
            break;
        case 'm':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
            break;
        case 't':
            command = 'T';
            break;
        case 'u':
            command = 'Z';
            break;
        case 'v':
        case 'w':
            break;
        case 'x':
            command = 'X';
            break;

            // wizard mode commands follow
        case CTRL_KEY('A'): // ^A = cure all
            break;
        case CTRL_KEY('B'): // ^B = objects
            command = CTRL_KEY('O');
            break;
        case CTRL_KEY('D'): // ^D = up/down
            break;
        case CTRL_KEY('H'): // ^H = wizhelp
            command = '\\';
            break;
        case CTRL_KEY('I'): // ^I = identify
            break;
        case CTRL_KEY('L'): // ^L = wizlight
            command = '*';
            break;
        case ':':
        case CTRL_KEY('T'): // ^T = teleport
        case CTRL_KEY('E'): // ^E = wizchar
        case CTRL_KEY('F'): // ^F = genocide
        case CTRL_KEY('G'): // ^G = treasure
        case '@':
        case '+':
            break;
        case CTRL_KEY('U'): // ^U = summon
            command = '&';
            break;
        default:
            command = '~'; // Anything illegal.
            break;
    }

    return command;
}

static bool moveWithoutPickup(char *command) {
    char com_val = *command;

    // hack for move without pickup.  Map '-' to a movement command.
    if (com_val != '-') {
        return true;
    }

    int dir_val;

    // Save current game.command_count as getDirectionWithMemory() may change it
    int countSave = game.command_count;

    if (getDirectionWithMemory(CNIL, dir_val)) {
        // Restore game.command_count
        game.command_count = countSave;

        switch (dir_val) {
            case 1:
                com_val = 'b';
                break;
            case 2:
                com_val = 'j';
                break;
            case 3:
                com_val = 'n';
                break;
            case 4:
                com_val = 'h';
                break;
            case 6:
                com_val = 'l';
                break;
            case 7:
                com_val = 'y';
                break;
            case 8:
                com_val = 'k';
                break;
            case 9:
                com_val = 'u';
                break;
            default:
                com_val = '~';
                break;
        }
    } else {
        com_val = ' ';
    }

    *command = com_val;

    return false;
}

static void commandQuit() {
    flushInputBuffer();

    if (getInputConfirmation("Do you really want to quit?")) {
        game.character_is_dead = true;
        dg.generate_new_level = true;

        (void) strcpy(game.character_died_from, "Quitting");
    }
}

static uint8_t calculateMaxMessageCount() {
    uint8_t max_messages = MESSAGE_HISTORY_SIZE;

    if (game.command_count > 0) {
        if (game.command_count < MESSAGE_HISTORY_SIZE) {
            max_messages = (uint8_t) game.command_count;
        }
        game.command_count = 0;
    } else if (game.last_command != CTRL_KEY('P')) {
        max_messages = 1;
    }

    return max_messages;
}

static void commandPreviousMessage() {
    uint8_t max_messages = calculateMaxMessageCount();

    if (max_messages <= 1) {
        // Distinguish real and recovered messages with a '>'. -CJS-
        putString(">", Coord_t{0, 0});
        putStringClearToEOL(messages[last_message_id], Coord_t{0, 1});
        return;
    }

    terminalSaveScreen();

    uint8_t lineNumber = max_messages;
    int16_t msg_id = last_message_id;

    while (max_messages > 0) {
        max_messages--;

        putStringClearToEOL(messages[msg_id], Coord_t{max_messages, 0});

        if (msg_id == 0) {
            msg_id = MESSAGE_HISTORY_SIZE - 1;
        } else {
            msg_id--;
        }
    }

    eraseLine(Coord_t{lineNumber, 0});
    waitForContinueKey(lineNumber);
    terminalRestoreScreen();
}

static void commandFlipWizardMode() {
    if (game.wizard_mode) {
        game.wizard_mode = false;
        printMessage("Wizard mode off.");
    } else if (enterWizardMode()) {
        printMessage("Wizard mode on.");
    }

    printCharacterWinner();
}

static void commandSaveAndExit() {
    if (game.total_winner) {
        printMessage("You are a Total Winner,  your character must be retired.");

        if (config.use_roguelike_keys) {
            printMessage("Use 'Q' to when you are ready to quit.");
        } else {
            printMessage("Use <Control>-K when you are ready to quit.");
        }
    } else {
        (void) strcpy(game.character_died_from, "(saved)");
        printMessage("Saving game...");

        if (saveGame()) {
            endGame();
        }

        (void) strcpy(game.character_died_from, "(alive and well)");
    }
}

static void commandLocateOnMap() {
    if (py.flags.blind > 0 || playerNoLight()) {
        printMessage("You can't see your map.");
        return;
    }

    int y = py.row;
    int x = py.col;
    if (coordOutsidePanel(Coord_t{y, x}, true)) {
        drawDungeonPanel();
    }

    int cy, cx, p_y, p_x;

    cy = dg.panel.row;
    cx = dg.panel.col;

    int dir_val;
    vtype_t out_val = {'\0'};
    vtype_t tmp_str = {'\0'};

    while (true) {
        p_y = dg.panel.row;
        p_x = dg.panel.col;

        if (p_y == cy && p_x == cx) {
            tmp_str[0] = '\0';
        } else {
            (void) sprintf(tmp_str, "%s%s of", p_y < cy ? " North" : p_y > cy ? " South" : "", p_x < cx ? " West" : p_x > cx ? " East" : "");
        }

        (void) sprintf(out_val, "Map sector [%d,%d], which is%s your sector. Look which direction?", p_y, p_x, tmp_str);

        if (!getDirectionWithMemory(out_val, dir_val)) {
            break;
        }

        // -CJS-
        // Should really use the move function, but what the hell. This
        // is nicer, as it moves exactly to the same place in another
        // section. The direction calculation is not intuitive. Sorry.
        while (true) {
            x += ((dir_val - 1) % 3 - 1) * SCREEN_WIDTH / 2;
            y -= ((dir_val - 1) / 3 - 1) * SCREEN_HEIGHT / 2;

            if (x < 0 || y < 0 || x >= dg.width || y >= dg.width) {
                printMessage("You've gone past the end of your map.");

                x -= ((dir_val - 1) % 3 - 1) * SCREEN_WIDTH / 2;
                y += ((dir_val - 1) / 3 - 1) * SCREEN_HEIGHT / 2;

                break;
            }

            if (coordOutsidePanel(Coord_t{y, x}, true)) {
                drawDungeonPanel();
                break;
            }
        }
    }

    // Move to a new panel - but only if really necessary.
    if (coordOutsidePanel(Coord_t{py.row, py.col}, false)) {
        drawDungeonPanel();
    }
}

static void commandToggleSearch() {
    if ((py.flags.status & PY_SEARCH) != 0u) {
        playerSearchOff();
    } else {
        playerSearchOn();
    }
}

static void doWizardCommands(char com_val) {
    int i, y, x;

    switch (com_val) {
        case CTRL_KEY('A'):
            // Cure all!
            (void) spellRemoveCurseFromAllItems();
            (void) playerCureBlindness();
            (void) playerCureConfusion();
            (void) playerCurePoison();
            (void) playerRemoveFear();
            (void) playerStatRestore(A_STR);
            (void) playerStatRestore(A_INT);
            (void) playerStatRestore(A_WIS);
            (void) playerStatRestore(A_CON);
            (void) playerStatRestore(A_DEX);
            (void) playerStatRestore(A_CHR);

            if (py.flags.slow > 1) {
                py.flags.slow = 1;
            }
            if (py.flags.image > 1) {
                py.flags.image = 1;
            }
            break;
        case CTRL_KEY('E'):
            // Edit Character
            wizardCharacterAdjustment();
            messageLineClear();
            break;
        case CTRL_KEY('F'):
            // Mass Genocide, vanquish all monsters
            (void) spellMassGenocide();
            break;
        case CTRL_KEY('G'):
            // Generate random items
            if (game.command_count > 0) {
                i = game.command_count;
                game.command_count = 0;
            } else {
                i = 1;
            }
            dungeonPlaceRandomObjectNear(Coord_t{py.row, py.col}, i);

            drawDungeonPanel();
            break;
        case CTRL_KEY('D'):
            // Go up/down to specified depth
            if (game.command_count > 0) {
                if (game.command_count > 99) {
                    i = 0;
                } else {
                    i = game.command_count;
                }
                game.command_count = 0;
            } else {
                i = -1;
                vtype_t input = {0};

                putStringClearToEOL("Go to which level (0-99) ? ", Coord_t{0, 0});

                if (getStringInput(input, Coord_t{0, 27}, 10)) {
                    (void) stringToNumber(input, i);
                }
            }

            if (i >= 0) {
                dg.current_level = (int16_t) i;
                if (dg.current_level > 99) {
                    dg.current_level = 99;
                }
                dg.generate_new_level = true;
            } else {
                messageLineClear();
            }
            break;
        case CTRL_KEY('O'):
            // Print random level object to a file
            outputRandomLevelObjectsToFile();
            break;
        case '\\':
            // Display wizard help
            if (config.use_roguelike_keys) {
                displayTextHelpFile(config.files.help_roguelike_wizard);
            } else {
                displayTextHelpFile(config.files.help_wizard);
            }
            break;
        case CTRL_KEY('I'):
            // Identify an item
            (void) spellIdentifyItem();
            break;
        case '*':
            // Light up entire dungeon
            wizardLightUpDungeon();
            break;
        case ':':
            // Light up current panel
            spellMapCurrentArea();
            break;
        case CTRL_KEY('T'):
            // Random player teleportation
            playerTeleport(100);
            break;
        case '%':
            // Generate a dungeon item!
            wizardGenerateObject();
            drawDungeonPanel();
            break;
        case '+':
            // Increase Experience
            if (game.command_count > 0) {
                py.misc.exp = game.command_count;
                game.command_count = 0;
            } else if (py.misc.exp == 0) {
                py.misc.exp = 1;
            } else {
                py.misc.exp = py.misc.exp * 2;
            }
            displayCharacterExperience();
            break;
        case '&':
            // Summon a random monster
            y = py.row;
            x = py.col;
            (void) monsterSummon(y, x, true);

            updateMonsters(false);
            break;
        case '@':
            // Generate an object
            // NOTE: every field from the struct needs to be filled correctly
            wizardCreateObjects();
            break;
        default:
            if (config.use_roguelike_keys) {
                putStringClearToEOL("Type '?' or '\\' for help.", Coord_t{0, 0});
            } else {
                putStringClearToEOL("Type '?' or ^H for help.", Coord_t{0, 0});
            }
    }
}

// TODO: use only commands here - don't just call the external functions.
// TODO: E.g. split playerEat() into command/action functions: commandEat(), playerEat().
// Possibly the "setup" happens in the command, such as the food check/selection of playerEat().
// The command then calls playerEat() in player_eat.cpp - passing the selected food `item_id`.
static void doCommand(char command) {
    bool do_pickup = moveWithoutPickup(&command);

    switch (command) {
        case 'Q': // (Q)uit    (^K)ill
            commandQuit();
            game.player_free_turn = true;
            break;
        case CTRL_KEY('P'): // (^P)revious message.
            commandPreviousMessage();
            game.player_free_turn = true;
            break;
        case CTRL_KEY('V'): // (^V)iew license
            displayTextHelpFile(config.files.license);
            game.player_free_turn = true;
            break;
        case CTRL_KEY('W'): // (^W)izard mode
            commandFlipWizardMode();
            game.player_free_turn = true;
            break;
        case CTRL_KEY('X'): // e(^X)it and save
            commandSaveAndExit();
            game.player_free_turn = true;
            break;
        case '=': // (=) set options
            terminalSaveScreen();
            setGameOptions();
            terminalRestoreScreen();
            game.player_free_turn = true;
            break;
        case '{': // ({) inscribe an object
            itemInscribe();
            game.player_free_turn = true;
            break;
        case '!': // (!) escape to the shell
        case '$':
            // escaping to shell disabled -MRC-
            game.player_free_turn = true;
            break;
        case ESCAPE: // (ESC)   do nothing.
        case ' ':    // (space) do nothing.
            game.player_free_turn = true;
            break;
        case 'b': // (b) down, left  (1)
            playerMove(1, do_pickup);
            break;
        case 'j': // (j) down    (2)
            playerMove(2, do_pickup);
            break;
        case 'n': // (n) down, right  (3)
            playerMove(3, do_pickup);
            break;
        case 'h': // (h) left    (4)
            playerMove(4, do_pickup);
            break;
        case 'l': // (l) right    (6)
            playerMove(6, do_pickup);
            break;
        case 'y': // (y) up, left    (7)
            playerMove(7, do_pickup);
            break;
        case 'k': // (k) up    (8)
            playerMove(8, do_pickup);
            break;
        case 'u': // (u) up, right  (9)
            playerMove(9, do_pickup);
            break;
        case 'B': // (B) run down, left  (. 1)
            playerFindInitialize(1);
            break;
        case 'J': // (J) run down    (. 2)
            playerFindInitialize(2);
            break;
        case 'N': // (N) run down, right  (. 3)
            playerFindInitialize(3);
            break;
        case 'H': // (H) run left    (. 4)
            playerFindInitialize(4);
            break;
        case 'L': // (L) run right  (. 6)
            playerFindInitialize(6);
            break;
        case 'Y': // (Y) run up, left  (. 7)
            playerFindInitialize(7);
            break;
        case 'K': // (K) run up    (. 8)
            playerFindInitialize(8);
            break;
        case 'U': // (U) run up, right  (. 9)
            playerFindInitialize(9);
            break;
        case '/': // (/) identify a symbol
            identifyGameObject();
            game.player_free_turn = true;
            break;
        case '.': // (.) stay in one place (5)
            playerMove(5, do_pickup);

            if (game.command_count > 1) {
                game.command_count--;
                playerRestOn();
            }
            break;
        case '<': // (<) go down a staircase
            dungeonGoUpLevel();
            break;
        case '>': // (>) go up a staircase
            dungeonGoDownLevel();
            break;
        case '?': // (?) help with commands
            if (config.use_roguelike_keys) {
                displayTextHelpFile(config.files.help_roguelike);
            } else {
                displayTextHelpFile(config.files.help);
            }
            game.player_free_turn = true;
            break;
        case 'f': // (f)orce    (B)ash
            playerBash();
            break;
        case 'C': // (C)haracter description
            terminalSaveScreen();
            changeCharacterName();
            terminalRestoreScreen();
            game.player_free_turn = true;
            break;
        case 'D': // (D)isarm trap
            playerDisarmTrap();
            break;
        case 'E': // (E)at food
            playerEat();
            break;
        case 'F': // (F)ill lamp
            inventoryRefillLamp();
            break;
        case 'G': // (G)ain magic spells
            playerGainSpells();
            break;
        case 'V': // (V)iew scores
            terminalSaveScreen();
            showScoresScreen();
            terminalRestoreScreen();
            game.player_free_turn = true;
            break;
        case 'W': // (W)here are we on the map  (L)ocate on map
            commandLocateOnMap();
            game.player_free_turn = true;
            break;
        case 'R': // (R)est a while
            playerRestOn();
            break;
        case '#': // (#) search toggle  (S)earch toggle
            commandToggleSearch();
            game.player_free_turn = true;
            break;
        case CTRL_KEY('B'): // (^B) tunnel down left  (T 1)
            playerTunnel(1);
            break;
        case CTRL_KEY('M'): // cr must be treated same as lf.
        case CTRL_KEY('J'): // (^J) tunnel down    (T 2)
            playerTunnel(2);
            break;
        case CTRL_KEY('N'): // (^N) tunnel down right  (T 3)
            playerTunnel(3);
            break;
        case CTRL_KEY('H'): // (^H) tunnel left    (T 4)
            playerTunnel(4);
            break;
        case CTRL_KEY('L'): // (^L) tunnel right    (T 6)
            playerTunnel(6);
            break;
        case CTRL_KEY('Y'): // (^Y) tunnel up left    (T 7)
            playerTunnel(7);
            break;
        case CTRL_KEY('K'): // (^K) tunnel up    (T 8)
            playerTunnel(8);
            break;
        case CTRL_KEY('U'): // (^U) tunnel up right    (T 9)
            playerTunnel(9);
            break;
        case 'z': // (z)ap a wand    (a)im a wand
            wandAim();
            break;
        case 'M':
            dungeonDisplayMap();
            game.player_free_turn = true;
            break;
        case 'P': // (P)eruse a book  (B)rowse in a book
            examineBook();
            game.player_free_turn = true;
            break;
        case 'c': // (c)lose an object
            playerCloseDoor();
            break;
        case 'd': // (d)rop something
            inventoryExecuteCommand('d');
            break;
        case 'e': // (e)quipment list
            inventoryExecuteCommand('e');
            break;
        case 't': // (t)hrow something  (f)ire something
            playerThrowItem();
            break;
        case 'i': // (i)nventory list
            inventoryExecuteCommand('i');
            break;
        case 'S': // (S)pike a door  (j)am a door
            dungeonJamDoor();
            break;
        case 'x': // e(x)amine surrounds  (l)ook about
            look();
            game.player_free_turn = true;
            break;
        case 'm': // (m)agic spells
            getAndCastMagicSpell();
            break;
        case 'o': // (o)pen something
            playerOpenClosedObject();
            break;
        case 'p': // (p)ray
            pray();
            break;
        case 'q': // (q)uaff
            quaff();
            break;
        case 'r': // (r)ead
            scrollRead();
            break;
        case 's': // (s)earch for a turn
            playerSearch(py.row, py.col, py.misc.chance_in_search);
            break;
        case 'T': // (T)ake off something  (t)ake off
            inventoryExecuteCommand('t');
            break;
        case 'Z': // (Z)ap a staff  (u)se a staff
            staffUse();
            break;
        case 'v': // (v)ersion of game
            displayTextHelpFile(config.files.versions_history);
            game.player_free_turn = true;
            break;
        case 'w': // (w)ear or wield
            inventoryExecuteCommand('w');
            break;
        case 'X': // e(X)change weapons  e(x)change
            inventoryExecuteCommand('x');
            break;
        default:
            // Wizard commands are free moves
            game.player_free_turn = true;

            if (game.wizard_mode) {
                doWizardCommands(command);
            } else {
                putStringClearToEOL("Type '?' for help.", Coord_t{0, 0});
            }
    }
    game.last_command = command;
}

// Check whether this command will accept a count. -CJS-
static bool validCountCommand(char command) {
    switch (command) {
        case 'Q':
        case CTRL_KEY('W'):
        case CTRL_KEY('X'):
        case '=':
        case '{':
        case '/':
        case '<':
        case '>':
        case '?':
        case 'C':
        case 'E':
        case 'F':
        case 'G':
        case 'V':
        case '#':
        case 'z':
        case 'P':
        case 'c':
        case 'd':
        case 'e':
        case 't':
        case 'i':
        case 'x':
        case 'm':
        case 'p':
        case 'q':
        case 'r':
        case 'T':
        case 'Z':
        case 'v':
        case 'w':
        case 'W':
        case 'X':
        case CTRL_KEY('A'):
        case '\\':
        case CTRL_KEY('I'):
        case '*':
        case ':':
        case CTRL_KEY('T'):
        case CTRL_KEY('E'):
        case CTRL_KEY('F'):
        case CTRL_KEY('S'):
        case CTRL_KEY('Q'):
            return false;
        case CTRL_KEY('P'):
        case ESCAPE:
        case ' ':
        case '-':
        case 'b':
        case 'f':
        case 'j':
        case 'n':
        case 'h':
        case 'l':
        case 'y':
        case 'k':
        case 'u':
        case '.':
        case 'B':
        case 'J':
        case 'N':
        case 'H':
        case 'L':
        case 'Y':
        case 'K':
        case 'U':
        case 'D':
        case 'R':
        case CTRL_KEY('Y'):
        case CTRL_KEY('K'):
        case CTRL_KEY('U'):
        case CTRL_KEY('L'):
        case CTRL_KEY('N'):
        case CTRL_KEY('J'):
        case CTRL_KEY('B'):
        case CTRL_KEY('H'):
        case 'S':
        case 'o':
        case 's':
        case CTRL_KEY('D'):
        case CTRL_KEY('G'):
        case '+':
            return true;
        default:
            return false;
    }
}

// Regenerate hit points -RAK-
static void playerRegenerateHitPoints(int percent) {
    int old_chp = py.misc.current_hp;
    int32_t new_chp = (int32_t) py.misc.max_hp * percent + PLAYER_REGEN_HPBASE;

    // div 65536
    py.misc.current_hp += new_chp >> 16;

    // check for overflow
    if (py.misc.current_hp < 0 && old_chp > 0) {
        py.misc.current_hp = MAX_SHORT;
    }

    // mod 65536
    int32_t new_chp_fraction = (new_chp & 0xFFFF) + py.misc.current_hp_fraction;

    if (new_chp_fraction >= 0x10000L) {
        py.misc.current_hp_fraction = (uint16_t) (new_chp_fraction - 0x10000L);
        py.misc.current_hp++;
    } else {
        py.misc.current_hp_fraction = (uint16_t) new_chp_fraction;
    }

    // must set frac to zero even if equal
    if (py.misc.current_hp >= py.misc.max_hp) {
        py.misc.current_hp = py.misc.max_hp;
        py.misc.current_hp_fraction = 0;
    }

    if (old_chp != py.misc.current_hp) {
        printCharacterCurrentHitPoints();
    }
}

// Regenerate mana points -RAK-
static void playerRegenerateMana(int percent) {
    int old_cmana = py.misc.current_mana;
    int32_t new_mana = (int32_t) py.misc.mana * percent + PLAYER_REGEN_MNBASE;

    // div 65536
    py.misc.current_mana += new_mana >> 16;

    // check for overflow
    if (py.misc.current_mana < 0 && old_cmana > 0) {
        py.misc.current_mana = MAX_SHORT;
    }

    // mod 65536
    int32_t new_mana_fraction = (new_mana & 0xFFFF) + py.misc.current_mana_fraction;

    if (new_mana_fraction >= 0x10000L) {
        py.misc.current_mana_fraction = (uint16_t) (new_mana_fraction - 0x10000L);
        py.misc.current_mana++;
    } else {
        py.misc.current_mana_fraction = (uint16_t) new_mana_fraction;
    }

    // must set frac to zero even if equal
    if (py.misc.current_mana >= py.misc.mana) {
        py.misc.current_mana = py.misc.mana;
        py.misc.current_mana_fraction = 0;
    }

    if (old_cmana != py.misc.current_mana) {
        printCharacterCurrentMana();
    }
}

// Is an item an enchanted weapon or armor and we don't know? -CJS-
// only returns true if it is a good enchantment
static bool itemEnchanted(Inventory_t const &item) {
    if (item.category_id < TV_MIN_ENCHANT || item.category_id > TV_MAX_ENCHANT || (item.flags & TR_CURSED) != 0u) {
        return false;
    }

    if (spellItemIdentified(item)) {
        return false;
    }

    if ((item.identification & ID_MAGIK) != 0) {
        return false;
    }

    if (item.to_hit > 0 || item.to_damage > 0 || item.to_ac > 0) {
        return true;
    }

    if ((0x4000107fL & item.flags) != 0 && item.misc_use > 0) {
        return true;
    }

    if ((0x07ffe980L & item.flags) != 0) {
        return true;
    }

    return false;
}

// Examine a Book -RAK-
static void examineBook() {
    int item_pos_start, item_pos_end;
    if (!inventoryFindRange(TV_MAGIC_BOOK, TV_PRAYER_BOOK, item_pos_start, item_pos_end)) {
        printMessage("You are not carrying any books.");
        return;
    }

    if (py.flags.blind > 0) {
        printMessage("You can't see to read your spell book!");
        return;
    }

    if (playerNoLight()) {
        printMessage("You have no light to read by.");
        return;
    }

    if (py.flags.confused > 0) {
        printMessage("You are too confused.");
        return;
    }

    int item_id;
    if (inventoryGetInputForItemId(item_id, "Which Book?", item_pos_start, item_pos_end, CNIL, CNIL)) {
        int spell_index[31];
        bool can_read = true;

        uint8_t treasure_type = inventory[item_id].category_id;

        if (classes[py.misc.class_id].class_to_use_mage_spells == SPELL_TYPE_MAGE) {
            if (treasure_type != TV_MAGIC_BOOK) {
                can_read = false;
            }
        } else if (classes[py.misc.class_id].class_to_use_mage_spells == SPELL_TYPE_PRIEST) {
            if (treasure_type != TV_PRAYER_BOOK) {
                can_read = false;
            }
        } else {
            can_read = false;
        }

        if (!can_read) {
            printMessage("You do not understand the language.");
            return;
        }

        uint32_t item_flags = inventory[item_id].flags;

        int spell_id = 0;
        while (item_flags != 0u) {
            item_pos_end = getAndClearFirstBit(item_flags);

            if (magic_spells[py.misc.class_id - 1][item_pos_end].level_required < 99) {
                spell_index[spell_id] = item_pos_end;
                spell_id++;
            }
        }

        terminalSaveScreen();
        displaySpellsList(spell_index, spell_id, true, -1);
        waitForContinueKey(0);
        terminalRestoreScreen();
    }
}

// Go up one level -RAK-
static void dungeonGoUpLevel() {
    uint8_t tile_id = dg.floor[py.row][py.col].treasure_id;

    if (tile_id != 0 && treasure_list[tile_id].category_id == TV_UP_STAIR) {
        dg.current_level--;

        printMessage("You enter a maze of up staircases.");
        printMessage("You pass through a one-way door.");

        dg.generate_new_level = true;
    } else {
        printMessage("I see no up staircase here.");
        game.player_free_turn = true;
    }
}

// Go down one level -RAK-
static void dungeonGoDownLevel() {
    uint8_t tile_id = dg.floor[py.row][py.col].treasure_id;

    if (tile_id != 0 && treasure_list[tile_id].category_id == TV_DOWN_STAIR) {
        dg.current_level++;

        printMessage("You enter a maze of down staircases.");
        printMessage("You pass through a one-way door.");

        dg.generate_new_level = true;
    } else {
        printMessage("I see no down staircase here.");
        game.player_free_turn = true;
    }
}

// Jam a closed door -RAK-
static void dungeonJamDoor() {
    game.player_free_turn = true;

    int y = py.row;
    int x = py.col;

    int direction;
    if (!getDirectionWithMemory(CNIL, direction)) {
        return;
    }
    (void) playerMovePosition(direction, y, x);

    Tile_t const &tile = dg.floor[y][x];

    if (tile.treasure_id == 0) {
        printMessage("That isn't a door!");
        return;
    }

    Inventory_t &item = treasure_list[tile.treasure_id];

    uint8_t item_id = item.category_id;
    if (item_id != TV_CLOSED_DOOR && item_id != TV_OPEN_DOOR) {
        printMessage("That isn't a door!");
        return;
    }

    if (item_id == TV_OPEN_DOOR) {
        printMessage("The door must be closed first.");
        return;
    }

    // If we reach here, the door is closed and we can try to jam it -MRC-

    if (tile.creature_id == 0) {
        int item_pos_start, item_pos_end;
        if (inventoryFindRange(TV_SPIKE, TV_NEVER, item_pos_start, item_pos_end)) {
            game.player_free_turn = false;

            printMessageNoCommandInterrupt("You jam the door with a spike.");

            if (item.misc_use > 0) {
                // Make locked to stuck.
                item.misc_use = -item.misc_use;
            }

            // Successive spikes have a progressively smaller effect.
            // Series is: 0 20 30 37 43 48 52 56 60 64 67 70 ...
            item.misc_use -= 1 + 190 / (10 - item.misc_use);

            if (inventory[item_pos_start].items_count > 1) {
                inventory[item_pos_start].items_count--;
                py.inventory_weight -= inventory[item_pos_start].weight;
            } else {
                inventoryDestroyItem(item_pos_start);
            }
        } else {
            printMessage("But you have no spikes.");
        }
    } else {
        game.player_free_turn = false;

        vtype_t msg = {'\0'};
        (void) sprintf(msg, "The %s is in your way!", creatures_list[monsters[tile.creature_id].creature_id].name);
        printMessage(msg);
    }
}

// Refill the players lamp -RAK-
static void inventoryRefillLamp() {
    game.player_free_turn = true;

    if (inventory[EQUIPMENT_LIGHT].sub_category_id != 0) {
        printMessage("But you are not using a lamp.");
        return;
    }

    int item_pos_start, item_pos_end;
    if (!inventoryFindRange(TV_FLASK, TV_NEVER, item_pos_start, item_pos_end)) {
        printMessage("You have no oil.");
        return;
    }

    game.player_free_turn = false;

    Inventory_t &item = inventory[EQUIPMENT_LIGHT];
    item.misc_use += inventory[item_pos_start].misc_use;

    if (item.misc_use > OBJECT_LAMP_MAX_CAPACITY) {
        item.misc_use = OBJECT_LAMP_MAX_CAPACITY;
        printMessage("Your lamp overflows, spilling oil on the ground.");
        printMessage("Your lamp is full.");
    } else if (item.misc_use > OBJECT_LAMP_MAX_CAPACITY / 2) {
        printMessage("Your lamp is more than half full.");
    } else if (item.misc_use == OBJECT_LAMP_MAX_CAPACITY / 2) {
        printMessage("Your lamp is half full.");
    } else {
        printMessage("Your lamp is less than half full.");
    }

    itemTypeRemainingCountDescription(item_pos_start);
    inventoryDestroyItem(item_pos_start);
}

// Main procedure for dungeon. -RAK-
static void playDungeon() {
    // Note: There is a lot of preliminary magic going on here at first
    playerInitializePlayerLight();
    playerUpdateMaxDungeonDepth();
    resetDungeonFlags();

    // Initialize find counter to `0`
    int find_count = 0;

    // Ensure we display the panel. Used to do this with a global var. -CJS-
    dg.panel.row = dg.panel.col = -1;

    // Light up the area around character
    dungeonResetView();

    // must do this after `dg.panel.row` / `dg.panel.col` set to -1, because playerSearchOff() will
    // call dungeonResetView(), and so the panel_* variables must be valid before
    // playerSearchOff() is called
    if ((py.flags.status & PY_SEARCH) != 0u) {
        playerSearchOff();
    }

    // Light,  but do not move critters
    updateMonsters(false);

    // Print the depth
    printCharacterCurrentDepth();

    // Note: yes, this last input command needs to be persisted
    // over different iterations of the main loop below -MRC-
    char lastInputCommand = {0};

    // Loop until dead,  or new level
    // Exit when `dg.generate_new_level` and `eof_flag` are both set
    do {
        // Increment turn counter
        dg.game_turn++;

        // turn over the store contents every, say, 1000 turns
        if (dg.current_level != 0 && dg.game_turn % 1000 == 0) {
            storeMaintenance();
        }

        // Check for creature generation
        if (randomNumber(MON_CHANCE_OF_NEW) == 1) {
            monsterPlaceNewWithinDistance(1, MON_MAX_SIGHT, false);
        }

        playerUpdateLightStatus();

        //
        // Update counters and messages
        //

        // Heroism and Super Heroism must precede anything that can damage player
        playerUpdateHeroStatus();

        int regen_amount = playerFoodConsumption();
        playerUpdateRegeneration(regen_amount);

        playerUpdateBlindness();
        playerUpdateConfusion();
        playerUpdateFearState();
        playerUpdatePoisonedState();
        playerUpdateSpeed();
        playerUpdateRestingState();

        // Check for interrupts to find or rest.
        int microseconds = (py.running_tracker != 0 ? 0 : 10000);
        if ((game.command_count > 0 || (py.running_tracker != 0) || py.flags.rest != 0) && checkForNonBlockingKeyPress(microseconds)) {
            playerDisturb(0, 0);
        }

        playerUpdateHallucination();
        playerUpdateParalysis();
        playerUpdateEvilProtection();
        playerUpdateInvulnerability();
        playerUpdateBlessedness();
        playerUpdateHeatResistance();
        playerUpdateColdResistance();
        playerUpdateDetectInvisible();
        playerUpdateInfraVision();
        playerUpdateWordOfRecall();

        // Random teleportation
        if (py.flags.teleport && randomNumber(100) == 1) {
            playerDisturb(0, 0);
            playerTeleport(40);
        }

        // See if we are too weak to handle the weapon or pack. -CJS-
        if ((py.flags.status & PY_STR_WGT) != 0u) {
            playerStrength();
        }

        if ((py.flags.status & PY_STUDY) != 0u) {
            printCharacterStudyInstruction();
        }

        playerUpdateStatusFlags();

        // Allow for a slim chance of detect enchantment -CJS-
        // for 1st level char, check once every 2160 turns
        // for 40th level char, check once every 416 turns
        int chance = 10 + 750 / (5 + py.misc.level);
        if ((dg.game_turn & 0xF) == 0 && py.flags.confused == 0 && randomNumber(chance) == 1) {
            playerDetectEnchantment();
        }

        // Check the state of the monster list, and delete some monsters if
        // the monster list is nearly full.  This helps to avoid problems in
        // creature.c when monsters try to multiply.  Compact_monsters() is
        // much more likely to succeed if called from here, than if called
        // from within updateMonsters().
        if (MON_TOTAL_ALLOCATIONS - next_free_monster_id < 10) {
            (void) compactMonsters();
        }

        // Accept a command?
        if (py.flags.paralysis < 1 && py.flags.rest == 0 && !game.character_is_dead) {
            executeInputCommands(lastInputCommand, find_count);
        } else {
            // if paralyzed, resting, or dead, flush output
            // but first move the cursor onto the player, for aesthetics
            panelMoveCursor(Coord_t{py.row, py.col});
            putQIO();
        }

        // Teleport?
        if (teleport_player) {
            playerTeleport(100);
        }

        // Move the creatures
        if (!dg.generate_new_level) {
            updateMonsters(true);
        }
    } while (!dg.generate_new_level && (eof_flag == 0));
}
