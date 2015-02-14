/* util/mc/creature.y: a Moria creature definition compiler
 *
 * Copyright 1989 by Joseph Hall.
 
   This file is part of Umoria.

   Umoria is free software; you can redistribute it and/or modify 
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Umoria is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of 
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Genral Public License for more details.

   You should have received a copy of the GNU General Public License 
   along with Umoria.  If not, see <http://www.gnu.org/licenses/>. */

/* This file was also released as part of an independent program, and
   may be redistributed and/or modified under the authors' original terms,
   which are given below.*/

/* All rights reserved except as stated below.
 * This program may contain portions excerpted from Moria which are
 * copyrighted by others.
 *
 * Jim Wilson and any other holders of copyright on substantial portions
 * of Moria are granted rights to use, modify, and distribute this program
 * as they see fit, so long as the terms of its use, modification and/or
 * distribution are no less restrictive than those applying to Moria,
 * version 5.0 or later, itself, and so long as this use is related to
 * the further development of Moria.
 *
 * Anyone having any other use in mind for this code should contact the
 * author at 4116 Brewster Dr., Raleigh NC 27606 (jnh@ecemwl.ncsu.edu).
 */

%{
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#ifdef ANSI_LIBS
#include <stdlib.h>
#else
extern double atof();
extern char *malloc();
extern char *calloc();
extern void free();
#endif

#include "st.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define VERBOSE /* to turn on debugging output */

typedef unsigned long  int32u;
typedef long           int32;
typedef unsigned short int16u;
typedef short          int16;
typedef unsigned char  int8u;

typedef struct creature_type
{
  char *name;           /* Descrip of creature  */
  int32u cmove;         /* Bit field            */
  int32u spells;        /* Creature spells      */
  int16u cdefense;      /* Bit field            */
  int16u mexp;          /* Exp value for kill   */
  int8u sleep;          /* Inactive counter/10  */
  int8u aaf;            /* Area affect radius   */
  int8u ac;             /* AC                   */
  int8u speed;          /* Movement speed+10    */
  int8u cchar;          /* Character rep.       */
  int8u hd[2];          /* Creatures hit die    */
  int8u damage[4];      /* Type attack and damage*/
  int8u level;          /* Level of creature    */

	int32u general;	/* general characteristics; not present in usual */
			/* moria creature_type */
} creature_type;



/*
 * defined_t is used to indicate whether all fields have been defined
 */

typedef struct {
    unsigned	move: 1,
		special: 1,
		treasure: 1,
		spell: 1,
		breath: 1,
		defense: 1,
		mexp: 1,
		sleep: 1,
		aaf: 1,
		ac: 1,
		speed: 1,
		cchar: 1,
		hd: 1,
		damage: 1,
		level: 1;
} defined_t;



/*
 * template_t contains creature definition & flags
 */

typedef struct {
	creature_type	val;
	defined_t	def;
} template_t;



/*
 * attack_t describes a monster attack
 */

typedef struct {
	int8u		type,
			desc,
			dice,
			sides;
} attack_t;



/*
 * symInit_t is used to initialize symbol tables with integer values
 */

typedef struct {
	char		*name;
	int32u		val;
} symInit_t;



static symInit_t defenseInit[] = {
	{ "dragon", 0 },
	{ "animal", 1 },
	{ "evil", 2 },
	{ "undead", 3 },
	{ "frost", 4 },
	{ "fire", 5 },
	{ "poison", 6 },
	{ "acid", 7 },
	{ "light", 8 },
	{ "stone", 9 },
	{ "bit_9", 10 },
	{ "bit_10", 11 },
	{ "no_sleep", 12 },
	{ "infra", 13 },
	{ "max_hp", 14 },
	{ "bit_15", 15 },
	{ NULL, 0 }
};

static symInit_t moveInit[] = {
	{ "attack_only", 0 },
	{ "move_normal", 1 },
	{ "bit_2", 2 },
	{ "random_20", 3 },
	{ "random_40", 4 },
	{ "random_75", 5 },
	{ NULL, 0 }
};

static symInit_t specialInit[] = {
	{ "invisible", 16 },
	{ "open_door", 17 },
	{ "phase", 18 },
	{ "eats_other", 19 },
	{ "picks_up", 20 },
	{ "multiply", 21 },
	{ "win_creature", 31 },
	{ NULL, 0 }
};

static symInit_t treasureInit[] = {
	{ "carry_obj", 24 },
	{ "carry_gold", 25 },
	{ "has_random_60", 26 },
	{ "has_random_90", 27 },
	{ "has_1d2_obj", 28 },
	{ "has_2d2_obj", 29 },
	{ "has_4d2_obj", 30 },
	{ NULL, 0 }
};

static symInit_t spellInit[] = {
	{ "tel_short", 4 },
	{ "tel_long", 5 },
	{ "tel_to", 6 },
	{ "lght_wnd", 7 },
	{ "ser_wnd", 8 },
	{ "hold_per", 9 },
	{ "blind", 10 },
	{ "confuse", 11 },
	{ "fear", 12 },
	{ "summon_mon", 13 },
	{ "summon_und", 14 },
	{ "slow_per", 15 },
	{ "drain_mana", 16 },
	{ "bit_17", 17 },
	{ "bit_18", 18 },
	{ NULL, 0 }
};

static symInit_t breathInit[] = {
	{ "light", 19 },
	{ "gas", 20 },
	{ "acid", 21 },
	{ "frost", 22 },
	{ "fire", 23 },
	{ NULL, 0 }
};

static symInit_t attackTypeInit[] = {
	{ "normal_damage", 1 },
	{ "lose_str", 2 },
	{ "confusion", 3 },
	{ "cause_fear", 4 },
	{ "fire_damage", 5 },
	{ "acid_damage", 6 },
	{ "cold_damage", 7 },
	{ "lightning_damage", 8 },
	{ "corrosion", 9 },
	{ "cause_blindness", 10 },
	{ "cause_paralysis", 11 },
	{ "steal_money", 12 },
	{ "steal_obj", 13 },
	{ "poison", 14 },
	{ "lose_dex", 15 },
	{ "lose_con", 16 },
	{ "lose_int", 17 },
	{ "lose_wis", 18 },
	{ "lose_exp", 19 },
	{ "aggravation", 20 },
	{ "disenchant", 21 },
	{ "eat_food", 22 },
	{ "eat_light", 23 },
	{ "eat_charges", 24 },
	{ "blank", 99 },
	{ NULL, 0 }
};

static symInit_t attackDescInit[] = {
	{ "hits", 1 },
	{ "bites", 2 },
	{ "claws", 3 },
	{ "stings", 4 },
	{ "touches", 5 },
	{ "kicks", 6 },
	{ "gazes", 7 },
	{ "breathes", 8 },
	{ "spits", 9 },
	{ "wails", 10 },
	{ "embraces", 11 },
	{ "crawls_on", 12 },
	{ "releases_spores", 13 },
	{ "begs_for_money", 14 },
	{ "slimes", 15 },
	{ "crushes", 16 },
	{ "tramples", 17 },
	{ "drools_on", 18 },
	{ "insults", 19 },
	{ "is_repelled", 99 },
	{ NULL, 0 }
};




/*
 * Maximum token length = maximum string constant length
 * Also, trim the stack to an "acceptable" size.
 */

#define	MAX_TOK_LEN	64		/* maximum acceptable token length  */
#define	YYSTACKSIZE	128

#define GEN_TYPE_TMPL	256		/* type of a template for st	    */

/*
 * Globals used by the tokenizer (lexical analyzer)
 */

#define INPUT_BUF_SIZE 256
static char	inputBuf[INPUT_BUF_SIZE] = { 0 };
					/* input line buffer		    */
static char	*inputBufp = inputBuf;	/* position in input line buffer    */
static int	lineNo = 0;		/* number of current line	    */
static FILE	*input_F;
static char	tokStr[MAX_TOK_LEN];	/* text of current token	    */
static	int	tokType;		/* type of current token	    */
static	double	tokVal;			/* numeric value of current token,  */
					/* if applicable		    */

static template_t blankTemplate = { 0 }; /* blank template for init-ing     */
static template_t tmpTemplate;		/* working template for current     */
					/* class or creature		    */

#define MAX_ATTACK 250
static attack_t attackList[MAX_ATTACK] = { 0 };
static int attackCt = 1, creatureAttacks = 0;
static int maxCreatureLevel = 0;

/*
 * Global symbol tables
 */

static st_Table_Pt keywordT_P,		/* parser's keywords		    */
		defenseT_P,		/* defense flags		    */
		moveT_P,		/* movement flags		    */
		specialT_P,		/* special flags		    */
		treasureT_P,		/* treasure flags		    */
		spellT_P,		/* spell flags			    */
		breathT_P,		/* breath flags			    */
		attackTypeT_P,		/* attack type flags		    */
		attackDescT_P,		/* attack desc flags		    */
		classT_P,		/* class templates		    */
		creatureT_P;		/* creature definitions		    */

/*
 * Function declarations
 */

extern void AddDefense();
extern void NegDefense();
extern void AddMove();
extern void NegMove();
extern void AddTreasure();
extern void NegTreasure();
extern void AddSpecial();
extern void NegSpecial();
extern void AddSpell();
extern void NegSpell();
extern void AddBreath();
extern void NegBreath();
extern void AddAttack();
extern void WriteCreature();
extern void PutClassTemplate();
extern template_t GetClassTemplate();
extern void PutCreature();

%}


