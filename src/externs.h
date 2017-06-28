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

extern int16_t current_dungeon_level;
extern int32_t character_max_score;
extern int32_t current_game_turn;

extern bool generate_new_level;
extern bool screen_has_changed;

extern bool player_free_turn;
extern int running_counter;
extern bool teleport_player;

extern bool player_carrying_light;
extern bool weapon_is_heavy;
extern int pack_heaviness;

extern bool total_winner;
extern bool character_generated;
extern bool character_saved;
extern bool character_is_dead;
extern int32_t character_birth_date;
extern vtype_t character_died_from;

extern char doing_inventory_command;
extern char last_command;
extern int command_count;
extern bool use_last_direction;

extern bool message_ready_to_print;
extern vtype_t messages[MAX_SAVE_MSG];
extern int16_t last_message_id;

extern int16_t missiles_counter;

extern uint32_t magic_seed;
extern uint32_t town_seed;

extern int eof_flag;
extern bool panic_save;
extern int16_t noscore;

extern bool to_be_wizard;
extern bool wizard_mode;

extern int16_t dungeon_height;
extern int16_t dungeon_width;

extern int16_t max_panel_rows, max_panel_cols;
extern int panel_row, panel_col;
extern int panel_row_min, panel_row_max;
extern int panel_col_min, panel_col_max;
extern int panel_col_prt, panel_row_prt;

// Following are all floor definitions
extern Cave_t cave[MAX_HEIGHT][MAX_WIDTH];

// Following are player variables
extern Player_t py;
extern int16_t char_row;
extern int16_t char_col;
extern uint32_t player_base_exp_levels[MAX_PLAYER_LEVEL];
extern uint16_t player_base_hp_levels[MAX_PLAYER_LEVEL];

extern ClassTitle_t class_titles[MAX_CLASS][MAX_PLAYER_LEVEL];
extern Race_t character_races[MAX_RACES];
extern Background_t character_backgrounds[MAX_BACKGROUND];

extern uint8_t race_gold_adjustments[MAX_RACES][MAX_RACES];

extern Class_t classes[MAX_CLASS];
extern int16_t class_level_adj[MAX_CLASS][MAX_LEV_ADJ];
extern uint16_t class_base_provisions[MAX_CLASS][5];

// Warriors don't have spells, so there is no entry for them.
extern Spell_t magic_spells[MAX_CLASS - 1][31];
extern char *spell_names[62];
extern uint32_t spells_learnt;           // Bit field for spells learnt -CJS-
extern uint32_t spells_worked;           // Bit field for spells tried -CJS-
extern uint32_t spells_forgotten;        // Bit field for spells forgotten -JEW-
extern uint8_t spells_learned_order[32]; // remember order that spells are learned in

// Following are store definitions
extern Owner_t store_owners[MAX_OWNERS];
extern Store_t stores[MAX_STORES];
extern uint16_t store_choices[MAX_STORES][STORE_CHOICES];
extern int (*store_buy[MAX_STORES])(int);

// Following are treasure arrays  and variables
extern GameObject_t game_objects[MAX_OBJECTS];
extern uint8_t objects_identified[OBJECT_IDENT_SIZE];
extern int16_t treasure_levels[MAX_OBJ_LEVEL + 1];
extern Inventory_t treasure_list[MAX_TALLOC];
extern Inventory_t inventory[INVEN_ARRAY_SIZE];
extern char *special_item_names[SN_ARRAY_SIZE];
extern int16_t sorted_objects[MAX_DUNGEON_OBJ];

extern int16_t inventory_count;
extern int16_t inventory_weight;
extern int16_t equipment_count;
extern int16_t current_treasure_id;

// Following are creature arrays and variables
extern Creature_t creatures_list[MAX_CREATURES];
extern Monster_t monsters[MAX_MALLOC];
extern int16_t monster_levels[MAX_MONS_LEVEL + 1];
extern MonsterAttack_t monster_attacks[N_MONS_ATTS];
extern Recall_t creature_recall[MAX_CREATURES]; // Monster memories. -CJS-
extern Monster_t blank_monster;
extern int16_t next_free_monster_id;
extern int16_t monster_multiply_total;

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

