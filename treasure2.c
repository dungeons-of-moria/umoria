#include "constants.h"
#include "config.h"
#include "types.h"

/* Identified objects flags					*/
int object_ident[MAX_OBJECTS] = {
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE
};

int t_level[MAX_OBJ_LEVEL+1];

/* Gold list (All types of gold and gems are defined here)	*/
treasure_type gold_list[MAX_GOLD] = {
{"copper"                                          , 100, '$',0x00000000,
    0,      3,   1,   0,   1,   0,   0,   0,   0, "0d0"    ,  1},
{"copper"                                          , 100, '$',0x00000000,
    0,      4,   2,   0,   1,   0,   0,   0,   0, "0d0"    ,  1},
{"copper"                                          , 100, '$',0x00000000,
    0,      5,   3,   0,   1,   0,   0,   0,   0, "0d0"    ,  1},
{"silver"                                          , 100, '$',0x00000000,
    0,      6,   4,   0,   1,   0,   0,   0,   0, "0d0"    ,  1},
{"silver"                                          , 100, '$',0x00000000,
    0,      7,   5,   0,   1,   0,   0,   0,   0, "0d0"    ,  1},
{"silver"                                          , 100, '$',0x00000000,
    0,      8,   6,   0,   1,   0,   0,   0,   0, "0d0"    ,  1},
{"garnets"                                         , 100, '*',0x00000000,
    0,      9,   7,   0,   1,   0,   0,   0,   0, "0d0"    ,  1},
{"garnets"                                         , 100, '*',0x00000000,
    0,     10,   8,   0,   1,   0,   0,   0,   0, "0d0"    ,  1},
{"gold"                                            , 100, '$',0x00000000,
    0,     12,   9,   0,   1,   0,   0,   0,   0, "0d0"    ,  1},
{"gold"                                            , 100, '$',0x00000000,
    0,     14,  10,   0,   1,   0,   0,   0,   0, "0d0"    ,  1},
{"gold"                                            , 100, '$',0x00000000,
    0,     16,  11,   0,   1,   0,   0,   0,   0, "0d0"    ,  1},
{"opals"                                           , 100, '*',0x00000000,
    0,     18,  12,   0,   1,   0,   0,   0,   0, "0d0"    ,  1},
{"sapphires"                                       , 100, '*',0x00000000,
    0,     20,  13,   0,   1,   0,   0,   0,   0, "0d0"    ,  1},
{"gold"                                            , 100, '$',0x00000000,
    0,     24,  14,   0,   1,   0,   0,   0,   0, "0d0"    ,  1},
{"rubies"                                          , 100, '*',0x00000000,
    0,     28,  15,   0,   1,   0,   0,   0,   0, "0d0"    ,  1},
{"diamonds"                                        , 100, '*',0x00000000,
    0,     32,  16,   0,   1,   0,   0,   0,   0, "0d0"    ,  1},
{"emeralds"                                        , 100, '*',0x00000000,
    0,     40,  17,   0,   1,   0,   0,   0,   0, "0d0"    ,  1},
{"mithril"                                         , 100, '$',0x00000000,
    0,     80,  18,   0,   1,   0,   0,   0,   0, "0d0"    ,  1}
};

treasure_type t_list[MAX_TALLOC];
treasure_type inventory[INVEN_ARRAY_SIZE];

