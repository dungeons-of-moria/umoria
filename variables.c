#include "constants.h"
#include "types.h"

int character_generated = 0;    /* don't save score until char gen finished */
int highscore_fd;	        /* File descriptor to high score file */
int player_max_exp;	        /* Max exp possible    */
char norm_state[STATE_SIZE];	/* normal seed */
char randes_state[STATE_SIZE];	/* For encoding colors */
int randes_seed;                /* for restarting randes_state */
char town_state[STATE_SIZE];	/* Seed for town genera*/
int town_seed;                  /* for restarting town_seed */
int cur_height,cur_width;	/* Cur dungeon size    */
int dun_level;	                /* Cur dungeon level   */
int missile_ctr = 0;             /* Counter for missiles */
int msg_line;	                /* Contains message txt*/
int msg_flag;	                /* Set with first msg  */
vtype old_msg;	                /* Last message	      */
int generate;	                /* Generate next level */
int death = FALSE;	        /* True if died	      */
vtype died_from;	        /* What killed him     */
int find_flag;	                /* Used in MORIA for .(dir) */
int reset_flag;	                /* Used in MORIA, do not move creatures  */
int stat_column = 0;	        /* Column for stats    */
unsigned int print_stat = 0;	/* Flag for stats      */
int turn = 0;	                /* Cur turn of game    */
int wizard1 = FALSE;	        /* Wizard flag	      */
int wizard2 = FALSE;	        /* Wizard flag	      */
/* keeps track of whether or not line has characters on it */
/*  avoid unneccesary clearing of lines */
int used_line[23] = { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
			FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
			FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE};
char password1[12];
char password2[12];

int key_bindings;

/* Operating hours for Moria				-RAK-	*/
/*       X = Open; . = Closed                                   */
char  days[7][80] = { "SUN:XXXXXXXXXXXXXXXXXXXXXXXX",
		    "MON:XXXXXXXX.........XXXXXXX",
		    "TUE:XXXXXXXX.........XXXXXXX",
		    "WED:XXXXXXXX.........XXXXXXX",
		    "THU:XXXXXXXX.........XXXXXXX",
		    "FRI:XXXXXXXX.........XXXXXXX",
		    "SAT:XXXXXXXXXXXXXXXXXXXXXXXX" };

int closing_flag = 0;	         /* Used for closing   */

/* Bit testing array						*/
unsigned int bit_array[32] = {0x00000001, 0x00000002, 0x00000004, 0x00000008,
				0x00000010, 0x00000020, 0x00000040, 0x00000080,
				0x00000100, 0x00000200, 0x00000400, 0x00000800,
				0x00001000, 0x00002000, 0x00004000, 0x00008000,
				0x00010000, 0x00020000, 0x00040000, 0x00080000,
				0x00100000, 0x00200000, 0x00400000, 0x00800000,
				0x01000000, 0x02000000, 0x04000000, 0x08000000,
   			      0x10000000, 0x20000000, 0x40000000, 0x80000000};

/*  Following are calculated from max dungeon sizes		*/
int max_panel_rows,max_panel_cols;
int quart_height,quart_width;
int panel_row,panel_col;
int panel_row_min,panel_row_max;
int panel_col_min,panel_col_max;
int panel_col_prt,panel_row_prt;

/*  Following are all floor definitions				*/
cave_type cave[MAX_HEIGHT][MAX_WIDTH];
/* Values for floor types					*/
/* if fval is more than 4 bits then the save file routines must be changed */
cave_type blank_floor = { 0, 0, 0, FALSE, FALSE, FALSE, FALSE};
floor_type dopen_floor = {1, TRUE};          /* Dark open floor       */
floor_type lopen_floor = {2, TRUE};          /* Light open floor      */
floor_type corr_floor1 = {4, TRUE};          /* Corridor open floor   */
floor_type corr_floor2 = {5, TRUE};          /* Room junction marker  */
floor_type corr_floor3 = {6, TRUE};          /* Door type floor       */
floor_type corr_floor4 = {7, TRUE};          /* Secret door type floor*/
/* Floor values 8 and 9 are used in generate             */
floor_type rock_wall1 = {10, FALSE};         /* Granite rock wall     */
floor_type rock_wall2 = {11, FALSE};         /* Magma rock wall       */
floor_type rock_wall3 = {12, FALSE};         /* Quartz rock wall      */
floor_type boundary_wall = {15, FALSE};       /* Indestructible wall   */

/* Following are player variables				*/
/* Player record is special					*/
player_type py = {
  {"", "", "", "", "",
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     "", "", "", "", ""
 },
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,7500,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,
     FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE}
 };

