// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// The main command interpreter, updating player status

#include "headers.h"
#include "externs.h"

static char originalCommands(char command);
static void doCommand(char command);
static bool validCountCommand(char command);
static void playerRegenerateHitPoints(int percent);
static void playerRegenerateMana(int percent);
static bool itemEnchanted(Inventory_t *item);
static void examineBook();
static void dungeonGoUpLevel();
static void dungeonGoDownLevel();
static void dungeonJamDoor();
static void inventoryRefillLamp();

// Moria game module -RAK-
// The code in this section has gone through many revisions, and
// some of it could stand some more hard work. -RAK-

// It has had a bit more hard work. -CJS-

// Reset flags and initialize variables
static void resetDungeonFlags() {
    command_count = 0;
    generate_new_level = false;
    running_counter = 0;
    teleport_player = false;
    monster_multiply_total = 0;
    cave[char_row][char_col].creature_id = 1;
}

// Check light status for dungeon setup
static void playerInitializePlayerLight() {
    player_carrying_light = (inventory[EQUIPMENT_LIGHT].misc_use > 0);
}

// Check for a maximum level
static void playerUpdateMaxDungeonDepth() {
    if (current_dungeon_level > py.misc.max_dungeon_depth) {
        py.misc.max_dungeon_depth = (uint16_t) current_dungeon_level;
    }
}

