// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// Functions related to Player stats

#include "headers.h"

// I don't really like this, but for now, it's better than being a global -MRC-
void playerInitializeBaseExperienceLevels() {
    // TODO: load from external data file
    uint32_t levels[PLAYER_MAX_LEVEL] = {
        10,   25,   45,    70,    100,   140,   200,    280,    380,    500,     650,     850,     1100,    1400,    1800,    2300,    2900,     3600,     4400,     5400,
        6800, 8400, 10200, 12500, 17500, 25000, 35000L, 50000L, 75000L, 100000L, 150000L, 200000L, 300000L, 400000L, 500000L, 750000L, 1500000L, 2500000L, 5000000L, 10000000L,
    };

    for (auto i = 0; i < PLAYER_MAX_LEVEL; i++) {
        py.base_exp_levels[i] = levels[i];
    }
}

// Calculate the players hit points
void playerCalculateHitPoints() {
    int hp = py.base_hp_levels[py.misc.level - 1] + (playerStatAdjustmentConstitution() * py.misc.level);

    // Always give at least one point per level + 1
    if (hp < (py.misc.level + 1)) {
        hp = py.misc.level + 1;
    }

    if ((py.flags.status & config::player::status::PY_HERO) != 0u) {
        hp += 10;
    }

    if ((py.flags.status & config::player::status::PY_SHERO) != 0u) {
        hp += 20;
    }

    // MHP can equal zero while character is being created
    if (hp != py.misc.max_hp && py.misc.max_hp != 0) {
        // Change current hit points proportionately to change of MHP,
        // divide first to avoid overflow, little loss of accuracy
        int32_t value = (((int32_t) py.misc.current_hp << 16) + py.misc.current_hp_fraction) / py.misc.max_hp * hp;
        py.misc.current_hp = (int16_t)(value >> 16);
        py.misc.current_hp_fraction = (uint16_t)(value & 0xFFFF);
        py.misc.max_hp = (int16_t) hp;

        // can't print hit points here, may be in store or inventory mode
        py.flags.status |= config::player::status::PY_HP;
    }
}

static int playerAttackBlowsDexterity(int dexterity) {
    int dex;

    if (dexterity < 10) {
        dex = 0;
    } else if (dexterity < 19) {
        dex = 1;
    } else if (dexterity < 68) {
        dex = 2;
    } else if (dexterity < 108) {
        dex = 3;
    } else if (dexterity < 118) {
        dex = 4;
    } else {
        dex = 5;
    }

    return dex;
}

static int playerAttackBlowsStrength(int strength, int weight) {
    int adj_weight = (strength * 10 / weight);

    int str;

    if (adj_weight < 2) {
        str = 0;
    } else if (adj_weight < 3) {
        str = 1;
    } else if (adj_weight < 4) {
        str = 2;
    } else if (adj_weight < 5) {
        str = 3;
    } else if (adj_weight < 7) {
        str = 4;
    } else if (adj_weight < 9) {
        str = 5;
    } else {
        str = 6;
    }

    return str;
}

// Weapon weight VS strength and dexterity -RAK-
int playerAttackBlows(int weight, int &weight_to_hit) {
    weight_to_hit = 0;

    int player_strength = py.stats.used[PlayerAttr::A_STR];

    if (player_strength * 15 < weight) {
        weight_to_hit = player_strength * 15 - weight;
        return 1;
    }

    int dexterity = playerAttackBlowsDexterity(py.stats.used[PlayerAttr::A_DEX]);
    int strength = playerAttackBlowsStrength(player_strength, weight);

    return (int) blows_table[strength][dexterity];
}

// Adjustment for wisdom/intelligence -JWT-
int playerStatAdjustmentWisdomIntelligence(int stat) {
    int value = py.stats.used[stat];

    int adjustment;

    if (value > 117) {
        adjustment = 7;
    } else if (value > 107) {
        adjustment = 6;
    } else if (value > 87) {
        adjustment = 5;
    } else if (value > 67) {
        adjustment = 4;
    } else if (value > 17) {
        adjustment = 3;
    } else if (value > 14) {
        adjustment = 2;
    } else if (value > 7) {
        adjustment = 1;
    } else {
        adjustment = 0;
    }

    return adjustment;
}

