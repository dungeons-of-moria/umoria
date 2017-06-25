// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Scroll code

#include "headers.h"
#include "externs.h"

// Note: naming of all the scroll functions needs verifying -MRC-

static bool canReadScroll(int *j, int *k) {
    if (py.flags.blind > 0) {
        msg_print("You can't see to read the scroll.");
        return false;
    }

    if (no_light()) {
        msg_print("You have no light to read by.");
        return false;
    }

    if (py.flags.confused > 0) {
        msg_print("You are too confused to read a scroll.");
        return false;
    }

    if (inven_ctr == 0) {
        msg_print("You are not carrying anything!");
        return false;
    }

    if (!find_range(TV_SCROLL1, TV_SCROLL2, j, k)) {
        msg_print("You are not carrying any scrolls!");
        return false;
    }

    return true;
}

static int getEnchantedItemID() {
    int itemCount = 0;
    int items[6];

    if (inventory[INVEN_BODY].tval != TV_NOTHING) {
        items[itemCount++] = INVEN_BODY;
    }
    if (inventory[INVEN_ARM].tval != TV_NOTHING) {
        items[itemCount++] = INVEN_ARM;
    }
    if (inventory[INVEN_OUTER].tval != TV_NOTHING) {
        items[itemCount++] = INVEN_OUTER;
    }
    if (inventory[INVEN_HANDS].tval != TV_NOTHING) {
        items[itemCount++] = INVEN_HANDS;
    }
    if (inventory[INVEN_HEAD].tval != TV_NOTHING) {
        items[itemCount++] = INVEN_HEAD;
    }
    // also enchant boots
    if (inventory[INVEN_FEET].tval != TV_NOTHING) {
        items[itemCount++] = INVEN_FEET;
    }

    int item = 0;

    if (itemCount > 0) {
        item = items[randint(itemCount) - 1];
    }

    if (TR_CURSED & inventory[INVEN_BODY].flags) {
        item = INVEN_BODY;
    } else if (TR_CURSED & inventory[INVEN_ARM].flags) {
        item = INVEN_ARM;
    } else if (TR_CURSED & inventory[INVEN_OUTER].flags) {
        item = INVEN_OUTER;
    } else if (TR_CURSED & inventory[INVEN_HEAD].flags) {
        item = INVEN_HEAD;
    } else if (TR_CURSED & inventory[INVEN_HANDS].flags) {
        item = INVEN_HANDS;
    } else if (TR_CURSED & inventory[INVEN_FEET].flags) {
        item = INVEN_FEET;
    }

    return item;
}

static bool readEnchantWeaponToHitScroll() {
    Inventory_t *i_ptr = &inventory[INVEN_WIELD];

    if (i_ptr->tval == TV_NOTHING) {
        return false;
    }

    obj_desc_t msg, desc;
    objdes(desc, i_ptr, false);

    (void) sprintf(msg, "Your %s glows faintly!", desc);
    msg_print(msg);

    if (enchant(&i_ptr->tohit, 10)) {
        i_ptr->flags &= ~TR_CURSED;
        calc_bonuses();
    } else {
        msg_print("The enchantment fails.");
    }

    return true;
}

static bool readEnchantWeaponToDamageScroll() {
    Inventory_t *i_ptr = &inventory[INVEN_WIELD];

    if (i_ptr->tval == TV_NOTHING) {
        return false;
    }

    obj_desc_t msg, desc;
    objdes(desc, i_ptr, false);

    (void) sprintf(msg, "Your %s glows faintly!", desc);
    msg_print(msg);

    int16_t scrollType;

    if (i_ptr->tval >= TV_HAFTED && i_ptr->tval <= TV_DIGGING) {
        scrollType = i_ptr->damage[0] * i_ptr->damage[1];
    } else {
        // Bows' and arrows' enchantments should not be
        // limited by their low base damages
        scrollType = 10;
    }

    if (enchant(&i_ptr->todam, scrollType)) {
        i_ptr->flags &= ~TR_CURSED;
        calc_bonuses();
    } else {
        msg_print("The enchantment fails.");
    }

    return true;
}

