/* mac/dumpres/DumpRes.h: prototypes for DumpRes.c

   Copyright (c) 1989-1991 Curtis McCauley, James E. Wilson

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

void DumpRes(
	char *fileName,
	long resType, long resID, char *resName, long resAttrs,
	char *elemPtr, unsigned long elemCnt, unsigned long elemSiz,
	void (*strProc)(char *elem, void (*proc)(char **str))
);

long LoadRes(
	char **memPtr,
	long resType, long resID,
	unsigned long elemCnt, unsigned long elemSiz,
	void (*strProc)(char *elem, void (*proc)(char **str))
);
