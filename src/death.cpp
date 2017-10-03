// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Code executed when player dies

#include "headers.h"
#include "externs.h"

// Prints the gravestone of the character -RAK-
static void printTomb() {
    char *p = nullptr;
    vtype_t str = {'\0'};
    vtype_t tmp_str = {'\0'};

    clearScreen();
    putString("_______________________", Coord_t{1, 15});
    putString("/", Coord_t{2, 14});
    putString("\\         ___", Coord_t{2, 38});
    putString("/", Coord_t{3, 13});
    putString("\\ ___   /   \\      ___", Coord_t{3, 39});
    putString("/            RIP            \\   \\  :   :     /   \\", Coord_t{4, 12});
    putString("/", Coord_t{5, 11});
    putString("\\  : _;,,,;_    :   :", Coord_t{5, 41});
    (void) sprintf(str, "/%s\\,;_          _;,,,;_", centerString(tmp_str, py.misc.name));
    putString(str, Coord_t{6, 10});
    putString("|               the               |   ___", Coord_t{7, 9});
    if (!total_winner) {
        p = playerTitle();
    } else {
        p = (char *) "Magnificent";
    }
    (void) sprintf(str, "| %s |  /   \\", centerString(tmp_str, p));
    putString(str, Coord_t{8, 9});
    putString("|", Coord_t{9, 9});
    putString("|  :   :", Coord_t{9, 43});
    if (!total_winner) {
        p = (char *) classes[py.misc.class_id].title;
    } else if (playerIsMale()) {
        p = (char *) "*King*";
    } else {
        p = (char *) "*Queen*";
    }
    (void) sprintf(str, "| %s | _;,,,;_   ____", centerString(tmp_str, p));
    putString(str, Coord_t{10, 9});
    (void) sprintf(str, "Level : %d", (int) py.misc.level);
    (void) sprintf(str, "| %s |          /    \\", centerString(tmp_str, str));
    putString(str, Coord_t{11, 9});
    (void) sprintf(str, "%d Exp", py.misc.exp);
    (void) sprintf(str, "| %s |          :    :", centerString(tmp_str, str));
    putString(str, Coord_t{12, 9});
    (void) sprintf(str, "%d Au", py.misc.au);
    (void) sprintf(str, "| %s |          :    :", centerString(tmp_str, str));
    putString(str, Coord_t{13, 9});
    (void) sprintf(str, "Died on Level : %d", current_dungeon_level);
    (void) sprintf(str, "| %s |         _;,,,,;_", centerString(tmp_str, str));
    putString(str, Coord_t{14, 9});
    putString("|            killed by            |", Coord_t{15, 9});
    p = character_died_from;

    auto len = (int) strlen(p);
    p[len] = '.'; // add a trailing period
    p[len + 1] = '\0';
    (void) sprintf(str, "| %s |", centerString(tmp_str, p));
    putString(str, Coord_t{16, 9});
    p[len] = '\0'; // strip off the period

    char day[11];
    humanDateString(day);
    (void) sprintf(str, "| %s |", centerString(tmp_str, day));

    putString(str, Coord_t{17, 9});
    putString("*|   *     *     *    *   *     *  | *", Coord_t{18, 8});
    putString("________)/\\\\_)_/___(\\/___(//_\\)/_\\//__\\\\(/_|_)_______", Coord_t{19, 0});

    retry:
    flushInputBuffer();

    putString("(ESC to abort, return to print on screen, or file name)", Coord_t{23, 0});
    putString("Character record?", Coord_t{22, 0});

    if (getStringInput(str, 22, 18, 60)) {
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

    // Let the player know they did good.
    clearScreen();
    putString("#", Coord_t{1, 34});
    putString("#####", Coord_t{2, 32});
    putString("#", Coord_t{3, 34});
    putString(",,,  $$$  ,,,", Coord_t{4, 28});
    putString(",,=$   \"$$$$$\"   $=,,", Coord_t{5, 24});
    putString(",$$        $$$        $$,", Coord_t{6, 22});
    putString("*>         <*>         <*", Coord_t{7, 22});
    putString("$$         $$$         $$", Coord_t{8, 22});
    putString("\"$$        $$$        $$\"", Coord_t{9, 22});
    putString("\"$$       $$$       $$\"", Coord_t{10, 23});
    p = "*#########*#########*";
    putString(p, Coord_t{11, 24});
    putString(p, Coord_t{12, 24});
    putString("Veni, Vidi, Vici!", Coord_t{15, 26});
    putString("I came, I saw, I conquered!", Coord_t{16, 21});
    if (playerIsMale()) {
        putString("All Hail the Mighty King!", Coord_t{17, 22});
    } else {
        putString("All Hail the Mighty Queen!", Coord_t{17, 22});
    }
    flushInputBuffer();
    waitForContinueKey(23);
}

// What happens upon dying -RAK-
// Handles the gravestone and top-twenty routines -RAK-
void exitGame() {
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
        recordNewHighScore();
        showScoresScreen();
    }
    eraseLine(Coord_t{23, 0});
    terminalRestore();

    exit(0);
}
