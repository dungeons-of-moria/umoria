#include "constants.h"

int set_1_2(element)
int element;
{
  if ((element == 1) || (element == 2))
    return(TRUE);
  return(FALSE);
}

int set_1_2_4(element)
int element;
{
  if ((element == 1) || (element == 2) || (element == 4))
    return(TRUE);
  return(FALSE);
}

int set_4(element)
int element;
{
  if (element == 4)
    return(TRUE);
  return(FALSE);
}

int set_corrodes(element)
int element;
{
  switch(element)
    {
    case 23: case 33: case 34: case 35: case 65:
      return(TRUE);
      break;
    }
  return(FALSE);
}


int set_flammable(element)
int element;
{
  switch(element)
    {
    case 12: case 20: case 21: case 22: case 30: case 31: case 32:
    case 36: case 55: case 70: case 71:
      return(TRUE);
      break;
    }
  return(FALSE);
}


int set_frost_destroy(element)
int element;
{
  if ((element == 75) || (element == 76))
    return(TRUE);
  return(FALSE);
}


int set_acid_affect(element)
int element;
{
  switch(element)
    {
    case 1: case 2: case 11: case 12: case 20: case 21: case 22:
    case 30: case 31: case 32: case 36:
      return(TRUE);
      break;
    }
  return(FALSE);
}


int set_floor(element)
{
  switch(element)
    {
    case 1: case 2: case 4: case 5: case 6: case 7:
      return(TRUE);
      break;
    }
  return(FALSE);
}


int set_lightning_destroy(element)
int element;
{
  switch(element)
    {
    case 45: case 60: case 65:
      return(FALSE);
      break;
    }
  return(TRUE);
}


/*ARGSUSED*/    /* to shut up lint about unused argument */
int set_null(element)
int element;
{
  return(FALSE);
}


int set_acid_destroy(element)
int element;
{
  switch(element)
    {
    case 12: case 20: case 21: case 22: case 30: case 31: case 32: case 33:
    case 34: case 35: case 36: case 55: case 70: case 71: case 80: case 104:
    case 105:
      return(TRUE);
      break;
    }
  return(FALSE);
}


int set_fire_destroy(element)
int element;
{
  switch(element)
    {
    case 12: case 20: case 21: case 22: case 30: case 31: case 32: case 36:
    case 55: case 70: case 71: case 75: case 76: case 80: case 104:
    case 105:
      return(TRUE);
      break;
    }
  return(FALSE);
}


int general_store(element)
int element;
{
  switch(element)
    {
    case 25: case 30: case 32: case 80: case 77: case 15: case 13:
      return(TRUE);
      break;
    }
  return(FALSE);
}


int armory(element)
int element;
{
  switch(element)
    {
    case 30: case 31: case 33: case 34: case 35: case 36:
      return(TRUE);
      break;
    }
  return(FALSE);
}


int weaponsmith(element)
int element;
{
  switch(element)
    {
    case 10: case 11: case 12: case 20: case 21: case 22: case 23:
      return(TRUE);
      break;
    }
  return(FALSE);
}


int temple(element)
int element;
{
  switch(element)
    {
    case 21: case 70: case 71: case 75: case 76: case 91:
      return(TRUE);
      break;
    }
  return(FALSE);
}


int alchemist(element)
int element;
{
  switch(element)
    {
    case 70: case 71: case 75: case 76:
      return(TRUE);
      break;
    }
  return(FALSE);
}


int magic_shop(element)
int element;
{
  switch(element)
    {
    case 40: case 45: case 55: case 65: case 70: case 71:
    case 75: case 76: case 90:
      return(TRUE);
      break;
    }
  return(FALSE);
}


int set_wall(element)
int element;
{
  switch(element)
    {
    case 10: case 11: case 12:
      return(TRUE);
      break;
    }
  return(FALSE);
}


int set_pwall(element)
int element;
{
  switch(element)
    {
    case 10: case 11: case 12: case 15:
      return(TRUE);
      break;
    }
  return(FALSE);
}


int set_corr(element)
int element;
{
  if ((element == 4) || (element == 5))
    return(TRUE);
  return(FALSE);
}


int set_trap(element)
int element;
{
  switch(element)
    {
    case 101: case 102: case 109: case 110:
      return(TRUE);
      break;
    }
  return(FALSE);
}


int set_light(element)
int element;
{
  switch(element)
    {
    case 102: case 103: case 104: case 105:
    case 107: case 108: case 109: case 110:
      return(TRUE);
      break;
    }
  return(FALSE);
}
