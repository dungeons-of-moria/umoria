// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// Player magic functions

#include "headers.h"

// Cure players confusion -RAK-
bool playerCureConfusion() {
    if (py.flags.confused > 1) {
        py.flags.confused = 1;
        return true;
    }
    return false;
}

// Cure players blindness -RAK-
bool playerCureBlindness() {
    if (py.flags.blind > 1) {
        py.flags.blind = 1;
        return true;
    }
    return false;
}

// Cure poisoning -RAK-
bool playerCurePoison() {
    if (py.flags.poisoned > 1) {
        py.flags.poisoned = 1;
        return true;
    }
    return false;
}

// Cure the players fear -RAK-
bool playerRemoveFear() {
    if (py.flags.afraid > 1) {
        py.flags.afraid = 1;
        return true;
    }
    return false;
}

// Evil creatures don't like this. -RAK-
bool playerProtectEvil() {
    bool is_protected = py.flags.protect_evil == 0;

    py.flags.protect_evil += randomNumber(25) + 3 * py.misc.level;

    return is_protected;
}

// Bless -RAK-
void playerBless(int adjustment) {
    py.flags.blessed += adjustment;
}

// Detect Invisible for period of time -RAK-
void playerDetectInvisible(int adjustment) {
    py.flags.detect_invisible += adjustment;
}

// Special damage due to magical abilities of object -RAK-
int itemMagicAbilityDamage(Inventory_t const &item, int total_damage, int monster_id) {
    bool is_ego_weapon = (item.flags & config::treasure::flags::TR_EGO_WEAPON) != 0;
    bool is_projectile = item.category_id >= TV_SLING_AMMO && item.category_id <= TV_ARROW;
    bool is_hafted_sword = item.category_id >= TV_HAFTED && item.category_id <= TV_SWORD;
    bool is_flask = item.category_id == TV_FLASK;

    if (is_ego_weapon && (is_projectile || is_hafted_sword || is_flask)) {
        Creature_t const &creature = creatures_list[monster_id];
        Recall_t &memory = creature_recall[monster_id];

        // Slay Dragon
        if (((creature.defenses & config::monsters::defense::CD_DRAGON) != 0) && ((item.flags & config::treasure::flags::TR_SLAY_DRAGON) != 0u)) {
            memory.defenses |= config::monsters::defense::CD_DRAGON;
            return total_damage * 4;
        }

        // Slay Undead
        if (((creature.defenses & config::monsters::defense::CD_UNDEAD) != 0) && ((item.flags & config::treasure::flags::TR_SLAY_UNDEAD) != 0u)) {
            memory.defenses |= config::monsters::defense::CD_UNDEAD;
            return total_damage * 3;
        }

        // Slay Animal
        if (((creature.defenses & config::monsters::defense::CD_ANIMAL) != 0) && ((item.flags & config::treasure::flags::TR_SLAY_ANIMAL) != 0u)) {
            memory.defenses |= config::monsters::defense::CD_ANIMAL;
            return total_damage * 2;
        }

        // Slay Evil
        if (((creature.defenses & config::monsters::defense::CD_EVIL) != 0) && ((item.flags & config::treasure::flags::TR_SLAY_EVIL) != 0u)) {
            memory.defenses |= config::monsters::defense::CD_EVIL;
            return total_damage * 2;
        }

        // Frost
        if (((creature.defenses & config::monsters::defense::CD_FROST) != 0) && ((item.flags & config::treasure::flags::TR_FROST_BRAND) != 0u)) {
            memory.defenses |= config::monsters::defense::CD_FROST;
            return total_damage * 3 / 2;
        }

        // Fire
        if (((creature.defenses & config::monsters::defense::CD_FIRE) != 0) && ((item.flags & config::treasure::flags::TR_FLAME_TONGUE) != 0u)) {
            memory.defenses |= config::monsters::defense::CD_FIRE;
            return total_damage * 3 / 2;
        }
    }

    return total_damage;
}
