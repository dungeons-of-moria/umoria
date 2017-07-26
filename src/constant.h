// Copyright (c) 1989-94 James E. Wilson, Robert A. Koeneke
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Global constants used by Moria
// clang-format off

#pragma once

#include "headers.h"

constexpr uint8_t MAX_UCHAR = std::numeric_limits<uint8_t>::max();
constexpr int16_t MAX_SHORT = std::numeric_limits<int16_t>::max(); // maximum short/long signed ints
constexpr int32_t MAX_LONG = std::numeric_limits<int32_t>::max();

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

// message line location
constexpr int32_t MSG_LINE = 0;

// number of messages to save in a buffer
constexpr int32_t MAX_SAVE_MSG = 22; // How many messages to save -CJS-

// Dungeon size parameters
constexpr int32_t MAX_HEIGHT = 66; // Multiple of 11; >= 22
constexpr int32_t MAX_WIDTH = 198; // Multiple of 33; >= 66
constexpr int32_t SCREEN_HEIGHT = 22;
constexpr int32_t SCREEN_WIDTH = 66;
constexpr int32_t QUART_HEIGHT = (SCREEN_HEIGHT / 4);
constexpr int32_t QUART_WIDTH = (SCREEN_WIDTH / 4);

// Dungeon generation values
// Note: The entire design of dungeon can be changed by only slight adjustments here.
constexpr int32_t DUN_TUN_RND = 9; // 1/Chance of Random direction
constexpr int32_t DUN_TUN_CHG = 70; // Chance of changing direction (99 max)
constexpr int32_t DUN_TUN_CON = 15; // Chance of extra tunneling
constexpr int32_t DUN_ROO_MEA = 32; // Mean of # of rooms, standard dev2
constexpr int32_t DUN_TUN_PEN = 25; // % chance of room doors
constexpr int32_t DUN_TUN_JCT = 15; // % chance of doors at tunnel junctions
constexpr int32_t DUN_STR_DEN = 5; // Density of streamers
constexpr int32_t DUN_STR_RNG = 2; // Width of streamers
constexpr int32_t DUN_STR_MAG = 3; // Number of magma streamers
constexpr int32_t DUN_STR_MC = 90; // 1/x chance of treasure per magma
constexpr int32_t DUN_STR_QUA = 2; // Number of quartz streamers
constexpr int32_t DUN_STR_QC = 40; // 1/x chance of treasure per quartz
constexpr int32_t DUN_UNUSUAL = 300; // Level/x chance of unusual room

// Store constants
constexpr int32_t MAX_OWNERS = 18; // Number of owners to choose from
constexpr int32_t MAX_STORES = 6; // Number of different stores
constexpr int32_t STORE_INVEN_MAX = 24; // Max number of discrete objects in inventory
constexpr int32_t STORE_CHOICES = 26; // NUMBER of items to choose stock from
constexpr int32_t STORE_MAX_INVEN = 18; // Max diff objects in stock for auto buy
constexpr int32_t STORE_MIN_INVEN = 10; // Min diff objects in stock for auto sell
constexpr int32_t STORE_TURN_AROUND = 9; // Amount of buying and selling normally
constexpr int32_t COST_ADJ = 100; // Adjust prices for buying and selling

// Treasure constants
constexpr int32_t INVEN_ARRAY_SIZE = 34; // Size of inventory array(Do not change)
constexpr int32_t MAX_OBJ_LEVEL = 50; // Maximum level of magic in dungeon
constexpr int32_t OBJ_GREAT = 12; // 1/n Chance of item being a Great Item

