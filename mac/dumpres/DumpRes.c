/* mac/dumpres/DumpRes.c: convert C data to resources and dump to file

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
#include <String.h>
#ifndef THINK_C
#include <Strings.h>
#endif
#include <SetJmp.h>

#ifndef THINK_C
#include <Types.h>
#include <Memory.h>
#include <OSUtils.h>
#include <Resources.h>
#include <Files.h>
#endif

#include "DumpRes.h"

#ifdef THINK_C
#define p2cstr(x)	(char *)PtoCstr((char *)x)
#define c2pstr(x)	(char *)CtoPstr((char *)x)
#endif

#define rsrcCreator					'RSED'
#define rsrcType					'rsrc'

#define strType						'STR#'

#define errDataHandle				1
#define errStrHandle				2
#define errStrLen					3
#define errOpenResFile				4
#define errAddData					5
#define errAddStr					6
#define errCloseResFile				7

static char *errtab[7] = {
	"Unable to move data into handle",
	"Unable to allocate string handle",
	"String too long",
	"Failure opening resource file",
	"Failure adding data resource",
	"Failure adding string resource",
	"Failure closing resource file"
};

static jmp_buf abort;

static unsigned long strLen;
static short strCount;
static char *strLoc;

static void SumStr(ptr)
char **ptr;

{
	strLen += strlen(*ptr) + 1;
	return;
}

static void AddStr(ptr)
char **ptr;

{
	unsigned len;

	len = strlen(*ptr);
	if (len > 255) longjmp(abort, errStrLen);

	strcpy(strLoc, *ptr);
	c2pstr(strLoc);

	strLoc += len + 1;

	strCount++;

	return;
}

static void NxtString(ptr)
char **ptr;

{
	*ptr = strLoc;
	strLoc += strlen(p2cstr(strLoc)) + 1;

	return;
}

void DumpRes(fileName, resType, resID, resName, resAttrs, elemPtr, elemCnt,
	     elemSiz, strProc)
char *fileName;
long resType;
long resID;
char *resName;
long resAttrs;
char *elemPtr;
unsigned long elemCnt, elemSiz;
void (*strProc)(char *elem, void (*proc)(char **str));

{
	Handle dataHandle, strHandle, oldHandle;
	OSErr err;
	Str255 pFileName, pResName;
	short resFile;
	char *anElem;
	long errno;
	unsigned long i;

	if ((errno = setjmp(abort)) != 0) {
		fprintf(stderr, "Error dumping to resource %s to file %s.\n",
			resName, fileName);
		fprintf(stderr, "%s\n", errtab[errno - 1]);
		exit(1);
	}

	strncpy((char *)pFileName, fileName, 255);
	pFileName[255] = '\0';
	c2pstr(pFileName);

	strncpy((char *)pResName, resName, 255);
	pResName[255] = '\0';
	c2pstr(pResName);

	err = PtrToHand(elemPtr, &dataHandle, elemCnt * elemSiz);
	if (err != noErr) longjmp(abort, errDataHandle);

	if (strProc != NULL) {

		strLen = sizeof(short);
		anElem = elemPtr;
		for (i = 0; i < elemCnt; i++) {
			(*strProc)(anElem, SumStr);
			anElem += elemSiz;
		}

		strHandle = NewHandle(strLen);
		if (MemError() != noErr) longjmp(abort, errStrHandle);

		strCount = 0;
		strLoc = (char *) *strHandle + sizeof(short);
		anElem = elemPtr;
		for (i = 0; i < elemCnt; i++) {
			(*strProc)(anElem, AddStr);
			anElem += elemSiz;
		}

		*((short *) *strHandle) = strCount;

	}

	else
		strHandle = NULL;

	(void) Create(pFileName, 0, rsrcCreator, rsrcType);
	CreateResFile(pFileName);

	resFile = OpenResFile(pFileName);
	if (resFile == -1) longjmp(abort, errOpenResFile);

	oldHandle = Get1Resource(resType, (short) resID);
	if (oldHandle != NULL) {
		RmveResource(oldHandle);
		DisposHandle(oldHandle);
	}

	oldHandle = Get1Resource(strType, (short) resID);
	if (oldHandle != NULL) {
		RmveResource(oldHandle);
		DisposHandle(oldHandle);
	}

	AddResource(dataHandle, resType, (short) resID, pResName);
	if (ResError() != noErr) longjmp(abort, errAddData);

	SetResAttrs(dataHandle, resAttrs | resChanged);

	if (strHandle != NULL) {

		AddResource(strHandle, strType, (short) resID, pResName);
		if (ResError() != noErr) longjmp(abort, errAddStr);

		SetResAttrs(strHandle, resAttrs | resChanged);

	}

	CloseResFile(resFile);
	if (ResError() != noErr) longjmp(abort, errCloseResFile);

	return;
}

long LoadRes(memPtr, resType, resID, elemCnt, elemSiz, strProc)
char **memPtr;
long resType;
long resID;
unsigned long elemCnt, elemSiz;
void (*strProc)(char *elem, void (*proc)(char **str));

{
	Handle dataHandle, strHandle;
	unsigned long dataLen, strLen;
	char *elemPtr, *anElem;
	long i;

	dataHandle = GetResource(resType, resID);
	if (dataHandle == NULL) return(false);

	MoveHHi(dataHandle);
	HLock(dataHandle);

	dataLen = GetHandleSize(dataHandle);

	if (strProc != NULL) {

		strHandle = GetResource(strType, resID);
		if (strHandle == NULL) return(false);

		MoveHHi(strHandle);
		HLock(strHandle);

		strLen = GetHandleSize(strHandle);

	}

	else {

		strHandle = NULL;
		strLen = 0;

	}

	if (*memPtr == NULL) {

		elemPtr = NewPtr(dataLen + strLen);
		if (elemPtr == NULL) return(false);

	}

	else {

		elemPtr = *memPtr;
		if (GetPtrSize(elemPtr) < (dataLen + strLen)) return(false);

	}

	if (strHandle != NULL) {

		BlockMove(*strHandle, elemPtr + dataLen, strLen);

		HUnlock(strHandle);
		ReleaseResource(strHandle);

	}

	BlockMove(*dataHandle, elemPtr, dataLen);

	HUnlock(dataHandle);
	ReleaseResource(dataHandle);

	if (strHandle != NULL) {
		strLoc = elemPtr + dataLen + sizeof(short);
		anElem = elemPtr;
		for (i = 0; i < elemCnt; i++) {
			(*strProc)(anElem, NxtString);
			anElem += elemSiz;
		}
	}

	*memPtr = elemPtr;

	return(true);
}
