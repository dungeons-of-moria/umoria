// src/save.c: save and restore games and monster memory info
//
// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke,
//                         David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

#include "headers.h"
#include "externs.h"
#include "version.h"

// For debugging the save file code on systems with broken compilers.
#define DEBUG(x)

DEBUG(static FILE *logfile);

static bool sv_write();

static void wr_bool(bool c);

static void wr_byte(uint8_t);

static void wr_short(uint16_t);

static void wr_long(uint32_t);

static void wr_bytes(uint8_t *, int);

static void wr_string(char *);

static void wr_shorts(uint16_t *, int);

static void wr_item(Inventory_t *);

static void wr_monster(Monster_t *);

static bool rd_bool();

static void rd_byte(uint8_t *);

static void rd_short(uint16_t *);

static void rd_long(uint32_t *);

static void rd_bytes(uint8_t *, int);

static void rd_string(char *);

static void rd_shorts(uint16_t *, int);

static void rd_item(Inventory_t *);

static void rd_monster(Monster_t *);

// these are used for the save file, to avoid having to pass them to every procedure
static FILE *fileptr;
static uint8_t xor_byte;
static int from_savefile;   // can overwrite old save file when save
static uint32_t start_time; // time that play started

// This save package was brought to by                -JWT-
// and                                                -RAK-
// and has been completely rewritten for UNIX by      -JEW-
// and has been completely rewritten again by         -CJS-
// and completely rewritten again! for portability by -JEW-

