/* variable.c: Global variables and misc initialized data. */

char *copyright[5] = {
"Copyright (c) 1989 James E. Wilson, Robert A. Keoneke",
"",
"This software may be copied and distributed for educational, research, and",
"not for profit purposes provided that this copyright and statement are",
"included in all such copies."};

#include "constant.h"
#include "config.h"
#include "types.h"

/* a horrible hack: needed because compact_monster() can be called from
   creatures() via summon_monster() and place_monster() */
int hack_monptr = -1;

int weapon_heavy = FALSE;
int pack_heavy = FALSE;
int16 log_index = -1;		/* Index to log file. (<= 0 means no log) */
vtype died_from;

#ifdef MAC
recall_type *c_recall;
#else
recall_type c_recall[MAX_CREATURES];	/* Monster memories */
#endif

vtype savefile;			/* The savefile to use. */

int16 total_winner = FALSE;
int character_generated = 0;	/* don't save score until char gen finished */
int character_saved = 0;	/* prevents save on kill after save_char() */
int highscore_fd;		/* File descriptor to high score file */
int32u randes_seed;		  /* for restarting randes_state */
int32u town_seed;		  /* for restarting town_seed */
int16 cur_height,cur_width;	/* Cur dungeon size    */
int16 dun_level = 0;		/* Cur dungeon level   */
int16 missile_ctr = 0;		/* Counter for missiles */
int msg_flag;			/* Set with first msg  */
vtype old_msg[MAX_SAVE_MSG];	/* Last message	      */
int16 last_msg = 0;		/* Where last is held */
int death = FALSE;		/* True if died	      */
int find_flag;			/* Used in MORIA for .(dir) */
int free_turn_flag;		   /* Used in MORIA, do not move creatures  */
int command_count;		/* Gives repetition of commands. -CJS- */
int default_dir = FALSE;	/* Use last direction for repeated command */
int32 turn = -1;		/* Cur turn of game    */
int wizard = FALSE;		/* Wizard flag	      */
int to_be_wizard = FALSE;	/* used during startup, when -w option used */
int16 panic_save = FALSE;		/* this is true if playing from a panic save */
int16 noscore = FALSE;		/* Don't log the game. -CJS- */

int rogue_like_commands; /* set in config.h/main.c */
int find_cut = TRUE;
int find_examine = TRUE;
int find_bound = FALSE;
int find_prself = FALSE;
int prompt_carry_flag = FALSE;
int show_weight_flag = FALSE;
int highlight_seams = FALSE;
int find_ignore_doors = FALSE;

char doing_inven = FALSE;	/* Track inventory commands. -CJS- */
int screen_change = FALSE; /* Track screen updates for inven_commands. -CJS- */

/* these used to be in dungeon.c */
int new_level_flag;		/* Next level when true	 */
int search_flag = FALSE;	/* Player is searching	 */
int teleport_flag;	/* Handle teleport traps  */
int player_light;	/* Player carrying light */
int eof_flag = FALSE;	/* Used to signal EOF/HANGUP condition */

int wait_for_more = FALSE;

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

int closing_flag = FALSE;		 /* Used for closing   */

/*  Following are calculated from max dungeon sizes		*/
int16 max_panel_rows,max_panel_cols;
int panel_row,panel_col;
int panel_row_min,panel_row_max;
int panel_col_min,panel_col_max;
int panel_col_prt,panel_row_prt;

/*  Following are all floor definitions				*/
#ifdef MAC
cave_type (*cave)[MAX_WIDTH];
#else
cave_type cave[MAX_HEIGHT][MAX_WIDTH];
#endif

/* Following are player variables				*/
/* Player record is special					*/
player_type py;

