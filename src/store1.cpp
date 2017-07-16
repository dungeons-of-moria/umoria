// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Store code, updating store inventory, pricing objects

#include "headers.h"
#include "externs.h"

static void storeItemInsert(int store_id, int pos, int32_t i_cost, Inventory_t *item);
static void storeItemCreate(int store_id, int16_t max_cost);

static int32_t getWeaponArmorBuyPrice(Inventory_t *item);
static int32_t getAmmoBuyPrice(Inventory_t *item);
static int32_t getPotionScrollBuyPrice(Inventory_t *item);
static int32_t getFoodBuyPrice(Inventory_t *item);
static int32_t getRingAmuletBuyPrice(Inventory_t *item);
static int32_t getWandStaffBuyPrice(Inventory_t *item);
static int32_t getPickShovelBuyPrice(Inventory_t *item);

// Returns the value for any given object -RAK-
int32_t storeItemValue(Inventory_t *item) {
    int32_t value;

    if (item->ident & ID_DAMD) {
        // don't purchase known cursed items
        value = 0;
    } else if ((item->tval >= TV_BOW && item->tval <= TV_SWORD) || (item->tval >= TV_BOOTS && item->tval <= TV_SOFT_ARMOR)) {
        value = getWeaponArmorBuyPrice(item);
    } else if (item->tval >= TV_SLING_AMMO && item->tval <= TV_SPIKE) {
        value = getAmmoBuyPrice(item);
    } else if (item->tval == TV_SCROLL1 || item->tval == TV_SCROLL2 || item->tval == TV_POTION1 || item->tval == TV_POTION2) {
        value = getPotionScrollBuyPrice(item);
    } else if (item->tval == TV_FOOD) {
        value = getFoodBuyPrice(item);
    } else if (item->tval == TV_AMULET || item->tval == TV_RING) {
        value = getRingAmuletBuyPrice(item);
    } else if (item->tval == TV_STAFF || item->tval == TV_WAND) {
        value = getWandStaffBuyPrice(item);
    } else if (item->tval == TV_DIGGING) {
        value = getPickShovelBuyPrice(item);
    } else {
        value = item->cost;
    }

    // Multiply value by number of items if it is a group stack item.
    // Do not include torches here.
    if (item->subval > ITEM_GROUP_MIN) {
        value = value * item->number;
    }

    return value;
}

static int32_t getWeaponArmorBuyPrice(Inventory_t *item) {
    if (!spellItemIdentified(item)) {
        return game_objects[item->index].cost;
    }

    if (item->tval >= TV_BOW && item->tval <= TV_SWORD) {
        if (item->tohit < 0 || item->todam < 0 || item->toac < 0) {
            return 0;
        }

        return item->cost + (item->tohit + item->todam + item->toac) * 100;
    }

    if (item->toac < 0) {
        return 0;
    }

    return item->cost + item->toac * 100;
}

static int32_t getAmmoBuyPrice(Inventory_t *item) {
    if (!spellItemIdentified(item)) {
        return game_objects[item->index].cost;
    }

    if (item->tohit < 0 || item->todam < 0 || item->toac < 0) {
        return 0;
    }

    // use 5, because missiles generally appear in groups of 20,
    // so 20 * 5 == 100, which is comparable to weapon bonus above
    return item->cost + (item->tohit + item->todam + item->toac) * 5;
}

static int32_t getPotionScrollBuyPrice(Inventory_t *item) {
    if (!itemSetColorlessAsIdentifed(item)) {
        return 20;
    }

    return item->cost;
}

static int32_t getFoodBuyPrice(Inventory_t *item) {
    if (item->subval < ITEM_SINGLE_STACK_MIN + MAX_MUSH && !itemSetColorlessAsIdentifed(item)) {
        return 1;
    }

    return item->cost;
}

static int32_t getRingAmuletBuyPrice(Inventory_t *item) {
    // player does not know what type of ring/amulet this is
    if (!itemSetColorlessAsIdentifed(item)) {
        return 45;
    }

    // player knows what type of ring, but does not know whether it
    // is cursed or not, if refuse to buy cursed objects here, then
    // player can use this to 'identify' cursed objects
    if (!spellItemIdentified(item)) {
        return game_objects[item->index].cost;
    }

    return item->cost;
}

static int32_t getWandStaffBuyPrice(Inventory_t *item) {
    if (!itemSetColorlessAsIdentifed(item)) {
        if (item->tval == TV_WAND) {
            return 50;
        }

        return 70;
    }

    if (spellItemIdentified(item)) {
        return item->cost + (item->cost / 20) * item->p1;
    }

    return item->cost;
}

