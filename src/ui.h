// Copyright (c) 1989-94 James E. Wilson, Robert A. Koeneke
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

#pragma once

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

// UI
bool coordOutsidePanel(Coord_t coord, bool force);
bool coordInsidePanel(Coord_t coord);
void drawDungeonPanel();
void drawCavePanel();
void dungeonResetView();