/*
 * YACC DEFINITIONS
 */

/*
 * The parser's stack can hold ints, doubles, and strings.
 */

%union {
	int ival;
	double dval;
	char sval[MAX_TOK_LEN];
	}

/*
 * Reserved words
 */

%token CLASS CREATURE NAMED HD D MOVE SPELL BREATH DEFENSE XP CCHAR SLEEP
%token RADIUS SPEED ATTACK FOR AC LEVEL TREASURE SPECIAL OF IN

/*
 * Entities
 */

%token <sval> IDENTIFIER	/* identifier, not a keyword		    */
%token <dval> FLOAT_LIT		/* floating-pt literal			    */
%token <ival> INT_LIT		/* integer literal			    */
%token <sval> STRING_LIT	/* string literal			    */
%token <ival> BOOL_LIT		/* boolean literal			    */

/*
 * ASCII chars are their own tokens
 */

%start	creatures


/*
 * THE PARSER
 */

%%

creatures	:	class_def ';' creatures
		|	creature_def ';' creatures
		|	/* empty */
		;

class_def	:	CLASS IDENTIFIER parent_class '{' features '}'
				{ PutClassTemplate($<sval>2, &tmpTemplate); }
		;

parent_class	:	':' IDENTIFIER
				{ tmpTemplate = GetClassTemplate($<sval>2);
				  creatureAttacks = 0; }
		|	/* empty */
				{ tmpTemplate = blankTemplate;
				  creatureAttacks = 0; }
		;

