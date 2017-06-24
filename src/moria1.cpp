// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Misc code, mainly handles player movement, inventory, etc

#include "headers.h"
#include "externs.h"

// Changes speed of monsters relative to player -RAK-
// Note: When the player is sped up or slowed down, I simply change
// the speed of all the monsters. This greatly simplified the logic.
void change_speed(int num) {
    py.flags.speed += num;
    py.flags.status |= PY_SPEED;

    for (int i = mfptr - 1; i >= MIN_MONIX; i--) {
        m_list[i].cspeed += num;
    }
}

// Player bonuses -RAK-
//
// When an item is worn or taken off, this re-adjusts the player bonuses.
//     Factor =  1 : wear
//     Factor = -1 : removed
//
// Only calculates properties with cumulative effect.  Properties that
// depend on everything being worn are recalculated by calc_bonuses() -CJS-
void py_bonuses(Inventory_t *t_ptr, int factor) {
    int amount = t_ptr->p1 * factor;

    if (t_ptr->flags & TR_STATS) {
        for (int i = 0; i < 6; i++) {
            if ((1 << i) & t_ptr->flags) {
                bst_stat(i, amount);
            }
        }
    }

    if (TR_SEARCH & t_ptr->flags) {
        py.misc.srh += amount;
        py.misc.fos -= amount;
    }

    if (TR_STEALTH & t_ptr->flags) {
        py.misc.stl += amount;
    }

    if (TR_SPEED & t_ptr->flags) {
        change_speed(-amount);
    }

    if ((TR_BLIND & t_ptr->flags) && factor > 0) {
        py.flags.blind += 1000;
    }

    if ((TR_TIMID & t_ptr->flags) && factor > 0) {
        py.flags.afraid += 50;
    }

    if (TR_INFRA & t_ptr->flags) {
        py.flags.see_infra += amount;
    }
}

static void playerResetFlags() {
    py.flags.see_inv = false;
    py.flags.teleport = false;
    py.flags.free_act = false;
    py.flags.slow_digest = false;
    py.flags.aggravate = false;
    py.flags.sustain_str = false;
    py.flags.sustain_int = false;
    py.flags.sustain_wis = false;
    py.flags.sustain_con = false;
    py.flags.sustain_dex = false;
    py.flags.sustain_chr = false;
    py.flags.fire_resist = false;
    py.flags.acid_resist = false;
    py.flags.cold_resist = false;
    py.flags.regenerate = false;
    py.flags.lght_resist = false;
    py.flags.ffall = false;
}

static void playerRecalculateBonusesFromInventory() {
    Inventory_t *item;

    for (int i = INVEN_WIELD; i < INVEN_LIGHT; i++) {
        item = &inventory[i];

        if (item->tval != TV_NOTHING) {
            py.misc.ptohit += item->tohit;

            // Bows can't damage. -CJS-
            if (item->tval != TV_BOW) {
                py.misc.ptodam += item->todam;
            }

            py.misc.ptoac += item->toac;
            py.misc.pac += item->ac;

            if (known2_p(item)) {
                py.misc.dis_th += item->tohit;

                // Bows can't damage. -CJS-
                if (item->tval != TV_BOW) {
                    py.misc.dis_td += item->todam;
                }

                py.misc.dis_tac += item->toac;
                py.misc.dis_ac += item->ac;
            } else if (!(TR_CURSED & item->flags)) {
                // Base AC values should always be visible,
                // as long as the item is not cursed.
                py.misc.dis_ac += item->ac;
            }
        }
    }
}

static uint32_t inventoryCollectAllItemFlags() {
    uint32_t flags = 0;

    for (int i = INVEN_WIELD; i < INVEN_LIGHT; i++) {
        flags |= inventory[i].flags;
    }

    return flags;
}

static void playerRecalculateSustainStatsFromInventory() {
    for (int i = INVEN_WIELD; i < INVEN_LIGHT; i++) {
        if (!(TR_SUST_STAT & inventory[i].flags)) {
            continue;
        }

        switch (inventory[i].p1) {
            case 1:
                py.flags.sustain_str = true;
                break;
            case 2:
                py.flags.sustain_int = true;
                break;
            case 3:
                py.flags.sustain_wis = true;
                break;
            case 4:
                py.flags.sustain_con = true;
                break;
            case 5:
                py.flags.sustain_dex = true;
                break;
            case 6:
                py.flags.sustain_chr = true;
                break;
            default:
                break;
        }
    }
}

// Recalculate the effect of all the stuff we use. -CJS-
void calc_bonuses() {
    // Temporarily adjust food_digested
    if (py.flags.slow_digest) {
        py.flags.food_digested++;
    }
    if (py.flags.regenerate) {
        py.flags.food_digested -= 3;
    }

    int savedDisplayAC = py.misc.dis_ac;

    playerResetFlags();

    py.misc.ptohit = (int16_t) tohit_adj();  // Real To Hit
    py.misc.ptodam = (int16_t) todam_adj();  // Real To Dam
    py.misc.ptoac = (int16_t) toac_adj();    // Real To AC
    py.misc.pac = 0;                         // Real AC
    py.misc.dis_th = py.misc.ptohit;         // Display To Hit
    py.misc.dis_td = py.misc.ptodam;         // Display To Dam
    py.misc.dis_ac = 0;                      // Display AC
    py.misc.dis_tac = py.misc.ptoac;         // Display To AC

    playerRecalculateBonusesFromInventory();

    py.misc.dis_ac += py.misc.dis_tac;

    if (weapon_heavy) {
        py.misc.dis_th += (py.stats.use_stat[A_STR] * 15 - inventory[INVEN_WIELD].weight);
    }

    // Add in temporary spell increases
    if (py.flags.invuln > 0) {
        py.misc.pac += 100;
        py.misc.dis_ac += 100;
    }

    if (py.flags.blessed > 0) {
        py.misc.pac += 2;
        py.misc.dis_ac += 2;
    }

    if (py.flags.detect_inv > 0) {
        py.flags.see_inv = true;
    }

    // can't print AC here because might be in a store
    if (savedDisplayAC != py.misc.dis_ac) {
        py.flags.status |= PY_ARMOR;
    }

    uint32_t item_flags = inventoryCollectAllItemFlags();

    if (TR_SLOW_DIGEST & item_flags) {
        py.flags.slow_digest = true;
    }
    if (TR_AGGRAVATE & item_flags) {
        py.flags.aggravate = true;
    }
    if (TR_TELEPORT & item_flags) {
        py.flags.teleport = true;
    }
    if (TR_REGEN & item_flags) {
        py.flags.regenerate = true;
    }
    if (TR_RES_FIRE & item_flags) {
        py.flags.fire_resist = true;
    }
    if (TR_RES_ACID & item_flags) {
        py.flags.acid_resist = true;
    }
    if (TR_RES_COLD & item_flags) {
        py.flags.cold_resist = true;
    }
    if (TR_FREE_ACT & item_flags) {
        py.flags.free_act = true;
    }
    if (TR_SEE_INVIS & item_flags) {
        py.flags.see_inv = true;
    }
    if (TR_RES_LIGHT & item_flags) {
        py.flags.lght_resist = true;
    }
    if (TR_FFALL & item_flags) {
        py.flags.ffall = true;
    }

    playerRecalculateSustainStatsFromInventory();

    // Reset food_digested values
    if (py.flags.slow_digest) {
        py.flags.food_digested--;
    }
    if (py.flags.regenerate) {
        py.flags.food_digested += 3;
    }
}

