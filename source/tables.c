/* source/tables.c: store/attack/RNG/etc tables and variables

   Copyright (c) 1989-92 James E. Wilson, Robert A. Koeneke

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include "config.h"
#include "constant.h"
#include "types.h"

#ifdef MORIA_HOU
/* Operating hours for Moria				-RAK-	*/
/*	 X = Open; . = Closed					*/
char  days[7][29] = { "SUN:XXXXXXXXXXXXXXXXXXXXXXXX",
		    "MON:XXXXXXXX.........XXXXXXX",
		    "TUE:XXXXXXXX.........XXXXXXX",
		    "WED:XXXXXXXX.........XXXXXXX",
		    "THU:XXXXXXXX.........XXXXXXX",
		    "FRI:XXXXXXXX.........XXXXXXX",
		    "SAT:XXXXXXXXXXXXXXXXXXXXXXXX" };
#endif

#ifdef MAC
store_type *store;
#else
store_type store[MAX_STORES];
#endif

/* Store owners have different characteristics for pricing and haggling*/
/* Note: Store owners should be added in groups, one for each store    */
#ifdef MACGAME
owner_type *owners;
#else
owner_type owners[MAX_OWNERS] = {
{"Erick the Honest       (Human)      General Store",
	  250,	175,  108,    4, 0, 12},
{"Mauglin the Grumpy     (Dwarf)      Armory"	    ,
	32000,	200,  112,    4, 5,  5},
{"Arndal Beast-Slayer    (Half-Elf)   Weaponsmith"  ,
	10000,	185,  110,    5, 1,  8},
{"Hardblow the Humble    (Human)      Temple"	    ,
	 3500,	175,  109,    6, 0, 15},
{"Ga-nat the Greedy      (Gnome)      Alchemist"    ,
	12000,	220,  115,    4, 4,  9},
{"Valeria Starshine      (Elf)        Magic Shop"   ,
	32000,	175,  110,    5, 2, 11},
{"Andy the Friendly      (Halfling)   General Store",
	  200,	170,  108,    5, 3, 15},
{"Darg-Low the Grim      (Human)      Armory"	    ,
	10000,	190,  111,    4, 0,  9},
{"Oglign Dragon-Slayer   (Dwarf)      Weaponsmith"  ,
	32000,	195,  112,    4, 5,  8},
{"Gunnar the Paladin     (Human)      Temple"	    ,
	 5000,	185,  110,    5, 0, 23},
{"Mauser the Chemist     (Half-Elf)   Alchemist"    ,
	10000,	190,  111,    5, 1,  8},
{"Gopher the Great!      (Gnome)      Magic Shop"   ,
	20000,	215,  113,    6, 4, 10},
{"Lyar-el the Comely     (Elf)        General Store",
	  300,	165,  107,    6, 2, 18},
{"Mauglim the Horrible   (Half-Orc)   Armory"	    ,
	 3000,	200,  113,    5, 6,  9},
{"Ithyl-Mak the Beastly  (Half-Troll) Weaponsmith"  ,
	 3000,	210,  115,    6, 7,  8},
{"Delilah the Pure       (Half-Elf)   Temple"	    ,
	25000,	180,  107,    6, 1, 20},
{"Wizzle the Chaotic     (Halfling)   Alchemist"    ,
	10000,	190,  110,    6, 3,  8},
{"Inglorian the Mage     (Human?)     Magic Shop"   ,
	32000,	200,  110,    7, 0, 10}
};
#endif

/* Buying and selling adjustments for character race VS store	*/
/* owner race							 */
int8u rgold_adj[MAX_RACES][MAX_RACES] = {
			/*  Hum,  HfE,	Elf,  Hal,  Gno,  Dwa,	HfO,  HfT*/
/*Human		 */	  {  100,  105,	 105,  110,  113,  115,	 120,  125},
/*Half-Elf	 */	  {  110,  100,	 100,  105,  110,  120,	 125,  130},
/*Elf		 */	  {  110,  105,	 100,  105,  110,  120,	 125,  130},
/*Halfling	 */	  {  115,  110,	 105,	95,  105,  110,	 115,  130},
/*Gnome		 */	  {  115,  115,	 110,  105,   95,  110,	 115,  130},
/*Dwarf		 */	  {  115,  120,	 120,  110,  110,   95,	 125,  135},
/*Half-Orc	 */	  {  115,  120,	 125,  115,  115,  130,	 110,  115},
/*Half-Troll	 */	  {  110,  115,	 115,  110,  110,  130,	 110,  110}
			};
