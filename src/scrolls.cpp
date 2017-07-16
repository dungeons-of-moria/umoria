// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Scroll code

#include "headers.h"
#include "externs.h"

// Note: naming of all the scroll functions needs verifying -MRC-

static bool playerCanReadScroll(int *item_pos_start, int *item_pos_end) {
    if (py.flags.blind > 0) {
        printMessage("You can't see to read the scroll.");
        return false;
    }

    if (playerNoLight()) {
        printMessage("You have no light to read by.");
        return false;
    }

    if (py.flags.confused > 0) {
        printMessage("You are too confused to read a scroll.");
        return false;
    }

    if (inventory_count == 0) {
        printMessage("You are not carrying anything!");
        return false;
    }

    if (!inventoryFindRange(TV_SCROLL1, TV_SCROLL2, item_pos_start, item_pos_end)) {
        printMessage("You are not carrying any scrolls!");
        return false;
    }

    return true;
}

static int inventoryItemIdOfCursedEquipment() {
    int item_count = 0;
    int items[6];

    if (inventory[INVEN_BODY].tval != TV_NOTHING) {
        items[item_count++] = INVEN_BODY;
    }
    if (inventory[INVEN_ARM].tval != TV_NOTHING) {
        items[item_count++] = INVEN_ARM;
    }
    if (inventory[INVEN_OUTER].tval != TV_NOTHING) {
        items[item_count++] = INVEN_OUTER;
    }
    if (inventory[INVEN_HANDS].tval != TV_NOTHING) {
        items[item_count++] = INVEN_HANDS;
    }
    if (inventory[INVEN_HEAD].tval != TV_NOTHING) {
        items[item_count++] = INVEN_HEAD;
    }
    // also enchant boots
    if (inventory[INVEN_FEET].tval != TV_NOTHING) {
        items[item_count++] = INVEN_FEET;
    }

    int item_id = 0;

    if (item_count > 0) {
        item_id = items[randomNumber(item_count) - 1];
    }

    if (inventory[INVEN_BODY].flags & TR_CURSED) {
        item_id = INVEN_BODY;
    } else if (inventory[INVEN_ARM].flags & TR_CURSED) {
        item_id = INVEN_ARM;
    } else if (inventory[INVEN_OUTER].flags & TR_CURSED) {
        item_id = INVEN_OUTER;
    } else if (inventory[INVEN_HEAD].flags & TR_CURSED) {
        item_id = INVEN_HEAD;
    } else if (inventory[INVEN_HANDS].flags & TR_CURSED) {
        item_id = INVEN_HANDS;
    } else if (inventory[INVEN_FEET].flags & TR_CURSED) {
        item_id = INVEN_FEET;
    }

    return item_id;
}

static bool scrollEnchantWeaponToHit() {
    Inventory_t *item = &inventory[INVEN_WIELD];

    if (item->tval == TV_NOTHING) {
        return false;
    }

    obj_desc_t msg, desc;
    itemDescription(desc, item, false);

    (void) sprintf(msg, "Your %s glows faintly!", desc);
    printMessage(msg);

    if (enchant(&item->tohit, 10)) {
        item->flags &= ~TR_CURSED;
        playerRecalculateBonuses();
    } else {
        printMessage("The enchantment fails.");
    }

    return true;
}

static bool scrollEnchantWeaponToDamage() {
    Inventory_t *item = &inventory[INVEN_WIELD];

    if (item->tval == TV_NOTHING) {
        return false;
    }

    obj_desc_t msg, desc;
    itemDescription(desc, item, false);

    (void) sprintf(msg, "Your %s glows faintly!", desc);
    printMessage(msg);

    int16_t scroll_type;

    if (item->tval >= TV_HAFTED && item->tval <= TV_DIGGING) {
        scroll_type = item->damage[0] * item->damage[1];
    } else {
        // Bows' and arrows' enchantments should not be
        // limited by their low base damages
        scroll_type = 10;
    }

    if (enchant(&item->todam, scroll_type)) {
        item->flags &= ~TR_CURSED;
        playerRecalculateBonuses();
    } else {
        printMessage("The enchantment fails.");
    }

    return true;
}

