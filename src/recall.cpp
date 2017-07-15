// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Print out monster memory info -CJS-

#include "headers.h"
#include "externs.h"

static void roff(const char *);

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

// the higher the level of the monster, the fewer the kills you need
#define knowarmor(l, d) ((d) > 304 / (4 + (l)))

// the higher the level of the monster, the fewer the attacks you need,
// the more damage an attack does, the more attacks you need.
#define knowdamage(l, a, d) ((4 + (l)) * (a) > 80 * (d))

// Do we know anything about this monster?
bool memoryMonsterKnown(int monster_id) {
    if (wizard_mode) {
        return true;
    }

    Recall_t *memory = &creature_recall[monster_id];

    if (memory->r_cmove || memory->r_cdefense || memory->r_kills || memory->r_spells || memory->r_deaths) {
        return true;
    }

    for (int i = 0; i < 4; i++) {
        if (memory->r_attacks[i]) {
            return true;
        }
    }

    return false;
}

static void memoryWizardModeInit(Recall_t *memory, Creature_t *creature) {
    memory->r_kills = MAX_SHORT;
    memory->r_wake = memory->r_ignore = MAX_UCHAR;

    uint32_t move = (uint32_t) ((creature->cmove & CM_4D2_OBJ) != 0) * 8;
    move += (uint32_t) ((creature->cmove & CM_2D2_OBJ) != 0) * 4;
    move += (uint32_t) ((creature->cmove & CM_1D2_OBJ) != 0) * 2;
    move += (uint32_t) ((creature->cmove & CM_90_RANDOM) != 0);
    move += (uint32_t) ((creature->cmove & CM_60_RANDOM) != 0);

    memory->r_cmove = (uint32_t) ((creature->cmove & ~CM_TREASURE) | (move << CM_TR_SHIFT));
    memory->r_cdefense = creature->cdefense;

    if (creature->spells & CS_FREQ) {
        memory->r_spells = (uint32_t) (creature->spells | CS_FREQ);
    } else {
        memory->r_spells = creature->spells;
    }

    uint8_t *pu = creature->damage;

    int attack_id = 0;
    while (*pu != 0 && attack_id < 4) {
        memory->r_attacks[attack_id] = MAX_UCHAR;
        attack_id++;
        pu++;
    }

    // A little hack to enable the display of info for Quylthulgs.
    if (memory->r_cmove & CM_ONLY_MAGIC) {
        memory->r_attacks[0] = MAX_UCHAR;
    }
}

// Conflict history.
static void memoryConflictHistory(uint16_t deaths, uint16_t kills) {
    vtype_t desc;

    if (deaths) {
        (void) sprintf(desc, "%d of the contributors to your monster memory %s", deaths, plural(deaths, "has", "have"));
        roff(desc);
        roff(" been killed by this creature, and ");
        if (kills == 0) {
            roff("it is not ever known to have been defeated.");
        } else {
            (void) sprintf(desc, "at least %d of the beasts %s been exterminated.", kills, plural(kills, "has", "have"));
            roff(desc);
        }
    } else if (kills) {
        (void) sprintf(desc, "At least %d of these creatures %s", kills, plural(kills, "has", "have"));
        roff(desc);
        roff(" been killed by contributors to your monster memory.");
    } else {
        roff("No known battles to the death are recalled.");
    }
}

// Immediately obvious.
static bool memoryDepthFoundAt(uint8_t level, uint16_t kills) {
    bool known = false;

    if (level == 0) {
        known = true;
        roff(" It lives in the town");
    } else if (kills) {
        known = true;

        // The Balrog is a level 100 monster, but appears at 50 feet.
        if (level > WIN_MON_APPEAR) {
            level = WIN_MON_APPEAR;
        }

        vtype_t desc;
        (void) sprintf(desc, " It is normally found at depths of %d feet", level * 50);
        roff(desc);
    }

    return known;
}

