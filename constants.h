/*Note to the Wizard:                                            */
/*       Tweaking these constants can *GREATLY* change the game. */
/*       Two years of constant tuning have generated these       */
/*       values.  Minor adjustments are encouraged, but you must */
/*       be very careful not to unbalance the game.  Moria was   */
/*       meant to be challenging, not a give away.  Many         */
/*       adjustments can cause the game to act strangely, or even*/
/*       cause errors.                                           */

/* Current version number of Moria				*/
#define CUR_VERSION 4.87

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/* Changing values below this line may be hazardous to your health! */

/* key bindings option */
#define ORIGINAL 0          /* uses keypad for directions */
#define ROGUE_LIKE 1        /* uses 'rogue' style keys for directions */

/* message line location */
#define MSG_LINE  0

/* number of messages to save in a buffer */
#define SAVED_MSGS 20

/* Dungeon size parameters					*/
#define MAX_HEIGHT  66      /* Multiple of 11; >= 22 */
#define MAX_WIDTH  198    /* Multiple of 33; >= 66 */
#define SCREEN_HEIGHT  22
#define SCREEN_WIDTH   66
#define QUART_HEIGHT (SCREEN_HEIGHT / 4)
#define QUART_WIDTH  (SCREEN_WIDTH / 4)

/* Output dungeon section sizes					*/
#define OUTPAGE_HEIGHT 44     /* 44 lines of dungeon per section       */
#define OUTPAGE_WIDTH  99     /* 100 columns of dungeon per section    */

/* Dungeon generation values					*/
/* Note: The entire design of dungeon can be changed by only     */
/*       slight adjustments here.                                */
#define DUN_TUN_RND      36   /* Random direction (4 is min)           */
#define DUN_TUN_CHG      70   /* Chance of changing direction (99 max) */
#define DUN_TUN_FND      12   /* Distance for auto find to kick in     */
#define DUN_TUN_CON      15   /* Chance of extra tunneling             */
#define DUN_ROO_MEA      32   /* Mean of # of rooms, standard dev2     */
#define DUN_TUN_PEN      25   /* % chance of room doors                */
#define DUN_TUN_JCT      15   /* % chance of doors at tunnel junctions */
#define DUN_STR_DEN      5    /* Density of streamers                  */
#define DUN_STR_RNG      2    /* Width of streamers                    */
#define DUN_STR_MAG      3    /* Number of magma streamers             */
#define DUN_STR_MC       95   /* 1/x chance of treasure per magma      */
#define DUN_STR_QUA      2    /* Number of quartz streamers            */
#define DUN_STR_QC       55   /* 1/x chance of treasure per quartz     */
#define DUN_UNUSUAL      300  /* Level/x chance of unusual room        */

/* Store constants						*/
#define MAX_OWNERS       18   /* Number of owners to choose from       */
#define MAX_STORES       6    /* Number of different stores            */
#define STORE_INVEN_MAX  24   /* Max number of discrete objs in inven  */
#define STORE_CHOICES    26   /* NUMBER of items to choice stock from  */
#define STORE_MAX_INVEN  20   /* Max diff objs in stock before auto sell*/
#define STORE_MIN_INVEN  14   /* Min diff objs in stock before auto buy*/
#define STORE_TURN_AROUND 3   /* Amount of buying and selling normally */
#define INVEN_INIT_MAX   105  /* Size of store init array              */
#define COST_ADJ         1.00 /* Adjust prices for buying and selling  */

/* Treasure constants						*/
#define INVEN_ARRAY_SIZE 35   /* Size of inventory array(Do not change)*/
#define INVEN_MAX      34     /* index of highest entry in inventory array */
#define MAX_OBJ_LEVEL  50     /* Maximum level of magic in dungeon     */
#define OBJ_GREAT      20     /* 1/n Chance of item being a Great Item */
#define MAX_OBJECTS    344    /* Number of objects for universe        */
#define MAX_GOLD       18     /* Number of different types of gold     */
#define MAX_TALLOC     225    /* Max objects per level                 */
#define TREAS_ROOM_ALLOC  7   /* Amount of objects for rooms           */
#define TREAS_ANY_ALLOC   2   /* Amount of objects for corridors       */
#define TREAS_GOLD_ALLOC  2   /* Amount of gold (and gems)             */

/* Magic Treasure Generation constants				*/
/* Note: Number of special objects, and degree of enchantments   */
/*       can be adjusted here.                                   */
#define OBJ_STD_ADJ      1.25 /* Adjust STD per level                  */
#define OBJ_STD_MIN      7    /* Minimum STD                           */
#define OBJ_TOWN_LEVEL   7    /* Town object generation level          */
#define OBJ_BASE_MAGIC   15   /* Base amount of magic                  */
#define OBJ_BASE_MAX     70   /* Max amount of magic                   */
#define OBJ_DIV_SPECIAL  6    /* magic_chance/#  special magic        */
#define OBJ_DIV_CURSED   1.3  /* magic_chance/#  cursed items         */

