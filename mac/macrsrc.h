/* mac/macrsrc.h: definitions for code handling C data as resources

   Copyright (c) 1989-1991 Curtis McCauley, James E. Wilson

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

#define treasureRsrc		'TREA'
#define monsterRsrc			'MONS'
#define spellRsrc			'SPLL'
#define charPtrRsrc			'CPTR'
#define raceRsrc			'RACE'
#define backgroundRsrc		'BKGD'
#define ownerRsrc			'OWNR'

#define restartRsrc			'RSRT'

#define stringRsrc			'STR#'

#define object_list_id		128
#define c_list_id			129
#define magic_spell_id		130
#define race_id				131
#define background_id		132
#define owners_id			133
#define player_title_id		134
#define colors_id			135
#define mushrooms_id		136
#define woods_id			137
#define metals_id			138
#define rocks_id			139
#define amulets_id			140
#define syllables_id		141

#define restart_id			150

typedef struct restable_type {
	#ifdef RSRC
		char *memPtr;
	#else
		char **memPtr;
	#endif
	char *resName;
	long resType, resID;
	unsigned long elemCnt, elemSiz;
	void (*strProc)(char *ptr, void (*proc)(char **str));
	int restartFlag;
} restable_type;

/* THINK C can't handle data > 32K, so we need to do this in 2 parts.  */

#ifdef RSRC_PART1
#define MAX_RESOURCES			1
#else
#ifdef RSRC_PART2
#define MAX_RESOURCES			13
#else
#define MAX_RESOURCES			14
#endif
#endif

extern restable_type restable[MAX_RESOURCES];

typedef struct memtable_type {
	char **memPtr;
	unsigned long elemCnt, elemSiz;
	int restartFlag;
} memtable_type;

#define MAX_PTRS				3

extern memtable_type memtable[MAX_PTRS];

typedef struct restart_type {
	char *ptr;
	unsigned long size;
} restart_type;

#define MAX_RESTART				34

extern restart_type restart_vars[MAX_RESTART];

typedef struct clrtable_type {
	char *ptr;
	unsigned long size;
} clrtable_type;

#define MAX_CLRS				16

extern clrtable_type clrtable[MAX_CLRS];

void treasure_strproc(char *ptr, void (*proc)(char **str));
void creature_strproc(char *ptr, void (*proc)(char **str));
void race_strproc(char *ptr, void (*proc)(char **str));
void background_strproc(char *ptr, void (*proc)(char **str));
void owner_strproc(char *ptr, void (*proc)(char **str));
void char_ptr_strproc(char *ptr, void (*proc)(char **str));
