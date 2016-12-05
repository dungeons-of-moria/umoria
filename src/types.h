// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Global type declarations

// some machines will not accept 'signed char' as a type, and some accept it
// but still treat it like an unsigned character, let's just avoid it,
// any variable which can ever hold a negative value must be 16 or 32 bits

#define VTYPESIZ 80
#define BIGVTYPESIZ 160
typedef char vtype[VTYPESIZ];
// note that since its output can easily exceed 80 characters, objdes must
// always be called with a bigvtype as the first paramter
typedef char bigvtype[BIGVTYPESIZ];
typedef char stat_type[7];

// Many of the character fields used to be fixed length, which greatly
// increased the size of the executable.  I have replaced many fixed
// length fields with variable length ones.
//
// all fields are given the smallest possbile type, and all fields are
// aligned within the structure to their natural size boundary, so that
// the structures contain no padding and are minimum size.
//
// bit fields are only used where they would cause a large reduction in
// data size, they should not be used otherwise because their use
// results in larger and slower code.

typedef struct creature_type {
    char *name;         // Descrip of creature
    uint32_t cmove;     // Bit field
    uint32_t spells;    // Creature spells
    uint16_t cdefense;  // Bit field
    uint16_t mexp;      // Exp value for kill
    uint8_t sleep;      // Inactive counter/10
    uint8_t aaf;        // Area affect radius
    uint8_t ac;         // AC
    uint8_t speed;      // Movement speed+10 (NOTE: +10 so that it can be a uint8_t)
    uint8_t cchar;      // Character rep.
    uint8_t hd[2];      // Creatures hit die
    uint8_t damage[4];  // Type attack and damage
    uint8_t level;      // Level of creature
} creature_type;

// Monster attack and damage types
typedef struct m_attack_type {
    uint8_t attack_type;
    uint8_t attack_desc;
    uint8_t attack_dice;
    uint8_t attack_sides;
} m_attack_type;

// Monster memories. -CJS-
typedef struct recall_type {
    uint32_t r_cmove;
    uint32_t r_spells;
    uint16_t r_kills, r_deaths;
    uint16_t r_cdefense;
    uint8_t r_wake, r_ignore;
    uint8_t r_attacks[MAX_MON_NATTACK];
} recall_type;

typedef struct monster_type {
    int16_t hp;         // Hit points
    int16_t csleep;     // Inactive counter
    int16_t cspeed;     // Movement speed
    uint16_t mptr;      // Pointer into creature

    // Note: fy, fx, and cdis constrain dungeon size to less than 256 by 256
    uint8_t fy;         // Y Pointer into map
    uint8_t fx;         // X Pointer into map
    uint8_t cdis;       // Cur dis from player

    uint8_t ml;
    uint8_t stunned;
    uint8_t confused;
} monster_type;

typedef struct treasure_type {
    char *name;         // Object name
    uint32_t flags;     // Special flags
    uint8_t tval;       // Category number
    uint8_t tchar;      // Character representation
    int16_t p1;         // Misc. use variable
    int32_t cost;       // Cost of item
    uint8_t subval;     // Sub-category number
    uint8_t number;     // Number of items
    uint16_t weight;    // Weight
    int16_t tohit;      // Plusses to hit
    int16_t todam;      // Plusses to damage
    int16_t ac;         // Normal AC
    int16_t toac;       // Plusses to AC
    uint8_t damage[2];  // Damage when hits
    uint8_t level;      // Level item first found
} treasure_type;

// only damage, ac, and tchar are constant; level could possibly be made
// constant by changing index instead; all are used rarely.
//
// extra fields x and y for location in dungeon would simplify pusht().
//
// making inscrip a pointer and mallocing space does not work, there are
// two many places where inven_types are copied, which results in dangling
// pointers, so we use a char array for them instead
#define INSCRIP_SIZE 13 // notice alignment, must be 4*x + 1
typedef struct inven_type {
    uint16_t index;                   // Index to object_list
    uint8_t name2;                    // Object special name
    char inscrip[INSCRIP_SIZE];       // Object inscription
    uint32_t flags;                   // Special flags
    uint8_t tval;                     // Category number
    uint8_t tchar;                    // Character representation
    int16_t p1;                       // Misc. use variable
    int32_t cost;                     // Cost of item
    uint8_t subval;                   // Sub-category number
    uint8_t number;                   // Number of items
    uint16_t weight;                  // Weight
    int16_t tohit;                    // Plusses to hit
    int16_t todam;                    // Plusses to damage
    int16_t ac;                       // Normal AC
    int16_t toac;                     // Plusses to AC
    uint8_t damage[2];                // Damage when hits
    uint8_t level;                    // Level item first found
    uint8_t ident;                    // Identify information
} inven_type;

