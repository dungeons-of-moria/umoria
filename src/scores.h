// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

#pragma once

#include <cstdint>

// HighScore_t is a score object used for saving to the high score file
// This structure is 64 bytes in size
typedef struct {
    int32_t points;
    int32_t birth_date;
    int16_t uid;
    int16_t mhp;
    int16_t chp;
    uint8_t dungeon_depth;
    uint8_t level;
    uint8_t deepest_dungeon_depth;
    uint8_t gender;
    uint8_t race;
    uint8_t character_class;
    char name[PLAYER_NAME_SIZE];
    char died_from[25];
} HighScore_t;

// Number of entries allowed in the score file.
constexpr uint16_t MAX_HIGH_SCORE_ENTRIES = 1000;

extern FILE *highscore_fp;

// TODO: these are implemented in `game_save.cpp` so need moving.
void saveHighScore(HighScore_t const &score);
void readHighScore(HighScore_t &score);

void recordNewHighScore();
void showScoresScreen();
int32_t playerCalculateTotalPoints();
