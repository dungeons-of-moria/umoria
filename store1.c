#include "constants.h"
#include "config.h"
#include "types.h"
#include "externs.h"

#ifdef USG
#include <string.h>
#else
#include <strings.h>
#endif

int search_list(x1, x2)
byteint x1;
int x2;
{
  register int i, j;
  register treasure_type *o_ptr;

  i = 0;
  j = 0;
  do
    {
      o_ptr = &object_list[i];
      if ((o_ptr->tval == x1) && (o_ptr->subval == x2))
	j = o_ptr->cost;
      i++;
    }
  while ((i != MAX_OBJECTS) && (j <= 0));
  return(j);
}


/* Returns the value for any given object		-RAK-	*/
int item_value(item)
treasure_type item;
{
  register int value;
  register treasure_type *i_ptr;

  i_ptr = &item;
  value = i_ptr->cost;
  if (((i_ptr->tval >= 20) && (i_ptr->tval <= 23)) ||
      ((i_ptr->tval >= 30) && (i_ptr->tval <= 36)))
    {	/* Weapons and armor	*/
      if (index(i_ptr->name, '^') != 0)
	value = search_list(i_ptr->tval, i_ptr->subval) * i_ptr->number;
      else if ((i_ptr->tval >= 20) && (i_ptr->tval <= 23))
	{
	  if (i_ptr->tohit < 0)
	    value = 0;
	  else if (i_ptr->todam < 0)
	    value = 0;
	  else if (i_ptr->toac < 0)
	    value = 0;
	  else
	    value = (i_ptr->cost+(i_ptr->tohit+i_ptr->todam+i_ptr->toac)*100)*
	      i_ptr->number;
	}
      else
	{
	  if (i_ptr->toac < 0)
	    value = 0;
	  else
	    value = (i_ptr->cost+i_ptr->toac*100)*i_ptr->number;
	}
    }
  else if ((i_ptr->tval >= 10) && (i_ptr->tval <= 13))
    {	/* Ammo			*/
      if (index(i_ptr->name, '^') != 0)
	value = search_list(i_ptr->tval, 1)*i_ptr->number;
      else
	{
	  if (i_ptr->tohit < 0)
	    value = 0;
	  else if (i_ptr->todam < 0)
	    value = 0;
	  else if (i_ptr->toac < 0)
	    value = 0;
	  else
	    value = (i_ptr->cost+(i_ptr->tohit+i_ptr->todam+i_ptr->toac)*10)*
	      i_ptr->number;
	}
    }
  else if ((i_ptr->tval == 70) || (i_ptr->tval == 71) ||
	   (i_ptr->tval == 75) || (i_ptr->tval == 76) || (i_ptr->tval == 80))
    {	/* Potions, Scrolls, and Food	*/
      if (index(i_ptr->name, '|') != 0)
	switch(i_ptr->tval)
	  {
	  case 70: case 71: value =  20; break;
	  case 75: case 76: value =  20; break;
	  case 80:          value =   1; break;
	  default:          break;
	  }
    }
  else if ((i_ptr->tval == 40) || (i_ptr->tval == 45))
    {	/* Rings and amulets	*/
      if (index(i_ptr->name, '|') != 0)
	/* player does not know what type of ring/amulet this is */
	switch(i_ptr->tval)
	  {
	  case 40: value = 45; break;
	  case 45: value = 45; break;
	  default: break;
	  }
      else if (index(i_ptr->name, '^') != 0)
	/* player knows what type of ring, but does not know whether it is
	   cursed or not, if refuse to buy cursed objects here, then
	   player can use this to 'identify' cursed objects */
	value = abs(i_ptr->cost);
    }
  else if ((i_ptr->tval == 55) || (i_ptr->tval == 60) || (i_ptr->tval == 65))
    {	/* Wands rods, and staffs*/
      if (index(i_ptr->name, '|') != 0)
	switch(i_ptr->tval)
	  {
	  case 55: value = 70; break;
	  case 60: value = 60; break;
	  case 65: value = 50; break;
	  default: break;
	  }
      else if (index(i_ptr->name, '^') == 0)
	{
	  value = i_ptr->cost + (i_ptr->cost/20.0)*i_ptr->p1;
	}
    }
  return(value);
}


