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
void createCharacter();

// creature.c
void monsterUpdateVisibility(int monster_id);
bool monsterMultiply(int y, int x, int creatureID, int monsterID);
void updateMonsters(bool attack);

// death.c
void showScoresScreen();
bool duplicate_character();
int32_t playerCalculateTotalPoints();
void exitGame();

// desc.c
bool isVowel(char ch);
void magicInitializeItemNames();
int16_t objectPositionOffset(Inventory_t *item);
void itemSetAsIdentified(Inventory_t *item);
bool itemSetColorlessAsIdentifed(Inventory_t *item);
void spellItemIdentifyAndRemoveRandomInscription(Inventory_t *item);
bool spellItemIdentified(Inventory_t *item);
void spellItemRemoveIdentification(Inventory_t *item);
void itemIdentificationClearEmpty(Inventory_t *item);
void itemIdentifyAsStoreBought(Inventory_t *item);
bool itemStoreBought(Inventory_t *item);
void itemSetAsTried(Inventory_t *item);
void itemIdentify(int *item_id);
void itemRemoveMagicNaming(Inventory_t *item);
void itemDescription(obj_desc_t description, Inventory_t *item, bool add_prefix);
void inventoryItemCopyTo(int from_item_id, Inventory_t *to_item);
void itemChargesRemainingDescription(int item_id);
void itemTypeRemainingCountDescription(int item_id);

// dungeon.c
void playDungeon();

// eat.c
void playerEat();

// files.c
void initializeScoreFile();
void displaySplashScreen();
void displayTextHelpFile(const char *filename);
void outputRandomLevelObjectsToFile();
bool outputPlayerCharacterToFile(char *filename);

// generate.c
void generateCave();

// help.c
void displayWorldObjectDescription();

// io.c
void terminalInitialize();
void putString(const char *out_str, int row, int col);
void putQIO();
void terminalRestore();
char getKeyInput();
void flushInputBuffer();
void eraseLine(int row, int col);
void clearScreen();
void clearToBottom(int row);
void putChar(char ch, int row, int col);
void moveCursorRelative(int row, int col);
void printMessageNoCommandInterrupt(const char *msg);
void putStringClearToEOL(const char *str, int row, int col);
void moveCursor(int y, int x);
void printMessage(const char *msg);
bool getInputConfirmation(const char *prompt);
int getCommand(const char *prompt, char *command);
bool getStringInput(char *in_str, int row, int col, int slen);
void waitForContinueKey(int line_number);
void waitAndConfirmCharacterCreation(int line_number, int delay);
void terminalSaveScreen();
void terminalRestoreScreen();
void terminalBellSound();
void displayDungeonMap();
bool checkForNonBlockingKeyPress(int microseconds);
void getDefaultPlayerName(char *buffer);

#ifndef _WIN32
// call functions which expand tilde before calling open/fopen
#define open topen
#define fopen tfopen

bool tilde(const char *file, char *expanded);
FILE *tfopen(const char *file, const char *mode);
int topen(const char *file, int flags, int mode);
#endif

// magic.c
void getAndCastMagicSpell();

// misc1.c
void seedsInitialize(uint32_t seed);
void seedSet(uint32_t seed);
void seedResetToOldSeed();
int randomNumber(int max);
int randomNumberNormalDistribution(int mean, int standard);
int getAndClearFirstBit(uint32_t *flag);
bool coordInBounds(int y, int x);
bool coordOutsidePanel(int y, int x, bool force);
bool coordInsidePanel(int y, int x);
int coordDistanceBetween(int y1, int x1, int y2, int x2);
int coordWallsNextTo(int y, int x);
int next_to_corr(int y, int x);
int damroll(int dice, int sides);
int pdamroll(uint8_t *notation_array);
bool los(int fromY, int fromX, int toY, int toX);
char loc_symbol(int y, int x);
bool test_light(int y, int x);
void prt_map();
bool compact_monsters();
void add_food(int amount);
bool place_monster(int y, int x, int monster_id, bool sleeping);
void place_win_monster();
void alloc_monster(int number, int dist, bool sleeping);
bool summon_monster(int *y, int *x, bool sleeping);
bool summon_undead(int *y, int *x);
int popt();
void pusht(uint8_t treasure_id);
bool magik(int chance);
int m_bonus(int base, int max_standard, int level);

