// Copyright (c)  = 1989-94 James E. Wilson, Robert A. Koeneke
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Global constants used by Moria
// clang-format off

#include "headers.h"

constexpr unsigned char MAX_UCHAR = std::numeric_limits<unsigned char>::max();
constexpr short MAX_SHORT = std::numeric_limits<short>::max(); // maximum short/long signed ints
constexpr int MAX_LONG = std::numeric_limits<int>::max();

// Note to the Wizard:
//
// Tweaking these constants can *GREATLY* change the game.
// Two years of constant tuning have generated these
// values.  Minor adjustments are encouraged, but you must
// be very careful not to unbalance the game.  Moria was
// meant to be challenging, not a give away.  Many
// adjustments can cause the game to act strangely, or even
// cause errors. -RAK-

// Addendum:
// I have greatly expanded the number of defined constants.  However, if
// you change anything below, without understanding EXACTLY how the game
// uses the number, the program may stop working correctly.  Modify the
// constants at your own risk. -JEW-

// message line location
constexpr unsigned int MSG_LINE = 0;

// number of messages to save in a buffer
constexpr int MAX_SAVE_MSG = 22; // How many messages to save -CJS-

// Dungeon size parameters
constexpr int  MAX_HEIGHT = 66; // Multiple of  = 11; >=  = 22
constexpr int MAX_WIDTH = 198; // Multiple of  = 33; >=  = 66
constexpr int  SCREEN_HEIGHT = 22;
constexpr int  SCREEN_WIDTH = 66;
constexpr int  QUART_HEIGHT = (SCREEN_HEIGHT / 4);
constexpr int  QUART_WIDTH = (SCREEN_WIDTH / 4);

// Dungeon generation values
// Note: The entire design of dungeon can be changed by only slight adjustments here.
constexpr int  DUN_TUN_RND = 9; //  = 1/Chance of Random direction
constexpr int  DUN_TUN_CHG = 70; // Chance of changing direction (99 max)
constexpr int  DUN_TUN_CON = 15; // Chance of extra tunneling
constexpr int  DUN_ROO_MEA = 32; // Mean of # of rooms, standard dev2
constexpr int  DUN_TUN_PEN = 25; // % chance of room doors
constexpr int  DUN_TUN_JCT = 15; // % chance of doors at tunnel junctions
constexpr int  DUN_STR_DEN = 5; // Density of streamers
constexpr int  DUN_STR_RNG = 2; // Width of streamers
constexpr int  DUN_STR_MAG = 3; // Number of magma streamers
constexpr int  DUN_STR_MC = 90; //  = 1/x chance of treasure per magma
constexpr int  DUN_STR_QUA = 2; // Number of quartz streamers
constexpr int  DUN_STR_QC = 40; //  = 1/x chance of treasure per quartz
constexpr int  DUN_UNUSUAL  = 300; // Level/x chance of unusual room

// Store constants
constexpr int  MAX_OWNERS = 18; // Number of owners to choose from
constexpr int  MAX_STORES  = 6; // Number of different stores
constexpr int  STORE_INVEN_MAX  = 24; // Max number of discrete objects in inventory
constexpr int  STORE_CHOICES = 26; // NUMBER of items to choose stock from
constexpr int  STORE_MAX_INVEN  = 18; // Max diff objects in stock for auto buy
constexpr int  STORE_MIN_INVEN  = 10; // Min diff objects in stock for auto sell
constexpr int  STORE_TURN_AROUND = 9; // Amount of buying and selling normally
constexpr int  COST_ADJ  = 100; // Adjust prices for buying and selling

// Treasure constants
constexpr int  INVEN_ARRAY_SIZE = 34; // Size of inventory array(Do not change)
constexpr int  MAX_OBJ_LEVEL = 50; // Maximum level of magic in dungeon
constexpr int  OBJ_GREAT = 12; //  = 1/n Chance of item being a Great Item

