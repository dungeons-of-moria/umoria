// Copyright (c) 1989-94 James E. Wilson, Robert A. Koeneke
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Handle object descriptions, mostly string handling code

#include "headers.h"
#include "externs.h"

static void unsample(Inventory_t *i_ptr);

char magic_item_titles[MAX_TITLES][10];

bool isVowel(char ch) {
    switch (ch) {
        case 'a':
        case 'e':
        case 'i':
        case 'o':
        case 'u':
        case 'A':
        case 'E':
        case 'I':
        case 'O':
        case 'U':
            return true;
        default:
            return false;
    }
}

// Initialize all Potions, wands, staves, scrolls, etc.
void magicInitializeItemNames() {
    int j;
    char *tmp;

    set_seed(magic_seed);

    // The first 3 entries for colors are fixed, (slime & apple juice, water)
    for (int i = 3; i < MAX_COLORS; i++) {
        j = randint(MAX_COLORS - 3) + 2;
        tmp = colors[i];
        colors[i] = colors[j];
        colors[j] = tmp;
    }

    for (int i = 0; i < MAX_WOODS; i++) {
        j = randint(MAX_WOODS) - 1;
        tmp = woods[i];
        woods[i] = woods[j];
        woods[j] = tmp;
    }

    for (int i = 0; i < MAX_METALS; i++) {
        j = randint(MAX_METALS) - 1;
        tmp = metals[i];
        metals[i] = metals[j];
        metals[j] = tmp;
    }

    for (int i = 0; i < MAX_ROCKS; i++) {
        j = randint(MAX_ROCKS) - 1;
        tmp = rocks[i];
        rocks[i] = rocks[j];
        rocks[j] = tmp;
    }

    for (int i = 0; i < MAX_AMULETS; i++) {
        j = randint(MAX_AMULETS) - 1;
        tmp = amulets[i];
        amulets[i] = amulets[j];
        amulets[j] = tmp;
    }

    for (int i = 0; i < MAX_MUSH; i++) {
        j = randint(MAX_MUSH) - 1;
        tmp = mushrooms[i];
        mushrooms[i] = mushrooms[j];
        mushrooms[j] = tmp;
    }

    int k;
    vtype_t string;

    for (int h = 0; h < MAX_TITLES; h++) {
        string[0] = '\0';
        k = randint(2) + 1;

        for (int i = 0; i < k; i++) {
            for (int s = randint(2); s > 0; s--) {
                (void) strcat(string, syllables[randint(MAX_SYLLABLES) - 1]);
            }
            if (i < k - 1) {
                (void) strcat(string, " ");
            }
        }

        if (string[8] == ' ') {
            string[8] = '\0';
        } else {
            string[9] = '\0';
        }

        (void) strcpy(magic_item_titles[h], string);
    }

    reset_seed();
}

int16_t objectPositionOffset(Inventory_t *item) {
    switch (item->tval) {
        case TV_AMULET:
            return 0;
        case TV_RING:
            return 1;
        case TV_STAFF:
            return 2;
        case TV_WAND:
            return 3;
        case TV_SCROLL1:
        case TV_SCROLL2:
            return 4;
        case TV_POTION1:
        case TV_POTION2:
            return 5;
        case TV_FOOD:
            if ((item->subval & (ITEM_SINGLE_STACK_MIN - 1)) < MAX_MUSH) {
                return 6;
            }
            return -1;
        default:
            return -1;
    }
}

static void clearObjectTriedFlag(int16_t id) {
    objects_identified[id] &= ~OD_TRIED;
}

static void setObjectTriedFlag(int16_t id) {
    objects_identified[id] |= OD_TRIED;
}

static bool isObjectKnown(int16_t id) {
    return (objects_identified[id] & OD_KNOWN1) != 0;
}

// Remove "Secret" symbol for identity of object
void itemSetAsIdentified(Inventory_t *item) {
    int16_t id = objectPositionOffset(item);

    if (id < 0) {
        return;
    }

    id <<= 6;
    id += (uint8_t) (item->subval & (ITEM_SINGLE_STACK_MIN - 1));

    objects_identified[id] |= OD_KNOWN1;

    // clear the tried flag, since it is now known
    clearObjectTriedFlag(id);
}

