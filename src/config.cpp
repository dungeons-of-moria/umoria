// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// clang-format off
#include "headers.h"

namespace config {
    // Data files used by Umoria
    // NOTE: use relative paths to the executable binary.
    namespace files {
        const std::string splash_screen = "data/splash.txt";
        const std::string welcome_screen = "data/welcome.txt";
        const std::string license = "LICENSE";
        const std::string versions_history = "data/versions.txt";
        const std::string help = "data/help.txt";
        const std::string help_wizard = "data/help_wizard.txt";
        const std::string help_roguelike = "data/rl_help.txt";
        const std::string help_roguelike_wizard = "data/rl_help_wizard.txt";
        const std::string death_tomb = "data/death_tomb.txt";
        const std::string death_royal = "data/death_royal.txt";
        const std::string scores = "scores.dat";
        std::string save_game = "game.sav";
    } // namespace files

    // Game options as set on startup and with `=` set options command -CJS-
    namespace options {
        bool display_counts = true;          // Display rest/repeat counts
        bool find_bound = false;             // Print yourself on a run (slower)
        bool run_cut_corners = true;         // Cut corners while running
        bool run_examine_corners = true;     // Check corners while running
        bool run_ignore_doors = false;       // Run through open doors
        bool run_print_self = false;         // Stop running when the map shifts
        bool highlight_seams = false;        // Highlight magma and quartz veins
        bool prompt_to_pickup = false;       // Prompt to pick something up
        bool use_roguelike_keys = false;     // Use classic Roguelike keys
        bool show_inventory_weights = false; // Display weights in inventory
        bool error_beep_sound = true;        // Beep for invalid characters
    } // namespace options

    // Dungeon generation values
    // Note: The entire design of dungeon can be changed by only slight adjustments here.
    namespace dungeon {
        const uint8_t DUN_RANDOM_DIR = 9;       // 1/Chance of Random direction
        const uint8_t DUN_DIR_CHANGE = 70;      // Chance of changing direction (99 max)
        const uint8_t DUN_TUNNELING = 15;       // Chance of extra tunneling
        const uint8_t DUN_ROOMS_MEAN = 32;      // Mean of # of rooms, standard dev2
        const uint8_t DUN_ROOM_DOORS = 25;      // % chance of room doors
        const uint8_t DUN_TUNNEL_DOORS = 15;    // % chance of doors at tunnel junctions
        const uint8_t DUN_STREAMER_DENSITY = 5; // Density of streamers
        const uint8_t DUN_STREAMER_WIDTH = 2;   // Width of streamers
        const uint8_t DUN_MAGMA_STREAMER = 3;   // Number of magma streamers
        const uint8_t DUN_MAGMA_TREASURE = 90;  // 1/x chance of treasure per magma
        const uint8_t DUN_QUARTZ_STREAMER = 2;  // Number of quartz streamers
        const uint8_t DUN_QUARTZ_TREASURE = 40; // 1/x chance of treasure per quartz
        const uint16_t DUN_UNUSUAL_ROOMS = 300; // Level/x chance of unusual room

        namespace objects {
            const uint16_t OBJ_OPEN_DOOR = 367;
            const uint16_t OBJ_CLOSED_DOOR = 368;
            const uint16_t OBJ_SECRET_DOOR = 369;
            const uint16_t OBJ_UP_STAIR = 370;
            const uint16_t OBJ_DOWN_STAIR = 371;
            const uint16_t OBJ_STORE_DOOR = 372;
            const uint16_t OBJ_TRAP_LIST = 378;
            const uint16_t OBJ_RUBBLE = 396;
            const uint16_t OBJ_MUSH = 397;
            const uint16_t OBJ_SCARE_MON = 398;
            const uint16_t OBJ_GOLD_LIST = 399;
            const uint16_t OBJ_NOTHING = 417;
            const uint16_t OBJ_RUINED_CHEST = 418;
            const uint16_t OBJ_WIZARD = 419;