extern bool temporary_light_only;


// Function return values
//
// Only extern functions declared here, static functions
// declared inside the file that defines them.

// create.c
void create_character();

// creature.c
void update_mon(int monsterID);
bool multiply_monster(int y, int x, int creatureID, int monsterID);
void creatures(bool attack);

// death.c
void display_scores();
bool duplicate_character();
int32_t total_points();
void exit_game();

// desc.c
bool is_a_vowel(char ch);
void magic_init();
int16_t object_offset(Inventory_t *t_ptr);
void known1(Inventory_t *i_ptr);
bool known1_p(Inventory_t *);
void known2(Inventory_t *i_ptr);
bool known2_p(Inventory_t *i_ptr);
void clear_known2(Inventory_t *i_ptr);
void clear_empty(Inventory_t *i_ptr);
void store_bought(Inventory_t *i_ptr);
bool store_bought_p(Inventory_t *i_ptr);
void sample(Inventory_t *i_ptr);
void identify(int *item);
void unmagic_name(Inventory_t *i_ptr);
void objdes(obj_desc_t out_val, Inventory_t *i_ptr, bool pref);
void invcopy(Inventory_t *to, int from_index);
void desc_charges(int item_val);
void desc_remain(int item_val);

// dungeon.c
void dungeon();

// eat.c
void eat();

// files.c
void init_scorefile();
void read_times();
void helpfile(const char *filename);
void print_objects();
bool file_character(char *filename);

// generate.c
void generate_cave();

// help.c
void ident_char();

// io.c
void init_curses();
void put_buffer(const char *out_str, int row, int col);
void put_qio();
void restore_term();
void shell_out();
char inkey();
void flush();
void erase_line(int row, int col);
void clear_screen();
void clear_from(int row);
void print(char ch, int row, int col);
void move_cursor_relative(int row, int col);
void count_msg_print(const char *msg);
void prt(const char *str, int row, int col);
void move_cursor(int row, int col);
void msg_print(const char *msg);
bool get_check(const char *prompt);
int get_com(const char *prompt, char *command);
bool get_string(char *in_str, int row, int column, int slen);
void pause_line(int lineNumber);
void pause_exit(int lineNumber, int delay);
void save_screen();
void restore_screen();
void bell();
void screen_map();
bool check_input(int microsec);
void user_name(char *buf);

#ifndef _WIN32
// call functions which expand tilde before calling open/fopen
#define open topen
#define fopen tfopen

bool tilde(const char *file, char *expanded);
FILE *tfopen(const char *file, const char *mode);
int topen(const char *file, int flags, int mode);
#endif

// magic.c
void cast();

// misc1.c
void init_seeds(uint32_t seed);
void set_seed(uint32_t seed);
void reset_seed();
int randint(int maxval);
int randnor(int mean, int stand);
int bit_pos(uint32_t *test);
bool in_bounds(int y, int x);
int get_panel(int y, int x, bool force);
bool panel_contains(int y, int x);
int distance(int y1, int x1, int y2, int x2);
int next_to_walls(int y, int x);
int next_to_corr(int y, int x);
int damroll(int num, int sides);
int pdamroll(uint8_t *array);
bool los(int fromY, int fromX, int toY, int toX);
char loc_symbol(int y, int x);
bool test_light(int y, int x);
void prt_map();
bool compact_monsters();
void add_food(int num);
bool place_monster(int y, int x, int monsterID, bool slp);
void place_win_monster();
void alloc_monster(int num, int dis, bool slp);
bool summon_monster(int *y, int *x, bool slp);
bool summon_undead(int *y, int *x);
int popt();
void pusht(uint8_t treasureID);
bool magik(int chance);
int m_bonus(int base, int max_std, int level);

// misc2.c
void magic_treasure(int x, int level);
void set_options();

