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

extern int search_flag;
extern int moria_flag;

/* Updates screen when monsters move about		-RAK-	*/
update_mon(monptr)
int monptr;
{
  int flag;
  char tmp_str[2];
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register creature_type *r_ptr;

  m_ptr = &m_list[monptr];
  c_ptr = &cave[m_ptr->fy][m_ptr->fx];
  flag = FALSE;
  if (m_ptr->cdis <= MAX_SIGHT)
    if (py.flags.blind < 1)
      if (panel_contains((int)m_ptr->fy, (int)m_ptr->fx))
	/* Wizard sight...       */
	if (wizard2)
	  flag = TRUE;
        /* Normal sight...       */
	else if (los(char_row, char_col, (int)m_ptr->fy, (int)m_ptr->fx))
	  {
	    r_ptr = &c_list[m_ptr->mptr];
	    if ((c_ptr->pl) || (c_ptr->tl))
	      {
		if (py.flags.see_inv)
		  flag = TRUE;
		else if ((0x10000 & r_ptr->cmove) == 0)
		  flag = TRUE;
	      }
	    /* Infra vision...       */
	    else if (py.flags.see_infra > 0)
	      if (m_ptr->cdis <= py.flags.see_infra)
		if (0x2000 & r_ptr->cdefense)
		  flag = TRUE;
	  }
  /* Light it up...        */
  if (flag)
    {
      if (!m_ptr->ml)
	{
	  tmp_str[0] = c_list[m_ptr->mptr].cchar;
	  tmp_str[1] = '\0';
	  print(tmp_str, (int)m_ptr->fy, (int)m_ptr->fx);
	  m_ptr->ml = TRUE;
	  if (search_flag)
	    search_off();
	  if (py.flags.rest > 0)
	    rest_off();
	  flush();
	  if (find_flag)
	    {
	      find_flag = FALSE;
	      move_light (char_row, char_col, char_row, char_col);
	    }
	}
    }
  /* Turn it off...        */
  else if (m_ptr->ml)
    {
      m_ptr->ml = FALSE;
      if ((c_ptr->tl) || (c_ptr->pl))
	lite_spot((int)m_ptr->fy, (int)m_ptr->fx);
      else
	unlite_spot((int)m_ptr->fy, (int)m_ptr->fx);
    }
}


/* Choose correct directions for monster movement	-RAK-	*/
get_moves(monptr, mm)
int monptr;
register int *mm;
{
  int y, ay, x, ax, move_val;

  y = m_list[monptr].fy - char_row;
  x = m_list[monptr].fx - char_col;
  if (y < 0)
    {
      move_val = 8;
      ay = -y;
    }
  else
    {
      move_val = 0;
      ay = y;
    }
  if (x > 0)
    {
      move_val += 4;
      ax = x;
    }
  else
    ax = -x;
  if (ay > (ax*1.7321))
    move_val += 2;
  else if (ax > (ay*1.7321))
    move_val++;
  switch(move_val)
    {
    case 0:
      mm[0] = 9;
      if (ay > ax)
	{
	  mm[1] = 8;
	  mm[2] = 6;
	  mm[3] = 7;
	  mm[4] = 3;
	}
      else
	{
	  mm[1] = 6;
	  mm[2] = 8;
	  mm[3] = 3;
	  mm[4] = 7;
	}
      break;
    case 1: case 9:
      mm[0] = 6;
      if (y < 0)
	{
	  mm[1] = 3;
	  mm[2] = 9;
	  mm[3] = 2;
	  mm[4] = 8;
	}
      else
	{
	  mm[1] = 9;
	  mm[2] = 3;
	  mm[3] = 8;
	  mm[4] = 2;
	}
      break;
    case 2: case 6:
      mm[0] = 8;
      if (x < 0)
	{
	  mm[1] = 9;
	  mm[2] = 7;
	  mm[3] = 6;
	  mm[4] = 4;
	}
      else
	{
	  mm[1] = 7;
	  mm[2] = 9;
	  mm[3] = 4;
	  mm[4] = 6;
	}
      break;
    case 4:
      mm[0] = 7;
      if (ay > ax)
	{
	  mm[1] = 8;
	  mm[2] = 4;
	  mm[3] = 9;
	  mm[4] = 1;
	}
      else
	{
	  mm[1] = 4;
	  mm[2] = 8;
	  mm[3] = 1;
	  mm[4] = 9;
	}
      break;
    case 5: case 13:
      mm[0] = 4;
      if (y < 0)
	{
	  mm[1] = 1;
	  mm[2] = 7;
	  mm[3] = 8;
	  mm[4] = 2;
	}
      else
	{
	  mm[1] = 7;
	  mm[2] = 1;
	  mm[3] = 2;
	  mm[4] = 8;
	}
      break;
    case 8:
      mm[0] = 3;
      if (ay > ax)
	{
	  mm[1] = 2;
	  mm[2] = 6;
	  mm[3] = 1;
	  mm[4] = 9;
	}
      else
	{
	  mm[1] = 6;
	  mm[2] = 2;
	  mm[3] = 9;
	  mm[4] = 1;
	}
      break;
    case 10: case 14:
      mm[0] = 2;
      if (x < 0)
	{
	  mm[1] = 1;
	  mm[2] = 3;
	  mm[3] = 4;
	  mm[4] = 6;
	}
      else
	{
	  mm[1] = 3;
	  mm[2] = 1;
	  mm[3] = 6;
	  mm[4] = 4;
	}
      break;
    case 12:
      mm[0] = 1;
      if (ay > ax)
	{
	  mm[1] = 2;
	  mm[2] = 4;
	  mm[3] = 3;
	  mm[4] = 7;
	}
      else
	{
	  mm[1] = 4;
	  mm[2] = 2;
	  mm[3] = 7;
	  mm[4] = 3;
	}
      break;
    }
}


