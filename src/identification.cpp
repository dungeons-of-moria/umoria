// Copyright (c) 1981-86 Robert A. Koeneke
// Copyright (c) 1987-94 James E. Wilson
//
// This work is free software released under the GNU General Public License
// version 2.0, and comes with ABSOLUTELY NO WARRANTY.
//
// See LICENSE and AUTHORS for more information.

// Handle object identification and descriptions
// ...mostly string handling code

#include "headers.h"

char magic_item_titles[MAX_TITLES][10];

// Identified objects flags
uint8_t objects_identified[OBJECT_IDENT_SIZE];

static const char *objectDescription(char command) {
    // every printing ASCII character is listed here, in the
    // order in which they appear in the ASCII character set.
    switch (command) {
        case ' ':
            return "  - An open pit.";
        case '!':
            return "! - A potion.";
        case '"':
            return "\" - An amulet, periapt, or necklace.";
        case '#':
            return "# - A stone wall.";
        case '$':
            return "$ - Treasure.";
        case '%':
            if (!config::options::highlight_seams) {
                return "% - Not used.";
            }
            return "% - A magma or quartz vein.";
        case '&':
            return "& - Treasure chest.";
        case '\'':
            return "' - An open door.";
        case '(':
            return "( - Soft armor.";
        case ')':
            return ") - A shield.";
        case '*':
            return "* - Gems.";
        case '+':
            return "+ - A closed door.";
        case ',':
            return ", - Food or mushroom patch.";
        case '-':
            return "- - A wand";
        case '.':
            return ". - Floor.";
        case '/':
            return "/ - A pole weapon.";
            //        case '0':
            //            return "0 - Not used.";
        case '1':
            return "1 - Entrance to General Store.";
        case '2':
            return "2 - Entrance to Armory.";
        case '3':
            return "3 - Entrance to Weaponsmith.";
        case '4':
            return "4 - Entrance to Temple.";
        case '5':
            return "5 - Entrance to Alchemy shop.";
        case '6':
            return "6 - Entrance to Magic-Users store.";
            // case '7':
            //     return "7 - Not used.";
            // case '8':
            //     return "8 - Not used.";
            // case '9':
            //     return "9 - Not used.";
        case ':':
            return ": - Rubble.";
        case ';':
            return "; - A loose rock.";
        case '<':
            return "< - An up staircase.";
        case '=':
            return "= - A ring.";
        case '>':
            return "> - A down staircase.";
        case '?':
            return "? - A scroll.";
        case '@':
            return py.misc.name;
        case 'A':
            return "A - Giant Ant Lion.";
        case 'B':
            return "B - The Balrog.";
        case 'C':
            return "C - Gelatinous Cube.";
        case 'D':
            return "D - An Ancient Dragon (Beware).";
        case 'E':
            return "E - Elemental.";
        case 'F':
            return "F - Giant Fly.";
        case 'G':
            return "G - Ghost.";
        case 'H':
            return "H - Hobgoblin.";
            // case 'I':
            //     return "I - Invisible Stalker.";
        case 'J':
            return "J - Jelly.";
        case 'K':
            return "K - Killer Beetle.";
        case 'L':
            return "L - Lich.";
        case 'M':
            return "M - Mummy.";
            // case 'N':
            //     return "N - Not used.";
        case 'O':
            return "O - Ooze.";
        case 'P':
            return "P - Giant humanoid.";
        case 'Q':
            return "Q - Quylthulg (Pulsing Flesh Mound).";
        case 'R':
            return "R - Reptile.";
        case 'S':
            return "S - Giant Scorpion.";
        case 'T':
            return "T - Troll.";
        case 'U':
            return "U - Umber Hulk.";
        case 'V':
            return "V - Vampire.";
        case 'W':
            return "W - Wight or Wraith.";
        case 'X':
            return "X - Xorn.";
        case 'Y':
            return "Y - Yeti.";
            // case 'Z':
            //     return "Z - Not used.";
        case '[':
            return "[ - Hard armor.";
        case '\\':
            return "\\ - A hafted weapon.";
        case ']':
            return "] - Misc. armor.";
        case '^':
            return "^ - A trap.";
        case '_':
            return "_ - A staff.";
            // case '`':
            //     return "` - Not used.";
        case 'a':
            return "a - Giant Ant.";
        case 'b':
            return "b - Giant Bat.";
        case 'c':
            return "c - Giant Centipede.";
        case 'd':
            return "d - Dragon.";
        case 'e':
            return "e - Floating Eye.";
        case 'f':
            return "f - Giant Frog.";
        case 'g':
            return "g - Golem.";
        case 'h':
            return "h - Harpy.";
        case 'i':
            return "i - Icky Thing.";
        case 'j':
            return "j - Jackal.";
        case 'k':
            return "k - Kobold.";
        case 'l':
            return "l - Giant Louse.";
        case 'm':
            return "m - Mold.";
        case 'n':
            return "n - Naga.";
        case 'o':
            return "o - Orc or Ogre.";
        case 'p':
            return "p - Person (Humanoid).";
        case 'q':
            return "q - Quasit.";
        case 'r':
            return "r - Rodent.";
        case 's':
            return "s - Skeleton.";
        case 't':
            return "t - Giant Tick.";
            // case 'u':
            //     return "u - Not used.";
            // case 'v':
            //     return "v - Not used.";
        case 'w':
            return "w - Worm or Worm Mass.";
            // case 'x':
            //     return "x - Not used.";
        case 'y':
            return "y - Yeek.";
        case 'z':
            return "z - Zombie.";
        case '{':
            return "{ - Arrow, bolt, or bullet.";
        case '|':
            return "| - A sword or dagger.";
        case '}':
            return "} - Bow, crossbow, or sling.";
        case '~':
            return "~ - Miscellaneous item.";
        default:
            return "Not Used.";
    }
}

