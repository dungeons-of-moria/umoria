// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Terminal I/O code, uses the curses package

#include <cstdlib>
#include "headers.h"
#include "curses.h"
#include "externs.h"

#define use_value2

static bool curses_on = false;

// Spare window for saving the screen. -CJS-
static WINDOW *save_screen;

static void moriaTerminalInitialize();
static void sleepInSeconds(int seconds);

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
        exitGame();
        return false;
    }

    moriaTerminalInitialize();

    (void) clear();
    (void) refresh();

    return true;
}

// Set up the terminal into a suitable state -MRC-
static void moriaTerminalInitialize() {
    raw();                 // <curses.h> disable control characters. I.e. Ctrl-C does not work!
    // cbreak();           // <curses.h> use raw() instead as it disables Ctrl chars
    noecho();              // <curses.h> do not echo typed characters
    nonl();                // <curses.h> disable translation return/newline for detection of return key
    keypad(stdscr, false); // <curses.h> disable keypad input as we handle that ourselves
    // curs_set(0);        // <curses.h> sets the appearance of the cursor based on the value of visibility

#ifdef __APPLE__
    set_escdelay(50);      // <curses.h> default delay on macOS is 1 second, let's do something about that!
#endif

    curses_on = true;
}

// Dump IO to buffer -RAK-
void putString(const char *out_str, int row, int col) {
    // truncate the string, to make sure that it won't go past right edge of screen.
    if (col > 79) {
        col = 79;
    }

    vtype_t str;
    (void) strncpy(str, out_str, (size_t) (79 - col));
    str[79 - col] = '\0';

    if (mvaddstr(row, col, str) == ERR) {
        abort();
    }
}

