// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// Global constants used by Moria

// Note to the Wizard:
//
// Tweaking these constants can *GREATLY* change the game.
// Two years of constant tuning have generated these
// values. Minor adjustments are encouraged, but you must
// be very careful not to unbalance the game. Moria was
// meant to be challenging, not a give away. Many
// adjustments can cause the game to act strangely, or even
// cause errors. -RAK-

// Addendum:
// I have greatly expanded the number of defined constants. However, if
// you change anything below, without understanding EXACTLY how the game
// uses the number, the program may stop working correctly. Modify the
// constants at your own risk. -JEW-

// How many messages to save in the buffer -CJS-
constexpr uint8_t MESSAGE_HISTORY_SIZE = 22;

// Store constants
constexpr uint8_t MAX_OWNERS = 18;                // Number of owners to choose from
constexpr uint8_t MAX_STORES = 6;                 // Number of different stores
constexpr uint8_t STORE_MAX_DISCRETE_ITEMS = 24;  // Max number of discrete objects in inventory
constexpr uint8_t STORE_MAX_ITEM_TYPES = 26;      // Number of items to choose stock from
constexpr uint8_t STORE_MAX_AUTO_BUY_ITEMS = 18;  // Max diff objects in stock for auto buy
constexpr uint8_t STORE_MIN_AUTO_SELL_ITEMS = 10; // Min diff objects in stock for auto sell
constexpr uint8_t STORE_STOCK_TURN_AROUND = 9;    // Amount of buying and selling normally
constexpr uint8_t COST_ADJUSTMENT = 100;          // Adjust prices for buying and selling

// Treasure constants
constexpr uint8_t TREASURE_MAX_LEVELS = 50;           // Maximum level of magic in dungeon
constexpr uint8_t TREASURE_CHANCE_OF_GREAT_ITEM = 12; // 1/n Chance of item being a Great Item

// Note that the following constants are all related, if you change one, you
// must also change all succeeding ones.
// Also, player_base_provisions[] and store_choices[] may also have to be changed.
constexpr uint16_t MAX_OBJECTS_IN_GAME = 420; // Number of objects for universe
constexpr uint16_t MAX_DUNGEON_OBJECTS = 344; // Number of dungeon objects
constexpr uint16_t OBJ_OPEN_DOOR = 367;
constexpr uint16_t OBJ_CLOSED_DOOR = 368;
constexpr uint16_t OBJ_SECRET_DOOR = 369;
constexpr uint16_t OBJ_UP_STAIR = 370;
constexpr uint16_t OBJ_DOWN_STAIR = 371;
constexpr uint16_t OBJ_STORE_DOOR = 372;
constexpr uint16_t OBJ_TRAP_LIST = 378;
constexpr uint16_t OBJ_RUBBLE = 396;
constexpr uint16_t OBJ_MUSH = 397;
constexpr uint16_t OBJ_SCARE_MON = 398;
constexpr uint16_t OBJ_GOLD_LIST = 399;
constexpr uint16_t OBJ_NOTHING = 417;
constexpr uint16_t OBJ_RUINED_CHEST = 418;
constexpr uint16_t OBJ_WIZARD = 419;
constexpr uint16_t OBJECT_IDENT_SIZE = 448; // 7*64, see object_offset() in desc.cpp, could be MAX_OBJECTS o_o() rewritten

constexpr uint8_t MAX_GOLD_TYPES = 18; // Number of different types of gold

// With LEVEL_MAX_OBJECTS set to 150, it's possible to get compacting
// objects during level generation, although it is extremely rare.
constexpr uint8_t LEVEL_MAX_OBJECTS = 175;        // Max objects per level
constexpr uint8_t MIN_TREASURE_LIST_ID = 1;       // Minimum treasure_list index used
constexpr uint8_t LEVEL_OBJECTS_PER_ROOM = 7;     // Amount of objects for rooms
constexpr uint8_t LEVEL_OBJECTS_PER_CORRIDOR = 2; // Amount of objects for corridors
constexpr uint8_t LEVEL_TOTAL_GOLD_AND_GEMS = 2;  // Amount of gold (and gems)

