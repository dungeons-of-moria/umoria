// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// Handle monster movement and attacks

#include "headers.h"

// A horrible hack, needed because compactMonsters() is called from deep
// within updateMonsters() via monsterPlaceNew() and monsterSummon().
int hack_monptr = -1;

static bool executeAttackOnPlayer(uint8_t creature_level, int16_t &monster_hp, int monster_id, int attack_type, int damage, vtype_t death_description, bool noticed);

static bool monsterIsVisible(Monster_t const &monster) {
    bool visible = false;

    Tile_t const &tile = dg.floor[monster.pos.y][monster.pos.x];
    Creature_t const &creature = creatures_list[monster.creature_id];

    if (tile.permanent_light || tile.temporary_light || ((py.running_tracker != 0) && monster.distance_from_player < 2 && py.carrying_light)) {
        // Normal sight.
        if ((creature.movement & config::monsters::move::CM_INVISIBLE) == 0) {
            visible = true;
        } else if (py.flags.see_invisible) {
            visible = true;
            creature_recall[monster.creature_id].movement |= config::monsters::move::CM_INVISIBLE;
        }
    } else if (py.flags.see_infra > 0 && monster.distance_from_player <= py.flags.see_infra && ((creature.defenses & config::monsters::defense::CD_INFRA) != 0)) {
        // Infra vision.
        visible = true;
        creature_recall[monster.creature_id].defenses |= config::monsters::defense::CD_INFRA;
    }

    return visible;
}

// Updates screen when monsters move about -RAK-
void monsterUpdateVisibility(int monster_id) {
    bool visible = false;
    Monster_t &monster = monsters[monster_id];

    if (monster.distance_from_player <= config::monsters::MON_MAX_SIGHT && ((py.flags.status & config::player::status::PY_BLIND) == 0u) &&
        coordInsidePanel(Coord_t{monster.pos.y, monster.pos.x})) {
        if (game.wizard_mode) {
            // Wizard sight.
            visible = true;
        } else if (los(py.pos, monster.pos)) {
            visible = monsterIsVisible(monster);
        }
    }

    if (visible) {
        // Light it up.
        if (!monster.lit) {
            playerDisturb(1, 0);
            monster.lit = true;
            dungeonLiteSpot(Coord_t{monster.pos.y, monster.pos.x});

            // notify inventoryExecuteCommand()
            screen_has_changed = true;
        }
    } else if (monster.lit) {
        // Turn it off.
        monster.lit = false;
        dungeonLiteSpot(Coord_t{monster.pos.y, monster.pos.x});

        // notify inventoryExecuteCommand()
        screen_has_changed = true;
    }
}

// Given speed, returns number of moves this turn. -RAK-
// NOTE: Player must always move at least once per iteration,
// a slowed player is handled by moving monsters faster
static int monsterMovementRate(int16_t speed) {
    if (speed > 0) {
        if (py.flags.rest != 0) {
            return 1;
        }
        return speed;
    }

    // speed must be negative here
    int rate = 0;
    if ((dg.game_turn % (2 - speed)) == 0) {
        rate = 1;
    }

    return rate;
}

// Makes sure a new creature gets lit up. -CJS-
static bool monsterMakeVisible(Coord_t coord) {
    int monster_id = dg.floor[coord.y][coord.x].creature_id;
    if (monster_id <= 1) {
        return false;
    }

    monsterUpdateVisibility(monster_id);
    return monsters[monster_id].lit;
}

// Choose correct directions for monster movement -RAK-
static void monsterGetMoveDirection(int monster_id, int *directions) {
    int ay, ax, movement;

    int y = monsters[monster_id].pos.y - py.pos.y;
    int x = monsters[monster_id].pos.x - py.pos.x;

    if (y < 0) {
        movement = 8;
        ay = -y;
    } else {
        movement = 0;
        ay = y;
    }
    if (x > 0) {
        movement += 4;
        ax = x;
    } else {
        ax = -x;
    }

    // this has the advantage of preventing the diamond maneuver, also faster
    if (ay > (ax << 1)) {
        movement += 2;
    } else if (ax > (ay << 1)) {
        movement++;
    }

    switch (movement) {
        case 0:
            directions[0] = 9;
            if (ay > ax) {
                directions[1] = 8;
                directions[2] = 6;
                directions[3] = 7;
                directions[4] = 3;
            } else {
                directions[1] = 6;
                directions[2] = 8;
                directions[3] = 3;
                directions[4] = 7;
            }
            break;
        case 1:
        case 9:
            directions[0] = 6;
            if (y < 0) {
                directions[1] = 3;

                directions[2] = 9;
                directions[3] = 2;
                directions[4] = 8;
            } else {
                directions[1] = 9;
                directions[2] = 3;
                directions[3] = 8;
                directions[4] = 2;
            }
            break;
        case 2:
        case 6:
            directions[0] = 8;
            if (x < 0) {
                directions[1] = 9;
                directions[2] = 7;
                directions[3] = 6;
                directions[4] = 4;
            } else {
                directions[1] = 7;
                directions[2] = 9;
                directions[3] = 4;
                directions[4] = 6;
            }
            break;
        case 4:
            directions[0] = 7;
            if (ay > ax) {
                directions[1] = 8;
                directions[2] = 4;
                directions[3] = 9;
                directions[4] = 1;
            } else {
                directions[1] = 4;
                directions[2] = 8;
                directions[3] = 1;
                directions[4] = 9;
            }
            break;
        case 5:
        case 13:
            directions[0] = 4;
            if (y < 0) {
                directions[1] = 1;
                directions[2] = 7;
                directions[3] = 2;
                directions[4] = 8;
            } else {
                directions[1] = 7;
                directions[2] = 1;
                directions[3] = 8;
                directions[4] = 2;
            }
            break;
        case 8:
            directions[0] = 3;
            if (ay > ax) {
                directions[1] = 2;
                directions[2] = 6;
                directions[3] = 1;
                directions[4] = 9;
            } else {
                directions[1] = 6;
                directions[2] = 2;
                directions[3] = 9;
                directions[4] = 1;
            }
            break;
        case 10:
        case 14:
            directions[0] = 2;
            if (x < 0) {
                directions[1] = 3;
                directions[2] = 1;
                directions[3] = 6;
                directions[4] = 4;
            } else {
                directions[1] = 1;
                directions[2] = 3;
                directions[3] = 4;
                directions[4] = 6;
            }
            break;
        case 12:
            directions[0] = 1;
            if (ay > ax) {
                directions[1] = 2;
                directions[2] = 4;
                directions[3] = 3;
                directions[4] = 7;
            } else {
                directions[1] = 4;
                directions[2] = 2;
                directions[3] = 7;
                directions[4] = 3;
            }
            break;
        default:
            break;
    }
}

