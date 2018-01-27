// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

#include "headers.h"

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
    if (py.unique_inventory_items == 0) {
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
    chance += playerStatAdjustmentWisdomIntelligence(py_attrs::A_INT);
    chance -= item.depth_first_found - 5;
    chance += class_level_adj[py.misc.class_id][py_class_level_adj::CLASS_DEVICE] * py.misc.level / 3;

    if (py.flags.confused > 0) {
        chance = chance / 2;
    }

    // Give everyone a slight chance
    if (chance < config::player::PLAYER_USE_DEVICE_DIFFICULTY && randomNumber(config::player::PLAYER_USE_DEVICE_DIFFICULTY - chance + 1) == 1) {
        chance = config::player::PLAYER_USE_DEVICE_DIFFICULTY;
    }

    if (chance < 1) {
        chance = 1;
    }

    if (randomNumber(chance) < config::player::PLAYER_USE_DEVICE_DIFFICULTY) {
        printMessage("You failed to use the staff properly.");
        return false;
    }

    if (item.misc_use < 1) {
        printMessage("The staff has no charges left.");
        if (!spellItemIdentified(item)) {
            itemAppendToInscription(item, config::identification::ID_EMPTY);
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
                identified = spellDetectSecretDoorssWithinVicinity();
                break;
            case StaffSpellTypes::trap_location:
                identified = spellDetectTrapsWithinVicinity();
                break;
            case StaffSpellTypes::treasure_location:
                identified = spellDetectTreasureWithinVicinity();
                break;
            case StaffSpellTypes::object_location:
                identified = spellDetectObjectsWithinVicinity();
                break;
            case StaffSpellTypes::teleportation:
                playerTeleport(100);
                identified = true;
                break;
            case StaffSpellTypes::earthquakes:
                identified = true;
                spellEarthquake();
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
                identified = spellDetectInvisibleCreaturesWithinVicinity();
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
void staffUse() {
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

enum class WandSpellTypes {
    light = 1,
    lightning_bolt,
    frost_bolt,
    fire_bolt,
    stone_to_mud,
    polymorph,
    heal_monster,
    haste_monster,
    slow_monster,
    confuse_monster,
    sleep_monster,
    drain_life,
    trap_door_destruction,
    magic_missile,
    wall_building,
    clone_monster,
    teleport_away,
    disarming,
    lightning_ball,
    cold_ball,
    fire_ball,
    stinking_cloud,
    acid_ball,
    wonder,
};

static bool wandDischarge(Inventory_t &item, int direction) {
    // decrement "use" variable
    item.misc_use--;

    bool identified = false;
    uint32_t flags = item.flags;

    while (flags != 0) {
        int y = py.row;
        int x = py.col;

        // Wand types
        switch ((WandSpellTypes) (getAndClearFirstBit(flags) + 1)) {
            case WandSpellTypes::light:
                printMessage("A line of blue shimmering light appears.");
                spellLightLine(py.col, py.row, direction);
                identified = true;
                break;
            case WandSpellTypes::lightning_bolt:
                spellFireBolt(y, x, direction, diceRoll(Dice_t{4, 8}), magic_spell_flags::GF_LIGHTNING, spell_names[8]);
                identified = true;
                break;
            case WandSpellTypes::frost_bolt:
                spellFireBolt(y, x, direction, diceRoll(Dice_t{6, 8}), magic_spell_flags::GF_FROST, spell_names[14]);
                identified = true;
                break;
            case WandSpellTypes::fire_bolt:
                spellFireBolt(y, x, direction, diceRoll(Dice_t{9, 8}), magic_spell_flags::GF_FIRE, spell_names[22]);
                identified = true;
                break;
            case WandSpellTypes::stone_to_mud:
                identified = spellWallToMud(y, x, direction);
                break;
            case WandSpellTypes::polymorph:
                identified = spellPolymorphMonster(y, x, direction);
                break;
            case WandSpellTypes::heal_monster:
                identified = spellChangeMonsterHitPoints(y, x, direction, -diceRoll(Dice_t{4, 6}));
                break;
            case WandSpellTypes::haste_monster:
                identified = spellSpeedMonster(y, x, direction, 1);
                break;
            case WandSpellTypes::slow_monster:
                identified = spellSpeedMonster(y, x, direction, -1);
                break;
            case WandSpellTypes::confuse_monster:
                identified = spellConfuseMonster(y, x, direction);
                break;
            case WandSpellTypes::sleep_monster:
                identified = spellSleepMonster(y, x, direction);
                break;
            case WandSpellTypes::drain_life:
                identified = spellDrainLifeFromMonster(y, x, direction);
                break;
            case WandSpellTypes::trap_door_destruction:
                identified = spellDestroyDoorsTrapsInDirection(y, x, direction);
                break;
            case WandSpellTypes::magic_missile:
                spellFireBolt(y, x, direction, diceRoll(Dice_t{2, 6}), magic_spell_flags::GF_MAGIC_MISSILE, spell_names[0]);
                identified = true;
                break;
            case WandSpellTypes::wall_building:
                identified = spellBuildWall(y, x, direction);
                break;
            case WandSpellTypes::clone_monster:
                identified = spellCloneMonster(y, x, direction);
                break;
            case WandSpellTypes::teleport_away:
                identified = spellTeleportAwayMonsterInDirection(y, x, direction);
                break;
            case WandSpellTypes::disarming:
                identified = spellDisarmAllInDirection(y, x, direction);
                break;
            case WandSpellTypes::lightning_ball:
                spellFireBall(y, x, direction, 32, magic_spell_flags::GF_LIGHTNING, "Lightning Ball");
                identified = true;
                break;
            case WandSpellTypes::cold_ball:
                spellFireBall(y, x, direction, 48, magic_spell_flags::GF_FROST, "Cold Ball");
                identified = true;
                break;
            case WandSpellTypes::fire_ball:
                spellFireBall(y, x, direction, 72, magic_spell_flags::GF_FIRE, spell_names[28]);
                identified = true;
                break;
            case WandSpellTypes::stinking_cloud:
                spellFireBall(y, x, direction, 12, magic_spell_flags::GF_POISON_GAS, spell_names[6]);
                identified = true;
                break;
            case WandSpellTypes::acid_ball:
                spellFireBall(y, x, direction, 60, magic_spell_flags::GF_ACID, "Acid Ball");
                identified = true;
                break;
            case WandSpellTypes::wonder:
                flags = (uint32_t) (1L << (randomNumber(23) - 1));
                break;
            default:
                printMessage("Internal error in wands()");
                break;
        }
    }

    return identified;
}

// Wands for the aiming.
void wandAim() {
    game.player_free_turn = true;

    if (py.unique_inventory_items == 0) {
        printMessage("But you are not carrying anything.");
        return;
    }

    int item_pos_start, item_pos_end;
    if (!inventoryFindRange(TV_WAND, TV_NEVER, item_pos_start, item_pos_end)) {
        printMessage("You are not carrying any wands.");
        return;
    }

    int item_id;
    if (!inventoryGetInputForItemId(item_id, "Aim which wand?", item_pos_start, item_pos_end, CNIL, CNIL)) {
        return;
    }

    game.player_free_turn = false;

    int direction;
    if (!getDirectionWithMemory(CNIL, direction)) {
        return;
    }

    if (py.flags.confused > 0) {
        printMessage("You are confused.");
        direction = getRandomDirection();
    }

    Inventory_t &item = inventory[item_id];

    int player_class_lev_adj = class_level_adj[py.misc.class_id][py_class_level_adj::CLASS_DEVICE] * py.misc.level / 3;
    int chance = py.misc.saving_throw + playerStatAdjustmentWisdomIntelligence(py_attrs::A_INT) - (int) item.depth_first_found + player_class_lev_adj;

    if (py.flags.confused > 0) {
        chance = chance / 2;
    }

    if (chance < config::player::PLAYER_USE_DEVICE_DIFFICULTY && randomNumber(config::player::PLAYER_USE_DEVICE_DIFFICULTY - chance + 1) == 1) {
        chance = config::player::PLAYER_USE_DEVICE_DIFFICULTY; // Give everyone a slight chance
    }

    if (chance <= 0) {
        chance = 1;
    }

    if (randomNumber(chance) < config::player::PLAYER_USE_DEVICE_DIFFICULTY) {
        printMessage("You failed to use the wand properly.");
        return;
    }

    if (item.misc_use < 1) {
        printMessage("The wand has no charges left.");
        if (!spellItemIdentified(item)) {
            itemAppendToInscription(item, config::identification::ID_EMPTY);
        }
        return;
    }

    bool identified = wandDischarge(item, direction);

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