void identifyGameObject() {
    char command;
    if (!getCommand("Enter character to be identified :", command)) {
        return;
    }

    putStringClearToEOL(objectDescription(command), Coord_t{0, 0});

    recallMonsterAttributes(command);
}

// Initialize all Potions, wands, staves, scrolls, etc.
void magicInitializeItemNames() {
    int id;

    seedSet(game.magic_seed);

    // The first 3 entries for colors are fixed, (slime & apple juice, water)
    for (int i = 3; i < MAX_COLORS; i++) {
        id = randomNumber(MAX_COLORS - 3) + 2;
        const char *color = colors[i];
        colors[i] = colors[id];
        colors[id] = color;
    }

    for (auto &w : woods) {
        id = randomNumber(MAX_WOODS) - 1;
        const char *wood = w;
        w = woods[id];
        woods[id] = wood;
    }

    for (auto &m : metals) {
        id = randomNumber(MAX_METALS) - 1;
        const char *metal = m;
        m = metals[id];
        metals[id] = metal;
    }

    for (auto &r : rocks) {
        id = randomNumber(MAX_ROCKS) - 1;
        const char *rock = r;
        r = rocks[id];
        rocks[id] = rock;
    }

    for (auto &a : amulets) {
        id = randomNumber(MAX_AMULETS) - 1;
        const char *amulet = a;
        a = amulets[id];
        amulets[id] = amulet;
    }

    for (auto &m : mushrooms) {
        id = randomNumber(MAX_MUSHROOMS) - 1;
        const char *mushroom = m;
        m = mushrooms[id];
        mushrooms[id] = mushroom;
    }

    int k;
    vtype_t title = {'\0'};

    for (auto &item_title : magic_item_titles) {
        title[0] = '\0';
        k = randomNumber(2) + 1;

        for (int i = 0; i < k; i++) {
            for (int s = randomNumber(2); s > 0; s--) {
                (void) strcat(title, syllables[randomNumber(MAX_SYLLABLES) - 1]);
            }
            if (i < k - 1) {
                (void) strcat(title, " ");
            }
        }

        if (title[8] == ' ') {
            title[8] = '\0';
        } else {
            title[9] = '\0';
        }

        (void) strcpy(item_title, title);
    }

    seedResetToOldSeed();
}

