#! /bin/sh
# This is a shell archive, meaning:
# 1. Remove everything above the #! /bin/sh line.
# 2. Save the resulting text in a file.
# 3. Execute the file with /bin/sh (not csh) to create:
#	Makefile
#	print_items.c
#	print_monsters.c
# This archive created: Wed Mar  2 09:26:44 1988
export PATH; PATH=/bin:/usr/bin:$PATH
if test -f 'Makefile'
then
	echo shar: "will not over-write existing file 'Makefile'"
else
cat << \SHAR_EOF > 'Makefile'
CFLAGS= -c

all:    print_items print_monsters

print_items:    print_items.c treasure1.o
	cc print_items.c treasture1.o -o print_items
	print_items > items

print_monsters:    print_monsters.c monsters.o
	cc print_monsters.c treasture1.o -o print_monsters
	print_monsters > monsters

SHAR_EOF
chmod +x 'Makefile'
fi
if test -f 'print_items.c'
then
	echo shar: "will not over-write existing file 'print_items.c'"
else
cat << \SHAR_EOF > 'print_items.c'
/* written by Carlton Hommel */
#include "constants.h"
#include "types.h"

#define	Strength	0x00000001
#define	Dexterity	0x00000002
#define	Constitution	0x00000004
#define	Intelligence	0x00000008
#define	Wisdom		0x00000010
#define	Charisma	0x00000020
#define	Searching	0x00000040
#define	Slow_digestion 	0x00000080
#define	Stealth		0x00000100
#define	Aggravation	0x00000200
#define	Teleportation	0x00000400
#define	Regeneration	0x00000800
#define	Speed		0x00001000
#define	Slay_Dragon    	0x00002000
#define	Slay_Monster    0x00004000
#define	Slay_Evil       0x00008000
#define	Slay_Undead     0x00010000
#define	Cold_Brand      0x00020000
#define	Flame_Brand     0x00040000
#define	Resist_Fire     0x00080000
#define	Resist_Acid     0x00100000
#define	Resist_Cold     0x00200000
#define	Sustain_Stat	0x00400000
#define	Free_Action     0x00800000
#define	See_Invisible   0x01000000
#define	Resist_Lightning 0x02000000
#define	Feather_Fall    0x04000000
#define	Blindness	0x08000000
#define	Timidness	0x10000000
#define	Tunneling	0x20000000
#define	Infra_vision    0x40000000
#define	Cursed		0x80000000

extern treasure_type object_list[];
extern treasure_type inventory_init[];
extern treasure_type gold_list[];
extern treasure_type trap_lista[];
extern treasure_type trap_listb[];
main()
{
    int i;
    treasure_type *t;

    printf("[tval,sub] p1\tLvl   $ lbs\t  Dam [+, +]\t[AC,+]\tflags\tName\n");
    for (i=0; i<MAX_OBJECTS; i++) {
	t = &object_list[i];
	call_it(t);
    }
    printf("\n[tval,sub] p1\tLvl   $ lbs\t  Dam [+, +]\t[AC,+]\tflags\tName\n");
    for (i=0; i<INVEN_INIT_MAX; i++) {
	t = &inventory_init[i];
	call_it(t);
    }
    printf("\n[tval,sub] p1\tLvl   $ lbs\t  Dam [+, +]\t[AC,+]\tflags\tName\n");
    for (i=0; i<MAX_GOLD; i++) {
	t = &gold_list[i];
	call_it(t);
    }
    printf("\n[tval,sub] p1\tLvl   $ lbs\t  Dam [+, +]\t[AC,+]\tflags\tName\n");
    for (i=0; i<MAX_TRAPB; i++) {
	t = &trap_listb[i];
	call_it(t);
    }
    printf("\n[tval,sub] p1\tLvl   $ lbs\t  Dam [+, +]\t[AC,+]\tflags\tName\n");
    for (i=0; i<MAX_TRAPA; i++) {
	t = &trap_lista[i];
	call_it(t);
    }
}

call_it(t)
treasure_type *t;
{
    printf("[%2d,%3d] ", t->tval, t->subval);

    if (t->p1)
	 printf("%d\t", t->p1);
    else printf(" \t");

    printf("%2d %4d %3d\t", t->level, t->cost, t->weight);

    if (strcmp(t->damage, "0d0"))
	printf("%5s ", t->damage);
    else printf("     ");

    if (t->tohit!=0 || t->todam!=0)
	 printf("[%d, %d]\t", t->tohit, t->todam);
    else printf("      \t");

    if (t->ac!=0 || t->toac!=0)
	printf("{%d, %d}\t",
	    t->ac, t->toac);
    else printf("      \t");

    printf("'%c' \"%s\"\t",
	t->tchar, t->name);

    print_flags(t->tval, t->flags);

    printf("\n");
}

