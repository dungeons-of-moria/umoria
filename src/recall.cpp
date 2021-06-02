// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Monster memory info -CJS-

#include "headers.h"

// Monster memories
Recall_t creature_recall[MON_MAX_CREATURES];

static vtype_t roff_buffer = {'\0'};        // Line buffer.
static char *roff_buffer_pointer = nullptr; // Pointer into line buffer.
static int roff_print_line;                 // Place to print line now being loaded.

#define plural(c, ss, sp) ((c) == 1 ? (ss) : (sp))

// Number of kills needed for information.
// the higher the level of the monster, the fewer the attacks you need,
// the more damage an attack does, the more attacks you need.
#define knowdamage(l, a, d) ((4 + (l)) * (a) > 80 * (d))

// Print out strings, filling up lines as we go.
static void memoryPrint(const char *p) {
    while (*p != 0) {
        *roff_buffer_pointer = *p;

        if (*p == '\n' || roff_buffer_pointer >= roff_buffer + sizeof(roff_buffer) - 1) {
            char *q = roff_buffer_pointer;
            if (*p != '\n') {
                while (*q != ' ') {
                    q--;
                }
            }
            *q = 0;
            putStringClearToEOL(roff_buffer, Coord_t{roff_print_line, 0});
            roff_print_line++;

            char *r = roff_buffer;

            while (q < roff_buffer_pointer) {
                q++;
                *r = *q;
                r++;
            }
            roff_buffer_pointer = r;
        } else {
            roff_buffer_pointer++;
        }
        p++;
    }
}

// Do we know anything about this monster?
static bool memoryMonsterKnown(Recall_t const &memory) {
    if (game.wizard_mode) {
        return true;
    }

    if ((memory.movement != 0u) || (memory.defenses != 0u) || (memory.kills != 0u) || (memory.spells != 0u) || (memory.deaths != 0u)) {
        return true;
    }

    for (uint8_t attack : memory.attacks) {
        if (attack != 0u) {
            return true;
        }
    }

    return false;
}

static void memoryWizardModeInit(Recall_t &memory, Creature_t const &creature) {
    memory.kills = (uint16_t) SHRT_MAX;
    memory.wake = memory.ignore = UCHAR_MAX;

    uint32_t move = (uint32_t)((creature.movement & config::monsters::move::CM_4D2_OBJ) != 0) * 8;
    move += (uint32_t)((creature.movement & config::monsters::move::CM_2D2_OBJ) != 0) * 4;
    move += (uint32_t)((creature.movement & config::monsters::move::CM_1D2_OBJ) != 0) * 2;
    move += (uint32_t)((creature.movement & config::monsters::move::CM_90_RANDOM) != 0);
    move += (uint32_t)((creature.movement & config::monsters::move::CM_60_RANDOM) != 0);

    memory.movement = (uint32_t)((creature.movement & ~config::monsters::move::CM_TREASURE) | (move << config::monsters::move::CM_TR_SHIFT));
    memory.defenses = creature.defenses;

    if ((creature.spells & config::monsters::spells::CS_FREQ) != 0u) {
        memory.spells = (uint32_t)(creature.spells | config::monsters::spells::CS_FREQ);
    } else {
        memory.spells = creature.spells;
    }

    for (int i = 0; i < MON_MAX_ATTACKS; i++) {
        if (creature.damage[i] == 0)
            break;
        memory.attacks[i] = UCHAR_MAX;
    }

    // A little hack to enable the display of info for Quylthulgs.
    if ((memory.movement & config::monsters::move::CM_ONLY_MAGIC) != 0u) {
        memory.attacks[0] = UCHAR_MAX;
    }
}