// Note that the following constants are all related, if you change one, you
// must also change all succeeding ones. Also, player_base_provisions[] and
// store_choices[] may also have to be changed.
constexpr int  MAX_OBJECTS = 420; // Number of objects for universe
constexpr int  MAX_DUNGEON_OBJ = 344; // Number of dungeon objects
constexpr int  OBJ_OPEN_DOOR = 367;
constexpr int  OBJ_CLOSED_DOOR = 368;
constexpr int  OBJ_SECRET_DOOR = 369;
constexpr int  OBJ_UP_STAIR = 370;
constexpr int  OBJ_DOWN_STAIR  = 371;
constexpr int  OBJ_STORE_DOOR  = 372;
constexpr int  OBJ_TRAP_LIST = 378;
constexpr int  OBJ_RUBBLE = 396;
constexpr int  OBJ_MUSH  = 397;
constexpr int  OBJ_SCARE_MON = 398;
constexpr int  OBJ_GOLD_LIST = 399;
constexpr int  OBJ_NOTHING  = 417;
constexpr int  OBJ_RUINED_CHEST = 418;
constexpr int  OBJ_WIZARD = 419;
constexpr int  OBJECT_IDENT_SIZE  = 448; //  = 7*64, see object_offset() in desc.c, could be MAX_OBJECTS o_o() rewritten
constexpr int  MAX_GOLD = 18; // Number of different types of gold

// with MAX_TALLOC  = 150, it is possible to get compacting objects during
// level generation, although it is extremely rare
constexpr int  MAX_TALLOC = 175; // Max objects per level
constexpr int  MIN_TRIX = 1; // Minimum treasure_list index used
constexpr int  TREAS_ROOM_ALLOC  = 7; // Amount of objects for rooms
constexpr int  TREAS_ANY_ALLOC = 2; // Amount of objects for corridors
constexpr int  TREAS_GOLD_ALLOC  = 2; // Amount of gold (and gems)

// Magic Treasure Generation constants
// Note: Number of special objects, and degree of enchantments can be adjusted here.
constexpr int  OBJ_STD_ADJ  = 125; // Adjust STD per level *  = 100
constexpr int  OBJ_STD_MIN = 7; // Minimum STD
constexpr int  OBJ_TOWN_LEVEL = 7; // Town object generation level
constexpr int  OBJ_BASE_MAGIC = 15; // Base amount of magic
constexpr int  OBJ_BASE_MAX  = 70; // Max amount of magic
constexpr int  OBJ_DIV_SPECIAL = 6; // magic_chance/# special magic
constexpr int  OBJ_DIV_CURSED = 13; //  = 10*magic_chance/#  cursed items

// Constants describing limits of certain objects
constexpr int  OBJ_LAMP_MAX  = 15000; // Maximum amount that lamp can be filled
constexpr int  OBJ_BOLT_RANGE = 18; // Maximum range of bolts and balls
constexpr int  OBJ_RUNE_PROT  = 3000; // Rune of protection resistance

// Creature constants
constexpr int  MAX_CREATURES = 279; // Number of creatures defined for univ
constexpr int  N_MONS_ATTS  = 215; // Number of monster attack types.

