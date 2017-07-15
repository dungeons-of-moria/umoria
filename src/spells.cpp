// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Player/creature spells, breaths, wands, scrolls, etc. code

#include <string>

#include "headers.h"
#include "externs.h"

static void printMonsterActionText(std::string name, std::string action) {
    vtype_t msg;
    (void) sprintf(msg, "%s %s", name.c_str(), action.c_str());
    printMessage(msg);
}

// Following are spell procedure/functions -RAK-
// These routines are commonly used in the scroll, potion, wands, and
// staves routines, and are occasionally called from other areas.
// Now included are creature spells also.           -RAK

static void monster_name(char *description, bool monsterLit, const char *monsterName) {
    if (!monsterLit) {
        (void) strcpy(description, "It");
    } else {
        (void) sprintf(description, "The %s", monsterName);
    }
}

static void lower_monster_name(char *description, bool monsterLit, const char *monsterName) {
    if (!monsterLit) {
        (void) strcpy(description, "it");
    } else {
        (void) sprintf(description, "the %s", monsterName);
    }
}

// Sleep creatures adjacent to player -RAK-
bool sleep_monsters1(int y, int x) {
    bool asleep = false;

    for (int row = y - 1; row <= y + 1; row++) {
        for (int col = x - 1; col <= x + 1; col++) {
            if (cave[row][col].cptr <= 1) {
                continue;
            }

            Monster_t *m_ptr = &monsters[cave[row][col].cptr];
            Creature_t *r_ptr = &creatures_list[m_ptr->mptr];

            vtype_t name;
            monster_name(name, m_ptr->ml, r_ptr->name);

            if (randomNumber(MAX_MONS_LEVEL) < r_ptr->level || (CD_NO_SLEEP & r_ptr->cdefense)) {
                if (m_ptr->ml && (r_ptr->cdefense & CD_NO_SLEEP)) {
                    creature_recall[m_ptr->mptr].r_cdefense |= CD_NO_SLEEP;
                }

                printMonsterActionText(name, "is unaffected.");
            } else {
                m_ptr->csleep = 500;
                asleep = true;

                printMonsterActionText(name, "falls asleep.");
            }
        }
    }

    return asleep;
}

// Detect any treasure on the current panel -RAK-
bool detect_treasure() {
    bool detected = false;

    for (int y = panel_row_min; y <= panel_row_max; y++) {
        for (int x = panel_col_min; x <= panel_col_max; x++) {
            Cave_t *c_ptr = &cave[y][x];

            if (c_ptr->tptr != 0 && treasure_list[c_ptr->tptr].tval == TV_GOLD && !caveTileVisible(y, x)) {
                c_ptr->fm = true;
                dungeonLiteSpot(y, x);

                detected = true;
            }
        }
    }

    return detected;
}

// Detect all objects on the current panel -RAK-
bool detect_object() {
    bool detected = false;

    for (int y = panel_row_min; y <= panel_row_max; y++) {
        for (int x = panel_col_min; x <= panel_col_max; x++) {
            Cave_t *c_ptr = &cave[y][x];

            if (c_ptr->tptr != 0 && treasure_list[c_ptr->tptr].tval < TV_MAX_OBJECT && !caveTileVisible(y, x)) {
                c_ptr->fm = true;
                dungeonLiteSpot(y, x);

                detected = true;
            }
        }
    }

    return detected;
}

// Locates and displays traps on current panel -RAK-
bool detect_trap() {
    bool detected = false;

    for (int y = panel_row_min; y <= panel_row_max; y++) {
        for (int x = panel_col_min; x <= panel_col_max; x++) {
            Cave_t *c_ptr = &cave[y][x];

            if (c_ptr->tptr == 0) {
                continue;
            }

            if (treasure_list[c_ptr->tptr].tval == TV_INVIS_TRAP) {
                c_ptr->fm = true;
                dungeonChangeTrapVisibility(y, x);

                detected = true;
            } else if (treasure_list[c_ptr->tptr].tval == TV_CHEST) {
                Inventory_t *t_ptr = &treasure_list[c_ptr->tptr];
                spellItemIdentifyAndRemoveRandomInscription(t_ptr);
            }
        }
    }

    return detected;
}

// Locates and displays all secret doors on current panel -RAK-
bool detect_sdoor() {
    bool detected = false;

    for (int y = panel_row_min; y <= panel_row_max; y++) {
        for (int x = panel_col_min; x <= panel_col_max; x++) {
            Cave_t *c_ptr = &cave[y][x];

            if (c_ptr->tptr == 0) {
                continue;
            }

            if (treasure_list[c_ptr->tptr].tval == TV_SECRET_DOOR) {
                // Secret doors

                c_ptr->fm = true;
                dungeonChangeTrapVisibility(y, x);

                detected = true;
            } else if ((treasure_list[c_ptr->tptr].tval == TV_UP_STAIR || treasure_list[c_ptr->tptr].tval == TV_DOWN_STAIR) && !c_ptr->fm) {
                // Staircases

                c_ptr->fm = true;
                dungeonLiteSpot(y, x);

                detected = true;
            }
        }
    }

    return detected;
}

// Locates and displays all invisible creatures on current panel -RAK-
bool detect_invisible() {
    bool detected = false;

    for (int id = next_free_monster_id - 1; id >= MIN_MONIX; id--) {
        Monster_t *m_ptr = &monsters[id];

        if (coordInsidePanel((int) m_ptr->fy, (int) m_ptr->fx) && (CM_INVISIBLE & creatures_list[m_ptr->mptr].cmove)) {
            m_ptr->ml = true;

            // works correctly even if hallucinating
            putChar((char) creatures_list[m_ptr->mptr].cchar, (int) m_ptr->fy, (int) m_ptr->fx);

            detected = true;
        }
    }

    if (detected) {
        printMessage("You sense the presence of invisible creatures!");
        printMessage(CNIL);

        // must unlight every monster just lighted
        updateMonsters(false);
    }

    return detected;
}

// Light an area: -RAK-
//     1.  If corridor  light immediate area
//     2.  If room      light entire room plus immediate area.
bool light_area(int y, int x) {
    if (py.flags.blind < 1) {
        printMessage("You are surrounded by a white light.");
    }

    // NOTE: this is not changed anywhere. A bug or correct? -MRC-
    bool lit = true;

    if (cave[y][x].lr && current_dungeon_level > 0) {
        dungeonLightRoom(y, x);
    }

    // Must always light immediate area, because one might be standing on
    // the edge of a room, or next to a destroyed area, etc.
    for (int i = y - 1; i <= y + 1; i++) {
        for (int j = x - 1; j <= x + 1; j++) {
            cave[i][j].pl = true;
            dungeonLiteSpot(i, j);
        }
    }

    return lit;
}

// Darken an area, opposite of light area -RAK-
bool unlight_area(int y, int x) {
    bool darkened = false;

    if (cave[y][x].lr && current_dungeon_level > 0) {
        int tmp1 = (SCREEN_HEIGHT / 2);
        int tmp2 = (SCREEN_WIDTH / 2);
        int start_row = (y / tmp1) * tmp1 + 1;
        int start_col = (x / tmp2) * tmp2 + 1;
        int end_row = start_row + tmp1 - 1;
        int end_col = start_col + tmp2 - 1;

        for (int row = start_row; row <= end_row; row++) {
            for (int col = start_col; col <= end_col; col++) {
                Cave_t *c_ptr = &cave[row][col];
                if (c_ptr->lr && c_ptr->fval <= MAX_CAVE_FLOOR) {
                    c_ptr->pl = false;
                    c_ptr->fval = DARK_FLOOR;
                    dungeonLiteSpot(row, col);
                    if (!caveTileVisible(row, col)) {
                        darkened = true;
                    }
                }
            }
        }
    } else {
        for (int row = y - 1; row <= y + 1; row++) {
            for (int col = x - 1; col <= x + 1; col++) {
                Cave_t *c_ptr = &cave[row][col];
                if (c_ptr->fval == CORR_FLOOR && c_ptr->pl) {
                    // pl could have been set by star-lite wand, etc
                    c_ptr->pl = false;
                    darkened = true;
                }
            }
        }
    }

    if (darkened && py.flags.blind < 1) {
        printMessage("Darkness surrounds you.");
    }

    return darkened;
}

static void lightTileArea(int row, int col) {
    for (int y = row - 1; y <= row + 1; y++) {
        for (int x = col - 1; x <= col + 1; x++) {
            Cave_t *c_ptr = &cave[y][x];

            if (c_ptr->fval >= MIN_CAVE_WALL) {
                c_ptr->pl = true;
            } else if (c_ptr->tptr != 0 && treasure_list[c_ptr->tptr].tval >= TV_MIN_VISIBLE && treasure_list[c_ptr->tptr].tval <= TV_MAX_VISIBLE) {
                c_ptr->fm = true;
            }
        }
    }
}