/* Asking price for an item				-RAK-	*/
int sell_price(snum, max_sell, min_sell, item)
int snum;
int *max_sell, *min_sell;
treasure_type item;
{
  register int i;
  register store_type *s_ptr;

  s_ptr = &store[snum];
  i = item_value(item);
  if (item.cost > 0)
    {
      i += (i * rgold_adj[owners[s_ptr->owner].owner_race][py.misc.prace]);
      if (i < 1)  i = 1;
      *max_sell = (i * (1+owners[s_ptr->owner].max_inflate));
      *min_sell = (i * (1+owners[s_ptr->owner].min_inflate));
      if (min_sell > max_sell)  min_sell = max_sell;
      return(i);
    }
  else
    {
      *max_sell = 0;
      *min_sell = 0;
      return(i);
    }
}


/* Check to see if he will be carrying too many objects	-RAK-	*/
int store_check_num(store_num)
int store_num;
{
  register int store_check;
  register int i;
  register store_type *s_ptr;
  register treasure_type *i_ptr;

  store_check = FALSE;
  s_ptr = &store[store_num];
  if (s_ptr->store_ctr < STORE_INVEN_MAX)
    store_check = TRUE;
  else if ((inventory[INVEN_MAX].subval > 255) &&
	   (inventory[INVEN_MAX].subval < 512))
    for (i = 0; i < s_ptr->store_ctr; i++)
      {
	i_ptr = &s_ptr->store_inven[i].sitem;
	if (i_ptr->tval == inventory[INVEN_MAX].tval)
	  if (i_ptr->subval == inventory[INVEN_MAX].subval)
	    store_check = TRUE;
      }
  return(store_check);
}


/* Insert INVEN_MAX at given location	*/
insert_store(store_num, pos, icost)
register int pos;
int store_num, icost;
{
  register int i;
  register store_type *s_ptr;

  s_ptr = &store[store_num];
  for (i = s_ptr->store_ctr-1; i >= pos; i--)
    s_ptr->store_inven[i+1] = s_ptr->store_inven[i];
  s_ptr->store_inven[pos].sitem = inventory[INVEN_MAX];
  s_ptr->store_inven[pos].scost = -icost;
  s_ptr->store_ctr++;
}


/* Add the item in INVEN_MAX to stores inventory.	-RAK-	*/
store_carry(store_num, ipos)
int store_num;
int *ipos;
{
  int item_num, item_val;
  register int typ, subt;
  int icost, dummy, flag;
  register treasure_type *i_ptr;
  register store_type *s_ptr;

  *ipos = 0;
  identify(inventory[INVEN_MAX]);
  known2(inventory[INVEN_MAX].name);
  (void) sell_price(store_num, &icost, &dummy, inventory[INVEN_MAX]);
  if (icost > 0)
    {
      i_ptr = &inventory[INVEN_MAX];
      s_ptr = &store[store_num];
      item_val = 0;
      item_num = i_ptr->number;
      flag = FALSE;
      typ  = i_ptr->tval;
      subt = i_ptr->subval;
      do
	{
	  i_ptr = &s_ptr->store_inven[item_val].sitem;
	  if (typ == i_ptr->tval)
	    {
	      if (subt == i_ptr->subval) /* Adds to other item	*/
		if (subt > 255)
		  {
		    if (i_ptr->number < 24)
		      i_ptr->number += item_num;
		    flag = TRUE;
		  }
	    }
	  else if (typ > i_ptr->tval)
	    {		/* Insert into list		*/
	      insert_store(store_num, item_val, icost);
	      flag = TRUE;
	      *ipos = item_val;
	    }
	  item_val++;
	}
      while ((item_val < s_ptr->store_ctr) && (!flag));
      if (!flag) 	/* Becomes last item in list	*/
	{
	  insert_store(store_num, (int)s_ptr->store_ctr, icost);
	  *ipos = s_ptr->store_ctr - 1;
	}
    }
}

