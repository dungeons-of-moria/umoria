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

byteint de_statt();
byteint in_statt();

/* global flags */
extern int moria_flag;        /* Next level when true  */
extern int search_flag;       /* Player is searching   */
extern int teleport_flag;     /* Handle teleport traps  */
extern int player_light;      /* Player carrying light */
extern int cave_flag;         /* used in get_panel */
extern int light_flag;        /* used in move_light */


/* Changes stats up or down for magic items		-RAK-	*/
change_stat_factor(stat, amount, factor)
register byteint *stat;
int amount, factor;
{
  register int i, j, k;

  j = amount * factor;
  if (amount < 0)
    k = -amount;
  else
    k = amount;
  for (i = 0; i < k; i++)
    if (j < 0)
      *stat = de_statt(*stat);
    else
      *stat = in_statt(*stat);
}


/* Changes speed of monsters relative to player		-RAK-	*/
/* Note: When the player is sped up or slowed down, I simply     */
/*       change the speed of all the monsters.  This greatly     */
/*       simplified the logic...                                 */
change_speed(num)
register int num;
{
  register int i;

  py.flags.speed += num;
  i = muptr;
  while (i > 0)
    {
      m_list[i].cspeed += num;
      i = m_list[i].nptr;
    }
}


/* Player bonuses					-RAK-	*/
/* When an item is worn or taken off, this re-adjusts the player */
/* bonuses.  Factor==1 : wear; Factor==-1 : removed                */
py_bonuses(tobj, factor)
treasure_type tobj;
int factor;
{
  register unsigned int item_flags;
  int old_dis_ac;
  register struct flags *p_ptr;
  register struct misc *m_ptr;
  register treasure_type *i_ptr;
  register int i;

  p_ptr = &py.flags;
  m_ptr = &py.misc;
  if (p_ptr->slow_digest)
    p_ptr->food_digested++;
  if (p_ptr->regenerate)
    p_ptr->food_digested -= 3;
  p_ptr->see_inv     = FALSE;
  p_ptr->teleport    = FALSE;
  p_ptr->free_act    = FALSE;
  p_ptr->slow_digest = FALSE;
  p_ptr->aggravate   = FALSE;
  p_ptr->sustain_str = FALSE;
  p_ptr->sustain_int = FALSE;
  p_ptr->sustain_wis = FALSE;
  p_ptr->sustain_con = FALSE;
  p_ptr->sustain_dex = FALSE;
  p_ptr->sustain_chr = FALSE;
  p_ptr->fire_resist = FALSE;
  p_ptr->acid_resist = FALSE;
  p_ptr->cold_resist = FALSE;
  p_ptr->regenerate  = FALSE;
  p_ptr->lght_resist = FALSE;
  p_ptr->ffall       = FALSE;

  if (0x00000001 & tobj.flags)
    {
      change_stat_factor(&py.stats.cstr, tobj.p1, factor);
      change_stat_factor(&py.stats.str, tobj.p1, factor);
      print_stat = (0x0001 | print_stat);
    }
  if (0x00000002 & tobj.flags)
    {
      change_stat_factor(&py.stats.cdex, tobj.p1, factor);
      change_stat_factor(&py.stats.dex, tobj.p1, factor);
      print_stat = (0x0002 | print_stat);
    }
  if (0x00000004 & tobj.flags)
    {
      change_stat_factor(&py.stats.ccon, tobj.p1, factor);
      change_stat_factor(&py.stats.con, tobj.p1, factor);
      print_stat = (0x0004 | print_stat);
    }
  if (0x00000008 & tobj.flags)
    {
      change_stat_factor(&py.stats.cint, tobj.p1, factor);
      change_stat_factor(&py.stats.intel, tobj.p1, factor);
      print_stat = (0x0008 | print_stat);
    }
  if (0x00000010 & tobj.flags)
    {
      change_stat_factor(&py.stats.cwis, tobj.p1, factor);
      change_stat_factor(&py.stats.wis, tobj.p1, factor);
      print_stat = (0x0010 | print_stat);
    }
  if (0x00000020 & tobj.flags)
    {
      change_stat_factor(&py.stats.cchr, tobj.p1, factor);
      change_stat_factor(&py.stats.chr, tobj.p1, factor);
      print_stat = (0x0020 | print_stat);
    }
  if (0x00000040 & tobj.flags)
    {
      m_ptr->srh += (tobj.p1 * factor);
      m_ptr->fos -= (tobj.p1 * factor);
    }
  if (0x00000100 & tobj.flags)
    m_ptr->stl += 2*factor;
  if (0x00001000 & tobj.flags)
    {
      i = tobj.p1*factor;
      change_speed(-i);
    }
  if (0x08000000 & tobj.flags)
    if (factor > 0)
      p_ptr->blind += 1000;
  if (0x10000000 & tobj.flags)
    if (factor > 0)
      p_ptr->afraid += 50;
  if (0x40000000 & tobj.flags)
    p_ptr->see_infra += (tobj.p1 * factor);

  old_dis_ac = m_ptr->dis_ac;
  m_ptr->ptohit  = tohit_adj();       /* Real To Hit   */
  m_ptr->ptodam  = todam_adj();       /* Real To Dam   */
  m_ptr->ptoac   = toac_adj();        /* Real To AC    */
  m_ptr->pac     = 0;               /* Real AC       */
  m_ptr->dis_th  = m_ptr->ptohit;  /* Display To Hit        */
  m_ptr->dis_td  = m_ptr->ptodam;  /* Display To Dam        */
  m_ptr->dis_ac  = 0;       /* Display To AC         */
  m_ptr->dis_tac = m_ptr->ptoac;   /* Display AC            */
  for (i = 22; i < INVEN_MAX-2; i++)
    {
      i_ptr = &inventory[i];
      if (i_ptr->tval != 0)
	{
	  if ((0x80000000 & i_ptr->flags) == 0)
	    {
	      m_ptr->pac += i_ptr->ac;
	      m_ptr->dis_ac += i_ptr->ac;
	    }
	  m_ptr->ptohit += i_ptr->tohit;
	  m_ptr->ptodam += i_ptr->todam;
	  m_ptr->ptoac  += i_ptr->toac;
	  if (index(i_ptr->name, '^') == 0)
	    {
	      m_ptr->dis_th  += i_ptr->tohit;
	      m_ptr->dis_td  += i_ptr->todam;
	      m_ptr->dis_tac += i_ptr->toac;
	    }
	}
    }
  m_ptr->dis_ac += m_ptr->dis_tac;

  /* Add in temporary spell increases	*/
  if (p_ptr->invuln > 0)
    {
      m_ptr->pac += 100;
      m_ptr->dis_ac += 100;
    }
  if (p_ptr->blessed > 0)
    {
      m_ptr->pac    += 2;
      m_ptr->dis_ac += 2;
    }
  if (p_ptr->detect_inv > 0)
    p_ptr->see_inv = TRUE;

  if (old_dis_ac != m_ptr->dis_ac)
    print_stat = (0x0040 | print_stat);

  item_flags = 0;
  for (i = 22; i < INVEN_MAX-2; i++)
    {
      i_ptr = &inventory[i];
      item_flags |= i_ptr->flags;
    }
  if (0x00000080 & item_flags)
    p_ptr->slow_digest = TRUE;
  if (0x00000200 & item_flags)
    p_ptr->aggravate = TRUE;
  if (0x00000400 & item_flags)
    p_ptr->teleport = TRUE;
  if (0x00000800 & item_flags)
    p_ptr->regenerate = TRUE;
  if (0x00080000 & item_flags)
    p_ptr->fire_resist = TRUE;
  if (0x00100000 & item_flags)
    p_ptr->acid_resist = TRUE;
  if (0x00200000 & item_flags)
    p_ptr->cold_resist = TRUE;
  if (0x00800000 & item_flags)
    p_ptr->free_act = TRUE;
  if (0x01000000 & item_flags)
    p_ptr->see_inv = TRUE;
  if (0x02000000 & item_flags)
    p_ptr->lght_resist = TRUE;
  if (0x04000000 & item_flags)
    p_ptr->ffall = TRUE;

  for (i = 22; i < INVEN_MAX-2; i++)
    {
      i_ptr = &inventory[i];
      if (0x00400000 & i_ptr->flags)
	switch(i_ptr->p1)
	  {
	  case 1: p_ptr->sustain_str = TRUE; break;
	  case 2: p_ptr->sustain_int = TRUE; break;
	  case 3: p_ptr->sustain_wis = TRUE; break;
	  case 4: p_ptr->sustain_con = TRUE; break;
	  case 5: p_ptr->sustain_dex = TRUE; break;
	  case 6: p_ptr->sustain_chr = TRUE; break;
	  default: break;
	  }
    }

  if (p_ptr->slow_digest)
    p_ptr->food_digested--;
  if (p_ptr->regenerate)
    p_ptr->food_digested += 3;
}


