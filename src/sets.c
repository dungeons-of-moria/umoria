/* source/sets.c: code to emulate the original Pascal sets
 *
 * Copyright (C) 1989-2008 James E. Wilson, Robert A. Koeneke,
 *                         David J. Grabiner
 *
 * This file is part of Umoria.
 *
 * Umoria is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Umoria is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Umoria.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "standard_library.h"

#include "config.h"
#include "constant.h"
#include "types.h"

int set_room(element)
int element;
{
    if ((element == DARK_FLOOR) || (element == LIGHT_FLOOR)) {
        return true;
    }
    return false;
}

int set_corr(element)
int element;
{
    if (element == CORR_FLOOR || element == BLOCKED_FLOOR) {
        return true;
    }
    return false;
}

int set_floor(element)
int element;
{
    if (element <= MAX_CAVE_FLOOR) {
        return true;
    } else {
        return false;
    }
}

int set_corrodes(item)
inven_type *item;
{
    switch (item->tval) {
    case TV_SWORD:
    case TV_HELM:
    case TV_SHIELD:
    case TV_HARD_ARMOR:
    case TV_WAND:
        return true;
    }
    return false;
}

int set_flammable(item)
inven_type *item;
{
    switch (item->tval) {
    case TV_ARROW:
    case TV_BOW:
    case TV_HAFTED:
    case TV_POLEARM:
    case TV_BOOTS:
    case TV_GLOVES:
    case TV_CLOAK:
    case TV_SOFT_ARMOR:
        /* Items of (RF) should not be destroyed. */
        if (item->flags & TR_RES_FIRE) {
            return false;
        } else {
            return true;
        }
    case TV_STAFF:
    case TV_SCROLL1:
    case TV_SCROLL2:
        return true;
    }
    return false;
}

int set_frost_destroy(item)
inven_type *item;
{
    if ((item->tval == TV_POTION1) || (item->tval == TV_POTION2) ||
        (item->tval == TV_FLASK)) {
        return true;
    }
    return false;
}

int set_acid_affect(item) inven_type *item;
{
    switch (item->tval) {
    case TV_MISC:
    case TV_CHEST:
        return true;
    case TV_BOLT:
    case TV_ARROW:
    case TV_BOW:
    case TV_HAFTED:
    case TV_POLEARM:
    case TV_BOOTS:
    case TV_GLOVES:
    case TV_CLOAK:
    case TV_SOFT_ARMOR:
        if (item->flags & TR_RES_ACID) {
            return false;
        } else {
            return true;
        }
    }
    return false;
}

int set_lightning_destroy(item)
inven_type *item;
{
    if ((item->tval == TV_RING) || (item->tval == TV_WAND) ||
        (item->tval == TV_SPIKE)) {
        return true;
    } else {
        return false;
    }
}

int set_null(item)
inven_type *item;
{
    return false;
}

int set_acid_destroy(item)
inven_type *item;
{
    switch (item->tval) {
    case TV_ARROW:
    case TV_BOW:
    case TV_HAFTED:
    case TV_POLEARM:
    case TV_BOOTS:
    case TV_GLOVES:
    case TV_CLOAK:
    case TV_HELM:
    case TV_SHIELD:
    case TV_HARD_ARMOR:
    case TV_SOFT_ARMOR:
        if (item->flags & TR_RES_ACID) {
            return false;
        } else {
            return true;
        }
    case TV_STAFF:
    case TV_SCROLL1:
    case TV_SCROLL2:
    case TV_FOOD:
    case TV_OPEN_DOOR:
    case TV_CLOSED_DOOR:
        return true;
    }
    return false;
}

int set_fire_destroy(item)
inven_type *item;
{
    switch (item->tval) {
    case TV_ARROW:
    case TV_BOW:
    case TV_HAFTED:
    case TV_POLEARM:
    case TV_BOOTS:
    case TV_GLOVES:
    case TV_CLOAK:
    case TV_SOFT_ARMOR:
        if (item->flags & TR_RES_FIRE) {
            return false;
        } else {
            return true;
        }
    case TV_STAFF:
    case TV_SCROLL1:
    case TV_SCROLL2:
    case TV_POTION1:
    case TV_POTION2:
    case TV_FLASK:
    case TV_FOOD:
    case TV_OPEN_DOOR:
    case TV_CLOSED_DOOR:
        return true;
    }
    return false;
}

int set_large(item)      /* Items too large to fit in chests   -DJG- */
    treasure_type *item; /* Use treasure_type since item not yet created */
{
    switch (item->tval) {
    case TV_CHEST:
    case TV_BOW:
    case TV_POLEARM:
    case TV_HARD_ARMOR:
    case TV_SOFT_ARMOR:
    case TV_STAFF:
        return true;
    case TV_HAFTED:
    case TV_SWORD:
    case TV_DIGGING:
        if (item->weight > 150) {
            return true;
        } else {
            return false;
        }
    }
    return false;
}

int general_store(element)
int element;
{
    switch (element) {
    case TV_DIGGING:
    case TV_BOOTS:
    case TV_CLOAK:
    case TV_FOOD:
    case TV_FLASK:
    case TV_LIGHT:
    case TV_SPIKE:
        return true;
    }
    return false;
}

int armory(element)
int element;
{
    switch (element) {
    case TV_BOOTS:
    case TV_GLOVES:
    case TV_HELM:
    case TV_SHIELD:
    case TV_HARD_ARMOR:
    case TV_SOFT_ARMOR:
        return true;
    }
    return false;
}

int weaponsmith(element)
int element;
{
    switch (element) {
    case TV_SLING_AMMO:
    case TV_BOLT:
    case TV_ARROW:
    case TV_BOW:
    case TV_HAFTED:
    case TV_POLEARM:
    case TV_SWORD:
        return true;
    }
    return false;
}

int temple(element)
int element;
{
    switch (element) {
    case TV_HAFTED:
    case TV_SCROLL1:
    case TV_SCROLL2:
    case TV_POTION1:
    case TV_POTION2:
    case TV_PRAYER_BOOK:
        return true;
    }
    return false;
}

int alchemist(element)
int element;
{
    switch (element) {
    case TV_SCROLL1:
    case TV_SCROLL2:
    case TV_POTION1:
    case TV_POTION2:
        return true;
    }
    return false;
}

int magic_shop(element)
int element;
{
    switch (element) {
    case TV_AMULET:
    case TV_RING:
    case TV_STAFF:
    case TV_WAND:
    case TV_SCROLL1:
    case TV_SCROLL2:
    case TV_POTION1:
    case TV_POTION2:
    case TV_MAGIC_BOOK:
        return true;
    }
    return false;
}
