/* source/treasure.c: dungeon object definitions
 *
 * Copyright (C) 1989-2008 James E. Wilson, Robert A. Koeneke,
 *                         David J. Grabiner
 *
 * This file is part of Umoria.
 *
 * Umoria is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Umoria is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Umoria.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "standard_library.h"

#include "config.h"
#include "constant.h"
#include "types.h"

/* Following are treasure arrays  and variables */

/* Object description:  Objects are defined here.  Each object has
   the following attributes:

    Descriptor : Name of item and formats.
                   & is replaced with 'a', 'an', or a number.
                   ~ is replaced with null or 's'.
    Character  : Character that represents the item.
    Type value : Value representing the type of object.
    Sub value  : separate value for each item of a type.
                     0 - 63:  object can not stack
                    64 - 127: dungeon object, can stack with other D object
                   128 - 191: unused, previously for store items
                   192:       stack with other iff have same p1 value, always
                              treated as individual objects
                   193 - 255: object can stack with others iff they have
                              the same p1 value, usually considered one group
                   Objects which have two type values, e.g. potions and
                   scrolls, need to have distinct subvals for
                   each item regardless of its tval
    Damage     : amount of damage item can cause.
    Weight     : relative weight of an item.
    Number     : number of items appearing in group.
    To hit     : magical plusses to hit.
    To damage  : magical plusses to damage.
    AC         : objects relative armor class.
                   1 is worse than 5 is worse than 10 etc.
    To AC      : Magical bonuses to AC.
    P1         : Catch all for magical abilities such as
                 plusses to strength, minuses to searching.
    Flags      : Abilities of object.  Each ability is a
                 bit.  Bits 1-31 are used. (Signed integer)
    Level      : Minimum level on which item can be found.
    Cost       : Relative cost of item.

    Special Abilities can be added to item by magic_init(),
    found in misc.c.

    Scrolls, Potions, and Food:
    Flags is used to define a function which reading/quaffing
    will cause.  Most scrolls and potions have only one bit
    set.  Potions will generally have some food value, found
    in p1.

    Wands and Staffs:
    Flags defines a function, p1 contains number of charges
    for item.  p1 is set in magic_init() in misc.c.

    Chests:
    Traps are added randomly by magic_init() in misc.c. */

/* Object list (All objects must be defined here) */