/* Returns a "*" for cursed items, a ")" for normal ones -RAK-	*/
/* NOTE: "*" returned only if item has been identified...        */
char cur_char1(item_val)
int item_val;
{
  register treasure_type *i_ptr;

  i_ptr = &inventory[item_val];
  if ((0x80000000 & i_ptr->flags) == 0)
    return(')');    /* Not cursed...                 */
  else if (index(i_ptr->name, '^') != 0)
    return(')');    /* Cursed, but not identified    */
  else
    return('*');   /* Cursed and identified...      */
}


/* Returns a "*" for cursed items, a ")" for normal ones -RAK-	*/
char cur_char2(item_val)
int item_val;
{
  register treasure_type *i_ptr;

  i_ptr = &inventory[item_val];
  if ((0x80000000 & i_ptr->flags) == 0)
    return(')');    /* Not cursed... */
  else
    return('*');   /* Cursed...     */
}


/* inventory functions, define some global variables here */
/* scr_state == 0 : normal screen (i.e. map of dungeon)
		     or partial inventory list, (calling function sets redraw)
   scr_state == 1 : inventory is displayed on the screen
   scr_state == 2 : equipment list is displayed on the screen */
int scr_state;

/* Displays inventory items from r1 to r2	-RAK-	*/
show_inven(r1, r2)
register int r1, r2;
{
  register int i;
  vtype tmp_val, out_val;

  if (r1 >= 0)                       /* R1 == 0 dummy call     */
    {
      for (i = r1; i <= r2; i++)             /* Print the items       */
	{
	  objdes(tmp_val, i, TRUE);
	  (void) sprintf(out_val, "%c%c %s", i+97, cur_char1(i), tmp_val);
	  prt(out_val, i+1, 0);
	}
      if (r2 < 22)
	prt("", r2+2, 0); /* Clear line after      */
      scr_state = 1;                   /* Set state to 1        */
    }
}

/* Displays equipment items from r1 to end	-RAK-	*/
show_equip(r1)
register int r1;
{
  register int i, j;
  vtype prt1, prt2, out_val;
  register treasure_type *i_ptr;

  if (r1 >= equip_ctr)       /* Last item gone                */
    prt("", equip_ctr+2, 0);  /* clear the line */
  else if (r1 >= 0)          /* R1 == 0 dummy call             */
    {
      j = 0;
      for (i = 22; i < INVEN_MAX; i++) /* Range of equipment        */
	{
	  i_ptr = &inventory[i];
	  if (i_ptr->tval != 0)
	    {
	      if (j >= r1) /* Display only given range    */
		{
		  switch(i)          /* Get position          */
		    {
		    case 22:
		      (void) strcpy(prt1, " You are wielding   : "); break;
		    case 23:
		      (void) strcpy(prt1, " Worn on head       : "); break;
		    case 24:
		      (void) strcpy(prt1, " Worn around neck   : "); break;
		    case 25:
		      (void) strcpy(prt1, " Worn on body       : "); break;
		    case 26:
		      (void) strcpy(prt1, " Worn on arm        : "); break;
		    case 27:
		      (void) strcpy(prt1, " Worn on hands      : "); break;
		    case 28:
		      (void) strcpy(prt1, " Worn on right hand : "); break;
		    case 29:
		      (void) strcpy(prt1, " Worn on left hand  : "); break;
		    case 30:
		      (void) strcpy(prt1, " Worn on feet       : "); break;
		    case 31:
		      (void) strcpy(prt1, " Worn about body    : "); break;
		    case 32:
		      (void) strcpy(prt1, " Light source       : "); break;
		    case 33:
		      (void) strcpy(prt1, " Secondary weapon   : "); break;
		    default:
		      (void) strcpy(prt1, " Unknown value      : "); break;
		    }
		  objdes(prt2, i, TRUE);
		  (void) sprintf(out_val,
				 "%c%c%s%s", j+97, cur_char2(i), prt1, prt2);
		  prt(out_val, j+2, 0);
		}
	      j++;
	    }
	}
      prt("", j+2, 0);   /* Clear last line       */
      scr_state = 2;   /* Set state of screen   */
    }
}

/* Remove item from equipment list		-RAK-	*/
int remove(item_val)
int item_val;
{
  register int i, j, typ;
  vtype out_val, prt1, prt2;
  int flag;
  register treasure_type *i_ptr;

  i = 0;
  flag = FALSE;
  typ  = inventory[item_val].tval;
  do
    {
      i_ptr = &inventory[i];
      if (typ > i_ptr->tval)
	{
	  for (j = inven_ctr-1; j >= i; j--)
	    inventory[j+1] = inventory[j];
	  inventory[i]  = inventory[item_val];
	  inven_ctr++;
	  equip_ctr--;
	  flag = TRUE;
	}
      i++;
    }
  while (!flag);
  i--;
  switch(typ)
    {
    case 10: case 11: case 12: case 20: case 21: case 22: case 23: case 25 :
      (void) strcpy(prt1, "Was wielding ");
      break;
    case 15:
      (void) strcpy(prt1, "Light source was ");
      break;
    default:
      (void) strcpy(prt1, "Was wearing ");
      break;
    }
  objdes(prt2, i, TRUE);
  (void) sprintf(out_val, "%s%s (%c)", prt1, prt2, i+97);
  msg_print(out_val);
  inventory[item_val] = blank_treasure;
  if (item_val != INVEN_AUX)      /* For secondary weapon  */
    py_bonuses(inventory[i], -1);
  return(i);
}


