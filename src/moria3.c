/* source/moria3.c: misc code, mainly to handle player commands

   Copyright (C) 1989-2008 James E. Wilson, Robert A. Koeneke, 
                           David J. Grabiner

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

#include	<stdio.h>

#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"

#ifdef USG
#ifndef ATARIST_MWC
#include <string.h>
#endif
#else
#include <strings.h>
#endif

#if defined(LINT_ARGS)
static void hit_trap(int, int);
static void carry(int, int, int);
static int summon_object(int, int, int, int);
#endif


/* Player hit a trap.	(Chuckle)			-RAK-	*/
static void hit_trap(y, x)
int y, x;
{
  int i, ty, tx, num, dam;
  register cave_type *c_ptr;
  register struct misc *p_ptr;
  register inven_type *t_ptr;
  bigvtype tmp;

  end_find();
  change_trap(y, x);
  c_ptr = &cave[y][x];
  p_ptr = &py.misc;
  t_ptr = &t_list[c_ptr->tptr];
  dam = pdamroll(t_ptr->damage);
  switch(t_ptr->subval)
    {
    case 1:  /* Open pit*/
      msg_print("You fell into a pit!");
      if (py.flags.ffall)
	msg_print("You gently float down.");
      else
      {
	objdes(tmp, t_ptr, TRUE);
	take_hit(dam, tmp);
      }
      break;
    case 2: /* Arrow trap*/
      if (test_hit(125, 0, 0, p_ptr->pac+p_ptr->ptoac, CLA_MISC_HIT))
	{
	  objdes(tmp, t_ptr, TRUE);
	  take_hit(dam, tmp);
	  msg_print("An arrow hits you.");
	}
      else
	msg_print("An arrow barely misses you.");
      break;
    case 3: /* Covered pit*/
      msg_print("You fell into a covered pit.");
      if (py.flags.ffall)
	msg_print("You gently float down.");
      else
      {
	objdes(tmp, t_ptr, TRUE);
	take_hit(dam, tmp);
      }
      place_trap(y, x, 0);
      break;
    case 4: /* Trap door*/
      msg_print("You fell through a trap door!");
      new_level_flag = TRUE;
      dun_level++;
      if (py.flags.ffall)
	msg_print("You gently float down.");
      else
	{
	  objdes(tmp, t_ptr, TRUE);
	  take_hit(dam, tmp);
	}
      /* Force the messages to display before starting to generate the
	 next level.  */
      msg_print (CNIL);
      break;
    case 5: /* Sleep gas*/
      if (py.flags.paralysis == 0)
	{
	  msg_print("A strange white mist surrounds you!");
	  if (py.flags.free_act)
	    msg_print("You are unaffected.");
	  else
	    {
	      msg_print("You fall asleep.");
	      py.flags.paralysis += randint(10) + 4;
	    }
	}
      break;
    case 6: /* Hid Obj*/
      (void) delete_object(y, x);
      place_object(y, x, FALSE);
      msg_print("Hmmm, there was something under this rock.");
      break;
    case 7:  /* STR Dart*/
      if (test_hit(125, 0, 0, p_ptr->pac+p_ptr->ptoac, CLA_MISC_HIT))
	{
	  if (!py.flags.sustain_str)
	    {
	      (void) dec_stat(A_STR);
	      objdes(tmp, t_ptr, TRUE);
	      take_hit(dam, tmp);
	      msg_print("A small dart weakens you!");
	    }
	  else
	    msg_print("A small dart hits you.");
	}
      else
	msg_print("A small dart barely misses you.");
      break;
    case 8: /* Teleport*/
      teleport_flag = TRUE;
      msg_print("You hit a teleport trap!");
      /* Light up the teleport trap, before we teleport away.  */
      move_light (y, x, y, x);
      break;
    case 9: /* Rockfall*/
      take_hit(dam, "a falling rock");
      (void) delete_object(y, x);
      place_rubble(y, x);
      msg_print("You are hit by falling rock.");
      break;
    case 10: /* Corrode gas*/
      /* Makes more sense to print the message first, then damage an
	 object.  */
      msg_print("A strange red gas surrounds you.");
      corrode_gas("corrosion gas");
      break;
    case 11: /* Summon mon*/
      (void) delete_object(y, x);	/* Rune disappears.    */
      num = 2 + randint (3);
      for (i = 0; i < num; i++)
	{
	  ty = y;
	  tx = x;
	  (void) summon_monster(&ty, &tx, FALSE);
	}
      break;
    case 12: /* Fire trap*/
      msg_print("You are enveloped in flames!");
      fire_dam(dam, "a fire trap");
      break;
    case 13: /* Acid trap*/
      msg_print("You are splashed with acid!");
      acid_dam(dam, "an acid trap");
      break;
    case 14: /* Poison gas*/
      msg_print("A pungent green gas surrounds you!");
      poison_gas(dam, "a poison gas trap");
      break;
    case 15: /* Blind Gas */
      msg_print("A black gas surrounds you!");
      py.flags.blind += randint(50) + 50;
      break;
    case 16: /* Confuse Gas*/
      msg_print("A gas of scintillating colors surrounds you!");
      py.flags.confused += randint(15) + 15;
      break;
    case 17: /* Slow Dart*/
      if (test_hit(125, 0, 0, p_ptr->pac+p_ptr->ptoac, CLA_MISC_HIT))
	{
	  objdes(tmp, t_ptr, TRUE);
	  take_hit(dam, tmp);
	  msg_print("A small dart hits you!");
	  if (py.flags.free_act)
	    msg_print("You are unaffected.");
	  else
	    py.flags.slow += randint(20) + 10;
	}
      else
	msg_print("A small dart barely misses you.");
      break;
    case 18: /* CON Dart*/
      if (test_hit(125, 0, 0, p_ptr->pac+p_ptr->ptoac, CLA_MISC_HIT))
	{
	  if (!py.flags.sustain_con)
	    {
	      (void) dec_stat(A_CON);
	      objdes(tmp, t_ptr, TRUE);
	      take_hit(dam, tmp);
	      msg_print("A small dart saps your health!");
	    }
	  else
	    msg_print("A small dart hits you.");
	}
      else
	msg_print("A small dart barely misses you.");
      break;
    case 19: /*Secret Door*/
      break;
    case 99: /* Scare Mon*/
      break;

      /* Town level traps are special,	the stores.	*/
    case 101: /* General    */
      enter_store(0);
      break;
    case 102: /* Armory	    */
      enter_store(1);
      break;
    case 103: /* Weaponsmith*/
      enter_store(2);
      break;
    case 104: /* Temple	    */
      enter_store(3);
      break;
    case 105: /* Alchemy    */
      enter_store(4);
      break;
    case 106: /* Magic-User */
      enter_store(5);
      break;

    default:
      msg_print("Unknown trap value.");
      break;
    }
}


