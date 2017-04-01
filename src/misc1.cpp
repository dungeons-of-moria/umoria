// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Misc utility and initialization code, magic objects code

#include "headers.h"
#include "externs.h"

// gets a new random seed for the random number generator
void init_seeds(uint32_t seed) {
    uint32_t clock_var;

    if (seed == 0) {
        clock_var = (uint32_t)time((time_t *)0);
    } else {
        clock_var = seed;
    }
    randes_seed = (int32_t)clock_var;

    clock_var += 8762;
    town_seed = (int32_t)clock_var;

    clock_var += 113452L;
    set_rnd_seed(clock_var);
    // make it a little more random
    for (clock_var = (uint32_t)randint(100); clock_var != 0; clock_var--) {
        (void)rnd();
    }
}

// holds the previous rnd state
static uint32_t old_seed;

// change to different random number generator state
void set_seed(uint32_t seed) {
    old_seed = get_rnd_seed();

    // want reproducible state here
    set_rnd_seed(seed);
}

// restore the normal random generator state
void reset_seed() {
    set_rnd_seed(old_seed);
}

// Check the day-time strings to see if open -RAK-
bool check_time() {
    // Play the game any time of day you like!
    return true;
}

// Generates a random integer x where 1<=X<=MAXVAL -RAK-
int randint(int maxval) {
    int32_t randval = rnd();
    return ((int)(randval % maxval) + 1);
}

// Generates a random integer number of NORMAL distribution -RAK-
int randnor(int mean, int stand) {
    // alternate randnor code, slower but much smaller since no table
    // 2 per 1,000,000 will be > 4*SD, max is 5*SD
    //
    // tmp = damroll(8, 99);   // mean 400, SD 81
    // tmp = (tmp - 400) * stand / 81;
    // return tmp + mean;

    int tmp = randint(MAX_SHORT);

    // off scale, assign random value between 4 and 5 times SD
    if (tmp == MAX_SHORT) {
        int offset = 4 * stand + randint(stand);

        // one half are negative
        if (randint(2) == 1) {
            offset = -offset;
        }

        return mean + offset;
    }

    // binary search normal normal_table to get index that
    // matches tmp this takes up to 8 iterations.
    int low = 0;
    int iindex = NORMAL_TABLE_SIZE >> 1;
    int high = NORMAL_TABLE_SIZE;

    while (true) {
        if ((normal_table[iindex] == tmp) || (high == (low + 1))) {
            break;
        }
        if (normal_table[iindex] > tmp) {
            high = iindex;
            iindex = low + ((iindex - low) >> 1);
        } else {
            low = iindex;
            iindex = iindex + ((high - iindex) >> 1);
        }
    }

    // might end up one below target, check that here
    if (normal_table[iindex] < tmp) {
        iindex = iindex + 1;
    }

    // normal_table is based on SD of 64, so adjust the
    // index value here, round the half way case up.
    int offset = ((stand * iindex) + (NORMAL_TABLE_SD >> 1)) / NORMAL_TABLE_SD;

    // one half should be negative
    if (randint(2) == 1) {
        offset = -offset;
    }

    return mean + offset;
}

// Returns position of first set bit -RAK-
// and clears that bit
int bit_pos(uint32_t *test) {
    uint32_t mask = 0x1;

    for (int i = 0; i < (int)sizeof(*test) * 8; i++) {
        if (*test & mask) {
            *test &= ~mask;
            return i;
        }
        mask <<= 1;
    }

    // no one bits found
    return -1;
}

// Checks a co-ordinate for in bounds status -RAK-
bool in_bounds(int y, int x) {
    return (y > 0 && y < cur_height - 1 && x > 0 && x < cur_width - 1);
}