// Note that the following constants are all related, if you change one, you
// must also change all succeeding ones. Also, player_base_provisions[] and
// store_choices[] may also have to be changed.
constexpr int32_t MAX_OBJECTS = 420; // Number of objects for universe
constexpr int32_t MAX_DUNGEON_OBJ = 344; // Number of dungeon objects
constexpr int32_t OBJ_OPEN_DOOR = 367;
constexpr int32_t OBJ_CLOSED_DOOR = 368;
constexpr int32_t OBJ_SECRET_DOOR = 369;
constexpr int32_t OBJ_UP_STAIR = 370;
constexpr int32_t OBJ_DOWN_STAIR = 371;
constexpr int32_t OBJ_STORE_DOOR = 372;
constexpr int32_t OBJ_TRAP_LIST = 378;
constexpr int32_t OBJ_RUBBLE = 396;
constexpr int32_t OBJ_MUSH = 397;
constexpr int32_t OBJ_SCARE_MON = 398;
constexpr int32_t OBJ_GOLD_LIST = 399;
constexpr int32_t OBJ_NOTHING = 417;
constexpr int32_t OBJ_RUINED_CHEST = 418;
constexpr int32_t OBJ_WIZARD = 419;
constexpr int32_t OBJECT_IDENT_SIZE = 448; // 7*64, see object_offset() in desc.c, could be MAX_OBJECTS o_o() rewritten
constexpr int32_t MAX_GOLD = 18; // Number of different types of gold

// with MAX_TALLOC = 150, it is possible to get compacting objects during
// level generation, although it is extremely rare
constexpr int32_t MAX_TALLOC = 175; // Max objects per level
constexpr int32_t MIN_TRIX = 1; // Minimum treasure_list index used
constexpr int32_t TREAS_ROOM_ALLOC = 7; // Amount of objects for rooms
constexpr int32_t TREAS_ANY_ALLOC = 2; // Amount of objects for corridors
constexpr int32_t TREAS_GOLD_ALLOC = 2; // Amount of gold (and gems)

// Magic Treasure Generation constants
// Note: Number of special objects, and degree of enchantments can be adjusted here.
constexpr int32_t OBJ_STD_ADJ = 125; // Adjust STD per level * 100
constexpr int32_t OBJ_STD_MIN = 7; // Minimum STD
constexpr int32_t OBJ_TOWN_LEVEL = 7; // Town object generation level
constexpr int32_t OBJ_BASE_MAGIC = 15; // Base amount of magic
constexpr int32_t OBJ_BASE_MAX = 70; // Max amount of magic
constexpr int32_t OBJ_DIV_SPECIAL = 6; // magic_chance/# special magic
constexpr int32_t OBJ_DIV_CURSED = 13; // 10*magic_chance/# cursed items

// Constants describing limits of certain objects
constexpr int32_t OBJ_LAMP_MAX = 15000; // Maximum amount that lamp can be filled
constexpr int32_t OBJ_BOLT_RANGE = 18; // Maximum range of bolts and balls
constexpr int32_t OBJ_RUNE_PROT = 3000; // Rune of protection resistance

// Creature constants
constexpr int32_t MAX_CREATURES = 279; // Number of creatures defined for univ
constexpr int32_t N_MONS_ATTS = 215; // Number of monster attack types.

// with MAX_MALLOC = 101, it is possible to get compacting monsters messages
// while breeding/cloning monsters
constexpr int32_t MAX_MALLOC = 125; // Max that can be allocated
constexpr int32_t MAX_MALLOC_CHANCE = 160; // 1/x chance of new monster each round
constexpr int32_t MAX_MONS_LEVEL = 40; // Maximum level of creatures
constexpr int32_t MAX_SIGHT = 20; // Maximum dis a creature can be seen
constexpr int32_t MAX_SPELL_DIS = 20; // Maximum dis creature spell can be cast
constexpr int32_t MAX_MON_MULT = 75; // Maximum reproductions on a level
constexpr int32_t MON_MULT_ADJ = 7; // High value slows multiplication
constexpr int32_t MON_NASTY = 50; // 1/x chance of high level creature
constexpr int32_t MIN_MALLOC_LEVEL = 14; // Minimum number of monsters/level
constexpr int32_t MIN_MALLOC_TD = 4; // Number of people on town level (day)
constexpr int32_t MIN_MALLOC_TN = 8; // Number of people on town level (night)
constexpr int32_t WIN_MON_TOT = 2; // Total number of "win" creatures
constexpr int32_t WIN_MON_APPEAR = 50; // Level where winning creatures begin
constexpr int32_t MON_SUMMON_ADJ = 2; // Adjust level of summoned creatures
constexpr int32_t MON_DRAIN_LIFE = 2; // Percent of player exp drained per hit
constexpr int32_t MAX_MON_NATTACK = 4; // Max num attacks (used in mons memory) -CJS-
constexpr int32_t MIN_MONIX = 2; // Minimum index in m_list (1 = py, 0 = no mon)

