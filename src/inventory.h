// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

#pragma once

// Size of inventory array (DO NOT CHANGE)
constexpr uint8_t PLAYER_INVENTORY_SIZE = 34;

// Inventory stacking `sub_category_id`s - these never stack
constexpr uint8_t ITEM_NEVER_STACK_MIN = 0;
constexpr uint8_t ITEM_NEVER_STACK_MAX = 63;
// these items always stack with others of same `sub_category_id`s, always treated as
// single objects, must be power of 2;
constexpr uint8_t ITEM_SINGLE_STACK_MIN = 64;
constexpr uint8_t ITEM_SINGLE_STACK_MAX = 192; // see NOTE below
// these items stack with others only if have same `sub_category_id`s and same `misc_use`,
// they are treated as a group for wielding, etc.
constexpr uint8_t ITEM_GROUP_MIN = 192;
constexpr uint8_t ITEM_GROUP_MAX = 255;
// NOTE: items with `sub_category_id`s = 192 are treated as single objects,
// but only stack with others of same `sub_category_id`s if have the same
// `misc_use` value, only used for torches.

// Size of an inscription in the Inventory_t. Notice alignment, must be 4*x + 1
constexpr uint8_t INSCRIP_SIZE = 13;

// Inventory_t is created for an item the player may wear about
// their person, or store in their inventory pack.
//
// Only damage, ac, and tchar are constant; level could possibly be made
// constant by changing index instead; all are used rarely.
//
// Extra fields x and y for location in dungeon would simplify pusht().
//
// Making inscrip[] a pointer and malloc-ing space does not work, there are
// two many places where `Inventory_t` are copied, which results in dangling
// pointers, so we use a char array for them instead.
typedef struct {
    uint16_t id;                    // Index to object_list
    uint8_t special_name_id;        // Object special name
    char inscription[INSCRIP_SIZE]; // Object inscription
    uint32_t flags;                 // Special flags
    uint8_t category_id;            // Category number (tval)
    uint8_t sprite;                 // Character representation - ASCII symbol (tchar)
    int16_t misc_use;               // Misc. use variable (p1)
    int32_t cost;                   // Cost of item
    uint8_t sub_category_id;        // Sub-category number
    uint8_t items_count;            // Number of items
    uint16_t weight;                // Weight
    int16_t to_hit;                 // Plusses to hit
    int16_t to_damage;              // Plusses to damage
    int16_t ac;                     // Normal AC
    int16_t to_ac;                  // Plusses to AC
    Dice_t damage;                  // Damage when hits
    uint8_t depth_first_found;      // Dungeon level item first found
    uint8_t identification;         // Identify information
} Inventory_t;

// magic numbers for players equipment inventory array
enum PlayerEquipment {
    Wield = 22, // must be first item in equipment list
    Head,
    Neck,
    Body,
    Arm,
    Hands,
    Right,
    Left,
    Feet,
    Outer,
    Light,
    Auxiliary,
};

uint32_t inventoryCollectAllItemFlags();

void inventoryDestroyItem(int item_id);
void inventoryTakeOneItem(Inventory_t *to_item, Inventory_t *from_item);
void inventoryDropItem(int item_id, bool drop_all);
bool inventoryDiminishLightAttack(bool noticed);
bool inventoryDiminishChargesAttack(uint8_t creature_level, int16_t &monster_hp, bool noticed);
bool executeDisenchantAttack();
bool inventoryCanCarryItemCount(Inventory_t const &item);
bool inventoryCanCarryItem(Inventory_t const &item);
int inventoryCarryItem(Inventory_t &new_item);
bool inventoryFindRange(int item_id_start, int item_id_end, int &j, int &k);
void inventoryItemCopyTo(int from_item_id, Inventory_t &to_item);
bool inventoryItemSingleStackable(Inventory_t const &item);
bool inventoryItemStackable(Inventory_t const &item);

bool setNull(Inventory_t *item);
bool setFrostDestroyableItems(Inventory_t *item);
bool setLightningDestroyableItems(Inventory_t *item);
bool setAcidDestroyableItems(Inventory_t *item);
bool setFireDestroyableItems(Inventory_t *item);

void damageCorrodingGas(const char *creature_name);
void damagePoisonedGas(int damage, const char *creature_name);
void damageFire(int damage, const char *creature_name);
void damageCold(int damage, const char *creature_name);
void damageLightningBolt(int damage, const char *creature_name);
void damageAcid(int damage, const char *creature_name);