static void monsterPrintAttackDescription(char *msg, int attack_id) {
    switch (attack_id) {
        case 1:
            printMessage(strcat(msg, "hits you."));
            break;
        case 2:
            printMessage(strcat(msg, "bites you."));
            break;
        case 3:
            printMessage(strcat(msg, "claws you."));
            break;
        case 4:
            printMessage(strcat(msg, "stings you."));
            break;
        case 5:
            printMessage(strcat(msg, "touches you."));
            break;
#if 0
        case 6:
                    msg_print(strcat(msg, "kicks you."));
                    break;
#endif
        case 7:
            printMessage(strcat(msg, "gazes at you."));
            break;
        case 8:
            printMessage(strcat(msg, "breathes on you."));
            break;
        case 9:
            printMessage(strcat(msg, "spits on you."));
            break;
        case 10:
            printMessage(strcat(msg, "makes a horrible wail."));
            break;
#if 0
        case 11:
                    msg_print(strcat(msg, "embraces you."));
                    break;
#endif
        case 12:
            printMessage(strcat(msg, "crawls on you."));
            break;
        case 13:
            printMessage(strcat(msg, "releases a cloud of spores."));
            break;
        case 14:
            printMessage(strcat(msg, "begs you for money."));
            break;
        case 15:
            printMessage("You've been slimed!");
            break;
        case 16:
            printMessage(strcat(msg, "crushes you."));
            break;
        case 17:
            printMessage(strcat(msg, "tramples you."));
            break;
        case 18:
            printMessage(strcat(msg, "drools on you."));
            break;
        case 19:
            switch (randomNumber(9)) {
                case 1:
                    printMessage(strcat(msg, "insults you!"));
                    break;
                case 2:
                    printMessage(strcat(msg, "insults your mother!"));
                    break;
                case 3:
                    printMessage(strcat(msg, "gives you the finger!"));
                    break;
                case 4:
                    printMessage(strcat(msg, "humiliates you!"));
                    break;
                case 5:
                    printMessage(strcat(msg, "wets on your leg!"));
                    break;
                case 6:
                    printMessage(strcat(msg, "defiles you!"));
                    break;
                case 7:
                    printMessage(strcat(msg, "dances around you!"));
                    break;
                case 8:
                    printMessage(strcat(msg, "makes obscene gestures!"));
                    break;
                case 9:
                    printMessage(strcat(msg, "moons you!!!"));
                    break;
                default:
                    break;
            }
            break;
        case 99:
            printMessage(strcat(msg, "is repelled."));
            break;
        default:
            break;
    }
}

static void monsterConfuseOnAttack(Creature_t const &creature, Monster_t &monster, int attack_type, vtype_t monster_name, bool visible) {
    if (py.flags.confuse_monster && attack_type != 99) {
        printMessage("Your hands stop glowing.");
        py.flags.confuse_monster = false;

        vtype_t msg = {'\0'};

        if (randomNumber(MON_MAX_LEVELS) < creature.level || ((creature.defenses & config::monsters::defense::CD_NO_SLEEP) != 0)) {
            (void) sprintf(msg, "%sis unaffected.", monster_name);
        } else {
            (void) sprintf(msg, "%sappears confused.", monster_name);
            if (monster.confused_amount != 0u) {
                monster.confused_amount += 3;
            } else {
                monster.confused_amount = (uint8_t)(2 + randomNumber(16));
            }
        }

        printMessage(msg);

        if (visible && !game.character_is_dead && randomNumber(4) == 1) {
            creature_recall[monster.creature_id].defenses |= creature.defenses & config::monsters::defense::CD_NO_SLEEP;
        }
    }
}

// Make an attack on the player (chuckle.) -RAK-
static void monsterAttackPlayer(int monster_id) {
    // don't beat a dead body!
    if (game.character_is_dead) {
        return;
    }

    Monster_t &monster = monsters[monster_id];
    Creature_t const &creature = creatures_list[monster.creature_id];

    vtype_t name = {'\0'};
    if (!monster.lit) {
        (void) strcpy(name, "It ");
    } else {
        (void) sprintf(name, "The %s ", creature.name);
    }

    vtype_t death_description = {'\0'};
    playerDiedFromString(&death_description, creature.name, creature.movement);

    int attack_counter = 0;
    for (auto &damage_type_id : creature.damage) {
        if (damage_type_id == 0 || game.character_is_dead) {
            break;
        }

        uint8_t attack_type = monster_attacks[damage_type_id].type_id;
        uint8_t attack_desc = monster_attacks[damage_type_id].description_id;
        Dice_t dice = monster_attacks[damage_type_id].dice;

        if (py.flags.protect_evil > 0 && ((creature.defenses & config::monsters::defense::CD_EVIL) != 0) && py.misc.level + 1 > creature.level) {
            if (monster.lit) {
                creature_recall[monster.creature_id].defenses |= config::monsters::defense::CD_EVIL;
            }
            attack_type = 99;
            attack_desc = 99;
        }

        if (playerTestAttackHits(attack_type, creature.level)) {
            playerDisturb(1, 0);

            // can not strcat to name because the creature may have multiple attacks.
            vtype_t description = {'\0'};
            (void) strcpy(description, name);
            monsterPrintAttackDescription(description, attack_desc);

            // always fail to notice attack if creature invisible, set notice
            // and visible here since creature may be visible when attacking
            // and then teleport afterwards (becoming effectively invisible)
            bool notice = true;
            bool visible = true;
            if (!monster.lit) {
                visible = false;
                notice = false;
            }

            int damage = diceRoll(dice);
            notice = executeAttackOnPlayer(creature.level, monster.hp, monster_id, attack_type, damage, death_description, notice);

            // Moved here from monsterMove, so that monster only confused if it
            // actually hits. A monster that has been repelled has not hit
            // the player, so it should not be confused.
            monsterConfuseOnAttack(creature, monster, attack_desc, name, visible);

            // increase number of attacks if notice true, or if visible and
            // had previously noticed the attack (in which case all this does
            // is help player learn damage), note that in the second case do
            // not increase attacks if creature repelled (no damage done)
            if ((notice || (visible && creature_recall[monster.creature_id].attacks[attack_counter] != 0 && attack_type != 99)) &&
                creature_recall[monster.creature_id].attacks[attack_counter] < UCHAR_MAX) {
                creature_recall[monster.creature_id].attacks[attack_counter]++;
            }

            if (game.character_is_dead && creature_recall[monster.creature_id].deaths < SHRT_MAX) {
                creature_recall[monster.creature_id].deaths++;
            }
        } else {
            if ((attack_desc >= 1 && attack_desc <= 3) || attack_desc == 6) {
                playerDisturb(1, 0);

                vtype_t description = {'\0'};
                (void) strcpy(description, name);
                printMessage(strcat(description, "misses you."));
            }
        }

        if (attack_counter < MON_MAX_ATTACKS - 1) {
            attack_counter++;
        } else {
            break;
        }
    }
}