// misc3.c
void place_trap(int y, int x, int subval);
void place_rubble(int y, int x);
void place_gold(int y, int x);
int get_obj_num(int level, bool must_be_small);
void place_object(int y, int x, bool must_be_small);
void alloc_object(bool (*alloc_set)(int), int typ, int num);
void random_object(int y, int x, int num);
void cnv_stat(uint8_t stat, char *str);
void prt_stat(int stat);
int stat_adj(int stat);
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
void set_use_stat(int stat);
bool inc_stat(int stat);
bool dec_stat(int stat);
bool res_stat(int stat);
void bst_stat(int stat, int amount);
int tohit_adj();
int toac_adj();
int todis_adj();
int todam_adj();
void prt_stat_block();
void draw_cave();
void put_character();
void put_stats();
const char *likert(int x, int y);
void put_misc1();
void put_misc2();
void put_misc3();
void display_char();
void get_name();
void change_name();
void inven_destroy(int item_val);
void take_one_item(Inventory_t *s_ptr, Inventory_t *i_ptr);
void inven_drop(int item_val, int drop_all);
int inven_damage(bool (*typ)(Inventory_t *), int perc);
int weight_limit();
bool inven_check_num(Inventory_t *t_ptr);
bool inven_check_weight(Inventory_t *t_ptr);
void check_strength();
int inven_carry(Inventory_t *t_ptr);
void print_spells(int *spell, int num, int comment, int nonconsec);
int get_spell(int *spell, int num, int *sn, int *sc, const char *prompt, int first_spell);
void calc_spells(int stat);
void gain_spells();
void calc_mana(int stat);
void prt_experience();
void calc_hitpoints();
void insert_str(char *object_str, const char *mtc_str, const char *insert);
void insert_lnum(char *object_str, const char *mtc_str, int32_t number, int show_sign);
bool enter_wiz_mode();
int attack_blows(int weight, int *wtohit);
int tot_dam(Inventory_t *i_ptr, int tdam, int monster);
int critical_blow(int weight, int plus, int dam, int attack_type);
bool mmove(int dir, int *y, int *x);
bool player_saves();
int find_range(int item1, int item2, int *j, int *k);
void teleport(int dis);

// misc4.c
void scribe_object();
void add_inscribe(Inventory_t *i_ptr, uint8_t type);
void inscribe(Inventory_t *i_ptr, const char *str);
void check_view();

// moria1.c
void change_speed(int num);
void py_bonuses(Inventory_t *t_ptr, int factor);
void calc_bonuses();
int show_inven(int r1, int r2, bool weight, int col, char *mask);
const char *describe_use(int positionID);
int show_equip(bool weight, int col);
void takeoff(int item_val, int posn);
void inven_command(char command);
int get_item(int *com_val, const char *pmt, int i, int j, char *mask, const char *message);
bool no_light();
bool get_dir(char *prompt, int *dir);
bool get_alldir(const char *prompt, int *dir);
void move_rec(int y1, int x1, int y2, int x2);
void light_room(int posY, int posX);
void lite_spot(int y, int x);
void move_light(int y1, int x1, int y2, int x2);
void disturb(int s, int l);
void search_on();
void search_off();
void rest();
void rest_off();
bool test_hit(int bth, int level, int pth, int ac, int attack_type);
void take_hit(int damage, const char *hit_from);

// moria2.c
void change_trap(int y, int x);
void search(int y, int x, int chance);
void find_init(int dir);
void find_run();
void end_find();
void area_affect(int dir, int y, int x);
void corrode_gas(const char *kb_str);
void poison_gas(int dam, const char *kb_str);
void fire_dam(int dam, const char *kb_str);
void cold_dam(int dam, const char *kb_str);
void light_dam(int dam, const char *kb_str);
void acid_dam(int dam, const char *kb_str);

// moria3.c
int cast_spell(const char *prompt, int item_val, int *sn, int *sc);
void delete_monster(int id);
void fix1_delete_monster(int id);
void fix2_delete_monster(int id);
int delete_object(int y, int x);
uint32_t monster_death(int y, int x, uint32_t flags);
int mon_take_hit(int monsterID, int damage);
void move_char(int dir, bool do_pickup);
void chest_trap(int y, int x);
void openobject();
void closeobject();
int twall(int y, int x, int t1, int t2);
void objectBlockedByMonster(int id);
void playerAttackPosition(int y, int x);

// moria4.c
void tunnel(int dir);
void disarm_trap();
void look();
void throw_object();
void bash();
int getRandomDirection();

// potions.c
void quaff();