/* Class titles for different levels				*/
btype player_title[MAX_CLASS][MAX_PLAYER_LEVEL] = {
	/* Warrior       */
{"Novice","Veteran(1st)","Veteran(2nd)","Veteran(3rd)","Warrior(1st)",
"Warrior(2nd)","Warrior(3rd)","Swordsman-1","Swordsman-2","Swordsman-3",
"Hero","Swashbuckler","Myrmidon","Champion-1","Champion-2","Champion-3",
"Superhero","Knight","Superior Knt","Gallant Knt","Knt Errant","Keeper",
"Protector","Defender","Warder","Guardian Knt","Chevalier","Justiciar",
"Lord (1st)","Lord (2nd)","Lord (3rd)","Lord (4th)","Lord (5th)","Lord (6th)",
"Lord (7th)","Lord (8th)","Lord (9th)","Lord Gallant","Lord Keeper",
"Lord Noble"},
	/* Mage          */
{"Novice","Apprentice","Trickster-1","Trickster-2","Trickster-3","Cabalist-1",
"Cabalist-2","Cabalist-3","Visionist","Phantasmist","Shadowist","Spellbinder",
"Illusionist","Evoker (1st)","Evoker (2nd)","Evoker (3rd)","Evoker (4th)",
"Conjurer","Theurgist","Thaumaturge","Magician","Enchanter","Warlock",
"Sorcerer","Necromancer","Mage (1st)","Mage (2nd)","Mage (3rd)","Mage (4th)",
"Mage (5th)","Wizard (1st)","Wizard (2nd)","Wizard (3rd)","Wizard (4th)",
"Wizard (5th)","Wizard (6th)","Wizard (7th)","Wizard (8th)","Wizard (9th)",
"Wizard Lord"},
	/* Priests       */
{"Believer","Acolyte(1st)","Acolyte(2nd)","Acolyte(3rd)","Adept (1st)",
"Adept (2nd)","Adept (3rd)","Priest (1st)","Priest (2nd)","Priest (3rd)",
"Priest (4th)","Priest (5th)","Priest (6th)","Priest (7th)","Priest (8th)",
"Priest (9th)","Curate (1st)","Curate (2nd)","Curate (3rd)","Curate (4th)",
"Curate (5th)","Curate (6th)","Curate (7th)","Curate (8th)","Curate (9th)",
"Canon (1st)","Canon (2nd)","Canon (3rd)","Canon (4th)","Canon (5th)",
"Low Lama","Lama-1","Lama-2","Lama-3","High Lama","Great Lama","Patriarch",
"High Priest","Great Priest","Noble Priest"},
	/* Rogues        */
{"Apprentice","Footpad","Cutpurse","Robber","Burglar","Filcher","Sharper",
"Magsman","Common Rogue","Rogue (1st)","Rogue (2nd)","Rogue (3rd)",
"Rogue (4th)","Rogue (5th)","Rogue (6th)","Rogue (7th)","Rogue (8th)",
"Rogue (9th)","Master Rogue","Expert Rogue","Senior Rogue","Chief Rogue",
"Prime Rogue","Low Thief","Thief (1st)","Thief (2nd)","Thief (3rd)",
"Thief (4th)","Thief (5th)","Thief (6th)","Thief (7th)","Thief (8th)",
"Thief (9th)","High Thief","Master Thief","Executioner","Low Assassin",
"Assassin","High Assassin","Guildsmaster"},
	/* Rangers       */
{"Runner (1st)","Runner (2nd)","Runner (3rd)","Strider (1st)","Strider (2nd)",
"Strider (3rd)","Scout (1st)","Scout (2nd)","Scout (3rd)","Scout (4th)",
"Scout (5th)","Courser (1st)","Courser (2nd)","Courser (3rd)","Courser (4th)",
"Courser (5th)","Tracker (1st)","Tracker (2nd)","Tracker (3rd)",
"Tracker (4th)","Tracker (5th)","Tracker (6th)","Tracker (7th)",
"Tracker (8th)","Tracker (9th)","Guide (1st)","Guide (2nd)","Guide (3rd)",
"Guide (4th)","Guide (5th)","Guide (6th)","Guide (7th)","Guide (8th)",
"Guide (9th)","Pathfinder-1","Pathfinder-2","Pathfinder-3","Ranger",
"High Ranger","Ranger Lord"},
	/* Paladins      */
{"Gallant","Keeper (1st)","Keeper (2nd)","Keeper (3rd)","Keeper (4th)",
"Keeper (5th)","Keeper (6th)","Keeper (7th)","Keeper (8th)","Keeper (9th)",
"Protector-1","Protector-2","Protector-3","Protector-4","Protector-5",
"Protector-6","Protector-7","Protector-8","Defender-1","Defender-2",
"Defender-3","Defender-4","Defender-5","Defender-6","Defender-7","Defender-8",
"Warder (1st)","Warder (2nd)","Warder (3rd)","Warder (4th)","Warder (5th)",
"Warder (6th)","Warder (7th)","Warder (8th)","Warder (9th)","Guardian",
"Chevalier","Justiciar","Paladin","High Lord"}
};

/* Base experience levels, may be adjusted up for race and/or class*/
int player_exp[MAX_PLAYER_LEVEL] = {
     10,     25,     45,     70,    100,    140,    200,    280,    380,   500,
    650,    850,   1100,   1400,   1800,   2300,   2900,   3600,   4400,  5400,
   6800,   8400,  10200,  12500,  17500,  25000,  35000,  50000,  75000,100000,
 150000, 200000, 300000, 400000, 500000, 750000,1500000,2500000,5000000,9999999
};

double acc_exp = 0.0;	                  /* Accumulator for fractional exp*/
dtype bare_hands = "1d1";
int char_row = 0;
int char_col = 0;
int global_com_val;

/*Race	STR,INT,WIS,DEX,CON,CHR,
        Ages, heights, and weights,
	Racial Bases for: dis,srh,stl,fos,bth,bthb,bsav,hitdie,
	infra, choice-classes                                   */
race_type race[MAX_RACES] = {
   {"Human",     0,  0,  0,  0,  0,  0,
      14,  6, 72,  6,180, 25, 66,  4,120, 20, 1.00,
      0,  0,  0,  0,  0,  0,  0, 10,  0,  0x3F,
    },
   {"Half-Elf", -1,  1,  0,  1, -1,  1,
      24, 16, 66,  6,130, 15, 62,  6,100, 10, 1.10,
      2,  6,  1, -1,  0,  5,  3,  9,  0,  0x3F,
    },
   {"Elf",      -1,  2,  1,  1, -2,  1,
      75, 75, 60,  4,100,  6, 54,  4, 80,  6, 1.20,
      5,  8,  1, -2, -5, 15,  6,  8,  0,  0x1F,
    },
   {"Halfling", -2,  2,  1,  3,  1,  1,
      21, 12, 36,  3, 60,  3, 33,  3, 50,  3, 1.10,
      15, 12,  4, -5,-10, 20, 18,  6,  4,  0x0B,
    },
   {"Gnome",    -1,  2,  0,  2,  1, -2,
      50, 40, 42,  3, 90,  6, 39,  3, 75,  3, 1.25,
      10,  6,  3, -3, -8, 12, 12,  7,  3,  0x0F,
    },
   {"Dwarf",     2, -3,  1, -2,  2, -3,
      35, 15, 48,  3,150, 10, 46,  3,120, 10, 1.20,
      2,  7,  0,  0, 15,  0,  9,  9,  5,  0x05,
    },
   {"Half-Orc",  2, -1,  0,  0,  1, -4,
      11,  4, 66,  1,150,  5, 62,  1,120,  5, 1.10,
      -3,  0, -1,  3, 12, -5, -3, 10,  3,  0x0D,
    },
   {"Half-Troll",4, -4, -2, -4,  3, -6,
      20, 10, 96, 10,300, 50, 84,  8,260, 40, 1.20,
      -5, -1, -2,  5, 20,-10, -8, 12,  3,  0x05,
    }
 };

