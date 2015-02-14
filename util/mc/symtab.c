/* util/mc/st_symtab.c: routines for managing symbol tables
 *
 * Copyright (c) 1989 by Joseph Hall.
 
   This file is part of Umoria.

   Umoria is free software; you can redistribute it and/or modify 
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Umoria is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of 
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Genral Public License for more details.

   You should have received a copy of the GNU General Public License 
   along with Umoria.  If not, see <http://www.gnu.org/licenses/>. */

/* This file was also released as part of an independent program, and
   may be redistributed and/or modified under the authors' original terms,
   which are given below.*/

/* All rights reserved except as stated below.
 *
 * Jim Wilson and any other holders of copyright on substantial portions
 * of Moria are granted rights to use, modify, and distribute this program
 * as they see fit, so long as the terms of its use, modification and/or
 * distribution are no less restrictive than those applying to Moria,
 * version 5.0 or later, itself, and so long as this use is related to
 * the further development of Moria.
 *
 * Anyone having any other use in mind for this code should contact the
 * author at 4116 Brewster Dr., Raleigh NC 27606 (jnh@ecemwl.ncsu.edu).
 */


#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifdef ANSI_LIBS
#include <stdlib.h>
#else
extern char *malloc();
extern char *calloc();
extern void free();
extern void cfree();
#endif

#include "st.h"

/* #define _TESTING_ */

/*
 * There's a symbol table containing all the names of the symbol tables,
 * and of course it contains its own name ...
 *
 * "tnt" stands for "Table Name Table" ...
 */
static st_Table_Pt  tnt_P = NULL;

/*
 * If there are a lot of symbol tables around you might want to increase
 * TNT_SIZE.  For best results use a prime number.
 */

#define	TNT_SIZE    11

/*
 * Names of basic generic types.
 */

#define NAMES 7

/*
 * Hash--
 *	Simple, stupid hash function.  Why be great, you know, when you can
 * be adequate so easily?
 */
static int Hash(s, mod)
register char *s;
int mod;
{
    register int h = 0;

    while (*s)
	h += (unsigned) *s++;

    return h % mod;
}


/*
 * LookupSym--
 *	Search for a name in a table.  Returns NULL if not found.
 */
static st_Entry_Pt LookupSym(st_P, name)
st_Table_Pt st_P;
char *name;
{
    st_Entry_Pt	entry_P;
    int		h;

    h = Hash(name, st_P->size);

    for (entry_P = st_P->tab_A[h]; entry_P; entry_P = entry_P->next_P)
	if (!strncmp(entry_P->name, name, ST_MAX_SYM_LEN))
	    break;

    return entry_P;
}



/*
 * AddSym--
 *	Add a name to a table and return a pointer to the new entry.
 * ASSUMES THAT NAME DOESN'T EXIST ALREADY IN TABLE (check with LookupSym
 * above before calling here).
 */
static st_Entry_Pt AddSym(st_P, name)
st_Table_Pt st_P;
char *name;

{
    int		h;
    st_Entry_Pt	new_P;

    h = Hash(name, st_P->size);

    new_P = (st_Entry_Pt) malloc(sizeof(st_Entry_t));
     strncpy(new_P->name, name, ST_MAX_SYM_LEN - 1);
    new_P->name[ST_MAX_SYM_LEN - 1] = 0;
    new_P->next_P = st_P->tab_A[h];
    st_P->tab_A[h] = new_P;

    st_P->entryCt++;

    return new_P;

}


/*
 * St_NewTable--
 *	Create a new symbol table header.  Returns NULL if name isn't
 * unique with respect to the symbol tables currently in existence.
 */
