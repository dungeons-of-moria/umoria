// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Misc code for maintaining the dungeon, printing player info

#include "headers.h"
#include "externs.h"

// Returns the array number of a random object -RAK-
int itemGetRandomObjectId(int level, bool must_be_small) {
    if (level == 0) {
        return randomNumber(treasure_levels[0]) - 1;
    }

    if (level >= TREASURE_MAX_LEVELS) {
        level = TREASURE_MAX_LEVELS;
    } else if (randomNumber(TREASURE_CHANCE_OF_GREAT_ITEM) == 1) {
        level = level * TREASURE_MAX_LEVELS / randomNumber(TREASURE_MAX_LEVELS) + 1;
        if (level > TREASURE_MAX_LEVELS) {
            level = TREASURE_MAX_LEVELS;
        }
    }

    int object_id;

    // This code has been added to make it slightly more likely to get the
    // higher level objects.  Originally a uniform distribution over all
    // objects less than or equal to the dungeon level. This distribution
    // makes a level n objects occur approx 2/n% of the time on level n,
    // and 1/2n are 0th level.
    do {
        if (randomNumber(2) == 1) {
            object_id = randomNumber(treasure_levels[level]) - 1;
        } else {
            // Choose three objects, pick the highest level.
            object_id = randomNumber(treasure_levels[level]) - 1;

            int j = randomNumber(treasure_levels[level]) - 1;

            if (object_id < j) {
                object_id = j;
            }

            j = randomNumber(treasure_levels[level]) - 1;

            if (object_id < j) {
                object_id = j;
            }

            int foundLevel = game_objects[sorted_objects[object_id]].depth_first_found;

            if (foundLevel == 0) {
                object_id = randomNumber(treasure_levels[0]) - 1;
            } else {
                object_id = randomNumber(treasure_levels[foundLevel] - treasure_levels[foundLevel - 1]) - 1 + treasure_levels[foundLevel - 1];
            }
        }
    } while (must_be_small && setItemsLargerThanChests(&game_objects[sorted_objects[object_id]]));

    return object_id;
}