static bool sv_write() {
    // clear the death flag when creating a HANGUP save file,
    // so that player can see tombstone when restart
    if (eof_flag) {
        death = false;
    }

    uint32_t l = 0;

    if (run_cut_corners) {
        l |= 0x1;
    }
    if (run_examine_corners) {
        l |= 0x2;
    }
    if (run_print_self) {
        l |= 0x4;
    }
    if (find_bound) {
        l |= 0x8;
    }
    if (prompt_carry_flag) {
        l |= 0x10;
    }
    if (rogue_like_commands) {
        l |= 0x20;
    }
    if (show_weight_flag) {
        l |= 0x40;
    }
    if (highlight_seams) {
        l |= 0x80;
    }
    if (run_ignore_doors) {
        l |= 0x100;
    }
    if (sound_beep_flag) {
        l |= 0x200;
    }
    if (display_counts) {
        l |= 0x400;
    }
    if (death) {
        // Sign bit
        l |= 0x80000000L;
    }
    if (total_winner) {
        l |= 0x40000000L;
    }

    for (int i = 0; i < MAX_CREATURES; i++) {
        Recall_t *r_ptr = &creature_recall[i];

        if (r_ptr->r_cmove || r_ptr->r_cdefense || r_ptr->r_kills ||
            r_ptr->r_spells || r_ptr->r_deaths || r_ptr->r_attacks[0] ||
            r_ptr->r_attacks[1] || r_ptr->r_attacks[2] || r_ptr->r_attacks[3]) {
            wr_short((uint16_t) i);
            wr_long(r_ptr->r_cmove);
            wr_long(r_ptr->r_spells);
            wr_short(r_ptr->r_kills);
            wr_short(r_ptr->r_deaths);
            wr_short(r_ptr->r_cdefense);
            wr_byte(r_ptr->r_wake);
            wr_byte(r_ptr->r_ignore);
            wr_bytes(r_ptr->r_attacks, MAX_MON_NATTACK);
        }
    }

    // sentinel to indicate no more monster info
    wr_short((uint16_t) 0xFFFF);

    wr_long(l);

    wr_string(py.misc.name);
    wr_bool(py.misc.male);
    wr_long((uint32_t) py.misc.au);
    wr_long((uint32_t) py.misc.max_exp);
    wr_long((uint32_t) py.misc.exp);
    wr_short(py.misc.exp_frac);
    wr_short(py.misc.age);
    wr_short(py.misc.ht);
    wr_short(py.misc.wt);
    wr_short(py.misc.lev);
    wr_short(py.misc.max_dlv);
    wr_short((uint16_t) py.misc.srh);
    wr_short((uint16_t) py.misc.fos);
    wr_short((uint16_t) py.misc.bth);
    wr_short((uint16_t) py.misc.bthb);
    wr_short((uint16_t) py.misc.mana);
    wr_short((uint16_t) py.misc.mhp);
    wr_short((uint16_t) py.misc.ptohit);
    wr_short((uint16_t) py.misc.ptodam);
    wr_short((uint16_t) py.misc.pac);
    wr_short((uint16_t) py.misc.ptoac);
    wr_short((uint16_t) py.misc.dis_th);
    wr_short((uint16_t) py.misc.dis_td);
    wr_short((uint16_t) py.misc.dis_ac);
    wr_short((uint16_t) py.misc.dis_tac);
    wr_short((uint16_t) py.misc.disarm);
    wr_short((uint16_t) py.misc.save);
    wr_short((uint16_t) py.misc.sc);
    wr_short((uint16_t) py.misc.stl);
    wr_byte(py.misc.pclass);
    wr_byte(py.misc.prace);
    wr_byte(py.misc.hitdie);
    wr_byte(py.misc.expfact);
    wr_short((uint16_t) py.misc.cmana);
    wr_short(py.misc.cmana_frac);
    wr_short((uint16_t) py.misc.chp);
    wr_short(py.misc.chp_frac);
    for (int i = 0; i < 4; i++) {
        wr_string(py.misc.history[i]);
    }

    wr_bytes(py.stats.max_stat, 6);
    wr_bytes(py.stats.cur_stat, 6);
    wr_shorts((uint16_t *) py.stats.mod_stat, 6);
    wr_bytes(py.stats.use_stat, 6);

    wr_long(py.flags.status);
    wr_short((uint16_t) py.flags.rest);
    wr_short((uint16_t) py.flags.blind);
    wr_short((uint16_t) py.flags.paralysis);
    wr_short((uint16_t) py.flags.confused);
    wr_short((uint16_t) py.flags.food);
    wr_short((uint16_t) py.flags.food_digested);
    wr_short((uint16_t) py.flags.protection);
    wr_short((uint16_t) py.flags.speed);
    wr_short((uint16_t) py.flags.fast);
    wr_short((uint16_t) py.flags.slow);
    wr_short((uint16_t) py.flags.afraid);
    wr_short((uint16_t) py.flags.poisoned);
    wr_short((uint16_t) py.flags.image);
    wr_short((uint16_t) py.flags.protevil);
    wr_short((uint16_t) py.flags.invuln);
    wr_short((uint16_t) py.flags.hero);
    wr_short((uint16_t) py.flags.shero);
    wr_short((uint16_t) py.flags.blessed);
    wr_short((uint16_t) py.flags.resist_heat);
    wr_short((uint16_t) py.flags.resist_cold);
    wr_short((uint16_t) py.flags.detect_inv);
    wr_short((uint16_t) py.flags.word_recall);
    wr_short((uint16_t) py.flags.see_infra);
    wr_short((uint16_t) py.flags.tim_infra);
    wr_bool(py.flags.see_inv);
    wr_bool(py.flags.teleport);
    wr_bool(py.flags.free_act);
    wr_bool(py.flags.slow_digest);
    wr_bool(py.flags.aggravate);
    wr_bool(py.flags.fire_resist);
    wr_bool(py.flags.cold_resist);
    wr_bool(py.flags.acid_resist);
    wr_bool(py.flags.regenerate);
    wr_bool(py.flags.lght_resist);
    wr_bool(py.flags.ffall);
    wr_bool(py.flags.sustain_str);
    wr_bool(py.flags.sustain_int);
    wr_bool(py.flags.sustain_wis);
    wr_bool(py.flags.sustain_con);
    wr_bool(py.flags.sustain_dex);
    wr_bool(py.flags.sustain_chr);
    wr_bool(py.flags.confuse_monster);
    wr_byte(py.flags.new_spells);

    wr_short((uint16_t) missile_ctr);
    wr_long((uint32_t) turn);
    wr_short((uint16_t) inven_ctr);
    for (int i = 0; i < inven_ctr; i++) {
        wr_item(&inventory[i]);
    }
    for (int i = INVEN_WIELD; i < INVEN_ARRAY_SIZE; i++) {
        wr_item(&inventory[i]);
    }
    wr_short((uint16_t) inven_weight);
    wr_short((uint16_t) equip_ctr);
    wr_long(spell_learned);
    wr_long(spell_worked);
    wr_long(spell_forgotten);
    wr_bytes(spell_order, 32);
    wr_bytes(objects_identified, OBJECT_IDENT_SIZE);
    wr_long(randes_seed);
    wr_long(town_seed);
    wr_short((uint16_t) last_msg);
    for (int i = 0; i < MAX_SAVE_MSG; i++) {
        wr_string(old_msgs[i]);
    }

    // this indicates 'cheating' if it is a one
    wr_short((uint16_t) panic_save);
    wr_short((uint16_t) total_winner);
    wr_short((uint16_t) noscore);
    wr_shorts(player_hp, MAX_PLAYER_LEVEL);

    for (int i = 0; i < MAX_STORES; i++) {
        Store_t *st_ptr = &stores[i];

        wr_long((uint32_t) st_ptr->store_open);
        wr_short((uint16_t) st_ptr->insult_cur);
        wr_byte(st_ptr->owner);
        wr_byte(st_ptr->store_ctr);
        wr_short(st_ptr->good_buy);
        wr_short(st_ptr->bad_buy);
        for (int j = 0; j < st_ptr->store_ctr; j++) {
            wr_long((uint32_t) st_ptr->store_inven[j].scost);
            wr_item(&st_ptr->store_inven[j].sitem);
        }
    }

    // save the current time in the save file
    l = (uint32_t) time((time_t *) 0);

    if (l < start_time) {
        // someone is messing with the clock!,
        // assume that we have been playing for 1 day
        l = (uint32_t) (start_time + 86400L);
    }
    wr_long(l);

    // starting with 5.2, put died_from string in save file
    wr_string(died_from);

    // starting with 5.2.2, put the max_score in the save file
    l = (uint32_t) (total_points());
    wr_long(l);

    // starting with 5.2.2, put the birth_date in the save file
    wr_long((uint32_t) birth_date);

    // only level specific info follows, this allows characters to be
    // resurrected, the dungeon level info is not needed for a resurrection
    if (death) {
        return !(ferror(fileptr) || fflush(fileptr) == EOF);
    }

    wr_short((uint16_t) dun_level);
    wr_short((uint16_t) char_row);
    wr_short((uint16_t) char_col);
    wr_short((uint16_t) mon_tot_mult);
    wr_short((uint16_t) cur_height);
    wr_short((uint16_t) cur_width);
    wr_short((uint16_t) max_panel_rows);
    wr_short((uint16_t) max_panel_cols);

    for (int i = 0; i < MAX_HEIGHT; i++) {
        for (int j = 0; j < MAX_WIDTH; j++) {
            Cave_t *c_ptr = &cave[i][j];
            if (c_ptr->cptr != 0) {
                wr_byte((uint8_t) i);
                wr_byte((uint8_t) j);
                wr_byte(c_ptr->cptr);
            }
        }
    }

    // marks end of cptr info
    wr_byte((uint8_t) 0xFF);

    for (int i = 0; i < MAX_HEIGHT; i++) {
        for (int j = 0; j < MAX_WIDTH; j++) {
            Cave_t *c_ptr = &cave[i][j];
            if (c_ptr->tptr != 0) {
                wr_byte((uint8_t) i);
                wr_byte((uint8_t) j);
                wr_byte(c_ptr->tptr);
            }
        }
    }

    // marks end of tptr info
    wr_byte((uint8_t) 0xFF);

    // must set counter to zero, note that code may write out two bytes unnecessarily
    int count = 0;
    uint8_t prev_char = 0;

    for (int i = 0; i < MAX_HEIGHT; i++) {
        for (int j = 0; j < MAX_WIDTH; j++) {
            Cave_t *c_ptr = &cave[i][j];

            uint8_t char_tmp = (uint8_t) (c_ptr->fval | (c_ptr->lr << 4) | (c_ptr->fm << 5) | (c_ptr->pl << 6) | (c_ptr->tl << 7));

            if (char_tmp != prev_char || count == MAX_UCHAR) {
                wr_byte((uint8_t) count);
                wr_byte(prev_char);
                prev_char = char_tmp;
                count = 1;
            } else {
                count++;
            }
        }
    }

    // save last entry
    wr_byte((uint8_t) count);
    wr_byte(prev_char);

    wr_short((uint16_t) tcptr);
    for (int i = MIN_TRIX; i < tcptr; i++) {
        wr_item(&treasure_list[i]);
    }
    wr_short((uint16_t) mfptr);
    for (int i = MIN_MONIX; i < mfptr; i++) {
        wr_monster(&monsters_list[i]);
    }

    return !(ferror(fileptr) || fflush(fileptr) == EOF);
}