#define PLAYER_NAME_SIZE 27

typedef struct player_type {
    struct misc {
        char name[PLAYER_NAME_SIZE];  // Name of character
        uint8_t male;                 // Sex of character
        int32_t au;                   // Gold
        int32_t max_exp;              // Max experience
        int32_t exp;                  // Cur experience
        uint16_t exp_frac;            // Cur exp fraction * 2^16
        uint16_t age;                 // Characters age
        uint16_t ht;                  // Height
        uint16_t wt;                  // Weight
        uint16_t lev;                 // Level
        uint16_t max_dlv;             // Max level explored
        int16_t srh;                  // Chance in search
        int16_t fos;                  // Frenq of search
        int16_t bth;                  // Base to hit
        int16_t bthb;                 // BTH with bows
        int16_t mana;                 // Mana points
        int16_t mhp;                  // Max hit pts
        int16_t ptohit;               // Plusses to hit
        int16_t ptodam;               // Plusses to dam
        int16_t pac;                  // Total AC
        int16_t ptoac;                // Magical AC
        int16_t dis_th;               // Display +ToHit
        int16_t dis_td;               // Display +ToDam
        int16_t dis_ac;               // Display +ToAC
        int16_t dis_tac;              // Display +ToTAC
        int16_t disarm;               // % to Disarm
        int16_t save;                 // Saving throw
        int16_t sc;                   // Social Class
        int16_t stl;                  // Stealth factor
        uint8_t pclass;               // # of class
        uint8_t prace;                // # of race
        uint8_t hitdie;               // Char hit die
        uint8_t expfact;              // Experience factor
        int16_t cmana;                // Cur mana pts
        uint16_t cmana_frac;          // Cur mana fraction * 2^16
        int16_t chp;                  // Cur hit pts
        uint16_t chp_frac;            // Cur hit fraction * 2^16
        char history[4][60];          // History record
    } misc;

    // Stats now kept in arrays, for more efficient access. -CJS-
    struct stats {
        uint8_t max_stat[6];          // What is restored
        uint8_t cur_stat[6];          // What is natural
        int16_t mod_stat[6];          // What is modified, may be +/-
        uint8_t use_stat[6];          // What is used
    } stats;

    struct flags {
        uint32_t status;              // Status of player
        int16_t rest;                 // Rest counter
        int16_t blind;                // Blindness counter
        int16_t paralysis;            // Paralysis counter
        int16_t confused;             // Confusion counter
        int16_t food;                 // Food counter
        int16_t food_digested;        // Food per round
        int16_t protection;           // Protection fr. evil
        int16_t speed;                // Cur speed adjust
        int16_t fast;                 // Temp speed change
        int16_t slow;                 // Temp speed change
        int16_t afraid;               // Fear
        int16_t poisoned;             // Poisoned
        int16_t image;                // Hallucinate
        int16_t protevil;             // Protect VS evil
        int16_t invuln;               // Increases AC
        int16_t hero;                 // Heroism
        int16_t shero;                // Super Heroism
        int16_t blessed;              // Blessed
        int16_t resist_heat;          // Timed heat resist
        int16_t resist_cold;          // Timed cold resist
        int16_t detect_inv;           // Timed see invisible
        int16_t word_recall;          // Timed teleport level
        int16_t see_infra;            // See warm creatures
        int16_t tim_infra;            // Timed infra vision
        uint8_t see_inv;              // Can see invisible
        uint8_t teleport;             // Random teleportation
        uint8_t free_act;             // Never paralyzed
        uint8_t slow_digest;          // Lower food needs
        uint8_t aggravate;            // Aggravate monsters
        uint8_t fire_resist;          // Resistance to fire
        uint8_t cold_resist;          // Resistance to cold
        uint8_t acid_resist;          // Resistance to acid
        uint8_t regenerate;           // Regenerate hit pts
        uint8_t lght_resist;          // Resistance to light
        uint8_t ffall;                // No damage falling
        uint8_t sustain_str;          // Keep strength
        uint8_t sustain_int;          // Keep intelligence
        uint8_t sustain_wis;          // Keep wisdom
        uint8_t sustain_con;          // Keep constitution
        uint8_t sustain_dex;          // Keep dexterity
        uint8_t sustain_chr;          // Keep charisma
        uint8_t confuse_monster;      // Glowing hands.
        uint8_t new_spells;           // Number of spells can learn.
    } flags;
} player_type;

