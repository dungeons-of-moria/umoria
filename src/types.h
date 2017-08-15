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

constexpr int PLAYER_NAME_SIZE = 27;
constexpr int MORIA_MESSAGE_SIZE = 80;
constexpr int OBJECT_DESCRIPTION_SIZE = 160;

// Size of an inscription in the Inventory_t. Notice alignment, must be 4*x + 1
constexpr int INSCRIP_SIZE = 13;

typedef char vtype_t[MORIA_MESSAGE_SIZE];

// Note: since its output can easily exceed 80 characters, an object description
// must always be called with a obj_desc_t type as the first parameter.
typedef char obj_desc_t[OBJECT_DESCRIPTION_SIZE];

// Player_t contains everything to be known about our player character
typedef struct {
    struct {
        char name[PLAYER_NAME_SIZE];    // Name of character
        bool gender;                    // Gender of character (Female = 0, Male = 1)
        int32_t au;                     // Gold
        int32_t max_exp;                // Max experience
        int32_t exp;                    // Cur experience
        uint16_t exp_frac;              // Cur exp fraction * 2^16
        uint16_t age;                   // Characters age
        uint16_t height;                // Height
        uint16_t weight;                // Weight
        uint16_t level;                 // Level
        uint16_t max_dungeon_depth;     // Max level explored
        int16_t chance_in_search;       // Chance in search
        int16_t freng_of_search;        // Frenq of search
        int16_t bth;                    // Base to hit
        int16_t bth_with_bows;          // BTH with bows
        int16_t mana;                   // Mana points
        int16_t max_hp;                 // Max hit pts
        int16_t plusses_to_hit;         // Plusses to hit
        int16_t plusses_to_damage;      // Plusses to dam
        int16_t ac;                     // Total AC
        int16_t magical_ac;             // Magical AC
        int16_t display_to_hit;         // Display +ToHit
        int16_t display_to_damage;      // Display +ToDam
        int16_t display_ac;             // Display +ToTAC
        int16_t display_to_ac;          // Display +ToAC
        int16_t disarm;                 // % to Disarm
        int16_t saving_throw;           // Saving throw
        int16_t social_class;           // Social Class
        int16_t stealth_factor;         // Stealth factor
        uint8_t class_id;               // # of class
        uint8_t race_id;                // # of race
        uint8_t hit_die;                // Char hit die
        uint8_t experience_factor;      // Experience factor
        int16_t current_mana;           // Current mana points
        uint16_t current_mana_fraction; // Current mana fraction * 2^16
        int16_t current_hp;             // Current hit points
        uint16_t current_hp_fraction;   // Current hit points fraction * 2^16
        char history[4][60];            // History record
    } misc;

    // Stats now kept in arrays, for more efficient access. -CJS-
    struct {
        uint8_t max[6];      // What is restored
        uint8_t current[6];  // What is natural
        int16_t modified[6]; // What is modified, may be +/-
        uint8_t used[6];     // What is used
    } stats;

    struct {
        uint32_t status;             // Status of player
        int16_t rest;                // Rest counter
        int16_t blind;               // Blindness counter
        int16_t paralysis;           // Paralysis counter
        int16_t confused;            // Confusion counter
        int16_t food;                // Food counter
        int16_t food_digested;       // Food per round
        int16_t protection;          // Protection fr. evil
        int16_t speed;               // Cur speed adjust
        int16_t fast;                // Temp speed change
        int16_t slow;                // Temp speed change
        int16_t afraid;              // Fear
        int16_t poisoned;            // Poisoned
        int16_t image;               // Hallucinate
        int16_t protect_evil;        // Protect VS evil
        int16_t invulnerability;     // Increases AC
        int16_t heroism;             // Heroism
        int16_t super_heroism;       // Super Heroism
        int16_t blessed;             // Blessed
        int16_t heat_resistance;     // Timed heat resist
        int16_t cold_resistance;     // Timed cold resist
        int16_t detect_invisible;    // Timed see invisible
        int16_t word_of_recall;      // Timed teleport level
        int16_t see_infra;           // See warm creatures
        int16_t timed_infra;         // Timed infra vision
        bool see_invisible;          // Can see invisible
        bool teleport;               // Random teleportation
        bool free_action;            // Never paralyzed
        bool slow_digest;            // Lower food needs
        bool aggravate;              // Aggravate monsters
        bool resistant_to_fire;      // Resistance to fire
        bool resistant_to_cold;      // Resistance to cold
        bool resistant_to_acid;      // Resistance to acid
        bool regenerate_hp;          // Regenerate hit pts
        bool resistant_to_light;     // Resistance to light
        bool free_fall;              // No damage falling
        bool sustain_str;            // Keep strength
        bool sustain_int;            // Keep intelligence
        bool sustain_wis;            // Keep wisdom
        bool sustain_con;            // Keep constitution
        bool sustain_dex;            // Keep dexterity
        bool sustain_chr;            // Keep charisma
        bool confuse_monster;        // Glowing hands.
        uint8_t new_spells_to_learn; // Number of spells can learn.
    } flags;
} Player_t;

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
    uint8_t damage[2];              // Damage when hits
    uint8_t depth_first_found;      // Dungeon level item first found
    uint8_t identification;         // Identify information
} Inventory_t;


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