// Calculates current boundaries -RAK-
void panel_bounds() {
    panel_row_min = panel_row * (SCREEN_HEIGHT / 2);
    panel_row_max = panel_row_min + SCREEN_HEIGHT - 1;
    panel_row_prt = panel_row_min - 1;
    panel_col_min = panel_col * (SCREEN_WIDTH / 2);
    panel_col_max = panel_col_min + SCREEN_WIDTH - 1;
    panel_col_prt = panel_col_min - 13;
}

// Given an row (y) and col (x), this routine detects -RAK-
// when a move off the screen has occurred and figures new borders.
// Force, forces the panel bounds to be recalculated, useful for 'W'here.
int get_panel(int y, int x, int force) {
    bool panel;

    int prow = panel_row;
    int pcol = panel_col;
    if (force || (y < panel_row_min + 2) || (y > panel_row_max - 2)) {
        prow = ((y - SCREEN_HEIGHT / 4) / (SCREEN_HEIGHT / 2));
        if (prow > max_panel_rows) {
            prow = max_panel_rows;
        } else if (prow < 0) {
            prow = 0;
        }
    }
    if (force || (x < panel_col_min + 3) || (x > panel_col_max - 3)) {
        pcol = ((x - SCREEN_WIDTH / 4) / (SCREEN_WIDTH / 2));
        if (pcol > max_panel_cols) {
            pcol = max_panel_cols;
        } else if (pcol < 0) {
            pcol = 0;
        }
    }
    if ((prow != panel_row) || (pcol != panel_col)) {
        panel_row = prow;
        panel_col = pcol;
        panel_bounds();
        panel = true;

        // stop movement if any
        if (find_bound) {
            end_find();
        }
    } else {
        panel = false;
    }
    return panel;
}

// Tests a given point to see if it is within the screen -RAK-
// boundaries.
bool panel_contains(int y, int x) {
    return (y >= panel_row_min && y <= panel_row_max && x >= panel_col_min && x <= panel_col_max);
}

// Distance between two points -RAK-
int distance(int y1, int x1, int y2, int x2) {
    int dy = y1 - y2;
    if (dy < 0) {
        dy = -dy;
    }

    int dx = x1 - x2;
    if (dx < 0) {
        dx = -dx;
    }

    return ((((dy + dx) << 1) - (dy > dx ? dx : dy)) >> 1);
}

// Checks points north, south, east, and west for a wall -RAK-
// note that y,x is always in_bounds(), i.e. 0 < y < cur_height-1,
// and 0 < x < cur_width-1
int next_to_walls(int y, int x) {
    int i = 0;
    cave_type *c_ptr = &cave[y - 1][x];

    if (c_ptr->fval >= MIN_CAVE_WALL) {
        i++;
    }
    c_ptr = &cave[y + 1][x];
    if (c_ptr->fval >= MIN_CAVE_WALL) {
        i++;
    }
    c_ptr = &cave[y][x - 1];
    if (c_ptr->fval >= MIN_CAVE_WALL) {
        i++;
    }
    c_ptr = &cave[y][x + 1];
    if (c_ptr->fval >= MIN_CAVE_WALL) {
        i++;
    }

    return i;
}

// Checks all adjacent spots for corridors -RAK-
// note that y, x is always in_bounds(), hence no need to check that
// j, k are in_bounds(), even if they are 0 or cur_x-1 is still works
int next_to_corr(int y, int x) {
    int i = 0;

    for (int j = y - 1; j <= (y + 1); j++) {
        for (int k = x - 1; k <= (x + 1); k++) {
            cave_type *c_ptr = &cave[j][k];

            // should fail if there is already a door present
            if (c_ptr->fval == CORR_FLOOR &&
                (c_ptr->tptr == 0 || t_list[c_ptr->tptr].tval < TV_MIN_DOORS)) {
                i++;
            }
        }
    }

    return i;
}

// generates damage for 2d6 style dice rolls
int damroll(int num, int sides) {
    int sum = 0;
    for (int i = 0; i < num; i++) {
        sum += randint(sides);
    }
    return sum;
}

