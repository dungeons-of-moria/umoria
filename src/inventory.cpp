// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// Inventory and inventory items

#include "headers.h"

uint32_t inventoryCollectAllItemFlags() {
    uint32_t flags = 0;

    for (int i = PlayerEquipment::Wield; i < PlayerEquipment::Light; i++) {
        flags |= py.inventory[i].flags;
    }

    return flags;
}

// Destroy an item in the inventory -RAK-
void inventoryDestroyItem(int item_id) {
    Inventory_t &item = py.inventory[item_id];

    if (item.items_count > 1 && item.sub_category_id <= ITEM_SINGLE_STACK_MAX) {
        item.items_count--;
        py.pack.weight -= item.weight;
    } else {
        py.pack.weight -= item.weight * item.items_count;

        for (int i = item_id; i < py.pack.unique_items - 1; i++) {
            py.inventory[i] = py.inventory[i + 1];
        }

        inventoryItemCopyTo(config::dungeon::objects::OBJ_NOTHING, py.inventory[py.pack.unique_items - 1]);
        py.pack.unique_items--;
    }

    py.flags.status |= config::player::status::PY_STR_WGT;
}

// Copies the object in the second argument over the first argument.
// However, the second always gets a number of one except for ammo etc.
void inventoryTakeOneItem(Inventory_t *to_item, Inventory_t *from_item) {
    *to_item = *from_item;

    if (to_item->items_count > 1 && inventoryItemSingleStackable(*to_item)) {
        to_item->items_count = 1;
    }
}

// Drops an item from inventory to given location -RAK-
void inventoryDropItem(int item_id, bool drop_all) {
    if (dg.floor[py.pos.y][py.pos.x].treasure_id != 0) {
        (void) dungeonDeleteObject(py.pos);
    }

    int treasure_id = popt();

    Inventory_t &item = py.inventory[item_id];
    game.treasure.list[treasure_id] = item;

    dg.floor[py.pos.y][py.pos.x].treasure_id = (uint8_t) treasure_id;

    if (item_id >= PlayerEquipment::Wield) {
        playerTakeOff(item_id, -1);
    } else {
        if (drop_all || item.items_count == 1) {
            py.pack.weight -= item.weight * item.items_count;
            py.pack.unique_items--;

            while (item_id < py.pack.unique_items) {
                py.inventory[item_id] = py.inventory[item_id + 1];
                item_id++;
            }

            inventoryItemCopyTo(config::dungeon::objects::OBJ_NOTHING, py.inventory[py.pack.unique_items]);
        } else {
            game.treasure.list[treasure_id].items_count = 1;
            py.pack.weight -= item.weight;
            item.items_count--;
        }

        obj_desc_t prt1 = {'\0'};
        obj_desc_t prt2 = {'\0'};
        itemDescription(prt1, game.treasure.list[treasure_id], true);
        (void) sprintf(prt2, "Dropped %s", prt1);
        printMessage(prt2);
    }

    py.flags.status |= config::player::status::PY_STR_WGT;
}

// Destroys a type of item on a given percent chance -RAK-
static int inventoryDamageItem(bool (*item_type)(Inventory_t *), int chance_percentage) {
    int damage = 0;

    for (int i = 0; i < py.pack.unique_items; i++) {
        if ((*item_type)(&py.inventory[i]) && randomNumber(100) < chance_percentage) {
            inventoryDestroyItem(i);
            damage++;
        }
    }

    return damage;
}

bool inventoryDiminishLightAttack(bool noticed) {
    Inventory_t &item = py.inventory[PlayerEquipment::Light];

    if (item.misc_use > 0) {
        item.misc_use -= (250 + randomNumber(250));

        if (item.misc_use < 1) {
            item.misc_use = 1;
        }

        if (py.flags.blind < 1) {
            printMessage("Your light dims.");
        } else {
            noticed = false;
        }
    } else {
        noticed = false;
    }

    return noticed;
}