// Map the current area plus some -RAK-
void map_area() {
    int rowMin = panel_row_min - randomNumber(10);
    int rowMax = panel_row_max + randomNumber(10);
    int colMin = panel_col_min - randomNumber(20);
    int colMax = panel_col_max + randomNumber(20);

    for (int row = rowMin; row <= rowMax; row++) {
        for (int col = colMin; col <= colMax; col++) {
            if (coordInBounds(row, col) && cave[row][col].fval <= MAX_CAVE_FLOOR) {
                lightTileArea(row, col);
            }
        }
    }

    drawDungeonPanel();
}

// Identify an object -RAK-
bool ident_spell() {
    int item_val;
    if (!inventoryGetInputForItemId(&item_val, "Item you wish identified?", 0, INVEN_ARRAY_SIZE, CNIL, CNIL)) {
        return false;
    }

    itemIdentify(&item_val);

    Inventory_t *i_ptr = &inventory[item_val];
    spellItemIdentifyAndRemoveRandomInscription(i_ptr);

    obj_desc_t tmp_str;
    itemDescription(tmp_str, i_ptr, true);

    obj_desc_t out_val;
    if (item_val >= INVEN_WIELD) {
        playerRecalculateBonuses();
        (void) sprintf(out_val, "%s: %s", playerItemWearingDescription(item_val), tmp_str);
    } else {
        (void) sprintf(out_val, "%c %s", item_val + 97, tmp_str);
    }
    printMessage(out_val);

    return true;
}

// Get all the monsters on the level pissed off. -RAK-
bool aggravate_monster(int distance_affect) {
    bool aggravated = false;

    for (int id = next_free_monster_id - 1; id >= MIN_MONIX; id--) {
        Monster_t *m_ptr = &monsters[id];
        m_ptr->csleep = 0;
        if (m_ptr->cdis <= distance_affect && m_ptr->cspeed < 2) {
            m_ptr->cspeed++;
            aggravated = true;
        }
    }

    if (aggravated) {
        printMessage("You hear a sudden stirring in the distance!");
    }

    return aggravated;
}

// Surround the fool with traps (chuckle) -RAK-
bool trap_creation() {
    // NOTE: this is not changed anywhere. A bug or correct? -MRC-
    bool trapped = true;

    for (int y = char_row - 1; y <= char_row + 1; y++) {
        for (int x = char_col - 1; x <= char_col + 1; x++) {
            // Don't put a trap under the player, since this can lead to
            // strange situations, e.g. falling through a trap door while
            // trying to rest, setting off a falling rock trap and ending
            // up under the rock.
            if (y == char_row && x == char_col) {
                continue;
            }

            Cave_t *c_ptr = &cave[y][x];

            if (c_ptr->fval <= MAX_CAVE_FLOOR) {
                if (c_ptr->tptr != 0) {
                    (void) dungeonDeleteObject(y, x);
                }
                dungeonSetTrap(y, x, randomNumber(MAX_TRAP) - 1);

                // don't let player gain exp from the newly created traps
                treasure_list[c_ptr->tptr].p1 = 0;

                // open pits are immediately visible, so call dungeonLiteSpot
                dungeonLiteSpot(y, x);
            }
        }
    }

    return trapped;
}

// Surround the player with doors. -RAK-
bool door_creation() {
    bool created = false;

    for (int y = char_row - 1; y <= char_row + 1; y++) {
        for (int x = char_col - 1; x <= char_col + 1; x++) {
            // Don't put a door under the player!
            if (y == char_row && x == char_col) {
                continue;
            }

            Cave_t *c_ptr = &cave[y][x];

            if (c_ptr->fval <= MAX_CAVE_FLOOR) {
                if (c_ptr->tptr != 0) {
                    (void) dungeonDeleteObject(y, x);
                }

                int k = popt();
                c_ptr->fval = BLOCKED_FLOOR;
                c_ptr->tptr = (uint8_t) k;
                inventoryItemCopyTo(OBJ_CLOSED_DOOR, &treasure_list[k]);
                dungeonLiteSpot(y, x);

                created = true;
            }
        }
    }

    return created;
}

// Destroys any adjacent door(s)/trap(s) -RAK-
bool td_destroy() {
    bool destroyed = false;

    for (int y = char_row - 1; y <= char_row + 1; y++) {
        for (int x = char_col - 1; x <= char_col + 1; x++) {
            Cave_t *c_ptr = &cave[y][x];

            if (c_ptr->tptr == 0) {
                continue;
            }

            if ((treasure_list[c_ptr->tptr].tval >= TV_INVIS_TRAP &&
                 treasure_list[c_ptr->tptr].tval <= TV_CLOSED_DOOR &&
                 treasure_list[c_ptr->tptr].tval != TV_RUBBLE) ||
                treasure_list[c_ptr->tptr].tval == TV_SECRET_DOOR) {
                if (dungeonDeleteObject(y, x)) {
                    destroyed = true;
                }
            } else if (treasure_list[c_ptr->tptr].tval == TV_CHEST && treasure_list[c_ptr->tptr].flags != 0) {
                // destroy traps on chest and unlock
                treasure_list[c_ptr->tptr].flags &= ~(CH_TRAPPED | CH_LOCKED);
                treasure_list[c_ptr->tptr].name2 = SN_UNLOCKED;

                destroyed = true;

                printMessage("You have disarmed the chest.");
                spellItemIdentifyAndRemoveRandomInscription(&treasure_list[c_ptr->tptr]);
            }
        }
    }

    return destroyed;
}

// Display all creatures on the current panel -RAK-
bool detect_monsters() {
    bool detected = false;

    for (int id = next_free_monster_id - 1; id >= MIN_MONIX; id--) {
        Monster_t *m_ptr = &monsters[id];

        if (coordInsidePanel((int) m_ptr->fy, (int) m_ptr->fx) && (CM_INVISIBLE & creatures_list[m_ptr->mptr].cmove) == 0) {
            m_ptr->ml = true;
            detected = true;

            // works correctly even if hallucinating
            putChar((char) creatures_list[m_ptr->mptr].cchar, (int) m_ptr->fy, (int) m_ptr->fx);
        }
    }

    if (detected) {
        printMessage("You sense the presence of monsters!");
        printMessage(CNIL);

        // must unlight every monster just lighted
        updateMonsters(false);
    }

    return detected;
}

// Update monster when light line spell touches it.
static void lightLineTouchesMonster(int monsterID) {
    Monster_t *monster = &monsters[monsterID];
    Creature_t *creature = &creatures_list[monster->mptr];

    // light up and draw monster
    monsterUpdateVisibility(monsterID);

    vtype_t name;
    monster_name(name, monster->ml, creature->name);

    if (CD_LIGHT & creature->cdefense) {
        if (monster->ml) {
            creature_recall[monster->mptr].r_cdefense |= CD_LIGHT;
        }

        if (mon_take_hit(monsterID, diceDamageRoll(2, 8) >= 0)) {
            printMonsterActionText(name, "shrivels away in the light!");
            displayCharacterExperience();
        } else {
            printMonsterActionText(name, "cringes from the light!");
        }
    }
}

// Leave a line of light in given dir, blue light can sometimes hurt creatures. -RAK-
void light_line(int x, int y, int direction) {
    int dist = 0;
    bool finished = false;

    while (!finished) {
        Cave_t *c_ptr = &cave[y][x];

        if (dist > OBJ_BOLT_RANGE || c_ptr->fval >= MIN_CLOSED_SPACE) {
            (void) playerMovePosition(direction, &y, &x);
            finished = true;
            continue; // we're done here, break out of the loop
        }

        if (!c_ptr->pl && !c_ptr->tl) {
            // set pl so that dungeonLiteSpot will work
            c_ptr->pl = true;

            if (c_ptr->fval == LIGHT_FLOOR) {
                if (coordInsidePanel(y, x)) {
                    dungeonLightRoom(y, x);
                }
            } else {
                dungeonLiteSpot(y, x);
            }
        }

        // set pl in case tl was true above
        c_ptr->pl = true;

        if (c_ptr->cptr > 1) {
            lightLineTouchesMonster((int) c_ptr->cptr);
        }

        // move must be at end because want to light up current spot
        (void) playerMovePosition(direction, &y, &x);
        dist++;
    }
}

// Light line in all directions -RAK-
void starlite(int y, int x) {
    if (py.flags.blind < 1) {
        printMessage("The end of the staff bursts into a blue shimmering light.");
    }

    for (int dir = 1; dir <= 9; dir++) {
        if (dir != 5) {
            light_line(x, y, dir);
        }
    }
}