// Creature constants
constexpr uint16_t MON_MAX_CREATURES = 279; // Number of creatures defined for univ
constexpr uint8_t MON_ATTACK_TYPES = 215;   // Number of monster attack types.

// With MON_TOTAL_ALLOCATIONS set to 101, it is possible to get compacting
// monsters messages while breeding/cloning monsters.
constexpr uint8_t MON_TOTAL_ALLOCATIONS = 125;        // Max that can be allocated
constexpr uint8_t MON_MAX_LEVELS = 40;                // Maximum level of creatures
constexpr uint8_t MON_MAX_ATTACKS = 4;                // Max num attacks (used in mons memory) -CJS-

// Trap constants
constexpr uint8_t MAX_TRAPS = 18; // Number of defined traps

// Descriptive constants
constexpr uint8_t MAX_COLORS = 49;     // Used with potions
constexpr uint8_t MAX_MUSHROOMS = 22;  // Used with mushrooms
constexpr uint8_t MAX_WOODS = 25;      // Used with staffs
constexpr uint8_t MAX_METALS = 25;     // Used with wands
constexpr uint8_t MAX_ROCKS = 32;      // Used with rings
constexpr uint8_t MAX_AMULETS = 11;    // Used with amulets
constexpr uint8_t MAX_TITLES = 45;     // Used with scrolls
constexpr uint8_t MAX_SYLLABLES = 153; // Used with scrolls

// Player constants
constexpr uint8_t PLAYER_MAX_LEVEL = 40;            // Maximum possible character level
constexpr uint8_t PLAYER_MAX_CLASSES = 6;           // Number of defined classes
constexpr uint8_t PLAYER_MAX_RACES = 8;             // Number of defined races
constexpr uint8_t PLAYER_MAX_BACKGROUNDS = 128;     // Number of types of histories for univ

// class level adjustment constants
constexpr uint8_t CLASS_BTH = 0;
constexpr uint8_t CLASS_BTHB = 1;
constexpr uint8_t CLASS_DEVICE = 2;
constexpr uint8_t CLASS_DISARM = 3;
constexpr uint8_t CLASS_SAVE = 4;

// this depends on the fact that CLASS_SAVE values are all the same,
// if not, then should add a separate column for this
constexpr uint8_t CLASS_MISC_HIT = 4;
constexpr uint8_t CLASS_MAX_LEVEL_ADJUST = 5;

// Base to hit constants
constexpr uint8_t BTH_PER_PLUS_TO_HIT_ADJUST = 3; // Adjust BTH per plus-to-hit

// magic numbers for players equipment inventory array
constexpr uint8_t EQUIPMENT_WIELD = 22; // must be first item in equipment list
constexpr uint8_t EQUIPMENT_HEAD = 23;
constexpr uint8_t EQUIPMENT_NECK = 24;
constexpr uint8_t EQUIPMENT_BODY = 25;
constexpr uint8_t EQUIPMENT_ARM = 26;
constexpr uint8_t EQUIPMENT_HANDS = 27;
constexpr uint8_t EQUIPMENT_RIGHT = 28;
constexpr uint8_t EQUIPMENT_LEFT = 29;
constexpr uint8_t EQUIPMENT_FEET = 30;
constexpr uint8_t EQUIPMENT_OUTER = 31;
constexpr uint8_t EQUIPMENT_LIGHT = 32;
constexpr uint8_t EQUIPMENT_AUX = 33;

// Attribute indexes -CJS-
constexpr uint8_t A_STR = 0;
constexpr uint8_t A_INT = 1;
constexpr uint8_t A_WIS = 2;
constexpr uint8_t A_DEX = 3;
constexpr uint8_t A_CON = 4;
constexpr uint8_t A_CHR = 5;