static bool readEnchantItemToACScroll() {
    int id = getEnchantedItemID();

    if (id <= 0) {
        return false;
    }

    Inventory_t *i_ptr = &inventory[id];

    obj_desc_t msg, desc;
    objdes(desc, i_ptr, false);

    (void) sprintf(msg, "Your %s glows faintly!", desc);
    msg_print(msg);

    if (enchant(&i_ptr->toac, 10)) {
        i_ptr->flags &= ~TR_CURSED;
        calc_bonuses();
    } else {
        msg_print("The enchantment fails.");
    }

    return true;
}

static int readIdentifyScroll(int itemID, bool *used_up) {
    msg_print("This is an identify scroll.");

    *used_up = ident_spell();

    // The identify may merge objects, causing the identify scroll
    // to move to a different place.  Check for that here.  It can
    // move arbitrarily far if an identify scroll was used on
    // another identify scroll, but it always moves down.
    Inventory_t *i_ptr = &inventory[itemID];
    while (itemID > 0 && (i_ptr->tval != TV_SCROLL1 || i_ptr->flags != 0x00000008)) {
        itemID--;
        i_ptr = &inventory[itemID];
    }

    return itemID;
}

static bool readRemoveCurseScroll() {
    if (remove_curse()) {
        msg_print("You feel as if someone is watching over you.");
        return true;
    }
    return false;
}

static bool readSummonMonsterScroll() {
    bool identified = false;

    for (int k = 0; k < randint(3); k++) {
        int y = (int) char_row;
        int x = (int) char_col;
        identified |= summon_monster(&y, &x, false);
    }

    return identified;
}

static void readTeleportLevelScroll() {
    current_dungeon_level += (-3) + 2 * randint(2);
    if (current_dungeon_level < 1) {
        current_dungeon_level = 1;
    }
    generate_new_level = true;
}

static bool readConfuseMonsterScroll() {
    if (!py.flags.confuse_monster) {
        msg_print("Your hands begin to glow.");
        py.flags.confuse_monster = true;
        return true;
    }
    return false;
}

static bool readEnchantWeaponScroll() {
    Inventory_t *i_ptr = &inventory[INVEN_WIELD];

    if (i_ptr->tval == TV_NOTHING) {
        return false;
    }

    obj_desc_t msg, desc;
    objdes(desc, i_ptr, false);

    (void) sprintf(msg, "Your %s glows brightly!", desc);
    msg_print(msg);

    bool flag = false;

    for (int k = 0; k < randint(2); k++) {
        if (enchant(&i_ptr->tohit, 10)) {
            flag = true;
        }
    }

    int16_t scrollType;

    if (i_ptr->tval >= TV_HAFTED && i_ptr->tval <= TV_DIGGING) {
        scrollType = i_ptr->damage[0] * i_ptr->damage[1];
    } else {
        // Bows' and arrows' enchantments should not be limited
        // by their low base damages
        scrollType = 10;
    }

    for (int k = 0; k < randint(2); k++) {
        if (enchant(&i_ptr->todam, scrollType)) {
            flag = true;
        }
    }

    if (flag) {
        i_ptr->flags &= ~TR_CURSED;
        calc_bonuses();
    } else {
        msg_print("The enchantment fails.");
    }

    return true;
}

static bool readCurseWeaponScroll() {
    Inventory_t *i_ptr = &inventory[INVEN_WIELD];

    if (i_ptr->tval == TV_NOTHING) {
        return false;
    }

    obj_desc_t msg, desc;
    objdes(desc, i_ptr, false);

    (void) sprintf(msg, "Your %s glows black, fades.", desc);
    msg_print(msg);

    unmagic_name(i_ptr);

    i_ptr->tohit = (int16_t) (-randint(5) - randint(5));
    i_ptr->todam = (int16_t) (-randint(5) - randint(5));
    i_ptr->toac = 0;

    // Must call py_bonuses() before set (clear) flags, and
    // must call calc_bonuses() after set (clear) flags, so that
    // all attributes will be properly turned off.
    py_bonuses(i_ptr, -1);
    i_ptr->flags = TR_CURSED;
    calc_bonuses();

    return true;
}