// Adjustment for charisma -RAK-
// Percent decrease or increase in price of goods
int playerStatAdjustmentCharisma() {
    int charisma = py.stats.used[PlayerAttr::A_CHR];

    if (charisma > 117) {
        return 90;
    }

    if (charisma > 107) {
        return 92;
    }

    if (charisma > 87) {
        return 94;
    }

    if (charisma > 67) {
        return 96;
    }

    if (charisma > 18) {
        return 98;
    }

    switch (charisma) {
        case 18:
            return 100;
        case 17:
            return 101;
        case 16:
            return 102;
        case 15:
            return 103;
        case 14:
            return 104;
        case 13:
            return 106;
        case 12:
            return 108;
        case 11:
            return 110;
        case 10:
            return 112;
        case 9:
            return 114;
        case 8:
            return 116;
        case 7:
            return 118;
        case 6:
            return 120;
        case 5:
            return 122;
        case 4:
            return 125;
        case 3:
            return 130;
        default:
            return 100;
    }
}

// Returns a character's adjustment to hit points -JWT-
int playerStatAdjustmentConstitution() {
    int con = py.stats.used[PlayerAttr::A_CON];

    if (con < 7) {
        return (con - 7);
    }

    if (con < 17) {
        return 0;
    }

    if (con == 17) {
        return 1;
    }

    if (con < 94) {
        return 2;
    }

    if (con < 117) {
        return 3;
    }

    return 4;
}

static uint8_t playerModifyStat(int stat, int16_t amount) {
    uint8_t new_stat = py.stats.current[stat];

    int loop = (amount < 0 ? -amount : amount);

    for (int i = 0; i < loop; i++) {
        if (amount > 0) {
            if (new_stat < 18) {
                new_stat++;
            } else if (new_stat < 108) {
                new_stat += 10;
            } else {
                new_stat = 118;
            }
        } else {
            if (new_stat > 27) {
                new_stat -= 10;
            } else if (new_stat > 18) {
                new_stat = 18;
            } else if (new_stat > 3) {
                new_stat--;
            }
        }
    }

    return new_stat;
}

// Set the value of the stat which is actually used. -CJS-
void playerSetAndUseStat(int stat) {
    py.stats.used[stat] = playerModifyStat(stat, py.stats.modified[stat]);

    if (stat == PlayerAttr::A_STR) {
        py.flags.status |= config::player::status::PY_STR_WGT;
        playerRecalculateBonuses();
    } else if (stat == PlayerAttr::A_DEX) {
        playerRecalculateBonuses();
    } else if (stat == PlayerAttr::A_INT && classes[py.misc.class_id].class_to_use_mage_spells == config::spells::SPELL_TYPE_MAGE) {
        playerCalculateAllowedSpellsCount(PlayerAttr::A_INT);
        playerGainMana(PlayerAttr::A_INT);
    } else if (stat == PlayerAttr::A_WIS && classes[py.misc.class_id].class_to_use_mage_spells == config::spells::SPELL_TYPE_PRIEST) {
        playerCalculateAllowedSpellsCount(PlayerAttr::A_WIS);
        playerGainMana(PlayerAttr::A_WIS);
    } else if (stat == PlayerAttr::A_CON) {
        playerCalculateHitPoints();
    }
}

// Increases a stat by one randomized level -RAK-
bool playerStatRandomIncrease(int stat) {
    int new_stat = py.stats.current[stat];

    if (new_stat >= 118) {
        return false;
    }

    if (new_stat >= 18 && new_stat < 116) {
        // stat increases by 1/6 to 1/3 of difference from max
        int gain = ((118 - new_stat) / 3 + 1) >> 1;

        new_stat += randomNumber(gain) + gain;
    } else {
        new_stat++;
    }

    py.stats.current[stat] = (uint8_t) new_stat;

    if (new_stat > py.stats.max[stat]) {
        py.stats.max[stat] = (uint8_t) new_stat;
    }

    playerSetAndUseStat(stat);
    displayCharacterStats(stat);

    return true;
}

// Decreases a stat by one randomized level -RAK-
bool playerStatRandomDecrease(int stat) {
    int new_stat = py.stats.current[stat];

    if (new_stat <= 3) {
        return false;
    }

    if (new_stat >= 19 && new_stat < 117) {
        int loss = (((118 - new_stat) >> 1) + 1) >> 1;
        new_stat += -randomNumber(loss) - loss;

        if (new_stat < 18) {
            new_stat = 18;
        }
    } else {
        new_stat--;
    }

    py.stats.current[stat] = (uint8_t) new_stat;

    playerSetAndUseStat(stat);
    displayCharacterStats(stat);

    return true;
}