// with MAX_MALLOC  = 101, it is possible to get compacting monsters messages
// while breeding/cloning monsters
constexpr int  MAX_MALLOC = 125; // Max that can be allocated
constexpr int  MAX_MALLOC_CHANCE  = 160; //  = 1/x chance of new monster each round
constexpr int  MAX_MONS_LEVEL = 40; // Maximum level of creatures
constexpr int  MAX_SIGHT  = 20; // Maximum dis a creature can be seen
constexpr int  MAX_SPELL_DIS = 20; // Maximum dis creature spell can be cast
constexpr int  MAX_MON_MULT  = 75; // Maximum reproductions on a level
constexpr int  MON_MULT_ADJ = 7; // High value slows multiplication
constexpr int  MON_NASTY  = 50; //  = 1/x chance of high level creature
constexpr int  MIN_MALLOC_LEVEL = 14; // Minimum number of monsters/level
constexpr int  MIN_MALLOC_TD  = 4; // Number of people on town level (day)
constexpr int  MIN_MALLOC_TN  = 8; // Number of people on town level (night)
constexpr int  WIN_MON_TOT = 2; // Total number of "win" creatures
constexpr int  WIN_MON_APPEAR = 50; // Level where winning creatures begin
constexpr int  MON_SUMMON_ADJ = 2; // Adjust level of summoned creatures
constexpr int  MON_DRAIN_LIFE = 2; // Percent of player exp drained per hit
constexpr int  MAX_MON_NATTACK = 4; // Max num attacks (used in mons memory) -CJS-
constexpr int  MIN_MONIX = 2; // Minimum index in m_list (1 = py, 0 = no mon)

// Trap constants
constexpr int  MAX_TRAP = 18; // Number of defined traps

// FIXME: this const is not used anywhere!
//constexpr int  SCARE_MONSTER = 99

// Descriptive constants
constexpr int  MAX_COLORS = 49; // Used with potions
constexpr int  MAX_MUSH = 22; // Used with mushrooms
constexpr int  MAX_WOODS  = 25; // Used with staffs
constexpr int  MAX_METALS = 25; // Used with wands
constexpr int  MAX_ROCKS  = 32; // Used with rings
constexpr int  MAX_AMULETS = 11; // Used with amulets
constexpr int  MAX_TITLES = 45; // Used with scrolls
constexpr int  MAX_SYLLABLES = 153; // Used with scrolls

// Player constants
constexpr int  MAX_PLAYER_LEVEL = 40; // Maximum possible character level
constexpr long  MAX_EXP  = 9999999L;  // Maximum amount of experience -CJS-
constexpr int  MAX_RACES = 8; // Number of defined races
constexpr int  MAX_CLASS = 6; // Number of defined classes
constexpr int  USE_DEVICE  = 3; // x> Harder devices x< Easier devices
constexpr int  MAX_BACKGROUND  = 128; // Number of types of histories for univ
constexpr int  PLAYER_FOOD_FULL = 10000; // Getting full
constexpr int  PLAYER_FOOD_MAX  = 15000; // Maximum food value, beyond is wasted
constexpr int  PLAYER_FOOD_FAINT  = 300; // Character begins fainting
constexpr int  PLAYER_FOOD_WEAK  = 1000; // Warn player that he is getting very low
constexpr int  PLAYER_FOOD_ALERT = 2000; // Warn player that he is getting low
constexpr int  PLAYER_REGEN_FAINT  = 33; // Regen factor*2^16 when fainting
constexpr int  PLAYER_REGEN_WEAK = 98; // Regen factor*2^16 when weak
constexpr int  PLAYER_REGEN_NORMAL = 197; // Regen factor*2^16 when full
constexpr int  PLAYER_REGEN_HPBASE  = 1442; // Min amount hp regen*2^16
constexpr int  PLAYER_REGEN_MNBASE = 524; // Min amount mana regen*2^16
constexpr int  PLAYER_WEIGHT_CAP  = 130; // "#"*(1/10 pounds) per strength point
constexpr int  PLAYER_EXIT_PAUSE = 2; // Pause time before player can re-roll

// class level adjustment constants
constexpr int  CLA_BTH = 0;
constexpr int  CLA_BTHB = 1;
constexpr int  CLA_DEVICE  = 2;
constexpr int  CLA_DISARM  = 3;
constexpr int  CLA_SAVE = 4;
// this depends on the fact that CLA_SAVE values are all the same, if not,
// then should add a separate column for this
constexpr int  CLA_MISC_HIT = 4;
constexpr int  MAX_LEV_ADJ = 5;

// Base to hit constants
constexpr int  BTH_PLUS_ADJ = 3; // Adjust BTH per plus-to-hit

