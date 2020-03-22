// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// Food code

#include "headers.h"

enum class FoodMagicTypes {
    poison = 1,
    blindness,
    paranoia,
    confusion,
    hallucination,
    cure_poison,
    cure_blindness,
    cure_paranoia,
    cure_confusion,
    weakness,
    unhealth,
    restore_str = 16, // 12-15 are no longer used
    restore_con,
    restore_int,
    restore_wis,
    restore_dex,
    restore_chr,
    first_aid,
    minor_cures,
    light_cures,
    major_cures = 26, // 25 no longer used
    poisonous_food,
};

// Eat some food. -RAK-
void playerEat() {
    game.player_free_turn = true;

    if (py.pack.unique_items == 0) {
        printMessage("But you are not carrying anything.");
        return;
    }

    int item_pos_start, item_pos_end;
    if (!inventoryFindRange(TV_FOOD, TV_NEVER, item_pos_start, item_pos_end)) {
        printMessage("You are not carrying any food.");
        return;
    }

    int item_id;
    if (!inventoryGetInputForItemId(item_id, "Eat what?", item_pos_start, item_pos_end, CNIL, CNIL)) {
        return;
    }

    game.player_free_turn = false;

    bool identified = false;

    Inventory_t *item = &inventory[item_id];
    uint32_t item_flags = item->flags;

    while (item_flags != 0) {
        switch ((FoodMagicTypes)(getAndClearFirstBit(item_flags) + 1)) {
            case FoodMagicTypes::poison:
                py.flags.poisoned += randomNumber(10) + item->depth_first_found;
                identified = true;
                break;
            case FoodMagicTypes::blindness:
                py.flags.blind += randomNumber(250) + 10 * item->depth_first_found + 100;
                drawCavePanel();
                printMessage("A veil of darkness surrounds you.");
                identified = true;
                break;
            case FoodMagicTypes::paranoia:
                py.flags.afraid += randomNumber(10) + item->depth_first_found;
                printMessage("You feel terrified!");
                identified = true;
                break;
            case FoodMagicTypes::confusion:
                py.flags.confused += randomNumber(10) + item->depth_first_found;
                printMessage("You feel drugged.");
                identified = true;
                break;
            case FoodMagicTypes::hallucination:
                py.flags.image += randomNumber(200) + 25 * item->depth_first_found + 200;
                printMessage("You feel drugged.");
                identified = true;
                break;
            case FoodMagicTypes::cure_poison:
                identified = playerCurePoison();
                break;
            case FoodMagicTypes::cure_blindness:
                identified = playerCureBlindness();
                break;
            case FoodMagicTypes::cure_paranoia:
                if (py.flags.afraid > 1) {
                    py.flags.afraid = 1;
                    identified = true;
                }
                break;
            case FoodMagicTypes::cure_confusion:
                identified = playerCureConfusion();
                break;
            case FoodMagicTypes::weakness:
                spellLoseSTR();
                identified = true;
                break;
            case FoodMagicTypes::unhealth:
                spellLoseCON();
                identified = true;
                break;
#if 0 // 12 through 15 are no longer used
            case 12:
                lose_int();
                identified = true;
                break;
            case 13:
                lose_wis();
                identified = true;
                break;
            case 14:
                lose_dex();
                identified = true;
                break;
            case 15:
                lose_chr();
                identified = true;
                break;
#endif
            case FoodMagicTypes::restore_str:
                if (playerStatRestore(py_attrs::A_STR)) {
                    printMessage("You feel your strength returning.");
                    identified = true;
                }
                break;
            case FoodMagicTypes::restore_con:
                if (playerStatRestore(py_attrs::A_CON)) {
                    printMessage("You feel your health returning.");
                    identified = true;
                }
                break;
            case FoodMagicTypes::restore_int:
                if (playerStatRestore(py_attrs::A_INT)) {
                    printMessage("Your head spins a moment.");
                    identified = true;
                }
                break;
            case FoodMagicTypes::restore_wis:
                if (playerStatRestore(py_attrs::A_WIS)) {
                    printMessage("You feel your wisdom returning.");
                    identified = true;
                }
                break;
            case FoodMagicTypes::restore_dex:
                if (playerStatRestore(py_attrs::A_DEX)) {
                    printMessage("You feel more dexterous.");
                    identified = true;
                }
                break;
            case FoodMagicTypes::restore_chr:
                if (playerStatRestore(py_attrs::A_CHR)) {
                    printMessage("Your skin stops itching.");
                    identified = true;
                }
                break;
            case FoodMagicTypes::first_aid:
                identified = spellChangePlayerHitPoints(randomNumber(6));
                break;
            case FoodMagicTypes::minor_cures:
                identified = spellChangePlayerHitPoints(randomNumber(12));
                break;
            case FoodMagicTypes::light_cures:
                identified = spellChangePlayerHitPoints(randomNumber(18));
                break;
#if 0 // 25 is no longer used
            case 25:
                identified = hp_player(damroll(3, 6));
                break;
#endif
            case FoodMagicTypes::major_cures:
                identified = spellChangePlayerHitPoints(diceRoll(Dice_t{3, 12}));
                break;
            case FoodMagicTypes::poisonous_food:
                playerTakesHit(randomNumber(18), "poisonous food.");
                identified = true;
                break;
#if 0 // 28 through 30 are no longer used
            case 28:
                take_hit(randint(8), "poisonous food.");
                identified = true;
                break;
            case 29:
                take_hit(damroll(2, 8), "poisonous food.");
                identified = true;
                break;
            case 30:
                take_hit(damroll(3, 8), "poisonous food.");
                identified = true;
                break;
#endif
            default:
                printMessage("Internal error in playerEat()");
                break;
        }
    }

    if (identified) {
        if (!itemSetColorlessAsIdentified(item->category_id, item->sub_category_id, item->identification)) {
            // use identified it, gain experience
            // round half-way case up
            py.misc.exp += (item->depth_first_found + (py.misc.level >> 1)) / py.misc.level;

            displayCharacterExperience();

            itemIdentify(inventory[item_id], item_id);
            item = &inventory[item_id];
        }
    } else if (!itemSetColorlessAsIdentified(item->category_id, item->sub_category_id, item->identification)) {
        itemSetAsTried(*item);
    }

    playerIngestFood(item->misc_use);

    py.flags.status &= ~(config::player::status::PY_WEAK | config::player::status::PY_HUNGRY);

    printCharacterHungerStatus();

    itemTypeRemainingCountDescription(item_id);
    inventoryDestroyItem(item_id);
}

// Add to the players food time -RAK-
void playerIngestFood(int amount) {
    if (py.flags.food < 0) {
        py.flags.food = 0;
    }

    py.flags.food += amount;

    if (py.flags.food > config::player::PLAYER_FOOD_MAX) {
        printMessage("You are bloated from overeating.");

        // Calculate how much of amount is responsible for the bloating. Give the
        // player food credit for 1/50, and also slow them for that many turns.
        int extra = py.flags.food - config::player::PLAYER_FOOD_MAX;
        if (extra > amount) {
            extra = amount;
        }
        int penalty = extra / 50;

        py.flags.slow += penalty;

        if (extra == amount) {
            py.flags.food = (int16_t)(py.flags.food - amount + penalty);
        } else {
            py.flags.food = (int16_t)(config::player::PLAYER_FOOD_MAX + penalty);
        }
    } else if (py.flags.food > config::player::PLAYER_FOOD_FULL) {
        printMessage("You are full.");
    }
}