st_Table_Pt St_NewTable(name, size)
char *name;
int size;
{
    st_Table_Pt	st_P;
    generic_t	gval;

    /*
     * Create the name table if doesn't already exist.  Obviously we
     * can't use St_NewTable for this ...
     */

    if (!tnt_P) {
	tnt_P = (st_Table_Pt) malloc(sizeof(st_Table_t));

	 strncpy(tnt_P->name, "_TNT_", ST_MAX_SYM_LEN - 1);
	tnt_P->name[ST_MAX_SYM_LEN - 1] = 0;
	tnt_P->size = TNT_SIZE;
	tnt_P->tab_A = (st_Entry_Pt *) calloc(TNT_SIZE, sizeof(st_Entry_Pt));
	tnt_P->entryCt = 0;

	gval.v = (char *) tnt_P;
	 St_DefSym(tnt_P, "_TNT_", GEN_TYPE_VOID_PTR, gval);

    }

    /*
     * See if the new table name is unique.
     */

    if (LookupSym(tnt_P, name))
	return NULL;

    /*
     * Create the new table.
     */

    st_P = (st_Table_Pt) malloc(sizeof(st_Table_t));

     strncpy(st_P->name, name, ST_MAX_SYM_LEN - 1);
    st_P->name[ST_MAX_SYM_LEN - 1] = 0;
    st_P->size = size;
    st_P->tab_A = (st_Entry_Pt *) calloc((unsigned) size,
					 sizeof(st_Entry_Pt));
    st_P->entryCt = 0;

    /*
     * Add the name of the new table to the "table name table" now.
     * gval.v is a pointer to the new table.
     */

    gval.v = (char *) st_P;
     St_DefSym(tnt_P, name, GEN_TYPE_VOID_PTR, gval);

    return st_P;
}




/*
 * St_DelTable--
 *	Delete a symbol table and associated storage.  If entries in the
 * table point to dynamically allocated objects, the user must free these
 * objects before calling this routine, else the pointers to those objects
 * will be lost.  (** NOTE: this feature has been removed from the version
 * accompanying the monster compiler, since it isn't needed --jnh **)
 */
void St_DelTable(st_P)
st_Table_Pt st_P;
{
    st_Entry_Pt entry_P;
    int		i;

    if (!st_P)
	return;

    for (i = 0; i < st_P->size; i++)
	for (entry_P = st_P->tab_A[i]; entry_P; entry_P = entry_P->next_P) {
	    free((char *) entry_P);
	}

    if (strncmp(st_P->name, "_TNT_", ST_MAX_SYM_LEN))
	 St_DelSym(tnt_P, st_P->name);

    cfree((char *) st_P->tab_A);
    cfree((char *) st_P);

    return;

}




/*
 * St_ListTable--
 *	Returns an unsorted list of symbols in the table.  The list will be
 * terminated with a NULL pointer.  This routine frees the storage used by
 * the last call to St_ListTable or St_SListTable; a call with a NULL
 * argument is a convenient way to free storage allocated by a previous call.
 */
char **St_ListTable(st_P)
st_Table_Pt st_P;
{

    st_Entry_Pt entry_P;
    int		i, j;
    static char	**list_A = NULL,
		*chars_P = NULL;

    if (list_A) {
	free((char *) list_A);
	free((char *) chars_P);
	list_A = NULL;
	chars_P = NULL;
    }

    if (!st_P)
	return NULL;

    list_A = (char **) malloc(sizeof(char *) * (st_P->entryCt + 1));
    chars_P = (char *) malloc(sizeof(char) * ST_MAX_SYM_LEN * st_P->entryCt);

    for (i = 0; i < st_P->entryCt; i++)
	list_A[i] = chars_P + ST_MAX_SYM_LEN * i;
    list_A[st_P->entryCt] = NULL;

    j = 0;
    for (i = 0; i < st_P->size; i++)
	for (entry_P = st_P->tab_A[i]; entry_P; entry_P = entry_P->next_P)
	     strcpy(list_A[j++], entry_P->name);

    list_A[st_P->entryCt] = NULL;

    return list_A;

}


/*
 * St_SListTable--
 *	Returns a sorted list of symbols in a table.  Otherwise is exactly
 * like St_ListTable, above.
 */
char **St_SListTable(st_P)
st_Table_Pt st_P;
{
    char    **list_A;

    if (!(list_A = St_ListTable(st_P)))
	return NULL;

    qsort(*list_A, st_P->entryCt, sizeof(char) * ST_MAX_SYM_LEN, strcmp);

    return list_A;
}



/*
 * St_GetSym--
 *	Look for a symbol in a table.  Return type and ptr to val if found.
 */
