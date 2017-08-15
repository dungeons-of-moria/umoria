// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Misc utility and initialization code, magic objects code

#include "headers.h"
#include "externs.h"

static void magicalArmor(Inventory_t *t_ptr, int special, int level) {
    t_ptr->to_ac += magicEnchantmentBonus(1, 30, level);

    if (!magicShouldBeEnchanted(special)) {
        return;
    }

    switch (randomNumber(9)) {
        case 1:
            t_ptr->flags |= (TR_RES_LIGHT | TR_RES_COLD | TR_RES_ACID | TR_RES_FIRE);
            t_ptr->special_name_id = SN_R;
            t_ptr->to_ac += 5;
            t_ptr->cost += 2500;
            break;
        case 2: // Resist Acid
            t_ptr->flags |= TR_RES_ACID;
            t_ptr->special_name_id = SN_RA;
            t_ptr->cost += 1000;
            break;
        case 3:
        case 4: // Resist Fire
            t_ptr->flags |= TR_RES_FIRE;
            t_ptr->special_name_id = SN_RF;
            t_ptr->cost += 600;
            break;
        case 5:
        case 6: // Resist Cold
            t_ptr->flags |= TR_RES_COLD;
            t_ptr->special_name_id = SN_RC;
            t_ptr->cost += 600;
            break;
        case 7:
        case 8:
        case 9: // Resist Lightning
            t_ptr->flags |= TR_RES_LIGHT;
            t_ptr->special_name_id = SN_RL;
            t_ptr->cost += 500;
            break;
        default:
            // Do not apply any special magic
            break;
    }
}

static void cursedArmor(Inventory_t *t_ptr, int level) {
    t_ptr->to_ac -= magicEnchantmentBonus(1, 40, level);
    t_ptr->cost = 0;
    t_ptr->flags |= TR_CURSED;
}

static void magicalSword(Inventory_t *t_ptr, int special, int level) {
    t_ptr->to_hit += magicEnchantmentBonus(0, 40, level);

    // Magical damage bonus now proportional to weapon base damage
    int damageBonus = t_ptr->damage[0] * t_ptr->damage[1];

    t_ptr->to_damage += magicEnchantmentBonus(0, 4 * damageBonus, damageBonus * level / 10);

    // the 3*special/2 is needed because weapons are not as common as
    // before change to treasure distribution, this helps keep same
    // number of ego weapons same as before, see also missiles
    if (magicShouldBeEnchanted(3 * special / 2)) {
        switch (randomNumber(16)) {
            case 1: // Holy Avenger
                t_ptr->flags |= (TR_SEE_INVIS | TR_SUST_STAT | TR_SLAY_UNDEAD | TR_SLAY_EVIL | TR_STR);
                t_ptr->to_hit += 5;
                t_ptr->to_damage += 5;
                t_ptr->to_ac += randomNumber(4);

                // the value in `misc_use` is used for strength increase
                // `misc_use` is also used for sustain stat
                t_ptr->misc_use = (int16_t) randomNumber(4);
                t_ptr->special_name_id = SN_HA;
                t_ptr->cost += t_ptr->misc_use * 500;
                t_ptr->cost += 10000;
                break;
            case 2: // Defender
                t_ptr->flags |= (TR_FFALL | TR_RES_LIGHT | TR_SEE_INVIS | TR_FREE_ACT | TR_RES_COLD | TR_RES_ACID | TR_RES_FIRE | TR_REGEN | TR_STEALTH);
                t_ptr->to_hit += 3;
                t_ptr->to_damage += 3;
                t_ptr->to_ac += 5 + randomNumber(5);
                t_ptr->special_name_id = SN_DF;

                // the value in `misc_use` is used for stealth
                t_ptr->misc_use = (int16_t) randomNumber(3);
                t_ptr->cost += t_ptr->misc_use * 500;
                t_ptr->cost += 7500;
                break;
            case 3:
            case 4: // Slay Animal
                t_ptr->flags |= TR_SLAY_ANIMAL;
                t_ptr->to_hit += 2;
                t_ptr->to_damage += 2;
                t_ptr->special_name_id = SN_SA;
                t_ptr->cost += 3000;
                break;
            case 5:
            case 6: // Slay Dragon
                t_ptr->flags |= TR_SLAY_DRAGON;
                t_ptr->to_hit += 3;
                t_ptr->to_damage += 3;
                t_ptr->special_name_id = SN_SD;
                t_ptr->cost += 4000;
                break;
            case 7:
            case 8: // Slay Evil
                t_ptr->flags |= TR_SLAY_EVIL;
                t_ptr->to_hit += 3;
                t_ptr->to_damage += 3;
                t_ptr->special_name_id = SN_SE;
                t_ptr->cost += 4000;
                break;
            case 9:
            case 10: // Slay Undead
                t_ptr->flags |= (TR_SEE_INVIS | TR_SLAY_UNDEAD);
                t_ptr->to_hit += 3;
                t_ptr->to_damage += 3;
                t_ptr->special_name_id = SN_SU;
                t_ptr->cost += 5000;
                break;
            case 11:
            case 12:
            case 13: // Flame Tongue
                t_ptr->flags |= TR_FLAME_TONGUE;
                t_ptr->to_hit++;
                t_ptr->to_damage += 3;
                t_ptr->special_name_id = SN_FT;
                t_ptr->cost += 2000;
                break;
            case 14:
            case 15:
            case 16: // Frost Brand
                t_ptr->flags |= TR_FROST_BRAND;
                t_ptr->to_hit++;
                t_ptr->to_damage++;
                t_ptr->special_name_id = SN_FB;
                t_ptr->cost += 1200;
                break;
        }
    }
}