// prayer.c
void pray();

// recall.c
bool bool_roff_recall(int monsterID);
int roff_recall(int mon_num);

// rnd.c
uint32_t get_rnd_seed();
void set_rnd_seed(uint32_t seedval);
int32_t rnd();

// save.c
bool save_char();
bool get_char(bool *generate);
void set_fileptr(FILE *file);
void wr_highscore(HighScore_t *score);
void rd_highscore(HighScore_t *score);

// scrolls.c
void read_scroll();

// sets.c
bool set_room(int element);
bool set_corr(int element);
bool set_floor(int element);
bool set_corrodes(Inventory_t *item);
bool set_flammable(Inventory_t *item);
bool set_frost_destroy(Inventory_t *item);
bool set_acid_affect(Inventory_t *item);
bool set_lightning_destroy(Inventory_t *item);
bool set_null(Inventory_t *item);
bool set_acid_destroy(Inventory_t *item);
bool set_fire_destroy(Inventory_t *item);
bool set_large(GameObject_t *item);
bool general_store(int element);
bool armory(int element);
bool weaponsmith(int element);
bool temple(int element);
bool alchemist(int element);
bool magic_shop(int element);

// spells.c
bool sleep_monsters1(int row, int col);
bool detect_treasure();
bool detect_object();
bool detect_trap();
bool detect_sdoor();
bool detect_invisible();
bool light_area(int y, int x);
bool unlight_area(int y, int x);
void map_area();
bool ident_spell();
bool aggravate_monster(int dis_affect);
bool trap_creation();
bool door_creation();
bool td_destroy();
bool detect_monsters();
void light_line(int dir, int y, int x);
void starlite(int y, int x);
bool disarm_all(int dir, int y, int x);
void fire_bolt(int typ, int dir, int y, int x, int dam, char *bolt_typ);
void fire_ball(int typ, int dir, int y, int x, int dam_hp, const char *descrip);
void breath(int typ, int y, int x, int dam_hp, char *ddesc, int monptr);
bool recharge(int num);
bool hp_monster(int dir, int y, int x, int dam);
bool drain_life(int dir, int y, int x);
bool speed_monster(int dir, int y, int x, int spd);
bool confuse_monster(int dir, int y, int x);
bool sleep_monster(int dir, int y, int x);
bool wall_to_mud(int dir, int y, int x);
bool td_destroy2(int dir, int y, int x);
bool poly_monster(int dir, int y, int x);
bool build_wall(int dir, int y, int x);
bool clone_monster(int dir, int y, int x);
void teleport_away(int monptr, int dis);
void teleport_to(int ny, int nx);
bool teleport_monster(int dir, int y, int x);
bool mass_genocide();
bool genocide();
bool speed_monsters(int spd);
bool sleep_monsters2();
bool mass_poly();
bool detect_evil();
bool hp_player(int num);
bool cure_confusion();
bool cure_blindness();
bool cure_poison();
bool remove_fear();
void earthquake();
bool protect_evil();
void create_food();
bool dispel_creature(int cflag, int damage);
bool turn_undead();
void warding_glyph();
void lose_str();
void lose_int();
void lose_wis();
void lose_dex();
void lose_con();
void lose_chr();
void lose_exp(int32_t amount);
bool slow_poison();
void bless(int amount);
void detect_inv2(int amount);
void destroy_area(int y, int x);
bool enchant(int16_t *plusses, int16_t limit);
bool remove_curse();
bool restore_level();

// staffs.c
void use();

// store1.c
int32_t item_value(Inventory_t *i_ptr);
int32_t sell_price(int snum, int32_t *max_sell, int32_t *min_sell, Inventory_t *item);
bool store_check_num(Inventory_t *t_ptr, int store_num);
void store_carry(int store_num, int *ipos, Inventory_t *t_ptr);
void store_destroy(int store_num, int item_val, bool one_of);
void store_init();
void store_maint();
bool noneedtobargain(int store_num, int32_t minprice);
void updatebargain(int store_num, int32_t price, int32_t minprice);

// store2.c
void enter_store(int store_num);

// wands.c
void aim();

// wizard.c
void wizard_light();
void change_character();
void wizard_create();