char *playerTitle() {
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

// Computes current weight limit -RAK-
int playerCarryingLoadLimit() {
    int weight_cap = py.stats.used[A_STR] * PLAYER_WEIGHT_CAP + py.misc.weight;

    if (weight_cap > 3000) {
        weight_cap = 3000;
    }

    return weight_cap;
}

// Are we strong enough for the current pack and weapon? -CJS-
void playerStrength() {
    const Inventory_t &item = inventory[EQUIPMENT_WIELD];

    if (item.category_id != TV_NOTHING && py.stats.used[A_STR] * 15 < item.weight) {
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

    if (limit < py.inventory_weight) {
        limit = py.inventory_weight / (limit + 1);
    } else {
        limit = 0;
    }

    if (py.pack_heaviness != limit) {
        if (py.pack_heaviness < limit) {
            printMessage("Your pack is so heavy that it slows you down.");
        } else {
            printMessage("You move more easily under the weight of your pack.");
        }
        playerChangeSpeed(limit - py.pack_heaviness);
        py.pack_heaviness = (int16_t) limit;
    }

    py.flags.status &= ~PY_STR_WGT;
}

// Returns spell chance of failure for class_to_use_mage_spells -RAK-
int spellChanceOfSuccess(int spell_id) {
    const Spell_t &spell = magic_spells[py.misc.class_id - 1][spell_id];

    int chance = spell.failure_chance - 3 * (py.misc.level - spell.level_required);

    int stat;
    if (classes[py.misc.class_id].class_to_use_mage_spells == SPELL_TYPE_MAGE) {
        stat = A_INT;
    } else {
        stat = A_WIS;
    }

    chance -= 3 * (playerStatAdjustmentWisdomIntelligence(stat) - 1);

    if (spell.mana_required > py.misc.current_mana) {
        chance += 5 * (spell.mana_required - py.misc.current_mana);
    }

    if (chance > 95) {
        chance = 95;
    } else if (chance < 5) {
        chance = 5;
    }

    return chance;
}

// Returns spell pointer -RAK-
bool spellGetId(int *spell_ids, int number_of_choices, int &spell_id, int &spell_chance, const char *prompt, int first_spell) {
    spell_id = -1;

    vtype_t str = {'\0'};
    (void) sprintf(str, "(Spells %c-%c, *=List, <ESCAPE>=exit) %s", spell_ids[0] + 'a' - first_spell, spell_ids[number_of_choices - 1] + 'a' - first_spell, prompt);

    bool spell_found = false;
    bool redraw = false;

    int offset = (classes[py.misc.class_id].class_to_use_mage_spells == SPELL_TYPE_MAGE ? NAME_OFFSET_SPELLS : NAME_OFFSET_PRAYERS);

    char choice;

    while (!spell_found && getCommand(str, choice)) {
        if (isupper((int) choice) != 0) {
            spell_id = choice - 'A' + first_spell;

            // verify that this is in spells[], at most 22 entries in class_to_use_mage_spells[]
            int test_spell_id;
            for (test_spell_id = 0; test_spell_id < number_of_choices; test_spell_id++) {
                if (spell_id == spell_ids[test_spell_id]) {
                    break;
                }
            }

            if (test_spell_id == number_of_choices) {
                spell_id = -2;
            } else {
                const Spell_t &spell = magic_spells[py.misc.class_id - 1][spell_id];

                vtype_t tmp_str = {'\0'};
                (void) sprintf(tmp_str, "Cast %s (%d mana, %d%% fail)?", spell_names[spell_id + offset], spell.mana_required, spellChanceOfSuccess(spell_id));
                if (getInputConfirmation(tmp_str)) {
                    spell_found = true;
                } else {
                    spell_id = -1;
                }
            }
        } else if (islower((int) choice) != 0) {
            spell_id = choice - 'a' + first_spell;

            // verify that this is in spells[], at most 22 entries in class_to_use_mage_spells[]
            int test_spell_id;
            for (test_spell_id = 0; test_spell_id < number_of_choices; test_spell_id++) {
                if (spell_id == spell_ids[test_spell_id]) {
                    break;
                }
            }

            if (test_spell_id == number_of_choices) {
                spell_id = -2;
            } else {
                spell_found = true;
            }
        } else if (choice == '*') {
            // only do this drawing once
            if (!redraw) {
                terminalSaveScreen();
                redraw = true;
                displaySpellsList(spell_ids, number_of_choices, false, first_spell);
            }
        } else if (isalpha((int) choice) != 0) {
            spell_id = -2;
        } else {
            spell_id = -1;
            terminalBellSound();
        }

        if (spell_id == -2) {
            vtype_t tmp_str = {'\0'};
            (void) sprintf(tmp_str, "You don't know that %s.", (offset == NAME_OFFSET_SPELLS ? "spell" : "prayer"));
            printMessage(tmp_str);
        }
    }

    if (redraw) {
        terminalRestoreScreen();
    }

    messageLineClear();

    if (spell_found) {
        spell_chance = spellChanceOfSuccess(spell_id);
    }

    return spell_found;
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
static int rememberForgottenSpells(Spell_t *msp_ptr, int allowedSpells, int newSpells, const char *p, int offset) {
    uint32_t mask;

    for (int n = 0; ((py.flags.spells_forgotten != 0u) && (newSpells != 0) && (n < allowedSpells) && (n < 32)); n++) {
        // orderID is (i+1)th spell learned
        int orderID = py.flags.spells_learned_order[n];

        // shifting by amounts greater than number of bits in long gives
        // an undefined result, so don't shift for unknown spells
        if (orderID == 99) {
            mask = 0x0;
        } else {
            mask = (uint32_t) (1L << orderID);
        }

        if ((mask & py.flags.spells_forgotten) != 0u) {
            if (msp_ptr[orderID].level_required <= py.misc.level) {
                newSpells--;
                py.flags.spells_forgotten &= ~mask;
                py.flags.spells_learnt |= mask;

                vtype_t msg = {'\0'};
                (void) sprintf(msg, "You have remembered the %s of %s.", p, spell_names[orderID + offset]);
                printMessage(msg);
            } else {
                allowedSpells++;
            }
        }
    }

    return newSpells;
}

// determine which spells player can learn must check all spells here,
// in gain_spell() we actually check if the books are present
static int learnableSpells(Spell_t *msp_ptr, int newSpells) {
    auto spell_flag = (uint32_t) (0x7FFFFFFFL & ~py.flags.spells_learnt);

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

    if (newSpells > id) {
        newSpells = id;
    }

    return newSpells;
}

// forget spells until new_spells_to_learn zero or no more spells know,
// spells are forgotten in the opposite order that they were learned
// NOTE: newSpells is always a negative value
static void forgetSpells(int newSpells, const char *p, int offset) {
    uint32_t mask;

    for (int i = 31; (newSpells != 0) && (py.flags.spells_learnt != 0u); i--) {
        // orderID is the (i+1)th spell learned
        int orderID = py.flags.spells_learned_order[i];

        // shifting by amounts greater than number of bits in long gives
        // an undefined result, so don't shift for unknown spells
        if (orderID == 99) {
            mask = 0x0;
        } else {
            mask = (uint32_t) (1L << orderID);
        }

        if ((mask & py.flags.spells_learnt) != 0u) {
            py.flags.spells_learnt &= ~mask;
            py.flags.spells_forgotten |= mask;
            newSpells++;

            vtype_t msg = {'\0'};
            (void) sprintf(msg, "You have forgotten the %s of %s.", p, spell_names[orderID + offset]);
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

    if (stat == A_INT) {
        magic_type_str = "spell";
        offset = NAME_OFFSET_SPELLS;
    } else {
        magic_type_str = "prayer";
        offset = NAME_OFFSET_PRAYERS;
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
        py.flags.status |= PY_STUDY;
    }
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

    for (int i = 0; i < py.unique_inventory_items; i++) {
        if (inventory[i].category_id == TV_MAGIC_BOOK) {
            spell_flag |= inventory[i].flags;
        }
    }

    return spell_flag;
}

// gain spells when player wants to -JW-
void playerGainSpells() {
    // Priests don't need light because they get spells from their god, so only
    // fail when can't see if player has SPELL_TYPE_MAGE spells. This check is done below.
    if (py.flags.confused > 0) {
        printMessage("You are too confused.");
        return;
    }

    int new_spells = py.flags.new_spells_to_learn;
    int diff_spells = 0;

    // TODO(cook) move access to `magic_spells[]` directly to the for loop it's used in, below?
    Spell_t *spells = &magic_spells[py.misc.class_id - 1][0];

    int stat, offset;

    if (classes[py.misc.class_id].class_to_use_mage_spells == SPELL_TYPE_MAGE) {
        // People with SPELL_TYPE_MAGE spells can't learn spell_bank if they can't read their books.
        if (!playerCanRead()) {
            return;
        }
        stat = A_INT;
        offset = NAME_OFFSET_SPELLS;
    } else {
        stat = A_WIS;
        offset = NAME_OFFSET_PRAYERS;
    }

    int last_known = lastKnownSpell();

    if (new_spells == 0) {
        vtype_t tmp_str = {'\0'};
        (void) sprintf(tmp_str, "You can't learn any new %ss!", (stat == A_INT ? "spell" : "prayer"));
        printMessage(tmp_str);

        game.player_free_turn = true;
        return;
    }

    uint32_t spell_flag;

    // determine which spells player can learn
    // mages need the book to learn a spell, priests do not need the book
    if (stat == A_INT) {
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
    } else if (stat == A_INT) {
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
                py.flags.spells_learned_order[last_known++] = (uint8_t) spell_bank[c];

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
            py.flags.spells_learned_order[last_known++] = (uint8_t) spell_bank[id];

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

    py.flags.new_spells_to_learn = (uint8_t) (new_spells + diff_spells);

    if (py.flags.new_spells_to_learn == 0) {
        py.flags.status |= PY_STUDY;
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
                py.misc.current_mana = (int16_t) (value >> 16);
                py.misc.current_mana_fraction = (uint16_t) (value & 0xFFFF);
            } else {
                py.misc.current_mana = (int16_t) new_mana;
                py.misc.current_mana_fraction = 0;
            }

            py.misc.mana = (int16_t) new_mana;

            // can't print mana here, may be in store or inventory mode
            py.flags.status |= PY_MANA;
        }
    } else if (py.misc.mana != 0) {
        py.misc.mana = 0;
        py.misc.current_mana = 0;

        // can't print mana here, may be in store or inventory mode
        py.flags.status |= PY_MANA;
    }
}

// lets anyone enter wizard mode after a disclaimer... -JEW-
bool enterWizardMode() {
    bool answer = false;

    if (game.noscore == 0) {
        printMessage("Wizard mode is for debugging and experimenting.");
        answer = getInputConfirmation("The game will not be scored if you enter wizard mode. Are you sure?");
    }

    if ((game.noscore != 0) || answer) {
        game.noscore |= 0x2;
        game.wizard_mode = true;
        return true;
    }

    return false;
}

// Special damage due to magical abilities of object -RAK-
int itemMagicAbilityDamage(const Inventory_t &item, int total_damage, int monster_id) {
    bool is_ego_weapon = (item.flags & TR_EGO_WEAPON) != 0;
    bool is_projectile = item.category_id >= TV_SLING_AMMO && item.category_id <= TV_ARROW;
    bool is_hafted_sword = item.category_id >= TV_HAFTED && item.category_id <= TV_SWORD;
    bool is_flask = item.category_id == TV_FLASK;

    if (is_ego_weapon && (is_projectile || is_hafted_sword || is_flask)) {
        const Creature_t &creature = creatures_list[monster_id];
        Recall_t &memory = creature_recall[monster_id];

        // Slay Dragon
        if (((creature.defenses & CD_DRAGON) != 0) && ((item.flags & TR_SLAY_DRAGON) != 0u)) {
            memory.defenses |= CD_DRAGON;
            return total_damage * 4;
        }

        // Slay Undead
        if (((creature.defenses & CD_UNDEAD) != 0) && ((item.flags & TR_SLAY_UNDEAD) != 0u)) {
            memory.defenses |= CD_UNDEAD;
            return total_damage * 3;
        }

        // Slay Animal
        if (((creature.defenses & CD_ANIMAL) != 0) && ((item.flags & TR_SLAY_ANIMAL) != 0u)) {
            memory.defenses |= CD_ANIMAL;
            return total_damage * 2;
        }

        // Slay Evil
        if (((creature.defenses & CD_EVIL) != 0) && ((item.flags & TR_SLAY_EVIL) != 0u)) {
            memory.defenses |= CD_EVIL;
            return total_damage * 2;
        }

        // Frost
        if (((creature.defenses & CD_FROST) != 0) && ((item.flags & TR_FROST_BRAND) != 0u)) {
            memory.defenses |= CD_FROST;
            return total_damage * 3 / 2;
        }

        // Fire
        if (((creature.defenses & CD_FIRE) != 0) && ((item.flags & TR_FLAME_TONGUE) != 0u)) {
            memory.defenses |= CD_FIRE;
            return total_damage * 3 / 2;
        }
    }

    return total_damage;
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
    int class_level_adjustment = class_level_adj[py.misc.class_id][CLASS_SAVE] * py.misc.level / 3;

    int saving = py.misc.saving_throw + playerStatAdjustmentWisdomIntelligence(A_WIS) + class_level_adjustment;

    return randomNumber(100) <= saving;
}
