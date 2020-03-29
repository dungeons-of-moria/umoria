// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// Magical treasure

#include "headers.h"

// Should the object be enchanted -RAK-
static bool magicShouldBeEnchanted(int chance) {
    return randomNumber(100) <= chance;
}

// Enchant a bonus based on degree desired -RAK-
static int magicEnchantmentBonus(int base, int max_standard, int level) {
    int stand_deviation = (config::treasure::LEVEL_STD_OBJECT_ADJUST * level / 100) + config::treasure::LEVEL_MIN_OBJECT_STD;

    // Check for level > max_standard since that may have generated an overflow.
    if (stand_deviation > max_standard || level > max_standard) {
        stand_deviation = max_standard;
    }

    // abs may be a macro, don't call it with randomNumberNormalDistribution() as a parameter
    auto abs_distribution = (int) std::abs((std::intmax_t) randomNumberNormalDistribution(0, stand_deviation));
    int bonus = (abs_distribution / 10) + base;

    if (bonus < base) {
        return base;
    }

    return bonus;
}

static void magicalArmor(Inventory_t &item, int special, int level) {
    item.to_ac += magicEnchantmentBonus(1, 30, level);

    if (!magicShouldBeEnchanted(special)) {
        return;
    }

    switch (randomNumber(9)) {
        case 1:
            item.flags |=
                (config::treasure::flags::TR_RES_LIGHT | config::treasure::flags::TR_RES_COLD | config::treasure::flags::TR_RES_ACID | config::treasure::flags::TR_RES_FIRE);
            item.special_name_id = SpecialNameIds::SN_R;
            item.to_ac += 5;
            item.cost += 2500;
            break;
        case 2: // Resist Acid
            item.flags |= config::treasure::flags::TR_RES_ACID;
            item.special_name_id = SpecialNameIds::SN_RA;
            item.cost += 1000;
            break;
        case 3:
        case 4: // Resist Fire
            item.flags |= config::treasure::flags::TR_RES_FIRE;
            item.special_name_id = SpecialNameIds::SN_RF;
            item.cost += 600;
            break;
        case 5:
        case 6: // Resist Cold
            item.flags |= config::treasure::flags::TR_RES_COLD;
            item.special_name_id = SpecialNameIds::SN_RC;
            item.cost += 600;
            break;
        case 7:
        case 8:
        case 9: // Resist Lightning
            item.flags |= config::treasure::flags::TR_RES_LIGHT;
            item.special_name_id = SpecialNameIds::SN_RL;
            item.cost += 500;
            break;
        default:
            // Do not apply any special magic
            break;
    }
}

static void cursedArmor(Inventory_t &item, int level) {
    item.to_ac -= magicEnchantmentBonus(1, 40, level);
    item.cost = 0;
    item.flags |= config::treasure::flags::TR_CURSED;
}