creature_def	:	CREATURE STRING_LIT parent_class
			'{' features '}'
				{ tmpTemplate.val.name =
				    (char *) malloc(strlen($<sval>2) + 1);
				  strcpy(tmpTemplate.val.name, $<sval>2);
				  PutCreature($<sval>2, &tmpTemplate);
				}
		;

features	:	feature ';' features
		|	/* empty */
		;

feature		:	LEVEL ':' INT_LIT
				{ tmpTemplate.val.level = $<ival>3;
				  tmpTemplate.def.level = TRUE; }
		|	HD ':' INT_LIT D INT_LIT
				{ tmpTemplate.val.hd[0] = $<ival>3;
				  tmpTemplate.val.hd[1] = $<ival>5;
				  tmpTemplate.def.hd = TRUE; }
		|	XP ':' INT_LIT
				{ tmpTemplate.val.mexp = $<ival>3;
				  tmpTemplate.def.mexp = TRUE; }
		|	CCHAR ':' STRING_LIT
				{ tmpTemplate.val.cchar = $<sval>3[0];
				  tmpTemplate.def.cchar = TRUE; }
		|	AC ':' INT_LIT
				{ tmpTemplate.val.ac = $<ival>3;
				  tmpTemplate.def.ac = TRUE; }
		|	SLEEP ':' INT_LIT
				{ tmpTemplate.val.sleep = $<ival>3;
				  tmpTemplate.def.sleep = TRUE; }
		|	RADIUS ':' INT_LIT
				{ tmpTemplate.val.aaf = $<ival>3;
				  tmpTemplate.def.aaf = TRUE; }
		|	SPEED ':' INT_LIT
				{ tmpTemplate.val.speed = $<ival>3 + 10;
				  tmpTemplate.def.speed = TRUE; }
		|	ATTACK ':' attacks
		|	MOVE ':' moves
		|	SPELL ':' spells
		|	SPELL INT_LIT '%' ':' spells
				{ float chance = 100.0 / $<ival>2;
				  if (chance > 15.0)
				  	chance = 0.0;
				  if (chance < 0.0)
				  	chance = 0.0;
				  tmpTemplate.val.spells &= ~0xf;
				  tmpTemplate.val.spells |=
				  	(int) ceil(chance);
				  tmpTemplate.def.spell = TRUE; }
		|	BREATH ':' breaths
		|	BREATH INT_LIT '%' ':' breaths
				{ float chance = 100.0 / $<ival>2;
				  if (chance > 15.0)
				  	chance = 0.0;
				  if (chance < 0.0)
				  	chance = 0.0;
				  tmpTemplate.val.spells &= ~0xf;
				  tmpTemplate.val.spells |=
				  	(int) ceil(chance);
				  tmpTemplate.def.spell = TRUE; }
		|	DEFENSE ':' defenses
		|	TREASURE ':' carries
		|	SPECIAL ':' specials
		;