print_flags(tval, flags)
char tval;
unsigned int flags;
{
    int count;
    unsigned int mask;

    if (flags == 0)
	return;

    if (tval==80) {
	printf("<");
	for (count=0,mask=0x01; count<32; mask <<= 1, count++) {
	    switch (flags & mask) {
	    case 0x00000001: printf("poison(lvl+rnd(10)) ");		break;
	    case 0x00000002: printf("blind(10*lvl+rnd(250)) ");		break;
	    case 0x00000004: printf("fear(lvl+rnd(10) ");		break;
	    case 0x00000008: printf("confused(lvl+rnd(10) ");		break;
	    case 0x00000010: printf("hallucinate(25*lvl+rnd(200)+200) ");		break;
	    case 0x00000020: printf("cure_poison ");		break;
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
	    case 0x00200000: printf("-hp(rnd(3)) ");		break;
	    case 0x00400000: printf("-hp(rnd(6)) ");		break;
	    case 0x00800000: printf("-hp(rnd(12)) ");		break;
	    case 0x01000000: printf("-hp(rnd(3d6)) ");		break;
	    case 0x02000000: printf("-hp(rnd(3d12)) ");		break;
	    case 0x04000000: printf("hp(rnd(4)) ");		break;
	    case 0x08000000: printf("hp(rnd(8)) ");		break;
	    case 0x10000000: printf("hp(rnd(2d8))  ");		break;
	    case 0x20000000: printf("hp(rnd(3d8))  ");		break;
	    case 0x40000000: printf("major_heal ");		break;
	    case 0x80000000: printf("??? ");		break;
	    }
	}
	printf(">");
    }
    else
    if (tval>=10 && tval<=45) {
	printf("<");
	for (count=0,mask=0x01; count<32; mask <<= 1, count++) {
	    switch (flags & mask) {
	    case Strength           : printf("Strength ");		break;
	    case Dexterity          : printf("Dexterity ");		break;
	    case Constitution       : printf("Constitution ");	break;
	    case Intelligence       : printf("Intelligence ");	break;
	    case Wisdom             : printf("Wisdom ");		break;
	    case Charisma           : printf("Charisma ");		break;
	    case Searching          : printf("Searching ");		break;
	    case Slow_digestion     : printf("Slow_digestion ");	break;
	    case Stealth            : printf("Stealth ");		break;
	    case Aggravation        : printf("Aggravation ");	break;
	    case Teleportation      : printf("Teleportation ");	break;
	    case Regeneration       : printf("Regeneration ");	break;
	    case Speed              : printf("Speed ");		break;
	    case Slay_Dragon        : printf("Slay_Dragon ");	break;
	    case Slay_Monster       : printf("Slay_Monster ");	break;
	    case Slay_Evil          : printf("Slay_Evil ");		break;
	    case Slay_Undead        : printf("Slay_Undead ");	break;
	    case Cold_Brand         : printf("Cold_Brand ");		break;
	    case Flame_Brand        : printf("Flame_Brand ");	break;
	    case Resist_Fire        : printf("Resist_Fire ");	break;
	    case Resist_Acid        : printf("Resist_Acid ");	break;
	    case Resist_Cold        : printf("Resist_Cold ");	break;
	    case Sustain_Stat       : printf("Sustain_Stat ");	break;
	    case Free_Action        : printf("Free_Action ");	break;
	    case See_Invisible      : printf("See_Invisible ");	break;
	    case Resist_Lightning   : printf("Resist_Lightning ");	break;
	    case Feather_Fall       : printf("Feather_Fall ");	break;
	    case Blindness          : printf("Blindness ");		break;
	    case Timidness          : printf("Timidness ");		break;
	    case Tunneling          : printf("Tunneling ");		break;
	    case Infra_vision       : printf("Infra_vision ");	break;
	    case Cursed             : printf("Cursed ");		break;
	    }
	}
	printf(">");
    }
}
SHAR_EOF
fi
if test -f 'print_monsters.c'
then
	echo shar: "will not over-write existing file 'print_monsters.c'"