static bool memoryMovement(uint32_t rc_move, int monster_speed, bool is_known) {
    // the creatures_list speed value is 10 greater, so that it can be a uint8_t
    monster_speed -= 10;

    if (rc_move & CM_ALL_MV_FLAGS) {
        if (is_known) {
            roff(", and");
        } else {
            roff(" It");
            is_known = true;
        }

        roff(" moves");

        if (rc_move & CM_RANDOM_MOVE) {
            roff(description_how_much[(rc_move & CM_RANDOM_MOVE) >> 3]);
            roff(" erratically");
        }

        if (monster_speed == 1) {
            roff(" at normal speed");
        } else {
            if (rc_move & CM_RANDOM_MOVE) {
                roff(", and");
            }

            if (monster_speed <= 0) {
                if (monster_speed == -1) {
                    roff(" very");
                } else if (monster_speed < -1) {
                    roff(" incredibly");
                }
                roff(" slowly");
            } else {
                if (monster_speed == 3) {
                    roff(" very");
                } else if (monster_speed > 3) {
                    roff(" unbelievably");
                }
                roff(" quickly");
            }
        }
    }

    if (rc_move & CM_ATTACK_ONLY) {
        if (is_known) {
            roff(", but");
        } else {
            roff(" It");
            is_known = true;
        }

        roff(" does not deign to chase intruders");
    }

    if (rc_move & CM_ONLY_MAGIC) {
        if (is_known) {
            roff(", but");
        } else {
            roff(" It");
            is_known = true;
        }

        roff(" always moves and attacks by using magic");
    }

    return is_known;
}

// Kill it once to know experience, and quality (evil, undead, monstrous).
// The quality of being a dragon is obvious.
static void memoryKillPoints(uint16_t creature_defense, uint16_t monster_exp, uint8_t level) {
    roff(" A kill of this");

    if (creature_defense & CD_ANIMAL) {
        roff(" natural");
    }
    if (creature_defense & CD_EVIL) {
        roff(" evil");
    }
    if (creature_defense & CD_UNDEAD) {
        roff(" undead");
    }

    // calculate the integer exp part, can be larger than 64K when first
    // level character looks at Balrog info, so must store in long
    int32_t quotient = (int32_t) monster_exp * level / py.misc.lev;

    // calculate the fractional exp part scaled by 100,
    // must use long arithmetic to avoid overflow
    int remainder = (uint32_t) ((((int32_t) monster_exp * level % py.misc.lev) * (int32_t) 1000 / py.misc.lev + 5) / 10);

    char plural;
    if (quotient == 1 && remainder == 0) {
        plural = '\0';
    } else {
        plural = 's';
    }

    vtype_t desc;
    (void) sprintf(desc, " creature is worth %d.%02d point%c", quotient, remainder, plural);
    roff(desc);

    const char *p, *q;

    if (py.misc.lev / 10 == 1) {
        p = "th";
    } else {
        int ord = py.misc.lev % 10;
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

    if (py.misc.lev == 8 || py.misc.lev == 11 || py.misc.lev == 18) {
        q = "n";
    } else {
        q = "";
    }

    (void) sprintf(desc, " for a%s %d%s level character.", q, py.misc.lev, p);
    roff(desc);
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
                    roff(" It can breathe ");
                } else {
                    roff(" It is resistant to ");
                }
                known = false;
            } else if (spell_flags & CS_BREATHE) {
                roff(", ");
            } else {
                roff(" and ");
            }
            roff(description_breath[i]);
        }
    }

    known = true;

    for (int i = 0; spell_flags & CS_SPELLS; i++) {
        if (spell_flags & (CS_TEL_SHORT << i)) {
            spell_flags &= ~(CS_TEL_SHORT << i);

            if (known) {
                if (memory_spell_flags & CS_BREATHE) {
                    roff(", and is also");
                } else {
                    roff(" It is");
                }
                roff(" magical, casting spells which ");
                known = false;
            } else if (spell_flags & CS_SPELLS) {
                roff(", ");
            } else {
                roff(" or ");
            }
            roff(description_spell[i]);
        }
    }

    if (memory_spell_flags & (CS_BREATHE | CS_SPELLS)) {
        // Could offset by level
        if ((monster_spell_flags & CS_FREQ) > 5) {
            vtype_t temp;
            (void) sprintf(temp, "; 1 time in %ld", creature_spell_flags & CS_FREQ);
            roff(temp);
        }
        roff(".");
    }
}

// Do we know how hard they are to kill? Armor class, hit die.
static void memoryKillDifficulty(Creature_t *creature, uint32_t monster_kills) {
    if (!knowarmor(creature->level, monster_kills)) {
        return;
    }

    vtype_t description;

    (void) sprintf(description, " It has an armor rating of %d", creature->ac);
    roff(description);

    (void) sprintf(description, " and a%s life rating of %dd%d.", ((creature->cdefense & CD_MAX_HP) ? " maximized" : ""), creature->hd[0], creature->hd[1]);
    roff(description);
}

