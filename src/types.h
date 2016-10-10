/* source/types.h: global type declarations
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

/* Use ISO C99 standard declarations to get correct lengths. */
typedef int_least16_t int16;
typedef int_least32_t int32;
typedef uint_least32_t int32u;

/* some machines will not accept 'signed char' as a type, and some accept it
   but still treat it like an unsigned character, let's just avoid it,
   any variable which can ever hold a negative value must be 16 or 32 bits */

#define VTYPESIZ 80
#define BIGVTYPESIZ 160
typedef char vtype[VTYPESIZ];
/* note that since its output can easily exceed 80 characters, objdes must
   always be called with a bigvtype as the first paramter */
typedef char bigvtype[BIGVTYPESIZ];
typedef char stat_type[7];

/* Many of the character fields used to be fixed length, which greatly
 * increased the size of the executable.  I have replaced many fixed
 * length fields with variable length ones.
 *
 * all fields are given the smallest possbile type, and all fields are
 * aligned within the structure to their natural size boundary, so that
 * the structures contain no padding and are minimum size.
 *
 * bit fields are only used where they would cause a large reduction in
 * data size, they should not be used otherwise because their use
 * results in larger and slower code.
 */

typedef struct creature_type {
    char *name;         /* Descrip of creature */
    int32u cmove;       /* Bit field */
    int32u spells;      /* Creature spells */
    uint16_t cdefense;  /* Bit field */
    uint16_t mexp;      /* Exp value for kill */
    uint8_t sleep;      /* Inactive counter/10 */
    uint8_t aaf;        /* Area affect radius */
    uint8_t ac;         /* AC */
    uint8_t speed;      /* Movement speed+10 */
    uint8_t cchar;      /* Character rep. */
    uint8_t hd[2];      /* Creatures hit die */
    uint8_t damage[4];  /* Type attack and damage*/
    uint8_t level;      /* Level of creature */
} creature_type;

/* Monster attack and damage types */
typedef struct m_attack_type {
    uint8_t attack_type;
    uint8_t attack_desc;
    uint8_t attack_dice;
    uint8_t attack_sides;
} m_attack_type;

/* Monster memories. -CJS- */
typedef struct recall_type {
    int32u r_cmove;
    int32u r_spells;
    uint16_t r_kills, r_deaths;
    uint16_t r_cdefense;
    uint8_t r_wake, r_ignore;
    uint8_t r_attacks[MAX_MON_NATTACK];
} recall_type;

typedef struct monster_type {
    int16 hp;         /* Hit points */
    int16 csleep;     /* Inactive counter */
    int16 cspeed;     /* Movement speed */
    uint16_t mptr;    /* Pointer into creature*/

    /* Note: fy, fx, and cdis constrain dungeon size to less than 256 by 256 */
    uint8_t fy;       /* Y Pointer into map */
    uint8_t fx;       /* X Pointer into map */
    uint8_t cdis;     /* Cur dis from player */

    uint8_t ml;
    uint8_t stunned;
    uint8_t confused;
} monster_type;

typedef struct treasure_type {
    char *name;         /* Object name */
    int32u flags;       /* Special flags */
    uint8_t tval;       /* Category number */
    uint8_t tchar;      /* Character representation*/
    int16 p1;           /* Misc. use variable */
    int32 cost;         /* Cost of item */
    uint8_t subval;     /* Sub-category number */
    uint8_t number;     /* Number of items */
    uint16_t weight;    /* Weight */
    int16 tohit;        /* Plusses to hit */
    int16 todam;        /* Plusses to damage */
    int16 ac;           /* Normal AC */
    int16 toac;         /* Plusses to AC */
    uint8_t damage[2];  /* Damage when hits */
    uint8_t level;      /* Level item first found */
} treasure_type;

/* only damage, ac, and tchar are constant; level could possibly be made
 * constant by changing index instead; all are used rarely.
 *
 * extra fields x and y for location in dungeon would simplify pusht().
 *
 * making inscrip a pointer and mallocing space does not work, there are
 * two many places where inven_types are copied, which results in dangling
 * pointers, so we use a char array for them instead
 */
#define INSCRIP_SIZE 13 /* notice alignment, must be 4*x + 1 */
typedef struct inven_type {
    uint16_t index;                 /* Index to object_list */
    uint8_t name2;                  /* Object special name */
    char inscrip[INSCRIP_SIZE];     /* Object inscription */
    int32u flags;                   /* Special flags */
    uint8_t tval;                   /* Category number */
    uint8_t tchar;                  /* Character representation*/
    int16 p1;                       /* Misc. use variable */
    int32 cost;                     /* Cost of item */
    uint8_t subval;                 /* Sub-category number */
    uint8_t number;                 /* Number of items */
    uint16_t weight;                /* Weight */
    int16 tohit;                    /* Plusses to hit */
    int16 todam;                    /* Plusses to damage */
    int16 ac;                       /* Normal AC */
    int16 toac;                     /* Plusses to AC */
    uint8_t damage[2];              /* Damage when hits */
    uint8_t level;                  /* Level item first found */
    uint8_t ident;                  /* Identify information */
} inven_type;

