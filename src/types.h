// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// Custom types for working with strings

// Many of the character fields used to be fixed length, which
// greatly increased the size of the executable. Many fixed
// length fields have been replaced with variable length ones.
typedef char vtype_t[MORIA_MESSAGE_SIZE];

// Note: since its output can easily exceed 80 characters,
// an object description must always be called with an
// obj_desc_t type as the first parameter.
typedef char obj_desc_t[OBJECT_DESCRIPTION_SIZE];
