// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// Player throw functions

#include "headers.h"

static void inventoryThrow(int item_id, Inventory_t *treasure) {
    Inventory_t *item = &py.inventory[item_id];

    *treasure = *item;

    if (item->items_count > 1) {
        treasure->items_count = 1;
        item->items_count--;
        py.pack.weight -= item->weight;
        py.flags.status |= config::player::status::PY_STR_WGT;
    } else {
        inventoryDestroyItem(item_id);
    }
}

// Obtain the hit and damage bonuses and the maximum distance for a thrown missile.
static void weaponMissileFacts(Inventory_t &item, int &base_to_hit, int &plus_to_hit, int &damage, int &distance) {
    int weight = item.weight;
    if (weight < 1) {
        weight = 1;
    }

    // Throwing objects
    damage = diceRoll(item.damage) + item.to_damage;
    base_to_hit = py.misc.bth_with_bows * 75 / 100;
    plus_to_hit = py.misc.plusses_to_hit + item.to_hit;

    // Add this back later if the correct throwing device. -CJS-
    if (py.inventory[PlayerEquipment::Wield].category_id != TV_NOTHING) {
        plus_to_hit -= py.inventory[PlayerEquipment::Wield].to_hit;
    }

    distance = ((py.stats.used[PlayerAttr::A_STR] + 20) * 10) / weight;
    if (distance > 10) {
        distance = 10;
    }

    // multiply damage bonuses instead of adding, when have proper
    // missile/weapon combo, this makes them much more useful

    // Using Bows, slings, or crossbows?
    if (py.inventory[PlayerEquipment::Wield].category_id != TV_BOW) {
        return;
    }

    switch (py.inventory[PlayerEquipment::Wield].misc_use) {
        case 1:
            if (item.category_id == TV_SLING_AMMO) { // Sling and ammo
                base_to_hit = py.misc.bth_with_bows;
                plus_to_hit += 2 * py.inventory[PlayerEquipment::Wield].to_hit;
                damage += py.inventory[PlayerEquipment::Wield].to_damage;
                damage = damage * 2;
                distance = 20;
            }
            break;
        case 2:
            if (item.category_id == TV_ARROW) { // Short Bow and Arrow
                base_to_hit = py.misc.bth_with_bows;
                plus_to_hit += 2 * py.inventory[PlayerEquipment::Wield].to_hit;
                damage += py.inventory[PlayerEquipment::Wield].to_damage;
                damage = damage * 2;
                distance = 25;
            }
            break;
        case 3:
            if (item.category_id == TV_ARROW) { // Long Bow and Arrow
                base_to_hit = py.misc.bth_with_bows;
                plus_to_hit += 2 * py.inventory[PlayerEquipment::Wield].to_hit;
                damage += py.inventory[PlayerEquipment::Wield].to_damage;
                damage = damage * 3;
                distance = 30;
            }
            break;
        case 4:
            if (item.category_id == TV_ARROW) { // Composite Bow and Arrow
                base_to_hit = py.misc.bth_with_bows;
                plus_to_hit += 2 * py.inventory[PlayerEquipment::Wield].to_hit;
                damage += py.inventory[PlayerEquipment::Wield].to_damage;
                damage = damage * 4;
                distance = 35;
            }
            break;
        case 5:
            if (item.category_id == TV_BOLT) { // Light Crossbow and Bolt
                base_to_hit = py.misc.bth_with_bows;
                plus_to_hit += 2 * py.inventory[PlayerEquipment::Wield].to_hit;
                damage += py.inventory[PlayerEquipment::Wield].to_damage;
                damage = damage * 3;
                distance = 25;
            }
            break;
        case 6:
            if (item.category_id == TV_BOLT) { // Heavy Crossbow and Bolt
                base_to_hit = py.misc.bth_with_bows;
                plus_to_hit += 2 * py.inventory[PlayerEquipment::Wield].to_hit;
                damage += py.inventory[PlayerEquipment::Wield].to_damage;
                damage = damage * 4;
                distance = 35;
            }
            break;
        default:
            // NOOP
            break;
    }
}

static void inventoryDropOrThrowItem(Coord_t coord, Inventory_t *item) {
    Coord_t position = Coord_t{coord.y, coord.x};

    bool flag = false;

    if (randomNumber(10) > 1) {
        for (int k = 0; !flag && k <= 9;) {
            if (coordInBounds(position)) {
                if (dg.floor[position.y][position.x].feature_id <= MAX_OPEN_SPACE && dg.floor[position.y][position.x].treasure_id == 0) {
                    flag = true;
                }
            }

            if (!flag) {
                position.y = coord.y + randomNumber(3) - 2;
                position.x = coord.x + randomNumber(3) - 2;
                k++;
            }
        }
    }

    if (flag) {
        int cur_pos = popt();
        dg.floor[position.y][position.x].treasure_id = (uint8_t) cur_pos;
        game.treasure.list[cur_pos] = *item;
        dungeonLiteSpot(position);
    } else {
        obj_desc_t description = {'\0'};
        obj_desc_t msg = {'\0'};
        itemDescription(description, *item, false);

        (void) sprintf(msg, "The %s disappears.", description);
        printMessage(msg);
    }
}

