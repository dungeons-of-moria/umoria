// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

#include "headers.h"

static void inventoryItemWeightText(char *text, int itemID) {
    int totalWeight = inventory[itemID].weight * inventory[itemID].items_count;
    int quotient = totalWeight / 10;
    int remainder = totalWeight % 10;

    (void) sprintf(text, "%3d.%d lb", quotient, remainder);
}

// Displays inventory items from `item_id_start` to `item_id_end` -RAK-
// Designed to keep the display as far to the right as possible. -CJS-
// The parameter col gives a column at which to start, but if the display
// does not fit, it may be moved left.  The return value is the left edge
// used. If mask is non-zero, then only display those items which have a
// non-zero entry in the mask array.
int displayInventory(int item_id_start, int item_id_end, bool weighted, int column, const char *mask) {
    vtype_t descriptions[23];

    int len = 79 - column;

    int lim;
    if (weighted) {
        lim = 68;
    } else {
        lim = 76;
    }

    // Generate the descriptions text
    for (int i = item_id_start; i <= item_id_end; i++) {
        if (mask != CNIL && (mask[i] == 0)) {
            continue;
        }

        obj_desc_t description = {'\0'};
        itemDescription(description, inventory[i], true);

        // Truncate if too long.
        description[lim] = 0;

        (void) sprintf(descriptions[i], "%c) %s", 'a' + i, description);

        int l = (int) strlen(descriptions[i]) + 2;

        if (weighted) {
            l += 9;
        }

        if (l > len) {
            len = l;
        }
    }

    column = 79 - len;
    if (column < 0) {
        column = 0;
    }

    int current_line = 1;

    // Print the descriptions
    for (int i = item_id_start; i <= item_id_end; i++) {
        if (mask != CNIL && (mask[i] == 0)) {
            continue;
        }

        // don't need first two spaces if in first column
        if (column == 0) {
            putStringClearToEOL(descriptions[i], Coord_t{current_line, column});
        } else {
            putString("  ", Coord_t{current_line, column});
            putStringClearToEOL(descriptions[i], Coord_t{current_line, column + 2});
        }

        if (weighted) {
            obj_desc_t text = {'\0'};
            inventoryItemWeightText(text, i);
            putStringClearToEOL(text, Coord_t{current_line, 71});
        }

        current_line++;
    }

    return column;
}

// Return a string describing how a given equipment item is carried. -CJS-
const char *playerItemWearingDescription(int body_location) {
    switch (body_location) {
        case EQUIPMENT_WIELD:
            return "wielding";
        case EQUIPMENT_HEAD:
            return "wearing on your head";
        case EQUIPMENT_NECK:
            return "wearing around your neck";
        case EQUIPMENT_BODY:
            return "wearing on your body";
        case EQUIPMENT_ARM:
            return "wearing on your arm";
        case EQUIPMENT_HANDS:
            return "wearing on your hands";
        case EQUIPMENT_RIGHT:
            return "wearing on your right hand";
        case EQUIPMENT_LEFT:
            return "wearing on your left hand";
        case EQUIPMENT_FEET:
            return "wearing on your feet";
        case EQUIPMENT_OUTER:
            return "wearing about your body";
        case EQUIPMENT_LIGHT:
            return "using to light the way";
        case EQUIPMENT_AUX:
            return "holding ready by your side";
        default:
            return "carrying in your pack";
    }
}

static const char *itemPositionDescription(int positionID, uint16_t weight) {
    switch (positionID) {
        case EQUIPMENT_WIELD:
            if (py.stats.used[A_STR] * 15 < weight) {
                return "Just lifting";
            }

            return "Wielding";
        case EQUIPMENT_HEAD:
            return "On head";
        case EQUIPMENT_NECK:
            return "Around neck";
        case EQUIPMENT_BODY:
            return "On body";
        case EQUIPMENT_ARM:
            return "On arm";
        case EQUIPMENT_HANDS:
            return "On hands";
        case EQUIPMENT_RIGHT:
            return "On right hand";
        case EQUIPMENT_LEFT:
            return "On left hand";
        case EQUIPMENT_FEET:
            return "On feet";
        case EQUIPMENT_OUTER:
            return "About body";
        case EQUIPMENT_LIGHT:
            return "Light source";
        case EQUIPMENT_AUX:
            return "Spare weapon";
        default:
            return "Unknown value";
    }
}

