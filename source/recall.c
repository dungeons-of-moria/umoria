/* source/recall.c: print out monster memory info			-CJS-

   Copyright (c) 1989-92 James E. Wilson, Christopher J. Stuart

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#ifdef __TURBOC__
#include	<stdio.h>
#endif /* __TURBOC__ */
 
#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"

#if defined(LINT_ARGS)
static void roff(char *);
#else
static void roff();
#endif

static char *desc_atype[] = {
  "do something undefined",
  "attack",
  "weaken",
  "confuse",
  "terrify",
  "shoot flames",
  "shoot acid",
  "freeze",
  "shoot lightning",
  "corrode",
  "blind",
  "paralyse",
  "steal money",
  "steal things",
  "poison",
  "reduce dexterity",
  "reduce constitution",
  "drain intelligence",
  "drain wisdom",
  "lower experience",
  "call for help",
  "disenchant",
  "eat your food",
  "absorb light",
  "absorb charges" };
static char *desc_amethod[] = {
  "make an undefined advance",
  "hit",
  "bite",
  "claw",
  "sting",
  "touch",
  "kick",
  "gaze",
  "breathe",
  "spit",
  "wail",
  "embrace",
  "crawl on you",
  "release spores",
  "beg",
  "slime you",
  "crush",
  "trample",
  "drool",
  "insult" };
static char *desc_howmuch[] = {
  " not at all",
  " a bit",
  "",
  " quite",
  " very",
  " most",
  " highly",
  " extremely" };

static char *desc_move[] = {
  "move invisibly",
  "open doors",
  "pass through walls",
  "kill weaker creatures",
  "pick up objects",
  "breed explosively" };
static char *desc_spell[] = {
  "teleport short distances",
  "teleport long distances",
  "teleport its prey",
  "cause light wounds",
  "cause serious wounds",
  "paralyse its prey",
  "induce blindness",
  "confuse",
  "terrify",
  "summon a monster",
  "summon the undead",
  "slow its prey",
  "drain mana",
  "unknown 1",
  "unknown 2" };
static char *desc_breath[] = {
  "lightning",
  "poison gases",
  "acid",
  "frost",
  "fire" };
static char *desc_weakness[] = {
  "frost",
  "fire",
  "poison",
  "acid",
  "bright light",
  "rock remover" };

static vtype roffbuf;		/* Line buffer. */
static char *roffp;		/* Pointer into line buffer. */
static int roffpline;		/* Place to print line now being loaded. */

#define plural(c, ss, sp)	((c) == 1 ? ss : sp)

/* Number of kills needed for information. */

/* the higher the level of the monster, the fewer the kills you need */
#define knowarmor(l,d)		((d) > 304 / (4 + (l)))
/* the higher the level of the monster, the fewer the attacks you need,
   the more damage an attack does, the more attacks you need */
#define knowdamage(l,a,d)	((4 + (l))*(a) > 80 * (d))

/* Do we know anything about this monster? */
int bool_roff_recall(mon_num)
int mon_num;
{
  register recall_type *mp;
  register int i;

  if (wizard)
    return TRUE;
  mp = &c_recall[mon_num];
  if (mp->r_cmove || mp->r_cdefense || mp->r_kills || mp->r_spells
      || mp->r_deaths)
    return TRUE;
  for (i = 0; i < 4; i++)
    if (mp->r_attacks[i])
      return TRUE;
  return FALSE;
}

