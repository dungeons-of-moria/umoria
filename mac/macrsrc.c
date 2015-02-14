/* mac/macrsrc.c: code for handling C data as resources

   Copyright (c) 1989-94 Curtis McCauley, James E. Wilson

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

#include <stdio.h>

#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"
#include "macrsrc.h"

restable_type restable[MAX_RESOURCES] = {
#ifndef RSRC_PART2
	{
		#ifdef RSRC
			(char *) object_list, "Object List",
		#else
			(char **) &object_list, NULL,
		#endif
		treasureRsrc, object_list_id,
		MAX_OBJECTS, sizeof(treasure_type),
		treasure_strproc,
		TRUE									},
#endif
#ifndef RSRC_PART1
	{
		#ifdef RSRC
			(char *) background, "Background Table",
		#else
			(char **) &background, NULL,
		#endif
		backgroundRsrc, background_id,
		MAX_BACKGROUND, sizeof(background_type),
		background_strproc,
		TRUE									},
	{
		#ifdef RSRC
			(char *) magic_spell, "Magic Spells",
		#else
			(char **) &magic_spell, NULL,
		#endif
		spellRsrc, magic_spell_id,
		(MAX_CLASS-1) * 31, sizeof(spell_type),
		NULL,
		TRUE									},
	{
		#ifdef RSRC
			(char *) player_title, "Player Titles",
		#else
			(char **) &player_title, NULL,
		#endif
		charPtrRsrc, player_title_id,
		MAX_CLASS * MAX_PLAYER_LEVEL, sizeof(char *),
		char_ptr_strproc,
		TRUE									},
	{
		#ifdef RSRC
			(char *) race, "Race List",
		#else
			(char **) &race, NULL,
		#endif
		raceRsrc, race_id,
		MAX_RACES, sizeof(race_type),
		race_strproc,
		TRUE									},
	{
		#ifdef RSRC
			(char *) c_list, "Creature List",
		#else
			(char **) &c_list, NULL,
		#endif
		monsterRsrc, c_list_id,
		MAX_CREATURES, sizeof(creature_type),
		creature_strproc,
		TRUE									},
	{
		#ifdef RSRC
			(char *) owners, "Owner List",
		#else
			(char **) &owners, NULL,
		#endif
		ownerRsrc, owners_id,
		MAX_OWNERS, sizeof(owner_type),
		owner_strproc,
		TRUE									},
	{
		#ifdef RSRC
			(char *) colors, "Colors",
		#else
			(char **) &colors, NULL,
		#endif
		charPtrRsrc, colors_id,
		MAX_COLORS, sizeof(char *),
		char_ptr_strproc,
		TRUE									},
	{
		#ifdef RSRC
			(char *) mushrooms, "Mushrooms",
		#else
			(char **) &mushrooms, NULL,
		#endif
		charPtrRsrc, mushrooms_id,
		MAX_MUSH, sizeof(char *),
		char_ptr_strproc,
		TRUE									},
	{
		#ifdef RSRC
			(char *) woods, "Woods",
		#else
			(char **) &woods, NULL,
		#endif
		charPtrRsrc, woods_id,
		MAX_WOODS, sizeof(char *),
		char_ptr_strproc,
		TRUE									},
	{
		#ifdef RSRC
			(char *) metals, "Metals",
		#else
			(char **) &metals, NULL,
		#endif
		charPtrRsrc, metals_id,
		MAX_METALS, sizeof(char *),
		char_ptr_strproc,
		TRUE									},
	{
		#ifdef RSRC
			(char *) rocks, "Rocks",
		#else
			(char **) &rocks, NULL,
		#endif
		charPtrRsrc, rocks_id,
		MAX_ROCKS, sizeof(char *),
		char_ptr_strproc,
		TRUE									},
	{
		#ifdef RSRC
			(char *) amulets, "Amulets",
		#else
			(char **) &amulets, NULL,
		#endif
		charPtrRsrc, amulets_id,
		MAX_AMULETS, sizeof(char *),
		char_ptr_strproc,
		TRUE									},
	{
		#ifdef RSRC
			(char *) syllables, "Syllables",
		#else
			(char **) &syllables, NULL,
		#endif
		charPtrRsrc, syllables_id,
		MAX_SYLLABLES, sizeof(char *),
		char_ptr_strproc,
		TRUE								}
#endif
};

#ifdef MACGAME
memtable_type memtable[MAX_PTRS] = {
	{ (char **) &cave,
		MAX_HEIGHT * MAX_WIDTH, sizeof(cave_type),
		TRUE										},
	{ (char **) &store,
		MAX_STORES, sizeof(store_type),
		TRUE										},
	{ (char **) &c_recall,
		MAX_CREATURES, sizeof(recall_type),
		TRUE										},
};
#endif

#ifndef RSRC_PART1
restart_type restart_vars[MAX_RESTART] = {

	/* treasure.c */
	{ (char *) &inven_ctr, 				sizeof(inven_ctr)				},
	{ (char *) &inven_weight, 			sizeof(inven_weight)			},
	{ (char *) &equip_ctr, 				sizeof(equip_ctr)				},

	/* variable.c */
	{ (char *) &weapon_heavy, 			sizeof(weapon_heavy)			},
	{ (char *) &pack_heavy, 			sizeof(pack_heavy)				},
	{ (char *) &total_winner, 			sizeof(total_winner)			},
	{ (char *) &character_generated,	sizeof(character_generated)		},
	{ (char *) &character_saved, 		sizeof(character_saved)			},
	{ (char *) &dun_level, 				sizeof(dun_level)				},
	{ (char *) &missile_ctr, 			sizeof(missile_ctr)				},
	{ (char *) &last_msg, 				sizeof(last_msg)				},
	{ (char *) &death, 					sizeof(death)					},
	{ (char *) &default_dir, 			sizeof(default_dir)				},
	{ (char *) &turn, 					sizeof(turn)					},
	{ (char *) &wizard, 				sizeof(wizard)					},
	{ (char *) &panic_save, 			sizeof(panic_save)				},
	{ (char *) &noscore, 				sizeof(noscore)					},
	{ (char *) &find_cut, 				sizeof(find_cut)				},
	{ (char *) &find_examine, 			sizeof(find_examine)			},
	{ (char *) &find_bound, 			sizeof(find_bound)				},
	{ (char *) &find_prself, 			sizeof(find_prself)				},
	{ (char *) &prompt_carry_flag, 		sizeof(prompt_carry_flag)		},
	{ (char *) &show_weight_flag, 		sizeof(show_weight_flag)		},
	{ (char *) &highlight_seams, 		sizeof(highlight_seams)			},
	{ (char *) &find_ignore_doors, 		sizeof(find_ignore_doors)		},
	{ (char *) &doing_inven, 			sizeof(doing_inven)				},
	{ (char *) &screen_change, 			sizeof(screen_change)			},
	{ (char *) &eof_flag,				sizeof (eof_flag)				},
	{ (char *) &wait_for_more, 			sizeof(wait_for_more)			},
	{ (char *) &spell_learned, 			sizeof(spell_learned)			},
	{ (char *) &spell_worked, 			sizeof(spell_worked)			},
	{ (char *) &spell_forgotten, 		sizeof(spell_forgotten)			},

	/* moved to variable.c from elsewhere */
	{ (char *) &last_command,			sizeof(last_command)			},
	{ (char *) &light_flag,				sizeof(light_flag)				}

};
#endif