// Disarms all traps/chests in a given direction -RAK-
bool disarm_all(int y, int x, int direction) {
    int dist = 0;
    bool disarmed = false;
    Cave_t *c_ptr;

    do {
        c_ptr = &cave[y][x];

        // note, must continue up to and including the first non open space,
        // because secret doors have fval greater than MAX_OPEN_SPACE
        if (c_ptr->tptr != 0) {
            Inventory_t *t_ptr = &treasure_list[c_ptr->tptr];

            if (t_ptr->tval == TV_INVIS_TRAP || t_ptr->tval == TV_VIS_TRAP) {
                if (dungeonDeleteObject(y, x)) {
                    disarmed = true;
                }
            } else if (t_ptr->tval == TV_CLOSED_DOOR) {
                // Locked or jammed doors become merely closed.
                t_ptr->p1 = 0;
            } else if (t_ptr->tval == TV_SECRET_DOOR) {
                c_ptr->fm = true;
                dungeonChangeTrapVisibility(y, x);

                disarmed = true;
            } else if (t_ptr->tval == TV_CHEST && t_ptr->flags != 0) {
                disarmed = true;

                printMessage("Click!");
                t_ptr->flags &= ~(CH_TRAPPED | CH_LOCKED);
                t_ptr->name2 = SN_UNLOCKED;
                spellItemIdentifyAndRemoveRandomInscription(t_ptr);
            }
        }

        // move must be at end because want to light up current spot
        (void) playerMovePosition(direction, &y, &x);
        dist++;
    } while (dist <= OBJ_BOLT_RANGE && c_ptr->fval <= MAX_OPEN_SPACE);

    return disarmed;
}

// Return flags for given type area affect -RAK-
static void get_flags(int typ, uint32_t *weapon_type, int *harm_type, bool (**destroy)(Inventory_t *)) {
    switch (typ) {
        case GF_MAGIC_MISSILE:
            *weapon_type = 0;
            *harm_type = 0;
            *destroy = set_null;
            break;
        case GF_LIGHTNING:
            *weapon_type = CS_BR_LIGHT;
            *harm_type = CD_LIGHT;
            *destroy = set_lightning_destroy;
            break;
        case GF_POISON_GAS:
            *weapon_type = CS_BR_GAS;
            *harm_type = CD_POISON;
            *destroy = set_null;
            break;
        case GF_ACID:
            *weapon_type = CS_BR_ACID;
            *harm_type = CD_ACID;
            *destroy = set_acid_destroy;
            break;
        case GF_FROST:
            *weapon_type = CS_BR_FROST;
            *harm_type = CD_FROST;
            *destroy = set_frost_destroy;
            break;
        case GF_FIRE:
            *weapon_type = CS_BR_FIRE;
            *harm_type = CD_FIRE;
            *destroy = set_fire_destroy;
            break;
        case GF_HOLY_ORB:
            *weapon_type = 0;
            *harm_type = CD_EVIL;
            *destroy = set_null;
            break;
        default:
            printMessage("ERROR in get_flags()\n");
    }
}

// Light up, draw, and check for monster damage when Fire Bolt touches it.
static void fireBoltTouchesMonster(Cave_t *tile, int dam, int harmType, uint32_t weaponID, std::string boltName) {
    Monster_t *monster = &monsters[tile->cptr];
    Creature_t *creature = &creatures_list[monster->mptr];

    // light up monster and draw monster, temporarily set
    // pl so that monsterUpdateVisibility() will work
    bool savedLitStatus = tile->pl;
    tile->pl = true;
    monsterUpdateVisibility((int) tile->cptr);
    tile->pl = savedLitStatus;

    // draw monster and clear previous bolt
    putQIO();

    vtype_t name;
    lower_monster_name(name, monster->ml, creature->name);

    vtype_t msg;
    (void) sprintf(msg, "The %s strikes %s.", boltName.c_str(), name);
    printMessage(msg);

    if (harmType & creature->cdefense) {
        dam = dam * 2;
        if (monster->ml) {
            creature_recall[monster->mptr].r_cdefense |= harmType;
        }
    } else if (weaponID & creature->spells) {
        dam = dam / 4;
        if (monster->ml) {
            creature_recall[monster->mptr].r_spells |= weaponID;
        }
    }

    monster_name(name, monster->ml, creature->name);

    if (mon_take_hit((int) tile->cptr, dam) >= 0) {
        printMonsterActionText(name, "dies in a fit of agony.");
        displayCharacterExperience();
    } else if (dam > 0) {
        printMonsterActionText(name, "screams in agony.");
    }
}

// Shoot a bolt in a given direction -RAK-
void fire_bolt(int y, int x, int direction, int damage_hp, int spell_type_id, char *spell_name) {
    bool (*dummy)(Inventory_t *);
    int harm_type = 0;
    uint32_t weapon_type;
    get_flags(spell_type_id, &weapon_type, &harm_type, &dummy);

    int dist = 0;
    bool finished = false;

    while (!finished) {
        int oldy = y;
        int oldx = x;

        (void) playerMovePosition(direction, &y, &x);
        dist++;

        Cave_t *c_ptr = &cave[y][x];

        dungeonLiteSpot(oldy, oldx);

        if (dist > OBJ_BOLT_RANGE || c_ptr->fval >= MIN_CLOSED_SPACE) {
            finished = true;
            continue; // we're done here, break out of the loop
        }

        if (c_ptr->cptr > 1) {
            finished = true;
            fireBoltTouchesMonster(c_ptr, damage_hp, harm_type, weapon_type, spell_name);
        } else if (coordInsidePanel(y, x) && py.flags.blind < 1) {
            putChar('*', y, x);

            // show the bolt
            putQIO();
        }
    }
}

// Shoot a ball in a given direction.  Note that balls have an area affect. -RAK-
void fire_ball(int y, int x, int direction, int damage_hp, int spell_type_id, const char *spell_name) {
    int thit = 0;
    int tkill = 0;
    int max_dis = 2;

    bool (*destroy)(Inventory_t *);
    int harm_type;
    uint32_t weapon_type;
    get_flags(spell_type_id, &weapon_type, &harm_type, &destroy);

    int dist = 0;

    bool finished = false;
    while (!finished) {
        int oldy = y;
        int oldx = x;

        (void) playerMovePosition(direction, &y, &x);
        dist++;

        dungeonLiteSpot(oldy, oldx);

        if (dist > OBJ_BOLT_RANGE) {
            finished = true;
            continue;
        }

        Cave_t *c_ptr = &cave[y][x];

        if (c_ptr->fval >= MIN_CLOSED_SPACE || c_ptr->cptr > 1) {
            finished = true;

            if (c_ptr->fval >= MIN_CLOSED_SPACE) {
                y = oldy;
                x = oldx;
            }

            // The ball hits and explodes.

            // The explosion.
            for (int row = y - max_dis; row <= y + max_dis; row++) {
                for (int col = x - max_dis; col <= x + max_dis; col++) {
                    if (coordInBounds(row, col) && coordDistanceBetween(y, x, row, col) <= max_dis && los(y, x, row, col)) {
                        c_ptr = &cave[row][col];

                        if (c_ptr->tptr != 0 && (*destroy)(&treasure_list[c_ptr->tptr])) {
                            (void) dungeonDeleteObject(row, col);
                        }

                        if (c_ptr->fval <= MAX_OPEN_SPACE) {
                            if (c_ptr->cptr > 1) {
                                Monster_t *m_ptr = &monsters[c_ptr->cptr];
                                Creature_t *r_ptr = &creatures_list[m_ptr->mptr];

                                // lite up creature if visible, temp set pl so that monsterUpdateVisibility works
                                bool savedLitStatus = c_ptr->pl;
                                c_ptr->pl = true;
                                monsterUpdateVisibility((int) c_ptr->cptr);

                                thit++;
                                int dam = damage_hp;

                                if (harm_type & r_ptr->cdefense) {
                                    dam = dam * 2;
                                    if (m_ptr->ml) {
                                        creature_recall[m_ptr->mptr].r_cdefense |= harm_type;
                                    }
                                } else if (weapon_type & r_ptr->spells) {
                                    dam = dam / 4;
                                    if (m_ptr->ml) {
                                        creature_recall[m_ptr->mptr].r_spells |= weapon_type;
                                    }
                                }

                                dam = (dam / (coordDistanceBetween(row, col, y, x) + 1));
                                int k = mon_take_hit((int) c_ptr->cptr, dam);

                                if (k >= 0) {
                                    tkill++;
                                }
                                c_ptr->pl = savedLitStatus;
                            } else if (coordInsidePanel(row, col) && py.flags.blind < 1) {
                                putChar('*', row, col);
                            }
                        }
                    }
                }
            }

            // show ball of whatever
            putQIO();

            for (int row = (y - 2); row <= (y + 2); row++) {
                for (int col = (x - 2); col <= (x + 2); col++) {
                    if (coordInBounds(row, col) && coordInsidePanel(row, col) && coordDistanceBetween(y, x, row, col) <= max_dis) {
                        dungeonLiteSpot(row, col);
                    }
                }
            }
            // End  explosion.

            if (thit == 1) {
                vtype_t out_val;
                (void) sprintf(out_val, "The %s envelops a creature!", spell_name);
                printMessage(out_val);
            } else if (thit > 1) {
                vtype_t out_val;
                (void) sprintf(out_val, "The %s envelops several creatures!", spell_name);
                printMessage(out_val);
            }

            if (tkill == 1) {
                printMessage("There is a scream of agony!");
            } else if (tkill > 1) {
                printMessage("There are several screams of agony!");
            }

            if (tkill >= 0) {
                displayCharacterExperience();
            }
            // End ball hitting.
        } else if (coordInsidePanel(y, x) && py.flags.blind < 1) {
            putChar('*', y, x);

            // show bolt
            putQIO();
        }
    }
}

