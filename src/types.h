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
typedef char obj_desc_t[160];

typedef struct {
    int y;
    int x;
} Coord_t;


// Added color support    SAC
// color string type
constexpr uint8_t MAX_COLOR_MSG_PAIRS = 10;
struct color_posit_t {
    int pos = 0;
    int color = config::colors::COL_DEFAULT;
};

// multi-color string type
// Option: could have full string, and array of index/color pairs that would point to
//  different sections of the string that should be different colors. 
//    Pro: less memory, no repeated string contents
//    Con: 
struct multicolor_msg_t {
    obj_desc_t str = {'\0'};
    color_posit_t pairs[MAX_COLOR_MSG_PAIRS];
};