static void inventoryItemWeightText(char *text, int itemID) {
    int totalWeight = inventory[itemID].weight * inventory[itemID].number;
    int quotient = totalWeight / 10;
    int remainder = totalWeight % 10;

    (void) sprintf(text, "%3d.%d lb", quotient, remainder);
}

// Displays inventory items from r1 to r2 -RAK-
// Designed to keep the display as far to the right as possible. -CJS-
// The parameter col gives a column at which to start, but if the display
// does not fit, it may be moved left.  The return value is the left edge
// used. If mask is non-zero, then only display those items which have a
// non-zero entry in the mask array.
int show_inven(int r1, int r2, bool weight, int col, char *mask) {
    vtype_t descriptions[23];

    int len = 79 - col;

    int lim;
    if (weight) {
        lim = 68;
    } else {
        lim = 76;
    }

    // Generate the descriptions text
    for (int i = r1; i <= r2; i++) {
        if (mask != CNIL && !mask[i]) {
            continue;
        }

        obj_desc_t description;
        objdes(description, &inventory[i], true);

        // Truncate if too long.
        description[lim] = 0;

        (void) sprintf(descriptions[i], "%c) %s", 'a' + i, description);

        int l = (int) strlen(descriptions[i]) + 2;

        if (weight) {
            l += 9;
        }

        if (l > len) {
            len = l;
        }
    }

    col = 79 - len;
    if (col < 0) {
        col = 0;
    }

    int current_line = 1;

    // Print the descriptions
    for (int i = r1; i <= r2; i++) {
        if (mask != CNIL && !mask[i]) {
            continue;
        }

        // don't need first two spaces if in first column
        if (col == 0) {
            prt(descriptions[i], current_line, col);
        } else {
            put_buffer("  ", current_line, col);
            prt(descriptions[i], current_line, col + 2);
        }

        if (weight) {
            obj_desc_t text;
            inventoryItemWeightText(text, i);
            prt(text, current_line, 71);
        }

        current_line++;
    }

    return col;
}

// Return a string describing how a given equipment item is carried. -CJS-
const char *describe_use(int positionID) {
    switch (positionID) {
        case INVEN_WIELD:
            return "wielding";
        case INVEN_HEAD:
            return "wearing on your head";
        case INVEN_NECK:
            return "wearing around your neck";
        case INVEN_BODY:
            return "wearing on your body";
        case INVEN_ARM:
            return "wearing on your arm";
        case INVEN_HANDS:
            return "wearing on your hands";
        case INVEN_RIGHT:
            return "wearing on your right hand";
        case INVEN_LEFT:
            return "wearing on your left hand";
        case INVEN_FEET:
            return "wearing on your feet";
        case INVEN_OUTER:
            return "wearing about your body";
        case INVEN_LIGHT:
            return "using to light the way";
        case INVEN_AUX:
            return "holding ready by your side";
        default:
            return "carrying in your pack";
    }
}

static const char *itemPostitionDescription(int positionID, uint16_t weight) {
    switch (positionID) {
        case INVEN_WIELD:
            if (py.stats.use_stat[A_STR] * 15 < weight) {
                return "Just lifting";
            }

            return "Wielding";
        case INVEN_HEAD:
            return "On head";
        case INVEN_NECK:
            return "Around neck";
        case INVEN_BODY:
            return "On body";
        case INVEN_ARM:
            return "On arm";
        case INVEN_HANDS:
            return "On hands";
        case INVEN_RIGHT:
            return "On right hand";
        case INVEN_LEFT:
            return "On left hand";
        case INVEN_FEET:
            return "On feet";
        case INVEN_OUTER:
            return "About body";
        case INVEN_LIGHT:
            return "Light source";
        case INVEN_AUX:
            return "Spare weapon";
        default:
            return "Unknown value";
    }
}

// Displays equipment items from r1 to end -RAK-
// Keep display as far right as possible. -CJS-
int show_equip(bool weight, int col) {
    vtype_t descriptions[INVEN_ARRAY_SIZE - INVEN_WIELD];

    int len = 79 - col;

    int lim;
    if (weight) {
        lim = 52;
    } else {
        lim = 60;
    }

    // Range of equipment
    int line = 0;
    for (int i = INVEN_WIELD; i < INVEN_ARRAY_SIZE; i++) {
        if (inventory[i].tval == TV_NOTHING) {
            continue;
        }

        // Get position
        const char *positionDescription = itemPostitionDescription(i, inventory[i].weight);

        obj_desc_t description;
        objdes(description, &inventory[i], true);

        // Truncate if necessary
        description[lim] = 0;

        (void) sprintf(descriptions[line], "%c) %-14s: %s", line + 'a', positionDescription, description);

        int l = (int) strlen(descriptions[line]) + 2;

        if (weight) {
            l += 9;
        }

        if (l > len) {
            len = l;
        }

        line++;
    }

    col = 79 - len;
    if (col < 0) {
        col = 0;
    }

    // Range of equipment
    line = 0;
    for (int i = INVEN_WIELD; i < INVEN_ARRAY_SIZE; i++) {
        if (inventory[i].tval == TV_NOTHING) {
            continue;
        }

        // don't need first two spaces when using whole screen
        if (col == 0) {
            prt(descriptions[line], line + 1, col);
        } else {
            put_buffer("  ", line + 1, col);
            prt(descriptions[line], line + 1, col + 2);
        }

        if (weight) {
            obj_desc_t text;
            inventoryItemWeightText(text, i);
            prt(text, line + 1, 71);
        }

        line++;
    }
    erase_line(line + 1, col);

    return col;
}