int16_t objectPositionOffset(int category_id, int sub_category_id) {
    switch (category_id) {
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
            if ((sub_category_id & (ITEM_SINGLE_STACK_MIN - 1)) < MAX_MUSHROOMS) {
                return 6;
            }
            return -1;
        default:
            return -1;
    }
}

static void clearObjectTriedFlag(int16_t id) {
    objects_identified[id] &= ~config::identification::OD_TRIED;
}

static void setObjectTriedFlag(int16_t id) {
    objects_identified[id] |= config::identification::OD_TRIED;
}

static bool isObjectKnown(int16_t id) {
    return (objects_identified[id] & config::identification::OD_KNOWN1) != 0u;
}

// Remove "Secret" symbol for identity of object
void itemSetAsIdentified(int category_id, int sub_category_id) {
    int16_t id = objectPositionOffset(category_id, sub_category_id);

    if (id < 0) {
        return;
    }

    id <<= 6;
    id += (uint8_t)(sub_category_id & (ITEM_SINGLE_STACK_MIN - 1));

    objects_identified[id] |= config::identification::OD_KNOWN1;

    // clear the tried flag, since it is now known
    clearObjectTriedFlag(id);
}

// Remove an automatically generated inscription. -CJS-
static void unsample(Inventory_t &item) {
    // this also used to clear config::identification::ID_DAMD flag, but I think it should remain set
    item.identification &= ~(config::identification::ID_MAGIK | config::identification::ID_EMPTY);

    int16_t id = objectPositionOffset(item.category_id, item.sub_category_id);

    if (id < 0) {
        return;
    }

    id <<= 6;
    id += (uint8_t)(item.sub_category_id & (ITEM_SINGLE_STACK_MIN - 1));

    // clear the tried flag, since it is now known
    clearObjectTriedFlag(id);
}

// Remove "Secret" symbol for identity of plusses
void spellItemIdentifyAndRemoveRandomInscription(Inventory_t &item) {
    unsample(item);
    item.identification |= config::identification::ID_KNOWN2;
}

bool spellItemIdentified(Inventory_t const &item) {
    return (item.identification & config::identification::ID_KNOWN2) != 0;
}

void spellItemRemoveIdentification(Inventory_t &item) {
    item.identification &= ~config::identification::ID_KNOWN2;
}

void itemIdentificationClearEmpty(Inventory_t &item) {
    item.identification &= ~config::identification::ID_EMPTY;
}

void itemIdentifyAsStoreBought(Inventory_t &item) {
    item.identification |= config::identification::ID_STORE_BOUGHT;
    spellItemIdentifyAndRemoveRandomInscription(item);
}

static bool itemStoreBought(int identification) {
    return (identification & config::identification::ID_STORE_BOUGHT) != 0;
}

// Items which don't have a 'color' are always known / itemSetAsIdentified(),
// so that they can be carried in order in the inventory.
bool itemSetColorlessAsIdentified(int category_id, int sub_category_id, int identification) {
    int16_t id = objectPositionOffset(category_id, sub_category_id);

    if (id < 0) {
        return config::identification::OD_KNOWN1 != 0u;
    }
    if (itemStoreBought(identification)) {
        return config::identification::OD_KNOWN1 != 0u;
    }

    id <<= 6;
    id += (uint8_t)(sub_category_id & (ITEM_SINGLE_STACK_MIN - 1));

    return isObjectKnown(id);
}

// Somethings been sampled -CJS-
void itemSetAsTried(Inventory_t const &item) {
    int16_t id = objectPositionOffset(item.category_id, item.sub_category_id);

    if (id < 0) {
        return;
    }

    id <<= 6;
    id += (uint8_t)(item.sub_category_id & (ITEM_SINGLE_STACK_MIN - 1));

    setObjectTriedFlag(id);
}