static void cursedSword(Inventory_t *t_ptr, int level) {
    t_ptr->to_hit -= magicEnchantmentBonus(1, 55, level);

    // Magical damage bonus now proportional to weapon base damage
    int damageBonus = t_ptr->damage[0] * t_ptr->damage[1];

    t_ptr->to_damage -= magicEnchantmentBonus(1, 11 * damageBonus / 2, damageBonus * level / 10);
    t_ptr->flags |= TR_CURSED;
    t_ptr->cost = 0;
}

static void magicalBow(Inventory_t *t_ptr, int level) {
    t_ptr->to_hit += magicEnchantmentBonus(1, 30, level);

    // add damage. -CJS-
    t_ptr->to_damage += magicEnchantmentBonus(1, 20, level);
}

static void cursedBow(Inventory_t *t_ptr, int level) {
    t_ptr->to_hit -= magicEnchantmentBonus(1, 50, level);

    // add damage. -CJS-
    t_ptr->to_damage -= magicEnchantmentBonus(1, 30, level);

    t_ptr->flags |= TR_CURSED;
    t_ptr->cost = 0;
}

static void magicalDiggingTool(Inventory_t *t_ptr, int level) {
    t_ptr->misc_use += magicEnchantmentBonus(0, 25, level);
}

static void cursedDiggingTool(Inventory_t *t_ptr, int level) {
    t_ptr->misc_use = (int16_t) -magicEnchantmentBonus(1, 30, level);
    t_ptr->cost = 0;
    t_ptr->flags |= TR_CURSED;
}

static void magicalGloves(Inventory_t *t_ptr, int special, int level) {
    t_ptr->to_ac += magicEnchantmentBonus(1, 20, level);

    if (!magicShouldBeEnchanted(special)) {
        return;
    }

    if (randomNumber(2) == 1) {
        t_ptr->flags |= TR_FREE_ACT;
        t_ptr->special_name_id = SN_FREE_ACTION;
        t_ptr->cost += 1000;
    } else {
        t_ptr->identification |= ID_SHOW_HIT_DAM;
        t_ptr->to_hit += 1 + randomNumber(3);
        t_ptr->to_damage += 1 + randomNumber(3);
        t_ptr->special_name_id = SN_SLAYING;
        t_ptr->cost += (t_ptr->to_hit + t_ptr->to_damage) * 250;
    }
}

static void cursedGloves(Inventory_t *t_ptr, int special, int level) {
    if (magicShouldBeEnchanted(special)) {
        if (randomNumber(2) == 1) {
            t_ptr->flags |= TR_DEX;
            t_ptr->special_name_id = SN_CLUMSINESS;
        } else {
            t_ptr->flags |= TR_STR;
            t_ptr->special_name_id = SN_WEAKNESS;
        }
        t_ptr->identification |= ID_SHOW_P1;
        t_ptr->misc_use = (int16_t) -magicEnchantmentBonus(1, 10, level);
    }

    t_ptr->to_ac -= magicEnchantmentBonus(1, 40, level);
    t_ptr->flags |= TR_CURSED;
    t_ptr->cost = 0;
}