// Fval definitions: these describe the various types of dungeon floors and
// walls, if numbers above 15 are ever used, then the test against MIN_CAVE_WALL
// will have to be changed, also the save routines will have to be changed.
constexpr uint8_t TILE_NULL_WALL = 0;
constexpr uint8_t TILE_DARK_FLOOR = 1;
constexpr uint8_t TILE_LIGHT_FLOOR = 2;
constexpr uint8_t MAX_CAVE_ROOM = 2;
constexpr uint8_t TILE_CORR_FLOOR = 3;
constexpr uint8_t TILE_BLOCKED_FLOOR = 4; // a corridor space with cl/st/se door or rubble
constexpr uint8_t MAX_CAVE_FLOOR = 4;

constexpr uint8_t MAX_OPEN_SPACE = 3;
constexpr uint8_t MIN_CLOSED_SPACE = 4;

constexpr uint8_t TMP1_WALL = 8;
constexpr uint8_t TMP2_WALL = 9;

constexpr uint8_t MIN_CAVE_WALL = 12;
constexpr uint8_t TILE_GRANITE_WALL = 12;
constexpr uint8_t TILE_MAGMA_WALL = 13;
constexpr uint8_t TILE_QUARTZ_WALL = 14;
constexpr uint8_t TILE_BOUNDARY_WALL = 15;

// Class spell types
constexpr uint8_t SPELL_TYPE_NONE = 0;
constexpr uint8_t SPELL_TYPE_MAGE = 1;
constexpr uint8_t SPELL_TYPE_PRIEST = 2;

// offsets to spell names in spell_names[] array
constexpr uint8_t NAME_OFFSET_SPELLS = 0;
constexpr uint8_t NAME_OFFSET_PRAYERS = 31;

// definitions for the pseudo-normal distribution generation
constexpr uint16_t NORMAL_TABLE_SIZE = 256;
constexpr uint8_t NORMAL_TABLE_SD = 64; // the standard deviation for the table

// definitions for the player's status field
constexpr uint32_t PY_HUNGRY = 0x00000001L;
constexpr uint32_t PY_WEAK = 0x00000002L;
constexpr uint32_t PY_BLIND = 0x00000004L;
constexpr uint32_t PY_CONFUSED = 0x00000008L;
constexpr uint32_t PY_FEAR = 0x00000010L;
constexpr uint32_t PY_POISONED = 0x00000020L;
constexpr uint32_t PY_FAST = 0x00000040L;
constexpr uint32_t PY_SLOW = 0x00000080L;
constexpr uint32_t PY_SEARCH = 0x00000100L;
constexpr uint32_t PY_REST = 0x00000200L;
constexpr uint32_t PY_STUDY = 0x00000400L;

constexpr uint32_t PY_INVULN = 0x00001000L;
constexpr uint32_t PY_HERO = 0x00002000L;
constexpr uint32_t PY_SHERO = 0x00004000L;
constexpr uint32_t PY_BLESSED = 0x00008000L;
constexpr uint32_t PY_DET_INV = 0x00010000L;
constexpr uint32_t PY_TIM_INFRA = 0x00020000L;
constexpr uint32_t PY_SPEED = 0x00040000L;
constexpr uint32_t PY_STR_WGT = 0x00080000L;
constexpr uint32_t PY_PARALYSED = 0x00100000L;
constexpr uint32_t PY_REPEAT = 0x00200000L;
constexpr uint32_t PY_ARMOR = 0x00400000L;

constexpr uint32_t PY_STATS = 0x3F000000L;
constexpr uint32_t PY_STR = 0x01000000L; // these 6 stat flags must be adjacent
constexpr uint32_t PY_INT = 0x02000000L;
constexpr uint32_t PY_WIS = 0x04000000L;
constexpr uint32_t PY_DEX = 0x08000000L;
constexpr uint32_t PY_CON = 0x10000000L;
constexpr uint32_t PY_CHR = 0x20000000L;

constexpr uint32_t PY_HP = 0x40000000L;
constexpr uint32_t PY_MANA = 0x80000000L;