// Items which don't have a 'color' are always known / itemSetAsIdentified(),
// so that they can be carried in order in the inventory.
bool itemSetColorlessAsIdentifed(Inventory_t *item) {
    int16_t id = objectPositionOffset(item);

    if (id < 0) {
        return OD_KNOWN1;
    }
    if (itemStoreBought(item)) {
        return OD_KNOWN1;
    }

    id <<= 6;
    id += (uint8_t) (item->subval & (ITEM_SINGLE_STACK_MIN - 1));

    return isObjectKnown(id);
}

// Remove "Secret" symbol for identity of plusses
void spellItemIdentifyAndRemoveRandomInscription(Inventory_t *item) {
    unsample(item);
    item->ident |= ID_KNOWN2;
}

bool spellItemIdentified(Inventory_t *item) {
    return (item->ident & ID_KNOWN2) != 0;
}

void spellItemRemoveIdentification(Inventory_t *item) {
    item->ident &= ~ID_KNOWN2;
}

void itemIdentificationClearEmpty(Inventory_t *item) {
    item->ident &= ~ID_EMPTY;
}

void itemIdentifyAsStoreBought(Inventory_t *item) {
    item->ident |= ID_STOREBOUGHT;
    spellItemIdentifyAndRemoveRandomInscription(item);
}

bool itemStoreBought(Inventory_t *item) {
    return (item->ident & ID_STOREBOUGHT) != 0;
}

// Remove an automatically generated inscription. -CJS-
static void unsample(Inventory_t *i_ptr) {
    // this also used to clear ID_DAMD flag, but I think it should remain set
    i_ptr->ident &= ~(ID_MAGIK | ID_EMPTY);

    int16_t id = objectPositionOffset(i_ptr);

    if (id < 0) {
        return;
    }

    id <<= 6;
    id += (uint8_t) (i_ptr->subval & (ITEM_SINGLE_STACK_MIN - 1));

    // clear the tried flag, since it is now known
    clearObjectTriedFlag(id);
}

// Somethings been sampled -CJS-
void itemSetAsTried(Inventory_t *item) {
    int16_t id = objectPositionOffset(item);

    if (id < 0) {
        return;
    }

    id <<= 6;
    id += (uint8_t) (item->subval & (ITEM_SINGLE_STACK_MIN - 1));

    setObjectTriedFlag(id);
}

// Somethings been identified.
// Extra complexity by CJS so that it can merge store/dungeon objects when appropriate.
void itemIdentify(int *item_id) {
    Inventory_t *i_ptr = &inventory[*item_id];

    if (i_ptr->flags & TR_CURSED) {
        add_inscribe(i_ptr, ID_DAMD);
    }

    if (itemSetColorlessAsIdentifed(i_ptr)) {
        return;
    }

    itemSetAsIdentified(i_ptr);

    int x1 = i_ptr->tval;
    int x2 = i_ptr->subval;

    // no merging possible
    if (x2 < ITEM_SINGLE_STACK_MIN || x2 >= ITEM_GROUP_MIN) {
        return;
    }

    int j;
    Inventory_t *t_ptr;

    for (int i = 0; i < inventory_count; i++) {
        t_ptr = &inventory[i];

        if (t_ptr->tval == x1 && t_ptr->subval == x2 && i != *item_id && ((int) t_ptr->number + (int) i_ptr->number) < 256) {
            // make *item_id the smaller number
            if (*item_id > i) {
                j = *item_id;
                *item_id = i;
                i = j;
            }

            msg_print("You combine similar objects from the shop and dungeon.");

            inventory[*item_id].number += inventory[i].number;
            inventory_count--;

            for (j = i; j < inventory_count; j++) {
                inventory[j] = inventory[j + 1];
            }

            inventoryItemCopyTo(OBJ_NOTHING, &inventory[j]);
        }
    }
}

// If an object has lost magical properties,
// remove the appropriate portion of the name. -CJS-
void itemRemoveMagicNaming(Inventory_t *item) {
    item->name2 = SN_NULL;
}

// defines for p1_use, determine how the p1 field is printed
#define IGNORED 0
#define CHARGES 1
#define PLUSSES 2
#define LIGHT 3
#define FLAGS 4
#define Z_PLUSSES 5

int bowDamageValue(int16_t p1) {
    if (p1 == 1 || p1 == 2) {
        return 2;
    }
    if (p1 == 3 || p1 == 5) {
        return 3;
    }
    if (p1 == 4 || p1 == 6) {
        return 4;
    }
    return -1;
}