// Dump the IO buffer to terminal -RAK-
void putQIO() {
    // Let inventoryExecuteCommand() know something has changed.
    screen_has_changed = true;

    (void) refresh();
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

// Returns a single character input from the terminal. -CJS-
//
// This silently consumes ^R to redraw the screen and reset the
// terminal, so that this operation can always be performed at
// any input prompt. getKeyInput() never returns ^R.
char getKeyInput() {
    putQIO();         // Dump IO buffer
    command_count = 0; // Just to be safe -CJS-

    while (true) {
        int ch = getch();

        // some machines may not sign extend.
        if (ch == EOF) {
            // avoid infinite loops while trying to call getKeyInput() for a -more- prompt.
            message_ready_to_print = false;

            eof_flag++;

            (void) refresh();

            if (!character_generated || character_saved) {
                exitGame();
            }

            playerDisturb(1, 0);

            if (eof_flag > 100) {
                // just in case, to make sure that the process eventually dies
                panic_save = true;

                (void) strcpy(character_died_from, "(end of input: panic saved)");
                if (!saveGame()) {
                    (void) strcpy(character_died_from, "panic: unexpected eof");
                    character_is_dead = true;
                }
                exitGame();
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

// Flush the buffer -RAK-
void flushInputBuffer() {
    if (eof_flag != 0) {
        return;
    }

    while (checkForNonBlockingKeyPress(0));
}

// Clears given line of text -RAK-
void eraseLine(int row, int col) {
    if (row == MSG_LINE && message_ready_to_print) {
        printMessage(CNIL);
    }

    (void) move(row, col);
    clrtoeol();
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

// Outputs a char to a given interpolated y, x position -RAK-
// sign bit of a character used to indicate standout mode. -CJS
void putChar(char ch, int row, int col) {
    // Real coords convert to screen positions
    row -= panel_row_prt;
    col -= panel_col_prt;

    if (mvaddch(row, col, ch) == ERR) {
        abort();
    }
}

// Moves the cursor to a given interpolated y, x position -RAK-
void moveCursorRelative(int row, int col) {
    // Real coords convert to screen positions
    row -= panel_row_prt;
    col -= panel_col_prt;

    if (move(row, col) == ERR) {
        abort();
    }
}

// Print a message so as not to interrupt a counted command. -CJS-
void printMessageNoCommandInterrupt(const char *msg) {
    // Save command count value
    int i = command_count;

    printMessage(msg);

    // Restore count value
    command_count = i;
}

// Outputs a line to a given y, x position -RAK-
void putStringClearToEOL(const char *str, int row, int col) {
    if (row == MSG_LINE && message_ready_to_print) {
        printMessage(CNIL);
    }

    (void) move(row, col);
    clrtoeol();
    putString(str, row, col);
}

// move cursor to a given y, x position
void moveCursor(int y, int x) {
    (void) move(y, x);
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

            putString(" -more-", MSG_LINE, old_len);

            char in_char;
            do {
                in_char = getKeyInput();
            } while ((in_char != ' ') && (in_char != ESCAPE) && (in_char != '\n') && (in_char != '\r'));
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

    command_count = 0;
    message_ready_to_print = true;

    // If the new message and the old message are short enough,
    // display them on the same line.

    if (combine_messages) {
        putString(msg, MSG_LINE, old_len + 2);
        strcat(messages[last_message_id], "  ");
        strcat(messages[last_message_id], msg);
    } else {
        putString(msg, MSG_LINE, 0);
        last_message_id++;

        if (last_message_id >= MESSAGE_HISTORY_SIZE) {
            last_message_id = 0;
        }

        (void) strncpy(messages[last_message_id], msg, MORIA_MESSAGE_SIZE);
        messages[last_message_id][MORIA_MESSAGE_SIZE - 1] = '\0';
    }
}

// Used to verify a choice - user gets the chance to abort choice. -CJS-
bool getInputConfirmation(const char *prompt) {
    putStringClearToEOL(prompt, 0, 0);

    int y, x;
    getyx(stdscr, y, x);

    if (x > 73) {
        (void) move(0, 73);
    } else if (y != 0) {
        // use `y` to prevent compiler warning.
    }

    (void) addstr(" [y/n]");

    char input = ' ';
    while (input == ' ') {
        input = getKeyInput();
    }

    eraseLine(0, 0);

    return (input == 'Y' || input == 'y');
}

// Prompts (optional) and returns ord value of input char
// Function returns false if <ESCAPE> is input
bool getCommand(const char *prompt, char *command) {
    if (prompt != nullptr) {
        putStringClearToEOL(prompt, 0, 0);
    }
    *command = getKeyInput();

    eraseLine(MSG_LINE, 0);

    return *command != ESCAPE;
}

// Gets a string terminated by <RETURN>
// Function returns false if <ESCAPE> is input
bool getStringInput(char *in_str, int row, int col, int slen) {
    (void) move(row, col);

    for (int i = slen; i > 0; i--) {
        (void) addch(' ');
    }

    (void) move(row, col);

    int start_col = col;
    int end_col = col + slen - 1;

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
                if (col > start_col) {
                    col--;
                    putString(" ", row, col);
                    moveCursor(row, col);
                    *--p = '\0';
                }
                break;
            default:
                if ((isprint(key) == 0) || col > end_col) {
                    terminalBellSound();
                } else {
                    use_value2 mvaddch(row, col, (char) key);
                    *p++ = (char) key;
                    col++;
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

// Pauses for user response before returning -RAK-
void waitForContinueKey(int line_number) {
    putStringClearToEOL("[Press any key to continue.]", line_number, 23);
    (void) getKeyInput();
    eraseLine(line_number, 0);
}

// Pauses for user response before returning -RAK-
// NOTE: Delay is for players trying to roll up "perfect"
// characters.  Make them wait a bit.
void waitAndConfirmCharacterCreation(int line_number, int delay) {
    putStringClearToEOL("[Press any key to continue, or Q to exit.]", line_number, 10);

    if (getKeyInput() == 'Q') {
        eraseLine(line_number, 0);

        if (delay > 0) {
            sleepInSeconds(delay);
        }

        exitGame();
    }

    eraseLine(line_number, 0);
}

void terminalSaveScreen() {
    overwrite(stdscr, save_screen);
}

void terminalRestoreScreen() {
    overwrite(save_screen, stdscr);
    touchwin(stdscr);
}

void terminalBellSound() {
    putQIO();

    // The player can turn off beeps if they find them annoying.
    if (config.error_beep_sound) {
        (void) write(1, "\007", 1);
    }
}

// definitions used by displayDungeonMap()
// index into border character array
#define TL 0 // top left
#define TR 1
#define BL 2
#define BR 3
#define HE 4 // horizontal edge
#define VE 5

// Display highest priority object in the RATIO by RATIO area
constexpr int RATIO = 3;

static char screen_border[6] = {'+', '+', '+', '+', '-', '|'};

// character set to use
static char getBorderTile(uint8_t id) {
    return screen_border[id];
}

void displayDungeonMap() {
    char map[MAX_WIDTH / RATIO + 1];
    char prntscrnbuf[80];

    int priority[256];
    for (int &i : priority) {
        i = 0;
    }

    priority[60] = 5;    // char '<'
    priority[62] = 5;    // char '>'
    priority[64] = 10;   // char '@'
    priority[35] = -5;   // char '#'
    priority[46] = -10;  // char '.'
    priority[92] = -3;   // char '\'
    priority[32] = -15;  // char ' '

    terminalSaveScreen();
    clearScreen();
    use_value2 mvaddch(0, 0, getBorderTile(TL));

    for (int i = 0; i < MAX_WIDTH / RATIO; i++) {
        (void) addch((const chtype) getBorderTile(HE));
    }

    (void) addch((const chtype) getBorderTile(TR));
    map[MAX_WIDTH / RATIO] = '\0';

    int myrow = 0;
    int mycol = 0;
    int orow = -1;

    for (int i = 0; i < MAX_HEIGHT; i++) {
        int row = i / RATIO;
        if (row != orow) {
            if (orow >= 0) {
                // can not use mvprintw() on IBM PC, because PC-Curses is horribly
                // written, and mvprintw() causes the fp emulation library to be
                // linked with PC-Moria, makes the program 10K bigger
                (void) sprintf(prntscrnbuf, "%c%s%c", getBorderTile(VE), map, getBorderTile(VE));
                use_value2 mvaddstr(orow + 1, 0, prntscrnbuf);
            }

            for (int j = 0; j < MAX_WIDTH / RATIO; j++) {
                map[j] = ' ';
            }

            orow = row;
        }

        for (int j = 0; j < MAX_WIDTH; j++) {
            int col = j / RATIO;
            char tmpChar = caveGetTileSymbol(i, j);
            if (priority[(uint8_t) map[col]] < priority[(uint8_t) tmpChar]) {
                map[col] = tmpChar;
            }

            if (map[col] == '@') {
                mycol = col + 1; // account for border
                myrow = row + 1;
            }
        }
    }

    if (orow >= 0) {
        (void) sprintf(prntscrnbuf, "%c%s%c", getBorderTile(VE), map, getBorderTile(VE));
        use_value2 mvaddstr(orow + 1, 0, prntscrnbuf);
    }

    use_value2 mvaddch(orow + 2, 0, getBorderTile(BL));

    for (int i = 0; i < MAX_WIDTH / RATIO; i++) {
        (void) addch((const chtype) getBorderTile(HE));
    }

    (void) addch((const chtype) getBorderTile(BR));

    use_value2 mvaddstr(23, 23, "Hit any key to continue");

    if (mycol > 0) {
        (void) move(myrow, mycol);
    }

    (void) getKeyInput();
    terminalRestoreScreen();
}

static void sleepInSeconds(int seconds) {
#ifdef _WIN32
    Sleep(seconds * 1000);
#else
    sleep((unsigned int) seconds);
#endif
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
    // Ugly non-blocking read...Ugh! -MRC-
    timeout(8);
    int result = getch();
    timeout(-1);

    if (result > 0) {
        return true;
    } else {
        return false;
    }
#else
    struct timeval tbuf{};
    int ch;
    int smask;

    // Return true if a read on descriptor 1 will not block.
    tbuf.tv_sec = 0;
    tbuf.tv_usec = microseconds;

    smask = 1; // i.e. (1 << 0)
    if (select(1, (fd_set *) &smask, (fd_set *) 0, (fd_set *) 0, &tbuf) == 1) {
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
    const char *defaultName = "X";

#ifdef _WIN32
    unsigned long bufCharCount = PLAYER_NAME_SIZE;

    if (!GetUserName(buffer, &bufCharCount)) {
        (void)strcpy(buffer, defaultName);
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
        (void) strcpy(buffer, defaultName);
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
        while (*file != '/' && i < (int) sizeof(user)) {
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
void checkFilePermissions() {
#ifndef _WIN32
    if (0 != setuid(getuid())) {
        perror("Can't set permissions correctly!  Setuid call failed.\n");
        exit(0);
    }
    if (0 != setgid(getgid())) {
        perror("Can't set permissions correctly!  Setgid call failed.\n");
        exit(0);
    }
#endif
}