static void magicalSword(Inventory_t &item, int special, int level) {
    item.to_hit += magicEnchantmentBonus(0, 40, level);

    // Magical damage bonus now proportional to weapon base damage
    int damage_bonus = maxDiceRoll(item.damage);

    item.to_damage += magicEnchantmentBonus(0, 4 * damage_bonus, damage_bonus * level / 10);

    // the 3*special/2 is needed because weapons are not as common as
    // before change to treasure distribution, this helps keep same
    // number of ego weapons same as before, see also missiles
    if (magicShouldBeEnchanted(3 * special / 2)) {
        switch (randomNumber(16)) {
            case 1: // Holy Avenger
                item.flags |= (config::treasure::flags::TR_SEE_INVIS | config::treasure::flags::TR_SUST_STAT | config::treasure::flags::TR_SLAY_UNDEAD |
                               config::treasure::flags::TR_SLAY_EVIL | config::treasure::flags::TR_STR);
                item.to_hit += 5;
                item.to_damage += 5;
                item.to_ac += randomNumber(4);

                // the value in `misc_use` is used for strength increase
                // `misc_use` is also used for sustain stat
                item.misc_use = (int16_t) randomNumber(4);
                item.special_name_id = SpecialNameIds::SN_HA;
                item.cost += item.misc_use * 500;
                item.cost += 10000;
                break;
            case 2: // Defender
                item.flags |= (config::treasure::flags::TR_FFALL | config::treasure::flags::TR_RES_LIGHT | config::treasure::flags::TR_SEE_INVIS |
                               config::treasure::flags::TR_FREE_ACT | config::treasure::flags::TR_RES_COLD | config::treasure::flags::TR_RES_ACID |
                               config::treasure::flags::TR_RES_FIRE | config::treasure::flags::TR_REGEN | config::treasure::flags::TR_STEALTH);
                item.to_hit += 3;
                item.to_damage += 3;
                item.to_ac += 5 + randomNumber(5);
                item.special_name_id = SpecialNameIds::SN_DF;

                // the value in `misc_use` is used for stealth
                item.misc_use = (int16_t) randomNumber(3);
                item.cost += item.misc_use * 500;
                item.cost += 7500;
                break;
            case 3:
            case 4: // Slay Animal
                item.flags |= config::treasure::flags::TR_SLAY_ANIMAL;
                item.to_hit += 2;
                item.to_damage += 2;
                item.special_name_id = SpecialNameIds::SN_SA;
                item.cost += 3000;
                break;
            case 5:
            case 6: // Slay Dragon
                item.flags |= config::treasure::flags::TR_SLAY_DRAGON;
                item.to_hit += 3;
                item.to_damage += 3;
                item.special_name_id = SpecialNameIds::SN_SD;
                item.cost += 4000;
                break;
            case 7:
            case 8: // Slay Evil
                item.flags |= config::treasure::flags::TR_SLAY_EVIL;
                item.to_hit += 3;
                item.to_damage += 3;
                item.special_name_id = SpecialNameIds::SN_SE;
                item.cost += 4000;
                break;
            case 9:
            case 10: // Slay Undead
                item.flags |= (config::treasure::flags::TR_SEE_INVIS | config::treasure::flags::TR_SLAY_UNDEAD);
                item.to_hit += 3;
                item.to_damage += 3;
                item.special_name_id = SpecialNameIds::SN_SU;
                item.cost += 5000;
                break;
            case 11:
            case 12:
            case 13: // Flame Tongue
                item.flags |= config::treasure::flags::TR_FLAME_TONGUE;
                item.to_hit++;
                item.to_damage += 3;
                item.special_name_id = SpecialNameIds::SN_FT;
                item.cost += 2000;
                break;
            case 14:
            case 15:
            case 16: // Frost Brand
                item.flags |= config::treasure::flags::TR_FROST_BRAND;
                item.to_hit++;
                item.to_damage++;
                item.special_name_id = SpecialNameIds::SN_FB;
                item.cost += 1200;
                break;
            default:
                break;
        }
    }
}

static void cursedSword(Inventory_t &item, int level) {
    item.to_hit -= magicEnchantmentBonus(1, 55, level);

    // Magical damage bonus now proportional to weapon base damage
    int damage_bonus = maxDiceRoll(item.damage);

    item.to_damage -= magicEnchantmentBonus(1, 11 * damage_bonus / 2, damage_bonus * level / 10);
    item.flags |= config::treasure::flags::TR_CURSED;
    item.cost = 0;
}

static void magicalBow(Inventory_t &item, int level) {
    item.to_hit += magicEnchantmentBonus(1, 30, level);

    // add damage. -CJS-
    item.to_damage += magicEnchantmentBonus(1, 20, level);
}

static void cursedBow(Inventory_t &item, int level) {
    item.to_hit -= magicEnchantmentBonus(1, 50, level);

    // add damage. -CJS-
    item.to_damage -= magicEnchantmentBonus(1, 30, level);

    item.flags |= config::treasure::flags::TR_CURSED;
    item.cost = 0;
}

static void magicalDiggingTool(Inventory_t &item, int level) {
    item.misc_use += magicEnchantmentBonus(0, 25, level);
}

static void cursedDiggingTool(Inventory_t &item, int level) {
    item.misc_use = (int16_t) -magicEnchantmentBonus(1, 30, level);
    item.cost = 0;
    item.flags |= config::treasure::flags::TR_CURSED;
}

static void magicalGloves(Inventory_t &item, int special, int level) {
    item.to_ac += magicEnchantmentBonus(1, 20, level);

    if (!magicShouldBeEnchanted(special)) {
        return;
    }

    if (randomNumber(2) == 1) {
        item.flags |= config::treasure::flags::TR_FREE_ACT;
        item.special_name_id = SpecialNameIds::SN_FREE_ACTION;
        item.cost += 1000;
    } else {
        item.identification |= config::identification::ID_SHOW_HIT_DAM;
        item.to_hit += 1 + randomNumber(3);
        item.to_damage += 1 + randomNumber(3);
        item.special_name_id = SpecialNameIds::SN_SLAYING;
        item.cost += (item.to_hit + item.to_damage) * 250;
    }
}