/* Make an attack on the player (chuckle...)		-RAK-	*/
make_attack(monptr)
int monptr;
{
  int xpos, attype, adesc, dam;
  int i, j, l;
  vtype attstr, attx;
  vtype cdesc, ddesc, tmp_str;
  dtype damstr;
  int flag;
  register creature_type *c_ptr;
  monster_type *m_ptr;
  register struct misc *p_ptr;
  register struct flags *f_ptr;
  register treasure_type *i_ptr;
  char *string;

  m_ptr = &m_list[monptr];
  c_ptr = &c_list[m_ptr->mptr];
  (void) strcpy(attstr, c_ptr->damage);
  if ((0x10000 & c_ptr->cmove) && (!py.flags.see_inv))
    (void) strcpy(cdesc, "It ");
  else if (py.flags.blind > 0)
    (void) strcpy(cdesc, "It ");
  else if (!m_ptr->ml)
    (void) strcpy(cdesc, "It ");
  else
    (void) sprintf(cdesc, "The %s ", c_ptr->name);
  /* For "DIED_FROM" string        */
  if (0x80000000 & c_ptr->cmove)
    (void) sprintf(ddesc, "The %s", c_ptr->name);
  else
    (void) sprintf(ddesc, "& %s", c_ptr->name);
  (void) strcpy(inventory[INVEN_MAX].name, ddesc);
  inventory[INVEN_MAX].number = 1;
  objdes(ddesc, INVEN_MAX, TRUE);
  /* End DIED_FROM                 */

  while (strlen(attstr) > 0)
    {
      string = index(attstr, '|');
      if (string)
	xpos = strlen(attstr) - strlen(string);
      else
	xpos = -1;
      if (xpos >= 0)
	{
	  (void) strncpy(attx, attstr, xpos);
	  attx[xpos] = '\0';
	  (void) strcpy(attstr, &attstr[xpos+1]);
	}
      else
	{
	  (void) strcpy(attx, attstr);
	  attstr[0] = '\0';
	}
      (void) sscanf(attx, "%d %d %s", &attype, &adesc, damstr);
      flag = FALSE;
      if (py.flags.protevil > 0)
	if (c_ptr->cdefense & 0x0004)
	  if ((py.misc.lev + 1) > c_ptr->level)
	    {
	      attype = 99;
	      adesc = 99;
	    }
      p_ptr = &py.misc;
      switch(attype)
	{
	case 1:       /*Normal attack  */
	  if (test_hit(60, (int)c_ptr->level, 0, p_ptr->pac+p_ptr->ptoac))
	    flag = TRUE;
	  break;
	case 2:       /*Poison Strength*/
	  if (test_hit(-3, (int)c_ptr->level, 0, p_ptr->pac+p_ptr->ptoac))
	    flag = TRUE;
	  break;
	case 3:       /*Confusion attack*/
	  if (test_hit(10, (int)c_ptr->level, 0, p_ptr->pac+p_ptr->ptoac))
	    flag = TRUE;
	  break;
	case 4:       /*Fear attack    */
	  if (test_hit(10, (int)c_ptr->level, 0, p_ptr->pac+p_ptr->ptoac))
	    flag = TRUE;
	  break;
	case 5:       /*Fire attack    */
	  if (test_hit(10, (int)c_ptr->level, 0, p_ptr->pac+p_ptr->ptoac))
	    flag = TRUE;
	  break;
	case 6:       /*Acid attack    */
	  if (test_hit(0, (int)c_ptr->level, 0, p_ptr->pac+p_ptr->ptoac))
	    flag = TRUE;
	  break;
	case 7:       /*Cold attack    */
	  if (test_hit(10, (int)c_ptr->level, 0, p_ptr->pac+p_ptr->ptoac))
	    flag = TRUE;
	  break;
	case 8:       /*Lightning attack*/
	  if (test_hit(10, (int)c_ptr->level, 0, p_ptr->pac+p_ptr->ptoac))
	    flag = TRUE;
	  break;
	case 9:       /*Corrosion attack*/
	  if (test_hit(0, (int)c_ptr->level, 0, p_ptr->pac+p_ptr->ptoac))
	    flag = TRUE;
	  break;
	case 10:      /*Blindness attack*/
	  if (test_hit(2, (int)c_ptr->level, 0, p_ptr->pac+p_ptr->ptoac))
	    flag = TRUE;
	  break;
	case 11:      /*Paralysis attack*/
	  if (test_hit(2, (int)c_ptr->level, 0, p_ptr->pac+p_ptr->ptoac))
	    flag = TRUE;
	  break;
	case 12:      /*Steal Money    */
	  if (test_hit(5, (int)c_ptr->level, 0, (int)py.misc.lev))
	    if (py.misc.au > 0)
	      flag = TRUE;
	  break;
	case 13:      /*Steal Object   */
	  if (test_hit(2, (int)c_ptr->level, 0, (int)py.misc.lev))
	    if (inven_ctr > 0)
	      flag = TRUE;
	  break;
	case 14:      /*Poison         */
	  if (test_hit(5, (int)c_ptr->level, 0, p_ptr->pac+p_ptr->ptoac))
	    flag = TRUE;
	  break;
	case 15:      /*Lose dexterity*/
	  if (test_hit(0, (int)c_ptr->level, 0, p_ptr->pac+p_ptr->ptoac))
	    flag = TRUE;
	  break;
	case 16:      /*Lose constitution*/
	  if (test_hit(0, (int)c_ptr->level, 0, p_ptr->pac+p_ptr->ptoac))
	    flag = TRUE;
	  break;
	case 17:      /*Lose intelligence*/
	  if (test_hit(2, (int)c_ptr->level, 0, p_ptr->pac+p_ptr->ptoac))
	    flag = TRUE;
	  break;
	case 18:      /*Lose wisdom*/
	  if (test_hit(0, (int)c_ptr->level, 0, p_ptr->pac+p_ptr->ptoac))
	    flag = TRUE;
	  break;
	case 19:      /*Lose experience*/
	  if (test_hit(5, (int)c_ptr->level, 0, p_ptr->pac+p_ptr->ptoac))
	    flag = TRUE;
	  break;
	case 20:      /*Aggravate monsters*/
	  flag = TRUE;
	  break;
	case 21:      /*Disenchant        */
	  if (test_hit(20, (int)c_ptr->level, 0, p_ptr->pac+p_ptr->ptoac))
	    flag = TRUE;
	  break;
	case 22:      /*Eat food          */
	  if (test_hit(5, (int)c_ptr->level, 0, p_ptr->pac+p_ptr->ptoac))
	    flag = TRUE;
	  break;
	case 23:      /*Eat light         */
	  if (test_hit(5, (int)c_ptr->level, 0, p_ptr->pac+p_ptr->ptoac))
	    flag = TRUE;
	  break;
	case 24:      /*Eat charges       */
	  if (test_hit(15, (int)c_ptr->level, 0, p_ptr->pac+p_ptr->ptoac))
	    flag = TRUE;
	  break;
	case 99:
	  flag = TRUE;
	  break;
	default:
	  break;
	}
      if (flag)
	{
	  /* can not strcat to cdesc because the creature may have
	     multiple attacks */
	  (void) strcpy(tmp_str, cdesc);
	  switch(adesc)
	    {
	    case 1: msg_print(strcat(tmp_str, "hits you.")); break;
	    case 2: msg_print(strcat(tmp_str, "bites you.")); break;
	    case 3: msg_print(strcat(tmp_str, "claws you.")); break;
	    case 4: msg_print(strcat(tmp_str, "stings you.")); break;
	    case 5: msg_print(strcat(tmp_str, "touches you.")); break;
	    case 6: msg_print(strcat(tmp_str, "kicks you.")); break;
	    case 7: msg_print(strcat(tmp_str, "gazes at you.")); break;
	    case 8: msg_print(strcat(tmp_str, "breathes on you.")); break;
	    case 9: msg_print(strcat(tmp_str, "spits on you.")); break;
	    case 10: msg_print(strcat(tmp_str,"makes a horrible wail."));break;
	    case 11: msg_print(strcat(tmp_str, "embraces you.")); break;
	    case 12: msg_print(strcat(tmp_str, "crawls on you.")); break;
	    case 13:
	      msg_print(strcat(tmp_str, "releases a cloud of spores.")); break;
	    case 14: msg_print(strcat(tmp_str, "begs you for money.")); break;
	    case 15: msg_print("You've been slimed!"); break;
	    case 16: msg_print(strcat(tmp_str, "crushes you.")); break;
	    case 17: msg_print(strcat(tmp_str, "tramples you.")); break;
	    case 18: msg_print(strcat(tmp_str, "drools on you.")); break;
	    case 19:
	      switch(randint(9))
		{
		case 1: msg_print(strcat(tmp_str, "insults you!")); break;
		case 2:
		  msg_print(strcat(tmp_str, "insults your mother!")); break;
		case 3:
		  msg_print(strcat(tmp_str, "gives you the finger!")); break;
		case 4: msg_print(strcat(tmp_str, "humiliates you!")); break;
		case 5: msg_print(strcat(tmp_str, "wets on your leg!")); break;
		case 6: msg_print(strcat(tmp_str, "defiles you!")); break;
		case 7: msg_print(strcat(tmp_str, "dances around you!"));break;
		case 8:
		  msg_print(strcat(tmp_str, "makes obscene gestures!")); break;
		case 9: msg_print(strcat(tmp_str, "moons you!!!")); break;
		}
	      break;
	    case 99: msg_print(strcat(tmp_str, "is repelled.")); break;
	    default: break;
	    }
	  switch(attype)
	    {
	    case 1:    /*Normal attack  */
	      dam = damroll(damstr);
	      dam -= (int)((((p_ptr->pac+p_ptr->ptoac)/200.0)*dam)+.5);
	      take_hit(dam, ddesc);
	      prt_chp();
	      break;
	    case 2:    /*Poison Strength*/
	      take_hit(damroll(damstr), ddesc);
	      if (py.flags.sustain_str)
		msg_print("You feel weaker for a moment,  it passes.");
	      else if (randint(2) == 1)
		{
		  msg_print("You feel weaker.");
		  py.stats.cstr = de_statp(py.stats.cstr);
		  prt_strength();
		  /* adjust misc stats */
		  py_bonuses(blank_treasure, 0);
		}
	      prt_chp();
	      break;
	    case 3:    /*Confusion attack*/
	      f_ptr = &py.flags;
	      take_hit(damroll(damstr), ddesc);
	      if (randint(2) == 1)
		{
		  if (f_ptr->confused < 1)
		    {
		      msg_print("You feel confused.");
		      f_ptr->confused += randint((int)c_ptr->level);
		    }
		  f_ptr->confused += 3;
		}
	      prt_chp();
	      break;
	    case 4:    /*Fear attack    */
	      f_ptr = &py.flags;
	      take_hit(damroll(damstr), ddesc);
	      if (player_saves(wis_adj()))
		msg_print("You resist the effects!");
	      else if (f_ptr->afraid < 1)
		{
		  msg_print("You are suddenly afraid!");
		  f_ptr->afraid += 3 + randint((int)c_ptr->level);
		}
	      else
		f_ptr->afraid += 3;
	      prt_chp();
	      break;
	    case 5:    /*Fire attack    */
	      msg_print("You are enveloped in flames!");
	      fire_dam(damroll(damstr), ddesc);
	      break;
	    case 6:    /*Acid attack    */
	      msg_print("You are covered in acid!");
	      acid_dam(damroll(damstr), ddesc);
	      break;
	    case 7:    /*Cold attack    */
	      msg_print("You are covered with frost!");
	      cold_dam(damroll(damstr), ddesc);
	      break;
	    case 8:    /*Lightning attack*/
	      msg_print("Lightning strikes you!");
	      light_dam(damroll(damstr), ddesc);
	      break;
	    case 9:    /*Corrosion attack*/
	      msg_print("A stinging red gas swirls about you.");
	      corrode_gas(ddesc);
	      take_hit(damroll(damstr), ddesc);
	      prt_chp();
	      break;
	    case 10:    /*Blindness attack*/
	      f_ptr = &py.flags;
	      take_hit(damroll(damstr), ddesc);
	      if (f_ptr->blind < 1)
		{
		  f_ptr->blind += 10 + randint((int)c_ptr->level);
		  msg_print("Your eyes begin to sting.");
		}
	      f_ptr->blind += 5;
	      prt_chp();
	      break;
	    case 11:    /*Paralysis attack*/
		f_ptr = &py.flags;
	      take_hit(damroll(damstr), ddesc);
	      if (player_saves(con_adj()))
		msg_print("You resist the effects!");
	      else if (f_ptr->paralysis < 1)
		{
		  if (f_ptr->free_act)
		    msg_print("You are unaffected.");
		  else
		    {
		      f_ptr->paralysis = randint((int)c_ptr->level) + 3;
		      msg_print("You are paralyzed.");
		    }
		}
	      prt_chp();
	      break;
	    case 12:    /*Steal Money     */
	      if ((randint(124) < py.stats.cdex) && (py.flags.paralysis < 1))
		msg_print("You quickly protect your money pouch!");
	      else
		{
		  i = (p_ptr->au/10) + randint(25);
		  if (i > p_ptr->au)
		    p_ptr->au = 0;
		  else
		    p_ptr->au -= i;
		  msg_print("Your purse feels lighter.");
		  prt_gold();
		}
	      if (randint(2) == 1)
		{
		  msg_print("There is a puff of smoke!");
		  teleport_away(monptr, MAX_SIGHT);
		}
	      break;
	    case 13:    /*Steal Object   */
	      if ((randint(124) < py.stats.cdex) && (py.flags.paralysis < 1))
		msg_print("You grab hold of your backpack!");
	      else
		{
		  i = randint(inven_ctr) - 1;
		  inven_destroy(i);
		  msg_print("Your backpack feels lighter.");
		}
	      if (randint(2) == 1)
		{
		  msg_print("There is a puff of smoke!");
		  teleport_away(monptr, MAX_SIGHT);
		}
	      break;
	    case 14:    /*Poison         */
	      f_ptr = &py.flags;
	      take_hit(damroll(damstr), ddesc);
	      prt_chp();
	      msg_print("You feel very sick.");
	      f_ptr->poisoned += randint((int)c_ptr->level)+5;
	      break;
	    case 15:    /*Lose dexterity */
	      f_ptr = &py.flags;
	      take_hit(damroll(damstr), ddesc);
	      if (f_ptr->sustain_dex)
		msg_print("You feel clumsy for a moment,  it passes.");
	      else
		{
		  msg_print("You feel more clumsy.");
		  py.stats.cdex = de_statp(py.stats.cdex);
		  prt_dexterity();
		  /* adjust misc stats */
		  py_bonuses(blank_treasure, 0);
		}
	      prt_chp();
	      break;
	    case 16:    /*Lose constitution */
	      f_ptr = &py.flags;
	      take_hit(damroll(damstr), ddesc);
	      if (f_ptr->sustain_con)
		msg_print("Your body resists the effects of the disease.");
	      else
		{
		  msg_print("Your health is damaged!");
		  py.stats.ccon = de_statp(py.stats.ccon);
		  prt_constitution();
		}
	      prt_chp();
	      break;
	    case 17:    /*Lose intelligence */
	      f_ptr = &py.flags;
	      take_hit(damroll(damstr), ddesc);
	      if (f_ptr->sustain_int)
		{
		  msg_print("You feel your memories fading...");
		  msg_print("Your memories are suddenly restored!");
		}
	      else
		{
		  msg_print("You feel your memories fading...");
		  py.stats.cint = de_statp(py.stats.cint);
		  prt_intelligence();
		}
	      prt_chp();
	      break;
	    case 18:    /*Lose wisdom      */
	      f_ptr = &py.flags;
	      take_hit(damroll(damstr), ddesc);
	      if (f_ptr->sustain_wis)
		msg_print("Your wisdom is sustained.");
	      else
		{
		  msg_print("Your wisdom is drained.");
		  py.stats.cwis = de_statp(py.stats.cwis);
		  prt_wisdom();
		}
	      prt_chp();
	      break;
	    case 19:    /*Lose experience  */
	      msg_print("You feel your life draining away!");
	      i = damroll(damstr) + (py.misc.exp / 100)*MON_DRAIN_LIFE;
	      lose_exp(i);
	      break;
	    case 20:    /*Aggravate monster*/
	      (void) aggravate_monster(5);
	      break;
	    case 21:    /*Disenchant       */
	      flag = FALSE;
	      switch(randint(7))
		{
		case 1: i = INVEN_WIELD; break;
		case 2: i = INVEN_BODY;  break;
		case 3: i = INVEN_ARM;   break;
		case 4: i = INVEN_OUTER; break;
		case 5: i = INVEN_HANDS; break;
		case 6: i = INVEN_HEAD;  break;
		case 7: i = INVEN_FEET;  break;
		}
	      i_ptr = &inventory[i];
	      if (i_ptr->tohit > 0)
		{
		  i_ptr->tohit -= randint(2);
		  flag = TRUE;
		}
	      if (i_ptr->todam > 0)
		{
		  i_ptr->todam -= randint(2);
		  flag = TRUE;
		}
	      if (i_ptr->toac > 0)
		{
		  i_ptr->toac  -= randint(2);
		  flag = TRUE;
		}
	      if (flag)
		{
		  msg_print("There is a static feeling in the air...");
		  py_bonuses(blank_treasure, 1);
		}
	      break;
	    case 22:    /*Eat food         */
	      if (find_range(80, -1, &i, &j))
		inven_destroy(i);
	      break;
	    case 23:    /*Eat light        */
	      i_ptr = &inventory[INVEN_LIGHT];
	      if (i_ptr->p1 > 0)
		{
		  i_ptr->p1 -= (250 + randint(250));
		  if (i_ptr->p1 < 1)  i_ptr->p1 = 1;
		  msg_print("Your light dims...");
		}
	      break;
	    case 24:    /*Eat charges     */
	      i = randint(inven_ctr) - 1;
	      l = c_ptr->level;
	      i_ptr = &inventory[i];
	      if ((i_ptr->tval == 55) || (i_ptr->tval == 60) ||
		  (i_ptr->tval == 65))
		if (i_ptr->p1 > 0)
		  {
		    m_ptr->hp += l*i_ptr->p1;
		    i_ptr->p1 = 0;
		    msg_print("Energy drains from your pack!");
		  }
	      break;
	    case 99:  break;
	    default: break;
	    }
	}
      else
	switch(adesc)
	  {
	  case 1: case 2: case 3: case 6:
	    (void) strcpy(tmp_str, cdesc);
	    msg_print(strcat(tmp_str, "misses you."));
	    break;
	  default: break;
	  }
    }
}


