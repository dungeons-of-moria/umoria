// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Terminal I/O code, uses the curses package

#include <cstdlib>
#include "headers.h"
#include "curses.h"

static bool curses_on = false;

// Spare window for saving the screen. -CJS-
static WINDOW *save_screen;

int eof_flag = 0;        // Is used to signal EOF/HANGUP condition
bool panic_save = false; // True if playing from a panic save

// Set up the terminal into a suitable state -MRC-
static void moriaTerminalInitialize() {
    // cbreak();           // <curses.h> use raw() instead as it disables Ctrl chars
    raw();                 // <curses.h> disable control characters. I.e. Ctrl-C does not work!
    noecho();              // <curses.h> do not echo typed characters
    nonl();                // <curses.h> disable translation return/newline for detection of return key
    keypad(stdscr, false); // <curses.h> disable keypad input as we handle that ourselves
    // curs_set(0);        // <curses.h> sets the appearance of the cursor based on the value of visibility

#ifdef __APPLE__
    set_escdelay(50); // <curses.h> default delay on macOS is 1 second, let's do something about that!
#endif

    curses_on = true;
}

// initializes the terminal / curses routines
bool terminalInitialize() {
    initscr();

    // Check we have enough screen. -CJS-
    if (LINES < 24 || COLS < 80) {
        (void) printf("Screen too small for moria.\n");
        return false;
    }

    save_screen = newwin(0, 0, 0, 0);
    if (save_screen == nullptr) {
        (void) printf("Out of memory in starting up curses.\n");
        return false;
    }

    moriaTerminalInitialize();

    (void) clear();
    (void) refresh();

    return true;
}

// Put the terminal in the original mode. -CJS-
void terminalRestore() {
    if (!curses_on) {
        return;
    }

    // Dump any remaining buffer
    putQIO();

    // this moves curses to bottom right corner
    int y = 0;
    int x = 0;
    getyx(stdscr, y, x);
    mvcur(y, x, LINES - 1, 0);

    // exit curses
    endwin();
    (void) fflush(stdout);

    curses_on = false;
}

void terminalSaveScreen() {
    overwrite(stdscr, save_screen);
}

void terminalRestoreScreen() {
    overwrite(save_screen, stdscr);
    touchwin(stdscr);
}

ssize_t terminalBellSound() {
    putQIO();

    // The player can turn off beeps if they find them annoying.
    if (config::options::error_beep_sound) {
#ifdef __EMSCRIPTEN__
        return beep();
#else
        return write(1, "\007", 1);
#endif
    }

    return 0;
}

// Dump the IO buffer to terminal -RAK-
void putQIO() {
    // Let inventoryExecuteCommand() know something has changed.
    screen_has_changed = true;

    (void) refresh();
}

// Flush the buffer -RAK-
void flushInputBuffer() {
    if (eof_flag != 0) {
        return;
    }

    while (checkForNonBlockingKeyPress(0))
        ;
}

// Clears screen
void clearScreen() {
    if (message_ready_to_print) {
        printMessage(CNIL);
    }
    (void) clear();
}

void clearToBottom(int row) {
    (void) move(row, 0);
    clrtobot();
}

// move cursor to a given y, x position
void moveCursor(Coord_t coord) {
    (void) move(coord.y, coord.x);
}

void addChar(char ch, Coord_t coord) {
    if (mvaddch(coord.y, coord.x, ch) == ERR) {
        abort();
    }
}

// Dump IO to buffer -RAK-
void putString(const char *out_str, Coord_t coord) {
    // truncate the string, to make sure that it won't go past right edge of screen.
    if (coord.x > 79) {
        coord.x = 79;
    }

    vtype_t str = {'\0'};
    (void) strncpy(str, out_str, (size_t)(79 - coord.x));
    str[79 - coord.x] = '\0';

    if (mvaddstr(coord.y, coord.x, str) == ERR) {
        abort();
    }
}

// Outputs a line to a given y, x position -RAK-
void putStringClearToEOL(const std::string &str, Coord_t coord) {
    if (coord.y == MSG_LINE && message_ready_to_print) {
        printMessage(CNIL);
    }

    (void) move(coord.y, coord.x);
    clrtoeol();
    putString(str.c_str(), coord);
}