// magic numbers for players inventory array
constexpr int  INVEN_WIELD = 22; // must be first item in equipment list
constexpr int  INVEN_HEAD = 23;
constexpr int  INVEN_NECK = 24;
constexpr int  INVEN_BODY = 25;
constexpr int  INVEN_ARM  = 26;
constexpr int  INVEN_HANDS = 27;
constexpr int  INVEN_RIGHT = 28;
constexpr int  INVEN_LEFT = 29;
constexpr int  INVEN_FEET = 30;
constexpr int  INVEN_OUTER = 31;
constexpr int  INVEN_LIGHT = 32;
constexpr int  INVEN_AUX  = 33;

// Attribute indexes -CJS-
constexpr int  A_STR = 0;
constexpr int  A_INT  = 1;
constexpr int  A_WIS  = 2;
constexpr int  A_DEX  = 3;
constexpr int  A_CON  = 4;
constexpr int  A_CHR  = 5;

constexpr char CTRL_KEY (char x) {
	return ((x) & 0x1F);
}

#undef DELETE
constexpr char  DELETE = 0x7f;

#undef ESCAPE
constexpr char ESCAPE = '\033';  // ESCAPE character -CJS-

// This used to be NULL, but that was technically incorrect.
// CNIL is used instead of null to help avoid lint errors.
#ifndef CNIL
constexpr char* CNIL = 0;
#endif

// Fval definitions: these describe the various types of dungeon floors and
// walls, if numbers above  = 15 are ever used, then the test against MIN_CAVE_WALL
// will have to be changed, also the save routines will have to be changed.
constexpr int  NULL_WALL = 0;
constexpr int  DARK_FLOOR = 1;
constexpr int  LIGHT_FLOOR  = 2;
constexpr int  MAX_CAVE_ROOM = 2;
constexpr int  CORR_FLOOR = 3;
constexpr int  BLOCKED_FLOOR = 4; // a corridor space with cl/st/se door or rubble
constexpr int  MAX_CAVE_FLOOR  = 4;

constexpr int  MAX_OPEN_SPACE  = 3;
constexpr int  MIN_CLOSED_SPACE = 4;

constexpr int  TMP1_WALL = 8;
constexpr int  TMP2_WALL = 9;

constexpr int  MIN_CAVE_WALL = 12;
constexpr int  GRANITE_WALL = 12;
constexpr int  MAGMA_WALL  = 13;
constexpr int  QUARTZ_WALL = 14;
constexpr int  BOUNDARY_WALL  = 15;

// Column for stats
constexpr int  STAT_COLUMN = 0;

// Class spell types
constexpr int  NONE = 0;
constexpr int  MAGE = 1;
constexpr int  PRIEST = 2;

// offsets to spell names in spell_names[] array
constexpr int  SPELL_OFFSET = 0;
constexpr int  PRAYER_OFFSET = 31;

// definitions for the pseudo-normal distribution generation
constexpr int  NORMAL_TABLE_SIZE = 256;
constexpr int  NORMAL_TABLE_SD = 64;  // the standard deviation for the table

// definitions for the player's status field
constexpr long PY_HUNGRY = 0x00000001L;
constexpr long PY_WEAK  = 0x00000002L;
constexpr long PY_BLIND = 0x00000004L;
constexpr long PY_CONFUSED = 0x00000008L;
constexpr long PY_FEAR  = 0x00000010L;
constexpr long PY_POISONED = 0x00000020L;
constexpr long PY_FAST  = 0x00000040L;
constexpr long PY_SLOW  = 0x00000080L;
constexpr long PY_SEARCH = 0x00000100L;
constexpr long PY_REST  = 0x00000200L;
constexpr long PY_STUDY = 0x00000400L;