static void monsterOpenDoor(Tile_t &tile, int16_t monster_hp, uint32_t move_bits, bool &do_turn, bool &do_move, uint32_t &rcmove, Coord_t coord) {
    Inventory_t &item = game.treasure.list[tile.treasure_id];

    // Creature can open doors.
    if ((move_bits & config::monsters::move::CM_OPEN_DOOR) != 0u) {
        bool door_is_stuck = false;

        if (item.category_id == TV_CLOSED_DOOR) {
            do_turn = true;

            if (item.misc_use == 0) {
                // Closed doors

                do_move = true;
            } else if (item.misc_use > 0) {
                // Locked doors

                if (randomNumber((monster_hp + 1) * (50 + item.misc_use)) < 40 * (monster_hp - 10 - item.misc_use)) {
                    item.misc_use = 0;
                }
            } else if (item.misc_use < 0) {
                // Stuck doors

                if (randomNumber((monster_hp + 1) * (50 - item.misc_use)) < 40 * (monster_hp - 10 + item.misc_use)) {
                    printMessage("You hear a door burst open!");
                    playerDisturb(1, 0);
                    door_is_stuck = true;
                    do_move = true;
                }
            }
        } else if (item.category_id == TV_SECRET_DOOR) {
            do_turn = true;
            do_move = true;
        }

        if (do_move) {
            inventoryItemCopyTo(config::dungeon::objects::OBJ_OPEN_DOOR, item);

            // 50% chance of breaking door
            if (door_is_stuck) {
                item.misc_use = (int16_t)(1 - randomNumber(2));
            }
            tile.feature_id = TILE_CORR_FLOOR;
            dungeonLiteSpot(coord);
            rcmove |= config::monsters::move::CM_OPEN_DOOR;
            do_move = false;
        }
    } else if (item.category_id == TV_CLOSED_DOOR) {
        // Creature can not open doors, must bash them
        do_turn = true;

        auto abs_misc_use = (int) std::abs((std::intmax_t) item.misc_use);
        if (randomNumber((monster_hp + 1) * (80 + abs_misc_use)) < 40 * (monster_hp - 20 - abs_misc_use)) {
            inventoryItemCopyTo(config::dungeon::objects::OBJ_OPEN_DOOR, item);

            // 50% chance of breaking door
            item.misc_use = (int16_t)(1 - randomNumber(2));
            tile.feature_id = TILE_CORR_FLOOR;
            dungeonLiteSpot(coord);
            printMessage("You hear a door burst open!");
            playerDisturb(1, 0);
        }
    }
}

static void glyphOfWardingProtection(uint16_t creature_id, uint32_t move_bits, bool &do_move, bool &do_turn, Coord_t coord) {
    if (randomNumber(config::treasure::OBJECTS_RUNE_PROTECTION) < creatures_list[creature_id].level) {
        if (coord.y == py.pos.y && coord.x == py.pos.x) {
            printMessage("The rune of protection is broken!");
        }
        (void) dungeonDeleteObject(coord);
        return;
    }

    do_move = false;

    // If the creature moves only to attack, don't let it
    // move if the glyph prevents it from attacking
    if ((move_bits & config::monsters::move::CM_ATTACK_ONLY) != 0u) {
        do_turn = true;
    }
}

static void monsterMovesOnPlayer(Monster_t const &monster, uint8_t creature_id, int monster_id, uint32_t move_bits, bool &do_move, bool &do_turn, uint32_t &rcmove, Coord_t coord) {
    if (creature_id == 1) {
        // if the monster is not lit, must call monsterUpdateVisibility, it
        // may be faster than character, and hence could have
        // just moved next to character this same turn.
        if (!monster.lit) {
            monsterUpdateVisibility(monster_id);
        }
        monsterAttackPlayer(monster_id);
        do_move = false;
        do_turn = true;
    } else if (creature_id > 1 && (coord.y != monster.pos.y || coord.x != monster.pos.x)) {
        // Creature is attempting to move on other creature?

        // Creature eats other creatures?
        if (((move_bits & config::monsters::move::CM_EATS_OTHER) != 0u) &&
            creatures_list[monster.creature_id].kill_exp_value >= creatures_list[monsters[creature_id].creature_id].kill_exp_value) {
            if (monsters[creature_id].lit) {
                rcmove |= config::monsters::move::CM_EATS_OTHER;
            }

            // It ate an already processed monster. Handle normally.
            if (monster_id < creature_id) {
                dungeonDeleteMonster((int) creature_id);
            } else {
                // If it eats this monster, an already processed
                // monster will take its place, causing all kinds
                // of havoc. Delay the kill a bit.
                dungeonRemoveMonsterFromLevel((int) creature_id);
            }
        } else {
            do_move = false;
        }
    }
}

static void monsterAllowedToMove(Monster_t &monster, uint32_t move_bits, bool &do_turn, uint32_t &rcmove, Coord_t coord) {
    // Pick up or eat an object
    if ((move_bits & config::monsters::move::CM_PICKS_UP) != 0u) {
        uint8_t treasure_id = dg.floor[coord.y][coord.x].treasure_id;

        if (treasure_id != 0 && game.treasure.list[treasure_id].category_id <= TV_MAX_OBJECT) {
            rcmove |= config::monsters::move::CM_PICKS_UP;
            (void) dungeonDeleteObject(coord);
        }
    }

    // Move creature record
    dungeonMoveCreatureRecord(Coord_t{monster.pos.y, monster.pos.x}, coord);

    if (monster.lit) {
        monster.lit = false;
        dungeonLiteSpot(Coord_t{monster.pos.y, monster.pos.x});
    }

    monster.pos.y = coord.y;
    monster.pos.x = coord.x;
    monster.distance_from_player = (uint8_t) coordDistanceBetween(py.pos, coord);

    do_turn = true;
}

// Make the move if possible, five choices -RAK-
static void makeMove(int monster_id, int *directions, uint32_t &rcmove) {
    bool do_turn = false;
    bool do_move = false;

    Monster_t &monster = monsters[monster_id];
    uint32_t move_bits = creatures_list[monster.creature_id].movement;

    // Up to 5 attempts at moving, give up.
    Coord_t coord = Coord_t{0, 0};
    for (int i = 0; !do_turn && i < 5; i++) {
        // Get new position
        coord.y = monster.pos.y;
        coord.x = monster.pos.x;

        (void) playerMovePosition(directions[i], coord);

        Tile_t &tile = dg.floor[coord.y][coord.x];

        if (tile.feature_id == TILE_BOUNDARY_WALL) {
            continue;
        }

        // Floor is open?
        if (tile.feature_id <= MAX_OPEN_SPACE) {
            do_move = true;
        } else if ((move_bits & config::monsters::move::CM_PHASE) != 0u) {
            // Creature moves through walls?
            do_move = true;
            rcmove |= config::monsters::move::CM_PHASE;
        } else if (tile.treasure_id != 0) {
            // Creature can open doors?
            monsterOpenDoor(tile, monster.hp, move_bits, do_turn, do_move, rcmove, coord);
        }

        // Glyph of warding present?
        if (do_move && tile.treasure_id != 0 && game.treasure.list[tile.treasure_id].category_id == TV_VIS_TRAP && game.treasure.list[tile.treasure_id].sub_category_id == 99) {
            glyphOfWardingProtection(monster.creature_id, move_bits, do_move, do_turn, coord);
        }

        // Creature has attempted to move on player?
        if (do_move) {
            monsterMovesOnPlayer(monster, tile.creature_id, monster_id, move_bits, do_move, do_turn, rcmove, coord);
        }

        // Creature has been allowed move.
        if (do_move) {
            monsterAllowedToMove(monster, move_bits, do_turn, rcmove, coord);
        }
    }
}

static bool monsterCanCastSpells(Monster_t const &monster, uint32_t spells) {
    // 1 in x chance of casting spell
    if (randomNumber((int) (spells & config::monsters::spells::CS_FREQ)) != 1) {
        return false;
    }

    // Must be within certain range
    bool within_range = monster.distance_from_player <= config::monsters::MON_MAX_SPELL_CAST_DISTANCE;

    // Must have unobstructed Line-Of-Sight
    bool unobstructed = los(py.pos, monster.pos);

    return within_range && unobstructed;
}