// Check light status
static void playerUpdateLightStatus() {
    Inventory_t *i_ptr = &inventory[EQUIPMENT_LIGHT];

    if (player_carrying_light) {
        if (i_ptr->misc_use > 0) {
            i_ptr->misc_use--;

            if (i_ptr->misc_use == 0) {
                player_carrying_light = false;
                printMessage("Your light has gone out!");
                playerDisturb(0, 1);

                // unlight creatures
                updateMonsters(false);
            } else if (i_ptr->misc_use < 40 && randomNumber(5) == 1 && py.flags.blind < 1) {
                playerDisturb(0, 0);
                printMessage("Your light is growing faint.");
            }
        } else {
            player_carrying_light = false;
            playerDisturb(0, 1);

            // unlight creatures
            updateMonsters(false);
        }
    } else if (i_ptr->misc_use > 0) {
        i_ptr->misc_use--;
        player_carrying_light = true;
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
        if ((PY_HERO & py.flags.status) == 0) {
            playerActivateHeroism();
        }

        py.flags.heroism--;

        if (py.flags.heroism == 0) {
            playerDisableHeroism();
        }
    }

    // Super Heroism
    if (py.flags.super_heroism > 0) {
        if ((PY_SHERO & py.flags.status) == 0) {
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

            if ((PY_WEAK & py.flags.status) == 0) {
                py.flags.status |= PY_WEAK;
                printMessage("You are getting weak from hunger.");
                playerDisturb(0, 0);
                printCharacterHungerstatus();
            }

            if (py.flags.food < PLAYER_FOOD_FAINT && randomNumber(8) == 1) {
                py.flags.paralysis += randomNumber(5);
                printMessage("You faint from the lack of food.");
                playerDisturb(1, 0);
            }
        } else if ((PY_HUNGRY & py.flags.status) == 0) {
            py.flags.status |= PY_HUNGRY;
            printMessage("You are getting hungry.");
            playerDisturb(0, 0);
            printCharacterHungerstatus();
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

    if ((PY_BLIND & py.flags.status) == 0) {
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

    if ((PY_CONFUSED & py.flags.status) == 0) {
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

    if ((PY_FEAR & py.flags.status) == 0) {
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

    if ((PY_POISONED & py.flags.status) == 0) {
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
            damage = ((current_game_turn % 2) == 0 ? 1 : 0);
            break;
        case 4:
        case 5:
            damage = ((current_game_turn % 3) == 0 ? 1 : 0);
            break;
        case 6:
            damage = ((current_game_turn % 4) == 0 ? 1 : 0);
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

    if ((PY_FAST & py.flags.status) == 0) {
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

    if ((PY_SLOW & py.flags.status) == 0) {
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

    if ((PY_INVULN & py.flags.status) == 0) {
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

    if ((PY_BLESSED & py.flags.status) == 0) {
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

    if ((PY_DET_INV & py.flags.status) == 0) {
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

    if ((PY_TIM_INFRA & py.flags.status) == 0) {
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
        generate_new_level = true;

        py.flags.paralysis++;
        py.flags.word_of_recall = 0;

        if (current_dungeon_level > 0) {
            current_dungeon_level = 0;
            printMessage("You feel yourself yanked upwards!");
        } else if (py.misc.max_dungeon_depth != 0) {
            current_dungeon_level = py.misc.max_dungeon_depth;
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
        if (i == inventory_count) {
            i = 22;
        }

        Inventory_t *i_ptr = &inventory[i];

        // if in inventory, succeed 1 out of 50 times,
        // if in equipment list, success 1 out of 10 times
        int chance = (i < 22 ? 50 : 10);

        if (i_ptr->category_id != TV_NOTHING && itemEnchanted(i_ptr) && randomNumber(chance) == 1) {
            // extern const char *describe_use(int); // FIXME: Why here? We have it in externs.

            vtype_t tmp_str = {'\0'};
            (void) sprintf(tmp_str, "There's something about what you are %s...", playerItemWearingDescription(i));
            playerDisturb(0, 0);
            printMessage(tmp_str);
            itemAppendToInscription(i_ptr, ID_MAGIK);
        }
    }
}

// Accept a command and execute it
static void executeInputCommands(char *command, int *find_count) {
    char lastInputCommand = *command;

    // Accept a command and execute it
    do {
        if ((py.flags.status & PY_REPEAT) != 0u) {
            printCharacterMovementState();
        }

        use_last_direction = false;
        player_free_turn = false;

        if (running_counter != 0) {
            playerRunAndFind();
            find_count--;
            if (find_count == 0) {
                playerEndRunning();
            }
            putQIO();
        } else if (doing_inventory_command != 0) {
            inventoryExecuteCommand(doing_inventory_command);
        } else {
            // move the cursor to the players character
            moveCursorRelative(char_row, char_col);

            if (command_count > 0) {
                message_ready_to_print = false;
                use_last_direction = true;
            } else {
                message_ready_to_print = false;
                lastInputCommand = getKeyInput();

                int counter = 0;

                // Get a count for a command.
                if ((config.use_roguelike_keys && lastInputCommand >= '0' && lastInputCommand <= '9') || (!config.use_roguelike_keys && lastInputCommand == '#')) {
                    char tmp[8];

                    putStringClearToEOL("Repeat count:", 0, 0);

                    if (lastInputCommand == '#') {
                        lastInputCommand = '0';
                    }

                    counter = 0;

                    while (true) {
                        if (lastInputCommand == DELETE || lastInputCommand == CTRL_KEY('H')) {
                            counter /= 10;
                            (void) sprintf(tmp, "%d", (int16_t) counter);
                            putStringClearToEOL(tmp, 0, 14);
                        } else if (lastInputCommand >= '0' && lastInputCommand <= '9') {
                            if (counter > 99) {
                                terminalBellSound();
                            } else {
                                counter = counter * 10 + lastInputCommand - '0';
                                (void) sprintf(tmp, "%d", counter);
                                putStringClearToEOL(tmp, 0, 14);
                            }
                        } else {
                            break;
                        }
                        lastInputCommand = getKeyInput();
                    }

                    if (counter == 0) {
                        counter = 99;
                        (void) sprintf(tmp, "%d", counter);
                        putStringClearToEOL(tmp, 0, 14);
                    }

                    // a special hack to allow numbers as commands
                    if (lastInputCommand == ' ') {
                        putStringClearToEOL("Command:", 0, 20);
                        lastInputCommand = getKeyInput();
                    }
                }

                // Another way of typing control codes -CJS-
                if (lastInputCommand == '^') {
                    if (command_count > 0) {
                        printCharacterMovementState();
                    }

                    if (getCommand("Control-", &lastInputCommand)) {
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
                }

                // move cursor to player char again, in case it moved
                moveCursorRelative(char_row, char_col);

                // Commands are always converted to rogue form. -CJS-
                if (!config.use_roguelike_keys) {
                    lastInputCommand = originalCommands(lastInputCommand);
                }

                if (counter > 0) {
                    if (!validCountCommand(lastInputCommand)) {
                        player_free_turn = true;
                        lastInputCommand = ' ';
                        printMessage("Invalid command with a count.");
                    } else {
                        command_count = counter;
                        printCharacterMovementState();
                    }
                }
            }

            // Flash the message line.
            eraseLine(MSG_LINE, 0);
            moveCursorRelative(char_row, char_col);
            putQIO();

            doCommand(lastInputCommand);

            // Find is counted differently, as the command changes.
            if (running_counter != 0) {
                *find_count = command_count - 1;
                command_count = 0;
            } else if (player_free_turn) {
                command_count = 0;
            } else if (command_count != 0) {
                command_count--;
            }
        }
    } while (player_free_turn && !generate_new_level && (eof_flag == 0));

    *command = lastInputCommand;
}

// Main procedure for dungeon. -RAK-
void playDungeon() {
    // Note: There is a lot of preliminary magic going on here at first
    playerInitializePlayerLight();
    playerUpdateMaxDungeonDepth();
    resetDungeonFlags();

    // Initialize find counter to `0`
    int find_count = 0;

    // Ensure we display the panel. Used to do this with a global var. -CJS-
    panel_row = panel_col = -1;

    // Light up the area around character
    dungeonResetView();

    // must do this after panel_row/col set to -1, because playerSearchOff() will
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
    // Exit when `generate_new_level` and `eof_flag` are both set
    do {
        // Increment turn counter
        current_game_turn++;

        // turn over the store contents every, say, 1000 turns
        if (current_dungeon_level != 0 && current_game_turn % 1000 == 0) {
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
        int microseconds = (running_counter != 0 ? 0 : 10000);
        if ((command_count > 0 || (running_counter != 0) || py.flags.rest != 0) && checkForNonBlockingKeyPress(microseconds)) {
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
        if ((current_game_turn & 0xF) == 0 && py.flags.confused == 0 && randomNumber(chance) == 1) {
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
        if (py.flags.paralysis < 1 && py.flags.rest == 0 && !character_is_dead) {
            executeInputCommands(&lastInputCommand, &find_count);
        } else {
            // if paralyzed, resting, or dead, flush output
            // but first move the cursor onto the player, for aesthetics
            moveCursorRelative(char_row, char_col);
            putQIO();
        }

        // Teleport?
        if (teleport_player) {
            playerTeleport(100);
        }

        // Move the creatures
        if (!generate_new_level) {
            updateMonsters(true);
        }
    } while (!generate_new_level && (eof_flag == 0));
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
            if (getDirectionWithMemory(CNIL, &direction)) {
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
            if (getDirectionWithMemory(CNIL, &direction)) {
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

    // Save current command_count as getDirectionWithMemory() may change it
    int countSave = command_count;

    if (getDirectionWithMemory(CNIL, &dir_val)) {
        // Restore command_count
        command_count = countSave;

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
        character_is_dead = true;
        generate_new_level = true;

        (void) strcpy(character_died_from, "Quitting");
    }
}

static uint8_t calculateMaxMessageCount() {
    uint8_t max_messages = MESSAGE_HISTORY_SIZE;

    if (command_count > 0) {
        if (command_count < MESSAGE_HISTORY_SIZE) {
            max_messages = (uint8_t) command_count;
        }
        command_count = 0;
    } else if (last_command != CTRL_KEY('P')) {
        max_messages = 1;
    }

    return max_messages;
}

static void commandPreviousMessage() {
    uint8_t max_messages = calculateMaxMessageCount();

    int16_t msg_id = last_message_id;

    if (max_messages > 1) {
        terminalSaveScreen();

        uint8_t lineNumber = max_messages;

        while(max_messages > 0) {
            max_messages--;

            putStringClearToEOL(messages[msg_id], max_messages, 0);

            if (msg_id == 0) {
                msg_id = MESSAGE_HISTORY_SIZE - 1;
            } else {
                msg_id--;
            }
        }

        eraseLine(lineNumber, 0);
        waitForContinueKey(lineNumber);
        terminalRestoreScreen();
    } else {
        // Distinguish real and recovered messages with a '>'. -CJS-
        putString(">", 0, 0);
        putStringClearToEOL(messages[msg_id], 0, 1);
    }
}

static void commandFlipWizardMode() {
    if (wizard_mode) {
        wizard_mode = false;
        printMessage("Wizard mode off.");
    } else if (enterWizardMode()) {
        printMessage("Wizard mode on.");
    }

    printCharacterWinner();
}

static void commandSaveAndExit() {
    if (total_winner) {
        printMessage("You are a Total Winner,  your character must be retired.");

        if (config.use_roguelike_keys) {
            printMessage("Use 'Q' to when you are ready to quit.");
        } else {
            printMessage("Use <Control>-K when you are ready to quit.");
        }
    } else {
        (void) strcpy(character_died_from, "(saved)");
        printMessage("Saving game...");

        if (saveGame()) {
            exitGame();
        }

        (void) strcpy(character_died_from, "(alive and well)");
    }
}

static void commandLocateOnMap() {
    if (py.flags.blind > 0 || playerNoLight()) {
        printMessage("You can't see your map.");
        return;
    }

    int y = char_row;
    int x = char_col;
    if (coordOutsidePanel(y, x, true)) {
        drawDungeonPanel();
    }

    int cy, cx, p_y, p_x;

    cy = panel_row;
    cx = panel_col;

    int dir_val;
    vtype_t out_val = {'\0'};
    vtype_t tmp_str = {'\0'};

    while (true) {
        p_y = panel_row;
        p_x = panel_col;

        if (p_y == cy && p_x == cx) {
            tmp_str[0] = '\0';
        } else {
            (void) sprintf(tmp_str, "%s%s of", p_y < cy ? " North" : p_y > cy ? " South" : "", p_x < cx ? " West" : p_x > cx ? " East" : "");
        }

        (void) sprintf(out_val, "Map sector [%d,%d], which is%s your sector. Look which direction?", p_y, p_x, tmp_str);

        if (!getDirectionWithMemory(out_val, &dir_val)) {
            break;
        }

        // -CJS-
        // Should really use the move function, but what the hell. This
        // is nicer, as it moves exactly to the same place in another
        // section. The direction calculation is not intuitive. Sorry.
        while (true) {
            x += ((dir_val - 1) % 3 - 1) * SCREEN_WIDTH / 2;
            y -= ((dir_val - 1) / 3 - 1) * SCREEN_HEIGHT / 2;

            if (x < 0 || y < 0 || x >= dungeon_width || y >= dungeon_width) {
                printMessage("You've gone past the end of your map.");

                x -= ((dir_val - 1) % 3 - 1) * SCREEN_WIDTH / 2;
                y += ((dir_val - 1) / 3 - 1) * SCREEN_HEIGHT / 2;

                break;
            }

            if (coordOutsidePanel(y, x, true)) {
                drawDungeonPanel();
                break;
            }
        }
    }

    // Move to a new panel - but only if really necessary.
    if (coordOutsidePanel(char_row, char_col, false)) {
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
        case CTRL_KEY('A'): // ^A = Cure all
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
        case CTRL_KEY('E'): // ^E = wizchar
            wizardCharacterAdjustment();
            eraseLine(MSG_LINE, 0);
            break;
        case CTRL_KEY('F'): // ^F = genocide
            (void) spellMassGenocide();
            break;
        case CTRL_KEY('G'): // ^G = treasure
            if (command_count > 0) {
                i = command_count;
                command_count = 0;
            } else {
                i = 1;
            }
            dungeonPlaceRandomObjectNear(char_row, char_col, i);

            drawDungeonPanel();
            break;
        case CTRL_KEY('D'): // ^D = up/down
            if (command_count > 0) {
                if (command_count > 99) {
                    i = 0;
                } else {
                    i = command_count;
                }
                command_count = 0;
            } else {
                i = -1;
                vtype_t input = {0};

                putStringClearToEOL("Go to which level (0-99) ? ", 0, 0);

                if (getStringInput(input, 0, 27, 10)) {
                    (void) stringToNumber(input, &i);
                }
            }

            if (i >= 0) {
                current_dungeon_level = (int16_t) i;
                if (current_dungeon_level > 99) {
                    current_dungeon_level = 99;
                }
                generate_new_level = true;
            } else {
                eraseLine(MSG_LINE, 0);
            }
            break;
        case CTRL_KEY('O'): // ^O = objects
            outputRandomLevelObjectsToFile();
            break;
        case '\\': // \ wizard help
            if (config.use_roguelike_keys) {
                displayTextHelpFile(MORIA_RL_WIZARD_HELP);
            } else {
                displayTextHelpFile(MORIA_WIZARD_HELP);
            }
            break;
        case CTRL_KEY('I'): // ^I = identify
            (void) spellIdentifyItem();
            break;
        case '*':
            wizardLightUpDungeon();
            break;
        case ':':
            spellMapCurrentArea();
            break;
        case CTRL_KEY('T'): // ^T = teleport
            playerTeleport(100);
            break;
        case '+':
            if (command_count > 0) {
                py.misc.exp = command_count;
                command_count = 0;
            } else if (py.misc.exp == 0) {
                py.misc.exp = 1;
            } else {
                py.misc.exp = py.misc.exp * 2;
            }
            displayCharacterExperience();
            break;
        case '&': // & = summon
            y = char_row;
            x = char_col;
            (void) monsterSummon(&y, &x, true);

            updateMonsters(false);
            break;
        case '@':
            wizardCreateObjects();
            break;
        default:
            if (config.use_roguelike_keys) {
                putStringClearToEOL("Type '?' or '\\' for help.", 0, 0);
            } else {
                putStringClearToEOL("Type '?' or ^H for help.", 0, 0);
            }
    }
}

static void doCommand(char command) {
    bool do_pickup = moveWithoutPickup(&command);

    switch (command) {
        case 'Q': // (Q)uit    (^K)ill
            commandQuit();
            player_free_turn = true;
            break;
        case CTRL_KEY('P'): // (^P)revious message.
            commandPreviousMessage();
            player_free_turn = true;
            break;
        case CTRL_KEY('V'): // (^V)iew license
            displayTextHelpFile(MORIA_LICENSE);
            player_free_turn = true;
            break;
        case CTRL_KEY('W'): // (^W)izard mode
            commandFlipWizardMode();
            player_free_turn = true;
            break;
        case CTRL_KEY('X'): // e(^X)it and save
            commandSaveAndExit();
            player_free_turn = true;
            break;
        case '=': // (=) set options
            terminalSaveScreen();
            setGameOptions();
            terminalRestoreScreen();
            player_free_turn = true;
            break;
        case '{': // ({) inscribe an object
            itemInscribe();
            player_free_turn = true;
            break;
        case '!': // (!) escape to the shell
        case '$':
            // escaping to shell disabled -MRC-
            player_free_turn = true;
            break;
        case ESCAPE: // (ESC)   do nothing.
        case ' ':    // (space) do nothing.
            player_free_turn = true;
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
            displayWorldObjectDescription();
            player_free_turn = true;
            break;
        case '.': // (.) stay in one place (5)
            playerMove(5, do_pickup);

            if (command_count > 1) {
                command_count--;
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
                displayTextHelpFile(MORIA_RL_HELP);
            } else {
                displayTextHelpFile(MORIA_HELP);
            }
            player_free_turn = true;
            break;
        case 'f': // (f)orce    (B)ash
            playerBash();
            break;
        case 'C': // (C)haracter description
            terminalSaveScreen();
            changeCharacterName();
            terminalRestoreScreen();
            player_free_turn = true;
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
            player_free_turn = true;
            break;
        case 'W': // (W)here are we on the map  (L)ocate on map
            commandLocateOnMap();
            player_free_turn = true;
            break;
        case 'R': // (R)est a while
            playerRestOn();
            break;
        case '#': // (#) search toggle  (S)earch toggle
            commandToggleSearch();
            player_free_turn = true;
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
            displayDungeonMap();
            player_free_turn = true;
            break;
        case 'P': // (P)eruse a book  (B)rowse in a book
            examineBook();
            player_free_turn = true;
            break;
        case 'c': // (c)lose an object
            dungeonCloseDoor();
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
            player_free_turn = true;
            break;
        case 'm': // (m)agic spells
            getAndCastMagicSpell();
            break;
        case 'o': // (o)pen something
            objectOpen();
            break;
        case 'p': // (p)ray
            pray();
            break;
        case 'q': // (q)uaff
            quaff();
            break;
        case 'r': // (r)ead
            readScroll();
            break;
        case 's': // (s)earch for a turn
            dungeonSearch(char_row, char_col, py.misc.chance_in_search);
            break;
        case 'T': // (T)ake off something  (t)ake off
            inventoryExecuteCommand('t');
            break;
        case 'Z': // (Z)ap a staff  (u)se a staff
            useStaff();
            break;
        case 'v': // (v)ersion of game
            displayTextHelpFile(MORIA_VERSIONS);
            player_free_turn = true;
            break;
        case 'w': // (w)ear or wield
            inventoryExecuteCommand('w');
            break;
        case 'X': // e(X)change weapons  e(x)change
            inventoryExecuteCommand('x');
            break;
        default:
            // Wizard commands are free moves
            player_free_turn = true;

            if (wizard_mode) {
                doWizardCommands(command);
            } else {
                putStringClearToEOL("Type '?' for help.", 0, 0);
            }
    }
    last_command = command;
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
static bool itemEnchanted(Inventory_t *item) {
    if (item->category_id < TV_MIN_ENCHANT || item->category_id > TV_MAX_ENCHANT || ((item->flags & TR_CURSED) != 0u)) {
        return false;
    } else if (spellItemIdentified(item)) {
        return false;
    } else if ((item->identification & ID_MAGIK) != 0) {
        return false;
    } else if (item->to_hit > 0 || item->to_damage > 0 || item->to_ac > 0) {
        return true;
    } else if (((0x4000107fL & item->flags) != 0) && item->misc_use > 0) {
        return true;
    } else if ((0x07ffe980L & item->flags) != 0) {
        return true;
    }
    return false;
}

// Examine a Book -RAK-
static void examineBook() {
    int item_pos_start, item_pos_end;
    if (!inventoryFindRange(TV_MAGIC_BOOK, TV_PRAYER_BOOK, &item_pos_start, &item_pos_end)) {
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
    if (inventoryGetInputForItemId(&item_id, "Which Book?", item_pos_start, item_pos_end, CNIL, CNIL)) {
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
            item_pos_end = getAndClearFirstBit(&item_flags);

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
    uint8_t tile_id = cave[char_row][char_col].treasure_id;

    if (tile_id != 0 && treasure_list[tile_id].category_id == TV_UP_STAIR) {
        current_dungeon_level--;

        printMessage("You enter a maze of up staircases.");
        printMessage("You pass through a one-way door.");

        generate_new_level = true;
    } else {
        printMessage("I see no up staircase here.");
        player_free_turn = true;
    }
}

// Go down one level -RAK-
static void dungeonGoDownLevel() {
    uint8_t tile_id = cave[char_row][char_col].treasure_id;

    if (tile_id != 0 && treasure_list[tile_id].category_id == TV_DOWN_STAIR) {
        current_dungeon_level++;

        printMessage("You enter a maze of down staircases.");
        printMessage("You pass through a one-way door.");

        generate_new_level = true;
    } else {
        printMessage("I see no down staircase here.");
        player_free_turn = true;
    }
}

// Jam a closed door -RAK-
static void dungeonJamDoor() {
    player_free_turn = true;

    int y = char_row;
    int x = char_col;

    int direction;
    if (!getDirectionWithMemory(CNIL, &direction)) {
        return;
    }
    (void) playerMovePosition(direction, &y, &x);

    Cave_t *tile = &cave[y][x];

    if (tile->treasure_id == 0) {
        printMessage("That isn't a door!");
        return;
    }

    Inventory_t *item = &treasure_list[tile->treasure_id];

    uint8_t item_id = item->category_id;
    if (item_id != TV_CLOSED_DOOR && item_id != TV_OPEN_DOOR) {
        printMessage("That isn't a door!");
        return;
    }

    if (item_id == TV_OPEN_DOOR) {
        printMessage("The door must be closed first.");
        return;
    }

    // If we reach here, the door is closed and we can try to jam it -MRC-

    if (tile->creature_id == 0) {
        int item_pos_start, item_pos_end;
        if (inventoryFindRange(TV_SPIKE, TV_NEVER, &item_pos_start, &item_pos_end)) {
            player_free_turn = false;

            printMessageNoCommandInterrupt("You jam the door with a spike.");

            if (item->misc_use > 0) {
                // Make locked to stuck.
                item->misc_use = -item->misc_use;
            }

            // Successive spikes have a progressively smaller effect.
            // Series is: 0 20 30 37 43 48 52 56 60 64 67 70 ...
            item->misc_use -= 1 + 190 / (10 - item->misc_use);

            if (inventory[item_pos_start].items_count > 1) {
                inventory[item_pos_start].items_count--;
                inventory_weight -= inventory[item_pos_start].weight;
            } else {
                inventoryDestroyItem(item_pos_start);
            }
        } else {
            printMessage("But you have no spikes.");
        }
    } else {
        player_free_turn = false;

        vtype_t msg = {'\0'};
        (void) sprintf(msg, "The %s is in your way!", creatures_list[monsters[tile->creature_id].creature_id].name);
        printMessage(msg);
    }
}

// Refill the players lamp -RAK-
static void inventoryRefillLamp() {
    player_free_turn = true;

    if (inventory[EQUIPMENT_LIGHT].sub_category_id != 0) {
        printMessage("But you are not using a lamp.");
        return;
    }

    int item_pos_start, item_pos_end;
    if (!inventoryFindRange(TV_FLASK, TV_NEVER, &item_pos_start, &item_pos_end)) {
        printMessage("You have no oil.");
        return;
    }

    player_free_turn = false;

    Inventory_t *item = &inventory[EQUIPMENT_LIGHT];
    item->misc_use += inventory[item_pos_start].misc_use;

    if (item->misc_use > OBJECT_LAMP_MAX_CAPACITY) {
        item->misc_use = OBJECT_LAMP_MAX_CAPACITY;
        printMessage("Your lamp overflows, spilling oil on the ground.");
        printMessage("Your lamp is full.");
    } else if (item->misc_use > OBJECT_LAMP_MAX_CAPACITY / 2) {
        printMessage("Your lamp is more than half full.");
    } else if (item->misc_use == OBJECT_LAMP_MAX_CAPACITY / 2) {
        printMessage("Your lamp is half full.");
    } else {
        printMessage("Your lamp is less than half full.");
    }

    itemTypeRemainingCountDescription(item_pos_start);
    inventoryDestroyItem(item_pos_start);
}