constexpr long PY_INVULN = 0x00001000L;
constexpr long PY_HERO  = 0x00002000L;
constexpr long PY_SHERO = 0x00004000L;
constexpr long PY_BLESSED  = 0x00008000L;
constexpr long PY_DET_INV  = 0x00010000L;
constexpr long PY_TIM_INFRA = 0x00020000L;
constexpr long PY_SPEED = 0x00040000L;
constexpr long PY_STR_WGT  = 0x00080000L;
constexpr long PY_PARALYSED = 0x00100000L;
constexpr long PY_REPEAT = 0x00200000L;
constexpr long PY_ARMOR = 0x00400000L;

constexpr long PY_STATS = 0x3F000000L;
constexpr long PY_STR = 0x01000000L; // these  = 6 stat flags must be adjacent

// FIXME: these const are not used anywhere!
//constexpr long PY_INT = 0x02000000L;
//constexpr long PY_WIS = 0x04000000L;
//constexpr long PY_DEX = 0x08000000L;
//constexpr long PY_CON = 0x10000000L;
//constexpr long PY_CHR = 0x20000000L;

constexpr long PY_HP = 0x40000000L;
constexpr long PY_MANA  = 0x80000000L;

// definitions for objects that can be worn
constexpr long TR_STATS = 0x0000003FL;  // the stats must be the low  = 6 bits
constexpr long TR_STR = 0x00000001L;
constexpr long TR_INT = 0x00000002L;
constexpr long TR_WIS = 0x00000004L;
constexpr long TR_DEX = 0x00000008L;
constexpr long TR_CON = 0x00000010L;
constexpr long TR_CHR = 0x00000020L;
constexpr long TR_SEARCH = 0x00000040L;
constexpr long TR_SLOW_DIGEST = 0x00000080L;
constexpr long TR_STEALTH  = 0x00000100L;
constexpr long TR_AGGRAVATE = 0x00000200L;
constexpr long TR_TELEPORT = 0x00000400L;
constexpr long TR_REGEN = 0x00000800L;
constexpr long TR_SPEED = 0x00001000L;

constexpr long TR_EGO_WEAPON  = 0x0007E000L;
constexpr long TR_SLAY_DRAGON = 0x00002000L;
constexpr long TR_SLAY_ANIMAL = 0x00004000L;
constexpr long TR_SLAY_EVIL = 0x00008000L;
constexpr long TR_SLAY_UNDEAD = 0x00010000L;
constexpr long TR_FROST_BRAND = 0x00020000L;
constexpr long TR_FLAME_TONGUE= 0x00040000L;

constexpr long TR_RES_FIRE = 0x00080000L;
constexpr long TR_RES_ACID = 0x00100000L;
constexpr long TR_RES_COLD = 0x00200000L;
constexpr long TR_SUST_STAT = 0x00400000L;
constexpr long TR_FREE_ACT = 0x00800000L;
constexpr long TR_SEE_INVIS = 0x01000000L;
constexpr long TR_RES_LIGHT = 0x02000000L;
constexpr long TR_FFALL = 0x04000000L;
constexpr long TR_BLIND = 0x08000000L;
constexpr long TR_TIMID = 0x10000000L;
constexpr long TR_TUNNEL = 0x20000000L;
constexpr long TR_INFRA = 0x40000000L;
constexpr long TR_CURSED = 0x80000000L;

// definitions for chests
constexpr long CH_LOCKED = 0x00000001L;
constexpr long CH_TRAPPED  = 0x000001F0L;
constexpr long CH_LOSE_STR = 0x00000010L;
constexpr long CH_POISON = 0x00000020L;
constexpr long CH_PARALYSED = 0x00000040L;
constexpr long CH_EXPLODE  = 0x00000080L;
constexpr long CH_SUMMON = 0x00000100L;

// definitions for creatures, cmove field
constexpr long CM_ALL_MV_FLAGS= 0x0000003FL;
constexpr long CM_ATTACK_ONLY = 0x00000001L;
constexpr long CM_MOVE_NORMAL = 0x00000002L;
// For Quylthulgs, which have no physical movement.
constexpr long CM_ONLY_MAGIC  = 0x00000004L;