// Do we know how clever they are? Special abilities.
static void memorySpecialAbilities(uint32_t move) {
    bool known = true;

    for (int i = 0; move & CM_SPECIAL; i++) {
        if (move & (CM_INVISIBLE << i)) {
            move &= ~(CM_INVISIBLE << i);

            if (known) {
                roff(" It can ");
                known = false;
            } else if (move & CM_SPECIAL) {
                roff(", ");
            } else {
                roff(" and ");
            }
            roff(description_move[i]);
        }
    }

    if (!known) {
        roff(".");
    }
}

// Do we know its special weaknesses? Most cdefense flags.
static void memoryWeaknesses(uint32_t defense) {
    bool known = true;

    for (int i = 0; defense & CD_WEAKNESS; i++) {
        if (defense & (CD_FROST << i)) {
            defense &= ~(CD_FROST << i);
            if (known) {
                roff(" It is susceptible to ");
                known = false;
            } else if (defense & CD_WEAKNESS) {
                roff(", ");
            } else {
                roff(" and ");
            }
            roff(description_weakness[i]);
        }
    }

    if (!known) {
        roff(".");
    }
}

// Do we know how aware it is?
static void memoryAwareness(Creature_t *creature, Recall_t *memory) {
    if (memory->r_wake * memory->r_wake > creature->sleep || memory->r_ignore == MAX_UCHAR || (creature->sleep == 0 && memory->r_kills >= 10)) {
        roff(" It ");

        if (creature->sleep > 200) {
            roff("prefers to ignore");
        } else if (creature->sleep > 95) {
            roff("pays very little attention to");
        } else if (creature->sleep > 75) {
            roff("pays little attention to");
        } else if (creature->sleep > 45) {
            roff("tends to overlook");
        } else if (creature->sleep > 25) {
            roff("takes quite a while to see");
        } else if (creature->sleep > 10) {
            roff("takes a while to see");
        } else if (creature->sleep > 5) {
            roff("is fairly observant of");
        } else if (creature->sleep > 3) {
            roff("is observant of");
        } else if (creature->sleep > 1) {
            roff("is very observant of");
        } else if (creature->sleep != 0) {
            roff("is vigilant for");
        } else {
            roff("is ever vigilant for");
        }

        vtype_t text;
        (void) sprintf(text, " intruders, which it may notice from %d feet.", 10 * creature->aaf);
        roff(text);
    }
}

// Do we know what it might carry?
static void lootCarried(uint32_t cp_cmove, uint32_t rcmove) {
    if (!(rcmove & (CM_CARRY_OBJ | CM_CARRY_GOLD))) {
        return;
    }

    roff(" It may");

    uint32_t carryingChance = (uint32_t) ((rcmove & CM_TREASURE) >> CM_TR_SHIFT);

    if (carryingChance == 1) {
        if ((cp_cmove & CM_TREASURE) == CM_60_RANDOM) {
            roff(" sometimes");
        } else {
            roff(" often");
        }
    } else if (carryingChance == 2 && (cp_cmove & CM_TREASURE) == (CM_60_RANDOM | CM_90_RANDOM)) {
        roff(" often");
    }

    roff(" carry");

    const char *p;

    if (rcmove & CM_SMALL_OBJ) {
        p = " small objects";
    } else {
        p = " objects";
    }

    if (carryingChance == 1) {
        if (rcmove & CM_SMALL_OBJ) {
            p = " a small object";
        } else {
            p = " an object";
        }
    } else if (carryingChance == 2) {
        roff(" one or two");
    } else {
        vtype_t temp;
        (void) sprintf(temp, " up to %d", carryingChance);
        roff(temp);
    }

    if (rcmove & CM_CARRY_OBJ) {
        roff(p);
        if (rcmove & CM_CARRY_GOLD) {
            roff(" or treasure");
            if (carryingChance > 1) {
                roff("s");
            }
        }
        roff(".");
    } else if (carryingChance != 1) {
        roff(" treasures.");
    } else {
        roff(" treasure.");
    }
}