            const uint8_t MAX_GOLD_TYPES = 18; // Number of different types of gold
            const uint8_t MAX_TRAPS = 18;      // Number of defined traps

            const uint8_t LEVEL_OBJECTS_PER_ROOM = 7;     // Amount of objects for rooms
            const uint8_t LEVEL_OBJECTS_PER_CORRIDOR = 2; // Amount of objects for corridors
            const uint8_t LEVEL_TOTAL_GOLD_AND_GEMS = 2;  // Amount of gold (and gems)
        } // namespace objects
    } // namespace dungeon

    // Note: Number of special objects, and degree of enchantments can be adjusted here.
    namespace treasure {
        const uint8_t MIN_TREASURE_LIST_ID = 1;           // Minimum treasure_list index used
        const uint8_t TREASURE_CHANCE_OF_GREAT_ITEM = 12; // 1/n Chance of item being a Great Item

        // Magic Treasure Generation constants
        const uint8_t LEVEL_STD_OBJECT_ADJUST = 125; // Adjust STD per level * 100
        const uint8_t LEVEL_MIN_OBJECT_STD = 7;      // Minimum STD
        const uint8_t LEVEL_TOWN_OBJECTS = 7;        // Town object generation level
        const uint8_t OBJECT_BASE_MAGIC = 15;        // Base amount of magic
        const uint8_t OBJECT_MAX_BASE_MAGIC = 70;    // Max amount of magic
        const uint8_t OBJECT_CHANCE_SPECIAL = 6;     // magic_chance/# special magic
        const uint8_t OBJECT_CHANCE_CURSED = 13;     // 10*magic_chance/# cursed items

        // Constants describing limits of certain objects
        const uint16_t OBJECT_LAMP_MAX_CAPACITY = 15000; // Maximum amount that lamp can be filled
        const uint8_t OBJECT_BOLTS_MAX_RANGE = 18;       // Maximum range of bolts and balls
        const uint16_t OBJECTS_RUNE_PROTECTION = 3000;   // Rune of protection resistance

        // definitions for objects that can be worn
        namespace flags {
            const uint32_t TR_STATS = 0x0000003FL; // the stats must be the low 6 bits
            const uint32_t TR_STR = 0x00000001L;
            const uint32_t TR_INT = 0x00000002L;
            const uint32_t TR_WIS = 0x00000004L;
            const uint32_t TR_DEX = 0x00000008L;
            const uint32_t TR_CON = 0x00000010L;
            const uint32_t TR_CHR = 0x00000020L;
            const uint32_t TR_SEARCH = 0x00000040L;
            const uint32_t TR_SLOW_DIGEST = 0x00000080L;
            const uint32_t TR_STEALTH = 0x00000100L;
            const uint32_t TR_AGGRAVATE = 0x00000200L;
            const uint32_t TR_TELEPORT = 0x00000400L;
            const uint32_t TR_REGEN = 0x00000800L;
            const uint32_t TR_SPEED = 0x00001000L;

            const uint32_t TR_EGO_WEAPON = 0x0007E000L;
            const uint32_t TR_SLAY_DRAGON = 0x00002000L;
            const uint32_t TR_SLAY_ANIMAL = 0x00004000L;
            const uint32_t TR_SLAY_EVIL = 0x00008000L;
            const uint32_t TR_SLAY_UNDEAD = 0x00010000L;
            const uint32_t TR_FROST_BRAND = 0x00020000L;
            const uint32_t TR_FLAME_TONGUE = 0x00040000L;

            const uint32_t TR_RES_FIRE = 0x00080000L;
            const uint32_t TR_RES_ACID = 0x00100000L;
            const uint32_t TR_RES_COLD = 0x00200000L;
            const uint32_t TR_SUST_STAT = 0x00400000L;
            const uint32_t TR_FREE_ACT = 0x00800000L;
            const uint32_t TR_SEE_INVIS = 0x01000000L;
            const uint32_t TR_RES_LIGHT = 0x02000000L;
            const uint32_t TR_FFALL = 0x04000000L;
            const uint32_t TR_BLIND = 0x08000000L;
            const uint32_t TR_TIMID = 0x10000000L;
            const uint32_t TR_TUNNEL = 0x20000000L;
            const uint32_t TR_INFRA = 0x40000000L;
            const uint32_t TR_CURSED = 0x80000000L;
        } // namespace flags