/* Unwear routine,  remove a piece of equipment	-RAK-	*/
unwear()
{
  register int i, j;
  int exit_flag, test_flag, com_val;
  char command;
  vtype out_val;

  if (scr_state == 1)
    {
      clear_screen(0, 0);
      show_equip(0);
    }
  exit_flag = FALSE;
  do
    {
      (void) sprintf(out_val,
	      "(a-%c, * for equipment list, ESC to exit) Take off which one ?",
	      equip_ctr+96);
      test_flag = FALSE;
      msg_print(out_val);
      do
	{
	  inkey(&command);
	  com_val = (command);
	  switch(com_val)
	    {
	    case 0: case 27:
	      test_flag = TRUE;
	      exit_flag = TRUE;
	      break;
	    case 42:
	      clear_screen(1, 0);
	      show_equip(0);
	      break;
	    default:
	      com_val -= 97;
	      if ((com_val >= 0) &&
		  (com_val < equip_ctr))
		test_flag = TRUE;
	      break;
	    }
	}
      while (!test_flag);
      if (!exit_flag)
	{
	  reset_flag = FALSE;    /* Player turn   */
	  i = -1;
	  j = 21;
	  do
	    {
	      j++;
	      if (inventory[j].tval != 0)
		i++;
	    }
	  while (i != com_val);
	  if (0x80000000 & inventory[j].flags)
	    {
	      msg_print("Hmmm, it seems to be cursed...");
	      com_val = 0;
	    }
	  else
	    (void) remove(j);
	}
      if (scr_state == 0)
	exit_flag = TRUE;
      else if (equip_ctr == 0)
	exit_flag = TRUE;
      else if (inven_ctr > 21)
	{
	  exit_flag = TRUE;
	  show_equip(0);
	}
      else if (!exit_flag)
	show_equip(0);
    }
  while (!exit_flag);
  if (scr_state != 0)
    if (equip_ctr == 0)
      clear_screen(0, 0);
    else
      prt("You are currently using -", 0, 0);
}


/* Wear routine, wear or wield an item		-RAK-	*/
wear()
{
  register int i, j, k;
  int com_val, tmp;
  vtype out_val, prt1, prt2;
  treasure_type unwear_obj;
  int exit_flag, test_flag;
  char command;
  register treasure_type *i_ptr;

  if (scr_state == 2)
    {
      clear_screen(0, 0);
      show_inven(0, inven_ctr-1);
    }
  exit_flag = FALSE;
  do
    {
      (void) sprintf(out_val,
	     "(a-%c, * for equipment list, ESC to exit) Wear/Wield which one?",
	      inven_ctr+96);
      test_flag = FALSE;
      msg_print(out_val);
      do
	{
	  inkey(&command);
	  com_val = (command);
	  switch(com_val)
	    {
	    case 0: case 27:
	      test_flag = TRUE;
	      exit_flag = TRUE;
	      break;
	    case 42:
	      clear_screen(1, 0);
	      show_inven(0, inven_ctr-1);
	      break;
	    default:
	      com_val -= 97;
	      if ((com_val >= 0) &&
		  (com_val < inven_ctr))
		test_flag = TRUE;
	      break;
	    }
	}
      while (!test_flag);
      if (!exit_flag)     /* Main logic for wearing        */
	{
	  reset_flag = FALSE;    /* Player turn   */
	  test_flag = TRUE;
	  switch(inventory[com_val].tval) /* Slot for equipment    */
	    {
	    case 10: i = 22; break;
	    case 11: i = 22; break;
	    case 12: i = 22; break;
	    case 15: i = 32; break;
	    case 20: i = 22; break;
	    case 21: i = 22; break;
	    case 22: i = 22; break;
	    case 23: i = 22; break;
	    case 25: i = 22; break;
	    case 30: i = 30; break;
	    case 31: i = 27; break;
	    case 32: i = 31; break;
	    case 33: i = 23; break;
	    case 34: i = 26; break;
	    case 35: i = 25; break;
	    case 36: i = 25; break;
	    case 40: i = 24; break;
	    case 45:
	      if (inventory[INVEN_RIGHT].tval == 0)       /* Rings */
	        i = INVEN_RIGHT;
	      else
	        i = INVEN_LEFT;
	      break;
            default:
	      msg_print("I don't see how you can use that.");
	      test_flag = FALSE;
	      com_val = 0;
	      break;
	    }
	  if (test_flag)
	    if (inventory[i].tval != 0)
	      {
		if (0x80000000 & inventory[i].flags)
		  {
		    objdes(prt1, i, FALSE);
		    (void) sprintf(out_val, "The %s you are ", prt1);
		    switch(i)
		      {
		      case 23: (void) strcat(out_val, "wielding "); break;
		      default: (void) strcat(out_val, "wearing "); break;
		      }
		    msg_print(strcat(out_val, "appears to be cursed."));
		    test_flag = FALSE;
		    com_val = 0;
		  }
		else if (inven_ctr > 21)
		  if (inventory[com_val].number > 1)
		    if (inventory[com_val].subval < 512)
		      {
			msg_print("You will have to drop something first.");
			test_flag = FALSE;
			com_val = 0;
		      }
	      }
	  if (test_flag)
	    {
	      /* save old item */
	      unwear_obj = inventory[i];
	      /* now wear/wield new object */
	      inventory[i] = inventory[com_val];
	      i_ptr = &inventory[i];
	      /* Fix for torches       */
	      if ((i_ptr->subval > 255) && (i_ptr->subval < 512))
		{
		  i_ptr->number = 1;
		  i_ptr->subval -= 255;
		}
	      /* Fix for weight        */
	      inven_weight += i_ptr->weight*i_ptr->number;
	      inven_destroy(com_val);     /* Subtracts weight      */
	      equip_ctr++;
	      /* subtract bonuses for old item before add bonuses for new */
	      /* must do this after inven_destroy, otherwise inventory
		 may increase to 23 items thus destroying INVEN_WIELD */
	      if (unwear_obj.tval != 0)
		{
		  inventory[INVEN_MAX] = unwear_obj;
		  /* decrements equip_ctr, and calls py_bonuses with -1 */
		  tmp = remove(INVEN_MAX);
		  if (tmp < com_val)
		    com_val = tmp;
		}
	      py_bonuses(inventory[i], 1);
	      switch(i)
		{
		case 22: (void) strcpy(prt1, "You are wielding "); break;
		case 32: (void) strcpy(prt1, "Your light source is "); break;
		default: (void) strcpy(prt1, "You are wearing "); break;
		}
	      objdes(prt2, i, TRUE);
	      j = -1;
	      k = 21;
	      do      /* Get the right letter of equipment     */
		{
		  k++;
		  if (inventory[k].tval != 0)
		    j++;
		}
	      while (k != i);
	      (void) sprintf(out_val, "%s%s (%c%c",
		      prt1, prt2, j+97, cur_char2(i));
	      msg_print(out_val);
	      if ((i == 22) && (py.stats.cstr*15 < i_ptr->weight))
		msg_print("You have trouble wielding such a heavy weapon.");
	    }
	}
      if (scr_state == 0)
	exit_flag = TRUE;
      else if (inven_ctr == 0)
	exit_flag = TRUE;
      else if (!exit_flag)
	show_inven(com_val, inven_ctr-1);
    }
  while (!exit_flag);
  if (scr_state != 0)  prt("You are currently carrying -", 0, 0);
}