/* Background information					*/
background_type background[MAX_BACKGROUND] = {
{"You are the illegitimate and unacknowledged child ",           10, 1, 2,-25},
{"You are the illegitimate but acknowledged child ",             20, 1, 2,-15},
{"You are one of several children ",                             95, 1, 2, -5},
{"You are the 1st child ",                                      100, 1, 2,  0},
{"of a Serf.  ",                                                 40, 2, 3, 15},
{"of a Yeoman.  ",                                               65, 2, 3, 30},
{"of a Townsman.  ",                                             80, 2, 3, 40},
{"of a Guildsman.  ",                                            90, 2, 3, 55},
{"of a Landed Knight.  ",                                        96, 2, 3, 70},
{"of a Titled Noble.  ",                                         99, 2, 3, 80},
{"of a Royal Blood Line.  ",                                    100, 2, 3, 90},
{"You are the black sheep of the family.  ",                     20, 3,50,-30},
{"You are a credit to the family.  ",                            80, 3,50,  5},
{"You are a well liked child.  ",                               100, 3,50, 10},
{"Your mother was a Green-Elf.  ",                               40, 4, 1,  0},
{"Your father was a Green-Elf.  ",                               75, 4, 1,  5},
{"Your mother was a Grey-Elf.  ",                                90, 4, 1,  5},
{"Your father was a Grey-Elf.  ",                                95, 4, 1, 10},
{"Your mother was a High-Elf.  ",                                98, 4, 1, 15},
{"Your father was a High-Elf.  ",                               100, 4, 1, 20},
{"You are one of several children ",                             60, 7, 8,  0},
{"You are the only child ",                                     100, 7, 8,  5},
{"of a Green-Elf ",                                              75, 8, 9,  0},
{"of a Grey-Elf ",                                               95, 8, 9,  5},
{"of a High-Elf ",                                              100, 8, 9, 10},
{"Ranger.  ",                                                    40, 9,54, 30},
{"Archer.  ",                                                    70, 9,54, 40},
{"Warrior.  ",                                                   87, 9,54, 60},
{"Mage.  ",                                                      95, 9,54, 75},
{"Prince.  ",                                                    99, 9,54, 90},
{"King.  ",                                                     100, 9,54, 95},
{"You are one of several children of a Halfling ",               85,10,11, -5},
{"You are the only child of a Halfling ",                       100,10,11,  5},
{"Bum.  ",                                                       20,11, 3,  5},
{"Tavern Owner.  ",                                              30,11, 3, 30},
{"Miller.  ",                                                    40,11, 3, 40},
{"Home Owner.  ",                                                50,11, 3, 50},
{"Burglar.  ",                                                   80,11, 3, 60},
{"Warrior.  ",                                                   95,11, 3, 65},
{"Mage.  ",                                                     100,11, 3, 75},
{"Clan Elder.  ",                                               100,11, 3, 90},
{"You are one of several children of a Gnome ",                  85,13,14, -5},
{"You are the only child of a Gnome ",                          100,13,14,  5},
{"Beggar.  ",                                                    20,14, 3,  5},
{"Braggart.  ",                                                   50,14, 3, 20},
{"Prankster.  ",                                                 75,14, 3, 35},
{"Warrior.  ",                                                   95,14, 3, 50},
{"Mage.  ",                                                     100,14, 3, 75},
{"You are one of two children of a Dwarven ",                    25,16,17,-10},
{"You are the only child of a Dwarven ",                        100,16,17,  0},
{"Thief.  ",                                                     10,17,18, 10},
{"Prison Guard.  ",                                              25,17,18, 25},
{"Miner.  ",                                                     75,17,18, 40},
{"Warrior.  ",                                                   90,17,18, 60},
{"Priest.  ",                                                    99,17,18, 80},
{"King.  ",                                                     100,17,18,100},
{"You are the black sheep of the family.  ",                     15,18,57,-40},
{"You are a credit to the family.  ",                            85,18,57,  0},
{"You are a well liked child.  ",                               100,18,57,  5},
{"Your mother was an Orc, but it is unacknowledged.  ",          25,19,20,-25},
{"Your father was an Orc, but it is unacknowledged.  ",         100,19,20,-25},
{"You are the adopted child ",                                  100,20, 2,  0},
{"Your mother was a Cave-Troll ",                                30,22,23,-30},
{"Your father was a Cave-Troll ",                                60,22,23,-25},
{"Your mother was a Hill-Troll ",                                75,22,23,-20},
{"Your father was a Hill-Troll ",                                90,22,23,-15},
{"Your mother was a Water-Troll ",                               95,22,23,-10},
{"Your father was a Water-Troll ",                              100,22,23, -5},
{"Cook.  ",                                                       5,23,62, 10},
{"Warrior.  ",                                                   95,23,62,  5},
{"Shaman.  ",                                                    99,23,62, 15},
{"Clan Chief.  ",                                               100,23,62, 30},
{"You have dark brown eyes, ",                                   20,50,51,  0},
{"You have brown eyes, ",                                        60,50,51,  0},
{"You have hazel eyes, ",                                        70,50,51,  0},
{"You have green eyes, ",                                        80,50,51,  0},
{"You have blue eyes, ",                                         90,50,51,  0},
{"You have blue-gray eyes, ",                                   100,50,51,  0},
{"straight ",                                                    70,51,52,  0},
{"wavy ",                                                       90,51,52,  0},
{"curly ",                                                      100,51,52,  0},
{"black hair, ",                                                 30,52,53,  0},
{"brown hair, ",                                                 70,52,53,  0},
{"auburn hair, ",                                                80,52,53,  0},
{"red hair, ",                                                   90,52,53,  0},
{"blonde hair, ",                                               100,52,53,  0},
{"and a very dark complexion.",                                  10,53,-1,  0},
{"and a dark complexion.",                                       30,53,-1,  0},
{"and an average complexion.",                                   80,53,-1,  0},
{"and a fair complexion.",                                       90,53,-1,  0},
{"and a very fair complexion.",                                 100,53,-1,  0},
{"You have light grey eyes, ",                                   85,54,55,  0},
{"You have light blue eyes, ",                                   95,54,55,  0},
{"You have light green eyes, ",                                 100,54,55,  0},
{"straight ",                                                    75,55,56,  0},
{"wavy ",                                                      100,55,56,  0},
{"black hair, and a fair complexion.",                           75,56,-1,  0},
{"brown hair, and a fair complexion.",                           85,56,-1,  0},
{"blonde hair, and a fair complexion.",                          95,56,-1,  0},
{"silver hair, and a fair complexion.",                         100,56,-1,  0},
{"You have dark brown eyes, ",                                   99,57,58,  0},
{"You have glowing red eyes, ",                                 100,57,58, 10},
{"straight ",                                                    90,58,59,  0},
{"wavy ",                                                      100,58,59,  0},
{"black hair, ",                                                 75,59,60,  0},
{"brown hair, ",                                                100,59,60,  0},
{"a one foot beard, ",                                           25,60,61,  0},
{"a two foot beard, ",                                           60,60,61,  1},
{"a three foot beard, ",                                         90,60,61,  3},
{"a four foot beard, ",                                         100,60,61,  5},
{"and a dark complexion.",                                      100,61,-1,  0},
{"You have slime green eyes, ",                                  60,62,63,  0},
{"You have puke yellow eyes, ",                                  85,62,63,  0},
{"You have blue-bloodshot eyes, ",                               99,62,63,  0},
{"You have glowing red eyes, ",                                 100,62,63,  5},
{"dirty ",                                                       33,63,64,  0},
{"mangy ",                                                       66,63,64,  0},
{"oily ",                                                       100,63,64,  0},
{"sea-weed green hair, ",                                        33,64,65,  0},
{"bright red hair, ",                                            66,64,65,  0},
{"dark purple hair, ",                                          100,64,65,  0},
{"and green ",                                                   25,65,66,  0},
{"and blue ",                                                    50,65,66,  0},
{"and white ",                                                   75,65,66,  0},
{"and black ",                                                  100,65,66,  0},
{"ulcerous skin.",                                               33,66,-1,  0},
{"scabby skin.",                                                 66,66,-1,  0},
{"leprous skin.",                                               100,66,-1,  0}
};