        // definitions for chests
        namespace chests {
            const uint32_t CH_LOCKED = 0x00000001L;
            const uint32_t CH_TRAPPED = 0x000001F0L;
            const uint32_t CH_LOSE_STR = 0x00000010L;
            const uint32_t CH_POISON = 0x00000020L;
            const uint32_t CH_PARALYSED = 0x00000040L;
            const uint32_t CH_EXPLODE = 0x00000080L;
            const uint32_t CH_SUMMON = 0x00000100L;
        } // namespace chests
    } // namespace treasure

    namespace monsters {
        const uint8_t MON_CHANCE_OF_NEW = 160;            // 1/x chance of new monster each round
        const uint8_t MON_MAX_SIGHT = 20;                 // Maximum dis a creature can be seen
        const uint8_t MON_MAX_SPELL_CAST_DISTANCE = 20;   // Maximum dis creature spell can be cast
        const uint8_t MON_MAX_MULTIPLY_PER_LEVEL = 75;    // Maximum reproductions on a level
        const uint8_t MON_MULTIPLY_ADJUST = 7;            // High value slows multiplication
        const uint8_t MON_CHANCE_OF_NASTY = 50;           // 1/x chance of high level creature
        const uint8_t MON_MIN_PER_LEVEL = 14;             // Minimum number of monsters/level
        const uint8_t MON_MIN_TOWNSFOLK_DAY = 4;          // Number of people on town level (day)
        const uint8_t MON_MIN_TOWNSFOLK_NIGHT = 8;        // Number of people on town level (night)
        const uint8_t MON_ENDGAME_MONSTERS = 2;           // Total number of "win" creatures
        const uint8_t MON_ENDGAME_LEVEL = 50;             // Level where winning creatures begin
        const uint8_t MON_SUMMONED_LEVEL_ADJUST = 2;      // Adjust level of summoned creatures
        const uint8_t MON_PLAYER_EXP_DRAINED_PER_HIT = 2; // Percent of player exp drained per hit
        const uint8_t MON_MIN_INDEX_ID = 2;               // Minimum index in m_list (1 = py, 0 = no mon)
        const uint8_t SCARE_MONSTER = 99;

        // definitions for creatures, cmove field
        namespace move {
            const uint32_t CM_ALL_MV_FLAGS = 0x0000003FL;
            const uint32_t CM_ATTACK_ONLY = 0x00000001L;
            const uint32_t CM_MOVE_NORMAL = 0x00000002L;
            const uint32_t CM_ONLY_MAGIC = 0x00000004L; // For Quylthulgs, which have no physical movement.

            const uint32_t CM_RANDOM_MOVE = 0x00000038L;
            const uint32_t CM_20_RANDOM = 0x00000008L;
            const uint32_t CM_40_RANDOM = 0x00000010L;
            const uint32_t CM_75_RANDOM = 0x00000020L;

            const uint32_t CM_SPECIAL = 0x003F0000L;
            const uint32_t CM_INVISIBLE = 0x00010000L;
            const uint32_t CM_OPEN_DOOR = 0x00020000L;
            const uint32_t CM_PHASE = 0x00040000L;
            const uint32_t CM_EATS_OTHER = 0x00080000L;
            const uint32_t CM_PICKS_UP = 0x00100000L;
            const uint32_t CM_MULTIPLY = 0x00200000L;

