/* util/printit/pr_items.c: pretty print item/treasure info

   Copyright (c) 1990-1992: Carlton Hommel, James E. Wilson

   This file is part of Umoria.

   Umoria is free software; you can redistribute it and/or modify 
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Umoria is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of 
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License 
   along with Umoria.  If not, see <http://www.gnu.org/licenses/>. */

#include "config.h"
#include "constant.h"
#include "types.h"

extern treasure_type object_list[];

main()
{
    int i;
    treasure_type *t;

    printf("[tval,sub] p1\tLvl   $ lbs\t  Dam [+, +]\t[AC,+]\tChar Name");
    printf("\tFlags\n");
    for (i=0; i<MAX_OBJECTS; i++) {
	t = &object_list[i];
	call_it(t);
    }
    return 0;
}

call_it(t)
treasure_type *t;
{
    printf("[%2d,%3d] ", t->tval, t->subval);

    if (t->p1)
	 printf("%d\t", t->p1);
    else printf(" \t");

    printf("%2d %4ld %3d\t", t->level, t->cost, t->weight);

    if (t->damage[0] || t->damage[1])
	printf("%2dd%d ", t->damage[0], t->damage[1]);
    else printf("     ");

    if (t->tohit || t->todam)
	 printf("[%d, %d]\t", t->tohit, t->todam);
    else printf("      \t");

    if (t->ac!=0 || t->toac!=0)
	printf("{%d, %d}\t", t->ac, t->toac);
    else printf("      \t");

    printf("'%c' \"%s\"\t",
	t->tchar, t->name);

    print_flags(t->tval, t->flags);

    printf("\n");
}

print_flags(tval, flags)
char tval;
unsigned long flags;
{
    int count;
    unsigned long mask;

    if (flags == 0)
	return;

    if (tval==80) {
	printf("<");
	for (count=0,mask=0x01; count<32; mask <<= 1, count++) {
	    switch (flags & mask) {
	    case 0x00000001: printf("poison(lvl+rnd(10)) ");		break;
	    case 0x00000002: printf("blind(10*lvl+rnd(250)+100) ");	break;
	    case 0x00000004: printf("fear(lvl+rnd(10) ");		break;
	    case 0x00000008: printf("confused(lvl+rnd(10) ");		break;
	    case 0x00000010: printf("hallucinate(25*lvl+rnd(200)+200) ");break;
	    case 0x00000020: printf("cure_poison ");			break;
	    case 0x00000040: printf("cure_blindness ");		break;
	    case 0x00000080: printf("cure_fear ");		break;
	    case 0x00000100: printf("cure_confuse ");		break;
	    case 0x00000200: printf("lose_str ");		break;
	    case 0x00000400: printf("lose_con ");		break;
	    case 0x00000800: printf("lose_int ");		break;
	    case 0x00001000: printf("lose_wis ");		break;
	    case 0x00002000: printf("lose_dex ");		break;
	    case 0x00004000: printf("lose_chr ");		break;
	    case 0x00008000: printf("rest_str ");		break;
	    case 0x00010000: printf("rest_con ");		break;
	    case 0x00020000: printf("rest_int ");		break;
	    case 0x00040000: printf("rest_wis ");		break;
	    case 0x00080000: printf("rest_dex ");		break;
	    case 0x00100000: printf("rest_chr ");		break;
	    case 0x00200000: printf("hp+(rnd(6)) ");		break;
	    case 0x00400000: printf("hp+(rnd(12)) ");		break;
	    case 0x00800000: printf("hp+(rnd(18)) ");		break;
	    case 0x01000000: printf("hp+(rnd(3d6)) ");		break;
	    case 0x02000000: printf("hp+(rnd(3d12)) ");		break;
	    case 0x04000000: printf("hp-(rnd(18)) ");		break;
	    case 0x08000000: printf("hp-(rnd(8)) ");		break;
	    case 0x10000000: printf("hp-(rnd(2d8))  ");		break;
	    case 0x20000000: printf("hp-(rnd(3d8))  ");		break;
	    case 0x40000000: printf("??? ");			break;
	    case 0x80000000: printf("??? ");			break;
	    }
	}
	printf(">");
    }
    else
    if (tval>=TV_MIN_WEAR && tval<=TV_MAX_WEAR) {
	printf("<");
	for (count=0,mask=0x01; count<32; mask <<= 1, count++) {
	    switch (flags & mask) {
	    case TR_STR:		printf("Strength ");		break;
	    case TR_INT:		printf("Intelligence ");	break;
	    case TR_WIS:		printf("Wisdom ");		break;
	    case TR_DEX:		printf("Dexterity ");		break;
	    case TR_CON:		printf("Constitution ");	break;
	    case TR_CHR:		printf("Charisma ");		break;
	    case TR_SEARCH:		printf("Searching ");		break;
	    case TR_SLOW_DIGEST:	printf("Slow_digestion ");	break;
	    case TR_STEALTH:		printf("Stealth ");		break;
	    case TR_AGGRAVATE:		printf("Aggravation ");		break;
	    case TR_TELEPORT:		printf("Teleportation ");	break;
	    case TR_REGEN:		printf("Regeneration ");	break;
	    case TR_SPEED:		printf("Speed ");		break;
	    case TR_SLAY_DRAGON:	printf("Slay_Dragon ");		break;
	    case TR_SLAY_ANIMAL:	printf("Slay_Animal ");		break;
	    case TR_SLAY_EVIL:		printf("Slay_Evil ");		break;
	    case TR_SLAY_UNDEAD:	printf("Slay_Undead ");		break;
	    case TR_FROST_BRAND:	printf("Cold_Brand ");		break;
	    case TR_FLAME_TONGUE:	printf("Flame_Brand ");		break;
	    case TR_RES_FIRE:		printf("Resist_Fire ");		break;
	    case TR_RES_ACID:		printf("Resist_Acid ");		break;
	    case TR_RES_COLD:		printf("Resist_Cold ");		break;
	    case TR_SUST_STAT:		printf("Sustain_Stat ");	break;
	    case TR_FREE_ACT:		printf("Free_Action ");		break;
	    case TR_SEE_INVIS:		printf("See_Invisible ");	break;
	    case TR_RES_LIGHT:		printf("Resist_Lightning ");	break;
	    case TR_FFALL:		printf("Feather_Fall ");	break;
	    case TR_BLIND:		printf("Blindness ");		break;
	    case TR_TIMID:		printf("Timidness ");		break;
	    case TR_TUNNEL:		printf("Tunneling ");		break;
	    case TR_INFRA:		printf("Infra_vision ");	break;
	    case TR_CURSED:		printf("Cursed ");		break;
	    }
	}
	printf(">");
    }
}