/* Make the move if possible, five choices		-RAK-	*/
int make_move(monptr, mm)
int monptr;
int *mm;
{
  int i, j, newy, newx;
  unsigned int movebits;
  int flag, tflag;
  int res;
  register cave_type *c_ptr;
  register monster_type *m_ptr;
  register treasure_type *t_ptr;
  register creature_type *r_ptr;
  char tmp_str[80];
  vtype m_name;

  i = 0;
  flag = FALSE;
  res = FALSE;
  movebits = c_list[m_list[monptr].mptr].cmove;
  do
    {
      /* Get new position               */
      newy = m_list[monptr].fy;
      newx = m_list[monptr].fx;
      (void) move(mm[i], &newy, &newx);
      c_ptr = &cave[newy][newx];
      if (c_ptr->fval != 15)
	{
	  tflag = FALSE;
	  /* Floor is open?                */
	  if (c_ptr->fopen)
	    tflag = TRUE;
	      /* Creature moves through walls? */
	  else if (movebits & 0x40000)
	    tflag = TRUE;
	      /* Creature can open doors?      */
	  else if (c_ptr->tptr != 0)
	    {
	      t_ptr = &t_list[c_ptr->tptr];
	      m_ptr = &m_list[monptr];
	      if (movebits & 0x20000)
		{     /* Creature can open doors...                    */
		  switch(t_ptr->tval)
		    {
		    case 105:    /* Closed doors...       */
		      if (t_ptr->p1 == 0)               /* Closed doors  */
			{
			  tflag = TRUE;
			  if (los(char_row, char_col, newy, newx))
			    {
			      t_list[c_ptr->tptr] = door_list[0];
			      c_ptr->fopen = TRUE;
			      lite_spot(newy, newx);
			      tflag = FALSE;
			    }
			}
		      else if (t_ptr->p1 > 0)          /* Locked doors  */
			{
			  if (randint(100-t_ptr->level) < 5)
			    t_ptr->p1 = 0;
			}
		      else if (t_ptr->p1 < 0)          /* Stuck doors   */
			{
			  if (randint(m_ptr->hp+1) > (10+abs(t_ptr->p1)))
			    t_ptr->p1 = 0;
			}
		      break;
		    case 109:    /* Secret doors...       */
		      tflag = TRUE;
		      if (los(char_row, char_col, newy, newx))
			{
			  t_list[c_ptr->tptr] = door_list[0];
			  c_ptr->fopen = TRUE;
			  lite_spot(newy, newx);
			  tflag = FALSE;
			}
		      break;
		    default: break;
		    }
		}
	      else
		{     /* Creature can not open doors, must bash them   */
		  switch(t_ptr->tval)
		    {
		    case 105:    /* Closed doors...       */
		      j = abs(t_ptr->p1) + 20;
		      if (randint(m_ptr->hp+1) > j)
			{
			  tflag = TRUE;
			  if (los(char_row, char_col, newy, newx))
			    {
			      t_list[c_ptr->tptr] = door_list[0];
			      t_list[c_ptr->tptr].p1 = randint(2) - 1;
			      c_ptr->fopen = TRUE;
			      lite_spot(newy, newx);
			      tflag = FALSE;
			    }
			}
		      break;
		    case 109:        /* Secret doors...       */
		      break;
		    default:
		      break;
		    }
		}
	    }
	  /* Glyph of warding present?     */
	  if (tflag)              /* Scare Monster trap    */
	    if (c_ptr->tptr != 0)
	      if (t_list[c_ptr->tptr].tval == 102)
		if (t_list[c_ptr->tptr].subval == 99)
		  {
		    if (randint(OBJ_RUNE_PROT) <
			c_list[m_list[monptr].mptr].level)
		      {
			if ((newy==char_row) && (newx==char_col))
			  msg_print("The rune of protection is broken!");
			(void) delete_object(newy, newx);
		      }
		    else
		      tflag = FALSE;
		  }
	  /* Creature has attempted to move on player?     */
	  if (tflag)
	    if (c_ptr->cptr == 1)
	      {
		if (!m_list[monptr].ml)
		  update_mon(monptr);
		if (search_flag)  search_off();
		if (py.flags.rest > 0)  rest_off();
		if (find_flag)
		  {
		    find_flag = FALSE;
		    move_light (char_row, char_col, char_row, char_col);
		  }
		flush();
		make_attack(monptr);
		/* Player has read a Confuse Monster?    */
		/* Monster gets a saving throw...        */
		if (py.flags.confuse_monster)
		  {
		    m_ptr = &m_list[monptr];
		    r_ptr = &c_list[m_ptr->mptr];
		    msg_print("Your hands stop glowing.");
		    py.flags.confuse_monster = FALSE;

		    /* Does the player know what he's fighting?      */
		    if ((0x10000 & r_ptr->cmove) && (!py.flags.see_inv))
		      (void) strcpy(m_name, "It");
		    else if (py.flags.blind > 0)
		      (void) strcpy(m_name, "It");
		    else if (!m_ptr->ml)
		      (void) strcpy(m_name, "It");
		    else
		      (void) sprintf(m_name, "The %s", r_ptr->name);
		    if ((randint(MAX_MONS_LEVEL) < r_ptr->level) ||
			(0x1000 & r_ptr->cdefense))
		      {
			(void) sprintf(tmp_str, "%s is unaffected.",
				m_name);
			msg_print(tmp_str);
		      }
		    else
		      {
			(void) sprintf(tmp_str, "%s appears confused.",
				       m_name);
			msg_print(tmp_str);
			m_ptr->confused = TRUE;
		      }
		  }
		tflag = FALSE;
		flag  = TRUE;
	      }
	  /* Creature is attempting to move on other creature?     */
	    else if ((c_ptr->cptr > 1) &&
		     ((newy != m_list[monptr].fy) ||
		      (newx != m_list[monptr].fx)))
	      {
		/* Creature eats other creatures?        */
		if (movebits & 0x80000)
		  delete_monster((int)c_ptr->cptr);
		else
		  tflag = FALSE;
	      }
	  /* Creature has been allowed move...     */
	  if (tflag)
	    {
	      m_ptr = &m_list[monptr];
	      /* Pick up or eat an object              */
	      if (movebits & 0x100000)
		{
		  c_ptr = &cave[newy][newx];

		  if (c_ptr->tptr != 0)
		    if (t_list[c_ptr->tptr].tval < 100)
		      (void) delete_object(newy, newx);
		}
	      /* Move creature record                  */
	      move_rec((int)m_ptr->fy, (int)m_ptr->fx, newy, newx);
	      m_ptr->fy = newy;
	      m_ptr->fx = newx;
	      flag = TRUE;
	      res = TRUE;
	    }
	}
      i++;
      /* Up to 5 attempts at moving,   give up...   */
    }
  while ((!flag) && (i < 5));
  return(res);
}