// definitions for objects that can be worn
constexpr uint32_t TR_STATS = 0x0000003FL; // the stats must be the low 6 bits
constexpr uint32_t TR_STR = 0x00000001L;
constexpr uint32_t TR_INT = 0x00000002L;
constexpr uint32_t TR_WIS = 0x00000004L;
constexpr uint32_t TR_DEX = 0x00000008L;
constexpr uint32_t TR_CON = 0x00000010L;
constexpr uint32_t TR_CHR = 0x00000020L;
constexpr uint32_t TR_SEARCH = 0x00000040L;
constexpr uint32_t TR_SLOW_DIGEST = 0x00000080L;
constexpr uint32_t TR_STEALTH = 0x00000100L;
constexpr uint32_t TR_AGGRAVATE = 0x00000200L;
constexpr uint32_t TR_TELEPORT = 0x00000400L;
constexpr uint32_t TR_REGEN = 0x00000800L;
constexpr uint32_t TR_SPEED = 0x00001000L;

constexpr uint32_t TR_EGO_WEAPON = 0x0007E000L;
constexpr uint32_t TR_SLAY_DRAGON = 0x00002000L;
constexpr uint32_t TR_SLAY_ANIMAL = 0x00004000L;
constexpr uint32_t TR_SLAY_EVIL = 0x00008000L;
constexpr uint32_t TR_SLAY_UNDEAD = 0x00010000L;
constexpr uint32_t TR_FROST_BRAND = 0x00020000L;
constexpr uint32_t TR_FLAME_TONGUE = 0x00040000L;

constexpr uint32_t TR_RES_FIRE = 0x00080000L;
constexpr uint32_t TR_RES_ACID = 0x00100000L;
constexpr uint32_t TR_RES_COLD = 0x00200000L;
constexpr uint32_t TR_SUST_STAT = 0x00400000L;
constexpr uint32_t TR_FREE_ACT = 0x00800000L;
constexpr uint32_t TR_SEE_INVIS = 0x01000000L;
constexpr uint32_t TR_RES_LIGHT = 0x02000000L;
constexpr uint32_t TR_FFALL = 0x04000000L;
constexpr uint32_t TR_BLIND = 0x08000000L;
constexpr uint32_t TR_TIMID = 0x10000000L;
constexpr uint32_t TR_TUNNEL = 0x20000000L;
constexpr uint32_t TR_INFRA = 0x40000000L;
constexpr uint32_t TR_CURSED = 0x80000000L;

// definitions for chests
constexpr uint32_t CH_LOCKED = 0x00000001L;
constexpr uint32_t CH_TRAPPED = 0x000001F0L;
constexpr uint32_t CH_LOSE_STR = 0x00000010L;
constexpr uint32_t CH_POISON = 0x00000020L;
constexpr uint32_t CH_PARALYSED = 0x00000040L;
constexpr uint32_t CH_EXPLODE = 0x00000080L;
constexpr uint32_t CH_SUMMON = 0x00000100L;

// definitions for creatures, cmove field
constexpr uint32_t CM_ALL_MV_FLAGS = 0x0000003FL;
constexpr uint32_t CM_ATTACK_ONLY = 0x00000001L;
constexpr uint32_t CM_MOVE_NORMAL = 0x00000002L;
// For Quylthulgs, which have no physical movement.
constexpr uint32_t CM_ONLY_MAGIC = 0x00000004L;

constexpr uint32_t CM_RANDOM_MOVE = 0x00000038L;
constexpr uint32_t CM_20_RANDOM = 0x00000008L;
constexpr uint32_t CM_40_RANDOM = 0x00000010L;
constexpr uint32_t CM_75_RANDOM = 0x00000020L;