/* Switch primary and secondary weapons		-RAK-	*/
switch_weapon()
{
  vtype prt1, prt2;
  treasure_type tmp_obj;

  if (0x80000000 & inventory[INVEN_WIELD].flags)
    {
      objdes(prt1, INVEN_WIELD, FALSE);
      (void) sprintf(prt2,
		     "The %s you are wielding appears to be cursed.", prt1);
      msg_print(prt2);
    }
  else
    {
      /* Switch weapons        */
      reset_flag = FALSE;
      tmp_obj = inventory[INVEN_AUX];
      inventory[INVEN_AUX] = inventory[INVEN_WIELD];
      inventory[INVEN_WIELD] = tmp_obj;
      py_bonuses(inventory[INVEN_AUX], -1);     /* Subtract bonuses      */
      py_bonuses(inventory[INVEN_WIELD], 1);      /* Add bonuses           */
      if (inventory[INVEN_WIELD].tval != 0)
	{
	  (void) strcpy(prt1, "Primary weapon   : ");
	  objdes(prt2, INVEN_WIELD, TRUE);
	  msg_print(strcat(prt1, prt2));
	  if (py.stats.cstr * 15 < inventory[INVEN_WIELD].weight)
	    msg_print("You have trouble wielding such a heavy weapon.");
	}
      if (inventory[INVEN_AUX].tval != 0)
	{
	  (void) strcpy(prt1, "Secondary weapon : ");
	  objdes(prt2, INVEN_AUX, TRUE);
	  msg_print(strcat(prt1, prt2));
	}
    }
  if (scr_state != 0)
    {
      /* make sure player sees last message */
      msg_print(" ");
      clear_screen(0, 0);
      prt("You are currently using -", 0, 0);
      show_equip(0);
    }
}


/* Comprehensive function block to handle all inventory	-RAK-	*/
/* and equipment routines.  Five kinds of calls can take place.  */
/* Note that "?" is a special call for other routines to display */
/* only a portion of the inventory, and take no other action.    */
int inven_command(command, r1, r2)
char command;
int r1, r2;
{
  register int com_val, inven;
  register int exit_flag, test_flag;

  /* Main logic for INVEN_COMMAND			-RAK-	*/
  inven = FALSE;
  exit_flag = FALSE;
  scr_state = 0;
  do
    {
      switch(command)
	{
	case 'i':          /* Inventory     */
	  if (inven_ctr == 0)
	    msg_print("You are not carrying anything.");
	  else if (scr_state != 1)
	    {     /* Sets scr_state to 1           */
	      clear_screen(0, 0);
	      prt("You are currently carrying -", 0, 0);
	      show_inven(0, inven_ctr-1);
	    }
	  break;
	case 'e':         /* Equipment     */
	  if (equip_ctr == 0)
	    msg_print("You are not using any equipment.");
	  else if (scr_state != 2)
	    {     /* Sets scr_state to 2           */
	      clear_screen(0, 0);
	      prt("You are currently using -", 0, 0);
	      show_equip(0);
	    }
	  break;
	case 't':         /* Take off      */
	  if (equip_ctr == 0)
	    msg_print("You are not using any equipment.");
	  else if (inven_ctr > 21)
	    msg_print("You will have to drop something first.");
	  else
	    unwear();   /* May set scr_state to 2        */
	  break;
	case 'w':         /* Wear/wield    */
	  if (inven_ctr == 0)
	    msg_print("You are not carrying anything.");
	  else
	    wear();     /* May set scr_state to 1        */
	  break;
	case 'x':
	  if (inventory[INVEN_WIELD].tval != 0)
	    switch_weapon();
	  else if (inventory[INVEN_AUX].tval != 0)
	    switch_weapon();
	  else
	    msg_print("But you are wielding no weapons.");
	  break;
	  /* Special function for other routines                   */
	case '?':  /* Displays part inven, returns  */
	  show_inven(r1, r2);
	  scr_state = 0;     /* Clear screen state    */
	  break;
	  /* Nonsense command                                      */
	default:
	  break;
	}
      if (scr_state > 0)
	{
	  prt(
    "<e>quip, <i>inven, <t>ake-off, <w>ear/wield, e<x>change, or ESC to exit.",
      23, 1);
	  test_flag = FALSE;
	  do
	    {
	      inkey(&command);
	      com_val = (command);
	      switch(com_val)
		{
		case 0: case 27: case 32:
		  /* Exit from module      */
		  exit_flag = TRUE;
		  test_flag = TRUE;
		  break;
		default:
		  switch(command)      /* Module commands       */
		    {
		    case 'e': test_flag = TRUE; break;
		    case 'i': test_flag = TRUE; break;
		    case 't': test_flag = TRUE; break;
		    case 'w': test_flag = TRUE; break;
		    case 'x': test_flag = TRUE; break;
		    case '?': break;      /* Trap special feature  */
		    default:  break; /* Nonsense command      */
		    }
		}
	    }
	  while (!test_flag);
	  prt("", 23, 0);   /* clear the line containing command list */
	}
      else
	exit_flag = TRUE;
    }
  while (!exit_flag);
  if (scr_state > 0)          /* If true, must redraw screen   */
    inven = TRUE;
  return(inven);
}


/* Get the ID of an item and return the CTR value of it	-RAK-	*/
int get_item(com_val, pmt, redraw, i, j)
int *com_val;
char *pmt;
int *redraw;
int i, j;
{
  char command;
  vtype out_val;
  register int test_flag, item;

  item = FALSE;
  *com_val = 0;
  if (inven_ctr > 0)
    {
      (void) sprintf(out_val,
		     "(Items %c-%c, * for inventory list, ESC to exit) %s",
	      i+97, j+97, pmt);
      test_flag = FALSE;
      prt(out_val, 0, 0);
      do
	{
	  inkey(&command);
	  *com_val = (command);
	  switch(*com_val)
	    {
	    case 0: case 27:
	      test_flag = TRUE;
	      reset_flag = TRUE;
	      break;
	    case 42:
	      clear_screen(1, 0);
	      (void) inven_command('?', i, j);
	      *redraw = TRUE;
	      break;
	    default:
	      *com_val -= 97;
	      if ((*com_val >= i) &&
		  (*com_val <= j))
		{
		  test_flag = TRUE;
		  item = TRUE;
		}
	      break;
	    }
	}
      while (!test_flag);
      erase_line(MSG_LINE, 0);
    }
  else
    msg_print("You are not carrying anything.");
  return(item);
}

/* I may have written the town level code, but I'm not exactly   */
/* proud of it.  Adding the stores required some real slucky     */
/* hooks which I have not had time to re-think.          -RAK-   */


/* Calculates current boundaries				-RAK-	*/
panel_bounds()
{
  panel_row_min = panel_row*(SCREEN_HEIGHT/2);
  panel_row_max = panel_row_min + SCREEN_HEIGHT - 1;
  panel_row_prt = panel_row_min - 1;
  panel_col_min = panel_col*(SCREEN_WIDTH/2);
  panel_col_max = panel_col_min + SCREEN_WIDTH - 1;
/* the value 13 puts one blank space between the stats and the map, leaving
     the last column empty
   the value 14 puts two blank spaces between the stats and the map, and
     ends up printing in the last column
   I think 14 gives a better display, but some curses wreak havoc when try to
   print characters in the last column, hence the BUGGY_CURSES ifdef */
#ifdef BUGGY_CURSES
  panel_col_prt = panel_col_min - 13;
#else
  panel_col_prt = panel_col_min - 14;
#endif
}


