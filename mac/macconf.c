/* mac/macconf.c: configuration routines

   Copyright (c) 1989-1991 Curtis McCauley, James E. Wilson

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#ifndef THINK_C
#include <Types.h>
#include <Quickdraw.h>
#include <Controls.h>
#include <Dialogs.h>

#include <ScrnMgr.h>
#else
#include "ScrnMgr.h"
#endif

#include "constant.h"
#include "types.h"

#define rogueFlag				'-r\0\0'
#define originalFlag			'-o\0\0'

#define cmdsetDlgID				256
#define cmdsetRogue				3
#define cmdsetOriginal			4
#define cmdsetGrpRect			6
#define cmdsetDfltBrdr			8

#define teDlgID					257
#define teFCFirst				3
#define teFCLast				5
#define teFCCount				(teFCLast - teFCFirst + 1)
#define teFCOther				6
#define teGrpRect				8
#define teDfltBrdr				10

#ifdef THINK_C
/* Cover up error in THINK C library.  */
#define ok	OK
#define cancel	Cancel
#endif

int32 GetCommandSet(hndl)
int32 **hndl;

{
	DialogPtr theDialog;
	short theItem;
	Handle theHandle;
	short itemHit;
	short itsType;
	Handle itsHandle;
	Rect itsRect;
	int32 h, v;

	theDialog = GetNewDialog(cmdsetDlgID, nil, (WindowPtr) -1);

	CenterScreenDLOG(cmdsetDlgID, fixHalf, fixThird, &h, &v);
	MoveWindow((WindowPtr) theDialog, (short) h, (short) v, false);

	GetDItem(theDialog, ok, &itsType, &itsHandle, &itsRect);
	InsetRect(&itsRect, -4, -4);

	SetDItem(theDialog, cmdsetDfltBrdr, userItem,
		 (Handle) DrawDefaultBorder, &itsRect);

	GetDItem(theDialog, cmdsetGrpRect, &itsType, &itsHandle, &itsRect);
	SetDItem(theDialog, cmdsetGrpRect, itsType, (Handle) DrawGroupRect,
		 &itsRect);

	theItem = (**hndl == originalFlag) ? cmdsetOriginal : cmdsetRogue;
	GetDItem(theDialog, theItem, &itsType, &theHandle, &itsRect);
	SetCtlValue((ControlHandle) theHandle, true);

	ShowWindow((WindowPtr) theDialog);

	do {
		ModalDialog(nil, &itemHit);
		if ((itemHit != theItem)
		    && ((itemHit == cmdsetOriginal) || (itemHit == cmdsetRogue))) {
			SetCtlValue((ControlHandle) theHandle, false);
			theItem = itemHit;
			GetDItem(theDialog, theItem, &itsType, &theHandle, &itsRect);
			SetCtlValue((ControlHandle) theHandle, true);
		}
	} while ( (itemHit != ok) && (itemHit != cancel) );

	if (itemHit == ok)
		**hndl = (theItem == cmdsetOriginal) ? originalFlag : rogueFlag;

	DisposDialog(theDialog);

	return(itemHit == ok);
}

int32 GetTextEditor(hndl)
int32 **hndl;

{
	DialogPtr theDialog;
	short theItem;
	Handle theHandle, fcHandle;
	short itemHit;
	short itsType;
	Handle itsHandle;
	Rect itsRect;
	int32 h, v;
	char *p, *q;
	Str255 fc;
	static int32 editors[teFCCount-1] = { 'MACA', 'MSWD' };

	theDialog = GetNewDialog(teDlgID, nil, (WindowPtr) -1);

	CenterScreenDLOG(teDlgID, fixHalf, fixThird, &h, &v);
	MoveWindow((WindowPtr) theDialog, (short) h, (short) v, false);

	GetDItem(theDialog, ok, &itsType, &itsHandle, &itsRect);
	InsetRect(&itsRect, -4, -4);

	SetDItem(theDialog, teDfltBrdr, userItem, (Handle) DrawDefaultBorder,
		 &itsRect);

	GetDItem(theDialog, teGrpRect, &itsType, &itsHandle, &itsRect);
	SetDItem(theDialog, teGrpRect, itsType, (Handle) DrawGroupRect, &itsRect);

	GetDItem(theDialog, teFCOther, &itsType, &fcHandle, &itsRect);

	for (theItem = 0; theItem < teFCCount-1; theItem++)
		if (**hndl == editors[theItem])
			break;

	theItem += teFCFirst;
	GetDItem(theDialog, theItem, &itsType, &theHandle, &itsRect);
	SetCtlValue((ControlHandle) theHandle, true);

	if (theItem == teFCLast) {
		p = (char *)fc;
		q = (char *) *hndl;
		*p++ = 4;
		*p++ = *q++;
		*p++ = *q++;
		*p++ = *q++;
		*p = *q;
		SetIText(fcHandle, fc);
	}

	ShowWindow((WindowPtr) theDialog);

	do {

		do {
			ModalDialog(nil, &itemHit);
			if ( (itemHit != theItem) && (itemHit >= teFCFirst)
			    && (itemHit <= teFCLast) ) {
				SetCtlValue((ControlHandle) theHandle, false);
				theItem = itemHit;
				GetDItem(theDialog, theItem, &itsType, &theHandle, &itsRect);
				SetCtlValue((ControlHandle) theHandle, true);
			}
		} while ( (itemHit != ok) && (itemHit != cancel) );

		if (itemHit == ok) {
			if (theItem != teFCLast)
				**hndl = editors[theItem - teFCFirst];
			else {
				GetIText(fcHandle, fc);
				p = (char *) *hndl;
				q = (char *)fc + 1;
				*p++ = (fc[0] > 0) ? *q++ : ' ';
				*p++ = (fc[0] > 1) ? *q++ : ' ';
				*p++ = (fc[0] > 2) ? *q++ : ' ';
				*p = (fc[0] > 3) ? *q : ' ';
			}
		}

	} while (!ok);

	DisposDialog(theDialog);

	return(itemHit == ok);
}
