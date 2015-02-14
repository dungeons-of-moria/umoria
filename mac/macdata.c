/* mac/macdata.c: this program creates the initialized global data resources

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

/* Monsters.c, player.c, tables.c, treasure.c, and variables.c are included.
   But, when this program is built, the compiler is instructed to
   actually include the initialized global data.  We just copy it
   into resource handles and dump them to the executable.	*/

/* Why do this?  MPW handles >32K global data ok (otherwise, this
   program could not compile).  However, we have to pay the price
   in efficiency.  The code hacks to keep the global data in
   resources are trivial, mainly redefining the variable to be a
   pointer instead of an array.  This scheme has the added
   advantage of providing a means for re-initialization of the
   global data.  MacMoria, which is "restartable" requires this.	*/

/* See macrsrc.c for a list of variables involved.			*/

#include <stdio.h>

#ifndef THINK_C
#include <types.h>
#include <memory.h>

#include <dumpres.h>
#else
#include <console.h>

#include "DumpRes.h"
#endif

#include "config.h"
#include "macrsrc.h"

#define RES_ATTRS				0

main(argc, argv)
int argc;
char *argv[];

{
	int i;
	unsigned size, temp;
	char *p, *q;
	restable_type *r;

#ifdef THINK_C
	argc = ccommand(&argv);
#endif

	if (argc != 2) {
		fprintf(stderr, "### %s: Usage - %s targetFileName\n", argv[0], argv[0]);
		return(1);
	}

	for (i = 0, r = restable; i < MAX_RESOURCES; i++, r++) {

		DumpRes(
			argv[1],
			r->resType, r->resID, r->resName, RES_ATTRS,
			r->memPtr, r->elemCnt, r->elemSiz,
			r->strProc
		);

	}

#ifndef RSRC_PART1
	size = 0;
	for (i = 0; i < MAX_RESTART; i++) size += restart_vars[i].size;

	p = NewPtr(size);
	if (p == NULL) {
		fprintf(stderr, "Cannot get %d bytes for restart vars.\n", size);
		exit(1);
	}

	q = p;
	for (i = 0; i < MAX_RESTART; i++) {
		BlockMove(restart_vars[i].ptr, q, restart_vars[i].size);
		q += restart_vars[i].size;
	}

	DumpRes(
		argv[1],
		restartRsrc, restart_id, "Restart Vars", RES_ATTRS,
		p, 1, size,
		NULL
	);
#endif

#ifndef THINK_C
	printf("restable info --\n");
	for (i = 0, size = 0; i < MAX_RESOURCES; ++i) {
		size += (temp = restable[i].elemCnt * restable[i].elemSiz);
		printf("%3d  %8d\n", i, temp);
	}
	printf("     --------\n");
	printf("     %8d\n\n", size);

	printf("memtable info --\n");
	for (i = 0, size = 0; i < MAX_PTRS; ++i) {
		size += (temp = memtable[i].elemCnt * memtable[i].elemSiz);
		printf("%3d  %8d\n", i, temp);
	}
	printf("     --------\n");
	printf("     %8d\n\n", size);
#endif

	return(0);
}