// Remove item from equipment list -RAK-
void takeoff(int item_val, int posn) {
    py.flags.status |= PY_STR_WGT;

    Inventory_t *t_ptr = &inventory[item_val];

    inven_weight -= t_ptr->weight * t_ptr->number;
    equip_ctr--;

    const char *p;
    if (item_val == INVEN_WIELD || item_val == INVEN_AUX) {
        p = "Was wielding ";
    } else if (item_val == INVEN_LIGHT) {
        p = "Light source was ";
    } else {
        p = "Was wearing ";
    }

    obj_desc_t description;
    objdes(description, t_ptr, true);

    obj_desc_t msg;
    if (posn >= 0) {
        (void) sprintf(msg, "%s%s (%c)", p, description, 'a' + posn);
    } else {
        (void) sprintf(msg, "%s%s", p, description);
    }
    msg_print(msg);

    // For secondary weapon
    if (item_val != INVEN_AUX) {
        py_bonuses(t_ptr, -1);
    }

    invcopy(t_ptr, OBJ_NOTHING);
}

// Used to verify if this really is the item we wish to -CJS-
// wear or read.
int verify(const char *prompt, int item) {
    obj_desc_t description;
    objdes(description, &inventory[item], true);

    // change the period to a question mark
    description[strlen(description) - 1] = '?';

    obj_desc_t msg;
    (void) sprintf(msg, "%s %s", prompt, description);

    return get_check(msg);
}

// All inventory commands (wear, exchange, take off, drop, inventory and
// equipment) are handled in an alternative command input mode, which accepts
// any of the inventory commands.
//
// It is intended that this function be called several times in succession,
// as some commands take up a turn, and the rest of moria must proceed in the
// interim. A global variable is provided, doing_inven, which is normally
// zero; however if on return from inven_command it is expected that
// inven_command should be called *again*, (being still in inventory command
// input mode), then doing_inven is set to the inventory command character
// which should be used in the next call to inven_command.
//
// On return, the screen is restored, but not flushed. Provided no flush of
// the screen takes place before the next call to inven_command, the inventory
// command screen is silently redisplayed, and no actual output takes place at
// all. If the screen is flushed before a subsequent call, then the player is
// prompted to see if we should continue. This allows the player to see any
// changes that take place on the screen during inventory command input.
//
// The global variable, screen_change, is cleared by inven_command, and set
// when the screen is flushed. This is the means by which inven_command tell
// if the screen has been flushed.
//
// The display of inventory items is kept to the right of the screen to
// minimize the work done to restore the screen afterwards. -CJS-

// Inventory command screen states.
#define BLANK_SCR 0
#define EQUIP_SCR 1
#define INVEN_SCR 2
#define WEAR_SCR 3
#define HELP_SCR 4
#define WRONG_SCR 5

// Keep track of the state of the inventory screen.
static int scr_state, scr_left, scr_base;
static int wear_low, wear_high;

// Draw the inventory screen.
static void inven_screen(int new_scr) {
    if (new_scr == scr_state) {
        return;
    } else {
        scr_state = new_scr;
    }

    int line = 0;

    switch (new_scr) {
        case BLANK_SCR:
            line = 0;
            break;
        case HELP_SCR:
            if (scr_left > 52) {
                scr_left = 52;
            }

            prt("  ESC: exit", 1, scr_left);
            prt("  w  : wear or wield object", 2, scr_left);
            prt("  t  : take off item", 3, scr_left);
            prt("  d  : drop object", 4, scr_left);
            prt("  x  : exchange weapons", 5, scr_left);
            prt("  i  : inventory of pack", 6, scr_left);
            prt("  e  : list used equipment", 7, scr_left);

            line = 7;
            break;
        case INVEN_SCR:
            scr_left = show_inven(0, inven_ctr - 1, show_weight_flag, scr_left, CNIL);
            line = inven_ctr;
            break;
        case WEAR_SCR:
            scr_left = show_inven(wear_low, wear_high, show_weight_flag, scr_left, CNIL);
            line = wear_high - wear_low + 1;
            break;
        case EQUIP_SCR:
            scr_left = show_equip(show_weight_flag, scr_left);
            line = equip_ctr;
            break;
    }

    if (line >= scr_base) {
        scr_base = line + 1;
        erase_line(scr_base, scr_left);
        return;
    }

    while (++line <= scr_base) {
        erase_line(line, scr_left);
    }
}

static void setInventoryCommandScreenState(char command) {
    // Take up where we left off after a previous inventory command. -CJS-
    if (doing_inven) {
        // If the screen has been flushed, we need to redraw. If the command
        // is a simple ' ' to recover the screen, just quit. Otherwise, check
        // and see what the user wants.
        if (screen_change) {
            if (command == ' ' || !get_check("Continuing with inventory command?")) {
                doing_inven = 0;
                return;
            }
            scr_left = 50;
            scr_base = 0;
        }

        int savedState = scr_state;
        scr_state = WRONG_SCR;
        inven_screen(savedState);

        return;
    }

    scr_left = 50;
    scr_base = 0;

    // this forces exit of inven_command() if selecting is not set true
    scr_state = BLANK_SCR;
}

static void displayInventory() {
    if (inven_ctr == 0) {
        msg_print("You are not carrying anything.");
    } else {
        inven_screen(INVEN_SCR);
    }
}

static void displayEquipment() {
    if (equip_ctr == 0) {
        msg_print("You are not using any equipment.");
    } else {
        inven_screen(EQUIP_SCR);
    }
}

static bool inventoryTakeOffItem(bool selecting) {
    if (equip_ctr == 0) {
        msg_print("You are not using any equipment.");
        // don't print message restarting inven command after taking off something, it is confusing
        return selecting;
    }

    if (inven_ctr >= INVEN_WIELD && !doing_inven) {
        msg_print("You will have to drop something first.");
        return selecting;
    }

    if (scr_state != BLANK_SCR) {
        inven_screen(EQUIP_SCR);
    }

    return true;
}

static bool inventoryDropItem(char *command, bool selecting) {
    if (inven_ctr == 0 && equip_ctr == 0) {
        msg_print("But you're not carrying anything.");
        return selecting;
    }

    if (cave[char_row][char_col].tptr != 0) {
        msg_print("There's no room to drop anything here.");
        return selecting;
    }

    if ((scr_state == EQUIP_SCR && equip_ctr > 0) || inven_ctr == 0) {
        if (scr_state != BLANK_SCR) {
            inven_screen(EQUIP_SCR);
        }
        *command = 'r'; // Remove - or take off and drop.
    } else if (scr_state != BLANK_SCR) {
        inven_screen(INVEN_SCR);
    }

    return true;
}