/* Buying and selling adjustments for character race VS store	*/
/* owner race                                                    */
double rgold_adj[MAX_RACES][MAX_RACES] = {
			/*  Hum,  HfE,  Elf,  Hal,  Gno,  Dwa,  HfO,  HfT*/
/*Human          */       { 0.00, 0.05, 0.05, 0.10, 0.13, 0.15, 0.20, 0.25},
/*Half-Elf       */       { 0.10, 0.00, 0.00, 0.05, 0.10, 0.20, 0.25, 0.30},
/*Elf            */       { 0.10, 0.05, 0.00, 0.05, 0.10, 0.20, 0.25, 0.30},
/*Halfling       */       { 0.15, 0.10, 0.05,-0.05, 0.05, 0.10, 0.15, 0.30},
/*Gnome          */       { 0.15, 0.15, 0.10, 0.05,-0.05, 0.10, 0.15, 0.30},
/*Dwarf          */       { 0.15, 0.20, 0.20, 0.10, 0.10,-0.05, 0.25, 0.35},
/*Half-Orc       */       { 0.15, 0.20, 0.25, 0.15, 0.15, 0.30, 0.10, 0.15},
/*Half-Troll     */       { 0.10, 0.15, 0.15, 0.10, 0.10, 0.30, 0.10, 0.10}
			};

/* Classes...							*/
class_type class[MAX_CLASS] = {
{"Warrior", 0.00,  9, 25, 14, 1, 38, 68, 55,30, 5,-2,-2, 2, 2,-1, FALSE,FALSE},
{"Mage",    0.30,  0, 30, 16, 2, 36, 34, 20,20,-5, 3, 0, 0,-2, 0, FALSE,TRUE },
{"Priest",  0.10,  3, 25, 16, 2, 32, 48, 35,20, 0,-3, 3,-1, 1, 2, TRUE ,FALSE},
{"Rogue",   0.00,  6, 45, 32, 4, 16, 60, 66,30, 3, 1,-2, 3, 2,-1, FALSE,TRUE },
{"Ranger",  0.40,  4, 30, 24, 3, 24, 56, 72,25, 2, 2, 0, 1, 1, 1, FALSE,TRUE },
{"Paladin", 0.35,  6, 20, 12, 1, 38, 70, 40,25, 3,-3, 1, 0, 2, 2, TRUE ,FALSE}
};