bool inventoryDiminishChargesAttack(uint8_t creature_level, int16_t &monster_hp, bool noticed) {
    Inventory_t &item = py.inventory[randomNumber(py.pack.unique_items) - 1];

    bool has_charges = item.category_id == TV_STAFF || item.category_id == TV_WAND;

    if (has_charges && item.misc_use > 0) {
        monster_hp += creature_level * item.misc_use;
        item.misc_use = 0;
        if (!spellItemIdentified(item)) {
            itemAppendToInscription(item, config::identification::ID_EMPTY);
        }
        printMessage("Energy drains from your pack!");
    } else {
        noticed = false;
    }

    return noticed;
}

bool executeDisenchantAttack() {
    int item_id;

    switch (randomNumber(7)) {
        case 1:
            item_id = PlayerEquipment::Wield;
            break;
        case 2:
            item_id = PlayerEquipment::Body;
            break;
        case 3:
            item_id = PlayerEquipment::Arm;
            break;
        case 4:
            item_id = PlayerEquipment::Outer;
            break;
        case 5:
            item_id = PlayerEquipment::Hands;
            break;
        case 6:
            item_id = PlayerEquipment::Head;
            break;
        case 7:
            item_id = PlayerEquipment::Feet;
            break;
        default:
            return false;
    }

    bool success = false;
    Inventory_t &item = py.inventory[item_id];

    if (item.to_hit > 0) {
        item.to_hit -= randomNumber(2);

        // don't send it below zero
        if (item.to_hit < 0) {
            item.to_hit = 0;
        }
        success = true;
    }
    if (item.to_damage > 0) {
        item.to_damage -= randomNumber(2);

        // don't send it below zero
        if (item.to_damage < 0) {
            item.to_damage = 0;
        }
        success = true;
    }
    if (item.to_ac > 0) {
        item.to_ac -= randomNumber(2);

        // don't send it below zero
        if (item.to_ac < 0) {
            item.to_ac = 0;
        }
        success = true;
    }

    return success;
}

// this code must be identical to the inventoryCarryItem() code below
bool inventoryCanCarryItemCount(Inventory_t const &item) {
    if (py.pack.unique_items < PlayerEquipment::Wield) {
        return true;
    }

    if (!inventoryItemStackable(item) ) {
        return false;
    }

    for (int i = 0; i < py.pack.unique_items; i++) {
        bool same_character = py.inventory[i].category_id == item.category_id;
        bool same_category = py.inventory[i].sub_category_id == item.sub_category_id;

        // make sure the number field doesn't overflow
        // NOTE: convert to bigger types before addition -MRC-
        bool same_number = uint16_t(py.inventory[i].items_count) + uint16_t(item.items_count) < 256;

        // they always stack (sub_category_id < 192), or else they have same `misc_use`
        bool same_group = item.sub_category_id < ITEM_GROUP_MIN || py.inventory[i].misc_use == item.misc_use;

        // only stack if both or neither are identified
        // TODO(cook): is it correct that they should be equal to each other, regardless of true/false value?
        bool inventory_item_is_colorless = itemSetColorlessAsIdentified(py.inventory[i].category_id, py.inventory[i].sub_category_id, py.inventory[i].identification);
        bool item_is_colorless = itemSetColorlessAsIdentified(item.category_id, item.sub_category_id, item.identification);
        bool identification = inventory_item_is_colorless == item_is_colorless;

        if (same_character && same_category && same_number && same_group && identification) {
            return true;
        }
    }

    return false;
}

// return false if picking up an object would change the players speed
bool inventoryCanCarryItem(Inventory_t const &item) {
    int limit = playerCarryingLoadLimit();
    int new_weight = item.items_count * item.weight + py.pack.weight;

    if (limit < new_weight) {
        limit = new_weight / (limit + 1);
    } else {
        limit = 0;
    }

    return py.pack.heaviness == limit;
}

