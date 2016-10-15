/* source/files.c: misc code to access files used by Moria
 *
 * Copyright (c) 1989-94 James E. Wilson, Robert A. Koeneke
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

/* This must be included after fcntl.h, which has a prototype for `open'
 * on some systems.  Otherwise, the `open' prototype conflicts with the
 * `topen' declaration.
 */
#include "externs.h"

/*
 *  init_scorefile
 *  Open the score file while we still have the setuid privileges.  Later
 *  when the score is being written out, you must be sure to flock the file
 *  so we don't have multiple people trying to write to it at the same time.
 *  Craig Norborg (doc)    Mon Aug 10 16:41:59 EST 1987
 */
void init_scorefile() {
    highscore_fp = fopen(MORIA_TOP, "r+");

    if (highscore_fp == NULL) {
        (void)fprintf(stderr, "Can't open score file \"%s\"\n", MORIA_TOP);
        exit(1);
    }
}

/* Attempt to open the intro file      -RAK- */
/* This routine also checks the hours file vs. what time it is  -Doc */
void read_times() {
    vtype in_line;
    int i;
    FILE *file1;

    /* Print the introduction message, news, etc. */
    if ((file1 = fopen(MORIA_MOR, "r")) != NULL) {
        clear_screen();

        for (i = 0; fgets(in_line, 80, file1) != CNIL; i++) {
            put_buffer(in_line, i, 0);
        }
        pause_line(23);
        (void)fclose(file1);
    }
}

/* File perusal.      -CJS-
   primitive, but portable */
void helpfile(char *filename) {
    bigvtype tmp_str;
    FILE *file;
    char input;
    int i;

    file = fopen(filename, "r");
    if (file == NULL) {
        (void)sprintf(tmp_str, "Can not find help file \"%s\".\n", filename);
        prt(tmp_str, 0, 0);
        return;
    }

    save_screen();

    while (!feof(file)) {
        clear_screen();
        for (i = 0; i < 23; i++) {
            if (fgets(tmp_str, BIGVTYPESIZ - 1, file) != CNIL) {
                put_buffer(tmp_str, i, 0);
            }
        }
        prt("[Press any key to continue.]", 23, 23);
        input = inkey();
        if (input == ESCAPE) {
            break;
        }
    }

    (void)fclose(file);
    restore_screen();
}

/* Prints a list of random objects to a file.  Note that -RAK- */
/* the objects produced is a sampling of objects which */
/* be expected to appear on that level. */
void print_objects() {
    int i;
    int nobj, j, level;
    bool small;
    vtype filename1;
    bigvtype tmp_str;
    FILE *file1;
    inven_type *i_ptr;

    prt("Produce objects on what level?: ", 0, 0);
    level = 0;
    if (!get_string(tmp_str, 0, 32, 10)) {
        return;
    }
    level = atoi(tmp_str);
    prt("Produce how many objects?: ", 0, 0);
    nobj = 0;
    if (!get_string(tmp_str, 0, 27, 10)) {
        return;
    }
    nobj = atoi(tmp_str);
    small = get_check("Small objects only?");
    if ((nobj > 0) && (level > -1) && (level < 1201)) {
        if (nobj > 10000) {
            nobj = 10000;
        }

        prt("File name: ", 0, 0);
        if (get_string(filename1, 0, 11, 64)) {
            if (strlen(filename1) == 0) {
                return;
            }

            if ((file1 = fopen(filename1, "w")) != NULL) {
                (void)sprintf(tmp_str, "%d", nobj);
                prt(strcat(tmp_str, " random objects being produced..."), 0, 0);
                put_qio();
                (void)fprintf(file1, "*** Random Object Sampling:\n");
                (void)fprintf(file1, "*** %d objects\n", nobj);
                (void)fprintf(file1, "*** For Level %d\n", level);
                (void)fprintf(file1, "\n");
                (void)fprintf(file1, "\n");
                j = popt();
                for (i = 0; i < nobj; i++) {
                    invcopy(&t_list[j], sorted_objects[get_obj_num(level, small)]);
                    magic_treasure(j, level);
                    i_ptr = &t_list[j];
                    store_bought(i_ptr);

                    if (i_ptr->flags & TR_CURSED) {
                        add_inscribe(i_ptr, ID_DAMD);
                    }

                    objdes(tmp_str, i_ptr, true);
                    (void)fprintf(file1, "%d %s\n", i_ptr->level, tmp_str);
                }
                pusht((uint8_t)j);
                (void)fclose(file1);
                prt("Completed.", 0, 0);
            } else {
                prt("File could not be opened.", 0, 0);
            }
        }
    } else {
        prt("Parameters no good.", 0, 0);
    }
}

