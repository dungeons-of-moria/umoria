// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Staff code

#include "headers.h"
#include "externs.h"

static bool isCarryingStaff(int *j, int *k) {
    if (inventory_count == 0) {
        printMessage("But you are not carrying anything.");
        return false;
    }

    if (!inventoryFindRange(TV_STAFF, TV_NEVER, j, k)) {
        printMessage("You are not carrying any staffs.");
        return false;
    }

    return true;
}

static bool canUseStaff(Inventory_t *staff_ptr) {
    int chance = py.misc.save + playerStatAdjustmentWisdomIntelligence(A_INT) - (int) staff_ptr->level - 5 + (class_level_adj[py.misc.pclass][CLA_DEVICE] * py.misc.lev / 3);

    if (py.flags.confused > 0) {
        chance = chance / 2;
    }

    // Give everyone a slight chance
    if (chance < USE_DEVICE && randomNumber(USE_DEVICE - chance + 1) == 1) {
        chance = USE_DEVICE;
    }

    if (chance < 1) {
        chance = 1;
    }

    if (randomNumber(chance) < USE_DEVICE) {
        printMessage("You failed to use the staff properly.");
        return false;
    }

    if (staff_ptr->p1 < 1) {
        printMessage("The staff has no charges left.");
        if (!spellItemIdentified(staff_ptr)) {
            itemAppendToInscription(staff_ptr, ID_EMPTY);
        }
        return false;
    }

    return true;
}

static bool dischargeStaff(Inventory_t *staff_ptr) {
    bool identified = false;

    staff_ptr->p1--;

    uint32_t flags = staff_ptr->flags;
    while (flags != 0) {
        int staff_type = getAndClearFirstBit(&flags) + 1;

        switch (staff_type) {
            case 1:
                identified = spellLightArea(char_row, char_col);
                break;
            case 2:
                identified = dungeonDetectSecretDoorsOnPanel();
                break;
            case 3:
                identified = dungeonDetectTrapOnPanel();
                break;
            case 4:
                identified = dungeonDetectTreasureOnPanel();
                break;
            case 5:
                identified = dungeonDetectObjectOnPanel();
                break;
            case 6:
                playerTeleport(100);
                identified = true;
                break;
            case 7:
                identified = true;
                earthquake();
                break;
            case 8:
                identified = false;

                for (int k = 0; k < randomNumber(4); k++) {
                    int y = char_row;
                    int x = char_col;
                    identified |= monsterSummon(&y, &x, false);
                }
                break;
            case 10:
                identified = true;
                destroy_area(char_row, char_col);
                break;
            case 11:
                identified = true;
                spellStarlite(char_row, char_col);
                break;
            case 12:
                identified = spellSpeedAllMonsters(1);
                break;
            case 13:
                identified = spellSpeedAllMonsters(-1);
                break;
            case 14:
                identified = spellSleepAllMonsters();
                break;
            case 15:
                identified = spellChangePlayerHitPoints(randomNumber(8));
                break;
            case 16:
                identified = spellDetectInvisibleCreaturesOnPanel();
                break;
            case 17:
                if (py.flags.fast == 0) {
                    identified = true;
                }
                py.flags.fast += randomNumber(30) + 15;
                break;
            case 18:
                if (py.flags.slow == 0) {
                    identified = true;
                }
                py.flags.slow += randomNumber(30) + 15;
                break;
            case 19:
                identified = spellMassPolymorph();
                break;
            case 20:
                if (remove_curse()) {
                    if (py.flags.blind < 1) {
                        printMessage("The staff glows blue for a moment..");
                    }
                    identified = true;
                }
                break;
            case 21:
                identified = spellDetectEvil();
                break;
            case 22:
                if (playerCureBlindness() || cure_poison() || playerCureConfusion()) {
                    identified = true;
                }
                break;
            case 23:
                identified = dispel_creature(CD_EVIL, 60);
                break;
            case 25:
                identified = spellDarkenArea(char_row, char_col);
                break;
            case 32:
                // store bought flag
                break;
            default:
                printMessage("Internal error in staffs()");
                break;
        }
    }

    return identified;
}

// Use a staff. -RAK-
void use() {
    int j, k;

    player_free_turn = true;

    if (!isCarryingStaff(&j, &k)) {
        return;
    }

    int staff_id;
    if (!inventoryGetInputForItemId(&staff_id, "Use which staff?", j, k, CNIL, CNIL)) {
        return;
    }

    // From here on player uses up a turn
    player_free_turn = false;

    Inventory_t *staff_ptr = &inventory[staff_id];

    if (!canUseStaff(staff_ptr)) {
        return;
    }

    bool identified = dischargeStaff(staff_ptr);

    if (identified) {
        if (!itemSetColorlessAsIdentifed(staff_ptr)) {
            // round half-way case up
            py.misc.exp += (staff_ptr->level + (py.misc.lev >> 1)) / py.misc.lev;

            displayCharacterExperience();

            itemIdentify(&staff_id);
        }
    } else if (!itemSetColorlessAsIdentifed(staff_ptr)) {
        itemSetAsTried(staff_ptr);
    }

    itemChargesRemainingDescription(staff_id);
}