#define PLAYER_NAME_SIZE 27

typedef struct player_type {
    struct misc {
        char name[PLAYER_NAME_SIZE];  /* Name of character */
        uint8_t male;                 /* Sex of character */
        int32 au;                     /* Gold */
        int32 max_exp;                /* Max experience */
        int32 exp;                    /* Cur experience */
        uint16_t exp_frac;            /* Cur exp fraction * 2^16 */
        uint16_t age;                 /* Characters age */
        uint16_t ht;                  /* Height */
        uint16_t wt;                  /* Weight */
        uint16_t lev;                 /* Level */
        uint16_t max_dlv;             /* Max level explored */
        int16 srh;                    /* Chance in search */
        int16 fos;                    /* Frenq of search */
        int16 bth;                    /* Base to hit */
        int16 bthb;                   /* BTH with bows */
        int16 mana;                   /* Mana points */
        int16 mhp;                    /* Max hit pts */
        int16 ptohit;                 /* Plusses to hit */
        int16 ptodam;                 /* Plusses to dam */
        int16 pac;                    /* Total AC */
        int16 ptoac;                  /* Magical AC */
        int16 dis_th;                 /* Display +ToHit */
        int16 dis_td;                 /* Display +ToDam */
        int16 dis_ac;                 /* Display +ToAC */
        int16 dis_tac;                /* Display +ToTAC */
        int16 disarm;                 /* % to Disarm */
        int16 save;                   /* Saving throw */
        int16 sc;                     /* Social Class */
        int16 stl;                    /* Stealth factor */
        uint8_t pclass;               /* # of class */
        uint8_t prace;                /* # of race */
        uint8_t hitdie;               /* Char hit die */
        uint8_t expfact;              /* Experience factor */
        int16 cmana;                  /* Cur mana pts */
        uint16_t cmana_frac;          /* Cur mana fraction * 2^16 */
        int16 chp;                    /* Cur hit pts */
        uint16_t chp_frac;            /* Cur hit fraction * 2^16 */
        char history[4][60];          /* History record */
    } misc;

    /* Stats now kept in arrays, for more efficient access. -CJS- */
    struct stats {
        uint8_t max_stat[6];          /* What is restored */
        uint8_t cur_stat[6];          /* What is natural */
        int16 mod_stat[6];            /* What is modified, may be +/- */
        uint8_t use_stat[6];          /* What is used */
    } stats;

    struct flags {
        int32u status;                /* Status of player */
        int16 rest;                   /* Rest counter */
        int16 blind;                  /* Blindness counter */
        int16 paralysis;              /* Paralysis counter */
        int16 confused;               /* Confusion counter */
        int16 food;                   /* Food counter */
        int16 food_digested;          /* Food per round */
        int16 protection;             /* Protection fr. evil */
        int16 speed;                  /* Cur speed adjust */
        int16 fast;                   /* Temp speed change */
        int16 slow;                   /* Temp speed change */
        int16 afraid;                 /* Fear */
        int16 poisoned;               /* Poisoned */
        int16 image;                  /* Hallucinate */
        int16 protevil;               /* Protect VS evil */
        int16 invuln;                 /* Increases AC */
        int16 hero;                   /* Heroism */
        int16 shero;                  /* Super Heroism */
        int16 blessed;                /* Blessed */
        int16 resist_heat;            /* Timed heat resist */
        int16 resist_cold;            /* Timed cold resist */
        int16 detect_inv;             /* Timed see invisible */
        int16 word_recall;            /* Timed teleport level*/
        int16 see_infra;              /* See warm creatures */
        int16 tim_infra;              /* Timed infra vision */
        uint8_t see_inv;              /* Can see invisible */
        uint8_t teleport;             /* Random teleportation*/
        uint8_t free_act;             /* Never paralyzed */
        uint8_t slow_digest;          /* Lower food needs */
        uint8_t aggravate;            /* Aggravate monsters */
        uint8_t fire_resist;          /* Resistance to fire */
        uint8_t cold_resist;          /* Resistance to cold */
        uint8_t acid_resist;          /* Resistance to acid */
        uint8_t regenerate;           /* Regenerate hit pts */
        uint8_t lght_resist;          /* Resistance to light */
        uint8_t ffall;                /* No damage falling */
        uint8_t sustain_str;          /* Keep strength */
        uint8_t sustain_int;          /* Keep intelligence */
        uint8_t sustain_wis;          /* Keep wisdom */
        uint8_t sustain_con;          /* Keep constitution */
        uint8_t sustain_dex;          /* Keep dexterity */
        uint8_t sustain_chr;          /* Keep charisma */
        uint8_t confuse_monster;      /* Glowing hands. */
        uint8_t new_spells;           /* Number of spells can learn. */
    } flags;
} player_type;