/* Print the character to a file or device    -RAK- */
bool file_character(char *filename1) {
    int i;
    int j, xbth, xbthb, xfos, xsrh, xstl, xdis, xsave, xdev;
    vtype xinfra;
    int fd;
    FILE *file1;
    bigvtype prt2;
    struct misc *p_ptr;
    inven_type *i_ptr;
    vtype out_val, prt1;
    char *p, *colon, *blank;

    fd = open(filename1, O_WRONLY | O_CREAT | O_EXCL, 0644);
    if (fd < 0 && errno == EEXIST) {
        (void)sprintf(out_val, "Replace existing file %s?", filename1);
        if (get_check(out_val)) {
            fd = open(filename1, O_WRONLY, 0644);
        }
    }

    if (fd >= 0) {
        /* on some non-unix machines, fdopen() is not reliable, hence must call
           close() and then fopen() */
        (void)close(fd);
        file1 = fopen(filename1, "w");
    } else {
        file1 = NULL;
    }

    if (file1 != NULL) {
        prt("Writing character sheet...", 0, 0);
        put_qio();
        colon = ":";
        blank = " ";

        (void)fprintf(file1, "%c\n\n", CTRL('L'));

        (void)fprintf(file1, " Name%9s %-23s", colon, py.misc.name);
        (void)fprintf(file1, " Age%11s %6d", colon, (int)py.misc.age);
        cnv_stat(py.stats.use_stat[A_STR], prt1);
        (void)fprintf(file1, "   STR : %s\n", prt1);
        (void)fprintf(file1, " Race%9s %-23s", colon, race[py.misc.prace].trace);
        (void)fprintf(file1, " Height%8s %6d", colon, (int)py.misc.ht);
        cnv_stat(py.stats.use_stat[A_INT], prt1);
        (void)fprintf(file1, "   INT : %s\n", prt1);
        (void)fprintf(file1, " Sex%10s %-23s", colon, (py.misc.male ? "Male" : "Female"));
        (void)fprintf(file1, " Weight%8s %6d", colon, (int)py.misc.wt);
        cnv_stat(py.stats.use_stat[A_WIS], prt1);
        (void)fprintf(file1, "   WIS : %s\n", prt1);
        (void)fprintf(file1, " Class%8s %-23s", colon, class[py.misc.pclass].title);
        (void)fprintf(file1, " Social Class : %6d", py.misc.sc);
        cnv_stat(py.stats.use_stat[A_DEX], prt1);
        (void)fprintf(file1, "   DEX : %s\n", prt1);
        (void)fprintf(file1, " Title%8s %-23s", colon, title_string());
        (void)fprintf(file1, "%22s", blank);
        cnv_stat(py.stats.use_stat[A_CON], prt1);
        (void)fprintf(file1, "   CON : %s\n", prt1);
        (void)fprintf(file1, "%34s", blank);
        (void)fprintf(file1, "%26s", blank);
        cnv_stat(py.stats.use_stat[A_CHR], prt1);
        (void)fprintf(file1, "   CHR : %s\n\n", prt1);

        (void)fprintf(file1, " + To Hit    : %6d", py.misc.dis_th);
        (void)fprintf(file1, "%7sLevel      : %7d", blank, (int)py.misc.lev);
        (void)fprintf(file1, "    Max Hit Points : %6d\n", py.misc.mhp);
        (void)fprintf(file1, " + To Damage : %6d", py.misc.dis_td);
        (void)fprintf(file1, "%7sExperience : %7d", blank, py.misc.exp);
        (void)fprintf(file1, "    Cur Hit Points : %6d\n", py.misc.chp);
        (void)fprintf(file1, " + To AC     : %6d", py.misc.dis_tac);
        (void)fprintf(file1, "%7sMax Exp    : %7d", blank, py.misc.max_exp);
        (void)fprintf(file1, "    Max Mana%8s %6d\n", colon, py.misc.mana);
        (void)fprintf(file1, "   Total AC  : %6d", py.misc.dis_ac);
        if (py.misc.lev >= MAX_PLAYER_LEVEL) {
            (void)fprintf(file1, "%7sExp to Adv : *******", blank);
        } else {
            (void)fprintf(
                file1, "%7sExp to Adv : %7d", blank,
                (int32_t)(player_exp[py.misc.lev - 1] * py.misc.expfact / 100));
        }
        (void)fprintf(file1, "    Cur Mana%8s %6d\n", colon, py.misc.cmana);
        (void)fprintf(file1, "%28sGold%8s %7d\n\n", blank, colon, py.misc.au);

        p_ptr = &py.misc;
        xbth = p_ptr->bth + p_ptr->ptohit * BTH_PLUS_ADJ +
               (class_level_adj[p_ptr->pclass][CLA_BTH] * p_ptr->lev);
        xbthb = p_ptr->bthb + p_ptr->ptohit * BTH_PLUS_ADJ +
                (class_level_adj[p_ptr->pclass][CLA_BTHB] * p_ptr->lev);
        /* this results in a range from 0 to 29 */
        xfos = 40 - p_ptr->fos;
        if (xfos < 0) {
            xfos = 0;
        }
        xsrh = p_ptr->srh;
        /* this results in a range from 0 to 9 */
        xstl = p_ptr->stl + 1;
        xdis = p_ptr->disarm + 2 * todis_adj() + stat_adj(A_INT) +
               (class_level_adj[p_ptr->pclass][CLA_DISARM] * p_ptr->lev / 3);
        xsave = p_ptr->save + stat_adj(A_WIS) +
                (class_level_adj[p_ptr->pclass][CLA_SAVE] * p_ptr->lev / 3);
        xdev = p_ptr->save + stat_adj(A_INT) +
               (class_level_adj[p_ptr->pclass][CLA_DEVICE] * p_ptr->lev / 3);

        (void)sprintf(xinfra, "%d feet", py.flags.see_infra * 10);

        (void)fprintf(file1, "(Miscellaneous Abilities)\n\n");
        (void)fprintf(file1, " Fighting    : %-10s", likert(xbth, 12));
        (void)fprintf(file1, "   Stealth     : %-10s", likert(xstl, 1));
        (void)fprintf(file1, "   Perception  : %s\n", likert(xfos, 3));
        (void)fprintf(file1, " Bows/Throw  : %-10s", likert(xbthb, 12));
        (void)fprintf(file1, "   Disarming   : %-10s", likert(xdis, 8));
        (void)fprintf(file1, "   Searching   : %s\n", likert(xsrh, 6));
        (void)fprintf(file1, " Saving Throw: %-10s", likert(xsave, 6));
        (void)fprintf(file1, "   Magic Device: %-10s", likert(xdev, 6));
        (void)fprintf(file1, "   Infra-Vision: %s\n\n", xinfra);

        /* Write out the character's history */
        (void)fprintf(file1, "Character Background\n");
        for (i = 0; i < 4; i++) {
            (void)fprintf(file1, " %s\n", py.misc.history[i]);
        }

        /* Write out the equipment list. */
        j = 0;
        (void)fprintf(file1, "\n  [Character's Equipment List]\n\n");
        if (equip_ctr == 0) {
            (void)fprintf(file1, "  Character has no equipment in use.\n");
        } else {
            for (i = INVEN_WIELD; i < INVEN_ARRAY_SIZE; i++) {
                i_ptr = &inventory[i];
                if (i_ptr->tval != TV_NOTHING) {
                    switch (i) {
                    case INVEN_WIELD:
                        p = "You are wielding";
                        break;
                    case INVEN_HEAD:
                        p = "Worn on head";
                        break;
                    case INVEN_NECK:
                        p = "Worn around neck";
                        break;
                    case INVEN_BODY:
                        p = "Worn on body";
                        break;
                    case INVEN_ARM:
                        p = "Worn on shield arm";
                        break;
                    case INVEN_HANDS:
                        p = "Worn on hands";
                        break;
                    case INVEN_RIGHT:
                        p = "Right ring finger";
                        break;
                    case INVEN_LEFT:
                        p = "Left  ring finger";
                        break;
                    case INVEN_FEET:
                        p = "Worn on feet";
                        break;
                    case INVEN_OUTER:
                        p = "Worn about body";
                        break;
                    case INVEN_LIGHT:
                        p = "Light source is";
                        break;
                    case INVEN_AUX:
                        p = "Secondary weapon";
                        break;
                    default:
                        p = "*Unknown value*";
                        break;
                    }
                    objdes(prt2, &inventory[i], true);
                    (void)fprintf(file1, "  %c) %-19s: %s\n", j + 'a', p, prt2);
                    j++;
                }
            }
        }

        /* Write out the character's inventory. */
        (void)fprintf(file1, "%c\n\n", CTRL('L'));

        (void)fprintf(file1, "  [General Inventory List]\n\n");
        if (inven_ctr == 0) {
            (void)fprintf(file1, "  Character has no objects in inventory.\n");
        } else {
            for (i = 0; i < inven_ctr; i++) {
                objdes(prt2, &inventory[i], true);
                (void)fprintf(file1, "%c) %s\n", i + 'a', prt2);
            }
        }

        (void)fprintf(file1, "%c", CTRL('L'));
        (void)fclose(file1);

        prt("Completed.", 0, 0);
        return true;
    } else {
        if (fd >= 0) {
            (void)close(fd);
        }
        (void)sprintf(out_val, "Can't open file %s:", filename1);
        msg_print(out_val);
        return false;
    }
}