// Set up prior to actual save, do the save, then clean up
bool save_char() {
    while (!_save_char(savegame_filename)) {
        vtype_t temp;

        (void) sprintf(temp, "Save file '%s' fails.", savegame_filename);
        msg_print(temp);

        int i = 0;
        if (access(savegame_filename, 0) < 0 || get_check("File exists. Delete old save file?") == 0 || (i = unlink(savegame_filename)) < 0) {
            if (i < 0) {
                (void) sprintf(temp, "Can't delete '%s'", savegame_filename);
                msg_print(temp);
            }
            prt("New Save file [ESC to give up]:", 0, 0);
            if (!get_string(temp, 0, 31, 45)) {
                return false;
            }
            if (temp[0]) {
                (void) strcpy(savegame_filename, temp);
            }
        }
        (void) sprintf(temp, "Saving with %s...", savegame_filename);
        prt(temp, 0, 0);
    }

    return true;
}

bool _save_char(char *fnam) {
    if (character_saved) {
        return true; // Nothing to save.
    }

    put_qio();
    disturb(1, 0);             // Turn off resting and searching.
    change_speed(-pack_heaviness); // Fix the speed
    pack_heaviness = 0;
    bool ok = false;

    fileptr = NULL; // Do not assume it has been init'ed

    int fd = open(fnam, O_RDWR | O_CREAT | O_EXCL, 0600);

    if (fd < 0 && access(fnam, 0) >= 0 && (from_savefile || (wizard && get_check("Can't make new save file. Overwrite old?")))) {
        (void) chmod(fnam, 0600);
        fd = open(fnam, O_RDWR | O_TRUNC, 0600);
    }

    if (fd >= 0) {
        (void) close(fd);
        fileptr = fopen(savegame_filename, "wb");
    }

    DEBUG(logfile = fopen("IO_LOG", "a"));
    DEBUG(fprintf(logfile, "Saving data to %s\n", savegame_filename));

    if (fileptr != NULL) {
        xor_byte = 0;
        wr_byte((uint8_t) CURRENT_VERSION_MAJOR);
        xor_byte = 0;
        wr_byte((uint8_t) CURRENT_VERSION_MINOR);
        xor_byte = 0;
        wr_byte((uint8_t) CURRENT_VERSION_PATCH);
        xor_byte = 0;

        uint8_t char_tmp = (uint8_t) (randint(256) - 1);
        wr_byte(char_tmp);
        // Note that xor_byte is now equal to char_tmp

        ok = sv_write();

        DEBUG(fclose(logfile));

        if (fclose(fileptr) == EOF) {
            ok = false;
        }
    }

    if (!ok) {
        if (fd >= 0) {
            (void) unlink(fnam);
        }

        vtype_t temp;
        if (fd >= 0) {
            (void) sprintf(temp, "Error writing to file %s", fnam);
        } else {
            (void) sprintf(temp, "Can't create new file %s", fnam);
        }
        msg_print(temp);

        return false;
    } else {
        character_saved = true;
    }

    turn = -1;

    return true;
}