// Breath weapon works like a fire_ball, but affects the player.
// Note the area affect. -RAK-
void breath(int x, int y, int monster_id, int damage_hp, char *spell_name, int spell_type_id) {
    int max_dis = 2;

    bool (*destroy)(Inventory_t *);
    int harm_type;
    uint32_t weapon_type;
    get_flags(spell_type_id, &weapon_type, &harm_type, &destroy);

    int dam;

    for (int row = y - 2; row <= y + 2; row++) {
        for (int col = x - 2; col <= x + 2; col++) {
            if (coordInBounds(row, col) && coordDistanceBetween(y, x, row, col) <= max_dis && los(y, x, row, col)) {
                Cave_t *c_ptr = &cave[row][col];

                if (c_ptr->tptr != 0 && (*destroy)(&treasure_list[c_ptr->tptr])) {
                    (void) dungeonDeleteObject(row, col);
                }

                if (c_ptr->fval <= MAX_OPEN_SPACE) {
                    // must test status bit, not py.flags.blind here, flag could have
                    // been set by a previous monster, but the breath should still
                    // be visible until the blindness takes effect
                    if (coordInsidePanel(row, col) && !(py.flags.status & PY_BLIND)) {
                        putChar('*', row, col);
                    }

                    if (c_ptr->cptr > 1) {
                        Monster_t *m_ptr = &monsters[c_ptr->cptr];
                        Creature_t *r_ptr = &creatures_list[m_ptr->mptr];

                        dam = damage_hp;

                        if (harm_type & r_ptr->cdefense) {
                            dam = dam * 2;
                        } else if (weapon_type & r_ptr->spells) {
                            dam = (dam / 4);
                        }

                        dam = (dam / (coordDistanceBetween(row, col, y, x) + 1));

                        // can not call mon_take_hit here, since player does not
                        // get experience for kill
                        m_ptr->hp = (int16_t) (m_ptr->hp - dam);
                        m_ptr->csleep = 0;

                        if (m_ptr->hp < 0) {
                            uint32_t treas = monsterDeath((int) m_ptr->fy, (int) m_ptr->fx, r_ptr->cmove);

                            if (m_ptr->ml) {
                                uint32_t tmp = (uint32_t) ((creature_recall[m_ptr->mptr].r_cmove & CM_TREASURE) >> CM_TR_SHIFT);
                                if (tmp > ((treas & CM_TREASURE) >> CM_TR_SHIFT)) {
                                    treas = (uint32_t) ((treas & ~CM_TREASURE) | (tmp << CM_TR_SHIFT));
                                }
                                creature_recall[m_ptr->mptr].r_cmove = (uint32_t) (treas | (creature_recall[m_ptr->mptr].r_cmove & ~CM_TREASURE));
                            }

                            // It ate an already processed monster. Handle normally.
                            if (monster_id < c_ptr->cptr) {
                                dungeonDeleteMonster((int) c_ptr->cptr);
                            } else {
                                // If it eats this monster, an already processed monster
                                // will take its place, causing all kinds of havoc.
                                // Delay the kill a bit.
                                dungeonDeleteMonsterFix1((int) c_ptr->cptr);
                            }
                        }
                    } else if (c_ptr->cptr == 1) {
                        dam = (damage_hp / (coordDistanceBetween(row, col, y, x) + 1));

                        // let's do at least one point of damage
                        // prevents randomNumber(0) problem with damagePoisonedGas, also
                        if (dam == 0) {
                            dam = 1;
                        }

                        switch (spell_type_id) {
                            case GF_LIGHTNING:
                                damageLightningBolt(dam, spell_name);
                                break;
                            case GF_POISON_GAS:
                                damagePoisonedGas(dam, spell_name);
                                break;
                            case GF_ACID:
                                damageAcid(dam, spell_name);
                                break;
                            case GF_FROST:
                                damageCold(dam, spell_name);
                                break;
                            case GF_FIRE:
                                damageFire(dam, spell_name);
                                break;
                        }
                    }
                }
            }
        }
    }

    // show the ball of gas
    putQIO();

    for (int row = (y - 2); row <= (y + 2); row++) {
        for (int col = (x - 2); col <= (x + 2); col++) {
            if (coordInBounds(row, col) && coordInsidePanel(row, col) && coordDistanceBetween(y, x, row, col) <= max_dis) {
                dungeonLiteSpot(row, col);
            }
        }
    }
}

// Recharge a wand, staff, or rod.  Sometimes the item breaks. -RAK-
bool recharge(int charges) {
    int i, j;
    if (!inventoryFindRange(TV_STAFF, TV_WAND, &i, &j)) {
        printMessage("You have nothing to recharge.");
        return false;
    }

    int item_val;
    if (!inventoryGetInputForItemId(&item_val, "Recharge which item?", i, j, CNIL, CNIL)) {
        return false;
    }

    Inventory_t *i_ptr = &inventory[item_val];

    // recharge  I = recharge(20) = 1/6  failure for empty 10th level wand
    // recharge II = recharge(60) = 1/10 failure for empty 10th level wand
    //
    // make it harder to recharge high level, and highly charged wands, note
    // that i can be negative, so check its value before trying to call randomNumber().
    int chance = charges + 50 - (int) i_ptr->level - i_ptr->p1;
    if (chance < 19) {
        // Automatic failure.
        chance = 1;
    } else {
        chance = randomNumber(chance / 10);
    }

    if (chance == 1) {
        printMessage("There is a bright flash of light.");
        inventoryDestroyItem(item_val);
    } else {
        charges = (charges / (i_ptr->level + 2)) + 1;
        i_ptr->p1 += 2 + randomNumber(charges);
        if (spellItemIdentified(i_ptr)) {
            spellItemRemoveIdentification(i_ptr);
        }
        itemIdentificationClearEmpty(i_ptr);
    }

    return true;
}

// Increase or decrease a creatures hit points -RAK-
bool hp_monster(int y, int x, int direction, int damage_hp) {
    bool changed = false;

    int dist = 0;

    bool finished = false;
    while (!finished) {
        (void) playerMovePosition(direction, &y, &x);
        dist++;

        Cave_t *c_ptr = &cave[y][x];

        if (dist > OBJ_BOLT_RANGE || c_ptr->fval >= MIN_CLOSED_SPACE) {
            finished = true;
        } else if (c_ptr->cptr > 1) {
            finished = true;

            Monster_t *m_ptr = &monsters[c_ptr->cptr];
            Creature_t *r_ptr = &creatures_list[m_ptr->mptr];

            vtype_t name;
            monster_name(name, m_ptr->ml, r_ptr->name);

            if (mon_take_hit((int) c_ptr->cptr, damage_hp) >= 0) {
                printMonsterActionText(name, "dies in a fit of agony.");
                displayCharacterExperience();
            } else if (damage_hp > 0) {
                printMonsterActionText(name, "screams in agony.");
            }

            changed = true;
        }
    }

    return changed;
}

// Drains life; note it must be living. -RAK-
bool drain_life(int y, int x, int direction) {
    bool drained = false;

    int dist = 0;

    bool finished = false;
    while (!finished) {
        (void) playerMovePosition(direction, &y, &x);
        dist++;

        Cave_t *c_ptr = &cave[y][x];

        if (dist > OBJ_BOLT_RANGE || c_ptr->fval >= MIN_CLOSED_SPACE) {
            finished = true;
        } else if (c_ptr->cptr > 1) {
            finished = true;

            Monster_t *m_ptr = &monsters[c_ptr->cptr];
            Creature_t *r_ptr = &creatures_list[m_ptr->mptr];

            if ((r_ptr->cdefense & CD_UNDEAD) == 0) {
                vtype_t name;
                monster_name(name, m_ptr->ml, r_ptr->name);

                if (mon_take_hit((int) c_ptr->cptr, 75) >= 0) {
                    printMonsterActionText(name, "dies in a fit of agony.");
                    displayCharacterExperience();
                } else {
                    printMonsterActionText(name, "screams in agony.");
                }

                drained = true;
            } else {
                creature_recall[m_ptr->mptr].r_cdefense |= CD_UNDEAD;
            }
        }
    }

    return drained;
}