// Somethings been identified.
// Extra complexity by CJS so that it can merge store/dungeon objects when appropriate.
void itemIdentify(Inventory_t &item, int &item_id) {
    if ((item.flags & config::treasure::flags::TR_CURSED) != 0u) {
        itemAppendToInscription(item, config::identification::ID_DAMD);
    }

    if (itemSetColorlessAsIdentified(item.category_id, item.sub_category_id, item.identification)) {
        return;
    }

    itemSetAsIdentified(item.category_id, item.sub_category_id);

    // no merging possible
    if (!inventoryItemSingleStackable(item)) {
        return;
    }

    int j;

    for (int i = 0; i < py.pack.unique_items; i++) {
        Inventory_t const &t_ptr = py.inventory[i];

        bool matching_cat = t_ptr.category_id == item.category_id;
        bool matching_sub_cat = t_ptr.sub_category_id == item.sub_category_id;
        int total_items_count = (int) t_ptr.items_count + item.items_count;

        if (matching_cat && matching_sub_cat && i != item_id && total_items_count < 256) {
            // make *item_id the smaller number
            if (item_id > i) {
                j = item_id;
                item_id = i;
                i = j;
            }

            printMessage("You combine similar objects from the shop and dungeon.");

            py.inventory[item_id].items_count += py.inventory[i].items_count;
            py.pack.unique_items--;

            for (j = i; j < py.pack.unique_items; j++) {
                py.inventory[j] = py.inventory[j + 1];
            }

            inventoryItemCopyTo(config::dungeon::objects::OBJ_NOTHING, py.inventory[j]);
        }
    }
}

// If an object has lost magical properties,
// remove the appropriate portion of the name. -CJS-
void itemRemoveMagicNaming(Inventory_t &item) {
    item.special_name_id = SpecialNameIds::SN_NULL;
}

int bowDamageValue(int16_t misc_use) {
    if (misc_use == 1 || misc_use == 2) {
        return 2;
    }
    if (misc_use == 3 || misc_use == 5) {
        return 3;
    }
    if (misc_use == 4 || misc_use == 6) {
        return 4;
    }
    return -1;
}

// determines how the `item.misc_use` field is printed
enum class ItemMiscUse {
    Ignored,
    Charges,
    Plusses,
    Light,
    Flags,
    ZPlusses,
};

