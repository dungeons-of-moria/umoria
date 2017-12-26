// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Misc code, mainly to handle player commands

#include "headers.h"
#include "externs.h"

void objectBlockedByMonster(int monster_id) {
    vtype_t description = {'\0'};
    vtype_t msg = {'\0'};

    const Monster_t &monster = monsters[monster_id];
    const char *name = creatures_list[monster.creature_id].name;

    if (monster.lit) {
        (void) sprintf(description, "The %s", name);
    } else {
        (void) strcpy(description, "Something");
    }

    (void) sprintf(msg, "%s is in your way!", description);
    printMessage(msg);
}
