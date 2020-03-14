// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// Custom types for working with strings

// maximum unsigned char: 255
constexpr uint8_t MAX_UCHAR = std::numeric_limits<uint8_t>::max();
// maximum short signed int: 32767
constexpr int16_t MAX_SHORT = std::numeric_limits<int16_t>::max();
// maximum long signed int: 0x7FFFFFFFL
constexpr int32_t MAX_LONG = std::numeric_limits<int32_t>::max();

// This used to be NULL, but that was technically incorrect.
// CNIL is used instead of null to help avoid lint errors.
constexpr char *CNIL = 0;

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