static bool readEnchantArmorScroll() {
    int id = getEnchantedItemID();

    if (id <= 0) {
        return false;
    }

    Inventory_t *i_ptr = &inventory[id];

    obj_desc_t msg, desc;
    objdes(desc, i_ptr, false);

    (void) sprintf(msg, "Your %s glows brightly!", desc);
    msg_print(msg);

    bool flag = false;

    for (int k = 0; k < randint(2) + 1; k++) {
        if (enchant(&i_ptr->toac, 10)) {
            flag = true;
        }
    }

    if (flag) {
        i_ptr->flags &= ~TR_CURSED;
        calc_bonuses();
    } else {
        msg_print("The enchantment fails.");
    }

    return true;
}

static bool readCurseArmorScroll() {
    int id;

    if (inventory[INVEN_BODY].tval != TV_NOTHING && randint(4) == 1) {
        id = INVEN_BODY;
    } else if (inventory[INVEN_ARM].tval != TV_NOTHING && randint(3) == 1) {
        id = INVEN_ARM;
    } else if (inventory[INVEN_OUTER].tval != TV_NOTHING && randint(3) == 1) {
        id = INVEN_OUTER;
    } else if (inventory[INVEN_HEAD].tval != TV_NOTHING && randint(3) == 1) {
        id = INVEN_HEAD;
    } else if (inventory[INVEN_HANDS].tval != TV_NOTHING && randint(3) == 1) {
        id = INVEN_HANDS;
    } else if (inventory[INVEN_FEET].tval != TV_NOTHING && randint(3) == 1) {
        id = INVEN_FEET;
    } else if (inventory[INVEN_BODY].tval != TV_NOTHING) {
        id = INVEN_BODY;
    } else if (inventory[INVEN_ARM].tval != TV_NOTHING) {
        id = INVEN_ARM;
    } else if (inventory[INVEN_OUTER].tval != TV_NOTHING) {
        id = INVEN_OUTER;
    } else if (inventory[INVEN_HEAD].tval != TV_NOTHING) {
        id = INVEN_HEAD;
    } else if (inventory[INVEN_HANDS].tval != TV_NOTHING) {
        id = INVEN_HANDS;
    } else if (inventory[INVEN_FEET].tval != TV_NOTHING) {
        id = INVEN_FEET;
    } else {
        id = 0;
    }

    if (id <= 0) {
        return false;
    }

    Inventory_t *i_ptr = &inventory[id];

    obj_desc_t msg, desc;
    objdes(desc, i_ptr, false);

    (void) sprintf(msg, "Your %s glows black, fades.", desc);
    msg_print(msg);

    unmagic_name(i_ptr);

    i_ptr->flags = TR_CURSED;
    i_ptr->tohit = 0;
    i_ptr->todam = 0;
    i_ptr->toac = (int16_t) (-randint(5) - randint(5));

    calc_bonuses();

    return true;
}

static bool readSummonUndeadScroll() {
    bool identified = false;

    for (int k = 0; k < randint(3); k++) {
        int y = char_row;
        int x = char_col;
        identified |= summon_undead(&y, &x);
    }

    return identified;
}

static void readWordOfRecallScroll() {
    if (py.flags.word_recall == 0) {
        py.flags.word_recall = (int16_t) (25 + randint(30));
    }
    msg_print("The air about you becomes charged.");
}

