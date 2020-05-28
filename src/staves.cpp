// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

#include "headers.h"

enum class StaffSpellTypes {
    StaffLight = 1, // TODO: name would clash with PlayerEquipment::Light
    DetectDoorsStairs,
    TrapLocation,
    TreasureLocation,
    ObjectLocation,
    Teleportation,
    Earthquakes,
    Summoning,
    // skipping 9
    Destruction = 10,
    Starlight,
    HasteMonsters,
    SlowMonsters,
    SleepMonsters,
    CureLightWounds,
    DetectInvisible,
    Speed,
    Slowness,
    MassPolymorph,
    RemoveCurse,
    DetectEvil,
    Curing,
    DispelEvil,
    // skipping 24
    Darkness = 25,
    // skipping lots...
    StoreBoughtFlag = 32,
};

static bool staffPlayerIsCarrying(int &item_pos_start, int &item_pos_end) {
    if (py.pack.unique_items == 0) {
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
    chance += playerStatAdjustmentWisdomIntelligence(PlayerAttr::A_INT);
    chance -= item.depth_first_found - 5;
    chance += class_level_adj[py.misc.class_id][PlayerClassLevelAdj::DEVICE] * py.misc.level / 3;

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
        switch ((StaffSpellTypes)(getAndClearFirstBit(flags) + 1)) {
            case StaffSpellTypes::StaffLight:
                identified = spellLightArea(py.pos);
                break;
            case StaffSpellTypes::DetectDoorsStairs:
                identified = spellDetectSecretDoorssWithinVicinity();
                break;
            case StaffSpellTypes::TrapLocation:
                identified = spellDetectTrapsWithinVicinity();
                break;
            case StaffSpellTypes::TreasureLocation:
                identified = spellDetectTreasureWithinVicinity();
                break;
            case StaffSpellTypes::ObjectLocation:
                identified = spellDetectObjectsWithinVicinity();
                break;
            case StaffSpellTypes::Teleportation:
                playerTeleport(100);
                identified = true;
                break;
            case StaffSpellTypes::Earthquakes:
                identified = true;
                spellEarthquake();
                break;
            case StaffSpellTypes::Summoning:
                identified = false;

                for (int i = 0; i < randomNumber(4); i++) {
                    Coord_t coord = py.pos;
                    identified |= monsterSummon(coord, false);
                }
                break;
            case StaffSpellTypes::Destruction:
                identified = true;
                spellDestroyArea(py.pos);
                break;
            case StaffSpellTypes::Starlight:
                identified = true;
                spellStarlite(py.pos);
                break;
            case StaffSpellTypes::HasteMonsters:
                identified = spellSpeedAllMonsters(1);
                break;
            case StaffSpellTypes::SlowMonsters:
                identified = spellSpeedAllMonsters(-1);
                break;
            case StaffSpellTypes::SleepMonsters:
                identified = spellSleepAllMonsters();
                break;
            case StaffSpellTypes::CureLightWounds:
                identified = spellChangePlayerHitPoints(randomNumber(8));
                break;
            case StaffSpellTypes::DetectInvisible:
                identified = spellDetectInvisibleCreaturesWithinVicinity();
                break;
            case StaffSpellTypes::Speed:
                if (py.flags.fast == 0) {
                    identified = true;
                }
                py.flags.fast += randomNumber(30) + 15;
                break;
            case StaffSpellTypes::Slowness:
                if (py.flags.slow == 0) {
                    identified = true;
                }
                py.flags.slow += randomNumber(30) + 15;
                break;
            case StaffSpellTypes::MassPolymorph:
                identified = spellMassPolymorph();
                break;
            case StaffSpellTypes::RemoveCurse:
                if (spellRemoveCurseFromAllItems()) {
                    if (py.flags.blind < 1) {
                        printMessage("The staff glows blue for a moment..");
                    }
                    identified = true;
                }
                break;
            case StaffSpellTypes::DetectEvil:
                identified = spellDetectEvil();
                break;
            case StaffSpellTypes::Curing:
                if (playerCureBlindness() || playerCurePoison() || playerCureConfusion()) {
                    identified = true;
                }
                break;
            case StaffSpellTypes::DispelEvil:
                identified = spellDispelCreature(config::monsters::defense::CD_EVIL, 60);
                break;
            case StaffSpellTypes::Darkness:
                identified = spellDarkenArea(py.pos);
                break;
            case StaffSpellTypes::StoreBoughtFlag:
                // store bought flag
                break;
            default:
                // All cases are handled, so this should never be reached!
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

    Inventory_t &item = py.inventory[item_id];

    if (!staffPlayerCanUse(item)) {
        return;
    }

    bool identified = staffDischarge(item);

    if (identified) {
        if (!itemSetColorlessAsIdentified(item.category_id, item.sub_category_id, item.identification)) {
            // round half-way case up
            py.misc.exp += (item.depth_first_found + (py.misc.level >> 1)) / py.misc.level;

            displayCharacterExperience();

            itemIdentify(py.inventory[item_id], item_id);
        }
    } else if (!itemSetColorlessAsIdentified(item.category_id, item.sub_category_id, item.identification)) {
        itemSetAsTried(item);
    }

    itemChargesRemainingDescription(item_id);
}

enum class WandSpellTypes {
    WandLight = 1, // TODO: name would clash with PlayerEquipment::Light
    LightningBolt,
    FrostBolt,
    FireBolt,
    StoneToMud,
    Polymorph,
    HealMonster,
    HasteMonster,
    SlowMonster,
    ConfuseMonster,
    SleepMonster,
    DrainLife,
    TrapDoorDestruction,
    WandMagicMissile, // TODO: name would clash with MagicSpellFlags::MagicMissile
    WallBuilding,
    CloneMonster,
    TeleportAway,
    Disarming,
    LightningBall,
    ColdBall,
    FireBall,
    StinkingCloud,
    AcidBall,
    Wonder,
};

static bool wandDischarge(Inventory_t &item, int direction) {
    // decrement "use" variable
    item.misc_use--;

    bool identified = false;
    uint32_t flags = item.flags;

    Coord_t coord = Coord_t{0, 0};

    while (flags != 0) {
        coord.y = py.pos.y;
        coord.x = py.pos.x;

        // Wand types
        switch ((WandSpellTypes)(getAndClearFirstBit(flags) + 1)) {
            case WandSpellTypes::WandLight:
                printMessage("A line of blue shimmering light appears.");
                spellLightLine(py.pos, direction);
                identified = true;
                break;
            case WandSpellTypes::LightningBolt:
                spellFireBolt(coord, direction, diceRoll(Dice_t{4, 8}), MagicSpellFlags::Lightning, spell_names[8]);
                identified = true;
                break;
            case WandSpellTypes::FrostBolt:
                spellFireBolt(coord, direction, diceRoll(Dice_t{6, 8}), MagicSpellFlags::Frost, spell_names[14]);
                identified = true;
                break;
            case WandSpellTypes::FireBolt:
                spellFireBolt(coord, direction, diceRoll(Dice_t{9, 8}), MagicSpellFlags::Fire, spell_names[22]);
                identified = true;
                break;
            case WandSpellTypes::StoneToMud:
                identified = spellWallToMud(coord, direction);
                break;
            case WandSpellTypes::Polymorph:
                identified = spellPolymorphMonster(coord, direction);
                break;
            case WandSpellTypes::HealMonster:
                identified = spellChangeMonsterHitPoints(coord, direction, -diceRoll(Dice_t{4, 6}));
                break;
            case WandSpellTypes::HasteMonster:
                identified = spellSpeedMonster(coord, direction, 1);
                break;
            case WandSpellTypes::SlowMonster:
                identified = spellSpeedMonster(coord, direction, -1);
                break;
            case WandSpellTypes::ConfuseMonster:
                identified = spellConfuseMonster(coord, direction);
                break;
            case WandSpellTypes::SleepMonster:
                identified = spellSleepMonster(coord, direction);
                break;
            case WandSpellTypes::DrainLife:
                identified = spellDrainLifeFromMonster(coord, direction);
                break;
            case WandSpellTypes::TrapDoorDestruction:
                identified = spellDestroyDoorsTrapsInDirection(coord, direction);
                break;
            case WandSpellTypes::WandMagicMissile:
                spellFireBolt(coord, direction, diceRoll(Dice_t{2, 6}), MagicSpellFlags::MagicMissile, spell_names[0]);
                identified = true;
                break;
            case WandSpellTypes::WallBuilding:
                identified = spellBuildWall(coord, direction);
                break;
            case WandSpellTypes::CloneMonster:
                identified = spellCloneMonster(coord, direction);
                break;
            case WandSpellTypes::TeleportAway:
                identified = spellTeleportAwayMonsterInDirection(coord, direction);
                break;
            case WandSpellTypes::Disarming:
                identified = spellDisarmAllInDirection(coord, direction);
                break;
            case WandSpellTypes::LightningBall:
                spellFireBall(coord, direction, 32, MagicSpellFlags::Lightning, "Lightning Ball");
                identified = true;
                break;
            case WandSpellTypes::ColdBall:
                spellFireBall(coord, direction, 48, MagicSpellFlags::Frost, "Cold Ball");
                identified = true;
                break;
            case WandSpellTypes::FireBall:
                spellFireBall(coord, direction, 72, MagicSpellFlags::Fire, spell_names[28]);
                identified = true;
                break;
            case WandSpellTypes::StinkingCloud:
                spellFireBall(coord, direction, 12, MagicSpellFlags::PoisonGas, spell_names[6]);
                identified = true;
                break;
            case WandSpellTypes::AcidBall:
                spellFireBall(coord, direction, 60, MagicSpellFlags::Acid, "Acid Ball");
                identified = true;
                break;
            case WandSpellTypes::Wonder:
                flags = (uint32_t)(1L << (randomNumber(23) - 1));
                break;
            default:
                // All cases are handled, so this should never be reached!
                printMessage("Internal error in wands()");
                break;
        }
    }

    return identified;
}

// Wands for the aiming.
void wandAim() {
    game.player_free_turn = true;

    if (py.pack.unique_items == 0) {
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

    Inventory_t &item = py.inventory[item_id];

    int player_class_lev_adj = class_level_adj[py.misc.class_id][PlayerClassLevelAdj::DEVICE] * py.misc.level / 3;
    int chance = py.misc.saving_throw + playerStatAdjustmentWisdomIntelligence(PlayerAttr::A_INT) - (int) item.depth_first_found + player_class_lev_adj;

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

            itemIdentify(py.inventory[item_id], item_id);
        }
    } else if (!itemSetColorlessAsIdentified(item.category_id, item.sub_category_id, item.identification)) {
        itemSetAsTried(item);
    }

    itemChargesRemainingDescription(item_id);
}
