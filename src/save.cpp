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

static bool _save_char(char *);
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
    // clear the character_is_dead flag when creating a HANGUP save file,
    // so that player can see tombstone when restart
    if (eof_flag) {
        character_is_dead = false;
    }

    uint32_t l = 0;

    if (config.run_cut_corners) {
        l |= 0x1;
    }
    if (config.run_examine_corners) {
        l |= 0x2;
    }
    if (config.run_print_self) {
        l |= 0x4;
    }
    if (config.find_bound) {
        l |= 0x8;
    }
    if (config.prompt_to_pickup) {
        l |= 0x10;
    }
    if (config.use_roguelike_keys) {
        l |= 0x20;
    }
    if (config.show_inventory_weights) {
        l |= 0x40;
    }
    if (config.highlight_seams) {
        l |= 0x80;
    }
    if (config.run_ignore_doors) {
        l |= 0x100;
    }
    if (config.error_beep_sound) {
        l |= 0x200;
    }
    if (config.display_counts) {
        l |= 0x400;
    }
    if (character_is_dead) {
        // Sign bit
        l |= 0x80000000L;
    }
    if (total_winner) {
        l |= 0x40000000L;
    }

    for (int i = 0; i < MON_MAX_CREATURES; i++) {
        Recall_t *r_ptr = &creature_recall[i];

        if (r_ptr->movement || r_ptr->defenses || r_ptr->kills ||
            r_ptr->spells || r_ptr->deaths || r_ptr->attacks[0] ||
            r_ptr->attacks[1] || r_ptr->attacks[2] || r_ptr->attacks[3]) {
            wr_short((uint16_t) i);
            wr_long(r_ptr->movement);
            wr_long(r_ptr->spells);
            wr_short(r_ptr->kills);
            wr_short(r_ptr->deaths);
            wr_short(r_ptr->defenses);
            wr_byte(r_ptr->wake);
            wr_byte(r_ptr->ignore);
            wr_bytes(r_ptr->attacks, MON_MAX_ATTACKS);
        }
    }

    // sentinel to indicate no more monster info
    wr_short((uint16_t) 0xFFFF);

    wr_long(l);

    wr_string(py.misc.name);
    wr_bool(py.misc.gender);
    wr_long((uint32_t) py.misc.au);
    wr_long((uint32_t) py.misc.max_exp);
    wr_long((uint32_t) py.misc.exp);
    wr_short(py.misc.exp_frac);
    wr_short(py.misc.age);
    wr_short(py.misc.height);
    wr_short(py.misc.weight);
    wr_short(py.misc.level);
    wr_short(py.misc.max_dungeon_depth);
    wr_short((uint16_t) py.misc.chance_in_search);
    wr_short((uint16_t) py.misc.freng_of_search);
    wr_short((uint16_t) py.misc.bth);
    wr_short((uint16_t) py.misc.bth_with_bows);
    wr_short((uint16_t) py.misc.mana);
    wr_short((uint16_t) py.misc.max_hp);
    wr_short((uint16_t) py.misc.plusses_to_hit);
    wr_short((uint16_t) py.misc.plusses_to_damage);
    wr_short((uint16_t) py.misc.ac);
    wr_short((uint16_t) py.misc.magical_ac);
    wr_short((uint16_t) py.misc.display_to_hit);
    wr_short((uint16_t) py.misc.display_to_damage);
    wr_short((uint16_t) py.misc.display_ac);
    wr_short((uint16_t) py.misc.display_to_ac);
    wr_short((uint16_t) py.misc.disarm);
    wr_short((uint16_t) py.misc.saving_throw);
    wr_short((uint16_t) py.misc.social_class);
    wr_short((uint16_t) py.misc.stealth_factor);
    wr_byte(py.misc.class_id);
    wr_byte(py.misc.race_id);
    wr_byte(py.misc.hit_die);
    wr_byte(py.misc.experience_factor);
    wr_short((uint16_t) py.misc.current_mana);
    wr_short(py.misc.current_mana_fraction);
    wr_short((uint16_t) py.misc.current_hp);
    wr_short(py.misc.current_hp_fraction);
    for (int i = 0; i < 4; i++) {
        wr_string(py.misc.history[i]);
    }

    wr_bytes(py.stats.max, 6);
    wr_bytes(py.stats.current, 6);
    wr_shorts((uint16_t *) py.stats.modified, 6);
    wr_bytes(py.stats.used, 6);

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
    wr_short((uint16_t) py.flags.protect_evil);
    wr_short((uint16_t) py.flags.invulnerability);
    wr_short((uint16_t) py.flags.heroism);
    wr_short((uint16_t) py.flags.super_heroism);
    wr_short((uint16_t) py.flags.blessed);
    wr_short((uint16_t) py.flags.heat_resistance);
    wr_short((uint16_t) py.flags.cold_resistance);
    wr_short((uint16_t) py.flags.detect_invisible);
    wr_short((uint16_t) py.flags.word_of_recall);
    wr_short((uint16_t) py.flags.see_infra);
    wr_short((uint16_t) py.flags.timed_infra);
    wr_bool(py.flags.see_invisible);
    wr_bool(py.flags.teleport);
    wr_bool(py.flags.free_action);
    wr_bool(py.flags.slow_digest);
    wr_bool(py.flags.aggravate);
    wr_bool(py.flags.resistant_to_fire);
    wr_bool(py.flags.resistant_to_cold);
    wr_bool(py.flags.resistant_to_acid);
    wr_bool(py.flags.regenerate_hp);
    wr_bool(py.flags.resistant_to_light);
    wr_bool(py.flags.free_fall);
    wr_bool(py.flags.sustain_str);
    wr_bool(py.flags.sustain_int);
    wr_bool(py.flags.sustain_wis);
    wr_bool(py.flags.sustain_con);
    wr_bool(py.flags.sustain_dex);
    wr_bool(py.flags.sustain_chr);
    wr_bool(py.flags.confuse_monster);
    wr_byte(py.flags.new_spells_to_learn);

    wr_short((uint16_t) missiles_counter);
    wr_long((uint32_t) current_game_turn);
    wr_short((uint16_t) inventory_count);
    for (int i = 0; i < inventory_count; i++) {
        wr_item(&inventory[i]);
    }
    for (int i = EQUIPMENT_WIELD; i < PLAYER_INVENTORY_SIZE; i++) {
        wr_item(&inventory[i]);
    }
    wr_short((uint16_t) inventory_weight);
    wr_short((uint16_t) equipment_count);
    wr_long(spells_learnt);
    wr_long(spells_worked);
    wr_long(spells_forgotten);
    wr_bytes(spells_learned_order, 32);
    wr_bytes(objects_identified, OBJECT_IDENT_SIZE);
    wr_long(magic_seed);
    wr_long(town_seed);
    wr_short((uint16_t) last_message_id);
    for (int i = 0; i < MESSAGE_HISTORY_SIZE; i++) {
        wr_string(messages[i]);
    }

    // this indicates 'cheating' if it is a one
    wr_short((uint16_t) panic_save);
    wr_short((uint16_t) total_winner);
    wr_short((uint16_t) noscore);
    wr_shorts(player_base_hp_levels, PLAYER_MAX_LEVEL);

    for (int i = 0; i < MAX_STORES; i++) {
        Store_t *st_ptr = &stores[i];

        wr_long((uint32_t) st_ptr->turns_left_before_closing);
        wr_short((uint16_t) st_ptr->insults_counter);
        wr_byte(st_ptr->owner);
        wr_byte(st_ptr->store_id);
        wr_short(st_ptr->good_purchases);
        wr_short(st_ptr->bad_purchases);
        for (int j = 0; j < st_ptr->store_id; j++) {
            wr_long((uint32_t) st_ptr->inventory[j].cost);
            wr_item(&st_ptr->inventory[j].item);
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

    // starting with 5.2, put character_died_from string in save file
    wr_string(character_died_from);

    // starting with 5.2.2, put the character_max_score in the save file
    l = (uint32_t) (playerCalculateTotalPoints());
    wr_long(l);

    // starting with 5.2.2, put the character_birth_date in the save file
    wr_long((uint32_t) character_birth_date);

    // only level specific info follows, this allows characters to be
    // resurrected, the dungeon level info is not needed for a resurrection
    if (character_is_dead) {
        return !(ferror(fileptr) || fflush(fileptr) == EOF);
    }

    wr_short((uint16_t) current_dungeon_level);
    wr_short((uint16_t) char_row);
    wr_short((uint16_t) char_col);
    wr_short((uint16_t) monster_multiply_total);
    wr_short((uint16_t) dungeon_height);
    wr_short((uint16_t) dungeon_width);
    wr_short((uint16_t) max_panel_rows);
    wr_short((uint16_t) max_panel_cols);

    for (int i = 0; i < MAX_HEIGHT; i++) {
        for (int j = 0; j < MAX_WIDTH; j++) {
            Cave_t *c_ptr = &cave[i][j];
            if (c_ptr->creature_id != 0) {
                wr_byte((uint8_t) i);
                wr_byte((uint8_t) j);
                wr_byte(c_ptr->creature_id);
            }
        }
    }

    // marks end of creature_id info
    wr_byte((uint8_t) 0xFF);

    for (int i = 0; i < MAX_HEIGHT; i++) {
        for (int j = 0; j < MAX_WIDTH; j++) {
            Cave_t *c_ptr = &cave[i][j];
            if (c_ptr->treasure_id != 0) {
                wr_byte((uint8_t) i);
                wr_byte((uint8_t) j);
                wr_byte(c_ptr->treasure_id);
            }
        }
    }

    // marks end of treasure_id info
    wr_byte((uint8_t) 0xFF);

    // must set counter to zero, note that code may write out two bytes unnecessarily
    int count = 0;
    uint8_t prev_char = 0;

    for (int i = 0; i < MAX_HEIGHT; i++) {
        for (int j = 0; j < MAX_WIDTH; j++) {
            Cave_t *c_ptr = &cave[i][j];

            uint8_t char_tmp = (uint8_t) (c_ptr->feature_id | (c_ptr->perma_lit_room << 4) | (c_ptr->field_mark << 5) | (c_ptr->permanent_light << 6) | (c_ptr->temporary_light << 7));

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

    wr_short((uint16_t) current_treasure_id);
    for (int i = MIN_TREASURE_LIST_ID; i < current_treasure_id; i++) {
        wr_item(&treasure_list[i]);
    }
    wr_short((uint16_t) next_free_monster_id);
    for (int i = MON_MIN_INDEX_ID; i < next_free_monster_id; i++) {
        wr_monster(&monsters[i]);
    }

    return !(ferror(fileptr) || fflush(fileptr) == EOF);
}

// Set up prior to actual save, do the save, then clean up
bool saveGame() {
    while (!_save_char(config.save_game_filename)) {
        vtype_t temp;

        (void) sprintf(temp, "Save file '%s' fails.", config.save_game_filename);
        printMessage(temp);

        int i = 0;
        if (access(config.save_game_filename, 0) < 0 || getInputConfirmation("File exists. Delete old save file?") == 0 || (i = unlink(config.save_game_filename)) < 0) {
            if (i < 0) {
                (void) sprintf(temp, "Can't delete '%s'", config.save_game_filename);
                printMessage(temp);
            }
            putStringClearToEOL("New Save file [ESC to give up]:", 0, 0);
            if (!getStringInput(temp, 0, 31, 45)) {
                return false;
            }
            if (temp[0]) {
                (void) strcpy(config.save_game_filename, temp);
            }
        }
        (void) sprintf(temp, "Saving with %s...", config.save_game_filename);
        putStringClearToEOL(temp, 0, 0);
    }

    return true;
}

static bool _save_char(char *fnam) {
    if (character_saved) {
        return true; // Nothing to save.
    }

    putQIO();
    playerDisturb(1, 0);             // Turn off resting and searching.
    playerChangeSpeed(-pack_heaviness); // Fix the speed
    pack_heaviness = 0;
    bool ok = false;

    fileptr = NULL; // Do not assume it has been init'ed

    int fd = open(fnam, O_RDWR | O_CREAT | O_EXCL, 0600);

    if (fd < 0 && access(fnam, 0) >= 0 && (from_savefile || (wizard_mode && getInputConfirmation("Can't make new save file. Overwrite old?")))) {
        (void) chmod(fnam, 0600);
        fd = open(fnam, O_RDWR | O_TRUNC, 0600);
    }

    if (fd >= 0) {
        (void) close(fd);
        fileptr = fopen(config.save_game_filename, "wb");
    }

    DEBUG(logfile = fopen("IO_LOG", "a"));
    DEBUG(fprintf(logfile, "Saving data to %s\n", config.save_game_filename));

    if (fileptr != NULL) {
        xor_byte = 0;
        wr_byte((uint8_t) CURRENT_VERSION_MAJOR);
        xor_byte = 0;
        wr_byte((uint8_t) CURRENT_VERSION_MINOR);
        xor_byte = 0;
        wr_byte((uint8_t) CURRENT_VERSION_PATCH);
        xor_byte = 0;

        uint8_t char_tmp = (uint8_t) (randomNumber(256) - 1);
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
        printMessage(temp);

        return false;
    } else {
        character_saved = true;
    }

    current_game_turn = -1;

    return true;
}

// Certain checks are omitted for the wizard. -CJS-
bool loadGame(bool *generate) {
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
    if (access(config.save_game_filename, 0) != 0) {
        printMessage("Save file does not exist.");
        return false; // Don't bother with messages here. File absent.
    }

    clearScreen();

    vtype_t temp;
    (void) sprintf(temp, "Save file %s present. Attempting restore.", config.save_game_filename);
    putString(temp, 23, 0);

    // FIXME: check this if/else logic! -- MRC
    if (current_game_turn >= 0) {
        printMessage("IMPOSSIBLE! Attempt to restore while still alive!");
    } else if ((fd = open(config.save_game_filename, O_RDONLY, 0)) < 0 && (chmod(config.save_game_filename, 0400) < 0 || (fd = open(config.save_game_filename, O_RDONLY, 0)) < 0)) {
        // Allow restoring a file belonging to someone else, if we can delete it.
        // Hence first try to read without doing a chmod.

        printMessage("Can't open file for reading.");
    } else {
        current_game_turn = -1;
        bool ok = true;

        (void) close(fd);
        fd = -1; // Make sure it isn't closed again
        fileptr = fopen(config.save_game_filename, "rb");

        if (fileptr == NULL) {
            goto error;
        }

        putStringClearToEOL("Restoring Memory...", 0, 0);
        putQIO();

        DEBUG(logfile = fopen("IO_LOG", "a"));
        DEBUG(fprintf(logfile, "Reading data from %s\n", config.save_game_filename));

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
            putStringClearToEOL("Sorry. This save file is from a different version of umoria.", 2, 0);
            goto error;
        }

        uint16_t uint16_t_tmp;
        rd_short(&uint16_t_tmp);
        while (uint16_t_tmp != 0xFFFF) {
            if (uint16_t_tmp >= MON_MAX_CREATURES) {
                goto error;
            }
            Recall_t *r_ptr = &creature_recall[uint16_t_tmp];
            rd_long(&r_ptr->movement);
            rd_long(&r_ptr->spells);
            rd_short(&r_ptr->kills);
            rd_short(&r_ptr->deaths);
            rd_short(&r_ptr->defenses);
            rd_byte(&r_ptr->wake);
            rd_byte(&r_ptr->ignore);
            rd_bytes(r_ptr->attacks, MON_MAX_ATTACKS);
            rd_short(&uint16_t_tmp);
        }

        // for save files before 5.2.2, read and ignore log_index (sic)
        if (version_min < 2 || (version_min == 2 && patch_level < 2)) {
            rd_short(&uint16_t_tmp);
        }

        uint32_t l;
        rd_long(&l);

        config.run_cut_corners = (l & 0x1) != 0;
        config.run_examine_corners = (l & 0x2) != 0;
        config.run_print_self = (l & 0x4) != 0;
        config.find_bound = (l & 0x8) != 0;
        config.prompt_to_pickup = (l & 0x10) != 0;
        config.use_roguelike_keys = (l & 0x20) != 0;
        config.show_inventory_weights = (l & 0x40) != 0;
        config.highlight_seams = (l & 0x80) != 0;
        config.run_ignore_doors = (l & 0x100) != 0;

        // save files before 5.2.2 don't have `error_beep_sound`, set it on for compatibility
        config.error_beep_sound = version_min < 2 || (version_min == 2 && patch_level < 2) || (l & 0x200) != 0;

        // save files before 5.2.2 don't have `display_counts`, set it on for compatibility
        config.display_counts = version_min < 2 || (version_min == 2 && patch_level < 2) || (l & 0x400) != 0;

        // Don't allow resurrection of total_winner characters.  It causes
        // problems because the character level is out of the allowed range.
        if (to_be_wizard && (l & 0x40000000L)) {
            printMessage("Sorry, this character is retired from moria.");
            printMessage("You can not resurrect a retired character.");
        } else if (to_be_wizard && (l & 0x80000000L) && getInputConfirmation("Resurrect a dead character?")) {
            l &= ~0x80000000L;
        }

        if ((l & 0x80000000L) == 0) {
            rd_string(py.misc.name);
            py.misc.gender = rd_bool();
            rd_long((uint32_t *) &py.misc.au);
            rd_long((uint32_t *) &py.misc.max_exp);
            rd_long((uint32_t *) &py.misc.exp);
            rd_short(&py.misc.exp_frac);
            rd_short(&py.misc.age);
            rd_short(&py.misc.height);
            rd_short(&py.misc.weight);
            rd_short(&py.misc.level);
            rd_short(&py.misc.max_dungeon_depth);
            rd_short((uint16_t *) &py.misc.chance_in_search);
            rd_short((uint16_t *) &py.misc.freng_of_search);
            rd_short((uint16_t *) &py.misc.bth);
            rd_short((uint16_t *) &py.misc.bth_with_bows);
            rd_short((uint16_t *) &py.misc.mana);
            rd_short((uint16_t *) &py.misc.max_hp);
            rd_short((uint16_t *) &py.misc.plusses_to_hit);
            rd_short((uint16_t *) &py.misc.plusses_to_damage);
            rd_short((uint16_t *) &py.misc.ac);
            rd_short((uint16_t *) &py.misc.magical_ac);
            rd_short((uint16_t *) &py.misc.display_to_hit);
            rd_short((uint16_t *) &py.misc.display_to_damage);
            rd_short((uint16_t *) &py.misc.display_ac);
            rd_short((uint16_t *) &py.misc.display_to_ac);
            rd_short((uint16_t *) &py.misc.disarm);
            rd_short((uint16_t *) &py.misc.saving_throw);
            rd_short((uint16_t *) &py.misc.social_class);
            rd_short((uint16_t *) &py.misc.stealth_factor);
            rd_byte(&py.misc.class_id);
            rd_byte(&py.misc.race_id);
            rd_byte(&py.misc.hit_die);
            rd_byte(&py.misc.experience_factor);
            rd_short((uint16_t *) &py.misc.current_mana);
            rd_short(&py.misc.current_mana_fraction);
            rd_short((uint16_t *) &py.misc.current_hp);
            rd_short(&py.misc.current_hp_fraction);
            for (int i = 0; i < 4; i++) {
                rd_string(py.misc.history[i]);
            }

            rd_bytes(py.stats.max, 6);
            rd_bytes(py.stats.current, 6);
            rd_shorts((uint16_t *) py.stats.modified, 6);
            rd_bytes(py.stats.used, 6);

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
            rd_short((uint16_t *) &py.flags.protect_evil);
            rd_short((uint16_t *) &py.flags.invulnerability);
            rd_short((uint16_t *) &py.flags.heroism);
            rd_short((uint16_t *) &py.flags.super_heroism);
            rd_short((uint16_t *) &py.flags.blessed);
            rd_short((uint16_t *) &py.flags.heat_resistance);
            rd_short((uint16_t *) &py.flags.cold_resistance);
            rd_short((uint16_t *) &py.flags.detect_invisible);
            rd_short((uint16_t *) &py.flags.word_of_recall);
            rd_short((uint16_t *) &py.flags.see_infra);
            rd_short((uint16_t *) &py.flags.timed_infra);
            py.flags.see_invisible = rd_bool();
            py.flags.teleport = rd_bool();
            py.flags.free_action = rd_bool();
            py.flags.slow_digest = rd_bool();
            py.flags.aggravate = rd_bool();
            py.flags.resistant_to_fire = rd_bool();
            py.flags.resistant_to_cold = rd_bool();
            py.flags.resistant_to_acid = rd_bool();
            py.flags.regenerate_hp = rd_bool();
            py.flags.resistant_to_light = rd_bool();
            py.flags.free_fall = rd_bool();
            py.flags.sustain_str = rd_bool();
            py.flags.sustain_int = rd_bool();
            py.flags.sustain_wis = rd_bool();
            py.flags.sustain_con = rd_bool();
            py.flags.sustain_dex = rd_bool();
            py.flags.sustain_chr = rd_bool();
            py.flags.confuse_monster = rd_bool();
            rd_byte(&py.flags.new_spells_to_learn);

            rd_short((uint16_t *) &missiles_counter);
            rd_long((uint32_t *) &current_game_turn);
            rd_short((uint16_t *) &inventory_count);
            if (inventory_count > EQUIPMENT_WIELD) {
                goto error;
            }
            for (int i = 0; i < inventory_count; i++) {
                rd_item(&inventory[i]);
            }
            for (int i = EQUIPMENT_WIELD; i < PLAYER_INVENTORY_SIZE; i++) {
                rd_item(&inventory[i]);
            }
            rd_short((uint16_t *) &inventory_weight);
            rd_short((uint16_t *) &equipment_count);
            rd_long(&spells_learnt);
            rd_long(&spells_worked);
            rd_long(&spells_forgotten);
            rd_bytes(spells_learned_order, 32);
            rd_bytes(objects_identified, OBJECT_IDENT_SIZE);
            rd_long(&magic_seed);
            rd_long(&town_seed);
            rd_short((uint16_t *) &last_message_id);
            for (int i = 0; i < MESSAGE_HISTORY_SIZE; i++) {
                rd_string(messages[i]);
            }

            rd_short((uint16_t *) &panic_save);
            rd_short((uint16_t *) &total_winner);
            rd_short((uint16_t *) &noscore);
            rd_shorts(player_base_hp_levels, PLAYER_MAX_LEVEL);

            if (version_min >= 2 || (version_min == 1 && patch_level >= 3)) {
                for (int i = 0; i < MAX_STORES; i++) {
                    Store_t *st_ptr = &stores[i];

                    rd_long((uint32_t *) &st_ptr->turns_left_before_closing);
                    rd_short((uint16_t *) &st_ptr->insults_counter);
                    rd_byte(&st_ptr->owner);
                    rd_byte(&st_ptr->store_id);
                    rd_short(&st_ptr->good_purchases);
                    rd_short(&st_ptr->bad_purchases);
                    if (st_ptr->store_id > STORE_MAX_DISCRETE_ITEMS) {
                        goto error;
                    }
                    for (int j = 0; j < st_ptr->store_id; j++) {
                        rd_long((uint32_t *) &st_ptr->inventory[j].cost);
                        rd_item(&st_ptr->inventory[j].item);
                    }
                }
            }

            if (version_min >= 2 || (version_min == 1 && patch_level >= 3)) {
                rd_long(&time_saved);
            }

            if (version_min >= 2) {
                rd_string(character_died_from);
            }

            if (version_min >= 3 || (version_min == 2 && patch_level >= 2)) {
                rd_long((uint32_t *) &character_max_score);
            } else {
                character_max_score = 0;
            }

            if (version_min >= 3 || (version_min == 2 && patch_level >= 2)) {
                rd_long((uint32_t *) &character_birth_date);
            } else {
                character_birth_date = (int32_t) time((time_t *) 0);
            }
        }

        c = getc(fileptr);
        if (c == EOF || (l & 0x80000000L)) {
            if ((l & 0x80000000L) == 0) {
                if (!to_be_wizard || current_game_turn < 0) {
                    goto error;
                }
                putStringClearToEOL("Attempting a resurrection!", 0, 0);
                if (py.misc.current_hp < 0) {
                    py.misc.current_hp = 0;
                    py.misc.current_hp_fraction = 0;
                }

                // don't let him starve to death immediately
                if (py.flags.food < 0) {
                    py.flags.food = 0;
                }

                // don't let him die of poison again immediately
                if (py.flags.poisoned > 1) {
                    py.flags.poisoned = 1;
                }

                current_dungeon_level = 0; // Resurrect on the town level.
                character_generated = true;

                // set noscore to indicate a resurrection, and don't enter
                // wizard mode
                to_be_wizard = false;
                noscore |= 0x1;
            } else {
                // Make sure that this message is seen, since it is a bit
                // more interesting than the other messages.
                printMessage("Restoring Memory of a departed spirit...");
                current_game_turn = -1;
            }
            putQIO();
            goto closefiles;
        }
        if (ungetc(c, fileptr) == EOF) {
            goto error;
        }

        putStringClearToEOL("Restoring Character...", 0, 0);
        putQIO();

        // only level specific info should follow,
        // not present for dead characters

        rd_short((uint16_t *) &current_dungeon_level);
        rd_short((uint16_t *) &char_row);
        rd_short((uint16_t *) &char_col);
        rd_short((uint16_t *) &monster_multiply_total);
        rd_short((uint16_t *) &dungeon_height);
        rd_short((uint16_t *) &dungeon_width);
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
            cave[ychar][xchar].creature_id = char_tmp;
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
            cave[ychar][xchar].treasure_id = char_tmp;
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
                c_ptr->feature_id = (uint8_t) (char_tmp & 0xF);
                c_ptr->perma_lit_room = (char_tmp >> 4) != 0;
                c_ptr->field_mark = (char_tmp >> 5) != 0;
                c_ptr->permanent_light = (char_tmp >> 6) != 0;
                c_ptr->temporary_light = (char_tmp >> 7) != 0;
                c_ptr++;
            }
            total_count += count;
        }

        rd_short((uint16_t *) &current_treasure_id);
        if (current_treasure_id > LEVEL_MAX_OBJECTS) {
            goto error;
        }
        for (int i = MIN_TREASURE_LIST_ID; i < current_treasure_id; i++) {
            rd_item(&treasure_list[i]);
        }
        rd_short((uint16_t *) &next_free_monster_id);
        if (next_free_monster_id > MON_TOTAL_ALLOCATIONS) {
            goto error;
        }
        for (int i = MON_MIN_INDEX_ID; i < next_free_monster_id; i++) {
            rd_monster(&monsters[i]);
        }

        *generate = false; // We have restored a cave - no need to generate.

        if ((version_min == 1 && patch_level < 3) || version_min == 0) {
            for (int i = 0; i < MAX_STORES; i++) {
                Store_t *st_ptr = &stores[i];

                rd_long((uint32_t *) &st_ptr->turns_left_before_closing);
                rd_short((uint16_t *) &st_ptr->insults_counter);
                rd_byte(&st_ptr->owner);
                rd_byte(&st_ptr->store_id);
                rd_short(&st_ptr->good_purchases);
                rd_short(&st_ptr->bad_purchases);
                if (st_ptr->store_id > STORE_MAX_DISCRETE_ITEMS) {
                    goto error;
                }
                for (int j = 0; j < st_ptr->store_id; j++) {
                    rd_long((uint32_t *) &st_ptr->inventory[j].cost);
                    rd_item(&st_ptr->inventory[j].item);
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

        if (current_game_turn < 0) {
            error:
            ok = false; // Assume bad data.
        } else {
            // don't overwrite the killed by string if character is dead
            if (py.misc.current_hp >= 0) {
                (void) strcpy(character_died_from, "(alive and well)");
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
            printMessage("Error during reading of file.");
        } else {
            // let the user overwrite the old save file when save/quit
            from_savefile = 1;

            if (panic_save) {
                (void) sprintf(temp, "This game is from a panic save.  Score will not be added to scoreboard.");
                printMessage(temp);
            } else if (((!noscore) & 0x04) && duplicate_character()) {
                (void) sprintf(temp, "This character is already on the scoreboard; it will not be scored again.");
                printMessage(temp);
                noscore |= 0x4;
            }

            if (current_game_turn >= 0) { // Only if a full restoration.
                weapon_is_heavy = false;
                pack_heaviness = 0;
                playerStrength();

                // rotate store inventory, depending on how old the save file
                // is foreach day old (rounded up), call storeMaintenance
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
                    storeMaintenance();
                }
            }

            if (noscore) {
                printMessage("This save file cannot be used to get on the score board.");
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
                printMessage(temp);
            }

            // if false: only restored options and monster memory.
            return current_game_turn >= 0;
        }
    }
    current_game_turn = -1;
    putStringClearToEOL("Please try again without that save file.", 1, 0);

    exitGame();

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
    wr_short(item->id);
    wr_byte(item->special_name_id);
    wr_string(item->inscription);
    wr_long(item->flags);
    wr_byte(item->category_id);
    wr_byte(item->sprite);
    wr_short((uint16_t) item->misc_use);
    wr_long((uint32_t) item->cost);
    wr_byte(item->sub_category_id);
    wr_byte(item->items_count);
    wr_short(item->weight);
    wr_short((uint16_t) item->to_hit);
    wr_short((uint16_t) item->to_damage);
    wr_short((uint16_t) item->ac);
    wr_short((uint16_t) item->to_ac);
    wr_bytes(item->damage, 2);
    wr_byte(item->depth_first_found);
    wr_byte(item->identification);
}

static void wr_monster(Monster_t *mon) {
    DEBUG(fprintf(logfile, "MONSTER:\n"));
    wr_short((uint16_t) mon->hp);
    wr_short((uint16_t) mon->sleep_count);
    wr_short((uint16_t) mon->speed);
    wr_short(mon->creature_id);
    wr_byte(mon->y);
    wr_byte(mon->x);
    wr_byte(mon->distance_from_player);
    wr_bool(mon->lit);
    wr_byte(mon->stunned_amount);
    wr_byte(mon->confused_amount);
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
    rd_short(&item->id);
    rd_byte(&item->special_name_id);
    rd_string(item->inscription);
    rd_long(&item->flags);
    rd_byte(&item->category_id);
    rd_byte(&item->sprite);
    rd_short((uint16_t *) &item->misc_use);
    rd_long((uint32_t *) &item->cost);
    rd_byte(&item->sub_category_id);
    rd_byte(&item->items_count);
    rd_short(&item->weight);
    rd_short((uint16_t *) &item->to_hit);
    rd_short((uint16_t *) &item->to_damage);
    rd_short((uint16_t *) &item->ac);
    rd_short((uint16_t *) &item->to_ac);
    rd_bytes(item->damage, 2);
    rd_byte(&item->depth_first_found);
    rd_byte(&item->identification);
}

static void rd_monster(Monster_t *mon) {
    DEBUG(fprintf(logfile, "MONSTER:\n"));
    rd_short((uint16_t *) &mon->hp);
    rd_short((uint16_t *) &mon->sleep_count);
    rd_short((uint16_t *) &mon->speed);
    rd_short(&mon->creature_id);
    rd_byte(&mon->y);
    rd_byte(&mon->x);
    rd_byte(&mon->distance_from_player);
    mon->lit = rd_bool();
    rd_byte(&mon->stunned_amount);
    rd_byte(&mon->confused_amount);
}

// functions called from death.c to implement the score file

// set the local fileptr to the score file fileptr
void setFileptr(FILE *file) {
    fileptr = file;
}

void saveHighScore(HighScore_t *score) {
    DEBUG(logfile = fopen("IO_LOG", "a"));
    DEBUG(fprintf(logfile, "Saving score:\n"));

    // Save the encryption byte for robustness.
    wr_byte(xor_byte);

    wr_long((uint32_t) score->points);
    wr_long((uint32_t) score->birth_date);
    wr_short((uint16_t) score->uid);
    wr_short((uint16_t) score->mhp);
    wr_short((uint16_t) score->chp);
    wr_byte(score->dungeon_depth);
    wr_byte(score->level);
    wr_byte(score->deepest_dungeon_depth);
    wr_byte(score->gender);
    wr_byte(score->race);
    wr_byte(score->character_class);
    wr_bytes((uint8_t *) score->name, PLAYER_NAME_SIZE);
    wr_bytes((uint8_t *) score->died_from, 25);
    DEBUG(fclose(logfile));
}

void readHighScore(HighScore_t *score) {
    DEBUG(logfile = fopen("IO_LOG", "a"));
    DEBUG(fprintf(logfile, "Reading score:\n"));

    // Read the encryption byte.
    rd_byte(&xor_byte);

    rd_long((uint32_t *) &score->points);
    rd_long((uint32_t *) &score->birth_date);
    rd_short((uint16_t *) &score->uid);
    rd_short((uint16_t *) &score->mhp);
    rd_short((uint16_t *) &score->chp);
    rd_byte(&score->dungeon_depth);
    rd_byte(&score->level);
    rd_byte(&score->deepest_dungeon_depth);
    rd_byte(&score->gender);
    rd_byte(&score->race);
    rd_byte(&score->character_class);
    rd_bytes((uint8_t *) score->name, PLAYER_NAME_SIZE);
    rd_bytes((uint8_t *) score->died_from, 25);
    DEBUG(fclose(logfile));
}
