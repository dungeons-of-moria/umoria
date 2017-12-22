// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Misc code, mainly handles player movement, inventory, etc

#include "headers.h"
#include "externs.h"

static bool damageMinusAC(uint32_t typ_dam);

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

// AC gets worse -RAK-
// Note: This routine affects magical AC bonuses so
// that stores can detect the damage.
static bool damageMinusAC(uint32_t typ_dam) {
    int itemsCount = 0;
    int items[6];

    if (inventory[EQUIPMENT_BODY].category_id != TV_NOTHING) {
        items[itemsCount] = EQUIPMENT_BODY;
        itemsCount++;
    }

    if (inventory[EQUIPMENT_ARM].category_id != TV_NOTHING) {
        items[itemsCount] = EQUIPMENT_ARM;
        itemsCount++;
    }

    if (inventory[EQUIPMENT_OUTER].category_id != TV_NOTHING) {
        items[itemsCount] = EQUIPMENT_OUTER;
        itemsCount++;
    }

    if (inventory[EQUIPMENT_HANDS].category_id != TV_NOTHING) {
        items[itemsCount] = EQUIPMENT_HANDS;
        itemsCount++;
    }

    if (inventory[EQUIPMENT_HEAD].category_id != TV_NOTHING) {
        items[itemsCount] = EQUIPMENT_HEAD;
        itemsCount++;
    }

    // also affect boots
    if (inventory[EQUIPMENT_FEET].category_id != TV_NOTHING) {
        items[itemsCount] = EQUIPMENT_FEET;
        itemsCount++;
    }

    bool minus = false;

    if (itemsCount == 0) {
        return minus;
    }

    int itemID = items[randomNumber(itemsCount) - 1];

    obj_desc_t description = {'\0'};
    obj_desc_t msg = {'\0'};

    if ((inventory[itemID].flags & typ_dam) != 0u) {
        minus = true;

        itemDescription(description, inventory[itemID], false);
        (void) sprintf(msg, "Your %s resists damage!", description);
        printMessage(msg);
    } else if (inventory[itemID].ac + inventory[itemID].to_ac > 0) {
        minus = true;

        itemDescription(description, inventory[itemID], false);
        (void) sprintf(msg, "Your %s is damaged!", description);
        printMessage(msg);

        inventory[itemID].to_ac--;
        playerRecalculateBonuses();
    }

    return minus;
}

// Corrode the unsuspecting person's armor -RAK-
void damageCorrodingGas(const char *creature_name) {
    if (!damageMinusAC((uint32_t) TR_RES_ACID)) {
        playerTakesHit(randomNumber(8), creature_name);
    }

    if (inventoryDamageItem(setCorrodableItems, 5) > 0) {
        printMessage("There is an acrid smell coming from your pack.");
    }
}

// Poison gas the idiot. -RAK-
void damagePoisonedGas(int damage, const char *creature_name) {
    playerTakesHit(damage, creature_name);

    py.flags.poisoned += 12 + randomNumber(damage);
}

// Burn the fool up. -RAK-
void damageFire(int damage, const char *creature_name) {
    if (py.flags.resistant_to_fire) {
        damage = damage / 3;
    }

    if (py.flags.heat_resistance > 0) {
        damage = damage / 3;
    }

    playerTakesHit(damage, creature_name);

    if (inventoryDamageItem(setFlammableItems, 3) > 0) {
        printMessage("There is smoke coming from your pack!");
    }
}

// Freeze them to death. -RAK-
void damageCold(int damage, const char *creature_name) {
    if (py.flags.resistant_to_cold) {
        damage = damage / 3;
    }

    if (py.flags.cold_resistance > 0) {
        damage = damage / 3;
    }

    playerTakesHit(damage, creature_name);

    if (inventoryDamageItem(setFrostDestroyableItems, 5) > 0) {
        printMessage("Something shatters inside your pack!");
    }
}

// Lightning bolt the sucker away. -RAK-
void damageLightningBolt(int damage, const char *creature_name) {
    if (py.flags.resistant_to_light) {
        damage = damage / 3;
    }

    playerTakesHit(damage, creature_name);

    if (inventoryDamageItem(setLightningDestroyableItems, 3) > 0) {
        printMessage("There are sparks coming from your pack!");
    }
}

// Throw acid on the hapless victim -RAK-
void damageAcid(int damage, const char *creature_name) {
    int flag = 0;

    if (damageMinusAC((uint32_t) TR_RES_ACID)) {
        flag = 1;
    }

    if (py.flags.resistant_to_acid) {
        flag += 2;
    }

    playerTakesHit(damage / (flag + 1), creature_name);

    if (inventoryDamageItem(setAcidAffectedItems, 3) > 0) {
        printMessage("There is an acrid smell coming from your pack!");
    }
}
