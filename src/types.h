// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Global type declarations

// Many of the character fields used to be fixed length, which greatly
// increased the size of the executable. Many fixed length fields
// have been replaced with variable length ones.
//
// All fields are given the smallest possible type, and all fields are
// aligned within the structure to their natural size boundary, so that
// the structures contain no padding and are minimum size.

typedef char vtype_t[MORIA_MESSAGE_SIZE];

// Note: since its output can easily exceed 80 characters, an object description
// must always be called with a obj_desc_t type as the first parameter.
typedef char obj_desc_t[OBJECT_DESCRIPTION_SIZE];

// Race_t for the generated player character
typedef struct {
    const char *name;       // Type of race
    int16_t str_adjustment; // adjustments
    int16_t int_adjustment;
    int16_t wis_adjustment;
    int16_t dex_adjustment;
    int16_t con_adjustment;
    int16_t chr_adjustment;
    uint8_t base_age;            // Base age of character
    uint8_t max_age;             // Maximum age of character
    uint8_t male_height_base;    // base height for males
    uint8_t male_height_mod;     // mod height for males
    uint8_t male_weight_base;    // base weight for males
    uint8_t male_weight_mod;     // mod weight for males
    uint8_t female_height_base;  // base height females
    uint8_t female_height_mod;   // mod height for females
    uint8_t female_weight_base;  // base weight for female
    uint8_t female_weight_mod;   // mod weight for females
    int16_t disarm_chance_base;  // base chance to disarm
    int16_t search_chance_base;  // base chance for search
    int16_t stealth;             // Stealth of character
    int16_t fos;                 // frequency of auto search
    int16_t base_to_hit;         // adj base chance to hit
    int16_t base_to_hit_bows;    // adj base to hit with bows
    int16_t saving_throw_base;   // Race base for saving throw
    uint8_t hit_points_base;     // Base hit points for race
    uint8_t infra_vision;        // See infra-red
    uint8_t exp_factor_base;     // Base experience factor
    uint8_t classes_bit_field;   // Bit field for class types
} Race_t;

// Class_t for the generated player character
typedef struct {
    const char *title;                   // type of class
    uint8_t hit_points;                  // Adjust hit points
    uint8_t disarm_traps;                // mod disarming traps
    uint8_t searching;                   // modifier to searching
    uint8_t stealth;                     // modifier to stealth
    uint8_t fos;                         // modifier to freq-of-search
    uint8_t base_to_hit;                 // modifier to base to hit
    uint8_t base_to_hit_with_bows;       // modifier to base to hit - bows
    uint8_t saving_throw;                // Class modifier to save
    int16_t strength;                    // Class modifier for strength
    int16_t intelligence;                // Class modifier for intelligence
    int16_t wisdom;                      // Class modifier for wisdom
    int16_t dexterity;                   // Class modifier for dexterity
    int16_t constitution;                // Class modifier for constitution
    int16_t charisma;                    // Class modifier for charisma
    uint8_t class_to_use_mage_spells;    // class use mage spells
    uint8_t experience_factor;           // Class experience factor
    uint8_t min_level_for_spell_casting; // First level where class can use spells.
} Class_t;

// Class Background_t for the generated player character
typedef struct {
    const char *info; // History information
    uint8_t roll;     // Die roll needed for history
    uint8_t chart;    // Table number
    uint8_t next;     // Pointer to next table
    uint8_t bonus;    // Bonus to the Social Class+50
} Background_t;

// Recall_t holds the player's known knowledge for any given monster, aka memories
typedef struct {
    uint32_t movement;
    uint32_t spells;
    uint16_t kills;
    uint16_t deaths;
    uint16_t defenses;
    uint8_t wake;
    uint8_t ignore;
    uint8_t attacks[MON_MAX_ATTACKS];
} Recall_t;

//
// The following are objects for storing the core game data,
// which is loaded from the large hash maps at game start up.
//

// GameObject_t is a base data object (treasure_type).
// Holds base game data for any given item in the game such
// as: stairs, rubble, secret doors, gold, potions, etc.
typedef struct {
    const char *name;          // Object name
    uint32_t flags;            // Special flags
    uint8_t category_id;       // Category number (tval)
    uint8_t sprite;            // Character representation - ASCII symbol (tchar)
    int16_t misc_use;          // Misc. use variable (p1)
    int32_t cost;              // Cost of item
    uint8_t sub_category_id;   // Sub-category number (subval)
    uint8_t items_count;       // Number of items
    uint16_t weight;           // Weight
    int16_t to_hit;            // Plusses to hit
    int16_t to_damage;         // Plusses to damage
    int16_t ac;                // Normal AC
    int16_t to_ac;             // Plusses to AC
    Dice_t damage;             // Damage when hits
    uint8_t depth_first_found; // Dungeon level item first found
} GameObject_t;

// ClassRankTitle_t is a base game object
// Holds the base game data for all character Class titles: Novice, Mage (5th), Paladin, etc.
typedef const char *ClassRankTitle_t;

// Spell_t is a base data object.
// Holds the base game data for a spell
// Note: the names for the spells are stored in spell_names[] array at index i, +31 if priest
typedef struct {
    uint8_t level_required;
    uint8_t mana_required;
    uint8_t failure_chance;
    uint8_t exp_gain_for_learning; // 1/4 of exp gained for learning spell
} Spell_t;

typedef struct {
    int32_t y;
    int32_t x;
} Coord_t;