// Clears given line of text -RAK-
void eraseLine(Coord_t coord) {
    if (coord.y == MSG_LINE && message_ready_to_print) {
        printMessage(CNIL);
    }

    (void) move(coord.y, coord.x);
    clrtoeol();
}

// Moves the cursor to a given interpolated y, x position -RAK-
void panelMoveCursor(Coord_t coord) {
    // Real coords convert to screen positions
    coord.y -= dg.panel.row_prt;
    coord.x -= dg.panel.col_prt;

    if (move(coord.y, coord.x) == ERR) {
        abort();
    }
}

// Outputs a char to a given interpolated y, x position -RAK-
// sign bit of a character used to indicate standout mode. -CJS
void panelPutTile(char ch, Coord_t coord) {
    // Real coords convert to screen positions
    coord.y -= dg.panel.row_prt;
    coord.x -= dg.panel.col_prt;

    if (mvaddch(coord.y, coord.x, ch) == ERR) {
        abort();
    }
}

static Coord_t currentCursorPosition() {
    int y, x;
    getyx(stdscr, y, x);
    return Coord_t{y, x};
}

// messageLinePrintMessage will print a line of text to the message line (0,0).
// first clearing the line of any text!
void messageLinePrintMessage(std::string message) {
    // save current cursor position
    Coord_t coord = currentCursorPosition();

    // move to beginning of message line, and clear it
    move(0, 0);
    clrtoeol();

    // truncate message if it's too long!
    message.resize(79);

    addstr(message.c_str());

    // restore cursor to old position
    move(coord.y, coord.x);
}

// deleteMessageLine will delete all text from the message line (0,0).
// The current cursor position will be maintained.
void messageLineClear() {
    // save current cursor position
    Coord_t coord = currentCursorPosition();

    // move to beginning of message line, and clear it
    move(0, 0);
    clrtoeol();

    // restore cursor to old position
    move(coord.y, coord.x);
}

// Outputs message to top line of screen
// These messages are kept for later reference.
void printMessage(const char *msg) {
    int new_len = 0;
    int old_len = 0;
    bool combine_messages = false;

    if (message_ready_to_print) {
        old_len = (int) strlen(messages[last_message_id]) + 1;

        // If the new message and the old message are short enough,
        // we want display them together on the same line.  So we
        // don't flush the old message in this case.

        if (msg != nullptr) {
            new_len = (int) strlen(msg);
        } else {
            new_len = 0;
        }

        if ((msg == nullptr) || new_len + old_len + 2 >= 73) {
            // ensure that the complete -more- message is visible.
            if (old_len > 73) {
                old_len = 73;
            }

            putString(" -more-", Coord_t{MSG_LINE, old_len});

            char key;
            do {
                key = getKeyInput();
            } while ((key != ' ') && (key != ESCAPE) && (key != '\n') && (key != '\r'));
        } else {
            combine_messages = true;
        }
    }

    if (!combine_messages) {
        (void) move(MSG_LINE, 0);
        clrtoeol();
    }

    // Make the null string a special case. -CJS-

    if (msg == nullptr) {
        message_ready_to_print = false;
        return;
    }

    game.command_count = 0;
    message_ready_to_print = true;

    // If the new message and the old message are short enough,
    // display them on the same line.

    if (combine_messages) {
        putString(msg, Coord_t{MSG_LINE, old_len + 2});
        strcat(messages[last_message_id], "  ");
        strcat(messages[last_message_id], msg);
    } else {
        messageLinePrintMessage(msg);
        last_message_id++;

        if (last_message_id >= MESSAGE_HISTORY_SIZE) {
            last_message_id = 0;
        }

        (void) strncpy(messages[last_message_id], msg, MORIA_MESSAGE_SIZE);
        messages[last_message_id][MORIA_MESSAGE_SIZE - 1] = '\0';
    }
}

// Print a message so as not to interrupt a counted command. -CJS-
void printMessageNoCommandInterrupt(const std::string &msg) {
    // Save command count value
    int i = game.command_count;

    printMessage(msg.c_str());

    // Restore count value
    game.command_count = i;
}