constexpr uint32_t CM_SPECIAL = 0x003F0000L;
constexpr uint32_t CM_INVISIBLE = 0x00010000L;
constexpr uint32_t CM_OPEN_DOOR = 0x00020000L;
constexpr uint32_t CM_PHASE = 0x00040000L;
constexpr uint32_t CM_EATS_OTHER = 0x00080000L;
constexpr uint32_t CM_PICKS_UP = 0x00100000L;
constexpr uint32_t CM_MULTIPLY = 0x00200000L;

constexpr uint32_t CM_SMALL_OBJ = 0x00800000L;
constexpr uint32_t CM_CARRY_OBJ = 0x01000000L;
constexpr uint32_t CM_CARRY_GOLD = 0x02000000L;
constexpr uint32_t CM_TREASURE = 0x7C000000L;
constexpr uint32_t CM_TR_SHIFT = 26; // used for recall of treasure
constexpr uint32_t CM_60_RANDOM = 0x04000000L;
constexpr uint32_t CM_90_RANDOM = 0x08000000L;
constexpr uint32_t CM_1D2_OBJ = 0x10000000L;
constexpr uint32_t CM_2D2_OBJ = 0x20000000L;
constexpr uint32_t CM_4D2_OBJ = 0x40000000L;
constexpr uint32_t CM_WIN = 0x80000000L;

// creature spell definitions
constexpr uint32_t CS_FREQ = 0x0000000FL;
constexpr uint32_t CS_SPELLS = 0x0001FFF0L;
constexpr uint32_t CS_TEL_SHORT = 0x00000010L;
constexpr uint32_t CS_TEL_LONG = 0x00000020L;
constexpr uint32_t CS_TEL_TO = 0x00000040L;
constexpr uint32_t CS_LGHT_WND = 0x00000080L;
constexpr uint32_t CS_SER_WND = 0x00000100L;
constexpr uint32_t CS_HOLD_PER = 0x00000200L;
constexpr uint32_t CS_BLIND = 0x00000400L;
constexpr uint32_t CS_CONFUSE = 0x00000800L;
constexpr uint32_t CS_FEAR = 0x00001000L;
constexpr uint32_t CS_SUMMON_MON = 0x00002000L;
constexpr uint32_t CS_SUMMON_UND = 0x00004000L;
constexpr uint32_t CS_SLOW_PER = 0x00008000L;
constexpr uint32_t CS_DRAIN_MANA = 0x00010000L;

constexpr uint32_t CS_BREATHE = 0x00F80000L;  // may also just indicate resistance
constexpr uint32_t CS_BR_LIGHT = 0x00080000L; // if no spell frequency set
constexpr uint32_t CS_BR_GAS = 0x00100000L;
constexpr uint32_t CS_BR_ACID = 0x00200000L;
constexpr uint32_t CS_BR_FROST = 0x00400000L;
constexpr uint32_t CS_BR_FIRE = 0x00800000L;

// creature defense flags
constexpr uint16_t CD_DRAGON = 0x0001;
constexpr uint16_t CD_ANIMAL = 0x0002;
constexpr uint16_t CD_EVIL = 0x0004;
constexpr uint16_t CD_UNDEAD = 0x0008;
constexpr uint16_t CD_WEAKNESS = 0x03F0;
constexpr uint16_t CD_FROST = 0x0010;
constexpr uint16_t CD_FIRE = 0x0020;
constexpr uint16_t CD_POISON = 0x0040;
constexpr uint16_t CD_ACID = 0x0080;
constexpr uint16_t CD_LIGHT = 0x0100;
constexpr uint16_t CD_STONE = 0x0200;

constexpr uint16_t CD_NO_SLEEP = 0x1000;
constexpr uint16_t CD_INFRA = 0x2000;
constexpr uint16_t CD_MAX_HP = 0x4000;

// id's used for object description, stored in objects_identified array
constexpr uint8_t OD_TRIED = 0x1;
constexpr uint8_t OD_KNOWN1 = 0x2;