/* Each character class has a list of spells they can use.  In	*/
/* cases the entire array is blank                               */

 spell_type magic_spell[MAX_CLASS][31] = {
   { /* Warrior */
     {""                            ,99,99,  0,  0,FALSE},
     {""                            ,99,99,  0,  0,FALSE},
     {""                            ,99,99,  0,  0,FALSE},
     {""                            ,99,99,  0,  0,FALSE},
     {""                            ,99,99,  0,  0,FALSE},
     {""                            ,99,99,  0,  0,FALSE},
     {""                            ,99,99,  0,  0,FALSE},
     {""                            ,99,99,  0,  0,FALSE},
     {""                            ,99,99,  0,  0,FALSE},
     {""                            ,99,99,  0,  0,FALSE},
     {""                            ,99,99,  0,  0,FALSE},
     {""                            ,99,99,  0,  0,FALSE},
     {""                            ,99,99,  0,  0,FALSE},
     {""                            ,99,99,  0,  0,FALSE},
     {""                            ,99,99,  0,  0,FALSE},
     {""                            ,99,99,  0,  0,FALSE},
     {""                            ,99,99,  0,  0,FALSE},
     {""                            ,99,99,  0,  0,FALSE},
     {""                            ,99,99,  0,  0,FALSE},
     {""                            ,99,99,  0,  0,FALSE},
     {""                            ,99,99,  0,  0,FALSE},
     {""                            ,99,99,  0,  0,FALSE},
     {""                            ,99,99,  0,  0,FALSE},
     {""                            ,99,99,  0,  0,FALSE},
     {""                            ,99,99,  0,  0,FALSE},
     {""                            ,99,99,  0,  0,FALSE},
     {""                            ,99,99,  0,  0,FALSE},
     {""                            ,99,99,  0,  0,FALSE},
     {""                            ,99,99,  0,  0,FALSE},
     {""                            ,99,99,  0,  0,FALSE},
     {""                            ,99,99,  0,  0,FALSE}
   },
   {               /* Mage          */
     {"Magic Missile"               , 1, 1,  5, 22,FALSE},
     {"Detect Monsters"             , 1, 1,  5, 23,FALSE},
     {"Phase Door"                  , 1, 2,  4, 24,FALSE},
     {"Light Area"                  , 1, 2,  4, 26,FALSE},
     {"Cure Light Wounds"           , 3, 3,  8, 25,FALSE},
     {"Find Hidden Traps/Doors"     , 3, 3,  6, 55,FALSE},
     {"Stinking Cloud"              , 3, 4,  8, 27,FALSE},
     {"Confusion"                   , 3, 4,  6, 30,FALSE},
     {"Lightning Bolt"              , 5, 4, 25, 30,FALSE},
     {"Trap/Door Destruction"       , 5, 5, 30, 30,FALSE},
     {"Sleep I"                     , 5, 5, 20, 30,FALSE},
     {"Cure Poison"                 , 5, 5, 25, 35,FALSE},
     {"Teleport Self"               , 7, 6, 35, 35,FALSE},
     {"Remove Curse"                , 7, 6, 40, 50,FALSE},
     {"Frost Bolt"                  , 7, 6, 50, 40,FALSE},
     {"Turn Stone to Mud"           , 9, 7, 75, 44,FALSE},
     {"Create Food"                 , 9, 7, 75, 45,FALSE},
     {"Recharge Item I"             , 9, 7, 90, 75,FALSE},
     {"Sleep II"                    , 9, 7, 75, 45,FALSE},
     {"Polymorph Other"             ,11, 7,100, 45,FALSE},
     {"Identify"                    ,11, 7, 75, 99,FALSE},
     {"Sleep III"                   ,13, 7, 90, 50,FALSE},
     {"Fire Bolt"                   ,15, 9,100, 50,FALSE},
     {"Slow Monster"                ,17, 9,125, 50,FALSE},
     {"Frost Ball"                  ,19,12,150, 55,FALSE},
     {"Recharge Item II"            ,21,12,175, 90,FALSE},
     {"Teleport Other"              ,23,12,200, 60,FALSE},
     {"Haste Self"                  ,25,12,250, 65,FALSE},
     {"Fire Ball"                   ,29,18,350, 65,FALSE},
     {"Word of Destruction"         ,33,21,500, 80,FALSE},
     {"Genocide"                    ,37,25,800, 95,FALSE}
   },
   {               /* Priest        */
     {"Detect Evil"                 , 1, 1,  3, 10,FALSE},
     {"Cure Light Wounds"           , 1, 2,  4, 15,FALSE},
     {"Bless"                       , 1, 2,  3, 20,FALSE},
     {"Remove Fear"                 , 1, 2,  3, 25,FALSE},
     {"Call Light"                  , 3, 2,  6, 25,FALSE},
     {"Find Traps"                  , 3, 3,  8, 27,FALSE},
     {"Detect Doors/Stairs"         , 3, 3,  8, 27,FALSE},
     {"Slow Poison"                 , 3, 3, 10, 28,FALSE},
     {"Blind Creature"              , 5, 4, 16, 29,FALSE},
     {"Portal"                      , 5, 4, 20, 30,FALSE},
     {"Cure Medium Wounds"          , 5, 4, 20, 32,FALSE},
     {"Chant"                       , 5, 5, 20, 34,FALSE},
     {"Sanctuary"                   , 7, 5, 30, 36,FALSE},
     {"Create Food"                 , 7, 5, 30, 38,FALSE},
     {"Remove Curse"                , 7, 6, 35, 38,FALSE},
     {"Resist Heat and Cold"        , 7, 7, 35, 38,FALSE},
     {"Neutralize Poison"           , 9, 6, 40, 38,FALSE},
     {"Orb of Draining"             , 9, 7, 40, 38,FALSE},
     {"Cure Serious Wounds"         , 9, 7, 40, 40,FALSE},
     {"Sense Invisible"             ,11, 8, 40, 42,FALSE},
     {"Protection from Evil"        ,11, 8, 50, 42,FALSE},
     {"Earthquake"                  ,11, 9, 60, 55,FALSE},
     {"Sense Surroundings"          ,13,10, 60, 45,FALSE},
     {"Cure Critical Wounds"        ,13,11, 65, 45,FALSE},
     {"Turn Undead"                 ,15,12, 80, 50,FALSE},
     {"Prayer"                      ,15,14, 90, 50,FALSE},
     {"Dispel Undead"              ,17,14,125, 55,FALSE},
     {"Heal"                        ,21,16,150, 60,FALSE},
     {"Dispel Evil"                ,25,20,300, 70,FALSE},
     {"Glyph of Warding"            ,33,24,500, 90,FALSE},
     {"Holy Word"                   ,39,32,800, 99,FALSE}
   },
   {               /* Rogue         */
     {"Magic Missile"               ,99,99,  0,  0,FALSE},
     {"Detect Monsters"             , 5, 1,  2, 50,FALSE},
     {"Phase Door"                  , 7, 2,  4, 55,FALSE},
     {"Light Area"                  , 9, 3,  6, 60,FALSE},
     {"Cure Light Wounds"           ,11, 4,  8, 65,FALSE},
     {"Find Hidden Traps/Doors"     ,13, 5, 10, 70,FALSE},
     {"Stinking Cloud"              ,99,99,  0,  0,FALSE},
     {"Confusion"                   ,15, 6, 12, 75,FALSE},
     {"Lightning Bolt"              ,99,99,  0,  0,FALSE},
     {"Trap/Door Destruction"       ,17, 7, 15, 80,FALSE},
     {"Sleep I"                     ,19, 8, 20, 85,FALSE},
     {"Cure Poison"                 ,21, 9, 25, 90,FALSE},
     {"Teleport Self"               ,99,99,  0,  0,FALSE},
     {"Remove Curse"                ,23,10, 30, 95,FALSE},
     {"Frost Bolt"                  ,99,99,  0,  0,FALSE},
     {"Turn Stone to Mud"           ,99,99,  0,  0,FALSE},
     {"Create Food"                 ,25,12, 35, 95,FALSE},
     {"Recharge Item I"             ,27,15, 45, 99,FALSE},
     {"Sleep II"                    ,99,99,  0,  0,FALSE},
     {"Polymorph Other"             ,99,99,  0,  0,FALSE},
     {"Identify"                    ,29,18, 75, 99,FALSE},
     {"Sleep III"                   ,99,99,  0,  0,FALSE},
     {"Fire Bolt"                   ,99,99,  0,  0,FALSE},
     {"Slow Monster"                ,99,99,  0,  0,FALSE},
     {"Frost Ball"                  ,99,99,  0,  0,FALSE},
     {"Recharge Item II"            ,99,99,  0,  0,FALSE},
     {"Teleport Other"              ,99,99,  0,  0,FALSE},
     {"Haste Self"                  ,99,99,  0,  0,FALSE},
     {"Fire Ball"                   ,99,99,  0,  0,FALSE},
     {"Word of Destruction"         ,99,99,  0,  0,FALSE},
     {"Genocide"                    ,99,99,  0,  0,FALSE}
   },
   {               /* Ranger        */
     {"Magic Missile"               , 3, 1,  6, 30,FALSE},
     {"Detect Monsters"             , 3, 2,  6, 35,FALSE},
     {"Phase Door"                  , 3, 2,  8, 35,FALSE},
     {"Light Area"                  , 5, 3,  8, 35,FALSE},
     {"Cure Light Wounds"           , 5, 3,  8, 40,FALSE},
     {"Find Hidden Traps/Doors"     , 5, 4, 10, 45,FALSE},
     {"Stinking Cloud"              , 7, 6, 24, 40,FALSE},
     {"Confusion"                   , 7, 6, 20, 40,FALSE},
     {"Lightning Bolt"              , 9, 7, 30, 40,FALSE},
     {"Trap/Door Destruction"       , 9, 8, 30, 45,FALSE},
     {"Sleep I"                     ,11, 8, 40, 40,FALSE},
     {"Cure Poison"                 ,11, 9, 40, 45,FALSE},
     {"Teleport Self"               ,13,10, 50, 45,FALSE},
     {"Remove Curse"                ,13,11, 50, 55,FALSE},
     {"Frost Bolt"                  ,15,12, 60, 50,FALSE},
     {"Turn Stone to Mud"           ,15,13, 60, 50,FALSE},
     {"Create Food"                 ,17,17, 60, 55,FALSE},
     {"Recharge Item I"             ,17,17, 70, 90,FALSE},
     {"Sleep II"                    ,21,17, 70, 55,FALSE},
     {"Polymorph Other"             ,21,19, 70, 60,FALSE},
     {"Identify"                    ,23,25, 80, 95,FALSE},
     {"Sleep III"                   ,23,20, 80, 60,FALSE},
     {"Fire Bolt"                   ,25,20, 80, 60,FALSE},
     {"Slow Monster"                ,25,21, 80, 65,FALSE},
     {"Frost Ball"                  ,27,21, 90, 65,FALSE},
     {"Recharge Item II"            ,29,23, 90, 95,FALSE},
     {"Teleport Other"              ,31,25,100, 70,FALSE},
     {"Haste Self"                  ,33,25,150, 75,FALSE},
     {"Fire Ball"                   ,35,25,200, 80,FALSE},
     {"Word of Destruction"         ,37,30,500, 95,FALSE},
     {""                            ,99,99,  0,  0,FALSE}
   },
   {               /* Paladin       */
     {"Detect Evil"                 , 1, 1,  4, 30,FALSE},
     {"Cure Light Wounds"           , 2, 2,  8, 35,FALSE},
     {"Bless"                       , 3, 3, 12, 35,FALSE},
     {"Remove Fear"                 , 5, 3, 20, 35,FALSE},
     {"Call Light"                  , 5, 4, 20, 35,FALSE},
     {"Find Traps"                  , 7, 5, 25, 40,FALSE},
     {"Detect Doors/Stairs"         , 7, 5, 25, 40,FALSE},
     {"Slow Poison"                 , 9, 7, 30, 40,FALSE},
     {"Blind Creature"              , 9, 7, 30, 40,FALSE},
     {"Portal"                      , 9, 8, 30, 40,FALSE},
     {"Cure Medium Wounds"          ,11, 9, 40, 40,FALSE},
     {"Chant"                       ,11,10, 40, 45,FALSE},
     {"Sanctuary"                   ,11,10, 40, 45,FALSE},
     {"Create Food"                 ,13,10, 50, 45,FALSE},
     {"Remove Curse"                ,13,11, 50, 45,FALSE},
     {"Resist Heat and Cold"        ,15,13, 60, 45,FALSE},
     {"Neutralize Poison"           ,15,15, 60, 50,FALSE},
     {"Orb of Draining"             ,17,15, 70, 50,FALSE},
     {"Cure Serious Wounds"         ,17,15, 70, 50,FALSE},
     {"Sense Invisible"             ,19,15, 75, 50,FALSE},
     {"Protection from Evil"        ,19,15, 75, 50,FALSE},
     {"Earthquake"                  ,21,17, 80, 50,FALSE},
     {"Sense Surroundings"          ,23,17, 80, 50,FALSE},
     {"Cure Critical Wounds"        ,25,20, 80, 50,FALSE},
     {"Turn Undead"                 ,27,21, 90, 50,FALSE},
     {"Prayer"                      ,29,22, 95, 50,FALSE},
     {"Dispel Undead"              ,31,24,100, 60,FALSE},
     {"Heal"                        ,33,28,125, 60,FALSE},
     {"Dispel Evil"                ,35,32,150, 70,FALSE},
     {"Glyph of Warding"            ,37,36,200, 90,FALSE},
     {"Holy Word"                   ,39,38,500, 95,FALSE}
   }
 };

