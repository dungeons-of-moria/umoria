// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Declarations for global variables and initialized data

extern int hack_monptr;

extern char days[7][29];

extern vtype_t savegame_filename;
extern FILE *highscore_fp;

// These are options, set with set_options command `=` -CJS-
extern bool display_counts;
extern bool find_bound;
extern bool run_cut_corners;
extern bool run_examine_corners;
extern bool run_ignore_doors;
extern bool run_print_self;
extern bool highlight_seams;
extern bool prompt_to_pickup;
extern bool use_roguelike_keys;
extern bool show_inventory_weights;
extern bool error_beep_sound;

// Global flags
extern int16_t current_dungeon_level;
extern int32_t character_max_score;
extern int32_t current_game_turn;

extern bool generate_new_level;
extern bool screen_has_changed;

extern bool player_free_turn;
extern int find_flag;
extern bool teleport_flag;

extern bool player_light;
extern bool weapon_heavy;
extern int pack_heaviness;

extern int32_t birth_date;
extern vtype_t died_from;
extern bool death;

extern bool total_winner;
extern bool character_generated;
extern bool character_saved;

extern char doing_inven;
extern char last_command;
extern int command_count;
extern bool default_dir;

extern bool msg_flag;
extern vtype_t old_msgs[MAX_SAVE_MSG];
extern int16_t last_msg;

extern int16_t missile_ctr;

extern uint32_t randes_seed;
extern uint32_t town_seed;

extern int eof_flag;
extern bool panic_save;
extern int16_t noscore;

extern bool to_be_wizard;
extern bool wizard;

// Dungeon and display panel sizes
extern int16_t cur_height;
extern int16_t cur_width;
extern int16_t max_panel_rows, max_panel_cols;
extern int panel_row, panel_col;
extern int panel_row_min, panel_row_max;
extern int panel_col_min, panel_col_max;
extern int panel_col_prt, panel_row_prt;

// Following are all floor definitions
extern Cave_t cave[MAX_HEIGHT][MAX_WIDTH];

// Following are player variables
extern Player_t py;
extern char *player_titles[MAX_CLASS][MAX_PLAYER_LEVEL];
extern Race_t races[MAX_RACES];
extern Background_t backgrounds[MAX_BACKGROUND];
extern uint32_t player_exp[MAX_PLAYER_LEVEL];
extern uint16_t player_hp[MAX_PLAYER_LEVEL];
extern int16_t char_row;
extern int16_t char_col;

extern uint8_t rgold_adj[MAX_RACES][MAX_RACES];

extern Class_t classes[MAX_CLASS];
extern int16_t class_level_adj[MAX_CLASS][MAX_LEV_ADJ];

// Warriors don't have spells, so there is no entry for them.
extern Spell_t magic_spells[MAX_CLASS - 1][31];
extern char *spell_names[62];
extern uint32_t spell_learned;   // Bit field for spells learnt -CJS-
extern uint32_t spell_worked;    // Bit field for spells tried -CJS-
extern uint32_t spell_forgotten; // Bit field for spells forgotten -JEW-
extern uint8_t spell_order[32];  // remember order that spells are learned in
extern uint16_t player_init[MAX_CLASS][5];

// Following are store definitions
extern Owner_t owners[MAX_OWNERS];
extern Store_t stores[MAX_STORES];
extern uint16_t store_choices[MAX_STORES][STORE_CHOICES];
extern int (*store_buy[MAX_STORES])(int);

// Following are treasure arrays  and variables
extern Treasure_t object_list[MAX_OBJECTS];
extern uint8_t objects_identified[OBJECT_IDENT_SIZE];
extern int16_t treasure_levels[MAX_OBJ_LEVEL + 1];
extern Inventory_t treasure_list[MAX_TALLOC];
extern Inventory_t inventory[INVEN_ARRAY_SIZE];
extern char *special_names[SN_ARRAY_SIZE];
extern int16_t sorted_objects[MAX_DUNGEON_OBJ];
extern int16_t inven_ctr;    // Total different obj's
extern int16_t inven_weight; // Cur carried weight
extern int16_t equip_ctr;    // Cur equipment ctr
extern int16_t tcptr;        // Cur treasure heap ptr

// Following are creature arrays and variables
extern Creature_t creatures_list[MAX_CREATURES];
extern Monster_t monsters_list[MAX_MALLOC];
extern int16_t monster_levels[MAX_MONS_LEVEL + 1];
extern MonsterAttack_t monster_attacks[N_MONS_ATTS];
extern Recall_t creature_recall[MAX_CREATURES]; // Monster memories. -CJS-
extern Monster_t blank_monster; // Blank monster values
extern int16_t mfptr;           // Cur free monster ptr
extern int16_t mon_tot_mult;    // # of repro's of creature

// Following are arrays for descriptive pieces
extern char *colors[MAX_COLORS];
extern char *mushrooms[MAX_MUSH];
extern char *woods[MAX_WOODS];
extern char *metals[MAX_METALS];
extern char *rocks[MAX_ROCKS];
extern char *amulets[MAX_AMULETS];
extern char *syllables[MAX_SYLLABLES];