constexpr long CM_RANDOM_MOVE = 0x00000038L;
constexpr long CM_20_RANDOM = 0x00000008L;
constexpr long CM_40_RANDOM = 0x00000010L;
constexpr long CM_75_RANDOM = 0x00000020L;

constexpr long CM_SPECIAL  = 0x003F0000L;
constexpr long CM_INVISIBLE = 0x00010000L;
constexpr long CM_OPEN_DOOR = 0x00020000L;
constexpr long CM_PHASE = 0x00040000L;
constexpr long CM_EATS_OTHER  = 0x00080000L;
constexpr long CM_PICKS_UP = 0x00100000L;
constexpr long CM_MULTIPLY = 0x00200000L;

constexpr long CM_SMALL_OBJ = 0x00800000L;
constexpr long CM_CARRY_OBJ = 0x01000000L;
constexpr long CM_CARRY_GOLD  = 0x02000000L;
constexpr long CM_TREASURE = 0x7C000000L;
constexpr long CM_TR_SHIFT  = 26;  // used for recall of treasure
constexpr long CM_60_RANDOM = 0x04000000L;
constexpr long CM_90_RANDOM = 0x08000000L;
constexpr long CM_1D2_OBJ  = 0x10000000L;
constexpr long CM_2D2_OBJ  = 0x20000000L;
constexpr long CM_4D2_OBJ  = 0x40000000L;
constexpr long CM_WIN = 0x80000000L;

// creature spell definitions
constexpr long CS_FREQ  = 0x0000000FL;
constexpr long CS_SPELLS = 0x0001FFF0L;
constexpr long CS_TEL_SHORT = 0x00000010L;

// FIXME: these const are not used anywhere!
//constexpr long CS_TEL_LONG = 0x00000020L;
//constexpr long CS_TEL_TO = 0x00000040L;
//constexpr long CS_LGHT_WND = 0x00000080L;
//constexpr long CS_SER_WND  = 0x00000100L;
//constexpr long CS_HOLD_PER = 0x00000200L;
//constexpr long CS_BLIND = 0x00000400L;
//constexpr long CS_CONFUSE  = 0x00000800L;
//constexpr long CS_FEAR  = 0x00001000L;
//constexpr long CS_SUMMON_MON  = 0x00002000L;
//constexpr long CS_SUMMON_UND  = 0x00004000L;
//constexpr long CS_SLOW_PER = 0x00008000L;
//constexpr long CS_DRAIN_MANA  = 0x00010000L;

constexpr long CS_BREATHE  = 0x00F80000L; // may also just indicate resistance
constexpr long CS_BR_LIGHT = 0x00080000L; // if no spell frequency set
constexpr long CS_BR_GAS = 0x00100000L;
constexpr long CS_BR_ACID  = 0x00200000L;
constexpr long CS_BR_FROST = 0x00400000L;
constexpr long CS_BR_FIRE  = 0x00800000L;

// creature defense flags
constexpr int  CD_DRAGON = 0x0001;
constexpr int CD_ANIMAL = 0x0002;
constexpr int CD_EVIL  = 0x0004;
constexpr int CD_UNDEAD = 0x0008;
constexpr int CD_WEAKNESS = 0x03F0;
constexpr int CD_FROST = 0x0010;
constexpr int CD_FIRE  = 0x0020;
constexpr int CD_POISON = 0x0040;
constexpr int CD_ACID  = 0x0080;
constexpr int CD_LIGHT = 0x0100;
constexpr int CD_STONE = 0x0200;

constexpr int CD_NO_SLEEP = 0x1000;
constexpr int CD_INFRA = 0x2000;
constexpr int CD_MAX_HP = 0x4000;