int St_GetSym(st_P, name, type_P, gval_P)
st_Table_Pt st_P;
char *name;
int *type_P;
generic_Pt gval_P;
{
    st_Entry_Pt	entry_P;

    if (!st_P)
	return ST_NULL_TABLE;

    if (!(entry_P = LookupSym(st_P, name)))
	return ST_SYM_NOT_FOUND;

    *type_P = entry_P->type;
    *gval_P = entry_P->gval;

    return ST_SYM_FOUND;
}



/*
 * St_DefSym--
 *	Add a symbol to a table.  Returns ST_SYM_FOUND and does nothing if
 * name is already in table.
 */
int St_DefSym(st_P, name, type, gval)
st_Table_Pt st_P;
char *name;
int type;
generic_t gval;
{
    st_Entry_Pt entry_P;

    if (!st_P)
	return ST_NULL_TABLE;

    if (LookupSym(st_P, name))
	return ST_SYM_FOUND;

    entry_P = AddSym(st_P, name);

    /*
     * Assign data.
     */

    entry_P->type = type;
    entry_P->gval = gval;

    return ST_SYM_NOT_FOUND;

}


/*
 * St_ReplSym--
 *	Add or supersede a symbol in a table.
 */
int St_ReplSym(st_P, name, type, gval)
st_Table_Pt st_P;
char *name;
int type;
generic_t gval;
{
    st_Entry_Pt entry_P;
    int		status;

    if (!st_P)
	return ST_NULL_TABLE;

    if (!(entry_P = LookupSym(st_P, name))) {
	entry_P = AddSym(st_P, name);
	status = ST_SYM_NOT_FOUND;
    } else {
	status = ST_SYM_FOUND;
    }

    /*
     * Assign data.
     */

    entry_P->type = type;
    entry_P->gval = gval;

    return status;
}




/*
 * St_DelSym--
 *	Delete a symbol from the table.
 */
int St_DelSym(st_P, name)
st_Table_Pt st_P;
char *name;
{
    st_Entry_Pt	entry_P, last_P;
    int		h;

    if (!st_P)
	return ST_NULL_TABLE;

    h = Hash(name, st_P->size);

    for (last_P = NULL, entry_P = st_P->tab_A[h]; entry_P;
				last_P = entry_P, entry_P = entry_P->next_P)
	if (!strncmp(entry_P->name, name, ST_MAX_SYM_LEN))
	    break;

    if (!entry_P)
	return ST_SYM_NOT_FOUND;

    if (last_P)
	last_P->next_P = entry_P->next_P;
    else
	st_P->tab_A[h] = NULL;

    cfree((char *) entry_P);
    st_P->entryCt--;

    return ST_SYM_FOUND;
}



/*
 * St_GetTable--
 *	Get a table by name
 */
st_Table_Pt St_GetTable(name)
char *name;
{
    int		type;
    generic_t	gval;

    if (!tnt_P)
	return NULL;

    if (St_GetSym(tnt_P, name, &type, &gval) != ST_SYM_FOUND)
	return NULL;

    return (st_Table_Pt) gval.v;
}


/* -Jim Wilson-
 * St_TableSize--
 *	Returns the number of entries in the table.
 */
int St_TableSize(st_P)
st_Table_Pt st_P;
{
  return st_P->entryCt;
}


/*
 * St_DumpTable--
 *	Dump a table (for debugging or utility purposes)
 */
