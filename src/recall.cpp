// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Print out monster memory info -CJS-

#include "headers.h"
#include "externs.h"

static void memoryPrint(const char *);

static const char *description_attack_type[] = {
        "do something undefined",
        "attack",
        "weaken",
        "confuse",
        "terrify",
        "shoot flames",
        "shoot acid",
        "freeze",
        "shoot lightning",
        "corrode",
        "blind",
        "paralyse",
        "steal money",
        "steal things",
        "poison",
        "reduce dexterity",
        "reduce constitution",
        "drain intelligence",
        "drain wisdom",
        "lower experience",
        "call for help",
        "disenchant",
        "eat your food",
        "absorb light",
        "absorb charges",
};

static const char *description_attack_method[] = {
        "make an undefined advance",
        "hit",
        "bite",
        "claw",
        "sting",
        "touch",
        "kick",
        "gaze",
        "breathe",
        "spit",
        "wail",
        "embrace",
        "crawl on you",
        "release spores",
        "beg",
        "slime you",
        "crush",
        "trample",
        "drool",
        "insult",
};

static const char *description_how_much[] = {
        " not at all",
        " a bit",
        "",
        " quite",
        " very",
        " most",
        " highly",
        " extremely",
};

static const char *description_move[] = {
        "move invisibly",
        "open doors",
        "pass through walls",
        "kill weaker creatures",
        "pick up objects",
        "breed explosively",
};

static const char *description_spell[] = {
        "teleport short distances",
        "teleport long distances",
        "teleport its prey",
        "cause light wounds",
        "cause serious wounds",
        "paralyse its prey",
        "induce blindness",
        "confuse",
        "terrify",
        "summon a monster",
        "summon the undead",
        "slow its prey",
        "drain mana",
        "unknown 1",
        "unknown 2",
};

static const char *description_breath[] = {
        "lightning",
        "poison gases",
        "acid",
        "frost",
        "fire",
};

static const char *description_weakness[] = {
        "frost",
        "fire",
        "poison",
        "acid",
        "bright light",
        "rock remover",
};

static vtype_t roff_buffer;       // Line buffer.
static char *roff_buffer_pointer; // Pointer into line buffer.
static int roff_print_line;       // Place to print line now being loaded.

#define plural(c, ss, sp) ((c) == 1 ? (ss) : (sp))

// Number of kills needed for information.

// the higher the level of the monster, the fewer the attacks you need,
// the more damage an attack does, the more attacks you need.
#define knowdamage(l, a, d) ((4 + (l)) * (a) > 80 * (d))

// Do we know anything about this monster?
bool memoryMonsterKnown(int monster_id) {
    if (wizard_mode) {
        return true;
    }

    Recall_t *memory = &creature_recall[monster_id];

    if (memory->movement || memory->defenses || memory->kills || memory->spells || memory->deaths) {
        return true;
    }

    for (int i = 0; i < 4; i++) {
        if (memory->attacks[i]) {
            return true;
        }
    }

    return false;
}

static void memoryWizardModeInit(Recall_t *memory, Creature_t *creature) {
    memory->kills = MAX_SHORT;
    memory->wake = memory->ignore = MAX_UCHAR;

    uint32_t move = (uint32_t) ((creature->cmove & CM_4D2_OBJ) != 0) * 8;
    move += (uint32_t) ((creature->cmove & CM_2D2_OBJ) != 0) * 4;
    move += (uint32_t) ((creature->cmove & CM_1D2_OBJ) != 0) * 2;
    move += (uint32_t) ((creature->cmove & CM_90_RANDOM) != 0);
    move += (uint32_t) ((creature->cmove & CM_60_RANDOM) != 0);

    memory->movement = (uint32_t) ((creature->cmove & ~CM_TREASURE) | (move << CM_TR_SHIFT));
    memory->defenses = creature->cdefense;

    if (creature->spells & CS_FREQ) {
        memory->spells = (uint32_t) (creature->spells | CS_FREQ);
    } else {
        memory->spells = creature->spells;
    }

    uint8_t *pu = creature->damage;

    int attack_id = 0;
    while (*pu != 0 && attack_id < 4) {
        memory->attacks[attack_id] = MAX_UCHAR;
        attack_id++;
        pu++;
    }

    // A little hack to enable the display of info for Quylthulgs.
    if (memory->movement & CM_ONLY_MAGIC) {
        memory->attacks[0] = MAX_UCHAR;
    }
}