extern uint8_t blows_table[7][6];

extern uint16_t normal_table[NORMAL_TABLE_SIZE];

extern bool light_flag;

// function return values

// only extern functions declared here, static functions
// declared inside the file that defines them.

// create.c
void create_character();

// creature.c
void update_mon(int);
bool multiply_monster(int, int, int, int);
void creatures(bool);

// death.c
void display_scores();
bool duplicate_character();
int32_t total_points();
void exit_game();

// desc.c
bool is_a_vowel(char);
void magic_init();
int16_t object_offset(Inventory_t *);
void known1(Inventory_t *);
bool known1_p(Inventory_t *);
void known2(Inventory_t *);
bool known2_p(Inventory_t *);
void clear_known2(Inventory_t *);
void clear_empty(Inventory_t *);
void store_bought(Inventory_t *);
bool store_bought_p(Inventory_t *);
void sample(Inventory_t *);
void identify(int *);
void unmagic_name(Inventory_t *);
void objdes(obj_desc_t out_val, Inventory_t *i_ptr, bool pref);
void invcopy(Inventory_t *, int);
void desc_charges(int);
void desc_remain(int);

// dungeon.c
void dungeon();

// eat.c
void eat();

// files.c
void init_scorefile();
void read_times();
void helpfile(const char *);
void print_objects();
bool file_character(char *);

// generate.c
void generate_cave();

// help.c
void ident_char();

// io.c
void init_curses();
void moriaterm();
void put_buffer(const char *, int, int);
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
void count_msg_print(const char *);
void prt(const char *, int, int);
void move_cursor(int, int);
void msg_print(const char *);
bool get_check(const char *);
int get_com(const char *, char *);
bool get_string(char *, int, int, int);
void pause_line(int);
void pause_exit(int, int);
void save_screen();
void restore_screen();
void bell();
void screen_map();
void sleep_in_seconds(int);
bool check_input(int);
void user_name(char *);

#ifndef _WIN32
// call functions which expand tilde before calling open/fopen
#define open topen
#define fopen tfopen

bool tilde(const char *, char *);
FILE *tfopen(const char *, const char *);
int topen(const char *, int, int);
#endif

// magic.c
void cast();

// main.c
void check_file_permissions();

// misc1.c
void init_seeds(uint32_t);
void set_seed(uint32_t);
void reset_seed();
int randint(int);
int randnor(int, int);
int bit_pos(uint32_t *);
bool in_bounds(int, int);
void panel_bounds();
int get_panel(int, int, bool);
bool panel_contains(int, int);
int distance(int, int, int, int);
int next_to_walls(int, int);
int next_to_corr(int, int);
int damroll(int, int);
int pdamroll(uint8_t *);
bool los(int, int, int, int);
char loc_symbol(int, int);
bool test_light(int, int);
void prt_map();
bool compact_monsters();
void add_food(int);
int popm();
int max_hp(uint8_t *);
bool place_monster(int, int, int, bool);
void place_win_monster();
int get_mons_num(int);
void alloc_monster(int, int, bool);
bool summon_monster(int *y, int *x, bool slp);
bool summon_undead(int *, int *);
int popt();
void pusht(uint8_t);
bool magik(int);
int m_bonus(int, int, int);

// misc2.c
void magic_treasure(int, int);
void set_options();

// misc3.c
void place_trap(int, int, int);
void place_rubble(int, int);
void place_gold(int, int);
int get_obj_num(int, bool);
void place_object(int, int, bool);
void alloc_object(bool (*)(int), int, int);
void random_object(int, int, int);
void cnv_stat(uint8_t, char *);
void prt_stat(int);
void prt_field(const char *, int, int);
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
const char *likert(int, int);
void put_misc1();
void put_misc2();
void put_misc3();
void display_char();
void get_name();
void change_name();
void inven_destroy(int);
void take_one_item(Inventory_t *, Inventory_t *);
void inven_drop(int, int);
int inven_damage(bool (*)(Inventory_t *), int);
int weight_limit();
bool inven_check_num(Inventory_t *);
bool inven_check_weight(Inventory_t *);
void check_strength();
int inven_carry(Inventory_t *);
int spell_chance(int);
void print_spells(int *, int, int, int);
int get_spell(int *, int, int *, int *, const char *, int);
void calc_spells(int);
void gain_spells();
void calc_mana(int);
void prt_experience();
void calc_hitpoints();
void insert_str(char *, const char *, const char *);
void insert_lnum(char *, const char *, int32_t, int);
bool enter_wiz_mode();
int attack_blows(int, int *);
int tot_dam(Inventory_t *, int, int);
int critical_blow(int, int, int, int);
bool mmove(int, int *, int *);
bool player_saves();
int find_range(int, int, int *, int *);
void teleport(int);

