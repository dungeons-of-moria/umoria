// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Misc code, mainly handles player movement, inventory, etc

#include "headers.h"
#include "externs.h"

// Change a trap from invisible to visible -RAK-
// Note: Secret doors are handled here
void trapChangeVisibility(int y, int x) {
    uint8_t treasure_id = dg.floor[y][x].treasure_id;

    Inventory_t &item = treasure_list[treasure_id];

    if (item.category_id == TV_INVIS_TRAP) {
        item.category_id = TV_VIS_TRAP;
        dungeonLiteSpot(y, x);
        return;
    }

    // change secret door to closed door
    if (item.category_id == TV_SECRET_DOOR) {
        item.id = OBJ_CLOSED_DOOR;
        item.category_id = game_objects[OBJ_CLOSED_DOOR].category_id;
        item.sprite = game_objects[OBJ_CLOSED_DOOR].sprite;
        dungeonLiteSpot(y, x);
    }
}
