// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Staff code

#include "headers.h"
#include "externs.h"

enum class StaffSpellTypes {
    light = 1,
    detect_doors_stairs,
    trap_location,
    treasure_location,
    object_location,
    teleportation,
    earthquakes,
    summoning,
    // skipping 9
    destruction = 10,
    starlight,
    haste_monsters,
    slow_monsters,
    sleep_monsters,
    cure_light_wounds,
    detect_invisible,
    speed,
    slowness,
    mass_polymorph,
    remove_curse,
    detect_evil,
    curing,
    dispel_evil,
    // skipping 24
    darkness = 25,
    // skipping lots...
    store_bought_flag = 32,
};

static bool staffPlayerIsCarrying(int &item_pos_start, int &item_pos_end) {
    if (inventory_count == 0) {
        printMessage("But you are not carrying anything.");
        return false;
    }

    if (!inventoryFindRange(TV_STAFF, TV_NEVER, item_pos_start, item_pos_end)) {
        printMessage("You are not carrying any staffs.");
        return false;
    }

    return true;
}

static bool staffPlayerCanUse(Inventory_t &item) {
    int chance = py.misc.saving_throw;
    chance += playerStatAdjustmentWisdomIntelligence(A_INT);
    chance -= item.depth_first_found - 5;
    chance += class_level_adj[py.misc.class_id][CLASS_DEVICE] * py.misc.level / 3;

    if (py.flags.confused > 0) {
        chance = chance / 2;
    }

    // Give everyone a slight chance
    if (chance < PLAYER_USE_DEVICE_DIFFICULTY && randomNumber(PLAYER_USE_DEVICE_DIFFICULTY - chance + 1) == 1) {
        chance = PLAYER_USE_DEVICE_DIFFICULTY;
    }

    if (chance < 1) {
        chance = 1;
    }

    if (randomNumber(chance) < PLAYER_USE_DEVICE_DIFFICULTY) {
        printMessage("You failed to use the staff properly.");
        return false;
    }

    if (item.misc_use < 1) {
        printMessage("The staff has no charges left.");
        if (!spellItemIdentified(item)) {
            itemAppendToInscription(item, ID_EMPTY);
        }
        return false;
    }

    return true;
}

static bool staffDischarge(Inventory_t &item) {
    bool identified = false;

    item.misc_use--;

    uint32_t flags = item.flags;
    while (flags != 0) {
        switch ((StaffSpellTypes) (getAndClearFirstBit(flags) + 1)) {
            case StaffSpellTypes::light:
                identified = spellLightArea(py.row, py.col);
                break;
            case StaffSpellTypes::detect_doors_stairs:
                identified = dungeonDetectSecretDoorsOnPanel();
                break;
            case StaffSpellTypes::trap_location:
                identified = dungeonDetectTrapOnPanel();
                break;
            case StaffSpellTypes::treasure_location:
                identified = dungeonDetectTreasureOnPanel();
                break;
            case StaffSpellTypes::object_location:
                identified = dungeonDetectObjectOnPanel();
                break;
            case StaffSpellTypes::teleportation:
                playerTeleport(100);
                identified = true;
                break;
            case StaffSpellTypes::earthquakes:
                identified = true;
                dungeonEarthquake();
                break;
            case StaffSpellTypes::summoning:
                identified = false;

                for (int i = 0; i < randomNumber(4); i++) {
                    int y = py.row;
                    int x = py.col;
                    identified |= monsterSummon(y, x, false);
                }
                break;
            case StaffSpellTypes::destruction:
                identified = true;
                spellDestroyArea(py.row, py.col);
                break;
            case StaffSpellTypes::starlight:
                identified = true;
                spellStarlite(py.row, py.col);
                break;
            case StaffSpellTypes::haste_monsters:
                identified = spellSpeedAllMonsters(1);
                break;
            case StaffSpellTypes::slow_monsters:
                identified = spellSpeedAllMonsters(-1);
                break;
            case StaffSpellTypes::sleep_monsters:
                identified = spellSleepAllMonsters();
                break;
            case StaffSpellTypes::cure_light_wounds:
                identified = spellChangePlayerHitPoints(randomNumber(8));
                break;
            case StaffSpellTypes::detect_invisible:
                identified = spellDetectInvisibleCreaturesOnPanel();
                break;
            case StaffSpellTypes::speed:
                if (py.flags.fast == 0) {
                    identified = true;
                }
                py.flags.fast += randomNumber(30) + 15;
                break;
            case StaffSpellTypes::slowness:
                if (py.flags.slow == 0) {
                    identified = true;
                }
                py.flags.slow += randomNumber(30) + 15;
                break;
            case StaffSpellTypes::mass_polymorph:
                identified = spellMassPolymorph();
                break;
            case StaffSpellTypes::remove_curse:
                if (spellRemoveCurseFromAllItems()) {
                    if (py.flags.blind < 1) {
                        printMessage("The staff glows blue for a moment..");
                    }
                    identified = true;
                }
                break;
            case StaffSpellTypes::detect_evil:
                identified = spellDetectEvil();
                break;
            case StaffSpellTypes::curing:
                if (playerCureBlindness() || playerCurePoison() || playerCureConfusion()) {
                    identified = true;
                }
                break;
            case StaffSpellTypes::dispel_evil:
                identified = spellDispelCreature(CD_EVIL, 60);
                break;
            case StaffSpellTypes::darkness:
                identified = spellDarkenArea(py.row, py.col);
                break;
            case StaffSpellTypes::store_bought_flag:
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
void useStaff() {
    game.player_free_turn = true;

    int item_pos_start, item_pos_end;
    if (!staffPlayerIsCarrying(item_pos_start, item_pos_end)) {
        return;
    }

    int item_id;
    if (!inventoryGetInputForItemId(item_id, "Use which staff?", item_pos_start, item_pos_end, CNIL, CNIL)) {
        return;
    }

    // From here on player uses up a turn
    game.player_free_turn = false;

    Inventory_t &item = inventory[item_id];

    if (!staffPlayerCanUse(item)) {
        return;
    }

    bool identified = staffDischarge(item);

    if (identified) {
        if (!itemSetColorlessAsIdentified(item.category_id, item.sub_category_id, item.identification)) {
            // round half-way case up
            py.misc.exp += (item.depth_first_found + (py.misc.level >> 1)) / py.misc.level;

            displayCharacterExperience();

            itemIdentify(inventory[item_id], item_id);
        }
    } else if (!itemSetColorlessAsIdentified(item.category_id, item.sub_category_id, item.identification)) {
        itemSetAsTried(item);
    }

    itemChargesRemainingDescription(item_id);
}