// Conflict history.
static void memoryConflictHistory(uint16_t deaths, uint16_t kills) {
    vtype_t desc;

    if (deaths) {
        (void) sprintf(desc, "%d of the contributors to your monster memory %s", deaths, plural(deaths, "has", "have"));
        memoryPrint(desc);
        memoryPrint(" been killed by this creature, and ");
        if (kills == 0) {
            memoryPrint("it is not ever known to have been defeated.");
        } else {
            (void) sprintf(desc, "at least %d of the beasts %s been exterminated.", kills, plural(kills, "has", "have"));
            memoryPrint(desc);
        }
    } else if (kills) {
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
    } else if (kills) {
        known = true;

        // The Balrog is a level 100 monster, but appears at 50 feet.
        if (level > MON_ENDGAME_LEVEL) {
            level = MON_ENDGAME_LEVEL;
        }

        vtype_t desc;
        (void) sprintf(desc, " It is normally found at depths of %d feet", level * 50);
        memoryPrint(desc);
    }

    return known;
}

static bool memoryMovement(uint32_t rc_move, int monster_speed, bool is_known) {
    // the creatures_list speed value is 10 greater, so that it can be a uint8_t
    monster_speed -= 10;

    if (rc_move & CM_ALL_MV_FLAGS) {
        if (is_known) {
            memoryPrint(", and");
        } else {
            memoryPrint(" It");
            is_known = true;
        }

        memoryPrint(" moves");

        if (rc_move & CM_RANDOM_MOVE) {
            memoryPrint(description_how_much[(rc_move & CM_RANDOM_MOVE) >> 3]);
            memoryPrint(" erratically");
        }

        if (monster_speed == 1) {
            memoryPrint(" at normal speed");
        } else {
            if (rc_move & CM_RANDOM_MOVE) {
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

    if (rc_move & CM_ATTACK_ONLY) {
        if (is_known) {
            memoryPrint(", but");
        } else {
            memoryPrint(" It");
            is_known = true;
        }

        memoryPrint(" does not deign to chase intruders");
    }

    if (rc_move & CM_ONLY_MAGIC) {
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

    if (creature_defense & CD_ANIMAL) {
        memoryPrint(" natural");
    }
    if (creature_defense & CD_EVIL) {
        memoryPrint(" evil");
    }
    if (creature_defense & CD_UNDEAD) {
        memoryPrint(" undead");
    }

    // calculate the integer exp part, can be larger than 64K when first
    // level character looks at Balrog info, so must store in long
    int32_t quotient = (int32_t) monster_exp * level / py.misc.level;

    // calculate the fractional exp part scaled by 100,
    // must use long arithmetic to avoid overflow
    int remainder = (uint32_t) ((((int32_t) monster_exp * level % py.misc.level) * (int32_t) 1000 / py.misc.level + 5) / 10);

    char plural;
    if (quotient == 1 && remainder == 0) {
        plural = '\0';
    } else {
        plural = 's';
    }

    vtype_t desc;
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

    for (int i = 0; spell_flags & CS_BREATHE; i++) {
        if (spell_flags & (CS_BR_LIGHT << i)) {
            spell_flags &= ~(CS_BR_LIGHT << i);

            if (known) {
                if (monster_spell_flags & CS_FREQ) {
                    memoryPrint(" It can breathe ");
                } else {
                    memoryPrint(" It is resistant to ");
                }
                known = false;
            } else if (spell_flags & CS_BREATHE) {
                memoryPrint(", ");
            } else {
                memoryPrint(" and ");
            }
            memoryPrint(description_breath[i]);
        }
    }

    known = true;

    for (int i = 0; spell_flags & CS_SPELLS; i++) {
        if (spell_flags & (CS_TEL_SHORT << i)) {
            spell_flags &= ~(CS_TEL_SHORT << i);

            if (known) {
                if (memory_spell_flags & CS_BREATHE) {
                    memoryPrint(", and is also");
                } else {
                    memoryPrint(" It is");
                }
                memoryPrint(" magical, casting spells which ");
                known = false;
            } else if (spell_flags & CS_SPELLS) {
                memoryPrint(", ");
            } else {
                memoryPrint(" or ");
            }
            memoryPrint(description_spell[i]);
        }
    }

    if (memory_spell_flags & (CS_BREATHE | CS_SPELLS)) {
        // Could offset by level
        if ((monster_spell_flags & CS_FREQ) > 5) {
            vtype_t temp;
            (void) sprintf(temp, "; 1 time in %d", creature_spell_flags & CS_FREQ);
            memoryPrint(temp);
        }
        memoryPrint(".");
    }
}

// Do we know how hard they are to kill? Armor class, hit die.
static void memoryKillDifficulty(Creature_t *creature, uint32_t monster_kills) {
    // the higher the level of the monster, the fewer the kills you need
    // Original knowarmor macro inlined
        if (monster_kills <= 304u / (4u + creature->level)) {
        return;
    }

    vtype_t description;

    (void) sprintf(description, " It has an armor rating of %d", creature->ac);
    memoryPrint(description);

    (void) sprintf(description, " and a%s life rating of %dd%d.", ((creature->cdefense & CD_MAX_HP) ? " maximized" : ""), creature->hd[0], creature->hd[1]);
    memoryPrint(description);
}

// Do we know how clever they are? Special abilities.
static void memorySpecialAbilities(uint32_t move) {
    bool known = true;

    for (int i = 0; move & CM_SPECIAL; i++) {
        if (move & (CM_INVISIBLE << i)) {
            move &= ~(CM_INVISIBLE << i);

            if (known) {
                memoryPrint(" It can ");
                known = false;
            } else if (move & CM_SPECIAL) {
                memoryPrint(", ");
            } else {
                memoryPrint(" and ");
            }
            memoryPrint(description_move[i]);
        }
    }

    if (!known) {
        memoryPrint(".");
    }
}

// Do we know its special weaknesses? Most cdefense flags.
static void memoryWeaknesses(uint32_t defense) {
    bool known = true;

    for (int i = 0; defense & CD_WEAKNESS; i++) {
        if (defense & (CD_FROST << i)) {
            defense &= ~(CD_FROST << i);
            if (known) {
                memoryPrint(" It is susceptible to ");
                known = false;
            } else if (defense & CD_WEAKNESS) {
                memoryPrint(", ");
            } else {
                memoryPrint(" and ");
            }
            memoryPrint(description_weakness[i]);
        }
    }

    if (!known) {
        memoryPrint(".");
    }
}

// Do we know how aware it is?
static void memoryAwareness(Creature_t *creature, Recall_t *memory) {
    if (memory->wake * memory->wake > creature->sleep || memory->ignore == MAX_UCHAR || (creature->sleep == 0 && memory->kills >= 10)) {
        memoryPrint(" It ");

        if (creature->sleep > 200) {
            memoryPrint("prefers to ignore");
        } else if (creature->sleep > 95) {
            memoryPrint("pays very little attention to");
        } else if (creature->sleep > 75) {
            memoryPrint("pays little attention to");
        } else if (creature->sleep > 45) {
            memoryPrint("tends to overlook");
        } else if (creature->sleep > 25) {
            memoryPrint("takes quite a while to see");
        } else if (creature->sleep > 10) {
            memoryPrint("takes a while to see");
        } else if (creature->sleep > 5) {
            memoryPrint("is fairly observant of");
        } else if (creature->sleep > 3) {
            memoryPrint("is observant of");
        } else if (creature->sleep > 1) {
            memoryPrint("is very observant of");
        } else if (creature->sleep != 0) {
            memoryPrint("is vigilant for");
        } else {
            memoryPrint("is ever vigilant for");
        }

        vtype_t text;
        (void) sprintf(text, " intruders, which it may notice from %d feet.", 10 * creature->aaf);
        memoryPrint(text);
    }
}

// Do we know what it might carry?
static void memoryLootCarried(uint32_t creature_move, uint32_t memory_move) {
    if (!(memory_move & (CM_CARRY_OBJ | CM_CARRY_GOLD))) {
        return;
    }

    memoryPrint(" It may");

    uint32_t carrying_chance = (uint32_t) ((memory_move & CM_TREASURE) >> CM_TR_SHIFT);

    if (carrying_chance == 1) {
        if ((creature_move & CM_TREASURE) == CM_60_RANDOM) {
            memoryPrint(" sometimes");
        } else {
            memoryPrint(" often");
        }
    } else if (carrying_chance == 2 && (creature_move & CM_TREASURE) == (CM_60_RANDOM | CM_90_RANDOM)) {
        memoryPrint(" often");
    }

    memoryPrint(" carry");

    const char *p;

    if (memory_move & CM_SMALL_OBJ) {
        p = " small objects";
    } else {
        p = " objects";
    }

    if (carrying_chance == 1) {
        if (memory_move & CM_SMALL_OBJ) {
            p = " a small object";
        } else {
            p = " an object";
        }
    } else if (carrying_chance == 2) {
        memoryPrint(" one or two");
    } else {
        vtype_t msg;
        (void) sprintf(msg, " up to %d", carrying_chance);
        memoryPrint(msg);
    }

    if (memory_move & CM_CARRY_OBJ) {
        memoryPrint(p);
        if (memory_move & CM_CARRY_GOLD) {
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

static void memoryAttackNumberAndDamage(Recall_t *memory, Creature_t *creature) {
    // We know about attacks it has used on us, and maybe the damage they do.
    // known_attacks is the total number of known attacks, used for punctuation
    int known_attacks = 0;

    for (int id = 0; id < 4; id++) {
        if (memory->attacks[id]) {
            known_attacks++;
        }
    }

    // attack_count counts the attacks as printed, used for punctuation
    int attack_count = 0;

    uint8_t *pu = creature->damage;

    for (int i = 0; *pu != 0 && i < 4; pu++, i++) {
        int attack_type, attack_description_id;
        int attack_dice, attack_sides;

        // don't print out unknown attacks
        if (!memory->attacks[i]) {
            continue;
        }

        attack_type = monster_attacks[*pu].attack_type;
        attack_description_id = monster_attacks[*pu].attack_desc;
        attack_dice = monster_attacks[*pu].attack_dice;
        attack_sides = monster_attacks[*pu].attack_sides;

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

        memoryPrint(description_attack_method[attack_description_id]);

        if (attack_type != 1 || (attack_dice > 0 && attack_sides > 0)) {
            memoryPrint(" to ");

            if (attack_type > 24) {
                attack_type = 0;
            }

            memoryPrint(description_attack_type[attack_type]);

            if (attack_dice && attack_sides) {
                if (knowdamage(creature->level, memory->attacks[i], attack_dice * attack_sides)) {
                    // Loss of experience
                    if (attack_type == 19) {
                        memoryPrint(" by");
                    } else {
                        memoryPrint(" with damage");
                    }

                    vtype_t msg;
                    (void) sprintf(msg, " %dd%d", attack_dice, attack_sides);
                    memoryPrint(msg);
                }
            }
        }
    }

    if (attack_count) {
        memoryPrint(".");
    } else if (known_attacks > 0 && memory->attacks[0] >= 10) {
        memoryPrint(" It has no physical attacks.");
    } else {
        memoryPrint(" Nothing is known about its attack.");
    }
}

// Print out what we have discovered about this monster.
int memoryRecall(int monster_id) {
    Recall_t *memory = &creature_recall[monster_id];
    Creature_t *creature = &creatures_list[monster_id];

    Recall_t saved_memory;

    if (wizard_mode) {
        saved_memory = *memory;
        memoryWizardModeInit(memory, creature);
    }

    roff_print_line = 0;
    roff_buffer_pointer = roff_buffer;

    uint32_t spells = (uint32_t) (memory->spells & creature->spells & ~CS_FREQ);

    // the CM_WIN property is always known, set it if a win monster
    uint32_t move = (uint32_t) (memory->movement | (CM_WIN & creature->cmove));

    uint16_t defense = memory->defenses & creature->cdefense;

    bool known;

    // Start the paragraph for the core monster description
    vtype_t msg;
    (void) sprintf(msg, "The %s:\n", creature->name);
    memoryPrint(msg);

    memoryConflictHistory(memory->deaths, memory->kills);
    known = memoryDepthFoundAt(creature->level, memory->kills);
    known = memoryMovement(move, creature->speed, known);

    // Finish off the paragraph with a period!
    if (known) {
        memoryPrint(".");
    }

    if (memory->kills) {
        memoryKillPoints(creature->cdefense, creature->mexp, creature->level);
    }

    memoryMagicSkills(spells, memory->spells, creature->spells);

    memoryKillDifficulty(creature, memory->kills);

    memorySpecialAbilities(move);

    memoryWeaknesses(defense);

    if (defense & CD_INFRA) {
        memoryPrint(" It is warm blooded");
    }

    if (defense & CD_NO_SLEEP) {
        if (defense & CD_INFRA) {
            memoryPrint(", and");
        } else {
            memoryPrint(" It");
        }
        memoryPrint(" cannot be charmed or slept");
    }

    if (defense & (CD_NO_SLEEP | CD_INFRA)) {
        memoryPrint(".");
    }

    memoryAwareness(creature, memory);

    memoryLootCarried(creature->cmove, move);

    memoryAttackNumberAndDamage(memory, creature);

    // Always know the win creature.
    if (creature->cmove & CM_WIN) {
        memoryPrint(" Killing one of these wins the game!");
    }

    memoryPrint("\n");
    putStringClearToEOL("--pause--", roff_print_line, 0);

    if (wizard_mode) {
        *memory = saved_memory;
    }

    return getKeyInput();
}

// Print out strings, filling up lines as we go.
static void memoryPrint(const char *p) {
    while (*p) {
        *roff_buffer_pointer = *p;

        if (*p == '\n' || roff_buffer_pointer >= roff_buffer + sizeof(roff_buffer) - 1) {
            char *q = roff_buffer_pointer;
            if (*p != '\n') {
                while (*q != ' ') {
                    q--;
                }
            }
            *q = 0;
            putStringClearToEOL(roff_buffer, roff_print_line, 0);
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
