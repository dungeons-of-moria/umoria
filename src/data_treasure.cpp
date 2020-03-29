// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// Treasure data

// clang-format off
#include "headers.h"

// Object description:  Objects are defined here.  Each object has
// the following attributes:
//
//  Descriptor : Name of item and formats.
//                 & is replaced with 'a', 'an', or a number.
//                 ~ is replaced with null or 's'.
//  Character  : Character that represents the item.
//  Type value : Value representing the type of object.
//  Sub value  : separate value for each item of a type.
//                   0 - 63:  object can not stack
//                  64 - 127: dungeon object, can stack with other D object
//                 128 - 191: unused, previously for store items
//                 192:       stack with other iff have same `misc_use` value, always
//                            treated as individual objects
//                 193 - 255: object can stack with others iff they have
//                            the same `misc_use` value, usually considered one group
//                 Objects which have two type values, e.g. potions and
//                 scrolls, need to have distinct `sub_category_id`s for
//                 each item regardless of its category_id
//  Damage     : amount of damage item can cause.
//  Weight     : relative weight of an item.
//  Number     : number of items appearing in group.
//  To hit     : magical plusses to hit.
//  To damage  : magical plusses to damage.
//  AC         : objects relative armor class.
//                 1 is worse than 5 is worse than 10 etc.
//  To AC      : Magical bonuses to AC.
//  misc_use   : Catch all for magical abilities such as
//               plusses to strength, minuses to searching.
//  Flags      : Abilities of object.  Each ability is a
//               bit.  Bits 1-31 are used. (Signed integer)
//  Level      : Minimum level on which item can be found.
//  Cost       : Relative cost of item.
//
//  Special Abilities can be added to item by magicInitializeItemNames(),
//  found in misc.c.
//
//  Scrolls, Potions, and Food:
//  Flags is used to define a function which reading/quaffing
//  will cause.  Most scrolls and potions have only one bit
//  set.  Potions will generally have some food value, found
//  in `misc_use`.
//
//  Wands and Staffs:
//  Flags defines a function, `misc_use` contains number of charges
//  for item.  `misc_use` is set in magicInitializeItemNames() in misc.c.
//
//  Chests:
//  Traps are added randomly by magicInitializeItemNames() in misc.c.

// Object list (All objects must be defined here)