// Conflict history.
static void memoryConflictHistory(uint16_t deaths, uint16_t kills) {
    vtype_t desc = {'\0'};

    if (deaths != 0u) {
        (void) sprintf(desc, "%d of the contributors to your monster memory %s", deaths, plural(deaths, "has", "have"));
        memoryPrint(desc);
        memoryPrint(" been killed by this creature, and ");
        if (kills == 0) {
            memoryPrint("it is not ever known to have been defeated.");
        } else {
            (void) sprintf(desc, "at least %d of the beasts %s been exterminated.", kills, plural(kills, "has", "have"));
            memoryPrint(desc);
        }
    } else if (kills != 0u) {
        (void) sprintf(desc, "At least %d of these creatures %s", kills, plural(kills, "has", "have"));
        memoryPrint(desc);
        memoryPrint(" been killed by contributors to your monster memory.");
    } else {
        memoryPrint("No known battles to the death are recalled.");
    }
}

// Immediately obvious.
static bool memoryDepthFoundAt(uint8_t level, uint16_t kills) {
    bool known = false;

    if (level == 0) {
        known = true;
        memoryPrint(" It lives in the town");
    } else if (kills != 0u) {
        known = true;

        // The Balrog is a level 100 monster, but appears at 50 feet.
        if (level > config::monsters::MON_ENDGAME_LEVEL) {
            level = config::monsters::MON_ENDGAME_LEVEL;
        }

        vtype_t desc = {'\0'};
        (void) sprintf(desc, " It is normally found at depths of %d feet", level * 50);
        memoryPrint(desc);
    }

    return known;
}

static bool memoryMovement(uint32_t rc_move, int monster_speed, bool is_known) {
    // the creatures_list speed value is 10 greater, so that it can be a uint8_t
    monster_speed -= 10;

    if ((rc_move & config::monsters::move::CM_ALL_MV_FLAGS) != 0u) {
        if (is_known) {
            memoryPrint(", and");
        } else {
            memoryPrint(" It");
            is_known = true;
        }

        memoryPrint(" moves");

        if ((rc_move & config::monsters::move::CM_RANDOM_MOVE) != 0u) {
            memoryPrint(recall_description_how_much[(rc_move & config::monsters::move::CM_RANDOM_MOVE) >> 3]);
            memoryPrint(" erratically");
        }

        if (monster_speed == 1) {
            memoryPrint(" at normal speed");
        } else {
            if ((rc_move & config::monsters::move::CM_RANDOM_MOVE) != 0u) {
                memoryPrint(", and");
            }

            if (monster_speed <= 0) {
                if (monster_speed == -1) {
                    memoryPrint(" very");
                } else if (monster_speed < -1) {
                    memoryPrint(" incredibly");
                }
                memoryPrint(" slowly");
            } else {
                if (monster_speed == 3) {
                    memoryPrint(" very");
                } else if (monster_speed > 3) {
                    memoryPrint(" unbelievably");
                }
                memoryPrint(" quickly");
            }
        }
    }

    if ((rc_move & config::monsters::move::CM_ATTACK_ONLY) != 0u) {
        if (is_known) {
            memoryPrint(", but");
        } else {
            memoryPrint(" It");
            is_known = true;
        }

        memoryPrint(" does not deign to chase intruders");
    }

    if ((rc_move & config::monsters::move::CM_ONLY_MAGIC) != 0u) {
        if (is_known) {
            memoryPrint(", but");
        } else {
            memoryPrint(" It");
            is_known = true;
        }

        memoryPrint(" always moves and attacks by using magic");
    }

    return is_known;
}

