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
int coordCorridorWallsNextTo(int y, int x);
int diceDamageRoll(int dice, int sides);
int dicePlayerDamageRoll(uint8_t *notation_array);
bool los(int from_y, int from_x, int to_y, int to_x);
char caveGetTileSymbol(int y, int x);
bool caveTileVisible(int y, int x);
void drawDungeonPanel();
bool compactMonsters();
void playerIngestFood(int amount);
bool monsterPlaceNew(int y, int x, int creature_id, bool sleeping);
void monsterPlaceWinning();
void monsterPlaceNewWithinDistance(int number, int distance_from_source, bool sleeping);
bool monsterSummon(int *y, int *x, bool sleeping);
bool monsterSummonUndead(int *y, int *x);
int popt();
void pusht(uint8_t treasure_id);
bool magicShouldBeEnchanted(int chance);
int magicEnchantmentBonus(int base, int max_standard, int level);

// misc2.c
void magicTreasureMagicalAbility(int item_id, int level);
void setGameOptions();

// misc3.c
void dungeonSetTrap(int y, int x, int sub_type_id);
void dungeonPlaceRubble(int y, int x);
void dungeonPlaceGold(int y, int x);
int itemGetRandomObjectId(int level, bool must_be_small);
void dungeonPlaceRandomObjectAt(int y, int x, bool must_be_small);
void dungeonAllocateAndPlaceObject(bool (*set_function)(int), int object_type, int number);
void dungeonPlaceRandomObjectNear(int y, int x, int tries);
void statsAsString(uint8_t stat, char *stat_string);
void displayCharacterStats(int stat);
int playerStatAdjustmentWisdomIntelligence(int stat);
int playerStatAdjustmentCharisma();
int playerStatAdjustmentConstitution();
char *playerTitle();
void printCharacterTitle();
void printCharacterLevel();
void printCharacterCurrentMana();
void printCharacterMaxHitPoints();
void printCharacterCurrentHitPoints();
void printCharacterCurrentArmorClass();
void printCharacterGoldValue();
void printCharacterCurrentDepth();
void printCharacterHungerstatus();
void printCharacterBlindStatus();
void printCharacterConfusedState();
void printCharacterFearState();
void printCharacterPoisonedState();
void printCharacterMovementState();
void printCharacterSpeed();
void printCharacterStudyInstruction();
void printCharacterWinner();
void playerSetAndUseStat(int stat);
bool playerStatRandomIncrease(int stat);
bool playerStatRandomDecrease(int stat);
bool playerStatRestore(int stat);
void playerStatBoost(int stat, int amount);
int playerToHitAdjustment();
int playerArmorClassAdjustment();
int playerDisarmAdjustment();
int playerDamageAdjustment();
void printCharacterStatsBlock();
void drawCavePanel();
void printCharacterInformation();
void printCharacterStats();
const char *statRating(int y, int x);
void printCharacterVitalStatistics();
void printCharacterLevelExperience();
void printCharacterAbilities();
void printCharacter();
void getCharacterName();
void changeCharacterName();
void inventoryDestroyItem(int item_id);
void inventoryTakeOneItem(Inventory_t *to_item, Inventory_t *from_item);
void inventoryDropItem(int item_id, bool drop_all);
int inventoryDamageItem(bool (*item_type)(Inventory_t *), int chance_percentage);
int playerCarryingLoadLimit();
bool inventoryCanCarryItemCount(Inventory_t *item);
bool inventoryCanCarryItem(Inventory_t *item);
void playerStrength();
int inventoryCarryItem(Inventory_t *item);
void displaySpellsList(int *spell, int number_of_choices, int comment, int non_consecutive);
bool spellGetId(int *spell, int number_of_choices, int *spell_id, int *spell_chances, const char *prompt, int first_spell);
void playerCalculateAllowedSpellsCount(int stat);
void playerGainSpells();
void playerGainMana(int stat);
void displayCharacterExperience();
void playerCalculateHitPoints();
void insertStringIntoString(char *to_string, const char *from_string, const char *str_to_insert);
void intertNumberIntoString(char *to_string, const char *from_string, int32_t number, bool show_sign);
bool enterWizardMode();
int playerAttackBlows(int weight, int *weight_to_hit);
int itemMagicAbilityDamage(Inventory_t *item, int total_damage, int monster_id);
int playerWeaponCriticalBlow(int weapon_weight, int plus_to_hit, int damage, int attack_type_id);
bool playerMovePosition(int dir, int *new_y, int *new_x);
bool playerSavingThrow();
bool inventoryFindRange(int item_id_start, int item_id_end, int *j, int *k);
void playerTeleport(int new_distance);

// misc4.c
void itemInscribe();
void itemAppendToInscription(Inventory_t *item, uint8_t item_ident_type);
void itemReplaceInscription(Inventory_t *item, const char *inscription);
void dungeonResetView();

// moria1.c
void playerChangeSpeed(int speed);
void playerAdjustBonusesForItem(Inventory_t *item, int factor);
void playerRecalculateBonuses();
int displayInventory(int item_id_start, int item_id_end, bool weighted, int column, char *mask);
const char *playerItemWearingDescription(int body_location);
int displayEquipment(bool weighted, int column);
void playerTakeOff(int item_id, int pack_position_id);
void inventoryExecuteCommand(char command);
bool inventoryGetInputForItemId(int *command_key_id, const char *prompt, int item_id_start, int item_id_end, char *mask, const char *message);
bool playerNoLight();
bool getDirectionWithMemory(char *prompt, int *direction);
bool getAllDirections(const char *prompt, int *direction);
void dungeonMoveCreatureRecord(int y1, int x1, int y2, int x2);
void dungeonLightRoom(int pos_y, int pos_x);
void dungeonLiteSpot(int y, int x);
void dungeonMoveCharacterLight(int y1, int x1, int y2, int x2);
void playerDisturb(int major_disturbance, int light_disturbance);
void playerSearchOn();
void playerSearchOff();
void playerRestOn();
void playerRestOff();
bool playerTestBeingHit(int base_to_hit, int level, int plus_to_hit, int armor_class, int attack_type_id);
void playerTakesHit(int damage, const char *creature_name);

// moria2.c
void dungeonChangeTrapVisibility(int y, int x);
void dungeonSearch(int y, int x, int chance);
void playerFindInitialize(int direction);
void playerRunAndFind();
void playerEndRunning();
void playerAreaAffect(int direction, int y, int x);
void damageCorrodingGas(const char *creature_name);
void damagePoisonedGas(int damage, const char *creature_name);
void damageFire(int damage, const char *creature_name);
void damageCold(int damage, const char *creature_name);
void damageLightningBolt(int damage, const char *creature_name);
void damageAcid(int damage, const char *creature_name);

// moria3.c
int castSpellGetId(const char *prompt, int item_id, int *spell_id, int *spell_chance);
void dungeonDeleteMonster(int id);
void dungeonDeleteMonsterFix1(int id);
void dungeonDeleteMonsterFix2(int id);
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