attacks		:	attack more_attacks
		;

attack		:	IDENTIFIER FOR INT_LIT D INT_LIT OF IDENTIFIER
				{ AddAttack($<sval>1, $<ival>3,
						$<ival>5, $<sval>7); }
		;

more_attacks	:	',' attack more_attacks
		|	/* empty */
		;

moves		:	move more_moves
		;

move		:	IDENTIFIER { AddMove($<sval>1); }
		|	'~' IDENTIFIER { NegMove($<sval>2); }
		;

more_moves	:	',' move more_moves
		|	/* empty */
		;

spells		:	spell more_spells
		|	/* empty */
		;

spell		:	IDENTIFIER { AddSpell($<sval>1); }
		|	'~' IDENTIFIER { NegSpell($<sval>2); }
		;

more_spells	:	',' spell more_spells
		|	/* empty */
		;

breaths		:	breath more_breaths
		;

breath		:	IDENTIFIER { AddBreath($<sval>1); }
		|	'~' IDENTIFIER { NegBreath($<sval>2); }
		;

more_breaths	:	',' breath more_breaths
		|	/* empty */
		;

defenses	:	defense more_defenses
		;

defense		:	IDENTIFIER { AddDefense($<sval>1); }
		|	'~' IDENTIFIER { NegDefense($<sval>2); }
		;

more_defenses	:	',' defense more_defenses
		|	/* empty */
		;

carries		:	carry more_carries
		;

carry		:	IDENTIFIER { AddTreasure($<sval>1); }
		|	'~' IDENTIFIER { NegTreasure($<sval>2); }
		;

more_carries	:	',' carry more_carries
		|	/* empty */
		;

specials	:	special more_specials
		;

special		:	IDENTIFIER { AddSpecial($<sval>1); }
		|	'~' IDENTIFIER { NegSpecial($<sval>2); }
		;

more_specials	:	',' special more_specials
		|	/* empty */
		;


%%

static symInit_t keywordInit[] = {
	{ "class", CLASS },
	{ "creature", CREATURE },
	{ "named", NAMED },
	{ "hd", HD },
	{ "d", D },
	{ "move", MOVE },
	{ "spell", SPELL },
	{ "breath", BREATH },
	{ "defense", DEFENSE },
	{ "xp", XP },
	{ "cchar", CCHAR },
	{ "sleep", SLEEP },
	{ "radius", RADIUS },
	{ "speed", SPEED },
	{ "attack", ATTACK },
	{ "for", FOR },
	{ "ac", AC },
	{ "level", LEVEL },
	{ "treasure", TREASURE },
	{ "special", SPECIAL },
	{ "of", OF },
	{ "in", IN },
	{ NULL, 0 }
};



/*
 * MyFGetC--
 *	fgetc with support for comments
 *
 *	# is the comment character.  comment lasts till end of line.
 * Spews out an extra char of whitespace at EOF since something seems to
 * need it.  I'll figure this out eventually...
 */
static int MyFGetC(input_F)
FILE *input_F;
{
    static int atEof = FALSE;
    while (!*inputBufp || (*inputBufp == '#')) {
	fgets(inputBuf, INPUT_BUF_SIZE, input_F);
    	if (feof(input_F))
	    return EOF;
	lineNo++;
	inputBufp = inputBuf;
    }
    return *inputBufp++;
}



/*
 * Advance--
 *	Advance to the next token in the input stream and set tokStr,
 * tokType, tokVal as appropriate.
 *
 *	On error, tokType is set to a negative value.
 */