static void cursedGloves(Inventory_t &item, int special, int level) {
    if (magicShouldBeEnchanted(special)) {
        if (randomNumber(2) == 1) {
            item.flags |= config::treasure::flags::TR_DEX;
            item.special_name_id = SpecialNameIds::SN_CLUMSINESS;
        } else {
            item.flags |= config::treasure::flags::TR_STR;
            item.special_name_id = SpecialNameIds::SN_WEAKNESS;
        }
        item.identification |= config::identification::ID_SHOW_P1;
        item.misc_use = (int16_t) -magicEnchantmentBonus(1, 10, level);
    }

    item.to_ac -= magicEnchantmentBonus(1, 40, level);
    item.flags |= config::treasure::flags::TR_CURSED;
    item.cost = 0;
}

static void magicalBoots(Inventory_t &item, int special, int level) {
    item.to_ac += magicEnchantmentBonus(1, 20, level);

    if (!magicShouldBeEnchanted(special)) {
        return;
    }

    int magic_type = randomNumber(12);

    if (magic_type > 5) {
        item.flags |= config::treasure::flags::TR_FFALL;
        item.special_name_id = SpecialNameIds::SN_SLOW_DESCENT;
        item.cost += 250;
    } else if (magic_type == 1) {
        item.flags |= config::treasure::flags::TR_SPEED;
        item.special_name_id = SpecialNameIds::SN_SPEED;
        item.identification |= config::identification::ID_SHOW_P1;
        item.misc_use = 1;
        item.cost += 5000;
    } else {
        // 2 - 5
        item.flags |= config::treasure::flags::TR_STEALTH;
        item.identification |= config::identification::ID_SHOW_P1;
        item.misc_use = (int16_t) randomNumber(3);
        item.special_name_id = SpecialNameIds::SN_STEALTH;
        item.cost += 500;
    }
}

static void cursedBoots(Inventory_t &item, int level) {
    int magic_type = randomNumber(3);

    switch (magic_type) {
        case 1:
            item.flags |= config::treasure::flags::TR_SPEED;
            item.special_name_id = SpecialNameIds::SN_SLOWNESS;
            item.identification |= config::identification::ID_SHOW_P1;
            item.misc_use = -1;
            break;
        case 2:
            item.flags |= config::treasure::flags::TR_AGGRAVATE;
            item.special_name_id = SpecialNameIds::SN_NOISE;
            break;
        default:
            item.special_name_id = SpecialNameIds::SN_GREAT_MASS;
            item.weight = (uint16_t)(item.weight * 5);
            break;
    }

    item.cost = 0;
    item.to_ac -= magicEnchantmentBonus(2, 45, level);
    item.flags |= config::treasure::flags::TR_CURSED;
}

static void magicalHelms(Inventory_t &item, int special, int level) {
    item.to_ac += magicEnchantmentBonus(1, 20, level);

    if (!magicShouldBeEnchanted(special)) {
        return;
    }

    if (item.sub_category_id < 6) {
        item.identification |= config::identification::ID_SHOW_P1;

        int magic_type = randomNumber(3);

        switch (magic_type) {
            case 1:
                item.misc_use = (int16_t) randomNumber(2);
                item.flags |= config::treasure::flags::TR_INT;
                item.special_name_id = SpecialNameIds::SN_INTELLIGENCE;
                item.cost += item.misc_use * 500;
                break;
            case 2:
                item.misc_use = (int16_t) randomNumber(2);
                item.flags |= config::treasure::flags::TR_WIS;
                item.special_name_id = SpecialNameIds::SN_WISDOM;
                item.cost += item.misc_use * 500;
                break;
            default:
                item.misc_use = (int16_t)(1 + randomNumber(4));
                item.flags |= config::treasure::flags::TR_INFRA;
                item.special_name_id = SpecialNameIds::SN_INFRAVISION;
                item.cost += item.misc_use * 250;
        }
        return;
    }

    switch (randomNumber(6)) {
        case 1:
            item.identification |= config::identification::ID_SHOW_P1;
            item.misc_use = (int16_t) randomNumber(3);
            item.flags |= (config::treasure::flags::TR_FREE_ACT | config::treasure::flags::TR_CON | config::treasure::flags::TR_DEX | config::treasure::flags::TR_STR);
            item.special_name_id = SpecialNameIds::SN_MIGHT;
            item.cost += 1000 + item.misc_use * 500;
            break;
        case 2:
            item.identification |= config::identification::ID_SHOW_P1;
            item.misc_use = (int16_t) randomNumber(3);
            item.flags |= (config::treasure::flags::TR_CHR | config::treasure::flags::TR_WIS);
            item.special_name_id = SpecialNameIds::SN_LORDLINESS;
            item.cost += 1000 + item.misc_use * 500;
            break;
        case 3:
            item.identification |= config::identification::ID_SHOW_P1;
            item.misc_use = (int16_t) randomNumber(3);
            item.flags |= (config::treasure::flags::TR_RES_LIGHT | config::treasure::flags::TR_RES_COLD | config::treasure::flags::TR_RES_ACID |
                           config::treasure::flags::TR_RES_FIRE | config::treasure::flags::TR_INT);
            item.special_name_id = SpecialNameIds::SN_MAGI;
            item.cost += 3000 + item.misc_use * 500;
            break;
        case 4:
            item.identification |= config::identification::ID_SHOW_P1;
            item.misc_use = (int16_t) randomNumber(3);
            item.flags |= config::treasure::flags::TR_CHR;
            item.special_name_id = SpecialNameIds::SN_BEAUTY;
            item.cost += 750;
            break;
        case 5:
            item.identification |= config::identification::ID_SHOW_P1;
            item.misc_use = (int16_t)(5 * (1 + randomNumber(4)));
            item.flags |= (config::treasure::flags::TR_SEE_INVIS | config::treasure::flags::TR_SEARCH);
            item.special_name_id = SpecialNameIds::SN_SEEING;
            item.cost += 1000 + item.misc_use * 100;
            break;
        case 6:
            item.flags |= config::treasure::flags::TR_REGEN;
            item.special_name_id = SpecialNameIds::SN_REGENERATION;
            item.cost += 1500;
            break;
        default:
            break;
    }
}