void monsterExecuteCastingOfSpell(Monster_t &monster, int monster_id, int spell_id, uint8_t level, vtype_t monster_name, vtype_t death_description) {
    Coord_t coord = py.pos; //  only used for cases 14 and 15.

    // Cast the spell.
    switch (spell_id) {
        case 5: // Teleport Short
            spellTeleportAwayMonster(monster_id, 5);
            break;
        case 6: // Teleport Long
            spellTeleportAwayMonster(monster_id, config::monsters::MON_MAX_SIGHT);
            break;
        case 7: // Teleport To
            spellTeleportPlayerTo(Coord_t{monster.pos.y, monster.pos.x});
            break;
        case 8: // Light Wound
            if (playerSavingThrow()) {
                printMessage("You resist the effects of the spell.");
            } else {
                playerTakesHit(diceRoll(Dice_t{3, 8}), death_description);
            }
            break;
        case 9: // Serious Wound
            if (playerSavingThrow()) {
                printMessage("You resist the effects of the spell.");
            } else {
                playerTakesHit(diceRoll(Dice_t{8, 8}), death_description);
            }
            break;
        case 10: // Hold Person
            if (py.flags.free_action) {
                printMessage("You are unaffected.");
            } else if (playerSavingThrow()) {
                printMessage("You resist the effects of the spell.");
            } else if (py.flags.paralysis > 0) {
                py.flags.paralysis += 2;
            } else {
                py.flags.paralysis = (int16_t)(randomNumber(5) + 4);
            }
            break;
        case 11: // Cause Blindness
            if (playerSavingThrow()) {
                printMessage("You resist the effects of the spell.");
            } else if (py.flags.blind > 0) {
                py.flags.blind += 6;
            } else {
                py.flags.blind += 12 + randomNumber(3);
            }
            break;
        case 12: // Cause Confuse
            if (playerSavingThrow()) {
                printMessage("You resist the effects of the spell.");
            } else if (py.flags.confused > 0) {
                py.flags.confused += 2;
            } else {
                py.flags.confused = (int16_t)(randomNumber(5) + 3);
            }
            break;
        case 13: // Cause Fear
            if (playerSavingThrow()) {
                printMessage("You resist the effects of the spell.");
            } else if (py.flags.afraid > 0) {
                py.flags.afraid += 2;
            } else {
                py.flags.afraid = (int16_t)(randomNumber(5) + 3);
            }
            break;
        case 14: // Summon Monster
            (void) strcat(monster_name, "magically summons a monster!");
            printMessage(monster_name);
            coord.y = py.pos.y;
            coord.x = py.pos.x;

            // in case compact_monster() is called,it needs monster_id
            hack_monptr = monster_id;
            (void) monsterSummon(coord, false);
            hack_monptr = -1;
            monsterUpdateVisibility((int) dg.floor[coord.y][coord.x].creature_id);
            break;
        case 15: // Summon Undead
            (void) strcat(monster_name, "magically summons an undead!");
            printMessage(monster_name);
            coord.y = py.pos.y;
            coord.x = py.pos.x;

            // in case compact_monster() is called,it needs monster_id
            hack_monptr = monster_id;
            (void) monsterSummonUndead(coord);
            hack_monptr = -1;
            monsterUpdateVisibility((int) dg.floor[coord.y][coord.x].creature_id);
            break;
        case 16: // Slow Person
            if (py.flags.free_action) {
                printMessage("You are unaffected.");
            } else if (playerSavingThrow()) {
                printMessage("You resist the effects of the spell.");
            } else if (py.flags.slow > 0) {
                py.flags.slow += 2;
            } else {
                py.flags.slow = (int16_t)(randomNumber(5) + 3);
            }
            break;
        case 17: // Drain Mana
            if (py.misc.current_mana > 0) {
                playerDisturb(1, 0);

                vtype_t msg = {'\0'};
                (void) sprintf(msg, "%sdraws psychic energy from you!", monster_name);
                printMessage(msg);

                if (monster.lit) {
                    (void) sprintf(msg, "%sappears healthier.", monster_name);
                    printMessage(msg);
                }

                int num = (randomNumber((int) level) >> 1) + 1;
                if (num > py.misc.current_mana) {
                    num = py.misc.current_mana;
                    py.misc.current_mana = 0;
                    py.misc.current_mana_fraction = 0;
                } else {
                    py.misc.current_mana -= num;
                }
                printCharacterCurrentMana();
                monster.hp += 6 * (num);
            }
            break;
        case 20: // Breath Light
            (void) strcat(monster_name, "breathes lightning.");
            printMessage(monster_name);
            spellBreath(py.pos, monster_id, (monster.hp / 4), MagicSpellFlags::Lightning, death_description);
            break;
        case 21: // Breath Gas
            (void) strcat(monster_name, "breathes gas.");
            printMessage(monster_name);
            spellBreath(py.pos, monster_id, (monster.hp / 3), MagicSpellFlags::PoisonGas, death_description);
            break;
        case 22: // Breath Acid
            (void) strcat(monster_name, "breathes acid.");
            printMessage(monster_name);
            spellBreath(py.pos, monster_id, (monster.hp / 3), MagicSpellFlags::Acid, death_description);
            break;
        case 23: // Breath Frost
            (void) strcat(monster_name, "breathes frost.");
            printMessage(monster_name);
            spellBreath(py.pos, monster_id, (monster.hp / 3), MagicSpellFlags::Frost, death_description);
            break;
        case 24: // Breath Fire
            (void) strcat(monster_name, "breathes fire.");
            printMessage(monster_name);
            spellBreath(py.pos, monster_id, (monster.hp / 3), MagicSpellFlags::Fire, death_description);
            break;
        default:
            (void) strcat(monster_name, "cast unknown spell.");
            printMessage(monster_name);
    }
}

// Creatures can cast spells too.  (Dragon Breath) -RAK-
//   castSpellGetId = true if creature changes position
//   return true (took_turn) if creature casts a spell
static bool monsterCastSpell(int monster_id) {
    if (game.character_is_dead) {
        return false;
    }

    Monster_t &monster = monsters[monster_id];
    Creature_t const &creature = creatures_list[monster.creature_id];

    if (!monsterCanCastSpells(monster, creature.spells)) {
        return false;
    }

    // Creature is going to cast a spell

    // Check to see if monster should be lit.
    monsterUpdateVisibility(monster_id);

    // Describe the attack
    vtype_t name = {'\0'};
    if (monster.lit) {
        (void) sprintf(name, "The %s ", creature.name);
    } else {
        (void) strcpy(name, "It ");
    }

    vtype_t death_description = {'\0'};
    playerDiedFromString(&death_description, creature.name, creature.movement);

    // Extract all possible spells into spell_choice
    int spell_choice[30];
    auto spell_flags = (uint32_t)(creature.spells & ~config::monsters::spells::CS_FREQ);

    int id = 0;
    while (spell_flags != 0) {
        spell_choice[id] = getAndClearFirstBit(spell_flags);
        id++;
    }

    // Choose a spell to cast
    int thrown_spell = spell_choice[randomNumber(id) - 1];
    thrown_spell++;

    // all except spellTeleportAwayMonster() and drain mana spells always disturb
    if (thrown_spell > 6 && thrown_spell != 17) {
        playerDisturb(1, 0);
    }

    // save some code/data space here, with a small time penalty
    if ((thrown_spell < 14 && thrown_spell > 6) || thrown_spell == 16) {
        (void) strcat(name, "casts a spell.");
        printMessage(name);
    }

    monsterExecuteCastingOfSpell(monster, monster_id, thrown_spell, creature.level, name, death_description);

    if (monster.lit) {
        creature_recall[monster.creature_id].spells |= 1L << (thrown_spell - 1);
        if ((creature_recall[monster.creature_id].spells & config::monsters::spells::CS_FREQ) != config::monsters::spells::CS_FREQ) {
            creature_recall[monster.creature_id].spells++;
        }
        if (game.character_is_dead && creature_recall[monster.creature_id].deaths < SHRT_MAX) {
            creature_recall[monster.creature_id].deaths++;
        }
    }

    return true;
}

