#include <stdio.h>

#include "constants.h"
#include "config.h"
#include "types.h"
#include "externs.h"

#ifdef USG
#include <string.h>
#else
#include <strings.h>
#endif

#ifdef sun   /* correct SUN stupidity in the stdio.h file */
char *sprintf();
#endif

/* global flags */
extern int moria_flag;        /* Next level when true  */
extern int search_flag;       /* Player is searching   */
extern int teleport_flag;     /* Handle teleport traps  */
extern int player_light;      /* Player carrying light */
extern int cave_flag;         /* used in get_panel */
extern int light_flag;        /* used in move_light */

char cur_char1();


/* Teleport the player to a new location 		-RAK-	*/
teleport(dis)
int dis;
{
  register int y, x;
  cave_type *c_ptr;
  register int i, j;

  do
    {
      y = randint(cur_height) - 1;
      x = randint(cur_width) - 1;
      while (distance(y, x, char_row, char_col) > dis)
	{
	  y += ((char_row-y)/2);
	  x += ((char_col-x)/2);
	}
    }
  while ((!cave[y][x].fopen) || (cave[y][x].cptr >= 2));
  move_rec(char_row, char_col, y, x);
  for (i = char_row-1; i <= char_row+1; i++)
    for (j = char_col-1; j <= char_col+1; j++)
      {
	c_ptr = &cave[i][j];
	c_ptr->tl = FALSE;
	if (!test_light(i, j))
	  unlite_spot(i, j);
      }
  if (test_light(char_row, char_col))
    lite_spot(char_row, char_col);
  char_row = y;
  char_col = x;
  move_char(5);
  creatures(FALSE);
  teleport_flag = FALSE;
}


