// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// Player specific variable definitions

#include "headers.h"

// Player record for most player related info
Player_t py = Player_t{};

static void playerResetFlags() {
    py.flags.see_invisible = false;
    py.flags.teleport = false;
    py.flags.free_action = false;
    py.flags.slow_digest = false;
    py.flags.aggravate = false;
    py.flags.sustain_str = false;
    py.flags.sustain_int = false;
    py.flags.sustain_wis = false;
    py.flags.sustain_con = false;
    py.flags.sustain_dex = false;
    py.flags.sustain_chr = false;
    py.flags.resistant_to_fire = false;
    py.flags.resistant_to_acid = false;
    py.flags.resistant_to_cold = false;
    py.flags.regenerate_hp = false;
    py.flags.resistant_to_light = false;
    py.flags.free_fall = false;
}

bool playerIsMale() {
    return py.misc.gender;
}

void playerSetGender(bool is_male) {
    py.misc.gender = is_male;
}

const char *playerGetGenderLabel() {
    if (playerIsMale()) {
        return "Male";
    }
    return "Female";
}

// Given direction "dir", returns new row, column location -RAK-
bool playerMovePosition(int dir, Coord_t &coord) {
    Coord_t new_coord = Coord_t{0, 0};

    switch (dir) {
        case 1:
            new_coord.y = coord.y + 1;
            new_coord.x = coord.x - 1;
            break;
        case 2:
            new_coord.y = coord.y + 1;
            new_coord.x = coord.x;
            break;
        case 3:
            new_coord.y = coord.y + 1;
            new_coord.x = coord.x + 1;
            break;
        case 4:
            new_coord.y = coord.y;
            new_coord.x = coord.x - 1;
            break;
        case 5:
            new_coord.y = coord.y;
            new_coord.x = coord.x;
            break;
        case 6:
            new_coord.y = coord.y;
            new_coord.x = coord.x + 1;
            break;
        case 7:
            new_coord.y = coord.y - 1;
            new_coord.x = coord.x - 1;
            break;
        case 8:
            new_coord.y = coord.y - 1;
            new_coord.x = coord.x;
            break;
        case 9:
            new_coord.y = coord.y - 1;
            new_coord.x = coord.x + 1;
            break;
        default:
            new_coord.y = 0;
            new_coord.x = 0;
            break;
    }

    bool can_move = false;

    if (new_coord.y >= 0 && new_coord.y < dg.height && new_coord.x >= 0 && new_coord.x < dg.width) {
        coord = new_coord;
        can_move = true;
    }

    return can_move;
}

// Teleport the player to a new location -RAK-
void playerTeleport(int new_distance) {
    Coord_t location = Coord_t{0, 0};

    do {
        location.y = randomNumber(dg.height) - 1;
        location.x = randomNumber(dg.width) - 1;

        while (coordDistanceBetween(location, py.pos) > new_distance) {
            location.y += (py.pos.y - location.y) / 2;
            location.x += (py.pos.x - location.x) / 2;
        }
    } while (dg.floor[location.y][location.x].feature_id >= MIN_CLOSED_SPACE || dg.floor[location.y][location.x].creature_id >= 2);

    dungeonMoveCreatureRecord(py.pos, location);

    Coord_t spot = Coord_t{0, 0};
    for (spot.y = py.pos.y - 1; spot.y <= py.pos.y + 1; spot.y++) {
        for (spot.x = py.pos.x - 1; spot.x <= py.pos.x + 1; spot.x++) {
            dg.floor[spot.y][spot.x].temporary_light = false;
            dungeonLiteSpot(spot);
        }
    }

    dungeonLiteSpot(py.pos);

    py.pos.y = location.y;
    py.pos.x = location.x;

    dungeonResetView();
    updateMonsters(false);

    game.teleport_player = false;
}

// Returns true if player has no light -RAK-
bool playerNoLight() {
    return !dg.floor[py.pos.y][py.pos.x].temporary_light && !dg.floor[py.pos.y][py.pos.x].permanent_light;
}

// Something happens to disturb the player. -CJS-
// The first arg indicates a major disturbance, which affects search.
// The second arg indicates a light change.
void playerDisturb(int major_disturbance, int light_disturbance) {
    game.command_count = 0;

    if ((major_disturbance != 0) && ((py.flags.status & config::player::status::PY_SEARCH) != 0u)) {
        playerSearchOff();
    }

    if (py.flags.rest != 0) {
        playerRestOff();
    }

    if ((light_disturbance != 0) || (py.running_tracker != 0)) {
        py.running_tracker = 0;
        dungeonResetView();
    }

    flushInputBuffer();
}

// Search Mode enhancement -RAK-
void playerSearchOn() {
    playerChangeSpeed(1);

    py.flags.status |= config::player::status::PY_SEARCH;

    printCharacterMovementState();
    printCharacterSpeed();

    py.flags.food_digested++;
}

void playerSearchOff() {
    dungeonResetView();
    playerChangeSpeed(-1);

    py.flags.status &= ~config::player::status::PY_SEARCH;

    printCharacterMovementState();
    printCharacterSpeed();
    py.flags.food_digested--;
}

// Resting allows a player to safely restore his hp -RAK-
void playerRestOn() {
    int rest_num;

    if (game.command_count > 0) {
        rest_num = game.command_count;
        game.command_count = 0;
    } else {
        rest_num = 0;
        vtype_t rest_str = {'\0'};

        putStringClearToEOL("Rest for how long? ", Coord_t{0, 0});

        if (getStringInput(rest_str, Coord_t{0, 19}, 5)) {
            if (rest_str[0] == '*') {
                rest_num = -SHRT_MAX;
            } else {
                (void) stringToNumber(rest_str, rest_num);
            }
        }
    }

    // check for reasonable value, must be positive number
    // in range of a short, or must be -MAX_SHORT
    if (rest_num == -SHRT_MAX || (rest_num > 0 && rest_num <= SHRT_MAX)) {
        if ((py.flags.status & config::player::status::PY_SEARCH) != 0u) {
            playerSearchOff();
        }

        py.flags.rest = (int16_t) rest_num;
        py.flags.status |= config::player::status::PY_REST;
        printCharacterMovementState();
        py.flags.food_digested--;

        putStringClearToEOL("Press any key to stop resting...", Coord_t{0, 0});
        putQIO();

        return;
    }

    // Something went wrong
    if (rest_num != 0) {
        printMessage("Invalid rest count.");
    }
    messageLineClear();

    game.player_free_turn = true;
}

