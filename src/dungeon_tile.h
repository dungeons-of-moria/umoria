// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

#pragma once

// Tile_t holds data about a specific tile in the dungeon.
typedef struct {
    uint8_t creature_id; // ID for any creature occupying the tile
    uint8_t treasure_id; // ID for any treasure item occupying the tile
    uint8_t feature_id;  // ID of cave feature; walls, floors, open space, etc.

    bool perma_lit_room : 1;  // Room should be lit with perm light, walls with this set should be perm lit after tunneled out.
    bool field_mark : 1;      // Field mark, used for traps/doors/stairs, object is hidden if fm is false.
    bool permanent_light : 1; // Permanent light, used for walls and lighted rooms.
    bool temporary_light : 1; // Temporary light, used for player's lamp light,etc.
} Tile_t;

// `fval` definitions: these describe the various types of dungeon floors and
// walls, if numbers above 15 are ever used, then the test against MIN_CAVE_WALL
// will have to be changed, also the save routines will have to be changed.
constexpr uint8_t TILE_NULL_WALL = 0;
constexpr uint8_t TILE_DARK_FLOOR = 1;
constexpr uint8_t TILE_LIGHT_FLOOR = 2;
constexpr uint8_t MAX_CAVE_ROOM = 2;
constexpr uint8_t TILE_CORR_FLOOR = 3;
constexpr uint8_t TILE_BLOCKED_FLOOR = 4; // a corridor space with cl/st/se door or rubble
constexpr uint8_t MAX_CAVE_FLOOR = 4;

constexpr uint8_t MAX_OPEN_SPACE = 3;
constexpr uint8_t MIN_CLOSED_SPACE = 4;

constexpr uint8_t TMP1_WALL = 8;
constexpr uint8_t TMP2_WALL = 9;

constexpr uint8_t MIN_CAVE_WALL = 12;
constexpr uint8_t TILE_GRANITE_WALL = 12;
constexpr uint8_t TILE_MAGMA_WALL = 13;
constexpr uint8_t TILE_QUARTZ_WALL = 14;
constexpr uint8_t TILE_BOUNDARY_WALL = 15;
