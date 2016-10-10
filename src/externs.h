/* source/externs.h: declarations for global variables and initialized data
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

extern int errno;

extern char *copyright[5];

/* horrible hack: needed because compact_monster() can be called from deep
   within creatures() via place_monster() and summon_monster() */
extern int hack_monptr;

extern vtype died_from;
extern vtype savefile;              /* The save file. -CJS- */
extern int32 birth_date;

/* These are options, set with set_options command -CJS- */
extern int rogue_like_commands;
extern int find_cut;                /* Cut corners on a run */
extern int find_examine;            /* Check corners on a run */
extern int find_prself;             /* Print yourself on a run (slower) */
extern int find_bound;              /* Stop run when the map shifts */
extern int prompt_carry_flag;       /* Prompt to pick something up */
extern int show_weight_flag;        /* Display weights in inventory */
extern int highlight_seams;         /* Highlight magma and quartz */
extern int find_ignore_doors;       /* Run through open doors */
extern int sound_beep_flag;         /* Beep for invalid character */
extern int display_counts;          /* Display rest/repeat counts */

/* global flags */
extern int new_level_flag;          /* Next level when true */
extern int teleport_flag;           /* Handle teleport traps */
extern int eof_flag;                /* Used to handle eof/HANGUP */
extern int player_light;            /* Player carrying light */
extern int find_flag;               /* Used in MORIA */
extern int free_turn_flag;          /* Used in MORIA */
extern int weapon_heavy;            /* Flag if the weapon too heavy -CJS- */
extern int pack_heavy;              /* Flag if the pack too heavy -CJS- */
extern char doing_inven;            /* Track inventory commands */
extern int screen_change;           /* Screen changes (used in inven_commands) */

extern int character_generated;     /* don't save score until char gen finished*/
extern int character_saved;         /* prevents save on kill after save_char() */
extern FILE *highscore_fp;          /* High score file pointer */
extern int command_count;           /* Repetition of commands. -CJS- */
extern int default_dir;             /* Use last direction in repeated commands */
extern int16 noscore;               /* Don't score this game. -CJS- */
extern int32u randes_seed;          /* For encoding colors */
extern int32u town_seed;            /* Seed for town genera*/
extern int16 dun_level;             /* Cur dungeon level */
extern int16 missile_ctr;           /* Counter for missiles */
extern int msg_flag;                /* Set with first msg */
extern vtype old_msg[MAX_SAVE_MSG]; /* Last messages -CJS- */
extern int16 last_msg;              /* Where in the array is the last */
extern int death;                   /* True if died */
extern int32 turn;                  /* Cur trun of game */
extern int wizard;                  /* Wizard flag */
extern int to_be_wizard;
extern int16 panic_save;            /* this is true if playing from a panic save */

extern int wait_for_more;

extern char days[7][29];
extern int closing_flag;            /* Used for closing */

extern int16 cur_height, cur_width; /* Cur dungeon size */

/*  Following are calculated from max dungeon sizes */
extern int16 max_panel_rows, max_panel_cols;
extern int panel_row, panel_col;
extern int panel_row_min, panel_row_max;
extern int panel_col_min, panel_col_max;
extern int panel_col_prt, panel_row_prt;

/*  Following are all floor definitions */
extern cave_type cave[MAX_HEIGHT][MAX_WIDTH];

/* Following are player variables */
extern player_type py;
extern char *player_title[MAX_CLASS][MAX_PLAYER_LEVEL];
extern race_type race[MAX_RACES];
extern background_type background[MAX_BACKGROUND];
extern int32u player_exp[MAX_PLAYER_LEVEL];
extern uint16_t player_hp[MAX_PLAYER_LEVEL];
extern int16 char_row;
extern int16 char_col;

extern uint8_t rgold_adj[MAX_RACES][MAX_RACES];

extern class_type class[MAX_CLASS];
extern int16 class_level_adj[MAX_CLASS][MAX_LEV_ADJ];

/* Warriors don't have spells, so there is no entry for them. */
extern spell_type magic_spell[MAX_CLASS - 1][31];
extern char *spell_names[62];
extern int32u spell_learned;        /* Bit field for spells learnt -CJS- */
extern int32u spell_worked;         /* Bit field for spells tried -CJS- */
extern int32u spell_forgotten;      /* Bit field for spells forgotten -JEW- */
extern uint8_t spell_order[32];     /* remember order that spells are learned in */
extern uint16_t player_init[MAX_CLASS][5];
extern int16 total_winner;
extern int32 max_score;

/* Following are store definitions */
extern owner_type owners[MAX_OWNERS];
extern store_type store[MAX_STORES];
extern uint16_t store_choice[MAX_STORES][STORE_CHOICES];
extern int (*store_buy[MAX_STORES])();

