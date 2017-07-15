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

static void wizardModeInit(Recall_t *mp, Creature_t *cp) {
    mp->r_kills = MAX_SHORT;
    mp->r_wake = mp->r_ignore = MAX_UCHAR;

    int i = (uint32_t) (
            (((cp->cmove & CM_4D2_OBJ) != 0) * 8) +
            (((cp->cmove & CM_2D2_OBJ) != 0) * 4) +
            (((cp->cmove & CM_1D2_OBJ) != 0) * 2) +
            ((cp->cmove & CM_90_RANDOM) != 0) +
            ((cp->cmove & CM_60_RANDOM) != 0)
    );

    mp->r_cmove = (uint32_t) ((cp->cmove & ~CM_TREASURE) | (i << CM_TR_SHIFT));
    mp->r_cdefense = cp->cdefense;

    if (cp->spells & CS_FREQ) {
        mp->r_spells = (uint32_t) (cp->spells | CS_FREQ);
    } else {
        mp->r_spells = cp->spells;
    }

    uint8_t *pu = cp->damage;

    int attackID = 0;
    while (*pu != 0 && attackID < 4) {
        mp->r_attacks[attackID] = MAX_UCHAR;
        attackID++;
        pu++;
    }

    // A little hack to enable the display of info for Quylthulgs.
    if (mp->r_cmove & CM_ONLY_MAGIC) {
        mp->r_attacks[0] = MAX_UCHAR;
    }
}

// Conflict history.
static void conflictHistory(uint16_t r_deaths, uint16_t r_kills) {
    vtype_t desc;

    if (r_deaths) {
        (void) sprintf(desc, "%d of the contributors to your monster memory %s", r_deaths, plural(r_deaths, "has", "have"));
        roff(desc);
        roff(" been killed by this creature, and ");
        if (r_kills == 0) {
            roff("it is not ever known to have been defeated.");
        } else {
            (void) sprintf(desc, "at least %d of the beasts %s been exterminated.", r_kills, plural(r_kills, "has", "have"));
            roff(desc);
        }
    } else if (r_kills) {
        (void) sprintf(desc, "At least %d of these creatures %s", r_kills, plural(r_kills, "has", "have"));
        roff(desc);
        roff(" been killed by contributors to your monster memory.");
    } else {
        roff("No known battles to the death are recalled.");
    }
}