// id's used for item description, stored in i_ptr->ident
constexpr uint8_t ID_MAGIK = 0x1;
constexpr uint8_t ID_DAMD = 0x2;
constexpr uint8_t ID_EMPTY = 0x4;
constexpr uint8_t ID_KNOWN2 = 0x8;
constexpr uint8_t ID_STORE_BOUGHT = 0x10;
constexpr uint8_t ID_SHOW_HIT_DAM = 0x20;
constexpr uint8_t ID_NO_SHOW_P1 = 0x40;
constexpr uint8_t ID_SHOW_P1 = 0x80;

// indexes into the special name table
constexpr uint8_t SN_NULL = 0;
constexpr uint8_t SN_R = 1;
constexpr uint8_t SN_RA = 2;
constexpr uint8_t SN_RF = 3;
constexpr uint8_t SN_RC = 4;
constexpr uint8_t SN_RL = 5;
constexpr uint8_t SN_HA = 6;
constexpr uint8_t SN_DF = 7;
constexpr uint8_t SN_SA = 8;
constexpr uint8_t SN_SD = 9;
constexpr uint8_t SN_SE = 10;
constexpr uint8_t SN_SU = 11;
constexpr uint8_t SN_FT = 12;
constexpr uint8_t SN_FB = 13;
constexpr uint8_t SN_FREE_ACTION = 14;
constexpr uint8_t SN_SLAYING = 15;
constexpr uint8_t SN_CLUMSINESS = 16;
constexpr uint8_t SN_WEAKNESS = 17;
constexpr uint8_t SN_SLOW_DESCENT = 18;
constexpr uint8_t SN_SPEED = 19;
constexpr uint8_t SN_STEALTH = 20;
constexpr uint8_t SN_SLOWNESS = 21;
constexpr uint8_t SN_NOISE = 22;
constexpr uint8_t SN_GREAT_MASS = 23;
constexpr uint8_t SN_INTELLIGENCE = 24;
constexpr uint8_t SN_WISDOM = 25;
constexpr uint8_t SN_INFRAVISION = 26;
constexpr uint8_t SN_MIGHT = 27;
constexpr uint8_t SN_LORDLINESS = 28;
constexpr uint8_t SN_MAGI = 29;
constexpr uint8_t SN_BEAUTY = 30;
constexpr uint8_t SN_SEEING = 31;
constexpr uint8_t SN_REGENERATION = 32;
constexpr uint8_t SN_STUPIDITY = 33;
constexpr uint8_t SN_DULLNESS = 34;
constexpr uint8_t SN_BLINDNESS = 35;
constexpr uint8_t SN_TIMIDNESS = 36;
constexpr uint8_t SN_TELEPORTATION = 37;
constexpr uint8_t SN_UGLINESS = 38;
constexpr uint8_t SN_PROTECTION = 39;
constexpr uint8_t SN_IRRITATION = 40;
constexpr uint8_t SN_VULNERABILITY = 41;
constexpr uint8_t SN_ENVELOPING = 42;
constexpr uint8_t SN_FIRE = 43;
constexpr uint8_t SN_SLAY_EVIL = 44;
constexpr uint8_t SN_DRAGON_SLAYING = 45;
constexpr uint8_t SN_EMPTY = 46;
constexpr uint8_t SN_LOCKED = 47;
constexpr uint8_t SN_POISON_NEEDLE = 48;
constexpr uint8_t SN_GAS_TRAP = 49;
constexpr uint8_t SN_EXPLOSION_DEVICE = 50;
constexpr uint8_t SN_SUMMONING_RUNES = 51;
constexpr uint8_t SN_MULTIPLE_TRAPS = 52;
constexpr uint8_t SN_DISARMED = 53;
constexpr uint8_t SN_UNLOCKED = 54;
constexpr uint8_t SN_SLAY_ANIMAL = 55;
constexpr uint8_t SN_ARRAY_SIZE = 56; // must be at end of this list

// defines for treasure type values (tval)
constexpr int8_t TV_NEVER = -1; // used by find_range() for non-search
constexpr uint8_t TV_NOTHING = 0;
constexpr uint8_t TV_MISC = 1;
constexpr uint8_t TV_CHEST = 2;