#ifdef MACGAME
clrtable_type clrtable[MAX_CLRS] = {

	/* these are not initialized, but probably ought to be */

	/* treasure.c */
	{ (char *) &object_ident, 			sizeof(object_ident)			},
	{ (char *) &t_level, 				sizeof(t_level)					},
	{ (char *) &t_list, 				sizeof(t_list)					},
	{ (char *) &inventory, 				sizeof(inventory)				},

	/* monsters.c */
	{ (char *) &m_level, 				sizeof(m_level)					},
	{ (char *) &m_list, 				sizeof(m_list)					},

	/* variable.c */
	{ (char *) &panel_row, 				sizeof(panel_row)				},
	{ (char *) &panel_col, 				sizeof(panel_col)				},
	{ (char *) &panel_row_min, 			sizeof(panel_row_min)			},
	{ (char *) &panel_row_max, 			sizeof(panel_row_max)			},
	{ (char *) &panel_col_min, 			sizeof(panel_col_min)			},
	{ (char *) &panel_col_max, 			sizeof(panel_col_max)			},
	{ (char *) &panel_row_prt, 			sizeof(panel_row_prt)			},
	{ (char *) &panel_col_prt, 			sizeof(panel_col_prt)			},
	{ (char *) &py,						sizeof(py)						},
	{ (char *) &max_score,				sizeof (max_score)				}

};
#endif

/* String procs are used by the DumpRes package to make sure that	*/
/* char *'s imbedded in the stucts are dumped or loaded properly.	*/
/* All that is required is to pass a pointer to the pointer to the	*/
/* supplied subroutine for each char * field in the struct.			*/

void treasure_strproc(ptr, proc)
char *ptr;
void (*proc)(char **str);

{
	(*proc)(&((treasure_type *) ptr)->name);
	return;
}

void creature_strproc(ptr, proc)
char *ptr;
void (*proc)(char **str);

{
	(*proc)(&((creature_type *) ptr)->name);
	return;
}

void race_strproc(ptr, proc)
char *ptr;
void (*proc)(char **str);

{
	(*proc)(&((race_type *) ptr)->trace);
	return;
}

void background_strproc(ptr, proc)
char *ptr;
void (*proc)(char **str);

{
	(*proc)(&((background_type *) ptr)->info);
	return;
}

void owner_strproc(ptr, proc)
char *ptr;
void (*proc)(char **str);

{
	(*proc)(&((owner_type *) ptr)->owner_name);
	return;
}

/* This is for arrays of char *'s, which can be treated essentially	*/
/* as structs with a single field - the char *.  It is inefficient,	*/
/* since the array of char *'s, garbage data, is created as a		*/
/* resource along with the STR# resource, but has the advantage of	*/
/* not requiring special casing.									*/

void char_ptr_strproc(ptr, proc)
char *ptr;
void (*proc)(char **str);

{
	(*proc)((char **) ptr);
	return;
}
