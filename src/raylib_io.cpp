// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Terminal I/O code, uses the raylib package

#include <cstdlib>
#include "headers.h"
#include "raylib.h"

int eof_flag = 0;        // Is used to signal EOF/HANGUP condition
bool panic_save = false; // True if playing from a panic save

// initializes the terminal / raylib routines
bool terminalInitialize() {
    InitWindow(80 * 20, 24 * 20, "umoria");
    SetTargetFPS(60);
    return true;
}

// Put the terminal in the original mode. -CJS-
void terminalRestore() {
    CloseWindow();
}

void terminalSaveScreen() {
}

void terminalRestoreScreen() {
}

ssize_t terminalBellSound() {
    return 0;
}

// Dump the IO buffer to terminal -RAK-
void putQIO() {
}

// Flush the buffer -RAK-
void flushInputBuffer() {
}

// Clears screen
void clearScreen() {
    BeginDrawing();
    ClearBackground(BLACK);
    EndDrawing();
}

void clearToBottom(int row) {
}

// move cursor to a given y, x position
void moveCursor(Coord_t coord) {
}

void addChar(char ch, Coord_t coord) {
    DrawText(TextFormat("%c", ch), coord.x * 20, coord.y * 20, 20, WHITE);
}

// Dump IO to buffer -RAK-
void putString(const char *out_str, Coord_t coord) {
    DrawText(out_str, coord.x * 20, coord.y * 20, 20, WHITE);
}

// Outputs a line to a given y, x position -RAK-
void putStringClearToEOL(const std::string &str, Coord_t coord) {
}

// Clears given line of text -RAK-
void eraseLine(Coord_t coord) {
}

// Moves the cursor to a given interpolated y, x position -RAK-
void panelMoveCursor(Coord_t coord) {
}

// Outputs a char to a given interpolated y, x position -RAK-
// sign bit of a character used to indicate standout mode. -CJS
void panelPutTile(char ch, Coord_t coord) {
}

static Coord_t currentCursorPosition() {
    return Coord_t{0, 0};
}

// messageLinePrintMessage will print a line of text to the message line (0,0).
// first clearing the line of any text!
void messageLinePrintMessage(std::string message) {
}

// deleteMessageLine will delete all text from the message line (0,0).
// The current cursor position will be maintained.
void messageLineClear() {
}

// Outputs message to top line of screen
// These messages are kept for later reference.
void printMessage(const char *msg) {
}

// Print a message so as not to interrupt a counted command. -CJS-
void printMessageNoCommandInterrupt(const std::string &msg) {
}

// Returns a single character input from the terminal. -CJS-
//
// This silently consumes ^R to redraw the screen and reset the
// terminal, so that this operation can always be performed at
// any input prompt. getKeyInput() never returns ^R.
char getKeyInput() {
    int key = GetKeyPressed();
    if (key == 0) {
        return ' ';
    }
    return (char)key;
}

// Prompts (optional) and returns ord value of input char
// Function returns false if <ESCAPE> is input
bool getCommand(const std::string &prompt, char &command) {
    if (!prompt.empty()) {
        putStringClearToEOL(prompt, Coord_t{0, 0});
    }
    command = getKeyInput();

    messageLineClear();

    return command != 27; // ESCAPE
}

// NOTE: currently this just wraps the getCommand() function, but better defines the different usages. -MRC-
bool getTileCharacter(const std::string &prompt, char &command) {
    return getCommand(prompt, command);
}
// NOTE: currently this just wraps the getCommand() function, but better defines the different usages. -MRC-
bool getMenuItemId(const std::string &prompt, char &command) {
    return getCommand(prompt, command);
}

// Gets a string terminated by <RETURN>
// Function returns false if <ESCAPE> is input
bool getStringInput(char *in_str, Coord_t coord, int slen) {
    return false;
}

// Used to verify a choice - user gets the chance to abort choice. -CJS-
bool getInputConfirmation(const std::string &prompt) {
    return false;
}

// Used to verify a choice, with the option of aborting (useful for "drop all items")
// and with the option of setting the column for displaying the prompt.
int getInputConfirmationWithAbort(int column, const std::string &prompt) {
    return 0;
}

// Pauses for user response before returning -RAK-
void waitForContinueKey(int line_number) {
}

// Provides for a timeout on input. Does a non-blocking read, consuming the data if
// any, and then returns 1 if data was read, zero otherwise.
//
// Porting:
//
// In systems without the select call, but with a sleep for fractional numbers of
// seconds, one could sleep for the time and then check for input.
//
// In systems which can only sleep for whole number of seconds, you might sleep by
// writing a lot of nulls to the terminal, and waiting for them to drain, or you
// might hack a static accumulation of times to wait. When the accumulation reaches
// a certain point, sleep for a second. There would need to be a way of resetting
// the count, with a call made for commands like run or rest.
bool checkForNonBlockingKeyPress(int microseconds) {
    return false;
}

// Find a default user name from the system.
void getDefaultPlayerName(char *buffer) {
}

#ifndef _WIN32
// On unix based systems we should expand `~` to the users home path,
// otherwise on Windows we can ignore all of this. -MRC-

// undefine these so that tfopen and topen will work
#undef fopen
#undef open

// open a file just as does fopen, but allow a leading ~ to specify a home directory
FILE *tfopen(const char *file, const char *mode) {
    return nullptr;
}

// open a file just as does open, but expand a leading ~ into a home directory name
int topen(const char *file, int flags, int mode) {
    return 0;
}

// expands a tilde at the beginning of a file name to a users home directory
bool tilde(const char *file, char *expanded) {
    return false;
}

#endif

// Check user permissions on Unix based systems,
// or if on Windows just return. -MRC-
bool checkFilePermissions() {
    return true;
}