            const uint32_t CM_SMALL_OBJ = 0x00800000L;
            const uint32_t CM_CARRY_OBJ = 0x01000000L;
            const uint32_t CM_CARRY_GOLD = 0x02000000L;
            const uint32_t CM_TREASURE = 0x7C000000L;
            const uint32_t CM_TR_SHIFT = 26; // used for recall of treasure
            const uint32_t CM_60_RANDOM = 0x04000000L;
            const uint32_t CM_90_RANDOM = 0x08000000L;
            const uint32_t CM_1D2_OBJ = 0x10000000L;
            const uint32_t CM_2D2_OBJ = 0x20000000L;
            const uint32_t CM_4D2_OBJ = 0x40000000L;
            const uint32_t CM_WIN = 0x80000000L;
        } // namespace move

        // creature spell definitions
        namespace spells {
            const uint32_t CS_FREQ = 0x0000000FL;
            const uint32_t CS_SPELLS = 0x0001FFF0L;
            const uint32_t CS_TEL_SHORT = 0x00000010L;
            const uint32_t CS_TEL_LONG = 0x00000020L;
            const uint32_t CS_TEL_TO = 0x00000040L;
            const uint32_t CS_LGHT_WND = 0x00000080L;
            const uint32_t CS_SER_WND = 0x00000100L;
            const uint32_t CS_HOLD_PER = 0x00000200L;
            const uint32_t CS_BLIND = 0x00000400L;
            const uint32_t CS_CONFUSE = 0x00000800L;
            const uint32_t CS_FEAR = 0x00001000L;
            const uint32_t CS_SUMMON_MON = 0x00002000L;
            const uint32_t CS_SUMMON_UND = 0x00004000L;
            const uint32_t CS_SLOW_PER = 0x00008000L;
            const uint32_t CS_DRAIN_MANA = 0x00010000L;

            const uint32_t CS_BREATHE = 0x00F80000L;  // may also just indicate resistance
            const uint32_t CS_BR_LIGHT = 0x00080000L; // if no spell frequency set
            const uint32_t CS_BR_GAS = 0x00100000L;
            const uint32_t CS_BR_ACID = 0x00200000L;
            const uint32_t CS_BR_FROST = 0x00400000L;
            const uint32_t CS_BR_FIRE = 0x00800000L;
        } // namespace spells

        // creature defense flags
        namespace defense {
            const uint16_t CD_DRAGON = 0x0001;
            const uint16_t CD_ANIMAL = 0x0002;
            const uint16_t CD_EVIL = 0x0004;
            const uint16_t CD_UNDEAD = 0x0008;
            const uint16_t CD_WEAKNESS = 0x03F0;
            const uint16_t CD_FROST = 0x0010;
            const uint16_t CD_FIRE = 0x0020;
            const uint16_t CD_POISON = 0x0040;
            const uint16_t CD_ACID = 0x0080;
            const uint16_t CD_LIGHT = 0x0100;
            const uint16_t CD_STONE = 0x0200;
            const uint16_t CD_NO_SLEEP = 0x1000;
            const uint16_t CD_INFRA = 0x2000;
            const uint16_t CD_MAX_HP = 0x4000;
        } // namespace defense
    } // namespace monsters

    namespace player {
        const int32_t PLAYER_MAX_EXP = 9999999L;        // Maximum amount of experience -CJS-
        const uint8_t PLAYER_USE_DEVICE_DIFFICULTY = 3; // x> Harder devices x< Easier devices
        const uint16_t PLAYER_FOOD_FULL = 10000;        // Getting full
        const uint16_t PLAYER_FOOD_MAX = 15000;         // Maximum food value, beyond is wasted
        const uint16_t PLAYER_FOOD_FAINT = 300;         // Character begins fainting
        const uint16_t PLAYER_FOOD_WEAK = 1000;         // Warn player that they're getting weak
        const uint16_t PLAYER_FOOD_ALERT = 2000;        // Alert player that they're getting low on food
        const uint8_t PLAYER_REGEN_FAINT = 33;          // Regen factor*2^16 when fainting
        const uint8_t PLAYER_REGEN_WEAK = 98;           // Regen factor*2^16 when weak
        const uint8_t PLAYER_REGEN_NORMAL = 197;        // Regen factor*2^16 when full
        const uint16_t PLAYER_REGEN_HPBASE = 1442;      // Min amount hp regen*2^16
        const uint16_t PLAYER_REGEN_MNBASE = 524;       // Min amount mana regen*2^16
        const uint8_t PLAYER_WEIGHT_CAP = 130;          // "#"*(1/10 pounds) per strength point