/* Print out what we have discovered about this monster. */
int roff_recall(mon_num)
int mon_num;
{
  char *p, *q;
  int8u *pu;
  vtype temp;
  register recall_type *mp;
  register creature_type *cp;
  register int i, k;
  register int32u j;
  int32 templong;
  int mspeed;
  int32u rcmove, rspells;
  int16u rcdefense;
  recall_type save_mem;
#ifdef ATARIST_MWC
  int32u holder;
  int32u holder2;
#endif

  mp = &c_recall[mon_num];
  cp = &c_list[mon_num];
  if (wizard)
    {
      save_mem = *mp;
      mp->r_kills = MAX_SHORT;
      mp->r_wake = mp->r_ignore = MAX_UCHAR;
#ifdef ATARIST_MWC
      j = (((cp->cmove & (holder = CM_4D2_OBJ)) != 0) * 8) +
	(((cp->cmove & (holder = CM_2D2_OBJ)) != 0) * 4) +
	  (((cp->cmove & (holder = CM_1D2_OBJ)) != 0) * 2) +
	    ((cp->cmove & (holder = CM_90_RANDOM)) != 0) +
	      ((cp->cmove & (holder = CM_60_RANDOM)) != 0);
      holder = CM_TREASURE;
      mp->r_cmove = (cp->cmove & ~holder) | (j << CM_TR_SHIFT);
#else
      j = (((cp->cmove & CM_4D2_OBJ) != 0) * 8) +
	(((cp->cmove & CM_2D2_OBJ) != 0) * 4) +
	  (((cp->cmove & CM_1D2_OBJ) != 0) * 2) +
	    ((cp->cmove & CM_90_RANDOM) != 0) +
	      ((cp->cmove & CM_60_RANDOM) != 0);
      mp->r_cmove = (cp->cmove & ~CM_TREASURE) | (j << CM_TR_SHIFT);
#endif
      mp->r_cdefense = cp->cdefense;
      mp->r_spells = cp->spells | CS_FREQ;
      j = 0;
      pu = cp->damage;
      while (*pu != 0 && j < 4)
	{
	  /* Turbo C needs a 16 bit int for the array index.  */
	  mp->r_attacks[(int)j] = MAX_UCHAR;
	  j++;
	  pu++;
	}
      /* A little hack to enable the display of info for Quylthulgs.  */
      if (mp->r_cmove & CM_ONLY_MAGIC)
	mp->r_attacks[0] = MAX_UCHAR;
    }
  roffpline = 0;
  roffp = roffbuf;
#ifdef ATARIST_MWC
  holder = ~CS_FREQ;
  rspells = mp->r_spells & cp->spells & holder;
  /* the CM_WIN property is always known, set it if a win monster */
  holder = CM_WIN;
  rcmove = mp->r_cmove | (holder & cp->cmove);
#else
  rspells = mp->r_spells & cp->spells & ~CS_FREQ;
  /* the CM_WIN property is always known, set it if a win monster */
  rcmove = mp->r_cmove | (CM_WIN & cp->cmove);
#endif
  rcdefense = mp->r_cdefense & cp->cdefense;
  (void) sprintf(temp, "The %s:\n", cp->name);
  roff(temp);
  /* Conflict history. */
  if(mp->r_deaths)
    {
      (void) sprintf(temp,
		     "%d of the contributors to your monster memory %s",
		     mp->r_deaths, plural(mp->r_deaths, "has", "have") );
      roff(temp);
      roff(" been killed by this creature, and ");
      if (mp->r_kills == 0)
	roff("it is not ever known to have been defeated.");
      else
	{
	  (void) sprintf(temp,
			 "at least %d of the beasts %s been exterminated.",
			 mp->r_kills, plural(mp->r_kills, "has", "have") );
	  roff(temp);
	}
    }
  else if (mp->r_kills)
    {
      (void) sprintf(temp, "At least %d of these creatures %s",
		     mp->r_kills, plural(mp->r_kills, "has", "have") );
      roff(temp);
      roff(" been killed by contributors to your monster memory.");
    }
  else
    roff("No known battles to the death are recalled.");
  /* Immediately obvious. */
  k = FALSE;
  if (cp->level == 0)
    {
      roff(" It lives in the town");
      k = TRUE;
    }
  else if (mp->r_kills)
    {
      /* The Balrog is a level 100 monster, but appears at 50 feet.  */
      i = cp->level;
      if (i > WIN_MON_APPEAR)
	i = WIN_MON_APPEAR;
      (void) sprintf(temp, " It is normally found at depths of %d feet",
		     i * 50);
      roff(temp);
      k = TRUE;
    }
  /* the c_list speed value is 10 greater, so that it can be a int8u */
  mspeed = cp->speed - 10;
  if (rcmove & CM_ALL_MV_FLAGS)
    {
      if (k)
	roff(", and");
      else
	{
	  roff(" It");
	  k = TRUE;
	}
      roff(" moves");
      if (rcmove & CM_RANDOM_MOVE)
	{
	  /* Turbo C needs a 16 bit int for the array index.  */
	  roff(desc_howmuch[(int)((rcmove & CM_RANDOM_MOVE) >> 3)]);
	  roff(" erratically");
	}
      if (mspeed == 1)
	roff(" at normal speed");
      else
	{
	  if (rcmove & CM_RANDOM_MOVE)
	    roff(", and");
	  if (mspeed <= 0)
	    {
	      if (mspeed == -1)
		roff(" very");
	      else if (mspeed < -1)
		roff(" incredibly");
	      roff(" slowly");
	    }
	  else
	    {
	      if (mspeed == 3)
		roff(" very");
	      else if (mspeed > 3)
		roff(" unbelievably");
	      roff(" quickly");
	    }
	}
    }
  if (rcmove & CM_ATTACK_ONLY)
    {
      if(k)
	roff(", but");
      else
	{
	  roff(" It");
	  k = TRUE;
	}
      roff(" does not deign to chase intruders");
    }
  if (rcmove & CM_ONLY_MAGIC)
    {
      if (k)
	roff (", but");
      else
	{
	  roff (" It");
	  k = TRUE;
	}
      roff (" always moves and attacks by using magic");
    }
  if(k)
    roff(".");
  /* Kill it once to know experience, and quality (evil, undead, monsterous).
     The quality of being a dragon is obvious. */
  if (mp->r_kills)
    {
      roff(" A kill of this");
      if (cp->cdefense & CD_ANIMAL)
	roff(" natural");
      if (cp->cdefense & CD_EVIL)
	roff(" evil");
      if (cp->cdefense & CD_UNDEAD)
	roff(" undead");

      /* calculate the integer exp part, can be larger than 64K when first
	 level character looks at Balrog info, so must store in long */
      templong = (long)cp->mexp * cp->level / py.misc.lev;
      /* calculate the fractional exp part scaled by 100,
	 must use long arithmetic to avoid overflow */
      j = (((long)cp->mexp * cp->level % py.misc.lev) * (long)1000 /
	   py.misc.lev+5) / 10;

      (void) sprintf(temp, " creature is worth %ld.%02ld point%s", templong,
		     j, (templong == 1 && j == 0 ? "" : "s"));
      roff(temp);

      if (py.misc.lev / 10 == 1) p = "th";
      else
	{
	  i = py.misc.lev % 10;
	  if (i == 1)		p = "st";
	  else if (i == 2)	p = "nd";
	  else if (i == 3)      p = "rd";
	  else			p = "th";
	}
      i = py.misc.lev;
      if (i == 8 || i == 11 || i == 18) q = "n";
      else				q = "";
      (void) sprintf(temp, " for a%s %d%s level character.", q, i, p);
      roff(temp);
    }
  /* Spells known, if have been used against us. */
  k = TRUE;
  j = rspells;
#ifdef ATARIST_MWC
  holder = CS_BREATHE;
  holder2 = CS_BR_LIGHT;
  for (i = 0; j & holder; i++)
#else
  for (i = 0; j & CS_BREATHE; i++)
#endif
    {
#ifdef ATARIST_MWC
      if (j & (holder2 << i))
#else
      if (j & (CS_BR_LIGHT << i))
#endif
	{
#ifdef ATARIST_MWC
	  j &= ~(holder2 << i);
#else
	  j &= ~(CS_BR_LIGHT << i);
#endif
	  if (k)
	    {
	      roff(" It can breathe ");
	      k = FALSE;
	    }
#ifdef ATARIST_MWC
	  else if (j & holder)
#else
	  else if (j & CS_BREATHE)
#endif
	    roff(", ");
	  else
	    roff(" and ");
	  roff(desc_breath[i]);
	}
    }
  k = TRUE;
#ifdef ATARIST_MWC
  holder = CS_SPELLS;
  for (i = 0; j & holder; i++)
#else
  for (i = 0; j & CS_SPELLS; i++)
#endif
    {
      if (j & (CS_TEL_SHORT << i))
	{
	  j &= ~(CS_TEL_SHORT << i);
	  if (k)
	    {
#ifdef ATARIST_MWC
	      holder2 = CS_BREATHE;
	      if (rspells & holder2)
#else
	      if (rspells & CS_BREATHE)
#endif
		roff(", and is also");
	      else
		roff(" It is");
	      roff(" magical, casting spells which ");
	      k = FALSE;
	    }
#ifdef ATARIST_MWC
	  else if (j & holder)
#else
	  else if (j & CS_SPELLS)
#endif
	    roff(", ");
	  else
	    roff(" or ");
	  roff(desc_spell[i]);
	}
    }
#ifdef ATARIST_MWC
  holder = CS_BREATHE|CS_SPELLS;
  if (rspells & holder)
#else
  if (rspells & (CS_BREATHE|CS_SPELLS))
#endif
    {
      if ((mp->r_spells & CS_FREQ) > 5)
	{	/* Could offset by level */
	  (void) sprintf(temp, "; 1 time in %ld", cp->spells & CS_FREQ);
	  roff(temp);
	}
      roff(".");
    }
  /* Do we know how hard they are to kill? Armor class, hit die. */
  if (knowarmor(cp->level, mp->r_kills))
    {
      (void) sprintf(temp, " It has an armor rating of %d", cp->ac);
      roff(temp);
      (void) sprintf(temp, " and a%s life rating of %dd%d.",
		     ((cp->cdefense & CD_MAX_HP) ? " maximized" : ""),
		     cp->hd[0], cp->hd[1]);
      roff(temp);
    }
  /* Do we know how clever they are? Special abilities. */
  k = TRUE;
  j = rcmove;
#ifdef ATARIST_MWC
  holder = CM_SPECIAL;
  holder2 = CM_INVISIBLE;
  for (i = 0; j & holder; i++)
#else
  for (i = 0; j & CM_SPECIAL; i++)
#endif
    {
#ifdef ATARIST_MWC
      if (j & (holder2 << i))
#else
      if (j & (CM_INVISIBLE << i))
#endif
	{
#ifdef ATARIST_MWC
	  j &= ~(holder2 << i);
#else
	  j &= ~(CM_INVISIBLE << i);
#endif
	  if (k)
	    {
	      roff(" It can ");
	      k = FALSE;
	    }
#ifdef ATARIST_MWC
	  else if (j & holder)
#else
	  else if (j & CM_SPECIAL)
#endif
	    roff(", ");
	  else
	    roff(" and ");
	  roff(desc_move[i]);
	}
    }
  if (!k)
    roff(".");
  /* Do we know its special weaknesses? Most cdefense flags. */
  k = TRUE;
  j = rcdefense;
  for (i = 0; j & CD_WEAKNESS; i++)
    {
      if (j & (CD_FROST << i))
	{
	  j &= ~(CD_FROST << i);
	  if (k)
	    {
	      roff(" It is susceptible to ");
	      k = FALSE;
	    }
	  else if (j & CD_WEAKNESS)
	    roff(", ");
	  else
	    roff(" and ");
	  roff(desc_weakness[i]);
	}
    }
  if (!k)
    roff(".");
  if (rcdefense & CD_INFRA)
    roff(" It is warm blooded");
  if (rcdefense & CD_NO_SLEEP)
    {
      if (rcdefense & CD_INFRA)
	roff(", and");
      else
	roff(" It");
      roff(" cannot be charmed or slept");
    }
  if (rcdefense & (CD_NO_SLEEP|CD_INFRA))
    roff(".");
  /* Do we know how aware it is? */
  if (((mp->r_wake * mp->r_wake) > cp->sleep) || mp->r_ignore == MAX_UCHAR ||
      (cp->sleep == 0 && mp->r_kills >= 10))
    {
      roff(" It ");
      if(cp->sleep > 200)
	roff("prefers to ignore");
      else if(cp->sleep > 95)
	roff("pays very little attention to");
      else if(cp->sleep > 75)
	roff("pays little attention to");
      else if(cp->sleep > 45)
	roff("tends to overlook");
      else if(cp->sleep > 25)
	roff("takes quite a while to see");
      else if(cp->sleep > 10)
	roff("takes a while to see");
      else if(cp->sleep > 5)
	roff("is fairly observant of");
      else if(cp->sleep > 3)
	roff("is observant of");
      else if(cp->sleep > 1)
	roff("is very observant of");
      else if(cp->sleep != 0)
	roff("is vigilant for");
      else
	roff("is ever vigilant for");
      (void) sprintf(temp, " intruders, which it may notice from %d feet.",
		     10 * cp->aaf);
      roff(temp);
    }
  /* Do we know what it might carry? */
#ifdef ATARIST_MWC
  holder = CM_CARRY_OBJ|CM_CARRY_BOLD;
  if (rcmove & holder)
#else
  if (rcmove & (CM_CARRY_OBJ|CM_CARRY_GOLD))
#endif
    {
      roff(" It may");
#ifdef ATARIST_MWC
      j = (rcmove & (holder = CM_TREASURE)) >> CM_TR_SHIFT;
#else
      j = (rcmove & CM_TREASURE) >> CM_TR_SHIFT;
#endif
      if (j == 1)
	{
#ifdef ATARIST_MWC
	  if ((cp->cmove & (holder = CM_TREASURE)) == CM_60_RANDOM)
#else
	  if ((cp->cmove & CM_TREASURE) == CM_60_RANDOM)
#endif
	    roff(" sometimes");
	  else
	    roff(" often");
	}
#ifdef ATARIST_MWC
      else if ((j == 2) && ((cp->cmove & (holder = CM_TREASURE)) ==
#else
      else if ((j == 2) && ((cp->cmove & CM_TREASURE) ==
#endif
			    (CM_60_RANDOM|CM_90_RANDOM)))
	roff (" often");
      roff(" carry");
      p = " objects";
      if (j == 1)
	p = " an object";
      else if (j == 2)
	roff(" one or two");
      else
	{
	  (void) sprintf(temp, " up to %ld", j);
	  roff(temp);
	}
#ifdef ATARIST_MWC
      if (rcmove & (holder = CM_CARRY_OBJ))
#else
      if (rcmove & CM_CARRY_OBJ)
#endif
	{
	  roff(p);
#ifdef ATARIST_MWC
	  if (rcmove & (holder = CM_CARRY_GOLD))
#else
	  if (rcmove & CM_CARRY_GOLD)
#endif
	    {
	      roff(" or treasure");
	      if (j > 1)
		roff("s");
	    }
	  roff(".");
	}
      else if (j != 1)
	roff(" treasures.");
      else
	roff(" treasure.");
    }

  /* We know about attacks it has used on us, and maybe the damage they do. */
  /* k is the total number of known attacks, used for punctuation */
  k = 0;
  for (j = 0; j < 4; j++)
    /* Turbo C needs a 16 bit int for the array index.  */
    if (mp->r_attacks[(int)j])
      k++;
  pu = cp->damage;
  /* j counts the attacks as printed, used for punctuation */
  j = 0;
  for (i = 0; *pu != 0 && i < 4; pu++, i++)
    {
      int att_type, att_how, d1, d2;

      /* don't print out unknown attacks */
      if (!mp->r_attacks[i])
	continue;

      att_type = monster_attacks[*pu].attack_type;
      att_how = monster_attacks[*pu].attack_desc;
      d1 = monster_attacks[*pu].attack_dice;
      d2 = monster_attacks[*pu].attack_sides;

      j++;
      if (j == 1)
	roff(" It can ");
      else if (j == k)
	roff(", and ");
      else
	roff(", ");

      if (att_how > 19)
	att_how = 0;
      roff(desc_amethod[att_how]);
      if (att_type != 1 || d1 > 0 && d2 > 0)
	{
	  roff(" to ");
	  if (att_type > 24)
	    att_type = 0;
	  roff(desc_atype[att_type]);
	  if (d1 && d2)
	    {
	      if (knowdamage(cp->level, mp->r_attacks[i], d1*d2))
		{
		  if (att_type == 19)	/* Loss of experience */
		    roff(" by");
		  else
		    roff(" with damage");
		  (void) sprintf(temp, " %dd%d", d1, d2 );
		  roff(temp);
		}
	    }
	}
    }
  if (j)
    roff(".");
  else if (k > 0 && mp->r_attacks[0] >= 10)
    roff(" It has no physical attacks.");
  else
    roff(" Nothing is known about its attack.");
  /* Always know the win creature. */
#ifdef ATARIST_MWC
  if (cp->cmove & (holder = CM_WIN))
#else
  if (cp->cmove & CM_WIN)
#endif
    roff(" Killing one of these wins the game!");
  roff("\n");
  prt("--pause--", roffpline, 0);
  if (wizard)
    *mp = save_mem;
  return inkey();
}

/* Print out strings, filling up lines as we go. */
static void roff(p)
register char *p;
{
  register char *q, *r;

  while (*p)
    {
      *roffp = *p;
      if (*p == '\n' || roffp >= roffbuf + sizeof(roffbuf)-1)
	{
	  q = roffp;
	  if (*p != '\n')
	    while (*q != ' ')
	      q--;
	  *q = 0;
	  prt(roffbuf, roffpline, 0);
	  roffpline++;
	  r = roffbuf;
	  while (q < roffp)
	    {
	      q++;
	      *r = *q;
	      r++;
	    }
	  roffp = r;
	}
      else
	roffp++;
      p++;
    }
}