// Scrolls for the reading -RAK-
void read_scroll() {
    free_turn_flag = true;

    int j, k;
    if (!canReadScroll(&j, &k)) {
        return;
    }

    int itemID;
    if (!get_item(&itemID, "Read which scroll?", j, k, CNIL, CNIL)) {
        return;
    }

    // From here on, no free turn for the player
    free_turn_flag = false;

    bool used_up = true;
    bool identified = false;

    Inventory_t *i_ptr = &inventory[itemID];
    uint32_t flags = i_ptr->flags;

    while (flags != 0) {
        int scrollType = bit_pos(&flags) + 1;

        if (i_ptr->tval == TV_SCROLL2) {
            scrollType += 32;
        }

        switch (scrollType) {
            case 1:
                identified = readEnchantWeaponToHitScroll();
                break;
            case 2:
                identified = readEnchantWeaponToDamageScroll();
                break;
            case 3:
                identified = readEnchantItemToACScroll();
                break;
            case 4:
                itemID = readIdentifyScroll(itemID, &used_up);
                identified = true;
                break;
            case 5:
                identified = readRemoveCurseScroll();
                break;
            case 6:
                identified = light_area(char_row, char_col);
                break;
            case 7:
                identified = readSummonMonsterScroll();
                break;
            case 8:
                teleport(10); // Teleport Short, aka Phase Door
                identified = true;
                break;
            case 9:
                teleport(100); // Teleport Long
                identified = true;
                break;
            case 10:
                readTeleportLevelScroll();
                identified = true;
                break;
            case 11:
                identified = readConfuseMonsterScroll();
                break;
            case 12:
                map_area();
                identified = true;
                break;
            case 13:
                identified = sleep_monsters1(char_row, char_col);
                break;
            case 14:
                warding_glyph();
                identified = true;
                break;
            case 15:
                identified = detect_treasure();
                break;
            case 16:
                identified = detect_object();
                break;
            case 17:
                identified = detect_trap();
                break;
            case 18:
                identified = detect_sdoor();
                break;
            case 19:
                msg_print("This is a mass genocide scroll.");
                (void) mass_genocide();
                identified = true;
                break;
            case 20:
                identified = detect_invisible();
                break;
            case 21:
                msg_print("There is a high pitched humming noise.");
                (void) aggravate_monster(20);
                identified = true;
                break;
            case 22:
                identified = trap_creation();
                break;
            case 23:
                identified = td_destroy();
                break;
            case 24:
                identified = door_creation();
                break;
            case 25:
                msg_print("This is a Recharge-Item scroll.");
                used_up = recharge(60);
                identified = true;
                break;
            case 26:
                msg_print("This is a genocide scroll.");
                (void) genocide();
                identified = true;
                break;
            case 27:
                identified = unlight_area(char_row, char_col);
                break;
            case 28:
                identified = protect_evil();
                break;
            case 29:
                create_food();
                identified = true;
                break;
            case 30:
                identified = dispel_creature(CD_UNDEAD, 60);
                break;
            case 33:
                identified = readEnchantWeaponScroll();
                break;
            case 34:
                identified = readCurseWeaponScroll();
                break;
            case 35:
                identified = readEnchantArmorScroll();
                break;
            case 36:
                identified = readCurseArmorScroll();
                break;
            case 37:
                identified = readSummonUndeadScroll();
                break;
            case 38:
                bless(randint(12) + 6);
                identified = true;
                break;
            case 39:
                bless(randint(24) + 12);
                identified = true;
                break;
            case 40:
                bless(randint(48) + 24);
                identified = true;
                break;
            case 41:
                readWordOfRecallScroll();
                identified = true;
                break;
            case 42:
                destroy_area(char_row, char_col);
                identified = true;
                break;
            default:
                msg_print("Internal error in scroll()");
                break;
        }
    }

    i_ptr = &inventory[itemID];

    if (identified) {
        if (!known1_p(i_ptr)) {
            // round half-way case up
            py.misc.exp += (i_ptr->level + (py.misc.lev >> 1)) / py.misc.lev;
            prt_experience();

            identify(&itemID);
        }
    } else if (!known1_p(i_ptr)) {
        sample(i_ptr);
    }

    if (used_up) {
        desc_remain(itemID);
        inven_destroy(itemID);
    }
}