/* Dungeon items from 0 to MAX_DUNGEON_OBJ */
treasure_type object_list[MAX_OBJECTS] = {
/*  0*/ {"Poison",                          0x00000001L, TV_FOOD,        ',', 500,  0,    64,  1, 1,    0,  0, 0,   0, {0, 0}, 7},
/*  1*/ {"Blindness",                       0x00000002L, TV_FOOD,        ',', 500,  0,    65,  1, 1,    0,  0, 0,   0, {0, 0}, 9},
/*  2*/ {"Paranoia",                        0x00000004L, TV_FOOD,        ',', 500,  0,    66,  1, 1,    0,  0, 0,   0, {0, 0}, 9},
/*  3*/ {"Confusion",                       0x00000008L, TV_FOOD,        ',', 500,  0,    67,  1, 1,    0,  0, 0,   0, {0, 0}, 7},
/*  4*/ {"Hallucination",                   0x00000010L, TV_FOOD,        ',', 500,  0,    68,  1, 1,    0,  0, 0,   0, {0, 0}, 13},
/*  5*/ {"Cure Poison",                     0x00000020L, TV_FOOD,        ',', 500,  60,   69,  1, 1,    0,  0, 0,   0, {0, 0}, 8},
/*  6*/ {"Cure Blindness",                  0x00000040L, TV_FOOD,        ',', 500,  50,   70,  1, 1,    0,  0, 0,   0, {0, 0}, 10},
/*  7*/ {"Cure Paranoia",                   0x00000080L, TV_FOOD,        ',', 500,  25,   71,  1, 1,    0,  0, 0,   0, {0, 0}, 12},
/*  8*/ {"Cure Confusion",                  0x00000100L, TV_FOOD,        ',', 500,  50,   72,  1, 1,    0,  0, 0,   0, {0, 0}, 6},
/*  9*/ {"Weakness",                        0x04000200L, TV_FOOD,        ',', 500,  0,    73,  1, 1,    0,  0, 0,   0, {0, 0}, 7},
/* 10*/ {"Unhealth",                        0x04000400L, TV_FOOD,        ',', 500,  50,   74,  1, 1,    0,  0, 0,   0, {10, 10}, 15},
/* 11*/ {"Restore Constitution",            0x00010000L, TV_FOOD,        ',', 500,  350,  75,  1, 1,    0,  0, 0,   0, {0, 0}, 20},
/* 12*/ {"First-Aid",                       0x00200000L, TV_FOOD,        ',', 500,  5,    76,  1, 1,    0,  0, 0,   0, {0, 0}, 6},
/* 13*/ {"Minor Cures",                     0x00400000L, TV_FOOD,        ',', 500,  20,   77,  1, 1,    0,  0, 0,   0, {0, 0}, 7},
/* 14*/ {"Light Cures",                     0x00800000L, TV_FOOD,        ',', 500,  30,   78,  1, 1,    0,  0, 0,   0, {0, 0}, 10},
/* 15*/ {"Restoration",                     0x001F8000L, TV_FOOD,        ',', 500,  1000, 79,  1, 1,    0,  0, 0,   0, {0, 0}, 30},
/* 16*/ {"Poison",                          0x00000001L, TV_FOOD,        ',', 1200, 0,    80,  1, 1,    0,  0, 0,   0, {0, 0}, 15},
/* 17*/ {"Hallucination",                   0x00000010L, TV_FOOD,        ',', 1200, 0,    81,  1, 1,    0,  0, 0,   0, {0, 0}, 18},
/* 18*/ {"Cure Poison",                     0x00000020L, TV_FOOD,        ',', 1200, 75,   82,  1, 1,    0,  0, 0,   0, {0, 0}, 19},
/* 19*/ {"Unhealth",                        0x04000400L, TV_FOOD,        ',', 1200, 75,   83,  1, 1,    0,  0, 0,   0, {10, 12}, 28},
/* 20*/ {"Major Cures",                     0x02000000L, TV_FOOD,        ',', 1200, 75,   84,  1, 2,    0,  0, 0,   0, {0, 0}, 16},
/* 21*/ {"& Ration~ of Food",               0x00000000L, TV_FOOD,        ',', 5000, 3,    90,  1, 10,   0,  0, 0,   0, {0, 0}, 0},
/* 22*/ {"& Ration~ of Food",               0x00000000L, TV_FOOD,        ',', 5000, 3,    90,  1, 10,   0,  0, 0,   0, {0, 0}, 5},
/* 23*/ {"& Ration~ of Food",               0x00000000L, TV_FOOD,        ',', 5000, 3,    90,  1, 10,   0,  0, 0,   0, {0, 0}, 10},
/* 24*/ {"& Slime Mold~",                   0x00000000L, TV_FOOD,        ',', 3000, 2,    91,  1, 5,    0,  0, 0,   0, {0, 0}, 1},
/* 25*/ {"& Piece~ of Elvish Waybread",     0x02000020L, TV_FOOD,        ',', 7500, 25,   92,  1, 3,    0,  0, 0,   0, {0, 0}, 6},
/* 26*/ {"& Piece~ of Elvish Waybread",     0x02000020L, TV_FOOD,        ',', 7500, 25,   92,  1, 3,    0,  0, 0,   0, {0, 0}, 12},
/* 27*/ {"& Piece~ of Elvish Waybread",     0x02000020L, TV_FOOD,        ',', 7500, 25,   92,  1, 3,    0,  0, 0,   0, {0, 0}, 20},

/* 28*/ {"& Dagger (Main Gauche)",          0x00000000L, TV_SWORD,       '|', 0,    25,   1,   1, 30,   0,  0, 0,   0, {1, 5}, 2},
/* 29*/ {"& Dagger (Misericorde)",          0x00000000L, TV_SWORD,       '|', 0,    10,   2,   1, 15,   0,  0, 0,   0, {1, 4}, 0},
/* 30*/ {"& Dagger (Stiletto)",             0x00000000L, TV_SWORD,       '|', 0,    10,   3,   1, 12,   0,  0, 0,   0, {1, 4}, 0},
/* 31*/ {"& Dagger (Bodkin)",               0x00000000L, TV_SWORD,       '|', 0,    10,   4,   1, 20,   0,  0, 0,   0, {1, 4}, 1},
/* 32*/ {"& Broken Dagger",                 0x00000000L, TV_SWORD,       '|', 0,    0,    5,   1, 15,  -2, -2, 0,   0, {1, 1}, 0},
/* 33*/ {"& Backsword",                     0x00000000L, TV_SWORD,       '|', 0,    150,  6,   1, 95,   0,  0, 0,   0, {1, 9}, 7},
/* 34*/ {"& Bastard Sword",                 0x00000000L, TV_SWORD,       '|', 0,    350,  7,   1, 140,  0,  0, 0,   0, {3, 4}, 14},
/* 35*/ {"& Thrusting Sword (Bilbo)",       0x00000000L, TV_SWORD,       '|', 0,    60,   8,   1, 80,   0,  0, 0,   0, {1, 6}, 4},
/* 36*/ {"& Thrusting Sword (Baselard)",    0x00000000L, TV_SWORD,       '|', 0,    80,   9,   1, 100,  0,  0, 0,   0, {1, 7}, 5},
/* 37*/ {"& Broadsword",                    0x00000000L, TV_SWORD,       '|', 0,    255,  10,  1, 150,  0,  0, 0,   0, {2, 5}, 9},
/* 38*/ {"& Two-Handed Sword (Claymore)",   0x00000000L, TV_SWORD,       '|', 0,    775,  11,  1, 200,  0,  0, 0,   0, {3, 6}, 30},
/* 39*/ {"& Cutlass",                       0x00000000L, TV_SWORD,       '|', 0,    85,   12,  1, 110,  0,  0, 0,   0, {1, 7}, 7},
/* 40*/ {"& Two-Handed Sword (Espadon)",    0x00000000L, TV_SWORD,       '|', 0,    655,  13,  1, 180,  0,  0, 0,   0, {3, 6}, 35},
/* 41*/ {"& Executioner's Sword",           0x00000000L, TV_SWORD,       '|', 0,    850,  14,  1, 260,  0,  0, 0,   0, {4, 5}, 40},
/* 42*/ {"& Two-Handed Sword (Flamberge)",  0x00000000L, TV_SWORD,       '|', 0,    1000, 15,  1, 240,  0,  0, 0,   0, {4, 5}, 45},
/* 43*/ {"& Foil",                          0x00000000L, TV_SWORD,       '|', 0,    35,   16,  1, 30,   0,  0, 0,   0, {1, 5}, 2},
/* 44*/ {"& Katana",                        0x00000000L, TV_SWORD,       '|', 0,    400,  17,  1, 120,  0,  0, 0,   0, {3, 4}, 18},
/* 45*/ {"& Longsword",                     0x00000000L, TV_SWORD,       '|', 0,    200,  18,  1, 130,  0,  0, 0,   0, {1, 10}, 12},
/* 46*/ {"& Two-Handed Sword (No-Dachi)",   0x00000000L, TV_SWORD,       '|', 0,    675,  19,  1, 200,  0,  0, 0,   0, {4, 4}, 45},
/* 47*/ {"& Rapier",                        0x00000000L, TV_SWORD,       '|', 0,    42,   20,  1, 40,   0,  0, 0,   0, {1, 6}, 4},
/* 48*/ {"& Sabre",                         0x00000000L, TV_SWORD,       '|', 0,    50,   21,  1, 50,   0,  0, 0,   0, {1, 7}, 5},
/* 49*/ {"& Small Sword",                   0x00000000L, TV_SWORD,       '|', 0,    48,   22,  1, 75,   0,  0, 0,   0, {1, 6}, 5},
/* 50*/ {"& Two-Handed Sword (Zweihander)", 0x00000000L, TV_SWORD,       '|', 0,    1500, 23,  1, 280,  0,  0, 0,   0, {4, 6}, 50},
/* 51*/ {"& Broken Sword",                  0x00000000L, TV_SWORD,       '|', 0,    0,    24,  1, 75,  -2, -2, 0,   0, {1, 1}, 0},
/* 52*/ {"& Ball and Chain",                0x00000000L, TV_HAFTED,     '\\', 0,    200,  1,   1, 150,  0,  0, 0,   0, {2, 4}, 20},
/* 53*/ {"& Cat-o'-Nine-Tails",             0x00000000L, TV_HAFTED,     '\\', 0,    14,   2,   1, 40,   0,  0, 0,   0, {1, 4}, 3},
/* 54*/ {"& Wooden Club",                   0x00000000L, TV_HAFTED,     '\\', 0,    10,   3,   1, 100,  0,  0, 0,   0, {1, 3}, 0},
/* 55*/ {"& Flail",                         0x00000000L, TV_HAFTED,     '\\', 0,    353,  4,   1, 150,  0,  0, 0,   0, {2, 6}, 12},
/* 56*/ {"& Two-Handed Great Flail",        0x00000000L, TV_HAFTED,     '\\', 0,    590,  5,   1, 280,  0,  0, 0,   0, {3, 6}, 45},
/* 57*/ {"& Morningstar",                   0x00000000L, TV_HAFTED,     '\\', 0,    396,  6,   1, 150,  0,  0, 0,   0, {2, 6}, 10},
/* 58*/ {"& Mace",                          0x00000000L, TV_HAFTED,     '\\', 0,    130,  7,   1, 120,  0,  0, 0,   0, {2, 4}, 6},
/* 59*/ {"& War Hammer",                    0x00000000L, TV_HAFTED,     '\\', 0,    225,  8,   1, 120,  0,  0, 0,   0, {3, 3}, 5},
/* 60*/ {"& Lead-Filled Mace",              0x00000000L, TV_HAFTED,     '\\', 0,    502,  9,   1, 180,  0,  0, 0,   0, {3, 4}, 15},
/* 61*/ {"& Awl-Pike",                      0x00000000L, TV_POLEARM,     '/', 0,    200,  1,   1, 160,  0,  0, 0,   0, {1, 8}, 8},
/* 62*/ {"& Beaked Axe",                    0x00000000L, TV_POLEARM,     '/', 0,    408,  2,   1, 180,  0,  0, 0,   0, {2, 6}, 15},
/* 63*/ {"& Fauchard",                      0x00000000L, TV_POLEARM,     '/', 0,    326,  3,   1, 170,  0,  0, 0,   0, {1, 10}, 17},
/* 64*/ {"& Glaive",                        0x00000000L, TV_POLEARM,     '/', 0,    363,  4,   1, 190,  0,  0, 0,   0, {2, 6}, 20},
/* 65*/ {"& Halberd",                       0x00000000L, TV_POLEARM,     '/', 0,    430,  5,   1, 190,  0,  0, 0,   0, {3, 4}, 22},
/* 66*/ {"& Lucerne Hammer",                0x00000000L, TV_POLEARM,     '/', 0,    376,  6,   1, 120,  0,  0, 0,   0, {2, 5}, 11},
/* 67*/ {"& Pike",                          0x00000000L, TV_POLEARM,     '/', 0,    358,  7,   1, 160,  0,  0, 0,   0, {2, 5}, 15},
/* 68*/ {"& Spear",                         0x00000000L, TV_POLEARM,     '/', 0,    36,   8,   1, 50,   0,  0, 0,   0, {1, 6}, 5},
/* 69*/ {"& Lance",                         0x00000000L, TV_POLEARM,     '/', 0,    230,  9,   1, 300,  0,  0, 0,   0, {2, 8}, 10},
/* 70*/ {"& Javelin",                       0x00000000L, TV_POLEARM,     '/', 0,    18,   10,  1, 30,   0,  0, 0,   0, {1, 4}, 4},
/* 71*/ {"& Battle Axe (Balestarius)",      0x00000000L, TV_POLEARM,     '/', 0,    500,  11,  1, 180,  0,  0, 0,   0, {2, 8}, 30},
/* 72*/ {"& Battle Axe (European)",         0x00000000L, TV_POLEARM,     '/', 0,    334,  12,  1, 170,  0,  0, 0,   0, {3, 4}, 13},
/* 73*/ {"& Broad Axe",                     0x00000000L, TV_POLEARM,     '/', 0,    304,  13,  1, 160,  0,  0, 0,   0, {2, 6}, 17},
/* 74*/ {"& Short Bow",                     0x00000000L, TV_BOW,         '}', 2,    50,   1,   1, 30,   0,  0, 0,   0, {0, 0}, 3},
/* 75*/ {"& Long Bow",                      0x00000000L, TV_BOW,         '}', 3,    120,  2,   1, 40,   0,  0, 0,   0, {0, 0}, 10},
/* 76*/ {"& Composite Bow",                 0x00000000L, TV_BOW,         '}', 4,    240,  3,   1, 40,   0,  0, 0,   0, {0, 0}, 40},
/* 77*/ {"& Light Crossbow",                0x00000000L, TV_BOW,         '}', 5,    140,  10,  1, 110,  0,  0, 0,   0, {0, 0}, 15},
/* 78*/ {"& Heavy Crossbow",                0x00000000L, TV_BOW,         '}', 6,    300,  11,  1, 200,  0,  0, 0,   0, {1, 1}, 30},
/* 79*/ {"& Sling",                         0x00000000L, TV_BOW,         '}', 1,    5,    20,  1, 5,    0,  0, 0,   0, {0, 0}, 1},
/* 80*/ {"& Arrow~",                        0x00000000L, TV_ARROW,       '{', 0,    1,    193, 1, 2,    0,  0, 0,   0, {1, 4}, 2},
/* 81*/ {"& Bolt~",                         0x00000000L, TV_BOLT,        '{', 0,    2,    193, 1, 3,    0,  0, 0,   0, {1, 5}, 2},
/* 82*/ {"& Rounded Pebble~",               0x00000000L, TV_SLING_AMMO,  '{', 0,    1,    193, 1, 4,    0,  0, 0,   0, {1, 2}, 0},
/* 83*/ {"& Iron Shot~",                    0x00000000L, TV_SLING_AMMO,  '{', 0,    2,    194, 1, 5,    0,  0, 0,   0, {1, 3}, 3},
/* 84*/ {"& Iron Spike~",                   0x00000000L, TV_SPIKE,       '~', 0,    1,    193, 1, 10,   0,  0, 0,   0, {1, 1}, 1},
/* 85*/ {"& Brass Lantern~",                0x00000000L, TV_LIGHT,       '~', 7500, 35,   1,   1, 50,   0,  0, 0,   0, {1, 1}, 1},
/* 86*/ {"& Wooden Torch~",                 0x00000000L, TV_LIGHT,       '~', 4000, 2,    193, 1, 30,   0,  0, 0,   0, {1, 1}, 1},
/* 87*/ {"& Orcish Pick",                   0x20000000L, TV_DIGGING,    '\\', 2,    500,  2,   1, 180,  0,  0, 0,   0, {1, 3}, 20},
/* 88*/ {"& Dwarven Pick",                  0x20000000L, TV_DIGGING,    '\\', 3,    1200, 3,   1, 200,  0,  0, 0,   0, {1, 4}, 50},
/* 89*/ {"& Gnomish Shovel",                0x20000000L, TV_DIGGING,    '\\', 1,    100,  5,   1, 50,   0,  0, 0,   0, {1, 2}, 20},
/* 90*/ {"& Dwarven Shovel",                0x20000000L, TV_DIGGING,    '\\', 2,    250,  6,   1, 120,  0,  0, 0,   0, {1, 3}, 40},
/* 91*/ {"& Pair of Soft Leather Shoes",    0x00000000L, TV_BOOTS,       ']', 0,    4,    1,   1, 5,    0,  0, 1,   0, {0, 0}, 1},
/* 92*/ {"& Pair of Soft Leather Boots",    0x00000000L, TV_BOOTS,       ']', 0,    7,    2,   1, 20,   0,  0, 2,   0, {1, 1}, 4},
/* 93*/ {"& Pair of Hard Leather Boots",    0x00000000L, TV_BOOTS,       ']', 0,    12,   3,   1, 40,   0,  0, 3,   0, {1, 1}, 6},
/* 94*/ {"& Soft Leather Cap",              0x00000000L, TV_HELM,        ']', 0,    4,    1,   1, 10,   0,  0, 1,   0, {0, 0}, 2},
/* 95*/ {"& Hard Leather Cap",              0x00000000L, TV_HELM,        ']', 0,    12,   2,   1, 15,   0,  0, 2,   0, {0, 0}, 4},
/* 96*/ {"& Metal Cap",                     0x00000000L, TV_HELM,        ']', 0,    30,   3,   1, 20,   0,  0, 3,   0, {1, 1}, 7},
/* 97*/ {"& Iron Helm",                     0x00000000L, TV_HELM,        ']', 0,    75,   4,   1, 75,   0,  0, 5,   0, {1, 3}, 20},
/* 98*/ {"& Steel Helm",                    0x00000000L, TV_HELM,        ']', 0,    200,  5,   1, 60,   0,  0, 6,   0, {1, 3}, 40},
/* 99*/ {"& Silver Crown",                  0x00000000L, TV_HELM,        ']', 0,    500,  6,   1, 20,   0,  0, 0,   0, {1, 1}, 44},
/*100*/ {"& Golden Crown",                  0x00000000L, TV_HELM,        ']', 0,    1000, 7,   1, 30,   0,  0, 0,   0, {1, 2}, 47},
/*101*/ {"& Jewel-Encrusted Crown",         0x00000000L, TV_HELM,        ']', 0,    2000, 8,   1, 40,   0,  0, 0,   0, {1, 3}, 50},
/*102*/ {"& Robe",                          0x00000000L, TV_SOFT_ARMOR,  '(', 0,    4,    1,   1, 20,   0,  0, 2,   0, {0, 0}, 1},
/*103*/ {"Soft Leather Armor",              0x00000000L, TV_SOFT_ARMOR,  '(', 0,    18,   2,   1, 80,   0,  0, 4,   0, {0, 0}, 2},
/*104*/ {"Soft Studded Leather",            0x00000000L, TV_SOFT_ARMOR,  '(', 0,    35,   3,   1, 90,   0,  0, 5,   0, {1, 1}, 3},
/*105*/ {"Hard Leather Armor",              0x00000000L, TV_SOFT_ARMOR,  '(', 0,    55,   4,   1, 100, -1,  0, 6,   0, {1, 1}, 5},
/*106*/ {"Hard Studded Leather",            0x00000000L, TV_SOFT_ARMOR,  '(', 0,    100,  5,   1, 110, -1,  0, 7,   0, {1, 2}, 7},
/*107*/ {"Woven Cord Armor",                0x00000000L, TV_SOFT_ARMOR,  '(', 0,    45,   6,   1, 150, -1,  0, 6,   0, {0, 0}, 7},
/*108*/ {"Soft Leather Ring Mail",          0x00000000L, TV_SOFT_ARMOR,  '(', 0,    160,  7,   1, 130, -1,  0, 6,   0, {1, 2}, 10},
/*109*/ {"Hard Leather Ring Mail",          0x00000000L, TV_SOFT_ARMOR,  '(', 0,    230,  8,   1, 150, -2,  0, 8,   0, {1, 3}, 12},
/*110*/ {"Leather Scale Mail",              0x00000000L, TV_SOFT_ARMOR,  '(', 0,    330,  9,   1, 140, -1,  0, 11,  0, {1, 1}, 14},
/*111*/ {"Metal Scale Mail",                0x00000000L, TV_HARD_ARMOR,  '[', 0,    430,  1,   1, 250, -2,  0, 13,  0, {1, 4}, 24},
/*112*/ {"Chain Mail",                      0x00000000L, TV_HARD_ARMOR,  '[', 0,    530,  2,   1, 220, -2,  0, 14,  0, {1, 4}, 26},
/*113*/ {"Rusty Chain Mail",                0x00000000L, TV_HARD_ARMOR,  '[', 0,    0,    3,   1, 220, -5,  0, 14, -8, {1, 4}, 26},
/*114*/ {"Double Chain Mail",               0x00000000L, TV_HARD_ARMOR,  '[', 0,    630,  4,   1, 260, -2,  0, 15,  0, {1, 4}, 28},
/*115*/ {"Augmented Chain Mail",            0x00000000L, TV_HARD_ARMOR,  '[', 0,    675,  5,   1, 270, -2,  0, 16,  0, {1, 4}, 30},
/*116*/ {"Bar Chain Mail",                  0x00000000L, TV_HARD_ARMOR,  '[', 0,    720,  6,   1, 280, -2,  0, 18,  0, {1, 4}, 34},
/*117*/ {"Metal Brigandine Armor",          0x00000000L, TV_HARD_ARMOR,  '[', 0,    775,  7,   1, 290, -3,  0, 19,  0, {1, 4}, 36},
/*118*/ {"Laminated Armor",                 0x00000000L, TV_HARD_ARMOR,  '[', 0,    825,  8,   1, 300, -3,  0, 20,  0, {1, 4}, 38},
/*119*/ {"Partial Plate Armor",             0x00000000L, TV_HARD_ARMOR,  '[', 0,    900,  9,   1, 320, -3,  0, 22,  0, {1, 6}, 42},
/*120*/ {"Metal Lamellar Armor",            0x00000000L, TV_HARD_ARMOR,  '[', 0,    950,  10,  1, 340, -3,  0, 23,  0, {1, 6}, 44},
/*121*/ {"Full Plate Armor",                0x00000000L, TV_HARD_ARMOR,  '[', 0,    1050, 11,  1, 380, -3,  0, 25,  0, {2, 4}, 48},
/*122*/ {"Ribbed Plate Armor",              0x00000000L, TV_HARD_ARMOR,  '[', 0,    1200, 12,  1, 380, -3,  0, 28,  0, {2, 4}, 50},
/*123*/ {"& Cloak",                         0x00000000L, TV_CLOAK,       '(', 0,    3,    1,   1, 10,   0,  0, 1,   0, {0, 0}, 1},
/*124*/ {"& Set of Leather Gloves",         0x00000000L, TV_GLOVES,      ']', 0,    3,    1,   1, 5,    0,  0, 1,   0, {0, 0}, 1},
/*125*/ {"& Set of Gauntlets",              0x00000000L, TV_GLOVES,      ']', 0,    35,   2,   1, 25,   0,  0, 2,   0, {1, 1}, 12},
/*126*/ {"& Small Leather Shield",          0x00000000L, TV_SHIELD,      ')', 0,    30,   1,   1, 50,   0,  0, 2,   0, {1, 1}, 3},
/*127*/ {"& Medium Leather Shield",         0x00000000L, TV_SHIELD,      ')', 0,    60,   2,   1, 75,   0,  0, 3,   0, {1, 2}, 8},
/*128*/ {"& Large Leather Shield",          0x00000000L, TV_SHIELD,      ')', 0,    120,  3,   1, 100,  0,  0, 4,   0, {1, 2}, 15},
/*129*/ {"& Small Metal Shield",            0x00000000L, TV_SHIELD,      ')', 0,    50,   4,   1, 65,   0,  0, 3,   0, {1, 2}, 10},
/*130*/ {"& Medium Metal Shield",           0x00000000L, TV_SHIELD,      ')', 0,    125,  5,   1, 90,   0,  0, 4,   0, {1, 3}, 20},
/*131*/ {"& Large Metal Shield",            0x00000000L, TV_SHIELD,      ')', 0,    200,  6,   1, 120,  0,  0, 5,   0, {1, 3}, 30},
/*132*/ {"Strength",                        0x00000001L, TV_RING,        '=', 0,    400,  0,   1, 2,    0,  0, 0,   0, {0, 0}, 30},
/*133*/ {"Dexterity",                       0x00000008L, TV_RING,        '=', 0,    400,  1,   1, 2,    0,  0, 0,   0, {0, 0}, 30},
/*134*/ {"Constitution",                    0x00000010L, TV_RING,        '=', 0,    400,  2,   1, 2,    0,  0, 0,   0, {0, 0}, 30},
/*135*/ {"Intelligence",                    0x00000002L, TV_RING,        '=', 0,    400,  3,   1, 2,    0,  0, 0,   0, {0, 0}, 30},
/*136*/ {"Speed",                           0x00001000L, TV_RING,        '=', 0,    3000, 4,   1, 2,    0,  0, 0,   0, {0, 0}, 50},
/*137*/ {"Searching",                       0x00000040L, TV_RING,        '=', 0,    250,  5,   1, 2,    0,  0, 0,   0, {0, 0}, 7},
/*138*/ {"Teleportation",                   0x80000400L, TV_RING,        '=', 0,    0,    6,   1, 2,    0,  0, 0,   0, {0, 0}, 7},
/*139*/ {"Slow Digestion",                  0x00000080L, TV_RING,        '=', 0,    200,  7,   1, 2,    0,  0, 0,   0, {0, 0}, 7},
/*140*/ {"Resist Fire",                     0x00080000L, TV_RING,        '=', 0,    250,  8,   1, 2,    0,  0, 0,   0, {0, 0}, 14},
/*141*/ {"Resist Cold",                     0x00200000L, TV_RING,        '=', 0,    250,  9,   1, 2,    0,  0, 0,   0, {0, 0}, 14},
/*142*/ {"Feather Falling",                 0x04000000L, TV_RING,        '=', 0,    200,  10,  1, 2,    0,  0, 0,   0, {0, 0}, 7},
/*143*/ {"Adornment",                       0x00000000L, TV_RING,        '=', 0,    20,   11,  1, 2,    0,  0, 0,   0, {0, 0}, 7},
/* was a ring of adornment, subval = 12 here */
/*144*/ {"& Arrow~",                        0x00000000L, TV_ARROW,        '{',  0, 1,    193, 1, 2, 0, 0, 0,  0, {1, 4}, 15},
/*145*/ {"Weakness",                        0x80000001L, TV_RING,         '=', -5, 0,    13,  1, 2, 0, 0, 0,  0, {0, 0}, 7},
/*146*/ {"Lordly Protection (FIRE)",        0x00080000L, TV_RING,         '=',  0, 1200, 14,  1, 2, 0, 0, 0,  5, {0, 0}, 50},
/*147*/ {"Lordly Protection (ACID)",        0x00100000L, TV_RING,         '=',  0, 1200, 15,  1, 2, 0, 0, 0,  5, {0, 0}, 50},
/*148*/ {"Lordly Protection (COLD)",        0x00200000L, TV_RING,         '=',  0, 1200, 16,  1, 2, 0, 0, 0,  5, {0, 0}, 50},
/*149*/ {"WOE",                             0x80000644L, TV_RING,         '=', -5, 0,    17,  1, 2, 0, 0, 0, -3, {0, 0}, 50},
/*150*/ {"Stupidity",                       0x80000002L, TV_RING,         '=', -5, 0,    18,  1, 2, 0, 0, 0,  0, {0, 0}, 7},
/*151*/ {"Increase Damage",                 0x00000000L, TV_RING,         '=',  0, 100,  19,  1, 2, 0, 0, 0,  0, {0, 0}, 20},
/*152*/ {"Increase To-Hit",                 0x00000000L, TV_RING,         '=',  0, 100,  20,  1, 2, 0, 0, 0,  0, {0, 0}, 20},
/*153*/ {"Protection",                      0x00000000L, TV_RING,         '=',  0, 100,  21,  1, 2, 0, 0, 0,  0, {0, 0}, 7},
/*154*/ {"Aggravate Monster",               0x80000200L, TV_RING,         '=',  0, 0,    22,  1, 2, 0, 0, 0,  0, {0, 0}, 7},
/*155*/ {"See Invisible",                   0x01000000L, TV_RING,         '=',  0, 500,  23,  1, 2, 0, 0, 0,  0, {0, 0}, 40},
/*156*/ {"Sustain Strength",                0x00400000L, TV_RING,         '=',  1, 750,  24,  1, 2, 0, 0, 0,  0, {0, 0}, 44},
/*157*/ {"Sustain Intelligence",            0x00400000L, TV_RING,         '=',  2, 600,  25,  1, 2, 0, 0, 0,  0, {0, 0}, 44},
/*158*/ {"Sustain Wisdom",                  0x00400000L, TV_RING,         '=',  3, 600,  26,  1, 2, 0, 0, 0,  0, {0, 0}, 44},
/*159*/ {"Sustain Constitution",            0x00400000L, TV_RING,         '=',  4, 750,  27,  1, 2, 0, 0, 0,  0, {0, 0}, 44},
/*160*/ {"Sustain Dexterity",               0x00400000L, TV_RING,         '=',  5, 750,  28,  1, 2, 0, 0, 0,  0, {0, 0}, 44},
/*161*/ {"Sustain Charisma",                0x00400000L, TV_RING,         '=',  6, 500,  29,  1, 2, 0, 0, 0,  0, {0, 0}, 44},
/*162*/ {"Slaying",                         0x00000000L, TV_RING,         '=',  0, 1000, 30,  1, 2, 0, 0, 0,  0, {0, 0}, 50},
/*163*/ {"Wisdom",                          0x00000004L, TV_AMULET,       '"',  0, 300,  0,   1, 3, 0, 0, 0,  0, {0, 0}, 20},
/*164*/ {"Charisma",                        0x00000020L, TV_AMULET,       '"',  0, 250,  1,   1, 3, 0, 0, 0,  0, {0, 0}, 20},
/*165*/ {"Searching",                       0x00000040L, TV_AMULET,       '"',  0, 250,  2,   1, 3, 0, 0, 0,  0, {0, 0}, 14},
/*166*/ {"Teleportation",                   0x80000400L, TV_AMULET,       '"',  0, 0,    3,   1, 3, 0, 0, 0,  0, {0, 0}, 14},
/*167*/ {"Slow Digestion",                  0x00000080L, TV_AMULET,       '"',  0, 200,  4,   1, 3, 0, 0, 0,  0, {0, 0}, 14},
/*168*/ {"Resist Acid",                     0x00100000L, TV_AMULET,       '"',  0, 250,  5,   1, 3, 0, 0, 0,  0, {0, 0}, 24},
/*169*/ {"Adornment",                       0x00000000L, TV_AMULET,       '"',  0, 20,   6,   1, 3, 0, 0, 0,  0, {0, 0}, 16},
/* was an amulet of adornment here, subval = 7 */
/*170*/ {"& Bolt~",                         0x00000000L, TV_BOLT,         '{',  0, 2,    193, 1, 3, 0, 0, 0, 0, {1, 5}, 25},
/*171*/ {"the Magi",                        0x01800040L, TV_AMULET,       '"',  0, 5000, 8,   1, 3, 0, 0, 0, 3, {0, 0}, 50},
/*172*/ {"DOOM",                            0x8000007FL, TV_AMULET,       '"', -5, 0,    9,   1, 3, 0, 0, 0, 0, {0, 0}, 50},
/*173*/ {"Enchant Weapon To-Hit",           0x00000001L, TV_SCROLL1,      '?',  0, 125,  64,  1, 5, 0, 0, 0, 0, {0, 0}, 12},
/*174*/ {"Enchant Weapon To-Dam",           0x00000002L, TV_SCROLL1,      '?',  0, 125,  65,  1, 5, 0, 0, 0, 0, {0, 0}, 12},
/*175*/ {"Enchant Armor",                   0x00000004L, TV_SCROLL1,      '?',  0, 125,  66,  1, 5, 0, 0, 0, 0, {0, 0}, 12},
/*176*/ {"Identify",                        0x00000008L, TV_SCROLL1,      '?',  0, 50,   67,  1, 5, 0, 0, 0, 0, {0, 0}, 1},
/*177*/ {"Identify",                        0x00000008L, TV_SCROLL1,      '?',  0, 50,   67,  1, 5, 0, 0, 0, 0, {0, 0}, 5},
/*178*/ {"Identify",                        0x00000008L, TV_SCROLL1,      '?',  0, 50,   67,  1, 5, 0, 0, 0, 0, {0, 0}, 10},
/*179*/ {"Identify",                        0x00000008L, TV_SCROLL1,      '?',  0, 50,   67,  1, 5, 0, 0, 0, 0, {0, 0}, 30},
/*180*/ {"Remove Curse",                    0x00000010L, TV_SCROLL1,      '?',  0, 100,  68,  1, 5, 0, 0, 0, 0, {0, 0}, 7},
/*181*/ {"Light",                           0x00000020L, TV_SCROLL1,      '?',  0, 15,   69,  1, 5, 0, 0, 0, 0, {0, 0}, 0},
/*182*/ {"Light",                           0x00000020L, TV_SCROLL1,      '?',  0, 15,   69,  1, 5, 0, 0, 0, 0, {0, 0}, 3},
/*183*/ {"Light",                           0x00000020L, TV_SCROLL1,      '?',  0, 15,   69,  1, 5, 0, 0, 0, 0, {0, 0}, 7},
/*184*/ {"Summon Monster",                  0x00000040L, TV_SCROLL1,      '?',  0, 0,    70,  1, 5, 0, 0, 0, 0, {0, 0}, 1},
/*185*/ {"Phase Door",                      0x00000080L, TV_SCROLL1,      '?',  0, 15,   71,  1, 5, 0, 0, 0, 0, {0, 0}, 1},
/*186*/ {"Teleport",                        0x00000100L, TV_SCROLL1,      '?',  0, 40,   72,  1, 5, 0, 0, 0, 0, {0, 0}, 10},
/*187*/ {"Teleport Level",                  0x00000200L, TV_SCROLL1,      '?',  0, 50,   73,  1, 5, 0, 0, 0, 0, {0, 0}, 20},
/*188*/ {"Monster Confusion",               0x00000400L, TV_SCROLL1,      '?',  0, 30,   74,  1, 5, 0, 0, 0, 0, {0, 0}, 5},
/*189*/ {"Magic Mapping",                   0x00000800L, TV_SCROLL1,      '?',  0, 40,   75,  1, 5, 0, 0, 0, 0, {0, 0}, 5},
/*190*/ {"Sleep Monster",                   0x00001000L, TV_SCROLL1,      '?',  0, 35,   76,  1, 5, 0, 0, 0, 0, {0, 0}, 5},
/*191*/ {"Rune of Protection",              0x00002000L, TV_SCROLL1,      '?',  0, 500,  77,  1, 5, 0, 0, 0, 0, {0, 0}, 50},
/*192*/ {"Treasure Detection",              0x00004000L, TV_SCROLL1,      '?',  0, 15,   78,  1, 5, 0, 0, 0, 0, {0, 0}, 0},
/*193*/ {"Object Detection",                0x00008000L, TV_SCROLL1,      '?',  0, 15,   79,  1, 5, 0, 0, 0, 0, {0, 0}, 0},
/*194*/ {"Trap Detection",                  0x00010000L, TV_SCROLL1,      '?',  0, 35,   80,  1, 5, 0, 0, 0, 0, {0, 0}, 5},
/*195*/ {"Trap Detection",                  0x00010000L, TV_SCROLL1,      '?',  0, 35,   80,  1, 5, 0, 0, 0, 0, {0, 0}, 8},
/*196*/ {"Trap Detection",                  0x00010000L, TV_SCROLL1,      '?',  0, 35,   80,  1, 5, 0, 0, 0, 0, {0, 0}, 12},
/*197*/ {"Door/Stair Location",             0x00020000L, TV_SCROLL1,      '?',  0, 35,   81,  1, 5, 0, 0, 0, 0, {0, 0}, 5},
/*198*/ {"Door/Stair Location",             0x00020000L, TV_SCROLL1,      '?',  0, 35,   81,  1, 5, 0, 0, 0, 0, {0, 0}, 10},
/*199*/ {"Door/Stair Location",             0x00020000L, TV_SCROLL1,      '?',  0, 35,   81,  1, 5, 0, 0, 0, 0, {0, 0}, 15},
/*200*/ {"Mass Genocide",                   0x00040000L, TV_SCROLL1,      '?',  0, 1000, 82,  1, 5, 0, 0, 0, 0, {0, 0}, 50},
/*201*/ {"Detect Invisible",                0x00080000L, TV_SCROLL1,      '?',  0, 15,   83,  1, 5, 0, 0, 0, 0, {0, 0}, 1},
/*202*/ {"Aggravate Monster",               0x00100000L, TV_SCROLL1,      '?',  0, 0,    84,  1, 5, 0, 0, 0, 0, {0, 0}, 5},
/*203*/ {"Trap Creation",                   0x00200000L, TV_SCROLL1,      '?',  0, 0,    85,  1, 5, 0, 0, 0, 0, {0, 0}, 12},
/*204*/ {"Trap/Door Destruction",           0x00400000L, TV_SCROLL1,      '?',  0, 50,   86,  1, 5, 0, 0, 0, 0, {0, 0}, 12},
/*205*/ {"Door Creation",                   0x00800000L, TV_SCROLL1,      '?',  0, 100,  87,  1, 5, 0, 0, 0, 0, {0, 0}, 12},
/*206*/ {"Recharging",                      0x01000000L, TV_SCROLL1,      '?',  0, 200,  88,  1, 5, 0, 0, 0, 0, {0, 0}, 40},
/*207*/ {"Genocide",                        0x02000000L, TV_SCROLL1,      '?',  0, 750,  89,  1, 5, 0, 0, 0, 0, {0, 0}, 35},
/*208*/ {"Darkness",                        0x04000000L, TV_SCROLL1,      '?',  0, 0,    90,  1, 5, 0, 0, 0, 0, {0, 0}, 1},
/*209*/ {"Protection from Evil",            0x08000000L, TV_SCROLL1,      '?',  0, 100,  91,  1, 5, 0, 0, 0, 0, {0, 0}, 30},
/*210*/ {"Create Food",                     0x10000000L, TV_SCROLL1,      '?',  0, 10,   92,  1, 5, 0, 0, 0, 0, {0, 0}, 5},
/*211*/ {"Dispel Undead",                   0x20000000L, TV_SCROLL1,      '?',  0, 200,  93,  1, 5, 0, 0, 0, 0, {0, 0}, 40},
/*212*/ {"*Enchant Weapon*",                0x00000001L, TV_SCROLL2,      '?',  0, 500,  94,  1, 5, 0, 0, 0, 0, {0, 0}, 50},
/*213*/ {"Curse Weapon",                    0x00000002L, TV_SCROLL2,      '?',  0, 0,    95,  1, 5, 0, 0, 0, 0, {0, 0}, 50},
/*214*/ {"*Enchant Armor*",                 0x00000004L, TV_SCROLL2,      '?',  0, 500,  96,  1, 5, 0, 0, 0, 0, {0, 0}, 50},
/*215*/ {"Curse Armor",                     0x00000008L, TV_SCROLL2,      '?',  0, 0,    97,  1, 5, 0, 0, 0, 0, {0, 0}, 50},
/*216*/ {"Summon Undead",                   0x00000010L, TV_SCROLL2,      '?',  0, 0,    98,  1, 5, 0, 0, 0, 0, {0, 0}, 15},
/*217*/ {"Blessing",                        0x00000020L, TV_SCROLL2,      '?',  0, 15,   99,  1, 5, 0, 0, 0, 0, {0, 0}, 1},
/*218*/ {"Holy Chant",                      0x00000040L, TV_SCROLL2,      '?',  0, 40,   100, 1, 5, 0, 0, 0, 0, {0, 0}, 12},
/*219*/ {"Holy Prayer",                     0x00000080L, TV_SCROLL2,      '?',  0, 80,   101, 1, 5, 0, 0, 0, 0, {0, 0}, 24},
/*220*/ {"Word-of-Recall",                  0x00000100L, TV_SCROLL2,      '?',  0, 150,  102, 1, 5, 0, 0, 0, 0, {0, 0}, 5},
/*221*/ {"*Destruction*",                   0x00000200L, TV_SCROLL2,      '?',  0, 750,  103, 1, 5, 0, 0, 0, 0, {0, 0}, 40},
/* SMJ, AJ, Water must be subval 64-66 resp. for objdes to work */
/*222*/ {"Slime Mold Juice",                0x30000000L, TV_POTION1,      '!', 400,  2,    64,  1, 4, 0, 0, 0, 0, {1, 1}, 0},
/*223*/ {"Apple Juice",                     0x00000000L, TV_POTION1,      '!', 250,  1,    65,  1, 4, 0, 0, 0, 0, {1, 1}, 0},
/*224*/ {"Water",                           0x00000000L, TV_POTION1,      '!', 200,  0,    66,  1, 4, 0, 0, 0, 0, {1, 1}, 0},
/*225*/ {"Strength",                        0x00000001L, TV_POTION1,      '!', 50,   300,  67,  1, 4, 0, 0, 0, 0, {1, 1}, 25},
/*226*/ {"Weakness",                        0x00000002L, TV_POTION1,      '!', 0,    0,    68,  1, 4, 0, 0, 0, 0, {1, 1}, 3},
/*227*/ {"Restore Strength",                0x00000004L, TV_POTION1,      '!', 0,    300,  69,  1, 4, 0, 0, 0, 0, {1, 1}, 40},
/*228*/ {"Intelligence",                    0x00000008L, TV_POTION1,      '!', 0,    300,  70,  1, 4, 0, 0, 0, 0, {1, 1}, 25},
/*229*/ {"Lose Intelligence",               0x00000010L, TV_POTION1,      '!', 0,    0,    71,  1, 4, 0, 0, 0, 0, {1, 1}, 25},
/*230*/ {"Restore Intelligence",            0x00000020L, TV_POTION1,      '!', 0,    300,  72,  1, 4, 0, 0, 0, 0, {1, 1}, 40},
/*231*/ {"Wisdom",                          0x00000040L, TV_POTION1,      '!', 0,    300,  73,  1, 4, 0, 0, 0, 0, {1, 1}, 25},
/*232*/ {"Lose Wisdom",                     0x00000080L, TV_POTION1,      '!', 0,    0,    74,  1, 4, 0, 0, 0, 0, {1, 1}, 25},
/*233*/ {"Restore Wisdom",                  0x00000100L, TV_POTION1,      '!', 0,    300,  75,  1, 4, 0, 0, 0, 0, {1, 1}, 40},
/*234*/ {"Charisma",                        0x00000200L, TV_POTION1,      '!', 0,    300,  76,  1, 4, 0, 0, 0, 0, {1, 1}, 25},
/*235*/ {"Ugliness",                        0x00000400L, TV_POTION1,      '!', 0,    0,    77,  1, 4, 0, 0, 0, 0, {1, 1}, 25},
/*236*/ {"Restore Charisma",                0x00000800L, TV_POTION1,      '!', 0,    300,  78,  1, 4, 0, 0, 0, 0, {1, 1}, 40},
/*237*/ {"Cure Light Wounds",               0x10001000L, TV_POTION1,      '!', 50,   15,   79,  1, 4, 0, 0, 0, 0, {1, 1}, 0},
/*238*/ {"Cure Light Wounds",               0x10001000L, TV_POTION1,      '!', 50,   15,   79,  1, 4, 0, 0, 0, 0, {1, 1}, 1},
/*239*/ {"Cure Light Wounds",               0x10001000L, TV_POTION1,      '!', 50,   15,   79,  1, 4, 0, 0, 0, 0, {1, 1}, 2},
/*240*/ {"Cure Serious Wounds",             0x30002000L, TV_POTION1,      '!', 100,  40,   80,  1, 4, 0, 0, 0, 0, {1, 1}, 3},
/*241*/ {"Cure Critical Wounds",            0x70004000L, TV_POTION1,      '!', 100,  100,  81,  1, 4, 0, 0, 0, 0, {1, 1}, 5},
/*242*/ {"Healing",                         0x70008000L, TV_POTION1,      '!', 200,  200,  82,  1, 4, 0, 0, 0, 0, {1, 1}, 12},
/*243*/ {"Constitution",                    0x00010000L, TV_POTION1,      '!', 50,   300,  83,  1, 4, 0, 0, 0, 0, {1, 1}, 25},
/*244*/ {"Gain Experience",                 0x00020000L, TV_POTION1,      '!', 0,    2500, 84,  1, 4, 0, 0, 0, 0, {1, 1}, 50},
/*245*/ {"Sleep",                           0x00040000L, TV_POTION1,      '!', 100,  0,    85,  1, 4, 0, 0, 0, 0, {1, 1}, 0},
/*246*/ {"Blindness",                       0x00080000L, TV_POTION1,      '!', 0,    0,    86,  1, 4, 0, 0, 0, 0, {1, 1}, 0},
/*247*/ {"Confusion",                       0x00100000L, TV_POTION1,      '!', 50,   0,    87,  1, 4, 0, 0, 0, 0, {1, 1}, 0},
/*248*/ {"Poison",                          0x00200000L, TV_POTION1,      '!', 0,    0,    88,  1, 4, 0, 0, 0, 0, {1, 1}, 3},
/*249*/ {"Haste Self",                      0x00400000L, TV_POTION1,      '!', 0,    75,   89,  1, 4, 0, 0, 0, 0, {1, 1}, 1},
/*250*/ {"Slowness",                        0x00800000L, TV_POTION1,      '!', 50,   0,    90,  1, 4, 0, 0, 0, 0, {1, 1}, 1},
/*251*/ {"Dexterity",                       0x02000000L, TV_POTION1,      '!', 0,    300,  91,  1, 4, 0, 0, 0, 0, {1, 1}, 25},
/*252*/ {"Restore Dexterity",               0x04000000L, TV_POTION1,      '!', 0,    300,  92,  1, 4, 0, 0, 0, 0, {1, 1}, 40},
/*253*/ {"Restore Constitution",            0x68000000L, TV_POTION1,      '!', 0,    300,  93,  1, 4, 0, 0, 0, 0, {1, 1}, 40},
/*254*/ {"Lose Experience",                 0x00000002L, TV_POTION2,      '!', 0,    0,    95,  1, 4, 0, 0, 0, 0, {1, 1}, 10},
/*255*/ {"Salt Water",                      0x00000004L, TV_POTION2,      '!', 0,    0,    96,  1, 4, 0, 0, 0, 0, {1, 1}, 0},
/*256*/ {"Invulnerability",                 0x00000008L, TV_POTION2,      '!', 0,    1000, 97,  1, 4, 0, 0, 0, 0, {1, 1}, 40},
/*257*/ {"Heroism",                         0x00000010L, TV_POTION2,      '!', 0,    35,   98,  1, 4, 0, 0, 0, 0, {1, 1}, 1},
/*258*/ {"Super Heroism",                   0x00000020L, TV_POTION2,      '!', 0,    100,  99,  1, 4, 0, 0, 0, 0, {1, 1}, 3},
/*259*/ {"Boldness",                        0x00000040L, TV_POTION2,      '!', 0,    10,   100, 1, 4, 0, 0, 0, 0, {1, 1}, 1},
/*260*/ {"Restore Life Levels",             0x00000080L, TV_POTION2,      '!', 0,    400,  101, 1, 4, 0, 0, 0, 0, {1, 1}, 40},
/*261*/ {"Resist Heat",                     0x00000100L, TV_POTION2,      '!', 0,    30,   102, 1, 4, 0, 0, 0, 0, {1, 1}, 1},
/*262*/ {"Resist Cold",                     0x00000200L, TV_POTION2,      '!', 0,    30,   103, 1, 4, 0, 0, 0, 0, {1, 1}, 1},
/*263*/ {"Detect Invisible",                0x00000400L, TV_POTION2,      '!', 0,    50,   104, 1, 4, 0, 0, 0, 0, {1, 1}, 3},
/*264*/ {"Slow Poison",                     0x00000800L, TV_POTION2,      '!', 0,    25,   105, 1, 4, 0, 0, 0, 0, {1, 1}, 1},
/*265*/ {"Neutralize Poison",               0x00001000L, TV_POTION2,      '!', 0,    75,   106, 1, 4, 0, 0, 0, 0, {1, 1}, 5},
/*266*/ {"Restore Mana",                    0x00002000L, TV_POTION2,      '!', 0,    350,  107, 1, 4, 0, 0, 0, 0, {1, 1}, 25},
/*267*/ {"Infra-Vision",                    0x00004000L, TV_POTION2,      '!', 0,    20,   108, 1, 4, 0, 0, 0, 0, {1, 1}, 3},
/*268*/ {"& Flask~ of Oil",                 0x00040000L, TV_FLASK,        '!', 7500, 3,    64,  1, 10, 0, 0, 0, 0, {2, 6}, 1},
/*269*/ {"Light",                           0x00000001L, TV_WAND,         '-', 0,    200,  0,   1, 10, 0, 0, 0, 0, {1, 1}, 2},
/*270*/ {"Lightning Bolts",                 0x00000002L, TV_WAND,         '-', 0,    600,  1,   1, 10, 0, 0, 0, 0, {1, 1}, 15},
/*271*/ {"Frost Bolts",                     0x00000004L, TV_WAND,         '-', 0,    800,  2,   1, 10, 0, 0, 0, 0, {1, 1}, 20},
/*272*/ {"Fire Bolts",                      0x00000008L, TV_WAND,         '-', 0,    1000, 3,   1, 10, 0, 0, 0, 0, {1, 1}, 30},
/*273*/ {"Stone-to-Mud",                    0x00000010L, TV_WAND,         '-', 0,    300,  4,   1, 10, 0, 0, 0, 0, {1, 1}, 12},
/*274*/ {"Polymorph",                       0x00000020L, TV_WAND,         '-', 0,    400,  5,   1, 10, 0, 0, 0, 0, {1, 1}, 20},
/*275*/ {"Heal Monster",                    0x00000040L, TV_WAND,         '-', 0,    0,    6,   1, 10, 0, 0, 0, 0, {1, 1}, 2},
/*276*/ {"Haste Monster",                   0x00000080L, TV_WAND,         '-', 0,    0,    7,   1, 10, 0, 0, 0, 0, {1, 1}, 2},
/*277*/ {"Slow Monster",                    0x00000100L, TV_WAND,         '-', 0,    500,  8,   1, 10, 0, 0, 0, 0, {1, 1}, 2},
/*278*/ {"Confuse Monster",                 0x00000200L, TV_WAND,         '-', 0,    400,  9,   1, 10, 0, 0, 0, 0, {1, 1}, 2},
/*279*/ {"Sleep Monster",                   0x00000400L, TV_WAND,         '-', 0,    500,  10,  1, 10, 0, 0, 0, 0, {1, 1}, 7},
/*280*/ {"Drain Life",                      0x00000800L, TV_WAND,         '-', 0,    1200, 11,  1, 10, 0, 0, 0, 0, {1, 1}, 50},
/*281*/ {"Trap/Door Destruction",           0x00001000L, TV_WAND,         '-', 0,    500,  12,  1, 10, 0, 0, 0, 0, {1, 1}, 12},
/*282*/ {"Magic Missile",                   0x00002000L, TV_WAND,         '-', 0,    200,  13,  1, 10, 0, 0, 0, 0, {1, 1}, 2},
/*283*/ {"Wall Building",                   0x00004000L, TV_WAND,         '-', 0,    400,  14,  1, 10, 0, 0, 0, 0, {1, 1}, 25},
/*284*/ {"Clone Monster",                   0x00008000L, TV_WAND,         '-', 0,    0,    15,  1, 10, 0, 0, 0, 0, {1, 1}, 15},
/*285*/ {"Teleport Away",                   0x00010000L, TV_WAND,         '-', 0,    350,  16,  1, 10, 0, 0, 0, 0, {1, 1}, 20},
/*286*/ {"Disarming",                       0x00020000L, TV_WAND,         '-', 0,    500,  17,  1, 10, 0, 0, 0, 0, {1, 1}, 20},
/*287*/ {"Lightning Balls",                 0x00040000L, TV_WAND,         '-', 0,    1200, 18,  1, 10, 0, 0, 0, 0, {1, 1}, 35},
/*288*/ {"Cold Balls",                      0x00080000L, TV_WAND,         '-', 0,    1500, 19,  1, 10, 0, 0, 0, 0, {1, 1}, 40},
/*289*/ {"Fire Balls",                      0x00100000L, TV_WAND,         '-', 0,    1800, 20,  1, 10, 0, 0, 0, 0, {1, 1}, 50},
/*290*/ {"Stinking Cloud",                  0x00200000L, TV_WAND,         '-', 0,    400,  21,  1, 10, 0, 0, 0, 0, {1, 1}, 5},
/*291*/ {"Acid Balls",                      0x00400000L, TV_WAND,         '-', 0,    1650, 22,  1, 10, 0, 0, 0, 0, {1, 1}, 48},
/*292*/ {"Wonder",                          0x00800000L, TV_WAND,         '-', 0,    250,  23,  1, 10, 0, 0, 0, 0, {1, 1}, 2},
/*293*/ {"Light",                           0x00000001L, TV_STAFF,        '_', 0,    250,  0,   1, 50, 0, 0, 0, 0, {1, 2}, 5},
/*294*/ {"Door/Stair Location",             0x00000002L, TV_STAFF,        '_', 0,    350,  1,   1, 50, 0, 0, 0, 0, {1, 2}, 10},
/*295*/ {"Trap Location",                   0x00000004L, TV_STAFF,        '_', 0,    350,  2,   1, 50, 0, 0, 0, 0, {1, 2}, 10},
/*296*/ {"Treasure Location",               0x00000008L, TV_STAFF,        '_', 0,    200,  3,   1, 50, 0, 0, 0, 0, {1, 2}, 5},
/*297*/ {"Object Location",                 0x00000010L, TV_STAFF,        '_', 0,    200,  4,   1, 50, 0, 0, 0, 0, {1, 2}, 5},
/*298*/ {"Teleportation",                   0x00000020L, TV_STAFF,        '_', 0,    800,  5,   1, 50, 0, 0, 0, 0, {1, 2}, 20},
/*299*/ {"Earthquakes",                     0x00000040L, TV_STAFF,        '_', 0,    350,  6,   1, 50, 0, 0, 0, 0, {1, 2}, 40},
/*300*/ {"Summoning",                       0x00000080L, TV_STAFF,        '_', 0,    0,    7,   1, 50, 0, 0, 0, 0, {1, 2}, 10},
/*301*/ {"Summoning",                       0x00000080L, TV_STAFF,        '_', 0,    0,    7,   1, 50, 0, 0, 0, 0, {1, 2}, 50},
/*302*/ {"*Destruction*",                   0x00000200L, TV_STAFF,        '_', 0,    2500, 8,   1, 50, 0, 0, 0, 0, {1, 2}, 50},
/*303*/ {"Starlight",                       0x00000400L, TV_STAFF,        '_', 0,    400,  9,   1, 50, 0, 0, 0, 0, {1, 2}, 20},
/*304*/ {"Haste Monsters",                  0x00000800L, TV_STAFF,        '_', 0,    0,    10,  1, 50, 0, 0, 0, 0, {1, 2}, 10},
/*305*/ {"Slow Monsters",                   0x00001000L, TV_STAFF,        '_', 0,    800,  11,  1, 50, 0, 0, 0, 0, {1, 2}, 10},
/*306*/ {"Sleep Monsters",                  0x00002000L, TV_STAFF,        '_', 0,    700,  12,  1, 50, 0, 0, 0, 0, {1, 2}, 10},
/*307*/ {"Cure Light Wounds",               0x00004000L, TV_STAFF,        '_', 0,    200,  13,  1, 50, 0, 0, 0, 0, {1, 2}, 5},
/*308*/ {"Detect Invisible",                0x00008000L, TV_STAFF,        '_', 0,    200,  14,  1, 50, 0, 0, 0, 0, {1, 2}, 5},
/*309*/ {"Speed",                           0x00010000L, TV_STAFF,        '_', 0,    1000, 15,  1, 50, 0, 0, 0, 0, {1, 2}, 40},
/*310*/ {"Slowness",                        0x00020000L, TV_STAFF,        '_', 0,    0,    16,  1, 50, 0, 0, 0, 0, {1, 2}, 40},
/*311*/ {"Mass Polymorph",                  0x00040000L, TV_STAFF,        '_', 0,    750,  17,  1, 50, 0, 0, 0, 0, {1, 2}, 46},
/*312*/ {"Remove Curse",                    0x00080000L, TV_STAFF,        '_', 0,    500,  18,  1, 50, 0, 0, 0, 0, {1, 2}, 47},
/*313*/ {"Detect Evil",                     0x00100000L, TV_STAFF,        '_', 0,    350,  19,  1, 50, 0, 0, 0, 0, {1, 2}, 20},
/*314*/ {"Curing",                          0x00200000L, TV_STAFF,        '_', 0,    1000, 20,  1, 50, 0, 0, 0, 0, {1, 2}, 25},
/*315*/ {"Dispel Evil",                     0x00400000L, TV_STAFF,        '_', 0,    1200, 21,  1, 50, 0, 0, 0, 0, {1, 2}, 49},
/*316*/ {"Darkness",                        0x01000000L, TV_STAFF,        '_', 0,    0,    22,  1, 50, 0, 0, 0, 0, {1, 2}, 50},
/*317*/ {"Darkness",                        0x01000000L, TV_STAFF,        '_', 0,    0,    22,  1, 50, 0, 0, 0, 0, {1, 2}, 5},
/*318*/ {"[Beginners-Magick]",              0x0000007FL, TV_MAGIC_BOOK,   '?', 0,    25,   64,  1, 30, 0, 0, 0, 0, {1, 1}, 40},
/*319*/ {"[Magick I]",                      0x0000FF80L, TV_MAGIC_BOOK,   '?', 0,    100,  65,  1, 30, 0, 0, 0, 0, {1, 1}, 40},
/*320*/ {"[Magick II]",                     0x00FF0000L, TV_MAGIC_BOOK,   '?', 0,    400,  66,  1, 30, 0, 0, 0, 0, {1, 1}, 40},
/*321*/ {"[The Mages' Guide to Power]",     0x7F000000L, TV_MAGIC_BOOK,   '?', 0,    800,  67,  1, 30, 0, 0, 0, 0, {1, 1}, 40},
/*322*/ {"[Beginners Handbook]",            0x000000FFL, TV_PRAYER_BOOK,  '?', 0,    25,   64,  1, 30, 0, 0, 0, 0, {1, 1}, 40},
/*323*/ {"[Words of Wisdom]",               0x0000FF00L, TV_PRAYER_BOOK,  '?', 0,    100,  65,  1, 30, 0, 0, 0, 0, {1, 1}, 40},
/*324*/ {"[Chants and Blessings]",          0x01FF0000L, TV_PRAYER_BOOK,  '?', 0,    400,  66,  1, 30, 0, 0, 0, 0, {1, 1}, 40},
/*325*/ {"[Exorcisms and Dispellings]",     0x7E000000L, TV_PRAYER_BOOK,  '?', 0,    800,  67,  1, 30, 0, 0, 0, 0, {1, 1}, 40},
/*326*/ {"& Small Wooden Chest",            0x13800000L, TV_CHEST,        '&', 0,    20,   1,   1, 250, 0, 0, 0, 0, {2, 3}, 7},
/*327*/ {"& Large Wooden Chest",            0x17800000L, TV_CHEST,        '&', 0,    60,   4,   1, 500, 0, 0, 0, 0, {2, 5}, 15},
/*328*/ {"& Small Iron Chest",              0x17800000L, TV_CHEST,        '&', 0,    100,  7,   1, 500, 0, 0, 0, 0, {2, 4}, 25},
/*329*/ {"& Large Iron Chest",              0x23800000L, TV_CHEST,        '&', 0,    150,  10,  1, 1000, 0, 0, 0, 0, {2, 6}, 35},
/*330*/ {"& Small Steel Chest",             0x1B800000L, TV_CHEST,        '&', 0,    200,  13,  1, 500, 0, 0, 0, 0, {2, 4}, 45},
/*331*/ {"& Large Steel Chest",             0x33800000L, TV_CHEST,        '&', 0,    250,  16,  1, 1000, 0, 0, 0, 0, {2, 6}, 50},
/*332*/ {"& Rat Skeleton",                  0x00000000L, TV_MISC,         's', 0,    0,    1,   1, 10, 0, 0, 0, 0, {1, 1}, 1},
/*333*/ {"& Giant Centipede Skeleton",      0x00000000L, TV_MISC,         's', 0,    0,    2,   1, 25, 0, 0, 0, 0, {1, 1}, 1},
/*334*/ {"some Filthy Rags",                0x00000000L, TV_SOFT_ARMOR,   '~', 0,    0,    63,  1, 20, 0, 0, 1, 0, {0, 0}, 0},
/*335*/ {"& empty bottle",                  0x00000000L, TV_MISC,         '!', 0,    0,    4,   1, 2, 0, 0, 0, 0, {1, 1}, 0},
/*336*/ {"some shards of pottery",          0x00000000L, TV_MISC,         '~', 0,    0,    5,   1, 5, 0, 0, 0, 0, {1, 1}, 0},
/*337*/ {"& Human Skeleton",                0x00000000L, TV_MISC,         's', 0,    0,    7,   1, 60, 0, 0, 0, 0, {1, 1}, 1},
/*338*/ {"& Dwarf Skeleton",                0x00000000L, TV_MISC,         's', 0,    0,    8,   1, 50, 0, 0, 0, 0, {1, 1}, 1},
/*339*/ {"& Elf Skeleton",                  0x00000000L, TV_MISC,         's', 0,    0,    9,   1, 40, 0, 0, 0, 0, {1, 1}, 1},
/*340*/ {"& Gnome Skeleton",                0x00000000L, TV_MISC,         's', 0,    0,    10,  1, 25, 0, 0, 0, 0, {1, 1}, 1},
/*341*/ {"& broken set of teeth",           0x00000000L, TV_MISC,         's', 0,    0,    11,  1, 3, 0, 0, 0, 0, {1, 1}, 0},
/*342*/ {"& large broken bone",             0x00000000L, TV_MISC,         's', 0,    0,    12,  1, 2, 0, 0, 0, 0, {1, 1}, 0},
/*343*/ {"& broken stick",                  0x00000000L, TV_MISC,         '~', 0,    0,    13,  1, 3, 0, 0, 0, 0, {1, 1}, 0},
/* end of Dungeon items */

/* Store items, which are not also dungeon items, some of these can be found above, except that the number is >1 below */
/*344*/ {"& Ration~ of Food",               0x00000000L, TV_FOOD,         ',', 5000,   3,  90, 5, 10,  0, 0, 0, 0, {0, 0}, 0},
/*345*/ {"& Hard Biscuit~",                 0x00000000L, TV_FOOD,         ',', 500,    1,  93, 5,  2,  0, 0, 0, 0, {0, 0}, 0},
/*346*/ {"& Strip~ of Beef Jerky",          0x00000000L, TV_FOOD,         ',', 1750,   2,  94, 5,  4,  0, 0, 0, 0, {0, 0}, 0},
/*347*/ {"& Pint~ of Fine Ale",             0x00000000L, TV_FOOD,         ',', 500,    1,  95, 3, 10,  0, 0, 0, 0, {0, 0}, 0},
/*348*/ {"& Pint~ of Fine Wine",            0x00000000L, TV_FOOD,         ',', 400,    2,  96, 1, 10,  0, 0, 0, 0, {0, 0}, 0},
/*349*/ {"& Pick",                          0x20000000L, TV_DIGGING,     '\\', 1,     50,   1, 1, 150, 0, 0, 0, 0, {1, 3}, 0},
/*350*/ {"& Shovel",                        0x20000000L, TV_DIGGING,     '\\', 0,     15,   4, 1, 60,  0, 0, 0, 0, {1, 2}, 0},
/*351*/ {"Identify",                        0x00000008L, TV_SCROLL1,      '?', 0,     50,  67, 2,  5,  0, 0, 0, 0, {0, 0}, 0},
/*352*/ {"Light",                           0x00000020L, TV_SCROLL1,      '?', 0,     15,  69, 3,  5,  0, 0, 0, 0, {0, 0}, 0},
/*353*/ {"Phase Door",                      0x00000080L, TV_SCROLL1,      '?', 0,     15,  71, 2,  5,  0, 0, 0, 0, {0, 0}, 0},
/*354*/ {"Magic Mapping",                   0x00000800L, TV_SCROLL1,      '?', 0,     40,  75, 2,  5,  0, 0, 0, 0, {0, 0}, 0},
/*355*/ {"Treasure Detection",              0x00004000L, TV_SCROLL1,      '?', 0,     15,  78, 2,  5,  0, 0, 0, 0, {0, 0}, 0},
/*356*/ {"Object Detection",                0x00008000L, TV_SCROLL1,      '?', 0,     15,  79, 2,  5,  0, 0, 0, 0, {0, 0}, 0},
/*357*/ {"Detect Invisible",                0x00080000L, TV_SCROLL1,      '?', 0,     15,  83, 2,  5,  0, 0, 0, 0, {0, 0}, 0},
/*358*/ {"Blessing",                        0x00000020L, TV_SCROLL2,      '?', 0,     15,  99, 2,  5,  0, 0, 0, 0, {0, 0}, 0},
/*359*/ {"Word-of-Recall",                  0x00000100L, TV_SCROLL2,      '?', 0,    150, 102, 3,  5,  0, 0, 0, 0, {0, 0}, 0},
/*360*/ {"Cure Light Wounds",               0x10001000L, TV_POTION1,      '!', 50,    15,  79, 2,  4,  0, 0, 0, 0, {1, 1}, 0},
/*361*/ {"Heroism",                         0x00000010L, TV_POTION2,      '!', 0,     35,  98, 2,  4,  0, 0, 0, 0, {1, 1}, 0},
/*362*/ {"Boldness",                        0x00000040L, TV_POTION2,      '!', 0,     10, 100, 2,  4,  0, 0, 0, 0, {1, 1}, 0},
/*363*/ {"Slow Poison",                     0x00000800L, TV_POTION2,      '!', 0,     25, 105, 2,  4,  0, 0, 0, 0, {1, 1}, 0},
/*364*/ {"& Brass Lantern~",                0x00000000L, TV_LIGHT,        '~', 7500,  35,   0, 1, 50,  0, 0, 0, 0, {1, 1}, 1},
/*365*/ {"& Wooden Torch~",                 0x00000000L, TV_LIGHT,        '~', 4000,   2, 192, 5, 30,  0, 0, 0, 0, {1, 1}, 1},
/*366*/ {"& Flask~ of Oil",                 0x00040000L, TV_FLASK,        '!', 7500,   3,  64, 5, 10,  0, 0, 0, 0, {2, 6}, 1},
/* end store items */

/* start doors */
/* Secret door must have same subval as closed door in */  /* TRAP_LISTB.  See CHANGE_TRAP. Must use & because of stone_to_mud. */
/* 367 */ {"& open door",                   0x00000000L, TV_OPEN_DOOR,   '\'', 0, 0,  1, 1, 0, 0, 0, 0, 0, {1, 1}, 0},
/* 368 */ {"& closed door",                 0x00000000L, TV_CLOSED_DOOR,  '+', 0, 0, 19, 1, 0, 0, 0, 0, 0, {1, 1}, 0},
/* 369 */ {"& secret door",                 0x00000000L, TV_SECRET_DOOR,  '#', 0, 0, 19, 1, 0, 0, 0, 0, 0, {1, 1}, 0},
/* end doors */

/* stairs */
/* 370 */ {"an up staircase",               0x00000000L, TV_UP_STAIR,     '<', 0, 0, 1, 1, 0, 0, 0, 0, 0, {1, 1}, 0},
/* 371 */ {"a down staircase",              0x00000000L, TV_DOWN_STAIR,   '>', 0, 0, 1, 1, 0, 0, 0, 0, 0, {1, 1}, 0},

/* store door */
/* Stores are just special traps */
/* 372 */ {"General Store",                 0x00000000L, TV_STORE_DOOR,   '1', 0, 0, 101, 1, 0, 0, 0, 0, 0, {0, 0}, 0},
/* 373 */ {"Armory",                        0x00000000L, TV_STORE_DOOR,   '2', 0, 0, 102, 1, 0, 0, 0, 0, 0, {0, 0}, 0},
/* 374 */ {"Weapon Smiths",                 0x00000000L, TV_STORE_DOOR,   '3', 0, 0, 103, 1, 0, 0, 0, 0, 0, {0, 0}, 0},
/* 375 */ {"Temple",                        0x00000000L, TV_STORE_DOOR,   '4', 0, 0, 104, 1, 0, 0, 0, 0, 0, {0, 0}, 0},
/* 376 */ {"Alchemy Shop",                  0x00000000L, TV_STORE_DOOR,   '5', 0, 0, 105, 1, 0, 0, 0, 0, 0, {0, 0}, 0},
/* 377 */ {"Magic Shop",                    0x00000000L, TV_STORE_DOOR,   '6', 0, 0, 106, 1, 0, 0, 0, 0, 0, {0, 0}, 0},
/* end store door */

/* Traps are just Nasty treasures. */
/* Traps: Level represents the relative difficulty of disarming; */
/* and P1 represents the experienced gained when disarmed*/
/* 378 */ {"an open pit",                   0x00000000L, TV_VIS_TRAP,     ' ',  1, 0,  1, 1, 0, 0, 0, 0, 0, {2, 6},  50},
/* 379 */ {"an arrow trap",                 0x00000000L, TV_INVIS_TRAP,   '^',  3, 0,  2, 1, 0, 0, 0, 0, 0, {1, 8},  90},
/* 380 */ {"a covered pit",                 0x00000000L, TV_INVIS_TRAP,   '^',  2, 0,  3, 1, 0, 0, 0, 0, 0, {2, 6},  60},
/* 381 */ {"a trap door",                   0x00000000L, TV_INVIS_TRAP,   '^',  5, 0,  4, 1, 0, 0, 0, 0, 0, {2, 8},  75},
/* 382 */ {"a gas trap",                    0x00000000L, TV_INVIS_TRAP,   '^',  3, 0,  5, 1, 0, 0, 0, 0, 0, {1, 4},  95},
/* 383 */ {"a loose rock",                  0x00000000L, TV_INVIS_TRAP,   ';',  0, 0,  6, 1, 0, 0, 0, 0, 0, {0, 0},  10},
/* 384 */ {"a dart trap",                   0x00000000L, TV_INVIS_TRAP,   '^',  5, 0,  7, 1, 0, 0, 0, 0, 0, {1, 4}, 110},
/* 385 */ {"a strange rune",                0x00000000L, TV_INVIS_TRAP,   '^',  5, 0,  8, 1, 0, 0, 0, 0, 0, {0, 0},  90},
/* 386 */ {"some loose rock",               0x00000000L, TV_INVIS_TRAP,   '^',  5, 0,  9, 1, 0, 0, 0, 0, 0, {2, 6},  90},
/* 387 */ {"a gas trap",                    0x00000000L, TV_INVIS_TRAP,   '^', 10, 0, 10, 1, 0, 0, 0, 0, 0, {1, 4}, 105},
/* 388 */ {"a strange rune",                0x00000000L, TV_INVIS_TRAP,   '^',  5, 0, 11, 1, 0, 0, 0, 0, 0, {0, 0},  90},
/* 389 */ {"a blackened spot",              0x00000000L, TV_INVIS_TRAP,   '^', 10, 0, 12, 1, 0, 0, 0, 0, 0, {4, 6}, 110},
/* 390 */ {"some corroded rock",            0x00000000L, TV_INVIS_TRAP,   '^', 10, 0, 13, 1, 0, 0, 0, 0, 0, {4, 6}, 110},
/* 391 */ {"a gas trap",                    0x00000000L, TV_INVIS_TRAP,   '^',  5, 0, 14, 1, 0, 0, 0, 0, 0, {2, 6}, 105},
/* 392 */ {"a gas trap",                    0x00000000L, TV_INVIS_TRAP,   '^',  5, 0, 15, 1, 0, 0, 0, 0, 0, {1, 4}, 110},
/* 393 */ {"a gas trap",                    0x00000000L, TV_INVIS_TRAP,   '^',  5, 0, 16, 1, 0, 0, 0, 0, 0, {1, 8}, 105},
/* 394 */ {"a dart trap",                   0x00000000L, TV_INVIS_TRAP,   '^',  5, 0, 17, 1, 0, 0, 0, 0, 0, {1, 8}, 110},
/* 395 */ {"a dart trap",                   0x00000000L, TV_INVIS_TRAP,   '^',  5, 0, 18, 1, 0, 0, 0, 0, 0, {1, 8}, 110},

/* rubble */
 /* 396 */ {"some rubble",                  0x00000000L, TV_RUBBLE,   ':', 0, 0, 1, 1, 0, 0, 0, 0, 0, {0, 0}, 0},

/* mush */
/* 397 */ {"& Pint~ of Fine Grade Mush",    0x00000000L, TV_FOOD,     ',', 1500, 1, 97, 1, 1, 0, 0, 0, 0, {1, 1}, 1},

/* Special trap */
/* 398 */ {"a strange rune",                0x00000000L, TV_VIS_TRAP, '^', 0, 0, 99, 1, 0, 0, 0, 0, 0, {0, 0}, 10},

/* Gold list (All types of gold and gems are defined here) */
/* 399 */ {"copper",                        0x00000000L, TV_GOLD,     '$', 0,  3,  1, 1, 0, 0, 0, 0, 0, {0, 0}, 1},
/* 400 */ {"copper",                        0x00000000L, TV_GOLD,     '$', 0,  4,  2, 1, 0, 0, 0, 0, 0, {0, 0}, 1},
/* 401 */ {"copper",                        0x00000000L, TV_GOLD,     '$', 0,  5,  3, 1, 0, 0, 0, 0, 0, {0, 0}, 1},
/* 402 */ {"silver",                        0x00000000L, TV_GOLD,     '$', 0,  6,  4, 1, 0, 0, 0, 0, 0, {0, 0}, 1},
/* 403 */ {"silver",                        0x00000000L, TV_GOLD,     '$', 0,  7,  5, 1, 0, 0, 0, 0, 0, {0, 0}, 1},
/* 404 */ {"silver",                        0x00000000L, TV_GOLD,     '$', 0,  8,  6, 1, 0, 0, 0, 0, 0, {0, 0}, 1},
/* 405 */ {"garnets",                       0x00000000L, TV_GOLD,     '*', 0,  9,  7, 1, 0, 0, 0, 0, 0, {0, 0}, 1},
/* 406 */ {"garnets",                       0x00000000L, TV_GOLD,     '*', 0, 10,  8, 1, 0, 0, 0, 0, 0, {0, 0}, 1},
/* 407 */ {"gold",                          0x00000000L, TV_GOLD,     '$', 0, 12,  9, 1, 0, 0, 0, 0, 0, {0, 0}, 1},
/* 408 */ {"gold",                          0x00000000L, TV_GOLD,     '$', 0, 14, 10, 1, 0, 0, 0, 0, 0, {0, 0}, 1},
/* 409 */ {"gold",                          0x00000000L, TV_GOLD,     '$', 0, 16, 11, 1, 0, 0, 0, 0, 0, {0, 0}, 1},
/* 410 */ {"opals",                         0x00000000L, TV_GOLD,     '*', 0, 18, 12, 1, 0, 0, 0, 0, 0, {0, 0}, 1},
/* 411 */ {"sapphires",                     0x00000000L, TV_GOLD,     '*', 0, 20, 13, 1, 0, 0, 0, 0, 0, {0, 0}, 1},
/* 412 */ {"gold",                          0x00000000L, TV_GOLD,     '$', 0, 24, 14, 1, 0, 0, 0, 0, 0, {0, 0}, 1},
/* 413 */ {"rubies",                        0x00000000L, TV_GOLD,     '*', 0, 28, 15, 1, 0, 0, 0, 0, 0, {0, 0}, 1},
/* 414 */ {"diamonds",                      0x00000000L, TV_GOLD,     '*', 0, 32, 16, 1, 0, 0, 0, 0, 0, {0, 0}, 1},
/* 415 */ {"emeralds",                      0x00000000L, TV_GOLD,     '*', 0, 40, 17, 1, 0, 0, 0, 0, 0, {0, 0}, 1},
/* 416 */ {"mithril",                       0x00000000L, TV_GOLD,     '$', 0, 80, 18, 1, 0, 0, 0, 0, 0, {0, 0}, 1},

/* nothing, used as inventory place holder */  /* must be stackable, so that can be picked up by inven_carry */
/* 417 */ {"nothing",                       0x00000000L, TV_NOTHING,  ' ', 0, 0, 64, 0, 0, 0, 0, 0, 0, {0, 0}, 0},

/* these next two are needed only for the names */
/* 418 */ {"& ruined chest",                0x00000000L, TV_CHEST,    '&', 0, 0, 0, 1, 250, 0, 0, 0, 0, {0, 0}, 0},
/* 419 */ {"",                              0x00000000L, TV_NOTHING,  ' ', 0, 0, 0, 0,   0, 0, 0, 0, 0, {0, 0}, 0}
};

char *special_names[SN_ARRAY_SIZE] = {
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
    "(Unlocked)",        "of Slay Animal"
};

int16 sorted_objects[MAX_DUNGEON_OBJ];

/* Identified objects flags */
int8u object_ident[OBJECT_IDENT_SIZE];
int16 t_level[MAX_OBJ_LEVEL + 1];
inven_type t_list[MAX_TALLOC];
inven_type inventory[INVEN_ARRAY_SIZE];

/* Treasure related values */
int16 inven_ctr = 0;    /* Total different obj's */
int16 inven_weight = 0; /* Cur carried weight */
int16 equip_ctr = 0;    /* Cur equipment ctr */
int16 tcptr;            /* Cur treasure heap ptr */
