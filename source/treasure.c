/* treasur1.c: dungeon object definitions

   Copyright (c) 1989 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include "constant.h"
#include "config.h"
#include "types.h"

/* Following are treasure arrays	and variables			*/

/* Object description:	Objects are defined here.  Each object has
  the following attributes:
	Descriptor : Name of item and formats.
		& is replaced with 'a', 'an', or a number.
		~ is replaced with null or 's'.
	Character  : Character that represents the item.
	Type value : Value representing the type of object.
	Sub value  : separate value for each item of a type.
		0 - 63: object can not stack
		64 - 127: dungeon object, can stack with other D object
		128 - 191: unused, previously for store items
		192: stack with other iff have same p1 value, always
			treated as individual objects
		193 - 255: object can stack with others iff they have
			the same p1 value, usually considered one group
		Objects which have two type values, e.g. potions and
		scrolls, need to have distinct subvals for
		each item regardless of its tval
	Damage	   : amount of damage item can cause.
	Weight	   : relative weight of an item.
	Number	   : number of items appearing in group.
	To hit	   : magical plusses to hit.
	To damage  : magical plusses to damage.
	AC	   : objects relative armor class.
		1 is worse than 5 is worse than 10 etc.
	To AC	   : Magical bonuses to AC.
	P1	   : Catch all for magical abilities such as
		     plusses to strength, minuses to searching.
	Flags	   : Abilities of object.  Each ability is a
		     bit.  Bits 1-31 are used. (Signed integer)
	Level	   : Minimum level on which item can be found.
	Cost	   : Relative cost of item.

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
	Traps are added randomly by magic_init() in misc.c.	*/

/* Object list (All objects must be defined here)		 */

