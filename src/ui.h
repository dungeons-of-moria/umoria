// Copyright (c) 1989-94 James E. Wilson, Robert A. Koeneke
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

#pragma once

// Panel_t holds data about a screen panel (the dungeon display)
// Screen panels calculated from the dungeon/screen dimensions
typedef struct {
    int row;
    int col;

    int top;
    int bottom;
    int left;
    int right;

    int col_prt;
    int row_prt;

    int16_t max_rows;
    int16_t max_cols;
} Panel_t;

typedef struct {
    int32_t y;
    int32_t x;
} Coord_t;

// UI - IO
bool terminalInitialize();
void terminalRestore();
void terminalSaveScreen();
void terminalRestoreScreen();
void terminalBellSound();
void putQIO();
void flushInputBuffer();
void clearScreen();
void clearToBottom(int row);
void moveCursor(Coord_t coords);
void addChar(char ch, Coord_t coords);
void putString(const char *out_str, Coord_t coords);
void putStringClearToEOL(const std::string &str, Coord_t coords);
void eraseLine(Coord_t coords);
void panelMoveCursor(Coord_t coords);
void panelPutTile(char ch, Coord_t coords);
void messageLinePrintMessage(std::string message);
void messageLineClear();
void printMessage(const char *msg);
void printMessageNoCommandInterrupt(const std::string &msg);
char getKeyInput();
bool getCommand(const std::string &prompt, char &command);
bool getStringInput(char *in_str, Coord_t coords, int slen);
bool getInputConfirmation(const std::string &prompt);
void waitForContinueKey(int line_number);
bool checkForNonBlockingKeyPress(int microseconds);
void getDefaultPlayerName(char *buffer);
bool checkFilePermissions();

#ifndef _WIN32
// call functions which expand tilde before calling open/fopen
#define open topen
#define fopen tfopen

FILE *tfopen(const char *file, const char *mode);
int topen(const char *file, int flags, int mode);
bool tilde(const char *file, char *expanded);
#endif


// UI
bool coordOutsidePanel(Coord_t coord, bool force);
bool coordInsidePanel(Coord_t coord);
void drawDungeonPanel();
void drawCavePanel();
void dungeonResetView();

void statsAsString(uint8_t stat, char *stat_string);
void displayCharacterStats(int stat);
void printCharacterTitle();
void printCharacterLevel();
void printCharacterCurrentMana();
void printCharacterMaxHitPoints();
void printCharacterCurrentHitPoints();
void printCharacterCurrentArmorClass();
void printCharacterGoldValue();
void printCharacterCurrentDepth();
void printCharacterHungerStatus();
void printCharacterBlindStatus();
void printCharacterConfusedState();
void printCharacterFearState();
void printCharacterPoisonedState();
void printCharacterMovementState();
void printCharacterSpeed();
void printCharacterStudyInstruction();
void printCharacterWinner();
void printCharacterStatsBlock();
void printCharacterInformation();
void printCharacterStats();
const char *statRating(int y, int x);
void printCharacterVitalStatistics();
void printCharacterLevelExperience();
void printCharacterAbilities();
void printCharacter();
void getCharacterName();
void changeCharacterName();
void displaySpellsList(const int *spell_ids, int number_of_choices, bool comment, int non_consecutive);
void displayCharacterExperience();

// UI Inventory/Equipment
int displayInventory(int item_id_start, int item_id_end, bool weighted, int column, const char *mask);
const char *playerItemWearingDescription(int body_location);
int displayEquipment(bool weighted, int column);
void inventoryExecuteCommand(char command);
bool inventoryGetInputForItemId(int &command_key_id, const char *prompt, int item_id_start, int item_id_end, char *mask, const char *message);
