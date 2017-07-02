// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Version history and info, and wizard mode debugging aids.

#include "headers.h"
#include "externs.h"

// Light up the dungeon -RAK-
void wizard_light() {
    bool flag;

    flag = !cave[char_row][char_col].pl;

    for (int i = 0; i < dungeon_height; i++) {
        for (int j = 0; j < dungeon_width; j++) {
            if (cave[i][j].fval <= MAX_CAVE_FLOOR) {
                for (int k = i - 1; k <= i + 1; k++) {
                    for (int l = j - 1; l <= j + 1; l++) {
                        Cave_t *c_ptr = &cave[k][l];
                        c_ptr->pl = flag;

                        if (!flag) {
                            c_ptr->fm = false;
                        }
                    }
                }
            }
        }
    }
    prt_map();
}

// Wizard routine for gaining on stats -RAK-
void change_character() {
    int tmp_val;
    vtype_t tmp_str;

    uint8_t *a_ptr = py.stats.max_stat;

    putStringClearToEOL("(3 - 118) Strength     = ", 0, 0);
    if (get_string(tmp_str, 0, 25, 3)) {
        tmp_val = atoi(tmp_str);
        if (tmp_val > 2 && tmp_val < 119) {
            a_ptr[A_STR] = (uint8_t) tmp_val;
            (void) res_stat(A_STR);
        }
    } else {
        return;
    }

    putStringClearToEOL("(3 - 118) Intelligence = ", 0, 0);
    if (get_string(tmp_str, 0, 25, 3)) {
        tmp_val = atoi(tmp_str);
        if (tmp_val > 2 && tmp_val < 119) {
            a_ptr[A_INT] = (uint8_t) tmp_val;
            (void) res_stat(A_INT);
        }
    } else {
        return;
    }

    putStringClearToEOL("(3 - 118) Wisdom       = ", 0, 0);
    if (get_string(tmp_str, 0, 25, 3)) {
        tmp_val = atoi(tmp_str);
        if (tmp_val > 2 && tmp_val < 119) {
            a_ptr[A_WIS] = (uint8_t) tmp_val;
            (void) res_stat(A_WIS);
        }
    } else {
        return;
    }

    putStringClearToEOL("(3 - 118) Dexterity    = ", 0, 0);
    if (get_string(tmp_str, 0, 25, 3)) {
        tmp_val = atoi(tmp_str);
        if (tmp_val > 2 && tmp_val < 119) {
            a_ptr[A_DEX] = (uint8_t) tmp_val;
            (void) res_stat(A_DEX);
        }
    } else {
        return;
    }

    putStringClearToEOL("(3 - 118) Constitution = ", 0, 0);
    if (get_string(tmp_str, 0, 25, 3)) {
        tmp_val = atoi(tmp_str);
        if (tmp_val > 2 && tmp_val < 119) {
            a_ptr[A_CON] = (uint8_t) tmp_val;
            (void) res_stat(A_CON);
        }
    } else {
        return;
    }

    putStringClearToEOL("(3 - 118) Charisma     = ", 0, 0);
    if (get_string(tmp_str, 0, 25, 3)) {
        tmp_val = atoi(tmp_str);
        if (tmp_val > 2 && tmp_val < 119) {
            a_ptr[A_CHR] = (uint8_t) tmp_val;
            (void) res_stat(A_CHR);
        }
    } else {
        return;
    }

    putStringClearToEOL("(1 - 32767) Hit points = ", 0, 0);
    if (get_string(tmp_str, 0, 25, 5)) {
        tmp_val = atoi(tmp_str);
        if (tmp_val > 0 && tmp_val <= MAX_SHORT) {
            py.misc.mhp = (int16_t) tmp_val;
            py.misc.chp = (int16_t) tmp_val;
            py.misc.chp_frac = 0;
            prt_mhp();
            prt_chp();
        }
    } else {
        return;
    }

    putStringClearToEOL("(0 - 32767) Mana       = ", 0, 0);
    if (get_string(tmp_str, 0, 25, 5)) {
        tmp_val = atoi(tmp_str);
        if (tmp_val > -1 && tmp_val <= MAX_SHORT && (*tmp_str != '\0')) {
            py.misc.mana = (int16_t) tmp_val;
            py.misc.cmana = (int16_t) tmp_val;
            py.misc.cmana_frac = 0;
            prt_cmana();
        }
    } else {
        return;
    }

    (void) sprintf(tmp_str, "Current=%d  Gold = ", py.misc.au);
    tmp_val = (int) strlen(tmp_str);
    putStringClearToEOL(tmp_str, 0, 0);
    if (get_string(tmp_str, 0, tmp_val, 7)) {
        int32_t tmp_lval = (int32_t) atol(tmp_str);
        if (tmp_lval > -1 && (*tmp_str != '\0')) {
            py.misc.au = tmp_lval;
            prt_gold();
        }
    } else {
        return;
    }

    (void) sprintf(tmp_str, "Current=%d  (0-200) Searching = ", py.misc.srh);
    tmp_val = (int) strlen(tmp_str);
    putStringClearToEOL(tmp_str, 0, 0);
    if (get_string(tmp_str, 0, tmp_val, 3)) {
        tmp_val = atoi(tmp_str);
        if (tmp_val > -1 && tmp_val < 201 && (*tmp_str != '\0')) {
            py.misc.srh = (int16_t) tmp_val;
        }
    } else {
        return;
    }

    (void) sprintf(tmp_str, "Current=%d  (-1-18) Stealth = ", py.misc.stl);
    tmp_val = (int) strlen(tmp_str);
    putStringClearToEOL(tmp_str, 0, 0);
    if (get_string(tmp_str, 0, tmp_val, 3)) {
        tmp_val = atoi(tmp_str);
        if (tmp_val > -2 && tmp_val < 19 && (*tmp_str != '\0')) {
            py.misc.stl = (int16_t) tmp_val;
        }
    } else {
        return;
    }

    (void) sprintf(tmp_str, "Current=%d  (0-200) Disarming = ", py.misc.disarm);
    tmp_val = (int) strlen(tmp_str);
    putStringClearToEOL(tmp_str, 0, 0);
    if (get_string(tmp_str, 0, tmp_val, 3)) {
        tmp_val = atoi(tmp_str);
        if (tmp_val > -1 && tmp_val < 201 && (*tmp_str != '\0')) {
            py.misc.disarm = (int16_t) tmp_val;
        }
    } else {
        return;
    }

    (void) sprintf(tmp_str, "Current=%d  (0-100) Save = ", py.misc.save);
    tmp_val = (int) strlen(tmp_str);
    putStringClearToEOL(tmp_str, 0, 0);
    if (get_string(tmp_str, 0, tmp_val, 3)) {
        tmp_val = atoi(tmp_str);
        if (tmp_val > -1 && tmp_val < 201 && (*tmp_str != '\0')) {
            py.misc.save = (int16_t) tmp_val;
        }
    } else {
        return;
    }

    (void) sprintf(tmp_str, "Current=%d  (0-200) Base to hit = ", py.misc.bth);
    tmp_val = (int) strlen(tmp_str);
    putStringClearToEOL(tmp_str, 0, 0);
    if (get_string(tmp_str, 0, tmp_val, 3)) {
        tmp_val = atoi(tmp_str);
        if (tmp_val > -1 && tmp_val < 201 && (*tmp_str != '\0')) {
            py.misc.bth = (int16_t) tmp_val;
        }
    } else {
        return;
    }

    (void) sprintf(tmp_str, "Current=%d  (0-200) Bows/Throwing = ", py.misc.bthb);
    tmp_val = (int) strlen(tmp_str);
    putStringClearToEOL(tmp_str, 0, 0);
    if (get_string(tmp_str, 0, tmp_val, 3)) {
        tmp_val = atoi(tmp_str);
        if (tmp_val > -1 && tmp_val < 201 && (*tmp_str != '\0')) {
            py.misc.bthb = (int16_t) tmp_val;
        }
    } else {
        return;
    }

    (void) sprintf(tmp_str, "Current=%d  Weight = ", py.misc.wt);
    tmp_val = (int) strlen(tmp_str);
    putStringClearToEOL(tmp_str, 0, 0);
    if (get_string(tmp_str, 0, tmp_val, 3)) {
        tmp_val = atoi(tmp_str);
        if (tmp_val > -1 && (*tmp_str != '\0')) {
            py.misc.wt = (uint16_t) tmp_val;
        }
    } else {
        return;
    }

    while (get_com("Alter speed? (+/-)", tmp_str)) {
        if (*tmp_str == '+') {
            change_speed(-1);
        } else if (*tmp_str == '-') {
            change_speed(1);
        } else {
            break;
        }
        prt_speed();
    }
}