/* Return spell number and failure chance		-RAK-	*/
/* returns -1 if no spells in book
   returns 1 if choose a spell in book to cast
   returns 0 if don't choose a spell, i.e. exit with an escape */
int cast_spell(prompt, item_val, sn, sc)
char *prompt;
int item_val;
int *sn, *sc;
{
  int32u j;
  register int i, k;
  int spell[31], result, first_spell;
  register spell_type *s_ptr;

  result = -1;
  i = 0;
  j = inventory[item_val].flags;
  first_spell = bit_pos(&j);
  /* set j again, since bit_pos modified it */
  j = inventory[item_val].flags & spell_learned;
  s_ptr = magic_spell[py.misc.pclass-1];
  while (j)
    {
      k = bit_pos(&j);
      if (s_ptr[k].slevel <= py.misc.lev)
	{
	  spell[i] = k;
	  i++;
	}
    }
  if (i > 0)
    {
      result = get_spell(spell, i, sn, sc, prompt, first_spell);
      if (result && magic_spell[py.misc.pclass-1][*sn].smana > py.misc.cmana)
	{
	  if (class[py.misc.pclass].spell == MAGE)
	    result = get_check("You summon your limited strength to cast \
this one! Confirm?");
	  else
	    result = get_check("The gods may think you presumptuous for \
this! Confirm?");
	}
    }
  return(result);
}


/* Player is on an object.  Many things can happen based -RAK-	*/
/* on the TVAL of the object.  Traps are set off, money and most */
/* objects are picked up.  Some objects, such as open doors, just*/
/* sit there.						       */
static void carry(y, x, pickup)
int y, x;
int pickup;
{
  register int locn, i;
  bigvtype out_val, tmp_str;
  register cave_type *c_ptr;
  register inven_type *i_ptr;

  c_ptr = &cave[y][x];
  i_ptr = &t_list[c_ptr->tptr];
  i = t_list[c_ptr->tptr].tval;
  if (i <= TV_MAX_PICK_UP)
    {
      end_find();
      /* There's GOLD in them thar hills!      */
      if (i == TV_GOLD)
	{
	  py.misc.au += i_ptr->cost;
	  objdes(tmp_str, i_ptr, TRUE);
	  (void) sprintf(out_val,
			 "You have found %ld gold pieces worth of %s",
			 i_ptr->cost, tmp_str);
	  prt_gold();
	  (void) delete_object(y, x);
	  msg_print(out_val);
	}
      else
	{
	  if (inven_check_num(i_ptr))	   /* Too many objects?	    */
	    {			    /* Okay,  pick it up      */
	      if (pickup && prompt_carry_flag)
		{
		  objdes(tmp_str, i_ptr, TRUE);
		  /* change the period to a question mark */
		  tmp_str[strlen(tmp_str)-1] = '?';
		  (void) sprintf(out_val, "Pick up %s", tmp_str);
		  pickup = get_check(out_val);
		}
	      /* Check to see if it will change the players speed. */
	      if (pickup && !inven_check_weight(i_ptr))
		{
		  objdes(tmp_str, i_ptr, TRUE);
		  /* change the period to a question mark */
		  tmp_str[strlen(tmp_str)-1] = '?';
		  (void) sprintf(out_val,
				 "Exceed your weight limit to pick up %s",
				 tmp_str);
		  pickup = get_check(out_val);
		}
	      /* Attempt to pick up an object.	       */
	      if (pickup)
		{
		  locn = inven_carry(i_ptr);
		  objdes(tmp_str, &inventory[locn], TRUE);
		  (void) sprintf(out_val, "You have %s (%c)",tmp_str,locn+'a');
		  msg_print(out_val);
		  (void) delete_object(y, x);
		}
	    }
	  else
	    {
	      objdes(tmp_str, i_ptr, TRUE);
	      (void) sprintf(out_val, "You can't carry %s", tmp_str);
	      msg_print(out_val);
	    }
	}
    }
  /* OPPS!				   */
  else if (i == TV_INVIS_TRAP || i == TV_VIS_TRAP || i == TV_STORE_DOOR)
    hit_trap(y, x);
}


