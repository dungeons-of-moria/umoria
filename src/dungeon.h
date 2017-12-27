// Copyright (c) 1989-94 James E. Wilson, Robert A. Koeneke
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

#pragma once

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

// Tile_t holds data about a specific tile in the dungeon.
typedef struct {
    uint8_t creature_id; // ID for any creature occupying the tile
    uint8_t treasure_id; // ID for any treasure item occupying the tile
    uint8_t feature_id;  // ID of cave feature; walls, floors, open space, etc.

    bool perma_lit_room  : 1; // Room should be lit with perm light, walls with this set should be perm lit after tunneled out.
    bool field_mark      : 1; // Field mark, used for traps/doors/stairs, object is hidden if fm is false.
    bool permanent_light : 1; // Permanent light, used for walls and lighted rooms.
    bool temporary_light : 1; // Temporary light, used for player's lamp light,etc.
} Tile_t;

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

void dungeonDisplayMap();

bool coordInBounds(Coord_t coord);
int coordDistanceBetween(Coord_t coord_a, Coord_t coord_b);
int coordWallsNextTo(Coord_t coord);
int coordCorridorWallsNextTo(Coord_t coord);
char caveGetTileSymbol(Coord_t coord);
bool caveTileVisible(Coord_t coord);

void dungeonSetTrap(int y, int x, int sub_type_id);
void trapChangeVisibility(int y, int x);

void dungeonPlaceRubble(int y, int x);
void dungeonPlaceGold(int y, int x);

void dungeonPlaceRandomObjectAt(int y, int x, bool must_be_small);
void dungeonAllocateAndPlaceObject(bool (*set_function)(int), int object_type, int number);
void dungeonPlaceRandomObjectNear(int y, int x, int tries);

void dungeonMoveCreatureRecord(int y1, int x1, int y2, int x2);
void dungeonLightRoom(int pos_y, int pos_x);
void dungeonLiteSpot(int y, int x);
void dungeonMoveCharacterLight(int y1, int x1, int y2, int x2);

void dungeonDeleteMonster(int id);
void dungeonDeleteMonsterFix1(int id);
void dungeonDeleteMonsterFix2(int id);
int dungeonSummonObject(int y, int x, int amount, int object_type);
bool dungeonDeleteObject(int y, int x);

// generate the dungeon
void generateCave();
