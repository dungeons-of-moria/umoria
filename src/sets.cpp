// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Code to emulate the original Pascal sets

#include "headers.h"

bool set_room(int element) {
    return (element == DARK_FLOOR || element == LIGHT_FLOOR);
}

bool set_corr(int element) {
    return (element == CORR_FLOOR || element == BLOCKED_FLOOR);
}

bool set_floor(int element) {
    return (element <= MAX_CAVE_FLOOR);
}

bool set_corrodes(Inventory_t *item) {
    switch (item->tval) {
        case TV_SWORD:
        case TV_HELM:
        case TV_SHIELD:
        case TV_HARD_ARMOR:
        case TV_WAND:
            return true;
        default:
            return false;
    }
}

bool set_flammable(Inventory_t *item) {
    switch (item->tval) {
        case TV_ARROW:
        case TV_BOW:
        case TV_HAFTED:
        case TV_POLEARM:
        case TV_BOOTS:
        case TV_GLOVES:
        case TV_CLOAK:
        case TV_SOFT_ARMOR:
            // Items of (RF) should not be destroyed.
            return (item->flags & TR_RES_FIRE) == 0;
        case TV_STAFF:
        case TV_SCROLL1:
        case TV_SCROLL2:
            return true;
        default:
            return false;
    }
}

bool set_frost_destroy(Inventory_t *item) {
    return (item->tval == TV_POTION1 || item->tval == TV_POTION2 || item->tval == TV_FLASK);
}

bool set_acid_affect(Inventory_t *item) {
    switch (item->tval) {
        case TV_MISC:
        case TV_CHEST:
            return true;
        case TV_BOLT:
        case TV_ARROW:
        case TV_BOW:
        case TV_HAFTED:
        case TV_POLEARM:
        case TV_BOOTS:
        case TV_GLOVES:
        case TV_CLOAK:
        case TV_SOFT_ARMOR:
            return (item->flags & TR_RES_ACID) == 0;
    }
    return false;
}

bool set_lightning_destroy(Inventory_t *item) {
    return (item->tval == TV_RING || item->tval == TV_WAND || item->tval == TV_SPIKE);
}

bool set_null(Inventory_t *item) {
    (void) item; // silence warnings
    return false;
}

bool set_acid_destroy(Inventory_t *item) {
    switch (item->tval) {
        case TV_ARROW:
        case TV_BOW:
        case TV_HAFTED:
        case TV_POLEARM:
        case TV_BOOTS:
        case TV_GLOVES:
        case TV_CLOAK:
        case TV_HELM:
        case TV_SHIELD:
        case TV_HARD_ARMOR:
        case TV_SOFT_ARMOR:
            return (item->flags & TR_RES_ACID) == 0;
        case TV_STAFF:
        case TV_SCROLL1:
        case TV_SCROLL2:
        case TV_FOOD:
        case TV_OPEN_DOOR:
        case TV_CLOSED_DOOR:
            return true;
        default:
            return false;
    }
}

bool set_fire_destroy(Inventory_t *item) {
    switch (item->tval) {
        case TV_ARROW:
        case TV_BOW:
        case TV_HAFTED:
        case TV_POLEARM:
        case TV_BOOTS:
        case TV_GLOVES:
        case TV_CLOAK:
        case TV_SOFT_ARMOR:
            return (item->flags & TR_RES_FIRE) == 0;
        case TV_STAFF:
        case TV_SCROLL1:
        case TV_SCROLL2:
        case TV_POTION1:
        case TV_POTION2:
        case TV_FLASK:
        case TV_FOOD:
        case TV_OPEN_DOOR:
        case TV_CLOSED_DOOR:
            return true;
        default:
            return false;
    }
}

// Items too large to fit in chests -DJG-
// Use GameObject_t since item not yet created
bool set_large(GameObject_t *item) {
    switch (item->tval) {
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

bool general_store(int element) {
    switch (element) {
        case TV_DIGGING:
        case TV_BOOTS:
        case TV_CLOAK:
        case TV_FOOD:
        case TV_FLASK:
        case TV_LIGHT:
        case TV_SPIKE:
            return true;
        default:
            return false;
    }
}

bool armory(int element) {
    switch (element) {
        case TV_BOOTS:
        case TV_GLOVES:
        case TV_HELM:
        case TV_SHIELD:
        case TV_HARD_ARMOR:
        case TV_SOFT_ARMOR:
            return true;
        default:
            return false;
    }
}

bool weaponsmith(int element) {
    switch (element) {
        case TV_SLING_AMMO:
        case TV_BOLT:
        case TV_ARROW:
        case TV_BOW:
        case TV_HAFTED:
        case TV_POLEARM:
        case TV_SWORD:
            return true;
        default:
            return false;
    }
}

bool temple(int element) {
    switch (element) {
        case TV_HAFTED:
        case TV_SCROLL1:
        case TV_SCROLL2:
        case TV_POTION1:
        case TV_POTION2:
        case TV_PRAYER_BOOK:
            return true;
        default:
            return false;
    }
}

bool alchemist(int element) {
    switch (element) {
        case TV_SCROLL1:
        case TV_SCROLL2:
        case TV_POTION1:
        case TV_POTION2:
            return true;
        default:
            return false;
    }
}

bool magic_shop(int element) {
    switch (element) {
        case TV_AMULET:
        case TV_RING:
        case TV_STAFF:
        case TV_WAND:
        case TV_SCROLL1:
        case TV_SCROLL2:
        case TV_POTION1:
        case TV_POTION2:
        case TV_MAGIC_BOOK:
            return true;
        default:
            return false;
    }
}
