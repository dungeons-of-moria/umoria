// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

#pragma once

// Dungeon size parameters
constexpr uint8_t RATIO = 3;       // Size ratio of the Map screen
constexpr uint8_t MAX_HEIGHT = 66; // Multiple of 11; >= 22
constexpr uint8_t MAX_WIDTH = 198; // Multiple of 33; >= 66
constexpr uint8_t SCREEN_HEIGHT = 22;
constexpr uint8_t SCREEN_WIDTH = 66;
constexpr uint8_t QUART_HEIGHT = (SCREEN_HEIGHT / 4);
constexpr uint8_t QUART_WIDTH = (SCREEN_WIDTH / 4);

// DungeonObject_t is a base data object.
// This holds data for any non-living object in the game such as
// stairs, rubble, doors, gold, potions, weapons, wands, etc.
typedef struct {
    const char *name;          // Object name
    uint32_t flags;            // Special flags
    uint8_t category_id;       // Category number (tval)
    uint8_t sprite;            // Character representation - ASCII symbol (tchar)
    int16_t misc_use;          // Misc. use variable (p1)
    int32_t cost;              // Cost of item
    uint8_t sub_category_id;   // Sub-category number (subval)
    uint8_t items_count;       // Number of items
    uint16_t weight;           // Weight
    int16_t to_hit;            // Plusses to hit
    int16_t to_damage;         // Plusses to damage
    int16_t ac;                // Normal AC
    int16_t to_ac;             // Plusses to AC
    Dice_t damage;             // Damage when hits
    uint8_t depth_first_found; // Dungeon level item first found
} DungeonObject_t;

typedef struct {
    // Dungeon size is either just big enough for town level, or the whole dungeon itself
    int16_t height;
    int16_t width;

    Panel_t panel;

    // Current turn of the game
    int32_t game_turn;

    // The current dungeon level
    int16_t current_level;

    // A `true` value means a new level will be generated on next loop iteration
    bool generate_new_level;

    // Floor definitions
    Tile_t floor[MAX_HEIGHT][MAX_WIDTH];
} Dungeon_t;

extern Dungeon_t dg;
extern DungeonObject_t game_objects[MAX_OBJECTS_IN_GAME];

void dungeonDisplayMap();

bool coordInBounds(Coord_t const &coord);
int coordDistanceBetween(Coord_t const &from, Coord_t const &to);
int coordWallsNextTo(Coord_t const &coord);
int coordCorridorWallsNextTo(Coord_t const &coord);
char caveGetTileSymbol(Coord_t const &coord);
bool caveTileVisible(Coord_t const &coord);

void dungeonSetTrap(Coord_t const &coord, int sub_type_id);
void trapChangeVisibility(Coord_t const &coord);

void dungeonPlaceRubble(Coord_t const &coord);
void dungeonPlaceGold(Coord_t const &coord);

void dungeonPlaceRandomObjectAt(Coord_t const &coord, bool must_be_small);
void dungeonAllocateAndPlaceObject(bool (*set_function)(int), int object_type, int number);
void dungeonPlaceRandomObjectNear(Coord_t coord, int tries);

void dungeonMoveCreatureRecord(Coord_t const &from, Coord_t const &to);
void dungeonLightRoom(Coord_t const &coord);
void dungeonLiteSpot(Coord_t const &coord);
void dungeonMoveCharacterLight(Coord_t const &from, Coord_t const &to);

void dungeonDeleteMonster(int id);
void dungeonRemoveMonsterFromLevel(int id);
void dungeonDeleteMonsterRecord(int id);
int dungeonSummonObject(Coord_t coord, int amount, int object_type);
bool dungeonDeleteObject(Coord_t const &coord);

// generate the dungeon
void generateCave();

// Line of Sight
bool los(Coord_t from, Coord_t to);
void look();