static int32_t getPickShovelBuyPrice(Inventory_t *item) {
    if (!spellItemIdentified(item)) {
        return game_objects[item->index].cost;
    }

    if (item->p1 < 0) {
        return 0;
    }

    // some digging tools start with non-zero p1 values, so only
    // multiply the plusses by 100, make sure result is positive
    int32_t value = item->cost + (item->p1 - game_objects[item->index].p1) * 100;

    if (value < 0) {
        value = 0;
    }

    return value;
}

// Asking price for an item -RAK-
int32_t storeItemSellPrice(int store_id, int32_t *min_price, int32_t *max_price, Inventory_t *item) {
    int32_t price = storeItemValue(item);

    // check `item->cost` in case it is cursed, check `price` in case it is damaged
    // don't let the item get into the store inventory
    if (item->cost < 1 || price < 1) {
        return 0;
    }

    Owner_t *owner = &store_owners[stores[store_id].owner];

    price = price * race_gold_adjustments[owner->owner_race][py.misc.prace] / 100;
    if (price < 1) {
        price = 1;
    }

    *max_price = price * owner->max_inflate / 100;
    *min_price = price * owner->min_inflate / 100;

    if (*min_price > *max_price) {
        *min_price = *max_price;
    }

    return price;
}

// Check to see if he will be carrying too many objects -RAK-
bool storeCheckPlayerItemsCount(int store_id, Inventory_t *item) {
    Store_t *store = &stores[store_id];

    if (store->store_ctr < STORE_INVEN_MAX) {
        return true;
    }

    if (item->subval < ITEM_SINGLE_STACK_MIN) {
        return false;
    }

    bool store_check = false;

    for (int i = 0; i < store->store_ctr; i++) {
        Inventory_t *store_item = &store->store_inven[i].sitem;

        // note: items with subval of gte ITEM_SINGLE_STACK_MAX only stack
        // if their subvals match
        if (store_item->tval == item->tval && store_item->subval == item->subval && (int) (store_item->number + item->number) < 256 &&
            (item->subval < ITEM_GROUP_MIN || store_item->p1 == item->p1)) {
            store_check = true;
        }
    }

    return store_check;
}

// Insert INVEN_MAX at given location
static void storeItemInsert(int store_id, int pos, int32_t i_cost, Inventory_t *item) {
    Store_t *store = &stores[store_id];

    for (int i = store->store_ctr - 1; i >= pos; i--) {
        store->store_inven[i + 1] = store->store_inven[i];
    }

    store->store_inven[pos].sitem = *item;
    store->store_inven[pos].scost = -i_cost;
    store->store_ctr++;
}

// Add the item in INVEN_MAX to stores inventory. -RAK-
void storeCarry(int store_id, int *index_id, Inventory_t *item) {
    *index_id = -1;

    int32_t i_cost, dummy;
    if (storeItemSellPrice(store_id, &dummy, &i_cost, item) < 1) {
        return;
    }

    Store_t *store = &stores[store_id];

    int item_id = 0;
    int item_num = item->number;
    int item_category = item->tval;
    int item_sub_catory = item->subval;

    bool flag = false;
    do {
        Inventory_t *store_item = &store->store_inven[item_id].sitem;

        if (item_category == store_item->tval) {
            if (item_sub_catory == store_item->subval && // Adds to other item
                item_sub_catory >= ITEM_SINGLE_STACK_MIN && (item_sub_catory < ITEM_GROUP_MIN || store_item->p1 == item->p1)) {
                *index_id = item_id;
                store_item->number += item_num;

                // must set new scost for group items, do this only for items
                // strictly greater than group_min, not for torches, this
                // must be recalculated for entire group
                if (item_sub_catory > ITEM_GROUP_MIN) {
                    (void) storeItemSellPrice(store_id, &dummy, &i_cost, store_item);
                    store->store_inven[item_id].scost = -i_cost;
                } else if (store_item->number > 24) {
                    // must let group objects (except torches) stack over 24
                    // since there may be more than 24 in the group
                    store_item->number = 24;
                }
                flag = true;
            }
        } else if (item_category > store_item->tval) { // Insert into list
            storeItemInsert(store_id, item_id, i_cost, item);
            flag = true;
            *index_id = item_id;
        }
        item_id++;
    } while (item_id < store->store_ctr && !flag);

    // Becomes last item in list
    if (!flag) {
        storeItemInsert(store_id, (int) store->store_ctr, i_cost, item);
        *index_id = store->store_ctr - 1;
    }
}