treasure_type mush = {"& pint~ of fine grade mush",  80, ',', 0x00000000,
			1500, 0, 308, 1, 1, 0, 0, 0, 0, "1d1", 1};

/* Each type of character starts out with a few provisions...	*/
/* Note the the entries refer to array elements of INVENTORY_INIT array*/
byteint player_init[MAX_CLASS][5] = {
		{   0, 103,  41,   6,  32},     /* Warrior       */
		{   0, 103,  41,   6,  66},     /* Mage          */
		{   0, 103,  41,   6,  70},     /* Priest        */
		{   0, 103,   6,  32,  66},     /* Rogue         */
		{   0, 103,  41,   6,  66},     /* Ranger        */
		{   0, 103,  41,   6,  70}      /* Paladin       */
};

int total_winner = FALSE;

/* Following are store definitions				*/

/* Store owners have different characteristics for pricing and haggling*/
/* Note: Store owners should be added in groups, one for each store    */
owner_type owners[MAX_OWNERS] = {
{"Erick the Honest       (Human)      General Store",
	  250, 0.75, 0.08, 0.04, 0, 12},
{"Mauglin the Grumpy     (Dwarf)      Armory"       ,
	32000, 1.00, 0.12, 0.04, 5,  5},
{"Arndal Beast-Slayer    (Half-Elf)   Weaponsmith"  ,
	10000, 0.85, 0.10, 0.05, 1,  8},
{"Hardblow the Humble    (Human)      Temple"       ,
	 3500, 0.75, 0.09, 0.06, 0, 15},
{"Ga-nat the Greedy      (Gnome)      Alchemist"    ,
	12000, 1.20, 0.15, 0.04, 4,  9},
{"Valeria Starshine      (Elf)        Magic Shop"   ,
	32000, 0.75, 0.10, 0.05, 2, 11},
{"Andy the Friendly      (Halfling)   General Store",
	  200, 0.70, 0.08, 0.05, 3, 15},
{"Darg-Low the Grim      (Human)      Armory"       ,
	10000, 0.90, 0.11, 0.04, 0,  9},
{"Oglign Dragon-Slayer   (Dwarf)      Weaponsmith"  ,
	32000, 0.95, 0.12, 0.04, 5,  8},
{"Gunnar the Paladin     (Human)      Temple"       ,
	 5000, 0.85, 0.10, 0.05, 0, 23},
{"Mauser the Chemist     (Half-Elf)   Alchemist"    ,
	10000, 0.90, 0.11, 0.05, 1,  8},
{"Gopher the Great!      (Gnome)      Magic Shop"   ,
	20000, 1.15, 0.13, 0.06, 4, 10},
{"Lyar-el the Comely     (Elf)        General Store",
	  300, 0.65, 0.07, 0.06, 2, 18},
{"Mauglim the Horrible   (Half-Orc)   Armory"       ,
	 3000, 1.00, 0.13, 0.05, 6,  9},
{"Ithyl-Mak the Beastly  (Half-Troll) Weaponsmith"  ,
	 3000, 1.10, 0.15, 0.06, 7,  8},
{"Delilah the Pure       (Half-Elf)   Temple"       ,
	25000, 0.80, 0.07, 0.06, 1, 20},
{"Wizzle the Chaotic     (Halfling)   Alchemist"    ,
	10000, 0.90, 0.10, 0.06, 3,  8},
{"Inglorian the Mage     (Human?)     Magic Shop"   ,
	32000, 1.00, 0.10, 0.07, 0, 10}
};