/* Items which are sold in the stores are different from dungeon */
/* items so that identify works properly.  Note that the players */
/* receive their initial items from this list, so position is    */
/* very important...                                             */
treasure_type inventory_init[INVEN_INIT_MAX] = {
{"& Ration~ of Food"                               ,  80, ',',0x00000000,
 5000,      3, 307,  10,   5,   0,   0,   0,   0, "0d0"  ,  0}, /*  1*/
{"& Hard Biscuit~"                                 ,  80, ',',0x00000000,
  500,      1, 309,   2,   5,   0,   0,   0,   0, "0d0"  ,  0}, /*  2*/
{"& Strip~ of Beef Jerky"                          ,  80, ',',0x00000000,
 1750,      2, 310,   2,   5,   0,   0,   0,   0, "0d0"  ,  0}, /*  3*/
{"& Pint of Fine Ale"                              ,  80, ',',0x00000000,
  500,      1, 311,  10,   3,   0,   0,   0,   0, "0d0"  ,  0}, /*  4*/
{"& Pint of Fine Wine"                             ,  80, ',',0x00000000,
  400,      2, 312,  10,   1,   0,   0,   0,   0, "0d0"  ,  0}, /*  5*/
{"& Dagger (Misericorde) (%P2,%P3)"                ,  23, '|',0x00000000,
    0,     10,   2,  15,   1,   0,   0,   0,   0, "1d4"  ,  0}, /*  6*/
{"& Dagger (Stiletto) (%P2,%P3)"                   ,  23, '|',0x00000000,
    0,     10,   3,  12,   1,   0,   0,   0,   0, "1d4"  ,  0}, /*  7*/
{"& Bastard Sword (%P2,%P3)"                       ,  23, '|',0x00000000,
    0,    350,   7, 140,   1,   0,   0,   0,   0, "3d4"  ,  0}, /*  8*/
{"& Broadsword (%P2,%P3)"                          ,  23, '|',0x00000000,
    0,    255,  10, 150,   1,   0,   0,   0,   0, "2d5"  ,  0}, /*  9*/
{"& Longsword (%P2,%P3)"                           ,  23, '|',0x00000000,
    0,    300,  18, 130,   1,   0,   0,   0,   0, "1d10" ,  0}, /* 10*/
{"& Small Sword (%P2,%P3)"                         ,  23, '|',0x00000000,
    0,     48,  22,  75,   1,   0,   0,   0,   0, "1d6"  ,  0}, /* 11*/
{"& Broad Axe (%P2,%P3)"                          ,  21, '\\',0x00000000,
    0,    304,   4, 160,   1,   0,   0,   0,   0, "2d5"  ,  0}, /* 12*/
{"& Morningstar (%P2,%P3)"                        ,  21, '\\',0x00000000,
    0,    396,   9, 150,   1,   0,   0,   0,   0, "2d6"  ,  0}, /* 13*/
{"& Mace (%P2,%P3)"                               ,  21, '\\',0x00000000,
    0,    130,  10, 120,   1,   0,   0,   0,   0, "2d4"  ,  0}, /* 14*/
{"& War Hammer (%P2,%P3)"                         ,  21, '\\',0x00000000,
    0,    225,  11, 120,   1,   0,   0,   0,   0, "3d3"  ,  0}, /* 15*/
{"& Halberd (%P2,%P3)"                             ,  22, '/',0x00000000,
    0,    430,   5, 190,   1,   0,   0,   0,   0, "3d4"  ,  0}, /* 16*/
{"& Pike (%P2,%P3)"                                ,  22, '/',0x00000000,
    0,    358,   7, 160,   1,   0,   0,   0,   0, "2d5"  ,  0}, /* 17*/
{"& Spear (%P2,%P3)"                               ,  22, '/',0x00000000,
    0,     36,   8,  50,   1,   0,   0,   0,   0, "1d6"  ,  0}, /* 18*/
{"& Short Bow (%P2)"                               ,  20, '}',0x00000000,
    2,     50,   1,  30,   1,   0,   0,   0,   0, "0d0"  ,  0}, /* 19*/
{"& Long Bow (%P2)"                                ,  20, '}',0x00000000,
    3,    120,   2,  40,   1,   0,   0,   0,   0, "0d0"  ,  0}, /* 20*/
{"& Light Crossbow (%P2)"                          ,  20, '}',0x00000000,
    5,    160,  10, 110,   1,   0,   0,   0,   0, "0d0"  ,  0}, /* 21*/
{"& Sling (%P2)"                                   ,  20, '}',0x00000000,
    1,      5,  20,   5,   1,   0,   0,   0,   0, "0d0"  ,  0}, /* 22*/
{"& Arrow~ (%P2,%P3)"                              ,  12, '{',0x00000000,
    0,      1,   1,   2,   1,   0,   0,   0,   0, "1d4"  ,  0}, /* 23*/
{"& Bolt~ (%P2,%P3)"                               ,  11, '{',0x00000000,
    0,      2,   1,   3,   1,   0,   0,   0,   0, "1d5"  ,  0}, /* 24*/
{"& Iron Shot~ (%P2,%P3)"                          ,  10, '{',0x00000000,
    0,      2,   1,   5,   1,   0,   0,   0,   0, "1d3"  ,  0}, /* 25*/
{"& Pick (%P1) (%P2,%P3)"                         ,  25, '\\',0x20000000,
    1,     50,   1, 150,   1,   0,   0,   0,   0, "1d3"  ,  0}, /* 26*/
{"& Shovel (%P1) (%P2,%P3)"                       ,  25, '\\',0x20000000,
    0,     15,   2,  60,   1,   0,   0,   0,   0, "1d2"  ,  0}, /* 27*/
{"& Pair of Soft Leather Boots [%P6,%P4]"          ,  30, ']',0x00000000,
    0,      7,   2,  30,   1,   0,   0,   2,   0, "1d1"  ,  0}, /* 28*/
{"& Pair of Hard Leather Boots [%P6,%P4]"          ,  30, ']',0x00000000,
    0,     12,   3,  40,   1,   0,   0,   3,   0, "1d1"  ,  0}, /* 29*/
{"& Hard Leather Cap [%P6,%P4]"                    ,  33, ']',0x00000000,
    0,     12,   2,  15,   1,   0,   0,   2,   0, "0d0"  ,  0}, /* 30*/
{"& Metal Cap [%P6,%P4]"                           ,  33, ']',0x00000000,
    0,     30,   3,  20,   1,   0,   0,   3,   0, "1d1"  ,  0}, /* 31*/
{"& Iron Helm [%P6,%P4]"                           ,  33, ']',0x00000000,
    0,     75,   4,  75,   1,   0,   0,   5,   0, "1d3"  ,  0}, /* 32*/
{"Soft Leather Armor [%P6,%P4]"                    ,  36, '(',0x00000000,
    0,     18,   2,  80,   1,   0,   0,   4,   0, "0d0"  ,  0}, /* 33*/
{"Soft Studded Leather [%P6,%P4]"                  ,  36, '(',0x00000000,
    0,     35,   3,  90,   1,   0,   0,   5,   0, "1d1"  ,  0}, /* 34*/
{"Hard Leather Armor [%P6,%P4]"                    ,  36, '(',0x00000000,
    0,     55,   4, 100,   1,  -1,   0,   6,   0, "1d1"  ,  0}, /* 35*/
{"Hard Studded Leather [%P6,%P4]"                  ,  36, '(',0x00000000,
    0,    100,   5, 110,   1,  -1,   0,   7,   0, "1d2"  ,  0}, /* 36*/
{"Leather Scale Mail [%P6,%P4]"                    ,  36, '(',0x00000000,
    0,    330,   9, 140,   1,  -1,   0,  11,   0, "1d1"  ,  0}, /* 37*/
{"Metal Scale Mail [%P6,%P4]"                      ,  35, '[',0x00000000,
    0,    430,   1, 250,   1,  -2,   0,  13,   0, "1d4"  ,  0}, /* 38*/
{"Chain Mail [%P6,%P4]"                            ,  35, '[',0x00000000,
    0,    530,   2, 220,   1,  -2,   0,  14,   0, "1d4"  ,  0}, /* 39*/
{"Partial Plate Armor [%P6,%P4]"                   ,  35, '[',0x00000000,
    0,    900,   9, 260,   1,  -3,   0,  22,   0, "1d6"  ,  0}, /* 40*/
{"Full Plate Armor [%P6,%P4]"                      ,  35, '[',0x00000000,
    0,   1050,  11, 380,   1,  -3,   0,  25,   0, "2d4"  ,  0}, /* 41*/
{"& Cloak [%P6,%P4]"                               ,  32, '(',0x00000000,
    0,      3,   1,  10,   1,   0,   0,   1,   0, "0d0"  ,  0}, /* 42*/
{"& Set of Leather Gloves [%P6,%P4]"               ,  31, ']',0x00000000,
    0,      3,   1,   5,   1,   0,   0,   1,   0, "0d0"  ,  0}, /* 43*/
{"& Set of Gauntlets [%P6,%P4]"                    ,  31, ']',0x00000000,
    0,     35,   2,  25,   1,   0,   0,   2,   0, "1d1"  ,  0}, /* 44*/
{"& Small Leather Shield [%P6,%P4]"                ,  34, ')',0x00000000,
    0,     30,   1,  50,   1,   0,   0,   2,   0, "1d1"  ,  0}, /* 45*/
{"& Medium Leather Shield [%P6,%P4]"               ,  34, ')',0x00000000,
    0,     60,   2,  75,   1,   0,   0,   3,   0, "1d2"  ,  0}, /* 46*/
{"& Small Metal Shield [%P6,%P4]"                  ,  34, ')',0x00000000,
    0,     50,   4,  65,   1,   0,   0,   3,   0, "1d3"  ,  0}, /* 47*/
{"& Ring of Resist Fire"                           ,  45, '=',0x00080000,
    0,    250,  11,   2,   1,   0,   0,   0,   0, "0d0"  ,  0}, /* 48*/
{"& Ring of Resist Cold"                           ,  45, '=',0x00200000,
    0,    250,  12,   2,   1,   0,   0,   0,   0, "0d0"  ,  0}, /* 49*/
{"& Ring of Feather Falling"                       ,  45, '=',0x04000000,
    0,    250,  13,   2,   1,   0,   0,   0,   0, "0d0"  ,  0}, /* 50*/
{"& Ring of Protection [%P4]"                      ,  45, '=',0x00000000,
    0,    100,  24,   2,   1,   0,   0,   0,   0, "0d0"  ,  0}, /* 51*/
{"& Amulet of Charisma (%P1)"                      ,  40, '"',0x00000020,
    0,    250,   6,   3,   1,   0,   0,   0,   0, "0d0"  ,  0}, /* 52*/
{"& Amulet of Slow Digestion"                      ,  40, '"',0x00000080,
    0,    200,   9,   3,   1,   0,   0,   0,   0, "0d0"  ,  0}, /* 53*/
{"& Amulet of Resist Acid"                         ,  40, '"',0x00100000,
    0,    300,  10,   3,   1,   0,   0,   0,   0, "0d0"  ,  0}, /* 54*/
{"& Scroll~ of Enchant Weapon To-Hit"              ,  70, '?',0x00000001,
    0,    125, 300,   5,   1,   0,   0,   0,   0, "0d0"  ,  0}, /* 55*/
{"& Scroll~ of Enchant Weapon To-Dam"              ,  70, '?',0x00000002,
    0,    125, 301,   5,   1,   0,   0,   0,   0, "0d0"  ,  0}, /* 56*/
{"& Scroll~ of Enchant Armor"                      ,  70, '?',0x00000004,
    0,    125, 302,   5,   1,   0,   0,   0,   0, "0d0"  ,  0}, /* 57*/
{"& Scroll~ of Identify"                           ,  70, '?',0x00000008,
    0,     50, 303,   5,   2,   0,   0,   0,   0, "0d0"  ,  0}, /* 58*/
{"& Scroll~ of Remove Curse"                       ,  70, '?',0x00000010,
    0,    100, 304,   5,   1,   0,   0,   0,   0, "0d0"  ,  0}, /* 59*/
{"& Scroll~ of Light"                              ,  70, '?',0x00000020,
    0,     15, 305,   5,   3,   0,   0,   0,   0, "0d0"  ,  0}, /* 60*/
{"& Scroll~ of Phase Door"                         ,  70, '?',0x00000080,
    0,     15, 306,   5,   2,   0,   0,   0,   0, "0d0"  ,  0}, /* 61*/
{"& Scroll~ of Magic Mapping"                      ,  70, '?',0x00000800,
    0,     40, 307,   5,   2,   0,   0,   0,   0, "0d0"  ,  0}, /* 62*/
{"& Scroll~ of Treasure Detection"                 ,  70, '?',0x00004000,
    0,     15, 308,   5,   2,   0,   0,   0,   0, "0d0"  ,  0}, /* 63*/
{"& Scroll~ of Object Detection"                   ,  70, '?',0x00008000,
    0,     15, 309,   5,   2,   0,   0,   0,   0, "0d0"  ,  0}, /* 64*/
{"& Scroll~ of Detect Invisible"                   ,  70, '?',0x00080000,
    0,     15, 310,   5,   2,   0,   0,   0,   0, "0d0"  ,  0}, /* 65*/
{"& Scroll~ of Recharging"                         ,  70, '?',0x01000000,
    0,    200, 311,   5,   1,   0,   0,   0,   0, "0d0"  ,  0}, /* 66*/
{"& Book of Magic Spells [Beginners-Magik]"        ,  90, '?',0x0000007F,
    0,     25, 257,  30,   1,-100,   0,   0,   0, "1d1"  ,  0}, /* 67*/
{"& Book of Magic Spells [Magik I]"                ,  90, '?',0x0000FF80,
    0,    100, 258,  30,   1,-100,   0,   0,   0, "1d1"  ,  0}, /* 68*/
{"& Book of Magic Spells [Magik II]"               ,  90, '?',0x00FF0000,
    0,    400, 259,  30,   1,-100,   0,   0,   0, "1d1"  ,  0}, /* 69*/
{"& Book of Magic Spells [The Mage's Guide to Power]",  90, '?',0x7F000000,
    0,    800, 261,  30,   1,-100,   0,   0,   0, "1d1"  ,  0}, /* 70*/
{"& Holy Book of Prayers [Beginners Handbook]"     ,  91, '?',0x000000FF,
    0,     25, 258,  30,   1,-100,   0,   0,   0, "1d1"  ,  0}, /* 71*/
{"& Holy Book of Prayers [Words of Wisdom]"        ,  91, '?',0x0000FF00,
    0,    100, 259,  30,   1,-100,   0,   0,   0, "1d1"  ,  0}, /* 72*/
{"& Holy Book of Prayers [Chants and Blessings]"   ,  91, '?',0x01FF0000,
    0,    300, 260,  30,   1,-100,   0,   0,   0, "1d1"  ,  0}, /* 73*/
{"& Holy Book of Prayers [Exorcism and Dispelling]",  91, '?',0x7E000000,
    0,    900, 261,  30,   1,-100,   0,   0,   0, "1d1"  ,  0}, /* 74*/
{"& Potion~ of Restore Strength"                   ,  75, '!',0x00000004,
    0,    300, 310,   4,   1,   0,   0,   0,   0, "1d1"  ,  0}, /* 75*/
{"& Potion~ of Restore Intelligence"               ,  75, '!',0x00000020,
    0,    300, 311,   4,   1,   0,   0,   0,   0, "1d1"  ,  0}, /* 76*/
{"& Potion~ of Restore Wisdom"                     ,  75, '!',0x00000100,
    0,    300, 312,   4,   1,   0,   0,   0,   0, "1d1"  ,  0}, /* 77*/
{"& Potion~ of Restore Charisma"                   ,  75, '!',0x00000800,
    0,    300, 313,   4,   1,   0,   0,   0,   0, "1d1"  ,  0}, /* 78*/
{"& Potion~ of Cure Light Wounds"                  ,  75, '!',0x10001000,
   50,     15, 314,   4,   2,   0,   0,   0,   0, "1d1"  ,  0}, /* 79*/
{"& Potion~ of Cure Serious Wounds"                ,  75, '!',0x30002000,
  100,     40, 315,   4,   1,   0,   0,   0,   0, "1d1"  ,  0}, /* 80*/
{"& Potion~ of Cure Critical Wounds"               ,  75, '!',0x70004000,
  100,    100, 316,   4,   1,   0,   0,   0,   0, "1d1"  ,  0}, /* 81*/
{"& Potion~ of Restore Dexterity"                  ,  75, '!',0x04000000,
    0,    300, 317,   4,   1,   0,   0,   0,   0, "1d1"  ,  0}, /* 82*/
{"& Potion~ of Restore Constitution"               ,  75, '!',0x68000000,
    0,    300, 318,   4,   1,   0,   0,   0,   0, "1d1"  ,  0}, /* 83*/
{"& Potion~ of Heroism"                            ,  76, '!',0x00000010,
    0,     35, 319,   4,   2,   0,   0,   0,   0, "1d1"  ,  0}, /* 84*/
{"& Potion~ of Boldness"                           ,  76, '!',0x00000040,
    0,     10, 320,   4,   2,   0,   0,   0,   0, "1d1"  ,  0}, /* 85*/
{"& Wand of Light (%P1 charges)"                   ,  65, '-',0x00000001,
    0,    200,   1,  10,   1,   0,   0,   0,   0, "1d1"  ,  2}, /* 86*/
{"& Wand of Lightning Bolts (%P1 charges)"         ,  65, '-',0x00000002,
    0,    600,   2,  10,   1,   0,   0,   0,   0, "1d1"  ,  6}, /* 87*/
{"& Wand of Magic Missile (%P1 charges)"           ,  65, '-',0x00002000,
    0,    200,  14,  10,   1,   0,   0,   0,   0, "1d1"  ,  2}, /* 88*/
{"& Wand of Disarming (%P1 charges)"               ,  65, '-',0x00020000,
    0,    700,  18,  10,   1,   0,   0,   0,   0, "1d1"  , 12}, /* 89*/
{"& Wand of Lightning Balls (%P1 charges)"         ,  65, '-',0x00040000,
    0,   1200,  19,  10,   1,   0,   0,   0,   0, "1d1"  , 20}, /* 90*/
{"& Wand of Wonder (%P1 charges)"                  ,  65, '-',0x00800000,
    0,    250,  24,  10,   1,   0,   0,   0,   0, "1d1"  , 10}, /* 91*/
{"& Staff of Light (%P1 charges)"                  ,  55, '_',0x00000001,
    0,    250,   1,  50,   1,   0,   0,   0,   0, "1d2"  ,  3}, /* 92*/
{"& Staff of Door/Stair Location (%P1 charges)"    ,  55, '_',0x00000002,
    0,    350,   2,  50,   1,   0,   0,   0,   0, "1d2"  ,  7}, /* 93*/
{"& Staff of Trap Location (%P1 charges)"          ,  55, '_',0x00000004,
    0,    350,   3,  50,   1,   0,   0,   0,   0, "1d2"  ,  7}, /* 94*/
{"& Staff of Detect Invisible (%P1 charges)"       ,  55, '_',0x00008000,
    0,    200,  16,  50,   1,   0,   0,   0,   0, "1d2"  ,  3}, /* 95*/
{"& Potion~ of Restore Life Levels"                ,  76, '!',0x00000080,
    0,    400, 321,   4,   1,   0,   0,   0,   0, "1d1"  ,  0}, /* 96*/
{"& Scroll~ of Blessing"                           ,  71, '?',0x00000020,
    0,     15, 312,   5,   2,   0,   0,   0,   0, "0d0"  ,  0}, /* 97*/
{"& Scroll~ of Word-of-Recall"                     ,  71, '?',0x00000100,
    0,    150, 313,   5,   3,   0,   0,   0,   0, "0d0"  ,  0}, /* 98*/
{"& Potion~ of Slow Poison"                        ,  76, '!',0x00000800,
    0,     25, 322,   4,   2,   0,   0,   0,   0, "1d1"  ,  0}, /* 99*/
{"& Potion~ of Neutralize Poison"                  ,  76, '!',0x00001000,
    0,     75, 323,   4,   1,   0,   0,   0,   0, "1d1"  ,  0}, /*100*/
{"& Wand of Stinking Cloud (%P1 charges)"          ,  65, '-',0x00200000,
    0,    400,  22,  10,   1,   0,   0,   0,   0, "1d1"  ,  5}, /*101*/
{"& Iron Spike~"                                   ,  13, '~',0x00000000,
    0,      1,   1,  10,   1,   0,   0,   0,   0, "1d1"  ,  1}, /*102*/
{"& Brass Lantern~ with %P5 turns of light"        ,  15, '~',0x00000000,
 7500,     35,   2,  50,   1,   0,   0,   0,   0, "1d1"  ,  1}, /*103*/
{"& Wooden Torch~ with %P5 turns of light"         ,  15, '~',0x00000000,
 4000,      2, 270,  30,   5,   0,   0,   0,   0, "1d1"  ,  1}, /*104*/
{"& Flask~ of oil"                                 ,  77, '!',0x00040000,
 7500,      3, 257,  10,   5,   0,   0,   0,   0, "2d6"  ,  1}  /*105*/
};

