// Copyright (c) 1989-94 James E. Wilson, Robert A. Koeneke
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

#pragma once

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

void monsterUpdateVisibility(int monster_id);
bool monsterMultiply(int y, int x, int creatureID, int monsterID);
void updateMonsters(bool attack);
uint32_t monsterDeath(int y, int x, uint32_t flags);
int monsterTakeHit(int monster_id, int damage);
void printMonsterActionText(const std::string &name, const std::string &action);
std::string monsterNameDescription(const std::string &real_name, bool is_lit);
bool monsterSleep(int y, int x);
bool executeAttackOnPlayer(uint8_t creature_level, int16_t &monster_hp, int monster_id, int attack_type, int damage, vtype_t death_description, bool noticed);

// monster management
bool compactMonsters();
bool monsterPlaceNew(int y, int x, int creature_id, bool sleeping);
void monsterPlaceWinning();
void monsterPlaceNewWithinDistance(int number, int distance_from_source, bool sleeping);
bool monsterSummon(int &y, int &x, bool sleeping);
bool monsterSummonUndead(int &y, int &x);
