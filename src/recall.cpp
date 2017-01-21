// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Print out monster memory info -CJS-

#include "headers.h"
#include "externs.h"

static void roff(const char *);

static const char *desc_atype[] = {
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

static const char *desc_amethod[] = {
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

static const char *desc_howmuch[] = {
    " not at all",
    " a bit",
    "",
    " quite",
    " very",
    " most",
    " highly",
    " extremely",
};

static const char *desc_move[] = {
    "move invisibly",
    "open doors",
    "pass through walls",
    "kill weaker creatures",
    "pick up objects",
    "breed explosively",
};

static const char *desc_spell[] = {
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

static const char *desc_breath[] = {
    "lightning", "poison gases", "acid", "frost", "fire",
};

static const char *desc_weakness[] = {
    "frost", "fire", "poison", "acid", "bright light", "rock remover",
};

static vtype roffbuf; // Line buffer.
static char *roffp;   // Pointer into line buffer.
static int roffpline; // Place to print line now being loaded.

#define plural(c, ss, sp) ((c) == 1 ? (ss) : (sp))

// Number of kills needed for information.

// the higher the level of the monster, the fewer the kills you need
#define knowarmor(l, d) ((d) > 304 / (4 + (l)))

// the higher the level of the monster, the fewer the attacks you need,
// the more damage an attack does, the more attacks you need.
#define knowdamage(l, a, d) ((4 + (l)) * (a) > 80 * (d))

// Do we know anything about this monster?
bool bool_roff_recall(int mon_num) {
    if (wizard) {
        return true;
    }

    recall_type *mp = &c_recall[mon_num];
    if (mp->r_cmove || mp->r_cdefense || mp->r_kills || mp->r_spells || mp->r_deaths) {
        return true;
    }
    for (int i = 0; i < 4; i++) {
        if (mp->r_attacks[i]) {
            return true;
        }
    }
    return false;
}

// Print out what we have discovered about this monster.
int roff_recall(int mon_num) {
    bool known;
    const char *p, *q;
    uint8_t *pu;
    uint32_t j;
    vtype temp;

    recall_type *mp = &c_recall[mon_num];
    creature_type *cp = &c_list[mon_num];

    recall_type save_mem;

    if (wizard) {
        save_mem = *mp;
        mp->r_kills = MAX_SHORT;
        mp->r_wake = mp->r_ignore = MAX_UCHAR;

        j = (((cp->cmove & CM_4D2_OBJ) != 0) * 8) +
            (((cp->cmove & CM_2D2_OBJ) != 0) * 4) +
            (((cp->cmove & CM_1D2_OBJ) != 0) * 2) +
            ((cp->cmove & CM_90_RANDOM) != 0) +
            ((cp->cmove & CM_60_RANDOM) != 0);

        mp->r_cmove = (cp->cmove & ~CM_TREASURE) | (j << CM_TR_SHIFT);
        mp->r_cdefense = cp->cdefense;

        if (cp->spells & CS_FREQ) {
            mp->r_spells = cp->spells | CS_FREQ;
        } else {
            mp->r_spells = cp->spells;
        }

        j = 0;
        pu = cp->damage;
        while (*pu != 0 && j < 4) {
            // Turbo C needs a 16 bit int for the array index.
            mp->r_attacks[(int)j] = MAX_UCHAR;
            j++;
            pu++;
        }

        // A little hack to enable the display of info for Quylthulgs.
        if (mp->r_cmove & CM_ONLY_MAGIC) {
            mp->r_attacks[0] = MAX_UCHAR;
        }
    }
    roffpline = 0;
    roffp = roffbuf;

    uint32_t rspells = mp->r_spells & cp->spells & ~CS_FREQ;

    // the CM_WIN property is always known, set it if a win monster
    uint32_t rcmove = mp->r_cmove | (CM_WIN & cp->cmove);

    uint16_t rcdefense = mp->r_cdefense & cp->cdefense;

    (void)sprintf(temp, "The %s:\n", cp->name);
    roff(temp);

    // Conflict history.
    if (mp->r_deaths) {
        (void)sprintf(temp, "%d of the contributors to your monster memory %s", mp->r_deaths, plural(mp->r_deaths, "has", "have"));
        roff(temp);
        roff(" been killed by this creature, and ");
        if (mp->r_kills == 0) {
            roff("it is not ever known to have been defeated.");
        } else {
            (void)sprintf(temp, "at least %d of the beasts %s been exterminated.", mp->r_kills, plural(mp->r_kills, "has", "have"));
            roff(temp);
        }
    } else if (mp->r_kills) {
        (void)sprintf(temp, "At least %d of these creatures %s", mp->r_kills, plural(mp->r_kills, "has", "have"));
        roff(temp);
        roff(" been killed by contributors to your monster memory.");
    } else {
        roff("No known battles to the death are recalled.");
    }

    // Immediately obvious.
    known = false;
    if (cp->level == 0) {
        roff(" It lives in the town");
        known = true;
    } else if (mp->r_kills) {
        // The Balrog is a level 100 monster, but appears at 50 feet.
        int i = cp->level;
        if (i > WIN_MON_APPEAR) {
            i = WIN_MON_APPEAR;
        }
        (void)sprintf(temp, " It is normally found at depths of %d feet", i * 50);
        roff(temp);
        known = true;
    }

    // the c_list speed value is 10 greater, so that it can be a uint8_t
    int mspeed = cp->speed - 10;

    if (rcmove & CM_ALL_MV_FLAGS) {
        if (known) {
            roff(", and");
        } else {
            roff(" It");
            known = true;
        }
        roff(" moves");
        if (rcmove & CM_RANDOM_MOVE) {
            // Turbo C needs a 16 bit int for the array index.
            roff(desc_howmuch[(int)((rcmove & CM_RANDOM_MOVE) >> 3)]);
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

    if (known) {
        roff(".");
    }

    // Kill it once to know experience, and quality (evil, undead, monstrous).
    // The quality of being a dragon is obvious.
    if (mp->r_kills) {
        roff(" A kill of this");

        if (cp->cdefense & CD_ANIMAL) {
            roff(" natural");
        }

        if (cp->cdefense & CD_EVIL) {
            roff(" evil");
        }

        if (cp->cdefense & CD_UNDEAD) {
            roff(" undead");
        }

        // calculate the integer exp part, can be larger than 64K when first
        // level character looks at Balrog info, so must store in long
        int32_t templong = (int32_t)cp->mexp * cp->level / py.misc.lev;

        // calculate the fractional exp part scaled by 100,
        // must use long arithmetic to avoid overflow
        j = (((int32_t)cp->mexp * cp->level % py.misc.lev) * (int32_t)1000 / py.misc.lev + 5) / 10;

        (void)sprintf(temp, " creature is worth %d.%02d point%s", templong, j, (templong == 1 && j == 0 ? "" : "s"));
        roff(temp);

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

        int n = py.misc.lev;
        if (n == 8 || n == 11 || n == 18) {
            q = "n";
        } else {
            q = "";
        }
        (void)sprintf(temp, " for a%s %d%s level character.", q, n, p);
        roff(temp);
    }

    // Spells known, if have been used against us.
    // Breath weapons or resistance might be known only because we cast spells at it.
    known = true;
    j = rspells;

    for (int i = 0; j & CS_BREATHE; i++) {
        if (j & (CS_BR_LIGHT << i)) {
            j &= ~(CS_BR_LIGHT << i);

            if (known) {
                if (mp->r_spells & CS_FREQ) {
                    roff(" It can breathe ");
                } else {
                    roff(" It is resistant to ");
                }
                known = false;
            } else if (j & CS_BREATHE) {
                roff(", ");
            } else {
                roff(" and ");
            }
            roff(desc_breath[i]);
        }
    }

    known = true;

    for (int i = 0; j & CS_SPELLS; i++) {
        if (j & (CS_TEL_SHORT << i)) {
            j &= ~(CS_TEL_SHORT << i);

            if (known) {
                if (rspells & CS_BREATHE) {
                    roff(", and is also");
                } else {
                    roff(" It is");
                }
                roff(" magical, casting spells which ");
                known = false;
            } else if (j & CS_SPELLS) {
                roff(", ");
            } else {
                roff(" or ");
            }
            roff(desc_spell[i]);
        }
    }

    if (rspells & (CS_BREATHE | CS_SPELLS)) {
        // Could offset by level
        if ((mp->r_spells & CS_FREQ) > 5) {
            (void)sprintf(temp, "; 1 time in %ld", cp->spells & CS_FREQ);
            roff(temp);
        }
        roff(".");
    }

    // Do we know how hard they are to kill? Armor class, hit die.
    if (knowarmor(cp->level, mp->r_kills)) {
        (void)sprintf(temp, " It has an armor rating of %d", cp->ac);
        roff(temp);
        (void)sprintf(temp, " and a%s life rating of %dd%d.", ((cp->cdefense & CD_MAX_HP) ? " maximized" : ""), cp->hd[0], cp->hd[1]);
        roff(temp);
    }

    // Do we know how clever they are? Special abilities.
    known = true;
    j = rcmove;

    for (int i = 0; j & CM_SPECIAL; i++) {
        if (j & (CM_INVISIBLE << i)) {
            j &= ~(CM_INVISIBLE << i);

            if (known) {
                roff(" It can ");
                known = false;
            } else if (j & CM_SPECIAL) {
                roff(", ");
            } else {
                roff(" and ");
            }
            roff(desc_move[i]);
        }
    }

    if (!known) {
        roff(".");
    }

    // Do we know its special weaknesses? Most cdefense flags.
    known = true;
    j = rcdefense;
    for (int i = 0; j & CD_WEAKNESS; i++) {
        if (j & (CD_FROST << i)) {
            j &= ~(CD_FROST << i);
            if (known) {
                roff(" It is susceptible to ");
                known = false;
            } else if (j & CD_WEAKNESS) {
                roff(", ");
            } else {
                roff(" and ");
            }
            roff(desc_weakness[i]);
        }
    }

    if (!known) {
        roff(".");
    }

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

    // Do we know how aware it is?
    if (((mp->r_wake * mp->r_wake) > cp->sleep) || mp->r_ignore == MAX_UCHAR || (cp->sleep == 0 && mp->r_kills >= 10)) {
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
        (void)sprintf(temp, " intruders, which it may notice from %d feet.", 10 * cp->aaf);
        roff(temp);
    }

    // Do we know what it might carry?
    if (rcmove & (CM_CARRY_OBJ | CM_CARRY_GOLD)) {
        roff(" It may");
        j = (rcmove & CM_TREASURE) >> CM_TR_SHIFT;

        if (j == 1) {
            if ((cp->cmove & CM_TREASURE) == CM_60_RANDOM) {
                roff(" sometimes");
            } else {
                roff(" often");
            }
        } else if ((j == 2) && ((cp->cmove & CM_TREASURE) == (CM_60_RANDOM | CM_90_RANDOM))) {
            roff(" often");
        }

        roff(" carry");

        if (rcmove & CM_SMALL_OBJ) {
            p = " small objects";
        } else {
            p = " objects";
        }

        if (j == 1) {
            if (rcmove & CM_SMALL_OBJ) {
                p = " a small object";
            } else {
                p = " an object";
            }
        } else if (j == 2) {
            roff(" one or two");
        } else {
            (void)sprintf(temp, " up to %d", j);
            roff(temp);
        }

        if (rcmove & CM_CARRY_OBJ) {
            roff(p);
            if (rcmove & CM_CARRY_GOLD) {
                roff(" or treasure");
                if (j > 1) {
                    roff("s");
                }
            }
            roff(".");
        } else if (j != 1) {
            roff(" treasures.");
        } else {
            roff(" treasure.");
        }
    }

    // We know about attacks it has used on us, and maybe the damage they do.
    // known_attacks is the total number of known attacks, used for punctuation
    int known_attacks = 0;

    // Turbo C needs a 16 bit int for the array index.
    for (j = 0; j < 4; j++) {
        if (mp->r_attacks[(int)j]) {
            known_attacks++;
        }
    }

    pu = cp->damage;

    // j counts the attacks as printed, used for punctuation
    j = 0;
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

        j++;
        if (j == 1) {
            roff(" It can ");
        } else if (j == known_attacks) {
            roff(", and ");
        } else {
            roff(", ");
        }

        if (att_how > 19) {
            att_how = 0;
        }

        roff(desc_amethod[att_how]);

        if (att_type != 1 || (d1 > 0 && d2 > 0)) {
            roff(" to ");
            if (att_type > 24) {
                att_type = 0;
            }
            roff(desc_atype[att_type]);
            if (d1 && d2) {
                if (knowdamage(cp->level, mp->r_attacks[i], d1 * d2)) {
                    // Loss of experience
                    if (att_type == 19) {
                        roff(" by");
                    } else {
                        roff(" with damage");
                    }
                    (void)sprintf(temp, " %dd%d", d1, d2);
                    roff(temp);
                }
            }
        }
    }

    if (j) {
        roff(".");
    } else if (known_attacks > 0 && mp->r_attacks[0] >= 10) {
        roff(" It has no physical attacks.");
    } else {
        roff(" Nothing is known about its attack.");
    }

    // Always know the win creature.
    if (cp->cmove & CM_WIN) {
        roff(" Killing one of these wins the game!");
    }

    roff("\n");
    prt("--pause--", roffpline, 0);
    if (wizard) {
        *mp = save_mem;
    }

    return inkey();
}

// Print out strings, filling up lines as we go.
static void roff(const char *p) {
    while (*p) {
        *roffp = *p;
        if (*p == '\n' || roffp >= roffbuf + sizeof(roffbuf) - 1) {
            char *q = roffp;
            if (*p != '\n') {
                while (*q != ' ') {
                    q--;
                }
            }
            *q = 0;
            prt(roffbuf, roffpline, 0);
            roffpline++;

            char *r = roffbuf;
            while (q < roffp) {
                q++;
                *r = *q;
                r++;
            }
            roffp = r;
        } else {
            roffp++;
        }
        p++;
    }
}