/* Deletes a monster entry from the level		-RAK-	*/
void delete_monster(j)
int j;
{
  register monster_type *m_ptr;

  m_ptr = &m_list[j];
  cave[m_ptr->fy][m_ptr->fx].cptr = 0;
  if (m_ptr->ml)
    lite_spot((int)m_ptr->fy, (int)m_ptr->fx);
  if (j != mfptr - 1)
    {
      m_ptr = &m_list[mfptr - 1];
      cave[m_ptr->fy][m_ptr->fx].cptr = j;
      m_list[j] = m_list[mfptr - 1];
    }
  mfptr--;
  m_list[mfptr] = blank_monster;
  if (mon_tot_mult > 0)
    mon_tot_mult--;
}

/* The following two procedures implement the same function as delete monster.
   However, they are used within creatures(), because deleting a monster
   while scanning the m_list causes two problems, monsters might get two
   turns, and m_ptr/monptr might be invalid after the delete_monster.
   Hence the delete is done in two steps. */
/* fix1_delete_monster does everything delete_monster does except delete
   the monster record and reduce mfptr, this is called in breathe, and
   a couple of places in creatures.c */
void fix1_delete_monster(j)
int j;
{
  register monster_type *m_ptr;

  m_ptr = &m_list[j];
  /* force the hp negative to ensure that the monster is dead, for example,
     if the monster was just eaten by another, it will still have positive
     hit points */
  m_ptr->hp = -1;
  cave[m_ptr->fy][m_ptr->fx].cptr = 0;
  if (m_ptr->ml)
    lite_spot((int)m_ptr->fy, (int)m_ptr->fx);
  if (mon_tot_mult > 0)
    mon_tot_mult--;
}

/* fix2_delete_monster does everything in delete_monster that wasn't done
   by fix1_monster_delete above, this is only called in creatures() */
void fix2_delete_monster(j)
int j;
{
  register monster_type *m_ptr;

  if (j != mfptr - 1)
    {
      m_ptr = &m_list[mfptr - 1];
      cave[m_ptr->fy][m_ptr->fx].cptr = j;
      m_list[j] = m_list[mfptr - 1];
    }
  m_list[mfptr - 1] = blank_monster;
  mfptr--;
}


/* Creates objects nearby the coordinates given		-RAK-	  */
static int summon_object(y, x, num, typ)
int y, x, num, typ;
{
  register int i, j, k;
  register cave_type *c_ptr;
  int real_typ, res;

  if ((typ == 1) || (typ == 5))
    real_typ = 1; /* typ == 1 -> objects */
  else
    real_typ = 256; /* typ == 2 -> gold */
  res = 0;
  do
    {
      i = 0;
      do
	{
	  j = y - 3 + randint(5);
	  k = x - 3 + randint(5);
	  if (in_bounds(j, k) && los(y, x, j, k))
	    {
	      c_ptr = &cave[j][k];
	      if (c_ptr->fval <= MAX_OPEN_SPACE && (c_ptr->tptr == 0))
		{
		  if ((typ == 3) || (typ == 7))
		    /* typ == 3 -> 50% objects, 50% gold */
		    {
		      if (randint(100) < 50)
			real_typ = 1;
		      else
			real_typ = 256;
		    }
		  if (real_typ == 1)
		    place_object(j, k, (typ >= 4));
		  else
		    place_gold(j, k);
		  lite_spot(j, k);
		  if (test_light(j, k))
		    res += real_typ;
		  i = 20;
		}
	    }
	  i++;
	}
      while (i <= 20);
      num--;
    }
  while (num != 0);
  return res;
}


/* Deletes object from given location			-RAK-	*/
int delete_object(y, x)
int y, x;
{
  register int delete;
  register cave_type *c_ptr;

  c_ptr = &cave[y][x];
  if (c_ptr->fval == BLOCKED_FLOOR)
    c_ptr->fval = CORR_FLOOR;
  pusht(c_ptr->tptr);
  c_ptr->tptr = 0;
  c_ptr->fm = FALSE;
  lite_spot(y, x);
  if (test_light(y, x))
    delete = TRUE;
  else
    delete = FALSE;
  return(delete);
}


