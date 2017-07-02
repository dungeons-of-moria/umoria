// Copyright (c) 1989-94 James E. Wilson, Robert A. Koeneke
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Misc code to access files used by Moria

#include "headers.h"

// This must be included after fcntl.h, which has a prototype for `open' on some
// systems.  Otherwise, the `open' prototype conflicts with the `topen' declaration.
#include "externs.h"

//  initializeScoreFile
//  Open the score file while we still have the setuid privileges.  Later
//  when the score is being written out, you must be sure to flock the file
//  so we don't have multiple people trying to write to it at the same time.
//  Craig Norborg (doc)    Mon Aug 10 16:41:59 EST 1987
void initializeScoreFile() {
    highscore_fp = fopen(MORIA_TOP, (char *) "rb+");
    if (highscore_fp == NULL) {
        (void) fprintf(stderr, "Can't open score file \"%s\"\n", MORIA_TOP);
        exit(1);
    }
}

// Attempt to open and print the file containing the intro splash screen text -RAK-
void displaySplashScreen() {
    vtype_t in_line;

    FILE *file1 = fopen(MORIA_MOR, "r");
    if (file1 != NULL) {
        clear_screen();
        for (int i = 0; fgets(in_line, 80, file1) != CNIL; i++) {
            put_buffer(in_line, i, 0);
        }
        pause_line(23);

        (void) fclose(file1);
    }
}

// Open and display a text help file
// File perusal, primitive, but portable -CJS-
void displayTextHelpFile(const char *filename) {
    int max_line_length = 80;
    char line_buffer[max_line_length];

    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        (void) sprintf(line_buffer, "Can not find help file \"%s\".\n", filename);
        prt(line_buffer, 0, 0);
        return;
    }

    save_screen();

    char input;
    while (!feof(file)) {
        clear_screen();

        for (int i = 0; i < 23; i++) {
            if (fgets(line_buffer, max_line_length - 1, file) != CNIL) {
                put_buffer(line_buffer, i, 0);
            }
        }

        prt("[Press any key to continue.]", 23, 23);
        input = inkey();
        if (input == ESCAPE) {
            break;
        }
    }

    (void) fclose(file);

    restore_screen();
}

// Prints a list of random objects to a file. -RAK-
// Note that the objects produced is a sampling of objects
// which be expected to appear on that level.
void outputRandomLevelObjectsToFile() {
    obj_desc_t tmp_str;

    prt("Produce objects on what level?: ", 0, 0);
    if (!get_string(tmp_str, 0, 32, 10)) {
        return;
    }
    int level = atoi(tmp_str);

    prt("Produce how many objects?: ", 0, 0);
    if (!get_string(tmp_str, 0, 27, 10)) {
        return;
    }
    int nobj = atoi(tmp_str);

    bool small_object = get_check("Small objects only?");

    if (nobj > 0 && level > -1 && level < 1201) {
        if (nobj > 10000) {
            nobj = 10000;
        }

        prt("File name: ", 0, 0);
        vtype_t filename1;
        if (get_string(filename1, 0, 11, 64)) {
            if (strlen(filename1) == 0) {
                return;
            }

            FILE *file1 = fopen(filename1, "w");
            if (file1 != NULL) {
                (void) sprintf(tmp_str, "%d", nobj);
                prt(strcat(tmp_str, " random objects being produced..."), 0, 0);

                put_qio();

                (void) fprintf(file1, "*** Random Object Sampling:\n");
                (void) fprintf(file1, "*** %d objects\n", nobj);
                (void) fprintf(file1, "*** For Level %d\n", level);
                (void) fprintf(file1, "\n");
                (void) fprintf(file1, "\n");

                int treasureID = popt();

                for (int i = 0; i < nobj; i++) {
                    int objectID = get_obj_num(level, small_object);
                    inventoryItemCopyTo(sorted_objects[objectID], &treasure_list[treasureID]);

                    magic_treasure(treasureID, level);

                    Inventory_t *i_ptr = &treasure_list[treasureID];
                    itemIdentifyAsStoreBought(i_ptr);

                    if (i_ptr->flags & TR_CURSED) {
                        add_inscribe(i_ptr, ID_DAMD);
                    }

                    itemDescription(tmp_str, i_ptr, true);
                    (void) fprintf(file1, "%d %s\n", i_ptr->level, tmp_str);
                }

                pusht((uint8_t) treasureID);

                (void) fclose(file1);
                prt("Completed.", 0, 0);
            } else {
                prt("File could not be opened.", 0, 0);
            }
        }
    } else {
        prt("Parameters no good.", 0, 0);
    }
}

