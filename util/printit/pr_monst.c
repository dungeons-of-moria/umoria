/* util/printit/pr_monst.c: pretty print monster info

   Copyright (c) 1990-1992 Carlton Hommel, James E. Wilson

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

extern creature_type c_list[];

main()
{
    int i;
    creature_type *c;

    printf("     Lvl(Ep) [  HD, AC]\tspd aaf/{sleep}\t\t    Name");
    printf("\tDamage(special) <Defense> {Moves} /* Spells */\n");
    for (i=0; i<MAX_CREATURES; i++) {
      c = &c_list[i];
      call_it(c);
    }
    return 0;
}

call_it(c)
creature_type *c;
{
    /* Line 1 */
    printf("'%c' ", c->cchar);
    printf("%2d", c->level);
    printf("(%4d) ", c->mexp);
    printf("[%2dd%d, %2d]\t", c->hd[0], c->hd[1], c->ac);

    printf("%2d ", ((int)c->speed)-10);
    printf("%2d/", c->aaf);
    printf("{%d}\t", c->sleep*10);

    printf("%20.20s\t", c->name);
    print_damage(c->damage);
    printf("\t");

    print_flags_cdefense(c->cdefense);
    printf(" ");
    print_flags_cmove(c->cmove);
    printf(" ");
    print_flags_spells(c->spells);

    printf("\n");

}

print_damage(attstr)
int8u attstr[4];
{
  int attype, adesc, adice, asides;
  int count;
  char damstr[50];

  extern struct m_attack_type monster_attacks[];

  for (count=0; count<4; count++) {
        if (attstr[count] == 0) continue;
	attype = monster_attacks[attstr[count]].attack_type;
	adesc  = monster_attacks[attstr[count]].attack_desc;
	adice  = monster_attacks[attstr[count]].attack_dice;
	asides = monster_attacks[attstr[count]].attack_sides;

	sprintf(damstr, "%dd%d", adice, asides);
	switch(attype) {
	case 1:  printf("%s ", damstr);		break;
	case 2:  printf("%s(-Str) ", damstr);	break;
	case 3:  printf("%s(Conf) ", damstr);	break;
	case 4:  printf("%s(Fear) ", damstr);	break;
	case 5:  printf("%s(Fire) ", damstr);	break;
	case 6:  printf("%s(Acid) ", damstr);	break;
	case 7:  printf("%s(Cold) ", damstr);	break;
	case 8:  printf("%s(Zap) ", damstr);	break;
	case 9:  printf("%s(Acid) ", damstr);	break;
	case 10: printf("%s(Blind) ", damstr);	break;
	case 11: printf("%s(Paral) ", damstr);	break;
	case 12: printf("%s(-Money) ", damstr);	break;
	case 13: printf("%s(-Object) ", damstr);break;
	case 14: printf("%s(Poison) ", damstr);	break;
	case 15: printf("%s(-Dex) ", damstr);	break;
	case 16: printf("%s(-Con ) ", damstr);	break;
	case 17: printf("%s(-Int ) ", damstr);	break;
	case 18: printf("%s(-Wis) ", damstr);	break;
	case 19: printf("%s(-Exp) ", damstr);	break;
	case 20: printf("%s(Aggr) ", damstr);	break;
	case 21: printf("%s(-Magic) ", damstr);	break;
	case 22: printf("%s(-Food) ", damstr);	break;
	case 23: printf("%s(-Light) ", damstr);	break;
	case 24: printf("%s(-Charges) ", damstr);break;
	default:
	case 99: printf("%s(Blank) ", damstr);	break;
	}

    }
}