static void attackNumberAndDamage(Recall_t *mp, Creature_t *cp) {
    // We know about attacks it has used on us, and maybe the damage they do.
    // known_attacks is the total number of known attacks, used for punctuation
    int known_attacks = 0;

    for (int id = 0; id < 4; id++) {
        if (mp->r_attacks[id]) {
            known_attacks++;
        }
    }

    // attackCount counts the attacks as printed, used for punctuation
    int attackCount = 0;

    uint8_t *pu = cp->damage;

    for (int i = 0; *pu != 0 && i < 4; pu++, i++) {
        int att_type, att_how, d1, d2;

        // don't print out unknown attacks
        if (!mp->r_attacks[i]) {
            continue;
        }

        att_type = monster_attacks[*pu].attack_type;
        att_how = monster_attacks[*pu].attack_desc;
        d1 = monster_attacks[*pu].attack_dice;
        d2 = monster_attacks[*pu].attack_sides;

        attackCount++;

        if (attackCount == 1) {
            roff(" It can ");
        } else if (attackCount == known_attacks) {
            roff(", and ");
        } else {
            roff(", ");
        }

        if (att_how > 19) {
            att_how = 0;
        }

        roff(description_attack_method[att_how]);

        if (att_type != 1 || (d1 > 0 && d2 > 0)) {
            roff(" to ");

            if (att_type > 24) {
                att_type = 0;
            }

            roff(description_attack_type[att_type]);

            if (d1 && d2) {
                if (knowdamage(cp->level, mp->r_attacks[i], d1 * d2)) {
                    // Loss of experience
                    if (att_type == 19) {
                        roff(" by");
                    } else {
                        roff(" with damage");
                    }

                    vtype_t temp;
                    (void) sprintf(temp, " %dd%d", d1, d2);
                    roff(temp);
                }
            }
        }
    }

    if (attackCount) {
        roff(".");
    } else if (known_attacks > 0 && mp->r_attacks[0] >= 10) {
        roff(" It has no physical attacks.");
    } else {
        roff(" Nothing is known about its attack.");
    }
}

// Print out what we have discovered about this monster.
int roff_recall(int monster_id) {
    Recall_t *mp = &creature_recall[monster_id];
    Creature_t *cp = &creatures_list[monster_id];

    Recall_t save_mem;

    if (wizard_mode) {
        save_mem = *mp;
        memoryWizardModeInit(mp, cp);
    }

    roff_print_line = 0;
    roff_buffer_pointer = roff_buffer;

    uint32_t rspells = (uint32_t) (mp->r_spells & cp->spells & ~CS_FREQ);

    // the CM_WIN property is always known, set it if a win monster
    uint32_t rcmove = (uint32_t) (mp->r_cmove | (CM_WIN & cp->cmove));

    uint16_t rcdefense = mp->r_cdefense & cp->cdefense;

    bool known;

    // Start the paragraph for the core monster description
    vtype_t temp;
    (void) sprintf(temp, "The %s:\n", cp->name);
    roff(temp);

    memoryConflictHistory(mp->r_deaths, mp->r_kills);
    known = memoryDepthFoundAt(cp->level, mp->r_kills);
    known = memoryMovement(rcmove, cp->speed, known);

    // Finish off the paragraph with a period!
    if (known) {
        roff(".");
    }

    if (mp->r_kills) {
        memoryKillPoints(cp->cdefense, cp->mexp, cp->level);
    }

    memoryMagicSkills(rspells, mp->r_spells, cp->spells);

    memoryKillDifficulty(cp, mp->r_kills);

    memorySpecialAbilities(rcmove);

    memoryWeaknesses(rcdefense);

    if (rcdefense & CD_INFRA) {
        roff(" It is warm blooded");
    }

    if (rcdefense & CD_NO_SLEEP) {
        if (rcdefense & CD_INFRA) {
            roff(", and");
        } else {
            roff(" It");
        }
        roff(" cannot be charmed or slept");
    }

    if (rcdefense & (CD_NO_SLEEP | CD_INFRA)) {
        roff(".");
    }

    memoryAwareness(cp, mp);

    lootCarried(cp->cmove, rcmove);

    attackNumberAndDamage(mp, cp);

    // Always know the win creature.
    if (cp->cmove & CM_WIN) {
        roff(" Killing one of these wins the game!");
    }

    roff("\n");
    putStringClearToEOL("--pause--", roff_print_line, 0);

    if (wizard_mode) {
        *mp = save_mem;
    }

    return getKeyInput();
}

// Print out strings, filling up lines as we go.
static void roff(const char *p) {
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
