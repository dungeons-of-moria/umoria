// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Player specific variable definitions

#include "headers.h"
#include "externs.h"

// Player record for most player related info
Player_t py = Player_t{};

bool playerIsMale() {
    return py.misc.gender;
}

void playerSetGender(bool is_male) {
    py.misc.gender = is_male;
}

const char *playerGetGenderLabel() {
    if (playerIsMale()) {
        return "Male";
    }
    return "Female";
}

// I don't really like this, but for now, I like it better than being a naked global -MRC-
void playerInitializeBaseExperienceLevels() {
    // TODO: load from external data file
    uint32_t levels[PLAYER_MAX_LEVEL] = {
            10,      25,     45,      70,       100,      140,      200,       280,
            380,     500,    650,     850,      1100,     1400,     1800,      2300,
            2900,    3600,   4400,    5400,      6800,     8400,    10200,     12500,
            17500,   25000,  35000L, 50000L,    75000L,  100000L,  150000L,   200000L,
            300000L, 400000L, 500000L, 750000L, 1500000L, 2500000L, 5000000L, 10000000L,
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

    if ((py.flags.status & PY_HERO) != 0u) {
        hp += 10;
    }

    if ((py.flags.status & PY_SHERO) != 0u) {
        hp += 20;
    }

    // MHP can equal zero while character is being created
    if (hp != py.misc.max_hp && py.misc.max_hp != 0) {
        // Change current hit points proportionately to change of MHP,
        // divide first to avoid overflow, little loss of accuracy
        int32_t value = (((int32_t) py.misc.current_hp << 16) + py.misc.current_hp_fraction) / py.misc.max_hp * hp;
        py.misc.current_hp = (int16_t) (value >> 16);
        py.misc.current_hp_fraction = (uint16_t) (value & 0xFFFF);
        py.misc.max_hp = (int16_t) hp;

        // can't print hit points here, may be in store or inventory mode
        py.flags.status |= PY_HP;
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

    int player_strength = py.stats.used[A_STR];

    if (player_strength * 15 < weight) {
        weight_to_hit = player_strength * 15 - weight;
        return 1;
    }

    int dexterity = playerAttackBlowsDexterity(py.stats.used[A_DEX]);
    int strength = playerAttackBlowsStrength(player_strength, weight);

    return (int) blows_table[strength][dexterity];
}

// Given direction "dir", returns new row, column location -RAK-
bool playerMovePosition(int dir, int &new_y, int &new_x) {
    int new_row;
    int new_col;

    switch (dir) {
        case 1:
            new_row = new_y + 1;
            new_col = new_x - 1;
            break;
        case 2:
            new_row = new_y + 1;
            new_col = new_x;
            break;
        case 3:
            new_row = new_y + 1;
            new_col = new_x + 1;
            break;
        case 4:
            new_row = new_y;
            new_col = new_x - 1;
            break;
        case 5:
            new_row = new_y;
            new_col = new_x;
            break;
        case 6:
            new_row = new_y;
            new_col = new_x + 1;
            break;
        case 7:
            new_row = new_y - 1;
            new_col = new_x - 1;
            break;
        case 8:
            new_row = new_y - 1;
            new_col = new_x;
            break;
        case 9:
            new_row = new_y - 1;
            new_col = new_x + 1;
            break;
        default:
            new_row = 0;
            new_col = 0;
            break;
    }

    bool can_move = false;

    if (new_row >= 0 && new_row < dg.height && new_col >= 0 && new_col < dg.width) {
        new_y = new_row;
        new_x = new_col;
        can_move = true;
    }

    return can_move;
}

// Teleport the player to a new location -RAK-
void playerTeleport(int new_distance) {
    int new_y, new_x;

    do {
        new_y = randomNumber(dg.height) - 1;
        new_x = randomNumber(dg.width) - 1;

        while (coordDistanceBetween(Coord_t{new_y, new_x}, Coord_t{py.row, py.col}) > new_distance) {
            new_y += (py.row - new_y) / 2;
            new_x += (py.col - new_x) / 2;
        }
    } while (dg.floor[new_y][new_x].feature_id >= MIN_CLOSED_SPACE || dg.floor[new_y][new_x].creature_id >= 2);

    dungeonMoveCreatureRecord(py.row, py.col, new_y, new_x);

    for (int y = py.row - 1; y <= py.row + 1; y++) {
        for (int x = py.col - 1; x <= py.col + 1; x++) {
            dg.floor[y][x].temporary_light = false;
            dungeonLiteSpot(y, x);
        }
    }

    dungeonLiteSpot(py.row, py.col);

    py.row = (int16_t) new_y;
    py.col = (int16_t) new_x;

    dungeonResetView();
    updateMonsters(false);

    teleport_player = false;
}