print_flags_spells(spells)
unsigned long spells;
{
    int count;
    unsigned long mask;
    int chance;

    if (spells == 0)
	return;

    printf("/* ");
    chance = (int)spells & CS_FREQ;
    printf("%2.1f%% ", 1.0 / chance * 100.0);
    for (count=4,mask=0x010; count<32; mask <<= 1, count++) {
	switch (spells & mask) {
	case CS_TEL_SHORT:	printf("CS_TEL_SHORT ");break;
	case CS_TEL_LONG:	printf("CS_TEL_LONG ");	break;
	case CS_TEL_TO:		printf("CS_TEL_TO ");	break;
	case CS_LGHT_WND:	printf("CS_LGHT_WND ");	break;
	case CS_SER_WND:	printf("CS_SER_WND ");	break;
	case CS_HOLD_PER:	printf("CS_HOLD_PER ");	break;
	case CS_BLIND:		printf("CS_BLIND ");	break;
	case CS_CONFUSE:	printf("CS_CONFUSE ");	break;
	case CS_FEAR:		printf("CS_FEAR ");	break;
	case CS_SUMMON_MON:	printf("CS_SUMMON_MON ");break;
	case CS_SUMMON_UND:	printf("CS_SUMMON_UND ");break;
	case CS_SLOW_PER:	printf("CS_SLOW_PER ");	break;
	case CS_DRAIN_MANA:	printf("CS_DRAIN_MANA ");break;
	case CS_BR_LIGHT:	printf("CS_BR_LIGHT ");	break;
	case CS_BR_GAS:		printf("CS_BR_GAS ");	break;
	case CS_BR_ACID:	printf("CS_BR_ACID ");	break;
	case CS_BR_FROST:	printf("CS_BR_FROST ");	break;
	case CS_BR_FIRE:	printf("CS_BR_FIRE ");	break;
	}
    }
    printf("*/");
}

print_flags_cmove(cmove)
unsigned long cmove;
{
    int count;
    unsigned long mask;

    if (cmove == 0)
	return;

    printf("<");
    for (count=0,mask=0x01; count<32; mask <<= 1, count++) {
	switch (cmove & mask) {
	case CM_ATTACK_ONLY:	printf("CM_ATTACK_ONLY ");	break;
	case CM_MOVE_NORMAL:	/*printf("CM_MOVE_NORMAL ");*/	break;
	case CM_20_RANDOM:	printf("CM_20_RANDOM ");	break;
	case CM_40_RANDOM:	printf("CM_40_RANDOM ");	break;
	case CM_75_RANDOM:	printf("CM_75_RANDOM ");	break;
	case CM_INVISIBLE:	printf("CM_INVISIBLE ");	break;
	case CM_OPEN_DOOR:	printf("CM_OPEN_DOOR ");	break;
	case CM_PHASE:		printf("CM_PHASE ");		break;
	case CM_EATS_OTHER:	printf("CM_EATS_OTHER ");	break;
	case CM_PICKS_UP:	printf("CM_PICKS_UP ");		break;
	case CM_MULTIPLY:	printf("CM_MULTIPLY ");		break;
	case CM_CARRY_OBJ:	printf("CM_CARRY_OTHER ");	break;
	case CM_CARRY_GOLD:	printf("CM_CARRY_GOLD ");	break;
	case CM_60_RANDOM:	printf("CM_60_RANDOM ");	break;
	case CM_90_RANDOM:	printf("CM_90_RANDOM ");	break;
	case CM_1D2_OBJ:	printf("CM_1D2_OBJ ");		break;
	case CM_2D2_OBJ:	printf("CM_2D2_OBJ ");		break;
	case CM_4D2_OBJ:	printf("CM_4D2_OBJ ");		break;
	case CM_WIN:		printf("CM_WIN ");		break;
	}
    }
    printf(">");
}

print_flags_cdefense(cdefense)
unsigned int cdefense;
{
    int count;
    unsigned int mask;

    if (cdefense == 0)
	return;

    printf("{");
    for (count=0,mask=0x01; count<16; mask <<= 1, count++) {
	switch (cdefense & mask) {
	case CD_DRAGON:		printf("CD_DRAGON ");	break;
	case CD_ANIMAL:		printf("CD_ANIMAL ");	break;
	case CD_EVIL:		printf("CD_EVIL ");	break;
	case CD_UNDEAD:		printf("CD_UNDEAD ");	break;
	case CD_FROST:		printf("CD_FROST ");	break;
	case CD_FIRE:		printf("CD_FIRE ");	break;
	case CD_POISON:		printf("CD_POISON ");	break;
	case CD_ACID:		printf("CD_ACID ");	break;
	case CD_LIGHT:		printf("CD_LIGHT ");	break;
	case CD_STONE:		printf("CD_STONE ");	break;
	case CD_NO_SLEEP:	printf("CD_NO_SLEEP ");	break;
	case CD_INFRA:		printf("CD_INFRA ");	break;
	case CD_MAX_HP:		printf("CD_MAX_HP ");	break;
	}
    }
    printf("}");
}
