// Copyright (c) 1989-94 James E. Wilson, Robert A. Koeneke
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

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

void saveHighScore(const HighScore_t &score);
void readHighScore(HighScore_t &score);

void recordNewHighScore();
void showScoresScreen();
uint8_t highScoreGenderLabel();
int32_t playerCalculateTotalPoints();