int pdamroll(uint8_t *array) {
    return damroll((int)array[0], (int)array[1]);
}

// A simple, fast, integer-based line-of-sight algorithm.  By Joseph Hall,
// 4116 Brewster Drive, Raleigh NC 27606.  Email to jnh@ecemwl.ncsu.edu.
//
// Returns true if a line of sight can be traced from x0, y0 to x1, y1.
//
// The LOS begins at the center of the tile [x0, y0] and ends at the center of
// the tile [x1, y1].  If los() is to return true, all of the tiles this line
// passes through must be transparent, WITH THE EXCEPTIONS of the starting and
// ending tiles.
//
// We don't consider the line to be "passing through" a tile if it only passes
// across one corner of that tile.

// Because this function uses (short) ints for all calculations, overflow may
// occur if deltaX and deltaY exceed 90.
bool los(int fromY, int fromX, int toY, int toX) {
    int deltaX = toX - fromX;
    int deltaY = toY - fromY;

    // Adjacent?
    if ((deltaX < 2) && (deltaX > -2) && (deltaY < 2) && (deltaY > -2)) {
        return true;
    }

    // Handle the cases where deltaX or deltaY == 0.
    if (deltaX == 0) {
        if (deltaY < 0) {
            int tmp = fromY;
            fromY = toY;
            toY = tmp;
        }

        for (int p_y = fromY + 1; p_y < toY; p_y++) {
            if (cave[p_y][fromX].fval >= MIN_CLOSED_SPACE) {
                return false;
            }
        }

        return true;
    }

    if (deltaY == 0) {
        if (deltaX < 0) {
            int tmp = fromX;
            fromX = toX;
            toX = tmp;
        }

        for (int px = fromX + 1; px < toX; px++) {
            if (cave[fromY][px].fval >= MIN_CLOSED_SPACE) {
                return false;
            }
        }

        return true;
    }

    // Now, we've eliminated all the degenerate cases.
    // In the computations below, dy (or dx) and m are multiplied by a scale factor,
    // scale = abs(deltaX * deltaY * 2), so that we can use integer arithmetic.
    {
        int px,     // x position
            p_y,    // y position
            scale2; // above scale factor / 2
        int scale,  // above scale factor
            xSign,  // sign of deltaX
            ySign,  // sign of deltaY
            m;      // slope or 1/slope of LOS

        scale2 = abs(deltaX * deltaY);
        scale = scale2 << 1;
        xSign = (deltaX < 0) ? -1 : 1;
        ySign = (deltaY < 0) ? -1 : 1;

        // Travel from one end of the line to the other, oriented along the longer axis.

        if (abs(deltaX) >= abs(deltaY)) {
            int dy; // "fractional" y position

            // We start at the border between the first and second tiles, where
            // the y offset = .5 * slope.  Remember the scale factor.
            // We have:     m = deltaY / deltaX * 2 * (deltaY * deltaX)
            //                = 2 * deltaY * deltaY.

            dy = deltaY * deltaY;
            m = dy << 1;
            px = fromX + xSign;

            // Consider the special case where slope == 1.
            if (dy == scale2) {
                p_y = fromY + ySign;
                dy -= scale;
            } else {
                p_y = fromY;
            }

            while (toX - px) {
                if (cave[p_y][px].fval >= MIN_CLOSED_SPACE) {
                    return false;
                }

                dy += m;
                if (dy < scale2) {
                    px += xSign;
                } else if (dy > scale2) {
                    p_y += ySign;
                    if (cave[p_y][px].fval >= MIN_CLOSED_SPACE) {
                        return false;
                    }
                    px += xSign;
                    dy -= scale;
                } else {
                    // This is the case, dy == scale2, where the LOS
                    // exactly meets the corner of a tile.
                    px += xSign;
                    p_y += ySign;
                    dy -= scale;
                }
            }
            return true;
        } else {
            int dx; // "fractional" x position
            dx = deltaX * deltaX;
            m = dx << 1;

            p_y = fromY + ySign;
            if (dx == scale2) {
                px = fromX + xSign;
                dx -= scale;
            } else {
                px = fromX;
            }

            while (toY - p_y) {
                if (cave[p_y][px].fval >= MIN_CLOSED_SPACE) {
                    return false;
                }
                dx += m;
                if (dx < scale2) {
                    p_y += ySign;
                } else if (dx > scale2) {
                    px += xSign;
                    if (cave[p_y][px].fval >= MIN_CLOSED_SPACE) {
                        return false;
                    }
                    p_y += ySign;
                    dx -= scale;
                } else {
                    px += xSign;
                    p_y += ySign;
                    dx -= scale;
                }
            }
            return true;
        }
    }
}