// Increase or decrease a creatures speed -RAK-
// NOTE: cannot slow a winning creature (BALROG)
bool speed_monster(int y, int x, int direction, int speed) {
    bool changed = false;

    int dist = 0;

    bool finished = false;
    while (!finished) {
        (void) playerMovePosition(direction, &y, &x);
        dist++;

        Cave_t *c_ptr = &cave[y][x];

        if (dist > OBJ_BOLT_RANGE || c_ptr->fval >= MIN_CLOSED_SPACE) {
            finished = true;
        } else if (c_ptr->cptr > 1) {
            finished = true;

            Monster_t *m_ptr = &monsters[c_ptr->cptr];
            Creature_t *r_ptr = &creatures_list[m_ptr->mptr];

            vtype_t name;
            monster_name(name, m_ptr->ml, r_ptr->name);

            if (speed > 0) {
                m_ptr->cspeed += speed;
                m_ptr->csleep = 0;

                changed = true;

                printMonsterActionText(name, "starts moving faster.");
            } else if (randomNumber(MAX_MONS_LEVEL) > r_ptr->level) {
                m_ptr->cspeed += speed;
                m_ptr->csleep = 0;

                changed = true;

                printMonsterActionText(name, "starts moving slower.");
            } else {
                m_ptr->csleep = 0;

                printMonsterActionText(name, "is unaffected.");
            }
        }
    }

    return changed;
}

// Confuse a creature -RAK-
bool confuse_monster(int y, int x, int direction) {
    bool confused = false;

    int dist = 0;

    bool finished = false;
    while (!finished) {
        (void) playerMovePosition(direction, &y, &x);
        dist++;

        Cave_t *c_ptr = &cave[y][x];

        if (dist > OBJ_BOLT_RANGE || c_ptr->fval >= MIN_CLOSED_SPACE) {
            finished = true;
        } else if (c_ptr->cptr > 1) {
            finished = true;

            Monster_t *m_ptr = &monsters[c_ptr->cptr];
            Creature_t *r_ptr = &creatures_list[m_ptr->mptr];

            vtype_t name;
            monster_name(name, m_ptr->ml, r_ptr->name);

            if (randomNumber(MAX_MONS_LEVEL) < r_ptr->level || (CD_NO_SLEEP & r_ptr->cdefense)) {
                if (m_ptr->ml && (r_ptr->cdefense & CD_NO_SLEEP)) {
                    creature_recall[m_ptr->mptr].r_cdefense |= CD_NO_SLEEP;
                }

                // Monsters which resisted the attack should wake up.
                // Monsters with innate resistance ignore the attack.
                if (!(CD_NO_SLEEP & r_ptr->cdefense)) {
                    m_ptr->csleep = 0;
                }

                printMonsterActionText(name, "is unaffected.");
            } else {
                if (m_ptr->confused) {
                    m_ptr->confused += 3;
                } else {
                    m_ptr->confused = (uint8_t) (2 + randomNumber(16));
                }
                m_ptr->csleep = 0;

                confused = true;

                printMonsterActionText(name, "appears confused.");
            }
        }
    }

    return confused;
}

// Sleep a creature. -RAK-
bool sleep_monster(int y, int x, int direction) {
    bool asleep = false;

    int dist = 0;

    bool finished = false;
    while (!finished) {
        (void) playerMovePosition(direction, &y, &x);
        dist++;

        Cave_t *c_ptr = &cave[y][x];

        if (dist > OBJ_BOLT_RANGE || c_ptr->fval >= MIN_CLOSED_SPACE) {
            finished = true;
        } else if (c_ptr->cptr > 1) {
            finished = true;

            Monster_t *m_ptr = &monsters[c_ptr->cptr];
            Creature_t *r_ptr = &creatures_list[m_ptr->mptr];

            vtype_t name;
            monster_name(name, m_ptr->ml, r_ptr->name);

            if (randomNumber(MAX_MONS_LEVEL) < r_ptr->level || (CD_NO_SLEEP & r_ptr->cdefense)) {
                if (m_ptr->ml && (r_ptr->cdefense & CD_NO_SLEEP)) {
                    creature_recall[m_ptr->mptr].r_cdefense |= CD_NO_SLEEP;
                }

                printMonsterActionText(name, "is unaffected.");
            } else {
                m_ptr->csleep = 500;

                asleep = true;

                printMonsterActionText(name, "falls asleep.");
            }
        }
    }

    return asleep;
}

// Turn stone to mud, delete wall. -RAK-
bool wall_to_mud(int y, int x, int direction) {
    bool turned = false;

    int dist = 0;

    bool finished = false;
    while (!finished) {
        (void) playerMovePosition(direction, &y, &x);
        dist++;

        Cave_t *c_ptr = &cave[y][x];

        // note, this ray can move through walls as it turns them to mud
        if (dist == OBJ_BOLT_RANGE) {
            finished = true;
        }

        if (c_ptr->fval >= MIN_CAVE_WALL && c_ptr->fval != BOUNDARY_WALL) {
            finished = true;

            (void) twall(y, x, 1, 0);

            if (caveTileVisible(y, x)) {
                turned = true;
                printMessage("The wall turns into mud.");
            }
        } else if (c_ptr->tptr != 0 && c_ptr->fval >= MIN_CLOSED_SPACE) {
            finished = true;

            if (coordInsidePanel(y, x) && caveTileVisible(y, x)) {
                turned = true;

                obj_desc_t description;
                itemDescription(description, &treasure_list[c_ptr->tptr], false);

                obj_desc_t out_val;
                (void) sprintf(out_val, "The %s turns into mud.", description);
                printMessage(out_val);
            }

            if (treasure_list[c_ptr->tptr].tval == TV_RUBBLE) {
                (void) dungeonDeleteObject(y, x);
                if (randomNumber(10) == 1) {
                    dungeonPlaceRandomObjectAt(y, x, false);
                    if (caveTileVisible(y, x)) {
                        printMessage("You have found something!");
                    }
                }
                dungeonLiteSpot(y, x);
            } else {
                (void) dungeonDeleteObject(y, x);
            }
        }

        if (c_ptr->cptr > 1) {
            Monster_t *m_ptr = &monsters[c_ptr->cptr];
            Creature_t *r_ptr = &creatures_list[m_ptr->mptr];

            if (CD_STONE & r_ptr->cdefense) {
                vtype_t name;
                monster_name(name, m_ptr->ml, r_ptr->name);

                // Should get these messages even if the monster is not visible.
                int i = mon_take_hit((int) c_ptr->cptr, 100);
                if (i >= 0) {
                    creature_recall[i].r_cdefense |= CD_STONE;
                    printMonsterActionText(name, "dissolves!");
                    displayCharacterExperience(); // print msg before calling prt_exp
                } else {
                    creature_recall[m_ptr->mptr].r_cdefense |= CD_STONE;
                    printMonsterActionText(name, "grunts in pain!");
                }
                finished = true;
            }
        }
    }

    return turned;
}

// Destroy all traps and doors in a given direction -RAK-
bool td_destroy2(int y, int x, int direction) {
    bool destroyed = false;

    int dist = 0;

    Cave_t *c_ptr;

    do {
        (void) playerMovePosition(direction, &y, &x);
        dist++;

        c_ptr = &cave[y][x];

        // must move into first closed spot, as it might be a secret door
        if (c_ptr->tptr != 0) {
            Inventory_t *t_ptr = &treasure_list[c_ptr->tptr];

            if (t_ptr->tval == TV_INVIS_TRAP ||
                t_ptr->tval == TV_CLOSED_DOOR ||
                t_ptr->tval == TV_VIS_TRAP ||
                t_ptr->tval == TV_OPEN_DOOR ||
                t_ptr->tval == TV_SECRET_DOOR) {
                if (dungeonDeleteObject(y, x)) {
                    destroyed = true;

                    printMessage("There is a bright flash of light!");
                }
            } else if (t_ptr->tval == TV_CHEST && t_ptr->flags != 0) {
                destroyed = true;

                printMessage("Click!");
                t_ptr->flags &= ~(CH_TRAPPED | CH_LOCKED);
                t_ptr->name2 = SN_UNLOCKED;
                spellItemIdentifyAndRemoveRandomInscription(t_ptr);
            }
        }
    } while ((dist <= OBJ_BOLT_RANGE) || c_ptr->fval <= MAX_OPEN_SPACE);

    return destroyed;
}