        // definitions for the player's status field
        namespace status {
            const uint32_t PY_HUNGRY = 0x00000001L;
            const uint32_t PY_WEAK = 0x00000002L;
            const uint32_t PY_BLIND = 0x00000004L;
            const uint32_t PY_CONFUSED = 0x00000008L;
            const uint32_t PY_FEAR = 0x00000010L;
            const uint32_t PY_POISONED = 0x00000020L;
            const uint32_t PY_FAST = 0x00000040L;
            const uint32_t PY_SLOW = 0x00000080L;
            const uint32_t PY_SEARCH = 0x00000100L;
            const uint32_t PY_REST = 0x00000200L;
            const uint32_t PY_STUDY = 0x00000400L;

            const uint32_t PY_INVULN = 0x00001000L;
            const uint32_t PY_HERO = 0x00002000L;
            const uint32_t PY_SHERO = 0x00004000L;
            const uint32_t PY_BLESSED = 0x00008000L;
            const uint32_t PY_DET_INV = 0x00010000L;
            const uint32_t PY_TIM_INFRA = 0x00020000L;
            const uint32_t PY_SPEED = 0x00040000L;
            const uint32_t PY_STR_WGT = 0x00080000L;
            const uint32_t PY_PARALYSED = 0x00100000L;
            const uint32_t PY_REPEAT = 0x00200000L;
            const uint32_t PY_ARMOR = 0x00400000L;

            const uint32_t PY_STATS = 0x3F000000L;
            const uint32_t PY_STR = 0x01000000L; // these 6 stat flags must be adjacent
            const uint32_t PY_INT = 0x02000000L;
            const uint32_t PY_WIS = 0x04000000L;
            const uint32_t PY_DEX = 0x08000000L;
            const uint32_t PY_CON = 0x10000000L;
            const uint32_t PY_CHR = 0x20000000L;

            const uint32_t PY_HP = 0x40000000L;
            const uint32_t PY_MANA = 0x80000000L;
        } // namespace status
    } // namespace player

    namespace identification {
        // id's used for object description, stored in objects_identified array
        const uint8_t OD_TRIED = 0x1;
        const uint8_t OD_KNOWN1 = 0x2;

        // id's used for item description, stored in i_ptr->ident
        const uint8_t ID_MAGIK = 0x1;
        const uint8_t ID_DAMD = 0x2;
        const uint8_t ID_EMPTY = 0x4;
        const uint8_t ID_KNOWN2 = 0x8;
        const uint8_t ID_STORE_BOUGHT = 0x10;
        const uint8_t ID_SHOW_HIT_DAM = 0x20;
        const uint8_t ID_NO_SHOW_P1 = 0x40;
        const uint8_t ID_SHOW_P1 = 0x80;
    } // namespace identification

    namespace spells {
        // Class spell types
        const uint8_t SPELL_TYPE_NONE = 0;
        const uint8_t SPELL_TYPE_MAGE = 1;
        const uint8_t SPELL_TYPE_PRIEST = 2;

        // offsets to spell names in spell_names[] array
        const uint8_t NAME_OFFSET_SPELLS = 0;
        const uint8_t NAME_OFFSET_PRAYERS = 31;
    } // namespace spells

    namespace stores {
        const uint8_t STORE_MAX_AUTO_BUY_ITEMS = 18;  // Max diff objects in stock for auto buy
        const uint8_t STORE_MIN_AUTO_SELL_ITEMS = 10; // Min diff objects in stock for auto sell
        const uint8_t STORE_STOCK_TURN_AROUND = 9;    // Amount of buying and selling normally
    } // namespace stores
} // namespace config