static bool inventoryWearWieldItem(bool selecting) {
    // Note: simple loop to get wear_low value
    for (wear_low = 0; wear_low < inven_ctr && inventory[wear_low].tval > TV_MAX_WEAR; wear_low++);

    // Note: simple loop to get wear_high value
    for (wear_high = wear_low; wear_high < inven_ctr && inventory[wear_high].tval >= TV_MIN_WEAR; wear_high++);

    wear_high--;

    if (wear_low > wear_high) {
        msg_print("You have nothing to wear or wield.");
        return selecting;
    }

    if (scr_state != BLANK_SCR && scr_state != INVEN_SCR) {
        inven_screen(WEAR_SCR);
    }

    return true;
}

static void inventoryUnwieldItem() {
    if (inventory[INVEN_WIELD].tval == TV_NOTHING && inventory[INVEN_AUX].tval == TV_NOTHING) {
        msg_print("But you are wielding no weapons.");
        return;
    }

    if (TR_CURSED & inventory[INVEN_WIELD].flags) {
        obj_desc_t description;
        objdes(description, &inventory[INVEN_WIELD], false);

        obj_desc_t msg;
        (void) sprintf(msg, "The %s you are wielding appears to be cursed.", description);

        msg_print(msg);

        return;
    }

    free_turn_flag = false;

    Inventory_t savedItem = inventory[INVEN_AUX];
    inventory[INVEN_AUX] = inventory[INVEN_WIELD];
    inventory[INVEN_WIELD] = savedItem;

    if (scr_state == EQUIP_SCR) {
        scr_left = show_equip(show_weight_flag, scr_left);
    }

    py_bonuses(&inventory[INVEN_AUX], -1);  // Subtract bonuses
    py_bonuses(&inventory[INVEN_WIELD], 1); // Add bonuses

    if (inventory[INVEN_WIELD].tval != TV_NOTHING) {
        obj_desc_t msgLabel;
        (void) strcpy(msgLabel, "Primary weapon   : ");

        obj_desc_t description;
        objdes(description, &inventory[INVEN_WIELD], true);

        msg_print(strcat(msgLabel, description));
    } else {
        msg_print("No primary weapon.");
    }

    // this is a new weapon, so clear the heavy flag
    weapon_heavy = false;
    check_strength();
}

// look for item whose inscription matches "which"
static int inventoryGetItemMatchingInscription(char which, char command, int from, int to) {
    int item;

    if (which >= '0' && which <= '9' && command != 'r' && command != 't') {
        int m;

        // Note: simple loop to get id
        for (m = from; m <= to && ((inventory[m].inscrip[0] != which) || (inventory[m].inscrip[1] != '\0')); m++);

        if (m <= to) {
            item = m;
        } else {
            item = -1;
        }
    } else if (which >= 'A' && which <= 'Z') {
        item = which - 'A';
    } else {
        item = which - 'a';
    }

    return item;
}

static void buildCommandHeading(char *prt1, int from, int to, const char *swap, char command, const char *prompt) {
    from = from + 'a';
    to = to + 'a';

    const char *listItems = "";
    if (scr_state == BLANK_SCR) {
        listItems = ", * to list";
    }

    const char *digits = "";
    if (command == 'w' || command == 'd') {
        digits = ", 0-9";
    }

    (void) sprintf(prt1, "(%c-%c%s%s%s, space to break, ESC to exit) %s which one?", from, to, listItems, swap, digits, prompt);
}

static void drawInventoryScreenForCommand(char command) {
    if (command == 't' || command == 'r') {
        inven_screen(EQUIP_SCR);
    } else if (command == 'w' && scr_state != INVEN_SCR) {
        inven_screen(WEAR_SCR);
    } else {
        inven_screen(INVEN_SCR);
    }
}

static void swapInventoryScreenForDrop() {
    if (scr_state == EQUIP_SCR) {
        inven_screen(INVEN_SCR);
    } else if (scr_state == INVEN_SCR) {
        inven_screen(EQUIP_SCR);
    }
}

static int inventoryGetSlotToWearEquipment(int item) {
    int slot;

    // Slot for equipment
    switch (inventory[item].tval) {
        case TV_SLING_AMMO:
        case TV_BOLT:
        case TV_ARROW:
        case TV_BOW:
        case TV_HAFTED:
        case TV_POLEARM:
        case TV_SWORD:
        case TV_DIGGING:
        case TV_SPIKE:
            slot = INVEN_WIELD;
            break;
        case TV_LIGHT:
            slot = INVEN_LIGHT;
            break;
        case TV_BOOTS:
            slot = INVEN_FEET;
            break;
        case TV_GLOVES:
            slot = INVEN_HANDS;
            break;
        case TV_CLOAK:
            slot = INVEN_OUTER;
            break;
        case TV_HELM:
            slot = INVEN_HEAD;
            break;
        case TV_SHIELD:
            slot = INVEN_ARM;
            break;
        case TV_HARD_ARMOR:
        case TV_SOFT_ARMOR:
            slot = INVEN_BODY;
            break;
        case TV_AMULET:
            slot = INVEN_NECK;
            break;
        case TV_RING:
            if (inventory[INVEN_RIGHT].tval == TV_NOTHING) {
                slot = INVEN_RIGHT;
            } else if (inventory[INVEN_LEFT].tval == TV_NOTHING) {
                slot = INVEN_LEFT;
            } else {
                slot = 0;

                // Rings. Give choice over where they go.
                do {
                    char query;
                    if (!get_com("Put ring on which hand (l/r/L/R)?", &query)) {
                        slot = -1;
                    } else if (query == 'l') {
                        slot = INVEN_LEFT;
                    } else if (query == 'r') {
                        slot = INVEN_RIGHT;
                    } else {
                        if (query == 'L') {
                            slot = INVEN_LEFT;
                        } else if (query == 'R') {
                            slot = INVEN_RIGHT;
                        } else {
                            bell();
                        }
                        if (slot && !verify("Replace", slot)) {
                            slot = 0;
                        }
                    }
                } while (slot == 0);
            }
            break;
        default:
            slot = -1;
            msg_print("IMPOSSIBLE: I don't see how you can use that.");
            break;
    }

    return slot;
}

