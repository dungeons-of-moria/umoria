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

/* Object descriptor routines					*/

int is_a_vowel(ch)
char ch;
{
  switch(ch)
    {
    case 'a': case 'e': case 'i': case 'o': case 'u':
    case 'A': case 'E': case 'I': case 'O': case 'U':
      return(TRUE);
    default:
      return(FALSE);
    }
}


/* Randomize colors, woods, and metals				*/
randes()
{
  register int i, j;
  vtype tmp;

  for (i = 0; i < MAX_COLORS; i++)
    {
      j = randint(MAX_COLORS) - 1;
      (void) strcpy(tmp, colors[i]);
      (void) strcpy(colors[i], colors[j]);
      (void) strcpy(colors[j], tmp);
    }
  for (i = 0; i < MAX_WOODS; i++)
    {
      j = randint(MAX_WOODS) - 1;
      (void) strcpy(tmp, woods[i]);
      (void) strcpy(woods[i], woods[j]);
      (void) strcpy(woods[j], tmp);
    }
  for (i = 0; i < MAX_METALS; i++)
    {
      j = randint(MAX_METALS) - 1;
      (void) strcpy(tmp, metals[i]);
      (void) strcpy(metals[i], metals[j]);
      (void) strcpy(metals[j], tmp);
    }
  for (i = 0; i < MAX_ROCKS; i++)
    {
      j = randint(MAX_ROCKS) - 1;
      (void) strcpy(tmp, rocks[i]);
      (void) strcpy(rocks[i], rocks[j]);
      (void) strcpy(rocks[j], tmp);
    }
  for (i = 0; i < MAX_AMULETS; i++)
    {
      j = randint(MAX_AMULETS) - 1;
      (void) strcpy(tmp, amulets[i]);
      (void) strcpy(amulets[i], amulets[j]);
      (void) strcpy(amulets[j], tmp);
    }
  for (i = 0; i < MAX_MUSH; i++)
    {
      j = randint(MAX_MUSH) - 1;
      (void) strcpy(tmp, mushrooms[i]);
      (void) strcpy(mushrooms[i], mushrooms[j]);
      (void) strcpy(mushrooms[j], tmp);
    }
}


/* Return random title						*/
rantitle(title)
char *title;
{
  register int i, j, k;

  k = randint(2) + 1;
  (void) strcpy(title, "Titled \"");
  for (i = 0; i < k; i++)
    {
      for (j = 0; j < randint(2); j++)
	(void) strcat(title, syllables[randint(MAX_SYLLABLES) - 1]);
      if (i < k-1)
	(void) strcat(title, " ");
    }
  (void) strcat(title, "\"");
}


/* Initialize all Potions, wands, staves, scrolls, ect...	*/
magic_init()
{
  register int i, tmpv;
  vtype tmps;

  set_seed(randes_state, randes_seed);
  randes();
  for (i = 0; i < MAX_OBJECTS; i++)
    {
      tmpv = (0xFF & object_list[i].subval);
      switch(object_list[i].tval)
	{
	case 75: case 76:
	  if (tmpv < MAX_COLORS)
	    insert_str(object_list[i].name, "%C", colors[tmpv]);
	  break;
	case 70: case 71:
	  rantitle(tmps);
	  insert_str(object_list[i].name, "%T", tmps);
	  break;
	case 45:
	  if (tmpv < MAX_ROCKS)
	    insert_str(object_list[i].name, "%R", rocks[tmpv]);
	  break;
	case 40: if (tmpv < MAX_AMULETS)
	  insert_str(object_list[i].name, "%A", amulets[tmpv]);
	  break;
	case 65:
	  if (tmpv < MAX_METALS)
	    insert_str(object_list[i].name, "%M", metals[tmpv]);
	  break;
	case 55:
	  if (tmpv < MAX_WOODS)
	    insert_str(object_list[i].name, "%W", woods[tmpv]);
	  break;
	case 80:
	  if (tmpv < MAX_MUSH)
	    insert_str(object_list[i].name, "%M", mushrooms[tmpv]);
	  break;
	case 60:
	  /*if (tmpv < MAX_RODS)
	    insert_str(object_list[i].name, "%D", rods[tmpv])*/;
	  break;
	default:
	  break;
	}
    }
  reset_seed();
}


/* Remove "Secret" symbol for identity of object			*/
known1(object_str)
char *object_str;
{
  register int pos;
  vtype str1, str2;
  register char *string;

  string = index(object_str, '|');
  if (string)
    pos = strlen(object_str) - strlen(string);
  else
    pos = -1;
  if (pos >= 0)
    {
      (void) strncpy(str1, object_str, pos);
      str1[pos] = '\0';
      (void) strcpy(str2, &object_str[pos+1]);
      (void) strcpy(object_str, strcat(str1, str2));
    }
}


/* Remove "Secret" symbol for identity of plusses			*/
known2(object_str)
char *object_str;
{
  register int pos;
  vtype str1, str2;
  register char *string;

  string = index(object_str, '^');
  if (string)
    pos = strlen(object_str) - strlen(string);
  else
    pos = -1;
  if (pos >= 0)
    {
      (void) strncpy(str1, object_str, pos);
      str1[pos] = '\0';
      (void) strcpy(str2, &object_str[pos+1]);
      (void) strcpy(object_str, strcat(str1, str2));
    }
}