// Monster_t is created for any living monster found on the current dungeon level
typedef struct {
    int16_t hp;           // Hit points
    int16_t sleep_count;  // Inactive counter
    int16_t speed;        // Movement speed
    uint16_t creature_id; // Pointer into creature

    // Note: fy, fx, and cdis constrain dungeon size to less than 256 by 256
    uint8_t y;  // Y Pointer into map
    uint8_t x;  // X Pointer into map
    uint8_t distance_from_player; // Current distance from player

    bool lit;
    uint8_t stunned_amount;
    uint8_t confused_amount;
} Monster_t;


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

// InventoryRecord_t data for a store inventory item
typedef struct {
    int32_t cost;
    Inventory_t item;
} InventoryRecord_t;

// Store_t holds all the data for any given store in the game
typedef struct {
    int32_t turns_left_before_closing;
    int16_t insults_counter;
    uint8_t owner;
    uint8_t store_id;
    uint16_t good_purchases;
    uint16_t bad_purchases;
    InventoryRecord_t inventory[STORE_MAX_DISCRETE_ITEMS];
} Store_t;

// HighScore_t is a score object used for saving to the high score file
// This structure is 64 bytes in size
typedef struct {
    int32_t points;
    int32_t birth_date;
    int16_t uid;
    int16_t mhp;
    int16_t chp;
    uint8_t dungeon_depth;
    uint8_t level;
    uint8_t deepest_dungeon_depth;
    uint8_t gender;
    uint8_t race;
    uint8_t character_class;
    char name[PLAYER_NAME_SIZE];
    char died_from[25];
} HighScore_t;


// Cave_t holds data about a specific tile in the dungeon.
typedef struct {
    uint8_t creature_id; // ID for any creature occupying the tile
    uint8_t treasure_id; // ID for any treasure item occupying the tile
    uint8_t feature_id;  // ID of cave feature; walls, floors, open space, etc.

    bool perma_lit_room;  // Room should be lit with perm light, walls with this set should be perm lit after tunneled out.
    bool field_mark;      // Field mark, used for traps/doors/stairs, object is hidden if fm is false.
    bool permanent_light; // Permanent light, used for walls and lighted rooms.
    bool temporary_light; // Temporary light, used for player's lamp light,etc.
} Cave_t;


//
// The following are objects for storing the core game data,
// which is loaded from the large hash maps at game start up.
//


// GameObject_t is a base data object.
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
    uint8_t damage[2];         // Damage when hits
    uint8_t depth_first_found; // Dungeon level item first found
} GameObject_t;

// Creature_t is a base data object.
// Holds the base game data for any given creature in the game such
// as: Kobold, Orc, Giant Red Ant, Quasit, Young Black Dragon, etc.
typedef struct {
    const char *name;           // Description of creature
    uint32_t movement;          // Bit field
    uint32_t spells;            // Creature spells
    uint16_t defenses;          // Bit field
    uint16_t kill_exp_value;    // Exp value for kill
    uint8_t sleep_counter;      // Inactive counter / 10
    uint8_t area_affect_radius; // Area affect radius
    uint8_t ac;                 // AC
    uint8_t speed;              // Movement speed+10 (NOTE: +10 so that it can be an unsigned int)
    uint8_t sprite;             // Character representation (cchar)
    uint8_t hit_die[2];         // Creatures hit die
    uint8_t damage[4];          // Type attack and damage
    uint8_t level;              // Level of creature
} Creature_t;

// MonsterAttack_t is a base data object.
// Holds the data for a monster's attack and damage type
typedef struct {
    uint8_t type_id;
    uint8_t description_id;
    uint8_t dice;
    uint8_t sides;
} MonsterAttack_t;

// ClassTitle_t is a base game object
// Holds the base game data for all character Class titles: Novice, Mage (5th), Paladin, etc.
typedef const char *ClassTitle_t;

// Spell_t is a base data object.
// Holds the base game data for a spell
// Note: the names for the spells are stored in spell_names[] array at index i, +31 if priest
typedef struct {
    uint8_t level_required;
    uint8_t mana_required;
    uint8_t failure_chance;
    uint8_t sexp; // 1/4 of exp gained for learning spell
} Spell_t;