// Certain checks are omitted for the wizard. -CJS-
bool get_char(bool *generate) {
    int c;
    Cave_t *c_ptr;
    uint32_t time_saved = 0;
    uint8_t version_maj = 0;
    uint8_t version_min = 0;
    uint8_t patch_level = 0;

    *generate = true;
    int fd = -1;
    int total_count = 0;

    // Not required for Mac, because the file name is obtained through a dialog.
    // There is no way for a nonexistent file to be specified. -BS-
    if (access(savegame_filename, 0) != 0) {
        msg_print("Save file does not exist.");
        return false; // Don't bother with messages here. File absent.
    }

    clear_screen();

    vtype_t temp;
    (void) sprintf(temp, "Save file %s present. Attempting restore.", savegame_filename);
    put_buffer(temp, 23, 0);

    // FIXME: check this if/else logic! -- MRC
    if (turn >= 0) {
        msg_print("IMPOSSIBLE! Attempt to restore while still alive!");
    } else if ((fd = open(savegame_filename, O_RDONLY, 0)) < 0 && (chmod(savegame_filename, 0400) < 0 || (fd = open(savegame_filename, O_RDONLY, 0)) < 0)) {
        // Allow restoring a file belonging to someone else, if we can delete it.
        // Hence first try to read without doing a chmod.

        msg_print("Can't open file for reading.");
    } else {
        turn = -1;
        bool ok = true;

        (void) close(fd);
        fd = -1; // Make sure it isn't closed again
        fileptr = fopen(savegame_filename, "rb");

        if (fileptr == NULL) {
            goto error;
        }

        prt("Restoring Memory...", 0, 0);
        put_qio();

        DEBUG(logfile = fopen("IO_LOG", "a"));
        DEBUG(fprintf(logfile, "Reading data from %s\n", savegame_filename));

        xor_byte = 0;
        rd_byte(&version_maj);
        xor_byte = 0;
        rd_byte(&version_min);
        xor_byte = 0;
        rd_byte(&patch_level);
        xor_byte = 0;
        rd_byte(&xor_byte);

        // COMPAT support save files from 5.0.14 to 5.0.17.
        // Support save files from 5.1.0 to present.
        // As of version 5.4, accept save files even if they have higher version numbers.
        // The save file format was frozen as of version 5.2.2.
        if (version_maj != CURRENT_VERSION_MAJOR || (version_min == 0 && patch_level < 14)) {
            prt("Sorry. This save file is from a different version of umoria.", 2, 0);
            goto error;
        }

        uint16_t uint16_t_tmp;
        rd_short(&uint16_t_tmp);
        while (uint16_t_tmp != 0xFFFF) {
            if (uint16_t_tmp >= MAX_CREATURES) {
                goto error;
            }
            Recall_t *r_ptr = &creature_recall[uint16_t_tmp];
            rd_long(&r_ptr->r_cmove);
            rd_long(&r_ptr->r_spells);
            rd_short(&r_ptr->r_kills);
            rd_short(&r_ptr->r_deaths);
            rd_short(&r_ptr->r_cdefense);
            rd_byte(&r_ptr->r_wake);
            rd_byte(&r_ptr->r_ignore);
            rd_bytes(r_ptr->r_attacks, MAX_MON_NATTACK);
            rd_short(&uint16_t_tmp);
        }

        // for save files before 5.2.2, read and ignore log_index (sic)
        if (version_min < 2 || (version_min == 2 && patch_level < 2)) {
            rd_short(&uint16_t_tmp);
        }

        uint32_t l;
        rd_long(&l);

        run_cut_corners = (l & 0x1) != 0;
        run_examine_corners = (l & 0x2) != 0;
        run_print_self = (l & 0x4) != 0;
        find_bound = (l & 0x8) != 0;
        prompt_carry_flag = (l & 0x10) != 0;
        rogue_like_commands = (l & 0x20) != 0;
        show_weight_flag = (l & 0x40) != 0;
        highlight_seams = (l & 0x80) != 0;
        run_ignore_doors = (l & 0x100) != 0;

        // save files before 5.2.2 don't have sound_beep_flag, set it on for compatibility
        sound_beep_flag = version_min < 2 || (version_min == 2 && patch_level < 2) || (l & 0x200) != 0;

        // save files before 5.2.2 don't have display_counts, set it on for compatibility
        display_counts = version_min < 2 || (version_min == 2 && patch_level < 2) || (l & 0x400) != 0;

        // Don't allow resurrection of total_winner characters.  It causes
        // problems because the character level is out of the allowed range.
        if (to_be_wizard && (l & 0x40000000L)) {
            msg_print("Sorry, this character is retired from moria.");
            msg_print("You can not resurrect a retired character.");
        } else if (to_be_wizard && (l & 0x80000000L) && get_check("Resurrect a dead character?")) {
            l &= ~0x80000000L;
        }

        if ((l & 0x80000000L) == 0) {
            rd_string(py.misc.name);
            py.misc.male = rd_bool();
            rd_long((uint32_t *) &py.misc.au);
            rd_long((uint32_t *) &py.misc.max_exp);
            rd_long((uint32_t *) &py.misc.exp);
            rd_short(&py.misc.exp_frac);
            rd_short(&py.misc.age);
            rd_short(&py.misc.ht);
            rd_short(&py.misc.wt);
            rd_short(&py.misc.lev);
            rd_short(&py.misc.max_dlv);
            rd_short((uint16_t *) &py.misc.srh);
            rd_short((uint16_t *) &py.misc.fos);
            rd_short((uint16_t *) &py.misc.bth);
            rd_short((uint16_t *) &py.misc.bthb);
            rd_short((uint16_t *) &py.misc.mana);
            rd_short((uint16_t *) &py.misc.mhp);
            rd_short((uint16_t *) &py.misc.ptohit);
            rd_short((uint16_t *) &py.misc.ptodam);
            rd_short((uint16_t *) &py.misc.pac);
            rd_short((uint16_t *) &py.misc.ptoac);
            rd_short((uint16_t *) &py.misc.dis_th);
            rd_short((uint16_t *) &py.misc.dis_td);
            rd_short((uint16_t *) &py.misc.dis_ac);
            rd_short((uint16_t *) &py.misc.dis_tac);
            rd_short((uint16_t *) &py.misc.disarm);
            rd_short((uint16_t *) &py.misc.save);
            rd_short((uint16_t *) &py.misc.sc);
            rd_short((uint16_t *) &py.misc.stl);
            rd_byte(&py.misc.pclass);
            rd_byte(&py.misc.prace);
            rd_byte(&py.misc.hitdie);
            rd_byte(&py.misc.expfact);
            rd_short((uint16_t *) &py.misc.cmana);
            rd_short(&py.misc.cmana_frac);
            rd_short((uint16_t *) &py.misc.chp);
            rd_short(&py.misc.chp_frac);
            for (int i = 0; i < 4; i++) {
                rd_string(py.misc.history[i]);
            }

            rd_bytes(py.stats.max_stat, 6);
            rd_bytes(py.stats.cur_stat, 6);
            rd_shorts((uint16_t *) py.stats.mod_stat, 6);
            rd_bytes(py.stats.use_stat, 6);

            rd_long(&py.flags.status);
            rd_short((uint16_t *) &py.flags.rest);
            rd_short((uint16_t *) &py.flags.blind);
            rd_short((uint16_t *) &py.flags.paralysis);
            rd_short((uint16_t *) &py.flags.confused);
            rd_short((uint16_t *) &py.flags.food);
            rd_short((uint16_t *) &py.flags.food_digested);
            rd_short((uint16_t *) &py.flags.protection);
            rd_short((uint16_t *) &py.flags.speed);
            rd_short((uint16_t *) &py.flags.fast);
            rd_short((uint16_t *) &py.flags.slow);
            rd_short((uint16_t *) &py.flags.afraid);
            rd_short((uint16_t *) &py.flags.poisoned);
            rd_short((uint16_t *) &py.flags.image);
            rd_short((uint16_t *) &py.flags.protevil);
            rd_short((uint16_t *) &py.flags.invuln);
            rd_short((uint16_t *) &py.flags.hero);
            rd_short((uint16_t *) &py.flags.shero);
            rd_short((uint16_t *) &py.flags.blessed);
            rd_short((uint16_t *) &py.flags.resist_heat);
            rd_short((uint16_t *) &py.flags.resist_cold);
            rd_short((uint16_t *) &py.flags.detect_inv);
            rd_short((uint16_t *) &py.flags.word_recall);
            rd_short((uint16_t *) &py.flags.see_infra);
            rd_short((uint16_t *) &py.flags.tim_infra);
            py.flags.see_inv = rd_bool();
            py.flags.teleport = rd_bool();
            py.flags.free_act = rd_bool();
            py.flags.slow_digest = rd_bool();
            py.flags.aggravate = rd_bool();
            py.flags.fire_resist = rd_bool();
            py.flags.cold_resist = rd_bool();
            py.flags.acid_resist = rd_bool();
            py.flags.regenerate = rd_bool();
            py.flags.lght_resist = rd_bool();
            py.flags.ffall = rd_bool();
            py.flags.sustain_str = rd_bool();
            py.flags.sustain_int = rd_bool();
            py.flags.sustain_wis = rd_bool();
            py.flags.sustain_con = rd_bool();
            py.flags.sustain_dex = rd_bool();
            py.flags.sustain_chr = rd_bool();
            py.flags.confuse_monster = rd_bool();
            rd_byte(&py.flags.new_spells);

            rd_short((uint16_t *) &missile_ctr);
            rd_long((uint32_t *) &turn);
            rd_short((uint16_t *) &inven_ctr);
            if (inven_ctr > INVEN_WIELD) {
                goto error;
            }
            for (int i = 0; i < inven_ctr; i++) {
                rd_item(&inventory[i]);
            }
            for (int i = INVEN_WIELD; i < INVEN_ARRAY_SIZE; i++) {
                rd_item(&inventory[i]);
            }
            rd_short((uint16_t *) &inven_weight);
            rd_short((uint16_t *) &equip_ctr);
            rd_long(&spell_learned);
            rd_long(&spell_worked);
            rd_long(&spell_forgotten);
            rd_bytes(spell_order, 32);
            rd_bytes(objects_identified, OBJECT_IDENT_SIZE);
            rd_long(&randes_seed);
            rd_long(&town_seed);
            rd_short((uint16_t *) &last_msg);
            for (int i = 0; i < MAX_SAVE_MSG; i++) {
                rd_string(old_msgs[i]);
            }

            rd_short((uint16_t *) &panic_save);
            rd_short((uint16_t *) &total_winner);
            rd_short((uint16_t *) &noscore);
            rd_shorts(player_hp, MAX_PLAYER_LEVEL);

            if (version_min >= 2 || (version_min == 1 && patch_level >= 3)) {
                for (int i = 0; i < MAX_STORES; i++) {
                    Store_t *st_ptr = &stores[i];

                    rd_long((uint32_t *) &st_ptr->store_open);
                    rd_short((uint16_t *) &st_ptr->insult_cur);
                    rd_byte(&st_ptr->owner);
                    rd_byte(&st_ptr->store_ctr);
                    rd_short(&st_ptr->good_buy);
                    rd_short(&st_ptr->bad_buy);
                    if (st_ptr->store_ctr > STORE_INVEN_MAX) {
                        goto error;
                    }
                    for (int j = 0; j < st_ptr->store_ctr; j++) {
                        rd_long((uint32_t *) &st_ptr->store_inven[j].scost);
                        rd_item(&st_ptr->store_inven[j].sitem);
                    }
                }
            }

            if (version_min >= 2 || (version_min == 1 && patch_level >= 3)) {
                rd_long(&time_saved);
            }

            if (version_min >= 2) {
                rd_string(died_from);
            }

            if (version_min >= 3 || (version_min == 2 && patch_level >= 2)) {
                rd_long((uint32_t *) &max_score);
            } else {
                max_score = 0;
            }

            if (version_min >= 3 || (version_min == 2 && patch_level >= 2)) {
                rd_long((uint32_t *) &birth_date);
            } else {
                birth_date = (int32_t) time((time_t *) 0);
            }
        }

        c = getc(fileptr);
        if (c == EOF || (l & 0x80000000L)) {
            if ((l & 0x80000000L) == 0) {
                if (!to_be_wizard || turn < 0) {
                    goto error;
                }
                prt("Attempting a resurrection!", 0, 0);
                if (py.misc.chp < 0) {
                    py.misc.chp = 0;
                    py.misc.chp_frac = 0;
                }

                // don't let him starve to death immediately
                if (py.flags.food < 0) {
                    py.flags.food = 0;
                }

                // don't let him die of poison again immediately
                if (py.flags.poisoned > 1) {
                    py.flags.poisoned = 1;
                }

                dun_level = 0; // Resurrect on the town level.
                character_generated = true;

                // set noscore to indicate a resurrection, and don't enter
                // wizard mode
                to_be_wizard = false;
                noscore |= 0x1;
            } else {
                // Make sure that this message is seen, since it is a bit
                // more interesting than the other messages.
                msg_print("Restoring Memory of a departed spirit...");
                turn = -1;
            }
            put_qio();
            goto closefiles;
        }
        if (ungetc(c, fileptr) == EOF) {
            goto error;
        }

        prt("Restoring Character...", 0, 0);
        put_qio();

        // only level specific info should follow,
        // not present for dead characters

        rd_short((uint16_t *) &dun_level);
        rd_short((uint16_t *) &char_row);
        rd_short((uint16_t *) &char_col);
        rd_short((uint16_t *) &mon_tot_mult);
        rd_short((uint16_t *) &cur_height);
        rd_short((uint16_t *) &cur_width);
        rd_short((uint16_t *) &max_panel_rows);
        rd_short((uint16_t *) &max_panel_cols);

        uint8_t char_tmp, ychar, xchar, count;

        // read in the creature ptr info
        rd_byte(&char_tmp);
        while (char_tmp != 0xFF) {
            ychar = char_tmp;
            rd_byte(&xchar);
            rd_byte(&char_tmp);
            if (xchar > MAX_WIDTH || ychar > MAX_HEIGHT) {
                goto error;
            }
            cave[ychar][xchar].cptr = char_tmp;
            rd_byte(&char_tmp);
        }

        // read in the treasure ptr info
        rd_byte(&char_tmp);
        while (char_tmp != 0xFF) {
            ychar = char_tmp;
            rd_byte(&xchar);
            rd_byte(&char_tmp);
            if (xchar > MAX_WIDTH || ychar > MAX_HEIGHT) {
                goto error;
            }
            cave[ychar][xchar].tptr = char_tmp;
            rd_byte(&char_tmp);
        }

        // read in the rest of the cave info
        c_ptr = &cave[0][0];
        total_count = 0;
        while (total_count != MAX_HEIGHT * MAX_WIDTH) {
            rd_byte(&count);
            rd_byte(&char_tmp);
            for (int i = count; i > 0; i--) {
                if (c_ptr >= &cave[MAX_HEIGHT][0]) {
                    goto error;
                }
                c_ptr->fval = (uint8_t) (char_tmp & 0xF);
                c_ptr->lr = (char_tmp >> 4) != 0;
                c_ptr->fm = (char_tmp >> 5) != 0;
                c_ptr->pl = (char_tmp >> 6) != 0;
                c_ptr->tl = (char_tmp >> 7) != 0;
                c_ptr++;
            }
            total_count += count;
        }

        rd_short((uint16_t *) &tcptr);
        if (tcptr > MAX_TALLOC) {
            goto error;
        }
        for (int i = MIN_TRIX; i < tcptr; i++) {
            rd_item(&treasure_list[i]);
        }
        rd_short((uint16_t *) &mfptr);
        if (mfptr > MAX_MALLOC) {
            goto error;
        }
        for (int i = MIN_MONIX; i < mfptr; i++) {
            rd_monster(&monsters_list[i]);
        }

        *generate = false; // We have restored a cave - no need to generate.

        if ((version_min == 1 && patch_level < 3) || version_min == 0) {
            for (int i = 0; i < MAX_STORES; i++) {
                Store_t *st_ptr = &stores[i];

                rd_long((uint32_t *) &st_ptr->store_open);
                rd_short((uint16_t *) &st_ptr->insult_cur);
                rd_byte(&st_ptr->owner);
                rd_byte(&st_ptr->store_ctr);
                rd_short(&st_ptr->good_buy);
                rd_short(&st_ptr->bad_buy);
                if (st_ptr->store_ctr > STORE_INVEN_MAX) {
                    goto error;
                }
                for (int j = 0; j < st_ptr->store_ctr; j++) {
                    rd_long((uint32_t *) &st_ptr->store_inven[j].scost);
                    rd_item(&st_ptr->store_inven[j].sitem);
                }
            }
        }

        // read the time that the file was saved
        if (version_min == 0 && patch_level < 16) {
            time_saved = 0; // no time in file, clear to zero
        } else if (version_min == 1 && patch_level < 3) {
            rd_long(&time_saved);
        }

        if (ferror(fileptr)) {
            goto error;
        }

        if (turn < 0) {
            error:
            ok = false; // Assume bad data.
        } else {
            // don't overwrite the killed by string if character is dead
            if (py.misc.chp >= 0) {
                (void) strcpy(died_from, "(alive and well)");
            }
            character_generated = true;
        }

        closefiles:

        DEBUG(fclose(logfile));

        if (fileptr != NULL) {
            if (fclose(fileptr) < 0) {
                ok = false;
            }
        }
        if (fd >= 0) {
            (void) close(fd);
        }

        if (!ok) {
            msg_print("Error during reading of file.");
        } else {
            // let the user overwrite the old save file when save/quit
            from_savefile = 1;

            if (panic_save) {
                (void) sprintf(temp, "This game is from a panic save.  Score will not be added to scoreboard.");
                msg_print(temp);
            } else if (((!noscore) & 0x04) && duplicate_character()) {
                (void) sprintf(temp, "This character is already on the scoreboard; it will not be scored again.");
                msg_print(temp);
                noscore |= 0x4;
            }

            if (turn >= 0) { // Only if a full restoration.
                weapon_heavy = false;
                pack_heaviness = 0;
                check_strength();

                // rotate store inventory, depending on how old the save file
                // is foreach day old (rounded up), call store_maint
                // calculate age in seconds
                start_time = (uint32_t) time((time_t *) 0);

                uint32_t age;

                // check for reasonable values of time here ...
                if (start_time < time_saved) {
                    age = 0;
                } else {
                    age = start_time - time_saved;
                }

                age = (uint32_t) ((age + 43200L) / 86400L); // age in days
                if (age > 10) {
                    age = 10; // in case save file is very old
                }

                for (int i = 0; i < (int) age; i++) {
                    store_maint();
                }
            }

            if (noscore) {
                msg_print("This save file cannot be used to get on the score board.");
            }

            if (version_maj != CURRENT_VERSION_MAJOR || version_min != CURRENT_VERSION_MINOR) {
                (void) sprintf(
                        temp,
                        "Save file version %d.%d %s on game version %d.%d.",
                        version_maj,
                        version_min,
                        version_min <= CURRENT_VERSION_MINOR ? "accepted" : "risky",
                        CURRENT_VERSION_MAJOR, CURRENT_VERSION_MINOR
                );
                msg_print(temp);
            }

            // if false: only restored options and monster memory.
            return turn >= 0;
        }
    }
    turn = -1;
    prt("Please try again without that save file.", 1, 0);

    exit_game();

    return false; // not reached
}