/* Creatures can cast spells too.  (Dragon Breath)	-RAK-	*/
/* cast_spell = true if creature changes position       */
/* took_turn  = true if creature casts a spell          */
int mon_cast_spell(monptr, took_turn)
int monptr;
int *took_turn;
{
  unsigned int i;
  int y, x;
  register int k;
  int chance, thrown_spell;
  double r1;
  int spell_choice[30];
  vtype cdesc, ddesc, outval;
  int flag;
  register monster_type *m_ptr;
  register creature_type *r_ptr;
  int cast;

  m_ptr = &m_list[monptr];
  r_ptr = &c_list[m_ptr->mptr];
  chance = (int)((r_ptr->spells & 0x0000000F));
  /* 1 in x chance of casting spell                */
  if (randint(chance) != 1)
    {
      cast = FALSE;
      *took_turn  = FALSE;
    }
  /* Must be within certain range                  */
  else if (m_ptr->cdis > MAX_SPELL_DIS)
    {
      cast = FALSE;
      *took_turn  = FALSE;
    }
  /* Must have unobstructed Line-Of-Sight          */
  else if (!los(char_row, char_col, (int)m_ptr->fy, (int)m_ptr->fx))
    {
      cast = FALSE;
      *took_turn  = FALSE;
    }
  else  /* Creature is going to cast a spell     */
    {
      *took_turn  = TRUE;
      cast = TRUE;
      /* Describe the attack                           */
      flag = TRUE;
      if (!m_ptr->ml)
	flag = FALSE;
      else if ((0x10000 & c_list[m_ptr->mptr].cmove) &&
	       (!py.flags.see_inv))
	flag = FALSE;
      else if (py.flags.blind > 0)
	flag = FALSE;
      if (flag)
	(void) sprintf(cdesc, "The %s ", r_ptr->name);
      else
	(void) strcpy(cdesc, "It ");
      /* For "DIED_FROM" string  */
      if (0x80000000 & r_ptr->cmove)
	(void) sprintf(ddesc, "The %s", r_ptr->name);
      else
	(void) sprintf(ddesc, "& %s", r_ptr->name);
      (void) strcpy(inventory[INVEN_MAX].name, ddesc);
      inventory[INVEN_MAX].number = 1;
      objdes(ddesc, INVEN_MAX, TRUE);
      /* End DIED_FROM                 */

      /* Extract all possible spells into spell_choice */
      i = (r_ptr->spells & 0xFFFFFFF0);
      k = 0;
      while (i != 0)
	{
	  spell_choice[k] = bit_pos(&i);
	  k++;
	}
      /* Choose a spell to cast                        */
      thrown_spell = spell_choice[randint(k) - 1];
      thrown_spell++;
      /* Cast the spell...                             */
      switch(thrown_spell)
	{
	case 5:  /*Teleport Short*/
	  teleport_away(monptr, 5);
	  break;
	case 6:  /*Teleport Long */
	  teleport_away(monptr, MAX_SIGHT);
	  break;
	case 7:  /*Teleport To   */
	  (void) strcat(cdesc, "casts a spell.");
	  msg_print(cdesc);
	  teleport_to((int)m_ptr->fy, (int)m_ptr->fx);
	  break;
	case 8:  /*Light Wound   */
	  (void) strcat(cdesc, "casts a spell.");
	  msg_print(cdesc);
	  if (player_saves(wis_adj()))
	    msg_print("You resist the effects of the spell.");
	  else
	    take_hit(damroll("3d8"), ddesc);
	  break;
	case 9:  /*Serious Wound */
	  (void) strcat(cdesc, "casts a spell.");
	  msg_print(cdesc);
	  if (player_saves(wis_adj()))
	    msg_print("You resist the effects of the spell.");
	  else
	    take_hit(damroll("8d8"), ddesc);
	  break;
	case 10:  /*Hold Person   */
	  (void) strcat(cdesc, "casts a spell.");
	  msg_print(cdesc);
	  if (py.flags.free_act)
	    msg_print("You are unaffected...");
	  else if (player_saves(wis_adj()))
	    msg_print("You resist the effects of the spell.");
	  else if (py.flags.paralysis > 0)
	    py.flags.paralysis += 2;
	  else
	    py.flags.paralysis = randint(5)+4;
	  break;
	case 11:  /*Cause Blindness*/
	  (void) strcat(cdesc, "casts a spell.");
	  msg_print(cdesc);
	  if (player_saves(wis_adj()))
	    msg_print("You resist the effects of the spell.");
	  else if (py.flags.blind > 0)
	    py.flags.blind += 6;
	  else
	    py.flags.blind += 12 + randint(3);
	  break;
	case 12:  /*Cause Confuse */
	  (void) strcat(cdesc, "casts a spell.");
	  msg_print(cdesc);
	  if (player_saves(wis_adj()))
	    msg_print("You resist the effects of the spell.");
	  else if (py.flags.confused > 0)
	    py.flags.confused += 2;
	  else
	    py.flags.confused = randint(5) + 3;
	  break;
	case 13:  /*Cause Fear    */
	  (void) strcat(cdesc, "casts a spell.");
	  msg_print(cdesc);
	  if (player_saves(wis_adj()))
	    msg_print("You resist the effects of the spell.");
	  else if (py.flags.afraid > 0)
	    py.flags.afraid += 2;
	  else
	    py.flags.afraid = randint(5) + 3;
	  break;
	case 14:  /*Summon Monster*/
	  (void) strcat(cdesc, "magically summons a monster!");
	  msg_print(cdesc);
	  y = char_row;
	  x = char_col;
	  (void) summon_monster(&y, &x, FALSE);
	  check_mon_lite(y, x);
	  break;
	case 15:  /*Summon Undead*/
	  (void) strcat(cdesc, "magically summons an undead!");
	  msg_print(cdesc);
	  y = char_row;
	  x = char_col;
	  (void) summon_undead(&y, &x);
	  check_mon_lite(y, x);
	  break;
	case 16:  /*Slow Person  */
	  (void) strcat(cdesc, "casts a spell.");
	  msg_print(cdesc);
	  if (py.flags.free_act)
	    msg_print("You are unaffected...");
	  else if (player_saves(wis_adj()))
	    msg_print("You resist the effects of the spell.");
	  else if (py.flags.slow > 0)
	    py.flags.slow += 2;
	  else
	    py.flags.slow = randint(5) + 3;
	  break;
	case 17:  /*Drain Mana   */
	  if ((py.misc.cmana) > 0)
	    {
	      (void) sprintf(outval, "%sdraws psychic energy from you!",cdesc);
	      msg_print(outval);
	      if (flag)
		{
		  (void) sprintf(outval, "%sappears healthier...", cdesc);
		  msg_print(outval);
		}
	      r1 = ( randint((int)r_ptr->level) / 2 ) + 1;
	      if (r1 > py.misc.cmana)  r1 = py.misc.cmana;
	      py.misc.cmana -= r1;
	      m_ptr->hp += 6*(r1);
	    }
	  break;
	case 20:  /*Breath Light */
	  (void) strcat(cdesc, "breathes lightning.");
	  msg_print(cdesc);
	  breath(1, char_row, char_col, (int)(m_ptr->hp/4.0), ddesc);
	  break;
	case 21:  /*Breath Gas   */
	  (void) strcat(cdesc, "breathes gas.");
	  msg_print(cdesc);
	  breath(2, char_row, char_col, (int)(m_ptr->hp/3.0), ddesc);
	  break;
	case 22:  /*Breath Acid  */
	  (void) strcat(cdesc, "breathes acid.");
	  msg_print(cdesc);
	  breath(3, char_row, char_col, (int)(m_ptr->hp/3.0), ddesc);
	  break;
	case 23:  /*Breath Frost */
	  (void) strcat(cdesc, "breathes frost.");
	  msg_print(cdesc);
	  breath(4, char_row, char_col, (int)(m_ptr->hp/3.0), ddesc);
	  break;
	case 24:  /*Breath Fire  */
	  (void) strcat(cdesc, "breathes fire.");
	  msg_print(cdesc);
	  breath(5, char_row, char_col, (int)(m_ptr->hp/3.0), ddesc);
	  break;
	default:
	  msg_print("Creature cast unknown spell.");
	  cdesc[0] = '\0';
	}
      /* End of spells                                 */
    }
  return(cast);
}