/* Allocates objects upon a creatures death		-RAK-	*/
/* Oh well,  another creature bites the dust.  Reward the victor*/
/* based on flags set in the main creature record		 */
/* Returns a mask of bits from the given flags which indicates what the
   monster is seen to have dropped.  This may be added to monster memory. */
int32u monster_death(y, x, flags)
int y, x;
register int32u flags;
{
  register int i, number;
  int32u dump, res;
#if defined(ATARIST_MWC)
  int32u holder;	/* avoid a compiler bug */
#endif

#if !defined(ATARIST_MWC)
  if (flags & CM_CARRY_OBJ)
    i = 1;
  else
    i = 0;
  if (flags & CM_CARRY_GOLD)
    i += 2;
  if (flags & CM_SMALL_OBJ)
    i += 4;

  number = 0;
  if ((flags & CM_60_RANDOM) && (randint(100) < 60))
    number++;
  if ((flags & CM_90_RANDOM) && (randint(100) < 90))
    number++;
  if (flags & CM_1D2_OBJ)
    number += randint(2);
  if (flags & CM_2D2_OBJ)
    number += damroll(2, 2);
  if (flags & CM_4D2_OBJ)
    number += damroll(4, 2);
  if (number > 0)
    dump = summon_object(y, x, number, i);
  else
    dump = 0;
#else
  holder = CM_CARRY_OBJ;
  if (flags & holder)
    i = 1;
  else
    i = 0;
  holder = CM_CARRY_GOLD;
  if (flags & holder)
    i += 2;
  holder = CM_SMALL_OBJ;
  if (flags & holder)
    i += 4;

  number = 0;
  holder = CM_60_RANDOM;
  if ((flags & holder) && (randint(100) < 60))
    number++;
  holder = CM_90_RANDOM;
  if ((flags & holder) && (randint(100) < 90))
    number++;
  holder = CM_1D2_OBJ;
  if (flags & holder)
    number += randint(2);
  holder = CM_2D2_OBJ;
  if (flags & holder)
    number += damroll(2, 2);
  holder = CM_4D2_OBJ;
  if (flags & holder)
    number += damroll(4, 2);
  if (number > 0)
    dump = summon_object(y, x, number, i);
  else
    dump = 0;


#endif

#if defined(ATARIST_MWC)
  holder = CM_WIN;
  if (flags & holder)
#else
  if (flags & CM_WIN)
#endif
    if (!death) /* maybe the player died in mid-turn */
      {
	total_winner = TRUE;
	prt_winner();
	msg_print("*** CONGRATULATIONS *** You have won the game.");
	msg_print("You cannot save this game, but you may retire when ready.");
      }

  if (dump)
    {
      res = 0;
      if (dump & 255)
#ifdef ATARIST_MWC
	{
	  holder = CM_CARRY_OBJ;
	  if (i & 0x04)
	    holder = CM_CARRY_OBJ|CM_SMALL_OBJ;
	  res |= holder;
	}
#else
        {
	  res |= CM_CARRY_OBJ;
	  if (i & 0x04)
	    res |= CM_SMALL_OBJ;
	}
#endif
      if (dump >= 256)
#ifdef ATARIST_MWC
	{
	  holder = CM_CARRY_GOLD;
	  res |= holder;
	}
#else
	res |= CM_CARRY_GOLD;
#endif
      dump = (dump % 256) + (dump / 256);  /* number of items */
      res |= dump << CM_TR_SHIFT;
    }
  else
    res = 0;

  return res;
}