store_type store[MAX_STORES];

/* Stores are just special traps 		*/
treasure_type store_door[MAX_STORES] = {
{"The entrance to the General Store."              , 110, '1',0x00000000,
    0,      0, 101,   0,   0,   0,   0,   0,   0, "0d0"  ,  0},
{"The entrance to the Armory."                     , 110, '2',0x00000000,
    0,      0, 102,   0,   0,   0,   0,   0,   0, "0d0"  ,  0},
{"The entrance to the Weapon Smiths."              , 110, '3',0x00000000,
    0,      0, 103,   0,   0,   0,   0,   0,   0, "0d0"  ,  0},
{"The entrance to the Temple."                     , 110, '4',0x00000000,
    0,      0, 104,   0,   0,   0,   0,   0,   0, "0d0"  ,  0},
{"The entrance to the Alchemy Shop."               , 110, '5',0x00000000,
    0,      0, 105,   0,   0,   0,   0,   0,   0, "0d0"  ,  0},
{"The entrance to the Magic Shop."                 , 110, '6',0x00000000,
    0,      0, 106,   0,   0,   0,   0,   0,   0, "0d0"  ,  0}
};

int store_choice[MAX_STORES][STORE_CHOICES] = {
	/* General Store */
{105,104,103,102,102,104,42,105,104,27,26,5,4,3,3,2,2,2,1,1,1,1,1,1,1,1
},
	/* Armory        */
{30,31,32,33,34,35,36,37,38,39,40,41,43,44,45,46,47,30,33,34,43,44,28,29,30,31
},
	/* Weaponsmith   */
{ 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25, 6, 7,23,25,23,25
},
	/* Temple        */
{71,72,73,74,59,59,77,79,80,81,84,85,13,14,15,96,97,98,100,79,79,80,80,81,72,98
},
	/* Alchemy shop  */
{55,56,57,58,58,60,61,62,63,64,65,66,75,76,78,82,83,60,61,62,63,64,98,99,98,98
},
	/* Magic-User store*/
{67,49,50,51,52,53,54,48,68,69,69,70,86,87,88,89,90,91,92,93,94,95,86,101,68,88
}
};