static void cursedHelms(Inventory_t &item, int special, int level) {
    item.to_ac -= magicEnchantmentBonus(1, 45, level);
    item.flags |= config::treasure::flags::TR_CURSED;
    item.cost = 0;

    if (!magicShouldBeEnchanted(special)) {
        return;
    }

    switch (randomNumber(7)) {
        case 1:
            item.identification |= config::identification::ID_SHOW_P1;
            item.misc_use = (int16_t) -randomNumber(5);
            item.flags |= config::treasure::flags::TR_INT;
            item.special_name_id = SpecialNameIds::SN_STUPIDITY;
            break;
        case 2:
            item.identification |= config::identification::ID_SHOW_P1;
            item.misc_use = (int16_t) -randomNumber(5);
            item.flags |= config::treasure::flags::TR_WIS;
            item.special_name_id = SpecialNameIds::SN_DULLNESS;
            break;
        case 3:
            item.flags |= config::treasure::flags::TR_BLIND;
            item.special_name_id = SpecialNameIds::SN_BLINDNESS;
            break;
        case 4:
            item.flags |= config::treasure::flags::TR_TIMID;
            item.special_name_id = SpecialNameIds::SN_TIMIDNESS;
            break;
        case 5:
            item.identification |= config::identification::ID_SHOW_P1;
            item.misc_use = (int16_t) -randomNumber(5);
            item.flags |= config::treasure::flags::TR_STR;
            item.special_name_id = SpecialNameIds::SN_WEAKNESS;
            break;
        case 6:
            item.flags |= config::treasure::flags::TR_TELEPORT;
            item.special_name_id = SpecialNameIds::SN_TELEPORTATION;
            break;
        case 7:
            item.identification |= config::identification::ID_SHOW_P1;
            item.misc_use = (int16_t) -randomNumber(5);
            item.flags |= config::treasure::flags::TR_CHR;
            item.special_name_id = SpecialNameIds::SN_UGLINESS;
            break;
        default:
            return;
    }
}