/* Decreases monsters hit points and deletes monster if needed.	*/
/* (Picking on my babies.)			       -RAK-   */
int mon_take_hit(monptr, dam)
int monptr, dam;
{
  register int32u i;
  int32 new_exp, new_exp_frac;
  register monster_type *m_ptr;
  register struct misc *p_ptr;
  register creature_type *c_ptr;
  int m_take_hit;
  int32u tmp;
#ifdef ATARIST_MWC
  int32u holder;
#endif

  m_ptr = &m_list[monptr];
  m_ptr->hp -= dam;
  m_ptr->csleep = 0;
  if (m_ptr->hp < 0)
    {
      i = monster_death((int)m_ptr->fy, (int)m_ptr->fx,
			c_list[m_ptr->mptr].cmove);
      if ((py.flags.blind < 1 && m_ptr->ml)
#ifdef ATARIST_MWC
	  || (c_list[m_ptr->mptr].cmove & (holder = CM_WIN)))
#else
	  || (c_list[m_ptr->mptr].cmove & CM_WIN))
#endif
	{
#ifdef ATARIST_MWC
	  holder = CM_TREASURE;
	  tmp = (c_recall[m_ptr->mptr].r_cmove & holder) >> CM_TR_SHIFT;
	  if (tmp > ((i & holder) >> CM_TR_SHIFT))
	    i = (i & ~holder) | (tmp << CM_TR_SHIFT);
	  c_recall[m_ptr->mptr].r_cmove =
	    (c_recall[m_ptr->mptr].r_cmove & ~holder) | i;
#else
	  tmp = (c_recall[m_ptr->mptr].r_cmove & CM_TREASURE) >> CM_TR_SHIFT;
	  if (tmp > ((i & CM_TREASURE) >> CM_TR_SHIFT))
	    i = (i & ~CM_TREASURE) | (tmp << CM_TR_SHIFT);
	  c_recall[m_ptr->mptr].r_cmove =
	    (c_recall[m_ptr->mptr].r_cmove & ~CM_TREASURE) | i;
#endif
	  if (c_recall[m_ptr->mptr].r_kills < MAX_SHORT)
	    c_recall[m_ptr->mptr].r_kills++;
	}
      c_ptr = &c_list[m_ptr->mptr];
      p_ptr = &py.misc;

      new_exp = ((long)c_ptr->mexp * c_ptr->level) / p_ptr->lev;
      new_exp_frac = ((((long)c_ptr->mexp * c_ptr->level) % p_ptr->lev)
	* 0x10000L / p_ptr->lev) + p_ptr->exp_frac;
      if (new_exp_frac >= 0x10000L)
	{
	  new_exp++;
	  p_ptr->exp_frac = new_exp_frac - 0x10000L;
	}
      else
	p_ptr->exp_frac = new_exp_frac;

      p_ptr->exp += new_exp;
      /* can't call prt_experience() here, as that would result in "new level"
	 message appearing before "monster dies" message */
      m_take_hit = m_ptr->mptr;
      /* in case this is called from within creatures(), this is a
	 horrible hack, the m_list/creatures() code needs to be
	 rewritten */
      if (hack_monptr < monptr)
	delete_monster(monptr);
      else
	fix1_delete_monster(monptr);
    }
  else
    m_take_hit = -1;
  return(m_take_hit);
}


/* Player attacks a (poor, defenseless) creature	-RAK-	*/
void py_attack(y, x)
int y, x;
{
  register int k, blows;
  int crptr, monptr, tot_tohit, base_tohit;
  vtype m_name, out_val;
  register inven_type *i_ptr;
  register struct misc *p_ptr;
#ifdef ATARIST_MWC
  int32u holder;
#endif

  crptr = cave[y][x].cptr;
  monptr = m_list[crptr].mptr;
  m_list[crptr].csleep = 0;
  i_ptr = &inventory[INVEN_WIELD];
  /* Does the player know what he's fighting?	   */
  if (!m_list[crptr].ml)
    (void) strcpy(m_name, "it");
  else
    (void) sprintf(m_name, "the %s", c_list[monptr].name);
  if (i_ptr->tval != TV_NOTHING)		/* Proper weapon */
    blows = attack_blows((int)i_ptr->weight, &tot_tohit);
  else					  /* Bare hands?   */
    {
      blows = 2;
      tot_tohit = -3;
    }
  if ((i_ptr->tval >= TV_SLING_AMMO) && (i_ptr->tval <= TV_SPIKE))
    /* Fix for arrows */
    blows = 1;
  p_ptr = &py.misc;
  tot_tohit += p_ptr->ptohit;
  /* if creature not lit, make it more difficult to hit */
  if (m_list[crptr].ml)
    base_tohit = p_ptr->bth;
  else
    base_tohit = (p_ptr->bth / 2) - (tot_tohit * (BTH_PLUS_ADJ-1))
      - (p_ptr->lev * class_level_adj[p_ptr->pclass][CLA_BTH] / 2);

  /* Loop for number of blows,	trying to hit the critter.	  */
  do
    {
      if (test_hit(base_tohit, (int)p_ptr->lev, tot_tohit,
		   (int)c_list[monptr].ac, CLA_BTH))
	{
	  (void) sprintf(out_val, "You hit %s.", m_name);
	  msg_print(out_val);
	  if (i_ptr->tval != TV_NOTHING)
	    {
	      k = pdamroll(i_ptr->damage);
	      k = tot_dam(i_ptr, k, monptr);
	      k = critical_blow((int)i_ptr->weight, tot_tohit, k, CLA_BTH);
	    }
	  else			      /* Bare hands!?  */
	    {
	      k = damroll(1, 1);
	      k = critical_blow(1, 0, k, CLA_BTH);
	    }
	  k += p_ptr->ptodam;
	  if (k < 0)  k = 0;

	  if (py.flags.confuse_monster)
	    {
	      py.flags.confuse_monster = FALSE;
	      msg_print("Your hands stop glowing.");
	      if ((c_list[monptr].cdefense & CD_NO_SLEEP)
		  || (randint(MAX_MONS_LEVEL) < c_list[monptr].level))
		(void) sprintf(out_val, "%s is unaffected.", m_name);
	      else
		{
		  (void) sprintf(out_val, "%s appears confused.", m_name);
		  if (m_list[crptr].confused)
		    m_list[crptr].confused += 3;
		  else
		    m_list[crptr].confused = 2 + randint(16);
		}
	      msg_print(out_val);
	      if (m_list[crptr].ml && randint(4) == 1)
		c_recall[monptr].r_cdefense |=
		  c_list[monptr].cdefense & CD_NO_SLEEP;
	    }

	  /* See if we done it in.				 */
	  if (mon_take_hit(crptr, k) >= 0)
	    {
	      (void) sprintf(out_val, "You have slain %s.", m_name);
	      msg_print(out_val);
	      prt_experience();
	      blows = 0;
	    }

	  if ((i_ptr->tval >= TV_SLING_AMMO)
	      && (i_ptr->tval <= TV_SPIKE)) /* Use missiles up*/
	    {
	      i_ptr->number--;
	      inven_weight -= i_ptr->weight;
#ifdef ATARIST_MWC
	      py.flags.status |= (holder = PY_STR_WGT);
#else
	      py.flags.status |= PY_STR_WGT;
#endif
	      if (i_ptr->number == 0)
		{
		  equip_ctr--;
		  py_bonuses(i_ptr, -1);
		  invcopy(i_ptr, OBJ_NOTHING);
		  calc_bonuses();
		}
	    }
	}
      else
	{
	  (void) sprintf(out_val, "You miss %s.", m_name);
	  msg_print(out_val);
	}
      blows--;
    }
  while (blows >= 1);
}


