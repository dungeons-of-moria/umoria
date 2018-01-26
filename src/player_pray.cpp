// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// Code for priest spells

#include "headers.h"

static bool playerCanPray(int &item_pos_begin, int &item_pos_end) {
    if (py.flags.blind > 0) {
        printMessage("You can't see to read your prayer!");
        return false;
    }

    if (playerNoLight()) {
        printMessage("You have no light to read by.");
        return false;
    }

    if (py.flags.confused > 0) {
        printMessage("You are too confused.");
        return false;
    }

    if (classes[py.misc.class_id].class_to_use_mage_spells != SPELL_TYPE_PRIEST) {
        printMessage("Pray hard enough and your prayers may be answered.");
        return false;
    }

    if (py.unique_inventory_items == 0) {
        printMessage("But you are not carrying anything!");
        return false;
    }

    if (!inventoryFindRange(TV_PRAYER_BOOK, TV_NEVER, item_pos_begin, item_pos_end)) {
        printMessage("You are not carrying any Holy Books!");
        return false;
    }

    return true;
}

// names based on spell_names[62] in data_player.cpp
enum class PriestSpellTypes {
    detect_evil = 1,
    cure_light_wounds,
    bless,
    remove_fear,
    call_light,
    find_traps,
    detect_doors_stairs,
    slow_poison,
    blind_creature,
    portal,
    cure_medium_wounds,
    chant,
    sanctuary,
    create_food,
    remove_curse,
    resist_head_cold,
    neutralize_poison,
    orb_of_draining,
    cure_serious_wounds,
    sense_invisible,
    protect_from_evil,
    earthquake,
    sense_surroundings,
    cure_critical_wounds,
    turn_undead,
    prayer,
    dispel_undead,
    heal,
    dispel_evil,
    glyph_of_warding,
    holy_word,
};

// Recite a prayers.
static void playerRecitePrayer(int prayer_type) {
    int dir;

    switch ((PriestSpellTypes) (prayer_type + 1)) {
        case PriestSpellTypes::detect_evil:
            (void) spellDetectEvil();
            break;
        case PriestSpellTypes::cure_light_wounds:
            (void) spellChangePlayerHitPoints(diceRoll(Dice_t{3, 3}));
            break;
        case PriestSpellTypes::bless:
            playerBless(randomNumber(12) + 12);
            break;
        case PriestSpellTypes::remove_fear:
            (void) playerRemoveFear();
            break;
        case PriestSpellTypes::call_light:
            (void) spellLightArea(py.row, py.col);
            break;
        case PriestSpellTypes::find_traps:
            (void) spellDetectTrapsWithinVicinity();
            break;
        case PriestSpellTypes::detect_doors_stairs:
            (void) spellDetectSecretDoorssWithinVicinity();
            break;
        case PriestSpellTypes::slow_poison:
            (void) spellSlowPoison();
            break;
        case PriestSpellTypes::blind_creature:
            if (getDirectionWithMemory(CNIL, dir)) {
                (void) spellConfuseMonster(py.row, py.col, dir);
            }
            break;
        case PriestSpellTypes::portal:
            playerTeleport((py.misc.level * 3));
            break;
        case PriestSpellTypes::cure_medium_wounds:
            (void) spellChangePlayerHitPoints(diceRoll(Dice_t{4, 4}));
            break;
        case PriestSpellTypes::chant:
            playerBless(randomNumber(24) + 24);
            break;
        case PriestSpellTypes::sanctuary:
            (void) monsterSleep(py.row, py.col);
            break;
        case PriestSpellTypes::create_food:
            spellCreateFood();
            break;
        case PriestSpellTypes::remove_curse:
            for (auto &entry : inventory) {
                // only clear flag for items that are wielded or worn
                if (entry.category_id >= TV_MIN_WEAR && entry.category_id <= TV_MAX_WEAR) {
                    entry.flags &= ~config::treasure::flags::TR_CURSED;
                }
            }
            break;
        case PriestSpellTypes::resist_head_cold:
            py.flags.heat_resistance += randomNumber(10) + 10;
            py.flags.cold_resistance += randomNumber(10) + 10;
            break;
        case PriestSpellTypes::neutralize_poison:
            (void) playerCurePoison();
            break;
        case PriestSpellTypes::orb_of_draining:
            if (getDirectionWithMemory(CNIL, dir)) {
                spellFireBall(py.row, py.col, dir, (diceRoll(Dice_t{3, 6}) + py.misc.level), magic_spell_flags::GF_HOLY_ORB, "Black Sphere");
            }
            break;
        case PriestSpellTypes::cure_serious_wounds:
            (void) spellChangePlayerHitPoints(diceRoll(Dice_t{8, 4}));
            break;
        case PriestSpellTypes::sense_invisible:
            playerDetectInvisible(randomNumber(24) + 24);
            break;
        case PriestSpellTypes::protect_from_evil:
            (void) playerProtectEvil();
            break;
        case PriestSpellTypes::earthquake:
            spellEarthquake();
            break;
        case PriestSpellTypes::sense_surroundings:
            spellMapCurrentArea();
            break;
        case PriestSpellTypes::cure_critical_wounds:
            (void) spellChangePlayerHitPoints(diceRoll(Dice_t{16, 4}));
            break;
        case PriestSpellTypes::turn_undead:
            (void) spellTurnUndead();
            break;
        case PriestSpellTypes::prayer:
            playerBless(randomNumber(48) + 48);
            break;
        case PriestSpellTypes::dispel_undead:
            (void) spellDispelCreature(CD_UNDEAD, (3 * py.misc.level));
            break;
        case PriestSpellTypes::heal:
            (void) spellChangePlayerHitPoints(200);
            break;
        case PriestSpellTypes::dispel_evil:
            (void) spellDispelCreature(CD_EVIL, (3 * py.misc.level));
            break;
        case PriestSpellTypes::glyph_of_warding:
            spellWardingGlyph();
            break;
        case PriestSpellTypes::holy_word:
            (void) playerRemoveFear();
            (void) playerCurePoison();
            (void) spellChangePlayerHitPoints(1000);

            for (int i = py_attrs::A_STR; i <= py_attrs::A_CHR; i++) {
                (void) playerStatRestore(i);
            }

            (void) spellDispelCreature(CD_EVIL, (4 * py.misc.level));
            (void) spellTurnUndead();

            if (py.flags.invulnerability < 3) {
                py.flags.invulnerability = 3;
            } else {
                py.flags.invulnerability++;
            }
            break;
        default:
            break;
    }
}