/* spell name is stored in spell_names[] array at index i, +31 if priest */
typedef struct spell_type {
    uint8_t slevel;
    uint8_t smana;
    uint8_t sfail;
    uint8_t sexp;     /* 1/4 of exp gained for learning spell */
} spell_type;

typedef struct race_type {
    char *trace;              /* Type of race */
    int16 str_adj;            /* adjustments */
    int16 int_adj;
    int16 wis_adj;
    int16 dex_adj;
    int16 con_adj;
    int16 chr_adj;
    uint8_t b_age;            /* Base age of character */
    uint8_t m_age;            /* Maximum age of character */
    uint8_t m_b_ht;           /* base height for males */
    uint8_t m_m_ht;           /* mod height for males */
    uint8_t m_b_wt;           /* base weight for males */
    uint8_t m_m_wt;           /* mod weight for males */
    uint8_t f_b_ht;           /* base height females */
    uint8_t f_m_ht;           /* mod height for females */
    uint8_t f_b_wt;           /* base weight for female */
    uint8_t f_m_wt;           /* mod weight for females */
    int16 b_dis;              /* base chance to disarm */
    int16 srh;                /* base chance for search */
    int16 stl;                /* Stealth of character */
    int16 fos;                /* frequency of auto search */
    int16 bth;                /* adj base chance to hit */
    int16 bthb;               /* adj base to hit with bows */
    int16 bsav;               /* Race base for saving throw */
    uint8_t bhitdie;          /* Base hit points for race */
    uint8_t infra;            /* See infra-red */
    uint8_t b_exp;            /* Base experience factor */
    uint8_t rtclass;          /* Bit field for class types */
} race_type;

typedef struct class_type {
    char *title;              /* type of class */
    uint8_t adj_hd;           /* Adjust hit points */
    uint8_t mdis;             /* mod disarming traps */
    uint8_t msrh;             /* modifier to searching */
    uint8_t mstl;             /* modifier to stealth */
    uint8_t mfos;             /* modifier to freq-of-search */
    uint8_t mbth;             /* modifier to base to hit */
    uint8_t mbthb;            /* modifier to base to hit - bows*/
    uint8_t msav;             /* Class modifier to save */
    int16 madj_str;           /* Class modifier for strength */
    int16 madj_int;           /* Class modifier for intelligence*/
    int16 madj_wis;           /* Class modifier for wisdom */
    int16 madj_dex;           /* Class modifier for dexterity */
    int16 madj_con;           /* Class modifier for constitution*/
    int16 madj_chr;           /* Class modifier for charisma */
    uint8_t spell;            /* class use mage spells */
    uint8_t m_exp;            /* Class experience factor */
    uint8_t first_spell_lev;  /* First level where class can use spells. */
} class_type;

typedef struct background_type {
    char *info;               /* History information */
    uint8_t roll;             /* Die roll needed for history */
    uint8_t chart;            /* Table number */
    uint8_t next;             /* Pointer to next table */
    uint8_t bonus;            /* Bonus to the Social Class+50 */
} background_type;

typedef struct cave_type {
    uint8_t cptr;
    uint8_t tptr;
    uint8_t fval;

    unsigned int lr : 1;      /* room should be lit with perm light, walls with
                                 this set should be perm lit after tunneled out */
    unsigned int fm : 1;      /* field mark, used for traps/doors/stairs, object is
                                 hidden if fm is FALSE */
    unsigned int pl : 1;      /* permanent light, used for walls and lighted rooms */
    unsigned int tl : 1;      /* temporary light, used for player's lamp light,etc.*/
} cave_type;

typedef struct owner_type {
    char *owner_name;
    int16 max_cost;
    uint8_t max_inflate;
    uint8_t min_inflate;
    uint8_t haggle_per;
    uint8_t owner_race;
    uint8_t insult_max;
} owner_type;

typedef struct inven_record {
    int32 scost;
    inven_type sitem;
} inven_record;

typedef struct store_type {
    int32 store_open;
    int16 insult_cur;
    uint8_t owner;
    uint8_t store_ctr;
    uint16_t good_buy;
    uint16_t bad_buy;
    inven_record store_inven[STORE_INVEN_MAX];
} store_type;

/* 64 bytes for this structure */
typedef struct high_scores {
    int32 points;
    int32 birth_date;
    int16 uid;
    int16 mhp;
    int16 chp;
    uint8_t dun_level;
    uint8_t lev;
    uint8_t max_dlv;
    uint8_t sex;
    uint8_t race;
    uint8_t class;
    char name[PLAYER_NAME_SIZE];
    char died_from[25];
} high_scores;
