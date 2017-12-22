// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Misc code for maintaining the dungeon, printing player info

#include "headers.h"
#include "externs.h"

// Add a comment to an object description. -CJS-
void itemInscribe() {
    if (py.unique_inventory_items == 0 && py.equipment_count == 0) {
        printMessage("You are not carrying anything to inscribe.");
        return;
    }

    int item_id;
    if (!inventoryGetInputForItemId(item_id, "Which one? ", 0, PLAYER_INVENTORY_SIZE, CNIL, CNIL)) {
        return;
    }

    obj_desc_t msg = {'\0'};
    itemDescription(msg, inventory[item_id], true);

    obj_desc_t inscription = {'\0'};
    (void) sprintf(inscription, "Inscribing %s", msg);

    printMessage(inscription);

    if (inventory[item_id].inscription[0] != '\0') {
        (void) sprintf(inscription, "Replace %s New inscription:", inventory[item_id].inscription);
    } else {
        (void) strcpy(inscription, "Inscription: ");
    }

    int msg_len = 78 - (int) strlen(msg);
    if (msg_len > 12) {
        msg_len = 12;
    }

    putStringClearToEOL(inscription, Coord_t{0, 0});

    if (getStringInput(inscription, Coord_t{0, (int) strlen(inscription)}, msg_len)) {
        itemReplaceInscription(inventory[item_id], inscription);
    }
}

// Append an additional comment to an object description. -CJS-
void itemAppendToInscription(Inventory_t &item, uint8_t item_ident_type) {
    item.identification |= item_ident_type;
}

// Replace any existing comment in an object description with a new one. -CJS-
void itemReplaceInscription(Inventory_t &item, const char *inscription) {
    (void) strcpy(item.inscription, inscription);
}