// misc4.c
void scribe_object();
void add_inscribe(Inventory_t *, uint8_t);
void inscribe(Inventory_t *, const char *);
void check_view();

// moria1.c
void change_speed(int);
void py_bonuses(Inventory_t *, int);
void calc_bonuses();
int show_inven(int, int, bool, int, char *);
const char *describe_use(int);
int show_equip(bool, int);
void takeoff(int, int);
int verify(const char *, int);
void inven_command(char);
int get_item(int *, const char *, int, int, char *, const char *);
bool no_light();
bool get_dir(char *, int *);
bool get_alldir(const char *, int *);
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
void take_hit(int, const char *);

// moria2.c
void change_trap(int, int);
void search(int, int, int);
void find_init(int);
void find_run();
void end_find();
void area_affect(int, int, int);
int minus_ac(uint32_t);
void corrode_gas(const char *);
void poison_gas(int, const char *);
void fire_dam(int, const char *);
void cold_dam(int, const char *);
void light_dam(int, const char *);
void acid_dam(int, const char *);

// moria3.c
int cast_spell(const char *, int, int *, int *);
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
void objectBlockedByMonster(int id);
void playerAttackPosition(int y, int x);

// moria4.c
void tunnel(int);
void disarm_trap();
void look();
void throw_object();
void bash();
int getRandomDirection(void);

// potions.c
void quaff();

// prayer.c
void pray();

// recall.c
bool bool_roff_recall(int);
int roff_recall(int);

// rnd.c
uint32_t get_rnd_seed();
void set_rnd_seed(uint32_t);
int32_t rnd();

// save.c
bool save_char();
bool _save_char(char *);
bool get_char(bool *);
void set_fileptr(FILE *);
void wr_highscore(HighScore_t *);
void rd_highscore(HighScore_t *);

// scrolls.c
void read_scroll();

// sets.c
bool set_room(int);
bool set_corr(int);
bool set_floor(int);
bool set_corrodes(Inventory_t *);
bool set_flammable(Inventory_t *);
bool set_frost_destroy(Inventory_t *);
bool set_acid_affect(Inventory_t *);
bool set_lightning_destroy(Inventory_t *);
bool set_null(Inventory_t *);
bool set_acid_destroy(Inventory_t *);
bool set_fire_destroy(Inventory_t *);
bool set_large(Treasure_t *);
bool general_store(int);
bool armory(int);
bool weaponsmith(int);
bool temple(int);
bool alchemist(int);
bool magic_shop(int);

// spells.c
bool sleep_monsters1(int, int);
bool detect_treasure();
bool detect_object();
bool detect_trap();
bool detect_sdoor();
bool detect_invisible();
bool light_area(int, int);
bool unlight_area(int, int);
void map_area();
bool ident_spell();
bool aggravate_monster(int);
bool trap_creation();
bool door_creation();
bool td_destroy();
bool detect_monsters();
void light_line(int, int, int);
void starlite(int, int);
bool disarm_all(int, int, int);
void fire_bolt(int, int, int, int, int, char *);
void fire_ball(int, int, int, int, int, const char *);
void breath(int, int, int, int, char *, int);
bool recharge(int);
bool hp_monster(int, int, int, int);
bool drain_life(int, int, int);
bool speed_monster(int, int, int, int);
bool confuse_monster(int, int, int);
bool sleep_monster(int, int, int);
bool wall_to_mud(int, int, int);
bool td_destroy2(int, int, int);
bool poly_monster(int, int, int);
bool build_wall(int, int, int);
bool clone_monster(int, int, int);
void teleport_away(int, int);
void teleport_to(int, int);
bool teleport_monster(int, int, int);
bool mass_genocide();
bool genocide();
bool speed_monsters(int);
bool sleep_monsters2();
bool mass_poly();
bool detect_evil();
bool hp_player(int);
bool cure_confusion();
bool cure_blindness();
bool cure_poison();
bool remove_fear();
void earthquake();
bool protect_evil();
void create_food();
bool dispel_creature(int, int);
bool turn_undead();
void warding_glyph();
void lose_str();
void lose_int();
void lose_wis();
void lose_dex();
void lose_con();
void lose_chr();
void lose_exp(int32_t);
bool slow_poison();
void bless(int);
void detect_inv2(int);
void destroy_area(int, int);
bool enchant(int16_t *, int16_t);
bool remove_curse();
bool restore_level();

// staffs.c
void use();

// store1.c
int32_t item_value(Inventory_t *);
int32_t sell_price(int, int32_t *, int32_t *, Inventory_t *);
bool store_check_num(Inventory_t *, int);
void store_carry(int, int *, Inventory_t *);
void store_destroy(int, int, bool);
void store_init();
void store_maint();
bool noneedtobargain(int, int32_t);
void updatebargain(int, int32_t, int32_t);

// store2.c
void enter_store(int);

// wands.c
void aim();

// wizard.c
void wizard_light();
void change_character();
void wizard_create();