// Places creature adjacent to given location -RAK-
// Rats and Flys are fun!
bool monsterMultiply(Coord_t coord, int creature_id, int monster_id) {
    Coord_t position = Coord_t{0, 0};

    for (int i = 0; i <= 18; i++) {
        position.y = coord.y - 2 + randomNumber(3);
        position.x = coord.x - 2 + randomNumber(3);

        // don't create a new creature on top of the old one, that
        // causes invincible/invisible creatures to appear.
        if (coordInBounds(position) && (position.y != coord.y || position.x != coord.x)) {
            Tile_t const &tile = dg.floor[position.y][position.x];

            if (tile.feature_id <= MAX_OPEN_SPACE && tile.treasure_id == 0 && tile.creature_id != 1) {
                // Creature there already?
                if (tile.creature_id > 1) {
                    // Some critters are cannibalistic!
                    bool cannibalistic = (creatures_list[creature_id].movement & config::monsters::move::CM_EATS_OTHER) != 0;

                    // Check the experience level -CJS-
                    bool experienced = creatures_list[creature_id].kill_exp_value >= creatures_list[monsters[tile.creature_id].creature_id].kill_exp_value;

                    if (cannibalistic && experienced) {
                        // It ate an already processed monster. Handle * normally.
                        if (monster_id < tile.creature_id) {
                            dungeonDeleteMonster((int) tile.creature_id);
                        } else {
                            // If it eats this monster, an already processed
                            // monster will take its place, causing all kinds
                            // of havoc. Delay the kill a bit.
                            dungeonRemoveMonsterFromLevel((int) tile.creature_id);
                        }

                        // in case compact_monster() is called, it needs monster_id.
                        hack_monptr = monster_id;
                        // Place_monster() may fail if monster list full.
                        bool result = monsterPlaceNew(position, creature_id, false);
                        hack_monptr = -1;
                        if (!result) {
                            return false;
                        }

                        monster_multiply_total++;
                        return monsterMakeVisible(position);
                    }
                } else {
                    // All clear,  place a monster

                    // in case compact_monster() is called,it needs monster_id
                    hack_monptr = monster_id;
                    // Place_monster() may fail if monster list full.
                    bool result = monsterPlaceNew(position, creature_id, false);
                    hack_monptr = -1;
                    if (!result) {
                        return false;
                    }

                    monster_multiply_total++;
                    return monsterMakeVisible(position);
                }
            }
        }
    }

    return false;
}

static void monsterMultiplyCritter(Monster_t const &monster, int monster_id, uint32_t &rcmove) {
    int counter = 0;

    Coord_t coord = Coord_t{0, 0};

    for (coord.y = monster.pos.y - 1; coord.y <= monster.pos.y + 1; coord.y++) {
        for (coord.x = monster.pos.x - 1; coord.x <= monster.pos.x + 1; coord.x++) {
            if (coordInBounds(coord) && (dg.floor[coord.y][coord.x].creature_id > 1)) {
                counter++;
            }
        }
    }

    // can't call randomNumber with a value of zero, increment
    // counter to allow creature multiplication.
    if (counter == 0) {
        counter++;
    }

    if (counter < 4 && randomNumber(counter * config::monsters::MON_MULTIPLY_ADJUST) == 1) {
        if (monsterMultiply(Coord_t{monster.pos.y, monster.pos.x}, monster.creature_id, monster_id)) {
            rcmove |= config::monsters::move::CM_MULTIPLY;
        }
    }
}

static void monsterMoveOutOfWall(Monster_t const &monster, int monster_id, uint32_t &rcmove) {
    // If the monster is already dead, don't kill it again!
    // This can happen for monsters moving faster than the player. They
    // will get multiple moves, but should not if they die on the first
    // move.  This is only a problem for monsters stuck in rock.
    if (monster.hp < 0) {
        return;
    }

    int id = 0;
    int dir = 1;
    int directions[9];

    // Note direction of for loops matches direction of keypad from 1 to 9
    // Do not allow attack against the player.
    // Must cast y-1 to signed int, so that a negative value
    // of i will fail the comparison.
    for (int y = monster.pos.y + 1; y >= (monster.pos.y - 1); y--) {
        for (int x = monster.pos.x - 1; x <= monster.pos.x + 1; x++) {
            if (dir != 5 && dg.floor[y][x].feature_id <= MAX_OPEN_SPACE && dg.floor[y][x].creature_id != 1) {
                directions[id] = dir;
                id++;
            }
            dir++;
        }
    }

    if (id != 0) {
        // put a random direction first
        dir = randomNumber(id) - 1;

        int saved_id = directions[0];

        directions[0] = directions[dir];
        directions[dir] = saved_id;

        // this can only fail if directions[0] has a rune of protection
        makeMove(monster_id, directions, rcmove);
    }

    // if still in a wall, let it dig itself out, but also apply some more damage
    if (dg.floor[monster.pos.y][monster.pos.x].feature_id >= MIN_CAVE_WALL) {
        // in case the monster dies, may need to callfix1_delete_monster()
        // instead of delete_monsters()
        hack_monptr = monster_id;
        int i = monsterTakeHit(monster_id, diceRoll(Dice_t{8, 8}));
        hack_monptr = -1;

        if (i >= 0) {
            printMessage("You hear a scream muffled by rock!");
            displayCharacterExperience();
        } else {
            printMessage("A creature digs itself out from the rock!");
            (void) playerTunnelWall(Coord_t{monster.pos.y, monster.pos.x}, 1, 0);
        }
    }
}

// Undead only get confused from turn undead, so they should flee
static void monsterMoveUndead(Creature_t const &creature, int monster_id, uint32_t &rcmove) {
    int directions[9];
    monsterGetMoveDirection(monster_id, directions);

    directions[0] = 10 - directions[0];
    directions[1] = 10 - directions[1];
    directions[2] = 10 - directions[2];
    directions[3] = randomNumber(9); // May attack only if cornered
    directions[4] = randomNumber(9);

    // don't move if it's is not supposed to move!
    if ((creature.movement & config::monsters::move::CM_ATTACK_ONLY) == 0u) {
        makeMove(monster_id, directions, rcmove);
    }
}

static void monsterMoveConfused(Creature_t const &creature, int monster_id, uint32_t &rcmove) {
    int directions[9];

    directions[0] = randomNumber(9);
    directions[1] = randomNumber(9);
    directions[2] = randomNumber(9);
    directions[3] = randomNumber(9);
    directions[4] = randomNumber(9);

    // don't move if it's is not supposed to move!
    if ((creature.movement & config::monsters::move::CM_ATTACK_ONLY) == 0u) {
        makeMove(monster_id, directions, rcmove);
    }
}

