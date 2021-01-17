// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// Store: updating store inventory, pricing objects

#include "headers.h"

Store_t stores[MAX_STORES];

static void storeItemInsert(int store_id, int pos, int32_t i_cost, Inventory_t *item);
static void storeItemCreate(int store_id, int16_t max_cost);

static int32_t getWeaponArmorBuyPrice(Inventory_t const &item);
static int32_t getAmmoBuyPrice(Inventory_t const &item);
static int32_t getPotionScrollBuyPrice(Inventory_t const &item);
static int32_t getFoodBuyPrice(Inventory_t const &item);
static int32_t getRingAmuletBuyPrice(Inventory_t const &item);
static int32_t getWandStaffBuyPrice(Inventory_t const &item);
static int32_t getPickShovelBuyPrice(Inventory_t const &item);

// Initialize and up-keep the store's inventory. -RAK-
void storeMaintenance() {
    for (int store_id = 0; store_id < MAX_STORES; store_id++) {
        Store_t &store = stores[store_id];

        store.insults_counter = 0;
        if (store.unique_items_counter >= config::stores::STORE_MIN_AUTO_SELL_ITEMS) {
            int turnaround = randomNumber(config::stores::STORE_STOCK_TURN_AROUND);
            if (store.unique_items_counter >= config::stores::STORE_MAX_AUTO_BUY_ITEMS) {
                turnaround += 1 + store.unique_items_counter - config::stores::STORE_MAX_AUTO_BUY_ITEMS;
            }
            turnaround--;
            while (turnaround >= 0) {
                storeDestroyItem(store_id, randomNumber(store.unique_items_counter) - 1, false);
                turnaround--;
            }
        }

        if (store.unique_items_counter <= config::stores::STORE_MAX_AUTO_BUY_ITEMS) {
            int turnaround = randomNumber(config::stores::STORE_STOCK_TURN_AROUND);
            if (store.unique_items_counter < config::stores::STORE_MIN_AUTO_SELL_ITEMS) {
                turnaround += config::stores::STORE_MIN_AUTO_SELL_ITEMS - store.unique_items_counter;
            }

            int16_t max_cost = store_owners[store.owner_id].max_cost;

            turnaround--;
            while (turnaround >= 0) {
                storeItemCreate(store_id, max_cost);
                turnaround--;
            }
        }
    }
}

// Returns the value for any given object -RAK-
int32_t storeItemValue(Inventory_t const &item) {
    int32_t value;

    if ((item.identification & config::identification::ID_DAMD) != 0) {
        // don't purchase known cursed items
        value = 0;
    } else if ((item.category_id >= TV_BOW && item.category_id <= TV_SWORD) || (item.category_id >= TV_BOOTS && item.category_id <= TV_SOFT_ARMOR)) {
        value = getWeaponArmorBuyPrice(item);
    } else if (item.category_id >= TV_SLING_AMMO && item.category_id <= TV_SPIKE) {
        value = getAmmoBuyPrice(item);
    } else if (item.category_id == TV_SCROLL1 || item.category_id == TV_SCROLL2 || item.category_id == TV_POTION1 || item.category_id == TV_POTION2) {
        value = getPotionScrollBuyPrice(item);
    } else if (item.category_id == TV_FOOD) {
        value = getFoodBuyPrice(item);
    } else if (item.category_id == TV_AMULET || item.category_id == TV_RING) {
        value = getRingAmuletBuyPrice(item);
    } else if (item.category_id == TV_STAFF || item.category_id == TV_WAND) {
        value = getWandStaffBuyPrice(item);
    } else if (item.category_id == TV_DIGGING) {
        value = getPickShovelBuyPrice(item);
    } else {
        value = item.cost;
    }

    // Multiply value by number of items if it is a group stack item.
    // Do not include torches here.
    if (item.sub_category_id > ITEM_GROUP_MIN) {
        value = value * item.items_count;
    }

    return value;
}

static int32_t getWeaponArmorBuyPrice(Inventory_t const &item) {
    if (!spellItemIdentified(item)) {
        return game_objects[item.id].cost;
    }

    if (item.category_id >= TV_BOW && item.category_id <= TV_SWORD) {
        if (item.to_hit < 0 || item.to_damage < 0 || item.to_ac < 0) {
            return 0;
        }

        return item.cost + (item.to_hit + item.to_damage + item.to_ac) * 100;
    }

    if (item.to_ac < 0) {
        return 0;
    }

    return item.cost + item.to_ac * 100;
}

