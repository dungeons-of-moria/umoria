// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

#pragma once

// indexes into the special name table
enum SpecialNameIds {
    SN_NULL,
    SN_R,
    SN_RA,
    SN_RF,
    SN_RC,
    SN_RL,
    SN_HA,
    SN_DF,
    SN_SA,
    SN_SD,
    SN_SE,
    SN_SU,
    SN_FT,
    SN_FB,
    SN_FREE_ACTION,
    SN_SLAYING,
    SN_CLUMSINESS,
    SN_WEAKNESS,
    SN_SLOW_DESCENT,
    SN_SPEED,
    SN_STEALTH,
    SN_SLOWNESS,
    SN_NOISE,
    SN_GREAT_MASS,
    SN_INTELLIGENCE,
    SN_WISDOM,
    SN_INFRAVISION,
    SN_MIGHT,
    SN_LORDLINESS,
    SN_MAGI,
    SN_BEAUTY,
    SN_SEEING,
    SN_REGENERATION,
    SN_STUPIDITY,
    SN_DULLNESS,
    SN_BLINDNESS,
    SN_TIMIDNESS,
    SN_TELEPORTATION,
    SN_UGLINESS,
    SN_PROTECTION,
    SN_IRRITATION,
    SN_VULNERABILITY,
    SN_ENVELOPING,
    SN_FIRE,
    SN_SLAY_EVIL,
    SN_DRAGON_SLAYING,
    SN_EMPTY,
    SN_LOCKED,
    SN_POISON_NEEDLE,
    SN_GAS_TRAP,
    SN_EXPLOSION_DEVICE,
    SN_SUMMONING_RUNES,
    SN_MULTIPLE_TRAPS,
    SN_DISARMED,
    SN_UNLOCKED,
    SN_SLAY_ANIMAL,
    SN_ARRAY_SIZE, // 56th item (size value for arrays)
};

constexpr uint8_t MAX_COLORS = 49;     // Used with potions
constexpr uint8_t MAX_MUSHROOMS = 22;  // Used with mushrooms
constexpr uint8_t MAX_WOODS = 25;      // Used with staffs
constexpr uint8_t MAX_METALS = 25;     // Used with wands
constexpr uint8_t MAX_ROCKS = 32;      // Used with rings
constexpr uint8_t MAX_AMULETS = 11;    // Used with amulets
constexpr uint8_t MAX_TITLES = 45;     // Used with scrolls
constexpr uint8_t MAX_SYLLABLES = 153; // Used with scrolls

extern uint8_t objects_identified[OBJECT_IDENT_SIZE];
extern const char *special_item_names[SpecialNameIds::SN_ARRAY_SIZE];

// Following are arrays for descriptive pieces
extern const char *colors[MAX_COLORS];
extern const char *mushrooms[MAX_MUSHROOMS];
extern const char *woods[MAX_WOODS];
extern const char *metals[MAX_METALS];
extern const char *rocks[MAX_ROCKS];
extern const char *amulets[MAX_AMULETS];
extern const char *syllables[MAX_SYLLABLES];

void identifyGameObject();

void magicInitializeItemNames();
int16_t objectPositionOffset(int category_id, int sub_category_id);
void itemSetAsIdentified(int category_id, int sub_category_id);
bool itemSetColorlessAsIdentified(int category_id, int sub_category_id, int identification);
void spellItemIdentifyAndRemoveRandomInscription(Inventory_t &item);
bool spellItemIdentified(Inventory_t const &item);
void spellItemRemoveIdentification(Inventory_t &item);
void itemIdentificationClearEmpty(Inventory_t &item);
void itemIdentifyAsStoreBought(Inventory_t &item);
void itemSetAsTried(Inventory_t const &item);
void itemIdentify(Inventory_t &item, int &item_id);
void itemRemoveMagicNaming(Inventory_t &item);
void itemDescription(obj_desc_t description, Inventory_t const &item, bool add_prefix);
void itemChargesRemainingDescription(int item_id);
void itemTypeRemainingCountDescription(int item_id);
void itemInscribe();
void itemAppendToInscription(Inventory_t &item, uint8_t item_ident_type);
void itemReplaceInscription(Inventory_t &item, const char *inscription);

void objectBlockedByMonster(int monster_id);