// Trap constants
constexpr int32_t MAX_TRAP = 18; // Number of defined traps

// FIXME: this const is not used anywhere!
//constexpr int32_t SCARE_MONSTER = 99

// Descriptive constants
constexpr int32_t MAX_COLORS = 49; // Used with potions
constexpr int32_t MAX_MUSH = 22; // Used with mushrooms
constexpr int32_t MAX_WOODS = 25; // Used with staffs
constexpr int32_t MAX_METALS = 25; // Used with wands
constexpr int32_t MAX_ROCKS = 32; // Used with rings
constexpr int32_t MAX_AMULETS = 11; // Used with amulets
constexpr int32_t MAX_TITLES = 45; // Used with scrolls
constexpr int32_t MAX_SYLLABLES = 153; // Used with scrolls

// Player constants
constexpr int32_t MAX_PLAYER_LEVEL = 40; // Maximum possible character level
constexpr int32_t MAX_EXP = 9999999L; // Maximum amount of experience -CJS-
constexpr int32_t MAX_RACES = 8; // Number of defined races
constexpr int32_t MAX_CLASS = 6; // Number of defined classes
constexpr int32_t USE_DEVICE = 3; // x> Harder devices x< Easier devices
constexpr int32_t MAX_BACKGROUND = 128; // Number of types of histories for univ
constexpr int32_t PLAYER_FOOD_FULL = 10000; // Getting full
constexpr int32_t PLAYER_FOOD_MAX = 15000; // Maximum food value, beyond is wasted
constexpr int32_t PLAYER_FOOD_FAINT = 300; // Character begins fainting
constexpr int32_t PLAYER_FOOD_WEAK = 1000; // Warn player that he is getting very low
constexpr int32_t PLAYER_FOOD_ALERT = 2000; // Warn player that he is getting low
constexpr int32_t PLAYER_REGEN_FAINT = 33; // Regen factor*2^16 when fainting
constexpr int32_t PLAYER_REGEN_WEAK = 98; // Regen factor*2^16 when weak
constexpr int32_t PLAYER_REGEN_NORMAL = 197; // Regen factor*2^16 when full
constexpr int32_t PLAYER_REGEN_HPBASE = 1442; // Min amount hp regen*2^16
constexpr int32_t PLAYER_REGEN_MNBASE = 524; // Min amount mana regen*2^16
constexpr int32_t PLAYER_WEIGHT_CAP = 130; // "#"*(1/10 pounds) per strength point
constexpr int32_t PLAYER_EXIT_PAUSE = 2; // Pause time before player can re-roll

// class level adjustment constants
constexpr int32_t CLA_BTH = 0;
constexpr int32_t CLA_BTHB = 1;
constexpr int32_t CLA_DEVICE = 2;
constexpr int32_t CLA_DISARM = 3;
constexpr int32_t CLA_SAVE = 4;
// this depends on the fact that CLA_SAVE values are all the same, if not,
// then should add a separate column for this
constexpr int32_t CLA_MISC_HIT = 4;
constexpr int32_t MAX_LEV_ADJ = 5;

// Base to hit constants
constexpr int32_t BTH_PLUS_ADJ = 3; // Adjust BTH per plus-to-hit

// magic numbers for players inventory array
constexpr int32_t INVEN_WIELD = 22; // must be first item in equipment list
constexpr int32_t INVEN_HEAD = 23;
constexpr int32_t INVEN_NECK = 24;
constexpr int32_t INVEN_BODY = 25;
constexpr int32_t INVEN_ARM = 26;
constexpr int32_t INVEN_HANDS = 27;
constexpr int32_t INVEN_RIGHT = 28;
constexpr int32_t INVEN_LEFT = 29;
constexpr int32_t INVEN_FEET = 30;
constexpr int32_t INVEN_OUTER = 31;
constexpr int32_t INVEN_LIGHT = 32;
constexpr int32_t INVEN_AUX = 33;