static bool monsterDoMove(int monster_id, uint32_t &rcmove, Monster_t &monster, Creature_t const &creature) {
    // Creature is confused or undead turned?
    if (monster.confused_amount != 0u) {
        if ((creature.defenses & config::monsters::defense::CD_UNDEAD) != 0) {
            monsterMoveUndead(creature, monster_id, rcmove);
        } else {
            monsterMoveConfused(creature, monster_id, rcmove);
        }
        monster.confused_amount--;
        return true;
    }

    // Creature may cast a spell
    if ((creature.spells & config::monsters::spells::CS_FREQ) != 0u) {
        return monsterCastSpell(monster_id);
    }

    return false;
}

static void monsterMoveRandomly(int monster_id, uint32_t &rcmove, int randomness) {
    int directions[9];

    directions[0] = randomNumber(9);
    directions[1] = randomNumber(9);
    directions[2] = randomNumber(9);
    directions[3] = randomNumber(9);
    directions[4] = randomNumber(9);

    rcmove |= randomness;

    makeMove(monster_id, directions, rcmove);
}

static void monsterMoveNormally(int monster_id, uint32_t &rcmove) {
    int directions[9];

    if (randomNumber(200) == 1) {
        directions[0] = randomNumber(9);
        directions[1] = randomNumber(9);
        directions[2] = randomNumber(9);
        directions[3] = randomNumber(9);
        directions[4] = randomNumber(9);
    } else {
        monsterGetMoveDirection(monster_id, directions);
    }

    rcmove |= config::monsters::move::CM_MOVE_NORMAL;

    makeMove(monster_id, directions, rcmove);
}

static void monsterAttackWithoutMoving(int monster_id, uint32_t &rcmove, uint8_t distance_from_player) {
    int directions[9];

    if (distance_from_player < 2) {
        monsterGetMoveDirection(monster_id, directions);
        makeMove(monster_id, directions, rcmove);
    } else {
        // Learn that the monster does does not move when
        // it should have moved, but didn't.
        rcmove |= config::monsters::move::CM_ATTACK_ONLY;
    }
}

// Move the critters about the dungeon -RAK-
static void monsterMove(int monster_id, uint32_t &rcmove) {
    Monster_t &monster = monsters[monster_id];
    Creature_t const &creature = creatures_list[monster.creature_id];

    // Does the critter multiply?
    // rest could be negative, to be safe, only use mod with positive values.
    auto abs_rest_period = (int) std::abs((std::intmax_t) py.flags.rest);
    if (((creature.movement & config::monsters::move::CM_MULTIPLY) != 0u) && config::monsters::MON_MAX_MULTIPLY_PER_LEVEL >= monster_multiply_total &&
        (abs_rest_period % config::monsters::MON_MULTIPLY_ADJUST) == 0) {
        monsterMultiplyCritter(monster, monster_id, rcmove);
    }

    // if in wall, must immediately escape to a clear area
    // then monster movement finished
    if (((creature.movement & config::monsters::move::CM_PHASE) == 0u) && dg.floor[monster.pos.y][monster.pos.x].feature_id >= MIN_CAVE_WALL) {
        monsterMoveOutOfWall(monster, monster_id, rcmove);
        return;
    }

    if (monsterDoMove(monster_id, rcmove, monster, creature)) {
        return;
    }

    // 75% random movement
    if (((creature.movement & config::monsters::move::CM_75_RANDOM) != 0u) && randomNumber(100) < 75) {
        monsterMoveRandomly(monster_id, rcmove, config::monsters::move::CM_75_RANDOM);
        return;
    }

    // 40% random movement
    if (((creature.movement & config::monsters::move::CM_40_RANDOM) != 0u) && randomNumber(100) < 40) {
        monsterMoveRandomly(monster_id, rcmove, config::monsters::move::CM_40_RANDOM);
        return;
    }

    // 20% random movement
    if (((creature.movement & config::monsters::move::CM_20_RANDOM) != 0u) && randomNumber(100) < 20) {
        monsterMoveRandomly(monster_id, rcmove, config::monsters::move::CM_20_RANDOM);
        return;
    }

    // Normal movement
    if ((creature.movement & config::monsters::move::CM_MOVE_NORMAL) != 0u) {
        monsterMoveNormally(monster_id, rcmove);
        return;
    }

    // Attack, but don't move
    if ((creature.movement & config::monsters::move::CM_ATTACK_ONLY) != 0u) {
        monsterAttackWithoutMoving(monster_id, rcmove, monster.distance_from_player);
        return;
    }

    if (((creature.movement & config::monsters::move::CM_ONLY_MAGIC) != 0u) && monster.distance_from_player < 2) {
        // A little hack for Quylthulgs, so that one will eventually
        // notice that they have no physical attacks.
        if (creature_recall[monster.creature_id].attacks[0] < UCHAR_MAX) {
            creature_recall[monster.creature_id].attacks[0]++;
        }

        // Another little hack for Quylthulgs, so that one can
        // eventually learn their speed.
        if (creature_recall[monster.creature_id].attacks[0] > 20) {
            creature_recall[monster.creature_id].movement |= config::monsters::move::CM_ONLY_MAGIC;
        }
    }
}

static void memoryUpdateRecall(Monster_t const &monster, bool wake, bool ignore, uint32_t rcmove) {
    if (!monster.lit) {
        return;
    }

    Recall_t &memory = creature_recall[monster.creature_id];

    if (wake) {
        if (memory.wake < UCHAR_MAX) {
            memory.wake++;
        }
    } else if (ignore) {
        if (memory.ignore < UCHAR_MAX) {
            memory.ignore++;
        }
    }

    memory.movement |= rcmove;
}

static void monsterAttackingUpdate(Monster_t &monster, int monster_id, int moves) {
    for (int i = moves; i > 0; i--) {
        bool wake = false;
        bool ignore = false;

        uint32_t rcmove = 0;

        // Monsters trapped in rock must be given a turn also,
        // so that they will die/dig out immediately.
        if (monster.lit || monster.distance_from_player <= creatures_list[monster.creature_id].area_affect_radius ||
            (((creatures_list[monster.creature_id].movement & config::monsters::move::CM_PHASE) == 0u) && dg.floor[monster.pos.y][monster.pos.x].feature_id >= MIN_CAVE_WALL)) {
            if (monster.sleep_count > 0) {
                if (py.flags.aggravate) {
                    monster.sleep_count = 0;
                } else if ((py.flags.rest == 0 && py.flags.paralysis < 1) || (randomNumber(50) == 1)) {
                    int notice = randomNumber(1024);

                    if (notice * notice * notice <= (1L << (29 - py.misc.stealth_factor))) {
                        monster.sleep_count -= (100 / monster.distance_from_player);
                        if (monster.sleep_count > 0) {
                            ignore = true;
                        } else {
                            wake = true;

                            // force it to be exactly zero
                            monster.sleep_count = 0;
                        }
                    }
                }
            }

            if (monster.stunned_amount != 0) {
                // NOTE: Balrog = 100*100 = 10000, it always recovers instantly
                if (randomNumber(5000) < creatures_list[monster.creature_id].level * creatures_list[monster.creature_id].level) {
                    monster.stunned_amount = 0;
                } else {
                    monster.stunned_amount--;
                }

                if (monster.stunned_amount == 0) {
                    if (monster.lit) {
                        vtype_t msg = {'\0'};
                        (void) sprintf(msg, "The %s ", creatures_list[monster.creature_id].name);
                        printMessage(strcat(msg, "recovers and glares at you."));
                    }
                }
            }
            if ((monster.sleep_count == 0) && (monster.stunned_amount == 0)) {
                monsterMove(monster_id, rcmove);
            }
        }

        monsterUpdateVisibility(monster_id);
        memoryUpdateRecall(monster, wake, ignore, rcmove);
    }
}

