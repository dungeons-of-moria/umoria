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


/* Print Moria credits					-RAK-	*/
game_version()
{
  vtype tmp_str;

  clear_screen(0, 0);
  (void) sprintf(tmp_str, "               Moria Version %f", CUR_VERSION);
  put_buffer(tmp_str, 0, 0);
  put_buffer("Version 0.1  : 03/25/83", 1, 0);
  put_buffer("Version 1.0  : 05/01/84", 2, 0);
  put_buffer("Version 2.0  : 07/10/84", 3, 0);
  put_buffer("Version 3.0  : 11/20/84", 4, 0);
  put_buffer("Version 4.0  : 01/20/85", 5, 0);
  put_buffer("Modules :", 7, 0);
  put_buffer("     V1.0  Dungeon Generator      - RAK", 8, 0);
  put_buffer("           Character Generator    - RAK & JWT", 9, 0);
  put_buffer("           Moria Module           - RAK", 10, 0);
  put_buffer("           Miscellaneous          - RAK & JWT", 11, 0);
  put_buffer("     V2.0  Town Level & Misc      - RAK", 12, 0);
  put_buffer("     V3.0  Internal Help & Misc   - RAK", 13, 0);
  put_buffer("     V4.0  Source Release Version - RAK", 14, 0);
  put_buffer("Robert Alan Koeneke               Jimmey Wayne Todd Jr.", 16, 0);
 put_buffer("Student/University of Oklahoma    Student/University of Oklahoma",
	    17, 0);
  put_buffer("119 Crystal Bend                  1912 Tiffany Dr.", 18, 0);
  put_buffer("Norman, OK 73069                  Norman, OK  73071", 19, 0);
  put_buffer("(405)-321-2925                    (405) 360-6792", 20, 0);
  pause_line(23);
  clear_screen(0, 0);
  put_buffer("UNIX MORIA Port by James E. Wilson", 2, 0);
  put_buffer("                   wilson@ernie.Berkeley.EDU", 3, 0);
  put_buffer("                   ucbvax!ucbernie!wilson", 4, 0);
  put_buffer("This version is based on the VMS version 4.8", 6, 0);
  put_buffer("but is no longer identical to the original VMS program.", 7, 0);
  put_buffer("Please use care when referring to this program.", 8, 0);
  put_buffer("Please call it 'umoria' or 'UNIX MORIA' or something", 9, 0);
  put_buffer("similar to avoid confusion.", 10, 0);
  pause_line(23);
  draw_cave();
}


/* Light up the dungeon					-RAK-	*/
wizard_light()
{
  register cave_type *c_ptr;
  register int k, l, i, j;
  int flag;

  if (cave[char_row][char_col].pl)
    flag = FALSE;
  else
    flag = TRUE;
  for (i = 0; i < cur_height; i++)
    for (j = 0; j < cur_width; j++)
      if (set_floor(cave[i][j].fval))
	for (k = i-1; k <= i+1; k++)
	  for (l = j-1; l <= j+1; l++)
	    {
	      c_ptr = &cave[k][l];
	      c_ptr->pl = flag;
	      if (!flag)
		c_ptr->fm = FALSE;
	    }
  prt_map();
}