// Wizard routine for creating objects -RAK-
void wizard_create() {
    int tmp_val;
    vtype_t tmp_str;

    printMessage("Warning: This routine can cause a fatal error.");

    Inventory_t forge;
    Inventory_t *i_ptr = &forge;

    i_ptr->index = OBJ_WIZARD;
    i_ptr->name2 = 0;
    inscribe(i_ptr, "wizard item");
    i_ptr->ident = ID_KNOWN2 | ID_STOREBOUGHT;

    putStringClearToEOL("Tval   : ", 0, 0);
    if (!get_string(tmp_str, 0, 9, 3)) {
        return;
    }
    tmp_val = atoi(tmp_str);
    i_ptr->tval = (uint8_t) tmp_val;

    putStringClearToEOL("Tchar  : ", 0, 0);
    if (!get_string(tmp_str, 0, 9, 1)) {
        return;
    }
    i_ptr->tchar = (uint8_t) tmp_str[0];

    putStringClearToEOL("Subval : ", 0, 0);
    if (!get_string(tmp_str, 0, 9, 5)) {
        return;
    }
    tmp_val = atoi(tmp_str);
    i_ptr->subval = (uint8_t) tmp_val;

    putStringClearToEOL("Weight : ", 0, 0);
    if (!get_string(tmp_str, 0, 9, 5)) {
        return;
    }
    tmp_val = atoi(tmp_str);
    i_ptr->weight = (uint16_t) tmp_val;

    putStringClearToEOL("Number : ", 0, 0);
    if (!get_string(tmp_str, 0, 9, 5)) {
        return;
    }
    tmp_val = atoi(tmp_str);
    i_ptr->number = (uint8_t) tmp_val;

    putStringClearToEOL("Damage (dice): ", 0, 0);
    if (!get_string(tmp_str, 0, 15, 3)) {
        return;
    }
    tmp_val = atoi(tmp_str);
    i_ptr->damage[0] = (uint8_t) tmp_val;

    putStringClearToEOL("Damage (sides): ", 0, 0);
    if (!get_string(tmp_str, 0, 16, 3)) {
        return;
    }
    tmp_val = atoi(tmp_str);
    i_ptr->damage[1] = (uint8_t) tmp_val;

    putStringClearToEOL("+To hit: ", 0, 0);
    if (!get_string(tmp_str, 0, 9, 3)) {
        return;
    }
    tmp_val = atoi(tmp_str);
    i_ptr->tohit = (int16_t) tmp_val;

    putStringClearToEOL("+To dam: ", 0, 0);
    if (!get_string(tmp_str, 0, 9, 3)) {
        return;
    }
    tmp_val = atoi(tmp_str);
    i_ptr->todam = (int16_t) tmp_val;

    putStringClearToEOL("AC     : ", 0, 0);
    if (!get_string(tmp_str, 0, 9, 3)) {
        return;
    }
    tmp_val = atoi(tmp_str);
    i_ptr->ac = (int16_t) tmp_val;

    putStringClearToEOL("+To AC : ", 0, 0);
    if (!get_string(tmp_str, 0, 9, 3)) {
        return;
    }
    tmp_val = atoi(tmp_str);
    i_ptr->toac = (int16_t) tmp_val;

    putStringClearToEOL("P1     : ", 0, 0);
    if (!get_string(tmp_str, 0, 9, 5)) {
        return;
    }
    tmp_val = atoi(tmp_str);
    i_ptr->p1 = (int16_t) tmp_val;

    putStringClearToEOL("Flags (In HEX): ", 0, 0);
    if (!get_string(tmp_str, 0, 16, 8)) {
        return;
    }

    // can't be constant string, this causes problems with
    // the GCC compiler and some scanf routines.
    char pattern[4];

    (void) strcpy(pattern, "%lx");

    int32_t tmp_lval;
    (void) sscanf(tmp_str, pattern, &tmp_lval);
    i_ptr->flags = (uint32_t) tmp_lval;

    putStringClearToEOL("Cost : ", 0, 0);
    if (!get_string(tmp_str, 0, 9, 8)) {
        return;
    }
    tmp_lval = (int) atol(tmp_str);
    i_ptr->cost = tmp_lval;

    putStringClearToEOL("Level : ", 0, 0);
    if (!get_string(tmp_str, 0, 10, 3)) {
        return;
    }
    tmp_val = atoi(tmp_str);
    i_ptr->level = (uint8_t) tmp_val;

    if (get_check("Allocate?")) {
        // delete object first if any, before call popt
        Cave_t *c_ptr = &cave[char_row][char_col];
        if (c_ptr->tptr != 0) {
            (void) delete_object(char_row, char_col);
        }

        tmp_val = popt();
        treasure_list[tmp_val] = forge;
        c_ptr->tptr = (uint8_t) tmp_val;
        printMessage("Allocated.");
    } else {
        printMessage("Aborted.");
    }
}