// Kill it once to know experience, and quality (evil, undead, monstrous).
// The quality of being a dragon is obvious.
static void memoryKillPoints(uint16_t creature_defense, uint16_t monster_exp, uint8_t level) {
    memoryPrint(" A kill of this");

    if ((creature_defense & config::monsters::defense::CD_ANIMAL) != 0) {
        memoryPrint(" natural");
    }
    if ((creature_defense & config::monsters::defense::CD_EVIL) != 0) {
        memoryPrint(" evil");
    }
    if ((creature_defense & config::monsters::defense::CD_UNDEAD) != 0) {
        memoryPrint(" undead");
    }

    // calculate the integer exp part, can be larger than 64K when first
    // level character looks at Balrog info, so must store in long
    int32_t quotient = (int32_t) monster_exp * level / py.misc.level;

    // calculate the fractional exp part scaled by 100,
    // must use long arithmetic to avoid overflow
    int remainder = (uint32_t)((((int32_t) monster_exp * level % py.misc.level) * (int32_t) 1000 / py.misc.level + 5) / 10);

    char plural;
    if (quotient == 1 && remainder == 0) {
        plural = '\0';
    } else {
        plural = 's';
    }

    vtype_t desc = {'\0'};
    (void) sprintf(desc, " creature is worth %d.%02d point%c", quotient, remainder, plural);
    memoryPrint(desc);

    const char *p, *q;

    if (py.misc.level / 10 == 1) {
        p = "th";
    } else {
        int ord = py.misc.level % 10;
        if (ord == 1) {
            p = "st";
        } else if (ord == 2) {
            p = "nd";
        } else if (ord == 3) {
            p = "rd";
        } else {
            p = "th";
        }
    }

    if (py.misc.level == 8 || py.misc.level == 11 || py.misc.level == 18) {
        q = "n";
    } else {
        q = "";
    }

    (void) sprintf(desc, " for a%s %d%s level character.", q, py.misc.level, p);
    memoryPrint(desc);
}

// Spells known, if have been used against us.
// Breath weapons or resistance might be known only because we cast spells at it.
static void memoryMagicSkills(uint32_t memory_spell_flags, uint32_t monster_spell_flags, uint32_t creature_spell_flags) {
    bool known = true;

    uint32_t spell_flags = memory_spell_flags;

    for (int i = 0; (spell_flags & config::monsters::spells::CS_BREATHE) != 0u; i++) {
        if ((spell_flags & (config::monsters::spells::CS_BR_LIGHT << i)) != 0u) {
            spell_flags &= ~(config::monsters::spells::CS_BR_LIGHT << i);

            if (known) {
                if ((monster_spell_flags & config::monsters::spells::CS_FREQ) != 0u) {
                    memoryPrint(" It can breathe ");
                } else {
                    memoryPrint(" It is resistant to ");
                }
                known = false;
            } else if ((spell_flags & config::monsters::spells::CS_BREATHE) != 0u) {
                memoryPrint(", ");
            } else {
                memoryPrint(" and ");
            }
            memoryPrint(recall_description_breath[i]);
        }
    }

    known = true;

    for (int i = 0; (spell_flags & config::monsters::spells::CS_SPELLS) != 0u; i++) {
        if ((spell_flags & (config::monsters::spells::CS_TEL_SHORT << i)) != 0u) {
            spell_flags &= ~(config::monsters::spells::CS_TEL_SHORT << i);

            if (known) {
                if ((memory_spell_flags & config::monsters::spells::CS_BREATHE) != 0u) {
                    memoryPrint(", and is also");
                } else {
                    memoryPrint(" It is");
                }
                memoryPrint(" magical, casting spells which ");
                known = false;
            } else if ((spell_flags & config::monsters::spells::CS_SPELLS) != 0u) {
                memoryPrint(", ");
            } else {
                memoryPrint(" or ");
            }
            memoryPrint(recall_description_spell[i]);
        }
    }

    if ((memory_spell_flags & (config::monsters::spells::CS_BREATHE | config::monsters::spells::CS_SPELLS)) != 0u) {
        // Could offset by level
        if ((monster_spell_flags & config::monsters::spells::CS_FREQ) > 5) {
            vtype_t temp = {'\0'};
            (void) sprintf(temp, "; 1 time in %d", creature_spell_flags & config::monsters::spells::CS_FREQ);
            memoryPrint(temp);
        }
        memoryPrint(".");
    }
}