/* Move the critters about the dungeon			-RAK-	*/
int mon_move(monptr)
int monptr;
{
  register int i, j;
  int k;
  int move_test;
  int movem;
  register creature_type *c_ptr;
  register monster_type *m_ptr;
  int mm[5];

  /* Main procedure for monster movement (MON_MOVE)	-RAK-	*/
  movem = FALSE;
  c_ptr = &c_list[m_list[monptr].mptr];
  /* Does the critter multiply?                            */
  if (c_ptr->cmove & 0x00200000)
    if (MAX_MON_MULT >= mon_tot_mult)
      if ((py.flags.rest % MON_MULT_ADJ) == 0)
	{
	  m_ptr = &m_list[monptr];
	  k = 0;
	  for (i = m_ptr->fy-1; i <= m_ptr->fy+1; i++)
	    for (j = m_ptr->fx-1; j <= m_ptr->fx+1; j++)
	      if (in_bounds(i, j))
		if (cave[i][j].cptr > 1)
		  k++;
	  /* can't call randint with a value of zero, increment counter
             to allow creature multiplication */
	  if (k == 0)
	    k++;
	  if (k < 4)
	    if (randint(k*MON_MULT_ADJ) == 1)
	      multiply_monster((int)m_ptr->fy, (int)m_ptr->fx,
			       (int)m_ptr->mptr, FALSE);
	}
  /* Creature is confused?  Chance it becomes un-confused  */
  move_test = FALSE;
  if (m_list[monptr].confused)
    {
      mm[0] = randint(9);
      mm[1] = randint(9);
      mm[2] = randint(9);
      mm[3] = randint(9);
      mm[4] = randint(9);
      /* don't move him if he is not supposed to move! */
      if (!(c_ptr->cmove & 0x00000001))
	movem = make_move(monptr, mm);
      if (randint(8) == 1)
	m_list[monptr].confused = FALSE;
      move_test = TRUE;
    }
  /* Creature may cast a spell                             */
  else if (c_ptr->spells != 0)
    movem = mon_cast_spell(monptr, &move_test);
  if (!move_test)
    {
      /* 75% random movement                                   */
      if ((randint(100) < 75) &&
	  (c_ptr->cmove & 0x00000020))
	{
	  mm[0] = randint(9);
	  mm[1] = randint(9);
	  mm[2] = randint(9);
	  mm[3] = randint(9);
	  mm[4] = randint(9);
	  movem = make_move(monptr, mm);
	}
      /* 40% random movement                                   */
      else if ((randint(100) < 40) &&
	       (c_ptr->cmove & 0x00000010))
	{
	  mm[0] = randint(9);
	  mm[1] = randint(9);
	  mm[2] = randint(9);
	  mm[3] = randint(9);
	  mm[4] = randint(9);
	  movem = make_move(monptr, mm);
	}
      /* 20% random movement                                   */
      else if ((randint(100) < 20) &&
	       (c_ptr->cmove & 0x00000008))
	{
	  mm[0] = randint(9);
	  mm[1] = randint(9);
	  mm[2] = randint(9);
	  mm[3] = randint(9);
	  mm[4] = randint(9);
	  movem = make_move(monptr, mm);
	}
      /* Normal movement                                       */
      else if (c_ptr->cmove & 0x00000002)
	{
	  if (randint(200) == 1)
	    {
	      mm[0] = randint(9);
	      mm[1] = randint(9);
	      mm[2] = randint(9);
	      mm[3] = randint(9);
	      mm[4] = randint(9);
	    }
	  else
	    get_moves(monptr, mm);
	  movem = make_move(monptr, mm);
	}
      /* Attack,  but don't move                                */
      else if (c_ptr->cmove & 0x00000001)
	if (m_list[monptr].cdis < 2)
	  {
	    get_moves(monptr, mm);
	    movem = make_move(monptr, mm);
	  }
    }
  return(movem);
}