static void magicalBoots(Inventory_t *t_ptr, int special, int level) {
    t_ptr->to_ac += magicEnchantmentBonus(1, 20, level);

    if (!magicShouldBeEnchanted(special)) {
        return;
    }

    int magicType = randomNumber(12);

    if (magicType > 5) {
        t_ptr->flags |= TR_FFALL;
        t_ptr->special_name_id = SN_SLOW_DESCENT;
        t_ptr->cost += 250;
    } else if (magicType == 1) {
        t_ptr->flags |= TR_SPEED;
        t_ptr->special_name_id = SN_SPEED;
        t_ptr->identification |= ID_SHOW_P1;
        t_ptr->misc_use = 1;
        t_ptr->cost += 5000;
    } else {
        // 2 - 5
        t_ptr->flags |= TR_STEALTH;
        t_ptr->identification |= ID_SHOW_P1;
        t_ptr->misc_use = (int16_t) randomNumber(3);
        t_ptr->special_name_id = SN_STEALTH;
        t_ptr->cost += 500;
    }
}

static void cursedBoots(Inventory_t *t_ptr, int level) {
    int magicType = randomNumber(3);

    if (magicType == 1) {
        t_ptr->flags |= TR_SPEED;
        t_ptr->special_name_id = SN_SLOWNESS;
        t_ptr->identification |= ID_SHOW_P1;
        t_ptr->misc_use = -1;
    } else if (magicType == 2) {
        t_ptr->flags |= TR_AGGRAVATE;
        t_ptr->special_name_id = SN_NOISE;
    } else {
        t_ptr->special_name_id = SN_GREAT_MASS;
        t_ptr->weight = (uint16_t) (t_ptr->weight * 5);
    }

    t_ptr->cost = 0;
    t_ptr->to_ac -= magicEnchantmentBonus(2, 45, level);
    t_ptr->flags |= TR_CURSED;
}

static void magicalHelms(Inventory_t *t_ptr, int special, int level) {
    t_ptr->to_ac += magicEnchantmentBonus(1, 20, level);

    if (!magicShouldBeEnchanted(special)) {
        return;
    }

    if (t_ptr->sub_category_id < 6) {
        t_ptr->identification |= ID_SHOW_P1;

        int magicType = randomNumber(3);

        if (magicType == 1) {
            t_ptr->misc_use = (int16_t) randomNumber(2);
            t_ptr->flags |= TR_INT;
            t_ptr->special_name_id = SN_INTELLIGENCE;
            t_ptr->cost += t_ptr->misc_use * 500;
        } else if (magicType == 2) {
            t_ptr->misc_use = (int16_t) randomNumber(2);
            t_ptr->flags |= TR_WIS;
            t_ptr->special_name_id = SN_WISDOM;
            t_ptr->cost += t_ptr->misc_use * 500;
        } else {
            t_ptr->misc_use = (int16_t) (1 + randomNumber(4));
            t_ptr->flags |= TR_INFRA;
            t_ptr->special_name_id = SN_INFRAVISION;
            t_ptr->cost += t_ptr->misc_use * 250;
        }

        return;
    }

    switch (randomNumber(6)) {
        case 1:
            t_ptr->identification |= ID_SHOW_P1;
            t_ptr->misc_use = (int16_t) randomNumber(3);
            t_ptr->flags |= (TR_FREE_ACT | TR_CON | TR_DEX | TR_STR);
            t_ptr->special_name_id = SN_MIGHT;
            t_ptr->cost += 1000 + t_ptr->misc_use * 500;
            break;
        case 2:
            t_ptr->identification |= ID_SHOW_P1;
            t_ptr->misc_use = (int16_t) randomNumber(3);
            t_ptr->flags |= (TR_CHR | TR_WIS);
            t_ptr->special_name_id = SN_LORDLINESS;
            t_ptr->cost += 1000 + t_ptr->misc_use * 500;
            break;
        case 3:
            t_ptr->identification |= ID_SHOW_P1;
            t_ptr->misc_use = (int16_t) randomNumber(3);
            t_ptr->flags |= (TR_RES_LIGHT | TR_RES_COLD | TR_RES_ACID | TR_RES_FIRE | TR_INT);
            t_ptr->special_name_id = SN_MAGI;
            t_ptr->cost += 3000 + t_ptr->misc_use * 500;
            break;
        case 4:
            t_ptr->identification |= ID_SHOW_P1;
            t_ptr->misc_use = (int16_t) randomNumber(3);
            t_ptr->flags |= TR_CHR;
            t_ptr->special_name_id = SN_BEAUTY;
            t_ptr->cost += 750;
            break;
        case 5:
            t_ptr->identification |= ID_SHOW_P1;
            t_ptr->misc_use = (int16_t) (5 * (1 + randomNumber(4)));
            t_ptr->flags |= (TR_SEE_INVIS | TR_SEARCH);
            t_ptr->special_name_id = SN_SEEING;
            t_ptr->cost += 1000 + t_ptr->misc_use * 100;
            break;
        case 6:
            t_ptr->flags |= TR_REGEN;
            t_ptr->special_name_id = SN_REGENERATION;
            t_ptr->cost += 1500;
            break;
    }
}