// Set the `description` for an inventory item.
// The `add_prefix` param indicates that an article must be added.
// Note that since out_val can easily exceed 80 characters, itemDescription
// must always be called with a obj_desc_t as the first parameter.
void itemDescription(obj_desc_t description, Inventory_t const &item, bool add_prefix) {
    int indexx = item.sub_category_id & (ITEM_SINGLE_STACK_MIN - 1);

    // base name, modifier string
    const char *basenm = game_objects[item.id].name;
    const char *modstr = CNIL;

    vtype_t damstr = {'\0'};
    damstr[0] = '\0';

    bool append_name = false;
    bool modify = !itemSetColorlessAsIdentified(item.category_id, item.sub_category_id, item.identification);
    ItemMiscUse misc_type = ItemMiscUse::Ignored;

    switch (item.category_id) {
        case TV_MISC:
        case TV_CHEST:
            break;
        case TV_SLING_AMMO:
        case TV_BOLT:
        case TV_ARROW:
            (void) sprintf(damstr, " (%dd%d)", item.damage.dice, item.damage.sides);
            break;
        case TV_LIGHT:
            misc_type = ItemMiscUse::Light;
            break;
        case TV_SPIKE:
            break;
        case TV_BOW:
            (void) sprintf(damstr, " (x%d)", bowDamageValue(item.misc_use));
            break;
        case TV_HAFTED:
        case TV_POLEARM:
        case TV_SWORD:
            (void) sprintf(damstr, " (%dd%d)", item.damage.dice, item.damage.sides);
            misc_type = ItemMiscUse::Flags;
            break;
        case TV_DIGGING:
            misc_type = ItemMiscUse::ZPlusses;
            (void) sprintf(damstr, " (%dd%d)", item.damage.sides, item.damage.sides);
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
            misc_type = ItemMiscUse::Plusses;
            break;
        case TV_RING:
            if (modify) {
                basenm = "& %s Ring";
                modstr = rocks[indexx];
            } else {
                basenm = "& Ring";
                append_name = true;
            }
            misc_type = ItemMiscUse::Plusses;
            break;
        case TV_STAFF:
            if (modify) {
                basenm = "& %s Staff";
                modstr = woods[indexx];
            } else {
                basenm = "& Staff";
                append_name = true;
            }
            misc_type = ItemMiscUse::Charges;
            break;
        case TV_WAND:
            if (modify) {
                basenm = "& %s Wand";
                modstr = metals[indexx];
            } else {
                basenm = "& Wand";
                append_name = true;
            }
            misc_type = ItemMiscUse::Charges;
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
            (void) strcpy(description, game_objects[item.id].name);
            (void) strcat(description, ".");
            return;
        case TV_STORE_DOOR:
            (void) sprintf(description, "the entrance to the %s.", game_objects[item.id].name);
            return;
        default:
            (void) strcpy(description, "Error in objdes()");
            return;
    }

    obj_desc_t tmp_val = {'\0'};

    if (modstr != CNIL) {
        (void) sprintf(tmp_val, basenm, modstr);
    } else {
        (void) strcpy(tmp_val, basenm);
    }

    if (append_name) {
        (void) strcat(tmp_val, " of ");
        (void) strcat(tmp_val, game_objects[item.id].name);
    }

    if (item.items_count != 1) {
        insertStringIntoString(tmp_val, "ch~", "ches");
        insertStringIntoString(tmp_val, "~", "s");
    } else {
        insertStringIntoString(tmp_val, "~", CNIL);
    }

    if (!add_prefix) {
        if (strncmp("some", tmp_val, 4) == 0) {
            (void) strcpy(description, &tmp_val[5]);
        } else if (tmp_val[0] == '&') {
            // eliminate the '& ' at the beginning
            (void) strcpy(description, &tmp_val[2]);
        } else {
            (void) strcpy(description, tmp_val);
        }
        return;
    }

    vtype_t tmp_str = {'\0'};

    // TODO(cook): `spellItemIdentified()` is called several times in this
    // function, but `item` is immutable, so we should be able to call and
    // assign it once, then use that value everywhere below.
    if (item.special_name_id != SpecialNameIds::SN_NULL && spellItemIdentified(item)) {
        (void) strcat(tmp_val, " ");
        (void) strcat(tmp_val, special_item_names[item.special_name_id]);
    }

    if (damstr[0] != '\0') {
        (void) strcat(tmp_val, damstr);
    }

    if (spellItemIdentified(item)) {
        auto abs_to_hit = (int) std::abs((std::intmax_t) item.to_hit);
        auto abs_to_damage = (int) std::abs((std::intmax_t) item.to_damage);

        if ((item.identification & config::identification::ID_SHOW_HIT_DAM) != 0) {
            (void) sprintf(tmp_str, " (%c%d,%c%d)", (item.to_hit < 0) ? '-' : '+', abs_to_hit, (item.to_damage < 0) ? '-' : '+', abs_to_damage);
        } else if (item.to_hit != 0) {
            (void) sprintf(tmp_str, " (%c%d)", (item.to_hit < 0) ? '-' : '+', abs_to_hit);
        } else if (item.to_damage != 0) {
            (void) sprintf(tmp_str, " (%c%d)", (item.to_damage < 0) ? '-' : '+', abs_to_damage);
        } else {
            tmp_str[0] = '\0';
        }
        (void) strcat(tmp_val, tmp_str);
    }

    // Crowns have a zero base AC, so make a special test for them.
    auto abs_to_ac = (int) std::abs((std::intmax_t) item.to_ac);
    if (item.ac != 0 || item.category_id == TV_HELM) {
        (void) sprintf(tmp_str, " [%d", item.ac);
        (void) strcat(tmp_val, tmp_str);
        if (spellItemIdentified(item)) {
            // originally used %+d, but several machines don't support it
            (void) sprintf(tmp_str, ",%c%d", (item.to_ac < 0) ? '-' : '+', abs_to_ac);
            (void) strcat(tmp_val, tmp_str);
        }
        (void) strcat(tmp_val, "]");
    } else if (item.to_ac != 0 && spellItemIdentified(item)) {
        // originally used %+d, but several machines don't support it
        (void) sprintf(tmp_str, " [%c%d]", (item.to_ac < 0) ? '-' : '+', abs_to_ac);
        (void) strcat(tmp_val, tmp_str);
    }

    // override defaults, check for `misc_type` flags in the `item.identification` field
    if ((item.identification & config::identification::ID_NO_SHOW_P1) != 0) {
        misc_type = ItemMiscUse::Ignored;
    } else if ((item.identification & config::identification::ID_SHOW_P1) != 0) {
        misc_type = ItemMiscUse::ZPlusses;
    }

    tmp_str[0] = '\0';

    if (misc_type == ItemMiscUse::Light) {
        (void) sprintf(tmp_str, " with %d turns of light", item.misc_use);
    } else if (misc_type == ItemMiscUse::Ignored) {
        // NOOP
    } else if (spellItemIdentified(item)) {
        auto abs_misc_use = (int) std::abs((std::intmax_t) item.misc_use);

        if (misc_type == ItemMiscUse::ZPlusses) {
            // originally used %+d, but several machines don't support it
            (void) sprintf(tmp_str, " (%c%d)", (item.misc_use < 0) ? '-' : '+', abs_misc_use);
        } else if (misc_type == ItemMiscUse::Charges) {
            (void) sprintf(tmp_str, " (%d charges)", item.misc_use);
        } else if (item.misc_use != 0) {
            if (misc_type == ItemMiscUse::Plusses) {
                (void) sprintf(tmp_str, " (%c%d)", (item.misc_use < 0) ? '-' : '+', abs_misc_use);
            } else if (misc_type == ItemMiscUse::Flags) {
                if ((item.flags & config::treasure::flags::TR_STR) != 0u) {
                    (void) sprintf(tmp_str, " (%c%d to STR)", (item.misc_use < 0) ? '-' : '+', abs_misc_use);
                } else if ((item.flags & config::treasure::flags::TR_STEALTH) != 0u) {
                    (void) sprintf(tmp_str, " (%c%d to stealth)", (item.misc_use < 0) ? '-' : '+', abs_misc_use);
                }
            }
        }
    }
    (void) strcat(tmp_val, tmp_str);

    // ampersand is always the first character
    if (tmp_val[0] == '&') {
        // use &tmp_val[1], so that & does not appear in output
        if (item.items_count > 1) {
            (void) sprintf(description, "%d%s", (int) item.items_count, &tmp_val[1]);
        } else if (item.items_count < 1) {
            (void) sprintf(description, "%s%s", "no more", &tmp_val[1]);
        } else if (isVowel(tmp_val[2])) {
            (void) sprintf(description, "an%s", &tmp_val[1]);
        } else {
            (void) sprintf(description, "a%s", &tmp_val[1]);
        }
    } else if (item.items_count < 1) {
        // handle 'no more' case specially

        // check for "some" at start
        if (strncmp("some", tmp_val, 4) == 0) {
            (void) sprintf(description, "no more %s", &tmp_val[5]);
        } else {
            // here if no article
            (void) sprintf(description, "no more %s", tmp_val);
        }
    } else {
        (void) strcpy(description, tmp_val);
    }

    tmp_str[0] = '\0';

    if ((indexx = objectPositionOffset(item.category_id, item.sub_category_id)) >= 0) {
        indexx <<= 6;
        indexx += (item.sub_category_id & (ITEM_SINGLE_STACK_MIN - 1));

        // don't print tried string for store bought items
        if (((objects_identified[indexx] & config::identification::OD_TRIED) != 0) && !itemStoreBought(item.identification)) {
            (void) strcat(tmp_str, "tried ");
        }
    }

    if ((item.identification & (config::identification::ID_MAGIK | config::identification::ID_EMPTY | config::identification::ID_DAMD)) != 0) {
        if ((item.identification & config::identification::ID_MAGIK) != 0) {
            (void) strcat(tmp_str, "magik ");
        }
        if ((item.identification & config::identification::ID_EMPTY) != 0) {
            (void) strcat(tmp_str, "empty ");
        }
        if ((item.identification & config::identification::ID_DAMD) != 0) {
            (void) strcat(tmp_str, "damned ");
        }
    }

    if (item.inscription[0] != '\0') {
        (void) strcat(tmp_str, item.inscription);
    } else if ((indexx = (int) strlen(tmp_str)) > 0) {
        // remove the extra blank at the end
        tmp_str[indexx - 1] = '\0';
    }

    if (tmp_str[0] != 0) {
        (void) sprintf(tmp_val, " {%s}", tmp_str);
        (void) strcat(description, tmp_val);
    }

    (void) strcat(description, ".");
}

