// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Version history and info, and wizard mode debugging aids.

#include "headers.h"
#include "externs.h"

// Light up the dungeon -RAK-
void wizardLightUpDungeon() {
    bool flag;

    flag = !cave[char_row][char_col].pl;

    for (int y = 0; y < dungeon_height; y++) {
        for (int x = 0; x < dungeon_width; x++) {
            if (cave[y][x].fval <= MAX_CAVE_FLOOR) {
                for (int yy = y - 1; yy <= y + 1; yy++) {
                    for (int xx = x - 1; xx <= x + 1; xx++) {
                        Cave_t *tile = &cave[yy][xx];
                        tile->pl = flag;
                        if (!flag) {
                            tile->fm = false;
                        }
                    }
                }
            }
        }
    }

    drawDungeonPanel();
}

// Wizard routine for gaining on stats -RAK-
void wizardCharacterAdjustment() {
    int number;
    vtype_t input;

    putStringClearToEOL("(3 - 118) Strength     = ", 0, 0);
    if (getStringInput(input, 0, 25, 3)) {
        number = atoi(input);
        if (number > 2 && number < 119) {
            py.stats.max_stat[A_STR] = (uint8_t) number;
            (void) playerStatRestore(A_STR);
        }
    } else {
        return;
    }

    putStringClearToEOL("(3 - 118) Intelligence = ", 0, 0);
    if (getStringInput(input, 0, 25, 3)) {
        number = atoi(input);
        if (number > 2 && number < 119) {
            py.stats.max_stat[A_INT] = (uint8_t) number;
            (void) playerStatRestore(A_INT);
        }
    } else {
        return;
    }

    putStringClearToEOL("(3 - 118) Wisdom       = ", 0, 0);
    if (getStringInput(input, 0, 25, 3)) {
        number = atoi(input);
        if (number > 2 && number < 119) {
            py.stats.max_stat[A_WIS] = (uint8_t) number;
            (void) playerStatRestore(A_WIS);
        }
    } else {
        return;
    }

    putStringClearToEOL("(3 - 118) Dexterity    = ", 0, 0);
    if (getStringInput(input, 0, 25, 3)) {
        number = atoi(input);
        if (number > 2 && number < 119) {
            py.stats.max_stat[A_DEX] = (uint8_t) number;
            (void) playerStatRestore(A_DEX);
        }
    } else {
        return;
    }

    putStringClearToEOL("(3 - 118) Constitution = ", 0, 0);
    if (getStringInput(input, 0, 25, 3)) {
        number = atoi(input);
        if (number > 2 && number < 119) {
            py.stats.max_stat[A_CON] = (uint8_t) number;
            (void) playerStatRestore(A_CON);
        }
    } else {
        return;
    }

    putStringClearToEOL("(3 - 118) Charisma     = ", 0, 0);
    if (getStringInput(input, 0, 25, 3)) {
        number = atoi(input);
        if (number > 2 && number < 119) {
            py.stats.max_stat[A_CHR] = (uint8_t) number;
            (void) playerStatRestore(A_CHR);
        }
    } else {
        return;
    }

    putStringClearToEOL("(1 - 32767) Hit points = ", 0, 0);
    if (getStringInput(input, 0, 25, 5)) {
        number = atoi(input);
        if (number > 0 && number <= MAX_SHORT) {
            py.misc.max_hp = (int16_t) number;
            py.misc.chp = (int16_t) number;
            py.misc.chp_frac = 0;
            printCharacterMaxHitPoints();
            printCharacterCurrentHitPoints();
        }
    } else {
        return;
    }

    putStringClearToEOL("(0 - 32767) Mana       = ", 0, 0);
    if (getStringInput(input, 0, 25, 5)) {
        number = atoi(input);
        if (number > -1 && number <= MAX_SHORT && (*input != '\0')) {
            py.misc.mana = (int16_t) number;
            py.misc.cmana = (int16_t) number;
            py.misc.cmana_frac = 0;
            printCharacterCurrentMana();
        }
    } else {
        return;
    }

    (void) sprintf(input, "Current=%d  Gold = ", py.misc.au);
    number = (int) strlen(input);
    putStringClearToEOL(input, 0, 0);
    if (getStringInput(input, 0, number, 7)) {
        int32_t new_gold = (int32_t) atol(input);
        if (new_gold > -1 && (*input != '\0')) {
            py.misc.au = new_gold;
            printCharacterGoldValue();
        }
    } else {
        return;
    }

    (void) sprintf(input, "Current=%d  (0-200) Searching = ", py.misc.chance_in_search);
    number = (int) strlen(input);
    putStringClearToEOL(input, 0, 0);
    if (getStringInput(input, 0, number, 3)) {
        number = atoi(input);
        if (number > -1 && number < 201 && (*input != '\0')) {
            py.misc.chance_in_search = (int16_t) number;
        }
    } else {
        return;
    }

    (void) sprintf(input, "Current=%d  (-1-18) Stealth = ", py.misc.stealth_factor);
    number = (int) strlen(input);
    putStringClearToEOL(input, 0, 0);
    if (getStringInput(input, 0, number, 3)) {
        number = atoi(input);
        if (number > -2 && number < 19 && (*input != '\0')) {
            py.misc.stealth_factor = (int16_t) number;
        }
    } else {
        return;
    }

    (void) sprintf(input, "Current=%d  (0-200) Disarming = ", py.misc.disarm);
    number = (int) strlen(input);
    putStringClearToEOL(input, 0, 0);
    if (getStringInput(input, 0, number, 3)) {
        number = atoi(input);
        if (number > -1 && number < 201 && (*input != '\0')) {
            py.misc.disarm = (int16_t) number;
        }
    } else {
        return;
    }

    (void) sprintf(input, "Current=%d  (0-100) Save = ", py.misc.saving_throw);
    number = (int) strlen(input);
    putStringClearToEOL(input, 0, 0);
    if (getStringInput(input, 0, number, 3)) {
        number = atoi(input);
        if (number > -1 && number < 201 && (*input != '\0')) {
            py.misc.saving_throw = (int16_t) number;
        }
    } else {
        return;
    }

    (void) sprintf(input, "Current=%d  (0-200) Base to hit = ", py.misc.bth);
    number = (int) strlen(input);
    putStringClearToEOL(input, 0, 0);
    if (getStringInput(input, 0, number, 3)) {
        number = atoi(input);
        if (number > -1 && number < 201 && (*input != '\0')) {
            py.misc.bth = (int16_t) number;
        }
    } else {
        return;
    }

    (void) sprintf(input, "Current=%d  (0-200) Bows/Throwing = ", py.misc.bth_with_bows);
    number = (int) strlen(input);
    putStringClearToEOL(input, 0, 0);
    if (getStringInput(input, 0, number, 3)) {
        number = atoi(input);
        if (number > -1 && number < 201 && (*input != '\0')) {
            py.misc.bth_with_bows = (int16_t) number;
        }
    } else {
        return;
    }

    (void) sprintf(input, "Current=%d  Weight = ", py.misc.weight);
    number = (int) strlen(input);
    putStringClearToEOL(input, 0, 0);
    if (getStringInput(input, 0, number, 3)) {
        number = atoi(input);
        if (number > -1 && (*input != '\0')) {
            py.misc.weight = (uint16_t) number;
        }
    } else {
        return;
    }

    while (getCommand("Alter speed? (+/-)", input)) {
        if (*input == '+') {
            playerChangeSpeed(-1);
        } else if (*input == '-') {
            playerChangeSpeed(1);
        } else {
            break;
        }
        printCharacterSpeed();
    }
}