// Returns a description of item for inventory
// `pref` indicates that there should be an article added (prefix).
// Note that since out_val can easily exceed 80 characters, itemDescription
// must always be called with a obj_desc_t as the first parameter.
void itemDescription(obj_desc_t description, Inventory_t *item, bool add_prefix) {
    int indexx = item->subval & (ITEM_SINGLE_STACK_MIN - 1);

    // base name, modifier string
    const char *basenm = game_objects[item->index].name;
    const char *modstr = CNIL;

    vtype_t damstr;
    damstr[0] = '\0';

    int p1_use = IGNORED;
    bool append_name = false;

    bool modify = !itemSetColorlessAsIdentifed(item);

    switch (item->tval) {
        case TV_MISC:
        case TV_CHEST:
            break;
        case TV_SLING_AMMO:
        case TV_BOLT:
        case TV_ARROW:
            (void) sprintf(damstr, " (%dd%d)", item->damage[0], item->damage[1]);
            break;
        case TV_LIGHT:
            p1_use = LIGHT;
            break;
        case TV_SPIKE:
            break;
        case TV_BOW:
            (void) sprintf(damstr, " (x%d)", bowDamageValue(item->p1));
            break;
        case TV_HAFTED:
        case TV_POLEARM:
        case TV_SWORD:
            (void) sprintf(damstr, " (%dd%d)", item->damage[0], item->damage[1]);
            p1_use = FLAGS;
            break;
        case TV_DIGGING:
            p1_use = Z_PLUSSES;
            (void) sprintf(damstr, " (%dd%d)", item->damage[0], item->damage[1]);
            break;
        case TV_BOOTS:
        case TV_GLOVES:
        case TV_CLOAK:
        case TV_HELM:
        case TV_SHIELD:
        case TV_HARD_ARMOR:
        case TV_SOFT_ARMOR:
            break;
        case TV_AMULET:
            if (modify) {
                basenm = "& %s Amulet";
                modstr = amulets[indexx];
            } else {
                basenm = "& Amulet";
                append_name = true;
            }
            p1_use = PLUSSES;
            break;
        case TV_RING:
            if (modify) {
                basenm = "& %s Ring";
                modstr = rocks[indexx];
            } else {
                basenm = "& Ring";
                append_name = true;
            }
            p1_use = PLUSSES;
            break;
        case TV_STAFF:
            if (modify) {
                basenm = "& %s Staff";
                modstr = woods[indexx];
            } else {
                basenm = "& Staff";
                append_name = true;
            }
            p1_use = CHARGES;
            break;
        case TV_WAND:
            if (modify) {
                basenm = "& %s Wand";
                modstr = metals[indexx];
            } else {
                basenm = "& Wand";
                append_name = true;
            }
            p1_use = CHARGES;
            break;
        case TV_SCROLL1:
        case TV_SCROLL2:
            if (modify) {
                basenm = "& Scroll~ titled \"%s\"";
                modstr = magic_item_titles[indexx];
            } else {
                basenm = "& Scroll~";
                append_name = true;
            }
            break;
        case TV_POTION1:
        case TV_POTION2:
            if (modify) {
                basenm = "& %s Potion~";
                modstr = colors[indexx];
            } else {
                basenm = "& Potion~";
                append_name = true;
            }
            break;
        case TV_FLASK:
            break;
        case TV_FOOD:
            if (modify) {
                if (indexx <= 15) {
                    basenm = "& %s Mushroom~";
                } else if (indexx <= 20) {
                    basenm = "& Hairy %s Mold~";
                }
                if (indexx <= 20) {
                    modstr = mushrooms[indexx];
                }
            } else {
                append_name = true;
                if (indexx <= 15) {
                    basenm = "& Mushroom~";
                } else if (indexx <= 20) {
                    basenm = "& Hairy Mold~";
                } else {
                    // Ordinary food does not have a name appended.
                    append_name = false;
                }
            }
            break;
        case TV_MAGIC_BOOK:
            modstr = basenm;
            basenm = "& Book~ of Magic Spells %s";
            break;
        case TV_PRAYER_BOOK:
            modstr = basenm;
            basenm = "& Holy Book~ of Prayers %s";
            break;
        case TV_OPEN_DOOR:
        case TV_CLOSED_DOOR:
        case TV_SECRET_DOOR:
        case TV_RUBBLE:
            break;
        case TV_GOLD:
        case TV_INVIS_TRAP:
        case TV_VIS_TRAP:
        case TV_UP_STAIR:
        case TV_DOWN_STAIR:
            (void) strcpy(description, game_objects[item->index].name);
            (void) strcat(description, ".");
            return;
        case TV_STORE_DOOR:
            (void) sprintf(description, "the entrance to the %s.", game_objects[item->index].name);
            return;
        default:
            (void) strcpy(description, "Error in objdes()");
            return;
    }

    obj_desc_t tmp_val;

    if (modstr != CNIL) {
        (void) sprintf(tmp_val, basenm, modstr);
    } else {
        (void) strcpy(tmp_val, basenm);
    }

    if (append_name) {
        (void) strcat(tmp_val, " of ");
        (void) strcat(tmp_val, game_objects[item->index].name);
    }

    if (item->number != 1) {
        insert_str(tmp_val, "ch~", "ches");
        insert_str(tmp_val, "~", "s");
    } else {
        insert_str(tmp_val, "~", CNIL);
    }

    if (!add_prefix) {
        if (!strncmp("some", tmp_val, 4)) {
            (void) strcpy(description, &tmp_val[5]);
        } else if (tmp_val[0] == '&') {
            // eliminate the '& ' at the beginning
            (void) strcpy(description, &tmp_val[2]);
        } else {
            (void) strcpy(description, tmp_val);
        }
        return;
    }

    vtype_t tmp_str;

    if (item->name2 != SN_NULL && spellItemIdentified(item)) {
        (void) strcat(tmp_val, " ");
        (void) strcat(tmp_val, special_item_names[item->name2]);
    }

    if (damstr[0] != '\0') {
        (void) strcat(tmp_val, damstr);
    }

    if (spellItemIdentified(item)) {
        // originally used %+d, but several machines don't support it
        if (item->ident & ID_SHOW_HITDAM) {
            (void) sprintf(tmp_str, " (%c%d,%c%d)", (item->tohit < 0) ? '-' : '+', abs(item->tohit), (item->todam < 0) ? '-' : '+', abs(item->todam));
        } else if (item->tohit != 0) {
            (void) sprintf(tmp_str, " (%c%d)", (item->tohit < 0) ? '-' : '+', abs(item->tohit));
        } else if (item->todam != 0) {
            (void) sprintf(tmp_str, " (%c%d)", (item->todam < 0) ? '-' : '+', abs(item->todam));
        } else {
            tmp_str[0] = '\0';
        }
        (void) strcat(tmp_val, tmp_str);
    }

    // Crowns have a zero base AC, so make a special test for them.
    if (item->ac != 0 || item->tval == TV_HELM) {
        (void) sprintf(tmp_str, " [%d", item->ac);
        (void) strcat(tmp_val, tmp_str);
        if (spellItemIdentified(item)) {
            // originally used %+d, but several machines don't support it
            (void) sprintf(tmp_str, ",%c%d", (item->toac < 0) ? '-' : '+', abs(item->toac));
            (void) strcat(tmp_val, tmp_str);
        }
        (void) strcat(tmp_val, "]");
    } else if (item->toac != 0 && spellItemIdentified(item)) {
        // originally used %+d, but several machines don't support it
        (void) sprintf(tmp_str, " [%c%d]", (item->toac < 0) ? '-' : '+', abs(item->toac));
        (void) strcat(tmp_val, tmp_str);
    }

    // override defaults, check for p1 flags in the ident field
    if (item->ident & ID_NOSHOW_P1) {
        p1_use = IGNORED;
    } else if (item->ident & ID_SHOW_P1) {
        p1_use = Z_PLUSSES;
    }

    tmp_str[0] = '\0';

    if (p1_use == LIGHT) {
        (void) sprintf(tmp_str, " with %d turns of light", item->p1);
    } else if (p1_use == IGNORED) {
        // NOOP
    } else if (spellItemIdentified(item)) {
        if (p1_use == Z_PLUSSES) {
            // originally used %+d, but several machines don't support it
            (void) sprintf(tmp_str, " (%c%d)", (item->p1 < 0) ? '-' : '+', abs(item->p1));
        } else if (p1_use == CHARGES) {
            (void) sprintf(tmp_str, " (%d charges)", item->p1);
        } else if (item->p1 != 0) {
            if (p1_use == PLUSSES) {
                (void) sprintf(tmp_str, " (%c%d)", (item->p1 < 0) ? '-' : '+', abs(item->p1));
            } else if (p1_use == FLAGS) {
                if (item->flags & TR_STR) {
                    (void) sprintf(tmp_str, " (%c%d to STR)", (item->p1 < 0) ? '-' : '+', abs(item->p1));
                } else if (item->flags & TR_STEALTH) {
                    (void) sprintf(tmp_str, " (%c%d to stealth)", (item->p1 < 0) ? '-' : '+', abs(item->p1));
                }
            }
        }
    }
    (void) strcat(tmp_val, tmp_str);

    // ampersand is always the first character
    if (tmp_val[0] == '&') {
        // use &tmp_val[1], so that & does not appear in output
        if (item->number > 1) {
            (void) sprintf(description, "%d%s", (int) item->number, &tmp_val[1]);
        } else if (item->number < 1) {
            (void) sprintf(description, "%s%s", "no more", &tmp_val[1]);
        } else if (isVowel(tmp_val[2])) {
            (void) sprintf(description, "an%s", &tmp_val[1]);
        } else {
            (void) sprintf(description, "a%s", &tmp_val[1]);
        }
    } else if (item->number < 1) {
        // handle 'no more' case specially

        // check for "some" at start
        if (!strncmp("some", tmp_val, 4)) {
            (void) sprintf(description, "no more %s", &tmp_val[5]);
        } else {
            // here if no article
            (void) sprintf(description, "no more %s", tmp_val);
        }
    } else {
        (void) strcpy(description, tmp_val);
    }

    tmp_str[0] = '\0';

    if ((indexx = objectPositionOffset(item)) >= 0) {
        indexx <<= 6;
        indexx += (item->subval & (ITEM_SINGLE_STACK_MIN - 1));

        // don't print tried string for store bought items
        if ((objects_identified[indexx] & OD_TRIED) && !itemStoreBought(item)) {
            (void) strcat(tmp_str, "tried ");
        }
    }

    if (item->ident & (ID_MAGIK | ID_EMPTY | ID_DAMD)) {
        if (item->ident & ID_MAGIK) {
            (void) strcat(tmp_str, "magik ");
        }
        if (item->ident & ID_EMPTY) {
            (void) strcat(tmp_str, "empty ");
        }
        if (item->ident & ID_DAMD) {
            (void) strcat(tmp_str, "damned ");
        }
    }

    if (item->inscrip[0] != '\0') {
        (void) strcat(tmp_str, item->inscrip);
    } else if ((indexx = (int) strlen(tmp_str)) > 0) {
        // remove the extra blank at the end
        tmp_str[indexx - 1] = '\0';
    }

    if (tmp_str[0]) {
        (void) sprintf(tmp_val, " {%s}", tmp_str);
        (void) strcat(description, tmp_val);
    }

    (void) strcat(description, ".");
}