static void cursedHelms(Inventory_t *t_ptr, int special, int level) {
    t_ptr->to_ac -= magicEnchantmentBonus(1, 45, level);
    t_ptr->flags |= TR_CURSED;
    t_ptr->cost = 0;

    if (!magicShouldBeEnchanted(special)) {
        return;
    }

    switch (randomNumber(7)) {
        case 1:
            t_ptr->identification |= ID_SHOW_P1;
            t_ptr->misc_use = (int16_t) -randomNumber(5);
            t_ptr->flags |= TR_INT;
            t_ptr->special_name_id = SN_STUPIDITY;
            break;
        case 2:
            t_ptr->identification |= ID_SHOW_P1;
            t_ptr->misc_use = (int16_t) -randomNumber(5);
            t_ptr->flags |= TR_WIS;
            t_ptr->special_name_id = SN_DULLNESS;
            break;
        case 3:
            t_ptr->flags |= TR_BLIND;
            t_ptr->special_name_id = SN_BLINDNESS;
            break;
        case 4:
            t_ptr->flags |= TR_TIMID;
            t_ptr->special_name_id = SN_TIMIDNESS;
            break;
        case 5:
            t_ptr->identification |= ID_SHOW_P1;
            t_ptr->misc_use = (int16_t) -randomNumber(5);
            t_ptr->flags |= TR_STR;
            t_ptr->special_name_id = SN_WEAKNESS;
            break;
        case 6:
            t_ptr->flags |= TR_TELEPORT;
            t_ptr->special_name_id = SN_TELEPORTATION;
            break;
        case 7:
            t_ptr->identification |= ID_SHOW_P1;
            t_ptr->misc_use = (int16_t) -randomNumber(5);
            t_ptr->flags |= TR_CHR;
            t_ptr->special_name_id = SN_UGLINESS;
            break;
    }
}

static void processRings(Inventory_t *t_ptr, int level, int cursed) {
    switch (t_ptr->sub_category_id) {
        case 0:
        case 1:
        case 2:
        case 3:
            if (magicShouldBeEnchanted(cursed)) {
                t_ptr->misc_use = (int16_t) -magicEnchantmentBonus(1, 20, level);
                t_ptr->flags |= TR_CURSED;
                t_ptr->cost = -t_ptr->cost;
            } else {
                t_ptr->misc_use = (int16_t) magicEnchantmentBonus(1, 10, level);
                t_ptr->cost += t_ptr->misc_use * 100;
            }
            break;
        case 4:
            if (magicShouldBeEnchanted(cursed)) {
                t_ptr->misc_use = (int16_t) -randomNumber(3);
                t_ptr->flags |= TR_CURSED;
                t_ptr->cost = -t_ptr->cost;
            } else {
                t_ptr->misc_use = 1;
            }
            break;
        case 5:
            t_ptr->misc_use = (int16_t) (5 * magicEnchantmentBonus(1, 20, level));
            t_ptr->cost += t_ptr->misc_use * 50;
            if (magicShouldBeEnchanted(cursed)) {
                t_ptr->misc_use = -t_ptr->misc_use;
                t_ptr->flags |= TR_CURSED;
                t_ptr->cost = -t_ptr->cost;
            }
            break;
        case 19: // Increase damage
            t_ptr->to_damage += magicEnchantmentBonus(1, 20, level);
            t_ptr->cost += t_ptr->to_damage * 100;
            if (magicShouldBeEnchanted(cursed)) {
                t_ptr->to_damage = -t_ptr->to_damage;
                t_ptr->flags |= TR_CURSED;
                t_ptr->cost = -t_ptr->cost;
            }
            break;
        case 20: // Increase To-Hit
            t_ptr->to_hit += magicEnchantmentBonus(1, 20, level);
            t_ptr->cost += t_ptr->to_hit * 100;
            if (magicShouldBeEnchanted(cursed)) {
                t_ptr->to_hit = -t_ptr->to_hit;
                t_ptr->flags |= TR_CURSED;
                t_ptr->cost = -t_ptr->cost;
            }
            break;
        case 21: // Protection
            t_ptr->to_ac += magicEnchantmentBonus(1, 20, level);
            t_ptr->cost += t_ptr->to_ac * 100;
            if (magicShouldBeEnchanted(cursed)) {
                t_ptr->to_ac = -t_ptr->to_ac;
                t_ptr->flags |= TR_CURSED;
                t_ptr->cost = -t_ptr->cost;
            }
            break;
        case 24:
        case 25:
        case 26:
        case 27:
        case 28:
        case 29:
            t_ptr->identification |= ID_NO_SHOW_P1;
            break;
        case 30: // Slaying
            t_ptr->identification |= ID_SHOW_HIT_DAM;
            t_ptr->to_damage += magicEnchantmentBonus(1, 25, level);
            t_ptr->to_hit += magicEnchantmentBonus(1, 25, level);
            t_ptr->cost += (t_ptr->to_hit + t_ptr->to_damage) * 100;
            if (magicShouldBeEnchanted(cursed)) {
                t_ptr->to_hit = -t_ptr->to_hit;
                t_ptr->to_damage = -t_ptr->to_damage;
                t_ptr->flags |= TR_CURSED;
                t_ptr->cost = -t_ptr->cost;
            }
            break;
        default:
            break;
    }
}

