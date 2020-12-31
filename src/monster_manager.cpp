// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// Monster management: generation, placement, cleanup

#include "headers.h"

Monster_t monsters[MON_TOTAL_ALLOCATIONS];
int16_t monster_levels[MON_MAX_LEVELS + 1];

// Values for a blank monster
Monster_t blank_monster = {0, 0, 0, 0, Coord_t{0, 0}, 0, false, 0, 0};

int16_t next_free_monster_id;   // ID for the next available monster ptr
int16_t monster_multiply_total; // Total number of reproduction's of creatures

// Returns a pointer to next free space -RAK-
// Returns -1 if could not allocate a monster.
static int popm() {
    if (next_free_monster_id == MON_TOTAL_ALLOCATIONS) {
        if (!compactMonsters()) {
            return -1;
        }
    }
    return next_free_monster_id++;
}

// Places a monster at given location -RAK-
bool monsterPlaceNew(Coord_t coord, int creature_id, bool sleeping) {
    int monster_id = popm();

    // Check for case where could not allocate space for the monster
    if (monster_id == -1) {
        return false;
    }

    Monster_t &monster = monsters[monster_id];

    monster.pos.y = coord.y;
    monster.pos.x = coord.x;
    monster.creature_id = (uint16_t) creature_id;

    if ((creatures_list[creature_id].defenses & config::monsters::defense::CD_MAX_HP) != 0) {
        monster.hp = (int16_t) maxDiceRoll(creatures_list[creature_id].hit_die);
    } else {
        monster.hp = (int16_t) diceRoll(creatures_list[creature_id].hit_die);
    }

    // the creatures_list[] speed value is 10 greater, so that it can be a uint8_t
    monster.speed = (int16_t)(creatures_list[creature_id].speed - 10 + py.flags.speed);
    monster.stunned_amount = 0;
    monster.distance_from_player = (uint8_t) coordDistanceBetween(py.pos, coord);
    monster.lit = false;

    dg.floor[coord.y][coord.x].creature_id = (uint8_t) monster_id;

    if (sleeping) {
        if (creatures_list[creature_id].sleep_counter == 0) {
            monster.sleep_count = 0;
        } else {
            monster.sleep_count = (int16_t)((creatures_list[creature_id].sleep_counter * 2) + randomNumber((int) creatures_list[creature_id].sleep_counter * 10));
        }
    } else {
        monster.sleep_count = 0;
    }

    return true;
}

// Places a monster at given location -RAK-
void monsterPlaceWinning() {
    if (game.total_winner) {
        return;
    }

    Coord_t coord = Coord_t{0, 0};

    do {
        coord.y = randomNumber(dg.height - 2);
        coord.x = randomNumber(dg.width - 2);
    } while (dg.floor[coord.y][coord.x].feature_id >= MIN_CLOSED_SPACE ||           //
             dg.floor[coord.y][coord.x].creature_id != 0 ||                         //
             dg.floor[coord.y][coord.x].treasure_id != 0 ||                         //
             coordDistanceBetween(coord, py.pos) <= config::monsters::MON_MAX_SIGHT //
    );

    int creature_id = randomNumber(config::monsters::MON_ENDGAME_MONSTERS) - 1 + monster_levels[MON_MAX_LEVELS];

    // TODO: duplicate code -MRC-
    // The following code is now exactly the same as monsterPlaceNew() except here
    // we `abort()` on failed placement, and do not set `monster->lit = false`.
    // Perhaps we can find a way to call `monsterPlaceNew()` instead of
    // duplicating everything here.

    int monster_id = popm();

    // Check for case where could not allocate space for the win monster, this should never happen.
    if (monster_id == -1) {
        abort();
    }

    Monster_t &monster = monsters[monster_id];

    monster.pos.y = coord.y;
    monster.pos.x = coord.x;
    monster.creature_id = (uint16_t) creature_id;

    if ((creatures_list[creature_id].defenses & config::monsters::defense::CD_MAX_HP) != 0) {
        monster.hp = (int16_t) maxDiceRoll(creatures_list[creature_id].hit_die);
    } else {
        monster.hp = (int16_t) diceRoll(creatures_list[creature_id].hit_die);
    }

    // the creatures_list speed value is 10 greater, so that it can be a uint8_t
    monster.speed = (int16_t)(creatures_list[creature_id].speed - 10 + py.flags.speed);
    monster.stunned_amount = 0;
    monster.distance_from_player = (uint8_t) coordDistanceBetween(py.pos, coord);

    dg.floor[coord.y][coord.x].creature_id = (uint8_t) monster_id;

    monster.sleep_count = 0;
}