// misc2.c
void magic_treasure(int item_id, int level);
void set_options();

// misc3.c
void place_trap(int y, int x, int sub_type_id);
void place_rubble(int y, int x);
void place_gold(int y, int x);
int get_obj_num(int level, bool must_be_small);
void place_object(int y, int x, bool must_be_small);
void alloc_object(bool (*set_function)(int), int object_type, int number);
void random_object(int y, int x, int tries);
void cnv_stat(uint8_t stat, char *stat_string);
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
const char *likert(int y, int x);
void put_misc1();
void put_misc2();
void put_misc3();
void display_char();
void get_name();
void change_name();
void inven_destroy(int item_id);
void take_one_item(Inventory_t *to_item, Inventory_t *from_item);
void inven_drop(int item_id, bool drop_all);
int inven_damage(bool (*item_type)(Inventory_t *), int chance_percentage);
int weight_limit();
bool inven_check_num(Inventory_t *item);
bool inven_check_weight(Inventory_t *item);
void check_strength();
int inven_carry(Inventory_t *item);
void print_spells(int *spell, int number_of_choices, int comment, int non_consecutive);
int get_spell(int *spell, int number_of_choices, int *spell_id, int *spell_chances, const char *prompt, int first_spell);
void calc_spells(int stat);
void gain_spells();
void calc_mana(int stat);
void prt_experience();
void calc_hitpoints();
void insert_str(char *to_string, const char *from_string, const char *str_to_insert);
void insert_lnum(char *to_string, const char *from_string, int32_t number, bool show_sign);
bool enter_wiz_mode();
int attack_blows(int weight, int *weight_to_hit);
int tot_dam(Inventory_t *item, int total_damage, int monster_id);
int critical_blow(int weapon_weight, int plus_to_hit, int damage, int attack_type_id);
bool mmove(int dir, int *new_y, int *new_x);
bool player_saves();
int find_range(int item_id_start, int item_id_end, int *j, int *k);
void teleport(int new_distance);

// misc4.c
void scribe_object();
void add_inscribe(Inventory_t *item, uint8_t item_ident_type);
void inscribe(Inventory_t *item, const char *inscription);
void check_view();

// moria1.c
void change_speed(int speed);
void py_bonuses(Inventory_t *item, int factor);
void calc_bonuses();
int show_inven(int item_id_start, int item_id_end, bool weighted, int column, char *mask);
const char *describe_use(int body_location);
int show_equip(bool weighted, int column);
void takeoff(int item_id, int pack_position_id);
void inven_command(char command);
int get_item(int *command_key_id, const char *prompt, int item_id_start, int item_id_end, char *mask, const char *message);
bool no_light();
bool get_dir(char *prompt, int *direction);
bool get_alldir(const char *prompt, int *direction);
void move_rec(int y1, int x1, int y2, int x2);
void light_room(int pos_y, int pos_x);
void lite_spot(int y, int x);
void move_light(int y1, int x1, int y2, int x2);
void disturb(int major_disturbance, int light_disturbance);
void search_on();
void search_off();
void rest();
void rest_off();
bool test_hit(int base_to_hit, int level, int plus_to_hit, int armor_class, int attack_type_id);
void take_hit(int damage, const char *creature_name);

// moria2.c
void change_trap(int y, int x);
void search(int y, int x, int chance);
void find_init(int direction);
void find_run();
void end_find();
void area_affect(int direction, int y, int x);
void corrode_gas(const char *creature_name);
void poison_gas(int dam, const char *creature_name);
void fire_dam(int dam, const char *creature_name);
void cold_dam(int dam, const char *creature_name);
void light_dam(int dam, const char *creature_name);
void acid_dam(int dam, const char *creature_name);