static void processAmulets(Inventory_t *t_ptr, int level, int cursed) {
    if (t_ptr->sub_category_id < 2) {
        if (magicShouldBeEnchanted(cursed)) {
            t_ptr->misc_use = (int16_t) -magicEnchantmentBonus(1, 20, level);
            t_ptr->flags |= TR_CURSED;
            t_ptr->cost = -t_ptr->cost;
        } else {
            t_ptr->misc_use = (int16_t) magicEnchantmentBonus(1, 10, level);
            t_ptr->cost += t_ptr->misc_use * 100;
        }
    } else if (t_ptr->sub_category_id == 2) {
        t_ptr->misc_use = (int16_t) (5 * magicEnchantmentBonus(1, 25, level));
        if (magicShouldBeEnchanted(cursed)) {
            t_ptr->misc_use = -t_ptr->misc_use;
            t_ptr->cost = -t_ptr->cost;
            t_ptr->flags |= TR_CURSED;
        } else {
            t_ptr->cost += 50 * t_ptr->misc_use;
        }
    } else if (t_ptr->sub_category_id == 8) {
        // amulet of the magi is never cursed
        t_ptr->misc_use = (int16_t) (5 * magicEnchantmentBonus(1, 25, level));
        t_ptr->cost += 20 * t_ptr->misc_use;
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
            return randomNumber(20) + 12;
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
            return randomNumber(5) + 3;
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
            return randomNumber(3) + 1;
        case 8:
            return randomNumber(3) + 1;
        case 9:
            return randomNumber(5) + 6;
        case 10:
            return randomNumber(10) + 12;
        case 11:
            return randomNumber(5) + 6;
        case 12:
            return randomNumber(5) + 6;
        case 13:
            return randomNumber(5) + 6;
        case 14:
            return randomNumber(10) + 12;
        case 15:
            return randomNumber(3) + 4;
        case 16:
            return randomNumber(5) + 6;
        case 17:
            return randomNumber(5) + 6;
        case 18:
            return randomNumber(3) + 4;
        case 19:
            return randomNumber(10) + 12;
        case 20:
            return randomNumber(3) + 4;
        case 21:
            return randomNumber(3) + 4;
        case 22:
            return randomNumber(10) + 6;
        default:
            return -1;
    }
}

static void magicalCloak(Inventory_t *t_ptr, int special, int level) {
    if (!magicShouldBeEnchanted(special)) {
        t_ptr->to_ac += magicEnchantmentBonus(1, 20, level);
        return;
    }

    if (randomNumber(2) == 1) {
        t_ptr->special_name_id = SN_PROTECTION;
        t_ptr->to_ac += magicEnchantmentBonus(2, 40, level);
        t_ptr->cost += 250;
        return;
    }

    t_ptr->to_ac += magicEnchantmentBonus(1, 20, level);
    t_ptr->identification |= ID_SHOW_P1;
    t_ptr->misc_use = (int16_t) randomNumber(3);
    t_ptr->flags |= TR_STEALTH;
    t_ptr->special_name_id = SN_STEALTH;
    t_ptr->cost += 500;
}

static void cursedCloak(Inventory_t *t_ptr, int level) {
    int magicType = randomNumber(3);

    if (magicType == 1) {
        t_ptr->flags |= TR_AGGRAVATE;
        t_ptr->special_name_id = SN_IRRITATION;
        t_ptr->to_ac -= magicEnchantmentBonus(1, 10, level);
        t_ptr->identification |= ID_SHOW_HIT_DAM;
        t_ptr->to_hit -= magicEnchantmentBonus(1, 10, level);
        t_ptr->to_damage -= magicEnchantmentBonus(1, 10, level);
        t_ptr->cost = 0;
    } else if (magicType == 2) {
        t_ptr->special_name_id = SN_VULNERABILITY;
        t_ptr->to_ac -= magicEnchantmentBonus(10, 100, level + 50);
        t_ptr->cost = 0;
    } else {
        t_ptr->special_name_id = SN_ENVELOPING;
        t_ptr->to_ac -= magicEnchantmentBonus(1, 10, level);
        t_ptr->identification |= ID_SHOW_HIT_DAM;
        t_ptr->to_hit -= magicEnchantmentBonus(2, 40, level + 10);
        t_ptr->to_damage -= magicEnchantmentBonus(2, 40, level + 10);
        t_ptr->cost = 0;
    }

    t_ptr->flags |= TR_CURSED;
}

