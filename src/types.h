// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Custom types for working with strings

// Many of the character fields used to be fixed length, which
// greatly increased the size of the executable. Many fixed
// length fields have been replaced with variable length ones.
typedef char vtype_t[MORIA_MESSAGE_SIZE];

// Note: since its output can easily exceed 80 characters,
// an object description must always be called with an
// obj_desc_t type as the first parameter.
typedef char obj_desc_t[OBJECT_DESCRIPTION_SIZE];