void inventoryItemCopyTo(int from_item_id, Inventory_t *to_item) {
    GameObject_t *from = &game_objects[from_item_id];

    to_item->index = (uint16_t) from_item_id;
    to_item->name2 = SN_NULL;
    to_item->inscrip[0] = '\0';
    to_item->flags = from->flags;
    to_item->tval = from->tval;
    to_item->tchar = from->tchar;
    to_item->p1 = from->p1;
    to_item->cost = from->cost;
    to_item->subval = from->subval;
    to_item->number = from->number;
    to_item->weight = from->weight;
    to_item->tohit = from->tohit;
    to_item->todam = from->todam;
    to_item->ac = from->ac;
    to_item->toac = from->toac;
    to_item->damage[0] = from->damage[0];
    to_item->damage[1] = from->damage[1];
    to_item->level = from->level;
    to_item->ident = 0;
}

// Describe number of remaining charges. -RAK-
void itemChargesRemainingDescription(int item_id) {
    if (!spellItemIdentified(&inventory[item_id])) {
        return;
    }

    int rem_num = inventory[item_id].p1;

    vtype_t out_val;
    (void) sprintf(out_val, "You have %d charges remaining.", rem_num);
    msg_print(out_val);
}

// Describe amount of item remaining. -RAK-
void desc_remain(int item_val) {
    Inventory_t *i_ptr = &inventory[item_val];

    i_ptr->number--;

    obj_desc_t tmp_str;
    itemDescription(tmp_str, i_ptr, true);

    i_ptr->number++;

    // the string already has a dot at the end.
    obj_desc_t out_val;
    (void) sprintf(out_val, "You have %s", tmp_str);
    msg_print(out_val);
}
