#include <StdIO.h>

#include <Resources.h>

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
	int i, rc;

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
			&((char *) data),
			'TEST', 256,
			5, sizeof(Test),
			MyStrProc
		);

		if (rc)
			for (i = 0; i < 5; i++)
				fprintf(stderr, "%s : %d %d %d\n", data[i].name, data[i].x, data[i].y, data[i].z);

		else
			fprintf(stderr, "LoadRes failed.\n");

		CloseResFile(resFile);

	}

	else
		fprintf(stderr, "Unable to open file for LoadRes testing.\n");

	return(0);
}