static void inventoryItemIsCursedMessage(int itemID) {
    obj_desc_t description;
    objdes(description, &inventory[itemID], false);

    obj_desc_t itemText;
    (void) sprintf(itemText, "The %s you are ", description);

    if (itemID == INVEN_HEAD) {
        (void) strcat(itemText, "wielding ");
    } else {
        (void) strcat(itemText, "wearing ");
    }

    msg_print(strcat(itemText, "appears to be cursed."));
}

static bool selectItemCommands(char *command, char *which, bool selecting) {
    int itemToTakeOff;
    int slot = 0;

    int from, to;
    const char *prompt;
    const char *swap;

    while (selecting && free_turn_flag) {
        swap = "";

        if (*command == 'w') {
            from = wear_low;
            to = wear_high;
            prompt = "Wear/Wield";
        } else {
            from = 0;
            if (*command == 'd') {
                to = inven_ctr - 1;
                prompt = "Drop";

                if (equip_ctr > 0) {
                    swap = ", / for Equip";
                }
            } else {
                to = equip_ctr - 1;

                if (*command == 't') {
                    prompt = "Take off";
                } else {
                    // command == 'r'

                    prompt = "Throw off";
                    if (inven_ctr > 0) {
                        swap = ", / for Inven";
                    }
                }
            }
        }

        if (from > to) {
            selecting = false;
            continue;
        }

        obj_desc_t headingText;
        buildCommandHeading(headingText, from, to, swap, *command, prompt);

        // Abort everything.
        if (!get_com(headingText, which)) {
            *which = ESCAPE;
            selecting = false;
            continue; // can we just return false from the function? -MRC-
        }

        // Draw the screen and maybe exit to main prompt.
        if (*which == ' ' || *which == '*') {
            drawInventoryScreenForCommand(*command);
            if (*which == ' ') {
                selecting = false;
            }
            continue;
        }

        // Swap screens (for drop)
        if (*which == '/' && swap[0]) {
            if (*command == 'd') {
                *command = 'r';
            } else {
                *command = 'd';
            }
            swapInventoryScreenForDrop();
            continue;
        }

        // look for item whose inscription matches "which"
        int item = inventoryGetItemMatchingInscription(*which, *command, from, to);

        if (item < from || item > to) {
            bell();
            continue;
        }

        // Found an item!

        if (*command == 'r' || *command == 't') {
            // Get its place in the equipment list.
            itemToTakeOff = item;
            item = 21;

            do {
                item++;
                if (inventory[item].tval != TV_NOTHING) {
                    itemToTakeOff--;
                }
            } while (itemToTakeOff >= 0);

            if (isupper((int) *which) && !verify((char *) prompt, item)) {
                item = -1;
            } else if (TR_CURSED & inventory[item].flags) {
                item = -1;
                msg_print("Hmmm, it seems to be cursed.");
            } else if (*command == 't' && !inven_check_num(&inventory[item])) {
                if (cave[char_row][char_col].tptr != 0) {
                    item = -1;
                    msg_print("You can't carry it.");
                } else if (get_check("You can't carry it.  Drop it?")) {
                    *command = 'r';
                } else {
                    item = -1;
                }
            }

            if (item >= 0) {
                if (*command == 'r') {
                    inven_drop(item, true);
                    // As a safety measure, set the player's inven
                    // weight to 0, when the last object is dropped.
                    if (inven_ctr == 0 && equip_ctr == 0) {
                        inven_weight = 0;
                    }
                } else {
                    slot = inven_carry(&inventory[item]);
                    takeoff(item, slot);
                }

                check_strength();

                free_turn_flag = false;

                if (*command == 'r') {
                    selecting = false;
                }
            }
        } else if (*command == 'w') {
            // Wearing. Go to a bit of trouble over replacing existing equipment.

            if (isupper((int) *which) && !verify((char *) prompt, item)) {
                item = -1;
            } else {
                slot = inventoryGetSlotToWearEquipment(item);
                if (slot == -1) {
                    item = -1;
                }
            }

            if (item >= 0 && inventory[slot].tval != TV_NOTHING) {
                if (TR_CURSED & inventory[slot].flags) {
                    inventoryItemIsCursedMessage(slot);
                    item = -1;
                } else if (inventory[item].subval == ITEM_GROUP_MIN && inventory[item].number > 1 && !inven_check_num(&inventory[slot])) {
                    // this can happen if try to wield a torch,
                    // and have more than one in inventory
                    msg_print("You will have to drop something first.");
                    item = -1;
                }
            }

            // OK. Wear it.
            if (item >= 0) {
                free_turn_flag = false;

                // first remove new item from inventory
                Inventory_t savedItem = inventory[item];
                Inventory_t *i_ptr = &savedItem;

                wear_high--;

                // Fix for torches
                if (i_ptr->number > 1 && i_ptr->subval <= ITEM_SINGLE_STACK_MAX) {
                    i_ptr->number = 1;
                    wear_high++;
                }

                inven_weight += i_ptr->weight * i_ptr->number;

                // Subtracts weight
                inven_destroy(item);

                // Second, add old item to inv and remove
                // from equipment list, if necessary.
                i_ptr = &inventory[slot];
                if (i_ptr->tval != TV_NOTHING) {
                    int savedCounter = inven_ctr;

                    itemToTakeOff = inven_carry(i_ptr);

                    // If item removed did not stack with anything
                    // in inventory, then increment wear_high.
                    if (inven_ctr != savedCounter) {
                        wear_high++;
                    }

                    takeoff(slot, itemToTakeOff);
                }

                // third, wear new item
                *i_ptr = savedItem;
                equip_ctr++;

                py_bonuses(i_ptr, 1);

                const char *text;
                if (slot == INVEN_WIELD) {
                    text = "You are wielding";
                } else if (slot == INVEN_LIGHT) {
                    text = "Your light source is";
                } else {
                    text = "You are wearing";
                }

                obj_desc_t description;
                objdes(description, i_ptr, true);

                // Get the right equipment letter.
                itemToTakeOff = INVEN_WIELD;
                item = 0;

                while (itemToTakeOff != slot) {
                    if (inventory[itemToTakeOff++].tval != TV_NOTHING) {
                        item++;
                    }
                }

                obj_desc_t msg;
                (void) sprintf(msg, "%s %s (%c)", text, description, 'a' + item);
                msg_print(msg);

                // this is a new weapon, so clear heavy flag
                if (slot == INVEN_WIELD) {
                    weapon_heavy = false;
                }
                check_strength();

                if (i_ptr->flags & TR_CURSED) {
                    msg_print("Oops! It feels deathly cold!");
                    add_inscribe(i_ptr, ID_DAMD);

                    // To force a cost of 0, even if unidentified.
                    i_ptr->cost = -1;
                }
            }
        } else {
            // command == 'd'

            // NOTE: initializing to `ESCAPE` as warnings were being given. -MRC-
            char query = ESCAPE;

            if (inventory[item].number > 1) {
                obj_desc_t description;
                objdes(description, &inventory[item], true);
                description[strlen(description) - 1] = '?';

                obj_desc_t msg;
                (void) sprintf(msg, "Drop all %s [y/n]", description);
                msg[strlen(description) - 1] = '.';

                prt(msg, 0, 0);

                query = inkey();

                if (query != 'y' && query != 'n') {
                    if (query != ESCAPE) {
                        bell();
                    }
                    erase_line(MSG_LINE, 0);
                    item = -1;
                }
            } else if (isupper((int) *which) && !verify((char *) prompt, item)) {
                item = -1;
            } else {
                query = 'y';
            }

            if (item >= 0) {
                free_turn_flag = false;

                inven_drop(item, query == 'y');
                check_strength();
            }

            selecting = false;

            // As a safety measure, set the player's inven weight
            // to 0, when the last object is dropped.
            if (inven_ctr == 0 && equip_ctr == 0) {
                inven_weight = 0;
            }
        }

        if (!free_turn_flag && scr_state == BLANK_SCR) {
            selecting = false;
        }
    }

    return selecting;
}

