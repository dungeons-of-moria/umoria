/* source/variable.c: Global variables
 *
 * Copyright (C) 1989-2008 James E. Wilson, Robert A. Koeneke,
 *                         David J. Grabiner
 *
 * This file is part of Umoria.
 *
 * Umoria is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Umoria is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Umoria.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "standard_library.h"

#include "config.h"
#include "constant.h"
#include "types.h"

char *copyright[17] = {
    "Copyright (C) 1989-2008 James E. Wilson, Robert A. Koeneke, ",
    "                        David J. Grabiner",
    "",
    "This file is part of Umoria.",
    "",
    "Umoria is free software; you can redistribute it and/or modify ",
    "it under the terms of the GNU General Public License as published by",
    "the Free Software Foundation, either version 3 of the License, or",
    "(at your option) any later version.",
    "",
    "Umoria is distributed in the hope that it will be useful,",
    "but WITHOUT ANY WARRANTY; without even the implied warranty of ",
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the",
    "GNU General Public License for more details.",
    "",
    "You should have received a copy of the GNU General Public License ",
    "along with Umoria.  If not, see <http://www.gnu.org/licenses/>."};

/* Save the store's last increment value. */
int16_t last_store_inc;

/* a horrible hack: needed because compact_monster() can be called from
   creatures() via summon_monster() and place_monster() */
int hack_monptr                 = -1;

int weapon_heavy                = false;
int pack_heavy                  = false;
vtype died_from;
int32_t birth_date;

vtype savefile; /* The savefile to use. */

int16_t total_winner            = false;
int32_t max_score               = 0;
int character_generated         = 0;        /* don't save score until char gen finished */
int character_saved             = 0;        /* prevents save on kill after save_char() */
FILE *highscore_fp;                         /* File pointer to high score file */
uint32_t randes_seed;                       /* for restarting randes_state */
uint32_t town_seed;                         /* for restarting town_seed */
int16_t cur_height, cur_width;              /* Cur dungeon size */
int16_t dun_level               = 0;        /* Cur dungeon level */
int16_t missile_ctr             = 0;        /* Counter for missiles */
int msg_flag;                               /* Set with first msg */
vtype old_msg[MAX_SAVE_MSG];                /* Last message */
int16_t last_msg                = 0;        /* Where last is held */
int death                       = false;    /* True if died */
int find_flag;                              /* Used in MORIA for .(dir) */
int free_turn_flag;                         /* Used in MORIA, do not move creatures */
int command_count;                          /* Gives repetition of commands. -CJS- */
int default_dir                 = false;    /* Use last direction for repeated command */
int32_t turn                    = -1;       /* Cur turn of game */
int wizard                      = false;    /* Wizard flag */
int to_be_wizard                = false;    /* used during startup, when -w option used */
int16_t panic_save              = false;    /* this is true if playing from a panic save */
int16_t noscore                 = false;    /* Don't log the game. -CJS- */

int rogue_like_commands; /* set in config.h/main.c */

/* options set via the '=' command */
int find_cut                    = true;
int find_examine                = true;
int find_bound                  = false;
int find_prself                 = false;
int prompt_carry_flag           = false;
int show_weight_flag            = false;
int highlight_seams             = false;
int find_ignore_doors           = false;
int sound_beep_flag             = true;
int display_counts              = true;

char doing_inven                = false;  /* Track inventory commands. -CJS- */
int screen_change               = false;  /* Track screen updates for inven_commands. */
char last_command               = ' ';    /* Memory of previous command. */

/* these used to be in dungeon.c */
int new_level_flag;                       /* Next level when true */
int teleport_flag;                        /* Handle teleport traps */
int player_light;                         /* Player carrying light */
int eof_flag                    = false;  /* Used to signal EOF/HANGUP condition */
int light_flag                  = false;  /* Track if temporary light about player. */

int wait_for_more               = false;  /* used when ^C hit during -more- prompt */
int closing_flag                = false;  /* Used for closing */

/*  Following are calculated from max dungeon sizes */
int16_t max_panel_rows, max_panel_cols;
int panel_row, panel_col;
int panel_row_min, panel_row_max;
int panel_col_min, panel_col_max;
int panel_col_prt, panel_row_prt;

cave_type cave[MAX_HEIGHT][MAX_WIDTH];

recall_type c_recall[MAX_CREATURES]; /* Monster memories */
