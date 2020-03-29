// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

#pragma once

// Race type for the generated player character
typedef struct {
    const char *name;       // Type of race
    int16_t str_adjustment; // adjustments
    int16_t int_adjustment;
    int16_t wis_adjustment;
    int16_t dex_adjustment;
    int16_t con_adjustment;
    int16_t chr_adjustment;
    uint8_t base_age;           // Base age of character
    uint8_t max_age;            // Maximum age of character
    uint8_t male_height_base;   // base height for males
    uint8_t male_height_mod;    // mod height for males
    uint8_t male_weight_base;   // base weight for males
    uint8_t male_weight_mod;    // mod weight for males
    uint8_t female_height_base; // base height females
    uint8_t female_height_mod;  // mod height for females
    uint8_t female_weight_base; // base weight for female
    uint8_t female_weight_mod;  // mod weight for females
    int16_t disarm_chance_base; // base chance to disarm
    int16_t search_chance_base; // base chance for search
    int16_t stealth;            // Stealth of character
    int16_t fos;                // frequency of auto search
    int16_t base_to_hit;        // adj base chance to hit
    int16_t base_to_hit_bows;   // adj base to hit with bows
    int16_t saving_throw_base;  // Race base for saving throw
    uint8_t hit_points_base;    // Base hit points for race
    uint8_t infra_vision;       // See infra-red
    uint8_t exp_factor_base;    // Base experience factor
    uint8_t classes_bit_field;  // Bit field for class types
} Race_t;

// Class type for the generated player character
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

// Class background for the generated player character
typedef struct {
    const char *info; // History information
    uint8_t roll;     // Die roll needed for history
    uint8_t chart;    // Table number
    uint8_t next;     // Pointer to next table
    uint8_t bonus;    // Bonus to the Social Class+50
} Background_t;

void characterCreate();