// Put an appropriate header.
static void inventoryDisplayAppropriateHeader() {
    if (scr_state == INVEN_SCR) {
        obj_desc_t msg;
        int weightQuotient = inven_weight / 10;
        int weightRemainder = inven_weight % 10;

        if (!show_weight_flag || inven_ctr == 0) {
            (void) sprintf(msg, "You are carrying %d.%d pounds. In your pack there is %s",
                           weightQuotient,
                           weightRemainder,
                           (inven_ctr == 0 ? "nothing." : "-")
            );
        } else {
            int limitQuotient = weight_limit() / 10;
            int limitRemainder = weight_limit() % 10;

            (void) sprintf(msg, "You are carrying %d.%d pounds. Your capacity is %d.%d pounds. In your pack is -",
                           weightQuotient,
                           weightRemainder,
                           limitQuotient,
                           limitRemainder
            );
        }

        prt(msg, 0, 0);
    } else if (scr_state == WEAR_SCR) {
        if (wear_high < wear_low) {
            prt("You have nothing you could wield.", 0, 0);
        } else {
            prt("You could wield -", 0, 0);
        }
    } else if (scr_state == EQUIP_SCR) {
        if (equip_ctr == 0) {
            prt("You are not using anything.", 0, 0);
        } else {
            prt("You are using -", 0, 0);
        }
    } else {
        prt("Allowed commands:", 0, 0);
    }

    erase_line(scr_base, scr_left);
}

// This does all the work.
void inven_command(char command) {
    free_turn_flag = true;

    save_screen();
    setInventoryCommandScreenState(command);

    do {
        if (isupper((int) command)) {
            command = (char) tolower((int) command);
        }

        // Simple command getting and screen selection.
        bool selecting = false;
        switch (command) {
            case 'i':
                displayInventory();
                break;
            case 'e':
                displayEquipment();
                break;
            case 't':
                selecting = inventoryTakeOffItem(selecting);
                break;
            case 'd':
                selecting = inventoryDropItem(&command, selecting);
                break;
            case 'w':
                selecting = inventoryWearWieldItem(selecting);
                break;
            case 'x':
                inventoryUnwieldItem();
                break;
            case ' ':
                // Dummy command to return again to main prompt.
                break;
            case '?':
                inven_screen(HELP_SCR);
                break;
            default:
                // Nonsense command
                bell();
                break;
        }

        // Clear the doing_inven flag here, instead of at beginning, so that
        // can use it to control when messages above appear.
        doing_inven = 0;

        // Keep looking for objects to drop/wear/take off/throw off
        char which = 'z';

        selecting = selectItemCommands(&command, &which, selecting);

        if (which == ESCAPE || scr_state == BLANK_SCR) {
            command = ESCAPE;
        } else if (!free_turn_flag) {
            // Save state for recovery if they want to call us again next turn.
            // Otherwise, set a dummy command to recover screen.
            if (selecting) {
                doing_inven = command;
            } else {
                doing_inven = ' ';
            }

            // flush last message before clearing screen_change and exiting
            msg_print(CNIL);

            // This lets us know if the world changes
            screen_change = false;

            command = ESCAPE;
        } else {
            inventoryDisplayAppropriateHeader();

            put_buffer("e/i/t/w/x/d/?/ESC:", scr_base, 60);
            command = inkey();

            erase_line(scr_base, scr_left);
        }
    } while (command != ESCAPE);

    if (scr_state != BLANK_SCR) {
        restore_screen();
    }

    calc_bonuses();
}