// Do we know how hard they are to kill? Armor class, hit die.
static void memoryKillDifficulty(Creature_t const &creature, uint32_t monster_kills) {
    // the higher the level of the monster, the fewer the kills you need
    // Original knowarmor macro inlined
    if (monster_kills <= 304u / (4u + creature.level)) {
        return;
    }

    vtype_t description = {'\0'};

    (void) sprintf(description, " It has an armor rating of %d", creature.ac);
    memoryPrint(description);

    (void) sprintf(description,                                                                           //
                   " and a%s life rating of %dd%d.",                                                      //
                   ((creature.defenses & config::monsters::defense::CD_MAX_HP) != 0 ? " maximized" : ""), //
                   creature.hit_die.dice,                                                                 //
                   creature.hit_die.sides                                                                 //
    );
    memoryPrint(description);
}

// Do we know how clever they are? Special abilities.
static void memorySpecialAbilities(uint32_t move) {
    bool known = true;

    for (int i = 0; (move & config::monsters::move::CM_SPECIAL) != 0u; i++) {
        if ((move & (config::monsters::move::CM_INVISIBLE << i)) != 0u) {
            move &= ~(config::monsters::move::CM_INVISIBLE << i);

            if (known) {
                memoryPrint(" It can ");
                known = false;
            } else if ((move & config::monsters::move::CM_SPECIAL) != 0u) {
                memoryPrint(", ");
            } else {
                memoryPrint(" and ");
            }
            memoryPrint(recall_description_move[i]);
        }
    }

    if (!known) {
        memoryPrint(".");
    }
}

// Do we know its special weaknesses? Most defenses flags.
static void memoryWeaknesses(uint32_t defense) {
    bool known = true;

    for (int i = 0; (defense & config::monsters::defense::CD_WEAKNESS) != 0u; i++) {
        if ((defense & (config::monsters::defense::CD_FROST << i)) != 0u) {
            defense &= ~(config::monsters::defense::CD_FROST << i);
            if (known) {
                memoryPrint(" It is susceptible to ");
                known = false;
            } else if ((defense & config::monsters::defense::CD_WEAKNESS) != 0u) {
                memoryPrint(", ");
            } else {
                memoryPrint(" and ");
            }
            memoryPrint(recall_description_weakness[i]);
        }
    }

    if (!known) {
        memoryPrint(".");
    }
}

// Do we know how aware it is?
static void memoryAwareness(Creature_t const &creature, Recall_t const &memory) {
    if (memory.wake * memory.wake > creature.sleep_counter || memory.ignore == UCHAR_MAX || (creature.sleep_counter == 0 && memory.kills >= 10)) {
        memoryPrint(" It ");

        if (creature.sleep_counter > 200) {
            memoryPrint("prefers to ignore");
        } else if (creature.sleep_counter > 95) {
            memoryPrint("pays very little attention to");
        } else if (creature.sleep_counter > 75) {
            memoryPrint("pays little attention to");
        } else if (creature.sleep_counter > 45) {
            memoryPrint("tends to overlook");
        } else if (creature.sleep_counter > 25) {
            memoryPrint("takes quite a while to see");
        } else if (creature.sleep_counter > 10) {
            memoryPrint("takes a while to see");
        } else if (creature.sleep_counter > 5) {
            memoryPrint("is fairly observant of");
        } else if (creature.sleep_counter > 3) {
            memoryPrint("is observant of");
        } else if (creature.sleep_counter > 1) {
            memoryPrint("is very observant of");
        } else if (creature.sleep_counter != 0) {
            memoryPrint("is vigilant for");
        } else {
            memoryPrint("is ever vigilant for");
        }

        vtype_t text = {'\0'};
        (void) sprintf(text, " intruders, which it may notice from %d feet.", 10 * creature.area_affect_radius);
        memoryPrint(text);
    }
}

