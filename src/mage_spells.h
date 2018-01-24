// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

#pragma once

// spell types used by get_flags(), breathe(), fire_bolt() and fire_ball()
enum magic_spell_flags {
    GF_MAGIC_MISSILE,
    GF_LIGHTNING,
    GF_POISON_GAS,
    GF_ACID,
    GF_FROST,
    GF_FIRE,
    GF_HOLY_ORB,
};

void getAndCastMagicSpell();
int spellChanceOfSuccess(int spell_id);