static void Advance(input_F)
FILE *input_F;
{

    register char
	    *tok = tokStr;	/* accumulating token string		    */
    register int
	    len = 0;		/* length of current token		    */
    static int c = 32;		/* current character; ' ' is harmless init  */


    /*
     * Skip whitespace in the stream
     */
    while ((c != EOF) && isspace(c))
	c = MyFGetC(input_F);

    /*
     * At end of file?
     */
    if (c == EOF) {
	tokType = EOF;
	strcpy(tokStr, "[EOF]");
	return;
    }

    /*
     * Recognize a number [+|-][dddd][.][dddd][{e|E}[+|-]dddd]
     */
    if (isdigit(c) || (c == '.') || (c == '+') || (c == '-')) {

	register int
		decPt = FALSE,	    /* seen a decimal point yet?	*/
		hasExp = FALSE;	    /* has an exponent?			*/

	if ((c == '-') || (c == '+')) {
	    *tok++ = c;
	    c = MyFGetC(input_F);
	}

	while ((len < MAX_TOK_LEN - 1) && (isdigit(c) || (c == '.'))) {
	    if (c == '.') {
		if (decPt)
		    break;
		else
		    decPt = TRUE;
	    }

	    *tok++ = c;
	    c = MyFGetC(input_F);
	    len++;
	}

	if ((c == 'e') || (c == 'E')) {
	    hasExp = TRUE;
	    *tok++ = c;
	    c = MyFGetC(input_F);
	    len++;

	    if ((c == '-') || (c == '+')) {
		*tok++ = c;
		c = MyFGetC(input_F);
		len++;
	    }

	    while ((len < MAX_TOK_LEN - 1) && isdigit(c)) {
		*tok++ = c;
		c = MyFGetC(input_F);
		len++;
	    }
	}

	*tok = 0;

	if (decPt || hasExp) {
	    tokType = FLOAT_LIT;
	    yylval.dval = atof(tokStr);
	} else {
	    tokType = INT_LIT;
	    yylval.ival = atoi(tokStr);
	}

	return;

    }

    /*
     * Recognize a quoted string
     */
    if (c == '\"') {

	c = MyFGetC(input_F);

	while ((len < MAX_TOK_LEN - 1) &&
			    (c != EOF) && (c != '\n') && (c != '\"')) {
	    *tok++ = c;
	    c = MyFGetC(input_F);
	}

	*tok = 0;

	c = MyFGetC(input_F);

	tokType = STRING_LIT;
	strncpy(yylval.sval, tokStr, MAX_TOK_LEN - 1);
	yylval.sval[MAX_TOK_LEN - 1] = 0;

	return;

    }

    /*
     * Recognize an identifier and try to match it with a keyword.
     * Identifiers begin with a letter and continue in letters and/or
     * digits.  Convert it to lowercase.
     */
    if (isalpha(c) || (c == '_') || (c == '$')) {

        if (isupper(c))
	  c = tolower(c);
        *tok++ = c;
	c = MyFGetC(input_F);
	len++;

	while ((len < MAX_TOK_LEN - 1) && (isalpha(c) || isdigit(c) ||
						(c == '_') || (c == '$'))) {
	    if (isupper(c))
	      c = tolower(c);
	    *tok++ = c;
	    c = MyFGetC(input_F);
	    len++;
	}

	*tok = 0;

	/*
	 * We've got the identifier; see if it matches any keywords.
	 */

	{
		generic_t gval;
		int type;
		if (St_GetSym(keywordT_P, tokStr, &type, &gval) ==
								ST_SYM_FOUND) {
			tokType = gval.i;
		        strncpy(yylval.sval, tokStr, MAX_TOK_LEN - 1);
			yylval.sval[MAX_TOK_LEN - 1] = 0;
		} else if (!strcmp(tokStr, "true")) {
		    tokType = BOOL_LIT;
		    yylval.ival = 1;
		} else if (!strcmp(tokStr, "false")) {
		    tokType = BOOL_LIT;
		    yylval.ival = 0;
		} else {
		    tokType = IDENTIFIER;
		    strncpy(yylval.sval, tokStr, MAX_TOK_LEN - 1);
		    yylval.sval[MAX_TOK_LEN - 1] = 0;
		}
	}

	return;

    }

    /*
     * Recognize punctuation
     */

    tokType = c;
    *tok++ = c;
    *tok = 0;
    c = MyFGetC(input_F);
    return;

}

void ErrMsg(s)
char *s;
{
	int i;

	fprintf(stderr, "Error: %s at line %d\n", s, lineNo);
	fprintf(stderr, "%s", inputBuf);
	for (i = 0; i < inputBufp - inputBuf; i++) {
		fputc((inputBuf[i] == '\t' ? '\t' : ' '), stderr);
	}
	fprintf(stderr, "^ before here\n\n");
}

int yyerror(s)
char *s;
{
	ErrMsg(s);
}


int yylex()
{
	Advance(input_F);
	return(tokType);
}

void AddSpell(s)
char *s;
{
	generic_t gval;
	int type;
	char s1[256];

	if (St_GetSym(spellT_P, s, &type, &gval) != ST_SYM_FOUND) {
		sprintf(s1, "unknown spell '%s'", s);
		ErrMsg(s1);
	} else {
		tmpTemplate.val.spells |= (1 << gval.i);
		tmpTemplate.def.spell = TRUE;
	}
}