static void magicalChests(Inventory_t *t_ptr, int level) {
    switch (randomNumber(level + 4)) {
        case 1:
            t_ptr->flags = 0;
            t_ptr->special_name_id = SN_EMPTY;
            break;
        case 2:
            t_ptr->flags |= CH_LOCKED;
            t_ptr->special_name_id = SN_LOCKED;
            break;
        case 3:
        case 4:
            t_ptr->flags |= (CH_LOSE_STR | CH_LOCKED);
            t_ptr->special_name_id = SN_POISON_NEEDLE;
            break;
        case 5:
        case 6:
            t_ptr->flags |= (CH_POISON | CH_LOCKED);
            t_ptr->special_name_id = SN_POISON_NEEDLE;
            break;
        case 7:
        case 8:
        case 9:
            t_ptr->flags |= (CH_PARALYSED | CH_LOCKED);
            t_ptr->special_name_id = SN_GAS_TRAP;
            break;
        case 10:
        case 11:
            t_ptr->flags |= (CH_EXPLODE | CH_LOCKED);
            t_ptr->special_name_id = SN_EXPLOSION_DEVICE;
            break;
        case 12:
        case 13:
        case 14:
            t_ptr->flags |= (CH_SUMMON | CH_LOCKED);
            t_ptr->special_name_id = SN_SUMMONING_RUNES;
            break;
        case 15:
        case 16:
        case 17:
            t_ptr->flags |= (CH_PARALYSED | CH_POISON | CH_LOSE_STR | CH_LOCKED);
            t_ptr->special_name_id = SN_MULTIPLE_TRAPS;
            break;
        default:
            t_ptr->flags |= (CH_SUMMON | CH_EXPLODE | CH_LOCKED);
            t_ptr->special_name_id = SN_MULTIPLE_TRAPS;
            break;
    }
}

static void magicalProjectiles(Inventory_t *t_ptr, int special, int level) {
    t_ptr->to_hit += magicEnchantmentBonus(1, 35, level);
    t_ptr->to_damage += magicEnchantmentBonus(1, 35, level);

    // see comment for weapons
    if (magicShouldBeEnchanted(3 * special / 2)) {
        switch (randomNumber(10)) {
            case 1:
            case 2:
            case 3:
                t_ptr->special_name_id = SN_SLAYING;
                t_ptr->to_hit += 5;
                t_ptr->to_damage += 5;
                t_ptr->cost += 20;
                break;
            case 4:
            case 5:
                t_ptr->flags |= TR_FLAME_TONGUE;
                t_ptr->to_hit += 2;
                t_ptr->to_damage += 4;
                t_ptr->special_name_id = SN_FIRE;
                t_ptr->cost += 25;
                break;
            case 6:
            case 7:
                t_ptr->flags |= TR_SLAY_EVIL;
                t_ptr->to_hit += 3;
                t_ptr->to_damage += 3;
                t_ptr->special_name_id = SN_SLAY_EVIL;
                t_ptr->cost += 25;
                break;
            case 8:
            case 9:
                t_ptr->flags |= TR_SLAY_ANIMAL;
                t_ptr->to_hit += 2;
                t_ptr->to_damage += 2;
                t_ptr->special_name_id = SN_SLAY_ANIMAL;
                t_ptr->cost += 30;
                break;
            case 10:
                t_ptr->flags |= TR_SLAY_DRAGON;
                t_ptr->to_hit += 3;
                t_ptr->to_damage += 3;
                t_ptr->special_name_id = SN_DRAGON_SLAYING;
                t_ptr->cost += 35;
                break;
        }
    }
}

static void cursedProjectiles(Inventory_t *t_ptr, int level) {
    t_ptr->to_hit -= magicEnchantmentBonus(5, 55, level);
    t_ptr->to_damage -= magicEnchantmentBonus(5, 55, level);
    t_ptr->flags |= TR_CURSED;
    t_ptr->cost = 0;
}