#ifdef MACGAME
treasure_type *object_list;
#else
treasure_type object_list[MAX_OBJECTS] = {
/* Dungeon items from 0 to MAX_DUNGEON_OBJ */
{"Poison"			,0x00000001L,	TV_FOOD, ',',	/*  0*/
  500,	   0,	64,   1,   1,	0,   0,	 0,   0, {0,0}	,  7},
{"Blindness"			,0x00000002L,	TV_FOOD, ',',	/*  1*/
  500,	   0,	65,   1,   1,	0,   0,	 0,   0, {0,0}	,  9},
{"Paranoia"			,0x00000004L,	TV_FOOD, ',',	/*  2*/
  500,	   0,	66,   1,   1,	0,   0,	 0,   0, {0,0}	,  9},
{"Confusion"			,0x00000008L,	TV_FOOD, ',',	/*  3*/
  500,	   0,	67,   1,   1,	0,   0,	 0,   0, {0,0}	,  7},
{"Hallucination"		,0x00000010L,	TV_FOOD, ',',	/*  4*/
  500,	   0,	68,   1,   1,	0,   0,	 0,   0, {0,0}	, 13},
{"Cure Poison"			,0x00000020L,	TV_FOOD, ',',	/*  5*/
  500,	  60,	69,   1,   1,	0,   0,	 0,   0, {0,0}	,  8},
{"Cure Blindness"		,0x00000040L,	TV_FOOD, ',',	/*  6*/
  500,	  50,	70,   1,   1,	0,   0,	 0,   0, {0,0}	, 10},
{"Cure Paranoia"		,0x00000080L,	TV_FOOD, ',',	/*  7*/
  500,	  25,	71,   1,   1,	0,   0,	 0,   0, {0,0}	, 12},
{"Cure Confusion"		,0x00000100L,	TV_FOOD, ',',	/*  8*/
  500,	  50,	72,   1,   1,	0,   0,	 0,   0, {0,0}	,  6},
{"Weakness"			,0x04000200L,	TV_FOOD, ',',	/*  9*/
  500,	   0,	73,   1,   1,	0,   0,	 0,   0, {0,0}	,  7},
{"Unhealth"			,0x04000400L,	TV_FOOD, ',',	/* 10*/
  500,	  50,	74,   1,   1,	0,   0,	 0,   0, {10,10}, 15},
{"Restore Constitution"		,0x00010000L,	TV_FOOD, ',',	/* 11*/
  500,	 350,	75,   1,   1,	0,   0,	 0,   0, {0,0}	, 20},
{"First-Aid"			,0x00200000L,	TV_FOOD, ',',	/* 12*/
  500,	   5,	76,   1,   1,	0,   0,	 0,   0, {0,0}	,  6},
{"Minor Cures"			,0x00400000L,	TV_FOOD, ',',	/* 13*/
  500,	  20,	77,   1,   1,	0,   0,	 0,   0, {0,0}	,  7},
{"Light Cures"			,0x00800000L,	TV_FOOD, ',',	/* 14*/
  500,	  30,	78,   1,   1,	0,   0,	 0,   0, {0,0}	, 10},
{"Restoring"			,0x001F8040L,	TV_FOOD, ',',	/* 15*/
  500,	1000,	79,   1,   1,	0,   0,	 0,   0, {0,0}	, 30},
{"Poison"			,0x00000001L,	TV_FOOD, ',',	/* 16*/
 1200,	   0,	80,   1,   1,	0,   0,	 0,   0, {0,0}	, 15},
{"Hallucinations"		,0x00000010L,	TV_FOOD, ',',	/* 17*/
 1200,	   0,	81,   1,   1,	0,   0,	 0,   0, {0,0}	, 18},
{"Cure Poison"			,0x00000020L,	TV_FOOD, ',',	/* 18*/
 1200,	  75,	82,   1,   1,	0,   0,	 0,   0, {0,0}	, 19},
{"Unhealth"			,0x00000400L,	TV_FOOD, ',',	/* 19*/
 1200,	  25,	83,   1,   1,	0,   0,	 0,   0, {6,8}	, 28},
{"Cure Serious Wounds"		,0x02000000L,	TV_FOOD, ',',	/* 20*/
 1200,	  75,	84,   1,   2,	0,   0,	 0,   0, {0,0}	, 16},
{"& Ration~ of Food"		,0x00000000L,	TV_FOOD, ',',	/* 21*/
 5000,	   3,	90,   1,  10,	0,   0,	 0,   0, {0,0}	,  0},
{"& Ration~ of Food"		,0x00000000L,	TV_FOOD, ',',	/* 22*/
 5000,	   3,	90,   1,  10,	0,   0,	 0,   0, {0,0}	,  5},
{"& Ration~ of Food"		,0x00000000L,	TV_FOOD, ',',	/* 23*/
 5000,	   3,	90,   1,  10,	0,   0,	 0,   0, {0,0}	, 10},
{"& Slime Mold~"		,0x00000000L,	TV_FOOD, ',',	/* 24*/
 3000,	   2,	91,   1,   5,	0,   0,	 0,   0, {0,0}	,  1},
{"& Piece~ of Elvish Waybread"	,0x02000020L,	TV_FOOD, ',',	/* 25*/
 7500,	  10,	92,   1,   3,	0,   0,	 0,   0, {0,0}	,  6},
{"& Piece~ of Elvish Waybread"	,0x02000020L,	TV_FOOD, ',',	/* 26*/
 7500,	  10,	92,   1,   3,	0,   0,	 0,   0, {0,0}	, 12},
{"& Piece~ of Elvish Waybread"	,0x02000020L,	TV_FOOD, ',',	/* 27*/
 7500,	  10,	92,   1,   3,	0,   0,	 0,   0, {0,0}	, 20},
{"& Dagger (Main Gauche)"	,0x00000000L,	TV_SWORD, '|',	/* 28*/
    0,	  25,	1,   1,  30,	0,   0,	 0,   0, {1,5}	,  2},
{"& Dagger (Misericorde)"	,0x00000000L,	TV_SWORD, '|',	/* 29*/
    0,	  10,	2,   1,  15,	0,   0,	 0,   0, {1,4}	,  0},
{"& Dagger (Stiletto)"		,0x00000000L,	TV_SWORD, '|',	/* 30*/
    0,	  10,	3,   1,  12,	0,   0,	 0,   0, {1,4}	,  0},
{"& Dagger (Bodkin)"		,0x00000000L,	TV_SWORD, '|',	/* 31*/
    0,	  10,	4,   1,  20,	0,   0,	 0,   0, {1,4}	,  1},
{"& Broken dagger"		,0x00000000L,	TV_SWORD, '|',	/* 32*/
    0,	   0,	5,   1,  15,  -2,  -2,	 0,   0, {1,1}	,  0},
{"& Backsword"			,0x00000000L,	TV_SWORD, '|',	/* 33*/
    0,	  60,	6,   1,  95,	0,   0,	 0,   0, {1,9}	,  7},
{"& Bastard Sword"		,0x00000000L,	TV_SWORD, '|',	/* 34*/
    0,	 350,	7,   1, 140,	0,   0,	 0,   0, {3,4}	, 14},
{"& Thrusting Sword (Bilbo)"	,0x00000000L,	TV_SWORD, '|',	/* 35*/
    0,	  60,	8,   1,  80,	0,   0,	 0,   0, {1,6}	,  4},
{"& Thrusting Sword (Baselard)"	,0x00000000L,	TV_SWORD, '|',	/* 36*/
    0,	  80,	9,   1, 100,	0,   0,	 0,   0, {1,7}	,  5},
{"& Broadsword"			,0x00000000L,	TV_SWORD, '|',	/* 37*/
    0,	 255,	10,   1, 150,	0,   0,	 0,   0, {2,5}	,  9},
{"& Two-Handed Sword (Claymore)",0x00000000L,	TV_SWORD, '|',	/* 38*/
    0,	 775,	11,   1, 200,	0,   0,	 0,   0, {3,6}	, 30},
{"& Cutlass"			,0x00000000L,	TV_SWORD, '|',	/* 39*/
    0,	  85,	12,   1, 110,	0,   0,	 0,   0, {1,7}	,  7},
{"& Two-Handed Sword (Espadon)"	,0x00000000L,	TV_SWORD, '|',	/* 40*/
    0,	 655,	13,   1, 180,	0,   0,	 0,   0, {3,6}	, 35},
{"& Executioner's Sword"	,0x00000000L,	TV_SWORD, '|',	/* 41*/
    0,	 850,	14,   1, 260,	0,   0,	 0,   0, {4,5}	, 40},
{"& Two-Handed Sword (Flamberge)",0x00000000L,	TV_SWORD, '|',	/* 42*/
    0,	1000,	15,   1, 240,	0,   0,	 0,   0, {4,5}	, 45},
{"& Foil"			,0x00000000L,	TV_SWORD, '|',	/* 43*/
    0,	  35,	16,   1,  30,	0,   0,	 0,   0, {1,5}	,  2},
{"& Katana"			,0x00000000L,	TV_SWORD, '|',	/* 44*/
    0,	 400,	17,   1, 120,	0,   0,	 0,   0, {3,4}	, 18},
{"& Longsword"			,0x00000000L,	TV_SWORD, '|',	/* 45*/
    0,	 300,	18,   1, 130,	0,   0,	 0,   0, {1,10} , 12},
{"& Two-Handed Sword (No-Dachi)",0x00000000L,	TV_SWORD, '|',	/* 46*/
    0,	 675,	19,   1, 200,	0,   0,	 0,   0, {4,4}	, 45},
{"& Rapier"			,0x00000000L,	TV_SWORD, '|',	/* 47*/
    0,	  42,	20,   1,  40,	0,   0,	 0,   0, {1,6}	,  4},
{"& Sabre"			,0x00000000L,	TV_SWORD, '|',	/* 48*/
    0,	  50,	21,   1,  50,	0,   0,	 0,   0, {1,7}	,  5},
{"& Small Sword"		,0x00000000L,	TV_SWORD, '|',	/* 49*/
    0,	  48,	22,   1,  75,	0,   0,	 0,   0, {1,6}	,  5},
{"& Two-Handed Sword (Zweihander)",0x00000000L,	TV_SWORD, '|',	/* 50*/
    0,	1000,	23,   1, 280,	0,   0,	 0,   0, {4,6}	, 50},
{"& Broken sword"		,0x00000000L,	TV_SWORD, '|',	/* 51*/
    0,	   0,	24,   1,  75,  -2,  -2,	 0,   0, {1,1}	,  0},
{"& Ball and Chain"		,0x00000000L,	TV_HAFTED, '\\',/* 52*/
    0,	 200,	1,   1, 150,	0,   0,	 0,   0, {2,4}	, 20},
{"& Cat-O-Nine Tails"		,0x00000000L,	TV_HAFTED, '\\',/* 53*/
    0,	  14,	2,   1,  40,	0,   0,	 0,   0, {1,4}	,  3},
{"& Wooden Club"		,0x00000000L,	TV_HAFTED, '\\',/* 54*/
    0,	  10,	3,   1, 100,	0,   0,	 0,   0, {1,3}	,  0},
{"& Flail"			,0x00000000L,	TV_HAFTED, '\\',/* 55*/
    0,	 353,	4,   1, 150,	0,   0,	 0,   0, {2,6}	, 12},
{"& Two-Handed Great Flail"	,0x00000000L,	TV_HAFTED, '\\',/* 56*/
    0,	 590,	5,   1, 280,	0,   0,	 0,   0, {3,6}	, 45},
{"& Morningstar"		,0x00000000L,	TV_HAFTED, '\\',/* 57*/
    0,	 396,	6,   1, 150,	0,   0,	 0,   0, {2,6}	, 10},
{"& Mace"			,0x00000000L,	TV_HAFTED, '\\',/* 58*/
    0,	 130,	7,   1, 120,	0,   0,	 0,   0, {2,4}	,  6},
{"& War Hammer"			,0x00000000L,	TV_HAFTED, '\\',/* 59*/
    0,	 225,	8,   1, 120,	0,   0,	 0,   0, {3,3}	,  5},
{"& Mace (Lead-filled)"		,0x00000000L,	TV_HAFTED, '\\',/* 60*/
    0,	 502,	9,   1, 180,	0,   0,	 0,   0, {3,4}	, 15},
{"& Awl-Pike"			,0x00000000L,	TV_POLEARM, '/',/* 61*/
    0,	 340,	1,   1, 160,	0,   0,	 0,   0, {1,8}	,  8},
{"& Beaked Axe"			,0x00000000L,	TV_POLEARM, '/',/* 62*/
    0,	 408,	2,   1, 180,	0,   0,	 0,   0, {2,6}	, 15},
{"& Fauchard"			,0x00000000L,	TV_POLEARM, '/',/* 63*/
    0,	 376,	3,   1, 170,	0,   0,	 0,   0, {1,10} , 17},
{"& Glaive"			,0x00000000L,	TV_POLEARM, '/',/* 64*/
    0,	 363,	4,   1, 190,	0,   0,	 0,   0, {2,6}	, 20},
{"& Halberd"			,0x00000000L,	TV_POLEARM, '/',/* 65*/
    0,	 430,	5,   1, 190,	0,   0,	 0,   0, {3,4}	, 22},
{"& Lucerne Hammer"		,0x00000000L,	TV_POLEARM, '/',/* 66*/
    0,	 376,	6,   1, 120,	0,   0,	 0,   0, {2,5}	, 11},
{"& Pike"			,0x00000000L,	TV_POLEARM, '/',/* 67*/
    0,	 358,	7,   1, 160,	0,   0,	 0,   0, {2,5}	, 15},
{"& Spear"			,0x00000000L,	TV_POLEARM, '/',/* 68*/
    0,	  36,	8,   1,  50,	0,   0,	 0,   0, {1,6}	,  5},
{"& Lance"			,0x00000000L,	TV_POLEARM, '/',/* 69*/
    0,	 230,	9,   1, 300,	0,   0,	 0,   0, {2,8}	, 10},
{"& Javelin"			,0x00000000L,	TV_POLEARM, '/',/* 70*/
    0,	  18,	10,   1,  30,	0,   0,	 0,   0, {1,4}	,  4},
{"& Battle Axe (Balestarius)"	,0x00000000L,	TV_POLEARM, '/',/* 71*/
    0,	 500,	11,   1, 180,	0,   0,	 0,   0, {2,8}	, 30},
{"& Battle Axe (European)"	,0x00000000L,	TV_POLEARM, '/',/* 72*/
    0,	 334,	12,   1, 170,	0,   0,	 0,   0, {3,4}	, 13},
{"& Broad Axe"			,0x00000000L,	TV_POLEARM, '/',/* 73*/
    0,	 304,	13,   1, 160,	0,   0,	 0,   0, {2,6}	, 17},
{"& Short Bow"			,0x00000000L,	TV_BOW, '}',	/* 74*/
    2,	  50,	1,   1,  30,	0,   0,	 0,   0, {0,0}	,  3},
{"& Long Bow"			,0x00000000L,	TV_BOW, '}',	/* 75*/
    3,	 120,	2,   1,  40,	0,   0,	 0,   0, {0,0}	, 10},
{"& Composite Bow"		,0x00000000L,	TV_BOW, '}',	/* 76*/
    4,	 240,	3,   1,  40,	0,   0,	 0,   0, {0,0}	, 40},
{"& Light Crossbow"		,0x00000000L,	TV_BOW, '}',	/* 77*/
    5,	 140,	10,   1, 110,	0,   0,	 0,   0, {0,0}	, 15},
{"& Heavy Crossbow"		,0x00000000L,	TV_BOW, '}',	/* 78*/
    6,	 300,	11,   1, 200,	0,   0,	 0,   0, {1,1}	, 30},
{"& Sling"			,0x00000000L,	TV_BOW, '}',	/* 79*/
    1,	   5,	20,   1,   5,	0,   0,	 0,   0, {0,0}	,  1},
{"& Arrow~"			,0x00000000L,	TV_ARROW, '{',	/* 80*/
    0,	   1, 193,   1,   2,	0,   0,	 0,   0, {1,4}	,  2},
{"& Bolt~"			,0x00000000L,	TV_BOLT, '{',	/* 81*/
    0,	   2, 193,   1,   3,	0,   0,	 0,   0, {1,5}	,  2},
{"& Rounded Pebble~"		,0x00000000L,	TV_SLING_AMMO, '{',/* 82*/
    0,	   1, 193,   1,   4,	0,   0,	 0,   0, {1,2}	,  0},
{"& Iron Shot~"			,0x00000000L,	TV_SLING_AMMO, '{',/* 83*/
    0,	   2, 194,   1,   5,	0,   0,	 0,   0, {1,3}	,  3},
{"& Iron Spike~"		,0x00000000L,	TV_SPIKE, '~',	/* 84*/
    0,	   1, 193,   1,  10,	0,   0,	 0,   0, {1,1}	,  1},
{"& Brass Lantern~"		,0x00000000L,	TV_LIGHT, '~',	/* 85*/
 7500,	  35,	1,   1,  50,	0,   0,	 0,   0, {1,1}	,  1},
{"& Wooden Torch~"		,0x00000000L,	TV_LIGHT, '~',	/* 86*/
 4000,	   2, 193,   1,  30,	0,   0,	 0,   0, {1,1}	,  1},
{"& Orcish Pick"		,0x20000000L,	TV_DIGGING, '\\',/* 87*/
    2,	 500,	2,   1, 180,	0,   0,	 0,   0, {1,3}	, 20},
{"& Dwarven Pick"	       ,0x20000000L,	TV_DIGGING, '\\',/* 88*/
    3,	1200,	3,   1, 200,	0,   0,	 0,   0, {1,4}	, 50},
{"& Gnomish Shovel"		,0x20000000L,	TV_DIGGING, '\\',/* 89*/
    1,	 100,	5,   1,  50,	0,   0,	 0,   0, {1,2}	, 20},
{"& Dwarven Shovel"		,0x20000000L,	TV_DIGGING, '\\',/* 90*/
    2,	 250,	6,   1, 120,	0,   0,	 0,   0, {1,3}	, 40},
{"& Pair of Soft Leather Shoes"	,0x00000000L,	TV_BOOTS, ']',	/* 91*/
    0,	   4,	1,   1,   5,	0,   0,	 1,   0, {0,0}	,  1},
{"& Pair of Soft Leather Boots"	,0x00000000L,	TV_BOOTS, ']',	/* 92*/
    0,	   7,	2,   1,  20,	0,   0,	 2,   0, {1,1}	,  4},
{"& Pair of Hard Leather Boots"	,0x00000000L,	TV_BOOTS, ']',	/* 93*/
    0,	  12,	3,   1,  40,	0,   0,	 3,   0, {1,1}	,  6},
{"& Soft Leather Cap"		,0x00000000L,	TV_HELM, ']',	/* 94*/
    0,	   4,	1,   1,  10,	0,   0,	 1,   0, {0,0}	,  2},
{"& Hard Leather Cap"		,0x00000000L,	TV_HELM, ']',	/* 95*/
    0,	  12,	2,   1,  15,	0,   0,	 2,   0, {0,0}	,  4},
{"& Metal Cap"			,0x00000000L,	TV_HELM, ']',	/* 96*/
    0,	  30,	3,   1,  20,	0,   0,	 3,   0, {1,1}	,  7},
{"& Iron Helm"			,0x00000000L,	TV_HELM, ']',	/* 97*/
    0,	  75,	4,   1,  75,	0,   0,	 5,   0, {1,3}	, 20},
{"& Steel Helm"			,0x00000000L,	TV_HELM, ']',	/* 98*/
    0,	 200,	5,   1,  60,	0,   0,	 6,   0, {1,3}	, 40},
{"& Silver Crown"		,0x00000000L,	TV_HELM, ']',	/* 99*/
    0,	 500,	6,   1,  20,	0,   0,	 0,   0, {1,1}	, 44},
{"& Golden Crown"		,0x00000000L,	TV_HELM, ']',	/*100*/
    0,	1000,	7,   1,  30,	0,   0,	 0,   0, {1,1}	, 47},
{"& Jewel-Encrusted Crown"	,0x00000000L,	TV_HELM, ']',	/*101*/
    0,	2000,	8,   1,  40,	0,   0,	 0,   0, {1,1}	, 50},
{"& Robe"			,0x00000000L,	TV_SOFT_ARMOR, '(',/*102*/
    0,	   4,	1,   1,  20,	0,   0,	 2,   0, {0,0}	,  1},
{"Soft Leather Armor"		,0x00000000L,	TV_SOFT_ARMOR, '(',/*103*/
    0,	  18,	2,   1,  80,	0,   0,	 4,   0, {0,0}	,  2},
{"Soft Studded Leather"		,0x00000000L,	TV_SOFT_ARMOR, '(',/*104*/
    0,	  35,	3,   1,  90,	0,   0,	 5,   0, {1,1}	,  3},
{"Hard Leather Armor"		,0x00000000L,	TV_SOFT_ARMOR, '(',/*105*/
    0,	  55,	4,   1, 100,  -1,   0,	 6,   0, {1,1}	,  5},
{"Hard Studded Leather"		,0x00000000L,	TV_SOFT_ARMOR, '(',/*106*/
    0,	 100,	5,   1, 110,  -1,   0,	 7,   0, {1,2}	,  7},
{"Woven Cord Armor"		,0x00000000L,	TV_SOFT_ARMOR, '(',/*107*/
    0,	  45,	6,   1, 150,  -1,   0,	 6,   0, {0,0}	,  7},
{"Soft Leather Ring Mail"	,0x00000000L,	TV_SOFT_ARMOR, '(',/*108*/
    0,	 160,	7,   1, 130,  -1,   0,	 6,   0, {1,2}	, 10},
{"Hard Leather Ring Mail"	,0x00000000L,	TV_SOFT_ARMOR, '(',/*109*/
    0,	 230,	8,   1, 150,  -2,   0,	 8,   0, {1,3}	, 12},
{"Leather Scale Mail"		,0x00000000L,	TV_SOFT_ARMOR, '(',/*110*/
    0,	 330,	9,   1, 140,  -1,   0,	11,   0, {1,1}	, 14},
{"Metal Scale Mail"		,0x00000000L,	TV_HARD_ARMOR, '[',/*111*/
    0,	 430,	1,   1, 250,  -2,   0,	13,   0, {1,4}	, 24},
{"Chain Mail"			,0x00000000L,	TV_HARD_ARMOR, '[',/*112*/
    0,	 530,	2,   1, 220,  -2,   0,	14,   0, {1,4}	, 26},
{"Rusty Chain Mail"		,0x00000000L,	TV_HARD_ARMOR, '[',/*113*/
    0,	   0,	3,   1, 200,  -5,   0,	14,  -8, {1,4}	, 26},
{"Double Chain Mail"		,0x00000000L,	TV_HARD_ARMOR, '[',/*114*/
    0,	 630,	4,   1, 260,  -2,   0,	15,   0, {1,4}	, 28},
{"Augmented Chain Mail"		,0x00000000L,	TV_HARD_ARMOR, '[',/*115*/
    0,	 675,	5,   1, 270,  -2,   0,	16,   0, {1,4}	, 30},
{"Bar Chain Mail"		,0x00000000L,	TV_HARD_ARMOR, '[',/*116*/
    0,	 720,	6,   1, 280,  -2,   0,	18,   0, {1,4}	, 34},
{"Metal Brigandine Armor"	,0x00000000L,	TV_HARD_ARMOR, '[',/*117*/
    0,	 775,	7,   1, 290,  -3,   0,	19,   0, {1,4}	, 36},
{"Laminated Armor"		,0x00000000L,	TV_HARD_ARMOR, '[',/*118*/
    0,	 825,	8,   1, 300,  -3,   0,	20,   0, {1,4}	, 38},
{"Partial Plate Armor"		,0x00000000L,	TV_HARD_ARMOR, '[',/*119*/
    0,	 900,	9,   1, 260,  -3,   0,	22,   0, {1,6}	, 42},
{"Metal Lamellar Armor"		,0x00000000L,	TV_HARD_ARMOR, '[',/*120*/
    0,	 950,	10,   1, 340,  -3,   0,	23,   0, {1,6}	, 44},
{"Full Plate Armor"		,0x00000000L,	TV_HARD_ARMOR, '[',/*121*/
    0,	1050,	11,   1, 380,  -3,   0,	25,   0, {2,4}	, 48},
{"Ribbed Plate Armor"		,0x00000000L,	TV_HARD_ARMOR, '[',/*122*/
    0,	1200,	12,   1, 380,  -3,   0,	28,   0, {2,4}	, 50},
{"& Cloak"			,0x00000000L,	TV_CLOAK, '(',	/*123*/
    0,	   3,	1,   1,  10,	0,   0,	 1,   0, {0,0}	,  1},
{"& Set of Leather Gloves"	,0x00000000L,	TV_GLOVES, ']',	/*124*/
    0,	   3,	1,   1,   5,	0,   0,	 1,   0, {0,0}	,  1},
{"& Set of Gauntlets"		,0x00000000L,	TV_GLOVES, ']',	/*125*/
    0,	  35,	2,   1,  25,	0,   0,	 2,   0, {1,1}	, 12},
{"& Small Leather Shield"	,0x00000000L,	TV_SHIELD, ')',	/*126*/
    0,	  30,	1,   1,  50,	0,   0,	 2,   0, {1,1}	,  3},
{"& Medium Leather Shield"	,0x00000000L,	TV_SHIELD, ')',	/*127*/
    0,	  60,	2,   1,  75,	0,   0,	 3,   0, {1,2}	,  8},
{"& Large Leather Shield"	,0x00000000L,	TV_SHIELD, ')',	/*128*/
    0,	 120,	3,   1, 100,	0,   0,	 4,   0, {1,2}	, 15},
{"& Small Metal Shield"		,0x00000000L,	TV_SHIELD, ')',	/*129*/
    0,	  50,	4,   1,  65,	0,   0,	 3,   0, {1,2}	, 10},
{"& Medium Metal Shield"	,0x00000000L,	TV_SHIELD, ')',	/*130*/
    0,	 125,	5,   1,  90,	0,   0,	 4,   0, {1,3}	, 20},
{"& Large Metal Shield"		,0x00000000L,	TV_SHIELD, ')',	/*131*/
    0,	 200,	6,   1, 120,	0,   0,	 5,   0, {1,3}	, 30},
{"Gain Strength"		,0x00000001L,	TV_RING, '=',	/*132*/
    0,	 400,	0,   1,   2,	0,   0,	 0,   0, {0,0}	, 30},
{"Gain Dexterity"		,0x00000008L,	TV_RING, '=',	/*133*/
    0,	 400,	1,   1,   2,	0,   0,	 0,   0, {0,0}	, 30},
{"Gain Constitution"		,0x00000010L,	TV_RING, '=',	/*134*/
    0,	 400,	2,   1,   2,	0,   0,	 0,   0, {0,0}	, 30},
{"Gain Intelligence"		,0x00000002L,	TV_RING, '=',	/*135*/
    0,	 350,	3,   1,   2,	0,   0,	 0,   0, {0,0}	, 30},
{"Speed"			,0x00001000L,	TV_RING, '=',	/*136*/
    0,	3000,	4,   1,   2,	0,   0,	 0,   0, {0,0}	, 50},
{"Searching"			,0x00000040L,	TV_RING, '=',	/*137*/
    0,	 250,	5,   1,   2,	0,   0,	 0,   0, {0,0}	,  7},
{"Teleportation"		,0x80000400L,	TV_RING, '=',	/*138*/
    0,	   0,	6,   1,   2,	0,   0,	 0,   0, {0,0}	,  7},
{"Slow Digestion"		,0x00000080L,	TV_RING, '=',	/*139*/
    0,	 250,	7,   1,   2,	0,   0,	 0,   0, {0,0}	,  7},
{"Resist Fire"			,0x00080000L,	TV_RING, '=',	/*140*/
    0,	 250,	8,   1,   2,	0,   0,	 0,   0, {0,0}	, 14},
{"Resist Cold"			,0x00200000L,	TV_RING, '=',	/*141*/
    0,	 250,	9,   1,   2,	0,   0,	 0,   0, {0,0}	, 14},
{"Feather Falling"		,0x04000000L,	TV_RING, '=',	/*142*/
    0,	 200,	10,   1,   2,	0,   0,	 0,   0, {0,0}	,  7},
{"Adornment"			,0x00000000L,	TV_RING, '=',	/*143*/
    0,	  20,	11,   1,   2,	0,   0,	 0,   0, {0,0}	,  7},
/* was a ring of adornment, subval = 12 here */
{"& Arrow~"			,0x00000000L,	TV_ARROW, '{',	/*144*/
    0,	   1, 193,   1,   2,	0,   0,	 0,   0, {1,4}	, 15},
{"Weakness"			,0x80000001L,	TV_RING, '=',	/*145*/
   -5,	   0,	13,   1,   2,	0,   0,	 0,   0, {0,0}	,  7},
{"Lordly Protection (FIRE)"	,0x00080000L,	TV_RING, '=',	/*146*/
    0,	1200,	14,   1,   2,	0,   0,	 0,   5, {0,0}	, 50},
{"Lordly Protection (ACID)"	,0x00100000L,	TV_RING, '=',	/*147*/
    0,	1200,	15,   1,   2,	0,   0,	 0,   5, {0,0}	, 50},
{"Lordly Protection (COLD)"	,0x00200000L,	TV_RING, '=',	/*148*/
    0,	1200,	16,   1,   2,	0,   0,	 0,   5, {0,0}	, 50},
{"WOE"				,0x80000644L,	TV_RING, '=',	/*149*/
   -5,	   0,	17,   1,   2,	0,   0,	 0,  -3, {0,0}	, 50},
{"Stupidity"			,0x80000002L,	TV_RING, '=',	/*150*/
   -5,	   0,	18,   1,   2,	0,   0,	 0,   0, {0,0}	,  7},
{"Increase Damage"		,0x00000000L,	TV_RING, '=',	/*151*/
    0,	 100,	19,   1,   2,	0,   0,	 0,   0, {0,0}	, 20},
{"Increase To-Hit"		,0x00000000L,	TV_RING, '=',	/*152*/
    0,	 100,	20,   1,   2,	0,   0,	 0,   0, {0,0}	, 20},
{"Protection"			,0x00000000L,	TV_RING, '=',	/*153*/
    0,	 100,	21,   1,   2,	0,   0,	 0,   0, {0,0}	,  7},
{"Aggravate Monster"		,0x80000200L,	TV_RING, '=',	/*154*/
    0,	   0,	22,   1,   2,	0,   0,	 0,   0, {0,0}	,  7},
{"See Invisible"		,0x01000000L,	TV_RING, '=',	/*155*/
    0,	 340,	23,   1,   2,	0,   0,	 0,   0, {0,0}	, 40},
{"Sustain Strength"		,0x00400000L,	TV_RING, '=',	/*156*/
    1,	 750,	24,   1,   2,	0,   0,	 0,   0, {0,0}	, 44},
{"Sustain Intelligence"		,0x00400000L,	TV_RING, '=',	/*157*/
    2,	 600,	25,   1,   2,	0,   0,	 0,   0, {0,0}	, 44},
{"Sustain Wisdom"		,0x00400000L,	TV_RING, '=',	/*158*/
    3,	 600,	26,   1,   2,	0,   0,	 0,   0, {0,0}	, 44},
{"Sustain Constitution"		,0x00400000L,	TV_RING, '=',	/*159*/
    4,	 750,	27,   1,   2,	0,   0,	 0,   0, {0,0}	, 44},
{"Sustain Dexterity"		,0x00400000L,	TV_RING, '=',	/*160*/
    5,	 750,	28,   1,   2,	0,   0,	 0,   0, {0,0}	, 44},
{"Sustain Charisma"		,0x00400000L,	TV_RING, '=',	/*161*/
    6,	 500,	29,   1,   2,	0,   0,	 0,   0, {0,0}	, 44},
{"Slaying"			,0x00000000L,	TV_RING, '=',	/*162*/
    0,	1000,	30,   1,   2,	0,   0,	 0,   0, {0,0}	, 50},
{"Wisdom"			,0x00000004L,	TV_AMULET, '"',	/*163*/
    0,	 300,	0,   1,   3,	0,   0,	 0,   0, {0,0}	, 20},
{"Charisma"			,0x00000020L,	TV_AMULET, '"',	/*164*/
    0,	 250,	1,   1,   3,	0,   0,	 0,   0, {0,0}	, 20},
{"Searching"			,0x00000040L,	TV_AMULET, '"',	/*165*/
    0,	 250,	2,   1,   3,	0,   0,	 0,   0, {0,0}	, 14},
{"Teleportation"		,0x80000400L,	TV_AMULET, '"',	/*166*/
    0,	   0,	3,   1,   3,	0,   0,	 0,   0, {0,0}	, 14},
{"Slow Digestion"		,0x00000080L,	TV_AMULET, '"',	/*167*/
    0,	 200,	4,   1,   3,	0,   0,	 0,   0, {0,0}	, 14},
{"Resist Acid"			,0x00100000L,	TV_AMULET, '"',	/*168*/
    0,	 300,	5,   1,   3,	0,   0,	 0,   0, {0,0}	, 24},
{"Adornment"			,0x00000000L,	TV_AMULET, '"',	/*169*/
    0,	  20,	6,   1,   3,	0,   0,	 0,   0, {0,0}	, 16},
/* was an amulet of adornment here, subval = 7 */
{"& Bolt~"			,0x00000000L,	TV_BOLT, '{',	/*170*/
    0,	   2, 193,   1,   3,	0,   0,	 0,   0, {1,5}	, 25},
{"the Magi"			,0x01800040L,	TV_AMULET, '"',	/*171*/
    0,	5000,	8,   1,   3,	0,   0,	 0,   3, {0,0}	, 50},
{"DOOM"				,0x8000007FL,	TV_AMULET, '"',	/*172*/
   -5,	   0,	9,   1,   3,	0,   0,	 0,   0, {0,0}	, 50},
{"Enchant Weapon To-Hit"	,0x00000001L,	TV_SCROLL1, '?',/*173*/
    0,	 125,	64,   1,   5,	0,   0,	 0,   0, {0,0}	, 12},
{"Enchant Weapon To-Dam"	,0x00000002L,	TV_SCROLL1, '?',/*174*/
    0,	 125,	65,   1,   5,	0,   0,	 0,   0, {0,0}	, 12},
{"Enchant Armor"		,0x00000004L,	TV_SCROLL1, '?',/*175*/
    0,	 125,	66,   1,   5,	0,   0,	 0,   0, {0,0}	, 12},
{"Identify"			,0x00000008L,	TV_SCROLL1, '?',/*176*/
    0,	  50,	67,   1,   5,	0,   0,	 0,   0, {0,0}	,  1},
{"Identify"			,0x00000008L,	TV_SCROLL1, '?',/*177*/
    0,	  50,	67,   1,   5,	0,   0,	 0,   0, {0,0}	,  5},
{"Identify"			,0x00000008L,	TV_SCROLL1, '?',/*178*/
    0,	  50,	67,   1,   5,	0,   0,	 0,   0, {0,0}	, 10},
{"Identify"			,0x00000008L,	TV_SCROLL1, '?',/*179*/
    0,	  50,	67,   1,   5,	0,   0,	 0,   0, {0,0}	, 30},
{"Remove Curse"			,0x00000010L,	TV_SCROLL1, '?',/*180*/
    0,	 100,	68,   1,   5,	0,   0,	 0,   0, {0,0}	,  7},
{"Light"			,0x00000020L,	TV_SCROLL1, '?',/*181*/
    0,	  15,	69,   1,   5,	0,   0,	 0,   0, {0,0}	,  0},
{"Light"			,0x00000020L,	TV_SCROLL1, '?',/*182*/
    0,	  15,	69,   1,   5,	0,   0,	 0,   0, {0,0}	,  3},
{"Light"			,0x00000020L,	TV_SCROLL1, '?',/*183*/
    0,	  15,	69,   1,   5,	0,   0,	 0,   0, {0,0}	,  7},
{"Summon Monster"		,0x00000040L,	TV_SCROLL1, '?',/*184*/
    0,	   0,	70,   1,   5,	0,   0,	 0,   0, {0,0}	,  1},
{"Phase Door"			,0x00000080L,	TV_SCROLL1, '?',/*185*/
    0,	  15,	71,   1,   5,	0,   0,	 0,   0, {0,0}	,  1},
{"Teleport"			,0x00000100L,	TV_SCROLL1, '?',/*186*/
    0,	  40,	72,   1,   5,	0,   0,	 0,   0, {0,0}	, 10},
{"Teleport Level"		,0x00000200L,	TV_SCROLL1, '?',/*187*/
    0,	  50,	73,   1,   5,	0,   0,	 0,   0, {0,0}	, 20},
{"Monster Confusion"		,0x00000400L,	TV_SCROLL1, '?',/*188*/
    0,	  30,	74,   1,   5,	0,   0,	 0,   0, {0,0}	,  5},
{"Magic Mapping"		,0x00000800L,	TV_SCROLL1, '?',/*189*/
    0,	  40,	75,   1,   5,	0,   0,	 0,   0, {0,0}	,  5},
{"Sleep Monster"		,0x00001000L,	TV_SCROLL1, '?',/*190*/
    0,	  35,	76,   1,   5,	0,   0,	 0,   0, {0,0}	,  5},
{"Rune of Protection"		,0x00002000L,	TV_SCROLL1, '?',/*191*/
    0,	 500,	77,   1,   5,	0,   0,	 0,   0, {0,0}	, 50},
{"Treasure Detection"		,0x00004000L,	TV_SCROLL1, '?',/*192*/
    0,	  15,	78,   1,   5,	0,   0,	 0,   0, {0,0}	,  0},
{"Object Detection"		,0x00008000L,	TV_SCROLL1, '?',/*193*/
    0,	  15,	79,   1,   5,	0,   0,	 0,   0, {0,0}	,  0},
{"Trap Detection"		,0x00010000L,	TV_SCROLL1, '?',/*194*/
    0,	  35,	80,   1,   5,	0,   0,	 0,   0, {0,0}	,  5},
{"Trap Detection"		,0x00010000L,	TV_SCROLL1, '?',/*195*/
    0,	  35,	80,   1,   5,	0,   0,	 0,   0, {0,0}	,  8},
{"Trap Detection"		,0x00010000L,	TV_SCROLL1, '?',/*196*/
    0,	  35,	80,   1,   5,	0,   0,	 0,   0, {0,0}	, 12},
{"Door/Stair Location"		,0x00020000L,	TV_SCROLL1, '?',/*197*/
    0,	  35,	81,   1,   5,	0,   0,	 0,   0, {0,0}	,  5},
{"Door/Stair Location"		,0x00020000L,	TV_SCROLL1, '?',/*198*/
    0,	  35,	81,   1,   5,	0,   0,	 0,   0, {0,0}	, 10},
{"Door/Stair Location"		,0x00020000L,	TV_SCROLL1, '?',/*199*/
    0,	  35,	81,   1,   5,	0,   0,	 0,   0, {0,0}	, 15},
{"Mass Genocide"		,0x00040000L,	TV_SCROLL1, '?',/*200*/
    0,	1000,	82,   1,   5,	0,   0,	 0,   0, {0,0}	, 50},
{"Detect Invisible"		,0x00080000L,	TV_SCROLL1, '?',/*201*/
    0,	  15,	83,   1,   5,	0,   0,	 0,   0, {0,0}	,  1},
{"Aggravate Monster"		,0x00100000L,	TV_SCROLL1, '?',/*202*/
    0,	   0,	84,   1,   5,	0,   0,	 0,   0, {0,0}	,  5},
{"Trap Creation"		,0x00200000L,	TV_SCROLL1, '?',/*203*/
    0,	   0,	85,   1,   5,	0,   0,	 0,   0, {0,0}	, 12},
{"Trap/Door Destruction"	,0x00400000L,	TV_SCROLL1, '?',/*204*/
    0,	  50,	86,   1,   5,	0,   0,	 0,   0, {0,0}	, 12},
{"Door Creation"		,0x00800000L,	TV_SCROLL1, '?',/*205*/
    0,	 100,	87,   1,   5,	0,   0,	 0,   0, {0,0}	, 12},
{"Recharging"			,0x01000000L,	TV_SCROLL1, '?',/*206*/
    0,	 200,	88,   1,   5,	0,   0,	 0,   0, {0,0}	, 40},
{"Genocide"			,0x02000000L,	TV_SCROLL1, '?',/*207*/
    0,	 750,	89,   1,   5,	0,   0,	 0,   0, {0,0}	, 35},
{"Darkness"			,0x04000000L,	TV_SCROLL1, '?',/*208*/
    0,	   0,	90,   1,   5,	0,   0,	 0,   0, {0,0}	,  1},
{"Protection from Evil"		,0x08000000L,	TV_SCROLL1, '?',/*209*/
    0,	  50,	91,   1,   5,	0,   0,	 0,   0, {0,0}	, 30},
{"Create Food"			,0x10000000L,	TV_SCROLL1, '?',/*210*/
    0,	  10,	92,   1,   5,	0,   0,	 0,   0, {0,0}	,  5},
{"Dispel Undead"		,0x20000000L,	TV_SCROLL1, '?',/*211*/
    0,	 200,	93,   1,   5,	0,   0,	 0,   0, {0,0}	, 40},
{"*Enchant Weapon*"		,0x00000001L,	TV_SCROLL2, '?',/*212*/
    0,	 500,	94,   1,   5,	0,   0,	 0,   0, {0,0}	, 50},
{"Curse Weapon"			,0x00000002L,	TV_SCROLL2, '?',/*213*/
    0,	   0,	95,   1,   5,	0,   0,	 0,   0, {0,0}	, 50},
{"*Enchant Armor*"		,0x00000004L,	TV_SCROLL2, '?',/*214*/
    0,	 500,	96,   1,   5,	0,   0,	 0,   0, {0,0}	, 50},
{"Curse Armor"			,0x00000008L,	TV_SCROLL2, '?',/*215*/
    0,	   0,	97,   1,   5,	0,   0,	 0,   0, {0,0}	, 50},
{"Summon Undead"		,0x00000010L,	TV_SCROLL2, '?',/*216*/
    0,	   0,	98,   1,   5,	0,   0,	 0,   0, {0,0}	, 15},
{"Blessing"			,0x00000020L,	TV_SCROLL2, '?',/*217*/
    0,	  15,	99,   1,   5,	0,   0,	 0,   0, {0,0}	,  1},
{"Holy Chant"			,0x00000040L,	TV_SCROLL2, '?',/*218*/
    0,	  40, 100,   1,   5,	0,   0,	 0,   0, {0,0}	, 12},
{"Holy Prayer"			,0x00000080L,	TV_SCROLL2, '?',/*219*/
    0,	  80, 101,   1,   5,	0,   0,	 0,   0, {0,0}	, 24},
{"Word-of-Recall"		,0x00000100L,	TV_SCROLL2, '?',/*220*/
    0,	 150, 102,   1,   5,	0,   0,	 0,   0, {0,0}	,  5},
{"*Destruction*"		,0x00000200L,	TV_SCROLL2, '?',/*221*/
    0,	 250, 103,   1,   5,	0,   0,	 0,   0, {0,0}	, 40},
/* SMJ, AJ, Water must be subval 64-66 resp. for objdes to work */
{"Slime Mold Juice"		,0x30000000L,	TV_POTION1, '!',/*222*/
  400,	   2,	64,   1,   4,	0,   0,	 0,   0, {1,1}	,  0},
{"Apple Juice"			,0x00000000L,	TV_POTION1, '!',/*223*/
  250,	   1,	65,   1,   4,	0,   0,	 0,   0, {1,1}	,  0},
{"Water"			,0x00000000L,	TV_POTION1, '!',/*224*/
  200,	   0,	66,   1,   4,	0,   0,	 0,   0, {1,1}	,  0},
{"Gain Strength"		,0x00000001L,	TV_POTION1, '!',/*225*/
    0,	 300,	67,   1,   4,	0,   0,	 0,   0, {1,1}	, 25},
{"Weakness"			,0x00000002L,	TV_POTION1, '!',/*226*/
    0,	   0,	68,   1,   4,	0,   0,	 0,   0, {1,1}	,  3},
{"Restore Strength"		,0x00000004L,	TV_POTION1, '!',/*227*/
    0,	 300,	69,   1,   4,	0,   0,	 0,   0, {1,1}	, 40},
{"Gain Intelligence"		,0x00000008L,	TV_POTION1, '!',/*228*/
    0,	 300,	70,   1,   4,	0,   0,	 0,   0, {1,1}	, 25},
{"Lose Intelligence"		,0x00000010L,	TV_POTION1, '!',/*229*/
    0,	   0,	71,   1,   4,	0,   0,	 0,   0, {1,1}	, 25},
{"Restore Intelligence"		,0x00000020L,	TV_POTION1, '!',/*230*/
    0,	 300,	72,   1,   4,	0,   0,	 0,   0, {1,1}	, 40},
{"Gain Wisdom"			,0x00000040L,	TV_POTION1, '!',/*231*/
    0,	 300,	73,   1,   4,	0,   0,	 0,   0, {1,1}	, 25},
{"Lose Wisdom"			,0x00000080L,	TV_POTION1, '!',/*232*/
    0,	   0,	74,   1,   4,	0,   0,	 0,   0, {1,1}	, 25},
{"Restore Wisdom"		,0x00000100L,	TV_POTION1, '!',/*233*/
    0,	 300,	75,   1,   4,	0,   0,	 0,   0, {1,1}	, 40},
{"Charisma"			,0x00000200L,	TV_POTION1, '!',/*234*/
    0,	 300,	76,   1,   4,	0,   0,	 0,   0, {1,1}	, 25},
{"Ugliness"			,0x00000400L,	TV_POTION1, '!',/*235*/
    0,	   0,	77,   1,   4,	0,   0,	 0,   0, {1,1}	, 25},
{"Restore Charisma"		,0x00000800L,	TV_POTION1, '!',/*236*/
    0,	 300,	78,   1,   4,	0,   0,	 0,   0, {1,1}	, 40},
{"Cure Light Wounds"		,0x10001000L,	TV_POTION1, '!',/*237*/
   50,	  15,	79,   1,   4,	0,   0,	 0,   0, {1,1}	,  0},
{"Cure Light Wounds"		,0x10001000L,	TV_POTION1, '!',/*238*/
   50,	  15,	79,   1,   4,	0,   0,	 0,   0, {1,1}	,  1},
{"Cure Light Wounds"		,0x10001000L,	TV_POTION1, '!',/*239*/
   50,	  15,	79,   1,   4,	0,   0,	 0,   0, {1,1}	,  2},
{"Cure Serious Wounds"		,0x30002000L,	TV_POTION1, '!',/*240*/
  100,	  40,	80,   1,   4,	0,   0,	 0,   0, {1,1}	,  3},
{"Cure Critical Wounds"		,0x70004000L,	TV_POTION1, '!',/*241*/
  100,	 100,	81,   1,   4,	0,   0,	 0,   0, {1,1}	,  5},
{"Healing"			,0x70008000L,	TV_POTION1, '!',/*242*/
  200,	 200,	82,   1,   4,	0,   0,	 0,   0, {1,1}	, 12},
{"Gain Constitution"		,0x00010000L,	TV_POTION1, '!',/*243*/
    0,	 300,	83,   1,   4,	0,   0,	 0,   0, {1,1}	, 25},
{"Gain Experience"		,0x00020000L,	TV_POTION1, '!',/*244*/
    0,	2500,	84,   1,   4,	0,   0,	 0,   0, {1,1}	, 50},
{"Sleep"			,0x00040000L,	TV_POTION1, '!',/*245*/
  100,	   0,	85,   1,   4,	0,   0,	 0,   0, {1,1}	,  0},
{"Blindness"			,0x00080000L,	TV_POTION1, '!',/*246*/
    0,	   0,	86,   1,   4,	0,   0,	 0,   0, {1,1}	,  0},
{"Confusion"			,0x00100000L,	TV_POTION1, '!',/*247*/
   50,	   0,	87,   1,   4,	0,   0,	 0,   0, {1,1}	,  0},
{"Poison"			,0x00200000L,	TV_POTION1, '!',/*248*/
    0,	   0,	88,   1,   4,	0,   0,	 0,   0, {1,1}	,  3},
{"Haste Self"			,0x00400000L,	TV_POTION1, '!',/*249*/
    0,	  75,	89,   1,   4,	0,   0,	 0,   0, {1,1}	,  1},
{"Slowness"			,0x00800000L,	TV_POTION1, '!',/*250*/
   50,	   0,	90,   1,   4,	0,   0,	 0,   0, {1,1}	,  1},
{"Gain Dexterity"		,0x02000000L,	TV_POTION1, '!',/*251*/
    0,	 300,	91,   1,   4,	0,   0,	 0,   0, {1,1}	, 25},
{"Restore Dexterity"		,0x04000000L,	TV_POTION1, '!',/*252*/
    0,	 300,	92,   1,   4,	0,   0,	 0,   0, {1,1}	, 40},
{"Restore Constitution"		,0x68000000L,	TV_POTION1, '!',/*253*/
    0,	 300,	93,   1,   4,	0,   0,	 0,   0, {1,1}	, 40},
{"Lose Memories"		,0x00000002L,	TV_POTION2, '!',/*254*/
    0,	   0,	95,   1,   4,	0,   0,	 0,   0, {1,1}	, 10},
{"Salt Water"			,0x00000004L,	TV_POTION2, '!',/*255*/
    0,	   0,	96,   1,   4,	0,   0,	 0,   0, {1,1}	,  0},
{"Invulnerability"		,0x00000008L,	TV_POTION2, '!',/*256*/
    0,	 250,	97,   1,   4,	0,   0,	 0,   0, {1,1}	, 40},
{"Heroism"			,0x00000010L,	TV_POTION2, '!',/*257*/
    0,	  35,	98,   1,   4,	0,   0,	 0,   0, {1,1}	,  1},
{"Super Heroism"		,0x00000020L,	TV_POTION2, '!',/*258*/
    0,	 100,	99,   1,   4,	0,   0,	 0,   0, {1,1}	,  3},
{"Boldness"			,0x00000040L,	TV_POTION2, '!',/*259*/
    0,	  10, 100,   1,   4,	0,   0,	 0,   0, {1,1}	,  1},
{"Restore Life Levels"		,0x00000080L,	TV_POTION2, '!',/*260*/
    0,	 400, 101,   1,   4,	0,   0,	 0,   0, {1,1}	, 40},
{"Resist Heat"			,0x00000100L,	TV_POTION2, '!',/*261*/
    0,	  30, 102,   1,   4,	0,   0,	 0,   0, {1,1}	,  1},
{"Resist Cold"			,0x00000200L,	TV_POTION2, '!',/*262*/
    0,	  30, 103,   1,   4,	0,   0,	 0,   0, {1,1}	,  1},
{"Detect Invisible"		,0x00000400L,	TV_POTION2, '!',/*263*/
    0,	  50, 104,   1,   4,	0,   0,	 0,   0, {1,1}	,  3},
{"Slow Poison"			,0x00000800L,	TV_POTION2, '!',/*264*/
    0,	  25, 105,   1,   4,	0,   0,	 0,   0, {1,1}	,  1},
{"Neutralize Poison"		,0x00001000L,	TV_POTION2, '!',/*265*/
    0,	  75, 106,   1,   4,	0,   0,	 0,   0, {1,1}	,  5},
{"Restore Mana"			,0x00002000L,	TV_POTION2, '!',/*266*/
    0,	 350, 107,   1,   4,	0,   0,	 0,   0, {1,1}	, 25},
{"Infra-Vision"			,0x00004000L,	TV_POTION2, '!',/*267*/
    0,	  20, 108,   1,   4,	0,   0,	 0,   0, {1,1}	,  3},
{"& Flask~ of oil"		,0x00040000L,	TV_FLASK, '!',	/*268*/
 7500,	   3,	64,   1,  10,	0,   0,	 0,   0, {2,6}	,  1},
{"Light"			,0x00000001L,	TV_WAND, '-',	/*269*/
    0,	 200,	0,   1,  10,	0,   0,	 0,   0, {1,1}	,  2},
{"Lightning Bolts"		,0x00000002L,	TV_WAND, '-',	/*270*/
    0,	 600,	1,   1,  10,	0,   0,	 0,   0, {1,1}	, 15},
{"Frost Bolts"			,0x00000004L,	TV_WAND, '-',	/*271*/
    0,	 800,	2,   1,  10,	0,   0,	 0,   0, {1,1}	, 20},
{"Fire Bolts"			,0x00000008L,	TV_WAND, '-',	/*272*/
    0,	1000,	3,   1,  10,	0,   0,	 0,   0, {1,1}	, 30},
{"Stone-to-Mud"			,0x00000010L,	TV_WAND, '-',	/*273*/
    0,	 300,	4,   1,  10,	0,   0,	 0,   0, {1,1}	, 12},
{"Polymorph"			,0x00000020L,	TV_WAND, '-',	/*274*/
    0,	 400,	5,   1,  10,	0,   0,	 0,   0, {1,1}	, 20},
{"Heal Monster"			,0x00000040L,	TV_WAND, '-',	/*275*/
    0,	   0,	6,   1,  10,	0,   0,	 0,   0, {1,1}	,  2},
{"Haste Monster"		,0x00000080L,	TV_WAND, '-',	/*276*/
    0,	   0,	7,   1,  10,	0,   0,	 0,   0, {1,1}	,  2},
{"Slow Monster"			,0x00000100L,	TV_WAND, '-',	/*277*/
    0,	 500,	8,   1,  10,	0,   0,	 0,   0, {1,1}	,  2},
{"Confuse Monster"		,0x00000200L,	TV_WAND, '-',	/*278*/
    0,	 400,	9,   1,  10,	0,   0,	 0,   0, {1,1}	,  2},
{"Sleep Monster"		,0x00000400L,	TV_WAND, '-',	/*279*/
    0,	 500,	10,   1,  10,	0,   0,	 0,   0, {1,1}	,  7},
{"Drain Life"			,0x00000800L,	TV_WAND, '-',	/*280*/
    0,	1200,	11,   1,  10,	0,   0,	 0,   0, {1,1}	, 50},
{"Trap/Door Destruction"	,0x00001000L,	TV_WAND, '-',	/*281*/
    0,	 100,	12,   1,  10,	0,   0,	 0,   0, {1,1}	, 12},
{"Magic Missile"		,0x00002000L,	TV_WAND, '-',	/*282*/
    0,	 200,	13,   1,  10,	0,   0,	 0,   0, {1,1}	,  2},
{"Wall Building"		,0x00004000L,	TV_WAND, '-',	/*283*/
    0,	 400,	14,   1,  10,	0,   0,	 0,   0, {1,1}	, 25},
{"Clone Monster"		,0x00008000L,	TV_WAND, '-',	/*284*/
    0,	   0,	15,   1,  10,	0,   0,	 0,   0, {1,1}	, 15},
{"Teleport Away"		,0x00010000L,	TV_WAND, '-',	/*285*/
    0,	 350,	16,   1,  10,	0,   0,	 0,   0, {1,1}	, 20},
{"Disarming"			,0x00020000L,	TV_WAND, '-',	/*286*/
    0,	 700,	17,   1,  10,	0,   0,	 0,   0, {1,1}	, 20},
{"Lightning Balls"		,0x00040000L,	TV_WAND, '-',	/*287*/
    0,	1200,	18,   1,  10,	0,   0,	 0,   0, {1,1}	, 35},
{"Cold Balls"			,0x00080000L,	TV_WAND, '-',	/*288*/
    0,	1500,	19,   1,  10,	0,   0,	 0,   0, {1,1}	, 40},
{"Fire Balls"			,0x00100000L,	TV_WAND, '-',	/*289*/
    0,	1800,	20,   1,  10,	0,   0,	 0,   0, {1,1}	, 50},
{"Stinking Cloud"		,0x00200000L,	TV_WAND, '-',	/*290*/
    0,	 400,	21,   1,  10,	0,   0,	 0,   0, {1,1}	,  5},
{"Acid Balls"			,0x00400000L,	TV_WAND, '-',	/*291*/
    0,	1650,	22,   1,  10,	0,   0,	 0,   0, {1,1}	, 48},
{"Wonder"			,0x00800000L,	TV_WAND, '-',	/*292*/
    0,	 250,	23,   1,  10,	0,   0,	 0,   0, {1,1}	,  2},
{"Light"			,0x00000001L,	TV_STAFF, '_',	/*293*/
    0,	 250,	0,   1,  50,	0,   0,	 0,   0, {1,2}	,  5},
{"Door/Stair Location"		,0x00000002L,	TV_STAFF, '_',	/*294*/
    0,	 350,	1,   1,  50,	0,   0,	 0,   0, {1,2}	, 10},
{"Trap Location"		,0x00000004L,	TV_STAFF, '_',	/*295*/
    0,	 350,	2,   1,  50,	0,   0,	 0,   0, {1,2}	, 10},
{"Treasure Location"		,0x00000008L,	TV_STAFF, '_',	/*296*/
    0,	 200,	3,   1,  50,	0,   0,	 0,   0, {1,2}	,  5},
{"Object Location"		,0x00000010L,	TV_STAFF, '_',	/*297*/
    0,	 200,	4,   1,  50,	0,   0,	 0,   0, {1,2}	,  5},
{"Teleportation"		,0x00000020L,	TV_STAFF, '_',	/*298*/
    0,	 400,	5,   1,  50,	0,   0,	 0,   0, {1,2}	, 20},
{"Earthquakes"			,0x00000040L,	TV_STAFF, '_',	/*299*/
    0,	 350,	6,   1,  50,	0,   0,	 0,   0, {1,2}	, 40},
{"Summoning"			,0x00000080L,	TV_STAFF, '_',	/*300*/
    0,	   0,	7,   1,  50,	0,   0,	 0,   0, {1,2}	, 10},
{"Summoning"			,0x00000080L,	TV_STAFF, '_',	/*301*/
    0,	   0,	7,   1,  50,	0,   0,	 0,   0, {1,2}	, 50},
{"*Destruction*"		,0x00000200L,	TV_STAFF, '_',	/*302*/
    0,	2500,	8,   1,  50,	0,   0,	 0,   0, {1,2}	, 50},
{"Starlight"			,0x00000400L,	TV_STAFF, '_',	/*303*/
    0,	 800,	9,   1,  50,	0,   0,	 0,   0, {1,2}	, 20},
{"Haste Monsters"		,0x00000800L,	TV_STAFF, '_',	/*304*/
    0,	   0,	10,   1,  50,	0,   0,	 0,   0, {1,2}	, 10},
{"Slow Monsters"		,0x00001000L,	TV_STAFF, '_',	/*305*/
    0,	 800,	11,   1,  50,	0,   0,	 0,   0, {1,2}	, 10},
{"Sleep Monsters"		,0x00002000L,	TV_STAFF, '_',	/*306*/
    0,	 700,	12,   1,  50,	0,   0,	 0,   0, {1,2}	, 10},
{"Cure Light Wounds"		,0x00004000L,	TV_STAFF, '_',	/*307*/
    0,	 350,	13,   1,  50,	0,   0,	 0,   0, {1,2}	,  5},
{"Detect Invisible"		,0x00008000L,	TV_STAFF, '_',	/*308*/
    0,	 200,	14,   1,  50,	0,   0,	 0,   0, {1,2}	,  5},
{"Speed"			,0x00010000L,	TV_STAFF, '_',	/*309*/
    0,	1000,	15,   1,  50,	0,   0,	 0,   0, {1,2}	, 40},
{"Slowness"			,0x00020000L,	TV_STAFF, '_',	/*310*/
    0,	   0,	16,   1,  50,	0,   0,	 0,   0, {1,2}	, 40},
{"Mass Polymorph"		,0x00040000L,	TV_STAFF, '_',	/*311*/
    0,	 750,	17,   1,  50,	0,   0,	 0,   0, {1,2}	, 46},
{"Remove Curse"			,0x00080000L,	TV_STAFF, '_',	/*312*/
    0,	 500,	18,   1,  50,	0,   0,	 0,   0, {1,2}	, 47},
{"Detect Evil"			,0x00100000L,	TV_STAFF, '_',	/*313*/
    0,	 350,	19,   1,  50,	0,   0,	 0,   0, {1,2}	, 20},
{"Curing"			,0x00200000L,	TV_STAFF, '_',	/*314*/
    0,	1000,	20,   1,  50,	0,   0,	 0,   0, {1,2}	, 25},
{"Dispel Evil"			,0x00400000L,	TV_STAFF, '_',	/*315*/
    0,	1200,	21,   1,  50,	0,   0,	 0,   0, {1,2}	, 49},
{"Darkness"			,0x01000000L,	TV_STAFF, '_',	/*316*/
    0,	   0,	22,   1,  50,	0,   0,	 0,   0, {1,2}	, 50},
{"Darkness"			,0x01000000L,	TV_STAFF, '_',	/*317*/
    0,	   0,	22,   1,  50,	0,   0,	 0,   0, {1,2}	,  5},
{"[Beginners-Magik]"		,0x0000007FL,	TV_MAGIC_BOOK, '?',/*318*/
    0,	  25,	64,   1,  30,   0,   0,	 0,   0, {1,1}	, 40},
{"[Magik I]"			,0x0000FF80L,	TV_MAGIC_BOOK, '?',/*319*/
    0,	 100,	65,   1,  30,   0,   0,	 0,   0, {1,1}	, 40},
{"[Magik II]"			,0x00FF0000L,	TV_MAGIC_BOOK, '?',/*320*/
    0,	 400,	66,   1,  30,   0,   0,	 0,   0, {1,1}	, 40},
{"[The Mages Guide to Power]"	,0x7F000000L,	TV_MAGIC_BOOK, '?',/*321*/
    0,	 800,	67,   1,  30,   0,   0,	 0,   0, {1,1}	, 40},
{"[Beginners Handbook]"		,0x000000FFL,	TV_PRAYER_BOOK, '?',/*322*/
    0,	  25,	64,   1,  30,   0,   0,	 0,   0, {1,1}	, 40},
{"[Words of Wisdom]"		,0x0000FF00L,	TV_PRAYER_BOOK, '?',/*323*/
    0,	 100,	65,   1,  30,   0,   0,	 0,   0, {1,1}	, 40},
{"[Chants and Blessings]"	,0x01FF0000L,	TV_PRAYER_BOOK, '?',/*324*/
    0,	 300,	66,   1,  30,   0,   0,	 0,   0, {1,1}	, 40},
{"[Exorcism and Dispelling]"	,0x7E000000L,	TV_PRAYER_BOOK, '?',/*325*/
    0,	 900,	67,   1,  30,   0,   0,	 0,   0, {1,1}	, 40},
{"& Small wooden chest"		,0x0F000000L,	 TV_CHEST, '&',	/*326*/
    0,	  20,	1,   1, 250,	0,   0,	 0,   0, {2,3}	,  7},
{"& Large wooden chest"		,0x15000000L,	 TV_CHEST, '&',	/*327*/
    0,	  60,	4,   1, 500,	0,   0,	 0,   0, {2,5}	, 15},
{"& Small iron chest"		,0x0F000000L,	 TV_CHEST, '&',	/*328*/
    0,	 100,	7,   1, 300,	0,   0,	 0,   0, {2,4}	, 25},
{"& Large iron chest"		,0x1F000000L,	 TV_CHEST, '&',	/*329*/
    0,	 150,	10,   1,1000,	0,   0,	 0,   0, {2,6}	, 35},
{"& Small steel chest"		,0x0F000000L,	 TV_CHEST, '&',	/*330*/
    0,	 200,	13,   1, 500,	0,   0,	 0,   0, {2,4}	, 45},
{"& Large steel chest"		,0x23000000L,	 TV_CHEST, '&',	/*331*/
    0,	 250,	16,   1,1000,	0,   0,	 0,   0, {2,6}	, 50},
{"& Rat Skeleton"		,0x00000000L,	 TV_MISC, 's',	/*332*/
    0,	   0,	1,   1,  10,	0,   0,	 0,   0, {1,1}	,  1},
{"& Giant Centipede Skeleton"	,0x00000000L,	 TV_MISC, 's',	/*333*/
    0,	   0,	2,   1,  25,	0,   0,	 0,   0, {1,1}	,  1},
{"some filthy rags"		,0x00000000L,	TV_SOFT_ARMOR, '~',/*334*/
    0,	   0,	63,   1,  20,	0,   0,	 1,   0, {0,0}	,  0},
{"& empty bottle"		,0x00000000L,	 TV_MISC, '!',	/*335*/
    0,	   0,	4,   1,   2,	0,   0,	 0,   0, {1,1}	,  0},
{"some shards of pottery"	,0x00000000L,	 TV_MISC, '~',	/*336*/
    0,	   0,	5,   1,   5,	0,   0,	 0,   0, {1,1}	,  0},
{"& Human Skeleton"		,0x00000000L,	 TV_MISC, 's',	/*337*/
    0,	   0,	7,   1,  50,	0,   0,	 0,   0, {1,1}	,  1},
{"& Dwarf Skeleton"		,0x00000000L,	 TV_MISC, 's',	/*338*/
    0,	   0,	8,   1,  60,	0,   0,	 0,   0, {1,1}	,  1},
{"& Elf Skeleton"		,0x00000000L,	 TV_MISC, 's',	/*339*/
    0,	   0,	9,   1,  40,	0,   0,	 0,   0, {1,1}	,  1},
{"& Gnome Skeleton"		,0x00000000L,	 TV_MISC, 's',	/*340*/
    0,	   0,	10,   1,  25,	0,   0,	 0,   0, {1,1}	,  1},
{"& broken set of teeth"	,0x00000000L,	 TV_MISC, 's',	/*341*/
    0,	   0,	11,   1,   3,	0,   0,	 0,   0, {1,1}	,  0},
{"& large broken bone"		,0x00000000L,	 TV_MISC, 's',	/*342*/
    0,	   0,	12,   1,   2,	0,   0,	 0,   0, {1,1}	,  0},
{"& broken stick"		,0x00000000L,	 TV_MISC, '~',	/*343*/
    0,	   0,	13,   1,   3,	0,   0,	 0,   0, {1,1}	,  0},
/* end of Dungeon items */
/* Store items, which are not also dungeon items, some of these can be
   found above, except that the number is >1 below */
{"& Ration~ of Food"		,0x00000000L,	TV_FOOD, ',',	/*344*/
 5000,	   3,	90,   5,  10,	0,   0,	 0,   0, {0,0}	,  0},
{"& Hard Biscuit~"		,0x00000000L,	TV_FOOD, ',',	/*345*/
  500,	   1,	93,   5,   2,	0,   0,	 0,   0, {0,0}	,  0},
{"& Strip~ of Beef Jerky"	,0x00000000L,	TV_FOOD, ',',	/*346*/
 1750,	   2,	94,   5,   2,	0,   0,	 0,   0, {0,0}	,  0},
{"& Pint~ of Fine Ale"		,0x00000000L,	TV_FOOD, ',',	/*347*/
  500,	   1,	95,   3,  10,	0,   0,	 0,   0, {0,0}	,  0},
{"& Pint~ of Fine Wine"		,0x00000000L,	TV_FOOD, ',',	/*348*/
  400,	   2,	96,   1,  10,	0,   0,	 0,   0, {0,0}	,  0},
{"& Pick"			,0x20000000L,	TV_DIGGING, '\\',/*349*/
    1,	  50,	1,   1, 150,	0,   0,	 0,   0, {1,3}	,  0},
{"& Shovel"			,0x20000000L,	TV_DIGGING, '\\',/*350*/
    0,	  15,	4,   1,  60,	0,   0,	 0,   0, {1,2}	,  0},
{"Identify"			,0x00000008L,	TV_SCROLL1, '?',/*351*/
    0,	  50,  67,   2,   5,	0,   0,	 0,   0, {0,0}	,  0},
{"Light"			,0x00000020L,	TV_SCROLL1, '?',/*352*/
    0,	  15,  69,   3,   5,	0,   0,	 0,   0, {0,0}	,  0},
{"Phase Door"			,0x00000080L,	TV_SCROLL1, '?',/*353*/
    0,	  15,  71,   2,   5,	0,   0,	 0,   0, {0,0}	,  0},
{"Magic Mapping"		,0x00000800L,	TV_SCROLL1, '?',/*354*/
    0,	  40,  75,   2,   5,	0,   0,	 0,   0, {0,0}	,  0},
{"Treasure Detection"		,0x00004000L,	TV_SCROLL1, '?',/*355*/
    0,	  15,  78,   2,   5,	0,   0,	 0,   0, {0,0}	,  0},
{"Object Detection"		,0x00008000L,	TV_SCROLL1, '?',/*356*/
    0,	  15,  79,   2,   5,	0,   0,	 0,   0, {0,0}	,  0},
{"Detect Invisible"		,0x00080000L,	TV_SCROLL1, '?',/*357*/
    0,	  15,  83,   2,   5,	0,   0,	 0,   0, {0,0}	,  0},
{"Blessing"			,0x00000020L,	TV_SCROLL2, '?',/*358*/
    0,	  15,  99,   2,   5,	0,   0,	 0,   0, {0,0}	,  0},
{"Word-of-Recall"		,0x00000100L,	TV_SCROLL2, '?',/*359*/
    0,	 150, 102,   3,   5,	0,   0,	 0,   0, {0,0}	,  0},
{"Cure Light Wounds"		,0x10001000L,	TV_POTION1, '!',/*360*/
   50,	  15,  79,   2,   4,	0,   0,	 0,   0, {1,1}	,  0},
{"Heroism"			,0x00000010L,	TV_POTION2, '!',/*361*/
    0,	  35,  98,   2,   4,	0,   0,	 0,   0, {1,1}	,  0},
{"Boldness"			,0x00000040L,	TV_POTION2, '!',/*362*/
    0,	  10, 100,   2,   4,	0,   0,	 0,   0, {1,1}	,  0},
{"Slow Poison"			,0x00000800L,	TV_POTION2, '!',/*363*/
    0,	  25, 105,   2,   4,	0,   0,	 0,   0, {1,1}	,  0},
{"& Brass Lantern~"		,0x00000000L,	TV_LIGHT, '~',	/*364*/
 7500,	  35,	0,   1,  50,	0,   0,	 0,   0, {1,1}	,  1},
{"& Wooden Torch~"		,0x00000000L,	TV_LIGHT, '~',	/*365*/
 4000,	   2, 192,   5,  30,	0,   0,	 0,   0, {1,1}	,  1},
{"& Flask~ of oil"		,0x00040000L,	TV_FLASK, '!',	/*366*/
 7500,	   3,	64,   5,  10,	0,   0,	 0,   0, {2,6}	,  1},
/* end store items */
/* start doors */
/* Secret door must have same subval as closed door in	*/
/* TRAP_LISTB.	See CHANGE_TRAP. Must use & because of stone_to_mud. */
{"& open door"			,0x00000000L, TV_OPEN_DOOR, '\'',
    0,	   0,	1,   1,   0,	0,   0,	 0,   0, {1,1}	,  0},
{"& closed door"		,0x00000000L, TV_CLOSED_DOOR, '+',
    0,	   0,	19,   1,   0,	0,   0,	 0,   0, {1,1}	,  0},
#ifdef ATARIST_MWC
{"& secret door"		,0x00000000L, TV_SECRET_DOOR,
   (unsigned char)240, /* 369 */
#else
{"& secret door"		,0x00000000L, TV_SECRET_DOOR, '#',/* 369 */
#endif
    0,	   0,	19,   1,   0,	0,   0,	 0,   0, {1,1}	,  0},
/* end doors */
/* stairs */
{"an up staircase "		,0x00000000L, TV_UP_STAIR, '<',	/* 370 */
    0,	   0,	1,   1,   0,	0,   0,	 0,   0, {1,1}	,  0},
{"a down staircase"		,0x00000000L, TV_DOWN_STAIR, '>',/* 371 */
    0,	   0,	1,   1,   0,	0,   0,	 0,   0, {1,1}	,  0},
/* store door */
/* Stores are just special traps		*/
{"General Store"		,0x00000000L, TV_STORE_DOOR, '1',/* 372 */
    0,	   0, 101,   1,   0,	0,   0,	 0,   0, {0,0}	,  0},
{"Armory"			,0x00000000L, TV_STORE_DOOR, '2',
    0,	   0, 102,   1,   0,	0,   0,	 0,   0, {0,0}	,  0},
{"Weapon Smiths"		,0x00000000L, TV_STORE_DOOR, '3',
    0,	   0, 103,   1,   0,	0,   0,	 0,   0, {0,0}	,  0},
{"Temple"			,0x00000000L, TV_STORE_DOOR, '4',
    0,	   0, 104,   1,   0,	0,   0,	 0,   0, {0,0}	,  0},
{"Alchemy Shop"			,0x00000000L, TV_STORE_DOOR, '5',
    0,	   0, 105,   1,   0,	0,   0,	 0,   0, {0,0}	,  0},
{"Magic Shop"			,0x00000000L, TV_STORE_DOOR, '6',/* 377 */
    0,	   0, 106,   1,   0,	0,   0,	 0,   0, {0,0}	,  0},
/* end store door */
/* Traps are just Nasty treasures.				*/
/* Traps: Level represents the relative difficulty of disarming;	*/
/* and P1 represents the experienced gained when disarmed*/
{"an open pit"			,0x00000000L, TV_VIS_TRAP, ' ',	/* 378 */
    1,	   0,	1,   1,   0,	0,   0,	 0,   0, {2,6}	,50},
{"an arrow trap"		,0x00000000L, TV_INVIS_TRAP, '^',
    3,	   0,	2,   1,   0,	0,   0,	 0,   0, {1,8}	,90},
{"a covered pit"		,0x00000000L, TV_INVIS_TRAP, '^',
    2,	   0,	3,   1,   0,	0,   0,	 0,   0, {2,6}	,60},
{"a trap door"			,0x00000000L, TV_INVIS_TRAP, '^',
    5,	   0,	4,   1,   0,	0,   0,	 0,   0, {2,8}	,75},
{"a gas trap"			,0x00000000L, TV_INVIS_TRAP, '^',
    3,	   0,	5,   1,   0,	0,   0,	 0,   0, {1,4}	,95},
{"a loose rock"			,0x00000000L, TV_INVIS_TRAP, ';',
    0,	   0,	6,   1,   0,	0,   0,	 0,   0, {0,0}	,10},
{"a dart trap"			,0x00000000L, TV_INVIS_TRAP, '^',
    5,	   0,	7,   1,   0,	0,   0,	 0,   0, {1,4}	,110},
{"a strange rune"		,0x00000000L, TV_INVIS_TRAP, '^',
    5,	   0,	8,   1,   0,	0,   0,	 0,   0, {0,0}	,90},
{"some loose rock"		,0x00000000L, TV_INVIS_TRAP, '^',
    5,	   0,	9,   1,   0,	0,   0,	 0,   0, {2,6}	,90},
{"a gas trap"			,0x00000000L, TV_INVIS_TRAP, '^',
   10,	   0,	10,   1,   0,	0,   0,	 0,   0, {1,4}	,105},
{"a strange rune"		,0x00000000L, TV_INVIS_TRAP, '^',
    5,	   0,	11,   1,   0,	0,   0,	 0,   0, {0,0}	,90},
{"a blackened spot"		,0x00000000L, TV_INVIS_TRAP, '^',
   10,	   0,	12,   1,   0,	0,   0,	 0,   0, {4,6}	,110},
{"some corroded rock"		,0x00000000L, TV_INVIS_TRAP, '^',
   10,	   0,	13,   1,   0,	0,   0,	 0,   0, {4,6}	,110},
{"a gas trap"			,0x00000000L, TV_INVIS_TRAP, '^',
    5,	   0,	14,   1,   0,	0,   0,	 0,   0, {2,6}	,105},
{"a gas trap"			,0x00000000L, TV_INVIS_TRAP, '^',
    5,	   0,	15,   1,   0,	0,   0,	 0,   0, {1,4}	,110},
{"a gas trap"			,0x00000000L, TV_INVIS_TRAP, '^',
    5,	   0,	16,   1,   0,	0,   0,	 0,   0, {1,8}	,105},
{"a dart trap"			,0x00000000L, TV_INVIS_TRAP, '^',
    5,	   0,	17,   1,   0,	0,   0,	 0,   0, {1,8}	,110},
{"a dart trap"			,0x00000000L, TV_INVIS_TRAP, '^',/* 395 */
    5,	   0,	18,   1,   0,	0,   0,	 0,   0, {1,8}	,110},
/* rubble */
{"some rubble"			,0x00000000L, TV_RUBBLE, ':',	/* 396 */
    0,	   0,	1,   1,   0,	0,   0,	 0,   0, {0,0}	,  0},
/* mush */
{"& pint~ of fine grade mush"	,0x00000000L, TV_FOOD, ',',	/* 397 */
 1500,	   0,  97,   1,   1,   0,   0,   0,   0, {1,1}  ,  1},
/* Special trap	*/
{"a strange rune"		,0x00000000L, TV_VIS_TRAP, '^',	/* 398 */
    0,	   0,	99,   1,   0,	0,   0,	 0,   0, {0,0}	, 10},
/* Gold list (All types of gold and gems are defined here)	*/
{"copper"			,0x00000000L, TV_GOLD, '$',	/* 399 */
    0,	   3,	1,   1,   0,	0,   0,	 0,   0, {0,0}	  ,  1},
{"copper"			,0x00000000L, TV_GOLD, '$',
    0,	   4,	2,   1,   0,	0,   0,	 0,   0, {0,0}	  ,  1},
{"copper"			,0x00000000L, TV_GOLD, '$',
    0,	   5,	3,   1,   0,	0,   0,	 0,   0, {0,0}	  ,  1},
{"silver"			,0x00000000L, TV_GOLD, '$',
    0,	   6,	4,   1,   0,	0,   0,	 0,   0, {0,0}	  ,  1},
{"silver"			,0x00000000L, TV_GOLD, '$',
    0,	   7,	5,   1,   0,	0,   0,	 0,   0, {0,0}	  ,  1},
{"silver"			,0x00000000L, TV_GOLD, '$',
    0,	   8,	6,   1,   0,	0,   0,	 0,   0, {0,0}	  ,  1},
{"garnets"			,0x00000000L, TV_GOLD, '*',
    0,	   9,	7,   1,   0,	0,   0,	 0,   0, {0,0}	  ,  1},
{"garnets"			,0x00000000L, TV_GOLD, '*',
    0,	  10,	8,   1,   0,	0,   0,	 0,   0, {0,0}	  ,  1},
{"gold"				,0x00000000L, TV_GOLD, '$',
    0,	  12,	9,   1,   0,	0,   0,	 0,   0, {0,0}	  ,  1},
{"gold"				,0x00000000L, TV_GOLD, '$',
    0,	  14,	10,   1,   0,	0,   0,	 0,   0, {0,0}	  ,  1},
{"gold"				,0x00000000L, TV_GOLD, '$',
    0,	  16,	11,   1,   0,	0,   0,	 0,   0, {0,0}	  ,  1},
{"opals"			,0x00000000L, TV_GOLD, '*',
    0,	  18,	12,   1,   0,	0,   0,	 0,   0, {0,0}	  ,  1},
{"sapphires"			,0x00000000L, TV_GOLD, '*',
    0,	  20,	13,   1,   0,	0,   0,	 0,   0, {0,0}	  ,  1},
{"gold"				,0x00000000L, TV_GOLD, '$',
    0,	  24,	14,   1,   0,	0,   0,	 0,   0, {0,0}	  ,  1},
{"rubies"			,0x00000000L, TV_GOLD, '*',
    0,	  28,	15,   1,   0,	0,   0,	 0,   0, {0,0}	  ,  1},
{"diamonds"			,0x00000000L, TV_GOLD, '*',
    0,	  32,	16,   1,   0,	0,   0,	 0,   0, {0,0}	  ,  1},
{"emeralds"			,0x00000000L, TV_GOLD, '*',
    0,	  40,	17,   1,   0,	0,   0,	 0,   0, {0,0}	  ,  1},
{"mithril"			,0x00000000L, TV_GOLD, '$',	/* 416 */
    0,	  80,	18,   1,   0,	0,   0,	 0,   0, {0,0}	  ,  1},
/* nothing, used as inventory place holder */
/* must be stackable, so that can be picked up by inven_carry */
{"nothing"			,0x00000000L,  TV_NOTHING, ' ',	/* 417 */
   0,       0,  64,   0,   0,   0,   0,   0,   0, {0,0}    ,  0},
/* these next two are needed only for the names */
{"& ruined chest"		,0x00000000L,   TV_CHEST, '&',	/* 418 */
   0,	   0,	0,   1, 250,	0,   0,	 0,   0, {0,0}	,  0},
{""				,0x00000000L,  TV_NOTHING, ' ',	/* 419 */
   0,       0,   0,   0,   0,   0,   0,   0,   0, {0,0}  ,  0}
};
#endif

char *special_names[SN_ARRAY_SIZE] = {
	NULL,			"(R)",			"(RA)",
	"(RF)",			"(RC)",			"(RL)",
	"(HA)",			"(DF)",			"(SA)",
	"(SD)",			"(SE)",			"(SU)",
	"(FT)",			"(FB)",			"of Free Action",
	"of Slaying",		"of Clumsiness",	"of Weakness",
	"of Slow Descent",	"of Speed",		"of Stealth",
	"of Slowness",		"of Noise",		"of Great Mass",
	"of Intelligence",	"of Wisdom",		"of Infra-Vision",
	"of Might",		"of Lordliness",	"of the Magi",
	"of Beauty",		"of Seeing",		"of Regeneration",
	"of Stupidity",		"of Dullness",		"of Blindness",
	"of Timidness",		"of Teleportation",	"of Ugliness",
	"of Protection",	"of Irritation",	"of Vulnerability",
	"of Enveloping",	"of Fire",		"of Slay Evil",
	"of Dragon Slaying",	"(Empty)",		"(Locked)",
	"(Poison Needle)",	"(Gas Trap)",		"(Explosion Device)",
	"(Summoning Runes)",	"(Multiple Traps)",	"(Disarmed)",
	"(Unlocked)",		"of Slay Animal"
};

int16 sorted_objects[MAX_DUNGEON_OBJ];

/* Identified objects flags					*/
int8u object_ident[OBJECT_IDENT_SIZE];
int16 t_level[MAX_OBJ_LEVEL+1];
inven_type t_list[MAX_TALLOC];
inven_type inventory[INVEN_ARRAY_SIZE];

/* Treasure related values					*/
int16 inven_ctr = 0;		/* Total different obj's	*/
int16 inven_weight = 0;		/* Cur carried weight	*/
int16 equip_ctr = 0;		/* Cur equipment ctr	*/
int16 tcptr;			/* Cur treasure heap ptr	*/
