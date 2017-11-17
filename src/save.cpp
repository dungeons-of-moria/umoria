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

#include <sstream>

// For debugging the save file code on systems with broken compilers.
#define DEBUG(x)

DEBUG(static FILE *logfile)

static bool _save_char(char *filename);
static bool sv_write();
static void wr_bool(bool value);
static void wr_byte(uint8_t value);
static void wr_short(uint16_t value);
static void wr_long(uint32_t value);
static void wr_bytes(uint8_t *value, int count);
static void wr_string(char *str);
static void wr_shorts(uint16_t *value, int count);
static void wr_item(Inventory_t &item);
static void wr_monster(Monster_t &monster);
static bool rd_bool();
static uint8_t rd_byte();
static uint16_t rd_short();
static uint32_t rd_long();
static void rd_bytes(uint8_t *value, int count);
static void rd_string(char *str);
static void rd_shorts(uint16_t *value, int count);
static void rd_item(Inventory_t &item);
static void rd_monster(Monster_t &monster);

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
    if (eof_flag != 0) {
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
        Recall_t &r = creature_recall[i];
        if (r.movement || r.defenses || r.kills || r.spells || r.deaths || r.attacks[0] || r.attacks[1] || r.attacks[2] || r.attacks[3]) {
            wr_short((uint16_t) i);
            wr_long(r.movement);
            wr_long(r.spells);
            wr_short(r.kills);
            wr_short(r.deaths);
            wr_short(r.defenses);
            wr_byte(r.wake);
            wr_byte(r.ignore);
            wr_bytes(r.attacks, MON_MAX_ATTACKS);
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
    wr_short(py.misc.exp_fraction);
    wr_short(py.misc.age);
    wr_short(py.misc.height);
    wr_short(py.misc.weight);
    wr_short(py.misc.level);
    wr_short(py.misc.max_dungeon_depth);
    wr_short((uint16_t) py.misc.chance_in_search);
    wr_short((uint16_t) py.misc.fos);
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
    for (auto &entry : py.misc.history) {
        wr_string(entry);
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
    wr_long((uint32_t) dg.game_turn);
    wr_short((uint16_t) inventory_count);
    for (int i = 0; i < inventory_count; i++) {
        wr_item(inventory[i]);
    }
    for (int i = EQUIPMENT_WIELD; i < PLAYER_INVENTORY_SIZE; i++) {
        wr_item(inventory[i]);
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
    for (auto &message : messages) {
        wr_string(message);
    }

    // this indicates 'cheating' if it is a one
    wr_short((uint16_t) panic_save);
    wr_short((uint16_t) total_winner);
    wr_short((uint16_t) noscore);
    wr_shorts(player_base_hp_levels, PLAYER_MAX_LEVEL);

    for (auto &store : stores) {
        wr_long((uint32_t) store.turns_left_before_closing);
        wr_short((uint16_t) store.insults_counter);
        wr_byte(store.owner_id);
        wr_byte(store.unique_items_counter);
        wr_short(store.good_purchases);
        wr_short(store.bad_purchases);
        for (int j = 0; j < store.unique_items_counter; j++) {
            wr_long((uint32_t) store.inventory[j].cost);
            wr_item(store.inventory[j].item);
        }
    }

    // save the current time in the save file
    l = getCurrentUnixTime();

    if (l < start_time) {
        // someone is messing with the clock!,
        // assume that we have been playing for 1 day
        l = (uint32_t) (start_time + 86400L);
    }
    wr_long(l);

    // put character_died_from string in save file
    wr_string(character_died_from);

    // put the character_max_score in the save file
    l = (uint32_t) (playerCalculateTotalPoints());
    wr_long(l);

    // put the date_of_birth in the save file
    wr_long((uint32_t) py.misc.date_of_birth);

    // only level specific info follows, this allows characters to be
    // resurrected, the dungeon level info is not needed for a resurrection
    if (character_is_dead) {
        return !((ferror(fileptr) != 0) || fflush(fileptr) == EOF);
    }

    wr_short((uint16_t) dg.current_level);
    wr_short((uint16_t) char_row);
    wr_short((uint16_t) char_col);
    wr_short((uint16_t) monster_multiply_total);
    wr_short((uint16_t) dg.height);
    wr_short((uint16_t) dg.width);
    wr_short((uint16_t) dg.max_panel_rows);
    wr_short((uint16_t) dg.max_panel_cols);

    for (int i = 0; i < MAX_HEIGHT; i++) {
        for (int j = 0; j < MAX_WIDTH; j++) {
            if (dg.cave[i][j].creature_id != 0) {
                wr_byte((uint8_t) i);
                wr_byte((uint8_t) j);
                wr_byte(dg.cave[i][j].creature_id);
            }
        }
    }

    // marks end of creature_id info
    wr_byte((uint8_t) 0xFF);

    for (int i = 0; i < MAX_HEIGHT; i++) {
        for (int j = 0; j < MAX_WIDTH; j++) {
            if (dg.cave[i][j].treasure_id != 0) {
                wr_byte((uint8_t) i);
                wr_byte((uint8_t) j);
                wr_byte(dg.cave[i][j].treasure_id);
            }
        }
    }

    // marks end of treasure_id info
    wr_byte((uint8_t) 0xFF);

    // must set counter to zero, note that code may write out two bytes unnecessarily
    int count = 0;
    uint8_t prev_char = 0;

    for (int y = 0; y < MAX_HEIGHT; y++) {
        for (int x = 0; x < MAX_WIDTH; x++) {
            Cave_t &tile = dg.cave[y][x];

            auto char_tmp = (uint8_t) (tile.feature_id | (tile.perma_lit_room << 4) | (tile.field_mark << 5) | (tile.permanent_light << 6) | (tile.temporary_light << 7));

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
        wr_item(treasure_list[i]);
    }
    wr_short((uint16_t) next_free_monster_id);
    for (int i = MON_MIN_INDEX_ID; i < next_free_monster_id; i++) {
        wr_monster(monsters[i]);
    }

    return !((ferror(fileptr) != 0) || fflush(fileptr) == EOF);
}

// Set up prior to actual save, do the save, then clean up
bool saveGame() {
    vtype_t input = {'\0'};
    std::string output;

    while (!_save_char(config.save_game_filename)) {
        output = "Save file '" + std::string(config.save_game_filename) + "' fails.";
        printMessage(output.c_str());

        int i = 0;
        if (access(config.save_game_filename, 0) < 0 || !getInputConfirmation("File exists. Delete old save file?") || (i = unlink(config.save_game_filename)) < 0) {
            if (i < 0) {
                output = "Can't delete '" + std::string(config.save_game_filename) + "'";
                printMessage(output.c_str());
            }
            putStringClearToEOL("New Save file [ESC to give up]:", Coord_t{0, 0});
            if (!getStringInput(input, Coord_t{0, 31}, 45)) {
                return false;
            }
            if (input[0] != 0) {
                (void) strcpy(config.save_game_filename, input);
            }
        }
        output = "Saving with '" + std::string(config.save_game_filename) + "'...";
        putStringClearToEOL(output.c_str(), Coord_t{0, 0});
    }

    return true;
}

static bool _save_char(char *filename) {
    if (character_saved) {
        return true; // Nothing to save.
    }

    putQIO();
    playerDisturb(1, 0);                   // Turn off resting and searching.
    playerChangeSpeed(-py.pack_heaviness); // Fix the speed
    py.pack_heaviness = 0;
    bool ok = false;

    fileptr = nullptr; // Do not assume it has been init'ed

    int fd = open(filename, O_RDWR | O_CREAT | O_EXCL, 0600);

    if (fd < 0 && access(filename, 0) >= 0 && ((from_savefile != 0) || (wizard_mode && getInputConfirmation("Can't make new save file. Overwrite old?")))) {
        (void) chmod(filename, 0600);
        fd = open(filename, O_RDWR | O_TRUNC, 0600);
    }

    if (fd >= 0) {
        (void) close(fd);
        fileptr = fopen(config.save_game_filename, "wb");
    }

    DEBUG(logfile = fopen("IO_LOG", "a"));
    DEBUG(fprintf(logfile, "Saving data to %s\n", config.save_game_filename));

    if (fileptr != nullptr) {
        xor_byte = 0;
        wr_byte(CURRENT_VERSION_MAJOR);
        xor_byte = 0;
        wr_byte(CURRENT_VERSION_MINOR);
        xor_byte = 0;
        wr_byte(CURRENT_VERSION_PATCH);
        xor_byte = 0;

        auto char_tmp = (uint8_t) (randomNumber(256) - 1);
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
            (void) unlink(filename);
        }

        std::string output;
        if (fd >= 0) {
            output = "Error writing to file '" + std::string(filename) + "'";
        } else {
            output = "Can't create new file '" + std::string(filename) + "'";
        }
        printMessage(output.c_str());

        return false;
    }

    character_saved = true;
    dg.game_turn = -1;

    return true;
}

// Certain checks are omitted for the wizard. -CJS-
bool loadGame(bool &generate) {
    Cave_t *tile;
    int c;
    uint32_t time_saved = 0;
    uint8_t version_maj = 0;
    uint8_t version_min = 0;
    uint8_t patch_level = 0;

    generate = true;
    int fd = -1;
    int total_count = 0;

    // Not required for Mac, because the file name is obtained through a dialog.
    // There is no way for a nonexistent file to be specified. -BS-
    if (access(config.save_game_filename, 0) != 0) {
        printMessage("Save file does not exist.");
        return false; // Don't bother with messages here. File absent.
    }

    clearScreen();

    std::string filename = "Save file '" + std::string(config.save_game_filename) + "' present. Attempting restore.";
    putString(filename.c_str(), Coord_t{23, 0});

    // FIXME: check this if/else logic! -- MRC
    if (dg.game_turn >= 0) {
        printMessage("IMPOSSIBLE! Attempt to restore while still alive!");
    } else if ((fd = open(config.save_game_filename, O_RDONLY, 0)) < 0 && (chmod(config.save_game_filename, 0400) < 0 || (fd = open(config.save_game_filename, O_RDONLY, 0)) < 0)) {
        // Allow restoring a file belonging to someone else, if we can delete it.
        // Hence first try to read without doing a chmod.

        printMessage("Can't open file for reading.");
    } else {
        dg.game_turn = -1;
        bool ok = true;

        (void) close(fd);
        fd = -1; // Make sure it isn't closed again
        fileptr = fopen(config.save_game_filename, "rb");

        if (fileptr == nullptr) {
            goto error;
        }

        putStringClearToEOL("Restoring Memory...", Coord_t{0, 0});
        putQIO();

        DEBUG(logfile = fopen("IO_LOG", "a"));
        DEBUG(fprintf(logfile, "Reading data from %s\n", config.save_game_filename));

        // Note: setting these xor_byte is correct!
        xor_byte = 0;
        version_maj = rd_byte();
        xor_byte = 0;
        version_min = rd_byte();
        xor_byte = 0;
        patch_level = rd_byte();
        xor_byte = 0;
        xor_byte = rd_byte();

        if (!validGameVersion(version_maj, version_min, patch_level)) {
            putStringClearToEOL("Sorry. This save file is from a different version of umoria.", Coord_t{2, 0});
            goto error;
        }

        uint16_t uint16_t_tmp;
        uint32_t l;

        uint16_t_tmp = rd_short();
        while (uint16_t_tmp != 0xFFFF) {
            if (uint16_t_tmp >= MON_MAX_CREATURES) {
                goto error;
            }
            Recall_t &memory = creature_recall[uint16_t_tmp];
            memory.movement = rd_long();
            memory.spells = rd_long();
            memory.kills = rd_short();
            memory.deaths = rd_short();
            memory.defenses = rd_short();
            memory.wake = rd_byte();
            memory.ignore = rd_byte();
            rd_bytes(memory.attacks, MON_MAX_ATTACKS);
            uint16_t_tmp = rd_short();
        }

        l = rd_long();

        config.run_cut_corners = (l & 0x1) != 0;
        config.run_examine_corners = (l & 0x2) != 0;
        config.run_print_self = (l & 0x4) != 0;
        config.find_bound = (l & 0x8) != 0;
        config.prompt_to_pickup = (l & 0x10) != 0;
        config.use_roguelike_keys = (l & 0x20) != 0;
        config.show_inventory_weights = (l & 0x40) != 0;
        config.highlight_seams = (l & 0x80) != 0;
        config.run_ignore_doors = (l & 0x100) != 0;
        config.error_beep_sound = (l & 0x200) != 0;
        config.display_counts = (l & 0x400) != 0;

        // Don't allow resurrection of total_winner characters.  It causes
        // problems because the character level is out of the allowed range.
        if (to_be_wizard && ((l & 0x40000000L) != 0)) {
            printMessage("Sorry, this character is retired from moria.");
            printMessage("You can not resurrect a retired character.");
        } else if (to_be_wizard && ((l & 0x80000000L) != 0) && getInputConfirmation("Resurrect a dead character?")) {
            l &= ~0x80000000L;
        }

        if ((l & 0x80000000L) == 0) {
            rd_string(py.misc.name);
            py.misc.gender = rd_bool();
            py.misc.au = rd_long();
            py.misc.max_exp = rd_long();
            py.misc.exp = rd_long();
            py.misc.exp_fraction = rd_short();
            py.misc.age = rd_short();
            py.misc.height = rd_short();
            py.misc.weight = rd_short();
            py.misc.level = rd_short();
            py.misc.max_dungeon_depth = rd_short();
            py.misc.chance_in_search = rd_short();
            py.misc.fos = rd_short();
            py.misc.bth = rd_short();
            py.misc.bth_with_bows = rd_short();
            py.misc.mana = rd_short();
            py.misc.max_hp = rd_short();
            py.misc.plusses_to_hit = rd_short();
            py.misc.plusses_to_damage = rd_short();
            py.misc.ac = rd_short();
            py.misc.magical_ac = rd_short();
            py.misc.display_to_hit = rd_short();
            py.misc.display_to_damage = rd_short();
            py.misc.display_ac = rd_short();
            py.misc.display_to_ac = rd_short();
            py.misc.disarm = rd_short();
            py.misc.saving_throw = rd_short();
            py.misc.social_class = rd_short();
            py.misc.stealth_factor = rd_short();
            py.misc.class_id = rd_byte();
            py.misc.race_id = rd_byte();
            py.misc.hit_die = rd_byte();
            py.misc.experience_factor = rd_byte();
            py.misc.current_mana = rd_short();
            py.misc.current_mana_fraction = rd_short();
            py.misc.current_hp = rd_short();
            py.misc.current_hp_fraction = rd_short();
            for (auto &entry : py.misc.history) {
                rd_string(entry);
            }

            rd_bytes(py.stats.max, 6);
            rd_bytes(py.stats.current, 6);
            rd_shorts((uint16_t *) py.stats.modified, 6);
            rd_bytes(py.stats.used, 6);

            py.flags.status = rd_long();
            py.flags.rest = rd_short();
            py.flags.blind = rd_short();
            py.flags.paralysis = rd_short();
            py.flags.confused = rd_short();
            py.flags.food = rd_short();
            py.flags.food_digested = rd_short();
            py.flags.protection = rd_short();
            py.flags.speed = rd_short();
            py.flags.fast = rd_short();
            py.flags.slow = rd_short();
            py.flags.afraid = rd_short();
            py.flags.poisoned = rd_short();
            py.flags.image = rd_short();
            py.flags.protect_evil = rd_short();
            py.flags.invulnerability = rd_short();
            py.flags.heroism = rd_short();
            py.flags.super_heroism = rd_short();
            py.flags.blessed = rd_short();
            py.flags.heat_resistance = rd_short();
            py.flags.cold_resistance = rd_short();
            py.flags.detect_invisible = rd_short();
            py.flags.word_of_recall = rd_short();
            py.flags.see_infra = rd_short();
            py.flags.timed_infra = rd_short();
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
            py.flags.new_spells_to_learn = rd_byte();

            missiles_counter = rd_short();
            dg.game_turn = rd_long();
            inventory_count = rd_short();
            if (inventory_count > EQUIPMENT_WIELD) {
                goto error;
            }
            for (int i = 0; i < inventory_count; i++) {
                rd_item(inventory[i]);
            }
            for (int i = EQUIPMENT_WIELD; i < PLAYER_INVENTORY_SIZE; i++) {
                rd_item(inventory[i]);
            }
            inventory_weight = rd_short();
            equipment_count = rd_short();
            spells_learnt = rd_long();
            spells_worked = rd_long();
            spells_forgotten = rd_long();
            rd_bytes(spells_learned_order, 32);
            rd_bytes(objects_identified, OBJECT_IDENT_SIZE);
            magic_seed = rd_long();
            town_seed = rd_long();
            last_message_id = rd_short();
            for (auto &message : messages) {
                rd_string(message);
            }

            uint16_t panic_save_short;
            uint16_t total_winner_short;
            panic_save_short = rd_short();
            total_winner_short = rd_short();
            panic_save = panic_save_short != 0;
            total_winner = total_winner_short != 0;

            noscore = rd_short();
            rd_shorts(player_base_hp_levels, PLAYER_MAX_LEVEL);

            for (auto &store : stores) {
                store.turns_left_before_closing = rd_long();
                store.insults_counter = rd_short();
                store.owner_id = rd_byte();
                store.unique_items_counter = rd_byte();
                store.good_purchases = rd_short();
                store.bad_purchases = rd_short();
                if (store.unique_items_counter > STORE_MAX_DISCRETE_ITEMS) {
                    goto error;
                }
                for (int j = 0; j < store.unique_items_counter; j++) {
                    store.inventory[j].cost = rd_long();
                    rd_item(store.inventory[j].item);
                }
            }

            time_saved = rd_long();
            rd_string(character_died_from);
            character_max_score = rd_long();
            py.misc.date_of_birth = rd_long();
        }

        c = getc(fileptr);
        if (c == EOF || ((l & 0x80000000L) != 0)) {
            if ((l & 0x80000000L) == 0) {
                if (!to_be_wizard || dg.game_turn < 0) {
                    goto error;
                }
                putStringClearToEOL("Attempting a resurrection!", Coord_t{0, 0});
                if (py.misc.current_hp < 0) {
                    py.misc.current_hp = 0;
                    py.misc.current_hp_fraction = 0;
                }

                // don't let them starve to death immediately
                if (py.flags.food < 0) {
                    py.flags.food = 0;
                }

                // don't let them immediately die of poison again
                if (py.flags.poisoned > 1) {
                    py.flags.poisoned = 1;
                }

                dg.current_level = 0; // Resurrect on the town level.
                character_generated = true;

                // set `noscore` to indicate a resurrection, and don't enter wizard mode
                to_be_wizard = false;
                noscore |= 0x1;
            } else {
                // Make sure that this message is seen, since it is a bit
                // more interesting than the other messages.
                printMessage("Restoring Memory of a departed spirit...");
                dg.game_turn = -1;
            }
            putQIO();
            goto closefiles;
        }
        if (ungetc(c, fileptr) == EOF) {
            goto error;
        }

        putStringClearToEOL("Restoring Character...", Coord_t{0, 0});
        putQIO();

        // only level specific info should follow,
        // not present for dead characters

        dg.current_level = rd_short();
        char_row = rd_short();
        char_col = rd_short();
        monster_multiply_total = rd_short();
        dg.height = rd_short();
        dg.width = rd_short();
        dg.max_panel_rows = rd_short();
        dg.max_panel_cols = rd_short();

        uint8_t char_tmp, ychar, xchar, count;

        // read in the creature ptr info
        char_tmp = rd_byte();
        while (char_tmp != 0xFF) {
            ychar = char_tmp;
            xchar = rd_byte();
            char_tmp = rd_byte();
            if (xchar > MAX_WIDTH || ychar > MAX_HEIGHT) {
                goto error;
            }
            dg.cave[ychar][xchar].creature_id = char_tmp;
            char_tmp = rd_byte();
        }

        // read in the treasure ptr info
        char_tmp = rd_byte();
        while (char_tmp != 0xFF) {
            ychar = char_tmp;
            xchar = rd_byte();
            char_tmp = rd_byte();
            if (xchar > MAX_WIDTH || ychar > MAX_HEIGHT) {
                goto error;
            }
            dg.cave[ychar][xchar].treasure_id = char_tmp;
            char_tmp = rd_byte();
        }

        // read in the rest of the cave info
        tile = &dg.cave[0][0];
        total_count = 0;
        while (total_count != MAX_HEIGHT * MAX_WIDTH) {
            count = rd_byte();
            char_tmp = rd_byte();
            for (int i = count; i > 0; i--) {
                if (tile >= &dg.cave[MAX_HEIGHT][0]) {
                    goto error;
                }
                tile->feature_id = (uint8_t) (char_tmp & 0xF);
                tile->perma_lit_room = (bool) ((char_tmp >> 4) & 0x1);
                tile->field_mark = (bool) ((char_tmp >> 5) & 0x1);
                tile->permanent_light = (bool) ((char_tmp >> 6) & 0x1);
                tile->temporary_light = (bool) ((char_tmp >> 7) & 0x1);
                tile++;
            }
            total_count += count;
        }

        current_treasure_id = rd_short();
        if (current_treasure_id > LEVEL_MAX_OBJECTS) {
            goto error;
        }
        for (int i = MIN_TREASURE_LIST_ID; i < current_treasure_id; i++) {
            rd_item(treasure_list[i]);
        }
        next_free_monster_id = rd_short();
        if (next_free_monster_id > MON_TOTAL_ALLOCATIONS) {
            goto error;
        }
        for (int i = MON_MIN_INDEX_ID; i < next_free_monster_id; i++) {
            rd_monster(monsters[i]);
        }

        generate = false; // We have restored a cave - no need to generate.

        if (ferror(fileptr) != 0) {
            goto error;
        }

        if (dg.game_turn < 0) {
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

        if (fileptr != nullptr) {
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
                printMessage("This game is from a panic save.  Score will not be added to scoreboard.");
            } else if ((!noscore) & 0x04) {
                printMessage("This character is already on the scoreboard; it will not be scored again.");
                noscore |= 0x4;
            }

            if (dg.game_turn >= 0) { // Only if a full restoration.
                py.weapon_is_heavy = false;
                py.pack_heaviness = 0;
                playerStrength();

                // rotate store inventory, depending on how old the save file
                // is foreach day old (rounded up), call storeMaintenance
                // calculate age in seconds
                start_time = getCurrentUnixTime();

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

            if (noscore != 0) {
                printMessage("This save file cannot be used to get on the score board.");
            }
            if (validGameVersion(version_maj, version_min, patch_level) && !isCurrentGameVersion(version_maj, version_min, patch_level)) {
                std::string msg = "Save file version ";
                msg += std::to_string(version_maj) + "." + std::to_string(version_min);
                msg += " accepted on game version ";
                msg += std::to_string(CURRENT_VERSION_MAJOR) + "." + std::to_string(CURRENT_VERSION_MINOR) + ".";
                printMessage(msg.c_str());
            }

            // if false: only restored options and monster memory.
            return dg.game_turn >= 0;
        }
    }
    dg.game_turn = -1;
    putStringClearToEOL("Please try again without that save file.", Coord_t{1, 0});

    exitGame();

    return false; // not reached
}

static void wr_bool(bool value) {
    wr_byte((uint8_t) value);
}

static void wr_byte(uint8_t value) {
    xor_byte ^= value;
    (void) putc((int) xor_byte, fileptr);
    DEBUG(fprintf(logfile, "BYTE:  %02X = %d\n", (int) xor_byte, (int) value));
}

static void wr_short(uint16_t value) {
    xor_byte ^= (value & 0xFF);
    (void) putc((int) xor_byte, fileptr);
    DEBUG(fprintf(logfile, "SHORT: %02X", (int) xor_byte));
    xor_byte ^= ((value >> 8) & 0xFF);
    (void) putc((int) xor_byte, fileptr);
    DEBUG(fprintf(logfile, " %02X = %d\n", (int) xor_byte, (int) value));
}

static void wr_long(uint32_t value) {
    xor_byte ^= (value & 0xFF);
    (void) putc((int) xor_byte, fileptr);
    DEBUG(fprintf(logfile, "LONG:  %02X", (int) xor_byte));
    xor_byte ^= ((value >> 8) & 0xFF);
    (void) putc((int) xor_byte, fileptr);
    DEBUG(fprintf(logfile, " %02X", (int) xor_byte));
    xor_byte ^= ((value >> 16) & 0xFF);
    (void) putc((int) xor_byte, fileptr);
    DEBUG(fprintf(logfile, " %02X", (int) xor_byte));
    xor_byte ^= ((value >> 24) & 0xFF);
    (void) putc((int) xor_byte, fileptr);
    DEBUG(fprintf(logfile, " %02X = %ld\n", (int) xor_byte, (int32_t) value));
}

static void wr_bytes(uint8_t *value, int count) {
    uint8_t *ptr;

    DEBUG(fprintf(logfile, "%d BYTES:", count));
    ptr = value;
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

static void wr_shorts(uint16_t *value, int count) {
    DEBUG(fprintf(logfile, "%d SHORTS:", count));

    uint16_t *sptr = value;

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

static void wr_item(Inventory_t &item) {
    DEBUG(fprintf(logfile, "ITEM:\n"));
    wr_short(item.id);
    wr_byte(item.special_name_id);
    wr_string(item.inscription);
    wr_long(item.flags);
    wr_byte(item.category_id);
    wr_byte(item.sprite);
    wr_short((uint16_t) item.misc_use);
    wr_long((uint32_t) item.cost);
    wr_byte(item.sub_category_id);
    wr_byte(item.items_count);
    wr_short(item.weight);
    wr_short((uint16_t) item.to_hit);
    wr_short((uint16_t) item.to_damage);
    wr_short((uint16_t) item.ac);
    wr_short((uint16_t) item.to_ac);
    wr_bytes(item.damage, 2);
    wr_byte(item.depth_first_found);
    wr_byte(item.identification);
}

static void wr_monster(Monster_t &monster) {
    DEBUG(fprintf(logfile, "MONSTER:\n"));
    wr_short((uint16_t) monster.hp);
    wr_short((uint16_t) monster.sleep_count);
    wr_short((uint16_t) monster.speed);
    wr_short(monster.creature_id);
    wr_byte(monster.y);
    wr_byte(monster.x);
    wr_byte(monster.distance_from_player);
    wr_bool(monster.lit);
    wr_byte(monster.stunned_amount);
    wr_byte(monster.confused_amount);
}

static bool rd_bool() {
    return (bool) rd_byte();
}

static uint8_t rd_byte() {
    auto c = (uint8_t) (getc(fileptr) & 0xFF);
    uint8_t decoded_byte = c ^xor_byte;
    xor_byte = c;

    DEBUG(fprintf(logfile, "BYTE:  %02X = %d\n", (int) c, decoded_byte));

    return decoded_byte;
}

static uint16_t rd_short() {
    auto c = (uint8_t) (getc(fileptr) & 0xFF);
    uint16_t decoded_int = c ^xor_byte;

    xor_byte = (uint8_t) (getc(fileptr) & 0xFF);
    decoded_int |= (uint16_t) (c ^ xor_byte) << 8;

    DEBUG(fprintf(logfile, "SHORT: %02X %02X = %d\n", (int) c, (int) xor_byte, decoded_int));

    return decoded_int;
}

static uint32_t rd_long() {
    auto c = (uint8_t) (getc(fileptr) & 0xFF);
    uint32_t decoded_long = c ^xor_byte;

    xor_byte = (uint8_t) (getc(fileptr) & 0xFF);
    decoded_long |= (uint32_t) (c ^ xor_byte) << 8;
    DEBUG(fprintf(logfile, "LONG:  %02X %02X ", (int) c, (int) xor_byte));

    c = (uint8_t) (getc(fileptr) & 0xFF);
    decoded_long |= (uint32_t) (c ^ xor_byte) << 16;

    xor_byte = (uint8_t) (getc(fileptr) & 0xFF);
    decoded_long |= (uint32_t) (c ^ xor_byte) << 24;
    DEBUG(fprintf(logfile, "%02X %02X = %ld\n", (int) c, (int) xor_byte, decoded_long));

    return decoded_long;
}

static void rd_bytes(uint8_t *value, int count) {
    DEBUG(fprintf(logfile, "%d BYTES:", count));
    uint8_t *ptr = value;
    for (int i = 0; i < count; i++) {
        auto c = (uint8_t) (getc(fileptr) & 0xFF);
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
        auto c = (uint8_t) (getc(fileptr) & 0xFF);
        *str = c ^ xor_byte;
        xor_byte = c;
        DEBUG(fprintf(logfile, "%02X ", (int) c));
    } while (*str++ != '\0');
    DEBUG(fprintf(logfile, "= \"%s\"\n", s));
}

static void rd_shorts(uint16_t *value, int count) {
    DEBUG(fprintf(logfile, "%d SHORTS:", count));
    uint16_t *sptr = value;

    for (int i = 0; i < count; i++) {
        auto c = (uint8_t) (getc(fileptr) & 0xFF);
        uint16_t s = c ^xor_byte;
        xor_byte = (uint8_t) (getc(fileptr) & 0xFF);
        s |= (uint16_t) (c ^ xor_byte) << 8;
        *sptr++ = s;
        DEBUG(fprintf(logfile, "  %02X %02X = %d", (int) c, (int) xor_byte, (int) s));
    }
    DEBUG(fprintf(logfile, "\n"));
}

static void rd_item(Inventory_t &item) {
    DEBUG(fprintf(logfile, "ITEM:\n"));
    item.id = rd_short();
    item.special_name_id = rd_byte();
    rd_string(item.inscription);
    item.flags = rd_long();
    item.category_id = rd_byte();
    item.sprite = rd_byte();
    item.misc_use = rd_short();
    item.cost = rd_long();
    item.sub_category_id = rd_byte();
    item.items_count = rd_byte();
    item.weight = rd_short();
    item.to_hit = rd_short();
    item.to_damage = rd_short();
    item.ac = rd_short();
    item.to_ac = rd_short();
    rd_bytes(item.damage, 2);
    item.depth_first_found = rd_byte();
    item.identification = rd_byte();
}

static void rd_monster(Monster_t &monster) {
    DEBUG(fprintf(logfile, "MONSTER:\n"));
    monster.hp = rd_short();
    monster.sleep_count = rd_short();
    monster.speed = rd_short();
    monster.creature_id = rd_short();
    monster.y = rd_byte();
    monster.x = rd_byte();
    monster.distance_from_player = rd_byte();
    monster.lit = rd_bool();
    monster.stunned_amount = rd_byte();
    monster.confused_amount = rd_byte();
}

// functions called from death.c to implement the score file

// set the local fileptr to the score file fileptr
void setFileptr(FILE *file) {
    fileptr = file;
}

void saveHighScore(const HighScore_t &score) {
    DEBUG(logfile = fopen("IO_LOG", "a"));
    DEBUG(fprintf(logfile, "Saving score:\n"));

    // Save the encryption byte for robustness.
    wr_byte(xor_byte);

    wr_long((uint32_t) score.points);
    wr_long((uint32_t) score.birth_date);
    wr_short((uint16_t) score.uid);
    wr_short((uint16_t) score.mhp);
    wr_short((uint16_t) score.chp);
    wr_byte(score.dungeon_depth);
    wr_byte(score.level);
    wr_byte(score.deepest_dungeon_depth);
    wr_byte(score.gender);
    wr_byte(score.race);
    wr_byte(score.character_class);
    wr_bytes((uint8_t *) score.name, PLAYER_NAME_SIZE);
    wr_bytes((uint8_t *) score.died_from, 25);
    DEBUG(fclose(logfile));
}

void readHighScore(HighScore_t &score) {
    DEBUG(logfile = fopen("IO_LOG", "a"));
    DEBUG(fprintf(logfile, "Reading score:\n"));

    // Read the encryption byte.
    xor_byte = rd_byte();

    score.points = rd_long();
    score.birth_date = rd_long();
    score.uid = rd_short();
    score.mhp = rd_short();
    score.chp = rd_short();
    score.dungeon_depth = rd_byte();
    score.level = rd_byte();
    score.deepest_dungeon_depth = rd_byte();
    score.gender = rd_byte();
    score.race = rd_byte();
    score.character_class = rd_byte();
    rd_bytes((uint8_t *) score.name, PLAYER_NAME_SIZE);
    rd_bytes((uint8_t *) score.died_from, 25);
    DEBUG(fclose(logfile));
}