// Displays equipment items from r1 to end -RAK-
// Keep display as far right as possible. -CJS-
int displayEquipment(bool weighted, int column) {
    vtype_t descriptions[PLAYER_INVENTORY_SIZE - EQUIPMENT_WIELD];

    int len = 79 - column;

    int lim;
    if (weighted) {
        lim = 52;
    } else {
        lim = 60;
    }

    // Range of equipment
    int line = 0;
    for (int i = EQUIPMENT_WIELD; i < PLAYER_INVENTORY_SIZE; i++) {
        if (inventory[i].category_id == TV_NOTHING) {
            continue;
        }

        // Get position
        const char *position_description = itemPositionDescription(i, inventory[i].weight);

        obj_desc_t description = {'\0'};
        itemDescription(description, inventory[i], true);

        // Truncate if necessary
        description[lim] = 0;

        (void) sprintf(descriptions[line], "%c) %-14s: %s", line + 'a', position_description, description);

        int l = (int) strlen(descriptions[line]) + 2;

        if (weighted) {
            l += 9;
        }

        if (l > len) {
            len = l;
        }

        line++;
    }

    column = 79 - len;
    if (column < 0) {
        column = 0;
    }

    // Range of equipment
    line = 0;
    for (int i = EQUIPMENT_WIELD; i < PLAYER_INVENTORY_SIZE; i++) {
        if (inventory[i].category_id == TV_NOTHING) {
            continue;
        }

        // don't need first two spaces when using whole screen
        if (column == 0) {
            putStringClearToEOL(descriptions[line], Coord_t{line + 1, column});
        } else {
            putString("  ", Coord_t{line + 1, column});
            putStringClearToEOL(descriptions[line], Coord_t{line + 1, column + 2});
        }

        if (weighted) {
            obj_desc_t text = {'\0'};
            inventoryItemWeightText(text, i);
            putStringClearToEOL(text, Coord_t{line + 1, 71});
        }

        line++;
    }
    eraseLine(Coord_t{line + 1, column});

    return column;
}

// TODO: Most of the functions below should probably not be part of the UI,
// TODO: but have dependencies on "screen state" variables, so need to be here for now.

// All inventory commands (wear, exchange, take off, drop, inventory and
// equipment) are handled in an alternative command input mode, which accepts
// any of the inventory commands.
//
// It is intended that this function be called several times in succession,
// as some commands take up a turn, and the rest of moria must proceed in the
// interim. A global variable is provided, game.doing_inventory_command, which is normally
// zero; however if on return from inventoryExecuteCommand() it is expected that
// inventoryExecuteCommand() should be called *again*, (being still in inventory command
// input mode), then game.doing_inventory_command is set to the inventory command character
// which should be used in the next call to inventoryExecuteCommand().
//
// On return, the screen is restored, but not flushed. Provided no flush of
// the screen takes place before the next call to inventoryExecuteCommand(), the inventory
// command screen is silently redisplayed, and no actual output takes place at
// all. If the screen is flushed before a subsequent call, then the player is
// prompted to see if we should continue. This allows the player to see any
// changes that take place on the screen during inventory command input.
//
// The global variable, screen_has_changed, is cleared by inventoryExecuteCommand(), and set
// when the screen is flushed. This is the means by which inventoryExecuteCommand() tell
// if the screen has been flushed.
//
// The display of inventory items is kept to the right of the screen to
// minimize the work done to restore the screen afterwards. -CJS-

// Inventory command screen states.
constexpr int BLANK_SCR = 0;
constexpr int EQUIP_SCR = 1;
constexpr int INVEN_SCR = 2;
constexpr int WEAR_SCR = 3;
constexpr int HELP_SCR = 4;
constexpr int WRONG_SCR = 5;

// Keep track of the state of the inventory screen.
int screen_state, screen_left, screen_base;
int wear_low, wear_high;

static void displayInventoryScreen(int new_screen) {
    if (new_screen == screen_state) {
        return;
    }

    screen_state = new_screen;

    int line;

    switch (new_screen) {
        case BLANK_SCR:
            line = 0;
            break;
        case HELP_SCR:
            if (screen_left > 52) {
                screen_left = 52;
            }

            putStringClearToEOL("  ESC: exit", Coord_t{1, screen_left});
            putStringClearToEOL("  w  : wear or wield object", Coord_t{2, screen_left});
            putStringClearToEOL("  t  : take off item", Coord_t{3, screen_left});
            putStringClearToEOL("  d  : drop object", Coord_t{4, screen_left});
            putStringClearToEOL("  x  : exchange weapons", Coord_t{5, screen_left});
            putStringClearToEOL("  i  : inventory of pack", Coord_t{6, screen_left});
            putStringClearToEOL("  e  : list used equipment", Coord_t{7, screen_left});

            line = 7;
            break;
        case INVEN_SCR:
            screen_left = displayInventory(0, py.unique_inventory_items - 1, config.show_inventory_weights, screen_left, CNIL);
            line = py.unique_inventory_items;
            break;
        case WEAR_SCR:
            screen_left = displayInventory(wear_low, wear_high, config.show_inventory_weights, screen_left, CNIL);
            line = wear_high - wear_low + 1;
            break;
        case EQUIP_SCR:
            screen_left = displayEquipment(config.show_inventory_weights, screen_left);
            line = py.equipment_count;
            break;
        default:
            line = 0;
            break;
    }

    if (line >= screen_base) {
        screen_base = line + 1;
        eraseLine(Coord_t{screen_base, screen_left});
        return;
    }

    while (++line <= screen_base) {
        eraseLine(Coord_t{line, screen_left});
    }
}