/* Given an row (y) and col (x), this routine detects  -RAK-	*/
/* when a move off the screen has occurred and figures new borders*/
int get_panel(y, x)
int y, x;
{
  register int prow, pcol;
  register int panel;

  prow = panel_row;
  pcol = panel_col;
  if ((y < panel_row_min + 2) || (y > panel_row_max - 2))
    {
      prow = ((y - 2)/(SCREEN_HEIGHT/2));
      if (prow > max_panel_rows)
	prow = max_panel_rows;
    }
  if ((x < panel_col_min + 3) || (x > panel_col_max - 3))
    {
      pcol = ((x - 3)/(SCREEN_WIDTH/2));
      if (pcol > max_panel_cols)
	pcol = max_panel_cols;
    }
  if ((prow != panel_row) || (pcol != panel_col) || (!cave_flag))
    {
      panel_row = prow;
      panel_col = pcol;
      panel_bounds();
      panel = TRUE;
      cave_flag = TRUE;
      /* stop movement if any */
      if (find_flag)
	find_flag = FALSE;  /* no need to call move_light () */
    }
  else
    panel = FALSE;
  return(panel);
}


/* Tests a given point to see if it is within the screen -RAK-	*/
/* boundaries.                                                    */
int panel_contains(y, x)
int y, x;
{
  register int panel;

  if ((y >= panel_row_min) && (y <= panel_row_max))
    if ((x >= panel_col_min) && (x <= panel_col_max))
      panel = TRUE;
    else
      panel = FALSE;
  else
    panel = FALSE;
  return(panel);
}


/* Returns true if player has no light			-RAK-	*/
int no_light()
{
  register int light;
  register cave_type *c_ptr;

  light = FALSE;
  c_ptr = &cave[char_row][char_col];
    if (!c_ptr->tl)
      if (!c_ptr->pl)
	light = TRUE;
  return(light);
}


/* map rogue_like direction commands into numbers */
int map_roguedir(comval)
register int *comval;
{
  switch(*comval)
    {
    case 'h':
      *comval = '4';
      return(4);
    case 'y':
      *comval = '7';
      return(7);
    case 'k':
      *comval = '8';
      return(8);
    case 'u':
      *comval = '9';
      return(9);
    case 'l':
      *comval = '6';
      return(6);
    case 'n':
      *comval = '3';
      return(3);
    case 'j':
      *comval = '2';
      return(2);
    case 'b':
      *comval = '1';
      return(1);
    }
  return(*comval - 48);
}



/* Prompts for a direction				-RAK-	*/
int get_dir(prompt, dir, com_val, y, x)
char *prompt;
int *dir, *com_val, *y, *x;
{
  int flag;
  char command;
  int res;

  flag = FALSE;
  do
    {
      if (get_com(prompt, &command))
	{
	  *com_val = (command);
	  if (key_bindings == ORIGINAL)
	    *dir = *com_val - 48;
	  else   /* rogue_like bindings */
	    *dir = map_roguedir(com_val);
	  /* Note that "5" is not a valid direction        */
	  if ((*dir >= 1) && (*dir <= 9) && (*dir != 5))
	    {
	      (void) move(*dir, y, x);
	      flag = TRUE;
	      res = TRUE;
	    }
	}
      else
	{
	  reset_flag = TRUE;
	  res = FALSE;
	  flag = TRUE;
	}
    }
  while (!flag);
  return(res);
}


/* Moves creature record from one space to another	-RAK-	*/
move_rec(y1, x1, y2, x2)
register int y1, x1, y2, x2;
{
  if ((y1 != y2) || (x1 != x2))
    {
      cave[y2][x2].cptr = cave[y1][x1].cptr;
      cave[y1][x1].cptr = 0;
      }
}


find_light(y1, x1, y2, x2)
int y1, x1, y2, x2;
{
  register int i, j, k, l;

  for (i = y1; i <= y2; i++)
    for (j = x1; j <= x2; j++)
      if ((cave[i][j].fval == 1) || (cave[i][j].fval == 2))
	{
	  for (k = i-1; k <= i+1; k++)
	    for (l = j-1; l <= j+1; l++)
	      cave[k][l].pl = TRUE;
	  cave[i][j].fval = 2;
	}
}


/* Room is lit, make it appear				-RAK-	*/
light_room(y, x)
int y, x;
{
  register cave_type *c_ptr;
  register int i, j;
  int tmp1, tmp2;
  int start_row, end_row;
  register int start_col, end_col;
  int ypos, xpos;
  vtype floor_str, tmp_str;

  tmp1 = (SCREEN_HEIGHT/2);
  tmp2 = (SCREEN_WIDTH /2);
  start_row = (y/tmp1)*tmp1;
  start_col = (x/tmp2)*tmp2;
  end_row = start_row + tmp1 - 1;
  end_col = start_col + tmp2 - 1;
  find_light(start_row, start_col, end_row, end_col);
  for (i = start_row; i <= end_row; i++)
    {
      floor_str[0] = '\0';
      ypos = i;
      for (j = start_col; j <= end_col; j++)
	{
	  c_ptr = &cave[i][j];
	  if ((c_ptr->pl) || (c_ptr->fm))
	    {
	      if (strlen(floor_str) == 0)
		xpos = j;
	      loc_symbol(i, j, tmp_str);
	      tmp_str[1] = '\0';
	      (void) strcat(floor_str, tmp_str);
	    }
	  else
	    if (strlen(floor_str) > 0)
	      {
		print(floor_str, ypos, xpos);
		floor_str[0] = '\0';
	      }
	}
      if (strlen(floor_str) > 0)
	print(floor_str, ypos, xpos);
    }
}


/* Lights up given location				-RAK-	*/
lite_spot(y, x)
register int y, x;
{
  char temp[2];

  temp[1] = '\0';
  if (panel_contains(y, x))
    {
      loc_symbol(y, x, temp);
      print(temp, y, x);
      }
}


/* Blanks out given location				-RAK-	*/
unlite_spot(y, x)
register int y, x;
{
  if (panel_contains(y, x))
    print(" ", y, x);
}


/* Minimum of a maximum				-RAK-	*/
int minmax(x, y, z)
register int x, y, z;
{
  register int max;

  max = ( y > x ? y : x) + 1;
  return((max > z ? z : max));
}

/* Maximum of a minimum				-RAK-	*/
int maxmin(x, y, z)
register int x, y, z;
{
  register int min;

  min = (x > y ? y : x) - 1;
  return((min > z ? min : z));
}


