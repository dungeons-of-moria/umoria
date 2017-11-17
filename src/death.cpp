// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Code executed when player dies

#include "headers.h"
#include "externs.h"

// Prints the gravestone of the character -RAK-
void printTomb() {
    displayDeathFile(DEATH_TOMB);

    std::string text;

    text = std::string(py.misc.name);
    putString(text.c_str(), Coord_t{6, (int) (26 - text.length() / 2)});

    if (!total_winner) {
        text = playerTitle();
    } else {
        text = "Magnificent";
    }
    putString(text.c_str(), Coord_t{8, (int) (26 - text.length() / 2)});

    if (!total_winner) {
        text = classes[py.misc.class_id].title;
    } else if (playerIsMale()) {
        text = "*King*";
    } else {
        text = "*Queen*";
    }
    putString(text.c_str(), Coord_t{10, (int) (26 - text.length() / 2)});

    text = std::to_string(py.misc.level);
    putString(text.c_str(), Coord_t{11, 30});

    text = std::to_string(py.misc.exp) + " Exp";
    putString(text.c_str(), Coord_t{12, (int) (26 - text.length() / 2)});

    text = std::to_string(py.misc.au) + " Au";
    putString(text.c_str(), Coord_t{13, (int) (26 - text.length() / 2)});

    text = std::to_string(dg.current_level);
    putString(text.c_str(), Coord_t{14, 34});

    text = std::string(character_died_from);
    putString(text.c_str(), Coord_t{16, (int) (26 - text.length() / 2)});

    char day[11];
    humanDateString(day);
    text = std::string(day);
    putString(text.c_str(), Coord_t{17, (int) (26 - text.length() / 2)});

    retry:
    flushInputBuffer();

    putString("(ESC to abort, return to print on screen, or file name)", Coord_t{23, 0});
    putString("Character record?", Coord_t{22, 0});

    vtype_t str = {'\0'};
    if (getStringInput(str, Coord_t{22, 18}, 60)) {
        for (auto &item : inventory) {
            itemSetAsIdentified(item.category_id, item.sub_category_id);
            spellItemIdentifyAndRemoveRandomInscription(item);
        }

        playerRecalculateBonuses();

        if (str[0] != 0) {
            if (!outputPlayerCharacterToFile(str)) {
                goto retry;
            }
        } else {
            clearScreen();
            printCharacter();
            putString("Type ESC to skip the inventory:", Coord_t{23, 0});
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

// Let the player know they did good.
static void printCrown() {
    displayDeathFile(DEATH_ROYAL);
    if (playerIsMale()) {
        putString("King!", Coord_t{17, 45});
    } else {
        putString("Queen!", Coord_t{17, 45});
    }
    flushInputBuffer();
    waitForContinueKey(23);
}

// Change the player into a King! -RAK-
static void kingly() {
    // Change the character attributes.
    dg.current_level = 0;
    (void) strcpy(character_died_from, "Ripe Old Age");

    (void) spellRestorePlayerLevels();

    py.misc.level += PLAYER_MAX_LEVEL;
    py.misc.au += 250000L;
    py.misc.max_exp += 5000000L;
    py.misc.exp = py.misc.max_exp;

    printCrown();
}

// What happens upon dying -RAK-
// Handles the gravestone and top-twenty routines -RAK-
void exitGame() {
    printMessage(CNIL);

    // flush all input
    flushInputBuffer();

    // If the game has been saved, then save sets turn back to -1,
    // which inhibits the printing of the tomb.
    if (dg.game_turn >= 0) {
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
        recordNewHighScore();
        showScoresScreen();
    }
    eraseLine(Coord_t{23, 0});
    terminalRestore();

    exit(0);
}