// Used to verify if this really is the item we wish to -CJS-
// wear or read.
static bool verify(const char *prompt, int item) {
    obj_desc_t description = {'\0'};
    itemDescription(description, inventory[item], true);

    // change the period to a question mark
    description[strlen(description) - 1] = '?';

    obj_desc_t msg = {'\0'};
    (void) sprintf(msg, "%s %s", prompt, description);

    return getInputConfirmation(msg);
}

static void setInventoryCommandScreenState(char command) {
    // Take up where we left off after a previous inventory command. -CJS-
    if (game.doing_inventory_command != 0) {
        // If the screen has been flushed, we need to redraw. If the command
        // is a simple ' ' to recover the screen, just quit. Otherwise, check
        // and see what the user wants.
        if (screen_has_changed) {
            if (command == ' ' || !getInputConfirmation("Continuing with inventory command?")) {
                game.doing_inventory_command = 0;
                return;
            }
            screen_left = 50;
            screen_base = 0;
        }

        int savedState = screen_state;
        screen_state = WRONG_SCR;
        displayInventoryScreen(savedState);

        return;
    }

    screen_left = 50;
    screen_base = 0;

    // this forces exit of inventoryExecuteCommand() if selecting is not set true
    screen_state = BLANK_SCR;
}

static bool inventoryTakeOffItem(bool selecting) {
    if (py.equipment_count == 0) {
        printMessage("You are not using any equipment.");
        // don't print message restarting inven command after taking off something, it is confusing
        return selecting;
    }

    if (py.unique_inventory_items >= EQUIPMENT_WIELD && (game.doing_inventory_command == 0)) {
        printMessage("You will have to drop something first.");
        return selecting;
    }

    if (screen_state != BLANK_SCR) {
        displayInventoryScreen(EQUIP_SCR);
    }

    return true;
}

static bool inventoryDropItem(char *command, bool selecting) {
    if (py.unique_inventory_items == 0 && py.equipment_count == 0) {
        printMessage("But you're not carrying anything.");
        return selecting;
    }

    if (dg.floor[py.row][py.col].treasure_id != 0) {
        printMessage("There's no room to drop anything here.");
        return selecting;
    }

    if ((screen_state == EQUIP_SCR && py.equipment_count > 0) || py.unique_inventory_items == 0) {
        if (screen_state != BLANK_SCR) {
            displayInventoryScreen(EQUIP_SCR);
        }
        *command = 'r'; // Remove - or take off and drop.
    } else if (screen_state != BLANK_SCR) {
        displayInventoryScreen(INVEN_SCR);
    }

    return true;
}

static bool inventoryWearWieldItem(bool selecting) {
    // Note: simple loop to get wear_low value
    for (wear_low = 0; wear_low < py.unique_inventory_items && inventory[wear_low].category_id > TV_MAX_WEAR; wear_low++);

    // Note: simple loop to get wear_high value
    for (wear_high = wear_low; wear_high < py.unique_inventory_items && inventory[wear_high].category_id >= TV_MIN_WEAR; wear_high++);

    wear_high--;

    if (wear_low > wear_high) {
        printMessage("You have nothing to wear or wield.");
        return selecting;
    }

    if (screen_state != BLANK_SCR && screen_state != INVEN_SCR) {
        displayInventoryScreen(WEAR_SCR);
    }

    return true;
}

static void inventoryUnwieldItem() {
    if (inventory[EQUIPMENT_WIELD].category_id == TV_NOTHING && inventory[EQUIPMENT_AUX].category_id == TV_NOTHING) {
        printMessage("But you are wielding no weapons.");
        return;
    }

    if ((TR_CURSED & inventory[EQUIPMENT_WIELD].flags) != 0u) {
        obj_desc_t description = {'\0'};
        itemDescription(description, inventory[EQUIPMENT_WIELD], false);

        obj_desc_t msg = {'\0'};
        (void) sprintf(msg, "The %s you are wielding appears to be cursed.", description);

        printMessage(msg);

        return;
    }

    game.player_free_turn = false;

    Inventory_t savedItem = inventory[EQUIPMENT_AUX];
    inventory[EQUIPMENT_AUX] = inventory[EQUIPMENT_WIELD];
    inventory[EQUIPMENT_WIELD] = savedItem;

    if (screen_state == EQUIP_SCR) {
        screen_left = displayEquipment(config.show_inventory_weights, screen_left);
    }

    playerAdjustBonusesForItem(inventory[EQUIPMENT_AUX], -1);  // Subtract bonuses
    playerAdjustBonusesForItem(inventory[EQUIPMENT_WIELD], 1); // Add bonuses

    if (inventory[EQUIPMENT_WIELD].category_id != TV_NOTHING) {
        obj_desc_t msgLabel = {'\0'};
        (void) strcpy(msgLabel, "Primary weapon   : ");

        obj_desc_t description = {'\0'};
        itemDescription(description, inventory[EQUIPMENT_WIELD], true);

        printMessage(strcat(msgLabel, description));
    } else {
        printMessage("No primary weapon.");
    }

    // this is a new weapon, so clear the heavy flag
    py.weapon_is_heavy = false;
    playerStrength();
}

