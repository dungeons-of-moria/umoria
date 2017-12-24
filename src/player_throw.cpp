// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Player throw functions

#include "headers.h"
#include "externs.h"

static void inventoryThrow(int item_id, Inventory_t *treasure) {
    Inventory_t *item = &inventory[item_id];

    *treasure = *item;

    if (item->items_count > 1) {
        treasure->items_count = 1;
        item->items_count--;
        py.inventory_weight -= item->weight;
        py.flags.status |= PY_STR_WGT;
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
    if (inventory[EQUIPMENT_WIELD].category_id != TV_NOTHING) {
        plus_to_hit -= inventory[EQUIPMENT_WIELD].to_hit;
    }

    distance = (((py.stats.used[A_STR] + 20) * 10) / weight);
    if (distance > 10) {
        distance = 10;
    }

    // multiply damage bonuses instead of adding, when have proper
    // missile/weapon combo, this makes them much more useful

    // Using Bows, slings, or crossbows?
    if (inventory[EQUIPMENT_WIELD].category_id != TV_BOW) {
        return;
    }

    switch (inventory[EQUIPMENT_WIELD].misc_use) {
        case 1:
            if (item.category_id == TV_SLING_AMMO) { // Sling and ammo
                base_to_hit = py.misc.bth_with_bows;
                plus_to_hit += 2 * inventory[EQUIPMENT_WIELD].to_hit;
                damage += inventory[EQUIPMENT_WIELD].to_damage;
                damage = damage * 2;
                distance = 20;
            }
            break;
        case 2:
            if (item.category_id == TV_ARROW) { // Short Bow and Arrow
                base_to_hit = py.misc.bth_with_bows;
                plus_to_hit += 2 * inventory[EQUIPMENT_WIELD].to_hit;
                damage += inventory[EQUIPMENT_WIELD].to_damage;
                damage = damage * 2;
                distance = 25;
            }
            break;
        case 3:
            if (item.category_id == TV_ARROW) { // Long Bow and Arrow
                base_to_hit = py.misc.bth_with_bows;
                plus_to_hit += 2 * inventory[EQUIPMENT_WIELD].to_hit;
                damage += inventory[EQUIPMENT_WIELD].to_damage;
                damage = damage * 3;
                distance = 30;
            }
            break;
        case 4:
            if (item.category_id == TV_ARROW) { // Composite Bow and Arrow
                base_to_hit = py.misc.bth_with_bows;
                plus_to_hit += 2 * inventory[EQUIPMENT_WIELD].to_hit;
                damage += inventory[EQUIPMENT_WIELD].to_damage;
                damage = damage * 4;
                distance = 35;
            }
            break;
        case 5:
            if (item.category_id == TV_BOLT) { // Light Crossbow and Bolt
                base_to_hit = py.misc.bth_with_bows;
                plus_to_hit += 2 * inventory[EQUIPMENT_WIELD].to_hit;
                damage += inventory[EQUIPMENT_WIELD].to_damage;
                damage = damage * 3;
                distance = 25;
            }
            break;
        case 6:
            if (item.category_id == TV_BOLT) { // Heavy Crossbow and Bolt
                base_to_hit = py.misc.bth_with_bows;
                plus_to_hit += 2 * inventory[EQUIPMENT_WIELD].to_hit;
                damage += inventory[EQUIPMENT_WIELD].to_damage;
                damage = damage * 4;
                distance = 35;
            }
            break;
        default:
            // NOOP
            break;
    }
}

static void inventoryDropOrThrowItem(int y, int x, Inventory_t *item) {
    int pos_y = y;
    int pos_x = x;

    bool flag = false;

    if (randomNumber(10) > 1) {
        for (int k = 0; !flag && k <= 9;) {
            if (coordInBounds(Coord_t{pos_y, pos_x})) {
                if (dg.floor[pos_y][pos_x].feature_id <= MAX_OPEN_SPACE && dg.floor[pos_y][pos_x].treasure_id == 0) {
                    flag = true;
                }
            }

            if (!flag) {
                pos_y = y + randomNumber(3) - 2;
                pos_x = x + randomNumber(3) - 2;
                k++;
            }
        }
    }

    if (flag) {
        int cur_pos = popt();
        dg.floor[pos_y][pos_x].treasure_id = (uint8_t) cur_pos;
        treasure_list[cur_pos] = *item;
        dungeonLiteSpot(pos_y, pos_x);
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
    if (py.unique_inventory_items == 0) {
        printMessage("But you are not carrying anything.");
        game.player_free_turn = true;
        return;
    }

    int item_id;
    if (!inventoryGetInputForItemId(item_id, "Fire/Throw which one?", 0, py.unique_inventory_items - 1, CNIL, CNIL)) {
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
    int y = py.row;
    int x = py.col;
    int old_y = py.row;
    int old_x = py.col;
    int current_distance = 0;

    bool flag = false;

    while (!flag) {
        (void) playerMovePosition(dir, y, x);

        if (current_distance + 1 > tdis) {
            break;
        }

        current_distance++;
        dungeonLiteSpot(old_y, old_x);

        const Tile_t &tile = dg.floor[y][x];

        if (tile.feature_id <= MAX_OPEN_SPACE && !flag) {
            if (tile.creature_id > 1) {
                flag = true;

                const Monster_t &m_ptr = monsters[tile.creature_id];

                tbth -= current_distance;

                // if monster not lit, make it much more difficult to hit, subtract
                // off most bonuses, and reduce bth_with_bows depending on distance.
                if (!m_ptr.lit) {
                    tbth /= current_distance + 2;
                    tbth -= py.misc.level * class_level_adj[py.misc.class_id][CLASS_BTHB] / 2;
                    tbth -= tpth * (BTH_PER_PLUS_TO_HIT_ADJUST - 1);
                }

                if (playerTestBeingHit(tbth, (int) py.misc.level, tpth, (int) creatures_list[m_ptr.creature_id].ac, CLASS_BTHB)) {
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
                    tdam = playerWeaponCriticalBlow((int) thrown_item.weight, tpth, tdam, CLASS_BTHB);

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
                    inventoryDropOrThrowItem(old_y, old_x, &thrown_item);
                }
            } else {
                // do not test tile.field_mark here

                if (coordInsidePanel(Coord_t{y, x}) && py.flags.blind < 1 && (tile.temporary_light || tile.permanent_light)) {
                    panelPutTile(tile_char, Coord_t{y, x});
                    putQIO(); // show object moving
                }
            }
        } else {
            flag = true;
            inventoryDropOrThrowItem(old_y, old_x, &thrown_item);
        }

        old_y = y;
        old_x = x;
    }
}
