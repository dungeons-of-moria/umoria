// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Code to emulate the original Pascal sets

#include "headers.h"

bool setRooms(int tile_id) {
    return (tile_id == TILE_DARK_FLOOR || tile_id == TILE_LIGHT_FLOOR);
}

bool setCorridors(int tile_id) {
    return (tile_id == TILE_CORR_FLOOR || tile_id == TILE_BLOCKED_FLOOR);
}

bool setFloors(int tile_id) {
    return (tile_id <= MAX_CAVE_FLOOR);
}

bool setCorrodableItems(Inventory_t *item) {
    switch (item->category_id) {
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

bool setFlammableItems(Inventory_t *item) {
    switch (item->category_id) {
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

bool setFrostDestroyableItems(Inventory_t *item) {
    return (item->category_id == TV_POTION1 || item->category_id == TV_POTION2 || item->category_id == TV_FLASK);
}

bool setAcidAffectedItems(Inventory_t *item) {
    switch (item->category_id) {
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
        default:
            return false;
    }
}

bool setLightningDestroyableItems(Inventory_t *item) {
    return (item->category_id == TV_RING || item->category_id == TV_WAND || item->category_id == TV_SPIKE);
}

bool setNull(Inventory_t *item) {
    (void) item; // silence warnings
    return false;
}

bool setAcidDestroyableItems(Inventory_t *item) {
    switch (item->category_id) {
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

bool setFireDestroyableItems(Inventory_t *item) {
    switch (item->category_id) {
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
bool setItemsLargerThanChests(GameObject_t *item) {
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

bool setGeneralStoreItems(int item_id) {
    switch (item_id) {
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

bool setArmoryItems(int item_id) {
    switch (item_id) {
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

bool setWeaponsmithItems(int item_id) {
    switch (item_id) {
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

bool setTempleItems(int item_id) {
    switch (item_id) {
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

bool setAlchemistItems(int item_id) {
    switch (item_id) {
        case TV_SCROLL1:
        case TV_SCROLL2:
        case TV_POTION1:
        case TV_POTION2:
            return true;
        default:
            return false;
    }
}

bool setMagicShopItems(int item_id) {
    switch (item_id) {
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