/* Class titles for different levels				*/
#ifdef MACGAME
char *(*player_title)[MAX_PLAYER_LEVEL];
#else
char *player_title[MAX_CLASS][MAX_PLAYER_LEVEL] = {
	/* Warrior	 */
{"Rookie","Private","Soldier","Mercenary","Veteran(1st)","Veteran(2nd)",
"Veteran(3rd)","Warrior(1st)","Warrior(2nd)","Warrior(3rd)","Warrior(4th)",
"Swordsman-1","Swordsman-2","Swordsman-3","Hero","Swashbuckler","Myrmidon",
"Champion-1","Champion-2","Champion-3","Superhero","Knight","Superior Knt",
"Gallant Knt","Knt Errant","Guardian Knt","Baron","Duke","Lord (1st)",
"Lord (2nd)","Lord (3rd)","Lord (4th)","Lord (5th)","Lord (6th)","Lord (7th)",
"Lord (8th)","Lord (9th)","Lord Gallant","Lord Keeper","Lord Noble"},
	/* Mage		 */
{"Novice","Apprentice","Trickster-1","Trickster-2","Trickster-3","Cabalist-1",
"Cabalist-2","Cabalist-3","Visionist","Phantasmist","Shadowist","Spellbinder",
"Illusionist","Evoker (1st)","Evoker (2nd)","Evoker (3rd)","Evoker (4th)",
"Conjurer","Theurgist","Thaumaturge","Magician","Enchanter","Warlock",
"Sorcerer","Necromancer","Mage (1st)","Mage (2nd)","Mage (3rd)","Mage (4th)",
"Mage (5th)","Wizard (1st)","Wizard (2nd)","Wizard (3rd)","Wizard (4th)",
"Wizard (5th)","Wizard (6th)","Wizard (7th)","Wizard (8th)","Wizard (9th)",
"Wizard Lord"},
	/* Priests	 */
{"Believer","Acolyte(1st)","Acolyte(2nd)","Acolyte(3rd)","Adept (1st)",
"Adept (2nd)","Adept (3rd)","Priest (1st)","Priest (2nd)","Priest (3rd)",
"Priest (4th)","Priest (5th)","Priest (6th)","Priest (7th)","Priest (8th)",
"Priest (9th)","Curate (1st)","Curate (2nd)","Curate (3rd)","Curate (4th)",
"Curate (5th)","Curate (6th)","Curate (7th)","Curate (8th)","Curate (9th)",
"Canon (1st)","Canon (2nd)","Canon (3rd)","Canon (4th)","Canon (5th)",
"Low Lama","Lama-1","Lama-2","Lama-3","High Lama","Great Lama","Patriarch",
"High Priest","Great Priest","Noble Priest"},
	/* Rogues	 */
{"Vagabond","Footpad","Cutpurse","Robber","Burglar","Filcher","Sharper",
"Magsman","Common Rogue","Rogue (1st)","Rogue (2nd)","Rogue (3rd)",
"Rogue (4th)","Rogue (5th)","Rogue (6th)","Rogue (7th)","Rogue (8th)",
"Rogue (9th)","Master Rogue","Expert Rogue","Senior Rogue","Chief Rogue",
"Prime Rogue","Low Thief","Thief (1st)","Thief (2nd)","Thief (3rd)",
"Thief (4th)","Thief (5th)","Thief (6th)","Thief (7th)","Thief (8th)",
"Thief (9th)","High Thief","Master Thief","Executioner","Low Assassin",
"Assassin","High Assassin","Guildsmaster"},
	/* Rangers	 */
{"Runner (1st)","Runner (2nd)","Runner (3rd)","Strider (1st)","Strider (2nd)",
"Strider (3rd)","Scout (1st)","Scout (2nd)","Scout (3rd)","Scout (4th)",
"Scout (5th)","Courser (1st)","Courser (2nd)","Courser (3rd)","Courser (4th)",
"Courser (5th)","Tracker (1st)","Tracker (2nd)","Tracker (3rd)",
"Tracker (4th)","Tracker (5th)","Tracker (6th)","Tracker (7th)",
"Tracker (8th)","Tracker (9th)","Guide (1st)","Guide (2nd)","Guide (3rd)",
"Guide (4th)","Guide (5th)","Guide (6th)","Guide (7th)","Guide (8th)",
"Guide (9th)","Pathfinder-1","Pathfinder-2","Pathfinder-3","Ranger",
"High Ranger","Ranger Lord"},
	/* Paladins	 */
{"Gallant","Keeper (1st)","Keeper (2nd)","Keeper (3rd)","Keeper (4th)",
"Keeper (5th)","Keeper (6th)","Keeper (7th)","Keeper (8th)","Keeper (9th)",
"Protector-1","Protector-2","Protector-3","Protector-4","Protector-5",
"Protector-6","Protector-7","Protector-8","Defender-1","Defender-2",
"Defender-3","Defender-4","Defender-5","Defender-6","Defender-7","Defender-8",
"Warder (1st)","Warder (2nd)","Warder (3rd)","Warder (4th)","Warder (5th)",
"Warder (6th)","Warder (7th)","Warder (8th)","Warder (9th)","Guardian",
"Chevalier","Justiciar","Paladin","High Lord"}
};
#endif