static void wr_bool(bool c) {
    wr_byte((uint8_t) c);
}

static void wr_byte(uint8_t c) {
    xor_byte ^= c;
    (void) putc((int) xor_byte, fileptr);
    DEBUG(fprintf(logfile, "BYTE:  %02X = %d\n", (int) xor_byte, (int) c));
}

static void wr_short(uint16_t s) {
    xor_byte ^= (s & 0xFF);
    (void) putc((int) xor_byte, fileptr);
    DEBUG(fprintf(logfile, "SHORT: %02X", (int) xor_byte));
    xor_byte ^= ((s >> 8) & 0xFF);
    (void) putc((int) xor_byte, fileptr);
    DEBUG(fprintf(logfile, " %02X = %d\n", (int) xor_byte, (int) s));
}

static void wr_long(uint32_t l) {
    xor_byte ^= (l & 0xFF);
    (void) putc((int) xor_byte, fileptr);
    DEBUG(fprintf(logfile, "LONG:  %02X", (int) xor_byte));
    xor_byte ^= ((l >> 8) & 0xFF);
    (void) putc((int) xor_byte, fileptr);
    DEBUG(fprintf(logfile, " %02X", (int) xor_byte));
    xor_byte ^= ((l >> 16) & 0xFF);
    (void) putc((int) xor_byte, fileptr);
    DEBUG(fprintf(logfile, " %02X", (int) xor_byte));
    xor_byte ^= ((l >> 24) & 0xFF);
    (void) putc((int) xor_byte, fileptr);
    DEBUG(fprintf(logfile, " %02X = %ld\n", (int) xor_byte, (int32_t) l));
}

