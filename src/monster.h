// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

#pragma once

// Monster_t is created for any living monster found on the current dungeon level
typedef struct {
    int16_t hp;           // Hit points
    int16_t sleep_count;  // Inactive counter
    int16_t speed;        // Movement speed
    uint16_t creature_id; // Pointer into creature

    // Note: fy, fx, and cdis constrain dungeon size to less than 256 by 256
    Coord_t pos;                  // (y,x) Pointer into map
    uint8_t distance_from_player; // Current distance from player

    bool lit;
    uint8_t stunned_amount;
    uint8_t confused_amount;
} Monster_t;

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
    Dice_t hit_die;             // Creatures hit die
    uint8_t damage[4];          // Type attack and damage
    uint8_t level;              // Level of creature
} Creature_t;

// MonsterAttack_t is a base data object.
// Holds the data for a monster's attack and damage type
typedef struct {
    uint8_t type_id;
    uint8_t description_id;
    Dice_t dice;
} MonsterAttack_t;

// Creature constants
constexpr uint16_t MON_MAX_CREATURES = 279; // Number of creatures defined for univ
constexpr uint8_t MON_ATTACK_TYPES = 215;   // Number of monster attack types.

// With MON_TOTAL_ALLOCATIONS set to 101, it is possible to get compacting
// monsters messages while breeding/cloning monsters.
constexpr uint8_t MON_TOTAL_ALLOCATIONS = 125; // Max that can be allocated
constexpr uint8_t MON_MAX_LEVELS = 40;         // Maximum level of creatures
constexpr uint8_t MON_MAX_ATTACKS = 4;         // Max num attacks (used in mons memory) -CJS-

extern int hack_monptr;
extern Creature_t creatures_list[MON_MAX_CREATURES];
extern Monster_t monsters[MON_TOTAL_ALLOCATIONS];
extern int16_t monster_levels[MON_MAX_LEVELS + 1];
extern MonsterAttack_t monster_attacks[MON_ATTACK_TYPES];
extern Monster_t blank_monster;
extern int16_t next_free_monster_id;
extern int16_t monster_multiply_total;

void monsterUpdateVisibility(int monster_id);
bool monsterMultiply(Coord_t coord, int creature_id, int monster_id);
void updateMonsters(bool attack);
uint32_t monsterDeath(Coord_t coord, uint32_t flags);
int monsterTakeHit(int monster_id, int damage);
void printMonsterActionText(const std::string &name, const std::string &action);
std::string monsterNameDescription(const std::string &real_name, bool is_lit);
bool monsterSleep(Coord_t coord);

// monster management
bool compactMonsters();
bool monsterPlaceNew(Coord_t coord, int creature_id, bool sleeping);
void monsterPlaceWinning();
void monsterPlaceNewWithinDistance(int number, int distance_from_source, bool sleeping);
bool monsterSummon(Coord_t &coord, bool sleeping);
bool monsterSummonUndead(Coord_t &coord);