treasure_type blank_treasure = {"nothing", 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, "0d0", 0};

/* Treasure related values					*/
int inven_ctr = 0;	        /* Total different obj's	*/
int inven_weight = 0;   	/* Cur carried weight	*/
int equip_ctr = 0;  	        /* Cur equipment ctr	*/
int tcptr;              	/* Cur treasure heap ptr	*/

/* Following are feature objects defined for dungeon		*/

/* Traps are just Nasty treasures...				*/
treasure_type trap_lista[MAX_TRAPA] = {
{"an open pit"                                     , 102, ' ',0x00000000,
    0,      0,   1,   0,   1,   0,   0,   0,   0, "2d6"  ,-50},
{"an arrow trap"                                   , 101, '.',0x00000000,
    0,      0,   2,   0,   1,   0,   0,   0,   0, "1d8"  ,  0},
{"a covered pit"                                   , 101, '.',0x00000000,
    0,      0,   3,   0,   1,   0,   0,   0,   0, "2d6"  ,  0},
{"a trap door"                                     , 101, '.',0x00000000,
    0,      0,   4,   0,   1,   0,   0,   0,   0, "2d8"  ,  0},
{"a gas trap"                                      , 101, '.',0x00000000,
    0,      0,   5,   0,   1,   0,   0,   0,   0, "1d4"  ,  0},
{"a loose rock"                                    , 101, '.',0x00000000,
    0,      0,   6,   0,   1,   0,   0,   0,   0, "0d0"  ,  0},
{"a dart trap"                                     , 101, '.',0x00000000,
    0,      0,   7,   0,   1,   0,   0,   0,   0, "1d4"  ,  0},
{"a strange rune"                                  , 101, '.',0x00000000,
    0,      0,   8,   0,   1,   0,   0,   0,   0, "0d0"  ,  0},
{"some loose rock"                                 , 101, '.',0x00000000,
    0,      0,   9,   0,   1,   0,   0,   0,   0, "2d6"  ,  0},
{"a gas trap"                                      , 101, '.',0x00000000,
    0,      0,  10,   0,   1,   0,   0,   0,   0, "1d4"  ,  0},
{"a strange rune"                                  , 101, '.',0x00000000,
    0,      0,  11,   0,   1,   0,   0,   0,   0, "0d0"  ,  0},
{"a blackened spot"                                , 101, '.',0x00000000,
    0,      0,  12,   0,   1,   0,   0,   0,   0, "4d6"  ,  0},
{"some corroded rock"                              , 101, '.',0x00000000,
    0,      0,  13,   0,   1,   0,   0,   0,   0, "4d6"  ,  0},
{"a gas trap"                                      , 101, '.',0x00000000,
    0,      0,  14,   0,   1,   0,   0,   0,   0, "2d6"  ,  0},
{"a gas trap"                                      , 101, '.',0x00000000,
    5,      0,  15,   0,   1,   0,   0,   0,   0, "1d4"  , 10},
{"a gas trap"                                      , 101, '.',0x00000000,
    5,      0,  16,   0,   1,   0,   0,   0,   0, "1d8"  ,  5},
{"a dart trap"                                     , 101, '.',0x00000000,
    5,      0,  17,   0,   1,   0,   0,   0,   0, "1d8"  , 10},
{"a dart trap"                                     , 101, '.',0x00000000,
    5,      0,  18,   0,   1,   0,   0,   0,   0, "1d8"  , 10}
};