// Pray like HELL. -RAK-
void pray() {
    game.player_free_turn = true;

    int item_pos_begin, item_pos_end;
    if (!playerCanPray(item_pos_begin, item_pos_end)) {
        return;
    }

    int item_id;
    if (!inventoryGetInputForItemId(item_id, "Use which Holy Book?", item_pos_begin, item_pos_end, CNIL, CNIL)) {
        return;
    }

    int choice, chance;
    int result = castSpellGetId("Recite which prayer?", item_id, choice, chance);
    if (result < 0) {
        printMessage("You don't know any prayers in that book.");
        return;
    }
    if (result == 0) {
        return;
    }

    if (randomNumber(100) < chance) {
        printMessage("You lost your concentration!");
        return;
    }

    Spell_t const &spell = magic_spells[py.misc.class_id - 1][choice];

    // NOTE: at least one function called by `playerRecitePrayer()` sets `player_free_turn = true`,
    // e.g. `spellCreateFood()`, so this check is required. -MRC-
    game.player_free_turn = false;
    playerRecitePrayer(choice);
    if (!game.player_free_turn) {
        if ((py.flags.spells_worked & (1L << choice)) == 0) {
            py.misc.exp += spell.exp_gain_for_learning << 2;
            displayCharacterExperience();
            py.flags.spells_worked |= (1L << choice);
        }
    }

    if (!game.player_free_turn) {
        if (spell.mana_required > py.misc.current_mana) {
            printMessage("You faint from fatigue!");
            py.flags.paralysis = (int16_t) randomNumber((5 * (spell.mana_required - py.misc.current_mana)));
            py.misc.current_mana = 0;
            py.misc.current_mana_fraction = 0;
            if (randomNumber(3) == 1) {
                printMessage("You have damaged your health!");
                (void) playerStatRandomDecrease(py_attrs::A_CON);
            }
        } else {
            py.misc.current_mana -= spell.mana_required;
        }

        printCharacterCurrentMana();
    }
}
