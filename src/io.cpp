// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Terminal I/O code, uses the curses package

#include "headers.h"
#include "curses.h"
#include "externs.h"

#define use_value2

static bool curses_on = false;

// Spare window for saving the screen. -CJS-
static WINDOW *savescr;

// initializes curses routines
void init_curses() {
    initscr();

    // Check we have enough screen. -CJS-
    if (LINES < 24 || COLS < 80) {
        (void) printf("Screen too small for moria.\n");
        exit(1);
    }

    savescr = newwin(0, 0, 0, 0);
    if (savescr == NULL) {
        (void) printf("Out of memory in starting up curses.\n");
        exit_game();
    }

    moriaterm();

    (void) clear();
    (void) refresh();
}

// Set up the terminal into a suitable state -MRC-
void moriaterm() {
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
void put_buffer(const char *out_str, int row, int col) {
    vtype tmp_str;

    // truncate the string, to make sure that it won't go past right edge of screen.
    if (col > 79) {
        col = 79;
    }
    (void) strncpy(tmp_str, out_str, (size_t) (79 - col));
    tmp_str[79 - col] = '\0';

    if (mvaddstr(row, col, tmp_str) == ERR) {
        abort();
        // clear msg_flag to avoid problems with unflushed messages.
        msg_flag = false;
        (void) sprintf(tmp_str, "error in put_buffer, row = %d col = %d\n", row, col);
        prt(tmp_str, 0, 0);
        bell();

        // wait so user can see error
        sleep_in_seconds(2);
    }
}

// Dump the IO buffer to terminal -RAK-
void put_qio() {
    // Let inven_command know something has changed.
    screen_change = true;

    (void) refresh();
}

// Put the terminal in the original mode. -CJS-
void restore_term() {
    if (!curses_on) {
        return;
    }

    // Dump any remaining buffer
    put_qio();

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

void shell_out() {
    put_buffer("[Opening new shells is not currently supported]\n", 0, 0);
}

// Returns a single character input from the terminal. -CJS-
//
// This silently consumes ^R to redraw the screen and reset the
// terminal, so that this operation can always be performed at
// any input prompt. inkey() never returns ^R.
char inkey() {
    put_qio();         // Dump IO buffer
    command_count = 0; // Just to be safe -CJS-

    while (true) {
        int i = getch();

        // some machines may not sign extend.
        if (i == EOF) {
            // avoid infinite loops while trying to call inkey() for a -more- prompt.
            msg_flag = false;

            eof_flag++;

            (void) refresh();

            if (!character_generated || character_saved) {
                exit_game();
            }

            disturb(1, 0);

            if (eof_flag > 100) {
                // just in case, to make sure that the process eventually dies
                panic_save = true;

                (void) strcpy(died_from, "(end of input: panic saved)");
                if (!save_char()) {
                    (void) strcpy(died_from, "panic: unexpected eof");
                    death = true;
                }
                exit_game();
            }
            return ESCAPE;
        }

        if (i != CTRL_KEY('R')) {
            return (char) i;
        }

        (void) wrefresh(curscr);
        moriaterm();
    }
}

// Flush the buffer -RAK-
void flush() {
    if (!eof_flag) {
        while (check_input(0)) { ;
        }
    }
}

// Clears given line of text -RAK-
void erase_line(int row, int col) {
    if (row == MSG_LINE && msg_flag) {
        msg_print(CNIL);
    }

    (void) move(row, col);
    clrtoeol();
}

// Clears screen
void clear_screen() {
    if (msg_flag) {
        msg_print(CNIL);
    }
    (void) clear();
}

void clear_from(int row) {
    (void) move(row, 0);
    clrtobot();
}

// Outputs a char to a given interpolated y, x position -RAK-
// sign bit of a character used to indicate standout mode. -CJS
void print(char ch, int row, int col) {
    // Real coords convert to screen positions
    row -= panel_row_prt;
    col -= panel_col_prt;

    if (mvaddch(row, col, ch) == ERR) {
        abort();

        // clear msg_flag to avoid problems with unflushed messages
        msg_flag = false;

        vtype tmp_str;
        (void) sprintf(tmp_str, "error in print, row = %d col = %d\n", row, col);
        prt(tmp_str, 0, 0);
        bell();

        // wait so user can see error
        sleep_in_seconds(2);
    }
}

// Moves the cursor to a given interpolated y, x position -RAK-
void move_cursor_relative(int row, int col) {
    // Real coords convert to screen positions
    row -= panel_row_prt;
    col -= panel_col_prt;

    if (move(row, col) == ERR) {
        abort();
        // clear msg_flag to avoid problems with unflushed messages
        msg_flag = false;

        vtype tmp_str;
        (void) sprintf(tmp_str, "error in move_cursor_relative, row = %d col = %d\n", row, col);
        prt(tmp_str, 0, 0);
        bell();

        // wait so user can see error
        sleep_in_seconds(2);
    }
}

// Print a message so as not to interrupt a counted command. -CJS-
void count_msg_print(const char *p) {
    int i = command_count;
    msg_print(p);
    command_count = i;
}

// Outputs a line to a given y, x position -RAK-
void prt(const char *str_buff, int row, int col) {
    if (row == MSG_LINE && msg_flag) {
        msg_print(CNIL);
    }

    (void) move(row, col);
    clrtoeol();
    put_buffer(str_buff, row, col);
}

// move cursor to a given y, x position
void move_cursor(int row, int col) {
    (void) move(row, col);
}

// Outputs message to top line of screen
// These messages are kept for later reference.
void msg_print(const char *str_buff) {
    int new_len = 0;
    int old_len = 0;
    bool combine_messages = false;

    if (msg_flag) {
        old_len = (int) strlen(old_msg[last_msg]) + 1;

        // If the new message and the old message are short enough,
        // we want display them together on the same line.  So we
        // don't flush the old message in this case.

        if (str_buff) {
            new_len = (int) strlen(str_buff);
        } else {
            new_len = 0;
        }

        if (!str_buff || new_len + old_len + 2 >= 73) {
            // ensure that the complete -more- message is visible.
            if (old_len > 73) {
                old_len = 73;
            }

            put_buffer(" -more-", MSG_LINE, old_len);

            // let sigint handler know that we are waiting for a space
            wait_for_more = true;

            char in_char;
            do {
                in_char = inkey();
            } while ((in_char != ' ') && (in_char != ESCAPE) && (in_char != '\n') && (in_char != '\r'));

            wait_for_more = false;
        } else {
            combine_messages = true;
        }
    }

    if (!combine_messages) {
        (void) move(MSG_LINE, 0);
        clrtoeol();
    }

    // Make the null string a special case. -CJS-
    if (str_buff) {
        command_count = 0;
        msg_flag = true;

        // If the new message and the old message are short enough,
        // display them on the same line.

        if (combine_messages) {
            put_buffer(str_buff, MSG_LINE, old_len + 2);
            strcat(old_msg[last_msg], "  ");
            strcat(old_msg[last_msg], str_buff);
        } else {
            put_buffer(str_buff, MSG_LINE, 0);
            last_msg++;

            if (last_msg >= MAX_SAVE_MSG) {
                last_msg = 0;
            }

            (void) strncpy(old_msg[last_msg], str_buff, VTYPESIZ);
            old_msg[last_msg][VTYPESIZ - 1] = '\0';
        }
    } else {
        msg_flag = false;
    }
}

// Used to verify a choice - user gets the chance to abort choice. -CJS-
bool get_check(const char *prompt) {
    prt(prompt, 0, 0);

    int y, x;
    getyx(stdscr, y, x);

    if (x > 73) {
        (void) move(0, 73);
    } else if (y) {
        // use `y` to prevent compiler warning.
    }

    (void) addstr(" [y/n]");

    char res = ' ';
    while (res == ' ') {
        res = inkey();
    }

    erase_line(0, 0);

    return (res == 'Y' || res == 'y');
}

// Prompts (optional) and returns ord value of input char
// Function returns false if <ESCAPE> is input
int get_com(const char *prompt, char *command) {
    if (prompt) {
        prt(prompt, 0, 0);
    }

    *command = inkey();

    bool res = (*command != ESCAPE);

    erase_line(MSG_LINE, 0);

    return res;
}

// Gets a string terminated by <RETURN>
// Function returns false if <ESCAPE> is input
bool get_string(char *in_str, int row, int column, int slen) {
    (void) move(row, column);

    for (int i = slen; i > 0; i--) {
        (void) addch(' ');
    }

    (void) move(row, column);

    int start_col = column;
    int end_col = column + slen - 1;
    if (end_col > 79) {
        end_col = 79;
    }

    char *p = in_str;

    bool aborted = false;
    bool flag = false;

    while (!flag && !aborted) {
        int i = inkey();

        switch (i) {
            case ESCAPE:
                aborted = true;
                break;
            case CTRL_KEY('J'):
            case CTRL_KEY('M'):
                flag = true;
                break;
            case DELETE:
            case CTRL_KEY('H'):
                if (column > start_col) {
                    column--;
                    put_buffer(" ", row, column);
                    move_cursor(row, column);
                    *--p = '\0';
                }
                break;
            default:
                if (!isprint(i) || column > end_col) {
                    bell();
                } else {
                    use_value2 mvaddch(row, column, (char) i);
                    *p++ = (char) i;
                    column++;
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
void pause_line(int prt_line) {
    prt("[Press any key to continue.]", prt_line, 23);
    (void) inkey();
    erase_line(prt_line, 0);
}

// Pauses for user response before returning -RAK-
// NOTE: Delay is for players trying to roll up "perfect"
// characters.  Make them wait a bit.
void pause_exit(int prt_line, int delay) {
    prt("[Press any key to continue, or Q to exit.]", prt_line, 10);

    char dummy = inkey();
    if (dummy == 'Q') {
        erase_line(prt_line, 0);

        if (delay > 0) {
            sleep_in_seconds(delay);
        }

        exit_game();
    }
    erase_line(prt_line, 0);
}

void save_screen() {
    overwrite(stdscr, savescr);
}

void restore_screen() {
    overwrite(savescr, stdscr);
    touchwin(stdscr);
}

void bell() {
    put_qio();

    // The player can turn off beeps if he/she finds them annoying.
    if (sound_beep_flag) {
        (void) write(1, "\007", 1);
    }
}

// definitions used by screen_map()
// index into border character array
#define TL 0 // top left
#define TR 1
#define BL 2
#define BR 3
#define HE 4 // horizontal edge
#define VE 5

// character set to use
#define CH(x) (screen_border[0][x])

// Display highest priority object in the RATIO by RATIO area
#define RATIO 3

void screen_map() {
    static uint8_t screen_border[2][6] = {
            {'+', '+', '+', '+', '-', '|'}, // normal chars
            {201, 187, 200, 188, 205, 186}, // graphics chars
    };

    uint8_t map[MAX_WIDTH / RATIO + 1];
    int priority[256];

    char prntscrnbuf[80];

    for (int i = 0; i < 256; i++) {
        priority[i] = 0;
    }

    priority[60] = 5;       // char '<'
    priority[62] = 5;       // char '>'
    priority[64] = 10;      // char '@'
    priority[35] = -5;      // char '#'
    priority[46] = -10;     // char '.'
    priority[92] = -3;      // char '\'
    priority[32] = -15;     // char ' '

    save_screen();
    clear_screen();
    use_value2 mvaddch(0, 0, CH(TL));

    for (int i = 0; i < MAX_WIDTH / RATIO; i++) {
        (void) addch(CH(HE));
    }

    (void) addch(CH(TR));
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
                (void) sprintf(prntscrnbuf, "%c%s%c", CH(VE), map, CH(VE));
                use_value2 mvaddstr(orow + 1, 0, prntscrnbuf);
            }

            for (int j = 0; j < MAX_WIDTH / RATIO; j++) {
                map[j] = ' ';
            }

            orow = row;
        }

        for (int j = 0; j < MAX_WIDTH; j++) {
            int col = j / RATIO;
            uint8_t tmp = loc_symbol(i, j);

            if (priority[map[col]] < priority[tmp]) {
                map[col] = tmp;
            }

            if (map[col] == '@') {
                mycol = col + 1; // account for border
                myrow = row + 1;
            }
        }
    }

    if (orow >= 0) {
        (void) sprintf(prntscrnbuf, "%c%s%c", CH(VE), map, CH(VE));
        use_value2 mvaddstr(orow + 1, 0, prntscrnbuf);
    }

    use_value2 mvaddch(orow + 2, 0, CH(BL));

    for (int i = 0; i < MAX_WIDTH / RATIO; i++) {
        (void) addch(CH(HE));
    }

    (void) addch(CH(BR));

    use_value2 mvaddstr(23, 23, "Hit any key to continue");

    if (mycol > 0) {
        (void) move(myrow, mycol);
    }

    (void) inkey();
    restore_screen();
}

void sleep_in_seconds(int seconds) {
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
bool check_input(int microsec) {
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
    struct timeval tbuf;
    int ch;
    int smask;

    // Return true if a read on descriptor 1 will not block.
    tbuf.tv_sec = 0;
    tbuf.tv_usec = microsec;

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
void user_name(char *buf) {
#ifdef _WIN32
    unsigned long bufCharCount = PLAYER_NAME_SIZE;

    if (!GetUserName(buf, &bufCharCount)) {
        (void)strcpy(buf, "X"); // Gotta have some name
    }
#else
    extern char *getlogin();

    char *p = getlogin();

    if (p && p[0]) {
        (void) strcpy(buf, p);
    } else {
        struct passwd *pwline = getpwuid((int) getuid());
        if (pwline) {
            (void) strcpy(buf, pwline->pw_name);
        }
    }
    if (!buf[0]) {
        (void) strcpy(buf, "X"); // Gotta have some name
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
    // extern int errno;

    char buf[1024];
    if (tilde(file, buf)) {
        return (fopen(buf, mode));
    }
    errno = ENOENT;
    return NULL;
}

// open a file just as does open, but expand a leading ~ into a home directory name
int topen(const char *file, int flags, int mode) {
    // extern int errno;

    char buf[1024];
    if (tilde(file, buf)) {
        return (open(buf, flags, mode));
    }
    errno = ENOENT;
    return -1;
}

// expands a tilde at the beginning of a file name to a users home directory
int tilde(const char *file, char *exp) {
    if (!file) {
        return 0;
    }

    *exp = '\0';

    if (*file == '~') {
        char user[128];
        struct passwd *pw = NULL;
        int i = 0;

        user[0] = '\0';
        file++;
        while (*file != '/' && i < (int) sizeof(user)) {
            user[i++] = *file++;
        }
        user[i] = '\0';
        if (i == 0) {
            char *login = getlogin();

            if (login != NULL) {
                (void) strcpy(user, login);
            } else if ((pw = getpwuid(getuid())) == NULL) {
                return 0;
            }
        }
        if (pw == NULL && (pw = getpwnam(user)) == NULL) {
            return 0;
        }
        (void) strcpy(exp, pw->pw_dir);
    }
    (void) strcat(exp, file);

    return 1;
}

#endif
