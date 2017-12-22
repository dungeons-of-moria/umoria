// Copyright (c) 1989-94 James E. Wilson, Robert A. Koeneke
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

#pragma once

bool coordOutsidePanel(Coord_t coord, bool force);
bool coordInsidePanel(Coord_t coord);
void drawDungeonPanel();
void drawCavePanel();
void dungeonResetView();
