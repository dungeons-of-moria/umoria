// Copyright (c) 1989-94 James E. Wilson, Robert A. Koeneke
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Cave_t holds data about a specific tile in the dungeon.
typedef struct {
    uint8_t creature_id; // ID for any creature occupying the tile
    uint8_t treasure_id; // ID for any treasure item occupying the tile
    uint8_t feature_id;  // ID of cave feature; walls, floors, open space, etc.

    bool perma_lit_room  : 1; // Room should be lit with perm light, walls with this set should be perm lit after tunneled out.
    bool field_mark      : 1; // Field mark, used for traps/doors/stairs, object is hidden if fm is false.
    bool permanent_light : 1; // Permanent light, used for walls and lighted rooms.
    bool temporary_light : 1; // Temporary light, used for player's lamp light,etc.
} Cave_t;

typedef struct {
    // Dungeon size is either big enough for town level, or the dungeon itself
    int16_t dungeon_height;
    int16_t dungeon_width;

    // Screen panels calculated from the dungeon/screen dimensions
    int16_t max_panel_rows;
    int16_t max_panel_cols;

    int panel_row;
    int panel_col;
    int panel_row_min;
    int panel_row_max;
    int panel_col_min;
    int panel_col_max;
    int panel_col_prt;
    int panel_row_prt;

    // Floor definitions
    Cave_t cave[MAX_HEIGHT][MAX_WIDTH];

    int16_t current_dungeon_level; // Current dungeon level
    int32_t current_game_turn;     // Current turn of game
    bool generate_new_level;       // True will generate a new level on next loop iteration
} Dungeon_t;