// Attribute indexes -CJS-
constexpr int32_t A_STR = 0;
constexpr int32_t A_INT = 1;
constexpr int32_t A_WIS = 2;
constexpr int32_t A_DEX = 3;
constexpr int32_t A_CON = 4;
constexpr int32_t A_CHR = 5;

constexpr char CTRL_KEY (char x) {
    return ((x) & 0x1F);
}

#undef DELETE
constexpr char DELETE = 0x7f;

#undef ESCAPE
constexpr char ESCAPE = '\033'; // ESCAPE character -CJS-

// This used to be NULL, but that was technically incorrect.
// CNIL is used instead of null to help avoid lint errors.
#ifndef CNIL
constexpr char* CNIL = 0;
#endif

// Fval definitions: these describe the various types of dungeon floors and
// walls, if numbers above 15 are ever used, then the test against MIN_CAVE_WALL
// will have to be changed, also the save routines will have to be changed.
constexpr int32_t NULL_WALL = 0;
constexpr int32_t DARK_FLOOR = 1;
constexpr int32_t LIGHT_FLOOR = 2;
constexpr int32_t MAX_CAVE_ROOM = 2;
constexpr int32_t CORR_FLOOR = 3;
constexpr int32_t BLOCKED_FLOOR = 4; // a corridor space with cl/st/se door or rubble
constexpr int32_t MAX_CAVE_FLOOR = 4;

constexpr int32_t MAX_OPEN_SPACE = 3;
constexpr int32_t MIN_CLOSED_SPACE = 4;

constexpr int32_t TMP1_WALL = 8;
constexpr int32_t TMP2_WALL = 9;

constexpr int32_t MIN_CAVE_WALL = 12;
constexpr int32_t GRANITE_WALL = 12;
constexpr int32_t MAGMA_WALL = 13;
constexpr int32_t QUARTZ_WALL = 14;
constexpr int32_t BOUNDARY_WALL = 15;

// Column for stats
constexpr int32_t STAT_COLUMN = 0;

// Class spell types
constexpr int32_t NONE = 0;
constexpr int32_t MAGE = 1;
constexpr int32_t PRIEST = 2;

// offsets to spell names in spell_names[] array
constexpr int32_t SPELL_OFFSET = 0;
constexpr int32_t PRAYER_OFFSET = 31;

// definitions for the pseudo-normal distribution generation
constexpr int32_t NORMAL_TABLE_SIZE = 256;
constexpr int32_t NORMAL_TABLE_SD = 64; // the standard deviation for the table

// definitions for the player's status field
constexpr int32_t PY_HUNGRY = 0x00000001L;
constexpr int32_t PY_WEAK = 0x00000002L;
constexpr int32_t PY_BLIND = 0x00000004L;
constexpr int32_t PY_CONFUSED = 0x00000008L;
constexpr int32_t PY_FEAR = 0x00000010L;
constexpr int32_t PY_POISONED = 0x00000020L;
constexpr int32_t PY_FAST = 0x00000040L;
constexpr int32_t PY_SLOW = 0x00000080L;
constexpr int32_t PY_SEARCH = 0x00000100L;
constexpr int32_t PY_REST = 0x00000200L;
constexpr int32_t PY_STUDY = 0x00000400L;

constexpr int32_t PY_INVULN = 0x00001000L;
constexpr int32_t PY_HERO = 0x00002000L;
constexpr int32_t PY_SHERO = 0x00004000L;
constexpr int32_t PY_BLESSED = 0x00008000L;
constexpr int32_t PY_DET_INV = 0x00010000L;
constexpr int32_t PY_TIM_INFRA = 0x00020000L;
constexpr int32_t PY_SPEED = 0x00040000L;
constexpr int32_t PY_STR_WGT = 0x00080000L;
constexpr int32_t PY_PARALYSED = 0x00100000L;
constexpr int32_t PY_REPEAT = 0x00200000L;
constexpr int32_t PY_ARMOR = 0x00400000L;

constexpr int32_t PY_STATS = 0x3F000000L;
constexpr int32_t PY_STR = 0x01000000L; // these 6 stat flags must be adjacent