static bool scrollEnchantItemToAC() {
    int item_id = inventoryItemIdOfCursedEquipment();

    if (item_id <= 0) {
        return false;
    }

    Inventory_t *item = &inventory[item_id];

    obj_desc_t msg, desc;
    itemDescription(desc, item, false);

    (void) sprintf(msg, "Your %s glows faintly!", desc);
    printMessage(msg);

    if (enchant(&item->toac, 10)) {
        item->flags &= ~TR_CURSED;
        playerRecalculateBonuses();
    } else {
        printMessage("The enchantment fails.");
    }

    return true;
}

static int scrollIdentifyItem(int item_id, bool *is_used_up) {
    printMessage("This is an identify scroll.");

    *is_used_up = ident_spell();

    // The identify may merge objects, causing the identify scroll
    // to move to a different place.  Check for that here.  It can
    // move arbitrarily far if an identify scroll was used on
    // another identify scroll, but it always moves down.
    Inventory_t *item = &inventory[item_id];
    while (item_id > 0 && (item->tval != TV_SCROLL1 || item->flags != 0x00000008)) {
        item_id--;
        item = &inventory[item_id];
    }

    return item_id;
}

static bool scrollRemoveCurse() {
    if (remove_curse()) {
        printMessage("You feel as if someone is watching over you.");
        return true;
    }
    return false;
}

static bool readSummonMonsterScroll() {
    bool identified = false;

    for (int k = 0; k < randomNumber(3); k++) {
        int y = (int) char_row;
        int x = (int) char_col;
        identified |= monsterSummon(&y, &x, false);
    }

    return identified;
}

static void readTeleportLevelScroll() {
    current_dungeon_level += (-3) + 2 * randomNumber(2);
    if (current_dungeon_level < 1) {
        current_dungeon_level = 1;
    }
    generate_new_level = true;
}