/* Return string without quoted portion				*/
unquote(object_str)
char *object_str;
{
  register int pos0, pos1, pos2;
  vtype str1, str2;
  register char *string;

  string = index(object_str, '\"');
  if (string)
    pos0 = strlen(string) - strlen(object_str);
  else
    pos0 = -1;
  if (pos0 >= 0)
    {
      string = index(object_str, '~');
      if (string)
	pos1 = strlen(string) - strlen(object_str);
      else
	pos1 = 0;
      string = index(object_str, '|');
      if (string)
	pos2 = strlen(string) - strlen(object_str);
      else
	pos2 = 0;
      (void) strncpy(str1, object_str, pos1);
      str1[pos1] = '\0';
      (void) strcpy(str2, &object_str[pos2+1]);
      (void) strcpy(object_str, strcat(str1, str2));
    }
}


/* Somethings been identified					*/
identify(item)
treasure_type item;
{
  register int i, x1, x2;
  register treasure_type *t_ptr;
  char *string;

  x1 = item.tval;
  x2 = item.subval;
  if (index(item.name, '|') != 0)
    {
      for (i = 0; i < MAX_TALLOC; i++)
	{
	  t_ptr = &t_list[i];
	  if ((t_ptr->tval == x1) && (t_ptr->subval == x2))
	    {
	      unquote(t_ptr->name);
	      known1(t_ptr->name);
	    }
	}
      for (i = 0; i <= INVEN_MAX; i++)
	{
	  t_ptr = &inventory[i];
	  if ((t_ptr->tval == x1) && (t_ptr->subval == x2))
	    {
	      unquote(t_ptr->name);
	      known1(t_ptr->name);
	    }
	}
      i = 0;
      do
	{
	  t_ptr = &object_list[i];
	  if ((t_ptr->tval == x1) && (t_ptr->subval == x2))
	    if ((string = index(t_ptr->name, '%')) && (string[0] == 'T'))
	      {
		insert_str(t_ptr->name, " %T|", "");
		object_ident[i] = TRUE;
	      }
	    else
	      {
		unquote(t_ptr->name);
		known1(t_ptr->name);
		object_ident[i] = TRUE;
	      }
	  i++;
	}
      while (i != MAX_OBJECTS);
    }
}


/* Returns a description of item for inventory			*/
/* pref indicates that there should be an article added (prefix) */
objdes(out_val, ptr, pref)
char *out_val;
int ptr;
int pref;
{
  register int pos;
  vtype tmp_val;
  register treasure_type *i_ptr;
  register char *string;

  i_ptr = &inventory[ptr];
  (void) strcpy(tmp_val, i_ptr->name);
  string = index(tmp_val, '|');
  if (string)
    pos = strlen(tmp_val) - strlen(string);
  else
    pos = -1;
  if (pos >= 0)
    {
      tmp_val[pos] = '\0';
    }
  string = index(tmp_val, '^');
  if (string)
    pos = strlen(tmp_val) - strlen(string);
  else
    pos = -1;
  if (pos >= 0)
    {
      tmp_val[pos] = '\0';
    }
  if (!pref)
    {
      string = index(tmp_val, '(');
      if (string)
	pos = strlen(tmp_val) - strlen(string);
      else
	pos = -1;
      if (pos >= 0)
	{
	  tmp_val[pos] = '\0';
	}
    }
  insert_num(tmp_val, "%P1", i_ptr->p1, TRUE);
  insert_num(tmp_val, "%P2", i_ptr->tohit, TRUE);
  insert_num(tmp_val, "%P3", i_ptr->todam, TRUE);
  insert_num(tmp_val, "%P4", i_ptr->toac, TRUE);
  insert_num(tmp_val, "%P5", i_ptr->p1, FALSE);
  insert_num(tmp_val, "%P6", i_ptr->ac, FALSE);
  if (i_ptr->number != 1)
    {
      insert_str(tmp_val, "ch~", "ches");
      insert_str(tmp_val, "~", "s");
    }
  else
    insert_str(tmp_val, "~", "");
  if (pref)
    {
      if (index(tmp_val, '&') != 0)
	{
	  insert_str(tmp_val, "&", "");
	  if (i_ptr->number > 1)
	    (void) sprintf(out_val, "%d%s", (int)i_ptr->number, tmp_val);
	  else if (i_ptr->number < 1)
	    (void) sprintf(out_val, "%s%s", "no more", tmp_val);
	  else if (is_a_vowel(tmp_val[1]))
	    (void) sprintf(out_val, "an%s", tmp_val);
	  else
	    (void) sprintf(out_val, "a%s", tmp_val);
	}
      /* handle 'no more' case specially */
      else if (i_ptr->number < 1)
	{
	  /* check for "some" at start */
	  if (!strncmp("some", tmp_val, 4))
	    (void) sprintf(out_val, "no more %s", &tmp_val[5]);
	  /* here if no article */
	  else
	    (void) sprintf(out_val, "no more %s", tmp_val);
	}
      else
	(void) strcpy(out_val, tmp_val);
      (void) strcat(out_val, ".");
    }
  else
    {
      insert_str(tmp_val, "& ", "");
      if (!strncmp("some", tmp_val, 4))
	(void) strcpy(out_val, &tmp_val[5]);
      else
	(void) strcpy(out_val, tmp_val);
    }
}