// FIXME: these const are not used anywhere!
//constexpr int32_t PY_INT = 0x02000000L;
//constexpr int32_t PY_WIS = 0x04000000L;
//constexpr int32_t PY_DEX = 0x08000000L;
//constexpr int32_t PY_CON = 0x10000000L;
//constexpr int32_t PY_CHR = 0x20000000L;

constexpr int32_t PY_HP = 0x40000000L;
constexpr int32_t PY_MANA = 0x80000000L;

// definitions for objects that can be worn
constexpr int32_t TR_STATS = 0x0000003FL; // the stats must be the low 6 bits
constexpr int32_t TR_STR = 0x00000001L;
constexpr int32_t TR_INT = 0x00000002L;
constexpr int32_t TR_WIS = 0x00000004L;
constexpr int32_t TR_DEX = 0x00000008L;
constexpr int32_t TR_CON = 0x00000010L;
constexpr int32_t TR_CHR = 0x00000020L;
constexpr int32_t TR_SEARCH = 0x00000040L;
constexpr int32_t TR_SLOW_DIGEST = 0x00000080L;
constexpr int32_t TR_STEALTH = 0x00000100L;
constexpr int32_t TR_AGGRAVATE = 0x00000200L;
constexpr int32_t TR_TELEPORT = 0x00000400L;
constexpr int32_t TR_REGEN = 0x00000800L;
constexpr int32_t TR_SPEED = 0x00001000L;

constexpr int32_t TR_EGO_WEAPON = 0x0007E000L;
constexpr int32_t TR_SLAY_DRAGON = 0x00002000L;
constexpr int32_t TR_SLAY_ANIMAL = 0x00004000L;
constexpr int32_t TR_SLAY_EVIL = 0x00008000L;
constexpr int32_t TR_SLAY_UNDEAD = 0x00010000L;
constexpr int32_t TR_FROST_BRAND = 0x00020000L;
constexpr int32_t TR_FLAME_TONGUE= 0x00040000L;

constexpr int32_t TR_RES_FIRE = 0x00080000L;
constexpr int32_t TR_RES_ACID = 0x00100000L;
constexpr int32_t TR_RES_COLD = 0x00200000L;
constexpr int32_t TR_SUST_STAT = 0x00400000L;
constexpr int32_t TR_FREE_ACT = 0x00800000L;
constexpr int32_t TR_SEE_INVIS = 0x01000000L;
constexpr int32_t TR_RES_LIGHT = 0x02000000L;
constexpr int32_t TR_FFALL = 0x04000000L;
constexpr int32_t TR_BLIND = 0x08000000L;
constexpr int32_t TR_TIMID = 0x10000000L;
constexpr int32_t TR_TUNNEL = 0x20000000L;
constexpr int32_t TR_INFRA = 0x40000000L;
constexpr int32_t TR_CURSED = 0x80000000L;

// definitions for chests
constexpr int32_t CH_LOCKED = 0x00000001L;
constexpr int32_t CH_TRAPPED = 0x000001F0L;
constexpr int32_t CH_LOSE_STR = 0x00000010L;
constexpr int32_t CH_POISON = 0x00000020L;
constexpr int32_t CH_PARALYSED = 0x00000040L;
constexpr int32_t CH_EXPLODE = 0x00000080L;
constexpr int32_t CH_SUMMON = 0x00000100L;

// definitions for creatures, cmove field
constexpr int32_t CM_ALL_MV_FLAGS= 0x0000003FL;
constexpr int32_t CM_ATTACK_ONLY = 0x00000001L;
constexpr int32_t CM_MOVE_NORMAL = 0x00000002L;
// For Quylthulgs, which have no physical movement.
constexpr int32_t CM_ONLY_MAGIC = 0x00000004L;

constexpr int32_t CM_RANDOM_MOVE = 0x00000038L;
constexpr int32_t CM_20_RANDOM = 0x00000008L;
constexpr int32_t CM_40_RANDOM = 0x00000010L;
constexpr int32_t CM_75_RANDOM = 0x00000020L;

