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

    *is_used_up = spellIdentifyItem();

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

static bool scrollSummonMonster() {
    bool identified = false;

    for (int i = 0; i < randomNumber(3); i++) {
        int y = (int) char_row;
        int x = (int) char_col;
        identified |= monsterSummon(&y, &x, false);
    }

    return identified;
}

static void scrollTeleportLevel() {
    current_dungeon_level += (-3) + 2 * randomNumber(2);
    if (current_dungeon_level < 1) {
        current_dungeon_level = 1;
    }
    generate_new_level = true;
}

static bool scrollConfuseMonster() {
    if (!py.flags.confuse_monster) {
        printMessage("Your hands begin to glow.");
        py.flags.confuse_monster = true;
        return true;
    }
    return false;
}

static bool scrollEnchantWeapon() {
    Inventory_t *item = &inventory[INVEN_WIELD];

    if (item->tval == TV_NOTHING) {
        return false;
    }

    obj_desc_t msg, desc;
    itemDescription(desc, item, false);

    (void) sprintf(msg, "Your %s glows brightly!", desc);
    printMessage(msg);

    bool enchanted = false;

    for (int i = 0; i < randomNumber(2); i++) {
        if (enchant(&item->tohit, 10)) {
            enchanted = true;
        }
    }

    int16_t scroll_type;

    if (item->tval >= TV_HAFTED && item->tval <= TV_DIGGING) {
        scroll_type = item->damage[0] * item->damage[1];
    } else {
        // Bows' and arrows' enchantments should not be limited
        // by their low base damages
        scroll_type = 10;
    }

    for (int i = 0; i < randomNumber(2); i++) {
        if (enchant(&item->todam, scroll_type)) {
            enchanted = true;
        }
    }

    if (enchanted) {
        item->flags &= ~TR_CURSED;
        playerRecalculateBonuses();
    } else {
        printMessage("The enchantment fails.");
    }

    return true;
}

static bool scrollCurseWeapon() {
    Inventory_t *item = &inventory[INVEN_WIELD];

    if (item->tval == TV_NOTHING) {
        return false;
    }

    obj_desc_t msg, desc;
    itemDescription(desc, item, false);

    (void) sprintf(msg, "Your %s glows black, fades.", desc);
    printMessage(msg);

    itemRemoveMagicNaming(item);

    item->tohit = (int16_t) (-randomNumber(5) - randomNumber(5));
    item->todam = (int16_t) (-randomNumber(5) - randomNumber(5));
    item->toac = 0;

    // Must call playerAdjustBonusesForItem() before set (clear) flags, and
    // must call playerRecalculateBonuses() after set (clear) flags, so that
    // all attributes will be properly turned off.
    playerAdjustBonusesForItem(item, -1);
    item->flags = TR_CURSED;
    playerRecalculateBonuses();

    return true;
}

static bool scrollEnchantArmor() {
    int item_id = inventoryItemIdOfCursedEquipment();

    if (item_id <= 0) {
        return false;
    }

    Inventory_t *item = &inventory[item_id];

    obj_desc_t msg, desc;
    itemDescription(desc, item, false);

    (void) sprintf(msg, "Your %s glows brightly!", desc);
    printMessage(msg);

    bool enchanted = false;

    for (int i = 0; i < randomNumber(2) + 1; i++) {
        if (enchant(&item->toac, 10)) {
            enchanted = true;
        }
    }

    if (enchanted) {
        item->flags &= ~TR_CURSED;
        playerRecalculateBonuses();
    } else {
        printMessage("The enchantment fails.");
    }

    return true;
}

static bool scrollCurseArmor() {
    int item_id;

    if (inventory[INVEN_BODY].tval != TV_NOTHING && randomNumber(4) == 1) {
        item_id = INVEN_BODY;
    } else if (inventory[INVEN_ARM].tval != TV_NOTHING && randomNumber(3) == 1) {
        item_id = INVEN_ARM;
    } else if (inventory[INVEN_OUTER].tval != TV_NOTHING && randomNumber(3) == 1) {
        item_id = INVEN_OUTER;
    } else if (inventory[INVEN_HEAD].tval != TV_NOTHING && randomNumber(3) == 1) {
        item_id = INVEN_HEAD;
    } else if (inventory[INVEN_HANDS].tval != TV_NOTHING && randomNumber(3) == 1) {
        item_id = INVEN_HANDS;
    } else if (inventory[INVEN_FEET].tval != TV_NOTHING && randomNumber(3) == 1) {
        item_id = INVEN_FEET;
    } else if (inventory[INVEN_BODY].tval != TV_NOTHING) {
        item_id = INVEN_BODY;
    } else if (inventory[INVEN_ARM].tval != TV_NOTHING) {
        item_id = INVEN_ARM;
    } else if (inventory[INVEN_OUTER].tval != TV_NOTHING) {
        item_id = INVEN_OUTER;
    } else if (inventory[INVEN_HEAD].tval != TV_NOTHING) {
        item_id = INVEN_HEAD;
    } else if (inventory[INVEN_HANDS].tval != TV_NOTHING) {
        item_id = INVEN_HANDS;
    } else if (inventory[INVEN_FEET].tval != TV_NOTHING) {
        item_id = INVEN_FEET;
    } else {
        item_id = 0;
    }

    if (item_id <= 0) {
        return false;
    }

    Inventory_t *item = &inventory[item_id];

    obj_desc_t msg, desc;
    itemDescription(desc, item, false);

    (void) sprintf(msg, "Your %s glows black, fades.", desc);
    printMessage(msg);

    itemRemoveMagicNaming(item);

    item->flags = TR_CURSED;
    item->tohit = 0;
    item->todam = 0;
    item->toac = (int16_t) (-randomNumber(5) - randomNumber(5));

    playerRecalculateBonuses();

    return true;
}