// Destroy an item in the stores inventory.  Note that if
// `only_one_of` is false, an entire slot is destroyed -RAK-
void storeDestroy(int store_id, int item_id, bool only_one_of) {
    Store_t *store = &stores[store_id];
    Inventory_t *store_item = &store->store_inven[item_id].sitem;

    int number;

    // for single stackable objects, only destroy one half on average,
    // this will help ensure that general store and alchemist have
    // reasonable selection of objects
    if (store_item->subval >= ITEM_SINGLE_STACK_MIN && store_item->subval <= ITEM_SINGLE_STACK_MAX) {
        if (only_one_of) {
            number = 1;
        } else {
            number = randomNumber((int) store_item->number);
        }
    } else {
        number = store_item->number;
    }

    if (number != store_item->number) {
        store_item->number -= number;
    } else {
        for (int i = item_id; i < store->store_ctr - 1; i++) {
            store->store_inven[i] = store->store_inven[i + 1];
        }
        inventoryItemCopyTo(OBJ_NOTHING, &store->store_inven[store->store_ctr - 1].sitem);
        store->store_inven[store->store_ctr - 1].scost = 0;
        store->store_ctr--;
    }
}

// Initializes the stores with owners -RAK-
void storeInitializeOwners() {
    int count = MAX_OWNERS / MAX_STORES;

    for (int store_id = 0; store_id < MAX_STORES; store_id++) {
        Store_t *store = &stores[store_id];

        store->owner = (uint8_t) (MAX_STORES * (randomNumber(count) - 1) + store_id);
        store->insult_cur = 0;
        store->store_open = 0;
        store->store_ctr = 0;
        store->good_buy = 0;
        store->bad_buy = 0;

        for (int item_id = 0; item_id < STORE_INVEN_MAX; item_id++) {
            inventoryItemCopyTo(OBJ_NOTHING, &store->store_inven[item_id].sitem);
            store->store_inven[item_id].scost = 0;
        }
    }
}

// Creates an item and inserts it into store's inven -RAK-
static void storeItemCreate(int store_id, int16_t max_cost) {
    int free_id = popt();

    for (int tries = 0; tries <= 3; tries++) {
        int id = store_choices[store_id][randomNumber(STORE_CHOICES) - 1];
        inventoryItemCopyTo(id, &treasure_list[free_id]);
        magicTreasureMagicalAbility(free_id, OBJ_TOWN_LEVEL);

        Inventory_t *item = &treasure_list[free_id];

        if (storeCheckPlayerItemsCount(store_id, item)) {
            // Item must be good: cost > 0.
            if (item->cost > 0 && item->cost < max_cost) {
                // equivalent to calling spellIdentifyItem(),
                // except will not change the objects_identified array.
                itemIdentifyAsStoreBought(item);

                int dummy;
                storeCarry(store_id, &dummy, item);

                tries = 10;
            }
        }
    }

    pusht((uint8_t) free_id);
}

// Initialize and up-keep the store's inventory. -RAK-
void storeMaintenance() {
    for (int store_id = 0; store_id < MAX_STORES; store_id++) {
        Store_t *store = &stores[store_id];

        store->insult_cur = 0;
        if (store->store_ctr >= STORE_MIN_INVEN) {
            int turnaround = randomNumber(STORE_TURN_AROUND);
            if (store->store_ctr >= STORE_MAX_INVEN) {
                turnaround += 1 + store->store_ctr - STORE_MAX_INVEN;
            }
            while (--turnaround >= 0) {
                storeDestroy(store_id, randomNumber((int) store->store_ctr) - 1, false);
            }
        }

        if (store->store_ctr <= STORE_MAX_INVEN) {
            int turnaround = randomNumber(STORE_TURN_AROUND);
            if (store->store_ctr < STORE_MIN_INVEN) {
                turnaround += STORE_MIN_INVEN - store->store_ctr;
            }

            int16_t max_cost = store_owners[store->owner].max_cost;

            while (--turnaround >= 0) {
                storeItemCreate(store_id, max_cost);
            }
        }
    }
}

// eliminate need to bargain if player has haggled well in the past -DJB-
bool noneedtobargain(int store_id, int32_t min_price) {
    Store_t *s_ptr = &stores[store_id];

    if (s_ptr->good_buy == MAX_SHORT) {
        return true;
    }

    int bargain_record = (s_ptr->good_buy - 3 * s_ptr->bad_buy - 5);

    return ((bargain_record > 0) && ((int32_t) bargain_record * (int32_t) bargain_record > min_price / 50));
}

// update the bargain info -DJB-
void updatebargain(int store_id, int32_t price, int32_t min_price) {
    Store_t *s_ptr = &stores[store_id];

    if (min_price > 9) {
        if (price == min_price) {
            if (s_ptr->good_buy < MAX_SHORT) {
                s_ptr->good_buy++;
            }
        } else {
            if (s_ptr->bad_buy < MAX_SHORT) {
                s_ptr->bad_buy++;
            }
        }
    }
}
