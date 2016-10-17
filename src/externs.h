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

extern char *copyright[5];

/* horrible hack: needed because compact_monster() can be called from deep
   within creatures() via place_monster() and summon_monster() */
extern int hack_monptr;

extern vtype died_from;
extern vtype savefile;              /* The save file. -CJS- */
extern int32_t birth_date;

/* These are options, set with set_options command -CJS- */
extern bool rogue_like_commands;
extern bool find_cut;               /* Cut corners on a run */
extern bool find_examine;           /* Check corners on a run */
extern bool find_prself;            /* Print yourself on a run (slower) */
extern bool find_bound;             /* Stop run when the map shifts */
extern bool prompt_carry_flag;      /* Prompt to pick something up */
extern bool show_weight_flag;       /* Display weights in inventory */
extern bool highlight_seams;        /* Highlight magma and quartz */
extern bool find_ignore_doors;      /* Run through open doors */
extern bool sound_beep_flag;        /* Beep for invalid character */
extern bool display_counts;         /* Display rest/repeat counts */

/* global flags */
extern bool new_level_flag;         /* Next level when true */
extern bool teleport_flag;          /* Handle teleport traps */
extern int eof_flag;                /* Used to handle eof/HANGUP */
extern bool player_light;           /* Player carrying light */
extern int find_flag;               /* Used in MORIA */
extern bool free_turn_flag;         /* Used in MORIA */
extern bool weapon_heavy;           /* Flag if the weapon too heavy -CJS- */
extern int pack_heavy;              /* Flag if the pack too heavy -CJS- */
extern char doing_inven;            /* Track inventory commands */
extern bool screen_change;          /* Screen changes (used in inven_commands) */

extern bool character_generated;    /* don't save score until char gen finished*/
extern bool character_saved;        /* prevents save on kill after save_char() */
extern FILE *highscore_fp;          /* High score file pointer */
extern int command_count;           /* Repetition of commands. -CJS- */
extern bool default_dir;            /* Use last direction in repeated commands */
extern int16_t noscore;             /* Don't score this game. -CJS- */
extern uint32_t randes_seed;        /* For encoding colors */
extern uint32_t town_seed;          /* Seed for town genera*/
extern int16_t dun_level;           /* Cur dungeon level */
extern int16_t missile_ctr;         /* Counter for missiles */
extern bool msg_flag;               /* Set with first msg */
extern vtype old_msg[MAX_SAVE_MSG]; /* Last messages -CJS- */
extern int16_t last_msg;            /* Where in the array is the last */
extern bool death;                  /* True if died */
extern int32_t turn;                /* Cur trun of game */
extern bool wizard;                 /* Wizard flag */
extern bool to_be_wizard;
extern bool panic_save;             /* this is true if playing from a panic save */

extern bool wait_for_more;

extern char days[7][29];
extern int closing_flag;            /* Used for closing */

extern int16_t cur_height, cur_width; /* Cur dungeon size */

/*  Following are calculated from max dungeon sizes */
extern int16_t max_panel_rows, max_panel_cols;
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
extern uint32_t player_exp[MAX_PLAYER_LEVEL];
extern uint16_t player_hp[MAX_PLAYER_LEVEL];
extern int16_t char_row;
extern int16_t char_col;

extern uint8_t rgold_adj[MAX_RACES][MAX_RACES];

extern class_type class[MAX_CLASS];
extern int16_t class_level_adj[MAX_CLASS][MAX_LEV_ADJ];

/* Warriors don't have spells, so there is no entry for them. */
extern spell_type magic_spell[MAX_CLASS - 1][31];
extern char *spell_names[62];
extern uint32_t spell_learned;      /* Bit field for spells learnt -CJS- */
extern uint32_t spell_worked;       /* Bit field for spells tried -CJS- */
extern uint32_t spell_forgotten;    /* Bit field for spells forgotten -JEW- */
extern uint8_t spell_order[32];     /* remember order that spells are learned in */
extern uint16_t player_init[MAX_CLASS][5];
extern bool total_winner;
extern int32_t max_score;