// Immediately obvious.
static bool depthFoundAt(uint8_t level, uint16_t r_kills) {
    bool known = false;

    if (level == 0) {
        known = true;
        roff(" It lives in the town");
    } else if (r_kills) {
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

static bool movement(uint32_t rcmove, int mspeed, bool known) {
    // the creatures_list speed value is 10 greater, so that it can be a uint8_t
    mspeed -= 10;

    if (rcmove & CM_ALL_MV_FLAGS) {
        if (known) {
            roff(", and");
        } else {
            roff(" It");
            known = true;
        }

        roff(" moves");

        if (rcmove & CM_RANDOM_MOVE) {
            roff(description_how_much[(rcmove & CM_RANDOM_MOVE) >> 3]);
            roff(" erratically");
        }

        if (mspeed == 1) {
            roff(" at normal speed");
        } else {
            if (rcmove & CM_RANDOM_MOVE) {
                roff(", and");
            }

            if (mspeed <= 0) {
                if (mspeed == -1) {
                    roff(" very");
                } else if (mspeed < -1) {
                    roff(" incredibly");
                }
                roff(" slowly");
            } else {
                if (mspeed == 3) {
                    roff(" very");
                } else if (mspeed > 3) {
                    roff(" unbelievably");
                }
                roff(" quickly");
            }
        }
    }

    if (rcmove & CM_ATTACK_ONLY) {
        if (known) {
            roff(", but");
        } else {
            roff(" It");
            known = true;
        }

        roff(" does not deign to chase intruders");
    }

    if (rcmove & CM_ONLY_MAGIC) {
        if (known) {
            roff(", but");
        } else {
            roff(" It");
            known = true;
        }

        roff(" always moves and attacks by using magic");
    }

    return known;
}

// Kill it once to know experience, and quality (evil, undead, monstrous).
// The quality of being a dragon is obvious.
static void killPoints(uint16_t cdefense, uint16_t mexp, uint8_t level) {
    roff(" A kill of this");

    if (cdefense & CD_ANIMAL) {
        roff(" natural");
    }
    if (cdefense & CD_EVIL) {
        roff(" evil");
    }
    if (cdefense & CD_UNDEAD) {
        roff(" undead");
    }

    // calculate the integer exp part, can be larger than 64K when first
    // level character looks at Balrog info, so must store in long
    int32_t quotient = (int32_t) mexp * level / py.misc.lev;

    // calculate the fractional exp part scaled by 100,
    // must use long arithmetic to avoid overflow
    int remainder = (uint32_t) ((((int32_t) mexp * level % py.misc.lev) * (int32_t) 1000 / py.misc.lev + 5) / 10);

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
static void magicSkills(uint32_t rspells, uint32_t mp_r_spells, uint32_t cp_spells) {
    bool known = true;

    uint32_t spell = rspells;

    for (int i = 0; spell & CS_BREATHE; i++) {
        if (spell & (CS_BR_LIGHT << i)) {
            spell &= ~(CS_BR_LIGHT << i);

            if (known) {
                if (mp_r_spells & CS_FREQ) {
                    roff(" It can breathe ");
                } else {
                    roff(" It is resistant to ");
                }
                known = false;
            } else if (spell & CS_BREATHE) {
                roff(", ");
            } else {
                roff(" and ");
            }
            roff(description_breath[i]);
        }
    }

    known = true;

    for (int i = 0; spell & CS_SPELLS; i++) {
        if (spell & (CS_TEL_SHORT << i)) {
            spell &= ~(CS_TEL_SHORT << i);

            if (known) {
                if (rspells & CS_BREATHE) {
                    roff(", and is also");
                } else {
                    roff(" It is");
                }
                roff(" magical, casting spells which ");
                known = false;
            } else if (spell & CS_SPELLS) {
                roff(", ");
            } else {
                roff(" or ");
            }
            roff(description_spell[i]);
        }
    }

    if (rspells & (CS_BREATHE | CS_SPELLS)) {
        // Could offset by level
        if ((mp_r_spells & CS_FREQ) > 5) {
            vtype_t temp;
            (void) sprintf(temp, "; 1 time in %ld", cp_spells & CS_FREQ);
            roff(temp);
        }
        roff(".");
    }
}

// Do we know how hard they are to kill? Armor class, hit die.
static void killDifficulty(Creature_t *cp, uint32_t mp_r_kills) {
    if (!knowarmor(cp->level, mp_r_kills)) {
        return;
    }

    vtype_t temp;

    (void) sprintf(temp, " It has an armor rating of %d", cp->ac);
    roff(temp);

    (void) sprintf(temp, " and a%s life rating of %dd%d.", ((cp->cdefense & CD_MAX_HP) ? " maximized" : ""), cp->hd[0], cp->hd[1]);
    roff(temp);
}

// Do we know how clever they are? Special abilities.
static void specialAbilities(uint32_t rcmove) {
    bool known = true;

    for (int i = 0; rcmove & CM_SPECIAL; i++) {
        if (rcmove & (CM_INVISIBLE << i)) {
            rcmove &= ~(CM_INVISIBLE << i);

            if (known) {
                roff(" It can ");
                known = false;
            } else if (rcmove & CM_SPECIAL) {
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
static void weaknesses(uint32_t rcdefense) {
    bool known = true;

    for (int i = 0; rcdefense & CD_WEAKNESS; i++) {
        if (rcdefense & (CD_FROST << i)) {
            rcdefense &= ~(CD_FROST << i);
            if (known) {
                roff(" It is susceptible to ");
                known = false;
            } else if (rcdefense & CD_WEAKNESS) {
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
static void awareness(Creature_t *cp, Recall_t *mp) {
    if (mp->r_wake * mp->r_wake > cp->sleep || mp->r_ignore == MAX_UCHAR || (cp->sleep == 0 && mp->r_kills >= 10)) {
        roff(" It ");

        if (cp->sleep > 200) {
            roff("prefers to ignore");
        } else if (cp->sleep > 95) {
            roff("pays very little attention to");
        } else if (cp->sleep > 75) {
            roff("pays little attention to");
        } else if (cp->sleep > 45) {
            roff("tends to overlook");
        } else if (cp->sleep > 25) {
            roff("takes quite a while to see");
        } else if (cp->sleep > 10) {
            roff("takes a while to see");
        } else if (cp->sleep > 5) {
            roff("is fairly observant of");
        } else if (cp->sleep > 3) {
            roff("is observant of");
        } else if (cp->sleep > 1) {
            roff("is very observant of");
        } else if (cp->sleep != 0) {
            roff("is vigilant for");
        } else {
            roff("is ever vigilant for");
        }

        vtype_t text;
        (void) sprintf(text, " intruders, which it may notice from %d feet.", 10 * cp->aaf);
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
        wizardModeInit(mp, cp);
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

    conflictHistory(mp->r_deaths, mp->r_kills);
    known = depthFoundAt(cp->level, mp->r_kills);
    known = movement(rcmove, cp->speed, known);

    // Finish off the paragraph with a period!
    if (known) {
        roff(".");
    }

    if (mp->r_kills) {
        killPoints(cp->cdefense, cp->mexp, cp->level);
    }

    magicSkills(rspells, mp->r_spells, cp->spells);

    killDifficulty(cp, mp->r_kills);

    specialAbilities(rcmove);

    weaknesses(rcdefense);

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

    awareness(cp, mp);

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