static void wr_bytes(uint8_t *c, int count) {
    uint8_t *ptr;

    DEBUG(fprintf(logfile, "%d BYTES:", count));
    ptr = c;
    for (int i = 0; i < count; i++) {
        xor_byte ^= *ptr++;
        (void) putc((int) xor_byte, fileptr);
        DEBUG(fprintf(logfile, "  %02X = %d", (int) xor_byte, (int) (ptr[-1])));
    }
    DEBUG(fprintf(logfile, "\n"));
}

static void wr_string(char *str) {
    DEBUG(char *s = str);
    DEBUG(fprintf(logfile, "STRING:"));
    while (*str != '\0') {
        xor_byte ^= *str++;
        (void) putc((int) xor_byte, fileptr);
        DEBUG(fprintf(logfile, " %02X", (int) xor_byte));
    }
    xor_byte ^= *str;
    (void) putc((int) xor_byte, fileptr);
    DEBUG(fprintf(logfile, " %02X = \"%s\"\n", (int) xor_byte, s));
}

static void wr_shorts(uint16_t *s, int count) {
    DEBUG(fprintf(logfile, "%d SHORTS:", count));

    uint16_t *sptr = s;

    for (int i = 0; i < count; i++) {
        xor_byte ^= (*sptr & 0xFF);
        (void) putc((int) xor_byte, fileptr);
        DEBUG(fprintf(logfile, "  %02X", (int) xor_byte));
        xor_byte ^= ((*sptr++ >> 8) & 0xFF);
        (void) putc((int) xor_byte, fileptr);
        DEBUG(fprintf(logfile, " %02X = %d", (int) xor_byte, (int) sptr[-1]));
    }
    DEBUG(fprintf(logfile, "\n"));
}