// Write character sheet to the file
static void writeCharacterSheetToFile(FILE *file1) {
    prt("Writing character sheet...", 0, 0);
    put_qio();

    const char *colon = ":";
    const char *blank = " ";

    vtype_t statDescription;

    (void) fprintf(file1, "%c\n\n", CTRL_KEY('L'));

    (void) fprintf(file1, " Name%9s %-23s", colon, py.misc.name);
    (void) fprintf(file1, " Age%11s %6d", colon, (int) py.misc.age);
    cnv_stat(py.stats.use_stat[A_STR], statDescription);
    (void) fprintf(file1, "   STR : %s\n", statDescription);
    (void) fprintf(file1, " Race%9s %-23s", colon, character_races[py.misc.prace].trace);
    (void) fprintf(file1, " Height%8s %6d", colon, (int) py.misc.ht);
    cnv_stat(py.stats.use_stat[A_INT], statDescription);
    (void) fprintf(file1, "   INT : %s\n", statDescription);
    (void) fprintf(file1, " Sex%10s %-23s", colon, (py.misc.male ? "Male" : "Female"));
    (void) fprintf(file1, " Weight%8s %6d", colon, (int) py.misc.wt);
    cnv_stat(py.stats.use_stat[A_WIS], statDescription);
    (void) fprintf(file1, "   WIS : %s\n", statDescription);
    (void) fprintf(file1, " Class%8s %-23s", colon, classes[py.misc.pclass].title);
    (void) fprintf(file1, " Social Class : %6d", py.misc.sc);
    cnv_stat(py.stats.use_stat[A_DEX], statDescription);
    (void) fprintf(file1, "   DEX : %s\n", statDescription);
    (void) fprintf(file1, " Title%8s %-23s", colon, title_string());
    (void) fprintf(file1, "%22s", blank);
    cnv_stat(py.stats.use_stat[A_CON], statDescription);
    (void) fprintf(file1, "   CON : %s\n", statDescription);
    (void) fprintf(file1, "%34s", blank);
    (void) fprintf(file1, "%26s", blank);
    cnv_stat(py.stats.use_stat[A_CHR], statDescription);
    (void) fprintf(file1, "   CHR : %s\n\n", statDescription);

    (void) fprintf(file1, " + To Hit    : %6d", py.misc.dis_th);
    (void) fprintf(file1, "%7sLevel      : %7d", blank, (int) py.misc.lev);
    (void) fprintf(file1, "    Max Hit Points : %6d\n", py.misc.mhp);
    (void) fprintf(file1, " + To Damage : %6d", py.misc.dis_td);
    (void) fprintf(file1, "%7sExperience : %7d", blank, py.misc.exp);
    (void) fprintf(file1, "    Cur Hit Points : %6d\n", py.misc.chp);
    (void) fprintf(file1, " + To AC     : %6d", py.misc.dis_tac);
    (void) fprintf(file1, "%7sMax Exp    : %7d", blank, py.misc.max_exp);
    (void) fprintf(file1, "    Max Mana%8s %6d\n", colon, py.misc.mana);
    (void) fprintf(file1, "   Total AC  : %6d", py.misc.dis_ac);
    if (py.misc.lev >= MAX_PLAYER_LEVEL) {
        (void) fprintf(file1, "%7sExp to Adv : *******", blank);
    } else {
        (void) fprintf(file1, "%7sExp to Adv : %7d", blank, (int32_t) (player_base_exp_levels[py.misc.lev - 1] * py.misc.expfact / 100));
    }
    (void) fprintf(file1, "    Cur Mana%8s %6d\n", colon, py.misc.cmana);
    (void) fprintf(file1, "%28sGold%8s %7d\n\n", blank, colon, py.misc.au);

    int xbth = py.misc.bth + py.misc.ptohit * BTH_PLUS_ADJ + (class_level_adj[py.misc.pclass][CLA_BTH] * py.misc.lev);
    int xbthb = py.misc.bthb + py.misc.ptohit * BTH_PLUS_ADJ + (class_level_adj[py.misc.pclass][CLA_BTHB] * py.misc.lev);

    // this results in a range from 0 to 29
    int xfos = 40 - py.misc.fos;
    if (xfos < 0) {
        xfos = 0;
    }
    int xsrh = py.misc.srh;

    // this results in a range from 0 to 9
    int xstl = py.misc.stl + 1;
    int xdis = py.misc.disarm + 2 * todis_adj() + stat_adj(A_INT) + (class_level_adj[py.misc.pclass][CLA_DISARM] * py.misc.lev / 3);
    int xsave = py.misc.save + stat_adj(A_WIS) + (class_level_adj[py.misc.pclass][CLA_SAVE] * py.misc.lev / 3);
    int xdev = py.misc.save + stat_adj(A_INT) + (class_level_adj[py.misc.pclass][CLA_DEVICE] * py.misc.lev / 3);

    vtype_t xinfra;
    (void) sprintf(xinfra, "%d feet", py.flags.see_infra * 10);

    (void) fprintf(file1, "(Miscellaneous Abilities)\n\n");
    (void) fprintf(file1, " Fighting    : %-10s", likert(12, xbth));
    (void) fprintf(file1, "   Stealth     : %-10s", likert(1, xstl));
    (void) fprintf(file1, "   Perception  : %s\n", likert(3, xfos));
    (void) fprintf(file1, " Bows/Throw  : %-10s", likert(12, xbthb));
    (void) fprintf(file1, "   Disarming   : %-10s", likert(8, xdis));
    (void) fprintf(file1, "   Searching   : %s\n", likert(6, xsrh));
    (void) fprintf(file1, " Saving Throw: %-10s", likert(6, xsave));
    (void) fprintf(file1, "   Magic Device: %-10s", likert(6, xdev));
    (void) fprintf(file1, "   Infra-Vision: %s\n\n", xinfra);

    // Write out the character's history
    (void) fprintf(file1, "Character Background\n");
    for (int i = 0; i < 4; i++) {
        (void) fprintf(file1, " %s\n", py.misc.history[i]);
    }
}