/* Constants describing limits of certain objects		*/
#define OBJ_LAMP_MAX    15000 /* Maximum amount that lamp can be filled*/
#define OBJ_BOLT_RANGE   18   /* Maximum range of bolts and balls      */
#define OBJ_RUNE_PROT    3000 /* Rune of protection resistance         */

/* Creature constants						*/
#define MAX_CREATURES     279 /* Number of creatures defined for univ  */
#define MAX_MALLOC        100+1 /* Max that can be allocated          */
#define MAX_MALLOC_CHANCE 160 /* 1/x chance of new monster each round  */
#define MAX_MONS_LEVEL     40 /* Maximum level of creatures            */
#define MAX_SIGHT          20 /* Maximum dis a creature can be seen    */
#define MAX_SPELL_DIS      20 /* Maximum dis creat. spell can be cast  */
#define MAX_MON_MULT       75 /* Maximum reproductions on a level      */
#define MON_MULT_ADJ        7 /* High value slows multiplication       */
#define MON_NASTY          50 /* Dun_level/x chance of high level creat*/
#define MIN_MALLOC_LEVEL   14 /* Minimum number of monsters/level      */
#define MIN_MALLOC_TD       4 /* Number of people on town level (day)  */
#define MIN_MALLOC_TN       8 /* Number of people on town level (night)*/
#define WIN_MON_TOT         2 /* Total number of "win" creatures       */
#define WIN_MON_APPEAR     50 /* Level where winning creatures begin   */
#define MON_SUMMON_ADJ      2 /* Adjust level of summoned creatures    */
#define MON_DRAIN_LIFE      2 /* Percent of player exp drained per hit */

/* Trap constants						*/
#define MAX_TRAPA         18  /* Number of defined traps               */
#define MAX_TRAPB         19  /* Includes secret doors                 */

/* Descriptive constants 					*/
#define MAX_COLORS     67     /* Used with potions     */
#define MAX_MUSH       29     /* Used with mushrooms   */
#define MAX_WOODS      41     /* Used with staffs      */
#define MAX_METALS     31     /* Used with wands       */
#define MAX_ROCKS      52     /* Used with rings       */
#define MAX_AMULETS    39     /* Used with amulets     */
#define MAX_SYLLABLES  153    /* Used with scrolls     */

/* Player constants						*/
#define MAX_PLAYER_LEVEL  40  /* Maximum possible character level      */
#define MAX_RACES         8   /* Number of defined races               */
#define MAX_CLASS         6   /* Number of defined classes             */
#define USE_DEVICE        3   /* x> Harder devices x< Easier devices   */
#define MAX_BACKGROUND    128 /* Number of types of histories for univ */
#define PLAYER_FOOD_FULL 10000/* Getting full                          */
#define PLAYER_FOOD_MAX  15000/* Maximum food value, beyond is wasted  */
#define PLAYER_FOOD_FAINT  300/* Character begins fainting             */
#define PLAYER_FOOD_WEAK  1000/* Warn player that he is getting very low*/
#define PLAYER_FOOD_ALERT 2000/* Warn player that he is getting low    */
#define PLAYER_REGEN_FAINT   0.0005   /* Regen factor when fainting    */
#define PLAYER_REGEN_WEAK    0.0015   /* Regen factor when weak        */
#define PLAYER_REGEN_NORMAL  0.0030   /* Regen factor when full        */
#define PLAYER_REGEN_HPBASE  0.0220   /* Min amount hp regen           */
#define PLAYER_REGEN_MNBASE  0.0080   /* Min amount mana regen         */
#define PLAYER_WEIGHT_CAP 130 /* "#"*(1/10 pounds) per strength point  */
#define PLAYER_EXIT_PAUSE 2   /* Pause time before player can re-roll  */

/* Base to hit constants 					*/
#define BTH_LEV_ADJ      3    /* Adjust BTH per level                  */
#define BTH_PLUS_ADJ     3    /* Adjust BTH per plus-to-hit            */
#define BTH_HIT          12   /* Automatic hit; 1/bth_hit              */

/* table size for random() */
#define STATE_SIZE 128

/* magic numbers for players inventory array */
#define INVEN_WIELD 22
#define INVEN_HEAD  23
#define INVEN_NECK  24
#define INVEN_BODY  25
#define INVEN_ARM   26
#define INVEN_HANDS 27
#define INVEN_RIGHT 28
#define INVEN_LEFT  29
#define INVEN_FEET  30
#define INVEN_OUTER 31
#define INVEN_LIGHT 32
#define INVEN_AUX   33