/* Following are store definitions */
extern owner_type owners[MAX_OWNERS];
extern store_type store[MAX_STORES];
extern uint16_t store_choice[MAX_STORES][STORE_CHOICES];
extern int (*store_buy[MAX_STORES])();

// FIXME: why is this extern? It's only used in store2.c.
/* Save the store's last increment value. */
extern int16_t last_store_inc;

/* Following are treasure arrays  and variables */
extern treasure_type object_list[MAX_OBJECTS];
extern uint8_t object_ident[OBJECT_IDENT_SIZE];
extern int16_t t_level[MAX_OBJ_LEVEL + 1];
extern inven_type t_list[MAX_TALLOC];
extern inven_type inventory[INVEN_ARRAY_SIZE];
extern char *special_names[SN_ARRAY_SIZE];
extern int16_t sorted_objects[MAX_DUNGEON_OBJ];
extern int16_t inven_ctr;           /* Total different obj's */
extern int16_t inven_weight;        /* Cur carried weight */
extern int16_t equip_ctr;           /* Cur equipment ctr */
extern int16_t tcptr;               /* Cur treasure heap ptr */

/* Following are creature arrays and variables */
extern creature_type c_list[MAX_CREATURES];
extern monster_type m_list[MAX_MALLOC];
extern int16_t m_level[MAX_MONS_LEVEL + 1];
extern m_attack_type monster_attacks[N_MONS_ATTS];
extern recall_type c_recall[MAX_CREATURES];  /* Monster memories. -CJS- */
extern monster_type blank_monster;           /* Blank monster values */
extern int16_t mfptr;                        /* Cur free monster ptr */
extern int16_t mon_tot_mult;                 /* # of repro's of creature */

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
extern bool light_flag;

/* function return values */

/* only extern functions declared here, static functions declared inside
   the file that defines them */

/* create.c */
void create_character();

/* creature.c */
void update_mon(int);
bool multiply_monster(int, int, int, int);
void creatures(int);

/* death.c */
void display_scores(int);
bool duplicate_character();
int32_t total_points();
void exit_game();

/* desc.c */
bool is_a_vowel(char);
void magic_init();
int16_t object_offset(inven_type *);
void known1(inven_type *);
int known1_p(inven_type *);
void known2(inven_type *);
int known2_p(inven_type *);
void clear_known2(inven_type *);
void clear_empty(inven_type *);
void store_bought(inven_type *);
int store_bought_p(inven_type *);
void sample(inven_type *);
void identify(int *);
void unmagic_name(inven_type *);
void objdes(char *, inven_type *, int);
void invcopy(inven_type *, int);
void desc_charges(int);
void desc_remain(int);

/* dungeon.c */
void dungeon();

/* eat.c */
void eat();

/* files.c */
void init_scorefile();
void read_times();
void helpfile(char *);
void print_objects();
bool file_character(char *);

/* generate.c */
void generate_cave();

/* help.c */
void ident_char();

/* io.c */
void init_curses();
void moriaterm();
void put_buffer(char *, int, int);
void put_qio();
void restore_term();
void shell_out();
char inkey();
void flush();
void erase_line(int, int);
void clear_screen();
void clear_from(int);
void print(char, int, int);
void move_cursor_relative(int, int);
void count_msg_print(char *);
void prt(char *, int, int);
void move_cursor(int, int);
void msg_print(char *);
bool get_check(char *);
int get_com(char *, char *);
bool get_string(char *, int, int, int);
void pause_line(int);
void pause_exit(int, int);
void save_screen();
void restore_screen();
void bell();
void screen_map();

/* magic.c */
void cast();

/* main.c */
int main(int, char **);