static const char *equipmentPlacementDescription(int itemID) {
    switch (itemID) {
        case INVEN_WIELD:
            return "You are wielding";
        case INVEN_HEAD:
            return "Worn on head";
        case INVEN_NECK:
            return "Worn around neck";
        case INVEN_BODY:
            return "Worn on body";
        case INVEN_ARM:
            return "Worn on shield arm";
        case INVEN_HANDS:
            return "Worn on hands";
        case INVEN_RIGHT:
            return "Right ring finger";
        case INVEN_LEFT:
            return "Left  ring finger";
        case INVEN_FEET:
            return "Worn on feet";
        case INVEN_OUTER:
            return "Worn about body";
        case INVEN_LIGHT:
            return "Light source is";
        case INVEN_AUX:
            return "Secondary weapon";
        default:
            return "*Unknown value*";
    }
}

// Write out the equipment list.
static void writeEquipmentListToFile(FILE *file1) {
    (void) fprintf(file1, "\n  [Character's Equipment List]\n\n");

    if (equipment_count == 0) {
        (void) fprintf(file1, "  Character has no equipment in use.\n");
        return;
    }

    obj_desc_t description;
    int itemSlotID = 0;

    for (int i = INVEN_WIELD; i < INVEN_ARRAY_SIZE; i++) {
        if (inventory[i].tval == TV_NOTHING) {
            continue;
        }

        itemDescription(description, &inventory[i], true);
        (void) fprintf(file1, "  %c) %-19s: %s\n", itemSlotID + 'a', equipmentPlacementDescription(i), description);

        itemSlotID++;
    }

    (void) fprintf(file1, "%c\n\n", CTRL_KEY('L'));
}

// Write out the character's inventory.
static void writeInventoryToFile(FILE *file1) {
    (void) fprintf(file1, "  [General Inventory List]\n\n");

    if (inventory_count == 0) {
        (void) fprintf(file1, "  Character has no objects in inventory.\n");
        return;
    }

    obj_desc_t description;

    for (int i = 0; i < inventory_count; i++) {
        itemDescription(description, &inventory[i], true);
        (void) fprintf(file1, "%c) %s\n", i + 'a', description);
    }

    (void) fprintf(file1, "%c", CTRL_KEY('L'));
}

// Print the character to a file or device -RAK-
bool file_character(char *filename) {
    vtype_t msg;

    int fd = open(filename, O_WRONLY | O_CREAT | O_EXCL, 0644);
    if (fd < 0 && errno == EEXIST) {
        (void) sprintf(msg, "Replace existing file %s?", filename);
        if (get_check(msg)) {
            fd = open(filename, O_WRONLY, 0644);
        }
    }

    FILE *file;
    if (fd >= 0) {
        // on some non-unix machines, fdopen() is not reliable,
        // hence must call close() and then fopen().
        (void) close(fd);
        file = fopen(filename, "w");
    } else {
        file = NULL;
    }

    if (file == NULL) {
        if (fd >= 0) {
            (void) close(fd);
        }
        (void) sprintf(msg, "Can't open file %s:", filename);
        msg_print(msg);
        return false;
    }

    writeCharacterSheetToFile(file);
    writeEquipmentListToFile(file);
    writeInventoryToFile(file);

    (void) fclose(file);

    prt("Completed.", 0, 0);

    return true;
}