constexpr int32_t CM_SPECIAL = 0x003F0000L;
constexpr int32_t CM_INVISIBLE = 0x00010000L;
constexpr int32_t CM_OPEN_DOOR = 0x00020000L;
constexpr int32_t CM_PHASE = 0x00040000L;
constexpr int32_t CM_EATS_OTHER = 0x00080000L;
constexpr int32_t CM_PICKS_UP = 0x00100000L;
constexpr int32_t CM_MULTIPLY = 0x00200000L;

constexpr int32_t CM_SMALL_OBJ = 0x00800000L;
constexpr int32_t CM_CARRY_OBJ = 0x01000000L;
constexpr int32_t CM_CARRY_GOLD = 0x02000000L;
constexpr int32_t CM_TREASURE = 0x7C000000L;
constexpr int32_t CM_TR_SHIFT = 26; // used for recall of treasure
constexpr int32_t CM_60_RANDOM = 0x04000000L;
constexpr int32_t CM_90_RANDOM = 0x08000000L;
constexpr int32_t CM_1D2_OBJ = 0x10000000L;
constexpr int32_t CM_2D2_OBJ = 0x20000000L;
constexpr int32_t CM_4D2_OBJ = 0x40000000L;
constexpr int32_t CM_WIN = 0x80000000L;

// creature spell definitions
constexpr int32_t CS_FREQ = 0x0000000FL;
constexpr int32_t CS_SPELLS = 0x0001FFF0L;
constexpr int32_t CS_TEL_SHORT = 0x00000010L;

// FIXME: these const are not used anywhere!
//constexpr int32_t CS_TEL_LONG = 0x00000020L;
//constexpr int32_t CS_TEL_TO = 0x00000040L;
//constexpr int32_t CS_LGHT_WND = 0x00000080L;
//constexpr int32_t CS_SER_WND = 0x00000100L;
//constexpr int32_t CS_HOLD_PER = 0x00000200L;
//constexpr int32_t CS_BLIND = 0x00000400L;
//constexpr int32_t CS_CONFUSE = 0x00000800L;
//constexpr int32_t CS_FEAR = 0x00001000L;
//constexpr int32_t CS_SUMMON_MON = 0x00002000L;
//constexpr int32_t CS_SUMMON_UND = 0x00004000L;
//constexpr int32_t CS_SLOW_PER = 0x00008000L;
//constexpr int32_t CS_DRAIN_MANA = 0x00010000L;

constexpr int32_t CS_BREATHE = 0x00F80000L; // may also just indicate resistance
constexpr int32_t CS_BR_LIGHT = 0x00080000L; // if no spell frequency set
constexpr int32_t CS_BR_GAS = 0x00100000L;
constexpr int32_t CS_BR_ACID = 0x00200000L;
constexpr int32_t CS_BR_FROST = 0x00400000L;
constexpr int32_t CS_BR_FIRE = 0x00800000L;

// creature defense flags
constexpr int32_t CD_DRAGON = 0x0001;
constexpr int32_t CD_ANIMAL = 0x0002;
constexpr int32_t CD_EVIL = 0x0004;
constexpr int32_t CD_UNDEAD = 0x0008;
constexpr int32_t CD_WEAKNESS = 0x03F0;
constexpr int32_t CD_FROST = 0x0010;
constexpr int32_t CD_FIRE = 0x0020;
constexpr int32_t CD_POISON = 0x0040;
constexpr int32_t CD_ACID = 0x0080;
constexpr int32_t CD_LIGHT = 0x0100;
constexpr int32_t CD_STONE = 0x0200;

constexpr int32_t CD_NO_SLEEP = 0x1000;
constexpr int32_t CD_INFRA = 0x2000;
constexpr int32_t CD_MAX_HP = 0x4000;

// FIXME: some of these const are not used anywhere!
// inventory stacking subvals
// these never stack
//constexpr int32_t ITEM_NEVER_STACK_MIN = 0;
//constexpr int32_t ITEM_NEVER_STACK_MAX = 63;
// these items always stack with others of same subval, always treated as
// single objects, must be power of 2;
constexpr int32_t ITEM_SINGLE_STACK_MIN = 64;
constexpr int32_t ITEM_SINGLE_STACK_MAX = 192; // see NOTE below
// these items stack with others only if have same subval and same p1,
// they are treated as a group for wielding, etc.
constexpr int32_t ITEM_GROUP_MIN = 192;
//constexpr int32_t ITEM_GROUP_MAX = 255;

