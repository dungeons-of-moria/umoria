/* mac/machelp.c -- support code for mac like help system

   Copyright (c) 1989-1991 Curtis McCauley, James E. Wilson

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#ifndef THINK_C
#include <Types.h>
#include <Resources.h>
#include <Events.h>
#include <Quickdraw.h>
#include <TextEdit.h>
#include <Controls.h>
#include <Dialogs.h>

#include <ScrnMgr.h>
#else
#include <stddef.h>
#include "ScrnMgr.h"
#endif

#include "config.h"
#include "constant.h"
#include "types.h"

#define textType				'TEXT'
#define textID					512

#define macHelpDlgID			512
#define dfltBorder				2
#define textBox					3
#define scrollBar				4

#define codeEnter				0x03
#define codeReturn				0x0D

#ifdef THINK_C
/* Cover up error in THINK C library.  */
#define ok	OK
#define cancel	Cancel
#endif

static ControlHandle okButton;

static ControlHandle myScrollBar;
static short scrollMin, scrollMax;
static short pageLines;

static TEHandle myTEHandle;

static pascal void ActOnScroll(theControl, partCode)
ControlHandle theControl;
short partCode;

{
	short delta;
	short newVal, origVal;

	if (theControl != myScrollBar) return;

	switch (partCode) {
		case inUpButton:
			delta = -1;
			break;
		case inDownButton:
			delta = 1;
			break;
		case inPageUp:
			delta = -pageLines;
			break;
		case inPageDown:
			delta = pageLines;
			break;
		default:
			delta = 0;
			break;
	}

	if (delta) {
		origVal = GetCtlValue(myScrollBar);
		newVal = origVal + delta;
		if (newVal < scrollMin) newVal = scrollMin;
		if (newVal > scrollMax) newVal = scrollMax;
		if (newVal != origVal) {
			delta = newVal - origVal;
			SetCtlValue(myScrollBar, newVal);
			TEScroll(0, -delta * (*myTEHandle)->lineHeight, myTEHandle);
		}
	}

	return;
}

static void DoHScroll(where, part)
Point where;
short part;

{
	short origVal, newVal;

	switch (part) {

		case inUpButton:
		case inDownButton:
		case inPageUp:
		case inPageDown:
			TrackControl(myScrollBar, where, (ProcPtr) ActOnScroll);
			break;

		case inThumb:
			origVal = GetCtlValue(myScrollBar);
			TrackControl(myScrollBar, where, NULL);
			newVal = GetCtlValue(myScrollBar);
			if (newVal != origVal)
				TEScroll(0, (origVal - newVal) * (*myTEHandle)->lineHeight, myTEHandle);
			break;

	}

	return;
}

static pascal Boolean MacHelpFilter(theDialog, theEvent, itemHit)
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
				if (theControl == myScrollBar) {
					handled = true;
					DoHScroll(localPt, part);
					*itemHit = scrollBar;
				}
				SetPort(oldPort);
			}
			break;

	}

	return(handled);
}

static pascal void DrawTextBox(theWindow, theItem)
WindowPtr theWindow;
short theItem;

{
	short itsType;
	Handle itsHandle;
	Rect itsRect;

	GetDItem((DialogPtr) theWindow, theItem, &itsType, &itsHandle, &itsRect);
	FrameRect(&itsRect);

	TEUpdate(&theWindow->portRect, myTEHandle);

	return;
}

void DoMacHelp()

{
	GrafPtr oldPort;
	DialogPtr theDialog;
	Handle itsHandle;
	Handle textHandle;
	Handle versionHandle;
	short itemHit;
	short itsType;
	Rect itsRect;
	Rect textRect;
	int32 h, v;

	theDialog = GetNewDialog(macHelpDlgID, nil, (WindowPtr) -1);

	GetPort(&oldPort);
	SetPort((GrafPtr) theDialog);

	CenterScreenDLOG(macHelpDlgID, fixHalf, fixThird, &h, &v);
	MoveWindow((WindowPtr) theDialog, (short) h, (short) v, false);

	GetDItem(theDialog, ok, &itsType, (Handle *) &okButton, &itsRect);
	InsetRect(&itsRect, -4, -4);

	SetDItem(theDialog, dfltBorder, userItem, (Handle) DrawDefaultBorder,
		 &itsRect);

	GetDItem(theDialog, scrollBar, &itsType, (Handle *) &myScrollBar, &itsRect);

	GetDItem(theDialog, textBox, &itsType, &itsHandle, &textRect);
	SetDItem(theDialog, textBox, userItem, (Handle) DrawTextBox, &textRect);
	InsetRect(&textRect, 4, 4);

	myTEHandle = TENew(&textRect, &textRect);

	textHandle = GetResource(textType, textID);
	DetachResource(textHandle);

	(*myTEHandle)->hText = textHandle;
	(*myTEHandle)->txFont = monaco;
	(*myTEHandle)->txSize = 9;
	TECalText(myTEHandle);

	pageLines = (textRect.bottom - textRect.top) / (*myTEHandle)->lineHeight;

	scrollMin = 0;
	scrollMax = (*myTEHandle)->nLines - pageLines;
	if (scrollMax < 0) scrollMax = 0;

	SetCtlMin(myScrollBar, scrollMin);
	SetCtlMax(myScrollBar, scrollMax);

	/* Get the version string.  */
	versionHandle = GetResource(MORIA_FCREATOR, 0);
	HLock(versionHandle);
	ParamText(*versionHandle, NULL, NULL, NULL);

	ShowWindow((WindowPtr) theDialog);

	do {
		ModalDialog(MacHelpFilter, &itemHit);
	} while (itemHit != ok);

	HUnlock(versionHandle);
	TEDispose(myTEHandle);

	SetPort(oldPort);

	DisposDialog(theDialog);

	return;
}
