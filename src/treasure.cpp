// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Dungeon treasure object definitions

#include "headers.h"

int16_t sorted_objects[MAX_DUNGEON_OBJECTS];

// Identified objects flags
uint8_t objects_identified[OBJECT_IDENT_SIZE];
int16_t treasure_levels[TREASURE_MAX_LEVELS + 1];
Inventory_t treasure_list[LEVEL_MAX_OBJECTS];
Inventory_t inventory[PLAYER_INVENTORY_SIZE];

// Treasure related values
int16_t current_treasure_id;   // Current treasure heap ptr