/* misc1.c */
void init_seeds(uint32_t);
void set_seed(uint32_t);
void reset_seed();
bool check_time();
int randint(int);
int randnor(int, int);
int bit_pos(uint32_t *);
bool in_bounds(int, int);
void panel_bounds();
int get_panel(int, int, int);
bool panel_contains(int, int);
int distance(int, int, int, int);
int next_to_walls(int, int);
int next_to_corr(int, int);
int damroll(int, int);
int pdamroll(uint8_t *);
bool los(int, int, int, int);
uint8_t loc_symbol(int, int);
bool test_light(int, int);
void prt_map();
bool compact_monsters();
void add_food(int);
int popm();
int max_hp(uint8_t *);
bool place_monster(int, int, int, int);
void place_win_monster();
int get_mons_num(int);
void alloc_monster(int, int, int);
bool summon_monster(int *, int*, int);
bool summon_undead(int *, int *);
int popt();
void pusht(uint8_t);
bool magik(int);
int m_bonus(int, int, int);

/* misc2.c */
void magic_treasure(int, int);
void set_options();

/* misc3.c */
void place_trap(int, int, int);
void place_rubble(int, int);
void place_gold(int, int);
int get_obj_num(int, bool);
void place_object(int, int, bool);
void alloc_object(bool (*)(), int, int);
void random_object(int, int, int);
void cnv_stat(uint8_t, char *);
void prt_stat(int);
void prt_field(char *, int, int);
int stat_adj(int);
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
uint8_t modify_stat(int, int16_t);
void set_use_stat(int);
bool inc_stat(int);
bool dec_stat(int);
bool res_stat(int);
void bst_stat(int, int);
int tohit_adj();
int toac_adj();
int todis_adj();
int todam_adj();
void prt_stat_block();
void draw_cave();
void put_character();
void put_stats();
char *likert(int, int);
void put_misc1();
void put_misc2();
void put_misc3();
void display_char();
void get_name();
void change_name();
void inven_destroy(int);
void take_one_item(inven_type *, inven_type *);
void inven_drop(int, int);
int inven_damage(bool (*)(), int);
int weight_limit();
bool inven_check_num(inven_type *);
bool inven_check_weight(inven_type *);
void check_strength();
int inven_carry(inven_type *);
int spell_chance(int);
void print_spells(int *, int, int, int);
int get_spell(int *, int, int *, int *, char *, int);
void calc_spells(int);
void gain_spells();
void calc_mana(int);
void prt_experience();
void calc_hitpoints();
void insert_str(char *, char *, char *);
void insert_lnum(char *, char *, int32_t, int);
bool enter_wiz_mode();
int attack_blows(int, int *);
int tot_dam(inven_type *, int, int);
int critical_blow(int, int, int, int);
int mmove(int, int *, int *);
bool player_saves();
int find_range(int, int, int *, int *);
void teleport(int);

/* misc4.c */
void scribe_object();
void add_inscribe(inven_type *, uint8_t);
void inscribe(inven_type *, char *);
void check_view();

/* monsters.c */

/* moria1.c */
void change_speed(int);
void py_bonuses(inven_type *, int);
void calc_bonuses();
int show_inven(int, int, bool, int, char *);
char *describe_use(int);
int show_equip(bool, int);
void takeoff(int, int);
int verify(char *, int);
void inven_command(char);
int get_item(int *, char *, int, int, char *, char *);
bool no_light();
bool get_dir(char *, int *);
bool get_alldir(char *, int *);
void move_rec(int, int, int, int);
void light_room(int, int);
void lite_spot(int, int);
void move_light(int, int, int, int);
void disturb(int, int);
void search_on();
void search_off();
void rest();
void rest_off();
bool test_hit(int, int, int, int, int);
void take_hit(int, char *);

/* moria2.c */
void change_trap(int, int);
void search(int, int, int);
void find_init(int);
void find_run();
void end_find();
void area_affect(int, int, int);
int minus_ac(uint32_t);
void corrode_gas(char *);
void poison_gas(int, char *);
void fire_dam(int, char *);
void cold_dam(int, char *);
void light_dam(int, char *);
void acid_dam(int, char *);

/* moria3.c */
int cast_spell(char *, int, int *, int *);
void delete_monster(int);
void fix1_delete_monster(int);
void fix2_delete_monster(int);
int delete_object(int, int);
uint32_t monster_death(int, int, uint32_t);
int mon_take_hit(int, int);
void py_attack(int, int);
void move_char(int, bool);
void chest_trap(int, int);
void openobject();
void closeobject();
int twall(int, int, int, int);