void NegSpell(s)
char *s;
{
	generic_t gval;
	int type;
	char s1[256];

	if (St_GetSym(spellT_P, s, &type, &gval) != ST_SYM_FOUND) {
		sprintf(s1, "unknown spell '%s'", s);
		ErrMsg(s1);
	} else {
		tmpTemplate.val.spells &= ~(1 << gval.i);
		tmpTemplate.def.spell = TRUE;
	}
}


void AddBreath(s)
char *s;
{
	generic_t gval;
	int type;
	char s1[256];

	if (St_GetSym(breathT_P, s, &type, &gval) != ST_SYM_FOUND) {
		sprintf(s1, "unknown breath '%s'", s);
		ErrMsg(s1);
	} else {
		tmpTemplate.val.spells |= (1 << gval.i);
		tmpTemplate.def.breath = TRUE;
	}
}


void NegBreath(s)
char *s;
{
	generic_t gval;
	int type;
	char s1[256];

	if (St_GetSym(breathT_P, s, &type, &gval) != ST_SYM_FOUND) {
		sprintf(s1, "unknown breath '%s'", s);
		ErrMsg(s1);
	} else {
		tmpTemplate.val.spells &= ~(1 << gval.i);
		tmpTemplate.def.breath = TRUE;
	}
}


void AddSpecial(s)
char *s;
{
	generic_t gval;
	int type;
	char s1[256];

	if (St_GetSym(specialT_P, s, &type, &gval) != ST_SYM_FOUND) {
		sprintf(s1, "unknown special '%s'", s);
		ErrMsg(s1);
	} else {
		tmpTemplate.val.cmove |= (1 << gval.i);
		tmpTemplate.def.special = TRUE;
	}
}


void NegSpecial(s)
char *s;
{
	generic_t gval;
	int type;
	char s1[256];

	if (St_GetSym(specialT_P, s, &type, &gval) != ST_SYM_FOUND) {
		sprintf(s1, "unknown special '%s'", s);
		ErrMsg(s1);
	} else {
		tmpTemplate.val.cmove &= ~(1 << gval.i);
		tmpTemplate.def.special = TRUE;
	}
}


void AddMove(s)
char *s;
{
	generic_t gval;
	int type;
	char s1[256];

	if (St_GetSym(moveT_P, s, &type, &gval) != ST_SYM_FOUND) {
		sprintf(s1, "unknown move '%s'", s);
		ErrMsg(s1);
	} else {
		tmpTemplate.val.cmove |= (1 << gval.i);
		tmpTemplate.def.move = TRUE;
	}
}


void NegMove(s)
char *s;
{
	generic_t gval;
	int type;
	char s1[256];

	if (St_GetSym(moveT_P, s, &type, &gval) != ST_SYM_FOUND) {
		sprintf(s1, "unknown move '%s'", s);
		ErrMsg(s1);
	} else {
		tmpTemplate.val.cmove &= ~(1 << gval.i);
		tmpTemplate.def.move = TRUE;
	}
}


void AddTreasure(s)
char *s;
{
	generic_t gval;
	int type;
	char s1[256];

	if (St_GetSym(treasureT_P, s, &type, &gval) != ST_SYM_FOUND) {
		sprintf(s1, "unknown treasure '%s'", s);
		ErrMsg(s1);
	} else {
		tmpTemplate.val.cmove |= (1 << gval.i);
		tmpTemplate.def.treasure = TRUE;
	}
}


void NegTreasure(s)
char *s;
{
	generic_t gval;
	int type;
	char s1[256];

	if (St_GetSym(treasureT_P, s, &type, &gval) != ST_SYM_FOUND) {
		sprintf(s1, "unknown treasure '%s'", s);
		ErrMsg(s1);
	} else {
		tmpTemplate.val.cmove &= ~(1 << gval.i);
		tmpTemplate.def.treasure = TRUE;
	}
}


void AddDefense(s)
char *s;
{
	generic_t gval;
	int type;
	char s1[256];

	if (St_GetSym(defenseT_P, s, &type, &gval) != ST_SYM_FOUND) {
		sprintf(s1, "unknown defense '%s'", s);
		ErrMsg(s1);
	} else {
		tmpTemplate.val.cdefense |= (1 << gval.i);
		tmpTemplate.def.defense = TRUE;
	}
}


void NegDefense(s)
char *s;
{
	generic_t gval;
	int type;
	char s1[256];

	if (St_GetSym(defenseT_P, s, &type, &gval) != ST_SYM_FOUND) {
		sprintf(s1, "unknown defense '%s'", s);
		ErrMsg(s1);
	} else {
		tmpTemplate.val.cdefense &= ~(1 << gval.i);
		tmpTemplate.def.defense = TRUE;
	}
}