static void processRings(Inventory_t &item, int level, int cursed) {
    switch (item.sub_category_id) {
        case 0:
        case 1:
        case 2:
        case 3:
            if (magicShouldBeEnchanted(cursed)) {
                item.misc_use = (int16_t) -magicEnchantmentBonus(1, 20, level);
                item.flags |= config::treasure::flags::TR_CURSED;
                item.cost = -item.cost;
            } else {
                item.misc_use = (int16_t) magicEnchantmentBonus(1, 10, level);
                item.cost += item.misc_use * 100;
            }
            break;
        case 4:
            if (magicShouldBeEnchanted(cursed)) {
                item.misc_use = (int16_t) -randomNumber(3);
                item.flags |= config::treasure::flags::TR_CURSED;
                item.cost = -item.cost;
            } else {
                item.misc_use = 1;
            }
            break;
        case 5:
            item.misc_use = (int16_t)(5 * magicEnchantmentBonus(1, 20, level));
            item.cost += item.misc_use * 50;
            if (magicShouldBeEnchanted(cursed)) {
                item.misc_use = -item.misc_use;
                item.flags |= config::treasure::flags::TR_CURSED;
                item.cost = -item.cost;
            }
            break;
        case 19: // Increase damage
            item.to_damage += magicEnchantmentBonus(1, 20, level);
            item.cost += item.to_damage * 100;
            if (magicShouldBeEnchanted(cursed)) {
                item.to_damage = -item.to_damage;
                item.flags |= config::treasure::flags::TR_CURSED;
                item.cost = -item.cost;
            }
            break;
        case 20: // Increase To-Hit
            item.to_hit += magicEnchantmentBonus(1, 20, level);
            item.cost += item.to_hit * 100;
            if (magicShouldBeEnchanted(cursed)) {
                item.to_hit = -item.to_hit;
                item.flags |= config::treasure::flags::TR_CURSED;
                item.cost = -item.cost;
            }
            break;
        case 21: // Protection
            item.to_ac += magicEnchantmentBonus(1, 20, level);
            item.cost += item.to_ac * 100;
            if (magicShouldBeEnchanted(cursed)) {
                item.to_ac = -item.to_ac;
                item.flags |= config::treasure::flags::TR_CURSED;
                item.cost = -item.cost;
            }
            break;
        case 24:
        case 25:
        case 26:
        case 27:
        case 28:
        case 29:
            item.identification |= config::identification::ID_NO_SHOW_P1;
            break;
        case 30: // Slaying
            item.identification |= config::identification::ID_SHOW_HIT_DAM;
            item.to_damage += magicEnchantmentBonus(1, 25, level);
            item.to_hit += magicEnchantmentBonus(1, 25, level);
            item.cost += (item.to_hit + item.to_damage) * 100;
            if (magicShouldBeEnchanted(cursed)) {
                item.to_hit = -item.to_hit;
                item.to_damage = -item.to_damage;
                item.flags |= config::treasure::flags::TR_CURSED;
                item.cost = -item.cost;
            }
            break;
        default:
            break;
    }
}

static void processAmulets(Inventory_t &item, int level, int cursed) {
    if (item.sub_category_id < 2) {
        if (magicShouldBeEnchanted(cursed)) {
            item.misc_use = (int16_t) -magicEnchantmentBonus(1, 20, level);
            item.flags |= config::treasure::flags::TR_CURSED;
            item.cost = -item.cost;
        } else {
            item.misc_use = (int16_t) magicEnchantmentBonus(1, 10, level);
            item.cost += item.misc_use * 100;
        }
    } else if (item.sub_category_id == 2) {
        item.misc_use = (int16_t)(5 * magicEnchantmentBonus(1, 25, level));
        if (magicShouldBeEnchanted(cursed)) {
            item.misc_use = -item.misc_use;
            item.cost = -item.cost;
            item.flags |= config::treasure::flags::TR_CURSED;
        } else {
            item.cost += 50 * item.misc_use;
        }
    } else if (item.sub_category_id == 8) {
        // amulet of the magi is never cursed
        item.misc_use = (int16_t)(5 * magicEnchantmentBonus(1, 25, level));
        item.cost += 20 * item.misc_use;
    }
}

static int wandMagic(uint8_t id) {
    switch (id) {
        case 0:
            return randomNumber(10) + 6;
        case 1:
            return randomNumber(8) + 6;
        case 2:
            return randomNumber(5) + 6;
        case 3:
            return randomNumber(8) + 6;
        case 4:
            return randomNumber(4) + 3;
        case 5:
            return randomNumber(8) + 6;
        case 6:
        case 7:
            return randomNumber(20) + 12;
        case 8:
            return randomNumber(10) + 6;
        case 9:
            return randomNumber(12) + 6;
        case 10:
            return randomNumber(10) + 12;
        case 11:
            return randomNumber(3) + 3;
        case 12:
            return randomNumber(8) + 6;
        case 13:
            return randomNumber(10) + 6;
        case 14:
        case 15:
            return randomNumber(5) + 3;
        case 16:
            return randomNumber(5) + 6;
        case 17:
            return randomNumber(5) + 4;
        case 18:
            return randomNumber(8) + 4;
        case 19:
            return randomNumber(6) + 2;
        case 20:
            return randomNumber(4) + 2;
        case 21:
            return randomNumber(8) + 6;
        case 22:
            return randomNumber(5) + 2;
        case 23:
            return randomNumber(12) + 12;
        default:
            return -1;
    }
}

