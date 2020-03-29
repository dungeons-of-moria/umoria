// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

#pragma once

constexpr uint8_t MAX_OWNERS = 18;               // Number of owners to choose from
constexpr uint8_t MAX_STORES = 6;                // Number of different stores
constexpr uint8_t STORE_MAX_DISCRETE_ITEMS = 24; // Max number of discrete objects in inventory
constexpr uint8_t STORE_MAX_ITEM_TYPES = 26;     // Number of items to choose stock from
constexpr uint8_t COST_ADJUSTMENT = 100;         // Adjust prices for buying and selling

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

extern uint8_t race_gold_adjustments[PLAYER_MAX_RACES][PLAYER_MAX_RACES];

extern Owner_t store_owners[MAX_OWNERS];
extern Store_t stores[MAX_STORES];
extern uint16_t store_choices[MAX_STORES][STORE_MAX_ITEM_TYPES];
extern bool (*store_buy[MAX_STORES])(uint8_t);
extern const char *speech_sale_accepted[14];
extern const char *speech_selling_haggle_final[3];
extern const char *speech_selling_haggle[16];
extern const char *speech_buying_haggle_final[3];
extern const char *speech_buying_haggle[15];
extern const char *speech_insulted_haggling_done[5];
extern const char *speech_get_out_of_my_store[5];
extern const char *speech_haggling_try_again[10];
extern const char *speech_sorry[5];

// store
void storeInitializeOwners();
void storeEnter(int store_id);

// store_inventory
void storeMaintenance();
int32_t storeItemValue(Inventory_t const &item);
int32_t storeItemSellPrice(Store_t const &store, int32_t &min_price, int32_t &max_price, Inventory_t const &item);
bool storeCheckPlayerItemsCount(Store_t const &store, Inventory_t const &item);
void storeCarryItem(int store_id, int &index_id, Inventory_t &item);
void storeDestroyItem(int store_id, int item_id, bool only_one_of);
