/* util/mc/st.h: definitions for the symbol table module
 *
 * Copyright (c) 1989 by Joseph Hall.
 * All rights reserved except as stated below.
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

#ifndef _ST_H_
#define	_ST_H_

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/*
 * Needs generic_t
 */

#include "generic.h"

/*
 * maximum length of symbol strings
 */

#define ST_MAX_SYM_LEN		32

/*
 * Struct for individual entries
 */

typedef	struct st_EntryStruct {

    char	name[ST_MAX_SYM_LEN];	/* symbol string		    */
    int		type;			/* symbol type			    */
    struct st_EntryStruct *next_P;	/* next entry in chain		    */
    generic_t	gval;			/* symbol value			    */

} st_Entry_t, *st_Entry_Pt;

/*
 * Struct for table header
 */

typedef struct {

    char	name[ST_MAX_SYM_LEN];
				    /* table name			    */
    int		size,		    /* size of table to be allocated	    */
		entryCt;	    /* # of entries in table currently	    */
    st_Entry_Pt	*tab_A;		    /* vector of ptrs to entries	    */

} st_Table_t, *st_Table_Pt;

#define	ST_SYM_FOUND	    0
#define ST_OK		    0
#define ST_SYM_NOT_FOUND    1
#define ST_NULL_TABLE	    2

#define ST_MAX_INPUT_LEN    256

/*
 * functions defined in st_symtab.c
 */

extern st_Table_Pt St_NewTable();
extern void St_DelTable();
extern int St_GetSym();
extern int St_DefSym();
extern int St_ReplSym();
extern int St_DelSym();
extern st_Table_Pt St_GetTable();
extern void St_DumpTable();
extern char **St_ListTable();
extern char **St_SListTable();
extern int St_TableSize();

#endif /* _ST_H_ */