/* Moves player from one space to another.		-RAK-	*/
/* Note: This routine has been pre-declared; see that for argument*/
void move_char(dir, do_pickup)
int dir, do_pickup;
{
  int old_row, old_col, old_find_flag;
  int y, x;
  register int i, j;
  register cave_type *c_ptr, *d_ptr;

  if ((py.flags.confused > 0) &&    /* Confused?	     */
      (randint(4) > 1) &&	    /* 75% random movement   */
      (dir != 5))		    /* Never random if sitting*/
    {
      dir = randint(9);
      end_find();
    }
  y = char_row;
  x = char_col;
  if (mmove(dir, &y, &x))    /* Legal move?	      */
    {
      c_ptr = &cave[y][x];
      /* if there is no creature, or an unlit creature in the walls then... */
      /* disallow attacks against unlit creatures in walls because moving into
	 a wall is a free turn normally, hence don't give player free turns
	 attacking each wall in an attempt to locate the invisible creature,
	 instead force player to tunnel into walls which always takes a turn */
      if ((c_ptr->cptr < 2)
	  || (!m_list[c_ptr->cptr].ml && c_ptr->fval >= MIN_CLOSED_SPACE))
	{
	  if (c_ptr->fval <= MAX_OPEN_SPACE) /* Open floor spot	*/
	    {
	      /* Make final assignments of char co-ords */
	      old_row = char_row;
	      old_col = char_col;
	      char_row = y;
	      char_col = x;
	      /* Move character record (-1)	       */
	      move_rec(old_row, old_col, char_row, char_col);
	      /* Check for new panel		       */
	      if (get_panel(char_row, char_col, FALSE))
		prt_map();
	      /* Check to see if he should stop	       */
	      if (find_flag)
		area_affect(dir, char_row, char_col);
	      /* Check to see if he notices something  */
	      /* fos may be negative if have good rings of searching */
	      if ((py.misc.fos <= 1) || (randint(py.misc.fos) == 1) ||
		  (py.flags.status & PY_SEARCH))
		search(char_row, char_col, py.misc.srh);
	      /* A room of light should be lit.	     */
	      if (c_ptr->fval == LIGHT_FLOOR)
		{
		  if (!c_ptr->pl && !py.flags.blind)
		    light_room(char_row, char_col);
		}
	      /* In doorway of light-room?	       */
	      else if (c_ptr->lr && (py.flags.blind < 1))
		for (i = (char_row - 1); i <= (char_row + 1); i++)
		  for (j = (char_col - 1); j <= (char_col + 1); j++)
		    {
		      d_ptr = &cave[i][j];
		      if ((d_ptr->fval == LIGHT_FLOOR) && (!d_ptr->pl))
			light_room(i, j);
		    }
	      /* Move the light source		       */
	      move_light(old_row, old_col, char_row, char_col);
	      /* An object is beneath him.	     */
	      if (c_ptr->tptr != 0)
		{
		  carry(char_row, char_col, do_pickup);
		  /* if stepped on falling rock trap, and space contains
		     rubble, then step back into a clear area */
		  if (t_list[c_ptr->tptr].tval == TV_RUBBLE)
		    {
		      move_rec(char_row, char_col, old_row, old_col);
		      move_light(char_row, char_col, old_row, old_col);
		      char_row = old_row;
		      char_col = old_col;
		      /* check to see if we have stepped back onto another
			 trap, if so, set it off */
		      c_ptr = &cave[char_row][char_col];
		      if (c_ptr->tptr != 0)
			{
			  i = t_list[c_ptr->tptr].tval;
			  if (i == TV_INVIS_TRAP || i == TV_VIS_TRAP
			      || i == TV_STORE_DOOR)
			    hit_trap(char_row, char_col);
			}
		    }
		}
	    }
	  else	  /*Can't move onto floor space*/
	    {
	      if (!find_flag && (c_ptr->tptr != 0))
		{
		  if (t_list[c_ptr->tptr].tval == TV_RUBBLE)
		    msg_print("There is rubble blocking your way.");
		  else if (t_list[c_ptr->tptr].tval == TV_CLOSED_DOOR)
		    msg_print("There is a closed door blocking your way.");
		}
	      else
		end_find();
	      free_turn_flag = TRUE;
	    }
	}
      else	  /* Attacking a creature! */
	{
	  old_find_flag = find_flag;
	  end_find();
	  /* if player can see monster, and was in find mode, then nothing */
	  if (m_list[c_ptr->cptr].ml && old_find_flag)
	    {
	      /* did not do anything this turn */
	      free_turn_flag = TRUE;
	    }
	  else
	    {
	      if (py.flags.afraid < 1)		/* Coward?	*/
		py_attack(y, x);
	      else				/* Coward!	*/
		msg_print("You are too afraid!");
	    }
	}
    }
}