// Do we know what it might carry?
static void memoryLootCarried(uint32_t creature_move, uint32_t memory_move) {
    if ((memory_move & (config::monsters::move::CM_CARRY_OBJ | config::monsters::move::CM_CARRY_GOLD)) == 0u) {
        return;
    }

    memoryPrint(" It may");

    auto carrying_chance = (uint32_t)((memory_move & config::monsters::move::CM_TREASURE) >> config::monsters::move::CM_TR_SHIFT);

    if (carrying_chance == 1) {
        if ((creature_move & config::monsters::move::CM_TREASURE) == config::monsters::move::CM_60_RANDOM) {
            memoryPrint(" sometimes");
        } else {
            memoryPrint(" often");
        }
    } else if (carrying_chance == 2 && (creature_move & config::monsters::move::CM_TREASURE) == (config::monsters::move::CM_60_RANDOM | config::monsters::move::CM_90_RANDOM)) {
        memoryPrint(" often");
    }

    memoryPrint(" carry");

    const char *p = nullptr;

    if ((memory_move & config::monsters::move::CM_SMALL_OBJ) != 0u) {
        p = " small objects";
    } else {
        p = " objects";
    }

    if (carrying_chance == 1) {
        if ((memory_move & config::monsters::move::CM_SMALL_OBJ) != 0u) {
            p = " a small object";
        } else {
            p = " an object";
        }
    } else if (carrying_chance == 2) {
        memoryPrint(" one or two");
    } else {
        vtype_t msg = {'\0'};
        (void) sprintf(msg, " up to %d", carrying_chance);
        memoryPrint(msg);
    }

    if ((memory_move & config::monsters::move::CM_CARRY_OBJ) != 0u) {
        memoryPrint(p);
        if ((memory_move & config::monsters::move::CM_CARRY_GOLD) != 0u) {
            memoryPrint(" or treasure");
            if (carrying_chance > 1) {
                memoryPrint("s");
            }
        }
        memoryPrint(".");
    } else if (carrying_chance != 1) {
        memoryPrint(" treasures.");
    } else {
        memoryPrint(" treasure.");
    }
}

static void memoryAttackNumberAndDamage(Recall_t const &memory, Creature_t const &creature) {
    // We know about attacks it has used on us, and maybe the damage they do.
    // known_attacks is the total number of known attacks, used for punctuation
    int known_attacks = 0;

    for (uint8_t attack : memory.attacks) {
        if (attack != 0u) {
            known_attacks++;
        }
    }

    // attack_count counts the attacks as printed, used for punctuation
    int attack_count = 0;
    for (int i = 0; i < MON_MAX_ATTACKS; i++) {
        int attack_id = creature.damage[i];
        if (attack_id == 0) {
            break;
        }

        // don't print out unknown attacks
        if (memory.attacks[i] == 0u) {
            continue;
        }

        uint8_t attack_type = monster_attacks[attack_id].type_id;
        uint8_t attack_description_id = monster_attacks[attack_id].description_id;
        Dice_t dice = monster_attacks[attack_id].dice;

        attack_count++;

        if (attack_count == 1) {
            memoryPrint(" It can ");
        } else if (attack_count == known_attacks) {
            memoryPrint(", and ");
        } else {
            memoryPrint(", ");
        }

        if (attack_description_id > 19) {
            attack_description_id = 0;
        }

        memoryPrint(recall_description_attack_method[attack_description_id]);

        if (attack_type != 1 || (dice.dice > 0 && dice.sides > 0)) {
            memoryPrint(" to ");

            if (attack_type > 24) {
                attack_type = 0;
            }

            memoryPrint(recall_description_attack_type[attack_type]);

            if ((dice.dice != 0) && (dice.sides != 0)) {
                if (knowdamage(creature.level, memory.attacks[i], dice.dice * dice.sides)) {
                    // Loss of experience
                    if (attack_type == 19) {
                        memoryPrint(" by");
                    } else {
                        memoryPrint(" with damage");
                    }

                    vtype_t msg = {'\0'};
                    (void) sprintf(msg, " %dd%d", dice.dice, dice.sides);
                    memoryPrint(msg);
                }
            }
        }
    }

    if (attack_count != 0) {
        memoryPrint(".");
    } else if (known_attacks > 0 && memory.attacks[0] >= 10) {
        memoryPrint(" It has no physical attacks.");
    } else {
        memoryPrint(" Nothing is known about its attack.");
    }
}

