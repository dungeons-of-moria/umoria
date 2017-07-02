// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Store code, updating store inventory, pricing objects

#include "headers.h"
#include "externs.h"

static void insert_store(int store_num, int pos, int32_t icost, Inventory_t *i_ptr);
static void store_create(int store_num, int16_t max_cost);

static int32_t getWeaponArmorBuyPrice(Inventory_t *i_ptr);
static int32_t getAmmoBuyPrice(Inventory_t *i_ptr);
static int32_t getPotionScrollBuyPrice(Inventory_t *i_ptr);
static int32_t getFoodBuyPrice(Inventory_t *i_ptr);
static int32_t getRingAmuletBuyPrice(Inventory_t *i_ptr);
static int32_t getWandStaffBuyPrice(Inventory_t *i_ptr);
static int32_t getPickShovelBuyPrice(Inventory_t *i_ptr);

// Returns the value for any given object -RAK-
int32_t item_value(Inventory_t *item) {
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

static int32_t getWeaponArmorBuyPrice(Inventory_t *i_ptr) {
    if (!spellItemIdentified(i_ptr)) {
        return game_objects[i_ptr->index].cost;
    }

    if (i_ptr->tval >= TV_BOW && i_ptr->tval <= TV_SWORD) {
        if (i_ptr->tohit < 0 || i_ptr->todam < 0 || i_ptr->toac < 0) {
            return 0;
        }

        return i_ptr->cost + (i_ptr->tohit + i_ptr->todam + i_ptr->toac) * 100;
    }

    if (i_ptr->toac < 0) {
        return 0;
    }

    return i_ptr->cost + i_ptr->toac * 100;
}

static int32_t getAmmoBuyPrice(Inventory_t *i_ptr) {
    if (!spellItemIdentified(i_ptr)) {
        return game_objects[i_ptr->index].cost;
    }

    if (i_ptr->tohit < 0 || i_ptr->todam < 0 || i_ptr->toac < 0) {
        return 0;
    }

    // use 5, because missiles generally appear in groups of 20,
    // so 20 * 5 == 100, which is comparable to weapon bonus above
    return i_ptr->cost + (i_ptr->tohit + i_ptr->todam + i_ptr->toac) * 5;
}

static int32_t getPotionScrollBuyPrice(Inventory_t *i_ptr) {
    if (!itemSetColorlessAsIdentifed(i_ptr)) {
        return 20;
    }

    return i_ptr->cost;
}

static int32_t getFoodBuyPrice(Inventory_t *i_ptr) {
    if (i_ptr->subval < ITEM_SINGLE_STACK_MIN + MAX_MUSH && !itemSetColorlessAsIdentifed(i_ptr)) {
        return 1;
    }

    return i_ptr->cost;
}

static int32_t getRingAmuletBuyPrice(Inventory_t *i_ptr) {
    // player does not know what type of ring/amulet this is
    if (!itemSetColorlessAsIdentifed(i_ptr)) {
        return 45;
    }

    // player knows what type of ring, but does not know whether it
    // is cursed or not, if refuse to buy cursed objects here, then
    // player can use this to 'identify' cursed objects
    if (!spellItemIdentified(i_ptr)) {
        return game_objects[i_ptr->index].cost;
    }

    return i_ptr->cost;
}

static int32_t getWandStaffBuyPrice(Inventory_t *i_ptr) {
    if (!itemSetColorlessAsIdentifed(i_ptr)) {
        if (i_ptr->tval == TV_WAND) {
            return 50;
        }

        return 70;
    }

    if (spellItemIdentified(i_ptr)) {
        return i_ptr->cost + (i_ptr->cost / 20) * i_ptr->p1;
    }

    return i_ptr->cost;
}

static int32_t getPickShovelBuyPrice(Inventory_t *i_ptr) {
    if (!spellItemIdentified(i_ptr)) {
        return game_objects[i_ptr->index].cost;
    }

    if (i_ptr->p1 < 0) {
        return 0;
    }

    // some digging tools start with non-zero p1 values, so only
    // multiply the plusses by 100, make sure result is positive
    int32_t value = i_ptr->cost + (i_ptr->p1 - game_objects[i_ptr->index].p1) * 100;

    if (value < 0) {
        value = 0;
    }

    return value;
}

// Asking price for an item -RAK-
int32_t sell_price(int store_id, int32_t *min_price, int32_t *max_price, Inventory_t *item) {
    int32_t price = item_value(item);

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
bool store_check_num(int store_id, Inventory_t *item) {
    Store_t *s_ptr = &stores[store_id];

    if (s_ptr->store_ctr < STORE_INVEN_MAX) {
        return true;
    }

    if (item->subval < ITEM_SINGLE_STACK_MIN) {
        return false;
    }

    bool store_check = false;

    for (int i = 0; i < s_ptr->store_ctr; i++) {
        Inventory_t *i_ptr = &s_ptr->store_inven[i].sitem;

        // note: items with subval of gte ITEM_SINGLE_STACK_MAX only stack
        // if their subvals match
        if (i_ptr->tval == item->tval && i_ptr->subval == item->subval && (int) (i_ptr->number + item->number) < 256 &&
            (item->subval < ITEM_GROUP_MIN || i_ptr->p1 == item->p1)) {
            store_check = true;
        }
    }

    return store_check;
}

// Insert INVEN_MAX at given location
static void insert_store(int store_num, int pos, int32_t icost, Inventory_t *i_ptr) {
    Store_t *s_ptr = &stores[store_num];

    for (int i = s_ptr->store_ctr - 1; i >= pos; i--) {
        s_ptr->store_inven[i + 1] = s_ptr->store_inven[i];
    }

    s_ptr->store_inven[pos].sitem = *i_ptr;
    s_ptr->store_inven[pos].scost = -icost;
    s_ptr->store_ctr++;
}

// Add the item in INVEN_MAX to stores inventory. -RAK-
void store_carry(int store_id, int *index_id, Inventory_t *item) {
    *index_id = -1;

    int32_t icost, dummy;
    if (sell_price(store_id, &dummy, &icost, item) < 1) {
        return;
    }

    Store_t *s_ptr = &stores[store_id];

    int item_val = 0;
    int item_num = item->number;
    int typ = item->tval;
    int subt = item->subval;

    bool flag = false;
    do {
        Inventory_t *i_ptr = &s_ptr->store_inven[item_val].sitem;

        if (typ == i_ptr->tval) {
            if (subt == i_ptr->subval && // Adds to other item
                subt >= ITEM_SINGLE_STACK_MIN && (subt < ITEM_GROUP_MIN || i_ptr->p1 == item->p1)) {
                *index_id = item_val;
                i_ptr->number += item_num;

                // must set new scost for group items, do this only for items
                // strictly greater than group_min, not for torches, this
                // must be recalculated for entire group
                if (subt > ITEM_GROUP_MIN) {
                    (void) sell_price(store_id, &dummy, &icost, i_ptr);
                    s_ptr->store_inven[item_val].scost = -icost;
                } else if (i_ptr->number > 24) {
                    // must let group objects (except torches) stack over 24
                    // since there may be more than 24 in the group
                    i_ptr->number = 24;
                }
                flag = true;
            }
        } else if (typ > i_ptr->tval) { // Insert into list
            insert_store(store_id, item_val, icost, item);
            flag = true;
            *index_id = item_val;
        }
        item_val++;
    } while (item_val < s_ptr->store_ctr && !flag);

    // Becomes last item in list
    if (!flag) {
        insert_store(store_id, (int) s_ptr->store_ctr, icost, item);
        *index_id = s_ptr->store_ctr - 1;
    }
}

// Destroy an item in the stores inventory.  Note that if
// "one_of" is false, an entire slot is destroyed -RAK-
void store_destroy(int store_id, int item_id, bool only_one) {
    Store_t *s_ptr = &stores[store_id];
    Inventory_t *i_ptr = &s_ptr->store_inven[item_id].sitem;

    int number;

    // for single stackable objects, only destroy one half on average,
    // this will help ensure that general store and alchemist have
    // reasonable selection of objects
    if (i_ptr->subval >= ITEM_SINGLE_STACK_MIN && i_ptr->subval <= ITEM_SINGLE_STACK_MAX) {
        if (only_one) {
            number = 1;
        } else {
            number = randomNumber((int) i_ptr->number);
        }
    } else {
        number = i_ptr->number;
    }

    if (number != i_ptr->number) {
        i_ptr->number -= number;
    } else {
        for (int j = item_id; j < s_ptr->store_ctr - 1; j++) {
            s_ptr->store_inven[j] = s_ptr->store_inven[j + 1];
        }
        inventoryItemCopyTo(OBJ_NOTHING, &s_ptr->store_inven[s_ptr->store_ctr - 1].sitem);
        s_ptr->store_inven[s_ptr->store_ctr - 1].scost = 0;
        s_ptr->store_ctr--;
    }
}

// Initializes the stores with owners -RAK-
void store_init() {
    int i = MAX_OWNERS / MAX_STORES;

    for (int j = 0; j < MAX_STORES; j++) {
        Store_t *s_ptr = &stores[j];

        s_ptr->owner = (uint8_t) (MAX_STORES * (randomNumber(i) - 1) + j);
        s_ptr->insult_cur = 0;
        s_ptr->store_open = 0;
        s_ptr->store_ctr = 0;
        s_ptr->good_buy = 0;
        s_ptr->bad_buy = 0;

        for (int k = 0; k < STORE_INVEN_MAX; k++) {
            inventoryItemCopyTo(OBJ_NOTHING, &s_ptr->store_inven[k].sitem);
            s_ptr->store_inven[k].scost = 0;
        }
    }
}

// Creates an item and inserts it into store's inven -RAK-
static void store_create(int store_num, int16_t max_cost) {
    int cur_pos = popt();

    for (int tries = 0; tries <= 3; tries++) {
        int i = store_choices[store_num][randomNumber(STORE_CHOICES) - 1];
        inventoryItemCopyTo(i, &treasure_list[cur_pos]);
        magic_treasure(cur_pos, OBJ_TOWN_LEVEL);

        Inventory_t *t_ptr = &treasure_list[cur_pos];

        if (store_check_num(store_num, t_ptr)) {
            // Item must be good: cost > 0.
            if (t_ptr->cost > 0 && t_ptr->cost < max_cost) {
                // equivalent to calling ident_spell(),
                // except will not change the objects_identified array.
                itemIdentifyAsStoreBought(t_ptr);

                int dummy;
                store_carry(store_num, &dummy, t_ptr);

                tries = 10;
            }
        }
    }

    pusht((uint8_t) cur_pos);
}

// Initialize and up-keep the store's inventory. -RAK-
void store_maint() {
    for (int store_id = 0; store_id < MAX_STORES; store_id++) {
        Store_t *s_ptr = &stores[store_id];

        s_ptr->insult_cur = 0;
        if (s_ptr->store_ctr >= STORE_MIN_INVEN) {
            int j = randomNumber(STORE_TURN_AROUND);
            if (s_ptr->store_ctr >= STORE_MAX_INVEN) {
                j += 1 + s_ptr->store_ctr - STORE_MAX_INVEN;
            }
            while (--j >= 0) {
                store_destroy(store_id, randomNumber((int) s_ptr->store_ctr) - 1, false);
            }
        }

        if (s_ptr->store_ctr <= STORE_MAX_INVEN) {
            int j = randomNumber(STORE_TURN_AROUND);
            if (s_ptr->store_ctr < STORE_MIN_INVEN) {
                j += STORE_MIN_INVEN - s_ptr->store_ctr;
            }

            int16_t max_cost = store_owners[s_ptr->owner].max_cost;

            while (--j >= 0) {
                store_create(store_id, max_cost);
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