/* Base experience levels, may be adjusted up for race and/or class*/
int32u player_exp[MAX_PLAYER_LEVEL] = {
      10,      25,	45,	 70,	  100,	    140,      200,	280,
     380,     500,     650,	850,	 1100,	   1400,     1800,     2300,
    2900,    3600,    4400,    5400,	 6800,	   8400,    10200,    12500,
   17500,   25000,  35000L,  50000L,   75000L,	100000L,  150000L,  200000L,
 300000L, 400000L, 500000L, 750000L, 1500000L, 2500000L, 5000000L, 10000000L
};

int16u player_hp[MAX_PLAYER_LEVEL];

int16 char_row;
int16 char_col;

/*Race	STR,INT,WIS,DEX,CON,CHR,
	Ages, heights, and weights (male then female)
	Racial Bases for: dis,srh,stl,fos,bth,bthb,bsav,hitdie,
	infra, exp base, choice-classes */
#ifdef MACGAME
race_type *race;
#else
race_type race[MAX_RACES] = {
   {"Human",	 0,  0,	 0,  0,	 0,  0,
      14,  6, 72,  6,180, 25, 66,  4,150, 20,
      0,  0,  0,  0,  0,  0,  0, 10,  0, 100, 0x3F,
    },
   {"Half-Elf", -1,  1,	 0,  1, -1,  1,
      24, 16, 66,  6,130, 15, 62,  6,100, 10,
      2,  6,  1, -1, -1,  5,  3,  9,  2, 110, 0x3F,
    },
   {"Elf",	-1,  2,	 1,  1, -2,  1,
      75, 75, 60,  4,100,  6, 54,  4, 80,  6,
      5,  8,  1, -2, -5, 15,  6,  8,  3, 120, 0x1F,
    },
   {"Halfling", -2,  2,	 1,  3,	 1,  1,
      21, 12, 36,  3, 60,  3, 33,  3, 50,  3,
      15, 12,  4, -5,-10, 20, 18,  6,  4, 110, 0x0B,
    },
   {"Gnome",	-1,  2,	 0,  2,	 1, -2,
      50, 40, 42,  3, 90,  6, 39,  3, 75,  3,
      10,  6,  3, -3, -8, 12, 12,  7,  4, 125, 0x0F,
    },
   {"Dwarf",	 2, -3,	 1, -2,	 2, -3,
      35, 15, 48,  3,150, 10, 46,  3,120, 10,
      2,  7,  -1,  0, 15,  0,  9,  9,  5, 120, 0x05,
    },
   {"Half-Orc",	 2, -1,	 0,  0,	 1, -4,
      11,  4, 66,  1,150,  5, 62,  1,120,  5,
      -3,  0, -1,  3, 12, -5, -3, 10,  3, 110, 0x0D,
    },
   {"Half-Troll",4, -4, -2, -4,	 3, -6,
      20, 10, 96, 10,255, 50, 84,  8,225, 40,
      -5, -1, -2,  5, 20,-10, -8, 12,  3, 120, 0x05,
    }
 };
#endif