/* Chests have traps too.				-RAK-	*/
/* Note: Chest traps are based on the FLAGS value		 */
void chest_trap(y, x)
int y, x;
{
  register int i;
  int j, k;
  register inven_type *t_ptr;

  t_ptr = &t_list[cave[y][x].tptr];
  if (CH_LOSE_STR & t_ptr->flags)
    {
      msg_print("A small needle has pricked you!");
      if (!py.flags.sustain_str)
	{
	  (void) dec_stat(A_STR);
	  take_hit(damroll(1, 4), "a poison needle");
	  msg_print("You feel weakened!");
	}
      else
	msg_print("You are unaffected.");
    }
  if (CH_POISON & t_ptr->flags)
    {
      msg_print("A small needle has pricked you!");
      take_hit(damroll(1, 6), "a poison needle");
      py.flags.poisoned += 10 + randint(20);
    }
  if (CH_PARALYSED & t_ptr->flags)
    {
      msg_print("A puff of yellow gas surrounds you!");
      if (py.flags.free_act)
	msg_print("You are unaffected.");
      else
	{
	  msg_print("You choke and pass out.");
	  py.flags.paralysis = 10 + randint(20);
	}
    }
  if (CH_SUMMON & t_ptr->flags)
    {
      for (i = 0; i < 3; i++)
	{
	  j = y;
	  k = x;
	  (void) summon_monster(&j, &k, FALSE);
	}
    }
  if (CH_EXPLODE & t_ptr->flags)
    {
      msg_print("There is a sudden explosion!");
      (void) delete_object(y, x);
      take_hit(damroll(5, 8), "an exploding chest");
    }
}


/* Opens a closed door or closed chest.		-RAK-	*/
void openobject()
{
  int y, x, i, dir;
  int flag, no_object;
  register cave_type *c_ptr;
  register inven_type *t_ptr;
  register struct misc *p_ptr;
  register monster_type *m_ptr;
  vtype m_name, out_val;
#ifdef ATARIST_MWC
  int32u holder;
#endif

  y = char_row;
  x = char_col;
  if (get_dir(CNIL, &dir))
    {
      (void) mmove(dir, &y, &x);
      c_ptr = &cave[y][x];
      no_object = FALSE;
      if (c_ptr->cptr > 1 && c_ptr->tptr != 0 &&
	  (t_list[c_ptr->tptr].tval == TV_CLOSED_DOOR
	   || t_list[c_ptr->tptr].tval == TV_CHEST))
	{
	  m_ptr = &m_list[c_ptr->cptr];
	  if (m_ptr->ml)
	    (void) sprintf (m_name, "The %s", c_list[m_ptr->mptr].name);
	  else
	    (void) strcpy (m_name, "Something");
	  (void) sprintf(out_val, "%s is in your way!", m_name);
	  msg_print(out_val);
	}
      else if (c_ptr->tptr != 0)
	/* Closed door		 */
	if (t_list[c_ptr->tptr].tval == TV_CLOSED_DOOR)
	  {
	    t_ptr = &t_list[c_ptr->tptr];
	    if (t_ptr->p1 > 0)	 /* It's locked.	*/
	      {
		p_ptr = &py.misc;
		i = p_ptr->disarm + 2*todis_adj() + stat_adj(A_INT)
		  + (class_level_adj[p_ptr->pclass][CLA_DISARM]
		     * p_ptr->lev / 3);
		if (py.flags.confused > 0)
		  msg_print("You are too confused to pick the lock.");
		else if ((i-t_ptr->p1) > randint(100))
		  {
		    msg_print("You have picked the lock.");
		    py.misc.exp++;
		    prt_experience();
		    t_ptr->p1 = 0;
		  }
		else
		  count_msg_print("You failed to pick the lock.");
	      }
	    else if (t_ptr->p1 < 0)	 /* It's stuck	  */
	      msg_print("It appears to be stuck.");
	    if (t_ptr->p1 == 0)
	      {
		invcopy(&t_list[c_ptr->tptr], OBJ_OPEN_DOOR);
		c_ptr->fval = CORR_FLOOR;
		lite_spot(y, x);
		command_count = 0;
	      }
	  }
      /* Open a closed chest.		     */
	else if (t_list[c_ptr->tptr].tval == TV_CHEST)
	  {
	    p_ptr = &py.misc;
	    i = p_ptr->disarm + 2*todis_adj() + stat_adj(A_INT)
	      + (class_level_adj[p_ptr->pclass][CLA_DISARM] * p_ptr->lev / 3);
	    t_ptr = &t_list[c_ptr->tptr];
	    flag = FALSE;
	    if (CH_LOCKED & t_ptr->flags)
	      if (py.flags.confused > 0)
		msg_print("You are too confused to pick the lock.");
	      else if ((i-(int)t_ptr->level) > randint(100))
		{
		  msg_print("You have picked the lock.");
		  flag = TRUE;
		  py.misc.exp += t_ptr->level;
		  prt_experience();
		}
	      else
		count_msg_print("You failed to pick the lock.");
	    else
	      flag = TRUE;
	    if (flag)
	      {
		t_ptr->flags &= ~CH_LOCKED;
		t_ptr->name2 = SN_EMPTY;
		known2(t_ptr);
		t_ptr->cost = 0;
	      }
	    flag = FALSE;
	    /* Was chest still trapped?	 (Snicker)   */
	    if ((CH_LOCKED & t_ptr->flags) == 0)
	      {
		chest_trap(y, x);
		if (c_ptr->tptr != 0)
		  flag = TRUE;
	      }
	    /* Chest treasure is allocated as if a creature   */
	    /* had been killed.				   */
	    if (flag)
	      {
		/* clear the cursed chest/monster win flag, so that people
		   can not win by opening a cursed chest */
#ifdef ATARIST_MWC
		t_list[c_ptr->tptr].flags &= ~(holder = TR_CURSED);
#else
		t_list[c_ptr->tptr].flags &= ~TR_CURSED;
#endif
		(void) monster_death(y, x, t_list[c_ptr->tptr].flags);
		t_list[c_ptr->tptr].flags = 0;
	      }
	  }
	else
	  no_object = TRUE;
      else
	no_object = TRUE;

      if (no_object)
	{
	  msg_print("I do not see anything you can open there.");
	  free_turn_flag = TRUE;
	}
    }
}