// Creatures movement and attacking are done from here -RAK-
void updateMonsters(bool attack) {
    // Process the monsters
    for (int id = next_free_monster_id - 1; id >= config::monsters::MON_MIN_INDEX_ID && !game.character_is_dead; id--) {
        Monster_t &monster = monsters[id];

        // Get rid of an eaten/breathed on monster.  Note: Be sure not to
        // process this monster. This is necessary because we can't delete
        // monsters while scanning the monsters here.
        if (monster.hp < 0) {
            dungeonDeleteMonsterRecord(id);
            continue;
        }

        monster.distance_from_player = (uint8_t) coordDistanceBetween(py.pos, Coord_t{monster.pos.y, monster.pos.x});

        // Attack is argument passed to CREATURE
        if (attack) {
            int moves = monsterMovementRate(monster.speed);

            if (moves <= 0) {
                monsterUpdateVisibility(id);
            } else {
                monsterAttackingUpdate(monster, id, moves);
            }
        } else {
            monsterUpdateVisibility(id);
        }

        // Get rid of an eaten/breathed on monster. This is necessary because
        // we can't delete monsters while scanning the monsters here.
        // This monster may have been killed during monsterMove().
        if (monster.hp < 0) {
            dungeonDeleteMonsterRecord(id);
            continue;
        }
    }
}

// Decreases monsters hit points and deletes monster if needed.
// (Picking on my babies.) -RAK-
int monsterTakeHit(int monster_id, int damage) {
    Monster_t &monster = monsters[monster_id];
    Creature_t const &creature = creatures_list[monster.creature_id];

    monster.sleep_count = 0;
    monster.hp -= damage;

    if (monster.hp >= 0) {
        return -1;
    }

    uint32_t treasure_flags = monsterDeath(Coord_t{monster.pos.y, monster.pos.x}, creature.movement);

    Recall_t &memory = creature_recall[monster.creature_id];

    if ((py.flags.blind < 1 && monster.lit) || ((creature.movement & config::monsters::move::CM_WIN) != 0u)) {
        auto tmp = (uint32_t)((memory.movement & config::monsters::move::CM_TREASURE) >> config::monsters::move::CM_TR_SHIFT);

        if (tmp > ((treasure_flags & config::monsters::move::CM_TREASURE) >> config::monsters::move::CM_TR_SHIFT)) {
            treasure_flags = (uint32_t)((treasure_flags & ~config::monsters::move::CM_TREASURE) | (tmp << config::monsters::move::CM_TR_SHIFT));
        }

        memory.movement = (uint32_t)((memory.movement & ~config::monsters::move::CM_TREASURE) | treasure_flags);

        if (memory.kills < SHRT_MAX) {
            memory.kills++;
        }
    }

    playerGainKillExperience(creature);

    // can't call displayCharacterExperience() here, as that would result in "new level"
    // message appearing before "monster dies" message.
    int m_take_hit = monster.creature_id;

    // in case this is called from within updateMonsters(), this is a horrible
    // hack, the monsters/updateMonsters() code needs to be rewritten.
    if (hack_monptr < monster_id) {
        dungeonDeleteMonster(monster_id);
    } else {
        dungeonRemoveMonsterFromLevel(monster_id);
    }

    return m_take_hit;
}

static int monsterDeathItemDropType(uint32_t flags) {
    int object;

    if ((flags & config::monsters::move::CM_CARRY_OBJ) != 0u) {
        object = 1;
    } else {
        object = 0;
    }

    if ((flags & config::monsters::move::CM_CARRY_GOLD) != 0u) {
        object += 2;
    }

    if ((flags & config::monsters::move::CM_SMALL_OBJ) != 0u) {
        object += 4;
    }

    return object;
}

static int monsterDeathItemDropCount(uint32_t flags) {
    int count = 0;

    if (((flags & config::monsters::move::CM_60_RANDOM) != 0u) && randomNumber(100) < 60) {
        count++;
    }

    if (((flags & config::monsters::move::CM_90_RANDOM) != 0u) && randomNumber(100) < 90) {
        count++;
    }

    if ((flags & config::monsters::move::CM_1D2_OBJ) != 0u) {
        count += randomNumber(2);
    }

    if ((flags & config::monsters::move::CM_2D2_OBJ) != 0u) {
        count += diceRoll(Dice_t{2, 2});
    }

    if ((flags & config::monsters::move::CM_4D2_OBJ) != 0u) {
        count += diceRoll(Dice_t{4, 2});
    }

    return count;
}

// Allocates objects upon a creatures death -RAK-
// Oh well,  another creature bites the dust. Reward the
// victor based on flags set in the main creature record.
//
// Returns a mask of bits from the given flags which indicates what the
// monster is seen to have dropped.  This may be added to monster memory.
uint32_t monsterDeath(Coord_t coord, uint32_t flags) {
    int item_type = monsterDeathItemDropType(flags);
    int item_count = monsterDeathItemDropCount(flags);

    uint32_t dropped_item_id = 0;

    if (item_count > 0) {
        dropped_item_id = (uint32_t) dungeonSummonObject(coord, item_count, item_type);
    }

    // maybe the player died in mid-turn
    if (((flags & config::monsters::move::CM_WIN) != 0u) && !game.character_is_dead) {
        game.total_winner = true;

        printCharacterWinner();

        printMessage("*** CONGRATULATIONS *** You have won the game.");
        printMessage("You cannot save this game, but you may retire when ready.");
    }

    if (dropped_item_id == 0) {
        return 0;
    }

    uint32_t return_flags = 0;

    if ((dropped_item_id & 255) != 0u) {
        return_flags |= config::monsters::move::CM_CARRY_OBJ;

        if ((item_type & 0x04) != 0) {
            return_flags |= config::monsters::move::CM_SMALL_OBJ;
        }
    }

    if (dropped_item_id >= 256) {
        return_flags |= config::monsters::move::CM_CARRY_GOLD;
    }

    int number_of_items = (dropped_item_id % 256) + (dropped_item_id / 256);
    number_of_items = number_of_items << config::monsters::move::CM_TR_SHIFT;

    return return_flags | number_of_items;
}

void printMonsterActionText(const std::string &name, const std::string &action) {
    printMessage((name + " " + action).c_str());
}

std::string monsterNameDescription(const std::string &real_name, bool is_lit) {
    if (is_lit) {
        return "The " + real_name;
    }
    return "It";
}

// Sleep creatures adjacent to player -RAK-
bool monsterSleep(Coord_t coord) {
    bool asleep = false;

    for (int y = coord.y - 1; y <= coord.y + 1 && y < MAX_HEIGHT; y++) {
        for (int x = coord.x - 1; x <= coord.x + 1 && x < MAX_WIDTH; x++) {
            uint8_t monster_id = dg.floor[y][x].creature_id;

            if (monster_id <= 1) {
                continue;
            }

            Monster_t &monster = monsters[monster_id];
            Creature_t const &creature = creatures_list[monster.creature_id];

            auto name = monsterNameDescription(creature.name, monster.lit);

            if (randomNumber(MON_MAX_LEVELS) < creature.level || ((creature.defenses & config::monsters::defense::CD_NO_SLEEP) != 0)) {
                if (monster.lit && ((creature.defenses & config::monsters::defense::CD_NO_SLEEP) != 0)) {
                    creature_recall[monster.creature_id].defenses |= config::monsters::defense::CD_NO_SLEEP;
                }

                printMonsterActionText(name, "is unaffected.");
            } else {
                monster.sleep_count = 500;
                asleep = true;

                printMonsterActionText(name, "falls asleep.");
            }
        }
    }

    return asleep;
}