static int32_t getAmmoBuyPrice(Inventory_t const &item) {
    if (!spellItemIdentified(item)) {
        return game_objects[item.id].cost;
    }

    if (item.to_hit < 0 || item.to_damage < 0 || item.to_ac < 0) {
        return 0;
    }

    // use 5, because missiles generally appear in groups of 20,
    // so 20 * 5 == 100, which is comparable to weapon bonus above
    return item.cost + (item.to_hit + item.to_damage + item.to_ac) * 5;
}

static int32_t getPotionScrollBuyPrice(Inventory_t const &item) {
    if (!itemSetColorlessAsIdentified(item.category_id, item.sub_category_id, item.identification)) {
        return 20;
    }

    return item.cost;
}

static int32_t getFoodBuyPrice(Inventory_t const &item) {
    if (item.sub_category_id < ITEM_SINGLE_STACK_MIN + MAX_MUSHROOMS && !itemSetColorlessAsIdentified(item.category_id, item.sub_category_id, item.identification)) {
        return 1;
    }

    return item.cost;
}

static int32_t getRingAmuletBuyPrice(Inventory_t const &item) {
    // player does not know what type of ring/amulet this is
    if (!itemSetColorlessAsIdentified(item.category_id, item.sub_category_id, item.identification)) {
        return 45;
    }

    // player knows what type of ring, but does not know whether it
    // is cursed or not, if refuse to buy cursed objects here, then
    // player can use this to 'identify' cursed objects
    if (!spellItemIdentified(item)) {
        return game_objects[item.id].cost;
    }

    return item.cost;
}

static int32_t getWandStaffBuyPrice(Inventory_t const &item) {
    if (!itemSetColorlessAsIdentified(item.category_id, item.sub_category_id, item.identification)) {
        if (item.category_id == TV_WAND) {
            return 50;
        }

        return 70;
    }

    if (spellItemIdentified(item)) {
        return item.cost + (item.cost / 20) * item.misc_use;
    }

    return item.cost;
}

static int32_t getPickShovelBuyPrice(Inventory_t const &item) {
    if (!spellItemIdentified(item)) {
        return game_objects[item.id].cost;
    }

    if (item.misc_use < 0) {
        return 0;
    }

    // some digging tools start with non-zero `misc_use` values, so only
    // multiply the plusses by 100, make sure result is positive
    int32_t value = item.cost + (item.misc_use - game_objects[item.id].misc_use) * 100;

    if (value < 0) {
        value = 0;
    }

    return value;
}

// Asking price for an item -RAK-
int32_t storeItemSellPrice(Store_t const &store, int32_t &min_price, int32_t &max_price, Inventory_t const &item) {
    int32_t price = storeItemValue(item);

    // check `item.cost` in case it is cursed, check `price` in case it is damaged
    // don't let the item get into the store inventory
    if (item.cost < 1 || price < 1) {
        return 0;
    }

    Owner_t const &owner = store_owners[store.owner_id];

    price = price * race_gold_adjustments[owner.race][py.misc.race_id] / 100;
    if (price < 1) {
        price = 1;
    }

    max_price = price * owner.max_inflate / 100;
    min_price = price * owner.min_inflate / 100;

    if (min_price > max_price) {
        min_price = max_price;
    }

    return price;
}

// Check to see if they will be carrying too many objects -RAK-
bool storeCheckPlayerItemsCount(Store_t const &store, Inventory_t const &item) {
    if (store.unique_items_counter < STORE_MAX_DISCRETE_ITEMS) {
        return true;
    }

    if (!inventoryItemStackable(item)) {
        return false;
    }

    bool store_check = false;

    for (int i = 0; i < store.unique_items_counter; i++) {
        Inventory_t const &store_item = store.inventory[i].item;

        // note: items with sub_category_id of gte ITEM_SINGLE_STACK_MAX only stack
        // if their `sub_category_id`s match
        if (store_item.category_id == item.category_id && store_item.sub_category_id == item.sub_category_id && (int) (store_item.items_count + item.items_count) < 256 &&
            (item.sub_category_id < ITEM_GROUP_MIN || store_item.misc_use == item.misc_use)) {
            store_check = true;
        }
    }

    return store_check;
}

