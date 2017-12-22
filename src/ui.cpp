// Copyright (c) 1989-94 James E. Wilson, Robert A. Koeneke
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

#include "headers.h"
#include "externs.h"

// Calculates current boundaries -RAK-
static void panelBounds() {
    dg.panel.top = dg.panel.row * (SCREEN_HEIGHT / 2);
    dg.panel.bottom = dg.panel.top + SCREEN_HEIGHT - 1;
    dg.panel.row_prt = dg.panel.top - 1;
    dg.panel.left = dg.panel.col * (SCREEN_WIDTH / 2);
    dg.panel.right = dg.panel.left + SCREEN_WIDTH - 1;
    dg.panel.col_prt = dg.panel.left - 13;
}

// Given an row (y) and col (x), this routine detects -RAK-
// when a move off the screen has occurred and figures new borders.
// `force` forces the panel bounds to be recalculated, useful for 'W'here.
bool coordOutsidePanel(Coord_t coord, bool force) {
    int row = dg.panel.row;
    int col = dg.panel.col;

    if (force || coord.y < dg.panel.top + 2 || coord.y > dg.panel.bottom - 2) {
        row = (coord.y - SCREEN_HEIGHT / 4) / (SCREEN_HEIGHT / 2);

        if (row > dg.panel.max_rows) {
            row = dg.panel.max_rows;
        } else if (row < 0) {
            row = 0;
        }
    }

    if (force || coord.x < dg.panel.left + 3 || coord.x > dg.panel.right - 3) {
        col = ((coord.x - SCREEN_WIDTH / 4) / (SCREEN_WIDTH / 2));
        if (col > dg.panel.max_cols) {
            col = dg.panel.max_cols;
        } else if (col < 0) {
            col = 0;
        }
    }

    if (row != dg.panel.row || col != dg.panel.col) {
        dg.panel.row = row;
        dg.panel.col = col;
        panelBounds();

        // stop movement if any
        if (config.find_bound) {
            playerEndRunning();
        }

        // Yes, the coordinates are beyond the current panel boundary
        return true;
    }

    return false;
}

// Is the given coordinate within the screen panel boundaries -RAK-
bool coordInsidePanel(Coord_t coord) {
    bool valid_y = coord.y >= dg.panel.top && coord.y <= dg.panel.bottom;
    bool valid_x = coord.x >= dg.panel.left && coord.x <= dg.panel.right;

    return valid_y && valid_x;
}

// Prints the map of the dungeon -RAK-
void drawDungeonPanel() {
    int line = 1;

    // Top to bottom
    for (int y = dg.panel.top; y <= dg.panel.bottom; y++) {
        eraseLine(Coord_t{line++, 13});

        // Left to right
        for (int x = dg.panel.left; x <= dg.panel.right; x++) {
            char ch = caveGetTileSymbol(Coord_t{y, x});
            if (ch != ' ') {
                panelPutTile(ch, Coord_t{y, x});
            }
        }
    }
}

// Draws entire screen -RAK-
void drawCavePanel() {
    clearScreen();
    printCharacterStatsBlock();
    drawDungeonPanel();
    printCharacterCurrentDepth();
}

// We need to reset the view of things. -CJS-
void dungeonResetView() {
    const Tile_t &tile = dg.floor[py.row][py.col];

    // Check for new panel
    if (coordOutsidePanel(Coord_t{py.row, py.col}, false)) {
        drawDungeonPanel();
    }

    // Move the light source
    dungeonMoveCharacterLight(py.row, py.col, py.row, py.col);

    // A room of light should be lit.
    if (tile.feature_id == TILE_LIGHT_FLOOR) {
        if (py.flags.blind < 1 && !tile.permanent_light) {
            dungeonLightRoom(py.row, py.col);
        }
        return;
    }

    // In doorway of light-room?
    if (tile.perma_lit_room && py.flags.blind < 1) {
        for (int i = py.row - 1; i <= py.row + 1; i++) {
            for (int j = py.col - 1; j <= py.col + 1; j++) {
                if (dg.floor[i][j].feature_id == TILE_LIGHT_FLOOR && !dg.floor[i][j].permanent_light) {
                    dungeonLightRoom(i, j);
                }
            }
        }
    }
}