static bool executeAttackOnPlayer(uint8_t creature_level, int16_t &monster_hp, int monster_id, int attack_type, int damage, vtype_t death_description, bool noticed) {
    int item_pos_start;
    int item_pos_end;
    int32_t gold;

    switch (attack_type) {
        case 1: // Normal attack
            // round half-way case down
            damage -= ((py.misc.ac + py.misc.magical_ac) * damage) / 200;
            playerTakesHit(damage, death_description);
            break;
        case 2: // Lose Strength
            playerTakesHit(damage, death_description);
            if (py.flags.sustain_str) {
                printMessage("You feel weaker for a moment, but it passes.");
            } else if (randomNumber(2) == 1) {
                printMessage("You feel weaker.");
                (void) playerStatRandomDecrease(PlayerAttr::A_STR);
            } else {
                noticed = false;
            }
            break;
        case 3: // Confusion attack
            playerTakesHit(damage, death_description);
            if (randomNumber(2) == 1) {
                if (py.flags.confused < 1) {
                    printMessage("You feel confused.");
                    py.flags.confused += randomNumber((int) creature_level);
                } else {
                    noticed = false;
                }
                py.flags.confused += 3;
            } else {
                noticed = false;
            }
            break;
        case 4: // Fear attack
            playerTakesHit(damage, death_description);
            if (playerSavingThrow()) {
                printMessage("You resist the effects!");
            } else if (py.flags.afraid < 1) {
                printMessage("You are suddenly afraid!");
                py.flags.afraid += 3 + randomNumber((int) creature_level);
            } else {
                py.flags.afraid += 3;
                noticed = false;
            }
            break;
        case 5: // Fire attack
            printMessage("You are enveloped in flames!");
            damageFire(damage, death_description);
            break;
        case 6: // Acid attack
            printMessage("You are covered in acid!");
            damageAcid(damage, death_description);
            break;
        case 7: // Cold attack
            printMessage("You are covered with frost!");
            damageCold(damage, death_description);
            break;
        case 8: // Lightning attack
            printMessage("Lightning strikes you!");
            damageLightningBolt(damage, death_description);
            break;
        case 9: // Corrosion attack
            printMessage("A stinging red gas swirls about you.");
            damageCorrodingGas(death_description);
            playerTakesHit(damage, death_description);
            break;
        case 10: // Blindness attack
            playerTakesHit(damage, death_description);
            if (py.flags.blind < 1) {
                py.flags.blind += 10 + randomNumber((int) creature_level);
                printMessage("Your eyes begin to sting.");
            } else {
                py.flags.blind += 5;
                noticed = false;
            }
            break;
        case 11: // Paralysis attack
            playerTakesHit(damage, death_description);
            if (playerSavingThrow()) {
                printMessage("You resist the effects!");
            } else if (py.flags.paralysis < 1) {
                if (py.flags.free_action) {
                    printMessage("You are unaffected.");
                } else {
                    py.flags.paralysis = (int16_t)(randomNumber((int) creature_level) + 3);
                    printMessage("You are paralyzed.");
                }
            } else {
                noticed = false;
            }
            break;
        case 12: // Steal Money
            if (py.flags.paralysis < 1 && randomNumber(124) < py.stats.used[PlayerAttr::A_DEX]) {
                printMessage("You quickly protect your money pouch!");
            } else {
                gold = (py.misc.au / 10) + randomNumber(25);
                if (gold > py.misc.au) {
                    py.misc.au = 0;
                } else {
                    py.misc.au -= gold;
                }
                printMessage("Your purse feels lighter.");
                printCharacterGoldValue();
            }
            if (randomNumber(2) == 1) {
                printMessage("There is a puff of smoke!");
                spellTeleportAwayMonster(monster_id, config::monsters::MON_MAX_SIGHT);
            }
            break;
        case 13: // Steal Object
            if (py.flags.paralysis < 1 && randomNumber(124) < py.stats.used[PlayerAttr::A_DEX]) {
                printMessage("You grab hold of your backpack!");
            } else {
                inventoryDestroyItem(randomNumber(py.pack.unique_items) - 1);
                printMessage("Your backpack feels lighter.");
            }
            if (randomNumber(2) == 1) {
                printMessage("There is a puff of smoke!");
                spellTeleportAwayMonster(monster_id, config::monsters::MON_MAX_SIGHT);
            }
            break;
        case 14: // Poison
            playerTakesHit(damage, death_description);
            printMessage("You feel very sick.");
            py.flags.poisoned += randomNumber((int) creature_level) + 5;
            break;
        case 15: // Lose dexterity
            playerTakesHit(damage, death_description);
            if (py.flags.sustain_dex) {
                printMessage("You feel clumsy for a moment, but it passes.");
            } else {
                printMessage("You feel more clumsy.");
                (void) playerStatRandomDecrease(PlayerAttr::A_DEX);
            }
            break;
        case 16: // Lose constitution
            playerTakesHit(damage, death_description);
            if (py.flags.sustain_con) {
                printMessage("Your body resists the effects of the disease.");
            } else {
                printMessage("Your health is damaged!");
                (void) playerStatRandomDecrease(PlayerAttr::A_CON);
            }
            break;
        case 17: // Lose intelligence
            playerTakesHit(damage, death_description);
            printMessage("You have trouble thinking clearly.");
            if (py.flags.sustain_int) {
                printMessage("But your mind quickly clears.");
            } else {
                (void) playerStatRandomDecrease(PlayerAttr::A_INT);
            }
            break;
        case 18: // Lose wisdom
            playerTakesHit(damage, death_description);
            if (py.flags.sustain_wis) {
                printMessage("Your wisdom is sustained.");
            } else {
                printMessage("Your wisdom is drained.");
                (void) playerStatRandomDecrease(PlayerAttr::A_WIS);
            }
            break;
        case 19: // Lose experience
            printMessage("You feel your life draining away!");
            spellLoseEXP(damage + (py.misc.exp / 100) * config::monsters::MON_PLAYER_EXP_DRAINED_PER_HIT);
            break;
        case 20: // Aggravate monster
            (void) spellAggravateMonsters(20);
            break;
        case 21: // Disenchant
            if (executeDisenchantAttack()) {
                printMessage("There is a static feeling in the air.");
                playerRecalculateBonuses();
            } else {
                noticed = false;
            }
            break;
        case 22: // Eat food
            if (inventoryFindRange(TV_FOOD, TV_NEVER, item_pos_start, item_pos_end)) {
                inventoryDestroyItem(item_pos_start);
                printMessage("It got at your rations!");
            } else {
                noticed = false;
            }
            break;
        case 23: // Eat light
            noticed = inventoryDiminishLightAttack(noticed);
            break;
        case 24: // Eat charges
            noticed = inventoryDiminishChargesAttack(creature_level, monster_hp, noticed);
            break;
            // NOTE: default handles this case
            // case 99:
            //     noticed = false;
            //     break;
        default:
            noticed = false;
            break;
    }

    return noticed;
}