void St_DumpTable(output_F, st_P)
FILE *output_F;
st_Table_Pt st_P;
{
    st_Entry_Pt	entry_P;
    int		bucket;

    if (!st_P) {
	 fprintf(output_F, "Table ptr is NULL.\n");
	return;
    }

     fprintf(output_F, "Dumping table '%s', size = %d, count = %d\n",
				st_P->name, st_P->size, st_P->entryCt);

    for (bucket = 0; bucket < st_P->size; bucket++) {

	 fprintf(output_F, " Bucket %d:\n", bucket);

	entry_P = st_P->tab_A[bucket];

	if (!entry_P) {
	     fprintf(output_F, "  empty\n");
	    continue;
	}

	while (entry_P) {

	    switch(entry_P->type) {

	    case GEN_TYPE_INT:
		 fprintf(output_F, "  '%s' = %d (int)\n", entry_P->name,
							entry_P->gval.i);
		break;

	    case GEN_TYPE_LONG:
		 fprintf(output_F, "  '%s' = %ld (long)\n", entry_P->name,
							entry_P->gval.l);
		break;

	    case GEN_TYPE_FLOAT:
		 fprintf(output_F, "  '%s' = %e (float)\n", entry_P->name,
							entry_P->gval.f);
		break;

	    case GEN_TYPE_DOUBLE:
		 fprintf(output_F, "  '%s' = %e (double)\n",
				entry_P->name, entry_P->gval.d);
		break;

	    case GEN_TYPE_CHAR:
		 fprintf(output_F, "  '%s' = '%c'/%d (char)\n",
					    entry_P->name,
					    entry_P->gval.c, entry_P->gval.c);
		break;

	    case GEN_TYPE_STRING:
		if (entry_P->gval.s)
		     fprintf(output_F, "  '%s' = '%s' (string)\n",
					entry_P->name, entry_P->gval.s);
		else
		     fprintf(output_F, "  '%s' = NULL (string)\n",
							    entry_P->name);
		break;

	    case GEN_TYPE_STRING_A:

		if (!entry_P->gval.s_A) {
		     fprintf(output_F, "  '%s' = NULL (string array)\n",
							    entry_P->name);
		} else {
		    char	**s_A;
		     fprintf(output_F, "  '%s' is string array:\n",
							    entry_P->name);
		    for (s_A = entry_P->gval.s_A; *s_A; s_A++)
			 fprintf(output_F, "    '%s'\n", *s_A);
		}
		break;

	    case GEN_TYPE_VOID_PTR:
		if (entry_P->gval.v)
		     fprintf(output_F, "  '%s' is user type (void ptr)\n",
							    entry_P->name);
		else
	 fprintf(output_F, "  '%s' is NULL user type (void ptr)\n",
							    entry_P->name);
		break;

	    default:
	 fprintf(output_F, "  '%s' is unknown type\n", entry_P->name);
		break;

	    }

	    entry_P = entry_P->next_P;

	}

    }

    return;
}



#ifdef _TESTING_

main()
{
    st_Table_Pt	st_P;
    generic_t	gval;
    int		type;
    static char *s_A[] = {"Joe", "Bloe", NULL};
    char	**list_A;

    st_P = St_NewTable("Test", 3);

    gval.i = 10;
     St_DefSym(st_P, "A", GEN_TYPE_INT, gval);

    gval.d = 3.14;
     St_DefSym(st_P, "PI", GEN_TYPE_DOUBLE, gval);

    gval.i = 1;
     St_DefSym(st_P, "ONE", GEN_TYPE_INT, gval);

    gval.s = "Testing!";
     St_DefSym(st_P, "TESTING", GEN_TYPE_STRING, gval);

    gval.c = 7;
     St_DefSym(st_P, "BELL", GEN_TYPE_CHAR, gval);

    gval.s_A = s_A;
     St_DefSym(st_P, "JOE BLOE", GEN_TYPE_STRING_A, gval);

     St_GetSym(st_P, "A", &type, &gval);
     printf("A = %d, type = %d\n", gval.i, type);

     St_GetSym(st_P, "PI", &type, &gval);
     printf("PI = %f, type = %d\n", gval.d, type);

    St_DumpTable(stdout, St_GetTable("Test"));

     St_DelSym(st_P, "TESTING");

    St_DumpTable(stdout, St_GetTable("Test"));

     St_DelSym(st_P, "PI");

    gval.s = "Joe Bloe";
     St_ReplSym(st_P, "JOE BLOE", GEN_TYPE_STRING, gval);
    gval.s = "Jane Bloe";
     St_ReplSym(st_P, "JANE BLOE", GEN_TYPE_STRING, gval);

    St_DumpTable(stdout, St_GetTable("Test"));

    list_A = St_ListTable(St_GetTable("Test"));

    while (*list_A)
	 printf("'%s'\n", *list_A++);

    list_A = St_SListTable(St_GetTable("Test"));

    while (*list_A)
	 printf("'%s'\n", *list_A++);

    return 0;
}

#endif
