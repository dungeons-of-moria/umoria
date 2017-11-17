// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Version history and info, and wizard mode debugging aids.

#include "headers.h"
#include "externs.h"

#include <sstream>

// Light up the dungeon -RAK-
void wizardLightUpDungeon() {
    bool flag;

    flag = !dg.floor[char_row][char_col].permanent_light;

    for (int y = 0; y < dg.height; y++) {
        for (int x = 0; x < dg.width; x++) {
            if (dg.floor[y][x].feature_id <= MAX_CAVE_FLOOR) {
                for (int yy = y - 1; yy <= y + 1; yy++) {
                    for (int xx = x - 1; xx <= x + 1; xx++) {
                        dg.floor[yy][xx].permanent_light = flag;
                        if (!flag) {
                            dg.floor[yy][xx].field_mark = false;
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
    vtype_t input = {'\0'};

    putStringClearToEOL("(3 - 118) Strength     = ", Coord_t{0, 0});
    if (getStringInput(input, Coord_t{0, 25}, 3)) {
        bool valid_number = stringToNumber(input, number);
        if (valid_number && number > 2 && number < 119) {
            py.stats.max[A_STR] = (uint8_t) number;
            (void) playerStatRestore(A_STR);
        }
    } else {
        return;
    }

    putStringClearToEOL("(3 - 118) Intelligence = ", Coord_t{0, 0});
    if (getStringInput(input, Coord_t{0, 25}, 3)) {
        bool valid_number = stringToNumber(input, number);
        if (valid_number && number > 2 && number < 119) {
            py.stats.max[A_INT] = (uint8_t) number;
            (void) playerStatRestore(A_INT);
        }
    } else {
        return;
    }

    putStringClearToEOL("(3 - 118) Wisdom       = ", Coord_t{0, 0});
    if (getStringInput(input, Coord_t{0, 25}, 3)) {
        bool valid_number = stringToNumber(input, number);
        if (valid_number && number > 2 && number < 119) {
            py.stats.max[A_WIS] = (uint8_t) number;
            (void) playerStatRestore(A_WIS);
        }
    } else {
        return;
    }

    putStringClearToEOL("(3 - 118) Dexterity    = ", Coord_t{0, 0});
    if (getStringInput(input, Coord_t{0, 25}, 3)) {
        bool valid_number = stringToNumber(input, number);
        if (valid_number && number > 2 && number < 119) {
            py.stats.max[A_DEX] = (uint8_t) number;
            (void) playerStatRestore(A_DEX);
        }
    } else {
        return;
    }

    putStringClearToEOL("(3 - 118) Constitution = ", Coord_t{0, 0});
    if (getStringInput(input, Coord_t{0, 25}, 3)) {
        bool valid_number = stringToNumber(input, number);
        if (valid_number && number > 2 && number < 119) {
            py.stats.max[A_CON] = (uint8_t) number;
            (void) playerStatRestore(A_CON);
        }
    } else {
        return;
    }

    putStringClearToEOL("(3 - 118) Charisma     = ", Coord_t{0, 0});
    if (getStringInput(input, Coord_t{0, 25}, 3)) {
        bool valid_number = stringToNumber(input, number);
        if (valid_number && number > 2 && number < 119) {
            py.stats.max[A_CHR] = (uint8_t) number;
            (void) playerStatRestore(A_CHR);
        }
    } else {
        return;
    }

    putStringClearToEOL("(1 - 32767) Hit points = ", Coord_t{0, 0});
    if (getStringInput(input, Coord_t{0, 25}, 5)) {
        bool valid_number = stringToNumber(input, number);
        if (valid_number && number > 0 && number <= MAX_SHORT) {
            py.misc.max_hp = (int16_t) number;
            py.misc.current_hp = (int16_t) number;
            py.misc.current_hp_fraction = 0;
            printCharacterMaxHitPoints();
            printCharacterCurrentHitPoints();
        }
    } else {
        return;
    }

    putStringClearToEOL("(0 - 32767) Mana       = ", Coord_t{0, 0});
    if (getStringInput(input, Coord_t{0, 25}, 5)) {
        bool valid_number = stringToNumber(input, number);
        if (valid_number && number > -1 && number <= MAX_SHORT) {
            py.misc.mana = (int16_t) number;
            py.misc.current_mana = (int16_t) number;
            py.misc.current_mana_fraction = 0;
            printCharacterCurrentMana();
        }
    } else {
        return;
    }

    (void) sprintf(input, "Current=%d  Gold = ", py.misc.au);
    number = (int) strlen(input);
    putStringClearToEOL(input, Coord_t{0, 0});
    if (getStringInput(input, Coord_t{0, number}, 7)) {
        int new_gold;
        bool valid_number = stringToNumber(input, new_gold);
        if (valid_number && new_gold > -1) {
            py.misc.au = new_gold;
            printCharacterGoldValue();
        }
    } else {
        return;
    }

    (void) sprintf(input, "Current=%d  (0-200) Searching = ", py.misc.chance_in_search);
    number = (int) strlen(input);
    putStringClearToEOL(input, Coord_t{0, 0});
    if (getStringInput(input, Coord_t{0, number}, 3)) {
        int new_gold;
        bool valid_number = stringToNumber(input, new_gold);
        if (valid_number && number > -1 && number < 201) {
            py.misc.chance_in_search = (int16_t) number;
        }
    } else {
        return;
    }

    (void) sprintf(input, "Current=%d  (-1-18) Stealth = ", py.misc.stealth_factor);
    number = (int) strlen(input);
    putStringClearToEOL(input, Coord_t{0, 0});
    if (getStringInput(input, Coord_t{0, number}, 3)) {
        bool valid_number = stringToNumber(input, number);
        if (valid_number && number > -2 && number < 19) {
            py.misc.stealth_factor = (int16_t) number;
        }
    } else {
        return;
    }

    (void) sprintf(input, "Current=%d  (0-200) Disarming = ", py.misc.disarm);
    number = (int) strlen(input);
    putStringClearToEOL(input, Coord_t{0, 0});
    if (getStringInput(input, Coord_t{0, number}, 3)) {
        bool valid_number = stringToNumber(input, number);
        if (valid_number && number > -1 && number < 201) {
            py.misc.disarm = (int16_t) number;
        }
    } else {
        return;
    }

    (void) sprintf(input, "Current=%d  (0-100) Save = ", py.misc.saving_throw);
    number = (int) strlen(input);
    putStringClearToEOL(input, Coord_t{0, 0});
    if (getStringInput(input, Coord_t{0, number}, 3)) {
        bool valid_number = stringToNumber(input, number);
        if (valid_number && number > -1 && number < 201) {
            py.misc.saving_throw = (int16_t) number;
        }
    } else {
        return;
    }

    (void) sprintf(input, "Current=%d  (0-200) Base to hit = ", py.misc.bth);
    number = (int) strlen(input);
    putStringClearToEOL(input, Coord_t{0, 0});
    if (getStringInput(input, Coord_t{0, number}, 3)) {
        bool valid_number = stringToNumber(input, number);
        if (valid_number && number > -1 && number < 201) {
            py.misc.bth = (int16_t) number;
        }
    } else {
        return;
    }

    (void) sprintf(input, "Current=%d  (0-200) Bows/Throwing = ", py.misc.bth_with_bows);
    number = (int) strlen(input);
    putStringClearToEOL(input, Coord_t{0, 0});
    if (getStringInput(input, Coord_t{0, number}, 3)) {
        bool valid_number = stringToNumber(input, number);
        if (valid_number && number > -1 && number < 201) {
            py.misc.bth_with_bows = (int16_t) number;
        }
    } else {
        return;
    }

    (void) sprintf(input, "Current=%d  Weight = ", py.misc.weight);
    number = (int) strlen(input);
    putStringClearToEOL(input, Coord_t{0, 0});
    if (getStringInput(input, Coord_t{0, number}, 3)) {
        bool valid_number = stringToNumber(input, number);
        if (valid_number && number > -1) {
            py.misc.weight = (uint16_t) number;
        }
    } else {
        return;
    }

    while (getCommand("Alter speed? (+/-)", *input)) {
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

// Request user input to get the array index of the `game_objects[]`
static bool wizardRequestObjectId(int &id, const std::string &label, int start_id, int end_id) {
    std::ostringstream id_str;
    id_str << start_id << "-" << end_id;

    std::string msg = label + " ID (" + id_str.str() + "): ";
    putStringClearToEOL(msg.c_str(), Coord_t{0, 0});

    vtype_t input = {0};
    if (!getStringInput(input, Coord_t{0, (int) msg.length()}, 3)) {
        return false;
    }

    int given_id;
    if (!stringToNumber(input, given_id)) {
        return false;
    }

    if (given_id < start_id || given_id > end_id) {
        putStringClearToEOL(("Invalid ID. Must be " + id_str.str()).c_str(), Coord_t{0, 0});
        return false;
    }
    id = given_id;

    return true;
}

// Simplified wizard routine for creating an object
void wizardGenerateObject() {
    int id;
    if (!wizardRequestObjectId(id, "Dungeon/Store object", 0, 366)) return;

    for (int i = 0; i < 10; i++) {
        int j = char_row - 3 + randomNumber(5);
        int k = char_col - 4 + randomNumber(7);

        if (coordInBounds(Coord_t{j, k}) && dg.floor[j][k].feature_id <= MAX_CAVE_FLOOR && dg.floor[j][k].treasure_id == 0) {
            // delete any object at location, before call popt()
            Tile_t &tile = dg.floor[j][k];
            if (tile.treasure_id != 0) {
                (void) dungeonDeleteObject(j, k);
            }

            // place the object
            int free_treasure_id = popt();
            dg.floor[j][k].treasure_id = (uint8_t) free_treasure_id;
            inventoryItemCopyTo(id, treasure_list[free_treasure_id]);
            magicTreasureMagicalAbility(free_treasure_id, dg.current_level);

            // auto identify the item
            itemIdentify(treasure_list[free_treasure_id], free_treasure_id);

            i = 9;
        }
    }
}

// Wizard routine for creating objects -RAK-
void wizardCreateObjects() {
    int number;
    vtype_t input = {0};

    printMessage("Warning: This routine can cause a fatal error.");

    Inventory_t forge{};
    Inventory_t &item = forge;

    item.id = OBJ_WIZARD;
    item.special_name_id = 0;
    itemReplaceInscription(item, "wizard item");
    item.identification = ID_KNOWN2 | ID_STORE_BOUGHT;

    putStringClearToEOL("Tval   : ", Coord_t{0, 0});
    if (!getStringInput(input, Coord_t{0, 9}, 3)) {
        return;
    }
    if (stringToNumber(input, number)) {
        item.category_id = (uint8_t) number;
    }

    putStringClearToEOL("Tchar  : ", Coord_t{0, 0});
    if (!getStringInput(input, Coord_t{0, 9}, 1)) {
        return;
    }
    item.sprite = (uint8_t) input[0];

    putStringClearToEOL("Subval : ", Coord_t{0, 0});
    if (!getStringInput(input, Coord_t{0, 9}, 5)) {
        return;
    }
    if (stringToNumber(input, number)) {
        item.sub_category_id = (uint8_t) number;
    }

    putStringClearToEOL("Weight : ", Coord_t{0, 0});
    if (!getStringInput(input, Coord_t{0, 9}, 5)) {
        return;
    }
    if (stringToNumber(input, number)) {
        item.weight = (uint16_t) number;
    }

    putStringClearToEOL("Number : ", Coord_t{0, 0});
    if (!getStringInput(input, Coord_t{0, 9}, 5)) {
        return;
    }
    if (stringToNumber(input, number)) {
        item.items_count = (uint8_t) number;
    }

    putStringClearToEOL("Damage (dice): ", Coord_t{0, 0});
    if (!getStringInput(input, Coord_t{0, 15}, 3)) {
        return;
    }
    if (stringToNumber(input, number)) {
        item.damage[0] = (uint8_t) number;
    }

    putStringClearToEOL("Damage (sides): ", Coord_t{0, 0});
    if (!getStringInput(input, Coord_t{0, 16}, 3)) {
        return;
    }
    if (stringToNumber(input, number)) {
        item.damage[1] = (uint8_t) number;
    }

    putStringClearToEOL("+To hit: ", Coord_t{0, 0});
    if (!getStringInput(input, Coord_t{0, 9}, 3)) {
        return;
    }
    if (stringToNumber(input, number)) {
        item.to_hit = (int16_t) number;
    }

    putStringClearToEOL("+To dam: ", Coord_t{0, 0});
    if (!getStringInput(input, Coord_t{0, 9}, 3)) {
        return;
    }
    if (stringToNumber(input, number)) {
        item.to_damage = (int16_t) number;
    }

    putStringClearToEOL("AC     : ", Coord_t{0, 0});
    if (!getStringInput(input, Coord_t{0, 9}, 3)) {
        return;
    }
    if (stringToNumber(input, number)) {
        item.ac = (int16_t) number;
    }

    putStringClearToEOL("+To AC : ", Coord_t{0, 0});
    if (!getStringInput(input, Coord_t{0, 9}, 3)) {
        return;
    }
    if (stringToNumber(input, number)) {
        item.to_ac = (int16_t) number;
    }

    putStringClearToEOL("P1     : ", Coord_t{0, 0});
    if (!getStringInput(input, Coord_t{0, 9}, 5)) {
        return;
    }
    if (stringToNumber(input, number)) {
        item.misc_use = (int16_t) number;
    }

    putStringClearToEOL("Flags (In HEX): ", Coord_t{0, 0});
    if (!getStringInput(input, Coord_t{0, 16}, 8)) {
        return;
    }

    // can't be constant string, this causes problems with
    // the GCC compiler and some scanf routines.
    char pattern[4];

    (void) strcpy(pattern, "%lx");

    int32_t input_number;
    (void) sscanf(input, pattern, &input_number);
    item.flags = (uint32_t) input_number;

    putStringClearToEOL("Cost : ", Coord_t{0, 0});
    if (!getStringInput(input, Coord_t{0, 9}, 8)) {
        return;
    }
    if (stringToNumber(input, input_number)) {
        item.cost = input_number;
    }

    putStringClearToEOL("Level : ", Coord_t{0, 0});
    if (!getStringInput(input, Coord_t{0, 10}, 3)) {
        return;
    }
    if (stringToNumber(input, number)) {
        item.depth_first_found = (uint8_t) number;
    }

    if (getInputConfirmation("Allocate?")) {
        // delete object first if any, before call popt()
        Tile_t &tile = dg.floor[char_row][char_col];

        if (tile.treasure_id != 0) {
            (void) dungeonDeleteObject(char_row, char_col);
        }

        number = popt();

        treasure_list[number] = forge;
        tile.treasure_id = (uint8_t) number;

        printMessage("Allocated.");
    } else {
        printMessage("Aborted.");
    }
}