// Add an item to players inventory.  Return the
// item position for a description if needed. -RAK-
// this code must be identical to the inventoryCanCarryItemCount() code above
int inventoryCarryItem(Inventory_t &new_item) {
    bool is_known = itemSetColorlessAsIdentified(new_item.category_id, new_item.sub_category_id, new_item.identification);
    bool is_always_known = objectPositionOffset(new_item.category_id, new_item.sub_category_id) == -1;

    int slot_id;

    // Now, check to see if player can carry object
    for (slot_id = 0; slot_id < PLAYER_INVENTORY_SIZE; slot_id++) {
        Inventory_t &item = py.inventory[slot_id];

        bool is_same_category = new_item.category_id == item.category_id;
        bool is_same_sub_category = new_item.sub_category_id == item.sub_category_id;
        bool not_too_many_items = int(new_item.items_count + item.items_count) < 256;

        // only stack if both or neither are identified
        bool same_known_status = itemSetColorlessAsIdentified(item.category_id, item.sub_category_id, item.identification) == is_known;

        bool is_stackable = inventoryItemStackable(new_item);
        bool is_same_group = (new_item.sub_category_id < ITEM_GROUP_MIN || item.misc_use == new_item.misc_use);

        if (is_same_category && is_same_sub_category && is_stackable && not_too_many_items && is_same_group && same_known_status) {
            item.items_count += new_item.items_count;
            break;
        }

        if ((is_same_category && new_item.sub_category_id < item.sub_category_id && is_always_known) || new_item.category_id > item.category_id) {
            // For items which are always `is_known`, i.e. never have a 'color',
            // insert them into the inventory in sorted order.
            for (int i = py.pack.unique_items - 1; i >= slot_id; i--) {
                py.inventory[i + 1] = py.inventory[i];
            }
            py.inventory[slot_id] = new_item;
            py.pack.unique_items++;
            break;
        }
    }

    py.pack.weight += new_item.items_count * new_item.weight;
    py.flags.status |= config::player::status::PY_STR_WGT;

    return slot_id;
}

// Finds range of item in inventory list -RAK-
bool inventoryFindRange(int item_id_start, int item_id_end, int &j, int &k) {
    j = -1;
    k = -1;

    bool at_end_of_range = false;

    for (int i = 0; i < py.pack.unique_items; i++) {
        int item_id = (int) py.inventory[i].category_id;

        if (!at_end_of_range) {
            if (item_id == item_id_start || item_id == item_id_end) {
                at_end_of_range = true;
                j = i;
            }
        } else {
            if (item_id != item_id_start && item_id != item_id_end) {
                k = i - 1;
                break;
            }
        }
    }

    if (at_end_of_range && k == -1) {
        k = py.pack.unique_items - 1;
    }

    return at_end_of_range;
}

void inventoryItemCopyTo(int from_item_id, Inventory_t &to_item) {
    DungeonObject_t const &from = game_objects[from_item_id];

    to_item.id = (uint16_t) from_item_id;
    to_item.special_name_id = SpecialNameIds::SN_NULL;
    to_item.inscription[0] = '\0';
    to_item.flags = from.flags;
    to_item.category_id = from.category_id;
    to_item.sprite = from.sprite;
    to_item.misc_use = from.misc_use;
    to_item.cost = from.cost;
    to_item.sub_category_id = from.sub_category_id;
    to_item.items_count = from.items_count;
    to_item.weight = from.weight;
    to_item.to_hit = from.to_hit;
    to_item.to_damage = from.to_damage;
    to_item.ac = from.ac;
    to_item.to_ac = from.to_ac;
    to_item.damage.dice = from.damage.dice;
    to_item.damage.sides = from.damage.sides;
    to_item.depth_first_found = from.depth_first_found;
    to_item.identification = 0;
}

// Checks if an item is stackable, only as a singles object.
bool inventoryItemSingleStackable(Inventory_t const &item) {
    return item.sub_category_id >= ITEM_SINGLE_STACK_MIN && item.sub_category_id <= ITEM_SINGLE_STACK_MAX;
}