// Insert INVEN_MAX at given location
static void storeItemInsert(int store_id, int pos, int32_t i_cost, Inventory_t *item) {
    Store_t &store = stores[store_id];

    for (int i = store.unique_items_counter - 1; i >= pos; i--) {
        store.inventory[i + 1] = store.inventory[i];
    }

    store.inventory[pos].item = *item;
    store.inventory[pos].cost = -i_cost;
    store.unique_items_counter++;
}

// Add the item in INVEN_MAX to stores inventory. -RAK-
void storeCarryItem(int store_id, int &index_id, Inventory_t &item) {
    index_id = -1;

    Store_t &store = stores[store_id];

    int32_t item_cost, dummy;
    if (storeItemSellPrice(store, dummy, item_cost, item) < 1) {
        return;
    }

    int item_id = 0;
    int item_num = item.items_count;
    int item_category = item.category_id;
    int item_sub_catagory = item.sub_category_id;

    bool flag = false;
    do {
        Inventory_t &store_item = store.inventory[item_id].item;

        if (item_category == store_item.category_id) {
            if (item_sub_catagory == store_item.sub_category_id && // Adds to other item
                item_sub_catagory >= ITEM_SINGLE_STACK_MIN && (item_sub_catagory < ITEM_GROUP_MIN || store_item.misc_use == item.misc_use)) {
                index_id = item_id;
                store_item.items_count += item_num;

                // must set new cost for group items, do this only for items
                // strictly greater than group_min, not for torches, this
                // must be recalculated for entire group
                if (item_sub_catagory > ITEM_GROUP_MIN) {
                    (void) storeItemSellPrice(store, dummy, item_cost, store_item);
                    store.inventory[item_id].cost = -item_cost;
                } else if (store_item.items_count > 24) {
                    // must let group objects (except torches) stack over 24
                    // since there may be more than 24 in the group
                    store_item.items_count = 24;
                }
                flag = true;
            }
        } else if (item_category > store_item.category_id) { // Insert into list
            storeItemInsert(store_id, item_id, item_cost, &item);
            flag = true;
            index_id = item_id;
        }
        item_id++;
    } while (item_id < store.unique_items_counter && !flag);

    // Becomes last item in list
    if (!flag) {
        storeItemInsert(store_id, (int) store.unique_items_counter, item_cost, &item);
        index_id = store.unique_items_counter - 1;
    }
}

// Destroy an item in the stores inventory.  Note that if
// `only_one_of` is false, an entire slot is destroyed -RAK-
void storeDestroyItem(int store_id, int item_id, bool only_one_of) {
    Store_t &store = stores[store_id];
    Inventory_t &store_item = store.inventory[item_id].item;

    uint8_t number;

    // for single stackable objects, only destroy one half on average,
    // this will help ensure that general store and alchemist have
    // reasonable selection of objects
    if (inventoryItemSingleStackable(store_item)) {
        if (only_one_of) {
            number = 1;
        } else {
            number = (uint8_t) randomNumber((int) store_item.items_count);
        }
    } else {
        number = store_item.items_count;
    }

    if (number != store_item.items_count) {
        store_item.items_count -= number;
    } else {
        for (int i = item_id; i < store.unique_items_counter - 1; i++) {
            store.inventory[i] = store.inventory[i + 1];
        }
        inventoryItemCopyTo(config::dungeon::objects::OBJ_NOTHING, store.inventory[store.unique_items_counter - 1].item);
        store.inventory[store.unique_items_counter - 1].cost = 0;
        store.unique_items_counter--;
    }
}

// Creates an item and inserts it into store's inven -RAK-
static void storeItemCreate(int store_id, int16_t max_cost) {
    int free_id = popt();

    for (int tries = 0; tries <= 3; tries++) {
        int id = store_choices[store_id][randomNumber(STORE_MAX_ITEM_TYPES) - 1];
        inventoryItemCopyTo(id, game.treasure.list[free_id]);
        magicTreasureMagicalAbility(free_id, config::treasure::LEVEL_TOWN_OBJECTS);

        Inventory_t &item = game.treasure.list[free_id];

        if (storeCheckPlayerItemsCount(stores[store_id], item)) {
            // Item must be good: cost > 0.
            if (item.cost > 0 && item.cost < max_cost) {
                // equivalent to calling spellIdentifyItem(),
                // except will not change the objects_identified array.
                itemIdentifyAsStoreBought(item);

                int dummy;
                storeCarryItem(store_id, dummy, item);

                tries = 10;
            }
        }
    }

    pusht((uint8_t) free_id);
}