else
cat << \SHAR_EOF > 'print_monsters.c'
/* written by Carlton Hommel */
#include "constants.h"
#include "types.h"

#define	M_A_ONLY	0x00000001
#define	M_NORMAL	0x00000002
#define	M_RANDOM_20	0x00000008
#define	M_RANDOM_40	0x00000010
#define	M_RANDOM_75	0x00000020
#define	M_INVIS		0x00010000
#define	M_DOOR		0x00020000
#define	M_WALL		0x00040000
#define	M_CREATURE	0x00080000
#define	M_SCAVANGE	0x00100000
#define	M_MULTIPLY	0x00200000
#define	M_CARRY_TREAS	0x01000000
#define	M_CARRY_GOLD	0x02000000
#define	M_TREAS_60	0x04000000
#define	M_TREAS_90	0x08000000
#define	M_LITTLE_TREAS	0x10000000
#define	M_SOME_TREAS	0x20000000
#define M_MUCH_TREAS	0x40000000
#define M_TOTAL_WINNER	0x80000000

#define	S_1		0x00000001
#define	S_2		0x00000002
#define	S_4		0x00000004
#define	S_8		0x00000008
#define	S_BLINK		0x00000010
#define	S_TELEPORT	0x00000020
#define	S_TELE_TO	0x00000040
#define	S_CAUSE_LIGHT	0x00000080
#define	S_CAUSE_HEAVY	0x00000100
#define	S_HOLD		0x00000200
#define	S_BLIND		0x00000400
#define	S_CONFUSION	0x00000800
#define	S_FEAR		0x00001000
#define	S_SUMMON_MONST	0x00002000
#define	S_SUMMON_UNDEAD	0x00004000
#define	S_SLOW		0x00008000
#define	S_DRAIN_MANA	0x00010000
#define	S_B_LIGHTNING	0x00080000
#define	S_B_GAS		0x00100000
#define	S_B_ACID	0x00200000
#define	S_B_COLD	0x00400000
#define	S_B_FIRE	0x00800000

#define	C_SLAY_DRAG	0x00000001
#define	C_SLAY_MONS	0x00000002
#define	C_SLAY_EVIL	0x00000004
#define	C_SLAY_UNDEAD	0x00000008
#define	C_COLD		0x00000010
#define	C_FIRE		0x00000020
#define	C_POISON	0x00000040
#define	C_ACID		0x00000080
#define	C_LIGHT		0x00000100
#define	C_ROCKY		0x00000200
#define	C_NOSLEEP	0x00001000
#define	C_INFRASEE	0x00002000
#define	C_MAXHPS	0x00004000

extern creature_type c_list[];

main()
{
    int i;
    creature_type *c;

    printf("     Lvl(Ep) [HD, AC]\tspd aaf/{sleep}\t\t    Name\tDamage(special) <Defense> {Moves} /* Spells */\n");
    for (i=0; i<MAX_CREATURES; i++) {
      c = &c_list[i];
      call_it(c);
    }
}