// spell name is stored in spell_names[] array at index i, +31 if priest
typedef struct spell_type {
    uint8_t slevel;
    uint8_t smana;
    uint8_t sfail;
    uint8_t sexp;             // 1/4 of exp gained for learning spell
} spell_type;

typedef struct race_type {
    char *trace;              // Type of race
    int16_t str_adj;          // adjustments
    int16_t int_adj;
    int16_t wis_adj;
    int16_t dex_adj;
    int16_t con_adj;
    int16_t chr_adj;
    uint8_t b_age;            // Base age of character
    uint8_t m_age;            // Maximum age of character
    uint8_t m_b_ht;           // base height for males
    uint8_t m_m_ht;           // mod height for males
    uint8_t m_b_wt;           // base weight for males
    uint8_t m_m_wt;           // mod weight for males
    uint8_t f_b_ht;           // base height females
    uint8_t f_m_ht;           // mod height for females
    uint8_t f_b_wt;           // base weight for female
    uint8_t f_m_wt;           // mod weight for females
    int16_t b_dis;            // base chance to disarm
    int16_t srh;              // base chance for search
    int16_t stl;              // Stealth of character
    int16_t fos;              // frequency of auto search
    int16_t bth;              // adj base chance to hit
    int16_t bthb;             // adj base to hit with bows
    int16_t bsav;             // Race base for saving throw
    uint8_t bhitdie;          // Base hit points for race
    uint8_t infra;            // See infra-red
    uint8_t b_exp;            // Base experience factor
    uint8_t rtclass;          // Bit field for class types
} race_type;

typedef struct class_type {
    char *title;              // type of class
    uint8_t adj_hd;           // Adjust hit points
    uint8_t mdis;             // mod disarming traps
    uint8_t msrh;             // modifier to searching
    uint8_t mstl;             // modifier to stealth
    uint8_t mfos;             // modifier to freq-of-search
    uint8_t mbth;             // modifier to base to hit
    uint8_t mbthb;            // modifier to base to hit - bows
    uint8_t msav;             // Class modifier to save
    int16_t madj_str;         // Class modifier for strength
    int16_t madj_int;         // Class modifier for intelligence
    int16_t madj_wis;         // Class modifier for wisdom
    int16_t madj_dex;         // Class modifier for dexterity
    int16_t madj_con;         // Class modifier for constitution
    int16_t madj_chr;         // Class modifier for charisma
    uint8_t spell;            // class use mage spells
    uint8_t m_exp;            // Class experience factor
    uint8_t first_spell_lev;  // First level where class can use spells.
} class_type;

typedef struct background_type {
    char *info;               // History information
    uint8_t roll;             // Die roll needed for history
    uint8_t chart;            // Table number
    uint8_t next;             // Pointer to next table
    uint8_t bonus;            // Bonus to the Social Class+50
} background_type;

typedef struct cave_type {
    uint8_t cptr;
    uint8_t tptr;
    uint8_t fval;

    unsigned int lr : 1;      // Room should be lit with perm light, walls with
                              //     this set should be perm lit after tunneled out.
    unsigned int fm : 1;      // Field mark, used for traps/doors/stairs, object is
                              //     hidden if fm is false.
    unsigned int pl : 1;      // Permanent light, used for walls and lighted rooms.
    unsigned int tl : 1;      // Temporary light, used for player's lamp light,etc.
} cave_type;

typedef struct owner_type {
    char *owner_name;
    int16_t max_cost;
    uint8_t max_inflate;
    uint8_t min_inflate;
    uint8_t haggle_per;
    uint8_t owner_race;
    uint8_t insult_max;
} owner_type;

typedef struct inven_record {
    int32_t scost;
    inven_type sitem;
} inven_record;

typedef struct store_type {
    int32_t store_open;
    int16_t insult_cur;
    uint8_t owner;
    uint8_t store_ctr;
    uint16_t good_buy;
    uint16_t bad_buy;
    inven_record store_inven[STORE_INVEN_MAX];
} store_type;

// 64 bytes for this structure
typedef struct high_scores {
    int32_t points;
    int32_t birth_date;
    int16_t uid;
    int16_t mhp;
    int16_t chp;
    uint8_t dun_level;
    uint8_t lev;
    uint8_t max_dlv;
    uint8_t sex;
    uint8_t race;
    uint8_t class;
    char name[PLAYER_NAME_SIZE];
    char died_from[25];
} high_scores;