/* Given two sets of points,  draw the block		*/
draw_block(y1, x1, y2, x2)
int y1, x1, y2, x2;
{
  register cave_type *c_ptr;
  register int i, j;
  int xpos;
  int topp, bott, left, righ;
  int new_topp, new_bott, new_left, new_righ;
  vtype floor_str, save_str;
  char tmp_char[2];
  int flag;

  tmp_char[1] = '\0';    /* This is supposed to be a one */
  /* From uppermost to bottom most lines player was on...  */
  /* Points are guaranteed to be on the screen (I hope...) */
  topp = maxmin(y1, y2, panel_row_min);
  bott = minmax(y1, y2, panel_row_max);
  left = maxmin(x1, x2, panel_col_min);
  righ = minmax(x1, x2, panel_col_max);
  new_topp = y2 - 1;     /* Margins for new things to appear*/
  new_bott = y2 + 1;
  new_left = x2 - 1;
  new_righ = x2 + 1;
  for (i = topp; i <= bott; i++)
    {
      floor_str[0] = '\0';    /* Null out print string         */
      xpos      = -1;
      save_str[0] = '\0';
      for (j = left; j <= righ; j++)   /* Leftmost to rightmost do*/
	{
	  c_ptr = &cave[i][j];
	  if ((c_ptr->pl) || (c_ptr->fm))
	    if (((i==y1) && (j==x1)) || ((i==y2) && (j==x2)))
	      flag = TRUE;
	    else
	      flag = FALSE;
	  else
	    {
	      flag = TRUE;
	      if (((i >= new_topp) && (i <= new_bott)) &&
		  ((j >= new_left) && (j <= new_righ)))
		{
		  if (c_ptr->tl)
		    if (((c_ptr->fval >= 10) && (c_ptr->fval <= 12)) ||
			(c_ptr->fval == 15))
		      c_ptr->pl = TRUE;
		    else if (c_ptr->tptr != 0)
		      if ((t_list[c_ptr->tptr].tval >= 102) &&
			  (t_list[c_ptr->tptr].tval <= 110) &&
			  (t_list[c_ptr->tptr].tval != 106))
			if (!c_ptr->fm)
			  c_ptr->fm = TRUE;
		}
	    }
	  if ((c_ptr->pl) || (c_ptr->tl) || (c_ptr->fm))
	    loc_symbol(i, j, tmp_char);
	  else
	    tmp_char[0] = ' ';
	  if (py.flags.image > 0)
	    if (randint(12) == 1)
	      tmp_char[0] = (randint(95) + 31);
	  if (flag)
	    {
	      if (xpos < 0)  xpos = j;
	      if (strlen(save_str) > 0)
		{
		  (void) strcat(floor_str, save_str);
		  save_str[0] = '\0';
		}
	      (void) strcat(floor_str, tmp_char);
	    }
	  else if (xpos >= 0)
	    (void) strcat(save_str, tmp_char);
	}
      if (xpos >= 0)
	{
	  print(floor_str, i, xpos);
	}
    }
}


/* Normal movement					*/
sub1_move_light(y1, x1, y2, x2)
register int x1, x2;
int y1, y2;
{
  register int i, j;

  light_flag = TRUE;
  for (i = y1-1; i <= y1+1; i++)       /* Turn off lamp light   */
    for (j = x1-1; j <= x1+1; j++)
      cave[i][j].tl = FALSE;
  for (i = y2-1; i <= y2+1; i++)
    for (j = x2-1; j <= x2+1; j++)
      cave[i][j].tl = TRUE;
  draw_block(y1, x1, y2, x2);        /* Redraw area           */
}

/* When FIND_FLAG,  light only permanent features 	*/
sub2_move_light(y1, x1, y2, x2)
int y1, x1, y2, x2;
{
  register int i, j;
  int xpos;
  vtype floor_str, save_str;
  char tmp_char[2];
  int flag;
  register cave_type *c_ptr;

  tmp_char[1] = '\0';
  if (light_flag)
    {
      for (i = y1-1; i <= y1+1; i++)
	for (j = x1-1; j <= x1+1; j++)
	  cave[i][j].tl = FALSE;
      draw_block(y1, x1, y1, x1);
      light_flag = FALSE;
    }
  for (i = y2-1; i <= y2+1; i++)
    {
      floor_str[0] = '\0';
      save_str[0] = '\0';
      xpos = 0;
      for (j = x2-1; j <= x2+1; j++)
	{
	  c_ptr = &cave[i][j];
	  flag = FALSE;
	  if ((!c_ptr->fm) && (!c_ptr->pl))
	    {
	      tmp_char[0] = ' ';
	      if (player_light)
		if (((c_ptr->fval >= 10) && (c_ptr->fval <= 12)) ||
		    (c_ptr->fval == 15))
		  {
		    c_ptr->pl = TRUE; /* Turn on perm light    */
		    loc_symbol(i, j, tmp_char);
		    flag = TRUE;
		  }
		else
		  if (c_ptr->tptr != 0)
		      if ((t_list[c_ptr->tptr].tval >= 102) &&
			  (t_list[c_ptr->tptr].tval <= 110) &&
			  (t_list[c_ptr->tptr].tval != 106))
		      {
			c_ptr->fm = TRUE;     /* Turn on field marker  */
			loc_symbol(i, j, tmp_char);
			flag = TRUE;
		      }
	    }
	  else
	    loc_symbol(i, j, tmp_char);
	  if (flag)
	    {
	      if (xpos == 0)  xpos = j;
	      if (strlen(save_str) > 0)
		{
		  (void) strcat(floor_str, save_str);
		  save_str[0] = '\0';
		}
	      (void) strcat(floor_str, tmp_char);
	    }
	  else if (xpos > 0)
	    (void) strcat(save_str, tmp_char);
	}
      if (xpos > 0)
	{
	  j = i;
	  print(floor_str, j, xpos);
	}
    }
}

/* When blinded,  move only the player symbol...		*/
sub3_move_light(y1, x1, y2, x2)
register int x1, y1;
int x2, y2;
{
  register int i, j;

  if (light_flag)
    {
      for (i = y1-1; i <= y1+1; i++)
	for (j = x1-1; j <= x1+1; j++)
	  cave[i][j].tl = FALSE;
      light_flag = FALSE;
    }
  print(" ", y1, x1);
  print("@", y2, x2);
}

/* With no light,  movement becomes involved...		*/
sub4_move_light(y1, x1, y2, x2)
register y1, x1;
int y2, x2;
{
  register int i, j;

  light_flag = TRUE;
  if (cave[y1][x1].tl)
    {
      for (i = y1-1; i <= y1+1; i++)
	for (j = x1-1; j <= x1+1; j++)
	  {
	    cave[i][j].tl = FALSE;
	    if (test_light(i, j))
	      lite_spot(i, j);
	    else
	      unlite_spot(i, j);
	  }
    }
  else if (test_light(y1, x1))
    lite_spot(y1, x1);
  else
    unlite_spot(y1, x1);
  print("@", y2, x2);
}

/* Package for moving the character's light about the screen     */
/* Three cases : Normal,  Finding, and Blind              -RAK-   */
move_light(y1, x1, y2, x2)
int y1, x1, y2, x2;
{
  if (py.flags.blind > 0)
    sub3_move_light(y1, x1, y2, x2);
  else if (find_flag)
    sub2_move_light(y1, x1, y2, x2);
  else if (!player_light)
    sub4_move_light(y1, x1, y2, x2);
  else
    sub1_move_light(y1, x1, y2, x2);
}


/* Returns random co-ordinates				-RAK-	*/
new_spot(y, x)
register int *y, *x;
{
  do
    {
      *y = randint(cur_height) - 1;
      *x = randint(cur_width) - 1;
    }
  while ((!cave[*y][*x].fopen) || (cave[*y][*x].cptr != 0) ||
	   (cave[*y][*x].tptr != 0));
}


/* Search Mode enhancement				-RAK-	*/
search_on()
{
  search_flag = TRUE;
  change_speed(1);
  py.flags.status |= 0x00000100;
  prt_search();
  py.flags.food_digested++;
}

search_off()
{
  search_flag = FALSE;
  find_flag = FALSE;
  move_light (char_row, char_col, char_row, char_col);
  change_speed(-1);
  py.flags.status &= 0xFFFFFEFF;
  prt_search();
  py.flags.food_digested--;
}


/* Resting allows a player to safely restore his hp	-RAK-	*/
rest()
{
  int rest_num;
  vtype rest_str;

  prt("Rest for how long? ", 0, 0);
  rest_num = 0;
  if (get_string(rest_str, 0, 19, 10))
    (void) sscanf(rest_str, "%d", &rest_num);
  if (rest_num > 0)
    {
      if (search_flag)
	search_off();
      py.flags.rest = rest_num;
      py.flags.status |= 0x00000200;
      prt_rest();
      py.flags.food_digested--;
      msg_print("Press ^C to wake up...");
      put_qio();
    }
  else
    {
      erase_line(MSG_LINE, 0);
      reset_flag = TRUE;
    }
}