void playerRestOff() {
    py.flags.rest = 0;
    py.flags.status &= ~config::player::status::PY_REST;

    printCharacterMovementState();

    // flush last message, or delete "press any key" message
    printMessage(CNIL);

    py.flags.food_digested++;
}

// For "DIED_FROM" string
void playerDiedFromString(vtype_t *description, const char *monster_name, uint32_t move) {
    if ((move & config::monsters::move::CM_WIN) != 0u) {
        (void) sprintf(*description, "The %s", monster_name);
    } else if (isVowel(monster_name[0])) {
        (void) sprintf(*description, "an %s", monster_name);
    } else {
        (void) sprintf(*description, "a %s", monster_name);
    }
}

bool playerTestAttackHits(int attack_id, uint8_t level) {
    bool success = false;

    switch (attack_id) {
        case 1: // Normal attack
            if (playerTestBeingHit(60, (int) level, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
                success = true;
            }
            break;
        case 2: // Lose Strength
            if (playerTestBeingHit(-3, (int) level, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
                success = true;
            }
            break;
        case 3: // Confusion attack
        case 4: // Fear attack
        case 5: // Fire attack
            if (playerTestBeingHit(10, (int) level, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
                success = true;
            }
            break;
        case 6: // Acid attack
            if (playerTestBeingHit(0, (int) level, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
                success = true;
            }
            break;
        case 7: // Cold attack
        case 8: // Lightning attack
            if (playerTestBeingHit(10, (int) level, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
                success = true;
            }
            break;
        case 9: // Corrosion attack
            if (playerTestBeingHit(0, (int) level, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
                success = true;
            }
            break;
        case 10: // Blindness attack
        case 11: // Paralysis attack
            if (playerTestBeingHit(2, (int) level, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
                success = true;
            }
            break;
        case 12: // Steal Money
            if (playerTestBeingHit(5, (int) level, 0, (int) py.misc.level, CLASS_MISC_HIT) && py.misc.au > 0) {
                success = true;
            }
            break;
        case 13: // Steal Object
            if (playerTestBeingHit(2, (int) level, 0, (int) py.misc.level, CLASS_MISC_HIT) && py.pack.unique_items > 0) {
                success = true;
            }
            break;
        case 14: // Poison
            if (playerTestBeingHit(5, (int) level, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
                success = true;
            }
            break;
        case 15: // Lose dexterity
        case 16: // Lose constitution
            if (playerTestBeingHit(0, (int) level, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
                success = true;
            }
            break;
        case 17: // Lose intelligence
        case 18: // Lose wisdom
            if (playerTestBeingHit(2, (int) level, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
                success = true;
            }
            break;
        case 19: // Lose experience
            if (playerTestBeingHit(5, (int) level, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
                success = true;
            }
            break;
        case 20: // Aggravate monsters
            success = true;
            break;
        case 21: // Disenchant
            if (playerTestBeingHit(20, (int) level, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
                success = true;
            }
            break;
        case 22: // Eat food
        case 23: // Eat light
            if (playerTestBeingHit(5, (int) level, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT)) {
                success = true;
            }
            break;
        case 24: // Eat charges
            // check to make sure an object exists
            if (playerTestBeingHit(15, (int) level, 0, py.misc.ac + py.misc.magical_ac, CLASS_MISC_HIT) && py.pack.unique_items > 0) {
                success = true;
            }
            break;
        case 99:
            success = true;
            break;
        default:
            break;
    }

    return success;
}

// Changes speed of monsters relative to player -RAK-
// Note: When the player is sped up or slowed down, I simply change
// the speed of all the monsters. This greatly simplified the logic.
void playerChangeSpeed(int speed) {
    py.flags.speed += speed;
    py.flags.status |= config::player::status::PY_SPEED;

    for (int i = next_free_monster_id - 1; i >= config::monsters::MON_MIN_INDEX_ID; i--) {
        monsters[i].speed += speed;
    }
}

// Player bonuses -RAK-
//
// When an item is worn or taken off, this re-adjusts the player bonuses.
//     Factor =  1 : wear
//     Factor = -1 : removed
//
// Only calculates properties with cumulative effect.  Properties that
// depend on everything being worn are recalculated by playerRecalculateBonuses() -CJS-
void playerAdjustBonusesForItem(Inventory_t const &item, int factor) {
    int amount = item.misc_use * factor;

    if ((item.flags & config::treasure::flags::TR_STATS) != 0u) {
        for (int i = 0; i < 6; i++) {
            if (((1 << i) & item.flags) != 0u) {
                playerStatBoost(i, amount);
            }
        }
    }

    if ((item.flags & config::treasure::flags::TR_SEARCH) != 0u) {
        py.misc.chance_in_search += amount;
        py.misc.fos -= amount;
    }

    if ((item.flags & config::treasure::flags::TR_STEALTH) != 0u) {
        py.misc.stealth_factor += amount;
    }

    if ((item.flags & config::treasure::flags::TR_SPEED) != 0u) {
        playerChangeSpeed(-amount);
    }

    if (((item.flags & config::treasure::flags::TR_BLIND) != 0u) && factor > 0) {
        py.flags.blind += 1000;
    }

    if (((item.flags & config::treasure::flags::TR_TIMID) != 0u) && factor > 0) {
        py.flags.afraid += 50;
    }

    if ((item.flags & config::treasure::flags::TR_INFRA) != 0u) {
        py.flags.see_infra += amount;
    }
}

static void playerRecalculateBonusesFromInventory() {
    for (int i = PlayerEquipment::Wield; i < PlayerEquipment::Light; i++) {
        Inventory_t const &item = py.inventory[i];

        if (item.category_id != TV_NOTHING) {
            py.misc.plusses_to_hit += item.to_hit;

            // Bows can't damage. -CJS-
            if (item.category_id != TV_BOW) {
                py.misc.plusses_to_damage += item.to_damage;
            }

            py.misc.magical_ac += item.to_ac;
            py.misc.ac += item.ac;

            if (spellItemIdentified(item)) {
                py.misc.display_to_hit += item.to_hit;

                // Bows can't damage. -CJS-
                if (item.category_id != TV_BOW) {
                    py.misc.display_to_damage += item.to_damage;
                }

                py.misc.display_to_ac += item.to_ac;
                py.misc.display_ac += item.ac;
            } else if ((item.flags & config::treasure::flags::TR_CURSED) == 0u) {
                // Base AC values should always be visible,
                // as long as the item is not cursed.
                py.misc.display_ac += item.ac;
            }
        }
    }
}

static void playerRecalculateSustainStatsFromInventory() {
    for (int i = PlayerEquipment::Wield; i < PlayerEquipment::Light; i++) {
        if ((py.inventory[i].flags & config::treasure::flags::TR_SUST_STAT) == 0u) {
            continue;
        }

        switch (py.inventory[i].misc_use) {
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
void playerRecalculateBonuses() {
    // Temporarily adjust food_digested
    if (py.flags.slow_digest) {
        py.flags.food_digested++;
    }
    if (py.flags.regenerate_hp) {
        py.flags.food_digested -= 3;
    }

    int saved_display_ac = py.misc.display_ac;

    playerResetFlags();

    // Real values
    py.misc.plusses_to_hit = playerToHitAdjustment();
    py.misc.plusses_to_damage = playerDamageAdjustment();
    py.misc.magical_ac = playerArmorClassAdjustment();
    py.misc.ac = 0;

    // Display values
    py.misc.display_to_hit = py.misc.plusses_to_hit;
    py.misc.display_to_damage = py.misc.plusses_to_damage;
    py.misc.display_ac = 0;
    py.misc.display_to_ac = py.misc.magical_ac;

    playerRecalculateBonusesFromInventory();

    py.misc.display_ac += py.misc.display_to_ac;

    if (py.weapon_is_heavy) {
        py.misc.display_to_hit += (py.stats.used[PlayerAttr::A_STR] * 15 - py.inventory[PlayerEquipment::Wield].weight);
    }

    // Add in temporary spell increases
    if (py.flags.invulnerability > 0) {
        py.misc.ac += 100;
        py.misc.display_ac += 100;
    }

    if (py.flags.blessed > 0) {
        py.misc.ac += 2;
        py.misc.display_ac += 2;
    }

    if (py.flags.detect_invisible > 0) {
        py.flags.see_invisible = true;
    }

    // can't print AC here because might be in a store
    if (saved_display_ac != py.misc.display_ac) {
        py.flags.status |= config::player::status::PY_ARMOR;
    }

    uint32_t item_flags = inventoryCollectAllItemFlags();

    if ((item_flags & config::treasure::flags::TR_SLOW_DIGEST) != 0u) {
        py.flags.slow_digest = true;
    }
    if ((item_flags & config::treasure::flags::TR_AGGRAVATE) != 0u) {
        py.flags.aggravate = true;
    }
    if ((item_flags & config::treasure::flags::TR_TELEPORT) != 0u) {
        py.flags.teleport = true;
    }
    if ((item_flags & config::treasure::flags::TR_REGEN) != 0u) {
        py.flags.regenerate_hp = true;
    }
    if ((item_flags & config::treasure::flags::TR_RES_FIRE) != 0u) {
        py.flags.resistant_to_fire = true;
    }
    if ((item_flags & config::treasure::flags::TR_RES_ACID) != 0u) {
        py.flags.resistant_to_acid = true;
    }
    if ((item_flags & config::treasure::flags::TR_RES_COLD) != 0u) {
        py.flags.resistant_to_cold = true;
    }
    if ((item_flags & config::treasure::flags::TR_FREE_ACT) != 0u) {
        py.flags.free_action = true;
    }
    if ((item_flags & config::treasure::flags::TR_SEE_INVIS) != 0u) {
        py.flags.see_invisible = true;
    }
    if ((item_flags & config::treasure::flags::TR_RES_LIGHT) != 0u) {
        py.flags.resistant_to_light = true;
    }
    if ((item_flags & config::treasure::flags::TR_FFALL) != 0u) {
        py.flags.free_fall = true;
    }

    playerRecalculateSustainStatsFromInventory();

    // Reset food_digested values
    if (py.flags.slow_digest) {
        py.flags.food_digested--;
    }
    if (py.flags.regenerate_hp) {
        py.flags.food_digested += 3;
    }
}

// Remove item from equipment list -RAK-
void playerTakeOff(int item_id, int pack_position_id) {
    py.flags.status |= config::player::status::PY_STR_WGT;

    Inventory_t &item = py.inventory[item_id];

    py.pack.weight -= item.weight * item.items_count;
    py.equipment_count--;

    const char *p = nullptr;
    if (item_id == PlayerEquipment::Wield || item_id == PlayerEquipment::Auxiliary) {
        p = "Was wielding ";
    } else if (item_id == PlayerEquipment::Light) {
        p = "Light source was ";
    } else {
        p = "Was wearing ";
    }

    obj_desc_t description = {'\0'};
    itemDescription(description, item, true);

    obj_desc_t msg = {'\0'};
    if (pack_position_id >= 0) {
        (void) sprintf(msg, "%s%s (%c)", p, description, 'a' + pack_position_id);
    } else {
        (void) sprintf(msg, "%s%s", p, description);
    }
    printMessage(msg);

    // For secondary weapon
    if (item_id != PlayerEquipment::Auxiliary) {
        playerAdjustBonusesForItem(item, -1);
    }

    inventoryItemCopyTo(config::dungeon::objects::OBJ_NOTHING, item);
}

// Attacker's level and plusses,  defender's AC -RAK-
bool playerTestBeingHit(int base_to_hit, int level, int plus_to_hit, int armor_class, int attack_type_id) {
    playerDisturb(1, 0);

    // `plus_to_hit` could be less than 0 if player wielding weapon too heavy for them
    int hit_chance = base_to_hit + plus_to_hit * BTH_PER_PLUS_TO_HIT_ADJUST + (level * class_level_adj[py.misc.class_id][attack_type_id]);

    // always miss 1 out of 20, always hit 1 out of 20
    int die = randomNumber(20);

    // normal hit
    return (die != 1 && (die == 20 || (hit_chance > 0 && randomNumber(hit_chance) > armor_class)));
}

// Decreases players hit points and sets game.character_is_dead flag if necessary -RAK-
void playerTakesHit(int damage, const char *creature_name_label) {
    if (py.flags.invulnerability > 0) {
        damage = 0;
    }
    py.misc.current_hp -= damage;

    if (py.misc.current_hp >= 0) {
        printCharacterCurrentHitPoints();
        return;
    }

    if (!game.character_is_dead) {
        game.character_is_dead = true;

        (void) strcpy(game.character_died_from, creature_name_label);

        game.total_winner = false;
    }

    dg.generate_new_level = true;
}

// Searches for hidden things. -RAK-
void playerSearch(Coord_t coord, int chance) {
    if (py.flags.confused > 0) {
        chance = chance / 10;
    }

    if (py.flags.blind > 0 || playerNoLight()) {
        chance = chance / 10;
    }

    if (py.flags.image > 0) {
        chance = chance / 10;
    }

    Coord_t spot = Coord_t{0, 0};
    for (spot.y = coord.y - 1; spot.y <= coord.y + 1; spot.y++) {
        for (spot.x = coord.x - 1; spot.x <= coord.x + 1; spot.x++) {
            // always coordInBounds() here
            if (randomNumber(100) >= chance) {
                continue;
            }

            if (dg.floor[spot.y][spot.x].treasure_id == 0) {
                continue;
            }

            // Search for hidden objects

            Inventory_t &item = game.treasure.list[dg.floor[spot.y][spot.x].treasure_id];

            if (item.category_id == TV_INVIS_TRAP) {
                // Trap on floor?

                obj_desc_t description = {'\0'};
                itemDescription(description, item, true);

                obj_desc_t msg = {'\0'};
                (void) sprintf(msg, "You have found %s", description);
                printMessage(msg);

                trapChangeVisibility(spot);
                playerEndRunning();
            } else if (item.category_id == TV_SECRET_DOOR) {
                // Secret door?

                printMessage("You have found a secret door.");

                trapChangeVisibility(spot);
                playerEndRunning();
            } else if (item.category_id == TV_CHEST) {
                // Chest is trapped?

                // mask out the treasure bits
                if ((item.flags & config::treasure::chests::CH_TRAPPED) > 1) {
                    if (!spellItemIdentified(item)) {
                        spellItemIdentifyAndRemoveRandomInscription(item);
                        printMessage("You have discovered a trap on the chest!");
                    } else {
                        printMessage("The chest is trapped!");
                    }
                }
            }
        }
    }
}

// Computes current weight limit -RAK-
int playerCarryingLoadLimit() {
    int weight_cap = py.stats.used[PlayerAttr::A_STR] * config::player::PLAYER_WEIGHT_CAP + py.misc.weight;

    if (weight_cap > 3000) {
        weight_cap = 3000;
    }

    return weight_cap;
}

// Are we strong enough for the current pack and weapon? -CJS-
void playerStrength() {
    Inventory_t const &item = py.inventory[PlayerEquipment::Wield];

    if (item.category_id != TV_NOTHING && py.stats.used[PlayerAttr::A_STR] * 15 < item.weight) {
        if (!py.weapon_is_heavy) {
            printMessage("You have trouble wielding such a heavy weapon.");
            py.weapon_is_heavy = true;
            playerRecalculateBonuses();
        }
    } else if (py.weapon_is_heavy) {
        py.weapon_is_heavy = false;
        if (item.category_id != TV_NOTHING) {
            printMessage("You are strong enough to wield your weapon.");
        }
        playerRecalculateBonuses();
    }

    int limit = playerCarryingLoadLimit();

    if (limit < py.pack.weight) {
        limit = py.pack.weight / (limit + 1);
    } else {
        limit = 0;
    }

    if (py.pack.heaviness != limit) {
        if (py.pack.heaviness < limit) {
            printMessage("Your pack is so heavy that it slows you down.");
        } else {
            printMessage("You move more easily under the weight of your pack.");
        }
        playerChangeSpeed(limit - py.pack.heaviness);
        py.pack.heaviness = (int16_t) limit;
    }

    py.flags.status &= ~config::player::status::PY_STR_WGT;
}

static bool playerCanRead() {
    if (py.flags.blind > 0) {
        printMessage("You can't see to read your spell book!");
        return false;
    }

    if (playerNoLight()) {
        printMessage("You have no light to read by.");
        return false;
    }

    return true;
}

static int lastKnownSpell() {
    for (int last_known = 0; last_known < 32; last_known++) {
        if (py.flags.spells_learned_order[last_known] == 99) {
            return last_known;
        }
    }

    // We should never actually reach this, but just in case... -MRC-
    return 0;
}

static uint32_t playerDetermineLearnableSpells() {
    uint32_t spell_flag = 0;

    for (int i = 0; i < py.pack.unique_items; i++) {
        if (py.inventory[i].category_id == TV_MAGIC_BOOK) {
            spell_flag |= py.inventory[i].flags;
        }
    }

    return spell_flag;
}

// gain spells when player wants to -JW-
void playerGainSpells() {
    if (py.flags.confused > 0) {
        printMessage("You are too confused.");
        return;
    }

    int new_spells = py.flags.new_spells_to_learn;
    int diff_spells = 0;

    // TODO(cook) move access to `magic_spells[]` directly to the for loop it's used in, below?
    Spell_t *spells = &magic_spells[py.misc.class_id - 1][0];

    int stat, offset;

    // Priests don't need light because they get spells from their god, so only
    // fail when can't see if player has SPELL_TYPE_MAGE spells. This check is done below.
    if (classes[py.misc.class_id].class_to_use_mage_spells == config::spells::SPELL_TYPE_MAGE) {
        // People with SPELL_TYPE_MAGE spells can't learn spell_bank if they can't read their books.
        if (!playerCanRead()) {
            return;
        }
        stat = PlayerAttr::A_INT;
        offset = config::spells::NAME_OFFSET_SPELLS;
    } else {
        stat = PlayerAttr::A_WIS;
        offset = config::spells::NAME_OFFSET_PRAYERS;
    }

    int last_known = lastKnownSpell();

    if (new_spells == 0) {
        vtype_t tmp_str = {'\0'};
        (void) sprintf(tmp_str, "You can't learn any new %ss!", (stat == PlayerAttr::A_INT ? "spell" : "prayer"));
        printMessage(tmp_str);

        game.player_free_turn = true;
        return;
    }

    uint32_t spell_flag;

    // determine which spells player can learn
    // mages need the book to learn a spell, priests do not need the book
    if (stat == PlayerAttr::A_INT) {
        spell_flag = playerDetermineLearnableSpells();
    } else {
        spell_flag = 0x7FFFFFFF;
    }

    // clear bits for spells already learned
    spell_flag &= ~py.flags.spells_learnt;

    int spell_id = 0;
    int spell_bank[31];
    uint32_t mask = 0x1;

    for (int i = 0; spell_flag != 0u; mask <<= 1, i++) {
        if ((spell_flag & mask) != 0u) {
            spell_flag &= ~mask;
            if (spells[i].level_required <= py.misc.level) {
                spell_bank[spell_id] = i;
                spell_id++;
            }
        }
    }

    if (new_spells > spell_id) {
        printMessage("You seem to be missing a book.");

        diff_spells = new_spells - spell_id;
        new_spells = spell_id;
    }

    if (new_spells == 0) {
        // do nothing
    } else if (stat == PlayerAttr::A_INT) {
        // get to choose which mage spells will be learned
        terminalSaveScreen();
        displaySpellsList(spell_bank, spell_id, false, -1);

        char query;
        while ((new_spells != 0) && getCommand("Learn which spell?", query)) {
            int c = query - 'a';

            // test j < 23 in case i is greater than 22, only 22 spells
            // are actually shown on the screen, so limit choice to those
            if (c >= 0 && c < spell_id && c < 22) {
                new_spells--;

                py.flags.spells_learnt |= 1L << spell_bank[c];
                py.flags.spells_learned_order[last_known] = (uint8_t) spell_bank[c];
                last_known++;

                for (; c <= spell_id - 1; c++) {
                    spell_bank[c] = spell_bank[c + 1];
                }

                spell_id--;

                eraseLine(Coord_t{c + 1, 31});
                displaySpellsList(spell_bank, spell_id, false, -1);
            } else {
                terminalBellSound();
            }
        }

        terminalRestoreScreen();
    } else {
        // pick a prayer at random
        while (new_spells != 0) {
            int id = randomNumber(spell_id) - 1;
            py.flags.spells_learnt |= 1L << spell_bank[id];
            py.flags.spells_learned_order[last_known] = (uint8_t) spell_bank[id];
            last_known++;

            vtype_t tmp_str = {'\0'};
            (void) sprintf(tmp_str, "You have learned the prayer of %s.", spell_names[spell_bank[id] + offset]);
            printMessage(tmp_str);

            for (; id <= spell_id - 1; id++) {
                spell_bank[id] = spell_bank[id + 1];
            }

            spell_id--;
            new_spells--;
        }
    }

    py.flags.new_spells_to_learn = (uint8_t)(new_spells + diff_spells);

    if (py.flags.new_spells_to_learn == 0) {
        py.flags.status |= config::player::status::PY_STUDY;
    }

    // set the mana for first level characters when they learn their first spell.
    if (py.misc.mana == 0) {
        playerGainMana(stat);
    }
}

static int newMana(int stat) {
    int levels = py.misc.level - classes[py.misc.class_id].min_level_for_spell_casting + 1;

    switch (playerStatAdjustmentWisdomIntelligence(stat)) {
        case 1:
        case 2:
            return 1 * levels;
        case 3:
            return 3 * levels / 2;
        case 4:
            return 2 * levels;
        case 5:
            return 5 * levels / 2;
        case 6:
            return 3 * levels;
        case 7:
            return 4 * levels;
        default:
            return 0;
    }
}

// Gain some mana if you know at least one spell -RAK-
void playerGainMana(int stat) {
    if (py.flags.spells_learnt != 0) {
        int new_mana = newMana(stat);

        // increment mana by one, so that first level chars have 2 mana
        if (new_mana > 0) {
            new_mana++;
        }

        // mana can be zero when creating character
        if (py.misc.mana != new_mana) {
            if (py.misc.mana != 0) {
                // change current mana proportionately to change of max mana,
                // divide first to avoid overflow, little loss of accuracy
                int32_t value = (((int32_t) py.misc.current_mana << 16) + py.misc.current_mana_fraction) / py.misc.mana * new_mana;
                py.misc.current_mana = (int16_t)(value >> 16);
                py.misc.current_mana_fraction = (uint16_t)(value & 0xFFFF);
            } else {
                py.misc.current_mana = (int16_t) new_mana;
                py.misc.current_mana_fraction = 0;
            }

            py.misc.mana = (int16_t) new_mana;

            // can't print mana here, may be in store or inventory mode
            py.flags.status |= config::player::status::PY_MANA;
        }
    } else if (py.misc.mana != 0) {
        py.misc.mana = 0;
        py.misc.current_mana = 0;

        // can't print mana here, may be in store or inventory mode
        py.flags.status |= config::player::status::PY_MANA;
    }
}

// Critical hits, Nasty way to die. -RAK-
int playerWeaponCriticalBlow(int weapon_weight, int plus_to_hit, int damage, int attack_type_id) {
    int critical = damage;

    // Weight of weapon, plusses to hit, and character level all
    // contribute to the chance of a critical
    if (randomNumber(5000) <= weapon_weight + 5 * plus_to_hit + (class_level_adj[py.misc.class_id][attack_type_id] * py.misc.level)) {
        weapon_weight += randomNumber(650);

        if (weapon_weight < 400) {
            critical = 2 * damage + 5;
            printMessage("It was a good hit! (x2 damage)");
        } else if (weapon_weight < 700) {
            critical = 3 * damage + 10;
            printMessage("It was an excellent hit! (x3 damage)");
        } else if (weapon_weight < 900) {
            critical = 4 * damage + 15;
            printMessage("It was a superb hit! (x4 damage)");
        } else {
            critical = 5 * damage + 20;
            printMessage("It was a *GREAT* hit! (x5 damage)");
        }
    }

    return critical;
}

// Saving throws for player character. -RAK-
bool playerSavingThrow() {
    int class_level_adjustment = class_level_adj[py.misc.class_id][PlayerClassLevelAdj::SAVE] * py.misc.level / 3;

    int saving = py.misc.saving_throw + playerStatAdjustmentWisdomIntelligence(PlayerAttr::A_WIS) + class_level_adjustment;

    return randomNumber(100) <= saving;
}

void playerGainKillExperience(Creature_t const &creature) {
    int32_t exp = creature.kill_exp_value * creature.level;

    int32_t quotient = exp / py.misc.level;
    int32_t remainder = exp % py.misc.level;

    remainder *= 0x10000L;
    remainder /= py.misc.level;
    remainder += py.misc.exp_fraction;

    if (remainder >= 0x10000L) {
        quotient++;
        py.misc.exp_fraction = (uint16_t)(remainder - 0x10000L);
    } else {
        py.misc.exp_fraction = (uint16_t) remainder;
    }

    py.misc.exp += quotient;
}

static void playerCalculateToHitBlows(int weapon_id, int weapon_weight, int &blows, int &total_to_hit) {
    if (weapon_id != TV_NOTHING) {
        // Proper weapon
        blows = playerAttackBlows(weapon_weight, total_to_hit);
    } else {
        // Bare hands?
        blows = 2;
        total_to_hit = -3;
    }

    // Fix for arrows
    if (weapon_id >= TV_SLING_AMMO && weapon_id <= TV_SPIKE) {
        blows = 1;
    }

    total_to_hit += py.misc.plusses_to_hit;
}

static int playerCalculateBaseToHit(bool creature_lit, int tot_tohit) {
    if (creature_lit) {
        return py.misc.bth;
    }

    // creature not lit, make it more difficult to hit
    int bth;

    bth = py.misc.bth / 2;
    bth -= tot_tohit * (BTH_PER_PLUS_TO_HIT_ADJUST - 1);
    bth -= py.misc.level * class_level_adj[py.misc.class_id][PlayerClassLevelAdj::BTH] / 2;

    return bth;
}

// Player attacks a (poor, defenseless) creature -RAK-
static void playerAttackMonster(Coord_t coord) {
    int creature_id = dg.floor[coord.y][coord.x].creature_id;

    Monster_t &monster = monsters[creature_id];
    Creature_t const &creature = creatures_list[monster.creature_id];
    Inventory_t &item = py.inventory[PlayerEquipment::Wield];

    monster.sleep_count = 0;

    // Does the player know what they're fighting?
    vtype_t name = {'\0'};
    if (!monster.lit) {
        (void) strcpy(name, "it");
    } else {
        (void) sprintf(name, "the %s", creature.name);
    }

    int blows, total_to_hit;
    playerCalculateToHitBlows(item.category_id, item.weight, blows, total_to_hit);

    int base_to_hit = playerCalculateBaseToHit(monster.lit, total_to_hit);

    int damage;
    vtype_t msg = {'\0'};

    // Loop for number of blows, trying to hit the critter.
    // Note: blows will always be greater than 0 at the start of the loop -MRC-
    for (int i = blows; i > 0; i--) {
        if (!playerTestBeingHit(base_to_hit, (int) py.misc.level, total_to_hit, (int) creature.ac, PlayerClassLevelAdj::BTH)) {
            (void) sprintf(msg, "You miss %s.", name);
            printMessage(msg);
            continue;
        }

        (void) sprintf(msg, "You hit %s.", name);
        printMessage(msg);

        if (item.category_id != TV_NOTHING) {
            damage = diceRoll(item.damage);
            damage = itemMagicAbilityDamage(item, damage, monster.creature_id);
            damage = playerWeaponCriticalBlow((int) item.weight, total_to_hit, damage, PlayerClassLevelAdj::BTH);
        } else {
            // Bare hands!?
            damage = diceRoll(Dice_t{1, 1});
            damage = playerWeaponCriticalBlow(1, 0, damage, PlayerClassLevelAdj::BTH);
        }

        damage += py.misc.plusses_to_damage;
        if (damage < 0) {
            damage = 0;
        }

        if (py.flags.confuse_monster) {
            py.flags.confuse_monster = false;

            printMessage("Your hands stop glowing.");

            if (((creature.defenses & config::monsters::defense::CD_NO_SLEEP) != 0) || randomNumber(MON_MAX_LEVELS) < creature.level) {
                (void) sprintf(msg, "%s is unaffected.", name);
            } else {
                (void) sprintf(msg, "%s appears confused.", name);
                if (monster.confused_amount != 0u) {
                    monster.confused_amount += 3;
                } else {
                    monster.confused_amount = (uint8_t)(2 + randomNumber(16));
                }
            }
            printMessage(msg);

            if (monster.lit && randomNumber(4) == 1) {
                creature_recall[monster.creature_id].defenses |= creature.defenses & config::monsters::defense::CD_NO_SLEEP;
            }
        }

        // See if we done it in.
        if (monsterTakeHit(creature_id, damage) >= 0) {
            (void) sprintf(msg, "You have slain %s.", name);
            printMessage(msg);
            displayCharacterExperience();

            return;
        }

        // Use missiles up
        if (item.category_id >= TV_SLING_AMMO && item.category_id <= TV_SPIKE) {
            item.items_count--;
            py.pack.weight -= item.weight;
            py.flags.status |= config::player::status::PY_STR_WGT;

            if (item.items_count == 0) {
                py.equipment_count--;
                playerAdjustBonusesForItem(item, -1);
                inventoryItemCopyTo(config::dungeon::objects::OBJ_NOTHING, item);
                playerRecalculateBonuses();
            }
        }
    }
}

static int16_t playerLockPickingSkill() {
    int16_t skill = py.misc.disarm;

    skill += 2;
    skill *= playerDisarmAdjustment();
    skill += playerStatAdjustmentWisdomIntelligence(PlayerAttr::A_INT);
    skill += class_level_adj[py.misc.class_id][PlayerClassLevelAdj::DISARM] * py.misc.level / 3;

    return skill;
}

static void openClosedDoor(Coord_t coord) {
    Tile_t &tile = dg.floor[coord.y][coord.x];
    Inventory_t &item = game.treasure.list[tile.treasure_id];

    if (item.misc_use > 0) {
        // It's locked.

        if (py.flags.confused > 0) {
            printMessage("You are too confused to pick the lock.");
        } else if (playerLockPickingSkill() - item.misc_use > randomNumber(100)) {
            printMessage("You have picked the lock.");
            py.misc.exp++;
            displayCharacterExperience();
            item.misc_use = 0;
        } else {
            printMessageNoCommandInterrupt("You failed to pick the lock.");
        }
    } else if (item.misc_use < 0) {
        // It's stuck

        printMessage("It appears to be stuck.");
    }

    if (item.misc_use == 0) {
        inventoryItemCopyTo(config::dungeon::objects::OBJ_OPEN_DOOR, game.treasure.list[tile.treasure_id]);
        tile.feature_id = TILE_CORR_FLOOR;
        dungeonLiteSpot(coord);
        game.command_count = 0;
    }
}

static void openClosedChest(Coord_t coord) {
    Tile_t const &tile = dg.floor[coord.y][coord.x];
    Inventory_t &item = game.treasure.list[tile.treasure_id];

    bool success = false;

    if ((item.flags & config::treasure::chests::CH_LOCKED) != 0u) {
        if (py.flags.confused > 0) {
            printMessage("You are too confused to pick the lock.");
        } else if (playerLockPickingSkill() - item.depth_first_found > randomNumber(100)) {
            printMessage("You have picked the lock.");

            py.misc.exp += item.depth_first_found;
            displayCharacterExperience();

            success = true;
        } else {
            printMessageNoCommandInterrupt("You failed to pick the lock.");
        }
    } else {
        success = true;
    }

    if (success) {
        item.flags &= ~config::treasure::chests::CH_LOCKED;
        item.special_name_id = SpecialNameIds::SN_EMPTY;
        spellItemIdentifyAndRemoveRandomInscription(item);
        item.cost = 0;
    }

    // Was chest still trapped?
    if ((item.flags & config::treasure::chests::CH_LOCKED) != 0) {
        return;
    }

    // Oh, yes it was...   (Snicker)
    chestTrap(coord);

    if (tile.treasure_id != 0) {
        // Chest treasure is allocated as if a creature had been killed.
        // clear the cursed chest/monster win flag, so that people
        // can not win by opening a cursed chest
        game.treasure.list[tile.treasure_id].flags &= ~config::treasure::flags::TR_CURSED;

        (void) monsterDeath(coord, game.treasure.list[tile.treasure_id].flags);

        game.treasure.list[tile.treasure_id].flags = 0;
    }
}

// Opens a closed door or closed chest. -RAK-
void playerOpenClosedObject() {
    int dir;
    if (!getDirectionWithMemory(CNIL, dir)) {
        return;
    }

    Coord_t coord = py.pos;
    (void) playerMovePosition(dir, coord);

    bool no_object = false;

    Tile_t const &tile = dg.floor[coord.y][coord.x];
    Inventory_t const &item = game.treasure.list[tile.treasure_id];

    if (tile.creature_id > 1 && tile.treasure_id != 0 && (item.category_id == TV_CLOSED_DOOR || item.category_id == TV_CHEST)) {
        objectBlockedByMonster(tile.creature_id);
    } else if (tile.treasure_id != 0) {
        if (item.category_id == TV_CLOSED_DOOR) {
            openClosedDoor(coord);
        } else if (item.category_id == TV_CHEST) {
            openClosedChest(coord);
        } else {
            no_object = true;
        }
    } else {
        no_object = true;
    }

    if (no_object) {
        game.player_free_turn = true;
        printMessage("I do not see anything you can open there.");
    }
}

// Closes an open door. -RAK-
void playerCloseDoor() {
    int dir;

    if (!getDirectionWithMemory(CNIL, dir)) {
        return;
    }

    Coord_t coord = py.pos;
    (void) playerMovePosition(dir, coord);

    Tile_t &tile = dg.floor[coord.y][coord.x];
    Inventory_t &item = game.treasure.list[tile.treasure_id];

    bool no_object = false;

    if (tile.treasure_id != 0) {
        if (item.category_id == TV_OPEN_DOOR) {
            if (tile.creature_id == 0) {
                if (item.misc_use == 0) {
                    inventoryItemCopyTo(config::dungeon::objects::OBJ_CLOSED_DOOR, item);
                    tile.feature_id = TILE_BLOCKED_FLOOR;
                    dungeonLiteSpot(coord);
                } else {
                    printMessage("The door appears to be broken.");
                }
            } else {
                objectBlockedByMonster(tile.creature_id);
            }
        } else {
            no_object = true;
        }
    } else {
        no_object = true;
    }

    if (no_object) {
        game.player_free_turn = true;
        printMessage("I do not see anything you can close there.");
    }
}

// Tunneling through real wall: 10, 11, 12 -RAK-
// Used by TUNNEL and WALL_TO_MUD
bool playerTunnelWall(Coord_t coord, int digging_ability, int digging_chance) {
    if (digging_ability <= digging_chance) {
        return false;
    }

    Tile_t &tile = dg.floor[coord.y][coord.x];

    if (tile.perma_lit_room) {
        // Should become a room space, check to see whether
        // it should be TILE_LIGHT_FLOOR or TILE_DARK_FLOOR.
        bool found = false;

        for (int y = coord.y - 1; y <= coord.y + 1 && y < MAX_HEIGHT; y++) {
            for (int x = coord.x - 1; x <= coord.x + 1 && x < MAX_WIDTH; x++) {
                if (dg.floor[y][x].feature_id <= MAX_CAVE_ROOM) {
                    tile.feature_id = dg.floor[y][x].feature_id;
                    tile.permanent_light = dg.floor[y][x].permanent_light;
                    found = true;
                    break;
                }
            }
        }

        if (!found) {
            tile.feature_id = TILE_CORR_FLOOR;
            tile.permanent_light = false;
        }
    } else {
        // should become a corridor space
        tile.feature_id = TILE_CORR_FLOOR;
        tile.permanent_light = false;
    }

    tile.field_mark = false;

    if (coordInsidePanel(coord) && (tile.temporary_light || tile.permanent_light) && tile.treasure_id != 0) {
        printMessage("You have found something!");
    }

    dungeonLiteSpot(coord);

    return true;
}

// let the player attack the creature
void playerAttackPosition(Coord_t coord) {
    // Is a Coward?
    if (py.flags.afraid > 0) {
        printMessage("You are too afraid!");
        return;
    }

    playerAttackMonster(coord);
}

// check to see if know any spells greater than level, eliminate them
static void eliminateKnownSpellsGreaterThanLevel(Spell_t *msp_ptr, const char *p, int offset) {
    uint32_t mask = 0x80000000L;

    for (int i = 31; mask != 0u; mask >>= 1, i--) {
        if ((mask & py.flags.spells_learnt) != 0u) {
            if (msp_ptr[i].level_required > py.misc.level) {
                py.flags.spells_learnt &= ~mask;
                py.flags.spells_forgotten |= mask;

                vtype_t msg = {'\0'};
                (void) sprintf(msg, "You have forgotten the %s of %s.", p, spell_names[i + offset]);
                printMessage(msg);
            } else {
                break;
            }
        }
    }
}

static int numberOfSpellsAllowed(int stat) {
    int levels = py.misc.level - classes[py.misc.class_id].min_level_for_spell_casting + 1;

    int allowed;

    switch (playerStatAdjustmentWisdomIntelligence(stat)) {
        case 1:
        case 2:
        case 3:
            allowed = 1 * levels;
            break;
        case 4:
        case 5:
            allowed = 3 * levels / 2;
            break;
        case 6:
            allowed = 2 * levels;
            break;
        case 7:
            allowed = 5 * levels / 2;
            break;
        default:
            allowed = 0;
            break;
    }

    return allowed;
}

static int numberOfSpellsKnown() {
    int known = 0;

    for (uint32_t mask = 0x1; mask != 0u; mask <<= 1) {
        if ((mask & py.flags.spells_learnt) != 0u) {
            known++;
        }
    }

    return known;
}

// remember forgotten spells while forgotten spells exist of new_spells_to_learn positive,
// remember the spells in the order that they were learned
static int rememberForgottenSpells(Spell_t *msp_ptr, int allowed_spells, int new_spells, const char *p, int offset) {
    uint32_t mask;

    for (int n = 0; ((py.flags.spells_forgotten != 0u) && (new_spells != 0) && (n < allowed_spells) && (n < 32)); n++) {
        // order ID is (i+1)th spell learned
        int order_id = py.flags.spells_learned_order[n];

        // shifting by amounts greater than number of bits in long gives
        // an undefined result, so don't shift for unknown spells
        if (order_id == 99) {
            mask = 0x0;
        } else {
            mask = (uint32_t)(1L << order_id);
        }

        if ((mask & py.flags.spells_forgotten) != 0u) {
            if (msp_ptr[order_id].level_required <= py.misc.level) {
                new_spells--;
                py.flags.spells_forgotten &= ~mask;
                py.flags.spells_learnt |= mask;

                vtype_t msg = {'\0'};
                (void) sprintf(msg, "You have remembered the %s of %s.", p, spell_names[order_id + offset]);
                printMessage(msg);
            } else {
                allowed_spells++;
            }
        }
    }

    return new_spells;
}

// determine which spells player can learn must check all spells here,
// in gain_spell() we actually check if the books are present
static int learnableSpells(Spell_t *msp_ptr, int new_spells) {
    auto spell_flag = (uint32_t)(0x7FFFFFFFL & ~py.flags.spells_learnt);

    int id = 0;
    uint32_t mask = 0x1;

    for (int i = 0; spell_flag != 0u; mask <<= 1, i++) {
        if ((spell_flag & mask) != 0u) {
            spell_flag &= ~mask;
            if (msp_ptr[i].level_required <= py.misc.level) {
                id++;
            }
        }
    }

    if (new_spells > id) {
        new_spells = id;
    }

    return new_spells;
}

// forget spells until new_spells_to_learn zero or no more spells know,
// spells are forgotten in the opposite order that they were learned
// NOTE: newSpells is always a negative value
static void forgetSpells(int new_spells, const char *p, int offset) {
    uint32_t mask;

    for (int i = 31; (new_spells != 0) && (py.flags.spells_learnt != 0u); i--) {
        // orderID is the (i+1)th spell learned
        int order_id = py.flags.spells_learned_order[i];

        // shifting by amounts greater than number of bits in long gives
        // an undefined result, so don't shift for unknown spells
        if (order_id == 99) {
            mask = 0x0;
        } else {
            mask = (uint32_t)(1L << order_id);
        }

        if ((mask & py.flags.spells_learnt) != 0u) {
            py.flags.spells_learnt &= ~mask;
            py.flags.spells_forgotten |= mask;
            new_spells++;

            vtype_t msg = {'\0'};
            (void) sprintf(msg, "You have forgotten the %s of %s.", p, spell_names[order_id + offset]);
            printMessage(msg);
        }
    }
}

// calculate number of spells player should have, and
// learn forget spells until that number is met -JEW-
void playerCalculateAllowedSpellsCount(int stat) {
    Spell_t &spell = magic_spells[py.misc.class_id - 1][0];

    const char *magic_type_str = nullptr;
    int offset;

    if (stat == PlayerAttr::A_INT) {
        magic_type_str = "spell";
        offset = config::spells::NAME_OFFSET_SPELLS;
    } else {
        magic_type_str = "prayer";
        offset = config::spells::NAME_OFFSET_PRAYERS;
    }

    // check to see if know any spells greater than level, eliminate them
    eliminateKnownSpellsGreaterThanLevel(&spell, magic_type_str, offset);

    // calc number of spells allowed
    int num_allowed = numberOfSpellsAllowed(stat);
    int num_known = numberOfSpellsKnown();
    int new_spells = num_allowed - num_known;

    if (new_spells > 0) {
        new_spells = rememberForgottenSpells(&spell, num_allowed, new_spells, magic_type_str, offset);

        // If `new_spells_to_learn` is still greater than zero
        if (new_spells > 0) {
            new_spells = learnableSpells(&spell, new_spells);
        }
    } else if (new_spells < 0) {
        forgetSpells(new_spells, magic_type_str, offset);
        new_spells = 0;
    }

    if (new_spells != py.flags.new_spells_to_learn) {
        if (new_spells > 0 && py.flags.new_spells_to_learn == 0) {
            vtype_t msg = {'\0'};
            (void) sprintf(msg, "You can learn some new %ss now.", magic_type_str);
            printMessage(msg);
        }

        py.flags.new_spells_to_learn = (uint8_t) new_spells;
        py.flags.status |= config::player::status::PY_STUDY;
    }
}

char *playerRankTitle() {
    const char *p = nullptr;

    if (py.misc.level < 1) {
        p = "Babe in arms";
    } else if (py.misc.level <= PLAYER_MAX_LEVEL) {
        p = class_rank_titles[py.misc.class_id][py.misc.level - 1];
    } else if (playerIsMale()) {
        p = "**KING**";
    } else {
        p = "**QUEEN**";
    }

    return (char *) p;
}