static bool scrollSummonUndead() {
    bool identified = false;

    for (int i = 0; i < randomNumber(3); i++) {
        int y = char_row;
        int x = char_col;
        identified |= monsterSummonUndead(&y, &x);
    }

    return identified;
}

static void scrollWordOfRecall() {
    if (py.flags.word_recall == 0) {
        py.flags.word_recall = (int16_t) (25 + randomNumber(30));
    }
    printMessage("The air about you becomes charged.");
}

// Scrolls for the reading -RAK-
void readScroll() {
    player_free_turn = true;

    int item_pos_start, item_pos_end;
    if (!playerCanReadScroll(&item_pos_start, &item_pos_end)) {
        return;
    }

    int item_id;
    if (!inventoryGetInputForItemId(&item_id, "Read which scroll?", item_pos_start, item_pos_end, CNIL, CNIL)) {
        return;
    }

    // From here on, no free turn for the player
    player_free_turn = false;

    bool used_up = true;
    bool identified = false;

    Inventory_t *item = &inventory[item_id];
    uint32_t item_flags = item->flags;

    while (item_flags != 0) {
        int scroll_type = getAndClearFirstBit(&item_flags) + 1;

        if (item->tval == TV_SCROLL2) {
            scroll_type += 32;
        }

        switch (scroll_type) {
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
                item_id = scrollIdentifyItem(item_id, &used_up);
                identified = true;
                break;
            case 5:
                identified = scrollRemoveCurse();
                break;
            case 6:
                identified = spellLightArea(char_row, char_col);
                break;
            case 7:
                identified = scrollSummonMonster();
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
                scrollTeleportLevel();
                identified = true;
                break;
            case 11:
                identified = scrollConfuseMonster();
                break;
            case 12:
                spellMapCurrentArea();
                identified = true;
                break;
            case 13:
                identified = monsterSleep(char_row, char_col);
                break;
            case 14:
                warding_glyph();
                identified = true;
                break;
            case 15:
                identified = dungeonDetectTreasureOnPanel();
                break;
            case 16:
                identified = dungeonDetectObjectOnPanel();
                break;
            case 17:
                identified = dungeonDetectTrapOnPanel();
                break;
            case 18:
                identified = dungeonDetectSecretDoorsOnPanel();
                break;
            case 19:
                printMessage("This is a mass genocide scroll.");
                (void) spellMassGenocide();
                identified = true;
                break;
            case 20:
                identified = spellDetectInvisibleCreaturesOnPanel();
                break;
            case 21:
                printMessage("There is a high pitched humming noise.");
                (void) spellAggravateMonsters(20);
                identified = true;
                break;
            case 22:
                identified = spellSurroundPlayerWithTraps();
                break;
            case 23:
                identified = spellDestroyAdjacentDoorsTraps();
                break;
            case 24:
                identified = spellSurroundPlayerWithDoors();
                break;
            case 25:
                printMessage("This is a Recharge-Item scroll.");
                used_up = spellRechargeItem(60);
                identified = true;
                break;
            case 26:
                printMessage("This is a genocide scroll.");
                (void) spellGenocide();
                identified = true;
                break;
            case 27:
                identified = spellDarkenArea(char_row, char_col);
                break;
            case 28:
                identified = playerProtectEvil();
                break;
            case 29:
                spellCreateFood();
                identified = true;
                break;
            case 30:
                identified = dispel_creature(CD_UNDEAD, 60);
                break;
            case 33:
                identified = scrollEnchantWeapon();
                break;
            case 34:
                identified = scrollCurseWeapon();
                break;
            case 35:
                identified = scrollEnchantArmor();
                break;
            case 36:
                identified = scrollCurseArmor();
                break;
            case 37:
                identified = scrollSummonUndead();
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
                scrollWordOfRecall();
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

    item = &inventory[item_id];

    if (identified) {
        if (!itemSetColorlessAsIdentifed(item)) {
            // round half-way case up
            py.misc.exp += (item->level + (py.misc.lev >> 1)) / py.misc.lev;
            displayCharacterExperience();

            itemIdentify(&item_id);
        }
    } else if (!itemSetColorlessAsIdentifed(item)) {
        itemSetAsTried(item);
    }

    if (used_up) {
        itemTypeRemainingCountDescription(item_id);
        inventoryDestroyItem(item_id);
    }
}