/* Traps: Level represents the difficulty of disarming;	*/
/* and P1 represents the experienced gained when disarmed*/
treasure_type trap_listb[MAX_TRAPB] = {
{"an open pit"                                     , 102, ' ',0x00000000,
    1,      0,   1,   0,   1,   0,   0,   0,   0, "2d6"  ,-50},
{"an arrow trap"                                   , 102, '^',0x00000000,
    3,      0,   2,   0,   1,   0,   0,   0,   0, "1d8"  ,-10},
{"a covered pit"                                   , 102, '^',0x00000000,
    2,      0,   3,   0,   1,   0,   0,   0,   0, "2d6"  ,-40},
{"a trap door"                                     , 102, '^',0x00000000,
    5,      0,   4,   0,   1,   0,   0,   0,   0, "2d8"  ,-25},
{"a gas trap"                                      , 102, '^',0x00000000,
    3,      0,   5,   0,   1,   0,   0,   0,   0, "1d4"  ,  5},
{"a loose rock"                                    , 102, ';',0x00000000,
    0,      0,   6,   0,   1,   0,   0,   0,   0, "0d0"  ,-90},
{"a dart trap"                                     , 102, '^',0x00000000,
    5,      0,   7,   0,   1,   0,   0,   0,   0, "1d4"  , 10},
{"a strange rune"                                  , 102, '^',0x00000000,
    5,      0,   8,   0,   1,   0,   0,   0,   0, "0d0"  ,-10},
{"some loose rock"                                 , 102, '^',0x00000000,
    5,      0,   9,   0,   1,   0,   0,   0,   0, "2d6"  ,-10},
{"a gas trap"                                      , 102, '^',0x00000000,
   10,      0,  10,   0,   1,   0,   0,   0,   0, "1d4"  ,  5},
{"a strange rune"                                  , 102, '^',0x00000000,
    5,      0,  11,   0,   1,   0,   0,   0,   0, "0d0"  ,-10},
{"a blackened spot"                                , 102, '^',0x00000000,
   10,      0,  12,   0,   1,   0,   0,   0,   0, "4d6"  , 10},
{"some corroded rock"                              , 102, '^',0x00000000,
   10,      0,  13,   0,   1,   0,   0,   0,   0, "4d6"  , 10},
{"a gas trap"                                      , 102, '^',0x00000000,
    5,      0,  14,   0,   1,   0,   0,   0,   0, "2d6"  ,  5},
{"a gas trap"                                      , 102, '^',0x00000000,
    5,      0,  15,   0,   1,   0,   0,   0,   0, "1d4"  , 10},
{"a gas trap"                                      , 102, '^',0x00000000,
    5,      0,  16,   0,   1,   0,   0,   0,   0, "1d8"  ,  5},
{"a dart trap"                                     , 102, '^',0x00000000,
    5,      0,  17,   0,   1,   0,   0,   0,   0, "1d8"  , 10},
{"a dart trap"                                     , 102, '^',0x00000000,
    5,      0,  18,   0,   1,   0,   0,   0,   0, "1d8"  , 10},
	/* Special case, see DOOR_LIST below (subvals must agree)        */
{"a closed door"                                   , 105, '+',0x00000000,
    0,      0,  19,   0,   1,   0,   0,   0,   0, "1d1"  ,  0}
};

