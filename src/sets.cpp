// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Code to emulate the original Pascal sets

#include "headers.h"

bool setNull(Inventory_t *item) {
    (void) item; // silence warnings
    return false;
}

// Items too large to fit in chests -DJG-
// Use DungeonObject_t since item not yet created
bool setItemsLargerThanChests(DungeonObject_t *item) {
    switch (item->category_id) {
        case TV_CHEST:
        case TV_BOW:
        case TV_POLEARM:
        case TV_HARD_ARMOR:
        case TV_SOFT_ARMOR:
        case TV_STAFF:
            return true;
        case TV_HAFTED:
        case TV_SWORD:
        case TV_DIGGING:
            return (item->weight > 150);
        default:
            return false;
    }
}