// NOTE: items with subval = 192 are treated as single objects,
// but only stack with others of same subval if have the same
// p1 value, only used for torches.

// id's used for object description, stored in objects_identified array
constexpr int32_t OD_TRIED = 0x1;
constexpr int32_t OD_KNOWN1 = 0x2;

// id's used for item description, stored in i_ptr->ident
constexpr int32_t ID_MAGIK = 0x1;
constexpr int32_t ID_DAMD = 0x2;
constexpr int32_t ID_EMPTY = 0x4;
constexpr int32_t ID_KNOWN2 = 0x8;
constexpr int32_t ID_STOREBOUGHT = 0x10;
constexpr int32_t ID_SHOW_HITDAM = 0x20;
constexpr int32_t ID_NOSHOW_P1 = 0x40;
constexpr int32_t ID_SHOW_P1 = 0x80;

// indexes intt the special name table
constexpr int32_t SN_NULL = 0;
constexpr int32_t SN_R = 1;
constexpr int32_t SN_RA = 2;
constexpr int32_t SN_RF = 3;
constexpr int32_t SN_RC = 4;
constexpr int32_t SN_RL = 5;
constexpr int32_t SN_HA = 6;
constexpr int32_t SN_DF = 7;
constexpr int32_t SN_SA = 8;
constexpr int32_t SN_SD = 9;
constexpr int32_t SN_SE = 10;
constexpr int32_t SN_SU = 11;
constexpr int32_t SN_FT = 12;
constexpr int32_t SN_FB = 13;
constexpr int32_t SN_FREE_ACTION = 14;
constexpr int32_t SN_SLAYING = 15;
constexpr int32_t SN_CLUMSINESS = 16;
constexpr int32_t SN_WEAKNESS = 17;
constexpr int32_t SN_SLOW_DESCENT = 18;
constexpr int32_t SN_SPEED = 19;
constexpr int32_t SN_STEALTH = 20;
constexpr int32_t SN_SLOWNESS = 21;
constexpr int32_t SN_NOISE = 22;
constexpr int32_t SN_GREAT_MASS = 23;
constexpr int32_t SN_INTELLIGENCE = 24;
constexpr int32_t SN_WISDOM = 25;
constexpr int32_t SN_INFRAVISION = 26;
constexpr int32_t SN_MIGHT = 27;
constexpr int32_t SN_LORDLINESS = 28;
constexpr int32_t SN_MAGI = 29;
constexpr int32_t SN_BEAUTY = 30;
constexpr int32_t SN_SEEING = 31;
constexpr int32_t SN_REGENERATION = 32;
constexpr int32_t SN_STUPIDITY = 33;
constexpr int32_t SN_DULLNESS = 34;
constexpr int32_t SN_BLINDNESS = 35;
constexpr int32_t SN_TIMIDNESS = 36;
constexpr int32_t SN_TELEPORTATION = 37;
constexpr int32_t SN_UGLINESS = 38;
constexpr int32_t SN_PROTECTION = 39;
constexpr int32_t SN_IRRITATION = 40;
constexpr int32_t SN_VULNERABILITY = 41;
constexpr int32_t SN_ENVELOPING = 42;
constexpr int32_t SN_FIRE = 43;
constexpr int32_t SN_SLAY_EVIL = 44;
constexpr int32_t SN_DRAGON_SLAYING = 45;
constexpr int32_t SN_EMPTY = 46;
constexpr int32_t SN_LOCKED = 47;
constexpr int32_t SN_POISON_NEEDLE = 48;
constexpr int32_t SN_GAS_TRAP = 49;
constexpr int32_t SN_EXPLOSION_DEVICE = 50;
constexpr int32_t SN_SUMMONING_RUNES = 51;
constexpr int32_t SN_MULTIPLE_TRAPS = 52;
constexpr int32_t SN_DISARMED = 53;
constexpr int32_t SN_UNLOCKED = 54;
constexpr int32_t SN_SLAY_ANIMAL = 55;
constexpr int32_t SN_ARRAY_SIZE = 56; // must be at end of this list