// Dungeon items from 0 to MAX_DUNGEON_OBJECTS
DungeonObject_t game_objects[MAX_OBJECTS_IN_GAME] = {
    {"Poison",                          0x00000001L, TV_FOOD,        ',', 500,  0,    64,  1, 1,    0,  0, 0,   0, {0, 0}, 7}, // 0
    {"Blindness",                       0x00000002L, TV_FOOD,        ',', 500,  0,    65,  1, 1,    0,  0, 0,   0, {0, 0}, 9}, // 1
    {"Paranoia",                        0x00000004L, TV_FOOD,        ',', 500,  0,    66,  1, 1,    0,  0, 0,   0, {0, 0}, 9}, // 2
    {"Confusion",                       0x00000008L, TV_FOOD,        ',', 500,  0,    67,  1, 1,    0,  0, 0,   0, {0, 0}, 7}, // 3
    {"Hallucination",                   0x00000010L, TV_FOOD,        ',', 500,  0,    68,  1, 1,    0,  0, 0,   0, {0, 0}, 13}, // 4
    {"Cure Poison",                     0x00000020L, TV_FOOD,        ',', 500,  60,   69,  1, 1,    0,  0, 0,   0, {0, 0}, 8}, // 5
    {"Cure Blindness",                  0x00000040L, TV_FOOD,        ',', 500,  50,   70,  1, 1,    0,  0, 0,   0, {0, 0}, 10}, // 6
    {"Cure Paranoia",                   0x00000080L, TV_FOOD,        ',', 500,  25,   71,  1, 1,    0,  0, 0,   0, {0, 0}, 12}, // 7
    {"Cure Confusion",                  0x00000100L, TV_FOOD,        ',', 500,  50,   72,  1, 1,    0,  0, 0,   0, {0, 0}, 6}, // 8
    {"Weakness",                        0x04000200L, TV_FOOD,        ',', 500,  0,    73,  1, 1,    0,  0, 0,   0, {0, 0}, 7}, // 9
    {"Unhealth",                        0x04000400L, TV_FOOD,        ',', 500,  50,   74,  1, 1,    0,  0, 0,   0, {10, 10}, 15}, // 10
    {"Restore Constitution",            0x00010000L, TV_FOOD,        ',', 500,  350,  75,  1, 1,    0,  0, 0,   0, {0, 0}, 20}, // 11
    {"First-Aid",                       0x00200000L, TV_FOOD,        ',', 500,  5,    76,  1, 1,    0,  0, 0,   0, {0, 0}, 6}, // 12
    {"Minor Cures",                     0x00400000L, TV_FOOD,        ',', 500,  20,   77,  1, 1,    0,  0, 0,   0, {0, 0}, 7}, // 13
    {"Light Cures",                     0x00800000L, TV_FOOD,        ',', 500,  30,   78,  1, 1,    0,  0, 0,   0, {0, 0}, 10}, // 14
    {"Restoration",                     0x001F8000L, TV_FOOD,        ',', 500,  1000, 79,  1, 1,    0,  0, 0,   0, {0, 0}, 30}, // 15
    {"Poison",                          0x00000001L, TV_FOOD,        ',', 1200, 0,    80,  1, 1,    0,  0, 0,   0, {0, 0}, 15}, // 16
    {"Hallucination",                   0x00000010L, TV_FOOD,        ',', 1200, 0,    81,  1, 1,    0,  0, 0,   0, {0, 0}, 18}, // 17
    {"Cure Poison",                     0x00000020L, TV_FOOD,        ',', 1200, 75,   82,  1, 1,    0,  0, 0,   0, {0, 0}, 19}, // 18
    {"Unhealth",                        0x04000400L, TV_FOOD,        ',', 1200, 75,   83,  1, 1,    0,  0, 0,   0, {10, 12}, 28}, // 19
    {"Major Cures",                     0x02000000L, TV_FOOD,        ',', 1200, 75,   84,  1, 2,    0,  0, 0,   0, {0, 0}, 16}, // 20
    {"& Ration~ of Food",               0x00000000L, TV_FOOD,        ',', 5000, 3,    90,  1, 10,   0,  0, 0,   0, {0, 0}, 0}, // 21
    {"& Ration~ of Food",               0x00000000L, TV_FOOD,        ',', 5000, 3,    90,  1, 10,   0,  0, 0,   0, {0, 0}, 5}, // 22
    {"& Ration~ of Food",               0x00000000L, TV_FOOD,        ',', 5000, 3,    90,  1, 10,   0,  0, 0,   0, {0, 0}, 10}, // 23
    {"& Slime Mold~",                   0x00000000L, TV_FOOD,        ',', 3000, 2,    91,  1, 5,    0,  0, 0,   0, {0, 0}, 1}, // 24
    {"& Piece~ of Elvish Waybread",     0x02000020L, TV_FOOD,        ',', 7500, 25,   92,  1, 3,    0,  0, 0,   0, {0, 0}, 6}, // 25
    {"& Piece~ of Elvish Waybread",     0x02000020L, TV_FOOD,        ',', 7500, 25,   92,  1, 3,    0,  0, 0,   0, {0, 0}, 12}, // 26
    {"& Piece~ of Elvish Waybread",     0x02000020L, TV_FOOD,        ',', 7500, 25,   92,  1, 3,    0,  0, 0,   0, {0, 0}, 20}, // 27

    {"& Dagger (Main Gauche)",          0x00000000L, TV_SWORD,       '|', 0,    25,   1,   1, 30,   0,  0, 0,   0, {1, 5}, 2}, // 28
    {"& Dagger (Misericorde)",          0x00000000L, TV_SWORD,       '|', 0,    10,   2,   1, 15,   0,  0, 0,   0, {1, 4}, 0}, // 29
    {"& Dagger (Stiletto)",             0x00000000L, TV_SWORD,       '|', 0,    10,   3,   1, 12,   0,  0, 0,   0, {1, 4}, 0}, // 30
    {"& Dagger (Bodkin)",               0x00000000L, TV_SWORD,       '|', 0,    10,   4,   1, 20,   0,  0, 0,   0, {1, 4}, 1}, // 31
    {"& Broken Dagger",                 0x00000000L, TV_SWORD,       '|', 0,    0,    5,   1, 15,  -2, -2, 0,   0, {1, 1}, 0}, // 32
    {"& Backsword",                     0x00000000L, TV_SWORD,       '|', 0,    150,  6,   1, 95,   0,  0, 0,   0, {1, 9}, 7}, // 33
    {"& Bastard Sword",                 0x00000000L, TV_SWORD,       '|', 0,    350,  7,   1, 140,  0,  0, 0,   0, {3, 4}, 14}, // 34
    {"& Thrusting Sword (Bilbo)",       0x00000000L, TV_SWORD,       '|', 0,    60,   8,   1, 80,   0,  0, 0,   0, {1, 6}, 4}, // 35
    {"& Thrusting Sword (Baselard)",    0x00000000L, TV_SWORD,       '|', 0,    80,   9,   1, 100,  0,  0, 0,   0, {1, 7}, 5}, // 36
    {"& Broadsword",                    0x00000000L, TV_SWORD,       '|', 0,    255,  10,  1, 150,  0,  0, 0,   0, {2, 5}, 9}, // 37
    {"& Two-Handed Sword (Claymore)",   0x00000000L, TV_SWORD,       '|', 0,    775,  11,  1, 200,  0,  0, 0,   0, {3, 6}, 30}, // 38
    {"& Cutlass",                       0x00000000L, TV_SWORD,       '|', 0,    85,   12,  1, 110,  0,  0, 0,   0, {1, 7}, 7}, // 39
    {"& Two-Handed Sword (Espadon)",    0x00000000L, TV_SWORD,       '|', 0,    655,  13,  1, 180,  0,  0, 0,   0, {3, 6}, 35}, // 40
    {"& Executioner's Sword",           0x00000000L, TV_SWORD,       '|', 0,    850,  14,  1, 260,  0,  0, 0,   0, {4, 5}, 40}, // 41
    {"& Two-Handed Sword (Flamberge)",  0x00000000L, TV_SWORD,       '|', 0,    1000, 15,  1, 240,  0,  0, 0,   0, {4, 5}, 45}, // 42
    {"& Foil",                          0x00000000L, TV_SWORD,       '|', 0,    35,   16,  1, 30,   0,  0, 0,   0, {1, 5}, 2}, // 43
    {"& Katana",                        0x00000000L, TV_SWORD,       '|', 0,    400,  17,  1, 120,  0,  0, 0,   0, {3, 4}, 18}, // 44
    {"& Longsword",                     0x00000000L, TV_SWORD,       '|', 0,    200,  18,  1, 130,  0,  0, 0,   0, {1, 10}, 12}, // 45
    {"& Two-Handed Sword (No-Dachi)",   0x00000000L, TV_SWORD,       '|', 0,    675,  19,  1, 200,  0,  0, 0,   0, {4, 4}, 45}, // 46
    {"& Rapier",                        0x00000000L, TV_SWORD,       '|', 0,    42,   20,  1, 40,   0,  0, 0,   0, {1, 6}, 4}, // 47
    {"& Sabre",                         0x00000000L, TV_SWORD,       '|', 0,    50,   21,  1, 50,   0,  0, 0,   0, {1, 7}, 5}, // 48
    {"& Small Sword",                   0x00000000L, TV_SWORD,       '|', 0,    48,   22,  1, 75,   0,  0, 0,   0, {1, 6}, 5}, // 49
    {"& Two-Handed Sword (Zweihander)", 0x00000000L, TV_SWORD,       '|', 0,    1500, 23,  1, 280,  0,  0, 0,   0, {4, 6}, 50}, // 50
    {"& Broken Sword",                  0x00000000L, TV_SWORD,       '|', 0,    0,    24,  1, 75,  -2, -2, 0,   0, {1, 1}, 0}, // 51
    {"& Ball and Chain",                0x00000000L, TV_HAFTED,     '\\', 0,    200,  1,   1, 150,  0,  0, 0,   0, {2, 4}, 20}, // 52
    {"& Cat-o'-Nine-Tails",             0x00000000L, TV_HAFTED,     '\\', 0,    14,   2,   1, 40,   0,  0, 0,   0, {1, 4}, 3}, // 53
    {"& Wooden Club",                   0x00000000L, TV_HAFTED,     '\\', 0,    10,   3,   1, 100,  0,  0, 0,   0, {1, 3}, 0}, // 54
    {"& Flail",                         0x00000000L, TV_HAFTED,     '\\', 0,    353,  4,   1, 150,  0,  0, 0,   0, {2, 6}, 12}, // 55
    {"& Two-Handed Great Flail",        0x00000000L, TV_HAFTED,     '\\', 0,    590,  5,   1, 280,  0,  0, 0,   0, {3, 6}, 45}, // 56
    {"& Morningstar",                   0x00000000L, TV_HAFTED,     '\\', 0,    396,  6,   1, 150,  0,  0, 0,   0, {2, 6}, 10}, // 57
    {"& Mace",                          0x00000000L, TV_HAFTED,     '\\', 0,    130,  7,   1, 120,  0,  0, 0,   0, {2, 4}, 6}, // 58
    {"& War Hammer",                    0x00000000L, TV_HAFTED,     '\\', 0,    225,  8,   1, 120,  0,  0, 0,   0, {3, 3}, 5}, // 59
    {"& Lead-Filled Mace",              0x00000000L, TV_HAFTED,     '\\', 0,    502,  9,   1, 180,  0,  0, 0,   0, {3, 4}, 15}, // 60
    {"& Awl-Pike",                      0x00000000L, TV_POLEARM,     '/', 0,    200,  1,   1, 160,  0,  0, 0,   0, {1, 8}, 8}, // 61
    {"& Beaked Axe",                    0x00000000L, TV_POLEARM,     '/', 0,    408,  2,   1, 180,  0,  0, 0,   0, {2, 6}, 15}, // 62
    {"& Fauchard",                      0x00000000L, TV_POLEARM,     '/', 0,    326,  3,   1, 170,  0,  0, 0,   0, {1, 10}, 17}, // 63
    {"& Glaive",                        0x00000000L, TV_POLEARM,     '/', 0,    363,  4,   1, 190,  0,  0, 0,   0, {2, 6}, 20}, // 64
    {"& Halberd",                       0x00000000L, TV_POLEARM,     '/', 0,    430,  5,   1, 190,  0,  0, 0,   0, {3, 4}, 22}, // 65
    {"& Lucerne Hammer",                0x00000000L, TV_POLEARM,     '/', 0,    376,  6,   1, 120,  0,  0, 0,   0, {2, 5}, 11}, // 66
    {"& Pike",                          0x00000000L, TV_POLEARM,     '/', 0,    358,  7,   1, 160,  0,  0, 0,   0, {2, 5}, 15}, // 67
    {"& Spear",                         0x00000000L, TV_POLEARM,     '/', 0,    36,   8,   1, 50,   0,  0, 0,   0, {1, 6}, 5}, // 68
    {"& Lance",                         0x00000000L, TV_POLEARM,     '/', 0,    230,  9,   1, 300,  0,  0, 0,   0, {2, 8}, 10}, // 69
    {"& Javelin",                       0x00000000L, TV_POLEARM,     '/', 0,    18,   10,  1, 30,   0,  0, 0,   0, {1, 4}, 4}, // 70
    {"& Battle Axe (Balestarius)",      0x00000000L, TV_POLEARM,     '/', 0,    500,  11,  1, 180,  0,  0, 0,   0, {2, 8}, 30}, // 71
    {"& Battle Axe (European)",         0x00000000L, TV_POLEARM,     '/', 0,    334,  12,  1, 170,  0,  0, 0,   0, {3, 4}, 13}, // 72
    {"& Broad Axe",                     0x00000000L, TV_POLEARM,     '/', 0,    304,  13,  1, 160,  0,  0, 0,   0, {2, 6}, 17}, // 73
    {"& Short Bow",                     0x00000000L, TV_BOW,         '}', 2,    50,   1,   1, 30,   0,  0, 0,   0, {0, 0}, 3}, // 74
    {"& Long Bow",                      0x00000000L, TV_BOW,         '}', 3,    120,  2,   1, 40,   0,  0, 0,   0, {0, 0}, 10}, // 75
    {"& Composite Bow",                 0x00000000L, TV_BOW,         '}', 4,    240,  3,   1, 40,   0,  0, 0,   0, {0, 0}, 40}, // 76
    {"& Light Crossbow",                0x00000000L, TV_BOW,         '}', 5,    140,  10,  1, 110,  0,  0, 0,   0, {0, 0}, 15}, // 77
    {"& Heavy Crossbow",                0x00000000L, TV_BOW,         '}', 6,    300,  11,  1, 200,  0,  0, 0,   0, {1, 1}, 30}, // 78
    {"& Sling",                         0x00000000L, TV_BOW,         '}', 1,    5,    20,  1, 5,    0,  0, 0,   0, {0, 0}, 1}, // 79
    {"& Arrow~",                        0x00000000L, TV_ARROW,       '{', 0,    1,    193, 1, 2,    0,  0, 0,   0, {1, 4}, 2}, // 80
    {"& Bolt~",                         0x00000000L, TV_BOLT,        '{', 0,    2,    193, 1, 3,    0,  0, 0,   0, {1, 5}, 2}, // 81
    {"& Rounded Pebble~",               0x00000000L, TV_SLING_AMMO,  '{', 0,    1,    193, 1, 4,    0,  0, 0,   0, {1, 2}, 0}, // 82
    {"& Iron Shot~",                    0x00000000L, TV_SLING_AMMO,  '{', 0,    2,    194, 1, 5,    0,  0, 0,   0, {1, 3}, 3}, // 83
    {"& Iron Spike~",                   0x00000000L, TV_SPIKE,       '~', 0,    1,    193, 1, 10,   0,  0, 0,   0, {1, 1}, 1}, // 84
    {"& Brass Lantern~",                0x00000000L, TV_LIGHT,       '~', 7500, 35,   1,   1, 50,   0,  0, 0,   0, {1, 1}, 1}, // 85
    {"& Wooden Torch~",                 0x00000000L, TV_LIGHT,       '~', 4000, 2,    193, 1, 30,   0,  0, 0,   0, {1, 1}, 1}, // 86
    {"& Orcish Pick",                   0x20000000L, TV_DIGGING,    '\\', 2,    500,  2,   1, 180,  0,  0, 0,   0, {1, 3}, 20}, // 87
    {"& Dwarven Pick",                  0x20000000L, TV_DIGGING,    '\\', 3,    1200, 3,   1, 200,  0,  0, 0,   0, {1, 4}, 50}, // 88
    {"& Gnomish Shovel",                0x20000000L, TV_DIGGING,    '\\', 1,    100,  5,   1, 50,   0,  0, 0,   0, {1, 2}, 20}, // 89
    {"& Dwarven Shovel",                0x20000000L, TV_DIGGING,    '\\', 2,    250,  6,   1, 120,  0,  0, 0,   0, {1, 3}, 40}, // 90
    {"& Pair of Soft Leather Shoes",    0x00000000L, TV_BOOTS,       ']', 0,    4,    1,   1, 5,    0,  0, 1,   0, {0, 0}, 1}, // 91
    {"& Pair of Soft Leather Boots",    0x00000000L, TV_BOOTS,       ']', 0,    7,    2,   1, 20,   0,  0, 2,   0, {1, 1}, 4}, // 92
    {"& Pair of Hard Leather Boots",    0x00000000L, TV_BOOTS,       ']', 0,    12,   3,   1, 40,   0,  0, 3,   0, {1, 1}, 6}, // 93
    {"& Soft Leather Cap",              0x00000000L, TV_HELM,        ']', 0,    4,    1,   1, 10,   0,  0, 1,   0, {0, 0}, 2}, // 94
    {"& Hard Leather Cap",              0x00000000L, TV_HELM,        ']', 0,    12,   2,   1, 15,   0,  0, 2,   0, {0, 0}, 4}, // 95
    {"& Metal Cap",                     0x00000000L, TV_HELM,        ']', 0,    30,   3,   1, 20,   0,  0, 3,   0, {1, 1}, 7}, // 96
    {"& Iron Helm",                     0x00000000L, TV_HELM,        ']', 0,    75,   4,   1, 75,   0,  0, 5,   0, {1, 3}, 20}, // 97
    {"& Steel Helm",                    0x00000000L, TV_HELM,        ']', 0,    200,  5,   1, 60,   0,  0, 6,   0, {1, 3}, 40}, // 98
    {"& Silver Crown",                  0x00000000L, TV_HELM,        ']', 0,    500,  6,   1, 20,   0,  0, 0,   0, {1, 1}, 44}, // 99
    {"& Golden Crown",                  0x00000000L, TV_HELM,        ']', 0,    1000, 7,   1, 30,   0,  0, 0,   0, {1, 2}, 47}, // 100
    {"& Jewel-Encrusted Crown",         0x00000000L, TV_HELM,        ']', 0,    2000, 8,   1, 40,   0,  0, 0,   0, {1, 3}, 50}, // 101
    {"& Robe",                          0x00000000L, TV_SOFT_ARMOR,  '(', 0,    4,    1,   1, 20,   0,  0, 2,   0, {0, 0}, 1}, // 102
    {"Soft Leather Armor",              0x00000000L, TV_SOFT_ARMOR,  '(', 0,    18,   2,   1, 80,   0,  0, 4,   0, {0, 0}, 2}, // 103
    {"Soft Studded Leather",            0x00000000L, TV_SOFT_ARMOR,  '(', 0,    35,   3,   1, 90,   0,  0, 5,   0, {1, 1}, 3}, // 104
    {"Hard Leather Armor",              0x00000000L, TV_SOFT_ARMOR,  '(', 0,    55,   4,   1, 100, -1,  0, 6,   0, {1, 1}, 5}, // 105
    {"Hard Studded Leather",            0x00000000L, TV_SOFT_ARMOR,  '(', 0,    100,  5,   1, 110, -1,  0, 7,   0, {1, 2}, 7}, // 106
    {"Woven Cord Armor",                0x00000000L, TV_SOFT_ARMOR,  '(', 0,    45,   6,   1, 150, -1,  0, 6,   0, {0, 0}, 7}, // 107
    {"Soft Leather Ring Mail",          0x00000000L, TV_SOFT_ARMOR,  '(', 0,    160,  7,   1, 130, -1,  0, 6,   0, {1, 2}, 10}, // 108
    {"Hard Leather Ring Mail",          0x00000000L, TV_SOFT_ARMOR,  '(', 0,    230,  8,   1, 150, -2,  0, 8,   0, {1, 3}, 12}, // 109
    {"Leather Scale Mail",              0x00000000L, TV_SOFT_ARMOR,  '(', 0,    330,  9,   1, 140, -1,  0, 11,  0, {1, 1}, 14}, // 110
    {"Metal Scale Mail",                0x00000000L, TV_HARD_ARMOR,  '[', 0,    430,  1,   1, 250, -2,  0, 13,  0, {1, 4}, 24}, // 111
    {"Chain Mail",                      0x00000000L, TV_HARD_ARMOR,  '[', 0,    530,  2,   1, 220, -2,  0, 14,  0, {1, 4}, 26}, // 112
    {"Rusty Chain Mail",                0x00000000L, TV_HARD_ARMOR,  '[', 0,    0,    3,   1, 220, -5,  0, 14, -8, {1, 4}, 26}, // 113
    {"Double Chain Mail",               0x00000000L, TV_HARD_ARMOR,  '[', 0,    630,  4,   1, 260, -2,  0, 15,  0, {1, 4}, 28}, // 114
    {"Augmented Chain Mail",            0x00000000L, TV_HARD_ARMOR,  '[', 0,    675,  5,   1, 270, -2,  0, 16,  0, {1, 4}, 30}, // 115
    {"Bar Chain Mail",                  0x00000000L, TV_HARD_ARMOR,  '[', 0,    720,  6,   1, 280, -2,  0, 18,  0, {1, 4}, 34}, // 116
    {"Metal Brigandine Armor",          0x00000000L, TV_HARD_ARMOR,  '[', 0,    775,  7,   1, 290, -3,  0, 19,  0, {1, 4}, 36}, // 117
    {"Laminated Armor",                 0x00000000L, TV_HARD_ARMOR,  '[', 0,    825,  8,   1, 300, -3,  0, 20,  0, {1, 4}, 38}, // 118
    {"Partial Plate Armor",             0x00000000L, TV_HARD_ARMOR,  '[', 0,    900,  9,   1, 320, -3,  0, 22,  0, {1, 6}, 42}, // 119
    {"Metal Lamellar Armor",            0x00000000L, TV_HARD_ARMOR,  '[', 0,    950,  10,  1, 340, -3,  0, 23,  0, {1, 6}, 44}, // 120
    {"Full Plate Armor",                0x00000000L, TV_HARD_ARMOR,  '[', 0,    1050, 11,  1, 380, -3,  0, 25,  0, {2, 4}, 48}, // 121
    {"Ribbed Plate Armor",              0x00000000L, TV_HARD_ARMOR,  '[', 0,    1200, 12,  1, 380, -3,  0, 28,  0, {2, 4}, 50}, // 122
    {"& Cloak",                         0x00000000L, TV_CLOAK,       '(', 0,    3,    1,   1, 10,   0,  0, 1,   0, {0, 0}, 1}, // 123
    {"& Set of Leather Gloves",         0x00000000L, TV_GLOVES,      ']', 0,    3,    1,   1, 5,    0,  0, 1,   0, {0, 0}, 1}, // 124
    {"& Set of Gauntlets",              0x00000000L, TV_GLOVES,      ']', 0,    35,   2,   1, 25,   0,  0, 2,   0, {1, 1}, 12}, // 125
    {"& Small Leather Shield",          0x00000000L, TV_SHIELD,      ')', 0,    30,   1,   1, 50,   0,  0, 2,   0, {1, 1}, 3}, // 126
    {"& Medium Leather Shield",         0x00000000L, TV_SHIELD,      ')', 0,    60,   2,   1, 75,   0,  0, 3,   0, {1, 2}, 8}, // 127
    {"& Large Leather Shield",          0x00000000L, TV_SHIELD,      ')', 0,    120,  3,   1, 100,  0,  0, 4,   0, {1, 2}, 15}, // 128
    {"& Small Metal Shield",            0x00000000L, TV_SHIELD,      ')', 0,    50,   4,   1, 65,   0,  0, 3,   0, {1, 2}, 10}, // 129
    {"& Medium Metal Shield",           0x00000000L, TV_SHIELD,      ')', 0,    125,  5,   1, 90,   0,  0, 4,   0, {1, 3}, 20}, // 130
    {"& Large Metal Shield",            0x00000000L, TV_SHIELD,      ')', 0,    200,  6,   1, 120,  0,  0, 5,   0, {1, 3}, 30}, // 131
    {"Strength",                        0x00000001L, TV_RING,        '=', 0,    400,  0,   1, 2,    0,  0, 0,   0, {0, 0}, 30}, // 132
    {"Dexterity",                       0x00000008L, TV_RING,        '=', 0,    400,  1,   1, 2,    0,  0, 0,   0, {0, 0}, 30}, // 133
    {"Constitution",                    0x00000010L, TV_RING,        '=', 0,    400,  2,   1, 2,    0,  0, 0,   0, {0, 0}, 30}, // 134
    {"Intelligence",                    0x00000002L, TV_RING,        '=', 0,    400,  3,   1, 2,    0,  0, 0,   0, {0, 0}, 30}, // 135
    {"Speed",                           0x00001000L, TV_RING,        '=', 0,    3000, 4,   1, 2,    0,  0, 0,   0, {0, 0}, 50}, // 136
    {"Searching",                       0x00000040L, TV_RING,        '=', 0,    250,  5,   1, 2,    0,  0, 0,   0, {0, 0}, 7}, // 137
    {"Teleportation",                   0x80000400L, TV_RING,        '=', 0,    0,    6,   1, 2,    0,  0, 0,   0, {0, 0}, 7}, // 138
    {"Slow Digestion",                  0x00000080L, TV_RING,        '=', 0,    200,  7,   1, 2,    0,  0, 0,   0, {0, 0}, 7}, // 139
    {"Resist Fire",                     0x00080000L, TV_RING,        '=', 0,    250,  8,   1, 2,    0,  0, 0,   0, {0, 0}, 14}, // 140
    {"Resist Cold",                     0x00200000L, TV_RING,        '=', 0,    250,  9,   1, 2,    0,  0, 0,   0, {0, 0}, 14}, // 141
    {"Feather Falling",                 0x04000000L, TV_RING,        '=', 0,    200,  10,  1, 2,    0,  0, 0,   0, {0, 0}, 7}, // 142
    {"Adornment",                       0x00000000L, TV_RING,        '=', 0,    20,   11,  1, 2,    0,  0, 0,   0, {0, 0}, 7}, // 143
    // was a ring of adornment, sub_category_id = 12 here
    {"& Arrow~",                        0x00000000L, TV_ARROW,        '{',  0, 1,    193, 1, 2, 0, 0, 0,  0, {1, 4}, 15}, // 144
    {"Weakness",                        0x80000001L, TV_RING,         '=', -5, 0,    13,  1, 2, 0, 0, 0,  0, {0, 0}, 7}, // 145
    {"Lordly Protection (FIRE)",        0x00080000L, TV_RING,         '=',  0, 1200, 14,  1, 2, 0, 0, 0,  5, {0, 0}, 50}, // 146
    {"Lordly Protection (ACID)",        0x00100000L, TV_RING,         '=',  0, 1200, 15,  1, 2, 0, 0, 0,  5, {0, 0}, 50}, // 147
    {"Lordly Protection (COLD)",        0x00200000L, TV_RING,         '=',  0, 1200, 16,  1, 2, 0, 0, 0,  5, {0, 0}, 50}, // 148
    {"WOE",                             0x80000644L, TV_RING,         '=', -5, 0,    17,  1, 2, 0, 0, 0, -3, {0, 0}, 50}, // 149
    {"Stupidity",                       0x80000002L, TV_RING,         '=', -5, 0,    18,  1, 2, 0, 0, 0,  0, {0, 0}, 7}, // 150
    {"Increase Damage",                 0x00000000L, TV_RING,         '=',  0, 100,  19,  1, 2, 0, 0, 0,  0, {0, 0}, 20}, // 151
    {"Increase To-Hit",                 0x00000000L, TV_RING,         '=',  0, 100,  20,  1, 2, 0, 0, 0,  0, {0, 0}, 20}, // 152
    {"Protection",                      0x00000000L, TV_RING,         '=',  0, 100,  21,  1, 2, 0, 0, 0,  0, {0, 0}, 7}, // 153
    {"Aggravate Monster",               0x80000200L, TV_RING,         '=',  0, 0,    22,  1, 2, 0, 0, 0,  0, {0, 0}, 7}, // 154
    {"See Invisible",                   0x01000000L, TV_RING,         '=',  0, 500,  23,  1, 2, 0, 0, 0,  0, {0, 0}, 40}, // 155
    {"Sustain Strength",                0x00400000L, TV_RING,         '=',  1, 750,  24,  1, 2, 0, 0, 0,  0, {0, 0}, 44}, // 156
    {"Sustain Intelligence",            0x00400000L, TV_RING,         '=',  2, 600,  25,  1, 2, 0, 0, 0,  0, {0, 0}, 44}, // 157
    {"Sustain Wisdom",                  0x00400000L, TV_RING,         '=',  3, 600,  26,  1, 2, 0, 0, 0,  0, {0, 0}, 44}, // 158
    {"Sustain Constitution",            0x00400000L, TV_RING,         '=',  4, 750,  27,  1, 2, 0, 0, 0,  0, {0, 0}, 44}, // 159
    {"Sustain Dexterity",               0x00400000L, TV_RING,         '=',  5, 750,  28,  1, 2, 0, 0, 0,  0, {0, 0}, 44}, // 160
    {"Sustain Charisma",                0x00400000L, TV_RING,         '=',  6, 500,  29,  1, 2, 0, 0, 0,  0, {0, 0}, 44}, // 161
    {"Slaying",                         0x00000000L, TV_RING,         '=',  0, 1000, 30,  1, 2, 0, 0, 0,  0, {0, 0}, 50}, // 162
    {"Wisdom",                          0x00000004L, TV_AMULET,       '"',  0, 300,  0,   1, 3, 0, 0, 0,  0, {0, 0}, 20}, // 163
    {"Charisma",                        0x00000020L, TV_AMULET,       '"',  0, 250,  1,   1, 3, 0, 0, 0,  0, {0, 0}, 20}, // 164
    {"Searching",                       0x00000040L, TV_AMULET,       '"',  0, 250,  2,   1, 3, 0, 0, 0,  0, {0, 0}, 14}, // 165
    {"Teleportation",                   0x80000400L, TV_AMULET,       '"',  0, 0,    3,   1, 3, 0, 0, 0,  0, {0, 0}, 14}, // 166
    {"Slow Digestion",                  0x00000080L, TV_AMULET,       '"',  0, 200,  4,   1, 3, 0, 0, 0,  0, {0, 0}, 14}, // 167
    {"Resist Acid",                     0x00100000L, TV_AMULET,       '"',  0, 250,  5,   1, 3, 0, 0, 0,  0, {0, 0}, 24}, // 168
    {"Adornment",                       0x00000000L, TV_AMULET,       '"',  0, 20,   6,   1, 3, 0, 0, 0,  0, {0, 0}, 16}, // 169
    // was an amulet of adornment here, sub_category_id = 7
    {"& Bolt~",                         0x00000000L, TV_BOLT,         '{',  0, 2,    193, 1, 3, 0, 0, 0, 0, {1, 5}, 25}, // 170
    {"the Magi",                        0x01800040L, TV_AMULET,       '"',  0, 5000, 8,   1, 3, 0, 0, 0, 3, {0, 0}, 50}, // 171
    {"DOOM",                            0x8000007FL, TV_AMULET,       '"', -5, 0,    9,   1, 3, 0, 0, 0, 0, {0, 0}, 50}, // 172
    {"Enchant Weapon To-Hit",           0x00000001L, TV_SCROLL1,      '?',  0, 125,  64,  1, 5, 0, 0, 0, 0, {0, 0}, 12}, // 173
    {"Enchant Weapon To-Dam",           0x00000002L, TV_SCROLL1,      '?',  0, 125,  65,  1, 5, 0, 0, 0, 0, {0, 0}, 12}, // 174
    {"Enchant Armor",                   0x00000004L, TV_SCROLL1,      '?',  0, 125,  66,  1, 5, 0, 0, 0, 0, {0, 0}, 12}, // 175
    {"Identify",                        0x00000008L, TV_SCROLL1,      '?',  0, 50,   67,  1, 5, 0, 0, 0, 0, {0, 0}, 1}, // 176
    {"Identify",                        0x00000008L, TV_SCROLL1,      '?',  0, 50,   67,  1, 5, 0, 0, 0, 0, {0, 0}, 5}, // 177
    {"Identify",                        0x00000008L, TV_SCROLL1,      '?',  0, 50,   67,  1, 5, 0, 0, 0, 0, {0, 0}, 10}, // 178
    {"Identify",                        0x00000008L, TV_SCROLL1,      '?',  0, 50,   67,  1, 5, 0, 0, 0, 0, {0, 0}, 30}, // 179
    {"Remove Curse",                    0x00000010L, TV_SCROLL1,      '?',  0, 100,  68,  1, 5, 0, 0, 0, 0, {0, 0}, 7}, // 180
    {"Light",                           0x00000020L, TV_SCROLL1,      '?',  0, 15,   69,  1, 5, 0, 0, 0, 0, {0, 0}, 0}, // 181
    {"Light",                           0x00000020L, TV_SCROLL1,      '?',  0, 15,   69,  1, 5, 0, 0, 0, 0, {0, 0}, 3}, // 182
    {"Light",                           0x00000020L, TV_SCROLL1,      '?',  0, 15,   69,  1, 5, 0, 0, 0, 0, {0, 0}, 7}, // 183
    {"Summon Monster",                  0x00000040L, TV_SCROLL1,      '?',  0, 0,    70,  1, 5, 0, 0, 0, 0, {0, 0}, 1}, // 184
    {"Phase Door",                      0x00000080L, TV_SCROLL1,      '?',  0, 15,   71,  1, 5, 0, 0, 0, 0, {0, 0}, 1}, // 185
    {"Teleport",                        0x00000100L, TV_SCROLL1,      '?',  0, 40,   72,  1, 5, 0, 0, 0, 0, {0, 0}, 10}, // 186
    {"Teleport Level",                  0x00000200L, TV_SCROLL1,      '?',  0, 50,   73,  1, 5, 0, 0, 0, 0, {0, 0}, 20}, // 187
    {"Monster Confusion",               0x00000400L, TV_SCROLL1,      '?',  0, 30,   74,  1, 5, 0, 0, 0, 0, {0, 0}, 5}, // 188
    {"Magic Mapping",                   0x00000800L, TV_SCROLL1,      '?',  0, 40,   75,  1, 5, 0, 0, 0, 0, {0, 0}, 5}, // 189
    {"Sleep Monster",                   0x00001000L, TV_SCROLL1,      '?',  0, 35,   76,  1, 5, 0, 0, 0, 0, {0, 0}, 5}, // 190
    {"Rune of Protection",              0x00002000L, TV_SCROLL1,      '?',  0, 500,  77,  1, 5, 0, 0, 0, 0, {0, 0}, 50}, // 191
    {"Treasure Detection",              0x00004000L, TV_SCROLL1,      '?',  0, 15,   78,  1, 5, 0, 0, 0, 0, {0, 0}, 0}, // 192
    {"Object Detection",                0x00008000L, TV_SCROLL1,      '?',  0, 15,   79,  1, 5, 0, 0, 0, 0, {0, 0}, 0}, // 193
    {"Trap Detection",                  0x00010000L, TV_SCROLL1,      '?',  0, 35,   80,  1, 5, 0, 0, 0, 0, {0, 0}, 5}, // 194
    {"Trap Detection",                  0x00010000L, TV_SCROLL1,      '?',  0, 35,   80,  1, 5, 0, 0, 0, 0, {0, 0}, 8}, // 195
    {"Trap Detection",                  0x00010000L, TV_SCROLL1,      '?',  0, 35,   80,  1, 5, 0, 0, 0, 0, {0, 0}, 12}, // 196
    {"Door/Stair Location",             0x00020000L, TV_SCROLL1,      '?',  0, 35,   81,  1, 5, 0, 0, 0, 0, {0, 0}, 5}, // 197
    {"Door/Stair Location",             0x00020000L, TV_SCROLL1,      '?',  0, 35,   81,  1, 5, 0, 0, 0, 0, {0, 0}, 10}, // 198
    {"Door/Stair Location",             0x00020000L, TV_SCROLL1,      '?',  0, 35,   81,  1, 5, 0, 0, 0, 0, {0, 0}, 15}, // 199
    {"Mass Genocide",                   0x00040000L, TV_SCROLL1,      '?',  0, 1000, 82,  1, 5, 0, 0, 0, 0, {0, 0}, 50}, // 200
    {"Detect Invisible",                0x00080000L, TV_SCROLL1,      '?',  0, 15,   83,  1, 5, 0, 0, 0, 0, {0, 0}, 1}, // 201
    {"Aggravate Monster",               0x00100000L, TV_SCROLL1,      '?',  0, 0,    84,  1, 5, 0, 0, 0, 0, {0, 0}, 5}, // 202
    {"Trap Creation",                   0x00200000L, TV_SCROLL1,      '?',  0, 0,    85,  1, 5, 0, 0, 0, 0, {0, 0}, 12}, // 203
    {"Trap/Door Destruction",           0x00400000L, TV_SCROLL1,      '?',  0, 50,   86,  1, 5, 0, 0, 0, 0, {0, 0}, 12}, // 204
    {"Door Creation",                   0x00800000L, TV_SCROLL1,      '?',  0, 100,  87,  1, 5, 0, 0, 0, 0, {0, 0}, 12}, // 205
    {"Recharging",                      0x01000000L, TV_SCROLL1,      '?',  0, 200,  88,  1, 5, 0, 0, 0, 0, {0, 0}, 40}, // 206
    {"Genocide",                        0x02000000L, TV_SCROLL1,      '?',  0, 750,  89,  1, 5, 0, 0, 0, 0, {0, 0}, 35}, // 207
    {"Darkness",                        0x04000000L, TV_SCROLL1,      '?',  0, 0,    90,  1, 5, 0, 0, 0, 0, {0, 0}, 1}, // 208
    {"Protection from Evil",            0x08000000L, TV_SCROLL1,      '?',  0, 100,  91,  1, 5, 0, 0, 0, 0, {0, 0}, 30}, // 209
    {"Create Food",                     0x10000000L, TV_SCROLL1,      '?',  0, 10,   92,  1, 5, 0, 0, 0, 0, {0, 0}, 5}, // 210
    {"Dispel Undead",                   0x20000000L, TV_SCROLL1,      '?',  0, 200,  93,  1, 5, 0, 0, 0, 0, {0, 0}, 40}, // 211
    {"*Enchant Weapon*",                0x00000001L, TV_SCROLL2,      '?',  0, 500,  94,  1, 5, 0, 0, 0, 0, {0, 0}, 50}, // 212
    {"Curse Weapon",                    0x00000002L, TV_SCROLL2,      '?',  0, 0,    95,  1, 5, 0, 0, 0, 0, {0, 0}, 50}, // 213
    {"*Enchant Armor*",                 0x00000004L, TV_SCROLL2,      '?',  0, 500,  96,  1, 5, 0, 0, 0, 0, {0, 0}, 50}, // 214
    {"Curse Armor",                     0x00000008L, TV_SCROLL2,      '?',  0, 0,    97,  1, 5, 0, 0, 0, 0, {0, 0}, 50}, // 215
    {"Summon Undead",                   0x00000010L, TV_SCROLL2,      '?',  0, 0,    98,  1, 5, 0, 0, 0, 0, {0, 0}, 15}, // 216
    {"Blessing",                        0x00000020L, TV_SCROLL2,      '?',  0, 15,   99,  1, 5, 0, 0, 0, 0, {0, 0}, 1}, // 217
    {"Holy Chant",                      0x00000040L, TV_SCROLL2,      '?',  0, 40,   100, 1, 5, 0, 0, 0, 0, {0, 0}, 12}, // 218
    {"Holy Prayer",                     0x00000080L, TV_SCROLL2,      '?',  0, 80,   101, 1, 5, 0, 0, 0, 0, {0, 0}, 24}, // 219
    {"Word-of-Recall",                  0x00000100L, TV_SCROLL2,      '?',  0, 150,  102, 1, 5, 0, 0, 0, 0, {0, 0}, 5}, // 220
    {"*Destruction*",                   0x00000200L, TV_SCROLL2,      '?',  0, 750,  103, 1, 5, 0, 0, 0, 0, {0, 0}, 40}, // 221
    // SMJ, AJ, Water must be sub_category_id 64-66 resp. for itemDescription to work
    {"Slime Mold Juice",                0x30000000L, TV_POTION1,      '!', 400,  2,    64,  1, 4, 0, 0, 0, 0, {1, 1}, 0}, // 222
    {"Apple Juice",                     0x00000000L, TV_POTION1,      '!', 250,  1,    65,  1, 4, 0, 0, 0, 0, {1, 1}, 0}, // 223
    {"Water",                           0x00000000L, TV_POTION1,      '!', 200,  0,    66,  1, 4, 0, 0, 0, 0, {1, 1}, 0}, // 224
    {"Strength",                        0x00000001L, TV_POTION1,      '!', 50,   300,  67,  1, 4, 0, 0, 0, 0, {1, 1}, 25}, // 225
    {"Weakness",                        0x00000002L, TV_POTION1,      '!', 0,    0,    68,  1, 4, 0, 0, 0, 0, {1, 1}, 3}, // 226
    {"Restore Strength",                0x00000004L, TV_POTION1,      '!', 0,    300,  69,  1, 4, 0, 0, 0, 0, {1, 1}, 40}, // 227
    {"Intelligence",                    0x00000008L, TV_POTION1,      '!', 0,    300,  70,  1, 4, 0, 0, 0, 0, {1, 1}, 25}, // 228
    {"Lose Intelligence",               0x00000010L, TV_POTION1,      '!', 0,    0,    71,  1, 4, 0, 0, 0, 0, {1, 1}, 25}, // 229
    {"Restore Intelligence",            0x00000020L, TV_POTION1,      '!', 0,    300,  72,  1, 4, 0, 0, 0, 0, {1, 1}, 40}, // 230
    {"Wisdom",                          0x00000040L, TV_POTION1,      '!', 0,    300,  73,  1, 4, 0, 0, 0, 0, {1, 1}, 25}, // 231
    {"Lose Wisdom",                     0x00000080L, TV_POTION1,      '!', 0,    0,    74,  1, 4, 0, 0, 0, 0, {1, 1}, 25}, // 232
    {"Restore Wisdom",                  0x00000100L, TV_POTION1,      '!', 0,    300,  75,  1, 4, 0, 0, 0, 0, {1, 1}, 40}, // 233
    {"Charisma",                        0x00000200L, TV_POTION1,      '!', 0,    300,  76,  1, 4, 0, 0, 0, 0, {1, 1}, 25}, // 234
    {"Ugliness",                        0x00000400L, TV_POTION1,      '!', 0,    0,    77,  1, 4, 0, 0, 0, 0, {1, 1}, 25}, // 235
    {"Restore Charisma",                0x00000800L, TV_POTION1,      '!', 0,    300,  78,  1, 4, 0, 0, 0, 0, {1, 1}, 40}, // 236
    {"Cure Light Wounds",               0x10001000L, TV_POTION1,      '!', 50,   15,   79,  1, 4, 0, 0, 0, 0, {1, 1}, 0}, // 237
    {"Cure Light Wounds",               0x10001000L, TV_POTION1,      '!', 50,   15,   79,  1, 4, 0, 0, 0, 0, {1, 1}, 1}, // 238
    {"Cure Light Wounds",               0x10001000L, TV_POTION1,      '!', 50,   15,   79,  1, 4, 0, 0, 0, 0, {1, 1}, 2}, // 239
    {"Cure Serious Wounds",             0x30002000L, TV_POTION1,      '!', 100,  40,   80,  1, 4, 0, 0, 0, 0, {1, 1}, 3}, // 240
    {"Cure Critical Wounds",            0x70004000L, TV_POTION1,      '!', 100,  100,  81,  1, 4, 0, 0, 0, 0, {1, 1}, 5}, // 241
    {"Healing",                         0x70008000L, TV_POTION1,      '!', 200,  200,  82,  1, 4, 0, 0, 0, 0, {1, 1}, 12}, // 242
    {"Constitution",                    0x00010000L, TV_POTION1,      '!', 50,   300,  83,  1, 4, 0, 0, 0, 0, {1, 1}, 25}, // 243
    {"Gain Experience",                 0x00020000L, TV_POTION1,      '!', 0,    2500, 84,  1, 4, 0, 0, 0, 0, {1, 1}, 50}, // 244
    {"Sleep",                           0x00040000L, TV_POTION1,      '!', 100,  0,    85,  1, 4, 0, 0, 0, 0, {1, 1}, 0}, // 245
    {"Blindness",                       0x00080000L, TV_POTION1,      '!', 0,    0,    86,  1, 4, 0, 0, 0, 0, {1, 1}, 0}, // 246
    {"Confusion",                       0x00100000L, TV_POTION1,      '!', 50,   0,    87,  1, 4, 0, 0, 0, 0, {1, 1}, 0}, // 247
    {"Poison",                          0x00200000L, TV_POTION1,      '!', 0,    0,    88,  1, 4, 0, 0, 0, 0, {1, 1}, 3}, // 248
    {"Haste Self",                      0x00400000L, TV_POTION1,      '!', 0,    75,   89,  1, 4, 0, 0, 0, 0, {1, 1}, 1}, // 249
    {"Slowness",                        0x00800000L, TV_POTION1,      '!', 50,   0,    90,  1, 4, 0, 0, 0, 0, {1, 1}, 1}, // 250
    {"Dexterity",                       0x02000000L, TV_POTION1,      '!', 0,    300,  91,  1, 4, 0, 0, 0, 0, {1, 1}, 25}, // 251
    {"Restore Dexterity",               0x04000000L, TV_POTION1,      '!', 0,    300,  92,  1, 4, 0, 0, 0, 0, {1, 1}, 40}, // 252
    {"Restore Constitution",            0x68000000L, TV_POTION1,      '!', 0,    300,  93,  1, 4, 0, 0, 0, 0, {1, 1}, 40}, // 253
    {"Lose Experience",                 0x00000002L, TV_POTION2,      '!', 0,    0,    95,  1, 4, 0, 0, 0, 0, {1, 1}, 10}, // 254
    {"Salt Water",                      0x00000004L, TV_POTION2,      '!', 0,    0,    96,  1, 4, 0, 0, 0, 0, {1, 1}, 0}, // 255
    {"Invulnerability",                 0x00000008L, TV_POTION2,      '!', 0,    1000, 97,  1, 4, 0, 0, 0, 0, {1, 1}, 40}, // 256
    {"Heroism",                         0x00000010L, TV_POTION2,      '!', 0,    35,   98,  1, 4, 0, 0, 0, 0, {1, 1}, 1}, // 257
    {"Super Heroism",                   0x00000020L, TV_POTION2,      '!', 0,    100,  99,  1, 4, 0, 0, 0, 0, {1, 1}, 3}, // 258
    {"Boldness",                        0x00000040L, TV_POTION2,      '!', 0,    10,   100, 1, 4, 0, 0, 0, 0, {1, 1}, 1}, // 259
    {"Restore Life Levels",             0x00000080L, TV_POTION2,      '!', 0,    400,  101, 1, 4, 0, 0, 0, 0, {1, 1}, 40}, // 260
    {"Resist Heat",                     0x00000100L, TV_POTION2,      '!', 0,    30,   102, 1, 4, 0, 0, 0, 0, {1, 1}, 1}, // 261
    {"Resist Cold",                     0x00000200L, TV_POTION2,      '!', 0,    30,   103, 1, 4, 0, 0, 0, 0, {1, 1}, 1}, // 262
    {"Detect Invisible",                0x00000400L, TV_POTION2,      '!', 0,    50,   104, 1, 4, 0, 0, 0, 0, {1, 1}, 3}, // 263
    {"Slow Poison",                     0x00000800L, TV_POTION2,      '!', 0,    25,   105, 1, 4, 0, 0, 0, 0, {1, 1}, 1}, // 264
    {"Neutralize Poison",               0x00001000L, TV_POTION2,      '!', 0,    75,   106, 1, 4, 0, 0, 0, 0, {1, 1}, 5}, // 265
    {"Restore Mana",                    0x00002000L, TV_POTION2,      '!', 0,    350,  107, 1, 4, 0, 0, 0, 0, {1, 1}, 25}, // 266
    {"Infra-Vision",                    0x00004000L, TV_POTION2,      '!', 0,    20,   108, 1, 4, 0, 0, 0, 0, {1, 1}, 3}, // 267
    {"& Flask~ of Oil",                 0x00040000L, TV_FLASK,        '!', 7500, 3,    64,  1, 10, 0, 0, 0, 0, {2, 6}, 1}, // 268
    {"Light",                           0x00000001L, TV_WAND,         '-', 0,    200,  0,   1, 10, 0, 0, 0, 0, {1, 1}, 2}, // 269
    {"Lightning Bolts",                 0x00000002L, TV_WAND,         '-', 0,    600,  1,   1, 10, 0, 0, 0, 0, {1, 1}, 15}, // 270
    {"Frost Bolts",                     0x00000004L, TV_WAND,         '-', 0,    800,  2,   1, 10, 0, 0, 0, 0, {1, 1}, 20}, // 271
    {"Fire Bolts",                      0x00000008L, TV_WAND,         '-', 0,    1000, 3,   1, 10, 0, 0, 0, 0, {1, 1}, 30}, // 272
    {"Stone-to-Mud",                    0x00000010L, TV_WAND,         '-', 0,    300,  4,   1, 10, 0, 0, 0, 0, {1, 1}, 12}, // 273
    {"Polymorph",                       0x00000020L, TV_WAND,         '-', 0,    400,  5,   1, 10, 0, 0, 0, 0, {1, 1}, 20}, // 274
    {"Heal Monster",                    0x00000040L, TV_WAND,         '-', 0,    0,    6,   1, 10, 0, 0, 0, 0, {1, 1}, 2}, // 275
    {"Haste Monster",                   0x00000080L, TV_WAND,         '-', 0,    0,    7,   1, 10, 0, 0, 0, 0, {1, 1}, 2}, // 276
    {"Slow Monster",                    0x00000100L, TV_WAND,         '-', 0,    500,  8,   1, 10, 0, 0, 0, 0, {1, 1}, 2}, // 277
    {"Confuse Monster",                 0x00000200L, TV_WAND,         '-', 0,    400,  9,   1, 10, 0, 0, 0, 0, {1, 1}, 2}, // 278
    {"Sleep Monster",                   0x00000400L, TV_WAND,         '-', 0,    500,  10,  1, 10, 0, 0, 0, 0, {1, 1}, 7}, // 279
    {"Drain Life",                      0x00000800L, TV_WAND,         '-', 0,    1200, 11,  1, 10, 0, 0, 0, 0, {1, 1}, 50}, // 280
    {"Trap/Door Destruction",           0x00001000L, TV_WAND,         '-', 0,    500,  12,  1, 10, 0, 0, 0, 0, {1, 1}, 12}, // 281
    {"Magic Missile",                   0x00002000L, TV_WAND,         '-', 0,    200,  13,  1, 10, 0, 0, 0, 0, {1, 1}, 2}, // 282
    {"Wall Building",                   0x00004000L, TV_WAND,         '-', 0,    400,  14,  1, 10, 0, 0, 0, 0, {1, 1}, 25}, // 283
    {"Clone Monster",                   0x00008000L, TV_WAND,         '-', 0,    0,    15,  1, 10, 0, 0, 0, 0, {1, 1}, 15}, // 284
    {"Teleport Away",                   0x00010000L, TV_WAND,         '-', 0,    350,  16,  1, 10, 0, 0, 0, 0, {1, 1}, 20}, // 285
    {"Disarming",                       0x00020000L, TV_WAND,         '-', 0,    500,  17,  1, 10, 0, 0, 0, 0, {1, 1}, 20}, // 286
    {"Lightning Balls",                 0x00040000L, TV_WAND,         '-', 0,    1200, 18,  1, 10, 0, 0, 0, 0, {1, 1}, 35}, // 287
    {"Cold Balls",                      0x00080000L, TV_WAND,         '-', 0,    1500, 19,  1, 10, 0, 0, 0, 0, {1, 1}, 40}, // 288
    {"Fire Balls",                      0x00100000L, TV_WAND,         '-', 0,    1800, 20,  1, 10, 0, 0, 0, 0, {1, 1}, 50}, // 289
    {"Stinking Cloud",                  0x00200000L, TV_WAND,         '-', 0,    400,  21,  1, 10, 0, 0, 0, 0, {1, 1}, 5}, // 290
    {"Acid Balls",                      0x00400000L, TV_WAND,         '-', 0,    1650, 22,  1, 10, 0, 0, 0, 0, {1, 1}, 48}, // 291
    {"Wonder",                          0x00800000L, TV_WAND,         '-', 0,    250,  23,  1, 10, 0, 0, 0, 0, {1, 1}, 2}, // 292
    {"Light",                           0x00000001L, TV_STAFF,        '_', 0,    250,  0,   1, 50, 0, 0, 0, 0, {1, 2}, 5}, // 293
    {"Door/Stair Location",             0x00000002L, TV_STAFF,        '_', 0,    350,  1,   1, 50, 0, 0, 0, 0, {1, 2}, 10}, // 294
    {"Trap Location",                   0x00000004L, TV_STAFF,        '_', 0,    350,  2,   1, 50, 0, 0, 0, 0, {1, 2}, 10}, // 295
    {"Treasure Location",               0x00000008L, TV_STAFF,        '_', 0,    200,  3,   1, 50, 0, 0, 0, 0, {1, 2}, 5}, // 296
    {"Object Location",                 0x00000010L, TV_STAFF,        '_', 0,    200,  4,   1, 50, 0, 0, 0, 0, {1, 2}, 5}, // 297
    {"Teleportation",                   0x00000020L, TV_STAFF,        '_', 0,    800,  5,   1, 50, 0, 0, 0, 0, {1, 2}, 20}, // 298
    {"Earthquakes",                     0x00000040L, TV_STAFF,        '_', 0,    350,  6,   1, 50, 0, 0, 0, 0, {1, 2}, 40}, // 299
    {"Summoning",                       0x00000080L, TV_STAFF,        '_', 0,    0,    7,   1, 50, 0, 0, 0, 0, {1, 2}, 10}, // 300
    {"Summoning",                       0x00000080L, TV_STAFF,        '_', 0,    0,    7,   1, 50, 0, 0, 0, 0, {1, 2}, 50}, // 301
    {"*Destruction*",                   0x00000200L, TV_STAFF,        '_', 0,    2500, 8,   1, 50, 0, 0, 0, 0, {1, 2}, 50}, // 302
    {"Starlight",                       0x00000400L, TV_STAFF,        '_', 0,    400,  9,   1, 50, 0, 0, 0, 0, {1, 2}, 20}, // 303
    {"Haste Monsters",                  0x00000800L, TV_STAFF,        '_', 0,    0,    10,  1, 50, 0, 0, 0, 0, {1, 2}, 10}, // 304
    {"Slow Monsters",                   0x00001000L, TV_STAFF,        '_', 0,    800,  11,  1, 50, 0, 0, 0, 0, {1, 2}, 10}, // 305
    {"Sleep Monsters",                  0x00002000L, TV_STAFF,        '_', 0,    700,  12,  1, 50, 0, 0, 0, 0, {1, 2}, 10}, // 306
    {"Cure Light Wounds",               0x00004000L, TV_STAFF,        '_', 0,    200,  13,  1, 50, 0, 0, 0, 0, {1, 2}, 5}, // 307
    {"Detect Invisible",                0x00008000L, TV_STAFF,        '_', 0,    200,  14,  1, 50, 0, 0, 0, 0, {1, 2}, 5}, // 308
    {"Speed",                           0x00010000L, TV_STAFF,        '_', 0,    1000, 15,  1, 50, 0, 0, 0, 0, {1, 2}, 40}, // 309
    {"Slowness",                        0x00020000L, TV_STAFF,        '_', 0,    0,    16,  1, 50, 0, 0, 0, 0, {1, 2}, 40}, // 310
    {"Mass Polymorph",                  0x00040000L, TV_STAFF,        '_', 0,    750,  17,  1, 50, 0, 0, 0, 0, {1, 2}, 46}, // 311
    {"Remove Curse",                    0x00080000L, TV_STAFF,        '_', 0,    500,  18,  1, 50, 0, 0, 0, 0, {1, 2}, 47}, // 312
    {"Detect Evil",                     0x00100000L, TV_STAFF,        '_', 0,    350,  19,  1, 50, 0, 0, 0, 0, {1, 2}, 20}, // 313
    {"Curing",                          0x00200000L, TV_STAFF,        '_', 0,    1000, 20,  1, 50, 0, 0, 0, 0, {1, 2}, 25}, // 314
    {"Dispel Evil",                     0x00400000L, TV_STAFF,        '_', 0,    1200, 21,  1, 50, 0, 0, 0, 0, {1, 2}, 49}, // 315
    {"Darkness",                        0x01000000L, TV_STAFF,        '_', 0,    0,    22,  1, 50, 0, 0, 0, 0, {1, 2}, 50}, // 316
    {"Darkness",                        0x01000000L, TV_STAFF,        '_', 0,    0,    22,  1, 50, 0, 0, 0, 0, {1, 2}, 5}, // 317
    {"[Beginners-Magick]",              0x0000007FL, TV_MAGIC_BOOK,   '?', 0,    25,   64,  1, 30, 0, 0, 0, 0, {1, 1}, 40}, // 318
    {"[Magick I]",                      0x0000FF80L, TV_MAGIC_BOOK,   '?', 0,    100,  65,  1, 30, 0, 0, 0, 0, {1, 1}, 40}, // 319
    {"[Magick II]",                     0x00FF0000L, TV_MAGIC_BOOK,   '?', 0,    400,  66,  1, 30, 0, 0, 0, 0, {1, 1}, 40}, // 320
    {"[The Mages' Guide to Power]",     0x7F000000L, TV_MAGIC_BOOK,   '?', 0,    800,  67,  1, 30, 0, 0, 0, 0, {1, 1}, 40}, // 321
    {"[Beginners Handbook]",            0x000000FFL, TV_PRAYER_BOOK,  '?', 0,    25,   64,  1, 30, 0, 0, 0, 0, {1, 1}, 40}, // 322
    {"[Words of Wisdom]",               0x0000FF00L, TV_PRAYER_BOOK,  '?', 0,    100,  65,  1, 30, 0, 0, 0, 0, {1, 1}, 40}, // 323
    {"[Chants and Blessings]",          0x01FF0000L, TV_PRAYER_BOOK,  '?', 0,    400,  66,  1, 30, 0, 0, 0, 0, {1, 1}, 40}, // 324
    {"[Exorcisms and Dispellings]",     0x7E000000L, TV_PRAYER_BOOK,  '?', 0,    800,  67,  1, 30, 0, 0, 0, 0, {1, 1}, 40}, // 325
    {"& Small Wooden Chest",            0x13800000L, TV_CHEST,        '&', 0,    20,   1,   1, 250, 0, 0, 0, 0, {2, 3}, 7}, // 326
    {"& Large Wooden Chest",            0x17800000L, TV_CHEST,        '&', 0,    60,   4,   1, 500, 0, 0, 0, 0, {2, 5}, 15}, // 327
    {"& Small Iron Chest",              0x17800000L, TV_CHEST,        '&', 0,    100,  7,   1, 500, 0, 0, 0, 0, {2, 4}, 25}, // 328
    {"& Large Iron Chest",              0x23800000L, TV_CHEST,        '&', 0,    150,  10,  1, 1000, 0, 0, 0, 0, {2, 6}, 35}, // 329
    {"& Small Steel Chest",             0x1B800000L, TV_CHEST,        '&', 0,    200,  13,  1, 500, 0, 0, 0, 0, {2, 4}, 45}, // 330
    {"& Large Steel Chest",             0x33800000L, TV_CHEST,        '&', 0,    250,  16,  1, 1000, 0, 0, 0, 0, {2, 6}, 50}, // 331
    {"& Rat Skeleton",                  0x00000000L, TV_MISC,         's', 0,    0,    1,   1, 10, 0, 0, 0, 0, {1, 1}, 1}, // 332
    {"& Giant Centipede Skeleton",      0x00000000L, TV_MISC,         's', 0,    0,    2,   1, 25, 0, 0, 0, 0, {1, 1}, 1}, // 333
    {"some Filthy Rags",                0x00000000L, TV_SOFT_ARMOR,   '~', 0,    0,    63,  1, 20, 0, 0, 1, 0, {0, 0}, 0}, // 334
    {"& empty bottle",                  0x00000000L, TV_MISC,         '!', 0,    0,    4,   1, 2, 0, 0, 0, 0, {1, 1}, 0}, // 335
    {"some shards of pottery",          0x00000000L, TV_MISC,         '~', 0,    0,    5,   1, 5, 0, 0, 0, 0, {1, 1}, 0}, // 336
    {"& Human Skeleton",                0x00000000L, TV_MISC,         's', 0,    0,    7,   1, 60, 0, 0, 0, 0, {1, 1}, 1}, // 337
    {"& Dwarf Skeleton",                0x00000000L, TV_MISC,         's', 0,    0,    8,   1, 50, 0, 0, 0, 0, {1, 1}, 1}, // 338
    {"& Elf Skeleton",                  0x00000000L, TV_MISC,         's', 0,    0,    9,   1, 40, 0, 0, 0, 0, {1, 1}, 1}, // 339
    {"& Gnome Skeleton",                0x00000000L, TV_MISC,         's', 0,    0,    10,  1, 25, 0, 0, 0, 0, {1, 1}, 1}, // 340
    {"& broken set of teeth",           0x00000000L, TV_MISC,         's', 0,    0,    11,  1, 3, 0, 0, 0, 0, {1, 1}, 0}, // 341
    {"& large broken bone",             0x00000000L, TV_MISC,         's', 0,    0,    12,  1, 2, 0, 0, 0, 0, {1, 1}, 0}, // 342
    {"& broken stick",                  0x00000000L, TV_MISC,         '~', 0,    0,    13,  1, 3, 0, 0, 0, 0, {1, 1}, 0}, // 343
    // end of Dungeon items

    // Store items, which are not also dungeon items, some of these
    // can be found above, except that the number is >1 below.
    {"& Ration~ of Food",               0x00000000L, TV_FOOD,         ',', 5000,   3,  90, 5, 10,  0, 0, 0, 0, {0, 0}, 0}, // 344
    {"& Hard Biscuit~",                 0x00000000L, TV_FOOD,         ',', 500,    1,  93, 5,  2,  0, 0, 0, 0, {0, 0}, 0}, // 345
    {"& Strip~ of Beef Jerky",          0x00000000L, TV_FOOD,         ',', 1750,   2,  94, 5,  4,  0, 0, 0, 0, {0, 0}, 0}, // 346
    {"& Pint~ of Fine Ale",             0x00000000L, TV_FOOD,         ',', 500,    1,  95, 3, 10,  0, 0, 0, 0, {0, 0}, 0}, // 347
    {"& Pint~ of Fine Wine",            0x00000000L, TV_FOOD,         ',', 400,    2,  96, 1, 10,  0, 0, 0, 0, {0, 0}, 0}, // 348
    {"& Pick",                          0x20000000L, TV_DIGGING,     '\\', 1,     50,   1, 1, 150, 0, 0, 0, 0, {1, 3}, 0}, // 349
    {"& Shovel",                        0x20000000L, TV_DIGGING,     '\\', 0,     15,   4, 1, 60,  0, 0, 0, 0, {1, 2}, 0}, // 350
    {"Identify",                        0x00000008L, TV_SCROLL1,      '?', 0,     50,  67, 2,  5,  0, 0, 0, 0, {0, 0}, 0}, // 351
    {"Light",                           0x00000020L, TV_SCROLL1,      '?', 0,     15,  69, 3,  5,  0, 0, 0, 0, {0, 0}, 0}, // 352
    {"Phase Door",                      0x00000080L, TV_SCROLL1,      '?', 0,     15,  71, 2,  5,  0, 0, 0, 0, {0, 0}, 0}, // 353
    {"Magic Mapping",                   0x00000800L, TV_SCROLL1,      '?', 0,     40,  75, 2,  5,  0, 0, 0, 0, {0, 0}, 0}, // 354
    {"Treasure Detection",              0x00004000L, TV_SCROLL1,      '?', 0,     15,  78, 2,  5,  0, 0, 0, 0, {0, 0}, 0}, // 355
    {"Object Detection",                0x00008000L, TV_SCROLL1,      '?', 0,     15,  79, 2,  5,  0, 0, 0, 0, {0, 0}, 0}, // 356
    {"Detect Invisible",                0x00080000L, TV_SCROLL1,      '?', 0,     15,  83, 2,  5,  0, 0, 0, 0, {0, 0}, 0}, // 357
    {"Blessing",                        0x00000020L, TV_SCROLL2,      '?', 0,     15,  99, 2,  5,  0, 0, 0, 0, {0, 0}, 0}, // 358
    {"Word-of-Recall",                  0x00000100L, TV_SCROLL2,      '?', 0,    150, 102, 3,  5,  0, 0, 0, 0, {0, 0}, 0}, // 359
    {"Cure Light Wounds",               0x10001000L, TV_POTION1,      '!', 50,    15,  79, 2,  4,  0, 0, 0, 0, {1, 1}, 0}, // 360
    {"Heroism",                         0x00000010L, TV_POTION2,      '!', 0,     35,  98, 2,  4,  0, 0, 0, 0, {1, 1}, 0}, // 361
    {"Boldness",                        0x00000040L, TV_POTION2,      '!', 0,     10, 100, 2,  4,  0, 0, 0, 0, {1, 1}, 0}, // 362
    {"Slow Poison",                     0x00000800L, TV_POTION2,      '!', 0,     25, 105, 2,  4,  0, 0, 0, 0, {1, 1}, 0}, // 363
    {"& Brass Lantern~",                0x00000000L, TV_LIGHT,        '~', 7500,  35,   0, 1, 50,  0, 0, 0, 0, {1, 1}, 1}, // 364
    {"& Wooden Torch~",                 0x00000000L, TV_LIGHT,        '~', 4000,   2, 192, 5, 30,  0, 0, 0, 0, {1, 1}, 1}, // 365
    {"& Flask~ of Oil",                 0x00040000L, TV_FLASK,        '!', 7500,   3,  64, 5, 10,  0, 0, 0, 0, {2, 6}, 1}, // 366
    // end store items

    // start doors
    // Secret door must have same sub_category_id as closed door in
    // TRAP_LISTB.  See CHANGE_TRAP. Must use & because of stone_to_mud.
    {"& open door",                   0x00000000L, TV_OPEN_DOOR,   '\'', 0, 0,  1, 1, 0, 0, 0, 0, 0, {1, 1}, 0}, // 367
    {"& closed door",                 0x00000000L, TV_CLOSED_DOOR,  '+', 0, 0, 19, 1, 0, 0, 0, 0, 0, {1, 1}, 0}, // 368
    {"& secret door",                 0x00000000L, TV_SECRET_DOOR,  '#', 0, 0, 19, 1, 0, 0, 0, 0, 0, {1, 1}, 0}, // 369
    // end doors

    // stairs
    {"an up staircase",               0x00000000L, TV_UP_STAIR,     '<', 0, 0, 1, 1, 0, 0, 0, 0, 0, {1, 1}, 0}, // 370
    {"a down staircase",              0x00000000L, TV_DOWN_STAIR,   '>', 0, 0, 1, 1, 0, 0, 0, 0, 0, {1, 1}, 0}, // 371

    // store door
    // Stores are just special traps
    {"General Store",                 0x00000000L, TV_STORE_DOOR,   '1', 0, 0, 101, 1, 0, 0, 0, 0, 0, {0, 0}, 0}, // 372
    {"Armory",                        0x00000000L, TV_STORE_DOOR,   '2', 0, 0, 102, 1, 0, 0, 0, 0, 0, {0, 0}, 0}, // 373
    {"Weapon Smiths",                 0x00000000L, TV_STORE_DOOR,   '3', 0, 0, 103, 1, 0, 0, 0, 0, 0, {0, 0}, 0}, // 374
    {"Temple",                        0x00000000L, TV_STORE_DOOR,   '4', 0, 0, 104, 1, 0, 0, 0, 0, 0, {0, 0}, 0}, // 375
    {"Alchemy Shop",                  0x00000000L, TV_STORE_DOOR,   '5', 0, 0, 105, 1, 0, 0, 0, 0, 0, {0, 0}, 0}, // 376
    {"Magic Shop",                    0x00000000L, TV_STORE_DOOR,   '6', 0, 0, 106, 1, 0, 0, 0, 0, 0, {0, 0}, 0}, // 377
    // end store door

    // Traps are just Nasty treasures.
    // Traps: Level represents the relative difficulty of disarming;
    // and `misc_use` represents the experienced gained when disarmed
    {"an open pit",                   0x00000000L, TV_VIS_TRAP,     ' ',  1, 0,  1, 1, 0, 0, 0, 0, 0, {2, 6},  50}, // 378
    {"an arrow trap",                 0x00000000L, TV_INVIS_TRAP,   '^',  3, 0,  2, 1, 0, 0, 0, 0, 0, {1, 8},  90}, // 379
    {"a covered pit",                 0x00000000L, TV_INVIS_TRAP,   '^',  2, 0,  3, 1, 0, 0, 0, 0, 0, {2, 6},  60}, // 380
    {"a trap door",                   0x00000000L, TV_INVIS_TRAP,   '^',  5, 0,  4, 1, 0, 0, 0, 0, 0, {2, 8},  75}, // 381
    {"a gas trap",                    0x00000000L, TV_INVIS_TRAP,   '^',  3, 0,  5, 1, 0, 0, 0, 0, 0, {1, 4},  95}, // 382
    {"a loose rock",                  0x00000000L, TV_INVIS_TRAP,   ';',  0, 0,  6, 1, 0, 0, 0, 0, 0, {0, 0},  10}, // 383
    {"a dart trap",                   0x00000000L, TV_INVIS_TRAP,   '^',  5, 0,  7, 1, 0, 0, 0, 0, 0, {1, 4}, 110}, // 384
    {"a strange rune",                0x00000000L, TV_INVIS_TRAP,   '^',  5, 0,  8, 1, 0, 0, 0, 0, 0, {0, 0},  90}, // 385
    {"some loose rock",               0x00000000L, TV_INVIS_TRAP,   '^',  5, 0,  9, 1, 0, 0, 0, 0, 0, {2, 6},  90}, // 386
    {"a gas trap",                    0x00000000L, TV_INVIS_TRAP,   '^', 10, 0, 10, 1, 0, 0, 0, 0, 0, {1, 4}, 105}, // 387
    {"a strange rune",                0x00000000L, TV_INVIS_TRAP,   '^',  5, 0, 11, 1, 0, 0, 0, 0, 0, {0, 0},  90}, // 388
    {"a blackened spot",              0x00000000L, TV_INVIS_TRAP,   '^', 10, 0, 12, 1, 0, 0, 0, 0, 0, {4, 6}, 110}, // 389
    {"some corroded rock",            0x00000000L, TV_INVIS_TRAP,   '^', 10, 0, 13, 1, 0, 0, 0, 0, 0, {4, 6}, 110}, // 390
    {"a gas trap",                    0x00000000L, TV_INVIS_TRAP,   '^',  5, 0, 14, 1, 0, 0, 0, 0, 0, {2, 6}, 105}, // 391
    {"a gas trap",                    0x00000000L, TV_INVIS_TRAP,   '^',  5, 0, 15, 1, 0, 0, 0, 0, 0, {1, 4}, 110}, // 392
    {"a gas trap",                    0x00000000L, TV_INVIS_TRAP,   '^',  5, 0, 16, 1, 0, 0, 0, 0, 0, {1, 8}, 105}, // 393
    {"a dart trap",                   0x00000000L, TV_INVIS_TRAP,   '^',  5, 0, 17, 1, 0, 0, 0, 0, 0, {1, 8}, 110}, // 394
    {"a dart trap",                   0x00000000L, TV_INVIS_TRAP,   '^',  5, 0, 18, 1, 0, 0, 0, 0, 0, {1, 8}, 110}, // 395

    // rubble
    {"some rubble",                  0x00000000L, TV_RUBBLE,   ':', 0, 0, 1, 1, 0, 0, 0, 0, 0, {0, 0}, 0}, // 396

    // mush
    {"& Pint~ of Fine Grade Mush",    0x00000000L, TV_FOOD,     ',', 1500, 1, 97, 1, 1, 0, 0, 0, 0, {1, 1}, 1}, // 397

    // Special trap
    {"a strange rune",                0x00000000L, TV_VIS_TRAP, '^', 0, 0, 99, 1, 0, 0, 0, 0, 0, {0, 0}, 10}, // 398

    // Gold list (All types of gold and gems are defined here)
    {"copper",                        0x00000000L, TV_GOLD,     '$', 0,  3,  1, 1, 0, 0, 0, 0, 0, {0, 0}, 1}, // 399
    {"copper",                        0x00000000L, TV_GOLD,     '$', 0,  4,  2, 1, 0, 0, 0, 0, 0, {0, 0}, 1}, // 400
    {"copper",                        0x00000000L, TV_GOLD,     '$', 0,  5,  3, 1, 0, 0, 0, 0, 0, {0, 0}, 1}, // 401
    {"silver",                        0x00000000L, TV_GOLD,     '$', 0,  6,  4, 1, 0, 0, 0, 0, 0, {0, 0}, 1}, // 402
    {"silver",                        0x00000000L, TV_GOLD,     '$', 0,  7,  5, 1, 0, 0, 0, 0, 0, {0, 0}, 1}, // 403
    {"silver",                        0x00000000L, TV_GOLD,     '$', 0,  8,  6, 1, 0, 0, 0, 0, 0, {0, 0}, 1}, // 404
    {"garnets",                       0x00000000L, TV_GOLD,     '*', 0,  9,  7, 1, 0, 0, 0, 0, 0, {0, 0}, 1}, // 405
    {"garnets",                       0x00000000L, TV_GOLD,     '*', 0, 10,  8, 1, 0, 0, 0, 0, 0, {0, 0}, 1}, // 406
    {"gold",                          0x00000000L, TV_GOLD,     '$', 0, 12,  9, 1, 0, 0, 0, 0, 0, {0, 0}, 1}, // 407
    {"gold",                          0x00000000L, TV_GOLD,     '$', 0, 14, 10, 1, 0, 0, 0, 0, 0, {0, 0}, 1}, // 408
    {"gold",                          0x00000000L, TV_GOLD,     '$', 0, 16, 11, 1, 0, 0, 0, 0, 0, {0, 0}, 1}, // 409
    {"opals",                         0x00000000L, TV_GOLD,     '*', 0, 18, 12, 1, 0, 0, 0, 0, 0, {0, 0}, 1}, // 410
    {"sapphires",                     0x00000000L, TV_GOLD,     '*', 0, 20, 13, 1, 0, 0, 0, 0, 0, {0, 0}, 1}, // 411
    {"gold",                          0x00000000L, TV_GOLD,     '$', 0, 24, 14, 1, 0, 0, 0, 0, 0, {0, 0}, 1}, // 412
    {"rubies",                        0x00000000L, TV_GOLD,     '*', 0, 28, 15, 1, 0, 0, 0, 0, 0, {0, 0}, 1}, // 413
    {"diamonds",                      0x00000000L, TV_GOLD,     '*', 0, 32, 16, 1, 0, 0, 0, 0, 0, {0, 0}, 1}, // 414
    {"emeralds",                      0x00000000L, TV_GOLD,     '*', 0, 40, 17, 1, 0, 0, 0, 0, 0, {0, 0}, 1}, // 415
    {"mithril",                       0x00000000L, TV_GOLD,     '$', 0, 80, 18, 1, 0, 0, 0, 0, 0, {0, 0}, 1}, // 416

    // nothing, used as inventory place holder
    // must be stackable, so that can be picked up by inventoryCarryItem
    {"nothing",                       0x00000000L, TV_NOTHING,  ' ', 0, 0, 64, 0, 0, 0, 0, 0, 0, {0, 0}, 0}, // 417

    // these next two are needed only for the names
    {"& ruined chest",                0x00000000L, TV_CHEST,    '&', 0, 0, 0, 1, 250, 0, 0, 0, 0, {0, 0}, 0}, // 418
    {"",                              0x00000000L, TV_NOTHING,  ' ', 0, 0, 0, 0,   0, 0, 0, 0, 0, {0, 0}, 0}, // 419
};

