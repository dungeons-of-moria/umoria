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
    if (inventory_count == 0 && equipment_count == 0) {
        printMessage("You are not carrying anything to inscribe.");
        return;
    }

    int item_id;
    if (!inventoryGetInputForItemId(&item_id, "Which one? ", 0, INVEN_ARRAY_SIZE, CNIL, CNIL)) {
        return;
    }

    obj_desc_t msg;
    itemDescription(msg, &inventory[item_id], true);

    obj_desc_t inscription;
    (void) sprintf(inscription, "Inscribing %s", msg);

    printMessage(inscription);

    if (inventory[item_id].inscrip[0] != '\0') {
        (void) sprintf(inscription, "Replace %s New inscription:", inventory[item_id].inscrip);
    } else {
        (void) strcpy(inscription, "Inscription: ");
    }

    int msg_len = 78 - (int) strlen(msg);
    if (msg_len > 12) {
        msg_len = 12;
    }

    putStringClearToEOL(inscription, 0, 0);

    if (getStringInput(inscription, 0, (int) strlen(inscription), msg_len)) {
        itemReplaceInscription(&inventory[item_id], inscription);
    }
}

// Append an additional comment to an object description. -CJS-
void itemAppendToInscription(Inventory_t *item, uint8_t item_ident_type) {
    item->ident |= item_ident_type;
}

// Replace any existing comment in an object description with a new one. -CJS-
void itemReplaceInscription(Inventory_t *item, const char *inscription) {
    (void) strcpy(item->inscrip, inscription);
}

// We need to reset the view of things. -CJS-
void dungeonResetView() {
    Cave_t *tile = &cave[char_row][char_col];

    // Check for new panel
    if (coordOutsidePanel(char_row, char_col, false)) {
        drawDungeonPanel();
    }

    // Move the light source
    dungeonMoveCharacterLight(char_row, char_col, char_row, char_col);

    // A room of light should be lit.
    if (tile->fval == LIGHT_FLOOR) {
        if (py.flags.blind < 1 && !tile->pl) {
            dungeonLightRoom(char_row, char_col);
        }
        return;
    }

    // In doorway of light-room?
    if (tile->lr && py.flags.blind < 1) {
        for (int i = char_row - 1; i <= char_row + 1; i++) {
            for (int j = char_col - 1; j <= char_col + 1; j++) {
                if (cave[i][j].fval == LIGHT_FLOOR && !cave[i][j].pl) {
                    dungeonLightRoom(i, j);
                }
            }
        }
    }
}