// Chance of treasure having magic abilities -RAK-
// Chance increases with each dungeon level
void magicTreasureMagicalAbility(int item_id, int level) {
    int chance = OBJECT_BASE_MAGIC + level;
    if (chance > OBJECT_MAX_BASE_MAGIC) {
        chance = OBJECT_MAX_BASE_MAGIC;
    }

    int special = chance / OBJECT_CHANCE_SPECIAL;
    int cursed = (10 * chance) / OBJECT_CHANCE_CURSED;

    int magicAmount;

    Inventory_t *t_ptr = &treasure_list[item_id];

    // some objects appear multiple times in the game_objects with different
    // levels, this is to make the object occur more often, however, for
    // consistency, must set the level of these duplicates to be the same
    // as the object with the lowest level

    // Depending on treasure type, it can have certain magical properties
    switch (t_ptr->category_id) {
        case TV_SHIELD:
        case TV_HARD_ARMOR:
        case TV_SOFT_ARMOR:
            if (magicShouldBeEnchanted(chance)) {
                magicalArmor(t_ptr, special, level);
            } else if (magicShouldBeEnchanted(cursed)) {
                cursedArmor(t_ptr, level);
            }
            break;
        case TV_HAFTED:
        case TV_POLEARM:
        case TV_SWORD:
            // always show to_hit/to_damage values if identified
            t_ptr->identification |= ID_SHOW_HIT_DAM;

            if (magicShouldBeEnchanted(chance)) {
                magicalSword(t_ptr, special, level);
            } else if (magicShouldBeEnchanted(cursed)) {
                cursedSword(t_ptr, level);
            }
            break;
        case TV_BOW:
            // always show to_hit/to_damage values if identified
            t_ptr->identification |= ID_SHOW_HIT_DAM;

            if (magicShouldBeEnchanted(chance)) {
                magicalBow(t_ptr, level);
            } else if (magicShouldBeEnchanted(cursed)) {
                cursedBow(t_ptr, level);
            }
            break;
        case TV_DIGGING:
            // always show to_hit/to_damage values if identified
            t_ptr->identification |= ID_SHOW_HIT_DAM;

            if (magicShouldBeEnchanted(chance)) {
                if (randomNumber(3) < 3) {
                    magicalDiggingTool(t_ptr, level);
                } else {
                    cursedDiggingTool(t_ptr, level);
                }
            }
            break;
        case TV_GLOVES:
            if (magicShouldBeEnchanted(chance)) {
                magicalGloves(t_ptr, special, level);
            } else if (magicShouldBeEnchanted(cursed)) {
                cursedGloves(t_ptr, special, level);
            }
            break;
        case TV_BOOTS:
            if (magicShouldBeEnchanted(chance)) {
                magicalBoots(t_ptr, special, level);
            } else if (magicShouldBeEnchanted(cursed)) {
                cursedBoots(t_ptr, level);
            }
            break;
        case TV_HELM:
            // give crowns a higher chance for magic
            if (t_ptr->sub_category_id >= 6 && t_ptr->sub_category_id <= 8) {
                chance += (int) (t_ptr->cost / 100);
                special += special;
            }

            if (magicShouldBeEnchanted(chance)) {
                magicalHelms(t_ptr, special, level);
            } else if (magicShouldBeEnchanted(cursed)) {
                cursedHelms(t_ptr, special, level);
            }
            break;
        case TV_RING:
            processRings(t_ptr, level, cursed);
            break;
        case TV_AMULET:
            processAmulets(t_ptr, level, cursed);
            break;
        case TV_LIGHT:
            // `sub_category_id` should be even for store, odd for dungeon
            // Dungeon found ones will be partially charged
            if ((t_ptr->sub_category_id % 2) == 1) {
                t_ptr->misc_use = (int16_t) randomNumber(t_ptr->misc_use);
                t_ptr->sub_category_id -= 1;
            }
            break;
        case TV_WAND:
            magicAmount = wandMagic(t_ptr->sub_category_id);
            if (magicAmount != -1) {
                t_ptr->misc_use = (uint16_t) magicAmount;
            }
            break;
        case TV_STAFF:
            magicAmount = staffMagic(t_ptr->sub_category_id);
            if (magicAmount != -1) {
                t_ptr->misc_use = (uint16_t) magicAmount;
            }

            // Change the level the items was first found on value
            if (t_ptr->sub_category_id == 7) {
                t_ptr->depth_first_found = 10;
            } else if (t_ptr->sub_category_id == 22) {
                t_ptr->depth_first_found = 5;
            }
            break;
        case TV_CLOAK:
            if (magicShouldBeEnchanted(chance)) {
                magicalCloak(t_ptr, special, level);
            } else if (magicShouldBeEnchanted(cursed)) {
                cursedCloak(t_ptr, level);
            }
            break;
        case TV_CHEST:
            magicalChests(t_ptr, level);
            break;
        case TV_SLING_AMMO:
        case TV_SPIKE:
        case TV_BOLT:
        case TV_ARROW:
            if (t_ptr->category_id == TV_SLING_AMMO || t_ptr->category_id == TV_BOLT || t_ptr->category_id == TV_ARROW) {
                // always show to_hit/to_damage values if identified
                t_ptr->identification |= ID_SHOW_HIT_DAM;

                if (magicShouldBeEnchanted(chance)) {
                    magicalProjectiles(t_ptr, special, level);
                } else if (magicShouldBeEnchanted(cursed)) {
                    cursedProjectiles(t_ptr, level);
                }
            }

            t_ptr->items_count = 0;

            for (int i = 0; i < 7; i++) {
                t_ptr->items_count += randomNumber(6);
            }

            if (missiles_counter == MAX_SHORT) {
                missiles_counter = -MAX_SHORT - 1;
            } else {
                missiles_counter++;
            }

            t_ptr->misc_use = missiles_counter;
            break;
        case TV_FOOD:
            // make sure all food rations have the same level
            if (t_ptr->sub_category_id == 90) {
                t_ptr->depth_first_found = 0;
            }

            // give all Elvish waybread the same level
            if (t_ptr->sub_category_id == 92) {
                t_ptr->depth_first_found = 6;
            }
            break;
        case TV_SCROLL1:
            if (t_ptr->sub_category_id == 67) {
                // give all identify scrolls the same level
                t_ptr->depth_first_found = 1;
            } else if (t_ptr->sub_category_id == 69) {
                // scroll of light
                t_ptr->depth_first_found = 0;
            } else if (t_ptr->sub_category_id == 80) {
                // scroll of trap detection
                t_ptr->depth_first_found = 5;
            } else if (t_ptr->sub_category_id == 81) {
                // scroll of door/stair location
                t_ptr->depth_first_found = 5;
            }
            break;
        case TV_POTION1:
            // cure light
            if (t_ptr->sub_category_id == 76) {
                t_ptr->depth_first_found = 0;
            }
            break;
        default:
            break;
    }
}