rest_off()
{
  py.flags.rest = 0;
  py.flags.status &= 0xFFFFFDFF;
  erase_line(0, 0);
  prt_rest();
  py.flags.food_digested++;
}


/* Attacker's level and plusses,  defender's AC		-RAK-	*/
int test_hit(bth, level, pth, ac)
int bth, level, pth, ac;
{
  register int i;
  int test;

  if (search_flag)
    search_off();
  if (py.flags.rest > 0)
    rest_off();
  i = bth + level*BTH_LEV_ADJ + pth*BTH_PLUS_ADJ;
  /* pth could be less than 0 if player wielding weapon too heavy for him */
  if ((i > 0) && (randint(i) > ac))               /* Normal hit            */
    test = TRUE;
  else if (randint(20) == 1)           /* Always hit 1/20       */
    test = TRUE;
  else                                    /* Missed                */
    {
      if (i <= 0)
	msg_print("You have trouble swinging such a heavy weapon.");
      test = FALSE;
    }
  return(test);
}


/* Decreases players hit points and sets death flag if necessary*/
/*                                                       -RAK-   */
take_hit(damage, hit_from)
int damage;
char *hit_from;
{
  if (py.flags.invuln > 0)  damage = 0;
  py.misc.chp -= (double)damage;
  if (search_flag)  search_off();
  if (py.flags.rest > 0)  rest_off();
  if (find_flag)
    {
      find_flag = FALSE;
      move_light (char_row, char_col, char_row, char_col);
    }
  flush();
  if (py.misc.chp <= -1)
    {
      if (!death)
	{             /* Hee,  hee... Ain't I mean?     */
	  death = TRUE;
	  (void) strcpy(died_from, hit_from);
	  total_winner = FALSE;
	}
      moria_flag = TRUE;
    }
  else
    prt_chp();
}


/* Given speed,  returns number of moves this turn.	-RAK-	*/
/* NOTE: Player must always move at least once per iteration,     */
/*       a slowed player is handled by moving monsters faster    */
int movement_rate(speed)
int speed;
{
  if (speed > 0)
    {
      if (py.flags.rest > 0)
	return 1;
      else
	return speed;
    }
  else
    {
      /* speed must be negative here */
      return ((turn % (2 - speed)) == 0);
/*    if ((turn % (2 - speed)) == 0)
	return 1;
      else
	return 0;  */
    }
}


/* Regenerate hit points 				-RAK-	*/
regenhp(percent)
double percent;
{
  register struct misc *p_ptr;

  p_ptr = &py.misc;
  p_ptr->chp += p_ptr->mhp*percent + PLAYER_REGEN_HPBASE;
}


/* Regenerate mana points				-RAK-	*/
regenmana(percent)
double percent;
{
  register struct misc *p_ptr;

  p_ptr = &py.misc;
  p_ptr->cmana += p_ptr->mana*percent + PLAYER_REGEN_MNBASE;
}


/* Change a trap from invisible to visible		-RAK-	*/
/* Note: Secret doors are handled here                           */
change_trap(y, x)
register int y, x;
{
  register int k;
  register cave_type *c_ptr;

  c_ptr = &cave[y][x];
  if ((t_list[c_ptr->tptr].tval == 101) || (t_list[c_ptr->tptr].tval == 109))
    {
      k = c_ptr->tptr;
      /* subtract one, since zeroth item has subval of one */
      place_trap(y, x, 2, t_list[k].subval-1);
      pusht(k);
      lite_spot(y, x);
    }
}


/* Searches for hidden things... 			-RAK-	*/
search(y, x, chance)
int y, x, chance;
{
  register int i, j;
  register cave_type *c_ptr;
  register treasure_type *t_ptr;
  register struct flags *p_ptr;
  vtype tmp_str;

  p_ptr = &py.flags;
  if (p_ptr->confused+p_ptr->blind > 0)
    chance = chance / 10.0;
  else if (no_light())
    chance = chance / 5.0;
  for (i = (y - 1); i <= (y + 1); i++)
    for (j = (x - 1); j <= (x + 1); j++)
      if (in_bounds(i, j))
	if ((i != y) || (j != x))
	  if (randint(100) < chance)
	    {
	      c_ptr = &cave[i][j];
	      /* Search for hidden objects             */
	      if (c_ptr->tptr != 0)
		{
		  t_ptr = &t_list[c_ptr->tptr];
		  /* Trap on floor?                */
		  if (t_ptr->tval == 101)
		    {
		      (void) sprintf(tmp_str,"You have found %s.",t_ptr->name);
		      msg_print(tmp_str);
		      change_trap(i, j);
		      if (find_flag)
			{
			  find_flag = FALSE;
			  move_light (char_row, char_col, char_row, char_col);
			}
		    }
		  /* Secret door?                  */
		  else if (t_ptr->tval == 109)
		    {
		      msg_print("You have found a secret door.");
		      c_ptr->fval = corr_floor2.ftval;
		      change_trap(i, j);
		      if (find_flag)
			{
			  find_flag = FALSE;
			  move_light (char_row, char_col, char_row, char_col);
			}
		    }
		  /* Chest is trapped?             */
		  else if (t_ptr->tval == 2)
		    {
		      /* mask out the treasure bits */
		      if ((t_ptr->flags & 0x00FFFFFF) > 1)
			if (index(t_ptr->name, '^') != 0)
			  {
			    known2(t_ptr->name);
			 msg_print("You have discovered a trap on the chest!");
			  }
			else
			  {
			    msg_print("The chest is trapped!");
			  }
		    }
		}
	    }
}


/* Turns off Find_flag if something interesting appears	-RAK-	*/
/* BUG: Does not handle corridor/room corners, but I didn't want */
/*      to add a lot of checking for such a minor detail         */
area_affect(dir, y, x)
int dir, y, x;
{
  int z[3];
  register int i;
  int row, col;
  register cave_type *c_ptr;
  monster_type *m_ptr;

  if (cave[y][x].fval == 4)
    {
      i = 0;
      if (next_to4(y, x, 4, 5, 6) > 2)
	{
	  find_flag = FALSE;  /* no need to call move_light () */
	  return;
	}
    }
  if ((find_flag) && (py.flags.blind < 1))
    {
      switch(dir)
	{
	case 1:
	  z[0] = 4;
	  z[1] = 1;
	  z[2] = 3;
	  break;
	case 2:
	  z[0] = 4;
	  z[1] = 2;
	  z[2] = 6;
	  break;
	case 3:
	  z[0] = 2;
	  z[1] = 3;
	  z[2] = 6;
	  break;
	case 4:
	  z[0] = 8;
	  z[1] = 4;
	  z[2] = 2;
	  break;
	case 6:
	  z[0] = 2;
	  z[1] = 6;
	  z[2] = 8;
	  break;
	case 7:
	  z[0] = 8;
	  z[1] = 7;
	  z[2] = 4;
	  break;
	case 8:
	  z[0] = 4;
	  z[1] = 8;
	  z[2] = 6;
	  break;
	case 9:
	  z[0] = 8;
	  z[1] = 9;
	  z[2] = 6;
	  break;
	}
      for (i = 0; i < 3; i++)
	{
	  row = y;
	  col = x;
	  if (move(z[i], &row, &col))
	    {
	      c_ptr = &cave[row][col];
	      /* Empty doorways        */
	      if (c_ptr->fval == 5)
		{
		  find_flag = FALSE;  /* no need to call move_light () */
		  return;
		}
	      /* Objects player can see*/
	      /* Including doors       */
	      if (find_flag)
		if (player_light)
		  {
		    if (c_ptr->tptr != 0)
		      if ((t_list[c_ptr->tptr].tval != 101) &&
			  (t_list[c_ptr->tptr].tval != 109))
			{
			  find_flag = FALSE;  /* no need to call move_light */
			  return;
			}
		  }
		else if ((c_ptr->tl) || (c_ptr->pl) || (c_ptr->fm))
		  if (c_ptr->tptr != 0)
		    if ((t_list[c_ptr->tptr].tval != 101) &&
			(t_list[c_ptr->tptr].tval != 109))
		      {
			find_flag = FALSE;  /* no need to call move_light */
			return;
		      }
	      /* Creatures             */
	      if (find_flag)
		if ((c_ptr->tl) || (c_ptr->pl) || (player_light))
		  if (c_ptr->cptr > 1)
		    {
		      m_ptr = &m_list[c_ptr->cptr];
		      if (m_ptr->ml)
			{
			  find_flag = FALSE;  /* no need to call move_light */
			  return;
			}
		    }
	    }
	}
    }
  return;
}