/* Wizard routine for gaining on stats			-RAK-	*/
change_character()
{
  int tmp_val;
  vtype tmp_str;
  register struct stats *s_ptr;
  register struct misc *m_ptr;

  s_ptr = &py.stats;
  prt("(3 - 118) Strength     == ", 0, 0);
  (void) get_string(tmp_str, 0, 25, 10);
  tmp_val = -999;
  (void) sscanf(tmp_str, "%d", &tmp_val);
  if ((tmp_val > 2) && (tmp_val < 119))
    {
      s_ptr->str  = tmp_val;
      s_ptr->cstr = tmp_val;
      prt_strength();
    }
  prt("(3 - 118) Intelligence == ", 0, 0);
  (void) get_string(tmp_str, 0, 25, 10);
  tmp_val = -999;
  (void) sscanf(tmp_str, "%d", &tmp_val);
  if ((tmp_val > 2) && (tmp_val < 119))
    {
      s_ptr->intel  = tmp_val;
      s_ptr->cint = tmp_val;
      prt_intelligence();
    }
  prt("(3 - 118) Wisdom       == ", 0, 0);
  (void) get_string(tmp_str, 0, 25, 10);
  tmp_val = -999;
  (void) sscanf(tmp_str, "%d", &tmp_val);
  if ((tmp_val > 2) && (tmp_val < 119))
    {
      s_ptr->wis  = tmp_val;
      s_ptr->cwis = tmp_val;
      prt_wisdom();
    }
  prt("(3 - 118) Dexterity    == ", 0, 0);
  (void) get_string(tmp_str, 0, 25, 10);
  tmp_val = -999;
  (void) sscanf(tmp_str, "%d", &tmp_val);
  if ((tmp_val > 2) && (tmp_val < 119))
    {
      s_ptr->dex  = tmp_val;
      s_ptr->cdex = tmp_val;
      prt_dexterity();
    }
  prt("(3 - 118) Constitution == ", 0, 0);
  (void) get_string(tmp_str, 0, 25, 10);
  tmp_val = -999;
  (void) sscanf(tmp_str, "%d", &tmp_val);
  if ((tmp_val > 2) && (tmp_val < 119))
    {
      s_ptr->con  = tmp_val;
      s_ptr->ccon = tmp_val;
      prt_constitution();
    }
  prt("(3 - 118) Charisma     == ", 0, 0);
  (void) get_string(tmp_str, 0, 25, 10);
  tmp_val = -999;
  (void) sscanf(tmp_str, "%d", &tmp_val);
  if ((tmp_val > 2) && (tmp_val < 119))
    {
      s_ptr->chr  = tmp_val;
      s_ptr->cchr = tmp_val;
      prt_charisma();
    }

  m_ptr = &py.misc;
  prt("(1 - 32767) Hit points == ", 0, 0);
  (void) get_string(tmp_str, 0, 25, 10);
  tmp_val = -1;
  (void) sscanf(tmp_str, "%d", &tmp_val);
  if ((tmp_val > 0) && (tmp_val < 32768))
    {
      m_ptr->mhp  = tmp_val;
      m_ptr->chp  = (double)tmp_val;
      prt_mhp();
      prt_chp();
    }
  prt("(0 - 32767) Mana       == ", 0, 0);
  (void) get_string(tmp_str, 0, 25, 10);
  tmp_val = -999;
  (void) sscanf(tmp_str, "%d", &tmp_val);
  if ((tmp_val > -1) && (tmp_val < 32768))
    {
      m_ptr->mana  = tmp_val;
      m_ptr->cmana = (double)tmp_val;
      prt_cmana();
    }
  (void) sprintf(tmp_str, "Current==%d  (0-200) Searching == ", m_ptr->srh);
  tmp_val = strlen(tmp_str);
  prt(tmp_str, 0, 0);
  (void) get_string(tmp_str, 0, tmp_val, 10);
  tmp_val = -999;
  (void) sscanf(tmp_str, "%d", &tmp_val);
  if ((tmp_val > -1) && (tmp_val < 201))
    m_ptr->srh  = tmp_val;
  (void) sprintf(tmp_str, "Current==%d  (0-10) Stealth == ", m_ptr->stl);
  tmp_val = strlen(tmp_str);
  prt(tmp_str, 0, 0);
  (void) get_string(tmp_str, 0, tmp_val, 10);
  tmp_val = -999;
  (void) sscanf(tmp_str, "%d", &tmp_val);
  if ((tmp_val > -1) && (tmp_val < 11))
    m_ptr->stl  = tmp_val;
  (void) sprintf(tmp_str, "Current==%d  (0-200) Disarming == ", m_ptr->disarm);
  tmp_val = strlen(tmp_str);
  prt(tmp_str, 0, 0);
  (void) get_string(tmp_str, 0, tmp_val, 10);
  tmp_val = -999;
  (void) sscanf(tmp_str, "%d", &tmp_val);
  if ((tmp_val > -1) && (tmp_val < 201))
    m_ptr->disarm = tmp_val;
  (void) sprintf(tmp_str, "Current==%d  (0-100) Save == ", m_ptr->save);
  tmp_val = strlen(tmp_str);
  prt(tmp_str, 0, 0);
  (void) get_string(tmp_str, 0, tmp_val, 10);
  tmp_val = -999;
  (void) sscanf(tmp_str, "%d", &tmp_val);
  if ((tmp_val > -1) && (tmp_val < 201))
    m_ptr->save = tmp_val;
  (void) sprintf(tmp_str, "Current==%d  (0-200) Base to hit == ", m_ptr->bth);
  tmp_val = strlen(tmp_str);
  prt(tmp_str, 0, 0);
  (void) get_string(tmp_str, 0, tmp_val, 10);
  tmp_val = -999;
  (void) sscanf(tmp_str, "%d", &tmp_val);
  if ((tmp_val > -1) && (tmp_val < 201))
    m_ptr->bth  = tmp_val;
  (void) sprintf(tmp_str, "Current==%d  (0-200) Bows/Throwing == ",
		 m_ptr->bthb);
  tmp_val = strlen(tmp_str);
  prt(tmp_str, 0, 0);
  (void) get_string(tmp_str, 0, tmp_val, 10);
  tmp_val = -999;
  (void) sscanf(tmp_str, "%d", &tmp_val);
  if ((tmp_val > -1) && (tmp_val < 201))
    m_ptr->bthb = tmp_val;
  (void) sprintf(tmp_str, "Current==%d  Gold == ", m_ptr->au);
  tmp_val = strlen(tmp_str);
  prt(tmp_str, 0, 0);
  (void) get_string(tmp_str, 0, tmp_val, 10);
  tmp_val = -999;
  (void) sscanf(tmp_str, "%d", &tmp_val);
  if (tmp_val > -1)
    {
      m_ptr->au = tmp_val;
      prt_gold();
    }

  erase_line(MSG_LINE, 0);
  py_bonuses(blank_treasure, 0);
}


