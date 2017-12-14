// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// The main command interpreter, updating player status

#include "headers.h"
#include "externs.h"

// The Dungeon global
// Yup, this initialization is ugly, we'll fix...eventually! -MRC-
Dungeon_t dg = Dungeon_t{0, 0, {}, -1, 0, true, {}};

// dungeonDisplayMap shrinks the dungeon to a single screen
void dungeonDisplayMap() {
    // Save the game screen
    terminalSaveScreen();
    clearScreen();

    int16_t priority[256] = {0};
    priority[60] = 5;    // char '<'
    priority[62] = 5;    // char '>'
    priority[64] = 10;   // char '@'
    priority[35] = -5;   // char '#'
    priority[46] = -10;  // char '.'
    priority[92] = -3;   // char '\'
    priority[32] = -15;  // char ' '

    // Display highest priority object in the RATIO, by RATIO area
    constexpr uint8_t RATIO = 3;
    uint8_t panel_width = MAX_WIDTH / RATIO;
    uint8_t panel_height = MAX_HEIGHT / RATIO;

    char map[MAX_WIDTH / RATIO + 1] = {'\0'};
    char line_buffer[80];

    // Add screen border
    addChar('+', Coord_t{0, 0});
    addChar('+', Coord_t{0, panel_width + 1});
    for (uint8_t i = 0; i < panel_width; i++) {
        addChar('-', Coord_t{0, i + 1});
        addChar('-', Coord_t{panel_height + 1, i + 1});
    }
    for (uint8_t i = 0; i < panel_height; i++) {
        addChar('|', Coord_t{i + 1, 0});
        addChar('|', Coord_t{i + 1, panel_width + 1});
    }
    addChar('+', Coord_t{panel_height + 1, 0});
    addChar('+', Coord_t{panel_height + 1, panel_width + 1});
    putString("Hit any key to continue", Coord_t{23, 23});

    int player_y = 0;
    int player_x = 0;
    int line = -1;

    // Shrink the dungeon!
    for (uint8_t y = 0; y < MAX_HEIGHT; y++) {
        uint8_t row = y / RATIO;
        if (row != line) {
            if (line >= 0) {
                sprintf(line_buffer, "|%s|", map);
                putString(line_buffer, Coord_t{line + 1, 0});
            }
            for (uint8_t j = 0; j < panel_width; j++) {
                map[j] = ' ';
            }
            line = row;
        }

        for (uint8_t x = 0; x < MAX_WIDTH; x++) {
            uint8_t col = x / RATIO;
            char cave_char = caveGetTileSymbol(Coord_t{y, x});
            if (priority[(uint8_t) map[col]] < priority[(uint8_t) cave_char]) {
                map[col] = cave_char;
            }
            if (map[col] == '@') {
                // +1 to account for border
                player_x = col + 1;
                player_y = row + 1;
            }
        }
    }

    if (line >= 0) {
        sprintf(line_buffer, "|%s|", map);
        putString(line_buffer, Coord_t{line + 1, 0});
    }

    // Move cursor onto player character
    moveCursor(Coord_t{player_y, player_x});

    // wait for any keypress
    (void) getKeyInput();

    // restore the game screen
    terminalRestoreScreen();
}