static struct {
    const char *o_prompt;
    bool *o_var;
} options[] = {{"Running: cut known corners",            &config.run_cut_corners},
               {"Running: examine potential corners",    &config.run_examine_corners},
               {"Running: print self during run",        &config.run_print_self},
               {"Running: stop when map sector changes", &config.find_bound},
               {"Running: run through open doors",       &config.run_ignore_doors},
               {"Prompt to pick up objects",             &config.prompt_to_pickup},
               {"Rogue like commands",                   &config.use_roguelike_keys},
               {"Show weights in inventory",             &config.show_inventory_weights},
               {"Highlight and notice mineral seams",    &config.highlight_seams},
               {"Beep for invalid character",            &config.error_beep_sound},
               {"Display rest/repeat counts",            &config.display_counts},
               {0,                                       0},
};

// Set or unset various boolean config.display_counts -CJS-
void setGameOptions() {
    putStringClearToEOL("  ESC when finished, y/n to set options, <return> or - to move cursor", 0, 0);

    int max;
    for (max = 0; options[max].o_prompt != 0; max++) {
        vtype_t string;
        (void) sprintf(string, "%-38s: %s", options[max].o_prompt, (*options[max].o_var ? "yes" : "no "));
        putStringClearToEOL(string, max + 1, 0);
    }
    eraseLine(max + 1, 0);

    int optionID = 0;
    while (true) {
        moveCursor(optionID + 1, 40);

        switch (getKeyInput()) {
            case ESCAPE:
                return;
            case '-':
                if (optionID > 0) {
                    optionID--;
                } else {
                    optionID = max - 1;
                }
                break;
            case ' ':
            case '\n':
            case '\r':
                if (optionID + 1 < max) {
                    optionID++;
                } else {
                    optionID = 0;
                }
                break;
            case 'y':
            case 'Y':
                putString("yes", optionID + 1, 40);

                *options[optionID].o_var = true;

                if (optionID + 1 < max) {
                    optionID++;
                } else {
                    optionID = 0;
                }
                break;
            case 'n':
            case 'N':
                putString("no ", optionID + 1, 40);

                *options[optionID].o_var = false;

                if (optionID + 1 < max) {
                    optionID++;
                } else {
                    optionID = 0;
                }
                break;
            default:
                terminalBellSound();
                break;
        }
    }
}