/* Picks new direction when in find mode 		-RAK-	*/
int pick_dir(dir)
int dir;
{
  int z[2];
  register int i, pick;
  int y, x;

  if ((find_flag) && (next_to4(char_row, char_col, 4, 5, -1) == 2))
    {
      switch(dir)
	{
	case 1:
	  z[0] = 2;
	  z[1] = 4;
	  break;
	case 2:
	  z[0] = 4;
	  z[1] = 6;
	  break;
	case 3:
	  z[0] = 2;
	  z[1] = 6;
	  break;
	case 4:
	  z[0] = 2;
	  z[1] = 8;
	  break;
	case 6:
	  z[0] = 2;
	  z[1] = 8;
	  break;
	case 7:
	  z[0] = 4;
	  z[1] = 8;
	  break;
	case 8:
	  z[0] = 4;
	  z[1] = 6;
	  break;
	case 9:
	  z[0] = 6;
	  z[1] = 8;
	  break;
	}
      pick = FALSE;
      for (i = 0; i < 2; i++)
	{
	  y = char_row;
	  x = char_col;
	  if (move(z[i], &y, &x))
	    if (cave[y][x].fopen)
	      {
		pick = TRUE;
		global_com_val = z[i] + 48;
		}
	}
    }
  else
    {
      pick = FALSE;
    }
  return(pick);
}


/* AC gets worse 					-RAK-	*/
/* Note: This routine affects magical AC bonuses so that stores   */
/*       can detect the damage.                                  */
int minus_ac(typ_dam)
int typ_dam;
{
  register int i, j;
  int tmp[6];
  int minus;
  register treasure_type *i_ptr;
  vtype out_val, tmp_str;

  i = 0;
  if (inventory[INVEN_BODY].tval != 0)
    {
      tmp[i] = INVEN_BODY;
      i++;
    }
  if (inventory[INVEN_ARM].tval != 0)
    {
      tmp[i] = INVEN_ARM;
      i++;
    }
  if (inventory[INVEN_OUTER].tval != 0)
    {
      tmp[i] = INVEN_OUTER;
      i++;
    }
  if (inventory[INVEN_HANDS].tval != 0)
    {
      tmp[i] = INVEN_HANDS;
      i++;
    }
  if (inventory[INVEN_HEAD].tval != 0)
    {
      tmp[i] = INVEN_HEAD;
      i++;
    }
  /* also affect boots */
  if (inventory[INVEN_FEET].tval != 0)
    {
      tmp[i] = INVEN_FEET;
      i++;
    }
  minus = FALSE;
  if (i > 0)
    {
      j = tmp[randint(i) - 1];
      i_ptr = &inventory[j];
      if (i_ptr->flags & typ_dam)
	{
	  objdes(tmp_str, j, FALSE);
	  (void) sprintf(out_val, "Your %s resists damage!", tmp_str);
	  msg_print(out_val);
	  minus = TRUE;
	}
      else if ((i_ptr->ac+i_ptr->toac) > 0)
	{
	  objdes(tmp_str, j, FALSE);
	  (void) sprintf(out_val, "Your %s is damaged!", tmp_str);
	  msg_print(out_val);
	  i_ptr->toac--;
	  py_bonuses(blank_treasure, 0);
	  minus = TRUE;
	}
    }
  return(minus);
}


/* Corrode the unsuspecting person's armor               -RAK-   */
corrode_gas(kb_str)
char *kb_str;
{
  int set_corrodes();

  if (!minus_ac((int)0x00100000))
    take_hit(randint(8), kb_str);
  print_stat |= 0x0040;
  if (inven_damage(set_corrodes, 5) > 0)
    msg_print("There is an acrid smell coming from your pack.");
}


/* Poison gas the idiot...				-RAK-	*/
poison_gas(dam, kb_str)
int dam;
char *kb_str;
{
  take_hit(dam, kb_str);
  print_stat |= 0x0040;
  py.flags.poisoned += 12 + randint(dam);
}


/* Burn the fool up...					-RAK-	*/
fire_dam(dam, kb_str)
int dam;
char *kb_str;
{
  int set_flammable();

  if (py.flags.fire_resist)
    dam = dam / 3;
  if (py.flags.resist_heat > 0)
    dam = dam / 3;
  take_hit(dam, kb_str);
  print_stat |= 0x0080;
  if (inven_damage(set_flammable, 3) > 0)
    msg_print("There is smoke coming from your pack!");
}


/* Freeze him to death...				-RAK-	*/
cold_dam(dam, kb_str)
int dam;
char *kb_str;
{
  int set_frost_destroy();

  if (py.flags.cold_resist)
    dam = dam / 3;
  if (py.flags.resist_cold > 0)
    dam = dam / 3;
  take_hit(dam, kb_str);
  print_stat |= 0x0080;
  if (inven_damage(set_frost_destroy, 5) > 0)
    msg_print("Something shatters inside your pack!");
}


/* Lightning bolt the sucker away...			-RAK-	*/
light_dam(dam, kb_str)
int dam;
char *kb_str;
{
  if (py.flags.lght_resist)
    take_hit((dam / 3), kb_str);
  else
    take_hit(dam, kb_str);
  print_stat |= 0x0080;
}


/* Throw acid on the hapless victim			-RAK-	*/
acid_dam(dam, kb_str)
int dam;
char *kb_str;
{
  int flag;
  int set_acid_affect();

  flag = 0;
  if (minus_ac((int)0x00100000))
    flag = 1;
  if (py.flags.acid_resist)
    flag += 2;
  switch(flag)
    {
    case 0: take_hit(dam, kb_str);       break;
    case 1: take_hit((dam / 2), kb_str); break;
    case 2: take_hit((dam / 3), kb_str); break;
    case 3: take_hit((dam / 4), kb_str); break;
    }
  print_stat |= 0x00C0;
  if (inven_damage(set_acid_affect, 3) > 0)
    msg_print("There is an acrid smell coming from your pack!");
}