/* object_list[] index of objects that may appear in the store */
int16u store_choice[MAX_STORES][STORE_CHOICES] = {
	/* General Store */
{366,365,364,84,84,365,123,366,365,350,349,348,347,346,346,345,345,345,
	344,344,344,344,344,344,344,344},
	/* Armory	 */
{94,95,96,109,103,104,105,106,110,111,112,114,116,124,125,126,127,129,103,
	104,124,125,91,92,95,96},
	/* Weaponsmith	 */
{29,30,34,37,45,49,57,58,59,65,67,68,73,74,75,77,79,80,81,83,29,30,80,83,
	80,83},
	/* Temple	 */
{322,323,324,325,180,180,233,237,240,241,361,362,57,58,59,260,358,359,265,
	237,237,240,240,241,323,359},
	/* Alchemy shop	 */
{173,174,175,351,351,352,353,354,355,356,357,206,227,230,236,252,253,352,
	353,354,355,356,359,363,359,359},
	/* Magic-User store*/
{318,141,142,153,164,167,168,140,319,320,320,321,269,270,282,286,287,292,
	293,294,295,308,269,290,319,282}
};

#ifndef MAC
/* MPW doesn't seem to handle this very well, so replace store_buy array
   with a function call on mac */
/* functions defined in sets.c */
extern int general_store(), armory(), weaponsmith(), temple(),
  alchemist(), magic_shop();

/* Each store will buy only certain items, based on TVAL */
int (*store_buy[MAX_STORES])() = {
       general_store, armory, weaponsmith, temple, alchemist, magic_shop};
#endif

/* Following are arrays for descriptive pieces			*/

#ifdef MACGAME

char **colors;
char **mushrooms;
char **woods;
char **metals;
char **rocks;
char **amulets;
char **syllables;

#else

char *colors[MAX_COLORS] = {
/* Do not move the first three */
  "Icky Green", "Light Brown", "Clear",
  "Azure","Blue","Blue Speckled","Black","Brown","Brown Speckled","Bubbling",
  "Chartreuse","Cloudy","Copper Speckled","Crimson","Cyan","Dark Blue",
  "Dark Green","Dark Red","Gold Speckled","Green","Green Speckled","Grey",
  "Grey Speckled","Hazy","Indigo","Light Blue","Light Green","Magenta",
  "Metallic Blue","Metallic Red","Metallic Green","Metallic Purple","Misty",
  "Orange","Orange Speckled","Pink","Pink Speckled","Puce","Purple",
  "Purple Speckled","Red","Red Speckled","Silver Speckled","Smoky",
  "Tangerine","Violet","Vermilion","White","Yellow"
};

char *mushrooms[MAX_MUSH] = {
  "Blue","Black","Black Spotted","Brown","Dark Blue","Dark Green","Dark Red",
  "Ecru","Furry","Green","Grey","Light Blue","Light Green","Plaid","Red",
  "Slimy","Tan","White","White Spotted","Wooden","Wrinkled","Yellow",
};

char *woods[MAX_WOODS] = {
  "Aspen","Balsa","Banyan","Birch","Cedar","Cottonwood","Cypress","Dogwood",
  "Elm","Eucalyptus","Hemlock","Hickory","Ironwood","Locust","Mahogany",
  "Maple","Mulberry","Oak","Pine","Redwood","Rosewood","Spruce","Sycamore",
  "Teak","Walnut",
};

char *metals[MAX_METALS] = {
  "Aluminum","Cast Iron","Chromium","Copper","Gold","Iron","Magnesium",
  "Molybdenum","Nickel","Rusty","Silver","Steel","Tin","Titanium","Tungsten",
  "Zirconium","Zinc","Aluminum-Plated","Copper-Plated","Gold-Plated",
  "Nickel-Plated","Silver-Plated","Steel-Plated","Tin-Plated","Zinc-Plated"
};

char *rocks[MAX_ROCKS] = {
  "Alexandrite","Amethyst","Aquamarine","Azurite","Beryl","Bloodstone",
  "Calcite","Carnelian","Corundum","Diamond","Emerald","Fluorite","Garnet",
  "Granite","Jade","Jasper","Lapis Lazuli","Malachite","Marble","Moonstone",
  "Onyx","Opal","Pearl","Quartz","Quartzite","Rhodonite","Ruby","Sapphire",
  "Tiger Eye","Topaz","Turquoise","Zircon"
};

char *amulets[MAX_AMULETS] = {
  "Amber","Driftwood","Coral","Agate","Ivory","Obsidian",
  "Bone","Brass","Bronze","Pewter","Tortoise Shell"
};