// look for item whose inscription matches "which"
static int inventoryGetItemMatchingInscription(char which, char command, int from, int to) {
    int item;

    if (which >= '0' && which <= '9' && command != 'r' && command != 't') {
        int m;

        // Note: simple loop to get id
        for (m = from; m <= to && m < PLAYER_INVENTORY_SIZE && ((inventory[m].inscription[0] != which) || (inventory[m].inscription[1] != '\0')); m++);

        if (m <= to) {
            item = m;
        } else {
            item = -1;
        }
    } else if (which >= 'A' && which <= 'Z') {
        item = which - 'A';
    } else {
        item = which - 'a';
    }

    return item;
}

static void buildCommandHeading(char *prt1, int from, int to, const char *swap, char command, const char *prompt) {
    from = from + 'a';
    to = to + 'a';

    const char *listItems = "";
    if (screen_state == BLANK_SCR) {
        listItems = ", * to list";
    }

    const char *digits = "";
    if (command == 'w' || command == 'd') {
        digits = ", 0-9";
    }

    (void) sprintf(prt1, "(%c-%c%s%s%s, space to break, ESC to exit) %s which one?", from, to, listItems, swap, digits, prompt);
}

static void drawInventoryScreenForCommand(char command) {
    if (command == 't' || command == 'r') {
        displayInventoryScreen(EQUIP_SCR);
    } else if (command == 'w' && screen_state != INVEN_SCR) {
        displayInventoryScreen(WEAR_SCR);
    } else {
        displayInventoryScreen(INVEN_SCR);
    }
}

static void swapInventoryScreenForDrop() {
    if (screen_state == EQUIP_SCR) {
        displayInventoryScreen(INVEN_SCR);
    } else if (screen_state == INVEN_SCR) {
        displayInventoryScreen(EQUIP_SCR);
    }
}

static int inventoryGetSlotToWearEquipment(int item) {
    int slot;

    // Slot for equipment
    switch (inventory[item].category_id) {
        case TV_SLING_AMMO:
        case TV_BOLT:
        case TV_ARROW:
        case TV_BOW:
        case TV_HAFTED:
        case TV_POLEARM:
        case TV_SWORD:
        case TV_DIGGING:
        case TV_SPIKE:
            slot = EQUIPMENT_WIELD;
            break;
        case TV_LIGHT:
            slot = EQUIPMENT_LIGHT;
            break;
        case TV_BOOTS:
            slot = EQUIPMENT_FEET;
            break;
        case TV_GLOVES:
            slot = EQUIPMENT_HANDS;
            break;
        case TV_CLOAK:
            slot = EQUIPMENT_OUTER;
            break;
        case TV_HELM:
            slot = EQUIPMENT_HEAD;
            break;
        case TV_SHIELD:
            slot = EQUIPMENT_ARM;
            break;
        case TV_HARD_ARMOR:
        case TV_SOFT_ARMOR:
            slot = EQUIPMENT_BODY;
            break;
        case TV_AMULET:
            slot = EQUIPMENT_NECK;
            break;
        case TV_RING:
            if (inventory[EQUIPMENT_RIGHT].category_id == TV_NOTHING) {
                slot = EQUIPMENT_RIGHT;
            } else if (inventory[EQUIPMENT_LEFT].category_id == TV_NOTHING) {
                slot = EQUIPMENT_LEFT;
            } else {
                slot = 0;

                // Rings. Give choice over where they go.
                do {
                    char query;
                    if (!getCommand("Put ring on which hand (l/r/L/R)?", query)) {
                        slot = -1;
                    } else if (query == 'l') {
                        slot = EQUIPMENT_LEFT;
                    } else if (query == 'r') {
                        slot = EQUIPMENT_RIGHT;
                    } else {
                        if (query == 'L') {
                            slot = EQUIPMENT_LEFT;
                        } else if (query == 'R') {
                            slot = EQUIPMENT_RIGHT;
                        } else {
                            terminalBellSound();
                        }
                        if ((slot != 0) && !verify("Replace", slot)) {
                            slot = 0;
                        }
                    }
                } while (slot == 0);
            }
            break;
        default:
            slot = -1;
            printMessage("IMPOSSIBLE: I don't see how you can use that.");
            break;
    }

    return slot;
}