/* code for these are all in sets.c  */
int general_store(), armory(), weaponsmith(),
  temple(), alchemist(), magic_shop();

/* Each store will buy only certain items, based on TVAL */
int (*store_buy[MAX_STORES])() = {
       general_store, armory, weaponsmith, temple, alchemist, magic_shop};


/* Following are arrays for descriptive pieces			*/

atype colors[MAX_COLORS] = {
  "Amber","Azure","Blue","Blue Speckled","Blue Spotted",
  "Black","Black Speckled","Black Spotted",
  "Brown","Brown Speckled","Brown Spotted",
  "Bubbling",
  "Chartreuse","Clear","Cloudy",
  "Copper","Copper Spotted","Crimson","Cyan",
  "Dark Blue","Dark Green","Dark Red","Ecru",
  "Gold","Gold Spotted",
  "Green","Green Speckled","Green Spotted",
  "Grey","Grey Spotted","Hazy","Indigo",
  "Light Blue","Light Green","Magenta","Metallic Blue",
  "Metallic Red","Metallic Green","Metallic Purple",
  "Misty",
  "Orange","Orange Speckled","Orange Spotted",
  "Pink","Pink Speckled",
  "Plaid","Puce","Purple","Purple Speckled",
  "Purple Spotted","Red","Red Speckled","Red Spotted",
  "Silver","Silver Speckled","Silver Spotted","Smoky",
  "Tan","Tangerine","Topaz","Turquoise",
  "Violet","Vermilion","White","White Speckled",
  "White Spotted","Yellow"
};

atype mushrooms[MAX_MUSH] = {
  "Blue","Black","Brown","Copper","Crimson",
  "Dark blue","Dark green","Dark red","Gold",
  "Green","Grey","Light Blue","Light Green",
  "Orange","Pink","Plaid","Purple","Red","Tan",
  "Turquoise","Violet","White","Yellow",
  "Wrinkled","Wooden","Slimy","Speckled",
  "Spotted","Furry"
};

atype woods[MAX_WOODS] = {
  "Applewood","Ashen","Aspen","Avocado wood",
  "Balsa","Banyan","Birch","Cedar","Cherrywood",
  "Cinnibar","Cottonwood","Cypress","Dogwood",
  "Driftwood","Ebony","Elm wood","Eucalyptus",
  "Grapevine","Hawthorn","Hemlock","Hickory",
  "Ironwood","Juniper","Locust","Mahogany",
  "Magnolia","Manzanita","Maple","Mulberry",
  "Oak","Pecan","Persimmon","Pine","Redwood",
  "Rosewood","Spruce","Sumac","Sycamore","Teak",
  "Walnut","Zebra wood"
};

atype metals[MAX_METALS] = {
  "Aluminum","Bone","Brass","Bronze","Cast Iron",
  "Chromium","Copper","Gold","Iron","Lead",
  "Magnesium","Molybdenum","Nickel",
  "Pewter","Rusty","Silver","Steel","Tin",
  "Titanium","Tungsten","Zirconium","Zinc",
  "Aluminum Plated","Brass Plated","Copper Plated",
  "Gold Plated","Nickel Plated","Silver Plated",
  "Steel Plated","Tin Plated","Zinc Plated"
};

atype rocks[MAX_ROCKS] = {
  "Amber","Agate","Alexandrite","Amethyst","Antlerite",
  "Aquamarine","Argentite","Azurite","Beryl","Bloodstone",
  "Calcite","Carnelian","Coral","Corundum","Cryolite",
  "Diamond","Diorite","Emerald","Flint","Fluorite",
  "Gabbro","Garnet","Granite","Gypsum","Hematite","Jade",
  "Jasper","Kryptonite","Lapus lazuli","Limestone",
  "Malachite","Manganite","Marble","Moonstone",
  "Neptunite","Obsidian","Onyx","Opal","Pearl","Pyrite",
  "Quartz","Quartzite","Rhodonite","Rhyolite","Ruby",
  "Sapphire","Sphalerite","Staurolite","Tiger eye","Topaz",
  "Turquoise","Zircon"
};

atype amulets[MAX_AMULETS] = {
  "Birch","Cedar","Dogwood","Driftwood",
  "Elm wood","Hemlock","Hickory","Mahogany",
  "Maple","Oak","Pine","Redwood","Rosewood",
  "Walnut","Aluminum","Bone","Brass","Bronze",
  "Copper","Iron","Lead","Nickel","Agate","Amethyst",
  "Diamond","Emerald","Flint","Garnet",
  "Jade","Obsidian","Onyx","Opal","Pearl","Quartz",
  "Ruby","Sapphire","Tiger eye","Topaz","Turquoise"
};

dtype syllables[MAX_SYLLABLES] = {
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
