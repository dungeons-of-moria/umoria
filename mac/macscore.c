/* mac/macscore.c: scorefile code

   Copyright (c) 1989-1991 Curtis McCauley, James E. Wilson

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include <stdio.h>

#ifndef THINK_C
#include <Types.h>
#include <Resources.h>
#include <Events.h>
#include <Quickdraw.h>
#include <Fonts.h>
#include <Controls.h>
#include <Dialogs.h>
#include <TextEdit.h>
#include <Lists.h>
#include <Packages.h>
#include <Files.h>
#include <SysEqu.h>
#include <Errors.h>

#include <String.h>
#include <Strings.h>

#include <ScrnMgr.h>

#else

#include <string.h>

#include "ScrnMgr.h"

#define p2cstr(x)	(char *)PtoCstr((char *)x)
#define c2pstr(x)	(char *)CtoPstr((char *)x)

#endif

#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"

#define maxEntries				100

#define horizIndent				4				/* same as std LDEF 0 */

#define ldefType				'LDEF'
#define ldefID					514

#define scoresDlgID				514
#define dfltBorder				2
#define listBox					3
#define titleBox				4

#define codeEnter				0x03
#define codeReturn				0x0D

#ifdef THINK_C
/* Cover up error in THINK C library.  */
#define ok	OK
#define cancel	Cancel
#endif

typedef struct LDEFRec {
	short braInstruction;
	short flags;
	int32 resType;
	short resID;
	short version;
	short jmpInstuction;
#ifdef THINK_C
	void (*defProc)();
#else
	pascal void (*defProc)();
#endif
} LDEFRec, *LDEFPtr, **LDEFHandle;

static short scoresRefNum;

static short lastVisTop;
static Rect titleRect;

static ControlHandle okButton;

static ListHandle theList;

void CreateScoreFile()

{
	Str255 fileName;
	OSErr err;

	strncpy((char *)fileName, MORIA_TOP, 255);
	fileName[255] = '\0';
	(void) c2pstr(fileName);

	appldirectory();
	err = FSOpen(fileName, 0, &scoresRefNum);
	if (err == noErr)
		(void) FSClose(scoresRefNum);
	else
		err = Create(fileName, 0, MORIA_FCREATOR, SCORE_FTYPE);
	restoredirectory();

	if (err != noErr)
		alert_error("Ooops!  Could not create the high scores \
file.  Disk may be locked.");

	return;
}

void EnterScoreFile(theScore)
int32 *theScore;

{
#if 0
	Str255 fileName;
	OSErr err;
	Boolean bump, append;
	int32 i, pos, count;
	int32 prev, next;
	char message[80];

	strncpy(fileName, MORIA_TOP, 255);
	fileName[255] = '\0';
	(void) c2pstr(fileName);

	appldirectory();
	err = FSOpen(fileName, 0, &scoresRefNum);
	restoredirectory();

	if (err != noErr) {
		alert_error("Sorry.  High scores file could not be opened.");
		return;
	}

	bump = false;
	append = false;

	prev = *theScore;

	for (i = 0, pos = 0; i < maxEntries; pos += sizeof(int32), i++) {

		count = sizeof(int32);
		err = FSRead(scoresRefNum, &count, (Ptr) &next);

		if (err == noErr)
			bump = (bump) || (prev.points >= next.points);
		else if (err == eofErr)
			append = true;
		else
			break;

		if ( (!bump) && (!append) ) continue;

		SetFPos(scoresRefNum, fsFromStart, pos);

		count = sizeof(int32);
		err = FSWrite(scoresRefNum, &count, (Ptr) &prev);

		if ( (err != noErr) || (append) ) break;

		prev = next;

	}

	(void) FSClose(scoresRefNum);

	if (err != noErr) {
		sprintf(message, "Bad news.  Error %d while updating scores file.", err);
		alert_error(message);
	}

	return;
#else
#pragma unused(theScore)
#endif
}

static void Init(list)
ListHandle list;

{
	GrafPtr thePort;
	FontInfo fi;
	short saveFont, saveSize;

	GetPort(&thePort);

	saveFont = thePort->txFont;
	saveSize = thePort->txSize;

	TextFont(monaco);
	TextSize(9);

	GetFontInfo(&fi);

	TextFont(saveFont);
	TextSize(saveSize);

	(*list)->indent.v = fi.ascent;
	(*list)->indent.h = horizIndent;

	return;
}