/* Destroy an item in the stores inventory.  Note that if	*/
/* "one_of" is false, an entire slot is destroyed	-RAK-	*/
store_destroy(store_num, item_val, one_of)
int store_num, item_val;
int one_of;
{
  register int j;
  register store_type *s_ptr;
  register treasure_type *i_ptr;

  s_ptr = &store[store_num];
  inventory[INVEN_MAX] = s_ptr->store_inven[item_val].sitem;
  i_ptr = &s_ptr->store_inven[item_val].sitem;
  if ((i_ptr->number > 1) && (i_ptr->subval < 512) && (one_of))
    {
      i_ptr->number--;
      inventory[INVEN_MAX].number = 1;
    }
  else
    {
      for (j = item_val; j < s_ptr->store_ctr-1; j++)
	s_ptr->store_inven[j] = s_ptr->store_inven[j+1];
      s_ptr->store_inven[s_ptr->store_ctr-1].sitem = blank_treasure;
      s_ptr->store_inven[s_ptr->store_ctr-1].scost = 0;
      s_ptr->store_ctr--;
    }
}


/* Initializes the stores with owners			-RAK-	*/
store_init()
{
  register int i, j, k;
  register store_type *s_ptr;

  i = MAX_OWNERS / MAX_STORES;
  for (j = 0; j < MAX_STORES; j++)
    {
      s_ptr = &store[j];
      s_ptr->owner = MAX_STORES*(randint(i)-1) + j;
      s_ptr->insult_cur = 0;
      s_ptr->store_open = 0;
      s_ptr->store_ctr  = 0;
      for (k = 0; k < STORE_INVEN_MAX; k++)
	{
	  s_ptr->store_inven[k].sitem = blank_treasure;
	  s_ptr->store_inven[k].scost = 0;
	}
    }
}


/* Creates an item and inserts it into store's inven	-RAK-	*/
store_create(store_num)
int store_num;
{
  register int i, tries;
  int cur_pos, dummy;
  register store_type *s_ptr;
  register treasure_type *t_ptr;

  tries = 0;
  popt(&cur_pos);
  s_ptr = &store[store_num];
  do
    {
      i = store_choice[store_num][randint(STORE_CHOICES)-1];
      /* this index is one more than it should be, so subtract one */
      /* see store_choice in variables.c */
      t_list[cur_pos] = inventory_init[i-1];
      magic_treasure(cur_pos, OBJ_TOWN_LEVEL);
      inventory[INVEN_MAX] = t_list[cur_pos];
      if (store_check_num(store_num))
	{
	  t_ptr = &t_list[cur_pos];
	  if (t_ptr->cost > 0) 	/* Item must be good	*/
	    if (t_ptr->cost < owners[s_ptr->owner].max_cost)
	      {
		store_carry(store_num, &dummy);
		tries = 10;
	      }
	}
      tries++;
    }
  while (tries <= 3);
  pusht(cur_pos);
}


/* Initialize and up-keep the store's inventory.		-RAK-	*/
store_maint()
{
  register int i, j;
  register store_type *s_ptr;

  for (i = 0; i < MAX_STORES; i++)
    {
      s_ptr = &store[i];
      s_ptr->insult_cur = 0;
      if (s_ptr->store_ctr > STORE_MAX_INVEN)
	for (j = 0; j < (s_ptr->store_ctr-STORE_MAX_INVEN+2); j++)
	  store_destroy(i, randint((int)s_ptr->store_ctr)-1, FALSE);
      else if (s_ptr->store_ctr < STORE_MIN_INVEN)
	{
	  for (j = 0; j < (STORE_MIN_INVEN-s_ptr->store_ctr+2); j++)
	    store_create(i);
	}
      else
	{
	  for (j = 0; j < (1+randint(STORE_TURN_AROUND)); j++)
	    store_destroy(i, randint((int)s_ptr->store_ctr)-1, TRUE);
	  for (j = 0; j < (1+randint(STORE_TURN_AROUND)); j++)
	    store_create(i);
	}
    }
}