// Throw an object across the dungeon. -RAK-
// Note: Flasks of oil do fire damage
// Note: Extra damage and chance of hitting when missiles are used
// with correct weapon. i.e. wield bow and throw arrow.
void playerThrowItem() {
    if (py.pack.unique_items == 0) {
        printMessage("But you are not carrying anything.");
        game.player_free_turn = true;
        return;
    }

    int item_id;
    if (!inventoryGetInputForItemId(item_id, "Fire/Throw which one?", 0, py.pack.unique_items - 1, CNIL, CNIL)) {
        return;
    }

    int dir;
    if (!getDirectionWithMemory(CNIL, dir)) {
        return;
    }

    itemTypeRemainingCountDescription(item_id);

    if (py.flags.confused > 0) {
        printMessage("You are confused.");
        dir = getRandomDirection();
    }

    Inventory_t thrown_item{};
    inventoryThrow(item_id, &thrown_item);

    int tbth, tpth, tdam, tdis;
    weaponMissileFacts(thrown_item, tbth, tpth, tdam, tdis);

    char tile_char = thrown_item.sprite;
    bool visible;
    int current_distance = 0;

    Coord_t coord = py.pos;
    Coord_t old_coord = py.pos;

    bool flag = false;

    while (!flag) {
        (void) playerMovePosition(dir, coord);
        current_distance++;
        dungeonLiteSpot(old_coord);

        if (current_distance > tdis) {
            flag = true;
        }

        Tile_t const &tile = dg.floor[coord.y][coord.x];

        if (tile.feature_id <= MAX_OPEN_SPACE && !flag) {
            if (tile.creature_id > 1) {
                flag = true;

                Monster_t const &m_ptr = monsters[tile.creature_id];

                tbth -= current_distance;

                // if monster not lit, make it much more difficult to hit, subtract
                // off most bonuses, and reduce bth_with_bows depending on distance.
                if (!m_ptr.lit) {
                    tbth /= current_distance + 2;
                    tbth -= py.misc.level * class_level_adj[py.misc.class_id][PlayerClassLevelAdj::BTHB] / 2;
                    tbth -= tpth * (BTH_PER_PLUS_TO_HIT_ADJUST - 1);
                }

                if (playerTestBeingHit(tbth, (int) py.misc.level, tpth, (int) creatures_list[m_ptr.creature_id].ac, PlayerClassLevelAdj::BTHB)) {
                    int damage = m_ptr.creature_id;

                    obj_desc_t description = {'\0'};
                    obj_desc_t msg = {'\0'};
                    itemDescription(description, thrown_item, false);

                    // Does the player know what they're fighting?
                    if (!m_ptr.lit) {
                        (void) sprintf(msg, "You hear a cry as the %s finds a mark.", description);
                        visible = false;
                    } else {
                        (void) sprintf(msg, "The %s hits the %s.", description, creatures_list[damage].name);
                        visible = true;
                    }
                    printMessage(msg);

                    tdam = itemMagicAbilityDamage(thrown_item, tdam, damage);
                    tdam = playerWeaponCriticalBlow((int) thrown_item.weight, tpth, tdam, PlayerClassLevelAdj::BTHB);

                    if (tdam < 0) {
                        tdam = 0;
                    }

                    damage = monsterTakeHit((int) tile.creature_id, tdam);

                    if (damage >= 0) {
                        if (!visible) {
                            printMessage("You have killed something!");
                        } else {
                            (void) sprintf(msg, "You have killed the %s.", creatures_list[damage].name);
                            printMessage(msg);
                        }
                        displayCharacterExperience();
                    }
                } else {
                    inventoryDropOrThrowItem(old_coord, &thrown_item);
                }
            } else {
                // do not test tile.field_mark here

                if (coordInsidePanel(coord) && py.flags.blind < 1 && (tile.temporary_light || tile.permanent_light)) {
                    panelPutTile(tile_char, coord);
                    putQIO(); // show object moving
                }
            }
        } else {
            flag = true;
            inventoryDropOrThrowItem(old_coord, &thrown_item);
        }

        old_coord.y = coord.y;
        old_coord.x = coord.x;
    }
}