// Returns symbol for given row, column -RAK-
uint8_t loc_symbol(int y, int x) {
    cave_type *cave_ptr = &cave[y][x];

    if ((cave_ptr->cptr == 1) && (!find_flag || find_prself)) {
        return '@';
    }

    struct player_type::flags *f_ptr = &py.flags;

    if (f_ptr->status & PY_BLIND) {
        return ' ';
    }

    if ((f_ptr->image > 0) && (randint(12) == 1)) {
        return (uint8_t) (randint(95) + 31);
    }

    if ((cave_ptr->cptr > 1) && (m_list[cave_ptr->cptr].ml)) {
        return c_list[m_list[cave_ptr->cptr].mptr].cchar;
    }

    if (!cave_ptr->pl && !cave_ptr->tl && !cave_ptr->fm) {
        return ' ';
    }

    if ((cave_ptr->tptr != 0) && (t_list[cave_ptr->tptr].tval != TV_INVIS_TRAP)) {
        return t_list[cave_ptr->tptr].tchar;
    }

    if (cave_ptr->fval <= MAX_CAVE_FLOOR) {
        return '.';
    }

    if (cave_ptr->fval == GRANITE_WALL || cave_ptr->fval == BOUNDARY_WALL || !highlight_seams) {
        return '#';
    }

    // Originally set highlight bit, but that is not portable,
    // now use the percent sign instead.
    return '%';
}

// Tests a spot for light or field mark status -RAK-
bool test_light(int y, int x) {
    cave_type *cave_ptr = &cave[y][x];

    return (cave_ptr->pl || cave_ptr->tl || cave_ptr->fm);
}

// Prints the map of the dungeon -RAK-
void prt_map() {
    int k = 0;

    // Top to bottom
    for (int i = panel_row_min; i <= panel_row_max; i++) {
        k++;
        erase_line(k, 13);

        // Left to right
        for (int j = panel_col_min; j <= panel_col_max; j++) {
            uint8_t tmp = loc_symbol(i, j);
            if (tmp != ' ') {
                print(tmp, i, j);
            }
        }
    }
}

// Compact monsters -RAK-
// Return true if any monsters were deleted, false if could not delete any monsters.
bool compact_monsters() {
    msg_print("Compacting monsters...");

    int cur_dis = 66;
    bool delete_any = false;

    do {
        for (int i = mfptr - 1; i >= MIN_MONIX; i--) {
            monster_type *mon_ptr = &m_list[i];
            if ((cur_dis < mon_ptr->cdis) && (randint(3) == 1)) {
                // Never compact away the Balrog!!
                if (c_list[mon_ptr->mptr].cmove & CM_WIN) {
                    ; // Do nothing
                } else if (hack_monptr < i) {
                    // in case this is called from within creatures(), this is a horrible
                    // hack, the m_list/creatures() code needs to be rewritten.
                    delete_monster(i);
                    delete_any = true;
                } else {
                    // fix1_delete_monster() does not decrement mfptr,
                    // so don't set delete_any if this was called.
                    fix1_delete_monster(i);
                }
            }
        }

        if (!delete_any) {
            cur_dis -= 6;
            // Can't delete any monsters, return failure.
            if (cur_dis < 0) {
                return false;
            }
        }
    } while (!delete_any);

    return true;
}