// Get the ID of an item and return the CTR value of it -RAK-
int get_item(int *com_val, const char *pmt, int i, int j, char *mask, const char *message) {
    int screenID = 1;
    bool full = false;

    if (j > INVEN_WIELD) {
        full = true;

        if (inven_ctr == 0) {
            screenID = 0;
            j = equip_ctr - 1;
        } else {
            j = inven_ctr - 1;
        }
    }

    if (inven_ctr < 1 && (!full || equip_ctr < 1)) {
        prt("You are not carrying anything.", 0, 0);
        return false;
    }

    *com_val = 0;

    bool itemFound = false;
    bool redrawScreen = false;

    do {
        if (redrawScreen) {
            if (screenID > 0) {
                (void) show_inven(i, j, false, 80, mask);
            } else {
                (void) show_equip(false, 80);
            }
        }

        vtype_t out_val;

        if (full) {
            (void) sprintf(
                    out_val,
                    "(%s: %c-%c,%s%s / for %s, or ESC) %s",
                    (screenID > 0 ? "Inven" : "Equip"),
                    i + 'a',
                    j + 'a',
                    (screenID > 0 ? " 0-9," : ""),
                    (redrawScreen ? "" : " * to see,"),
                    (screenID > 0 ? "Equip" : "Inven"),
                    pmt
            );
        } else {
            (void) sprintf(
                    out_val,
                    "(Items %c-%c,%s%s ESC to exit) %s",
                    i + 'a',
                    j + 'a',
                    (screenID > 0 ? " 0-9," : ""),
                    (redrawScreen ? "" : " * for inventory list,"),
                    pmt
            );
        }

        prt(out_val, 0, 0);

        bool commandFinished = false;
        while (!commandFinished) {
            char which = inkey();

            switch (which) {
                case ESCAPE:
                    screenID = -1;
                    commandFinished = true;

                    free_turn_flag = true;

                    break;
                case '/':
                    if (full) {
                        if (screenID > 0) {
                            if (equip_ctr == 0) {
                                prt("But you're not using anything -more-", 0, 0);
                                (void) inkey();
                            } else {
                                screenID = 0;
                                commandFinished = true;

                                if (redrawScreen) {
                                    j = equip_ctr;

                                    while (j < inven_ctr) {
                                        j++;
                                        erase_line(j, 0);
                                    }
                                }
                                j = equip_ctr - 1;
                            }

                            prt(out_val, 0, 0);
                        } else {
                            if (inven_ctr == 0) {
                                prt("But you're not carrying anything -more-", 0, 0);
                                (void) inkey();
                            } else {
                                screenID = 1;
                                commandFinished = true;

                                if (redrawScreen) {
                                    j = inven_ctr;

                                    while (j < equip_ctr) {
                                        j++;
                                        erase_line(j, 0);
                                    }
                                }
                                j = inven_ctr - 1;
                            }
                        }
                    }
                    break;
                case '*':
                    if (!redrawScreen) {
                        commandFinished = true;
                        save_screen();
                        redrawScreen = true;
                    }
                    break;
                default:
                    // look for item whose inscription matches "which"
                    if (which >= '0' && which <= '9' && screenID != 0) {
                        int m;

                        // Note: loop to find the inventory item
                        for (m = i; m < INVEN_WIELD && (inventory[m].inscrip[0] != which || inventory[m].inscrip[1] != '\0'); m++);

                        if (m < INVEN_WIELD) {
                            *com_val = m;
                        } else {
                            *com_val = -1;
                        }
                    } else if (isupper((int) which)) {
                        *com_val = which - 'A';
                    } else {
                        *com_val = which - 'a';
                    }

                    if (*com_val >= i && *com_val <= j && (mask == CNIL || mask[*com_val])) {
                        if (screenID == 0) {
                            i = 21;
                            j = *com_val;

                            do {
                                // Note: a simple loop to find first inventory item
                                while (inventory[++i].tval == TV_NOTHING);

                                j--;
                            } while (j >= 0);

                            *com_val = i;
                        }

                        if (isupper((int) which) && !verify("Try", *com_val)) {
                            screenID = -1;
                            commandFinished = true;

                            free_turn_flag = true;

                            break;
                        }

                        screenID = -1;
                        commandFinished = true;

                        itemFound = true;
                    } else if (message) {
                        msg_print(message);

                        // Set commandFinished to force redraw of the question.
                        commandFinished = true;
                    } else {
                        bell();
                    }
                    break;
            }
        }
    } while (screenID >= 0);

    if (redrawScreen) {
        restore_screen();
    }

    erase_line(MSG_LINE, 0);

    return itemFound;
}

// I may have written the town level code, but I'm not exactly
// proud of it.   Adding the stores required some real slucky
// hooks which I have not had time to re-think. -RAK-

// Returns true if player has no light -RAK-
bool no_light() {
    return !cave[char_row][char_col].tl && !cave[char_row][char_col].pl;
}

// map rogue_like direction commands into numbers
static char map_roguedir(char command) {
    switch (command) {
        case 'h':
            return '4';
        case 'y':
            return '7';
        case 'k':
            return '8';
        case 'u':
            return '9';
        case 'l':
            return '6';
        case 'n':
            return '3';
        case 'j':
            return '2';
        case 'b':
            return '1';
        case '.':
            return '5';
        default:
            return command;
    }
}

// Prompts for a direction -RAK-
// Direction memory added, for repeated commands.  -CJS
bool get_dir(char *prompt, int *dir) {
    static char prev_dir; // Direction memory. -CJS-

    // used in counted commands. -CJS-
    if (default_dir) {
        *dir = prev_dir;
        return true;
    }

    if (prompt == CNIL) {
        prompt = (char *) "Which direction?";
    }

    char command;

    while (true) {
        // Don't end a counted command. -CJS-
        int save = command_count;

        if (!get_com(prompt, &command)) {
            free_turn_flag = true;
            return false;
        }

        command_count = save;

        if (rogue_like_commands) {
            command = map_roguedir(command);
        }

        if (command >= '1' && command <= '9' && command != '5') {
            prev_dir = command - '0';
            *dir = prev_dir;
            return true;
        }

        bell();
    }
}

// Similar to get_dir, except that no memory exists, and it is -CJS-
// allowed to enter the null direction.
bool get_alldir(const char *prompt, int *dir) {
    char command;

    while (true) {
        if (!get_com(prompt, &command)) {
            free_turn_flag = true;
            return false;
        }

        if (rogue_like_commands) {
            command = map_roguedir(command);
        }

        if (command >= '1' && command <= '9') {
            *dir = command - '0';
            return true;
        }

        bell();
    }
}

// Moves creature record from one space to another -RAK-
// this always works correctly, even if y1==y2 and x1==x2
void move_rec(int y1, int x1, int y2, int x2) {
    int id = cave[y1][x1].cptr;
    cave[y1][x1].cptr = 0;
    cave[y2][x2].cptr = (uint8_t) id;
}

// Room is lit, make it appear -RAK-
void light_room(int posY, int posX) {
    int heightMiddle = (SCREEN_HEIGHT / 2);
    int widthMiddle = (SCREEN_WIDTH / 2);

    int top = (posY / heightMiddle) * heightMiddle;
    int left = (posX / widthMiddle) * widthMiddle;
    int bottom = top + heightMiddle - 1;
    int right = left + widthMiddle - 1;

    for (int y = top; y <= bottom; y++) {
        for (int x = left; x <= right; x++) {
            Cave_t *c_ptr = &cave[y][x];

            if (c_ptr->lr && !c_ptr->pl) {
                c_ptr->pl = true;

                if (c_ptr->fval == DARK_FLOOR) {
                    c_ptr->fval = LIGHT_FLOOR;
                }
                if (!c_ptr->fm && c_ptr->tptr != 0) {
                    int tval = t_list[c_ptr->tptr].tval;

                    if (tval >= TV_MIN_VISIBLE && tval <= TV_MAX_VISIBLE) {
                        c_ptr->fm = true;
                    }
                }
                print(loc_symbol(y, x), y, x);
            }
        }
    }
}