call_it(c)
creature_type *c;
{
    /* Line 1 */
    printf("'%c' ", c->cchar);
    printf("%2d", c->level);
    printf("(%4d) ", c->mexp);
    printf("[%4s, %2d]\t", c->hd, c->ac);

    printf("%2d ", c->speed);
    printf("%2d/", c->aaf);
    printf("{%d}\t", c->sleep);

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
char *attstr;
{
  int xpos, attype, adesc;
  vtype attx;
  dtype damstr;
  char *string, *index();

/*    printf("\"%s\" ", c->damage); */

    while (strlen(attstr) > 0) {
	if (string = index(attstr, '|'))
	     xpos = strlen(attstr) - strlen(string);
	else xpos = -1;
	if (xpos >= 0) {
	    (void) strncpy(attx, attstr, xpos);
	    attx[xpos] = '\0';
	    (void) strcpy(attstr, &attstr[xpos+1]);
	}
	else {
	    (void) strcpy(attx, attstr);
	    attstr[0] = '\0';
	}

	(void) sscanf(attx, "%d %d %s", &attype, &adesc, damstr);
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
unsigned int spells;
{
    int count;
    unsigned int mask;

    if (spells == 0)
	return;

    printf("/*");
    for (count=0,mask=0x01; count<32; mask <<= 1, count++) {
	switch (spells & mask) {
	case S_1:		printf("S_1 ");	break;
	case S_2:		printf("S_2 ");	break;
	case S_4:		printf("S_4 ");	break;
	case S_8:		printf("S_8 ");	break;
	case S_BLINK:		printf("S_BLINK ");	break;
	case S_TELEPORT:	printf("S_TELEPORT ");	break;
	case S_TELE_TO:		printf("S_TELE_TO ");	break;
	case S_CAUSE_LIGHT:	printf("S_CAUSE_LIGHT ");	break;
	case S_CAUSE_HEAVY:	printf("S_CAUSE_HEAVY ");	break;
	case S_HOLD:		printf("S_HOLD ");	break;
	case S_BLIND:		printf("S_BLIND ");	break;
	case S_CONFUSION:	printf("S_CONFUSION ");	break;
	case S_FEAR:		printf("S_FEAR ");	break;
	case S_SUMMON_MONST:	printf("S_SUMMON_MONST ");	break;
	case S_SUMMON_UNDEAD:	printf("S_SUMMON_UNDEAD ");	break;
	case S_SLOW:		printf("S_SLOW ");	break;
	case S_DRAIN_MANA:	printf("S_DRAIN_MANA ");	break;
	case S_B_LIGHTNING:	printf("S_B_LIGHTNING ");	break;
	case S_B_GAS:		printf("S_B_GAS ");	break;
	case S_B_ACID:		printf("S_B_ACID ");	break;
	case S_B_COLD:		printf("S_B_COLD ");	break;
	case S_B_FIRE:		printf("S_B_FIRE ");	break;
	}
    }
    printf("*/");
}

print_flags_cmove(cmove)
unsigned int cmove;
{
    int count;
    unsigned int mask;

    if (cmove == 0)
	return;

    printf("<");
    for (count=0,mask=0x01; count<32; mask <<= 1, count++) {
	switch (cmove & mask) {
	case M_A_ONLY:		printf("M_A_ONLY ");	break;
	case M_NORMAL:		/*printf("M_NORMAL ");*/	break;
	case M_RANDOM_20:	printf("M_20 ");	break;
	case M_RANDOM_40:	printf("M_40 ");	break;
	case M_RANDOM_75:	printf("M_75 ");	break;
	case M_INVIS:		printf("M_INVIS ");	break;
	case M_DOOR:		printf("M_DOOR ");	break;
	case M_WALL:		printf("M_WALL ");	break;
	case M_CREATURE:	printf("M_GHOSTLY ");	break;
	case M_SCAVANGE:	printf("M_SCAVANGE ");	break;
	case M_MULTIPLY:	printf("M_MULTIPLY ");	break;
	case M_CARRY_TREAS:	printf("M_TREAS ");	break;
	case M_CARRY_GOLD:	printf("M_GOLD ");	break;
	case M_TREAS_60:	printf("M_T_60 ");	break;
	case M_TREAS_90:	printf("M_T_90 ");	break;
	case M_LITTLE_TREAS:	printf("M_T_1D2 ");	break;
	case M_SOME_TREAS:	printf("M_T_2D2 ");	break;
	case M_MUCH_TREAS:	printf("M_T_4D2 ");	break;
	case M_TOTAL_WINNER:	printf("M_WINNER ");	break;
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
    for (count=0,mask=0x01; count<32; mask <<= 1, count++) {
	switch (cdefense & mask) {
	case C_SLAY_DRAG:	printf("DRAGON ");	break;
	case C_SLAY_MONS:	printf("MONSTER ");	break;
	case C_SLAY_EVIL:	printf("EVIL ");	break;
	case C_SLAY_UNDEAD:	printf("UNDEAD ");	break;
	case C_COLD:		printf("C_COLD ");	break;
	case C_FIRE:		printf("C_FIRE ");	break;
	case C_POISON:		printf("C_POISON ");	break;
	case C_ACID:		printf("C_ACID ");	break;
	case C_LIGHT:		printf("C_LIGHT ");	break;
	case C_ROCKY:		printf("C_ROCKY ");	break;
	case C_NOSLEEP:		printf("C_NOSLEEP ");	break;
	case C_INFRASEE:	printf("C_INFRASEE ");	break;
	case C_MAXHPS:		printf("C_MAXHPS ");	break;
	}
    }
    printf("}");
}
SHAR_EOF
fi
exit 0
#	End of shell archive