int PutAttack(attack)
attack_t attack;
{
	register int i;

	for (i = 0; i < attackCt; i++) {
		if ((attack.type == attackList[i].type) &&
		    (attack.desc == attackList[i].desc) &&
		    (attack.dice == attackList[i].dice) &&
		    (attack.sides == attackList[i].sides)) {
		    	return(i);
		}
	}

	if (attackCt == MAX_ATTACK) {
		fprintf(stderr, "fatal error: too many different attacks.\n");
		fprintf(stderr, "increase MAX_ATTACK.\n");
		exit(1);
	}

	attackList[attackCt].type = attack.type;
	attackList[attackCt].desc = attack.desc;
	attackList[attackCt].dice = attack.dice;
	attackList[attackCt].sides = attack.sides;

	return attackCt++;
}

void AddAttack(s1, dice, sides, s2)
char *s1, *s2;
int dice, sides;
{
	generic_t gval;
	int type, aDesc;
	attack_t attack;
	char s[256];

	if (St_GetSym(attackDescT_P, s1, &type, &gval) != ST_SYM_FOUND) {
		sprintf(s, "unknown attack description '%s'", s1);
		ErrMsg(s);
		return;
	} else {
		aDesc = gval.i;
	}

	if (St_GetSym(attackTypeT_P, s2, &type, &gval) != ST_SYM_FOUND) {
		sprintf(s, "unknown attack type '%s'", s2);
		ErrMsg(s);
	} else {
		if (creatureAttacks > 3) {
			sprintf(s, "creature limited to 4 attacks");
			ErrMsg(s);
			return;
		}
		attack.type = gval.i;
		attack.dice = dice;
		attack.desc = aDesc;
		attack.sides = sides;
		tmpTemplate.val.damage[creatureAttacks++] =
						PutAttack(attack);
		tmpTemplate.def.damage = TRUE;
	}
}

st_Table_Pt InitTable(name, init)
char *name;
symInit_t *init;
{
	int i;
	st_Table_Pt table_P;
	generic_t gval;

	table_P = St_NewTable(name, 20);
	for (i = 0; init[i].name; i++) {
		gval.i = init[i].val;
		St_DefSym(table_P, init[i].name, GEN_TYPE_INT, gval);
	}

	return table_P;
}


void InitTables()
{
	int i;
	generic_t gval;

	keywordT_P = InitTable("keyword", keywordInit);
	defenseT_P = InitTable("defense", defenseInit);
	spellT_P = InitTable("spell", spellInit);
	moveT_P = InitTable("move", moveInit);
	specialT_P = InitTable("special", specialInit);
	breathT_P = InitTable("breath", breathInit);
	treasureT_P = InitTable("treasure", treasureInit);
	attackTypeT_P = InitTable("attackType", attackTypeInit);
	attackDescT_P = InitTable("attackDesc", attackDescInit);

	classT_P = St_NewTable("class", 40);
	creatureT_P = St_NewTable("creature", 200);
}

void WriteCreature(tmpl_P)
template_t *tmpl_P;
{
	char s[256];
	strcpy(s, "\"");
	strcat(s, tmpl_P->val.name);
	strcat(s, "\"");
	printf("{%-28s, 0x%08x,0x%06x,0x%04x,%5d,%3d,\n",
		s, tmpl_P->val.cmove, tmpl_P->val.spells,
		tmpl_P->val.cdefense, tmpl_P->val.mexp, tmpl_P->val.sleep);
	printf(" %2d, %3d, %2d, '%c', {%3d,%2d}, {%3d,%3d,%3d,%3d}, %3d},\n",
		tmpl_P->val.aaf, tmpl_P->val.ac, tmpl_P->val.speed,
		tmpl_P->val.cchar,
		tmpl_P->val.hd[0], tmpl_P->val.hd[1],
		tmpl_P->val.damage[0], tmpl_P->val.damage[1],
		tmpl_P->val.damage[2], tmpl_P->val.damage[3],
		tmpl_P->val.level);
}

void WriteCreatures()
{
	char **s_A, **sp;
	int level, type;
	generic_t gval;

	s_A = St_SListTable(creatureT_P);

	printf("creature_type c_list[MAX_CREATURES] = {\n");

	for (level = 0; level <= maxCreatureLevel; level++) {
	    for (sp = s_A; *sp; sp++) {
		if (St_GetSym(creatureT_P, *sp, &type, &gval) !=
							ST_SYM_FOUND) {
			fprintf(stderr, "internal err. in WriteCreatures\n");
			exit(1);
		}
		if ((*(template_t *) gval.v).val.level == level) {
			WriteCreature((template_t *) gval.v);
		}
	   }
	}

	printf("};\n\n");

	St_SListTable(NULL);
}