/* Wizard routine for creating objects			-RAK-	*/
wizard_create()
{
  int tmp_val;
  vtype tmp_str;
  register int flag;
  register treasure_type *i_ptr;
  register cave_type *c_ptr;
  char command;

  msg_print("Warning: This routine can cause fatal error.");
  /* make sure player sees the message */
  msg_print(" ");
  msg_flag = FALSE;
  i_ptr = &inventory[INVEN_MAX];
  prt("Name   : ", 0, 0);
  if (get_string(tmp_str, 0, 9, 80))
    (void) strcpy(i_ptr->name, tmp_str);
  else
    (void) strcpy(i_ptr->name, "& Wizard Object!");
  do
    {
      prt("Tval   : ", 0, 0);
      (void) get_string(tmp_str, 0, 9, 10);
      tmp_val = 0;
      (void) sscanf(tmp_str, "%d", &tmp_val);
      flag = TRUE;
      switch(tmp_val)
	{
	case 1: case 13: case 15 :	i_ptr->tchar = '~'; break;
	case 2: 	i_ptr->tchar = '&'; break;
	case 10:	i_ptr->tchar = '{'; break;
	case 11:	i_ptr->tchar = '{'; break;
	case 12:	i_ptr->tchar = '{'; break;
	case 20:	i_ptr->tchar = '}'; break;
	case 21:	i_ptr->tchar = '/'; break;
	case 22:	i_ptr->tchar = '\\'; break;
	case 23:	i_ptr->tchar = '|'; break;
	case 25:	i_ptr->tchar = '\\'; break;
	case 30:	i_ptr->tchar = ']'; break;
	case 31:	i_ptr->tchar = ']'; break;
	case 32:	i_ptr->tchar = '('; break;
	case 33:	i_ptr->tchar = ']'; break;
	case 34:	i_ptr->tchar = ')'; break;
	case 35:	i_ptr->tchar = '['; break;
	case 36:	i_ptr->tchar = '('; break;
	case 40:	i_ptr->tchar = '\''; break;
	case 45:	i_ptr->tchar = '='; break;
	case 55:	i_ptr->tchar = '_'; break;
	case 60:	i_ptr->tchar = '-'; break;
	case 65:	i_ptr->tchar = '-'; break;
	case 70: case 71:	i_ptr->tchar = '?'; break;
	case 75: case 76: case 77:	i_ptr->tchar = '!'; break;
	case 80:	i_ptr->tchar = ','; break;
	case 90:	i_ptr->tchar = '?'; break;
	case 91:	i_ptr->tchar = '?'; break;
	default:	flag = FALSE; break;
	}
    }
  while (!flag);
  i_ptr->tval = tmp_val;
  prt("Subval : ", 0, 0);
  (void) get_string(tmp_str, 0, 9, 10);
  tmp_val = 1;
  (void) sscanf(tmp_str, "%d", &tmp_val);
  i_ptr->subval = tmp_val;
  prt("Weight : ", 0, 0);
  (void) get_string(tmp_str, 0, 9, 10);
  tmp_val = 1;
  (void) sscanf(tmp_str, "%d", &tmp_val);
  i_ptr->weight = tmp_val;
  prt("Number : ", 0, 0);
  (void) get_string(tmp_str, 0, 9, 10);
  tmp_val = 1;
  (void) sscanf(tmp_str, "%d", &tmp_val);
  i_ptr->number = tmp_val;
  prt("Damage : ", 0, 0);
  (void) get_string(tmp_str, 0, 9, 5);
  (void) strcpy(i_ptr->damage, tmp_str);
  prt("+To hit: ", 0, 0);
  (void) get_string(tmp_str, 0, 9, 10);
  tmp_val = 0;
  (void) sscanf(tmp_str, "%d", &tmp_val);
  i_ptr->tohit = tmp_val;
  prt("+To dam: ", 0, 0);
  (void) get_string(tmp_str, 0, 9, 10);
  tmp_val = 0;
  (void) sscanf(tmp_str, "%d", &tmp_val);
  i_ptr->todam = tmp_val;
  prt("AC     : ", 0, 0);
  (void) get_string(tmp_str, 0, 9, 10);
  tmp_val = 0;
  (void) sscanf(tmp_str, "%d", &tmp_val);
  i_ptr->ac = tmp_val;
  prt("+To AC : ", 0, 0);
  (void) get_string(tmp_str, 0, 9, 10);
  tmp_val = 0;
  (void) sscanf(tmp_str, "%d", &tmp_val);
  i_ptr->toac = tmp_val;
  prt("P1     : ", 0, 0);
  (void) get_string(tmp_str, 0, 9, 10);
  tmp_val = 0;
  (void) sscanf(tmp_str, "%d", &tmp_val);
  i_ptr->p1 = tmp_val;
  prt("Flags (In HEX): ", 0, 0);
  i_ptr->flags = get_hex_value(0, 16, 8);
  prt("Cost : ", 0, 0);
  (void) get_string(tmp_str, 0, 9, 10);
  tmp_val = 0;
  (void) sscanf(tmp_str, "%d", &tmp_val);
  i_ptr->cost = tmp_val;

  prt("Level : ", 0, 0);
  (void) get_string(tmp_str, 0, 10, 10);
  tmp_val = 0;
  (void) sscanf(tmp_str, "%d", &tmp_val);
  i_ptr->level = tmp_val;

  if (get_com("Allocate? (Y/N)", &command))
    switch(command)
      {
      case 'y': case 'Y':
	popt(&tmp_val);
	t_list[tmp_val] = inventory[INVEN_MAX];
	c_ptr = &cave[char_row][char_col];
	if (c_ptr->tptr != 0)
	  (void) delete_object(char_row, char_col);
	c_ptr->tptr = tmp_val;
	msg_print("Allocated...");
	break;
      default:
	msg_print("Aborted...");
	break;
      }
  inventory[INVEN_MAX] = blank_treasure;
}