/* moria4.c */
void tunnel(int);
void disarm_trap();
void look();
void throw_object();
void bash();

/* potions.c */
void quaff();

/* prayer.c */
void pray();

/* recall.c */
bool bool_roff_recall(int);
int roff_recall(int);

/* rnd.c */
uint32_t get_rnd_seed();
void set_rnd_seed(uint32_t);
int32_t rnd();

/* save.c */
bool save_char();
bool _save_char(char *);
bool get_char(bool *);
void set_fileptr(FILE *);
void wr_highscore(high_scores *);
void rd_highscore(high_scores *);

/* scrolls.c */
void read_scroll();

/* sets.c */
bool set_room(int);
bool set_corr(int);
bool set_floor(int);
bool set_corrodes(inven_type *);
bool set_flammable(inven_type *);
bool set_frost_destroy(inven_type *);
bool set_acid_affect(inven_type *);
bool set_lightning_destroy(inven_type *);
bool set_null(inven_type *);
bool set_acid_destroy(inven_type *);
bool set_fire_destroy(inven_type *);
bool set_large(treasure_type *);
bool general_store(int);
bool armory(int);
bool weaponsmith(int);
bool temple(int);
bool alchemist(int);
bool magic_shop(int);

/* spells.c */
void monster_name(char *, monster_type *, creature_type *);
void lower_monster_name(char *, monster_type *, creature_type *);
int sleep_monsters1(int, int);
int detect_treasure();
int detect_object();
int detect_trap();
int detect_sdoor();
int detect_invisible();
int light_area(int, int);
int unlight_area(int, int);
void map_area();
int ident_spell();
int aggravate_monster(int);
int trap_creation();
int door_creation();
int td_destroy();
int detect_monsters();
void light_line(int, int, int);
void starlite(int, int);
int disarm_all(int, int, int);
void get_flags(int, uint32_t *, int *, bool (**)());
void fire_bolt(int, int, int, int, int, char *);
void fire_ball(int, int, int, int, int, char *);
void breath(int, int, int, int, char *, int);
int recharge(int);
int hp_monster(int, int, int, int);
int drain_life(int, int, int);
int speed_monster(int, int, int, int);
int confuse_monster(int, int, int);
int sleep_monster(int, int, int);
int wall_to_mud(int, int, int);
int td_destroy2(int, int, int);
int poly_monster(int, int, int);
int build_wall(int, int, int);
bool clone_monster(int, int, int);
void teleport_away(int, int);
void teleport_to(int, int);
int teleport_monster(int, int, int);
int mass_genocide();
int genocide();
int speed_monsters(int);
int sleep_monsters2();
int mass_poly();
int detect_evil();
int hp_player(int);
int cure_confusion();
int cure_blindness();
int cure_poison();
int remove_fear();
void earthquake();
int protect_evil();
void create_food();
int dispel_creature(int, int);
int turn_undead();
void warding_glyph();
void lose_str();
void lose_int();
void lose_wis();
void lose_dex();
void lose_con();
void lose_chr();
void lose_exp(int32_t);
int slow_poison();
void bless(int);
void detect_inv2(int);
void destroy_area(int, int);
bool enchant(int16_t *, int16_t);
int remove_curse();
int restore_level();

/* staffs.c */
void use();

/* store1.c */
int32_t item_value(inven_type *);
int32_t sell_price(int, int32_t *, int32_t *, inven_type *);
bool store_check_num(inven_type *, int);
void store_carry(int, int *, inven_type *);
void store_destroy(int, int, int);
void store_init();
void store_maint();
bool noneedtobargain(int, int32_t);
void updatebargain(int, int32_t, int32_t);

/* store2.c */
void enter_store(int);

/* tables.c */

/* treasur.c */

/* unix.c */
int check_input(int);
void user_name(char *);
int tilde(char *, char *);
FILE *tfopen(char *, char *);
int topen(char *, int, int);

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