static int staffMagic(uint8_t id) {
    switch (id) {
        case 0:
            return randomNumber(20) + 12;
        case 1:
            return randomNumber(8) + 6;
        case 2:
            return randomNumber(5) + 6;
        case 3:
            return randomNumber(20) + 12;
        case 4:
            return randomNumber(15) + 6;
        case 5:
            return randomNumber(4) + 5;
        case 6:
            return randomNumber(5) + 3;
        case 7:
        case 8:
            return randomNumber(3) + 1;
        case 9:
            return randomNumber(5) + 6;
        case 10:
            return randomNumber(10) + 12;
        case 11:
        case 12:
        case 13:
            return randomNumber(5) + 6;
        case 14:
            return randomNumber(10) + 12;
        case 15:
            return randomNumber(3) + 4;
        case 16:
        case 17:
            return randomNumber(5) + 6;
        case 18:
            return randomNumber(3) + 4;
        case 19:
            return randomNumber(10) + 12;
        case 20:
        case 21:
            return randomNumber(3) + 4;
        case 22:
            return randomNumber(10) + 6;
        default:
            return -1;
    }
}

static void magicalCloak(Inventory_t &item, int special, int level) {
    if (!magicShouldBeEnchanted(special)) {
        item.to_ac += magicEnchantmentBonus(1, 20, level);
        return;
    }

    if (randomNumber(2) == 1) {
        item.special_name_id = SpecialNameIds::SN_PROTECTION;
        item.to_ac += magicEnchantmentBonus(2, 40, level);
        item.cost += 250;
        return;
    }

    item.to_ac += magicEnchantmentBonus(1, 20, level);
    item.identification |= config::identification::ID_SHOW_P1;
    item.misc_use = (int16_t) randomNumber(3);
    item.flags |= config::treasure::flags::TR_STEALTH;
    item.special_name_id = SpecialNameIds::SN_STEALTH;
    item.cost += 500;
}

static void cursedCloak(Inventory_t &item, int level) {
    int magic_type = randomNumber(3);

    switch (magic_type) {
        case 1:
            item.flags |= config::treasure::flags::TR_AGGRAVATE;
            item.special_name_id = SpecialNameIds::SN_IRRITATION;
            item.to_ac -= magicEnchantmentBonus(1, 10, level);
            item.identification |= config::identification::ID_SHOW_HIT_DAM;
            item.to_hit -= magicEnchantmentBonus(1, 10, level);
            item.to_damage -= magicEnchantmentBonus(1, 10, level);
            item.cost = 0;
            break;
        case 2:
            item.special_name_id = SpecialNameIds::SN_VULNERABILITY;
            item.to_ac -= magicEnchantmentBonus(10, 100, level + 50);
            item.cost = 0;
            break;
        default:
            item.special_name_id = SpecialNameIds::SN_ENVELOPING;
            item.to_ac -= magicEnchantmentBonus(1, 10, level);
            item.identification |= config::identification::ID_SHOW_HIT_DAM;
            item.to_hit -= magicEnchantmentBonus(2, 40, level + 10);
            item.to_damage -= magicEnchantmentBonus(2, 40, level + 10);
            item.cost = 0;
            break;
    }

    item.flags |= config::treasure::flags::TR_CURSED;
}

static void magicalChests(Inventory_t &item, int level) {
    int magic_type = randomNumber(level + 4);

    switch (magic_type) {
        case 1:
            item.flags = 0;
            item.special_name_id = SpecialNameIds::SN_EMPTY;
            break;
        case 2:
            item.flags |= config::treasure::chests::CH_LOCKED;
            item.special_name_id = SpecialNameIds::SN_LOCKED;
            break;
        case 3:
        case 4:
            item.flags |= (config::treasure::chests::CH_LOSE_STR | config::treasure::chests::CH_LOCKED);
            item.special_name_id = SpecialNameIds::SN_POISON_NEEDLE;
            break;
        case 5:
        case 6:
            item.flags |= (config::treasure::chests::CH_POISON | config::treasure::chests::CH_LOCKED);
            item.special_name_id = SpecialNameIds::SN_POISON_NEEDLE;
            break;
        case 7:
        case 8:
        case 9:
            item.flags |= (config::treasure::chests::CH_PARALYSED | config::treasure::chests::CH_LOCKED);
            item.special_name_id = SpecialNameIds::SN_GAS_TRAP;
            break;
        case 10:
        case 11:
            item.flags |= (config::treasure::chests::CH_EXPLODE | config::treasure::chests::CH_LOCKED);
            item.special_name_id = SpecialNameIds::SN_EXPLOSION_DEVICE;
            break;
        case 12:
        case 13:
        case 14:
            item.flags |= (config::treasure::chests::CH_SUMMON | config::treasure::chests::CH_LOCKED);
            item.special_name_id = SpecialNameIds::SN_SUMMONING_RUNES;
            break;
        case 15:
        case 16:
        case 17:
            item.flags |=
                (config::treasure::chests::CH_PARALYSED | config::treasure::chests::CH_POISON | config::treasure::chests::CH_LOSE_STR | config::treasure::chests::CH_LOCKED);
            item.special_name_id = SpecialNameIds::SN_MULTIPLE_TRAPS;
            break;
        default:
            item.flags |= (config::treasure::chests::CH_SUMMON | config::treasure::chests::CH_EXPLODE | config::treasure::chests::CH_LOCKED);
            item.special_name_id = SpecialNameIds::SN_MULTIPLE_TRAPS;
            break;
    }
}