static bool readConfuseMonsterScroll() {
    if (!py.flags.confuse_monster) {
        printMessage("Your hands begin to glow.");
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
    itemDescription(desc, i_ptr, false);

    (void) sprintf(msg, "Your %s glows brightly!", desc);
    printMessage(msg);

    bool flag = false;

    for (int k = 0; k < randomNumber(2); k++) {
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

    for (int k = 0; k < randomNumber(2); k++) {
        if (enchant(&i_ptr->todam, scrollType)) {
            flag = true;
        }
    }

    if (flag) {
        i_ptr->flags &= ~TR_CURSED;
        playerRecalculateBonuses();
    } else {
        printMessage("The enchantment fails.");
    }

    return true;
}

static bool readCurseWeaponScroll() {
    Inventory_t *i_ptr = &inventory[INVEN_WIELD];

    if (i_ptr->tval == TV_NOTHING) {
        return false;
    }

    obj_desc_t msg, desc;
    itemDescription(desc, i_ptr, false);

    (void) sprintf(msg, "Your %s glows black, fades.", desc);
    printMessage(msg);

    itemRemoveMagicNaming(i_ptr);

    i_ptr->tohit = (int16_t) (-randomNumber(5) - randomNumber(5));
    i_ptr->todam = (int16_t) (-randomNumber(5) - randomNumber(5));
    i_ptr->toac = 0;

    // Must call playerAdjustBonusesForItem() before set (clear) flags, and
    // must call playerRecalculateBonuses() after set (clear) flags, so that
    // all attributes will be properly turned off.
    playerAdjustBonusesForItem(i_ptr, -1);
    i_ptr->flags = TR_CURSED;
    playerRecalculateBonuses();

    return true;
}

static bool readEnchantArmorScroll() {
    int id = inventoryItemIdOfCursedEquipment();

    if (id <= 0) {
        return false;
    }

    Inventory_t *i_ptr = &inventory[id];

    obj_desc_t msg, desc;
    itemDescription(desc, i_ptr, false);

    (void) sprintf(msg, "Your %s glows brightly!", desc);
    printMessage(msg);

    bool flag = false;

    for (int k = 0; k < randomNumber(2) + 1; k++) {
        if (enchant(&i_ptr->toac, 10)) {
            flag = true;
        }
    }

    if (flag) {
        i_ptr->flags &= ~TR_CURSED;
        playerRecalculateBonuses();
    } else {
        printMessage("The enchantment fails.");
    }

    return true;
}

static bool readCurseArmorScroll() {
    int id;

    if (inventory[INVEN_BODY].tval != TV_NOTHING && randomNumber(4) == 1) {
        id = INVEN_BODY;
    } else if (inventory[INVEN_ARM].tval != TV_NOTHING && randomNumber(3) == 1) {
        id = INVEN_ARM;
    } else if (inventory[INVEN_OUTER].tval != TV_NOTHING && randomNumber(3) == 1) {
        id = INVEN_OUTER;
    } else if (inventory[INVEN_HEAD].tval != TV_NOTHING && randomNumber(3) == 1) {
        id = INVEN_HEAD;
    } else if (inventory[INVEN_HANDS].tval != TV_NOTHING && randomNumber(3) == 1) {
        id = INVEN_HANDS;
    } else if (inventory[INVEN_FEET].tval != TV_NOTHING && randomNumber(3) == 1) {
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
    itemDescription(desc, i_ptr, false);

    (void) sprintf(msg, "Your %s glows black, fades.", desc);
    printMessage(msg);

    itemRemoveMagicNaming(i_ptr);

    i_ptr->flags = TR_CURSED;
    i_ptr->tohit = 0;
    i_ptr->todam = 0;
    i_ptr->toac = (int16_t) (-randomNumber(5) - randomNumber(5));

    playerRecalculateBonuses();

    return true;
}

static bool readSummonUndeadScroll() {
    bool identified = false;

    for (int k = 0; k < randomNumber(3); k++) {
        int y = char_row;
        int x = char_col;
        identified |= monsterSummonUndead(&y, &x);
    }

    return identified;
}

static void readWordOfRecallScroll() {
    if (py.flags.word_recall == 0) {
        py.flags.word_recall = (int16_t) (25 + randomNumber(30));
    }
    printMessage("The air about you becomes charged.");
}

// Scrolls for the reading -RAK-
void read_scroll() {
    player_free_turn = true;

    int j, k;
    if (!playerCanReadScroll(&j, &k)) {
        return;
    }

    int itemID;
    if (!inventoryGetInputForItemId(&itemID, "Read which scroll?", j, k, CNIL, CNIL)) {
        return;
    }

    // From here on, no free turn for the player
    player_free_turn = false;

    bool used_up = true;
    bool identified = false;

    Inventory_t *i_ptr = &inventory[itemID];
    uint32_t flags = i_ptr->flags;

    while (flags != 0) {
        int scrollType = getAndClearFirstBit(&flags) + 1;

        if (i_ptr->tval == TV_SCROLL2) {
            scrollType += 32;
        }

        switch (scrollType) {
            case 1:
                identified = scrollEnchantWeaponToHit();
                break;
            case 2:
                identified = scrollEnchantWeaponToDamage();
                break;
            case 3:
                identified = scrollEnchantItemToAC();
                break;
            case 4:
                itemID = scrollIdentifyItem(itemID, &used_up);
                identified = true;
                break;
            case 5:
                identified = scrollRemoveCurse();
                break;
            case 6:
                identified = light_area(char_row, char_col);
                break;
            case 7:
                identified = readSummonMonsterScroll();
                break;
            case 8:
                playerTeleport(10); // Teleport Short, aka Phase Door
                identified = true;
                break;
            case 9:
                playerTeleport(100); // Teleport Long
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
                printMessage("This is a mass genocide scroll.");
                (void) mass_genocide();
                identified = true;
                break;
            case 20:
                identified = detect_invisible();
                break;
            case 21:
                printMessage("There is a high pitched humming noise.");
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
                printMessage("This is a Recharge-Item scroll.");
                used_up = recharge(60);
                identified = true;
                break;
            case 26:
                printMessage("This is a genocide scroll.");
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
                bless(randomNumber(12) + 6);
                identified = true;
                break;
            case 39:
                bless(randomNumber(24) + 12);
                identified = true;
                break;
            case 40:
                bless(randomNumber(48) + 24);
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
                printMessage("Internal error in scroll()");
                break;
        }
    }

    i_ptr = &inventory[itemID];

    if (identified) {
        if (!itemSetColorlessAsIdentifed(i_ptr)) {
            // round half-way case up
            py.misc.exp += (i_ptr->level + (py.misc.lev >> 1)) / py.misc.lev;
            displayCharacterExperience();

            itemIdentify(&itemID);
        }
    } else if (!itemSetColorlessAsIdentifed(i_ptr)) {
        itemSetAsTried(i_ptr);
    }

    if (used_up) {
        itemTypeRemainingCountDescription(itemID);
        inventoryDestroyItem(itemID);
    }
}