// Wizard routine for creating objects -RAK-
void wizardCreateObjects() {
    int number;
    vtype_t input;

    printMessage("Warning: This routine can cause a fatal error.");

    Inventory_t forge;
    Inventory_t *item = &forge;

    item->index = OBJ_WIZARD;
    item->name2 = 0;
    itemReplaceInscription(item, "wizard item");
    item->ident = ID_KNOWN2 | ID_STORE_BOUGHT;

    putStringClearToEOL("Tval   : ", 0, 0);
    if (!getStringInput(input, 0, 9, 3)) {
        return;
    }
    number = atoi(input);
    item->tval = (uint8_t) number;

    putStringClearToEOL("Tchar  : ", 0, 0);
    if (!getStringInput(input, 0, 9, 1)) {
        return;
    }
    item->tchar = (uint8_t) input[0];

    putStringClearToEOL("Subval : ", 0, 0);
    if (!getStringInput(input, 0, 9, 5)) {
        return;
    }
    number = atoi(input);
    item->subval = (uint8_t) number;

    putStringClearToEOL("Weight : ", 0, 0);
    if (!getStringInput(input, 0, 9, 5)) {
        return;
    }
    number = atoi(input);
    item->weight = (uint16_t) number;

    putStringClearToEOL("Number : ", 0, 0);
    if (!getStringInput(input, 0, 9, 5)) {
        return;
    }
    number = atoi(input);
    item->number = (uint8_t) number;

    putStringClearToEOL("Damage (dice): ", 0, 0);
    if (!getStringInput(input, 0, 15, 3)) {
        return;
    }
    number = atoi(input);
    item->damage[0] = (uint8_t) number;

    putStringClearToEOL("Damage (sides): ", 0, 0);
    if (!getStringInput(input, 0, 16, 3)) {
        return;
    }
    number = atoi(input);
    item->damage[1] = (uint8_t) number;

    putStringClearToEOL("+To hit: ", 0, 0);
    if (!getStringInput(input, 0, 9, 3)) {
        return;
    }
    number = atoi(input);
    item->tohit = (int16_t) number;

    putStringClearToEOL("+To dam: ", 0, 0);
    if (!getStringInput(input, 0, 9, 3)) {
        return;
    }
    number = atoi(input);
    item->todam = (int16_t) number;

    putStringClearToEOL("AC     : ", 0, 0);
    if (!getStringInput(input, 0, 9, 3)) {
        return;
    }
    number = atoi(input);
    item->ac = (int16_t) number;

    putStringClearToEOL("+To AC : ", 0, 0);
    if (!getStringInput(input, 0, 9, 3)) {
        return;
    }
    number = atoi(input);
    item->toac = (int16_t) number;

    putStringClearToEOL("P1     : ", 0, 0);
    if (!getStringInput(input, 0, 9, 5)) {
        return;
    }
    number = atoi(input);
    item->p1 = (int16_t) number;

    putStringClearToEOL("Flags (In HEX): ", 0, 0);
    if (!getStringInput(input, 0, 16, 8)) {
        return;
    }

    // can't be constant string, this causes problems with
    // the GCC compiler and some scanf routines.
    char pattern[4];

    (void) strcpy(pattern, "%lx");

    int32_t input_number;
    (void) sscanf(input, pattern, &input_number);
    item->flags = (uint32_t) input_number;

    putStringClearToEOL("Cost : ", 0, 0);
    if (!getStringInput(input, 0, 9, 8)) {
        return;
    }
    input_number = (int) atol(input);
    item->cost = input_number;

    putStringClearToEOL("Level : ", 0, 0);
    if (!getStringInput(input, 0, 10, 3)) {
        return;
    }
    number = atoi(input);
    item->level = (uint8_t) number;

    if (getInputConfirmation("Allocate?")) {
        // delete object first if any, before call popt()
        Cave_t *tile = &cave[char_row][char_col];

        if (tile->tptr != 0) {
            (void) dungeonDeleteObject(char_row, char_col);
        }

        number = popt();

        treasure_list[number] = forge;
        tile->tptr = (uint8_t) number;

        printMessage("Allocated.");
    } else {
        printMessage("Aborted.");
    }
}
