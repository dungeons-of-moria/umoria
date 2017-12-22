// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Misc code, mainly handles player movement, inventory, etc

#include "headers.h"
#include "externs.h"

// Changes speed of monsters relative to player -RAK-
// Note: When the player is sped up or slowed down, I simply change
// the speed of all the monsters. This greatly simplified the logic.
void playerChangeSpeed(int speed) {
    py.flags.speed += speed;
    py.flags.status |= PY_SPEED;

    for (int i = next_free_monster_id - 1; i >= MON_MIN_INDEX_ID; i--) {
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
void playerAdjustBonusesForItem(const Inventory_t &item, int factor) {
    int amount = item.misc_use * factor;

    if ((item.flags & TR_STATS) != 0u) {
        for (int i = 0; i < 6; i++) {
            if (((1 << i) & item.flags) != 0u) {
                playerStatBoost(i, amount);
            }
        }
    }

    if ((TR_SEARCH & item.flags) != 0u) {
        py.misc.chance_in_search += amount;
        py.misc.fos -= amount;
    }

    if ((TR_STEALTH & item.flags) != 0u) {
        py.misc.stealth_factor += amount;
    }

    if ((TR_SPEED & item.flags) != 0u) {
        playerChangeSpeed(-amount);
    }

    if (((TR_BLIND & item.flags) != 0u) && factor > 0) {
        py.flags.blind += 1000;
    }

    if (((TR_TIMID & item.flags) != 0u) && factor > 0) {
        py.flags.afraid += 50;
    }

    if ((TR_INFRA & item.flags) != 0u) {
        py.flags.see_infra += amount;
    }
}

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

static void playerRecalculateBonusesFromInventory() {
    for (int i = EQUIPMENT_WIELD; i < EQUIPMENT_LIGHT; i++) {
        const Inventory_t &item = inventory[i];

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
            } else if ((TR_CURSED & item.flags) == 0u) {
                // Base AC values should always be visible,
                // as long as the item is not cursed.
                py.misc.display_ac += item.ac;
            }
        }
    }
}

static uint32_t inventoryCollectAllItemFlags() {
    uint32_t flags = 0;

    for (int i = EQUIPMENT_WIELD; i < EQUIPMENT_LIGHT; i++) {
        flags |= inventory[i].flags;
    }

    return flags;
}

static void playerRecalculateSustainStatsFromInventory() {
    for (int i = EQUIPMENT_WIELD; i < EQUIPMENT_LIGHT; i++) {
        if ((TR_SUST_STAT & inventory[i].flags) == 0u) {
            continue;
        }

        switch (inventory[i].misc_use) {
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

    int savedDisplayAC = py.misc.display_ac;

    playerResetFlags();

    // Real values
    py.misc.plusses_to_hit = (int16_t) playerToHitAdjustment();
    py.misc.plusses_to_damage = (int16_t) playerDamageAdjustment();
    py.misc.magical_ac = (int16_t) playerArmorClassAdjustment();
    py.misc.ac = 0;

    // Display values
    py.misc.display_to_hit = py.misc.plusses_to_hit;
    py.misc.display_to_damage = py.misc.plusses_to_damage;
    py.misc.display_ac = 0;
    py.misc.display_to_ac = py.misc.magical_ac;

    playerRecalculateBonusesFromInventory();

    py.misc.display_ac += py.misc.display_to_ac;

    if (py.weapon_is_heavy) {
        py.misc.display_to_hit += (py.stats.used[A_STR] * 15 - inventory[EQUIPMENT_WIELD].weight);
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
    if (savedDisplayAC != py.misc.display_ac) {
        py.flags.status |= PY_ARMOR;
    }

    uint32_t item_flags = inventoryCollectAllItemFlags();

    if ((TR_SLOW_DIGEST & item_flags) != 0u) {
        py.flags.slow_digest = true;
    }
    if ((TR_AGGRAVATE & item_flags) != 0u) {
        py.flags.aggravate = true;
    }
    if ((TR_TELEPORT & item_flags) != 0u) {
        py.flags.teleport = true;
    }
    if ((TR_REGEN & item_flags) != 0u) {
        py.flags.regenerate_hp = true;
    }
    if ((TR_RES_FIRE & item_flags) != 0u) {
        py.flags.resistant_to_fire = true;
    }
    if ((TR_RES_ACID & item_flags) != 0u) {
        py.flags.resistant_to_acid = true;
    }
    if ((TR_RES_COLD & item_flags) != 0u) {
        py.flags.resistant_to_cold = true;
    }
    if ((TR_FREE_ACT & item_flags) != 0u) {
        py.flags.free_action = true;
    }
    if ((TR_SEE_INVIS & item_flags) != 0u) {
        py.flags.see_invisible = true;
    }
    if ((TR_RES_LIGHT & item_flags) != 0u) {
        py.flags.resistant_to_light = true;
    }
    if ((TR_FFALL & item_flags) != 0u) {
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
    py.flags.status |= PY_STR_WGT;

    Inventory_t &item = inventory[item_id];

    py.inventory_weight -= item.weight * item.items_count;
    py.equipment_count--;

    const char *p = nullptr;
    if (item_id == EQUIPMENT_WIELD || item_id == EQUIPMENT_AUX) {
        p = "Was wielding ";
    } else if (item_id == EQUIPMENT_LIGHT) {
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
    if (item_id != EQUIPMENT_AUX) {
        playerAdjustBonusesForItem(item, -1);
    }

    inventoryItemCopyTo(OBJ_NOTHING, item);
}


// I may have written the town level code, but I'm not exactly
// proud of it.   Adding the stores required some real slucky
// hooks which I have not had time to re-think. -RAK-

// map roguelike direction commands into numbers
static char mapRoguelikeKeysToKeypad(char command) {
    switch (command) {
        case 'h':
            return '4';
        case 'y':
            return '7';
        case 'k':
            return '8';
        case 'u':
            return '9';
        case 'l':
            return '6';
        case 'n':
            return '3';
        case 'j':
            return '2';
        case 'b':
            return '1';
        case '.':
            return '5';
        default:
            return command;
    }
}

// Prompts for a direction -RAK-
// Direction memory added, for repeated commands.  -CJS
bool getDirectionWithMemory(char *prompt, int &direction) {
    static char prev_dir; // Direction memory. -CJS-

    // used in counted commands. -CJS-
    if (game.use_last_direction) {
        direction = prev_dir;
        return true;
    }

    if (prompt == CNIL) {
        prompt = (char *) "Which direction?";
    }

    char command;

    while (true) {
        // Don't end a counted command. -CJS-
        int save = game.command_count;

        if (!getCommand(prompt, command)) {
            game.player_free_turn = true;
            return false;
        }

        game.command_count = save;

        if (config.use_roguelike_keys) {
            command = mapRoguelikeKeysToKeypad(command);
        }

        if (command >= '1' && command <= '9' && command != '5') {
            prev_dir = command - '0';
            direction = prev_dir;
            return true;
        }

        terminalBellSound();
    }
}

// Similar to getDirectionWithMemory(), except that no memory exists,
// and it is allowed to enter the null direction. -CJS-
bool getAllDirections(const char *prompt, int &direction) {
    char command;

    while (true) {
        if (!getCommand(prompt, command)) {
            game.player_free_turn = true;
            return false;
        }

        if (config.use_roguelike_keys) {
            command = mapRoguelikeKeysToKeypad(command);
        }

        if (command >= '1' && command <= '9') {
            direction = command - '0';
            return true;
        }

        terminalBellSound();
    }
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