/* Following are treasure arrays  and variables */
extern treasure_type object_list[MAX_OBJECTS];
extern uint8_t object_ident[OBJECT_IDENT_SIZE];
extern int16 t_level[MAX_OBJ_LEVEL + 1];
extern inven_type t_list[MAX_TALLOC];
extern inven_type inventory[INVEN_ARRAY_SIZE];
extern char *special_names[SN_ARRAY_SIZE];
extern int16 sorted_objects[MAX_DUNGEON_OBJ];
extern int16 inven_ctr;             /* Total different obj's */
extern int16 inven_weight;          /* Cur carried weight */
extern int16 equip_ctr;             /* Cur equipment ctr */
extern int16 tcptr;                 /* Cur treasure heap ptr */

/* Following are creature arrays and variables */
extern creature_type c_list[MAX_CREATURES];
extern monster_type m_list[MAX_MALLOC];
extern int16 m_level[MAX_MONS_LEVEL + 1];
extern m_attack_type monster_attacks[N_MONS_ATTS];
extern recall_type c_recall[MAX_CREATURES];  /* Monster memories. -CJS- */
extern monster_type blank_monster;           /* Blank monster values */
extern int16 mfptr;                          /* Cur free monster ptr */
extern int16 mon_tot_mult;                   /* # of repro's of creature */

/* Following are arrays for descriptive pieces */
extern char *colors[MAX_COLORS];
extern char *mushrooms[MAX_MUSH];
extern char *woods[MAX_WOODS];
extern char *metals[MAX_METALS];
extern char *rocks[MAX_ROCKS];
extern char *amulets[MAX_AMULETS];
extern char *syllables[MAX_SYLLABLES];

extern uint8_t blows_table[7][6];

extern uint16_t normal_table[NORMAL_TABLE_SIZE];

/* Initialized data which had to be moved from some other file
 * Since these get modified, macrsrc.c must be able to access them
 * Otherwise, game cannot be made restartable
 * dungeon.c
*/
extern char last_command; /* Memory of previous command. */

/* moria1.c */
/* Track if temporary light about player. */
extern int light_flag;

/* function return values */

/* only extern functions declared here, static functions declared inside
   the file that defines them */

/* create.c */
void create_character();

/* creature.c */
void update_mon();
int multiply_monster();
void creatures();

/* death.c */
void display_scores();
int duplicate_character();
int32 total_points();
void exit_game();

/* desc.c */
int is_a_vowel();
void magic_init();
int16 object_offset();
void known1();
int known1_p();
void known2();
int known2_p();
void clear_known2();
void clear_empty();
void store_bought();
int store_bought_p();
void sample();
void identify();
void unmagic_name();
void objdes();
void invcopy();
void desc_charges();
void desc_remain();

/* dungeon.c */
void dungeon();

/* eat.c */
void eat();

/* files.c */
void init_scorefile();
void read_times();
void helpfile();
void print_objects();
int file_character();

/* generate.c */
void generate_cave();

/* help.c */
void ident_char();

/* io.c */
void init_curses();
void moriaterm();
void put_buffer();
void put_qio();
void restore_term();
void shell_out();
char inkey();
void flush();
void erase_line();
void clear_screen();
void clear_from();
void print(char, int, int);
void move_cursor_relative();
void count_msg_print();
void prt();
void move_cursor();
void msg_print();
int get_check();
int get_com();
int get_string();
void pause_line();
void pause_exit();
void save_screen();
void restore_screen();
void bell();
void screen_map();

/* magic.c */
void cast();

/* main.c */
int main();

/* misc1.c */
void init_seeds();
void set_seed();
void reset_seed();
int check_time();
int randint();
int randnor();
int bit_pos();
int in_bounds();
void panel_bounds();
int get_panel();
int panel_contains();
int distance();
int next_to_walls();
int next_to_corr();
int damroll();
int pdamroll();
int los();
uint8_t loc_symbol();
int test_light();
void prt_map();
int compact_monsters();
void add_food();
int popm();
int max_hp();
int place_monster();
void place_win_monster();
int get_mons_num();
void alloc_monster();
int summon_monster();
int summon_undead();
int popt();
void pusht();
int magik();
int m_bonus();

/* misc2.c */
void magic_treasure();
void set_options();