// Lights up given location -RAK-
void lite_spot(int y, int x) {
    if (panel_contains(y, x)) {
        print(loc_symbol(y, x), y, x);
    }
}

// Normal movement
// When FIND_FLAG,  light only permanent features
static void sub1_move_light(int y1, int x1, int y2, int x2) {
    if (light_flag) {
        // Turn off lamp light
        for (int y = y1 - 1; y <= y1 + 1; y++) {
            for (int x = x1 - 1; x <= x1 + 1; x++) {
                cave[y][x].tl = false;
            }
        }
        if (find_flag && !find_prself) {
            light_flag = false;
        }
    } else if (!find_flag || find_prself) {
        light_flag = true;
    }

    for (int y = y2 - 1; y <= y2 + 1; y++) {
        for (int x = x2 - 1; x <= x2 + 1; x++) {
            Cave_t *c_ptr = &cave[y][x];

            // only light up if normal movement
            if (light_flag) {
                c_ptr->tl = true;
            }

            if (c_ptr->fval >= MIN_CAVE_WALL) {
                c_ptr->pl = true;
            } else if (!c_ptr->fm && c_ptr->tptr != 0) {
                int tval = t_list[c_ptr->tptr].tval;

                if (tval >= TV_MIN_VISIBLE && tval <= TV_MAX_VISIBLE) {
                    c_ptr->fm = true;
                }
            }
        }
    }

    // From uppermost to bottom most lines player was on.
    int top, left, bottom, right;

    if (y1 < y2) {
        top = y1 - 1;
        bottom = y2 + 1;
    } else {
        top = y2 - 1;
        bottom = y1 + 1;
    }
    if (x1 < x2) {
        left = x1 - 1;
        right = x2 + 1;
    } else {
        left = x2 - 1;
        right = x1 + 1;
    }

    for (int y = top; y <= bottom; y++) {
        // Leftmost to rightmost do
        for (int x = left; x <= right; x++) {
            print(loc_symbol(y, x), y, x);
        }
    }
}

// When blinded,  move only the player symbol.
// With no light,  movement becomes involved.
static void sub3_move_light(int y1, int x1, int y2, int x2) {
    if (light_flag) {
        for (int y = y1 - 1; y <= y1 + 1; y++) {
            for (int x = x1 - 1; x <= x1 + 1; x++) {
                cave[y][x].tl = false;
                print(loc_symbol(y, x), y, x);
            }
        }

        light_flag = false;
    } else if (!find_flag || find_prself) {
        print(loc_symbol(y1, x1), y1, x1);
    }

    if (!find_flag || find_prself) {
        print('@', y2, x2);
    }
}

// Package for moving the character's light about the screen
// Four cases : Normal, Finding, Blind, and No light -RAK-
void move_light(int y1, int x1, int y2, int x2) {
    if (py.flags.blind > 0 || !player_light) {
        sub3_move_light(y1, x1, y2, x2);
    } else {
        sub1_move_light(y1, x1, y2, x2);
    }
}

// Something happens to disturb the player. -CJS-
// The first arg indicates a major disturbance, which affects search.
// The second arg indicates a light change.
void disturb(int s, int l) {
    command_count = 0;

    if (s && (py.flags.status & PY_SEARCH)) {
        search_off();
    }

    if (py.flags.rest != 0) {
        rest_off();
    }

    if (l || find_flag) {
        find_flag = 0;
        check_view();
    }

    flush();
}

// Search Mode enhancement -RAK-
void search_on() {
    change_speed(1);

    py.flags.status |= PY_SEARCH;

    prt_state();
    prt_speed();

    py.flags.food_digested++;
}

void search_off() {
    check_view();
    change_speed(-1);

    py.flags.status &= ~PY_SEARCH;

    prt_state();
    prt_speed();
    py.flags.food_digested--;
}

// Resting allows a player to safely restore his hp -RAK-
void rest() {
    int rest_num;

    if (command_count > 0) {
        rest_num = command_count;
        command_count = 0;
    } else {
        prt("Rest for how long? ", 0, 0);
        rest_num = 0;

        vtype_t rest_str;
        if (get_string(rest_str, 0, 19, 5)) {
            if (rest_str[0] == '*') {
                rest_num = -MAX_SHORT;
            } else {
                rest_num = atoi(rest_str);
            }
        }
    }

    // check for reasonable value, must be positive number
    // in range of a short, or must be -MAX_SHORT
    if (rest_num == -MAX_SHORT || (rest_num > 0 && rest_num < MAX_SHORT)) {
        if (py.flags.status & PY_SEARCH) {
            search_off();
        }

        py.flags.rest = (int16_t) rest_num;
        py.flags.status |= PY_REST;
        prt_state();
        py.flags.food_digested--;

        prt("Press any key to stop resting...", 0, 0);
        put_qio();

        return;
    }

    // Something went wrong
    if (rest_num != 0) {
        msg_print("Invalid rest count.");
    }
    erase_line(MSG_LINE, 0);

    free_turn_flag = true;
}

void rest_off() {
    py.flags.rest = 0;
    py.flags.status &= ~PY_REST;

    prt_state();

    // flush last message, or delete "press any key" message
    msg_print(CNIL);

    py.flags.food_digested++;
}

// Attacker's level and plusses,  defender's AC -RAK-
bool test_hit(int bth, int level, int pth, int ac, int attack_type) {
    disturb(1, 0);

    // pth could be less than 0 if player wielding weapon too heavy for him
    int i = bth + pth * BTH_PLUS_ADJ + (level * class_level_adj[py.misc.pclass][attack_type]);

    // always miss 1 out of 20, always hit 1 out of 20
    int die = randint(20);

    // normal hit
    return (die != 1 && (die == 20 || (i > 0 && randint(i) > ac)));
}

// Decreases players hit points and sets death flag if necessary -RAK-
void take_hit(int damage, const char *hit_from) {
    if (py.flags.invuln > 0) {
        damage = 0;
    }
    py.misc.chp -= damage;

    if (py.misc.chp >= 0) {
        prt_chp();
        return;
    }

    if (!death) {
        death = true;

        (void) strcpy(died_from, hit_from);

        total_winner = false;
    }

    new_level_flag = true;
}