static void wr_item(Inventory_t *item) {
    DEBUG(fprintf(logfile, "ITEM:\n"));
    wr_short(item->index);
    wr_byte(item->name2);
    wr_string(item->inscrip);
    wr_long(item->flags);
    wr_byte(item->tval);
    wr_byte(item->tchar);
    wr_short((uint16_t) item->p1);
    wr_long((uint32_t) item->cost);
    wr_byte(item->subval);
    wr_byte(item->number);
    wr_short(item->weight);
    wr_short((uint16_t) item->tohit);
    wr_short((uint16_t) item->todam);
    wr_short((uint16_t) item->ac);
    wr_short((uint16_t) item->toac);
    wr_bytes(item->damage, 2);
    wr_byte(item->level);
    wr_byte(item->ident);
}

static void wr_monster(Monster_t *mon) {
    DEBUG(fprintf(logfile, "MONSTER:\n"));
    wr_short((uint16_t) mon->hp);
    wr_short((uint16_t) mon->csleep);
    wr_short((uint16_t) mon->cspeed);
    wr_short(mon->mptr);
    wr_byte(mon->fy);
    wr_byte(mon->fx);
    wr_byte(mon->cdis);
    wr_bool(mon->ml);
    wr_byte(mon->stunned);
    wr_byte(mon->confused);
}

static bool rd_bool() {
    uint8_t value;
    rd_byte(&value);
    return (bool) value;
}

static void rd_byte(uint8_t *ptr) {
    uint8_t c = (uint8_t) (getc(fileptr) & 0xFF);
    *ptr = c ^ xor_byte;
    xor_byte = c;
    DEBUG(fprintf(logfile, "BYTE:  %02X = %d\n", (int) c, (int) *ptr));
}

static void rd_short(uint16_t *ptr) {
    uint8_t c = (uint8_t) (getc(fileptr) & 0xFF);
    uint16_t s = c ^xor_byte;

    xor_byte = (uint8_t) (getc(fileptr) & 0xFF);
    s |= (uint16_t) (c ^ xor_byte) << 8;
    *ptr = s;
    DEBUG(fprintf(logfile, "SHORT: %02X %02X = %d\n", (int) c, (int) xor_byte, (int) s));
}

