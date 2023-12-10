// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// SPDX-License-Identifier: GPL-3.0-or-later

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

// message line location
constexpr uint8_t MSG_LINE = 0;

// How many messages to save in the buffer -CJS-
constexpr uint8_t MESSAGE_HISTORY_SIZE = 22;

// Column for stats
constexpr uint8_t STAT_COLUMN = 0;

constexpr char CTRL_KEY(char x) {
    return static_cast<char>((x) & 0x1F);
}

#undef DELETE
constexpr char DELETE = 0x7f;

#undef ESCAPE
constexpr char ESCAPE = '\033'; // ESCAPE character -CJS-

extern bool screen_has_changed;
extern bool message_ready_to_print;
extern vtype_t messages[MESSAGE_HISTORY_SIZE];
extern int16_t last_message_id;

extern int eof_flag;
extern bool panic_save;

// UI - IO
bool terminalInitialize();
void terminalRestore();
void terminalSaveScreen();
void terminalRestoreScreen();
ssize_t terminalBellSound();
void putQIO();
void flushInputBuffer();
void clearScreen();
void clearToBottom(int row);
void moveCursor(Coord_t coord);
void addChar(char ch, Coord_t coord);
void putString(const char *out_str, Coord_t coord);
void putStringClearToEOL(const std::string &str, Coord_t coord);
void eraseLine(Coord_t coord);
void panelMoveCursor(Coord_t coord);
void panelPutTile(char ch, Coord_t coord);
void messageLinePrintMessage(std::string message);
void messageLineClear();
void printMessage(const char *msg);
void printMessageNoCommandInterrupt(const std::string &msg);
char getKeyInput();
bool getCommand(const std::string &prompt, char &command);
bool getMenuItemId(const std::string &prompt, char &command);
bool getTileCharacter(const std::string &prompt, char &command);
bool getStringInput(char *in_str, Coord_t coord, int slen);
bool getInputConfirmation(const std::string &prompt);
int getInputConfirmationWithAbort(int column, const std::string &prompt);
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
const char *statRating(Coord_t coord);
void printCharacterVitalStatistics();
void printCharacterLevelExperience();
void printCharacterAbilities();
void printCharacter();
void getCharacterName();
void changeCharacterName();
void displaySpellsList(const int *spell_ids, int number_of_choices, bool comment, int non_consecutive);
void displayCharacterExperience();

// UI Inventory/Equipment
int displayInventoryItems(int itemIdStart, int itemIdEnd, bool weighted, int column, const char *mask);
const char *playerItemWearingDescription(int body_location);
int displayEquipment(bool showWeights, int column);
void inventoryExecuteCommand(char command);
bool inventoryGetInputForItemId(int &commandKeyId, const char *prompt, int itemIdStart, int itemIdEnd, char *mask, const char *message);
