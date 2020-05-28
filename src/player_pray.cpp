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

    if (classes[py.misc.class_id].class_to_use_mage_spells != config::spells::SPELL_TYPE_PRIEST) {
        printMessage("Pray hard enough and your prayers may be answered.");
        return false;
    }

    if (py.pack.unique_items == 0) {
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
    DetectEvil = 1,
    CureLightWounds,
    Bless,
    RemoveFear,
    CallLight,
    FindTraps,
    DetectDoorsStairs,
    SlowPoison,
    BlindCreature,
    Portal,
    CureMediumWounds,
    Chant,
    Sanctuary,
    CreateFood,
    RemoveCurse,
    ResistHeadCold,
    NeutralizePoison,
    OrbOfDraining,
    CureSeriousWounds,
    SenseInvisible,
    ProtectFromEvil,
    Earthquake,
    SenseSurroundings,
    CureCriticalWounds,
    TurnUndead,
    Prayer,
    DispelUndead,
    Heal,
    DispelEvil,
    GlyphOfWarding,
    HolyWord,
};

// Recite a prayers.
static void playerRecitePrayer(int prayer_type) {
    int dir;

    switch ((PriestSpellTypes)(prayer_type + 1)) {
        case PriestSpellTypes::DetectEvil:
            (void) spellDetectEvil();
            break;
        case PriestSpellTypes::CureLightWounds:
            (void) spellChangePlayerHitPoints(diceRoll(Dice_t{3, 3}));
            break;
        case PriestSpellTypes::Bless:
            playerBless(randomNumber(12) + 12);
            break;
        case PriestSpellTypes::RemoveFear:
            (void) playerRemoveFear();
            break;
        case PriestSpellTypes::CallLight:
            (void) spellLightArea(py.pos);
            break;
        case PriestSpellTypes::FindTraps:
            (void) spellDetectTrapsWithinVicinity();
            break;
        case PriestSpellTypes::DetectDoorsStairs:
            (void) spellDetectSecretDoorssWithinVicinity();
            break;
        case PriestSpellTypes::SlowPoison:
            (void) spellSlowPoison();
            break;
        case PriestSpellTypes::BlindCreature:
            if (getDirectionWithMemory(CNIL, dir)) {
                (void) spellConfuseMonster(py.pos, dir);
            }
            break;
        case PriestSpellTypes::Portal:
            playerTeleport((py.misc.level * 3));
            break;
        case PriestSpellTypes::CureMediumWounds:
            (void) spellChangePlayerHitPoints(diceRoll(Dice_t{4, 4}));
            break;
        case PriestSpellTypes::Chant:
            playerBless(randomNumber(24) + 24);
            break;
        case PriestSpellTypes::Sanctuary:
            (void) monsterSleep(py.pos);
            break;
        case PriestSpellTypes::CreateFood:
            spellCreateFood();
            break;
        case PriestSpellTypes::RemoveCurse:
            for (auto &entry : py.inventory) {
                // only clear flag for items that are wielded or worn
                if (entry.category_id >= TV_MIN_WEAR && entry.category_id <= TV_MAX_WEAR) {
                    entry.flags &= ~config::treasure::flags::TR_CURSED;
                }
            }
            break;
        case PriestSpellTypes::ResistHeadCold:
            py.flags.heat_resistance += randomNumber(10) + 10;
            py.flags.cold_resistance += randomNumber(10) + 10;
            break;
        case PriestSpellTypes::NeutralizePoison:
            (void) playerCurePoison();
            break;
        case PriestSpellTypes::OrbOfDraining:
            if (getDirectionWithMemory(CNIL, dir)) {
                spellFireBall(py.pos, dir, (diceRoll(Dice_t{3, 6}) + py.misc.level), MagicSpellFlags::HolyOrb, "Black Sphere");
            }
            break;
        case PriestSpellTypes::CureSeriousWounds:
            (void) spellChangePlayerHitPoints(diceRoll(Dice_t{8, 4}));
            break;
        case PriestSpellTypes::SenseInvisible:
            playerDetectInvisible(randomNumber(24) + 24);
            break;
        case PriestSpellTypes::ProtectFromEvil:
            (void) playerProtectEvil();
            break;
        case PriestSpellTypes::Earthquake:
            spellEarthquake();
            break;
        case PriestSpellTypes::SenseSurroundings:
            spellMapCurrentArea();
            break;
        case PriestSpellTypes::CureCriticalWounds:
            (void) spellChangePlayerHitPoints(diceRoll(Dice_t{16, 4}));
            break;
        case PriestSpellTypes::TurnUndead:
            (void) spellTurnUndead();
            break;
        case PriestSpellTypes::Prayer:
            playerBless(randomNumber(48) + 48);
            break;
        case PriestSpellTypes::DispelUndead:
            (void) spellDispelCreature(config::monsters::defense::CD_UNDEAD, (3 * py.misc.level));
            break;
        case PriestSpellTypes::Heal:
            (void) spellChangePlayerHitPoints(200);
            break;
        case PriestSpellTypes::DispelEvil:
            (void) spellDispelCreature(config::monsters::defense::CD_EVIL, (3 * py.misc.level));
            break;
        case PriestSpellTypes::GlyphOfWarding:
            spellWardingGlyph();
            break;
        case PriestSpellTypes::HolyWord:
            (void) playerRemoveFear();
            (void) playerCurePoison();
            (void) spellChangePlayerHitPoints(1000);

            for (int i = PlayerAttr::A_STR; i <= PlayerAttr::A_CHR; i++) {
                (void) playerStatRestore(i);
            }

            (void) spellDispelCreature(config::monsters::defense::CD_EVIL, (4 * py.misc.level));
            (void) spellTurnUndead();

            if (py.flags.invulnerability < 3) {
                py.flags.invulnerability = 3;
            } else {
                py.flags.invulnerability++;
            }
            break;
        default:
            // All cases are handled, so this should never be reached!
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

    Spell_t const &spell = magic_spells[py.misc.class_id - 1][choice];

    // NOTE: at least one function called by `playerRecitePrayer()` sets `player_free_turn = true`,
    // e.g. `spellCreateFood()`, so this check is required. -MRC-
    game.player_free_turn = false;

    if (randomNumber(100) < chance) {
        printMessage("You lost your concentration!");
    } else {
        playerRecitePrayer(choice);

        if (!game.player_free_turn) {
            if ((py.flags.spells_worked & (1L << choice)) == 0) {
                py.misc.exp += spell.exp_gain_for_learning << 2;
                displayCharacterExperience();
                py.flags.spells_worked |= (1L << choice);
            }
        }
    }

    if (game.player_free_turn) {
        return;
    }

    if (spell.mana_required > py.misc.current_mana) {
        printMessage("You faint from fatigue!");

        py.flags.paralysis = (int16_t) randomNumber((5 * (spell.mana_required - py.misc.current_mana)));
        py.misc.current_mana = 0;
        py.misc.current_mana_fraction = 0;

        if (randomNumber(3) == 1) {
            printMessage("You have damaged your health!");
            (void) playerStatRandomDecrease(PlayerAttr::A_CON);
        }
    } else {
        py.misc.current_mana -= spell.mana_required;
    }

    printCharacterCurrentMana();
}