static void inventoryItemIsCursedMessage(int itemID) {
    obj_desc_t description = {'\0'};
    itemDescription(description, inventory[itemID], false);

    obj_desc_t itemText = {'\0'};
    (void) sprintf(itemText, "The %s you are ", description);

    if (itemID == EQUIPMENT_HEAD) {
        (void) strcat(itemText, "wielding ");
    } else {
        (void) strcat(itemText, "wearing ");
    }

    printMessage(strcat(itemText, "appears to be cursed."));
}

static bool selectItemCommands(char *command, char *which, bool selecting) {
    int itemToTakeOff;
    int slot = 0;

    int from, to;
    const char *prompt = nullptr;
    const char *swap = nullptr;

    while (selecting && game.player_free_turn) {
        swap = "";

        if (*command == 'w') {
            from = wear_low;
            to = wear_high;
            prompt = "Wear/Wield";
        } else {
            from = 0;
            if (*command == 'd') {
                to = py.unique_inventory_items - 1;
                prompt = "Drop";

                if (py.equipment_count > 0) {
                    swap = ", / for Equip";
                }
            } else {
                to = py.equipment_count - 1;

                if (*command == 't') {
                    prompt = "Take off";
                } else {
                    // command == 'r'

                    prompt = "Throw off";
                    if (py.unique_inventory_items > 0) {
                        swap = ", / for Inven";
                    }
                }
            }
        }

        if (from > to) {
            selecting = false;
            continue;
        }

        obj_desc_t headingText = {'\0'};
        buildCommandHeading(headingText, from, to, swap, *command, prompt);

        // Abort everything.
        if (!getCommand(headingText, *which)) {
            *which = ESCAPE;
            selecting = false;
            continue; // can we just return false from the function? -MRC-
        }

        // Draw the screen and maybe exit to main prompt.
        if (*which == ' ' || *which == '*') {
            drawInventoryScreenForCommand(*command);
            if (*which == ' ') {
                selecting = false;
            }
            continue;
        }

        // Swap screens (for drop)
        if (*which == '/' && (swap[0] != 0)) {
            if (*command == 'd') {
                *command = 'r';
            } else {
                *command = 'd';
            }
            swapInventoryScreenForDrop();
            continue;
        }

        // look for item whose inscription matches "which"
        int item_id = inventoryGetItemMatchingInscription(*which, *command, from, to);

        if (item_id < from || item_id > to) {
            terminalBellSound();
            continue;
        }

        // Found an item!

        if (*command == 'r' || *command == 't') {
            // Get its place in the equipment list.
            itemToTakeOff = item_id;
            item_id = 21;

            do {
                item_id++;
                if (inventory[item_id].category_id != TV_NOTHING) {
                    itemToTakeOff--;
                }
            } while (itemToTakeOff >= 0);

            if ((isupper((int) *which) != 0) && !verify((char *) prompt, item_id)) {
                item_id = -1;
            } else if ((TR_CURSED & inventory[item_id].flags) != 0u) {
                item_id = -1;
                printMessage("Hmmm, it seems to be cursed.");
            } else if (*command == 't' && !inventoryCanCarryItemCount(inventory[item_id])) {
                if (dg.floor[py.row][py.col].treasure_id != 0) {
                    item_id = -1;
                    printMessage("You can't carry it.");
                } else if (getInputConfirmation("You can't carry it.  Drop it?")) {
                    *command = 'r';
                } else {
                    item_id = -1;
                }
            }

            if (item_id >= 0) {
                if (*command == 'r') {
                    inventoryDropItem(item_id, true);
                    // As a safety measure, set the player's inven
                    // weight to 0, when the last object is dropped.
                    if (py.unique_inventory_items == 0 && py.equipment_count == 0) {
                        py.inventory_weight = 0;
                    }
                } else {
                    slot = inventoryCarryItem(inventory[item_id]);
                    playerTakeOff(item_id, slot);
                }

                playerStrength();

                game.player_free_turn = false;

                if (*command == 'r') {
                    selecting = false;
                }
            }
        } else if (*command == 'w') {
            // Wearing. Go to a bit of trouble over replacing existing equipment.

            if ((isupper((int) *which) != 0) && !verify((char *) prompt, item_id)) {
                item_id = -1;
            } else {
                slot = inventoryGetSlotToWearEquipment(item_id);
                if (slot == -1) {
                    item_id = -1;
                }
            }

            if (item_id >= 0 && inventory[slot].category_id != TV_NOTHING) {
                if ((TR_CURSED & inventory[slot].flags) != 0u) {
                    inventoryItemIsCursedMessage(slot);
                    item_id = -1;
                } else if (inventory[item_id].sub_category_id == ITEM_GROUP_MIN && inventory[item_id].items_count > 1 && !inventoryCanCarryItemCount(inventory[slot])) {
                    // this can happen if try to wield a torch,
                    // and have more than one in inventory
                    printMessage("You will have to drop something first.");
                    item_id = -1;
                }
            }

            // OK. Wear it.
            if (item_id >= 0) {
                game.player_free_turn = false;

                // first remove new item from inventory
                Inventory_t savedItem = inventory[item_id];
                Inventory_t *item = &savedItem;

                wear_high--;

                // Fix for torches
                if (item->items_count > 1 && item->sub_category_id <= ITEM_SINGLE_STACK_MAX) {
                    item->items_count = 1;
                    wear_high++;
                }

                py.inventory_weight += item->weight * item->items_count;

                // Subtracts weight
                inventoryDestroyItem(item_id);

                // Second, add old item to inv and remove
                // from equipment list, if necessary.
                item = &inventory[slot];
                if (item->category_id != TV_NOTHING) {
                    int savedCounter = py.unique_inventory_items;

                    itemToTakeOff = inventoryCarryItem(*item);

                    // If item removed did not stack with anything
                    // in inventory, then increment wear_high.
                    if (py.unique_inventory_items != savedCounter) {
                        wear_high++;
                    }

                    playerTakeOff(slot, itemToTakeOff);
                }

                // third, wear new item
                *item = savedItem;
                py.equipment_count++;

                playerAdjustBonusesForItem(*item, 1);

                const char *text = nullptr;
                if (slot == EQUIPMENT_WIELD) {
                    text = "You are wielding";
                } else if (slot == EQUIPMENT_LIGHT) {
                    text = "Your light source is";
                } else {
                    text = "You are wearing";
                }

                obj_desc_t description = {'\0'};
                itemDescription(description, *item, true);

                // Get the right equipment letter.
                itemToTakeOff = EQUIPMENT_WIELD;
                item_id = 0;

                while (itemToTakeOff != slot) {
                    if (inventory[itemToTakeOff++].category_id != TV_NOTHING) {
                        item_id++;
                    }
                }

                obj_desc_t msg = {'\0'};
                (void) sprintf(msg, "%s %s (%c)", text, description, 'a' + item_id);
                printMessage(msg);

                // this is a new weapon, so clear heavy flag
                if (slot == EQUIPMENT_WIELD) {
                    py.weapon_is_heavy = false;
                }
                playerStrength();

                if ((item->flags & TR_CURSED) != 0u) {
                    printMessage("Oops! It feels deathly cold!");
                    itemAppendToInscription(*item, ID_DAMD);

                    // To force a cost of 0, even if unidentified.
                    item->cost = -1;
                }
            }
        } else {
            // command == 'd'

            // NOTE: initializing to `ESCAPE` as warnings were being given. -MRC-
            char query = ESCAPE;

            if (inventory[item_id].items_count > 1) {
                obj_desc_t description = {'\0'};
                itemDescription(description, inventory[item_id], true);
                description[strlen(description) - 1] = '?';

                obj_desc_t msg = {'\0'};
                (void) sprintf(msg, "Drop all %s [y/n]", description);
                msg[strlen(description) - 1] = '.';

                putStringClearToEOL(msg, Coord_t{0, 0});

                query = getKeyInput();

                if (query != 'y' && query != 'n') {
                    if (query != ESCAPE) {
                        terminalBellSound();
                    }
                    messageLineClear();
                    item_id = -1;
                }
            } else if ((isupper((int) *which) != 0) && !verify((char *) prompt, item_id)) {
                item_id = -1;
            } else {
                query = 'y';
            }

            if (item_id >= 0) {
                game.player_free_turn = false;

                inventoryDropItem(item_id, query == 'y');
                playerStrength();
            }

            selecting = false;

            // As a safety measure, set the player's inven weight
            // to 0, when the last object is dropped.
            if (py.unique_inventory_items == 0 && py.equipment_count == 0) {
                py.inventory_weight = 0;
            }
        }

        if (!game.player_free_turn && screen_state == BLANK_SCR) {
            selecting = false;
        }
    }

    return selecting;
}