// Add to the players food time -RAK-
void add_food(int num) {
    struct player_type::flags *p_ptr = &py.flags;
    if (p_ptr->food < 0) {
        p_ptr->food = 0;
    }

    p_ptr->food += num;
    if (p_ptr->food > PLAYER_FOOD_MAX) {
        msg_print("You are bloated from overeating.");

        // Calculate how much of num is responsible for the bloating. Give the
        // player food credit for 1/50, and slow him for that many turns also.
        int extra = p_ptr->food - PLAYER_FOOD_MAX;
        if (extra > num) {
            extra = num;
        }
        int penalty = extra / 50;

        p_ptr->slow += penalty;
        if (extra == num) {
            p_ptr->food = (int16_t) (p_ptr->food - num + penalty);
        } else {
            p_ptr->food = (int16_t) (PLAYER_FOOD_MAX + penalty);
        }
    } else if (p_ptr->food > PLAYER_FOOD_FULL) {
        msg_print("You are full.");
    }
}

// Returns a pointer to next free space -RAK-
// Returns -1 if could not allocate a monster.
int popm() {
    if (mfptr == MAX_MALLOC) {
        if (!compact_monsters()) {
            return -1;
        }
    }
    return mfptr++;
}

// Gives Max hit points -RAK-
int max_hp(uint8_t *array) {
    return (array[0] * array[1]);
}

// Places a monster at given location -RAK-
bool place_monster(int y, int x, int z, int slp) {
    int cur_pos = popm();
    if (cur_pos == -1) {
        return false;
    }

    monster_type *mon_ptr = &m_list[cur_pos];
    mon_ptr->fy = (uint8_t) y;
    mon_ptr->fx = (uint8_t) x;
    mon_ptr->mptr = (uint16_t) z;

    if (c_list[z].cdefense & CD_MAX_HP) {
        mon_ptr->hp = (int16_t) max_hp(c_list[z].hd);
    } else {
        mon_ptr->hp = (int16_t) pdamroll(c_list[z].hd);
    }

    // the c_list speed value is 10 greater, so that it can be a uint8_t
    mon_ptr->cspeed = (int16_t) (c_list[z].speed - 10 + py.flags.speed);
    mon_ptr->stunned = 0;
    mon_ptr->cdis = (uint8_t) distance(char_row, char_col, y, x);
    mon_ptr->ml = false;
    cave[y][x].cptr = (uint8_t) cur_pos;
    if (slp) {
        if (c_list[z].sleep == 0) {
            mon_ptr->csleep = 0;
        } else {
            mon_ptr->csleep = (int16_t) ((c_list[z].sleep * 2) + randint((int)c_list[z].sleep * 10));
        }
    } else {
        mon_ptr->csleep = 0;
    }
    return true;
}

// Places a monster at given location -RAK-
void place_win_monster() {
    if (total_winner) {
        return;
    }

    int cur_pos = popm();

    // Check for case where could not allocate space for
    // the win monster, this should never happen.
    if (cur_pos == -1) {
        abort();
    }

    monster_type *mon_ptr = &m_list[cur_pos];

    int y, x;
    do {
        y = randint(cur_height - 2);
        x = randint(cur_width - 2);
    } while ((cave[y][x].fval >= MIN_CLOSED_SPACE) ||
             (cave[y][x].cptr != 0) || (cave[y][x].tptr != 0) ||
             (distance(y, x, char_row, char_col) <= MAX_SIGHT));

    mon_ptr->fy = (uint8_t) y;
    mon_ptr->fx = (uint8_t) x;
    mon_ptr->mptr = (uint16_t) (randint(WIN_MON_TOT) - 1 + m_level[MAX_MONS_LEVEL]);

    if (c_list[mon_ptr->mptr].cdefense & CD_MAX_HP) {
        mon_ptr->hp = (int16_t) max_hp(c_list[mon_ptr->mptr].hd);
    } else {
        mon_ptr->hp = (int16_t) pdamroll(c_list[mon_ptr->mptr].hd);
    }

    // the c_list speed value is 10 greater, so that it can be a uint8_t
    mon_ptr->cspeed = (int16_t) (c_list[mon_ptr->mptr].speed - 10 + py.flags.speed);
    mon_ptr->stunned = 0;
    mon_ptr->cdis = (uint8_t) distance(char_row, char_col, y, x);
    cave[y][x].cptr = (uint8_t) cur_pos;
    mon_ptr->csleep = 0;
}