// Describe number of remaining charges. -RAK-
void itemChargesRemainingDescription(int item_id) {
    if (!spellItemIdentified(py.inventory[item_id])) {
        return;
    }

    int rem_num = py.inventory[item_id].misc_use;

    vtype_t out_val = {'\0'};
    (void) sprintf(out_val, "You have %d charges remaining.", rem_num);
    printMessage(out_val);
}

// Describe amount of item remaining. -RAK-
void itemTypeRemainingCountDescription(int item_id) {
    Inventory_t &item = py.inventory[item_id];

    item.items_count--;

    obj_desc_t tmp_str = {'\0'};
    itemDescription(tmp_str, item, true);

    item.items_count++;

    // the string already has a dot at the end.
    obj_desc_t out_val = {'\0'};
    (void) sprintf(out_val, "You have %s", tmp_str);
    printMessage(out_val);
}

// Add a comment to an object description. -CJS-
void itemInscribe() {
    if (py.pack.unique_items == 0 && py.equipment_count == 0) {
        printMessage("You are not carrying anything to inscribe.");
        return;
    }

    int item_id;
    if (!inventoryGetInputForItemId(item_id, "Which one? ", 0, PLAYER_INVENTORY_SIZE, CNIL, CNIL)) {
        return;
    }

    obj_desc_t msg = {'\0'};
    itemDescription(msg, py.inventory[item_id], true);

    obj_desc_t inscription = {'\0'};
    (void) sprintf(inscription, "Inscribing %s", msg);

    printMessage(inscription);

    if (py.inventory[item_id].inscription[0] != '\0') {
        (void) sprintf(inscription, "Replace %s New inscription:", py.inventory[item_id].inscription);
    } else {
        (void) strcpy(inscription, "Inscription: ");
    }

    int msg_len = 78 - (int) strlen(msg);
    if (msg_len > 12) {
        msg_len = 12;
    }

    putStringClearToEOL(inscription, Coord_t{0, 0});

    if (getStringInput(inscription, Coord_t{0, (int) strlen(inscription)}, msg_len)) {
        itemReplaceInscription(py.inventory[item_id], inscription);
    }
}

// Append an additional comment to an object description. -CJS-
void itemAppendToInscription(Inventory_t &item, uint8_t item_ident_type) {
    item.identification |= item_ident_type;
}

// Replace any existing comment in an object description with a new one. -CJS-
void itemReplaceInscription(Inventory_t &item, const char *inscription) {
    (void) strcpy(item.inscription, inscription);
}

void objectBlockedByMonster(int monster_id) {
    vtype_t description = {'\0'};
    vtype_t msg = {'\0'};

    Monster_t const &monster = monsters[monster_id];
    const char *name = creatures_list[monster.creature_id].name;

    if (monster.lit) {
        (void) sprintf(description, "The %s", name);
    } else {
        (void) strcpy(description, "Something");
    }

    (void) sprintf(msg, "%s is in your way!", description);
    printMessage(msg);
}
