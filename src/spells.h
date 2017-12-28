// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

#pragma once

// Spell_t is a base data object.
// Holds the base game data for a spell
// Note: the names for the spells are stored in spell_names[] array at index i, +31 if priest
typedef struct {
    uint8_t level_required;
    uint8_t mana_required;
    uint8_t failure_chance;
    uint8_t exp_gain_for_learning; // 1/4 of exp gained for learning spell
} Spell_t;

extern Spell_t magic_spells[PLAYER_MAX_CLASSES - 1][31];
extern char *spell_names[62];

int castSpellGetId(const char *prompt, int item_id, int &spell_id, int &spell_chance);

bool spellDetectTreasureWithinVicinity();
bool spellDetectObjectsWithinVicinity();
bool spellDetectTrapsWithinVicinity();
bool spellDetectSecretDoorssWithinVicinity();
bool spellDetectInvisibleCreaturesWithinVicinity();
bool spellLightArea(int y, int x);
bool spellDarkenArea(int y, int x);
void spellMapCurrentArea();
bool spellIdentifyItem();
bool spellAggravateMonsters(int affect_distance);
bool spellSurroundPlayerWithTraps();
bool spellSurroundPlayerWithDoors();
bool spellDestroyAdjacentDoorsTraps();
bool spellDetectMonsters();
void spellLightLine(int x, int y, int direction);
void spellStarlite(int y, int x);
bool spellDisarmAllInDirection(int y, int x, int direction);
void spellFireBolt(int y, int x, int direction, int damage_hp, int spell_type, const std::string &spell_name);
void spellFireBall(int y, int x, int direction, int damage_hp, int spell_type, const std::string &spell_name);
void spellBreath(int y, int x, int monster_id, int damage_hp, int spell_type, const std::string &spell_name);
bool spellRechargeItem(int number_of_charges);
bool spellChangeMonsterHitPoints(int y, int x, int direction, int damage_hp);
bool spellDrainLifeFromMonster(int y, int x, int direction);
bool spellSpeedMonster(int y, int x, int direction, int speed);
bool spellConfuseMonster(int y, int x, int direction);
bool spellSleepMonster(int y, int x, int direction);
bool spellWallToMud(int y, int x, int direction);
bool spellDestroyDoorsTrapsInDirection(int y, int x, int direction);
bool spellPolymorphMonster(int y, int x, int direction);
bool spellBuildWall(int y, int x, int direction);
bool spellCloneMonster(int y, int x, int direction);
void spellTeleportAwayMonster(int monster_id, int distance_from_player);
void spellTeleportPlayerTo(int y, int x);
bool spellTeleportAwayMonsterInDirection(int y, int x, int direction);
bool spellMassGenocide();
bool spellGenocide();
bool spellSpeedAllMonsters(int speed);
bool spellSleepAllMonsters();
bool spellMassPolymorph();
bool spellDetectEvil();
bool spellChangePlayerHitPoints(int adjustment);
void spellEarthquake();
void spellCreateFood();
bool spellDispelCreature(int creature_defense, int damage);
bool spellTurnUndead();
void spellWardingGlyph();
void spellLoseSTR();
void spellLoseINT();
void spellLoseWIS();
void spellLoseDEX();
void spellLoseCON();
void spellLoseCHR();
void spellLoseEXP(int32_t adjustment);
bool spellSlowPoison();
void spellDestroyArea(int y, int x);
bool spellEnchantItem(int16_t &plusses, int16_t max_bonus_limit);
bool spellRemoveCurseFromAllItems();
bool spellRestorePlayerLevels();