// Return a monster suitable to be placed at a given level. This
// makes high level monsters (up to the given level) slightly more
// common than low level monsters at any given level. -CJS-
int get_mons_num(int level) {
    if (level == 0) {
        return randint(m_level[0]) - 1;
    }

    if (level > MAX_MONS_LEVEL) {
        level = MAX_MONS_LEVEL;
    }

    int i;

    if (randint(MON_NASTY) == 1) {
        i = randnor(0, 4);
        level = level + abs(i) + 1;
        if (level > MAX_MONS_LEVEL) {
            level = MAX_MONS_LEVEL;
        }
    } else {
        // This code has been added to make it slightly more likely to get
        // the higher level monsters. Originally a uniform distribution over
        // all monsters of level less than or equal to the dungeon level.
        // This distribution makes a level n monster occur approx 2/n% of the
        // time on level n, and 1/n*n% are 1st level.
        int num = m_level[level] - m_level[0];
        i = randint(num) - 1;
        int j = randint(num) - 1;
        if (j > i) {
            i = j;
        }
        level = c_list[i + m_level[0]].level;
    }

    return randint(m_level[level] - m_level[level - 1]) - 1 + m_level[level - 1];
}

// Allocates a random monster -RAK-
void alloc_monster(int num, int dis, int slp) {
    int y, x;

    for (int i = 0; i < num; i++) {
        do {
            y = randint(cur_height - 2);
            x = randint(cur_width - 2);
        } while (cave[y][x].fval >= MIN_CLOSED_SPACE || (cave[y][x].cptr != 0) || (distance(y, x, char_row, char_col) <= dis));

        int l = get_mons_num(dun_level);

        // Dragons are always created sleeping here,
        // so as to give the player a sporting chance.
        if (c_list[l].cchar == 'd' || c_list[l].cchar == 'D') {
            slp = true;
        }

        // Place_monster() should always return true here.
        // It does not matter if it fails though.
        (void)place_monster(y, x, l, slp);
    }
}

// Places creature adjacent to given location -RAK-
bool summon_monster(int *y, int *x, int slp) {
    int i = 0;
    int l = get_mons_num(dun_level + MON_SUMMON_ADJ);

    bool summon = false;

    do {
        int j = *y - 2 + randint(3);
        int k = *x - 2 + randint(3);
        if (in_bounds(j, k)) {
            cave_type *cave_ptr = &cave[j][k];
            if (cave_ptr->fval <= MAX_OPEN_SPACE && (cave_ptr->cptr == 0)) {
                // Place_monster() should always return true here.
                if (!place_monster(j, k, l, slp)) {
                    return false;
                }
                summon = true;
                i = 9;
                *y = j;
                *x = k;
            }
        }
        i++;
    } while (i <= 9);

    return summon;
}

