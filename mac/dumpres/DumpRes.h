/* mac/dumpres/DumpRes.h: prototypes for DumpRes.c

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