// Polymorph a monster -RAK-
// NOTE: cannot polymorph a winning creature (BALROG)
bool poly_monster(int y, int x, int direction) {
    bool morphed = false;

    int dist = 0;

    bool finished = false;
    while (!finished) {
        (void) playerMovePosition(direction, &y, &x);
        dist++;

        Cave_t *c_ptr = &cave[y][x];

        if (dist > OBJ_BOLT_RANGE || c_ptr->fval >= MIN_CLOSED_SPACE) {
            finished = true;
        } else if (c_ptr->cptr > 1) {
            Monster_t *m_ptr = &monsters[c_ptr->cptr];
            Creature_t *r_ptr = &creatures_list[m_ptr->mptr];

            if (randomNumber(MAX_MONS_LEVEL) > r_ptr->level) {
                finished = true;

                dungeonDeleteMonster((int) c_ptr->cptr);

                // Place_monster() should always return true here.
                morphed = monsterPlaceNew(y, x, randomNumber(monster_levels[MAX_MONS_LEVEL] - monster_levels[0]) - 1 + monster_levels[0], false);

                // don't test c_ptr->fm here, only pl/tl
                if (morphed && coordInsidePanel(y, x) && (c_ptr->tl || c_ptr->pl)) {
                    morphed = true;
                }
            } else {
                vtype_t name;
                monster_name(name, m_ptr->ml, r_ptr->name);
                printMonsterActionText(name, "is unaffected.");
            }
        }
    }

    return morphed;
}

// Create a wall. -RAK-
bool build_wall(int y, int x, int direction) {
    bool built = false;

    int dist = 0;

    bool finished = false;
    while (!finished) {
        (void) playerMovePosition(direction, &y, &x);
        dist++;

        Cave_t *c_ptr = &cave[y][x];

        if (dist > OBJ_BOLT_RANGE || c_ptr->fval >= MIN_CLOSED_SPACE) {
            finished = true;
            continue; // we're done here, break out of the loop
        }

        if (c_ptr->tptr != 0) {
            (void) dungeonDeleteObject(y, x);
        }

        if (c_ptr->cptr > 1) {
            finished = true;

            Monster_t *m_ptr = &monsters[c_ptr->cptr];
            Creature_t *r_ptr = &creatures_list[m_ptr->mptr];

            if (!(r_ptr->cmove & CM_PHASE)) {
                // monster does not move, can't escape the wall
                int damage;
                if (r_ptr->cmove & CM_ATTACK_ONLY) {
                    // this will kill everything
                    damage = 3000;
                } else {
                    damage = diceDamageRoll(4, 8);
                }

                vtype_t name;
                monster_name(name, m_ptr->ml, r_ptr->name);

                printMonsterActionText(name, "wails out in pain!");

                if (mon_take_hit((int) c_ptr->cptr, damage) >= 0) {
                    printMonsterActionText(name, "is embedded in the rock.");
                    displayCharacterExperience();
                }
            } else if (r_ptr->cchar == 'E' || r_ptr->cchar == 'X') {
                // must be an earth elemental or an earth spirit, or a Xorn
                // increase its hit points
                m_ptr->hp += diceDamageRoll(4, 8);
            }
        }

        c_ptr->fval = MAGMA_WALL;
        c_ptr->fm = false;

        // Permanently light this wall if it is lit by player's lamp.
        c_ptr->pl = (c_ptr->tl || c_ptr->pl);
        dungeonLiteSpot(y, x);

        // NOTE: this was never used anywhere. Is that a bug or just obsolete code?
        // i++;

        built = true;
    }

    return built;
}

// Replicate a creature -RAK-
bool clone_monster(int y, int x, int direction) {
    int dist = 0;

    bool finished = false;
    while (!finished) {
        (void) playerMovePosition(direction, &y, &x);
        dist++;

        Cave_t *c_ptr = &cave[y][x];

        if (dist > OBJ_BOLT_RANGE || c_ptr->fval >= MIN_CLOSED_SPACE) {
            finished = true;
        } else if (c_ptr->cptr > 1) {
            monsters[c_ptr->cptr].csleep = 0;

            // monptr of 0 is safe here, since can't reach here from creatures
            return monsterMultiply(y, x, (int) monsters[c_ptr->cptr].mptr, 0);
        }
    }

    return false;
}

// Move the creature record to a new location -RAK-
void teleport_away(int monster_id, int distance_from_player) {
    int yn, xn;

    Monster_t *m_ptr = &monsters[monster_id];

    int ctr = 0;
    do {
        do {
            yn = m_ptr->fy + (randomNumber(2 * distance_from_player + 1) - (distance_from_player + 1));
            xn = m_ptr->fx + (randomNumber(2 * distance_from_player + 1) - (distance_from_player + 1));
        } while (!coordInBounds(yn, xn));

        ctr++;
        if (ctr > 9) {
            ctr = 0;
            distance_from_player += 5;
        }
    } while ((cave[yn][xn].fval >= MIN_CLOSED_SPACE) || (cave[yn][xn].cptr != 0));

    dungeonMoveCreatureRecord((int) m_ptr->fy, (int) m_ptr->fx, yn, xn);
    dungeonLiteSpot((int) m_ptr->fy, (int) m_ptr->fx);
    m_ptr->fy = (uint8_t) yn;
    m_ptr->fx = (uint8_t) xn;

    // this is necessary, because the creature is
    // not currently visible in its new position.
    m_ptr->ml = false;
    m_ptr->cdis = (uint8_t) coordDistanceBetween(char_row, char_col, yn, xn);
    monsterUpdateVisibility(monster_id);
}

// Teleport player to spell casting creature -RAK-
void teleport_to(int to_y, int to_x) {
    int y, x;

    int dis = 1;
    int ctr = 0;
    do {
        y = to_y + (randomNumber(2 * dis + 1) - (dis + 1));
        x = to_x + (randomNumber(2 * dis + 1) - (dis + 1));
        ctr++;
        if (ctr > 9) {
            ctr = 0;
            dis++;
        }
    } while (!coordInBounds(y, x) || (cave[y][x].fval >= MIN_CLOSED_SPACE) || (cave[y][x].cptr >= 2));

    dungeonMoveCreatureRecord(char_row, char_col, y, x);

    for (int row = char_row - 1; row <= char_row + 1; row++) {
        for (int col = char_col - 1; col <= char_col + 1; col++) {
            Cave_t *c_ptr = &cave[row][col];
            c_ptr->tl = false;
            dungeonLiteSpot(row, col);
        }
    }

    dungeonLiteSpot(char_row, char_col);

    char_row = (int16_t) y;
    char_col = (int16_t) x;
    dungeonResetView();

    // light creatures
    updateMonsters(false);
}

// Teleport all creatures in a given direction away -RAK-
bool teleport_monster(int y, int x, int direction) {
    bool teleported = false;

    int dist = 0;

    bool finished = false;
    while (!finished) {
        (void) playerMovePosition(direction, &y, &x);
        dist++;

        Cave_t *c_ptr = &cave[y][x];

        if (dist > OBJ_BOLT_RANGE || c_ptr->fval >= MIN_CLOSED_SPACE) {
            finished = true;
        } else if (c_ptr->cptr > 1) {
            // wake it up
            monsters[c_ptr->cptr].csleep = 0;

            teleport_away((int) c_ptr->cptr, MAX_SIGHT);

            teleported = true;
        }
    }

    return teleported;
}

// Delete all creatures within max_sight distance -RAK-
// NOTE : Winning creatures cannot be killed by genocide.
bool mass_genocide() {
    bool killed = false;

    for (int id = next_free_monster_id - 1; id >= MIN_MONIX; id--) {
        Monster_t *m_ptr = &monsters[id];
        Creature_t *r_ptr = &creatures_list[m_ptr->mptr];

        if (m_ptr->cdis <= MAX_SIGHT && (r_ptr->cmove & CM_WIN) == 0) {
            dungeonDeleteMonster(id);

            killed = true;
        }
    }

    return killed;
}

// Delete all creatures of a given type from level. -RAK-
// This does not keep creatures of type from appearing later.
// NOTE : Winning creatures can not be killed by genocide.
bool genocide() {
    char typ;
    if (!getCommand("Which type of creature do you wish exterminated?", &typ)) {
        return false;
    }

    bool killed = false;

    for (int id = next_free_monster_id - 1; id >= MIN_MONIX; id--) {
        Monster_t *m_ptr = &monsters[id];
        Creature_t *r_ptr = &creatures_list[m_ptr->mptr];

        if (typ == creatures_list[m_ptr->mptr].cchar) {
            if ((r_ptr->cmove & CM_WIN) == 0) {
                dungeonDeleteMonster(id);

                killed = true;
            } else {
                // genocide is a powerful spell, so we will let the player
                // know the names of the creatures he did not destroy,
                // this message makes no sense otherwise
                vtype_t msg;
                (void) sprintf(msg, "The %s is unaffected.", r_ptr->name);
                printMessage(msg);
            }
        }
    }

    return killed;
}