// Put an appropriate header.
static void inventoryDisplayAppropriateHeader() {
    if (screen_state == INVEN_SCR) {
        obj_desc_t msg = {'\0'};
        int weightQuotient = py.inventory_weight / 10;
        int weightRemainder = py.inventory_weight % 10;

        if (!config.show_inventory_weights || py.unique_inventory_items == 0) {
            (void) sprintf(msg, "You are carrying %d.%d pounds. In your pack there is %s",
                           weightQuotient,
                           weightRemainder,
                           (py.unique_inventory_items == 0 ? "nothing." : "-")
            );
        } else {
            int limitQuotient = playerCarryingLoadLimit() / 10;
            int limitRemainder = playerCarryingLoadLimit() % 10;

            (void) sprintf(msg, "You are carrying %d.%d pounds. Your capacity is %d.%d pounds. In your pack is -",
                           weightQuotient,
                           weightRemainder,
                           limitQuotient,
                           limitRemainder
            );
        }

        putStringClearToEOL(msg, Coord_t{0, 0});
    } else if (screen_state == WEAR_SCR) {
        if (wear_high < wear_low) {
            putStringClearToEOL("You have nothing you could wield.", Coord_t{0, 0});
        } else {
            putStringClearToEOL("You could wield -", Coord_t{0, 0});
        }
    } else if (screen_state == EQUIP_SCR) {
        if (py.equipment_count == 0) {
            putStringClearToEOL("You are not using anything.", Coord_t{0, 0});
        } else {
            putStringClearToEOL("You are using -", Coord_t{0, 0});
        }
    } else {
        putStringClearToEOL("Allowed commands:", Coord_t{0, 0});
    }

    eraseLine(Coord_t{screen_base, screen_left});
}