static void magicalProjectileAdjustment(Inventory_t &item, int special, int level) {
    item.to_hit += magicEnchantmentBonus(1, 35, level);
    item.to_damage += magicEnchantmentBonus(1, 35, level);

    // see comment for weapons
    if (magicShouldBeEnchanted(3 * special / 2)) {
        switch (randomNumber(10)) {
            case 1:
            case 2:
            case 3:
                item.special_name_id = SpecialNameIds::SN_SLAYING;
                item.to_hit += 5;
                item.to_damage += 5;
                item.cost += 20;
                break;
            case 4:
            case 5:
                item.flags |= config::treasure::flags::TR_FLAME_TONGUE;
                item.to_hit += 2;
                item.to_damage += 4;
                item.special_name_id = SpecialNameIds::SN_FIRE;
                item.cost += 25;
                break;
            case 6:
            case 7:
                item.flags |= config::treasure::flags::TR_SLAY_EVIL;
                item.to_hit += 3;
                item.to_damage += 3;
                item.special_name_id = SpecialNameIds::SN_SLAY_EVIL;
                item.cost += 25;
                break;
            case 8:
            case 9:
                item.flags |= config::treasure::flags::TR_SLAY_ANIMAL;
                item.to_hit += 2;
                item.to_damage += 2;
                item.special_name_id = SpecialNameIds::SN_SLAY_ANIMAL;
                item.cost += 30;
                break;
            case 10:
                item.flags |= config::treasure::flags::TR_SLAY_DRAGON;
                item.to_hit += 3;
                item.to_damage += 3;
                item.special_name_id = SpecialNameIds::SN_DRAGON_SLAYING;
                item.cost += 35;
                break;
            default:
                break;
        }
    }
}

static void cursedProjectileAdjustment(Inventory_t &item, int level) {
    item.to_hit -= magicEnchantmentBonus(5, 55, level);
    item.to_damage -= magicEnchantmentBonus(5, 55, level);
    item.flags |= config::treasure::flags::TR_CURSED;
    item.cost = 0;
}

// Counter for missiles
// Note: converted to uint16_t when saving the game.
int16_t missiles_counter = 0;

static void magicalProjectile(Inventory_t &item, int special, int level, int chance, int cursed) {
    if (item.category_id == TV_SLING_AMMO || item.category_id == TV_BOLT || item.category_id == TV_ARROW) {
        // always show to_hit/to_damage values if identified
        item.identification |= config::identification::ID_SHOW_HIT_DAM;

        if (magicShouldBeEnchanted(chance)) {
            magicalProjectileAdjustment(item, special, level);
        } else if (magicShouldBeEnchanted(cursed)) {
            cursedProjectileAdjustment(item, level);
        }
    }

    item.items_count = 0;

    for (int i = 0; i < 7; i++) {
        item.items_count += randomNumber(6);
    }

    if (missiles_counter == SHRT_MAX) {
        missiles_counter = -SHRT_MAX - 1;
    } else {
        missiles_counter++;
    }

    item.misc_use = missiles_counter;
}