// Checks if an item is stackable; either singles objects or group items.
bool inventoryItemStackable(Inventory_t const &item) {
    return item.sub_category_id >= ITEM_SINGLE_STACK_MIN;
}

// AC gets worse -RAK-
// Note: This routine affects magical AC bonuses so
// that stores can detect the damage.
static bool damageMinusAC(uint32_t typ_dam) {
    int items_count = 0;
    uint8_t items[6];

    if (py.inventory[PlayerEquipment::Body].category_id != TV_NOTHING) {
        items[items_count] = PlayerEquipment::Body;
        items_count++;
    }

    if (py.inventory[PlayerEquipment::Arm].category_id != TV_NOTHING) {
        items[items_count] = PlayerEquipment::Arm;
        items_count++;
    }

    if (py.inventory[PlayerEquipment::Outer].category_id != TV_NOTHING) {
        items[items_count] = PlayerEquipment::Outer;
        items_count++;
    }

    if (py.inventory[PlayerEquipment::Hands].category_id != TV_NOTHING) {
        items[items_count] = PlayerEquipment::Hands;
        items_count++;
    }

    if (py.inventory[PlayerEquipment::Head].category_id != TV_NOTHING) {
        items[items_count] = PlayerEquipment::Head;
        items_count++;
    }

    // also affect boots
    if (py.inventory[PlayerEquipment::Feet].category_id != TV_NOTHING) {
        items[items_count] = PlayerEquipment::Feet;
        items_count++;
    }

    bool minus = false;

    if (items_count == 0) {
        return minus;
    }

    uint8_t item_id = items[randomNumber(items_count) - 1];

    obj_desc_t description = {'\0'};
    obj_desc_t msg = {'\0'};

    if ((py.inventory[item_id].flags & typ_dam) != 0u) {
        minus = true;

        itemDescription(description, py.inventory[item_id], false);
        (void) sprintf(msg, "Your %s resists damage!", description);
        printMessage(msg);
    } else if (py.inventory[item_id].ac + py.inventory[item_id].to_ac > 0) {
        minus = true;

        itemDescription(description, py.inventory[item_id], false);
        (void) sprintf(msg, "Your %s is damaged!", description);
        printMessage(msg);

        py.inventory[item_id].to_ac--;
        playerRecalculateBonuses();
    }

    return minus;
}

// Functions to emulate the original Pascal sets
bool setNull(Inventory_t *item) {
    (void) item; // silence warnings
    return false;
}

static bool setCorrodableItems(Inventory_t *item) {
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

static bool setFlammableItems(Inventory_t *item) {
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
            return (item->flags & config::treasure::flags::TR_RES_FIRE) == 0;
        case TV_STAFF:
        case TV_SCROLL1:
        case TV_SCROLL2:
            return true;
        default:
            return false;
    }
}

static bool setAcidAffectedItems(Inventory_t *item) {
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
            return (item->flags & config::treasure::flags::TR_RES_ACID) == 0;
        default:
            return false;
    }
}

bool setFrostDestroyableItems(Inventory_t *item) {
    return (item->category_id == TV_POTION1 || item->category_id == TV_POTION2 || item->category_id == TV_FLASK);
}

bool setLightningDestroyableItems(Inventory_t *item) {
    return (item->category_id == TV_RING || item->category_id == TV_WAND || item->category_id == TV_SPIKE);
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
            return (item->flags & config::treasure::flags::TR_RES_ACID) == 0;
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
            return (item->flags & config::treasure::flags::TR_RES_FIRE) == 0;
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

// Corrode the unsuspecting person's armor -RAK-
void damageCorrodingGas(const char *creature_name) {
    if (!damageMinusAC(config::treasure::flags::TR_RES_ACID)) {
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

    if (damageMinusAC(config::treasure::flags::TR_RES_ACID)) {
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
