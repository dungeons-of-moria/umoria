/* This program creates the initialized global data resources.		*/
/* It is compiled as an MPW tool.									*/

/* Monsters.c, treasur1.c, treasur2.c, variable.c are included.		*/
/* But, when this program is built, the compiler is instructed to	*/
/* actually include the initialized global data.  We just copy it	*/
/* into resource handles and dump them to the executable.			*/

/* Why do this?  MPW handles >32K global data ok (otherwise, this	*/
/* program could not compile).  However, we have to pay the price	*/
/* in efficiency.  The code hacks to keep the global data in		*/
/* resources are trivial, mainly redefining the variable to be a	*/
/* pointer instead of an array.  This scheme has the added			*/
/* advantage of providing a means for re-initialization of the		*/
/* global data.  MacMoria, which is "restartable" requires this.	*/

/* See macrsrc.c for a list of variables involved.					*/

#include <stdio.h>

#include <types.h>
#include <memory.h>

#include <dumpres.h>

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

	return(0);
}