// min tval for wearable items, all items between TV_MIN_WEAR and
// TV_MAX_WEAR use the same flag bits, see the TR_* defines.
constexpr uint8_t TV_MIN_WEAR = 10;

// items tested for enchantments, i.e. the MAGIK inscription,
// see the enchanted() procedure.
constexpr uint8_t TV_MIN_ENCHANT = 10;
constexpr uint8_t TV_SLING_AMMO = 10;
constexpr uint8_t TV_BOLT = 11;
constexpr uint8_t TV_ARROW = 12;
constexpr uint8_t TV_SPIKE = 13;
constexpr uint8_t TV_LIGHT = 15;
constexpr uint8_t TV_BOW = 20;
constexpr uint8_t TV_HAFTED = 21;
constexpr uint8_t TV_POLEARM = 22;
constexpr uint8_t TV_SWORD = 23;
constexpr uint8_t TV_DIGGING = 25;
constexpr uint8_t TV_BOOTS = 30;
constexpr uint8_t TV_GLOVES = 31;
constexpr uint8_t TV_CLOAK = 32;
constexpr uint8_t TV_HELM = 33;
constexpr uint8_t TV_SHIELD = 34;
constexpr uint8_t TV_HARD_ARMOR = 35;
constexpr uint8_t TV_SOFT_ARMOR = 36;
// max tval that uses the TR_* flags
constexpr uint8_t TV_MAX_ENCHANT = 39;
constexpr uint8_t TV_AMULET = 40;
constexpr uint8_t TV_RING = 45;
// max tval for wearable items
constexpr uint8_t TV_MAX_WEAR = 50;
constexpr uint8_t TV_STAFF = 55;
constexpr uint8_t TV_WAND = 65;
constexpr uint8_t TV_SCROLL1 = 70;
constexpr uint8_t TV_SCROLL2 = 71;
constexpr uint8_t TV_POTION1 = 75;
constexpr uint8_t TV_POTION2 = 76;
constexpr uint8_t TV_FLASK = 77;
constexpr uint8_t TV_FOOD = 80;
constexpr uint8_t TV_MAGIC_BOOK = 90;
constexpr uint8_t TV_PRAYER_BOOK = 91;
// objects with tval above this are never picked up by monsters
constexpr uint8_t TV_MAX_OBJECT = 99;
constexpr uint8_t TV_GOLD = 100;
// objects with higher tvals can not be picked up
constexpr uint8_t TV_MAX_PICK_UP = 100;
constexpr uint8_t TV_INVIS_TRAP = 101;
// objects between TV_MIN_VISIBLE and TV_MAX_VISIBLE are always visible,
// i.e. the cave fm flag is set when they are present
constexpr uint8_t TV_MIN_VISIBLE = 102;
constexpr uint8_t TV_VIS_TRAP = 102;
constexpr uint8_t TV_RUBBLE = 103;
// following objects are never deleted when trying to create another one
// during level generation
constexpr uint8_t TV_MIN_DOORS = 104;
constexpr uint8_t TV_OPEN_DOOR = 104;
constexpr uint8_t TV_CLOSED_DOOR = 105;
constexpr uint8_t TV_UP_STAIR = 107;
constexpr uint8_t TV_DOWN_STAIR = 108;
constexpr uint8_t TV_SECRET_DOOR = 109;
constexpr uint8_t TV_STORE_DOOR = 110;
constexpr uint8_t TV_MAX_VISIBLE = 110;

// spell types used by get_flags(), breathe(), fire_bolt() and fire_ball()
constexpr uint8_t GF_MAGIC_MISSILE = 0;
constexpr uint8_t GF_LIGHTNING = 1;
constexpr uint8_t GF_POISON_GAS = 2;
constexpr uint8_t GF_ACID = 3;
constexpr uint8_t GF_FROST = 4;
constexpr uint8_t GF_FIRE = 5;
constexpr uint8_t GF_HOLY_ORB = 6;