void PutClassTemplate(s, tmpl_P)
char *s;
template_t *tmpl_P;
{
	generic_t gval;
	char s1[256];

	gval.v = malloc(sizeof(template_t));
	*(template_t *) gval.v = *tmpl_P;

	if (St_DefSym(classT_P, s, GEN_TYPE_TMPL, gval) == ST_SYM_FOUND) {
		sprintf(s1, "attempt to redefine class '%s'", s);
		ErrMsg(s1);
		free(gval.v);
		return;
	}
}


template_t GetClassTemplate(s)
char *s;
{
	generic_t gval;
	int type;
	char s1[256];

	if (St_GetSym(classT_P, s, &type, &gval) != ST_SYM_FOUND) {
		sprintf(s1, "class '%s' undefined\n", s);
		ErrMsg(s1);
		return blankTemplate;
	} else {
		return *(template_t *) gval.v;
	}
}




void NotDefined(name, s)
char *name, *s;
{
	printf("Warning: %s not defined for \"%s\", line %d\n",
		s, name, lineNo);
}


void PutCreature(s, tmpl_P)
char *s;
template_t *tmpl_P;
{
	generic_t gval;
	char s1[256];

	gval.v = malloc(sizeof(template_t));
	*(template_t *) gval.v = *tmpl_P;

	if (!tmpl_P->def.move)
		NotDefined(tmpl_P->val.name, "MOVE");
	if (!tmpl_P->def.treasure)
		NotDefined(tmpl_P->val.name, "TREASURE");
	if (!tmpl_P->def.defense)
		NotDefined(tmpl_P->val.name, "DEFENSE");
	if (!tmpl_P->def.mexp)
		NotDefined(tmpl_P->val.name, "XP");
	if (!tmpl_P->def.sleep)
		NotDefined(tmpl_P->val.name, "SLEEP");
	if (!tmpl_P->def.aaf)
		NotDefined(tmpl_P->val.name, "RADIUS");
	if (!tmpl_P->def.ac)
		NotDefined(tmpl_P->val.name, "AC");
	if (!tmpl_P->def.speed)
		NotDefined(tmpl_P->val.name, "SPEED");
	if (!tmpl_P->def.cchar)
		NotDefined(tmpl_P->val.name, "CCHAR");
	if (!tmpl_P->def.hd)
		NotDefined(tmpl_P->val.name, "HD");
	if (!tmpl_P->def.damage)
		NotDefined(tmpl_P->val.name, "ATTACK");
	if (!tmpl_P->def.level)
		NotDefined(tmpl_P->val.name, "LEVEL");

	if (St_DefSym(creatureT_P, s, GEN_TYPE_TMPL, gval) == ST_SYM_FOUND) {
		sprintf(s1, "attempt to redefine creature '%s'\n", s);
		ErrMsg(s1);
		free(gval.v);
		return;
	}

	if (tmpl_P->val.level > maxCreatureLevel)
		maxCreatureLevel = tmpl_P->val.level;

}

void WriteAttacks()
{
	int i;

	printf("struct m_attack_type monster_attacks[N_MONS_ATTS] = {\n");
	for (i = 0; i < attackCt; i++) {
		printf("/* %3d */ { %2d, %2d, %2d, %2d },\n",
			i, attackList[i].type, attackList[i].desc,
			attackList[i].dice, attackList[i].sides);
	};
	printf("};\n");
}

void WriteConstants()
{
  printf("/* These values should match the values defined in constant.h. */");
  printf("\n#define MAX_CREATURES\t%d\n", St_TableSize(creatureT_P));
  printf("#define N_MON_ATTS\t%d\n\n", attackCt);
}

void WriteMonsterHdr()
{
  printf("/* The following code belongs in the file monster.c. */\n\n");
  printf("/* The following data was generated by the program in util/mc.");
  printf("*/\n\n");
}

main(argc, argv)
int argc;
char *argv[];
{
	char inputFilename[256];

	InitTables();

	if (argc > 1) {
		strncpy(inputFilename, argv[1], 255);
		inputFilename[255] = 0;
	} else {
		fprintf(stderr, "input file: ");
		scanf("%255s", inputFilename);
	}

	input_F = fopen(inputFilename, "r");
	if (!input_F) {
		printf("Error: couldn't open file.\n");
		exit(1);
	}

	if (yyparse()) {
	    printf("Errors prevent continuation.\n");
	    exit(1);
	}

	WriteConstants();
	WriteMonsterHdr();
	WriteCreatures();
	WriteAttacks();
}