static void rd_long(uint32_t *ptr) {
    uint8_t c = (uint8_t) (getc(fileptr) & 0xFF);
    uint32_t l = c ^xor_byte;

    xor_byte = (uint8_t) (getc(fileptr) & 0xFF);
    l |= (uint32_t) (c ^ xor_byte) << 8;
    DEBUG(fprintf(logfile, "LONG:  %02X %02X ", (int) c, (int) xor_byte));
    c = (uint8_t) (getc(fileptr) & 0xFF);
    l |= (uint32_t) (c ^ xor_byte) << 16;
    xor_byte = (uint8_t) (getc(fileptr) & 0xFF);
    l |= (uint32_t) (c ^ xor_byte) << 24;
    *ptr = l;
    DEBUG(fprintf(logfile, "%02X %02X = %ld\n", (int) c, (int) xor_byte, (int32_t) l));
}

static void rd_bytes(uint8_t *ch_ptr, int count) {
    DEBUG(fprintf(logfile, "%d BYTES:", count));
    uint8_t *ptr = ch_ptr;
    for (int i = 0; i < count; i++) {
        uint8_t c = (uint8_t) (getc(fileptr) & 0xFF);
        *ptr++ = c ^ xor_byte;
        xor_byte = c;
        DEBUG(fprintf(logfile, "  %02X = %d", (int) c, (int) ptr[-1]));
    }
    DEBUG(fprintf(logfile, "\n"));
}

static void rd_string(char *str) {
    DEBUG(char *s = str);
    DEBUG(fprintf(logfile, "STRING: "));
    do {
        uint8_t c = (uint8_t) (getc(fileptr) & 0xFF);
        *str = c ^ xor_byte;
        xor_byte = c;
        DEBUG(fprintf(logfile, "%02X ", (int) c));
    } while (*str++ != '\0');
    DEBUG(fprintf(logfile, "= \"%s\"\n", s));
}

static void rd_shorts(uint16_t *ptr, int count) {
    DEBUG(fprintf(logfile, "%d SHORTS:", count));
    uint16_t *sptr = ptr;

    for (int i = 0; i < count; i++) {
        uint8_t c = (uint8_t) (getc(fileptr) & 0xFF);
        uint16_t s = c ^xor_byte;
        xor_byte = (uint8_t) (getc(fileptr) & 0xFF);
        s |= (uint16_t) (c ^ xor_byte) << 8;
        *sptr++ = s;
        DEBUG(fprintf(logfile, "  %02X %02X = %d", (int) c, (int) xor_byte, (int) s));
    }
    DEBUG(fprintf(logfile, "\n"));
}

static void rd_item(Inventory_t *item) {
    DEBUG(fprintf(logfile, "ITEM:\n"));
    rd_short(&item->index);
    rd_byte(&item->name2);
    rd_string(item->inscrip);
    rd_long(&item->flags);
    rd_byte(&item->tval);
    rd_byte(&item->tchar);
    rd_short((uint16_t *) &item->p1);
    rd_long((uint32_t *) &item->cost);
    rd_byte(&item->subval);
    rd_byte(&item->number);
    rd_short(&item->weight);
    rd_short((uint16_t *) &item->tohit);
    rd_short((uint16_t *) &item->todam);
    rd_short((uint16_t *) &item->ac);
    rd_short((uint16_t *) &item->toac);
    rd_bytes(item->damage, 2);
    rd_byte(&item->level);
    rd_byte(&item->ident);
}

static void rd_monster(Monster_t *mon) {
    DEBUG(fprintf(logfile, "MONSTER:\n"));
    rd_short((uint16_t *) &mon->hp);
    rd_short((uint16_t *) &mon->csleep);
    rd_short((uint16_t *) &mon->cspeed);
    rd_short(&mon->mptr);
    rd_byte(&mon->fy);
    rd_byte(&mon->fx);
    rd_byte(&mon->cdis);
    mon->ml = rd_bool();
    rd_byte(&mon->stunned);
    rd_byte(&mon->confused);
}

// functions called from death.c to implement the score file

// set the local fileptr to the score file fileptr
void set_fileptr(FILE *file) {
    fileptr = file;
}

void wr_highscore(HighScore_t *score) {
    DEBUG(logfile = fopen("IO_LOG", "a"));
    DEBUG(fprintf(logfile, "Saving score:\n"));

    // Save the encryption byte for robustness.
    wr_byte(xor_byte);

    wr_long((uint32_t) score->points);
    wr_long((uint32_t) score->birth_date);
    wr_short((uint16_t) score->uid);
    wr_short((uint16_t) score->mhp);
    wr_short((uint16_t) score->chp);
    wr_byte(score->dun_level);
    wr_byte(score->lev);
    wr_byte(score->max_dlv);
    wr_byte(score->sex);
    wr_byte(score->race);
    wr_byte(score->character_class);
    wr_bytes((uint8_t *) score->name, PLAYER_NAME_SIZE);
    wr_bytes((uint8_t *) score->died_from, 25);
    DEBUG(fclose(logfile));
}

void rd_highscore(HighScore_t *score) {
    DEBUG(logfile = fopen("IO_LOG", "a"));
    DEBUG(fprintf(logfile, "Reading score:\n"));

    // Read the encryption byte.
    rd_byte(&xor_byte);

    rd_long((uint32_t *) &score->points);
    rd_long((uint32_t *) &score->birth_date);
    rd_short((uint16_t *) &score->uid);
    rd_short((uint16_t *) &score->mhp);
    rd_short((uint16_t *) &score->chp);
    rd_byte(&score->dun_level);
    rd_byte(&score->lev);
    rd_byte(&score->max_dlv);
    rd_byte(&score->sex);
    rd_byte(&score->race);
    rd_byte(&score->character_class);
    rd_bytes((uint8_t *) score->name, PLAYER_NAME_SIZE);
    rd_bytes((uint8_t *) score->died_from, 25);
    DEBUG(fclose(logfile));
}
