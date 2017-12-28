// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

#pragma once

#include <cstdint>

// InventoryRecord_t data for a store inventory item
typedef struct {
    int32_t cost;
    Inventory_t item;
} InventoryRecord_t;

// Store_t holds all the data for any given store in the game
typedef struct {
    int32_t turns_left_before_closing;
    int16_t insults_counter;
    uint8_t owner_id;
    uint8_t unique_items_counter;
    uint16_t good_purchases;
    uint16_t bad_purchases;
    InventoryRecord_t inventory[STORE_MAX_DISCRETE_ITEMS];
} Store_t;

// Owner_t holds data about a given store owner
typedef struct {
    const char *name;
    int16_t max_cost;
    uint8_t max_inflate;
    uint8_t min_inflate;
    uint8_t haggles_per;
    uint8_t race;
    uint8_t max_insults;
} Owner_t;

// store
void storeInitializeOwners();
void storeEnter(int store_id);

// store_inventory
void storeMaintenance();
int32_t storeItemValue(const Inventory_t &item);
int32_t storeItemSellPrice(const Store_t &store, int32_t &min_price, int32_t &max_price, const Inventory_t &item);
bool storeCheckPlayerItemsCount(const Store_t &store, const Inventory_t &item);
void storeCarryItem(int store_id, int &index_id, Inventory_t &item);
void storeDestroyItem(int store_id, int item_id, bool only_one_of);