// Restore a stat.  Return true only if this actually makes a difference.
bool playerStatRestore(int stat) {
    int new_stat = py.stats.max[stat] - py.stats.current[stat];

    if (new_stat == 0) {
        return false;
    }

    py.stats.current[stat] += new_stat;

    playerSetAndUseStat(stat);
    displayCharacterStats(stat);

    return true;
}

// Boost a stat artificially (by wearing something). If the display
// argument is true, then increase is shown on the screen.
void playerStatBoost(int stat, int amount) {
    py.stats.modified[stat] += amount;

    playerSetAndUseStat(stat);

    // can not call displayCharacterStats() here:
    //   might be in a store,
    //   might be in inventoryExecuteCommand()
    py.flags.status |= (config::player::status::PY_STR << stat);
}

// Returns a character's adjustment to hit. -JWT-
int16_t playerToHitAdjustment() {
    int16_t total;

    int dexterity = py.stats.used[PlayerAttr::A_DEX];
    if (dexterity < 4) {
        total = -3;
    } else if (dexterity < 6) {
        total = -2;
    } else if (dexterity < 8) {
        total = -1;
    } else if (dexterity < 16) {
        total = 0;
    } else if (dexterity < 17) {
        total = 1;
    } else if (dexterity < 18) {
        total = 2;
    } else if (dexterity < 69) {
        total = 3;
    } else if (dexterity < 118) {
        total = 4;
    } else {
        total = 5;
    }

    int strength = py.stats.used[PlayerAttr::A_STR];
    if (strength < 4) {
        total -= 3;
    } else if (strength < 5) {
        total -= 2;
    } else if (strength < 7) {
        total -= 1;
    } else if (strength < 18) {
        total -= 0;
    } else if (strength < 94) {
        total += 1;
    } else if (strength < 109) {
        total += 2;
    } else if (strength < 117) {
        total += 3;
    } else {
        total += 4;
    }

    return total;
}

// Returns a character's adjustment to armor class -JWT-
int16_t playerArmorClassAdjustment() {
    int stat = py.stats.used[PlayerAttr::A_DEX];

    int16_t adjustment;

    if (stat < 4) {
        adjustment = -4;
    } else if (stat == 4) {
        adjustment = -3;
    } else if (stat == 5) {
        adjustment = -2;
    } else if (stat == 6) {
        adjustment = -1;
    } else if (stat < 15) {
        adjustment = 0;
    } else if (stat < 18) {
        adjustment = 1;
    } else if (stat < 59) {
        adjustment = 2;
    } else if (stat < 94) {
        adjustment = 3;
    } else if (stat < 117) {
        adjustment = 4;
    } else {
        adjustment = 5;
    }

    return adjustment;
}

// Returns a character's adjustment to disarm -RAK-
int16_t playerDisarmAdjustment() {
    auto stat = py.stats.used[PlayerAttr::A_DEX];

    int16_t adjustment = 0;

    if (stat < 4) {
        adjustment = -8;
    } else if (stat == 4) {
        adjustment = -6;
    } else if (stat == 5) {
        adjustment = -4;
    } else if (stat == 6) {
        adjustment = -2;
    } else if (stat == 7) {
        adjustment = -1;
    } else if (stat < 13) {
        adjustment = 0;
    } else if (stat < 16) {
        adjustment = 1;
    } else if (stat < 18) {
        adjustment = 2;
    } else if (stat < 59) {
        adjustment = 4;
    } else if (stat < 94) {
        adjustment = 5;
    } else if (stat < 117) {
        adjustment = 6;
    } else {
        adjustment = 8;
    }

    return adjustment;
}

// Returns a character's adjustment to damage -JWT-
int16_t playerDamageAdjustment() {
    int stat = py.stats.used[PlayerAttr::A_STR];

    int16_t adjustment;

    if (stat < 4) {
        adjustment = -2;
    } else if (stat < 5) {
        adjustment = -1;
    } else if (stat < 16) {
        adjustment = 0;
    } else if (stat < 17) {
        adjustment = 1;
    } else if (stat < 18) {
        adjustment = 2;
    } else if (stat < 94) {
        adjustment = 3;
    } else if (stat < 109) {
        adjustment = 4;
    } else if (stat < 117) {
        adjustment = 5;
    } else {
        adjustment = 6;
    }

    return adjustment;
}