static void Draw(scoreptr, clip, indent)
int32 *scoreptr;
Rect *clip;
Point indent;

{
#if 0
	char line[80];
	GrafPtr thePort;
	short saveFont, saveSize;

	if (scoreptr != NULL) {
		(void) sprintf(line, "%-7d%-15.15s%-10.10s%-10.10s%-5d%-25.25s%5d",
						(int32) scoreptr->points, scoreptr->name,
						race[scoreptr->prace].trace, class[scoreptr->pclass].title,
						(int32) scoreptr->lev, scoreptr->died_from, scoreptr->dun_level);
		(void) c2pstr(line);
	}
	else
		(void) c2pstr(strcpy(line, "No Scores\311"));

	GetPort(&thePort);

	saveFont = thePort->txFont;
	saveSize = thePort->txSize;

	TextFont(monaco);
	TextSize(9);

	PenNormal();
	EraseRect(clip);

	MoveTo(clip->left + indent.h, clip->top + indent.v);
	DrawString(line);

	TextFont(saveFont);
	TextSize(saveSize);

	return;
#else
#pragma unused(scoreptr, clip, indent)
#endif
}

static pascal void HighScoresLDEF(msg, select, clip, cno, dataOffset,
				  dataLen, list)
short msg;
Boolean select;
Rect *clip;
Cell cno;
short dataOffset, dataLen;
ListHandle list;

{
#ifndef THINK_C
	#pragma unused(select, cno, dataLen)
#endif
	short offset;
	int32 count;
	int32 score;

	switch (msg) {

		case lInitMsg:
			Init(list);
			break;

		case lDrawMsg:
			offset = *((short *) (*(*list)->cells + dataOffset));
			if (offset == -1) {
				Draw(NULL, clip, (*list)->indent);
			}
			else {
				count = sizeof(int32);
				(void) SetFPos(scoresRefNum, fsFromStart, (int32) offset);
				(void) FSRead(scoresRefNum, &count, (Ptr) &score);
				Draw(&score, clip, (*list)->indent);
			}
			break;

	}

	return;
}

static void InvalScoreRange()

{
	char from[8], to[8];
	short first, last;

	first = (*theList)->visible.top + 1;
	last = (*theList)->visible.bottom;
	if (last > (*theList)->dataBounds.bottom)
	  last = (*theList)->dataBounds.bottom;

	NumToString(first, from);
	NumToString(last, to);

	ParamText(from, to, NULL, NULL);

	InvalRect(&titleRect);

	lastVisTop = (*theList)->visible.top;

	return;
}

static pascal Boolean ScoresDlgFilter(theDialog, theEvent, itemHit)
DialogPtr theDialog;
EventRecord *theEvent;
short *itemHit;

{
	GrafPtr oldPort;
	Point localPt;
	WindowPtr theWindow;
	ControlHandle theControl;
	Boolean handled;
	char key;
	short part;

	handled = false;

	switch (theEvent->what) {

		case keyDown:
			key = theEvent->message & charCodeMask;
			if ( (key == codeEnter) || (key == codeReturn) ) {
				handled = true;
				HiliteControl(okButton, inButton);
				*itemHit = ok;
			}
			break;

		case mouseDown:
			part = FindWindow(theEvent->where, &theWindow);
			if ( (theWindow == (WindowPtr) theDialog) && (part == inContent) ) {
				GetPort(&oldPort);
				SetPort((GrafPtr) theWindow);
				localPt = theEvent->where;
				GlobalToLocal(&localPt);
				part = FindControl(localPt, theWindow, &theControl);
				if (theControl == (*theList)->vScroll) {
					handled = true;
					(void) LClick(localPt, theEvent->modifiers, theList);
					if ((*theList)->visible.top != lastVisTop) InvalScoreRange();
					*itemHit = listBox;
				}
				SetPort(oldPort);
			}
			break;

	}

	return(handled);
}

static pascal void DrawListBox(theWindow, theItem)
WindowPtr theWindow;
short theItem;