static void displayInventory() {
    if (py.unique_inventory_items == 0) {
        printMessage("You are not carrying anything.");
    } else {
        displayInventoryScreen(INVEN_SCR);
    }
}

static void displayEquipment() {
    if (py.equipment_count == 0) {
        printMessage("You are not using any equipment.");
    } else {
        displayInventoryScreen(EQUIP_SCR);
    }
}

// This does all the work.
void inventoryExecuteCommand(char command) {
    game.player_free_turn = true;

    terminalSaveScreen();
    setInventoryCommandScreenState(command);

    do {
        if (isupper((int) command) != 0) {
            command = (char) tolower((int) command);
        }

        // Simple command getting and screen selection.
        bool selecting = false;
        switch (command) {
            case 'i':
                displayInventory();
                break;
            case 'e':
                displayEquipment();
                break;
            case 't':
                selecting = inventoryTakeOffItem(selecting);
                break;
            case 'd':
                selecting = inventoryDropItem(&command, selecting);
                break;
            case 'w':
                selecting = inventoryWearWieldItem(selecting);
                break;
            case 'x':
                inventoryUnwieldItem();
                break;
            case ' ':
                // Dummy command to return again to main prompt.
                break;
            case '?':
                displayInventoryScreen(HELP_SCR);
                break;
            default:
                // Nonsense command
                terminalBellSound();
                break;
        }

        // Clear the game.doing_inventory_command flag here, instead of at beginning, so that
        // can use it to control when messages above appear.
        game.doing_inventory_command = 0;

        // Keep looking for objects to drop/wear/take off/throw off
        char which = 'z';

        selecting = selectItemCommands(&command, &which, selecting);

        if (which == ESCAPE || screen_state == BLANK_SCR) {
            command = ESCAPE;
        } else if (!game.player_free_turn) {
            // Save state for recovery if they want to call us again next turn.
            // Otherwise, set a dummy command to recover screen.
            if (selecting) {
                game.doing_inventory_command = command;
            } else {
                game.doing_inventory_command = ' ';
            }

            // flush last message before clearing screen_has_changed and exiting
            printMessage(CNIL);

            // This lets us know if the world changes
            screen_has_changed = false;

            command = ESCAPE;
        } else {
            inventoryDisplayAppropriateHeader();

            putString("e/i/t/w/x/d/?/ESC:", Coord_t{screen_base, 60});
            command = getKeyInput();

            eraseLine(Coord_t{screen_base, screen_left});
        }
    } while (command != ESCAPE);

    if (screen_state != BLANK_SCR) {
        terminalRestoreScreen();
    }

    playerRecalculateBonuses();
}