/* Closes an open door.				-RAK-	*/
void closeobject()
{
  int y, x, dir, no_object;
  vtype out_val, m_name;
  register cave_type *c_ptr;
  register monster_type *m_ptr;

  y = char_row;
  x = char_col;
  if (get_dir(CNIL, &dir))
    {
      (void) mmove(dir, &y, &x);
      c_ptr = &cave[y][x];
      no_object = FALSE;
      if (c_ptr->tptr != 0)
	if (t_list[c_ptr->tptr].tval == TV_OPEN_DOOR)
	  if (c_ptr->cptr == 0)
	    if (t_list[c_ptr->tptr].p1 == 0)
	      {
		invcopy(&t_list[c_ptr->tptr], OBJ_CLOSED_DOOR);
		c_ptr->fval = BLOCKED_FLOOR;
		lite_spot(y, x);
	      }
	    else
	      msg_print("The door appears to be broken.");
	  else
	    {
	      m_ptr = &m_list[c_ptr->cptr];
	      if (m_ptr->ml)
		(void) sprintf (m_name, "The %s", c_list[m_ptr->mptr].name);
	      else
		(void) strcpy (m_name, "Something");
	      (void) sprintf(out_val, "%s is in your way!", m_name);
	      msg_print(out_val);
	    }
	else
	  no_object = TRUE;
      else
	no_object = TRUE;

      if (no_object)
	{
	  msg_print("I do not see anything you can close there.");
	  free_turn_flag = TRUE;
	}
    }
}


/* Tunneling through real wall: 10, 11, 12		-RAK-	*/
/* Used by TUNNEL and WALL_TO_MUD				 */
int twall(y, x, t1, t2)
int y, x, t1, t2;
{
  register int i, j;
  register cave_type *c_ptr;
  int res, found;

  res = FALSE;
  if (t1 > t2)
    {
      c_ptr = &cave[y][x];
      if (c_ptr->lr)
	{
	  /* should become a room space, check to see whether it should be
	     LIGHT_FLOOR or DARK_FLOOR */
	  found = FALSE;
	  for (i = y-1; i <= y+1; i++)
	    for (j = x-1; j <= x+1; j++)
	      if (cave[i][j].fval <= MAX_CAVE_ROOM)
		{
		  c_ptr->fval = cave[i][j].fval;
		  c_ptr->pl = cave[i][j].pl;
		  found = TRUE;
		  break;
		}
	  if (!found)
	    {
	      c_ptr->fval = CORR_FLOOR;
	      c_ptr->pl = FALSE;
	    }
	}
      else
	{
	  /* should become a corridor space */
	  c_ptr->fval  = CORR_FLOOR;
	  c_ptr->pl = FALSE;
	}
      c_ptr->fm = FALSE;
      if (panel_contains(y, x))
	if ((c_ptr->tl || c_ptr->pl) && c_ptr->tptr != 0)
	  msg_print("You have found something!");
      lite_spot(y, x);
      res = TRUE;
    }
  return(res);
}