char *syllables[MAX_SYLLABLES] = {
  "a","ab","ag","aks","ala","an","ankh","app",
  "arg","arze","ash","aus","ban","bar","bat","bek",
  "bie","bin","bit","bjor","blu","bot","bu",
  "byt","comp","con","cos","cre","dalf","dan",
  "den","doe","dok","eep","el","eng","er","ere","erk",
  "esh","evs","fa","fid","for","fri","fu","gan",
  "gar","glen","gop","gre","ha","he","hyd","i",
  "ing","ion","ip","ish","it","ite","iv","jo",
  "kho","kli","klis","la","lech","man","mar",
  "me","mi","mic","mik","mon","mung","mur","nej",
  "nelg","nep","ner","nes","nis","nih","nin","o",
  "od","ood","org","orn","ox","oxy","pay","pet",
  "ple","plu","po","pot","prok","re","rea","rhov",
  "ri","ro","rog","rok","rol","sa","san","sat",
  "see","sef","seh","shu","ski","sna","sne","snik",
  "sno","so","sol","sri","sta","sun","ta","tab",
  "tem","ther","ti","tox","trol","tue","turs","u",
  "ulk","um","un","uni","ur","val","viv","vly",
  "vom","wah","wed","werg","wex","whon","wun","x",
  "yerg","yp","zun"
};
#endif

/* used to calculate the number of blows the player gets in combat */
int8u blows_table[7][6] = {
/* STR/W:	   9  18  67 107 117 118   : DEX */
/* <2 */	{  1,  1,  1,  1,  1,  1 },
/* <3 */	{  1,  1,  1,  1,  2,  2 },
/* <4 */	{  1,  1,  1,  2,  2,  3 },
/* <5 */	{  1,  1,  2,  2,  3,  3 },
/* <7 */	{  1,  2,  2,  3,  3,  4 },
/* <9 */	{  1,  2,  2,  3,  4,  4 },
/* >9 */	{  2,  2,  3,  3,  4,  4 }
};

/* this table is used to generate a psuedo-normal distribution.	 See the
   function randnor() in misc1.c, this is much faster than calling
   transcendental function to calculate a true normal distribution */
int16u normal_table[NORMAL_TABLE_SIZE] = {
     206,     613,    1022,    1430,	1838,	 2245,	  2652,	   3058,
    3463,    3867,    4271,    4673,	5075,	 5475,	  5874,	   6271,
    6667,    7061,    7454,    7845,	8234,	 8621,	  9006,	   9389,
    9770,   10148,   10524,   10898,   11269,	11638,	 12004,	  12367,
   12727,   13085,   13440,   13792,   14140,	14486,	 14828,	  15168,
   15504,   15836,   16166,   16492,   16814,	17133,	 17449,	  17761,
   18069,   18374,   18675,   18972,   19266,	19556,	 19842,	  20124,
   20403,   20678,   20949,   21216,   21479,	21738,	 21994,	  22245,
   22493,   22737,   22977,   23213,   23446,	23674,	 23899,	  24120,
   24336,   24550,   24759,   24965,   25166,	25365,	 25559,	  25750,
   25937,   26120,   26300,   26476,   26649,	26818,	 26983,	  27146,
   27304,   27460,   27612,   27760,   27906,	28048,	 28187,	  28323,
   28455,   28585,   28711,   28835,   28955,	29073,	 29188,	  29299,
   29409,   29515,   29619,   29720,   29818,	29914,	 30007,	  30098,
   30186,   30272,   30356,   30437,   30516,	30593,	 30668,	  30740,
   30810,   30879,   30945,   31010,   31072,	31133,	 31192,	  31249,
   31304,   31358,   31410,   31460,   31509,	31556,	 31601,	  31646,
   31688,   31730,   31770,   31808,   31846,	31882,	 31917,	  31950,
   31983,   32014,   32044,   32074,   32102,	32129,	 32155,	  32180,
   32205,   32228,   32251,   32273,   32294,	32314,	 32333,	  32352,
   32370,   32387,   32404,   32420,   32435,	32450,	 32464,	  32477,
   32490,   32503,   32515,   32526,   32537,	32548,	 32558,	  32568,
   32577,   32586,   32595,   32603,   32611,	32618,	 32625,	  32632,
   32639,   32645,   32651,   32657,   32662,	32667,	 32672,	  32677,
   32682,   32686,   32690,   32694,   32698,	32702,	 32705,	  32708,
   32711,   32714,   32717,   32720,   32722,	32725,	 32727,	  32729,
   32731,   32733,   32735,   32737,   32739,	32740,	 32742,	  32743,
   32745,   32746,   32747,   32748,   32749,	32750,	 32751,	  32752,
   32753,   32754,   32755,   32756,   32757,	32757,	 32758,	  32758,
   32759,   32760,   32760,   32761,   32761,	32761,	 32762,	  32762,
   32763,   32763,   32763,   32764,   32764,	32764,	 32764,	  32765,
   32765,   32765,   32765,   32766,   32766,	32766,	 32766,	  32766,
};