const char *special_item_names[SpecialNameIds::SN_ARRAY_SIZE] = {
    CNIL,                "(R)",              "(RA)",
    "(RF)",              "(RC)",             "(RL)",
    "(HA)",              "(DF)",             "(SA)",
    "(SD)",              "(SE)",             "(SU)",
    "(FT)",              "(FB)",             "of Free Action",
    "of Slaying",        "of Clumsiness",    "of Weakness",
    "of Slow Descent",   "of Speed",         "of Stealth",
    "of Slowness",       "of Noise",         "of Great Mass",
    "of Intelligence",   "of Wisdom",        "of Infra-Vision",
    "of Might",          "of Lordliness",    "of the Magi",
    "of Beauty",         "of Seeing",        "of Regeneration",
    "of Stupidity",      "of Dullness",      "of Blindness",
    "of Timidness",      "of Teleportation", "of Ugliness",
    "of Protection",     "of Irritation",    "of Vulnerability",
    "of Enveloping",     "of Fire",          "of Slay Evil",
    "of Dragon Slaying", "(Empty)",          "(Locked)",
    "(Poison Needle)",   "(Gas Trap)",       "(Explosion Device)",
    "(Summoning Runes)", "(Multiple Traps)", "(Disarmed)",
    "(Unlocked)",        "of Slay Animal",
};