// moria3.c
int cast_spell(const char *prompt, int item_id, int *spell_id, int *spell_chance);
void delete_monster(int id);
void fix1_delete_monster(int id);
void fix2_delete_monster(int id);
int delete_object(int y, int x);
uint32_t monster_death(int y, int x, uint32_t flags);
int mon_take_hit(int monster_id, int damage);
void move_char(int direction, bool do_pickup);
void chest_trap(int y, int x);
void openobject();
void closeobject();
int twall(int y, int x, int digging_ability, int digging_chance);
void objectBlockedByMonster(int monster_id);
void playerAttackPosition(int y, int x);

// moria4.c
void tunnel(int direction);
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
bool bool_roff_recall(int monster_id);
int roff_recall(int monster_id);

// rnd.c
uint32_t get_rnd_seed();
void set_rnd_seed(uint32_t seed);
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
bool sleep_monsters1(int y, int x);
bool detect_treasure();
bool detect_object();
bool detect_trap();
bool detect_sdoor();
bool detect_invisible();
bool light_area(int y, int x);
bool unlight_area(int y, int x);
void map_area();
bool ident_spell();
bool aggravate_monster(int distance_affect);
bool trap_creation();
bool door_creation();
bool td_destroy();
bool detect_monsters();
void light_line(int x, int y, int direction);
void starlite(int y, int x);
bool disarm_all(int y, int x, int direction);
void fire_bolt(int y, int x, int direction, int damage_hp, int spell_type_id, char *spell_name);
void fire_ball(int y, int x, int direction, int damage_hp, int spell_type_id, const char *spell_name);
void breath(int x, int y, int monster_id, int damage_hp, char *spell_name, int spell_type_id);
bool recharge(int charges);
bool hp_monster(int y, int x, int direction, int damage_hp);
bool drain_life(int y, int x, int direction);
bool speed_monster(int y, int x, int direction, int speed);
bool confuse_monster(int y, int x, int direction);
bool sleep_monster(int y, int x, int direction);
bool wall_to_mud(int y, int x, int direction);
bool td_destroy2(int y, int x, int direction);
bool poly_monster(int y, int x, int direction);
bool build_wall(int y, int x, int direction);
bool clone_monster(int y, int x, int direction);
void teleport_away(int monster_id, int distance_from_player);
void teleport_to(int to_y, int to_x);
bool teleport_monster(int y, int x, int direction);
bool mass_genocide();
bool genocide();
bool speed_monsters(int speed);
bool sleep_monsters2();
bool mass_poly();
bool detect_evil();
bool hp_player(int adjustment);
bool cure_confusion();
bool cure_blindness();
bool cure_poison();
bool remove_fear();
void earthquake();
bool protect_evil();
void create_food();
bool dispel_creature(int creature_defense, int damage);
bool turn_undead();
void warding_glyph();
void lose_str();
void lose_int();
void lose_wis();
void lose_dex();
void lose_con();
void lose_chr();
void lose_exp(int32_t adjustment);
bool slow_poison();
void bless(int adjustment);
void detect_inv2(int adjustment);
void destroy_area(int y, int x);
bool enchant(int16_t *plusses, int16_t max_bonus_limit);
bool remove_curse();
bool restore_level();

// staffs.c
void use();

// store1.c
int32_t item_value(Inventory_t *item);
int32_t sell_price(int store_id, int32_t *min_price, int32_t *max_price, Inventory_t *item);
bool store_check_num(int store_id, Inventory_t *item);
void store_carry(int store_id, int *index_id, Inventory_t *item);
void store_destroy(int store_id, int item_id, bool only_one);
void store_init();
void store_maint();
bool noneedtobargain(int store_id, int32_t min_price);
void updatebargain(int store_id, int32_t price, int32_t min_price);

// store2.c
void enter_store(int store_id);

// wands.c
void aim();

// wizard.c
void wizard_light();
void change_character();
void wizard_create();