// Chance of treasure having magic abilities -RAK-
// Chance increases with each dungeon level
void magicTreasureMagicalAbility(int item_id, int level) {
    int chance = config::treasure::OBJECT_BASE_MAGIC + level;
    if (chance > config::treasure::OBJECT_MAX_BASE_MAGIC) {
        chance = config::treasure::OBJECT_MAX_BASE_MAGIC;
    }

    int special = chance / config::treasure::OBJECT_CHANCE_SPECIAL;
    int cursed = (10 * chance) / config::treasure::OBJECT_CHANCE_CURSED;

    int magic_amount;

    Inventory_t &item = game.treasure.list[item_id];

    // some objects appear multiple times in the game_objects with different
    // levels, this is to make the object occur more often, however, for
    // consistency, must set the level of these duplicates to be the same
    // as the object with the lowest level

    // Depending on treasure type, it can have certain magical properties
    switch (item.category_id) {
        case TV_SHIELD:
        case TV_HARD_ARMOR:
        case TV_SOFT_ARMOR:
            if (magicShouldBeEnchanted(chance)) {
                magicalArmor(item, special, level);
            } else if (magicShouldBeEnchanted(cursed)) {
                cursedArmor(item, level);
            }
            break;
        case TV_HAFTED:
        case TV_POLEARM:
        case TV_SWORD:
            // always show to_hit/to_damage values if identified
            item.identification |= config::identification::ID_SHOW_HIT_DAM;

            if (magicShouldBeEnchanted(chance)) {
                magicalSword(item, special, level);
            } else if (magicShouldBeEnchanted(cursed)) {
                cursedSword(item, level);
            }
            break;
        case TV_BOW:
            // always show to_hit/to_damage values if identified
            item.identification |= config::identification::ID_SHOW_HIT_DAM;

            if (magicShouldBeEnchanted(chance)) {
                magicalBow(item, level);
            } else if (magicShouldBeEnchanted(cursed)) {
                cursedBow(item, level);
            }
            break;
        case TV_DIGGING:
            // always show to_hit/to_damage values if identified
            item.identification |= config::identification::ID_SHOW_HIT_DAM;

            if (magicShouldBeEnchanted(chance)) {
                if (randomNumber(3) < 3) {
                    magicalDiggingTool(item, level);
                } else {
                    cursedDiggingTool(item, level);
                }
            }
            break;
        case TV_GLOVES:
            if (magicShouldBeEnchanted(chance)) {
                magicalGloves(item, special, level);
            } else if (magicShouldBeEnchanted(cursed)) {
                cursedGloves(item, special, level);
            }
            break;
        case TV_BOOTS:
            if (magicShouldBeEnchanted(chance)) {
                magicalBoots(item, special, level);
            } else if (magicShouldBeEnchanted(cursed)) {
                cursedBoots(item, level);
            }
            break;
        case TV_HELM:
            // give crowns a higher chance for magic
            if (item.sub_category_id >= 6 && item.sub_category_id <= 8) {
                chance += (int) (item.cost / 100);
                special += special;
            }

            if (magicShouldBeEnchanted(chance)) {
                magicalHelms(item, special, level);
            } else if (magicShouldBeEnchanted(cursed)) {
                cursedHelms(item, special, level);
            }
            break;
        case TV_RING:
            processRings(item, level, cursed);
            break;
        case TV_AMULET:
            processAmulets(item, level, cursed);
            break;
        case TV_LIGHT:
            // `sub_category_id` should be even for store, odd for dungeon
            // Dungeon found ones will be partially charged
            if ((item.sub_category_id % 2) == 1) {
                item.misc_use = (int16_t) randomNumber(item.misc_use);
                item.sub_category_id -= 1;
            }
            break;
        case TV_WAND:
            magic_amount = wandMagic(item.sub_category_id);
            if (magic_amount != -1) {
                item.misc_use = (uint16_t) magic_amount;
            }
            break;
        case TV_STAFF:
            magic_amount = staffMagic(item.sub_category_id);
            if (magic_amount != -1) {
                item.misc_use = (uint16_t) magic_amount;
            }

            // Change the level the items was first found on value
            if (item.sub_category_id == 7) {
                item.depth_first_found = 10;
            } else if (item.sub_category_id == 22) {
                item.depth_first_found = 5;
            }
            break;
        case TV_CLOAK:
            if (magicShouldBeEnchanted(chance)) {
                magicalCloak(item, special, level);
            } else if (magicShouldBeEnchanted(cursed)) {
                cursedCloak(item, level);
            }
            break;
        case TV_CHEST:
            magicalChests(item, level);
            break;
        case TV_SLING_AMMO:
        case TV_SPIKE:
        case TV_BOLT:
        case TV_ARROW:
            magicalProjectile(item, special, level, chance, cursed);
            break;
        case TV_FOOD:
            // make sure all food rations have the same level
            if (item.sub_category_id == 90) {
                item.depth_first_found = 0;
            }

            // give all Elvish waybread the same level
            if (item.sub_category_id == 92) {
                item.depth_first_found = 6;
            }
            break;
        case TV_SCROLL1:
            if (item.sub_category_id == 67) {
                // give all identify scrolls the same level
                item.depth_first_found = 1;
            } else if (item.sub_category_id == 69) {
                // scroll of light
                item.depth_first_found = 0;
            } else if (item.sub_category_id == 80) {
                // scroll of trap detection
                item.depth_first_found = 5;
            } else if (item.sub_category_id == 81) {
                // scroll of door/stair location
                item.depth_first_found = 5;
            }
            break;
        case TV_POTION1:
            // cure light
            if (item.sub_category_id == 76) {
                item.depth_first_found = 0;
            }
            break;
        default:
            break;
    }
}