/* misc3.c */
void place_trap();
void place_rubble();
void place_gold();
int get_obj_num();
void place_object();
void alloc_object();
void random_object();
void cnv_stat();
void prt_stat();
void prt_field();
int stat_adj();
int chr_adj();
int con_adj();
char *title_string();
void prt_title();
void prt_level();
void prt_cmana();
void prt_mhp();
void prt_chp();
void prt_pac();
void prt_gold();
void prt_depth();
void prt_hunger();
void prt_blind();
void prt_confused();
void prt_afraid();
void prt_poisoned();
void prt_state();
void prt_speed();
void prt_study();
void prt_winner();
uint8_t modify_stat();
void set_use_stat();
int inc_stat();
int dec_stat();
int res_stat();
void bst_stat();
int tohit_adj();
int toac_adj();
int todis_adj();
int todam_adj();
void prt_stat_block();
void draw_cave();
void put_character();
void put_stats();
char *likert();
void put_misc1();
void put_misc2();
void put_misc3();
void display_char();
void get_name();
void change_name();
void inven_destroy();
void take_one_item();
void inven_drop();
int inven_damage();
int weight_limit();
int inven_check_num();
int inven_check_weight();
void check_strength();
int inven_carry();
int spell_chance();
void print_spells();
int get_spell();
void calc_spells();
void gain_spells();
void calc_mana();
void prt_experience();
void calc_hitpoints();
void insert_str();
void insert_lnum();
int enter_wiz_mode();
int attack_blows();
int tot_dam();
int critical_blow();
int mmove();
int player_saves();
int find_range();
void teleport();

/* misc4.c */
void scribe_object();
void add_inscribe();
void inscribe();
void check_view();

/* monsters.c */

/* moria1.c */
void change_speed();
void py_bonuses();
void calc_bonuses();
int show_inven();
char *describe_use();
int show_equip();
void takeoff();
int verify();
void inven_command();
int get_item();
int no_light();
int get_dir();
int get_alldir();
void move_rec();
void light_room();
void lite_spot();
void move_light();
void disturb();
void search_on();
void search_off();
void rest();
void rest_off();
int test_hit();
void take_hit();

/* moria2.c */
void change_trap();
void search();
void find_init();
void find_run();
void end_find();
void area_affect();
int minus_ac();
void corrode_gas();
void poison_gas();
void fire_dam();
void cold_dam();
void light_dam();
void acid_dam();

/* moria3.c */
int cast_spell();
void delete_monster();
void fix1_delete_monster();
void fix2_delete_monster();
int delete_object();
int32u monster_death();
int mon_take_hit();
void py_attack();
void move_char();
void chest_trap();
void openobject();
void closeobject();
int twall();

/* moria4.c */
void tunnel();
void disarm_trap();
void look();
void throw_object();
void bash();

/* potions.c */
void quaff();

/* prayer.c */
void pray();

/* recall.c */
int bool_roff_recall();
int roff_recall();

/* rnd.c */
int32u get_rnd_seed();
void set_rnd_seed();
int32 rnd();

/* save.c */
int save_char();
int _save_char();
int get_char();
void set_fileptr();
void wr_highscore();
void rd_highscore();

/* scrolls.c */
void read_scroll();

/* sets.c */
int set_room();
int set_corr();
int set_floor();
int set_corrodes();
int set_flammable();
int set_frost_destroy();
int set_acid_affect();
int set_lightning_destroy();
int set_null();
int set_acid_destroy();
int set_fire_destroy();
int set_large();
int general_store();
int armory();
int weaponsmith();
int temple();
int alchemist();
int magic_shop();

/* spells.c */
void monster_name();
void lower_monster_name();
int sleep_monsters1();
int detect_treasure();
int detect_object();
int detect_trap();
int detect_sdoor();
int detect_invisible();
int light_area();
int unlight_area();
void map_area();
int ident_spell();
int aggravate_monster();
int trap_creation();
int door_creation();
int td_destroy();
int detect_monsters();
void light_line();
void starlite();
int disarm_all();
void get_flags();
void fire_bolt();
void fire_ball();
void breath();
int recharge();
int hp_monster();
int drain_life();
int speed_monster();
int confuse_monster();
int sleep_monster();
int wall_to_mud();
int td_destroy2();
int poly_monster();
int build_wall();
int clone_monster();
void teleport_away();
void teleport_to();
int teleport_monster();
int mass_genocide();
int genocide();
int speed_monsters();
int sleep_monsters2();
int mass_poly();
int detect_evil();
int hp_player();
int cure_confusion();
int cure_blindness();
int cure_poison();
int remove_fear();
void earthquake();
int protect_evil();
void create_food();
int dispel_creature();
int turn_undead();
void warding_glyph();
void lose_str();
void lose_int();
void lose_wis();
void lose_dex();
void lose_con();
void lose_chr();
void lose_exp();
int slow_poison();
void bless();
void detect_inv2();
void destroy_area();
int enchant();
int remove_curse();
int restore_level();

/* staffs.c */
void use();

/* store1.c */
int32 item_value();
int32 sell_price();
int store_check_num();
void store_carry();
void store_destroy();
void store_init();
void store_maint();
int noneedtobargain();
void updatebargain();

/* store2.c */
void enter_store();

/* treasur1.c */

/* treasur2.c */

/* unix.c */
int check_input();
void user_name(char *);
int tilde();
FILE *tfopen();
int topen();

/* variable.c */

/* wands.c */
void aim();

/* wizard.c */
void wizard_light();
void change_character();
void wizard_create();


/* call functions which expand tilde before calling open/fopen */
#define open topen
#define fopen tfopen