// FIXME: some of these const are not used anywhere!
// inventory stacking subvals
// these never stack
//constexpr int ITEM_NEVER_STACK_MIN 0;
//constexpr int ITEM_NEVER_STACK_MAX = 63;
// these items always stack with others of same subval, always treated as
// single objects, must be power of  = 2;
constexpr int ITEM_SINGLE_STACK_MIN = 64;
constexpr int ITEM_SINGLE_STACK_MAX = 192;  // see NOTE below
// these items stack with others only if have same subval and same p1,
// they are treated as a group for wielding, etc.
constexpr int ITEM_GROUP_MIN = 192;
//constexpr int ITEM_GROUP_MAX = 255;

// NOTE: items with subval  = 192 are treated as single objects,
// but only stack with others of same subval if have the same
// p1 value, only used for torches.

// id's used for object description, stored in objects_identified array
constexpr int OD_TRIED = 0x1;
constexpr int OD_KNOWN1 = 0x2;

// id's used for item description, stored in i_ptr->ident
constexpr int ID_MAGIK = 0x1;
constexpr int ID_DAMD  = 0x2;
constexpr int ID_EMPTY = 0x4;
constexpr int ID_KNOWN2 = 0x8;
constexpr int ID_STOREBOUGHT = 0x10;
constexpr int ID_SHOW_HITDAM = 0x20;
constexpr int ID_NOSHOW_P1 = 0x40;
constexpr int ID_SHOW_P1  = 0x80;

// indexes into the special name table
constexpr int SN_NULL = 0;
constexpr int SN_R  = 1;
constexpr int SN_RA = 2;
constexpr int SN_RF = 3;
constexpr int SN_RC = 4;
constexpr int SN_RL = 5;
constexpr int SN_HA = 6;
constexpr int SN_DF = 7;
constexpr int SN_SA = 8;
constexpr int SN_SD = 9;
constexpr int SN_SE = 10;
constexpr int SN_SU = 11;
constexpr int SN_FT = 12;
constexpr int SN_FB = 13;
constexpr int SN_FREE_ACTION = 14;
constexpr int SN_SLAYING = 15;
constexpr int SN_CLUMSINESS = 16;
constexpr int SN_WEAKNESS = 17;
constexpr int SN_SLOW_DESCENT = 18;
constexpr int SN_SPEED = 19;
constexpr int SN_STEALTH = 20;
constexpr int SN_SLOWNESS = 21;
constexpr int SN_NOISE = 22;
constexpr int SN_GREAT_MASS = 23;
constexpr int SN_INTELLIGENCE = 24;
constexpr int SN_WISDOM  = 25;
constexpr int SN_INFRAVISION = 26;
constexpr int SN_MIGHT = 27;
constexpr int SN_LORDLINESS = 28;
constexpr int SN_MAGI = 29;
constexpr int SN_BEAUTY  = 30;
constexpr int SN_SEEING  = 31;
constexpr int SN_REGENERATION = 32;
constexpr int SN_STUPIDITY  = 33;
constexpr int SN_DULLNESS = 34;
constexpr int SN_BLINDNESS  = 35;
constexpr int SN_TIMIDNESS  = 36;
constexpr int SN_TELEPORTATION = 37;
constexpr int SN_UGLINESS = 38;
constexpr int SN_PROTECTION = 39;
constexpr int SN_IRRITATION = 40;
constexpr int SN_VULNERABILITY = 41;
constexpr int SN_ENVELOPING = 42;
constexpr int SN_FIRE = 43;
constexpr int SN_SLAY_EVIL = 44;
constexpr int SN_DRAGON_SLAYING = 45;
constexpr int SN_EMPTY = 46;
constexpr int SN_LOCKED  = 47;
constexpr int SN_POISON_NEEDLE = 48;
constexpr int SN_GAS_TRAP = 49;
constexpr int SN_EXPLOSION_DEVICE = 50;
constexpr int SN_SUMMONING_RUNES  = 51;
constexpr int SN_MULTIPLE_TRAPS = 52;
constexpr int SN_DISARMED = 53;
constexpr int SN_UNLOCKED = 54;
constexpr int SN_SLAY_ANIMAL = 55;
constexpr int SN_ARRAY_SIZE = 56; // must be at end of this list