// Get the ID of an item and return the CTR value of it -RAK-
bool inventoryGetInputForItemId(int &command_key_id, const char *prompt, int item_id_start, int item_id_end, char *mask, const char *message) {
    int screen_id = 1;
    bool full = false;

    if (item_id_end > EQUIPMENT_WIELD) {
        full = true;

        if (py.unique_inventory_items == 0) {
            screen_id = 0;
            item_id_end = py.equipment_count - 1;
        } else {
            item_id_end = py.unique_inventory_items - 1;
        }
    }

    if (py.unique_inventory_items < 1 && (!full || py.equipment_count < 1)) {
        putStringClearToEOL("You are not carrying anything.", Coord_t{0, 0});
        return false;
    }

    command_key_id = 0;

    bool item_found = false;
    bool redraw_screen = false;

    do {
        if (redraw_screen) {
            if (screen_id > 0) {
                (void) displayInventory(item_id_start, item_id_end, false, 80, mask);
            } else {
                (void) displayEquipment(false, 80);
            }
        }

        vtype_t description = {'\0'};

        if (full) {
            (void) sprintf(
                    description,
                    "(%s: %c-%c,%s%s / for %s, or ESC) %s",
                    (screen_id > 0 ? "Inven" : "Equip"),
                    item_id_start + 'a',
                    item_id_end + 'a',
                    (screen_id > 0 ? " 0-9," : ""),
                    (redraw_screen ? "" : " * to see,"),
                    (screen_id > 0 ? "Equip" : "Inven"),
                    prompt
            );
        } else {
            (void) sprintf(
                    description,
                    "(Items %c-%c,%s%s ESC to exit) %s",
                    item_id_start + 'a',
                    item_id_end + 'a',
                    (screen_id > 0 ? " 0-9," : ""),
                    (redraw_screen ? "" : " * for inventory list,"),
                    prompt
            );
        }

        putStringClearToEOL(description, Coord_t{0, 0});

        bool command_finished = false;

        while (!command_finished) {
            char which = getKeyInput();

            switch (which) {
                case ESCAPE:
                    screen_id = -1;
                    command_finished = true;

                    game.player_free_turn = true;

                    break;
                case '/':
                    if (full) {
                        if (screen_id > 0) {
                            if (py.equipment_count == 0) {
                                putStringClearToEOL("But you're not using anything -more-", Coord_t{0, 0});
                                (void) getKeyInput();
                            } else {
                                screen_id = 0;
                                command_finished = true;

                                if (redraw_screen) {
                                    item_id_end = py.equipment_count;

                                    while (item_id_end < py.unique_inventory_items) {
                                        item_id_end++;
                                        eraseLine(Coord_t{item_id_end, 0});
                                    }
                                }
                                item_id_end = py.equipment_count - 1;
                            }

                            putStringClearToEOL(description, Coord_t{0, 0});
                        } else {
                            if (py.unique_inventory_items == 0) {
                                putStringClearToEOL("But you're not carrying anything -more-", Coord_t{0, 0});
                                (void) getKeyInput();
                            } else {
                                screen_id = 1;
                                command_finished = true;

                                if (redraw_screen) {
                                    item_id_end = py.unique_inventory_items;

                                    while (item_id_end < py.equipment_count) {
                                        item_id_end++;
                                        eraseLine(Coord_t{item_id_end, 0});
                                    }
                                }
                                item_id_end = py.unique_inventory_items - 1;
                            }
                        }
                    }
                    break;
                case '*':
                    if (!redraw_screen) {
                        command_finished = true;
                        terminalSaveScreen();
                        redraw_screen = true;
                    }
                    break;
                default:
                    // look for item whose inscription matches "which"
                    if (which >= '0' && which <= '9' && screen_id != 0) {
                        int m;

                        // Note: loop to find the inventory item
                        for (m = item_id_start; m < EQUIPMENT_WIELD && (inventory[m].inscription[0] != which || inventory[m].inscription[1] != '\0'); m++);

                        if (m < EQUIPMENT_WIELD) {
                            command_key_id = m;
                        } else {
                            command_key_id = -1;
                        }
                    } else if (isupper((int) which) != 0) {
                        command_key_id = which - 'A';
                    } else {
                        command_key_id = which - 'a';
                    }

                    if (command_key_id >= item_id_start && command_key_id <= item_id_end && (mask == CNIL || (mask[command_key_id] != 0))) {
                        if (screen_id == 0) {
                            item_id_start = 21;
                            item_id_end = command_key_id;

                            do {
                                // Note: a simple loop to find first inventory item
                                while (inventory[++item_id_start].category_id == TV_NOTHING);

                                item_id_end--;
                            } while (item_id_end >= 0);

                            command_key_id = item_id_start;
                        }

                        if ((isupper((int) which) != 0) && !verify("Try", command_key_id)) {
                            screen_id = -1;
                            command_finished = true;

                            game.player_free_turn = true;

                            break;
                        }

                        screen_id = -1;
                        command_finished = true;

                        item_found = true;
                    } else if (message != nullptr) {
                        printMessage(message);

                        // Set command_finished to force redraw of the question.
                        command_finished = true;
                    } else {
                        terminalBellSound();
                    }
                    break;
            }
        }
    } while (screen_id >= 0);

    if (redraw_screen) {
        terminalRestoreScreen();
    }

    messageLineClear();

    return item_found;
}