// Places undead adjacent to given location -RAK-
bool summon_undead(int *y, int *x) {
    int m;
    int i = 0;
    int l = m_level[MAX_MONS_LEVEL];

    bool summon = false;

    do {
        m = randint(l) - 1;
        int ctr = 0;
        do {
            if (c_list[m].cdefense & CD_UNDEAD) {
                ctr = 20;
                l = 0;
            } else {
                m++;
                if (m > l) {
                    ctr = 20;
                } else {
                    ctr++;
                }
            }
        } while (ctr <= 19);
    } while (l != 0);

    do {
        int j = *y - 2 + randint(3);
        int k = *x - 2 + randint(3);
        if (in_bounds(j, k)) {
            cave_type *cave_ptr = &cave[j][k];
            if (cave_ptr->fval <= MAX_OPEN_SPACE && (cave_ptr->cptr == 0)) {
                // Place_monster() should always return true here.
                if (!place_monster(j, k, m, false)) {
                    return false;
                }
                summon = true;
                i = 9;
                *y = j;
                *x = k;
            }
        }
        i++;
    } while (i <= 9);

    return summon;
}

// If too many objects on floor level, delete some of them-RAK-
static void compact_objects() {
    msg_print("Compacting objects...");

    int ctr = 0;
    int cur_dis = 66;

    do {
        for (int i = 0; i < cur_height; i++) {
            for (int j = 0; j < cur_width; j++) {
                int chance;

                cave_type *cave_ptr = &cave[i][j];
                if ((cave_ptr->tptr != 0) &&
                    (distance(i, j, char_row, char_col) > cur_dis)) {
                    switch (t_list[cave_ptr->tptr].tval) {
                    case TV_VIS_TRAP:
                        chance = 15;
                        break;
                    case TV_INVIS_TRAP:
                    case TV_RUBBLE:
                    case TV_OPEN_DOOR:
                    case TV_CLOSED_DOOR:
                        chance = 5;
                        break;
                    case TV_UP_STAIR:
                    case TV_DOWN_STAIR:
                    case TV_STORE_DOOR:
                        // Stairs, don't delete them.
                        // Shop doors, don't delete them.
                        chance = 0;
                        break;
                    case TV_SECRET_DOOR: // secret doors
                        chance = 3;
                        break;
                    default:
                        chance = 10;
                    }
                    if (randint(100) <= chance) {
                        (void)delete_object(i, j);
                        ctr++;
                    }
                }
            }
        }
        if (ctr == 0) {
            cur_dis -= 6;
        }
    } while (ctr <= 0);

    if (cur_dis < 66) {
        prt_map();
    }
}

// Gives pointer to next free space -RAK-
int popt() {
    if (tcptr == MAX_TALLOC) {
        compact_objects();
    }
    return tcptr++;
}

// Pushs a record back onto free space list -RAK-
// Delete_object() should always be called instead, unless the object
// in question is not in the dungeon, e.g. in store1.c and files.c
void pusht(uint8_t x) {
    if (x != tcptr - 1) {
        t_list[x] = t_list[tcptr - 1];

        // must change the tptr in the cave of the object just moved
        for (int i = 0; i < cur_height; i++) {
            for (int j = 0; j < cur_width; j++) {
                if (cave[i][j].tptr == tcptr - 1) {
                    cave[i][j].tptr = x;
                }
            }
        }
    }
    tcptr--;
    invcopy(&t_list[tcptr], OBJ_NOTHING);
}

// Should the object be enchanted -RAK-
bool magik(int chance) {
    return (randint(100) <= chance);
}

// Enchant a bonus based on degree desired -RAK-
int m_bonus(int base, int max_std, int level) {
    int stand_dev = (OBJ_STD_ADJ * level / 100) + OBJ_STD_MIN;

    // Check for level > max_std since that may have generated an overflow.
    if (stand_dev > max_std || level > max_std) {
        stand_dev = max_std;
    }

    // abs may be a macro, don't call it with randnor as a parameter
    int tmp = randnor(0, stand_dev);
    int x = (abs(tmp) / 10) + base;

    if (x < base) {
        return base;
    }

    return x;
}