// defines for treasure type values (tval)
constexpr int TV_NEVER  (-1); // used by find_range() for non-search
constexpr int TV_NOTHING = 0;
constexpr int TV_MISC  = 1;
constexpr int TV_CHEST = 2;

// min tval for wearable items, all items between TV_MIN_WEAR and
// TV_MAX_WEAR use the same flag bits, see the TR_* defines.
constexpr int TV_MIN_WEAR = 10;

// items tested for enchantments, i.e. the MAGIK inscription,
// see the enchanted() procedure.
constexpr int TV_MIN_ENCHANT = 10;
constexpr int TV_SLING_AMMO = 10;
constexpr int TV_BOLT = 11;
constexpr int TV_ARROW = 12;
constexpr int TV_SPIKE = 13;
constexpr int TV_LIGHT = 15;
constexpr int TV_BOW  = 20;
constexpr int TV_HAFTED  = 21;
constexpr int TV_POLEARM = 22;
constexpr int TV_SWORD = 23;
constexpr int TV_DIGGING = 25;
constexpr int TV_BOOTS = 30;
constexpr int TV_GLOVES  = 31;
constexpr int TV_CLOAK = 32;
constexpr int TV_HELM = 33;
constexpr int TV_SHIELD  = 34;
constexpr int TV_HARD_ARMOR = 35;
constexpr int TV_SOFT_ARMOR = 36;
// max tval that uses the TR_* flags
constexpr int TV_MAX_ENCHANT = 39;
constexpr int TV_AMULET  = 40;
constexpr int TV_RING = 45;
// max tval for wearable items
constexpr int TV_MAX_WEAR = 50;
constexpr int TV_STAFF = 55;
constexpr int TV_WAND = 65;
constexpr int TV_SCROLL1 = 70;
constexpr int TV_SCROLL2 = 71;
constexpr int TV_POTION1 = 75;
constexpr int TV_POTION2 = 76;
constexpr int TV_FLASK = 77;
constexpr int TV_FOOD = 80;
constexpr int TV_MAGIC_BOOK = 90;
constexpr int TV_PRAYER_BOOK = 91;
// objects with tval above this are never picked up by monsters
constexpr int TV_MAX_OBJECT = 99;
constexpr int TV_GOLD = 100;
// objects with higher tvals can not be picked up
constexpr int TV_MAX_PICK_UP  = 100;
constexpr int TV_INVIS_TRAP = 101;
// objects between TV_MIN_VISIBLE and TV_MAX_VISIBLE are always visible,
// i.e. the cave fm flag is set when they are present
constexpr int TV_MIN_VISIBLE  = 102;
constexpr int TV_VIS_TRAP  = 102;
constexpr int TV_RUBBLE = 103;
// following objects are never deleted when trying to create another one
// during level generation
constexpr int TV_MIN_DOORS = 104;
constexpr int TV_OPEN_DOOR = 104;
constexpr int TV_CLOSED_DOOR  = 105;
constexpr int TV_UP_STAIR  = 107;
constexpr int TV_DOWN_STAIR = 108;
constexpr int TV_SECRET_DOOR  = 109;
constexpr int TV_STORE_DOOR = 110;
constexpr int TV_MAX_VISIBLE  = 110;

// spell types used by get_flags(), breathe(), fire_bolt() and fire_ball()
constexpr int GF_MAGIC_MISSILE = 0;
constexpr int GF_LIGHTNING = 1;
constexpr int GF_POISON_GAS  = 2;
constexpr int GF_ACID  = 3;
constexpr int GF_FROST = 4;
constexpr int GF_FIRE  = 5;
constexpr int GF_HOLY_ORB = 6;

// Number of entries allowed in the score file.
constexpr int SCOREFILE_SIZE = 1000;