// Print out what we have discovered about this monster.
int memoryRecall(int monster_id) {
    Recall_t &memory = creature_recall[monster_id];
    Creature_t const &creature = creatures_list[monster_id];

    Recall_t saved_memory{};

    if (game.wizard_mode) {
        saved_memory = memory;
        memoryWizardModeInit(memory, creature);
    }

    roff_print_line = 0;
    roff_buffer_pointer = roff_buffer;

    auto spells = (uint32_t)(memory.spells & creature.spells & ~config::monsters::spells::CS_FREQ);

    // the config::monsters::move::CM_WIN property is always known, set it if a win monster
    auto move = (uint32_t)(memory.movement | (creature.movement & config::monsters::move::CM_WIN));

    uint16_t defense = memory.defenses & creature.defenses;

    bool known;

    // Start the paragraph for the core monster description
    vtype_t msg = {'\0'};
    (void) sprintf(msg, "The %s:\n", creature.name);
    memoryPrint(msg);

    memoryConflictHistory(memory.deaths, memory.kills);
    known = memoryDepthFoundAt(creature.level, memory.kills);
    known = memoryMovement(move, creature.speed, known);

    // Finish off the paragraph with a period!
    if (known) {
        memoryPrint(".");
    }

    if (memory.kills != 0u) {
        memoryKillPoints(creature.defenses, creature.kill_exp_value, creature.level);
    }

    memoryMagicSkills(spells, memory.spells, creature.spells);

    memoryKillDifficulty(creature, memory.kills);

    memorySpecialAbilities(move);

    memoryWeaknesses(defense);

    if ((defense & config::monsters::defense::CD_INFRA) != 0) {
        memoryPrint(" It is warm blooded");
    }

    if ((defense & config::monsters::defense::CD_NO_SLEEP) != 0) {
        if ((defense & config::monsters::defense::CD_INFRA) != 0) {
            memoryPrint(", and");
        } else {
            memoryPrint(" It");
        }
        memoryPrint(" cannot be charmed or slept");
    }

    if ((defense & (config::monsters::defense::CD_NO_SLEEP | config::monsters::defense::CD_INFRA)) != 0) {
        memoryPrint(".");
    }

    memoryAwareness(creature, memory);

    memoryLootCarried(creature.movement, move);

    memoryAttackNumberAndDamage(memory, creature);

    // Always know the win creature.
    if ((creature.movement & config::monsters::move::CM_WIN) != 0u) {
        memoryPrint(" Killing one of these wins the game!");
    }

    memoryPrint("\n");
    putStringClearToEOL("--pause--", Coord_t{roff_print_line, 0});

    if (game.wizard_mode) {
        memory = saved_memory;
    }

    return getKeyInput();
}

// Allow access to monster memory. -CJS-
void recallMonsterAttributes(char command) {
    int n = 0;

    for (int i = MON_MAX_CREATURES - 1; i >= 0; i--) {
        if (creatures_list[i].sprite == command && memoryMonsterKnown(creature_recall[i])) {
            if (n == 0) {
                int confirmed = getInputConfirmationWithAbort(40, "You recall those details?");
                if (confirmed != 1) {
                    break;
                }

                eraseLine(Coord_t{0, 40});
                terminalSaveScreen();
            }
            n++;

            int query = (char) memoryRecall(i);
            terminalRestoreScreen();
            if (query == ESCAPE) {
                break;
            }
        }
    }
}