/* Creatures movement and attacking are done from here	-RAK-	*/
creatures(attack)
int attack;
{
  register int i, j, k;
  int moldy, moldx;
  register monster_type *m_ptr;
  /* Main procedure for creatures				-RAK-	*/

  /* Process the monsters  */
  i = muptr;
  while ((i > 0) && (!moria_flag))
    {
      m_ptr = &m_list[i];
      m_ptr->cdis = distance(char_row, char_col,
			     (int)m_ptr->fy, (int)m_ptr->fx);
      if (attack)   /* Attack is argument passed to CREATURE*/
	{
	  k = movement_rate(m_ptr->cspeed);
	  if (k > 0)
	    for (j = 0; j < movement_rate(m_ptr->cspeed); j++)
	      {
		if ((m_ptr->cdis <= c_list[m_ptr->mptr].aaf) ||
		    (m_ptr->ml))
		  {
		    if (m_ptr->csleep > 0)
		      if (py.flags.aggravate)
			m_ptr->csleep = 0;
		      else if (py.flags.rest < 1)
			if (randint(10) > py.misc.stl)
			  m_ptr->csleep -= (75.0/m_ptr->cdis);
		    if (m_ptr->stunned > 0)
		      m_ptr->stunned--;
		    if ((m_ptr->csleep <= 0) && (m_ptr->stunned <= 0))
		      {
			moldy = m_ptr->fy;
			moldx = m_ptr->fx;
			if (mon_move(i))
			  if (m_ptr->ml)
			    {
			      m_ptr->ml = FALSE;
			      if (test_light(moldy, moldx))
				lite_spot(moldy, moldx);
			      else
				unlite_spot(moldy, moldx);
			    }
		      }
		  }
		update_mon(i);
	      }
	  else
	    update_mon(i);
	}
      else
	update_mon(i);
      i = m_list[i].nptr;
    }
  /* End processing monsters       */
}