/* 5 char race for printing scores. */
char *dsp_race[MAX_RACES] = {
  "Human",
  "H-Elf",
  "Elf  ",
  "Hobbt",
  "Gnome",
  "Dwarf",
  "H-Orc",
  "H-Tro"
};

/* Background information					*/
#ifdef MACGAME
background_type *background;
#else
background_type background[MAX_BACKGROUND] = {
{"You are the illegitimate and unacknowledged child ",		 10, 1, 2, 25},
{"You are the illegitimate but acknowledged child ",		 20, 1, 2, 35},
{"You are one of several children ",				 95, 1, 2, 45},
{"You are the first child ",					100, 1, 2, 50},
{"of a Serf.  ",						 40, 2, 3, 65},
{"of a Yeoman.  ",						 65, 2, 3, 80},
{"of a Townsman.  ",						 80, 2, 3, 90},
{"of a Guildsman.  ",						 90, 2, 3,105},
{"of a Landed Knight.  ",					 96, 2, 3,120},
{"of a Titled Noble.  ",					 99, 2, 3,130},
{"of a Royal Blood Line.  ",					100, 2, 3,140},
{"You are the black sheep of the family.  ",			 20, 3,50, 20},
{"You are a credit to the family.  ",				 80, 3,50, 55},
{"You are a well liked child.  ",				100, 3,50, 60},
{"Your mother was a Green-Elf.  ",				 40, 4, 1, 50},
{"Your father was a Green-Elf.  ",				 75, 4, 1, 55},
{"Your mother was a Grey-Elf.  ",				 90, 4, 1, 55},
{"Your father was a Grey-Elf.  ",				 95, 4, 1, 60},
{"Your mother was a High-Elf.  ",				 98, 4, 1, 65},
{"Your father was a High-Elf.  ",				100, 4, 1, 70},
{"You are one of several children ",				 60, 7, 8, 50},
{"You are the only child ",					100, 7, 8, 55},
{"of a Green-Elf ",						 75, 8, 9, 50},
{"of a Grey-Elf ",						 95, 8, 9, 55},
{"of a High-Elf ",						100, 8, 9, 60},
{"Ranger.  ",							 40, 9,54, 80},
{"Archer.  ",							 70, 9,54, 90},
{"Warrior.  ",							 87, 9,54,110},
{"Mage.  ",							 95, 9,54,125},
{"Prince.  ",							 99, 9,54,140},
{"King.  ",							100, 9,54,145},
{"You are one of several children of a Halfling ",		 85,10,11, 45},
{"You are the only child of a Halfling ",			100,10,11, 55},
{"Bum.  ",							 20,11, 3, 55},
{"Tavern Owner.  ",						 30,11, 3, 80},
{"Miller.  ",							 40,11, 3, 90},
{"Home Owner.  ",						 50,11, 3,100},
{"Burglar.  ",							 80,11, 3,110},
{"Warrior.  ",							 95,11, 3,115},
{"Mage.  ",							 99,11, 3,125},
{"Clan Elder.  ",						100,11, 3,140},
{"You are one of several children of a Gnome ",			 85,13,14, 45},
{"You are the only child of a Gnome ",				100,13,14, 55},
{"Beggar.  ",							 20,14, 3, 55},
{"Braggart.  ",							 50,14, 3, 70},
{"Prankster.  ",						 75,14, 3, 85},
{"Warrior.  ",							 95,14, 3,100},
{"Mage.  ",							100,14, 3,125},
{"You are one of two children of a Dwarven ",			 25,16,17, 40},
{"You are the only child of a Dwarven ",			100,16,17, 50},
{"Thief.  ",							 10,17,18, 60},
{"Prison Guard.  ",						 25,17,18, 75},
{"Miner.  ",							 75,17,18, 90},
{"Warrior.  ",							 90,17,18,110},
{"Priest.  ",							 99,17,18,130},
{"King.  ",							100,17,18,150},
{"You are the black sheep of the family.  ",			 15,18,57, 10},
{"You are a credit to the family.  ",				 85,18,57, 50},
{"You are a well liked child.  ",				100,18,57, 55},
{"Your mother was an Orc, but it is unacknowledged.  ",		 25,19,20, 25},
{"Your father was an Orc, but it is unacknowledged.  ",		100,19,20, 25},
{"You are the adopted child ",					100,20, 2, 50},
{"Your mother was a Cave-Troll ",				 30,22,23, 20},
{"Your father was a Cave-Troll ",				 60,22,23, 25},
{"Your mother was a Hill-Troll ",				 75,22,23, 30},
{"Your father was a Hill-Troll ",				 90,22,23, 35},
{"Your mother was a Water-Troll ",				 95,22,23, 40},
{"Your father was a Water-Troll ",				100,22,23, 45},
{"Cook.  ",							  5,23,62, 60},
{"Warrior.  ",							 95,23,62, 55},
{"Shaman.  ",							 99,23,62, 65},
{"Clan Chief.  ",						100,23,62, 80},
{"You have dark brown eyes, ",					 20,50,51, 50},
{"You have brown eyes, ",					 60,50,51, 50},
{"You have hazel eyes, ",					 70,50,51, 50},
{"You have green eyes, ",					 80,50,51, 50},
{"You have blue eyes, ",					 90,50,51, 50},
{"You have blue-gray eyes, ",					100,50,51, 50},
{"straight ",							 70,51,52, 50},
{"wavy ",							 90,51,52, 50},
{"curly ",							100,51,52, 50},
{"black hair, ",						 30,52,53, 50},
{"brown hair, ",						 70,52,53, 50},
{"auburn hair, ",						 80,52,53, 50},
{"red hair, ",							 90,52,53, 50},
{"blond hair, ",						100,52,53, 50},
{"and a very dark complexion.",					 10,53, 0, 50},
{"and a dark complexion.",					 30,53, 0, 50},
{"and an average complexion.",					 80,53, 0, 50},
{"and a fair complexion.",					 90,53, 0, 50},
{"and a very fair complexion.",					100,53, 0, 50},
{"You have light grey eyes, ",					 85,54,55, 50},
{"You have light blue eyes, ",					 95,54,55, 50},
{"You have light green eyes, ",					100,54,55, 50},
{"straight ",							 75,55,56, 50},
{"wavy ",							100,55,56, 50},
{"black hair, and a fair complexion.",				 75,56, 0, 50},
{"brown hair, and a fair complexion.",				 85,56, 0, 50},
{"blond hair, and a fair complexion.",				 95,56, 0, 50},
{"silver hair, and a fair complexion.",				100,56, 0, 50},
{"You have dark brown eyes, ",					 99,57,58, 50},
{"You have glowing red eyes, ",					100,57,58, 60},
{"straight ",							 90,58,59, 50},
{"wavy ",							100,58,59, 50},
{"black hair, ",						 75,59,60, 50},
{"brown hair, ",						100,59,60, 50},
{"a one foot beard, ",						 25,60,61, 50},
{"a two foot beard, ",						 60,60,61, 51},
{"a three foot beard, ",					 90,60,61, 53},
{"a four foot beard, ",						100,60,61, 55},
{"and a dark complexion.",					100,61, 0, 50},
{"You have slime green eyes, ",					 60,62,63, 50},
{"You have puke yellow eyes, ",					 85,62,63, 50},
{"You have blue-bloodshot eyes, ",				 99,62,63, 50},
{"You have glowing red eyes, ",					100,62,63, 55},
{"dirty ",							 33,63,64, 50},
{"mangy ",							 66,63,64, 50},
{"oily ",							100,63,64, 50},
{"sea-weed green hair, ",					 33,64,65, 50},
{"bright red hair, ",						 66,64,65, 50},
{"dark purple hair, ",						100,64,65, 50},
{"and green ",							 25,65,66, 50},
{"and blue ",							 50,65,66, 50},
{"and white ",							 75,65,66, 50},
{"and black ",							100,65,66, 50},
{"ulcerous skin.",						 33,66, 0, 50},
{"scabby skin.",						 66,66, 0, 50},
{"leprous skin.",						100,66, 0, 50}
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

/* Classes.							*/
class_type class[MAX_CLASS] = {
/*	  HP Dis Src Stl Fos bth btb sve S  I  W  D Co Ch  Spell Exp  spl */
{"Warrior",9, 25, 14, 1, 38, 70, 55, 18, 5,-2,-2, 2, 2,-1, NONE,    0, 0},
{"Mage",   0, 30, 16, 2, 20, 34, 20, 36,-5, 3, 0, 1,-2, 1, MAGE,   30, 1},
{"Priest", 2, 25, 16, 2, 32, 48, 35, 30,-3,-3, 3,-1, 0, 2, PRIEST, 20, 1},
{"Rogue",  6, 45, 32, 5, 16, 60, 66, 30, 2, 1,-2, 3, 1,-1, MAGE,    0, 5},
{"Ranger", 4, 30, 24, 3, 24, 56, 72, 30, 2, 2, 0, 1, 1, 1, MAGE,   40, 3},
{"Paladin",6, 20, 12, 1, 38, 68, 40, 24, 3,-3, 1, 0, 2, 2, PRIEST, 35, 1}
};

/* making it 16 bits wastes a little space, but saves much signed/unsigned
   headaches in its use */
/* CLA_MISC_HIT is identical to CLA_SAVE, which takes advantage of
   the fact that the save values are independent of the class */
int16 class_level_adj[MAX_CLASS][MAX_LEV_ADJ] = {
/*	       bth    bthb   device  disarm   save/misc hit  */
/* Warrior */ {	4,	4,	2,	2,	3 },
/* Mage    */ { 2,	2,	4,	3,	3 },
/* Priest  */ { 2,	2,	4,	3,	3 },
/* Rogue   */ { 3,	4,	3,	4,	3 },
/* Ranger  */ { 3,	4,	3,	3,	3 },
/* Paladin */ { 3,	3,	3,	2,	3 }
};

/* Each character class has a list of spells they can use.  In	*/
/* cases the entire array is blank				 */

int32u spell_learned = 0;
int32u spell_worked = 0;
int32u spell_forgotten = 0;
int8u spell_order[32];

/* Warriors don't have spells, so there is no entry for them.  Note that
   this means you must always subtract one from the py.misc.pclass before
   indexing into magic_spell[]. */
#ifdef MACGAME
spell_type (*magic_spell)[31];
#else
spell_type magic_spell[MAX_CLASS-1][31] = {
  {		  /* Mage	   */
     {	1,  1, 22,   1},
     {	1,  1, 23,   1},
     {	1,  2, 24,   1},
     {	1,  2, 26,   1},
     {	3,  3, 25,   2},
     {	3,  3, 25,   1},
     {	3,  3, 27,   2},
     {	3,  4, 30,   1},
     {	5,  4, 30,   6},
     {	5,  5, 30,   8},
     {	5,  5, 30,   5},
     {	5,  5, 35,   6},
     {	7,  6, 35,   9},
     {	7,  6, 50,  10},
     {	7,  6, 40,  12},
     {	9,  7, 44,  19},
     {	9,  7, 45,  19},
     {	9,  7, 75,  22},
     {	9,  7, 45,  19},
     { 11,  7, 45,  25},
     { 11,  7, 99,  19},
     { 13,  7, 50,  22},
     { 15,  9, 50,  25},
     { 17,  9, 50,  31},
     { 19, 12, 55,  38},
     { 21, 12, 90,  44},
     { 23, 12, 60,  50},
     { 25, 12, 65,  63},
     { 29, 18, 65,  88},
     { 33, 21, 80, 125},
     { 37, 25, 95, 200}
   },
   {		  /* Priest	   */
     {	1,  1, 10,   1},
     {	1,  2, 15,   1},
     {	1,  2, 20,   1},
     {	1,  2, 25,   1},
     {	3,  2, 25,   1},
     {	3,  3, 27,   2},
     {	3,  3, 27,   2},
     {	3,  3, 28,   3},
     {	5,  4, 29,   4},
     {	5,  4, 30,   5},
     {	5,  4, 32,   5},
     {	5,  5, 34,   5},
     {	7,  5, 36,   6},
     {	7,  5, 38,   7},
     {	7,  6, 38,   9},
     {	7,  7, 38,   9},
     {	9,  6, 38,  10},
     {	9,  7, 38,  10},
     {	9,  7, 40,  10},
     { 11,  8, 42,  10},
     { 11,  8, 42,  12},
     { 11,  9, 55,  15},
     { 13, 10, 45,  15},
     { 13, 11, 45,  16},
     { 15, 12, 50,  20},
     { 15, 14, 50,  22},
     { 17, 14, 55,  32},
     { 21, 16, 60,  38},
     { 25, 20, 70,  75},
     { 33, 24, 90, 125},
     { 39, 32, 99, 200}
   },
   {		  /* Rogue	   */
     { 99, 99,	0,   0},
     {	5,  1, 50,   1},
     {	7,  2, 55,   1},
     {	9,  3, 60,   2},
     { 11,  4, 65,   2},
     { 13,  5, 70,   3},
     { 99, 99,	0,   0},
     { 15,  6, 75,   3},
     { 99, 99,	0,   0},
     { 17,  7, 80,   4},
     { 19,  8, 85,   5},
     { 21,  9, 90,   6},
     { 99, 99,	0,   0},
     { 23, 10, 95,   7},
     { 99, 99,	0,   0},
     { 99, 99,	0,   0},
     { 25, 12, 95,   9},
     { 27, 15, 99,  11},
     { 99, 99,	0,   0},
     { 99, 99,	0,   0},
     { 29, 18, 99,  19},
     { 99, 99,	0,   0},
     { 99, 99,	0,   0},
     { 99, 99,	0,   0},
     { 99, 99,	0,   0},
     { 99, 99,	0,   0},
     { 99, 99,	0,   0},
     { 99, 99,	0,   0},
     { 99, 99,	0,   0},
     { 99, 99,	0,   0},
     { 99, 99,	0,   0},
   },
   {		   /* Ranger	    */
     {	3,  1, 30,   1},
     {	3,  2, 35,   2},
     {	3,  2, 35,   2},
     {	5,  3, 35,   2},
     {	5,  3, 40,   2},
     {	5,  4, 45,   3},
     {	7,  5, 40,   6},
     {	7,  6, 40,   5},
     {	9,  7, 40,   7},
     {	9,  8, 45,   8},
     { 11,  8, 40,  10},
     { 11,  9, 45,  10},
     { 13, 10, 45,  12},
     { 13, 11, 55,  13},
     { 15, 12, 50,  15},
     { 15, 13, 50,  15},
     { 17, 17, 55,  15},
     { 17, 17, 90,  17},
     { 21, 17, 55,  17},
     { 21, 19, 60,  18},
     { 23, 25, 95,  20},
     { 23, 20, 60,  20},
     { 25, 20, 60,  20},
     { 25, 21, 65,  20},
     { 27, 21, 65,  22},
     { 29, 23, 95,  23},
     { 31, 25, 70,  25},
     { 33, 25, 75,  38},
     { 35, 25, 80,  50},
     { 37, 30, 95, 100},
     { 99, 99,	0,   0}
   },
   {		  /* Paladin	   */
     {	1,  1, 30,   1},
     {	2,  2, 35,   2},
     {	3,  3, 35,   3},
     {	5,  3, 35,   5},
     {	5,  4, 35,   5},
     {	7,  5, 40,   6},
     {	7,  5, 40,   6},
     {	9,  7, 40,   7},
     {	9,  7, 40,   8},
     {	9,  8, 40,   8},
     { 11,  9, 40,  10},
     { 11, 10, 45,  10},
     { 11, 10, 45,  10},
     { 13, 10, 45,  12},
     { 13, 11, 45,  13},
     { 15, 13, 45,  15},
     { 15, 15, 50,  15},
     { 17, 15, 50,  17},
     { 17, 15, 50,  18},
     { 19, 15, 50,  19},
     { 19, 15, 50,  19},
     { 21, 17, 50,  20},
     { 23, 17, 50,  20},
     { 25, 20, 50,  20},
     { 27, 21, 50,  22},
     { 29, 22, 50,  24},
     { 31, 24, 60,  25},
     { 33, 28, 60,  31},
     { 35, 32, 70,  38},
     { 37, 36, 90,  50},
     { 39, 38, 95, 100}
   }
 };
#endif

char *spell_names[62] = {
  /* Mage Spells */
  "Magic Missile",  "Detect Monsters",	"Phase Door",  "Light Area",
  "Cure Light Wounds",	"Find Hidden Traps/Doors",  "Stinking Cloud",
  "Confusion",	"Lightning Bolt",  "Trap/Door Destruction", "Sleep I",
  "Cure Poison",  "Teleport Self",  "Remove Curse",  "Frost Bolt",
  "Turn Stone to Mud",	"Create Food",	"Recharge Item I",  "Sleep II",
  "Polymorph Other",  "Identify",  "Sleep III",	 "Fire Bolt",  "Slow Monster",
  "Frost Ball",	 "Recharge Item II", "Teleport Other",	"Haste Self",
  "Fire Ball", "Word of Destruction", "Genocide",
  /* Priest Spells, start at index 31 */
  "Detect Evil",  "Cure Light Wounds",	"Bless",  "Remove Fear", "Call Light",
  "Find Traps",	 "Detect Doors/Stairs",	 "Slow Poison",	 "Blind Creature",
  "Portal",  "Cure Medium Wounds",  "Chant",  "Sanctuary",  "Create Food",
  "Remove Curse",  "Resist Heat and Cold",  "Neutralize Poison",
  "Orb of Draining",  "Cure Serious Wounds",  "Sense Invisible",
  "Protection from Evil",  "Earthquake",  "Sense Surroundings",
  "Cure Critical Wounds",  "Turn Undead",  "Prayer",  "Dispel Undead",
  "Heal",  "Dispel Evil",  "Glyph of Warding",	"Holy Word"
};

/* Each type of character starts out with a few provisions.	*/
/* Note that the entries refer to elements of the object_list[] array*/
/* 344 = Food Ration, 365 = Wooden Torch, 123 = Cloak, 30 = Stiletto,
   103 = Soft Leather Armor, 318 = Beginners-Majik, 322 = Beginners Handbook */
int16u player_init[MAX_CLASS][5] = {
		{ 344, 365, 123,  30, 103},	/* Warrior	 */
		{ 344, 365, 123,  30, 318},	/* Mage		 */
		{ 344, 365, 123,  30, 322},	/* Priest	 */
		{ 344, 365,  30, 103, 318},	/* Rogue	 */
		{ 344, 365, 123,  30, 318},	/* Ranger	 */
		{ 344, 365, 123,  30, 322}	/* Paladin	 */
};

/* Following are store definitions				*/

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

#ifdef MAC
store_type *store;
#else
store_type store[MAX_STORES];
#endif

int16u store_choice[MAX_STORES][STORE_CHOICES] = {
	/* General Store */
{366,365,364,84,84,365,123,366,365,350,349,348,347,346,346,345,345,345,
	344,344,344,344,344,344,344,344},
	/* Armory	 */
{95,96,97,103,104,105,106,110,111,112,119,121,124,125,126,127,129,95,103,
	104,124,125,92,93,95,96},
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
   function randnor() in misc1.c */
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

/* Initialized data which had to be moved from some other file */
/* Since these get modified, macrsrc.c must be able to access them */
/* Otherwise, game cannot be made restartable */
/* dungeon.c */
char last_command = ' ';  /* Memory of previous command. */
/* moria1.c */
/* Track if temporary light about player.  */
int light_flag = FALSE;

