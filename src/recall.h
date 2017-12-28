// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

#pragma once

// Recall_t holds the player's known knowledge for any given monster, aka memories
typedef struct {
    uint32_t movement;
    uint32_t spells;
    uint16_t kills;
    uint16_t deaths;
    uint16_t defenses;
    uint8_t wake;
    uint8_t ignore;
    uint8_t attacks[MON_MAX_ATTACKS];
} Recall_t;

int memoryRecall(int monster_id);
void recallMonsterAttributes(char command);