// Change speed of any creature . -RAK-
// NOTE: cannot slow a winning creature (BALROG)
bool speed_monsters(int speed) {
    bool speedy = false;

    for (int id = next_free_monster_id - 1; id >= MIN_MONIX; id--) {
        Monster_t *m_ptr = &monsters[id];
        Creature_t *r_ptr = &creatures_list[m_ptr->mptr];

        vtype_t name;
        monster_name(name, m_ptr->ml, r_ptr->name);

        if (m_ptr->cdis > MAX_SIGHT || !los(char_row, char_col, (int) m_ptr->fy, (int) m_ptr->fx)) {
            continue; // do nothing
        }

        if (speed > 0) {
            m_ptr->cspeed += speed;
            m_ptr->csleep = 0;

            if (m_ptr->ml) {
                speedy = true;
                printMonsterActionText(name, "starts moving faster.");
            }
        } else if (randomNumber(MAX_MONS_LEVEL) > r_ptr->level) {
            m_ptr->cspeed += speed;
            m_ptr->csleep = 0;

            if (m_ptr->ml) {
                speedy = true;
                printMonsterActionText(name, "starts moving slower.");
            }
        } else if (m_ptr->ml) {
            m_ptr->csleep = 0;
            printMonsterActionText(name, "is unaffected.");
        }
    }

    return speedy;
}

// Sleep any creature . -RAK-
bool sleep_monsters2() {
    bool asleep = false;

    for (int id = next_free_monster_id - 1; id >= MIN_MONIX; id--) {
        Monster_t *m_ptr = &monsters[id];
        Creature_t *r_ptr = &creatures_list[m_ptr->mptr];

        vtype_t name;
        monster_name(name, m_ptr->ml, r_ptr->name);

        if (m_ptr->cdis > MAX_SIGHT || !los(char_row, char_col, (int) m_ptr->fy, (int) m_ptr->fx)) {
            continue; // do nothing
        }

        if (randomNumber(MAX_MONS_LEVEL) < r_ptr->level || (CD_NO_SLEEP & r_ptr->cdefense)) {
            if (m_ptr->ml) {
                if (r_ptr->cdefense & CD_NO_SLEEP) {
                    creature_recall[m_ptr->mptr].r_cdefense |= CD_NO_SLEEP;
                }
                printMonsterActionText(name, "is unaffected.");
            }
        } else {
            m_ptr->csleep = 500;
            if (m_ptr->ml) {
                asleep = true;
                printMonsterActionText(name, "falls asleep.");
            }
        }
    }

    return asleep;
}

// Polymorph any creature that player can see. -RAK-
// NOTE: cannot polymorph a winning creature (BALROG)
bool mass_poly() {
    bool morphed = false;

    for (int id = next_free_monster_id - 1; id >= MIN_MONIX; id--) {
        Monster_t *m_ptr = &monsters[id];

        if (m_ptr->cdis <= MAX_SIGHT) {
            Creature_t *r_ptr = &creatures_list[m_ptr->mptr];

            if ((r_ptr->cmove & CM_WIN) == 0) {
                int y = m_ptr->fy;
                int x = m_ptr->fx;
                dungeonDeleteMonster(id);

                // Place_monster() should always return true here.
                morphed = monsterPlaceNew(y, x, randomNumber(monster_levels[MAX_MONS_LEVEL] - monster_levels[0]) - 1 + monster_levels[0], false);
            }
        }
    }

    return morphed;
}

// Display evil creatures on current panel -RAK-
bool detect_evil() {
    bool detected = false;

    for (int id = next_free_monster_id - 1; id >= MIN_MONIX; id--) {
        Monster_t *m_ptr = &monsters[id];

        if (coordInsidePanel((int) m_ptr->fy, (int) m_ptr->fx) && (CD_EVIL & creatures_list[m_ptr->mptr].cdefense)) {
            m_ptr->ml = true;

            detected = true;

            // works correctly even if hallucinating
            putChar((char) creatures_list[m_ptr->mptr].cchar, (int) m_ptr->fy, (int) m_ptr->fx);
        }
    }

    if (detected) {
        printMessage("You sense the presence of evil!");
        printMessage(CNIL);

        // must unlight every monster just lighted
        updateMonsters(false);
    }

    return detected;
}

// Change players hit points in some manner -RAK-
bool hp_player(int adjustment) {
    if (py.misc.chp >= py.misc.mhp) {
        return false;
    }

    py.misc.chp += adjustment;
    if (py.misc.chp > py.misc.mhp) {
        py.misc.chp = py.misc.mhp;
        py.misc.chp_frac = 0;
    }
    printCharacterCurrentHitPoints();

    adjustment = adjustment / 5;
    if (adjustment < 3) {
        if (adjustment == 0) {
            printMessage("You feel a little better.");
        } else {
            printMessage("You feel better.");
        }
    } else {
        if (adjustment < 7) {
            printMessage("You feel much better.");
        } else {
            printMessage("You feel very good.");
        }
    }

    return true;
}

// Cure players confusion -RAK-
bool cure_confusion() {
    if (py.flags.confused > 1) {
        py.flags.confused = 1;
        return true;
    }
    return false;
}

// Cure players blindness -RAK-
bool cure_blindness() {
    if (py.flags.blind > 1) {
        py.flags.blind = 1;
        return true;
    }
    return false;
}

// Cure poisoning -RAK-
bool cure_poison() {
    if (py.flags.poisoned > 1) {
        py.flags.poisoned = 1;
        return true;
    }
    return false;
}

// Cure the players fear -RAK-
bool remove_fear() {
    if (py.flags.afraid > 1) {
        py.flags.afraid = 1;
        return true;
    }
    return false;
}

static void earthquakeHitsMonster(int monsterID) {
    Monster_t *monster = &monsters[monsterID];
    Creature_t *creature = &creatures_list[monster->mptr];

    if (!(creature->cmove & CM_PHASE)) {
        int damage;
        if (creature->cmove & CM_ATTACK_ONLY) {
            // this will kill everything
            damage = 3000;
        } else {
            damage = diceDamageRoll(4, 8);
        }

        vtype_t name;
        monster_name(name, monster->ml, creature->name);

        printMonsterActionText(name, "wails out in pain!");

        if (mon_take_hit(monsterID, damage) >= 0) {
            printMonsterActionText(name, "is embedded in the rock.");
            displayCharacterExperience();
        }
    } else if (creature->cchar == 'E' || creature->cchar == 'X') {
        // must be an earth elemental or an earth spirit, or a
        // Xorn increase its hit points
        monster->hp += diceDamageRoll(4, 8);
    }
}

// This is a fun one.  In a given block, pick some walls and
// turn them into open spots.  Pick some open spots and current_game_turn
// them into walls.  An "Earthquake" effect. -RAK-
void earthquake() {
    for (int y = char_row - 8; y <= char_row + 8; y++) {
        for (int x = char_col - 8; x <= char_col + 8; x++) {
            if ((y != char_row || x != char_col) && coordInBounds(y, x) && randomNumber(8) == 1) {
                Cave_t *c_ptr = &cave[y][x];

                if (c_ptr->tptr != 0) {
                    (void) dungeonDeleteObject(y, x);
                }

                if (c_ptr->cptr > 1) {
                    earthquakeHitsMonster(c_ptr->cptr);
                }

                if (c_ptr->fval >= MIN_CAVE_WALL && c_ptr->fval != BOUNDARY_WALL) {
                    c_ptr->fval = CORR_FLOOR;
                    c_ptr->pl = false;
                    c_ptr->fm = false;
                } else if (c_ptr->fval <= MAX_CAVE_FLOOR) {
                    int tmp = randomNumber(10);

                    if (tmp < 6) {
                        c_ptr->fval = QUARTZ_WALL;
                    } else if (tmp < 9) {
                        c_ptr->fval = MAGMA_WALL;
                    } else {
                        c_ptr->fval = GRANITE_WALL;
                    }

                    c_ptr->fm = false;
                }
                dungeonLiteSpot(y, x);
            }
        }
    }
}

// Evil creatures don't like this. -RAK-
bool protect_evil() {
    bool isProtected = py.flags.protevil == 0;

    py.flags.protevil += randomNumber(25) + 3 * py.misc.lev;

    return isProtected;
}

// Create some high quality mush for the player. -RAK-
void create_food() {
    uint8_t treasureID = cave[char_row][char_col].tptr;

    // take no action here, don't want to destroy object under player
    if (treasureID != 0) {
        // set player_free_turn so that scroll/spell points won't be used
        player_free_turn = true;

        printMessage("There is already an object under you.");

        return;
    }

    dungeonPlaceRandomObjectAt(char_row, char_col, false);
    inventoryItemCopyTo(OBJ_MUSH, &treasure_list[treasureID]);
}

