/* mac/dumpres/TestDR.c: test driver for resource dumping code

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

#include <StdIO.h>

#ifndef THINK_C
#include <Resources.h>
#endif

#include "DumpRes.h"

typedef struct {
	char *name;
	int x, y, z;
} Test;

Test table[5] = {
	{ "Huron", 1, 2, 3 },
	{ "Ontario", 4, 5, 6 },
	{ "Michigan", 7, 8, 9 },
	{ "Erie", 10, 11, 12 },
	{ "Superior", 13, 14, 15 }
};

Test *data;

void MyStrProc(ptr, proc)
char *ptr;
void (*proc)(char **);

{
	(*proc)(&((Test *) ptr)->name);
	return;
}

main()

{
	short resFile;
	long i, rc;

	DumpRes(
		"DumpResTest.rsrc",
		'TEST', 256, "Test Data", 0,
		(char *) table, 5, sizeof(Test),
		MyStrProc
	);

	resFile = OpenResFile("\pDumpResTest.rsrc");

	if (resFile != -1) {

		data = NULL;

		rc = LoadRes(
			(char **) &data),
			'TEST', 256,
			5, sizeof(Test),
			MyStrProc
		);

		if (rc)
			for (i = 0; i < 5; i++)
				fprintf(stderr, "%s : %d %d %d\n",
					data[i].name, data[i].x, data[i].y,
					data[i].z);

		else
			fprintf(stderr, "LoadRes failed.\n");

		CloseResFile(resFile);

	}

	else
		fprintf(stderr, "Unable to open file for LoadRes testing.\n");

	return(0);
}