// Returns a single character input from the terminal. -CJS-
//
// This silently consumes ^R to redraw the screen and reset the
// terminal, so that this operation can always be performed at
// any input prompt. getKeyInput() never returns ^R.
char getKeyInput() {
    putQIO();               // Dump IO buffer
    game.command_count = 0; // Just to be safe -CJS-

    while (true) {
        int ch = getch();

        // some machines may not sign extend.
        if (ch == EOF) {

            // avoid infinite loops while trying to call getKeyInput() for a -more- prompt.
            message_ready_to_print = false;

            eof_flag++;

            (void) refresh();

            if (!game.character_generated || game.character_saved) {
                endGame();
            }

            playerDisturb(1, 0);

            if (eof_flag > 100) {
                // just in case, to make sure that the process eventually dies
                panic_save = true;

                (void) strcpy(game.character_died_from, "(end of input: panic saved)");
                if (!saveGame()) {
                    (void) strcpy(game.character_died_from, "panic: unexpected eof");
                    game.character_is_dead = true;
                }
                endGame();
            }
            return ESCAPE;
        }

        if (ch != CTRL_KEY('R')) {
            return (char) ch;
        }
        
        (void) wrefresh(curscr);
        moriaTerminalInitialize();
    }
}

// Prompts (optional) and returns ord value of input char
// Function returns false if <ESCAPE> is input
bool getCommand(const std::string &prompt, char &command) {
    if (!prompt.empty()) {
        putStringClearToEOL(prompt, Coord_t{0, 0});
    }
    command = getKeyInput();

    messageLineClear();

    return command != ESCAPE;
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
    (void) move(coord.y, coord.x);

    for (int i = slen; i > 0; i--) {
        (void) addch(' ');
    }

    (void) move(coord.y, coord.x);

    int start_col = coord.x;
    int end_col = coord.x + slen - 1;

    if (end_col > 79) {
        end_col = 79;
    }

    char *p = in_str;

    bool flag = false;
    bool aborted = false;

    while (!flag && !aborted) {
        int key = getKeyInput();
        switch (key) {
            case ESCAPE:
                aborted = true;
                break;
            case CTRL_KEY('J'):
            case CTRL_KEY('M'):
                flag = true;
                break;
            case DELETE:
            case CTRL_KEY('H'):
                if (coord.x > start_col) {
                    coord.x--;
                    putString(" ", coord);
                    moveCursor(coord);
                    *--p = '\0';
                }
                break;
            default:
                if ((isprint(key) == 0) || coord.x > end_col) {
                    terminalBellSound();
                } else {
                    mvaddch(coord.y, coord.x, (char) key);
                    *p++ = (char) key;
                    coord.x++;
                }
                break;
        }
    }

    if (aborted) {
        return false;
    }

    // Remove trailing blanks
    while (p > in_str && p[-1] == ' ') {
        p--;
    }
    *p = '\0';

    return true;
}

// Used to verify a choice - user gets the chance to abort choice. -CJS-
bool getInputConfirmation(const std::string &prompt) {
    int confirmed = getInputConfirmationWithAbort(0, prompt);
    return confirmed == 1;
}

// Used to verify a choice, with the option of aborting (useful for "drop all items")
// and with the option of setting the column for displaying the prompt.
int getInputConfirmationWithAbort(int column, const std::string &prompt) {
    putStringClearToEOL(prompt, Coord_t{0, column});

    int y, x;
    getyx(stdscr, y, x);

    if (x > 73) {
        (void) move(0, 73);
    } else if (y != 0) {
        // use `y` to prevent compiler warning.
    }

    (void) addstr(" [y/n]");

    char key = ' ';
    while (key == ' ') {
        key = getKeyInput();
    }

    messageLineClear();

    if (key == 'N' || key == 'n') {
        return 0;
    } else if (key == 'Y' || key == 'y') {
        return 1;
    } else {
        return -1;
    }
}