{
	GrafPtr thePort;
	short saveFont, saveSize;
	short itsType;
	Handle itsHandle;
	Rect itsRect;

	GetDItem((DialogPtr) theWindow, theItem, &itsType, &itsHandle, &itsRect);

	GetPort(&thePort);

	saveFont = thePort->txFont;
	saveSize = thePort->txSize;

	TextFont(monaco);
	TextSize(9);

	MoveTo(itsRect.left + (*theList)->indent.h + 1,
	       itsRect.top + (*theList)->indent.v);
	DrawString("\pPoints Name           Race      Class     Lv   Killed \
By                Dun Lv");

	TextFont(saveFont);
	TextSize(saveSize);

	itsRect.top += (*theList)->cellSize.v;
	FrameRect(&itsRect);

	EraseRect(&(*theList)->rView);
	LUpdate(theWindow->visRgn, theList);

	return;
}

void DoScoresDlg()

{
	Str255 fileName;
	OSErr err;
	int32 bytes;
	short entries;
	GrafPtr oldPort;
	FontInfo fi;
	short saveFont, saveSize;
	DialogPtr theDialog;
	Handle itsHandle;
	Handle ldef;
	short itemHit;
	short itsType;
	Rect itsRect;
	Rect listRect, cellRect;
	Point cellSize;
	int32 i;
	int32 h, v;
	Point cno;
	short offset;

	CreateScoreFile();

	strncpy((char *)fileName, MORIA_TOP, 255);
	fileName[255] = '\0';
	(void) c2pstr(fileName);

	appldirectory();
	err = FSOpen(fileName, 0, &scoresRefNum);
	restoredirectory();

	if (err != noErr) {
		alert_error("Sorry.  High scores file could not be opened.");
		return;
	}

	(void) GetEOF(scoresRefNum, &bytes);
	entries = bytes / sizeof(int32);

	theDialog = GetNewDialog(scoresDlgID, nil, (WindowPtr) -1);

	GetPort(&oldPort);
	SetPort((GrafPtr) theDialog);

	saveFont = theDialog->txFont;
	saveSize= theDialog->txSize;

	TextFont(monaco);
	TextSize(9);

	GetFontInfo(&fi);

	TextFont(saveFont);
	TextSize(saveSize);

	CenterScreenDLOG(scoresDlgID, fixHalf, fixHalf, &h, &v);
	MoveWindow((WindowPtr) theDialog, (short) h, (short) v, false);

	GetDItem(theDialog, ok, &itsType, (Handle *) &okButton, &itsRect);
	InsetRect(&itsRect, -4, -4);

	SetDItem(theDialog, dfltBorder, userItem, (Handle) DrawDefaultBorder,
		 &itsRect);

	GetDItem(theDialog, titleBox, &itsType, &itsHandle, &titleRect);

	GetDItem(theDialog, listBox, &itsType, &itsHandle, &listRect);

	SetPt(&cellSize, listRect.right - listRect.left - 2,
	      fi.ascent + fi.descent + fi.leading);

	listRect.bottom =
		listRect.top +
		((listRect.bottom - listRect.top - 2) / cellSize.v) * cellSize.v +
		2;

	SetDItem(theDialog, listBox, userItem, (Handle) DrawListBox, &listRect);

	InsetRect(&listRect, 1, 1);
	listRect.top += cellSize.v;
	listRect.right -= 15;

	ldef = GetResource(ldefType, ldefID);
	MoveHHi(ldef);
	HLock(ldef);

	(*((LDEFHandle) ldef))->defProc = HighScoresLDEF;

	SetRect(&cellRect, 0, 0, 1, entries ? entries : 1);

	theList = LNew(
		&listRect, &cellRect, cellSize,
		ldefID, (WindowPtr) theDialog,
		false, false, false, true);

	if (!entries) {
		offset = -1;
		SetPt(&cno, 0, 0);
		LSetCell((Ptr) &offset, (short) sizeof(offset), cno, theList);
	}
	else {
		for (i = 0, offset = 0; i < entries; i++, offset += sizeof(int32)) {
			SetPt(&cno, 0, i);
			LSetCell((Ptr) &offset, (short) sizeof(offset), cno, theList);
		}
	}

	LDoDraw(true, theList);

	InvalScoreRange();

	ShowWindow((WindowPtr) theDialog);

	do {
		ModalDialog(ScoresDlgFilter, &itemHit);
	} while (itemHit != ok);

	LDispose(theList);

	HUnlock(ldef);

	SetPort(oldPort);

	DisposDialog(theDialog);

	(void) FSClose(scoresRefNum);

	return;
}
