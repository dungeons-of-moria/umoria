#include <StdIO.h>
#include <String.h>
#include <Strings.h>
#include <SetJmp.h>

#include <Types.h>
#include <Memory.h>
#include <OSUtils.h>
#include <Resources.h>
#include <Files.h>

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

static unsigned strLen;
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

void DumpRes(fileName, resType, resID, resName, resAttrs, elemPtr, elemCnt, elemSiz, strProc)
char *fileName;
long resType;
int resID;
char *resName;
int resAttrs;
char *elemPtr;
unsigned elemCnt, elemSiz;
void (*strProc)(char *elem, void (*proc)(char **str));

{
	Handle dataHandle, strHandle, oldHandle;
	OSErr err;
	Str255 pFileName, pResName;
	short resFile;
	char *anElem;
	int errno;
	unsigned i;

	if ((errno = setjmp(abort)) != 0) {
		fprintf(stderr, "Error dumping to resource %s to file %s.\n", resName, fileName);
		fprintf(stderr, "%s\n", errtab[errno - 1]);
		exit(1);
	}

	strncpy(pFileName, fileName, 255);
	pFileName[255] = '\0';
	c2pstr(pFileName);

	strncpy(pResName, resName, 255);
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

int LoadRes(memPtr, resType, resID, elemCnt, elemSiz, strProc)
char **memPtr;
int resType;
int resID;
unsigned elemCnt, elemSiz;
void (*strProc)(char *elem, void (*proc)(char **str));

{
	Handle dataHandle, strHandle;
	unsigned dataLen, strLen;
	char *elemPtr, *anElem;
	int i;

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