// Pauses for user response before returning -RAK-
void waitForContinueKey(int line_number) {
    putStringClearToEOL("[ press any key to continue ]", Coord_t{line_number, 23});
    (void) getKeyInput();
    eraseLine(Coord_t{line_number, 0});
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
#ifdef _WIN32
    (void) microseconds;

    // Ugly non-blocking read...Ugh! -MRC-
    timeout(8);
    int result = getch();
    timeout(-1);

    return result > 0;
#elif __EMSCRIPTEN__
    /* timeout should be in milliseconds */
    timeout(microseconds/1000);
    int result = getch();
    timeout(-1);

    return result > 0;
#else
    struct timeval tbuf {};
    int ch;
    int smask;

    // Return true if a read on descriptor 1 will not block.
    tbuf.tv_sec = 0;
    tbuf.tv_usec = microseconds;

    smask = 1; // i.e. (1 << 0)
    if (select(1, (fd_set *) &smask, (fd_set *) nullptr, (fd_set *) nullptr, &tbuf) == 1) {
        ch = getch();
        // check for EOF errors here, select sometimes works even when EOF
        if (ch == -1) {
            eof_flag++;
            return false;
        }
        return true;
    }

    return false;
#endif
}

// Find a default user name from the system.
void getDefaultPlayerName(char *buffer) {
    // Gotta have some name
    const char *default_name = "X";

#ifdef _WIN32
    unsigned long bufCharCount = PLAYER_NAME_SIZE;

    if (!GetUserName(buffer, &bufCharCount)) {
        (void) strcpy(buffer, default_name);
    }
#else
    char *p = getlogin();

    if ((p != nullptr) && (p[0] != 0)) {
        (void) strcpy(buffer, p);
    } else {
        struct passwd *pwline = getpwuid((int) getuid());
        if (pwline != nullptr) {
            (void) strcpy(buffer, pwline->pw_name);
        }
    }

    if (buffer[0] == 0) {
        (void) strcpy(buffer, default_name);
    }
#endif
}

#ifndef _WIN32
// On unix based systems we should expand `~` to the users home path,
// otherwise on Windows we can ignore all of this. -MRC-

// undefine these so that tfopen and topen will work
#undef fopen
#undef open

// open a file just as does fopen, but allow a leading ~ to specify a home directory
FILE *tfopen(const char *file, const char *mode) {
    char expanded[1024];
    if (tilde(file, expanded)) {
        return (fopen(expanded, mode));
    }
    errno = ENOENT;
    return nullptr;
}

// open a file just as does open, but expand a leading ~ into a home directory name
int topen(const char *file, int flags, int mode) {
    char expanded[1024];
    if (tilde(file, expanded)) {
        return (open(expanded, flags, mode));
    }
    errno = ENOENT;
    return -1;
}

// expands a tilde at the beginning of a file name to a users home directory
bool tilde(const char *file, char *expanded) {
    if (file == nullptr) {
        return false;
    }

    *expanded = '\0';

    if (*file == '~') {
        char user[128];
        struct passwd *pw = nullptr;
        int i = 0;

        user[0] = '\0';
        file++;
        while (*file != '/' && i < (int) sizeof(user) - 1) {
            user[i++] = *file++;
        }
        user[i] = '\0';
        if (i == 0) {
            char *login = getlogin();

            if (login != nullptr) {
                (void) strcpy(user, login);
            } else if ((pw = getpwuid(getuid())) == nullptr) {
                return false;
            }
        }
        if (pw == nullptr && (pw = getpwnam(user)) == nullptr) {
            return false;
        }
        (void) strcpy(expanded, pw->pw_dir);
    }

    (void) strcat(expanded, file);

    return true;
}

#endif

// Check user permissions on Unix based systems,
// or if on Windows just return. -MRC-
bool checkFilePermissions() {
#if !defined(_WIN32) && !defined(__EMSCRIPTEN__)
    if (0 != setuid(getuid())) {
        perror("Can't set permissions correctly!  Setuid call failed.\n");
        return false;
    }
    if (0 != setgid(getgid())) {
        perror("Can't set permissions correctly!  Setgid call failed.\n");
        return false;
    }
#endif

    return true;
}
