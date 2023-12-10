// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// SPDX-License-Identifier: GPL-3.0-or-later

// This used to be NULL, but that was technically incorrect.
// CNIL is used instead of null to help avoid lint errors.
constexpr char *CNIL = nullptr;

// Many of the character fields used to be fixed length, which
// greatly increased the size of the executable. Many fixed
// length fields have been replaced with variable length ones.
constexpr uint8_t MORIA_MESSAGE_SIZE = 80;
typedef char vtype_t[MORIA_MESSAGE_SIZE];

// Note: since its output can easily exceed 80 characters,
// an object description must always be called with an
// obj_desc_t type as the first parameter.
constexpr uint8_t MORIA_OBJ_DESC_SIZE = 160;
typedef char obj_desc_t[MORIA_OBJ_DESC_SIZE];

typedef struct {
    int y;
    int x;
} Coord_t;