/* Player hit a trap...	(Chuckle)			-RAK-	*/
hit_trap(y, x)
int *y, *x;
{
  int i, ty, tx;
  int dam;
  register cave_type *c_ptr;
  register struct misc *p_ptr;

  change_trap(*y, *x);
  lite_spot(char_row, char_col);
  if (find_flag)
    {
      find_flag = FALSE;
      move_light (char_row, char_col, char_row, char_col);
    }
  c_ptr = &cave[*y][*x];
  p_ptr = &py.misc;
  dam = damroll(t_list[c_ptr->tptr].damage);
  switch(t_list[c_ptr->tptr].subval)
    {
    case 1:  /* Open pit*/
      msg_print("You fell into a pit!");
      if (py.flags.ffall)
	msg_print("You gently float down.");
      else
	take_hit(dam, "an open pit.");
      break;
    case 2: /* Arrow trap*/
      if (test_hit(125, 0, 0, p_ptr->pac+p_ptr->ptoac))
	{
	  take_hit(dam, "an arrow trap.");
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
	take_hit(dam, "a covered pit.");
      place_trap(*y, *x, 2, 1);
      break;
    case 4: /* Trap door*/
      msg_print("You fell through a trap door!");
      /* make sure player reads message before new level drawn! */
      msg_print(" ");
      moria_flag = TRUE;
      dun_level++;
      if (py.flags.ffall)
	msg_print("You gently float down.");
      else
	take_hit(dam, "a trap door.");
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
      c_ptr->fm = FALSE;
      pusht((int)c_ptr->tptr);
      place_object(*y, *x);
      msg_print("Hmmm, there was something under this rock.");
      break;
    case 7:  /* STR Dart*/
      if (test_hit(125, 0, 0, p_ptr->pac+p_ptr->ptoac))
	{
	  if (!py.flags.sustain_str)
	    {
	      py.stats.cstr = de_statp(py.stats.cstr);
	      take_hit(dam, "a dart trap.");
	      print_stat |= 0x0001;
	      msg_print("A small dart weakens you!");
	      /* adjust misc stats */
	      py_bonuses(blank_treasure, 0);
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
      break;
    case 9: /* Rockfall*/
      take_hit(dam, "a falling rock.");
      pusht((int)c_ptr->tptr);
      place_rubble(*y, *x);
      msg_print("You are hit by a falling rock");
      break;
    case 10: /* Corrode gas*/
      corrode_gas("corrosion gas.");
      msg_print("A strange red gas surrounds you.");
      break;
    case 11: /* Summon mon*/
      c_ptr->fm = FALSE;        /* Rune disappears...    */
      pusht((int)c_ptr->tptr);
      c_ptr->tptr = 0;
      for (i = 0; i < (2+randint(3)); i++)
	{
	  ty = char_row;
	  tx = char_col;
	  (void) summon_monster(&ty, &tx, FALSE);
	}
      break;
    case 12: /* Fire trap*/
      fire_dam(dam, "a fire trap.");
      msg_print("You are enveloped in flames!");
      break;
    case 13: /* Acid trap*/
      acid_dam(dam, "an acid trap.");
      msg_print("You are splashed with acid!");
      break;
    case 14: /* Poison gas*/
      poison_gas(dam, "a poison gas trap.");
      msg_print("A pungent green gas surrounds you!");
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
      if (test_hit(125, 0, 0, p_ptr->pac+p_ptr->ptoac))
	{
	  take_hit(dam, "a dart trap.");
	  msg_print("A small dart hits you!");
	  py.flags.slow += randint(20) + 10;
	}
      else
	msg_print("A small dart barely misses you.");
      break;
    case 18: /* CON Dart*/
      if (test_hit(125, 0, 0, p_ptr->pac+p_ptr->ptoac))
	{
	  if (!py.flags.sustain_con)
	    {
	      py.stats.ccon = de_statp(py.stats.ccon);
	      take_hit(dam, "a dart trap.");
	      print_stat |= 0x0004;
	      msg_print("A small dart weakens you!");
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

      /* Town level traps are special,  the stores...	*/
    case 101: /* General    */
      enter_store(0);
      break;
    case 102: /* Armory     */
      enter_store(1);
      break;
    case 103: /* Weaponsmith*/
      enter_store(2);
      break;
    case 104: /* Temple     */
      enter_store(3);
      break;
    case 105: /* Alchemy    */
      enter_store(4);
      break;
    case 106: /* Magic-User */
      enter_store(5);
      break;

    default:
      msg_print("Unknown trap value");
      break;
    }
}


/* Return spell number and failure chance		-RAK-	*/
/* returns -1 if no spells in book
   returns 1 if choose a spell in book to cast
   returns 0 if don't choose a spell, i.e. exit with an escape */
int cast_spell(prompt, item_val, sn, sc, redraw)
char *prompt;
int item_val;
int *sn, *sc;
int *redraw;
{
  unsigned int j;
  register int i, k;
  spl_type spell;
  int cast;
  register spell_type *s_ptr;

  cast = -1;
  i = 0;
  j = inventory[item_val].flags;
  do
    {
      k = bit_pos(&j);
      if (k >= 0)
	{
	  s_ptr = &magic_spell[py.misc.pclass][k];
	  if (s_ptr->slevel <= py.misc.lev)
	    if (s_ptr->learned)
	      {
		spell[i].splnum = k;
		i++;
	      }
	}
    }
  while (j != 0);
  if (i > 0)
    {
      cast = get_spell(spell, i, sn, sc, prompt, redraw);
      if (cast && magic_spell[py.misc.pclass][*sn].smana > py.misc.cmana)
	cast = confirm();
    }
  if (*redraw)
    draw_cave();
  return(cast);
}


/* Finds range of item in inventory list 		-RAK-	*/
int find_range(item1, item2, j, k)
int item1, item2;
register int *j, *k;
{
  register int i;
  int flag;

  i = 0;
  *j = -1;
  *k = -1;
  flag = FALSE;
  while(i < inven_ctr)
    {
      if (((inventory[i].tval == item1)  || (inventory[i].tval == item2))
	  && (!flag))
	{
	  flag = TRUE;
	  *j = i;
	}
      if (((inventory[i].tval != item1) && (inventory[i].tval != item2))
	  && (flag) && (*k == -1))
	*k = i - 1;
      i++;
    }
  if ((flag) && (*k == -1))
    *k = inven_ctr - 1;
  return(flag);
}


/* Examine a Book					-RAK-	*/
examine_book()
{
  unsigned int j;
  int i, k, item_val;
  int redraw, flag;
  char dummy;
  vtype out_val;
  register treasure_type *i_ptr;
  register spell_type *s_ptr;

  redraw = FALSE;
  if (!find_range(90, 91, &i, &k))
    msg_print("You are not carrying any books.");
  else if (get_item(&item_val, "Which Book?", &redraw, i, k))
    {
      flag = TRUE;
      i_ptr = &inventory[item_val];
      if (class[py.misc.pclass].mspell)
	{
	  if (i_ptr->tval != 90)
	    {
	      msg_print("You do not understand the language.");
	      flag = FALSE;
	    }
	}
      else if (class[py.misc.pclass].pspell)
	{
	  if (i_ptr->tval != 91)
	    {
	      msg_print("You do not understand the language.");
	      flag = FALSE;
	    }
	}
      else
	{
	  msg_print("You do not understand the language.");
	  flag = FALSE;
	}
      if (flag)
	{
	  redraw = TRUE;
	  i = 0;
	  j = inventory[item_val].flags;
	  clear_screen(0, 0);
	  (void) strcpy(out_val,
		 "   Name                         Level  Mana   Known");
	  prt(out_val, 0, 0);
	  do
	    {
	      k = bit_pos(&j);
	      if (k >= 0)
		s_ptr = &magic_spell[py.misc.pclass][k];
		  {
		    if (s_ptr->slevel < 99)
		      {
			(void) sprintf(out_val, "%c) %s%d     %d   %d", 97+i,
				pad(s_ptr->sname, " ", 30), s_ptr->slevel,
				s_ptr->smana, s_ptr->learned);
			prt(out_val, i+1, 0);
		      }
		    else
		      prt("", i+1, 0);  /* clear the line */
		    i++;
		  }
	    }
	  while (j != 0);
	  prt("[Press any key to continue]", 23, 19);
	  inkey(&dummy);
	}
    }
  if (redraw)  draw_cave();
}


/* Player is on an object.  Many things can happen BASED -RAK-	*/
/* on the TVAL of the object.  Traps are set off, money and most */
/* objects are picked up.  Some objects, such as open doors, just*/
/* sit there...                                                  */
carry(y, x)
int y, x;
{
  int item_val;
  vtype out_val, tmp_str;
  register cave_type *c_ptr;
  register treasure_type *i_ptr;

  if (find_flag)
    {
      find_flag = FALSE;
      move_light (char_row, char_col, char_row, char_col);
    }
  c_ptr = &cave[y][x];
  inventory[INVEN_MAX] = t_list[c_ptr->tptr];
  /* There's GOLD in them thar hills!      */
  if (t_list[c_ptr->tptr].tval == 100)
    {
      pusht((int)c_ptr->tptr);
      c_ptr->tptr = 0;
      i_ptr = &inventory[INVEN_MAX];
      py.misc.au += i_ptr->cost;
      (void) sprintf(out_val, "You have found %d gold pieces worth of %s.",
	      i_ptr->cost, i_ptr->name);
      prt_gold();
      msg_print(out_val);
    }
  /* OPPS!                                 */
  else if ((t_list[c_ptr->tptr].tval == 101) ||
	   (t_list[c_ptr->tptr].tval == 102) ||
	   (t_list[c_ptr->tptr].tval == 109) ||
	   (t_list[c_ptr->tptr].tval == 110))
    hit_trap(&y, &x);
  /* Attempt to pick up an object.         */
  else if (t_list[c_ptr->tptr].tval < 100)
    {
      if (inven_check_weight())     /* Weight limit check    */
	if (inven_check_num())      /* Too many objects?     */
	  {                       /* Okay,  pick it up      */
	    pusht((int)c_ptr->tptr);
	    c_ptr->tptr = 0;
	    inven_carry(&item_val);
	    objdes(tmp_str, item_val, TRUE);
	    (void) sprintf(out_val, "You have %s (%c%c", tmp_str, item_val+97,
		    cur_char1(item_val));
	    msg_print(out_val);
	  }
	else
	  msg_print("You can't carry that many items.");
      else
	msg_print("You can't carry that much weight.");
    }
}


/* Drop an object being carried				-RAK-	*/
/* Note: Only one object per floor spot...                       */
drop()
{
  int com_val;
  int redraw;
  vtype out_val, tmp_str;
  register cave_type *c_ptr;

  redraw = FALSE;
  reset_flag = TRUE;
  if (inven_ctr > 0)
    {
      c_ptr = &cave[char_row][char_col];
      if (c_ptr->tptr != 0)
	msg_print("There is something there already.");
      else if (get_item(&com_val, "Which one? ", &redraw, 0, inven_ctr-1))
	{
	  reset_flag = FALSE;
	  if (redraw)  draw_cave();
	  inven_drop(com_val, char_row, char_col);
	  objdes(tmp_str, INVEN_MAX, TRUE);
	  (void) sprintf(out_val, "Dropped %s", tmp_str);
	  msg_print(out_val);
	}
      else if (redraw)
	draw_cave();
    }
  else
    msg_print("You are not carrying anything.");
}


/* Deletes a monster entry from the level		-RAK-	*/
delete_monster(j)
int j;
{
  register int i, k;
  register cave_type *c_ptr;
  register monster_type *m_ptr;

  i = muptr;
  k = m_list[j].nptr;
  if (i == j)
    muptr = k;
  else
    {
      while (m_list[i].nptr != j)
	i = m_list[i].nptr;
      m_list[i].nptr = k;
    }
  m_ptr = &m_list[j];
  cave[m_ptr->fy][m_ptr->fx].cptr = 0;
  if (m_ptr->ml)
    {
      c_ptr = &cave[m_ptr->fy][m_ptr->fx];
      if ((c_ptr->pl) || (c_ptr->tl))
	lite_spot((int)m_ptr->fy, (int)m_ptr->fx);
      else
	unlite_spot((int)m_ptr->fy, (int)m_ptr->fx);
    }
  pushm(j);
  if (mon_tot_mult > 0)
    mon_tot_mult--;
}


/* Makes sure new creature gets lit up			-RAK-	*/
check_mon_lite(y, x)
register int y, x;
{
  register cave_type *c_ptr;

  c_ptr = &cave[y][x];
  if (c_ptr->cptr > 1)
    if (!m_list[c_ptr->cptr].ml)
      if ((c_ptr->tl) || (c_ptr->pl))
	if (los(char_row, char_col, y, x))
	  {
	    m_list[c_ptr->cptr].ml = TRUE;
	    lite_spot(y, x);
	  }
}


/* Places creature adjacent to given location		-RAK-	*/
/* Rats and Flys are fun!                                        */
multiply_monster(y, x, z, slp)
int y, x, z;
int slp;
{
  register int i, j, k;
  register cave_type *c_ptr;

  i = 0;
  do
    {
      j = y - 2 + randint(3);
      k = x - 2 + randint(3);
      if (in_bounds(j, k))
	{
	  c_ptr = &cave[j][k];
	  if ((c_ptr->fval >= 1) && (c_ptr->fval <= 7 ) && (c_ptr->fval != 3))
	    if ((c_ptr->tptr == 0) && (c_ptr->cptr != 1))
	      {
		if (c_ptr->cptr > 1)     /* Creature there already?       */
		  {
		    /* Some critters are cannibalistic!       */
		    if (c_list[z].cmove & 0x00080000)
		      {
			delete_monster((int)c_ptr->cptr);
			place_monster(j, k, z, slp);
			check_mon_lite(j, k);
			mon_tot_mult++;
		      }
		  }
		else
		  /* All clear,  place a monster    */
		  {
		    place_monster(j, k, z, slp);
		    check_mon_lite(j, k);
		    mon_tot_mult++;
		  }
		i = 18;
	      }
	}
      i++;
    }
  while (i <= 18);
}


/* Creates objects nearby the coordinates given		-RAK-	  */
/* BUG: Because of the range, objects can actually be placed into */
/*      areas closed off to the player, this is rarely noticeable, */
/*      and never a problem to the game.                          */
summon_object(y, x, num, typ)
int y, x, num, typ;
{
  register int i, j, k;
  register cave_type *c_ptr;

  do
    {
      i = 0;
      do
	{
	  j = y - 3 + randint(5);
	  k = x - 3 + randint(5);
	  if (in_bounds(j, k))
	    {
	      c_ptr = &cave[j][k];
	      if ((c_ptr->fval >= 1) && (c_ptr->fval <= 7) &&
		  (c_ptr->fval != 3))
		if (c_ptr->tptr == 0)
		  {
		    switch(typ)                 /* Select type of object */
		      {
		      case 1:  place_object(j, k); break;
		      case 2:  place_gold(j, k); break;
		      case 3:
			if (randint(100) < 50)
			  place_object(j, k);
			else
			  place_gold(j, k);
			break;
		      default:
			break;
		      }
		    if (test_light(j, k))
		      lite_spot(j, k);
		    i = 10;
		  }
	    }
	  i++;
	}
      while (i <= 10);
      num--;
    }
  while (num != 0);
}


/* Deletes object from given location			-RAK-	*/
int delete_object(y, x)
int y, x;
{
  register int delete;
  register cave_type *c_ptr;

  delete = FALSE;
  c_ptr = &cave[y][x];
  if ((t_list[c_ptr->tptr].tval == 109) || (t_list[c_ptr->tptr].tval == 105)
      || (t_list[c_ptr->tptr].tval == 104))
    c_ptr->fval = corr_floor2.ftval;
  c_ptr->fopen = TRUE;
  pusht((int)c_ptr->tptr);
  c_ptr->tptr = 0;
  c_ptr->fm = FALSE;
  if (test_light(y, x))
    {
      lite_spot(y, x);
      delete = TRUE;
    }
  else
    unlite_spot(y, x);
  return(delete);
}


/* Allocates objects upon a creatures death		-RAK-	*/
/* Oh well,  another creature bites the dust...  Reward the victor*/
/* based on flags set in the main creature record                */
monster_death(y, x, flags)
int y, x;
register unsigned int flags;
{
  register int i;

  if (flags & 0x01000000)
    i = 1;
  else
    i = 0;
  if (flags & 0x02000000)
    i += 2;
  if (flags & 0x04000000)
    if (randint(100) < 60)
      summon_object(y, x, 1, i);
  if (flags & 0x08000000)
    if (randint(100) < 90)
      summon_object(y, x, 1, i);
  if (flags & 0x10000000)
    summon_object(y, x, randint(2), i);
  if (flags & 0x20000000)
    summon_object(y, x, damroll("2d2"), i);
  if (flags & 0x40000000)
    summon_object(y, x, damroll("4d3"), i);
  if (flags & 0x80000000)
    {
      total_winner = TRUE;
      prt_winner();
      msg_print("*** CONGRATULATIONS *** You have won the game...");
      msg_print("Use <CONTROL>-K when you are ready to quit.");
    }
}


/* Decreases monsters hit points and deletes monster if needed.	*/
/* (Picking on my babies...)                             -RAK-   */
int mon_take_hit(monptr, dam)
int monptr, dam;
{
  register int i;
  double acc_tmp;
  register monster_type *m_ptr;
  register struct misc *p_ptr;
  register creature_type *c_ptr;
  int m_take_hit;

  m_ptr = &m_list[monptr];
  m_ptr->hp -= dam;
  m_ptr->csleep = 0;
  if (m_ptr->hp < 0)
    {
      monster_death((int)m_ptr->fy, (int)m_ptr->fx,c_list[m_ptr->mptr].cmove);
      c_ptr = &c_list[m_ptr->mptr];
      p_ptr = &py.misc;
      acc_tmp = c_ptr->mexp*((c_ptr->level+0.1)/p_ptr->lev);
      i = (int)(acc_tmp);
      acc_exp += (acc_tmp - i);
      if (acc_exp > 1)
	{
	  i++;
	  acc_exp -= 1.0;
	}
      p_ptr->exp += i;
      if (i > 0)  prt_experience();
      m_take_hit = m_ptr->mptr;
      delete_monster(monptr);
    }
  else
    m_take_hit = -1;
  return(m_take_hit);
}


/* Special damage due to magical abilities of object	-RAK-	*/
int tot_dam(item, tdam, monster)
treasure_type item;
register int tdam;
creature_type monster;
{
  register treasure_type *i_ptr;
  register creature_type *m_ptr;

  i_ptr = &item;
  if (((i_ptr->tval >= 10) && (i_ptr->tval <= 12)) ||
      ((i_ptr->tval >= 20) && (i_ptr->tval <= 23)) ||
      (i_ptr->tval == 77))
    {
      m_ptr = &monster;
      /* Slay Dragon  */
      if ((m_ptr->cdefense & 0x0001) && (i_ptr->flags & 0x00002000))
	tdam = tdam * 4;
      /* Slay Undead  */
      else if ((m_ptr->cdefense & 0x0008) && (i_ptr->flags & 0x00010000))
	tdam = tdam * 3;
      /* Slay Monster  */
      else if ((m_ptr->cdefense & 0x0002) && (i_ptr->flags & 0x00004000))
	tdam = tdam * 2;
      /* Slay Evil     */
      else if ((m_ptr->cdefense & 0x0004) && (i_ptr->flags & 0x00008000))
	tdam = tdam * 2;
      /* Frost         */
      else if ((m_ptr->cdefense & 0x0010) && (i_ptr->flags & 0x00020000))
	tdam = tdam * 1.5;
      /* Fire         */
      else if ((m_ptr->cdefense & 0x0020) && (i_ptr->flags & 0x00040000))
	tdam = tdam * 1.5;
    }
  return(tdam);
}


/* Player attacks a (poor, defenseless) creature 	-RAK-	*/
int py_attack(y, x)
int y, x;
{
  register int k, blows;
  int i, j, tot_tohit, attack;
  vtype m_name, out_val;
  register treasure_type *i_ptr;
  register struct misc *p_ptr;

  attack = FALSE;
  i = cave[y][x].cptr;
  j = m_list[i].mptr;
  m_list[i].csleep = 0;
  /* Does the player know what he's fighting?      */
  if ((0x10000 & c_list[j].cmove) && (!py.flags.see_inv))
    (void) strcpy(m_name, "it");
  else if (py.flags.blind > 0)
    (void) strcpy(m_name, "it");
  else if (!m_list[i].ml)
    (void) strcpy(m_name, "it");
  else
    (void) sprintf(m_name, "the %s", c_list[j].name);
  if (inventory[INVEN_WIELD].tval != 0)         /* Proper weapon */
    blows = attack_blows((int)inventory[INVEN_WIELD].weight, &tot_tohit);
  else                                    /* Bare hands?   */
    {
      blows = 2;
      tot_tohit = -3;
    }
  if ((inventory[INVEN_WIELD].tval >= 10) && (inventory[INVEN_WIELD].tval <= 12))
    /* Fix for arrows */
    blows = 1;
  tot_tohit += py.misc.ptohit;
  /* Loop for number of blows,  trying to hit the critter...        */
  p_ptr = &py.misc;
  do
    {
      if (test_hit(p_ptr->bth, (int)p_ptr->lev, tot_tohit, (int)c_list[j].ac))
	{
	  (void) sprintf(out_val, "You hit %s.", m_name);
	  msg_print(out_val);
	  i_ptr = &inventory[INVEN_WIELD];
	  if (i_ptr->tval != 0)           /* Weapon?       */
	    {
	      k = damroll(i_ptr->damage);
	      k = tot_dam(inventory[INVEN_WIELD], k, c_list[j]);
	      k = critical_blow((int)i_ptr->weight, tot_tohit, k);
	    }
	  else                        /* Bare hands!?  */
	    {
	      k = damroll(bare_hands);
	      k = critical_blow(1, 0, k);
	    }
	  k += p_ptr->ptodam;
	  if (k < 0)  k = 0;
	  /* See if we done it in...                               */
	  if (mon_take_hit(i, k) >= 0)
	    {
	      (void) sprintf(out_val, "You have slain %s.", m_name);
	      msg_print(out_val);
	      blows = 0;
	      attack = FALSE;
	    }
	  else
	    attack = TRUE;  /* If creature hit,  but alive...*/
	  i_ptr = &inventory[INVEN_WIELD];
	  if ((i_ptr->tval >= 10) && (i_ptr->tval <= 12)) /* Use missiles up*/
	    {
	      i_ptr->number--;
	      if (i_ptr->number == 0)
		{
		  inven_weight = inven_weight - i_ptr->weight;
		  equip_ctr--;
		  inventory[INVEN_MAX] = inventory[INVEN_WIELD];
		  inventory[INVEN_WIELD] = blank_treasure;
		  py_bonuses(inventory[INVEN_MAX], -1);
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
  return(attack);
}


/* Moves player from one space to another...		-RAK-	*/
/* Note: This routine has been pre-declared; see that for argument*/
move_char(dir)
int dir;
{
  int test_row, test_col;
  register int i, j;
  register cave_type *c_ptr, *d_ptr;

  test_row = char_row;
  test_col = char_col;
  if (py.flags.confused > 0)          /* Confused?             */
    if (randint(4) > 1)               /* 75% random movement   */
      if (dir != 5)                   /* Never random if sitting*/
	{
	  dir = randint(9);
	  find_flag = FALSE; /* no need for move_light () */
	}
  if (move(dir, &test_row, &test_col))    /* Legal move?           */
    {
      c_ptr = &cave[test_row][test_col];
      if (c_ptr->cptr < 2)                   /* No creature?          */
	{
	  if (c_ptr->fopen)                  /* Open floor spot       */
	    {
	      /* Move character record (-1)            */
	      move_rec(char_row, char_col, test_row, test_col);
	      /* Check for new panel                   */
	      if (get_panel(test_row, test_col))
		prt_map();
	      /* Check to see if he should stop        */
	      if (find_flag)
		area_affect(dir, test_row, test_col);
	      /* Check to see if he notices something  */
	      if (py.flags.blind < 1)
		/* fos may be negative if have good rings of searching */
		if ((py.misc.fos <= 1) || (randint(py.misc.fos) == 1) ||
		    (search_flag))
		  search(test_row, test_col, py.misc.srh);
	      /* An object is beneath him...           */
	      if (c_ptr->tptr != 0)
		carry(test_row, test_col);
	      /* Move the light source                 */
	      move_light(char_row, char_col, test_row, test_col);
	      /* A room of light should be lit...      */
	      if (c_ptr->fval == lopen_floor.ftval)
		{
		  if (py.flags.blind < 1)
		    if (!c_ptr->pl)
		      light_room(test_row, test_col);
		}
	      /* In doorway of light-room?             */
	      else if ((c_ptr->fval == 5) || (c_ptr->fval == 6))
		if (py.flags.blind < 1)
		  {
		    for (i = (test_row - 1); i <= (test_row + 1); i++)
		      for (j = (test_col - 1); j <= (test_col + 1); j++)
			if (in_bounds(i, j))
			  {
			    d_ptr = &cave[i][j];
			    if (d_ptr->fval == lopen_floor.ftval)
			      if (!d_ptr->pl)
				light_room(i, j);
			  }
		  }
	      /* Make final assignments of char co-ords*/
	      char_row = test_row;
	      char_col = test_col;
	    }
	  else    /*Can't move onto floor space*/
	    /* Try a new direction if in find mode   */
	    if (!pick_dir(dir))
	      {
		if (find_flag)
		  {
		    find_flag = FALSE;
		    move_light (char_row, char_col, char_row, char_col);
		  }
		else if (c_ptr->tptr != 0)
		  {
		    reset_flag = TRUE;
		    if (t_list[c_ptr->tptr].tval == 103)
		      msg_print("There is rubble blocking your way.");
		    else if (t_list[c_ptr->tptr].tval == 105)
		      msg_print("There is a closed door blocking your way.");
		  }
		else
		  reset_flag = TRUE;
	      }
	}
      else        /* Attacking a creature! */
	{
	  if (find_flag)
	    {
	      find_flag = FALSE;
	      move_light(char_row, char_col, char_row, char_col);
	    }
	  if (py.flags.afraid < 1)    /* Coward?       */
	    (void) py_attack(test_row, test_col);
	  else                            /* Coward!       */
	    msg_print("You are too afraid!");
	}
    }
}


/* Chests have traps too...				-RAK-	*/
/* Note: Chest traps are based on the FLAGS value                */
chest_trap(y, x)
int y, x;
{
  register int i;
  int j, k;
  register treasure_type *t_ptr;

  t_ptr = &t_list[cave[y][x].tptr];
  if (0x00000010 & t_ptr->flags)
    {
      msg_print("A small needle has pricked you!");
      if (!py.flags.sustain_str)
	{
	  py.stats.cstr = de_statp(py.stats.cstr);
	  take_hit(damroll("1d4"), "a poison needle.");
	  print_stat |= 0x0001;
	  msg_print("You feel weakened!");
	  /* adjust misc stats */
	  py_bonuses(blank_treasure, 0);
	}
      else
	msg_print("You are unaffected.");
    }
  if (0x00000020 & t_ptr->flags)
    {
      msg_print("A small needle has pricked you!");
      take_hit(damroll("1d6"), "a poison needle.");
      py.flags.poisoned += 10 + randint(20);
    }
  if (0x00000040 & t_ptr->flags)
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
  if (0x00000080 & t_ptr->flags)
    {
      msg_print("There is a sudden explosion!");
      (void) delete_object(y, x);
      take_hit(damroll("5d8"), "an exploding chest.");
    }
  if (0x00000100 & t_ptr->flags)
    {
      for (i = 0; i < 3; i++)
	{
	  j = y;
	  k = x;
	  (void) summon_monster(&j, &k, FALSE);
	}
    }
}


/* Opens a closed door or closed chest...		-RAK-	*/
openobject()
{
  int y, x, tmp;
  int flag;
  char *tmp_str;
  register cave_type *c_ptr;
  register treasure_type *t_ptr;
  register struct misc *p_ptr;

  y = char_row;
  x = char_col;
  if (get_dir("Which direction?", &tmp, &tmp, &y, &x))
    {
      c_ptr = &cave[y][x];
      if (c_ptr->tptr != 0)
	/* Closed door           */
	if (t_list[c_ptr->tptr].tval == 105)
	  {
	    t_ptr = &t_list[c_ptr->tptr];
	    if (t_ptr->p1 > 0)   /* It's locked...        */
	      {
		p_ptr = &py.misc;
		tmp = p_ptr->disarm + p_ptr->lev + 2*todis_adj() + int_adj();
		if (py.flags.confused > 0)
		  msg_print("You are too confused to pick the lock.");
		else if ((tmp-t_ptr->p1) > randint(100))
		  {
		    msg_print("You have picked the lock.");
		    py.misc.exp++;
		    prt_experience();
		    t_ptr->p1 = 0;
		  }
		else
		  msg_print("You failed to pick the lock.");
	      }
	    else if (t_ptr->p1 < 0)      /* It's stuck    */
	      msg_print("It appears to be stuck.");
	    if (t_ptr->p1 == 0)
	      {
		t_list[c_ptr->tptr] = door_list[0];
		c_ptr->fopen = TRUE;
		lite_spot(y, x);
	      }
	  }
      /* Open a closed chest...                */
	else if (t_list[c_ptr->tptr].tval == 2)
	  {
	    p_ptr = &py.misc;
	    tmp = p_ptr->disarm + p_ptr->lev + 2*todis_adj() + int_adj();
	    t_ptr = &t_list[c_ptr->tptr];
	    flag = FALSE;
	    if (0x00000001 & t_ptr->flags)
	      if (py.flags.confused > 0)
		msg_print("You are too confused to pick the lock.");
	      else if ((tmp-(2*t_ptr->level)) > randint(100))
		{
		  msg_print("You have picked the lock.");
		  flag = TRUE;
		  py.misc.exp += t_ptr->level;
		  prt_experience();
		}
	      else
		msg_print("You failed to pick the lock.");
	    else
	      flag = TRUE;
	    if (flag)
	      {
		t_ptr->flags &= 0xFFFFFFFE;
		tmp_str = index(t_ptr->name, '(');
		if (tmp_str != 0)
		  tmp_str[0] = '\0';
		(void) strcat(t_ptr->name, " (Empty)");
		known2(t_ptr->name);
		t_ptr->cost = 0;
	      }
	    flag = FALSE;
	    /* Was chest still trapped?  (Snicker)   */
	    if ((0x00000001 & t_ptr->flags) == 0)
	      {
		chest_trap(y, x);
		if (c_ptr->tptr != 0)
		  flag = TRUE;
	      }
	    /* Chest treasure is allocated as if a creature   */
	    /* had been killed...                            */
	    if (flag)
	      {
		monster_death(y, x, t_list[c_ptr->tptr].flags);
		t_list[c_ptr->tptr].flags = 0;
	      }
	  }
	else
	  msg_print("I do not see anything you can open there.");
      else
	msg_print("I do not see anything you can open there.");
	}
}


/* Closes an open door...				-RAK-	*/
closeobject()
{
  int y, x, tmp;
  vtype out_val;
  register cave_type *c_ptr;

  y = char_row;
  x = char_col;
  if (get_dir("Which direction?", &tmp, &tmp, &y, &x))
    {
      c_ptr = &cave[y][x];
      if (c_ptr->tptr != 0)
	if (t_list[c_ptr->tptr].tval == 104)
	  if (c_ptr->cptr == 0)
	    if (t_list[c_ptr->tptr].p1 == 0)
	      {
		t_list[c_ptr->tptr] = door_list[1];
		c_ptr->fopen = FALSE;
		lite_spot(y, x);
	      }
	    else
	      msg_print("The door appears to be broken.");
	  else
	    {
	      (void) sprintf(out_val, "The %s is in your way!",
		      c_list[m_list[c_ptr->cptr].mptr].name);
	      msg_print(out_val);
	    }
	else
	  msg_print("I do not see anything you can close there.");
      else
	msg_print("I do not see anything you can close there.");
    }
}


/* Go up one level					-RAK-	*/
go_up()
{
  register cave_type *c_ptr;

  c_ptr = &cave[char_row][char_col];
  if (c_ptr->tptr != 0)
    if (t_list[c_ptr->tptr].tval == 107)
      {
	dun_level--;
	moria_flag = TRUE;
	msg_print("You enter a maze of up staircases.");
	msg_print("You pass through a one-way door.");
      }
    else
      msg_print("I see no up staircase here.");
  else
    msg_print("I see no up staircase here.");
}


/* Go down one level					-RAK-	*/
go_down()
{
  register cave_type *c_ptr;

  c_ptr = &cave[char_row][char_col];
  if (c_ptr->tptr != 0)
    if (t_list[c_ptr->tptr].tval == 108)
      {
	dun_level++;
	moria_flag = TRUE;
	msg_print("You enter a maze of down staircases.");
	msg_print("You pass through a one-way door.");
      }
    else
      msg_print("I see no down staircase here.");
  else
    msg_print("I see no down staircase here.");
}


/* Tunneling through real wall: 10, 11, 12 		-RAK-	*/
/* Used by TUNNEL and WALL_TO_MUD                                */
int twall(y, x, t1, t2)
int y, x, t1, t2;
{
  int res;
  register cave_type *c_ptr;

  res = FALSE;
  c_ptr = &cave[y][x];
  if (t1 > t2)
    {
      if (next_to4(y, x, 1, 2, -1) > 0)
	{
	  c_ptr->fval  = corr_floor2.ftval;
	  c_ptr->fopen = corr_floor2.ftopen;
	}
      else
	{
	  c_ptr->fval  = corr_floor1.ftval;
	  c_ptr->fopen = corr_floor1.ftopen;
	}
      if (test_light(y, x))
	if (panel_contains(y, x))
	  {
	    if (c_ptr->tptr != 0)
	      msg_print("You have found something!");
	    lite_spot(y, x);
	  }
      c_ptr->fm = FALSE;
      c_ptr->pl = FALSE;
      res = TRUE;
    }
  return(res);
}


/* Tunnels through rubble and walls			-RAK-	*/
/* Must take into account: secret doors,  special tools           */
tunnel(y, x)
int y, x;
{
  register int i, tabil;
  register cave_type *c_ptr;
  register treasure_type *i_ptr;

  c_ptr = &cave[y][x];
  /* Compute the digging ability of player; based on       */
  /* strength, and type of tool used                       */
  tabil = py.stats.cstr;
  if (inventory[INVEN_WIELD].tval != 0)
    {
      i_ptr = &inventory[INVEN_WIELD];
      if (0x20000000 & i_ptr->flags)
	tabil += 25 + i_ptr->p1*50;
      /* Regular walls; Granite, magma intrusion, quartz vein  */
      /* Don't forget the boundary walls, made of titanium (255)*/
      switch(c_ptr->fval)
	{
	case 10:
	  i = randint(1200) + 80;
	  if (twall(y, x, tabil, i))
	    msg_print("You have finished the tunnel.");
	  else
	    msg_print("You tunnel into the granite wall.");
	  break;
	case 11:
	  i = randint(600) + 10;
	  if (twall(y, x, tabil, i))
	    msg_print("You have finished the tunnel.");
	  else
	    msg_print("You tunnel into the magma intrusion.");
	  break;
	case 12:
	  i = randint(400) + 10;
	  if (twall(y, x, tabil, i))
	    msg_print("You have finished the tunnel.");
	  else
	    msg_print("You tunnel into the quartz vein.");
	  break;
	case 15:
	  msg_print("This seems to be permanent rock.");
	  break;
	default:
	  /* Is there an object in the way?  (Rubble and secret doors)*/
	  if (c_ptr->tptr != 0)
	    {
	      /* Rubble...     */
	      if (t_list[c_ptr->tptr].tval == 103)
		{
		  if (tabil > randint(180))
		    {
		      pusht((int)c_ptr->tptr);
		      c_ptr->tptr = 0;
		      c_ptr->fm = FALSE;
		      c_ptr->fopen = TRUE;
		      msg_print("You have removed the rubble.");
		      if (randint(10) == 1)
			{
			  place_object(y, x);
			  if (test_light(y, x))
			    msg_print("You have found something!");
			}
		      lite_spot(y, x);
		    }
		  else
		    msg_print("You dig in the rubble...");
		}
	      /* Secret doors...*/
	      else if (t_list[c_ptr->tptr].tval == 109)
		{
		  msg_print("You tunnel into the granite wall.");
		  search(char_row, char_col, py.misc.srh);
		}
	      else
		msg_print("You can't tunnel through that.");
	    }
	  else
	    msg_print("Tunnel through what?  Empty air???");
	  break;
	}
    }
}


/* Disarms a trap					-RAK-	*/
disarm_trap()
{
  int y, x, i, tdir;
  int tot, t1, t2, t3, t4, t5;
  register cave_type *c_ptr;
  register treasure_type *i_ptr;
  char *tmp_str;

  y = char_row;
  x = char_col;
  if (get_dir("Which direction?", &tdir, &i, &y, &x))
    {
      c_ptr = &cave[y][x];
      if (c_ptr->tptr != 0)
	{
	  t1 = py.misc.disarm; /* Ability to disarm     */
	  t2 = py.misc.lev;    /* Level adjustment      */
	  t3 = 2*todis_adj();    /* Dexterity adjustment  */
	  t4 = int_adj();        /* Intelligence adjustment*/
	  tot = t1 + t2 + t3 + t4;
	  if (py.flags.blind > 0)
	    tot = tot / 5.0;
	  else if (no_light())
	    tot = tot / 2.0;
	  if (py.flags.confused > 0)
	    tot = tot / 3.0;
	  i = t_list[c_ptr->tptr].tval;
	  t5 = t_list[c_ptr->tptr].level;
	  if (i == 102)             /* Floor trap    */
	    {
	      i_ptr = &t_list[c_ptr->tptr];
	      if ((tot - t5) > randint(100))
		{
		  msg_print("You have disarmed the trap.");
		  py.misc.exp += i_ptr->p1;
		  c_ptr->fm = FALSE;
		  pusht((int)c_ptr->tptr);
		  c_ptr->tptr = 0;
		  move_char(tdir);
		  lite_spot(y, x);
		  prt_experience();
		}
	      /* avoid randint(0) call */
	      else if ((tot > 5) && (randint(tot) > 5))
		msg_print("You failed to disarm the trap.");
	      else
		{
		  msg_print("You set the trap off!");
		  move_char(tdir);
		}
	    }
	  else if (i == 2)          /* Chest trap    */
	    {
	      i_ptr = &t_list[c_ptr->tptr];
	      if (index(i_ptr->name, '^') != 0)
		msg_print("I don't see a trap...");
	      else if (0x000001F0 & i_ptr->flags)
		{
		  if ((tot - t5) > randint(100))
		    {
		      i_ptr->flags &= 0xFFFFFE0F;
		      tmp_str = index(i_ptr->name, '(');
		      if (tmp_str != 0)
		        tmp_str[0] = '\0';
		      if (0x00000001 & i_ptr->flags)
			(void) strcat(i_ptr->name, " (Locked)");
		      else
			(void) strcat(i_ptr->name, " (Disarmed)");
		      msg_print("You have disarmed the chest.");
		      known2(i_ptr->name);
		      py.misc.exp += t5;
		      prt_experience();
		    }
		  else if ((tot > 5) && (randint(tot) > 5))
		    msg_print("You failed to disarm the chest.");
		  else
		    {
		      msg_print("You set a trap off!");
		      known2(i_ptr->name);
		      chest_trap(y, x);
		    }
		}
	      else
		msg_print("The chest was not trapped.");
	    }
	  else
	    msg_print("I do not see anything to disarm there.");
	}
      else
	msg_print("I do not see anything to disarm there.");
    }
}


/* Look at an object,  trap,  or monster			-RAK-	*/
/* Note: Looking is a free move,  see where invoked...            */
look()
{
  register int i, j;
  int y, x;
  int dir, dummy;
  int flag;
  char fchar;
  register cave_type *c_ptr;
  vtype out_val, tmp_str;

  flag = FALSE;
  y = char_row;
  x = char_col;
  if (get_dir("Look which direction?", &dir, &dummy, &y, &x))
    if (py.flags.blind < 1)
      {
	y = char_row;
	x = char_col;
	i = 0;
	do
	  {
	    (void) move(dir, &y, &x);
	    c_ptr = &cave[y][x];
	    if (c_ptr->cptr > 1)
	      if (m_list[c_ptr->cptr].ml)
		{
		  j = m_list[c_ptr->cptr].mptr;
		  fchar = c_list[j].name[0];
		  if (is_a_vowel(fchar))
		    (void) sprintf(out_val, "You see an %s.", c_list[j].name);
		  else
		    (void) sprintf(out_val, "You see a %s.", c_list[j].name);
		  msg_print(out_val);
		  flag = TRUE;
		}
	    if ((c_ptr->tl) || (c_ptr->pl) || (c_ptr->fm))
	      {
		if (c_ptr->tptr != 0)
		  if (t_list[c_ptr->tptr].tval == 109)
		    msg_print("You see a granite wall.");
		  else if (t_list[c_ptr->tptr].tval != 101)
		    {
		      inventory[INVEN_MAX] = t_list[c_ptr->tptr];
		      objdes(tmp_str, INVEN_MAX, TRUE);
		      (void) sprintf(out_val, "You see %s", tmp_str);
		      msg_print(out_val);
		      flag = TRUE;
		    }
		if (!c_ptr->fopen)
		  {
		    flag = TRUE;
		    switch(c_ptr->fval)
		      {
		      case 10: msg_print("You see a granite wall."); break;
		      case 11: msg_print("You see some dark rock."); break;
		      case 12: msg_print("You see a quartz vein."); break;
		      case 15: msg_print("You see a granite wall."); break;
		      default: break;
		      }
		  }
	      }
	    i++;
	  }
	while ((cave[y][x].fopen) && (i <= MAX_SIGHT));
	if (!flag)
	  msg_print("You see nothing of interest in that direction.");
      }
    else
      msg_print("You can't see a damn thing!");
}


/* Add to the players food time				-RAK-	*/
add_food(num)
int num;
{
  register struct flags *p_ptr;

  p_ptr = &py.flags;
  if (p_ptr->food < 0)  p_ptr->food = 0;
  p_ptr->food += num;
  if (p_ptr->food > PLAYER_FOOD_MAX)
    {
      msg_print("You are bloated from overeating.");
      p_ptr->slow = (p_ptr->food - PLAYER_FOOD_MAX) / 50;
      p_ptr->food = PLAYER_FOOD_MAX;
    }
  else if (p_ptr->food > PLAYER_FOOD_FULL)
    msg_print("You are full.");
}


/* Describe number of remaining charges...		-RAK-	*/
desc_charges(item_val)
int item_val;
{
  register int rem_num;
  vtype out_val;

  if (index(inventory[item_val].name, '^') == 0)
    {
      rem_num = inventory[item_val].p1;
      (void) sprintf(out_val, "You have %d charges remaining.", rem_num);
      msg_print(out_val);
    }
}


/* Describe amount of item remaining...			-RAK-	*/
desc_remain(item_val)
int item_val;
{
  vtype out_val, tmp_str;
  register treasure_type *i_ptr;

  inventory[INVEN_MAX] = inventory[item_val];
  i_ptr = &inventory[INVEN_MAX];
  i_ptr->number--;
  objdes(tmp_str, INVEN_MAX, TRUE);
  tmp_str[strlen(tmp_str)-1] = '\0';
  (void) sprintf(out_val, "You have %s.", tmp_str);
  msg_print(out_val);
}


inven_throw(item_val)
int item_val;
{
  register treasure_type *i_ptr;

  inventory[INVEN_MAX] = inventory[item_val];
  inventory[INVEN_MAX].number = 1;
  i_ptr = &inventory[item_val];
  if ((i_ptr->number > 1) && (i_ptr->subval > 511))
    {
      i_ptr->number--;
      inven_weight -= i_ptr->weight;
    }
  else
    inven_destroy(item_val);
}


facts(tbth, tpth, tdam, tdis)
int *tbth, *tpth, *tdam, *tdis;
{
  register int tmp_weight;
  register treasure_type *i_ptr;

  i_ptr = &inventory[INVEN_MAX];
  if (i_ptr->weight < 1)
    tmp_weight = 1;
  else
    tmp_weight = i_ptr->weight;

  /* Throwing objects			*/
  *tdam = damroll(i_ptr->damage) + i_ptr->todam;
  *tbth = (py.misc.bthb*0.75);
  *tpth = py.misc.ptohit + i_ptr->tohit;
  *tdis = (((py.stats.cstr+20)*10)/tmp_weight);
  if (*tdis > 10)  *tdis = 10;

  /* Using Bows,  slings,  or crossbows	*/
  if (inventory[INVEN_WIELD].tval == 20)
    switch(inventory[INVEN_WIELD].p1)
      {
      case 1:
	if (i_ptr->tval == 10)        /* Sling and Bullet       */
	  {
	    *tbth = py.misc.bthb;
	    *tpth += inventory[INVEN_WIELD].tohit;
	    *tdam += 2;
	    *tdis = 20;
	  }
	break;
      case 2:
	if (i_ptr->tval == 12)        /* Short Bow and Arrow    */
	  {
	    *tbth = py.misc.bthb;
	    *tpth += inventory[INVEN_WIELD].tohit;
	    *tdam += 2;
	    *tdis = 25;
	  }
	break;
      case 3:
	if (i_ptr->tval == 12)        /* Long Bow and Arrow     */
	  {
	    *tbth = py.misc.bthb;
	    *tpth += inventory[INVEN_WIELD].tohit;
	    *tdam += 3;
	    *tdis = 30;
	  }
	break;
      case 4:
	if (i_ptr->tval == 12)        /* Composite Bow and Arrow*/
	  {
	    *tbth = py.misc.bthb;
	    *tpth += inventory[INVEN_WIELD].tohit;
	    *tdam += 4;
	    *tdis = 35;
	  }
	break;
      case 5:
	if (i_ptr->tval == 11)        /* Light Crossbow and Bolt*/
	  {
	    *tbth = py.misc.bthb;
	    *tpth += inventory[INVEN_WIELD].tohit;
	    *tdam += 2;
	    *tdis = 25;
	  }
	break;
      case 6:
	if (i_ptr->tval == 11)        /* Heavy Crossbow and Bolt*/
	  {
	    *tbth = py.misc.bthb;
	    *tpth += inventory[INVEN_WIELD].tohit;
	    *tdam += 4;
	    *tdis = 35;
	    break;
	  }
      }
}


drop_throw(y, x)
int y, x;
{
  register int i, j, k;
  int flag, cur_pos;
  vtype out_val, tmp_str;
  register cave_type *c_ptr;

  flag = FALSE;
  i = y;
  j = x;
  k = 0;
  if (randint(10) > 1)
    {
      do
	{
	  if (in_bounds(i, j))
	    {
	      c_ptr = &cave[i][j];
	      if (c_ptr->fopen)
		if (c_ptr->tptr == 0)
		  flag = TRUE;
	    }
	  if (!flag)
	    {
	      i = y + randint(3) - 2;
	      j = x + randint(3) - 2;
	      k++;
	    }
	}
      while ((!flag) && (k <= 9));
    }
  if (flag)
    {
      popt(&cur_pos);
      cave[i][j].tptr = cur_pos;
      t_list[cur_pos] = inventory[INVEN_MAX];
      if (test_light(i, j))
	lite_spot(i, j);
    }
  else
    {
      objdes(tmp_str, INVEN_MAX, FALSE);
      (void) sprintf(out_val, "The %s disappears", tmp_str);
      msg_print(out_val);
    }
}

/* Throw an object across the dungeon... 		-RAK-	*/
/* Note: Flasks of oil do fire damage                            */
/* Note: Extra damage and chance of hitting when missiles are used*/
/*       with correct weapon.  I.E.  wield bow and throw arrow.  */
throw_object()
{
  int item_val, tbth, tpth, tdam, tdis;
  int y_dumy, x_dumy, dumy;
  int y, x, oldy, oldx, dir, cur_dis;
  int redraw, flag;
  char tchar[2];
  vtype out_val, tmp_str, m_name;
  register treasure_type *i_ptr;
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register int i;

  redraw = FALSE;
  if (inven_ctr == 0)
    msg_print("But you are not carrying anything.");
  else if (get_item(&item_val, "Fire/Throw which one?",
		    &redraw, 0, inven_ctr-1))
    {
      if (redraw)
	draw_cave();
      y_dumy = char_row;
      x_dumy = char_col;
      if (get_dir("Which direction?", &dir, &dumy, &y_dumy, &x_dumy))
	{
	  desc_remain(item_val);
	  if (py.flags.confused > 0)
	    {
	      msg_print("You are confused...");
	      do
		{
		  dir = randint(9);
		}
	      while (dir == 5);
	    }
	  inven_throw(item_val);
	  facts(&tbth, &tpth, &tdam, &tdis);
	  i_ptr = &inventory[INVEN_MAX];
	  tchar[0] = i_ptr->tchar;
	  tchar[1] = '\0';
	  flag = FALSE;
	  y = char_row;
	  x = char_col;
	  oldy = char_row;
	  oldx = char_col;
	  cur_dis = 0;
	  do
	    {
	      (void) move(dir, &y, &x);
	      cur_dis++;
	      if (test_light(oldy, oldx))
		lite_spot(oldy, oldx);
	      if (cur_dis > tdis)  flag = TRUE;
	      c_ptr = &cave[y][x];
	      if ((c_ptr->fopen) && (!flag))
		{
		  if (c_ptr->cptr > 1)
		    {
		      flag = TRUE;
		      m_ptr = &m_list[c_ptr->cptr];
		      tbth = tbth - cur_dis;
		      if (test_hit(tbth, (int)py.misc.lev, tpth,
				   (int)c_list[m_ptr->mptr].ac))
			{
			  i = m_ptr->mptr;
			  objdes(tmp_str, INVEN_MAX, FALSE);
			  /* Does the player know what he's fighting?      */
			  if ((0x10000 & c_list[i].cmove) &&
			      (!py.flags.see_inv))
			    (void) strcpy(m_name, "it");
			  else if (py.flags.blind > 0)
			    (void) strcpy(m_name, "it");
			  else if (!m_ptr->ml)
			    (void) strcpy(m_name, "it");
			  else
			    (void) sprintf(m_name, "the %s", c_list[i].name);

			  (void) sprintf(out_val,"The %s hits %s.", tmp_str,
				  m_name);
			  msg_print(out_val);
			  tdam = tot_dam(inventory[INVEN_MAX], tdam,
					 c_list[i]);
			  i_ptr = &inventory[INVEN_MAX];
			  tdam = critical_blow((int)i_ptr->weight, tpth, tdam);
			  i = mon_take_hit((int)c_ptr->cptr, tdam);
			  if (i >= 0)
			    {
			      (void) sprintf(out_val,"You have killed %s.",
				      m_name);
			      msg_print(out_val);
			    }
			}
		      else
			drop_throw(oldy, oldx);
		    }
		  else
		    {
		      if (panel_contains(y, x))
			if (test_light(y, x))
			  {
			    print(tchar, y, x);
			    put_qio();
			  }
		    }
		}
	      else
		{
		  flag = TRUE;
		  drop_throw(oldy, oldx);
		}
	      oldy = y;
	      oldx = x;
	    }
	  while (!flag);
	}
    }
  else
    if (redraw)
      draw_cave();
}


/* Bash open a door or chest				-RAK-	*/
/* Note: Affected by strength and weight of character            */
bash()
{
  int y, x, tmp;
  int old_ptodam, old_ptohit, old_bth;
  vtype tmp_str, m_name;
  register cave_type *c_ptr;
  register treasure_type *i_ptr, *t_ptr;
  register player_type *p_ptr;
  register monster_type *m_ptr;

  y = char_row;
  x = char_col;
  if (get_dir("Which direction?", &tmp, &tmp, &y, &x))
    {
      c_ptr = &cave[y][x];
      if (c_ptr->cptr > 1)
	{
	  if (py.flags.afraid > 0)
	    msg_print("You are afraid!");
	  else
	    {
	      /* Save old values of attacking  */
	      inventory[INVEN_MAX] = inventory[INVEN_WIELD];
	      old_ptohit = py.misc.ptohit;
	      old_ptodam = py.misc.ptodam;
	      old_bth    = py.misc.bth;
	      /* Use these values              */
	      inventory[INVEN_WIELD] = blank_treasure;
	      i_ptr = &inventory[INVEN_WIELD];
	      (void) strcpy(i_ptr->damage, inventory[26].damage);
	      i_ptr->weight = py.stats.cstr;
	      i_ptr->tval   = 1;
	      p_ptr = &py;
	      p_ptr->misc.bth    = ((p_ptr->stats.cstr+
					 p_ptr->misc.wt)/6.0);
	      p_ptr->misc.ptohit = 0;
	      p_ptr->misc.ptodam = (p_ptr->misc.wt/75.0) + 1;
	      if (py_attack(y, x))
		{
		  m_ptr = &m_list[c_ptr->cptr];
		  m_ptr->stunned += randint(2) + 1;
		  if (m_ptr->stunned > 24)  m_ptr->stunned = 24;
		  /* Does the player know what he's fighting?      */
		  if ((0x10000 & c_list[m_ptr->mptr].cmove) &&
		      (!py.flags.see_inv))
		    (void) strcpy(m_name, "It");
		  else if (py.flags.blind > 0)
		    (void) strcpy(m_name, "It");
		  else if (!m_list[c_ptr->cptr].ml)
		    (void) strcpy(m_name, "It");
		  else
		    (void) sprintf(m_name, "The %s", c_list[m_ptr->mptr].name);
		  (void) sprintf(tmp_str, "%s appears stunned!",
			  m_name);
		  msg_print(tmp_str);
		}
	      /* Restore old values            */
	      inventory[INVEN_WIELD] = inventory[INVEN_MAX];
	      py.misc.ptohit = old_ptohit;
	      py.misc.ptodam = old_ptodam;
	      py.misc.bth    = old_bth;
	      if (randint(140) > py.stats.cdex)
		{
		  msg_print("You are off-balance.");
		  py.flags.paralysis = randint(3);
		}
	    }
	}
      else if (c_ptr->tptr != 0)
	{
	  t_ptr = &t_list[c_ptr->tptr];
	  if (t_ptr->tval == 105)
	    {
	      msg_print("You smash into the door!");
	      p_ptr = &py;
	      if (test_hit((int)(p_ptr->misc.wt+p_ptr->stats.cstr), 0, 0,
			   abs(t_ptr->p1)+150))
		{
		  msg_print("The door crashes open!");
		  t_list[c_ptr->tptr] = door_list[0];
		  t_ptr->p1 = 1;
		  c_ptr->fopen = TRUE;
		  lite_spot(y, x);
		}
	      else
		{
		  msg_print("The door holds firm.");
		  py.flags.paralysis = 2;
		}
	    }
	  else if (t_ptr->tval == 2)
	    {
	      if (randint(10) == 1)
		{
		  msg_print("You have destroyed the chest...");
		  msg_print("and it's contents!");
		  (void) strcpy(t_ptr->name, "& ruined chest");
		  t_ptr->flags = 0;
		}
	      else if (0x00000001 & t_ptr->flags)
		if (randint(10) == 1)
		  {
		    msg_print("The lock breaks open!");
		    t_ptr->flags &= 0xFFFFFFFE;
		  }
	    }
	  else
	    msg_print("I do not see anything you can bash there.");
	}
      else
	msg_print("I do not see anything you can bash there.");
    }
}


/* Jam a closed door					-RAK-	*/
jamdoor()
{
  int y, x, tmp;
  int i, j;
  register cave_type *c_ptr;
  register treasure_type *t_ptr, *i_ptr;
  char tmp_str[80];

  y = char_row;
  x = char_col;
  if (get_dir("Which direction?", &tmp, &tmp, &y, &x))
    {
      c_ptr = &cave[y][x];
      if (c_ptr->tptr != 0)
	{
	  t_ptr = &t_list[c_ptr->tptr];
	  if (t_ptr->tval == 105)
	    if (c_ptr->cptr == 0)
	      {
		if (find_range(13, -1, &i, &j))
		  {
		    msg_print("You jam the door with a spike.");
		    i_ptr = &inventory[i];
		    if (i_ptr->number > 1)
			i_ptr->number--;
		    else
		      inven_destroy(i);
		    t_ptr->p1 = -abs(i_ptr->p1) - 20;
		  }
		else
		  msg_print("But you have no spikes...");
	      }
	    else
	      {
		(void) sprintf(tmp_str, "The %s is in your way!",
			c_list[m_list[c_ptr->cptr].mptr].name);
		msg_print(tmp_str);
		}
	  else if (t_ptr->tval == 104)
	    msg_print("The door must be closed first.");
	  else
	    msg_print("That isn't a door!");
	}
      else
	msg_print("That isn't a door!");
    }
}


/* Refill the players lamp				-RAK-	*/
refill_lamp()
{
  int i, j;
  register int k;
  register treasure_type *i_ptr;

  k = inventory[32].subval;
  if ((k > 0) && (k < 10))
    if (find_range(77, -1, &i, &j))
      {
	i_ptr = &inventory[32];
	i_ptr->p1 += inventory[i].p1;
	if (i_ptr->p1 > OBJ_LAMP_MAX)
	  {
	    i_ptr->p1 = OBJ_LAMP_MAX;
	    msg_print ("Your lamp overflows, spilling oil on the ground.");
	    msg_print("Your lamp is full.");
	  }
	else if (i_ptr->p1 > OBJ_LAMP_MAX/2)
	  msg_print ("Your lamp is more than half full.");
	else if (i_ptr->p1 == OBJ_LAMP_MAX/2)
	  msg_print ("Your lamp is half full.");
	else
	  msg_print ("Your lamp is less than half full.");
	desc_remain(i);
	inven_destroy(i);
      }
    else
      msg_print("You have no oil.");
  else
    msg_print("But you are not using a lamp.");
}
