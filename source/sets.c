/* source/sets.c: code to emulate the original Pascal sets

   Copyright (c) 1989-92 James E. Wilson

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include "config.h"
#include "constant.h"
#include "types.h"

int set_room(element)
register int element;
{
  if ((element == DARK_FLOOR) || (element == LIGHT_FLOOR))
    return(TRUE);
  return(FALSE);
}

int set_corr(element)
register int element;
{
  if (element == CORR_FLOOR || element == BLOCKED_FLOOR)
    return(TRUE);
  return(FALSE);
}

int set_floor(element)
int element;
{
  if (element <= MAX_CAVE_FLOOR)
    return(TRUE);
  else
    return(FALSE);
}

int set_corrodes(item)
inven_type *item;
{
  switch(item->tval)
    {
    case TV_SWORD: case TV_HELM: case TV_SHIELD: case TV_HARD_ARMOR:
    case TV_WAND:
      return (TRUE);
    }
  return(FALSE);
}


int set_flammable(item)
inven_type *item;
{
  switch(item->tval)
    {
    case TV_ARROW: case TV_BOW: case TV_HAFTED: case TV_POLEARM:
    case TV_BOOTS: case TV_GLOVES: case TV_CLOAK: case TV_SOFT_ARMOR:
      /* Items of (RF) should not be destroyed.  */
      if (item->flags & TR_RES_FIRE)
	return FALSE;
      else
	return TRUE;

    case TV_STAFF: case TV_SCROLL1: case TV_SCROLL2:
      return TRUE;
    }
  return(FALSE);
}


int set_frost_destroy(item)
inven_type *item;
{
  if ((item->tval == TV_POTION1) || (item->tval == TV_POTION2)
      || (item->tval == TV_FLASK))
    return(TRUE);
  return(FALSE);
}


int set_acid_affect(item)
inven_type *item;
{
  switch(item->tval)
    {
    case TV_MISC: case TV_CHEST:
      return TRUE;
    case TV_BOLT: case TV_ARROW:
    case TV_BOW: case TV_HAFTED: case TV_POLEARM: case TV_BOOTS:
    case TV_GLOVES: case TV_CLOAK: case TV_SOFT_ARMOR:
      if (item->flags & TR_RES_ACID)
	return (FALSE);
      else
	return (TRUE);
    }
  return(FALSE);
}


int set_lightning_destroy(item)
inven_type *item;
{
  if ((item->tval == TV_RING) || (item->tval == TV_WAND)
      || (item->tval == TV_SPIKE))
    return(TRUE);
  else
    return(FALSE);
}


/*ARGSUSED*/	/* to shut up lint about unused argument */
#ifdef __TURBOC__
	#pragma argused
#endif
int set_null(item)
inven_type *item;
#if defined(MAC) && !defined(THINK_C)
	#pragma unused(item)
#endif
{
  return(FALSE);
}


int set_acid_destroy(item)
inven_type *item;
{
  switch(item->tval)
    {
    case TV_ARROW: case TV_BOW: case TV_HAFTED: case TV_POLEARM:
    case TV_BOOTS: case TV_GLOVES: case TV_CLOAK: case TV_HELM:
    case TV_SHIELD: case TV_HARD_ARMOR: case TV_SOFT_ARMOR:
      if (item->flags & TR_RES_ACID)
	return FALSE;
      else
	return TRUE;
    case TV_STAFF:
    case TV_SCROLL1: case TV_SCROLL2: case TV_FOOD: case TV_OPEN_DOOR:
    case TV_CLOSED_DOOR:
      return(TRUE);
    }
  return(FALSE);
}


int set_fire_destroy(item)
inven_type *item;
{
  switch(item->tval)
    {
    case TV_ARROW: case TV_BOW: case TV_HAFTED: case TV_POLEARM:
    case TV_BOOTS: case TV_GLOVES: case TV_CLOAK: case TV_SOFT_ARMOR:
      if (item->flags & TR_RES_FIRE)
	return FALSE;
      else
	return TRUE;
    case TV_STAFF: case TV_SCROLL1: case TV_SCROLL2: case TV_POTION1:
    case TV_POTION2: case TV_FLASK: case TV_FOOD: case TV_OPEN_DOOR:
    case TV_CLOSED_DOOR:
      return(TRUE);
    }
  return(FALSE);
}


int general_store(element)
int element;
{
  switch(element)
    {
    case TV_DIGGING: case TV_BOOTS: case TV_CLOAK: case TV_FOOD:
    case TV_FLASK: case TV_LIGHT: case TV_SPIKE:
      return(TRUE);
    }
  return(FALSE);
}


int armory(element)
int element;
{
  switch(element)
    {
    case TV_BOOTS: case TV_GLOVES: case TV_HELM: case TV_SHIELD:
    case TV_HARD_ARMOR: case TV_SOFT_ARMOR:
      return(TRUE);
    }
  return(FALSE);
}


int weaponsmith(element)
int element;
{
  switch(element)
    {
    case TV_SLING_AMMO: case TV_BOLT: case TV_ARROW: case TV_BOW:
    case TV_HAFTED: case TV_POLEARM: case TV_SWORD:
      return(TRUE);
    }
  return(FALSE);
}


int temple(element)
int element;
{
  switch(element)
    {
    case TV_HAFTED: case TV_SCROLL1: case TV_SCROLL2: case TV_POTION1:
    case TV_POTION2: case TV_PRAYER_BOOK:
      return(TRUE);
    }
  return(FALSE);
}


int alchemist(element)
int element;
{
  switch(element)
    {
    case TV_SCROLL1: case TV_SCROLL2: case TV_POTION1: case TV_POTION2:
      return(TRUE);
    }
  return(FALSE);
}


int magic_shop(element)
int element;
{
  switch(element)
    {
    case TV_AMULET: case TV_RING: case TV_STAFF: case TV_WAND:
    case TV_SCROLL1: case TV_SCROLL2: case TV_POTION1: case TV_POTION2:
    case TV_MAGIC_BOOK:
      return(TRUE);
    }
  return(FALSE);
}

#ifdef MAC
/* The last time tried, MPW failed to handle the initialized array of
   function pointers properly.  hence, this hack. */
int store_buy(storeno, element)
int storeno;
int element;
{
  switch (storeno)
    {
    case 0: return(general_store(element));
    case 1: return(armory(element));
    case 2: return(weaponsmith(element));
    case 3: return(temple(element));
    case 4: return(alchemist(element));
    case 5: return(magic_shop(element));
    }
  return(FALSE);
}
#endif