// Attempts to destroy a type of creature.  Success depends on
// the creatures level VS. the player's level -RAK-
bool dispel_creature(int creature_defense, int damage) {
    bool dispelled = false;

    for (int id = next_free_monster_id - 1; id >= MIN_MONIX; id--) {
        Monster_t *m_ptr = &monsters[id];

        if (m_ptr->cdis <= MAX_SIGHT && (creature_defense & creatures_list[m_ptr->mptr].cdefense) && los(char_row, char_col, (int) m_ptr->fy, (int) m_ptr->fx)) {
            Creature_t *r_ptr = &creatures_list[m_ptr->mptr];

            creature_recall[m_ptr->mptr].r_cdefense |= creature_defense;

            dispelled = true;

            vtype_t name;
            monster_name(name, m_ptr->ml, r_ptr->name);

            int hit = mon_take_hit(id, randomNumber(damage));

            // Should get these messages even if the monster is not visible.
            if (hit >= 0) {
                printMonsterActionText(name, "dissolves!");
            } else {
                printMonsterActionText(name, "shudders.");
            }

            if (hit >= 0) {
                displayCharacterExperience();
            }
        }
    }

    return dispelled;
}

// Attempt to turn (confuse) undead creatures. -RAK-
bool turn_undead() {
    bool turned = false;

    for (int id = next_free_monster_id - 1; id >= MIN_MONIX; id--) {
        Monster_t *m_ptr = &monsters[id];
        Creature_t *r_ptr = &creatures_list[m_ptr->mptr];

        if (m_ptr->cdis <= MAX_SIGHT && (CD_UNDEAD & r_ptr->cdefense) && los(char_row, char_col, (int) m_ptr->fy, (int) m_ptr->fx)) {
            vtype_t name;
            monster_name(name, m_ptr->ml, r_ptr->name);

            if (py.misc.lev + 1 > r_ptr->level || randomNumber(5) == 1) {
                if (m_ptr->ml) {
                    creature_recall[m_ptr->mptr].r_cdefense |= CD_UNDEAD;

                    turned = true;

                    printMonsterActionText(name, "runs frantically!");
                }

                m_ptr->confused = (uint8_t) py.misc.lev;
            } else if (m_ptr->ml) {
                printMonsterActionText(name, "is unaffected.");
            }
        }
    }

    return turned;
}

// Leave a glyph of warding. Creatures will not pass over! -RAK-
void warding_glyph() {
    if (cave[char_row][char_col].tptr == 0) {
        int newID = popt();
        cave[char_row][char_col].tptr = (uint8_t) newID;
        inventoryItemCopyTo(OBJ_SCARE_MON, &treasure_list[newID]);
    }
}

// Lose a strength point. -RAK-
void lose_str() {
    if (!py.flags.sustain_str) {
        (void) playerStatRandomDecrease(A_STR);
        printMessage("You feel very sick.");
    } else {
        printMessage("You feel sick for a moment,  it passes.");
    }
}

// Lose an intelligence point. -RAK-
void lose_int() {
    if (!py.flags.sustain_int) {
        (void) playerStatRandomDecrease(A_INT);
        printMessage("You become very dizzy.");
    } else {
        printMessage("You become dizzy for a moment,  it passes.");
    }
}

// Lose a wisdom point. -RAK-
void lose_wis() {
    if (!py.flags.sustain_wis) {
        (void) playerStatRandomDecrease(A_WIS);
        printMessage("You feel very naive.");
    } else {
        printMessage("You feel naive for a moment,  it passes.");
    }
}

// Lose a dexterity point. -RAK-
void lose_dex() {
    if (!py.flags.sustain_dex) {
        (void) playerStatRandomDecrease(A_DEX);
        printMessage("You feel very sore.");
    } else {
        printMessage("You feel sore for a moment,  it passes.");
    }
}

// Lose a constitution point. -RAK-
void lose_con() {
    if (!py.flags.sustain_con) {
        (void) playerStatRandomDecrease(A_CON);
        printMessage("You feel very sick.");
    } else {
        printMessage("You feel sick for a moment,  it passes.");
    }
}

// Lose a charisma point. -RAK-
void lose_chr() {
    if (!py.flags.sustain_chr) {
        (void) playerStatRandomDecrease(A_CHR);
        printMessage("Your skin starts to itch.");
    } else {
        printMessage("Your skin starts to itch, but feels better now.");
    }
}

// Lose experience -RAK-
void lose_exp(int32_t adjustment) {
    if (adjustment > py.misc.exp) {
        py.misc.exp = 0;
    } else {
        py.misc.exp -= adjustment;
    }
    displayCharacterExperience();

    int exp = 0;
    while ((signed) (player_base_exp_levels[exp] * py.misc.expfact / 100) <= py.misc.exp) {
        exp++;
    }

    // increment exp once more, because level 1 exp is stored in player_base_exp_levels[0]
    exp++;

    if (py.misc.lev != exp) {
        py.misc.lev = (uint16_t) exp;

        playerCalculateHitPoints();

        Class_t *c_ptr = &classes[py.misc.pclass];

        if (c_ptr->spell == MAGE) {
            playerCalculateAllowedSpellsCount(A_INT);
            playerGainMana(A_INT);
        } else if (c_ptr->spell == PRIEST) {
            playerCalculateAllowedSpellsCount(A_WIS);
            playerGainMana(A_WIS);
        }
        printCharacterLevel();
        printCharacterTitle();
    }
}

// Slow Poison -RAK-
bool slow_poison() {
    if (py.flags.poisoned > 0) {
        py.flags.poisoned = (int16_t) (py.flags.poisoned / 2);
        if (py.flags.poisoned < 1) {
            py.flags.poisoned = 1;
        }
        printMessage("The effect of the poison has been reduced.");
        return true;
    }

    return false;
}

// Bless -RAK-
void bless(int adjustment) {
    py.flags.blessed += adjustment;
}

// Detect Invisible for period of time -RAK-
void detect_inv2(int adjustment) {
    py.flags.detect_inv += adjustment;
}

static void replace_spot(int y, int x, int typ) {
    Cave_t *c_ptr = &cave[y][x];

    switch (typ) {
        case 1:
        case 2:
        case 3:
            c_ptr->fval = CORR_FLOOR;
            break;
        case 4:
        case 7:
        case 10:
            c_ptr->fval = GRANITE_WALL;
            break;
        case 5:
        case 8:
        case 11:
            c_ptr->fval = MAGMA_WALL;
            break;
        case 6:
        case 9:
        case 12:
            c_ptr->fval = QUARTZ_WALL;
            break;
    }

    c_ptr->pl = false;
    c_ptr->fm = false;
    c_ptr->lr = false; // this is no longer part of a room

    if (c_ptr->tptr != 0) {
        (void) dungeonDeleteObject(y, x);
    }

    if (c_ptr->cptr > 1) {
        dungeonDeleteMonster((int) c_ptr->cptr);
    }
}

// The spell of destruction. -RAK-
// NOTE : Winning creatures that are deleted will be considered
//        as teleporting to another level.  This will NOT win
//        the game.
void destroy_area(int y, int x) {
    if (current_dungeon_level > 0) {
        for (int i = (y - 15); i <= (y + 15); i++) {
            for (int j = (x - 15); j <= (x + 15); j++) {
                if (coordInBounds(i, j) && cave[i][j].fval != BOUNDARY_WALL) {
                    int dist = coordDistanceBetween(i, j, y, x);

                    // clear player's spot, but don't put wall there
                    if (dist == 0) {
                        replace_spot(i, j, 1);
                    } else if (dist < 13) {
                        replace_spot(i, j, randomNumber(6));
                    } else if (dist < 16) {
                        replace_spot(i, j, randomNumber(9));
                    }
                }
            }
        }
    }
    printMessage("There is a searing blast of light!");
    py.flags.blind += 10 + randomNumber(10);
}

// Enchants a plus onto an item. -RAK-
// `limit` param is the maximum bonus allowed; usually 10,
// but weapon's maximum damage when enchanting melee weapons to damage.
bool enchant(int16_t *plusses, int16_t max_bonus_limit) {
    // avoid randomNumber(0) call
    if (max_bonus_limit <= 0) {
        return false;
    }

    int chance = 0;

    if (*plusses > 0) {
        chance = *plusses;

        // very rarely allow enchantment over limit
        if (randomNumber(100) == 1) {
            chance = randomNumber(chance) - 1;
        }
    }

    if (randomNumber(max_bonus_limit) > chance) {
        *plusses += 1;
        return true;
    }

    return false;
}

// Removes curses from items in inventory -RAK-
bool remove_curse() {
    bool removed = false;

    for (int id = INVEN_WIELD; id <= INVEN_OUTER; id++) {
        if (TR_CURSED & inventory[id].flags) {
            inventory[id].flags &= ~TR_CURSED;
            playerRecalculateBonuses();
            removed = true;
        }
    }

    return removed;
}

// Restores any drained experience -RAK-
bool restore_level() {
    if (py.misc.max_exp > py.misc.exp) {
        printMessage("You feel your life energies returning.");

        // this while loop is not redundant, ptr_exp may reduce the exp level
        while (py.misc.exp < py.misc.max_exp) {
            py.misc.exp = py.misc.max_exp;
            displayCharacterExperience();
        }

        return true;
    }

    return false;
}