// Return a monster suitable to be placed at a given level. This
// makes high level monsters (up to the given level) slightly more
// common than low level monsters at any given level. -CJS-
static int monsterGetOneSuitableForLevel(int level) {
    if (level == 0) {
        return randomNumber(monster_levels[0]) - 1;
    }

    if (level > MON_MAX_LEVELS) {
        level = MON_MAX_LEVELS;
    }

    if (randomNumber(config::monsters::MON_CHANCE_OF_NASTY) == 1) {
        auto abs_distribution = (int) std::abs((std::intmax_t) randomNumberNormalDistribution(0, 4));
        level += abs_distribution + 1;
        if (level > MON_MAX_LEVELS) {
            level = MON_MAX_LEVELS;
        }
    } else {
        // This code has been added to make it slightly more likely to get
        // the higher level monsters. Originally a uniform distribution over
        // all monsters of level less than or equal to the dungeon level.
        // This distribution makes a level n monster occur approx 2/n% of the
        // time on level n, and 1/n*n% are 1st level.
        int num = monster_levels[level] - monster_levels[0];
        int i = randomNumber(num) - 1;
        int j = randomNumber(num) - 1;
        if (j > i) {
            i = j;
        }
        level = creatures_list[i + monster_levels[0]].level;
    }

    return randomNumber(monster_levels[level] - monster_levels[level - 1]) - 1 + monster_levels[level - 1];
}

// Allocates a random monster -RAK-
void monsterPlaceNewWithinDistance(int number, int distance_from_source, bool sleeping) {
    Coord_t position = Coord_t{0, 0};

    for (int i = 0; i < number; i++) {
        do {
            position.y = randomNumber(dg.height - 2);
            position.x = randomNumber(dg.width - 2);
        } while (dg.floor[position.y][position.x].feature_id >= MIN_CLOSED_SPACE || //
                 dg.floor[position.y][position.x].creature_id != 0 ||               //
                 coordDistanceBetween(position, py.pos) <= distance_from_source     //
        );

        int l = monsterGetOneSuitableForLevel(dg.current_level);

        // Dragons are always created sleeping here,
        // so as to give the player a sporting chance.
        if (creatures_list[l].sprite == 'd' || creatures_list[l].sprite == 'D') {
            sleeping = true;
        }

        // Place_monster() should always return true here.
        // It does not matter if it fails though.
        (void) monsterPlaceNew(position, l, sleeping);
    }
}

static bool placeMonsterAdjacentTo(int monster_id, Coord_t &coord, bool slp) {
    bool placed = false;

    Coord_t position = Coord_t{0, 0};

    for (int i = 0; i <= 9; i++) {
        position.y = coord.y - 2 + randomNumber(3);
        position.x = coord.x - 2 + randomNumber(3);

        if (coordInBounds(position)) {
            if (dg.floor[position.y][position.x].feature_id <= MAX_OPEN_SPACE && dg.floor[position.y][position.x].creature_id == 0) {
                // Place_monster() should always return true here.
                if (!monsterPlaceNew(position, monster_id, slp)) {
                    return false;
                }

                coord.y = position.y;
                coord.x = position.x;

                placed = true;
                i = 9;
            }
        }
    }

    return placed;
}

// Places creature adjacent to given location -RAK-
bool monsterSummon(Coord_t &coord, bool sleeping) {
    int monster_id = monsterGetOneSuitableForLevel(dg.current_level + config::monsters::MON_SUMMONED_LEVEL_ADJUST);
    return placeMonsterAdjacentTo(monster_id, coord, sleeping);
}

// Places undead adjacent to given location -RAK-
bool monsterSummonUndead(Coord_t &coord) {
    int monster_id;
    int max_levels = monster_levels[MON_MAX_LEVELS];

    do {
        monster_id = randomNumber(max_levels) - 1;
        for (int i = 0; i <= 19;) {
            if ((creatures_list[monster_id].defenses & config::monsters::defense::CD_UNDEAD) != 0) {
                i = 20;
                max_levels = 0;
            } else {
                monster_id++;
                if (monster_id > max_levels) {
                    i = 20;
                } else {
                    i++;
                }
            }
        }
    } while (max_levels != 0);

    return placeMonsterAdjacentTo(monster_id, coord, false);
}

// Compact monsters -RAK-
// Return true if any monsters were deleted, false if could not delete any monsters.
bool compactMonsters() {
    printMessage("Compacting monsters...");

    int cur_dis = 66;
    bool delete_any = false;

    while (!delete_any) {
        for (int i = next_free_monster_id - 1; i >= config::monsters::MON_MIN_INDEX_ID; i--) {
            if (cur_dis < monsters[i].distance_from_player && randomNumber(3) == 1) {
                if ((creatures_list[monsters[i].creature_id].movement & config::monsters::move::CM_WIN) != 0u) {
                    // Never compact away the Balrog!!
                } else if (hack_monptr < i) {
                    // In case this is called from within updateMonsters().
                    // This is a horrible hack, the monsters/updateMonsters() code needs to be rewritten.
                    dungeonDeleteMonster(i);
                    delete_any = true;
                } else {
                    // dungeonRemoveMonsterFromLevel() does not decrement next_free_monster_id,
                    // so don't set delete_any if this was called.
                    dungeonRemoveMonsterFromLevel(i);
                }
            }
        }

        if (!delete_any) {
            cur_dis -= 6;

            // Can't delete any monsters, return failure.
            if (cur_dis < 0) {
                return false;
            }
        }
    }

    return true;
}