// defines for treasure type values (tval)
constexpr int32_t TV_NEVER (-1); // used by find_range() for non-search
constexpr int32_t TV_NOTHING = 0;
constexpr int32_t TV_MISC = 1;
constexpr int32_t TV_CHEST = 2;

// min tval for wearable items, all items between TV_MIN_WEAR and
// TV_MAX_WEAR use the same flag bits, see the TR_* defines.
constexpr int32_t TV_MIN_WEAR = 10;

// items tested for enchantments, i.e. the MAGIK inscription,
// see the enchanted() procedure.
constexpr int32_t TV_MIN_ENCHANT = 10;
constexpr int32_t TV_SLING_AMMO = 10;
constexpr int32_t TV_BOLT = 11;
constexpr int32_t TV_ARROW = 12;
constexpr int32_t TV_SPIKE = 13;
constexpr int32_t TV_LIGHT = 15;
constexpr int32_t TV_BOW = 20;
constexpr int32_t TV_HAFTED = 21;
constexpr int32_t TV_POLEARM = 22;
constexpr int32_t TV_SWORD = 23;
constexpr int32_t TV_DIGGING = 25;
constexpr int32_t TV_BOOTS = 30;
constexpr int32_t TV_GLOVES = 31;
constexpr int32_t TV_CLOAK = 32;
constexpr int32_t TV_HELM = 33;
constexpr int32_t TV_SHIELD = 34;
constexpr int32_t TV_HARD_ARMOR = 35;
constexpr int32_t TV_SOFT_ARMOR = 36;
// max tval that uses the TR_* flags
constexpr int32_t TV_MAX_ENCHANT = 39;
constexpr int32_t TV_AMULET = 40;
constexpr int32_t TV_RING = 45;
// max tval for wearable items
constexpr int32_t TV_MAX_WEAR = 50;
constexpr int32_t TV_STAFF = 55;
constexpr int32_t TV_WAND = 65;
constexpr int32_t TV_SCROLL1 = 70;
constexpr int32_t TV_SCROLL2 = 71;
constexpr int32_t TV_POTION1 = 75;
constexpr int32_t TV_POTION2 = 76;
constexpr int32_t TV_FLASK = 77;
constexpr int32_t TV_FOOD = 80;
constexpr int32_t TV_MAGIC_BOOK = 90;
constexpr int32_t TV_PRAYER_BOOK = 91;
// objects with tval above this are never picked up by monsters
constexpr int32_t TV_MAX_OBJECT = 99;
constexpr int32_t TV_GOLD = 100;
// objects with higher tvals can not be picked up
constexpr int32_t TV_MAX_PICK_UP = 100;
constexpr int32_t TV_INVIS_TRAP = 101;
// objects between TV_MIN_VISIBLE and TV_MAX_VISIBLE are always visible,
// i.e. the cave fm flag is set when they are present
constexpr int32_t TV_MIN_VISIBLE = 102;
constexpr int32_t TV_VIS_TRAP = 102;
constexpr int32_t TV_RUBBLE = 103;
// following objects are never deleted when trying to create another one
// during level generation
constexpr int32_t TV_MIN_DOORS = 104;
constexpr int32_t TV_OPEN_DOOR = 104;
constexpr int32_t TV_CLOSED_DOOR = 105;
constexpr int32_t TV_UP_STAIR = 107;
constexpr int32_t TV_DOWN_STAIR = 108;
constexpr int32_t TV_SECRET_DOOR = 109;
constexpr int32_t TV_STORE_DOOR = 110;
constexpr int32_t TV_MAX_VISIBLE = 110;

// spell types used by get_flags(), breathe(), fire_bolt() and fire_ball()
constexpr int32_t GF_MAGIC_MISSILE = 0;
constexpr int32_t GF_LIGHTNING = 1;
constexpr int32_t GF_POISON_GAS = 2;
constexpr int32_t GF_ACID = 3;
constexpr int32_t GF_FROST = 4;
constexpr int32_t GF_FIRE = 5;
constexpr int32_t GF_HOLY_ORB = 6;

// Number of entries allowed in the score file.
constexpr int32_t SCOREFILE_SIZE = 1000;