/* Special trap	*/
treasure_type scare_monster =
{"a strange rune"                                  , 102, '^',0x00000000,
    0,      0,  99,   0,   1,   0,   0,   0,   0, "0d0"  ,-90};

treasure_type rubble =
{"some rubble"                                     , 103, ':',0x00000000,
    0,      0,   1,   0,   1,   0,   0,   0,   0, "0d0"  ,  0};

/* Secret door must have same subval as closed door in	*/
/* TRAP_LISTB.  See CHANGE_TRAP                          */
treasure_type door_list[3] = {
{"an open door"                                    , 104,'\'',0x00000000,
    0,      0,   1,   0,   1,   0,   0,   0,   0, "1d1"  ,  0},
{"a closed door"                                   , 105, '+',0x00000000,
    0,      0,  19,   0,   1,   0,   0,   0,   0, "1d1"  ,  0},
{"a secret door"                                   , 109, '#',0x00000000,
    0,      0,  19,   0,   1,   0,   0,   0,   0, "1d1"  ,  0}
};

treasure_type up_stair =
{"an up staircase "                                , 107, '<',0x00000000,
    0,      0,   1,   0,   1,   0,   0,   0,   0, "1d1"  ,  0};

treasure_type down_stair =
{"a down staircase"                                , 108, '>',0x00000000,
    0,      0,   1,   0,   1,   0,   0,   0,   0, "1d1"  ,  0};

