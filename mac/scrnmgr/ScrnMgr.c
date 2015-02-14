/* mac/scrnmgr/ScrnMgr.c: a screen management package for the Mac

   Copyright (c) 1989-1991 Curtis McCauley, James E. Wilson

   This file is part of Umoria.

   Umoria is free software; you can redistribute it and/or modify 
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Umoria is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of 
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Genral Public License for more details.

   You should have received a copy of the GNU General Public License 
   along with Umoria.  If not, see <http://www.gnu.org/licenses/>. */

/* This file was also released as part of an independent program, and
   may be redistributed and/or modified under the authors' original terms,
   which are given below. */

/* You may copy this subroutine package freely, modify it as you desire,
   and distribute it at will, as long as the copyright notice in the source
   material is not disturbed, excepting that no one may use this package or
   any part of it for commercial purposes of any kind without the express
   written consent of its author. */

/*	Think C port by Benjamin Schreiber, 1991.  */

#ifndef THINK_C

#include <Types.h>
#include <OSUtils.h>
#include <Files.h>
#include <Quickdraw.h>
#include <Windows.h>
#include <Memory.h>
#include <Controls.h>
#include <Resources.h>
#include <Desk.h>
#include <Dialogs.h>
#include <Events.h>
#include <Fonts.h>
#include <Menus.h>
#include <OSEvents.h>
#include <SegLoad.h>
#include <TextEdit.h>
#include <ToolUtils.h>
#include <Packages.h>
#include <Retrace.h>
#include <Script.h>
#include <Traps.h>
#include <SysEqu.h>

#include <StdDef.h>
#include <String.h>
#include <Strings.h>

#else

/* defined(THINK_C) */

#include <VRetraceMgr.h>
#include <ScriptMgr.h>
#include <MultiFinder.h>	/* This may not be necessary.  */

#include <string.h>
#include <stddef.h>

#define p2cstr	PtoCstr		/* Correct difference in names.  */
#define c2pstr	CtoPstr

typedef char Str63[64];

/* Cover up error in THINK C library.  */
#define ok	OK
#define cancel	Cancel
#define normal	0

#endif


#include "ScrnMgr.h"

#define osEvent					app4Evt
#define suspendResumeMessage	1
#define resumeMask				1

#define infoResType			(ResType) 'INFO'
#define infoResID			1

#define ABS(i)				((i < 0) ? -i : i)

#define menuBarID1			128
#define menuBarID2			228

#define appleID				128
#define fileID1				129
#define fileID2				229
#define editID1				130
#define editID2				230
#define screenID			131
#define fontSizeID			132
#define appID1				133
#define appID2				233

#define aboutItem			1

#define openItem			1
#define closeItem			2
#define quitItem			4

#define undoItem			1
#define cutItem				3
#define copyItem			4
#define pasteItem			5
#define clearItem			6

#define colorsItem			1

#define aboutDlgID			128
#define aboutDfltBorder		2

#define aboutStrID			128

#define colorsDlgID			129
#define colorsDfltBorder	23
#define foregroundRect		19
#define backgroundRect		20
#define foreColors			3
#define backColors			11

#define yesOrNoDlgID		130
#define yesOrNoDfltBorder	3
#define yesOrNoText			4

#define fontNameID			129

#define fontSizeCount		50
#define minFontSize			1
#define maxFontSize			100
#define dfltFontSize		9

#define acurID				128

#define titleMargin			20
#define titleMinHgt			8
#define titleMinWid			8
#define titleDfltLeft		32
#define titleDfltTop		64

#define ESC					'\x1B'
#define BACKQUOTE			'\x60'

typedef struct Info {
	Rect bounds;
	short size;
	long colorStdFore;
	long colorStdBack;
} Info, *InfoPtr;

typedef struct Acur {
	short frames;
	short next;
	union {
		struct { short id; short fill; } cursID;
		Cursor **cursHandle;
	} table[1];
} Acur, *AcurPtr, **AcurHandle;

typedef struct Evt {
	char keycode;
	char modifiers;
	char ascii;
	short h;
	short v;
} Evt, *EvtPtr;

#define qSize			32

typedef struct SaveScreenRec {
	struct SaveScreenRec **link;	/* link to previous saved screen		*/
	char **chars;					/* handle to saved character buffer		*/
	char **attrs;					/* handle to saved attribute buffer		*/
	Point cursor;					/* saved cursor location				*/
} SaveScreenRec, *SaveScreenPtr, **SaveScreenHandle;

typedef struct ScreenRec {
    WindowPtr window;				/* the screen window					*/
	EventRecord event;				/* the last event						*/
	SysEnvRec env;					/* the outside world					*/
	long wneImplemented;			/* do we have WaitNextEvent?			*/
	long backgrounding;				/* are we in the background?			*/
	long reconfigFlag;				/* true if user want to change options	*/
	long colorFlag;					/* true if multiple colors supported	*/
	long cmdKeyFlag;				/* true if cmd key equivalents ok		*/
	long mouseFlag;					/* true if mouse clicks are captured	*/
	long escMapFlag;				/* true if backquote maps to escape		*/
	long waitFlag;					/* true if in wait						*/
	long waitRate;					/* how fast to spin the cursor			*/
	long colorStdFore;				/* standard foreground color			*/
	long colorStdBack;				/* standard background color			*/
	long windowW;					/* its content width in pixels			*/
	long windowH;					/* its content height in pixels			*/
	short txFont;					/* the font number in use				*/
	short txSize;					/* the font size in use					*/
	short txNum;					/* index into fontSizes table + 1		*/
	short **fontSizes;				/* a table of available sizes			*/
    FontInfo info;					/* font info for the current font		*/
    Point cursor;					/* cursor location in char coords		*/
    Point charPDims;				/* char cell size in pixels				*/
    Point scrnCDims;				/* screen size in chars					*/
	Point origin;					/* logical origin of screen for drawing */
	Rect stdStateGRect;				/* original standard state				*/
    Rect scrnCRect;					/* screen bounds in chars				*/
    Rect scrnLRect;					/* screen bounds in pixels				*/
    Rect scrnXLRect;				/* screen bounds in pixels plus margin	*/
	Rect sizeLimitPRect;			/* limits to growth						*/
	Rect dragLimitGRect;			/* limits to movement					*/
	Rect hScrollPRect;				/* horiz scroll bar bounds				*/
	Rect vScrollPRect;				/* vert scroll bar bounds				*/
	Rect growPRect;					/* grow icon bounds						*/
	Rect picPRect;					/* picture area bounds					*/
	Rect picLRect;					/* drawing is clipped here (pic area)	*/
	Rect drawLRect;					/* drawing is clipped here (screen area)*/
	Rect drawXCRect;				/* rect to draw in char coords			*/
	Rect drawXLRect;				/* rect to draw in pixel coords			*/
	Rect mouseCRect;				/* watch mouse activity in this area	*/
	Rect mouseLRect;				/* watch mouse activity in this area	*/
	ControlHandle hScrollHandle;	/* handle to horiz scroll control		*/
	ControlHandle vScrollHandle;	/* handle to vert scroll control		*/
	short infoResFile;				/* ref no of resource file				*/
	Handle infoHandle;				/* handle to info resource				*/
	Handle versHandle;				/* handle to vers. string, for about dlg */
    char **chars;					/* handle to character buffer			*/
    char **attrs;					/* handle to attribute buffer			*/
	short **updLeft;				/* handle to left margin for updates	*/
	short **updRight;				/* handle to right margin for updates	*/
	long keyFlush;					/* tick count at last key flush			*/
	long mouseFlush;				/* tick count at last mouse flush		*/
	long qBegin;					/* index to first elem in queue			*/
	long qEnd;						/* index to next elem to be stored		*/
	Evt qBuffer[qSize];				/* queue of keystrokes					*/
	long cursorLevel;				/* greater than zero when cursor visible*/
	long cursorColor;				/* color of cursor lines				*/
	long cursorLines;				/* number of lines in cursor			*/
	long cursorBlink;				/* ticks between cursor blinks			*/
	long cursorStatus;				/* non-zero when cursor in on screen	*/
	long cursorChangeTick;			/* when to change cursor status			*/
	void (*aboutProc)(void);		/* what to call when about is selected  */
	void (*quitProc)(void);			/* what to call when quit is selected   */
	void (*fileMenuProc)(long item);/* file menu handler					*/
	void (*appMenuProc)(long item);	/* app menu handler						*/
	long quitReturns;				/* true if quit proc will always return */
	AcurHandle acur;				/* table of cursors						*/
	VBLTask vbl;					/* for animating said cursors			*/
	SaveScreenHandle stack;			/* stack of saved screens				*/
} ScreenRec;

ScreenRec theScreen;

static long colors[8] = {
	whiteColor, blackColor,
	redColor, greenColor, blueColor,
	cyanColor, magentaColor, yellowColor
};

static void DisposeStackTop()

{
	SaveScreenHandle next;

	if (theScreen.stack != NULL) {

		next = (*theScreen.stack)->link;

		DisposHandle((Handle) (*theScreen.stack)->chars);
		DisposHandle((Handle) (*theScreen.stack)->attrs);

		DisposHandle((Handle) theScreen.stack);

		theScreen.stack = next;

	}

	return;
}

static long WNETest(envp)
SysEnvRec *envp;

{
#ifndef THINK_C
	long hasWNE;

	if (envp->machineType < 0)
		hasWNE = false;

	else
		hasWNE =
		NGetTrapAddress(_WaitNextEvent, ToolTrap) != GetTrapAddress(_Unimplemented);

	return(hasWNE);
#else
	return 0;
#endif
}

static long NoEscTest(envp)
SysEnvRec *envp;

{
	long hasNoEsc;

	switch (envp->keyBoardType) {

		case envAExtendKbd:
		case envStandADBKbd:
			hasNoEsc = false;
			break;

		default:
			hasNoEsc = true;
			break;

	}

	return(hasNoEsc);
}

static void FlushQueue()

{
	theScreen.qBegin = theScreen.qEnd = 0;
	return;
}

static void PushQueue(keycode, modifiers, ascii, h, v)
char keycode;
char modifiers;
char ascii;
short h;
short v;

{
	long end;
	EvtPtr next;

	end = (theScreen.qEnd + 1) & (qSize - 1);
	if (end != theScreen.qBegin) {
		next = &theScreen.qBuffer[theScreen.qEnd];
		next->keycode = keycode;
		next->modifiers = modifiers;
		next->ascii = ascii;
		next->h = h;
		next->v = v;
		theScreen.qEnd = end;
	}
	else {
		/* should I beep? */
	}

	return;
}

static long LenQueue()

{
	return(ABS(theScreen.qEnd - theScreen.qBegin));
}

static long PopQueue(keycode, modifiers, ascii, h, v)
char *keycode;
char *modifiers;
char *ascii;
short *h;
short *v;

{
	long flag;
	EvtPtr next;

	if (theScreen.qEnd != theScreen.qBegin) {
		flag = true;
		next = &theScreen.qBuffer[theScreen.qBegin];
		if (keycode != NULL) *keycode = next->keycode;
		if (modifiers != NULL) *modifiers = next->modifiers;
		if (ascii != NULL ) *ascii = next->ascii;
		if (h != NULL ) *h = next->h;
		if (v != NULL ) *v = next->v;
		theScreen.qBegin = (theScreen.qBegin + 1) & (qSize - 1);
	}
	else {
		flag = false;
	}

	return(flag);
}

static int GtoC(gh, gv, ch, cv)
short gh, gv;
short *ch, *cv;

{
	int flag;
	Point temp;

	temp.h = gh;
	temp.v = gv;

	SetOrigin(theScreen.origin.h, theScreen.origin.v);
	GlobalToLocal(&temp);
	SetOrigin(0, 0);

	if ( PtInRect(temp, &theScreen.mouseLRect)
	    && PtInRect(temp, &theScreen.picLRect) ) {
		flag = true;
		*ch = temp.h / theScreen.charPDims.h;
		*cv = temp.v / theScreen.charPDims.v;
	}
	else
		flag = false;

	return(flag);
}

static void DefaultCursor()

{
	theScreen.cursorLevel = 0;
	theScreen.cursorColor = attrColorBlack;
	theScreen.cursorLines = 2;
	theScreen.cursorBlink = 0;
	theScreen.cursorStatus = 0;
	theScreen.cursorChangeTick = 0;

	return;
}

static void Prolog()

{
#ifdef THINK_C
	InitGraf(&thePort);
#else
	InitGraf(&qd.thePort);
#endif
	InitFonts();
	FlushEvents(everyEvent, 0);
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(NULL);
	InitCursor();

	return;
}

static void LoadCursors()

{
	short i, count;
	Cursor **c;

	theScreen.acur = (AcurHandle) GetResource('acur', acurID);

	count = (*theScreen.acur)->frames;

	for (i = 0; i < count; i++) {
		c = GetCursor((*theScreen.acur)->table[i].cursID.id);
		(*theScreen.acur)->table[i].cursHandle = c;
	}

	return;
}

static long GetFontSizes()

{
	long i, j;
	short fsize, sizes[fontSizeCount];
	char **fname;

	fname = (char **) GetString(fontNameID);

	if (fname == NULL)
		theScreen.txFont = 0;
	else {
		HLock(fname);
		GetFNum(*fname, &theScreen.txFont);
		HUnlock(fname);
	}

	for (i = 0, fsize = minFontSize;
		 (i < fontSizeCount) && (fsize < maxFontSize);
		 fsize++)
		if (RealFont(theScreen.txFont, fsize))
			sizes[i++] = fsize;

	if (i == 0)
		sizes[i++] = dfltFontSize;

	theScreen.fontSizes = (short **) NewHandle(sizeof(short) * (i + 1));

	if (theScreen.fontSizes != NULL)
		for (j = 0; j < i; j++)
			(*theScreen.fontSizes)[j] = sizes[j];

	(*theScreen.fontSizes)[i++] = 0;

	return(theScreen.fontSizes != NULL);
}

static void SetUpMenus()

{
	long i, n, offset;
	Handle h;
	MenuHandle appMenuHndl, fontMenuHndl;
	Str255 ftitle;

	SetMenuBar(GetNewMBar(theScreen.cmdKeyFlag ? menuBarID1 : menuBarID2));
	AddResMenu(GetMHandle(appleID), (ResType) 'DRVR');
	if (theScreen.colorFlag || !theScreen.env.hasColorQD) {
		InsertMenu(GetMenu(fontSizeID), 0);
		fontMenuHndl = GetMHandle(fontSizeID);
		offset = 1;
	}
	else {
		InsertMenu(GetMenu(screenID), 0);
		fontMenuHndl = GetMHandle(screenID);
		offset = 3;
	}

	h = (Handle) GetString(aboutStrID);
	if (h != NULL) {
		MoveHHi(h);
		HLock(h);
		SetItem(GetMHandle(appleID), aboutItem, *h);
		HUnlock(h);
	}

	for (i = 0; n = (*theScreen.fontSizes)[i]; i++) {
		NumToString(n, ftitle);
		(void) c2pstr(strcat(p2cstr((char *)ftitle), " point"));
		AppendMenu(fontMenuHndl, ftitle);
		if (n == theScreen.txSize) {
			theScreen.txNum = i + offset;
			CheckItem(fontMenuHndl, theScreen.txNum, true);
		}
	}

	if (theScreen.appMenuProc != NULL) {
		appMenuHndl = GetMenu(theScreen.cmdKeyFlag ? appID1 : appID2);
		if (appMenuHndl != NULL) InsertMenu(appMenuHndl, 0);
	}

	DrawMenuBar();

	return;
}

static void LoadInfo(resourceFile, rfCreator, rfType)
Str255 resourceFile;
OSType rfCreator, rfType;

{
	ResType dfltType;
	short dfltID;
	Str63 dfltName;
	short saveVol;

	(void) GetVol(NULL, &saveVol);
	(void) SetVol(NULL, theScreen.env.sysVRefNum);

	(void) Create(resourceFile, 0, rfCreator, rfType);
	CreateResFile(resourceFile);

	theScreen.infoResFile = OpenResFile(resourceFile);

	theScreen.infoHandle = GetResource(infoResType, infoResID);

	if ( (theScreen.infoResFile != -1) &&
	     (theScreen.infoResFile != HomeResFile(theScreen.infoHandle)) ) {
		GetResInfo(theScreen.infoHandle, &dfltID, &dfltType, dfltName);
		DetachResource(theScreen.infoHandle);
		AddResource(theScreen.infoHandle, dfltType, dfltID, dfltName);
	}

	(void) SetVol(NULL, saveVol);

	return;
}

static void UseInfo(boundsRectPtr, sizePtr, colorStdForePtr, colorStdBackPtr)
Rect *boundsRectPtr;
short *sizePtr;
long *colorStdForePtr;
long *colorStdBackPtr;

{
	InfoPtr theInfo;

	LoadResource(theScreen.infoHandle);

	theInfo = (InfoPtr) *theScreen.infoHandle;

	*boundsRectPtr = theInfo->bounds;
	*sizePtr = theInfo->size;

	if (theScreen.env.hasColorQD) {
		*colorStdForePtr = theInfo->colorStdFore;
		*colorStdBackPtr = theInfo->colorStdBack;
	}
	else {
		*colorStdForePtr = attrColorBlack;
		*colorStdBackPtr = attrColorWhite;
	}

	return;
}

static void SaveInfo()

{
	Point topLeft, bottomRight;
	InfoPtr theInfo;

	if (theScreen.infoResFile == -1) return;

	LoadResource(theScreen.infoHandle);
	HLock(theScreen.infoHandle);

	theInfo = (InfoPtr) *theScreen.infoHandle;

#ifdef THINK_C
	topLeft.h = thePort->portRect.left;
	topLeft.v = thePort->portRect.top;

	bottomRight.h = thePort->portRect.right;
	bottomRight.v = thePort->portRect.bottom;
#else
	topLeft.h = qd.thePort->portRect.left;
	topLeft.v = qd.thePort->portRect.top;

	bottomRight.h = qd.thePort->portRect.right;
	bottomRight.v = qd.thePort->portRect.bottom;
#endif

	LocalToGlobal(&topLeft);
	LocalToGlobal(&bottomRight);

	if ( (topLeft.h     != theInfo->bounds.left)   ||
		 (topLeft.v     != theInfo->bounds.top)    ||
		 (bottomRight.h != theInfo->bounds.right)  ||
		 (bottomRight.v != theInfo->bounds.bottom)    ) {

		theInfo->bounds.left   = topLeft.h;
		theInfo->bounds.top    = topLeft.v;
		theInfo->bounds.right  = bottomRight.h;
		theInfo->bounds.bottom = bottomRight.v;
		ChangedResource(theScreen.infoHandle);

	}

	if ( theScreen.txSize != theInfo->size ) {

		theInfo->size = theScreen.txSize;
		ChangedResource(theScreen.infoHandle);

	}

	if ( (theScreen.colorStdFore != theInfo->colorStdFore) ||
	     (theScreen.colorStdBack != theInfo->colorStdBack)    ) {

		theInfo->colorStdFore = theScreen.colorStdFore;
		theInfo->colorStdBack = theScreen.colorStdBack;
		ChangedResource(theScreen.infoHandle);

	}

	HUnlock(theScreen.infoHandle);

	return;
}

static void AdjustPortBounds(bounds)
Rect *bounds;

{
	long repos;
	long wid, hgt;
	Rect test, sect;
#ifndef THINK_C
	GDHandle gdh;
#endif

	test.left = bounds->left;
	test.top = bounds->top - titleMargin;
	test.right = bounds->right;
	test.bottom = bounds->top;

#ifdef THINK_C			/* Ignore color issues. -- BS  */

	(void) SectRect (&screenBits.bounds, &test, &sect);
	wid = sect.right - sect.left;
	hgt = sect.bottom - sect.top;
	if ((wid < titleMinWid) || (hgt < titleMinHgt))
		OffsetRect (bounds,
			(screenBits.bounds.left + titleDfltLeft) - bounds->left\,
			(screenBits.bounds.top + titleDfltTop) - bounds->top);

#else

	repos = true;

	if (!theScreen.env.hasColorQD) {

		(void) SectRect(&qd.screenBits.bounds, &test, &sect);
		wid = sect.right - sect.left;
		hgt = sect.bottom - sect.top;
		if ( (wid >= titleMinWid) && (hgt >= titleMinHgt) ) repos = false;

	}

	else {

		gdh = GetDeviceList();

		while ( (gdh != NULL) && (repos) ) {

			if ( ((*gdh)->gdFlags && (1 << screenDevice)) &&
					((*gdh)->gdFlags && (1 << screenActive)) ) {

				(void) SectRect(&(*gdh)->gdRect, &test, &sect);
				wid = sect.right - sect.left;
				hgt = sect.bottom - sect.top;
				if ( (wid >= titleMinWid) && (hgt >= titleMinHgt) ) repos = false;

			}

			gdh = GetNextDevice(gdh);

		}

	}

	if (repos)
		OffsetRect(bounds,
			(qd.screenBits.bounds.left + titleDfltLeft) - bounds->left,
			(qd.screenBits.bounds.top + titleDfltTop) - bounds->top);
#endif

	return;
}

static void SetScreenParameters()

{
	long stdWid, stdHgt;
	Rect stdState;
	WStateData **zoomInfo;

    GetFontInfo(&theScreen.info);
    theScreen.charPDims.h = theScreen.info.widMax;
    theScreen.charPDims.v = theScreen.info.ascent
						 + theScreen.info.descent
						 + theScreen.info.leading;

    theScreen.scrnCRect.left = 0;
    theScreen.scrnCRect.right = theScreen.scrnCDims.h;
    theScreen.scrnCRect.top = 0;
    theScreen.scrnCRect.bottom = theScreen.scrnCDims.v;

    theScreen.scrnLRect = theScreen.scrnCRect;
    theScreen.scrnLRect.right *= theScreen.charPDims.h;
    theScreen.scrnLRect.bottom *= theScreen.charPDims.v;

    theScreen.scrnXLRect = theScreen.scrnLRect;
    InsetRect(&theScreen.scrnXLRect, -4, -4);

	theScreen.sizeLimitPRect.left = 64;
	theScreen.sizeLimitPRect.top = 64;

	theScreen.sizeLimitPRect.right =
		theScreen.scrnXLRect.right - theScreen.scrnXLRect.left + 16;
	if (theScreen.sizeLimitPRect.right < 65) theScreen.sizeLimitPRect.right = 65;

	theScreen.sizeLimitPRect.bottom =
		theScreen.scrnXLRect.bottom - theScreen.scrnXLRect.top + 16;
	if (theScreen.sizeLimitPRect.bottom < 65)
	  theScreen.sizeLimitPRect.bottom = 65;

#ifdef THINK_C
	theScreen.dragLimitGRect = screenBits.bounds;
#else
	theScreen.dragLimitGRect = qd.screenBits.bounds;
#endif
	theScreen.dragLimitGRect.top += GetMBarHeight();
	InsetRect(&theScreen.dragLimitGRect, -4, -4);

	stdState = theScreen.stdStateGRect;
	stdWid = stdState.right - stdState.left - theScreen.sizeLimitPRect.right + 1;
	stdHgt = stdState.bottom - stdState.top
	  - theScreen.sizeLimitPRect.bottom + 1;

	if (stdWid > 0) {
		InsetRect(&stdState, stdWid / 2, 0);
		stdState.right -= stdWid % 2;
	}

	if (stdHgt > 0) {
		InsetRect(&stdState, 0, stdHgt / 2);
		stdState.bottom -= stdHgt % 2;
	}

	zoomInfo = (WStateData **) ((WindowPeek) theScreen.window)->dataHandle;
	(*zoomInfo)->stdState = stdState;

	theScreen.mouseLRect = theScreen.mouseCRect;
	theScreen.mouseLRect.left *= theScreen.charPDims.h;
	theScreen.mouseLRect.top *= theScreen.charPDims.v;
	theScreen.mouseLRect.right *= theScreen.charPDims.h;
	theScreen.mouseLRect.bottom *= theScreen.charPDims.v;

	return;
}

static void SetScreenPortSize()

{
	long oldW, oldH;
	long newW, newH;

	oldW = theScreen.window->portRect.right;
	oldH = theScreen.window->portRect.bottom;

	if (oldW > theScreen.sizeLimitPRect.right-1)
		newW = theScreen.sizeLimitPRect.right-1;
	else if (oldW < theScreen.sizeLimitPRect.left)
		newW = theScreen.sizeLimitPRect.left;
	else
		newW = oldW;

	if (oldH > theScreen.sizeLimitPRect.bottom-1)
		newH = theScreen.sizeLimitPRect.bottom-1;
	else if (oldH < theScreen.sizeLimitPRect.top)
		newH = theScreen.sizeLimitPRect.top;
	else
		newH = oldH;

	if ( (newW != oldW) || (newH != oldH) )
		SizeWindow(theScreen.window, newW, newH, true);

	return;
}

static void SetScreenPortRects()

{
	ClipRect(&theScreen.window->portRect);

	theScreen.windowW = theScreen.window->portRect.right;
	theScreen.windowH =	theScreen.window->portRect.bottom;

	theScreen.vScrollPRect =
	theScreen.hScrollPRect =
	theScreen.growPRect    =
	theScreen.picPRect     = theScreen.window->portRect;

	theScreen.vScrollPRect.right += 1;
	theScreen.vScrollPRect.top -= 1;
	theScreen.vScrollPRect.left = theScreen.vScrollPRect.right - 16;
	theScreen.vScrollPRect.bottom -= 14;

	theScreen.hScrollPRect.bottom += 1;
	theScreen.hScrollPRect.left -= 1;
	theScreen.hScrollPRect.top = theScreen.hScrollPRect.bottom - 16;
	theScreen.hScrollPRect.right -= 14;

	theScreen.growPRect.top = theScreen.growPRect.bottom - 15;
	theScreen.growPRect.left = theScreen.growPRect.right - 15;
	theScreen.growPRect.bottom += 1;
	theScreen.growPRect.right += 1;

	theScreen.picPRect.right -= 15;
	theScreen.picPRect.bottom -= 15;

	return;
}

static void SetScreenDrawRects()

{
	theScreen.picLRect = theScreen.picPRect;
	OffsetRect(&theScreen.picLRect, theScreen.origin.h, theScreen.origin.v);

	SectRect(&theScreen.picLRect, &theScreen.scrnLRect, &theScreen.drawLRect);

	theScreen.drawXCRect = theScreen.drawLRect;
	theScreen.drawXCRect.right += theScreen.charPDims.h - 1;
	theScreen.drawXCRect.bottom += theScreen.charPDims.v - 1;
	theScreen.drawXCRect.left /= theScreen.charPDims.h;
	theScreen.drawXCRect.right /= theScreen.charPDims.h;
	theScreen.drawXCRect.top /= theScreen.charPDims.v;
	theScreen.drawXCRect.bottom /= theScreen.charPDims.v;

	theScreen.drawXLRect = theScreen.drawXCRect;
	theScreen.drawXLRect.left *= theScreen.charPDims.h;
	theScreen.drawXLRect.right *= theScreen.charPDims.h;
	theScreen.drawXLRect.top *= theScreen.charPDims.v;
	theScreen.drawXLRect.bottom *= theScreen.charPDims.v;

	return;
}

static int RepositionScreen(deltaH, deltaV)
int deltaH, deltaV;

{
	int changed;
	int newH, newV;
	int min, max;
	RgnHandle updtRgn;

	newH = GetCtlValue(theScreen.hScrollHandle) + deltaH;
	if (newH < (min = GetCtlMin(theScreen.hScrollHandle))) newH = min;
	if (newH > (max = GetCtlMax(theScreen.hScrollHandle))) newH = max;

	newV = GetCtlValue(theScreen.vScrollHandle) + deltaV;
	if (newV < (min = GetCtlMin(theScreen.vScrollHandle))) newV = min;
	if (newV > (max = GetCtlMax(theScreen.vScrollHandle))) newV = max;

	if ( (newH != theScreen.origin.h) || (newV != theScreen.origin.v) ) {
		if (newH != theScreen.origin.h)
			SetCtlValue(theScreen.hScrollHandle, newH);
		if (newV != theScreen.origin.v)
			SetCtlValue(theScreen.vScrollHandle, newV);
		updtRgn = NewRgn();
		ScrollRect(&theScreen.picPRect,
			theScreen.origin.h - newH,
			theScreen.origin.v - newV,
			updtRgn);
		InvalRgn(updtRgn);
		DisposeRgn(updtRgn);
		theScreen.origin.h = newH;
		theScreen.origin.v = newV;
		SetScreenDrawRects();
		changed = true;
	}

	else
		changed = false;

	return(changed);
}

/* ARGH!  This routine needs error checking code. */

long InitScreenMgr(h, v, title, resFile, rfCreator, rfType, fileMenuProc,
		  appMenuProc, colorFlag)
long h, v;
char *title;
char *resFile;
OSType rfCreator, rfType;
void (*fileMenuProc)(long item);
void (*appMenuProc)(long item);
long colorFlag;

{
    long bytes, i;
	Rect bounds;
	WStateData **zoomInfo;
	Str63 pstrTitle, pstrResFile;

	Prolog();

	SysEnvirons(curSysEnvVers, &theScreen.env);

	theScreen.wneImplemented = WNETest(&theScreen.env);

	theScreen.escMapFlag = NoEscTest(&theScreen.env);

	theScreen.backgrounding = false;

#if 1
    {
      char keys[128];
      GetKeys ((KeyMap *) &keys);
      theScreen.reconfigFlag = keys[7] & 0x04;
    }
#else
	theScreen.reconfigFlag = ((char *) KeyMapLM)[7] & 0x04;
#endif

	strncpy(pstrTitle, title, 63);
	strncpy(pstrResFile, resFile, 63);
	pstrTitle[63] = '\0';
	pstrResFile[63] = '\0';
	c2pstr(pstrTitle);
	c2pstr(pstrResFile);

	LoadInfo(pstrResFile, rfCreator, rfType);

	UseInfo(&bounds,
		&theScreen.txSize,
		&theScreen.colorStdFore,
		&theScreen.colorStdBack);

	theScreen.versHandle = GetResource (rfCreator, 0);

	AdjustPortBounds(&bounds);

	LoadCursors();

	theScreen.waitFlag = false;

	theScreen.txNum = -1;

	theScreen.fileMenuProc = fileMenuProc;
	theScreen.appMenuProc = appMenuProc;

	theScreen.colorFlag = colorFlag;

	switch (theScreen.env.keyBoardType) {

		/* These are the old machines that we know.  */
		case envUnknownKbd:
		case envMacKbd:
		case envMacAndPad:
		case envMacPlusKbd:
		  theScreen.cmdKeyFlag = false;
		  break;

		case envAExtendKbd:
		case envStandADBKbd:
		case 10:		/* Mac Classic.  */
		  /* Anything else is probably new and has ctrl.  */
		default:
		  theScreen.cmdKeyFlag = true;

	}

	theScreen.mouseFlag = false;
	theScreen.mouseCRect.left	=
	theScreen.mouseCRect.top	=
	theScreen.mouseCRect.right	=
	theScreen.mouseCRect.bottom	= 0;

    bytes = h * v;
    theScreen.chars = (char **) NewHandle(bytes);
    theScreen.attrs = (char **) NewHandle(bytes);

    memset(*theScreen.chars, ' ', bytes);
    memset(*theScreen.attrs, attrNormal, bytes);

	bytes = v * sizeof(short);
    theScreen.updLeft  = (short **) NewHandle(bytes);
    theScreen.updRight = (short **) NewHandle(bytes);

	for (i = 0; i < v; i++) {
		(*theScreen.updLeft)[i] = h;
		(*theScreen.updRight)[i] = 0;
	}

    theScreen.cursor.h = 0;
    theScreen.cursor.v = 0;

    theScreen.scrnCDims.h = h;
    theScreen.scrnCDims.v = v;

	FlushQueue();

	DefaultCursor();

	GetFontSizes();

	SetUpMenus();

    theScreen.window = NewWindow(NULL, &bounds, pstrTitle,
								 false, zoomDocProc,
								 (WindowPtr) -1, true, 0);

    SetPort(theScreen.window);

	zoomInfo = (WStateData **) ((WindowPeek) theScreen.window)->dataHandle;
	theScreen.stdStateGRect = (*zoomInfo)->stdState;

    TextFont(theScreen.txFont);
    TextSize(theScreen.txSize);

	SetScreenParameters();
	SetScreenPortSize();
	SetScreenPortRects();

	theScreen.origin.h = theScreen.scrnXLRect.left;
	theScreen.origin.v = theScreen.scrnXLRect.top;

	SetScreenDrawRects();

	theScreen.hScrollHandle = NewControl(theScreen.window,
		&theScreen.hScrollPRect, "", true,
		theScreen.origin.h,
		theScreen.scrnXLRect.left,
		theScreen.scrnXLRect.right - theScreen.picPRect.right,
		scrollBarProc, 0L);
	theScreen.vScrollHandle = NewControl(theScreen.window,
		&theScreen.vScrollPRect, "", true,
		theScreen.origin.v,
		theScreen.scrnXLRect.top,
		theScreen.scrnXLRect.bottom - theScreen.picPRect.bottom,
		scrollBarProc, 0L);

	ShowWindow(theScreen.window);
	EraseRect(&theScreen.picPRect);

	theScreen.aboutProc = NULL;
	theScreen.quitProc = NULL;

	theScreen.keyFlush = theScreen.mouseFlush = TickCount();

	theScreen.stack = NULL;

    return(scrnErrOk);
}

static void ChangeScreenFontSize(num)
long num;

{
	Point oldCharPDims;

	DisposeControl(theScreen.hScrollHandle);
	DisposeControl(theScreen.vScrollHandle);

	oldCharPDims = theScreen.charPDims;

	theScreen.txSize = (*theScreen.fontSizes)[num];

    TextSize(theScreen.txSize);

	SetScreenParameters();
	SetScreenPortSize();
	SetScreenPortRects();

	theScreen.origin.h *= theScreen.charPDims.h;
	theScreen.origin.h /= oldCharPDims.h;
	if (theScreen.origin.h < theScreen.scrnXLRect.left)
		theScreen.origin.h = theScreen.scrnXLRect.left;
	if (theScreen.origin.h > theScreen.scrnXLRect.right
	    - theScreen.picPRect.right)
		theScreen.origin.h = theScreen.scrnXLRect.right - theScreen.picPRect.right;
	theScreen.origin.v *= theScreen.charPDims.v;
	theScreen.origin.v /= oldCharPDims.v;
	if (theScreen.origin.v < theScreen.scrnXLRect.top)
		theScreen.origin.v = theScreen.scrnXLRect.top;
	if (theScreen.origin.v > theScreen.scrnXLRect.bottom -
	    theScreen.picPRect.bottom)
		theScreen.origin.v = theScreen.scrnXLRect.bottom
		  - theScreen.picPRect.bottom;

	SetScreenDrawRects();

	theScreen.hScrollHandle = NewControl(theScreen.window,
		&theScreen.hScrollPRect, "", true,
		theScreen.origin.h,
		theScreen.scrnXLRect.left,
		theScreen.scrnXLRect.right - theScreen.picPRect.right,
		scrollBarProc, 0L);
	theScreen.vScrollHandle = NewControl(theScreen.window,
		&theScreen.vScrollPRect, "", true,
		theScreen.origin.v,
		theScreen.scrnXLRect.top,
		theScreen.scrnXLRect.bottom - theScreen.picPRect.bottom,
		scrollBarProc, 0L);

	InvalRect(&theScreen.picPRect);
	InvalRect(&theScreen.growPRect);

	return;
}

void CloseScreenMgr()

{
	SaveInfo();

	while (theScreen.stack != NULL)
		DisposeStackTop();

	DisposHandle((Handle) theScreen.fontSizes);

    DisposHandle((Handle) theScreen.chars);
    DisposHandle((Handle) theScreen.attrs);

    DisposeWindow(theScreen.window);

    return;
}

pascal void DrawDefaultBorder(theWindow, theItem)
WindowPtr theWindow;
short theItem;

{
	short itsType;
	Handle itsHandle;
	Rect itsRect;

	GetDItem((DialogPtr) theWindow, theItem, &itsType, &itsHandle, &itsRect);
	PenSize(3, 3);
	ForeColor(redColor);
	FrameRoundRect(&itsRect, 16, 16);
	ForeColor(blackColor);
	PenSize(1, 1);

	return;
}

pascal void DrawGroupRect(theWindow, theItem)
WindowPtr theWindow;
short theItem;

{
	short itsType;
	Handle itsHandle;
	Rect itsRect;

	GetDItem((DialogPtr) theWindow, theItem, &itsType, &itsHandle, &itsRect);
	FrameRect(&itsRect);

	return;
}

static void DoAboutDialog()

{
	DialogPtr theDialog;
	short itemHit;
	short itsType;
	Handle itsHandle;
	Rect itsRect;
	long h, v;

	theDialog = GetNewDialog(aboutDlgID, nil, (WindowPtr) -1);

	CenterScreenDLOG(aboutDlgID, fixThird, fixThird, &h, &v);
	MoveWindow((WindowPtr) theDialog, (short) h, (short) v, false);

	GetDItem(theDialog, ok, &itsType, &itsHandle, &itsRect);
	InsetRect(&itsRect, -4, -4);

	SetDItem(theDialog, aboutDfltBorder, userItem,
		 (Handle) DrawDefaultBorder, &itsRect);

	if (theScreen.versHandle) {
	  MoveHHi (theScreen.versHandle);
	  HLock (theScreen.versHandle);
	  ParamText(*(theScreen.versHandle), nil, nil, nil);
	  }

	ShowWindow((WindowPtr) theDialog);

	do {
		ModalDialog(nil, &itemHit);
	} while (itemHit != ok);

	if (theScreen.versHandle)
	  HUnlock(theScreen.versHandle);

	DisposDialog(theDialog);

	return;
}

static void DoColorsDialog()

{
	DialogPtr theDialog;
	long saveForeColor, saveBackColor;
	short itemHit;
	short itsType;
	Handle itsHandle, fgHandle, bgHandle;
	Rect itsRect;
	GrafPtr savePort;
	long redrawFlag;
	long h, v;

	theDialog = GetNewDialog(colorsDlgID, nil, (WindowPtr) -1);

	CenterScreenDLOG(colorsDlgID, fixHalf, fixThird, &h, &v);
	MoveWindow((WindowPtr) theDialog, (short) h, (short) v, false);

	GetDItem(theDialog, ok, &itsType, &itsHandle, &itsRect);
	InsetRect(&itsRect, -4, -4);

	SetDItem(theDialog, colorsDfltBorder, userItem,
		 (Handle) DrawDefaultBorder, &itsRect);

	GetDItem(theDialog, foregroundRect, &itsType, &itsHandle, &itsRect);
	SetDItem(theDialog, foregroundRect, itsType, (Handle) DrawGroupRect,
		 &itsRect);
	GetDItem(theDialog, backgroundRect, &itsType, &itsHandle, &itsRect);
	SetDItem(theDialog, backgroundRect, itsType, (Handle) DrawGroupRect,
		 &itsRect);

	GetDItem(theDialog, foreColors+theScreen.colorStdFore, &itsType,
		 &fgHandle, &itsRect);
	SetCtlValue((ControlHandle) fgHandle, true);

	GetDItem(theDialog, backColors+theScreen.colorStdBack, &itsType,
		 &bgHandle, &itsRect);
	SetCtlValue((ControlHandle) bgHandle, true);

	ShowWindow((WindowPtr) theDialog);

	saveForeColor = theScreen.colorStdFore;
	saveBackColor = theScreen.colorStdBack;

	do {
		redrawFlag = false;
		ModalDialog(nil, &itemHit);
		if ( (itemHit >= foreColors) &&
			(itemHit <= (foreColors+7)) &&
			(itemHit != (foreColors+theScreen.colorStdFore)) ) {
				theScreen.colorStdFore = itemHit - foreColors;
				SetCtlValue((ControlHandle) fgHandle, false);
				GetDItem(theDialog, foreColors+theScreen.colorStdFore,
					&itsType, &fgHandle, &itsRect);
				SetCtlValue((ControlHandle) fgHandle, true);
				redrawFlag = true;
		}
		else if ( (itemHit >= backColors) &&
			(itemHit <= (backColors+7)) &&
			(itemHit != (backColors+theScreen.colorStdBack)) ) {
				theScreen.colorStdBack = itemHit - backColors;
				SetCtlValue((ControlHandle) bgHandle, false);
				GetDItem(theDialog, backColors+theScreen.colorStdBack,
					&itsType, &bgHandle, &itsRect);
				SetCtlValue((ControlHandle) bgHandle, true);
				redrawFlag = true;
		}
		if (redrawFlag) {
			GetPort(&savePort);
			SetPort(theScreen.window);
			InvalRect(&theScreen.picPRect);
			UpdateScreen();
			SetPort(savePort);
		}
	} while ( (itemHit != ok) && (itemHit != cancel) );

	if (itemHit == cancel)
		if ( (theScreen.colorStdFore != saveForeColor) ||
			(theScreen.colorStdBack != saveBackColor) ) {
			theScreen.colorStdFore = saveForeColor;
			theScreen.colorStdBack = saveBackColor;
			GetPort(&savePort);
			SetPort(theScreen.window);
			InvalRect(&theScreen.picPRect);
			UpdateScreen();
			SetPort(savePort);
		}

	DisposDialog(theDialog);

	return;
}

static void DoAppleMenu(item)
long item;

{
	GrafPtr savePort;
	Str255 daName;

	switch (item) {

		case aboutItem:		if (theScreen.aboutProc != NULL)
								(*theScreen.aboutProc)();
							else
								DoAboutDialog();
							break;

		default:			GetItem(GetMHandle(appleID), item, daName);
							GetPort(&savePort);
							OpenDeskAcc(daName);
							SetPort(savePort);
							break;

	}

	return;
}

static void DoFileMenu(item)
long item;

{
	switch (item) {

		case closeBoxItem:	HideWindow(theScreen.window);
							break;

		case openItem:		ShowWindow(theScreen.window);
							break;

		case closeItem:		HideWindow(theScreen.window);
							break;

		case quitItem:		if (theScreen.quitProc != NULL) {
								if (!theScreen.quitReturns) HiliteMenu(0);
								(*theScreen.quitProc)();
							}
							break;

	}

	return;
}

static void DoEditMenu(item)
long item;

{
	switch (item) {

		case undoItem:
		case cutItem:
		case copyItem:
		case pasteItem:
		case clearItem:		SystemEdit(item-1);
							break;

	}

	return;
}

static void DoFontChange(hndl, item, offset)
MenuHandle hndl;
long item, offset;

{
	if (item != theScreen.txNum) {
		if (theScreen.txNum > 0)
			CheckItem(hndl, theScreen.txNum, false);
		ChangeScreenFontSize(item - offset);
		theScreen.txNum = item;
		CheckItem(hndl, theScreen.txNum, true);
	}

	return;
}

static void DoScreenMenu(item)
long item;

{
	switch (item) {

		case colorsItem:	DoColorsDialog();
							break;

		default:			DoFontChange(GetMHandle(screenID), item, 3);
							break;

	}

	return;
}

static void DoFontSizeMenu(item)
long item;

{
	DoFontChange(GetMHandle(fontSizeID), item, 1L);
	return;
}

static void DoMenuItem(select)
long select;

{
	long menuID, menuItem;

	menuID = HiWord(select);
	menuItem = LoWord(select);

	switch (menuID) {

		case appleID:		DoAppleMenu(menuItem);
							break;

		case fileID1:
		case fileID2:		if (theScreen.fileMenuProc != NULL)
								(*theScreen.fileMenuProc)(menuItem);
							else
								DoFileMenu(menuItem);
							break;

		case editID1:
		case editID2:		DoEditMenu(menuItem);
							break;

		case screenID:		DoScreenMenu(menuItem);
							break;

		case fontSizeID:	DoFontSizeMenu(menuItem);
							break;

		case appID1:
		case appID2:		if (theScreen.appMenuProc != NULL)
								(*theScreen.appMenuProc)(menuItem);
							break;

	}

	HiliteMenu(0);

	return;
}

static void DoDrag(whichWindow, cursorLoc)
WindowPtr whichWindow;
Point *cursorLoc;

{
	if (whichWindow == theScreen.window)
		DragWindow(whichWindow, *cursorLoc, &theScreen.dragLimitGRect);

	return;
}

static void HandlePreGrow(oldWidth, oldHeight, newWidth, newHeight)
long oldWidth, oldHeight, newWidth, newHeight;

{
	if (newWidth > oldWidth) {
		InvalRect(&theScreen.vScrollPRect);
		InvalRect(&theScreen.growPRect);
	}
	if (newHeight > oldHeight) {
		InvalRect(&theScreen.hScrollPRect);
		InvalRect(&theScreen.growPRect);
	}
	return;
}

static void HandlePostGrow(oldWidth, oldHeight, newWidth, newHeight)
long oldWidth, oldHeight, newWidth, newHeight;

{
	if ( (newWidth < oldWidth) || (newHeight < oldHeight) )
		InvalRect(&theScreen.growPRect);
	HideControl(theScreen.hScrollHandle);
	HideControl(theScreen.vScrollHandle);
	if (newWidth != oldWidth) {
		MoveControl(theScreen.vScrollHandle,
			theScreen.vScrollPRect.left, theScreen.vScrollPRect.top);
		SizeControl(theScreen.hScrollHandle,
			theScreen.hScrollPRect.right - theScreen.hScrollPRect.left,
			theScreen.hScrollPRect.bottom - theScreen.hScrollPRect.top);
		SetCtlMax(theScreen.hScrollHandle,
			theScreen.scrnXLRect.right - theScreen.picPRect.right);
	}
	if (newHeight != oldHeight) {
		MoveControl(theScreen.hScrollHandle,
			theScreen.hScrollPRect.left, theScreen.hScrollPRect.top);
		SizeControl(theScreen.vScrollHandle,
			theScreen.vScrollPRect.right - theScreen.vScrollPRect.left,
			theScreen.vScrollPRect.bottom - theScreen.vScrollPRect.top);
		SetCtlMax(theScreen.vScrollHandle,
			theScreen.scrnXLRect.bottom - theScreen.picPRect.bottom);
	}
	RepositionScreen(0, 0);
	ShowControl(theScreen.hScrollHandle);
	ShowControl(theScreen.vScrollHandle);
	ValidRect(&theScreen.hScrollPRect);
	ValidRect(&theScreen.vScrollPRect);
	return;
}

static void DoGrow(whichWindow, cursorLoc)
WindowPtr whichWindow;
Point *cursorLoc;

{
	long newSize;
	long oldWidth, oldHeight;
	long newWidth, newHeight;

	if (whichWindow == theScreen.window) {
		oldWidth = theScreen.windowW;
		oldHeight = theScreen.windowH;
		newSize = GrowWindow(whichWindow, *cursorLoc, &theScreen.sizeLimitPRect);
		if (newSize) {
			newWidth = LoWord(newSize);
			newHeight = HiWord(newSize);
			HandlePreGrow(oldWidth, oldHeight, newWidth, newHeight);
			SizeWindow(whichWindow, newWidth, newHeight, true);
			SetScreenPortRects();
			SetScreenDrawRects();
			HandlePostGrow(oldWidth, oldHeight, newWidth, newHeight);
		}

	}

	return;
}

static void DoZoom(whichWindow, part)
WindowPtr whichWindow;
long part;

{
	WStateData **zoomInfo;
	Rect newSize;
	long oldWidth, oldHeight;
	long newWidth, newHeight;

	if (whichWindow == theScreen.window) {
		oldWidth = theScreen.windowW;
		oldHeight = theScreen.windowH;
		zoomInfo = (WStateData **) ((WindowPeek) whichWindow)->dataHandle;
		if (part == inZoomIn)
			newSize = ((*zoomInfo)->userState);
		else
			newSize = ((*zoomInfo)->stdState);
		newWidth = newSize.right - newSize.left;
		newHeight = newSize.bottom - newSize.top;
		HandlePreGrow(oldWidth, oldHeight, newWidth, newHeight);
		EraseRect(&theScreen.window->portRect);
		ZoomWindow(whichWindow, part, false);
		SetScreenPortRects();
		SetScreenDrawRects();
		HandlePostGrow(oldWidth, oldHeight, newWidth, newHeight);
	}

	return;
}

static pascal void ActOnScroll(theControl, partCode)
ControlHandle theControl;
short partCode;

{
	int delta;

	if (theControl == theScreen.hScrollHandle) {

		switch (partCode) {
			case inUpButton:
				delta = -theScreen.charPDims.h;
				break;
			case inDownButton:
				delta = theScreen.charPDims.h;
				break;
			case inPageUp:
				delta = theScreen.charPDims.h - theScreen.picPRect.right;
				break;
			case inPageDown:
				delta = theScreen.picPRect.right - theScreen.charPDims.h;
				break;
			default:
				delta = 0;
				break;
		}

		if (delta)
			if (RepositionScreen(delta, 0)) UpdateScreen();

	}

	if (theControl == theScreen.vScrollHandle) {

		switch (partCode) {
			case inUpButton:
				delta = -theScreen.charPDims.v;
				break;
			case inDownButton:
				delta = theScreen.charPDims.v;
				break;
			case inPageUp:
				delta = theScreen.charPDims.v - theScreen.picPRect.bottom;
				break;
			case inPageDown:
				delta = theScreen.picPRect.bottom - theScreen.charPDims.v;
				break;
			default:
				delta = 0;
				break;
		}

		if (delta)
			if (RepositionScreen(0, delta)) UpdateScreen();

	}

	return;
}

static void DoHScroll(cursorLoc, thePart)
Point *cursorLoc;
long thePart;

{
	switch (thePart) {

		case inUpButton:
		case inDownButton:
		case inPageUp:
		case inPageDown:
			TrackControl(theScreen.hScrollHandle, *cursorLoc, (ProcPtr) ActOnScroll);
			break;

		case inThumb:
			TrackControl(theScreen.hScrollHandle, *cursorLoc, NULL);
			break;

	}

	RepositionScreen(0, 0);

	return;
}

static void DoVScroll(cursorLoc, thePart)
Point *cursorLoc;
long thePart;

{
	switch (thePart) {

		case inUpButton:
		case inDownButton:
		case inPageUp:
		case inPageDown:
			TrackControl(theScreen.vScrollHandle, *cursorLoc, (ProcPtr) ActOnScroll);
			break;

		case inThumb:
			TrackControl(theScreen.vScrollHandle, *cursorLoc, NULL);
			break;

	}

	RepositionScreen(0, 0);

	return;
}

static void DoCharClick()

{
	int reversed;
	short h, v;
	char c, an, ar;
	char modifiers;
	Rect charCell;
	Point mouse;
	void GetScreenCharAttr(), XSetScreenCharAttr(), UpdateScreen();

	if ( (theScreen.event.when > theScreen.mouseFlush)		&&
			(theScreen.window == FrontWindow())				&&
			(((WindowPeek) theScreen.window)->visible) ) {

		if (GtoC(theScreen.event.where.h, theScreen.event.where.v, &h, &v)) {

			modifiers = (char) ((theScreen.event.modifiers & 0xF0) >> 8);
			modifiers |= maskModMouse;

			charCell.left = charCell.right = h;
			charCell.top = charCell.bottom = v;
			charCell.right++;
			charCell.bottom++;

			charCell.left *= theScreen.charPDims.h;
			charCell.top *= theScreen.charPDims.v;
			charCell.right *= theScreen.charPDims.h;
			charCell.bottom *= theScreen.charPDims.v;

			OffsetRect(&charCell, -theScreen.origin.h, -theScreen.origin.v);

			GetScreenCharAttr(&c, &an, h, v);
			ar = (an & maskAttrFlags) | (MakeAttr(AttrBack(an), AttrFore(an)));

			XSetScreenCharAttr(false, c, ar, h, v);
			UpdateScreen();
			reversed = true;

			while (StillDown()) {
				GetMouse(&mouse);
				if (reversed) {
					if (!PtInRect(mouse, &charCell)) {
						XSetScreenCharAttr(false, c, an, h, v);
						UpdateScreen();
						reversed = false;
					}
				}
				else {
					if (PtInRect(mouse, &charCell)) {
						XSetScreenCharAttr(false, c, ar, h, v);
						UpdateScreen();
						reversed = true;
					}
				}
			}

			if (reversed) {

				XSetScreenCharAttr(false, c, an, h, v);
				UpdateScreen();
				reversed = false;

				PushQueue(0, modifiers, 0, h, v);

			}

		}

	}

	return;
}

static void DoContentClick(whichWindow, cursorLoc)
WindowPtr whichWindow;
Point *cursorLoc;

{
	short thePart;
	ControlHandle theControl;
	Point localCursor;

	if (whichWindow == theScreen.window) {
		localCursor = *cursorLoc;
		GlobalToLocal(&localCursor);
		if (thePart = FindControl(localCursor, whichWindow, &theControl)) {
			if (theControl == theScreen.hScrollHandle)
				DoHScroll(&localCursor, thePart);
			else if (theControl == theScreen.vScrollHandle)
				DoVScroll(&localCursor, thePart);
		}
		else
			if (theScreen.mouseFlag) DoCharClick();
	}

	return;
}

static void DoMouseDown(cursorLoc)
Point *cursorLoc;

{
	long part;
	WindowPtr whichWindow;

	switch (part = FindWindow(*cursorLoc, &whichWindow)) {

		case inMenuBar:		DoMenuItem(MenuSelect(*cursorLoc));
							break;

		case inDrag:		DoDrag(whichWindow, cursorLoc);
							break;

		case inGrow:		DoGrow(whichWindow, cursorLoc);
							break;

		case inSysWindow:	SystemClick(&theScreen.event, whichWindow);
							break;

		case inContent:		if (whichWindow != FrontWindow())
								SelectWindow(whichWindow);
							else
								DoContentClick(whichWindow, cursorLoc);
							break;

		case inZoomIn:
		case inZoomOut:		if (whichWindow == theScreen.window)
								if (TrackBox(whichWindow, *cursorLoc, part))
									DoZoom(whichWindow, part);
							break;

		case inGoAway:		if (whichWindow == theScreen.window)
								if (TrackGoAway(whichWindow, *cursorLoc)) {
									if (theScreen.fileMenuProc != NULL)
										(*theScreen.fileMenuProc)(closeBoxItem);
									else
										DoFileMenu(closeBoxItem);
								}
							break;

	}

	return;
}

static void DoKeyDown(cmdFlag)
int cmdFlag;

{
	char keycode, modifiers, ascii;
	char upper;

	if ( (theScreen.event.when > theScreen.keyFlush)		&&
			(theScreen.window == FrontWindow())		&&
			(((WindowPeek) theScreen.window)->visible) ) {

		ObscureCursor();

		keycode = (char) ((theScreen.event.message & keyCodeMask) >> 8);

		modifiers = (char) ((theScreen.event.modifiers & 0xFF00) >> 8);
		modifiers &= ~maskModMouse;

		ascii = (char) (theScreen.event.message & charCodeMask);
		if (theScreen.escMapFlag) {
			if (ascii == BACKQUOTE) ascii = ESC;
		}
		if (cmdFlag) {
			upper = ascii;
			if ( (upper >= 'a') && (upper <= 'z') )
				upper = 'A' + (upper - 'a');
			if ( (upper >= '@') && (upper <= '_') )
				ascii = upper - '@';
		}

		PushQueue(keycode, modifiers, ascii, 0, 0);

	}

	return;
}

static void DoUpdate(whichWindow)
WindowPtr whichWindow;

{
	if (whichWindow == theScreen.window) {
		UpdateScreen();
	}

	return;
}

static void DoActivate(whichWindow, activated)
WindowPtr whichWindow;
Boolean activated;

{
	MenuHandle menu;

	menu = GetMHandle(theScreen.cmdKeyFlag ? editID1 : editID2);

	if (whichWindow == theScreen.window) {

		if (activated) {
			DisableItem(menu, undoItem);
			DisableItem(menu, cutItem);
			DisableItem(menu, copyItem);
			DisableItem(menu, pasteItem);
			DisableItem(menu, clearItem);
			ShowControl(theScreen.vScrollHandle);
			ShowControl(theScreen.hScrollHandle);
			DrawGrowIcon(whichWindow);
		}
		else {
			EnableItem(menu, undoItem);
			EnableItem(menu, cutItem);
			EnableItem(menu, copyItem);
			EnableItem(menu, pasteItem);
			EnableItem(menu, clearItem);
			HideControl(theScreen.vScrollHandle);
			HideControl(theScreen.hScrollHandle);
			DrawGrowIcon(whichWindow);
		}
	}

	return;
}

static void InvalScreenRect(d, r)
int d;
Rect *r;

{
	register long v;
	register short *left, *right;
	short rl, rt, rr, rb;

	if (d) {

		rl = r->left;
		rt = r->top;
		rr = r->right;
		rb = r->bottom;

		left = *theScreen.updLeft + rt;
		right = *theScreen.updRight + rt;

		for (v = rt; v < rb; v++, left++, right++) {
			if (rl < *left) *left = rl;
			if (rr > *right) *right = rr;
		}

	}

	else {

		r->left *= theScreen.charPDims.h;
		r->right *= theScreen.charPDims.h;
		r->top *= theScreen.charPDims.v;
		r->bottom *= theScreen.charPDims.v;

		SetOrigin(theScreen.origin.h, theScreen.origin.v);
		InvalRect(r);
		SetOrigin(0, 0);

	}

	return;
}

static void InvalCursor(d)
int d;

{
	Rect curs;

	curs.left = curs.right = theScreen.cursor.h;
	curs.top = curs.bottom = theScreen.cursor.v;
	curs.right += 1;
	curs.bottom += 1;

	if (d)
		InvalScreenRect(d, &curs);

	else {

		curs.left *= theScreen.charPDims.h;
		curs.right *= theScreen.charPDims.h;
		curs.top = curs.bottom = curs.bottom * theScreen.charPDims.v;
		curs.top -= theScreen.cursorLines;

		SetOrigin(theScreen.origin.h, theScreen.origin.v);
		InvalRect(&curs);
		SetOrigin(0, 0);

	}

	return;
}

void XSetScreenChar(d, c, h, v)
int d;
char c;
long h, v;

{
    long loc;
    Rect area;
	Point pos;

	pos.h = h;
	pos.v = v;

    if (PtInRect(pos, &theScreen.scrnCRect)) {

		loc = v * theScreen.scrnCDims.h + h;
		(*theScreen.chars)[loc] = c;

		area.left = area.right = h;
		area.right += 1;
		area.top = area.bottom = v;
		area.bottom += 1;

		InvalScreenRect(d, &area);

    }

    return;
}

void XSetScreenBuffer(d, c, row, bounds, h, v)
int d;
char *c;
long row;
Rect *bounds;
long h, v;

{
    long i;
    long wid;
    long srcLoc, dstLoc;
    char *srcC, *dstC;
    Rect temp, area;

    temp.right = temp.left = h;
    temp.right += bounds->right - bounds->left;
    temp.bottom = temp.top = v;
    temp.bottom += bounds->bottom - bounds->top;

    if (SectRect(&temp, &theScreen.scrnCRect, &area)) {

		srcLoc = (area.top + bounds->top - v) * row +
			 (area.left + bounds->left - h);
		dstLoc = area.top * theScreen.scrnCDims.h + area.left;

		srcC = c + srcLoc;
		dstC = *theScreen.chars + dstLoc;

		wid = area.right - area.left;

		if ( (wid == row) && (wid == theScreen.scrnCDims.h) ) {
			wid *= (area.bottom - area.top);
			memcpy(dstC, srcC, wid);
		}

		else {

			for (i = area.top; i < area.bottom; i++) {
				memcpy(dstC, srcC, wid);
				srcC += row;
				dstC += theScreen.scrnCDims.h;
			}

		}

		InvalScreenRect(d, &area);

    }

    return;
}

void XSetScreenString(d, s, h, v)
int d;
char *s;
long h, v;

{
    Rect bounds;

    bounds.left = 0;
    bounds.right = strlen(s);
    bounds.top = 0;
    bounds.bottom = 1;

    XSetScreenBuffer(d, s, bounds.right, &bounds, h, v);

    return;
}

void XSetScreenCharAttr(d, c, a, h, v)
int d;
char c;
char a;
long h, v;

{
    long loc;
    Rect area;
	Point pos;

	pos.h = h;
	pos.v = v;

    if (PtInRect(pos, &theScreen.scrnCRect)) {

		loc = v * theScreen.scrnCDims.h + h;
		(*theScreen.chars)[loc] = c;
		(*theScreen.attrs)[loc] = a;

		area.left = area.right = h;
		area.right += 1;
		area.top = area.bottom = v;
		area.bottom += 1;

		InvalScreenRect(d, &area);

    }

    return;
}

void XSetScreenBufferAttr(d, c, a, row, bounds, h, v)
int d;
char *c;
char a;
long row;
Rect *bounds;
long h, v;

{
    long i;
    long wid;
    long srcLoc, dstLoc;
    char *srcC, *dstC;
    char *dstA;
    Rect temp, area;

    temp.right = temp.left = h;
    temp.right += bounds->right - bounds->left;
    temp.bottom = temp.top = v;
    temp.bottom += bounds->bottom - bounds->top;

    if (SectRect(&temp, &theScreen.scrnCRect, &area)) {

		srcLoc = (area.top + bounds->top - v) * row +
			 (area.left + bounds->left - h);
		dstLoc = area.top * theScreen.scrnCDims.h + area.left;

		srcC = c + srcLoc;
		dstC = *theScreen.chars + dstLoc;

		dstA = *theScreen.attrs + dstLoc;

		wid = area.right - area.left;

		if ( (wid == row) && (wid == theScreen.scrnCDims.h) ) {
			wid *= (area.bottom - area.top);
			memcpy(dstC, srcC, wid);
			memset(dstA, a, wid);
		}

		else {

			for (i = area.top; i < area.bottom; i++) {
				memcpy(dstC, srcC, wid);
				memset(dstA, a, wid);
				srcC += row;
				dstC += theScreen.scrnCDims.h;
				dstA += theScreen.scrnCDims.h;
			}

		}

		InvalScreenRect(d, &area);

    }

    return;
}

void XSetScreenStringAttr(d, s, a, h, v)
int d;
char *s;
char a;
long h, v;

{
    Rect bounds;

    bounds.left = 0;
    bounds.right = strlen(s);
    bounds.top = 0;
    bounds.bottom = 1;

    XSetScreenBufferAttr(d, s, a, bounds.right, &bounds, h, v);

    return;
}

void XSetScreenImage(d, c, a, row, bounds, h, v)
int d;
char *c;
char *a;
long row;
Rect *bounds;
long h, v;

{
    long i;
    long wid;
    long srcLoc, dstLoc;
    char *srcC, *dstC;
    char *srcA, *dstA;
    Rect temp, area;

    temp.right = temp.left = h;
    temp.right += bounds->right - bounds->left;
    temp.bottom = temp.top = v;
    temp.bottom += bounds->bottom - bounds->top;

    if (SectRect(&temp, &theScreen.scrnCRect, &area)) {

		srcLoc = (area.top + bounds->top - v) * row +
			 (area.left + bounds->left - h);
		dstLoc = area.top * theScreen.scrnCDims.h + area.left;

		srcC = c + srcLoc;
		dstC = *theScreen.chars + dstLoc;

		srcA = a + srcLoc;
		dstA = *theScreen.attrs + dstLoc;

		wid = area.right - area.left;

		if ( (wid == row) && (wid == theScreen.scrnCDims.h) ) {
			wid *= (area.bottom - area.top);
			memcpy(dstC, srcC, wid);
			memcpy(dstA, srcA, wid);
		}

		else {

			for (i = area.top; i < area.bottom; i++) {
				memcpy(dstC, srcC, wid);
				memcpy(dstA, srcA, wid);
				srcC += row;
				srcA += row;
				dstC += theScreen.scrnCDims.h;
				dstA += theScreen.scrnCDims.h;
			}

		}

		InvalScreenRect(d, &area);

    }

    return;
}

void XWriteScreenChar(d, c)
int d;
char c;

{
	InvalCursor(d);

    XSetScreenChar(d, c, theScreen.cursor.h, theScreen.cursor.v);
    theScreen.cursor.h++;

	InvalCursor(d);

    return;
}

void XWriteScreenBuffer(d, c, row, bounds)
int d;
char *c;
long row;
Rect *bounds;

{
    XSetScreenBuffer(d, c, row, bounds, theScreen.cursor.h,
		     theScreen.cursor.v);
    theScreen.cursor.h += bounds->right - bounds->left;
    theScreen.cursor.v += bounds->bottom - bounds->top;

	InvalCursor(d);

    return;
}

void XWriteScreenString(d, s)
int d;
char *s;

{
    XSetScreenString(d, s, theScreen.cursor.h, theScreen.cursor.v);
    theScreen.cursor.h += strlen(s);

	InvalCursor(d);

    return;
}

void XWriteScreenCharAttr(d, c, a)
int d;
char c;
char a;

{
    XSetScreenCharAttr(d, c, a, theScreen.cursor.h, theScreen.cursor.v);
    theScreen.cursor.h++;

	InvalCursor(d);

    return;
}

void XWriteScreenBufferAttr(d, c, a, row, bounds)
int d;
char *c;
char a;
long row;
Rect *bounds;

{
    XSetScreenBufferAttr(d, c, a, row, bounds, theScreen.cursor.h,
			 theScreen.cursor.v);
    theScreen.cursor.h += bounds->right - bounds->left;
    theScreen.cursor.v += bounds->bottom - bounds->top;

	InvalCursor(d);

    return;
}

void XWriteScreenStringAttr(d, s, a)
int d;
char *s;
char a;

{
    XSetScreenStringAttr(d, s, a, theScreen.cursor.h, theScreen.cursor.v);
    theScreen.cursor.h += strlen(s);

	InvalCursor(d);

    return;
}

void XWriteScreenImage(d, c, a, row, bounds)
int d;
char *c;
char *a;
long row;
Rect *bounds;

{
    XSetScreenImage(d, c, a, row, bounds, theScreen.cursor.h,
		    theScreen.cursor.v);
    theScreen.cursor.h += bounds->right - bounds->left;
    theScreen.cursor.v += bounds->bottom - bounds->top;

	InvalCursor(d);

    return;
}

void XFillScreen(d, c, a, bounds)
int d;
char c;
char a;
Rect *bounds;

{
    long i;
    long wid;
    long dstLoc;
    char *dstC;
    char *dstA;
    Rect area;

    if (SectRect(bounds, &theScreen.scrnCRect, &area)) {

		dstLoc = area.top * theScreen.scrnCDims.h + area.left;

		dstC = *theScreen.chars + dstLoc;

		dstA = *theScreen.attrs + dstLoc;

		wid = area.right - area.left;

		if (wid == theScreen.scrnCDims.h) {
			wid *= (area.bottom - area.top);
			memset(dstC, c, wid);
			memset(dstA, a, wid);
		}

		else {

			for (i = area.top; i < area.bottom; i++) {
				memset(dstC, c, wid);
				memset(dstA, a, wid);
				dstC += theScreen.scrnCDims.h;
				dstA += theScreen.scrnCDims.h;
			}

		}

		InvalScreenRect(d, &area);

    }

    return;
}

void XEraseScreen(d, bounds)
int d;
Rect *bounds;

{
    XFillScreen(d, ' ', attrNormal, bounds);
    return;
}

static void ScrollScreenPositive(area, offset, factor)
Rect *area;
long offset;
short factor;

{
    register short i, j;
    short loc;		/* Index of initial destination.  */
	short nxt;		/* Amount to skip going from one row to the next.  */
    register char *srcc, *dstc;
    register char *srca, *dsta;

    loc = (area->bottom - 1) * theScreen.scrnCDims.h + area->right - 1;

    dstc = *theScreen.chars + loc;
    srcc = dstc - offset * factor;

    dsta = *theScreen.attrs + loc;
    srca = dsta - offset * factor;

    nxt = theScreen.scrnCDims.h - (area->right - area->left);

    for (j = area->bottom; j > area->top; j--) {
		for (i = area->right; i > area->left; i--) {
			*dstc-- = *srcc--;
			*dsta-- = *srca--;
		}
		srcc -= nxt;
		dstc -= nxt;
		srca -= nxt;
		dsta -= nxt;
    }

    return;
}

static void ScrollScreenNegative(area, offset, factor)
Rect *area;
long offset;
short factor;

{
    register short i, j;
    short loc;
	short nxt;
    register char *srcc, *dstc;
    register char *srca, *dsta;

    loc = area->top * theScreen.scrnCDims.h + area->left;

    dstc = *theScreen.chars + loc;
    srcc = dstc - offset * factor;	/* Offset is negative!  */

    dsta = *theScreen.attrs + loc;
    srca = dsta - offset * factor;

    nxt = theScreen.scrnCDims.h - (area->right - area->left);

    for (j = area->top; j < area->bottom; j++) {
		for (i = area->left; i < area->right; i++) {
			*dstc++ = *srcc++;
			*dsta++ = *srca++;
		}
		srcc += nxt;
		dstc += nxt;
		srca += nxt;
		dsta += nxt;
    }

    return;
}

void XScrollScreen(d, dh, dv, bounds, attr)
int d;
long dh, dv;
Rect *bounds;
char attr;

{
    long wid, dep;
    Rect area, clear;

    if (!dh && !dv) return;

    if (SectRect(bounds, &theScreen.scrnCRect, &area)) {

		wid = area.right - area.left;
		dep = area.bottom - area.top;

		if ( (ABS(dh) >= wid) || (ABS(dv) >= dep) )

			XFillScreen(d, ' ', attr, &area);

		else {

			if (dv > 0) {
				clear = area;
				clear.bottom = area.top += dv;
				ScrollScreenPositive(&area, dv, (short)theScreen.scrnCDims.h);
				XFillScreen(d, ' ', attr, &clear);
			}

			else if (dv < 0) {
				clear = area;
				clear.top = area.bottom += dv;
				ScrollScreenNegative(&area, dv, (short)theScreen.scrnCDims.h);
				XFillScreen(d, ' ', attr, &clear);
			}

			if (dh > 0) {
				clear = area;
				clear.right = area.left += dh;
				ScrollScreenPositive(&area, dh, (short)1);
				XFillScreen(d, ' ', attr, &clear);
			}

			else if (dh < 0) {
				clear = area;
				clear.left = area.right += dh;
				ScrollScreenNegative(&area, dh, (short)1);
				XFillScreen(d, ' ', attr, &clear);
			}

			InvalScreenRect(d, &area);

		}

    }

    return;
}

void GetScreenCharAttr(c, a, h, v)
char *c;
char *a;
long h, v;

{
    long loc;
	Point pos;

	pos.h = h;
	pos.v = v;

    if (PtInRect(pos, &theScreen.scrnCRect)) {

		loc = v * theScreen.scrnCDims.h + h;
		*c = (*theScreen.chars)[loc];
		*a = (*theScreen.attrs)[loc];

    }

    return;
}

void GetScreenImage(c, a, row, bounds, h, v)
char *c;
char *a;
long row;
Rect *bounds;
long h, v;

{
    long i;
    long wid;
    long srcLoc, dstLoc;
    char *srcC, *dstC;
    char *srcA, *dstA;
    Rect temp, area;

    temp.right = temp.left = h;
    temp.right += bounds->right - bounds->left;
    temp.bottom = temp.top = v;
    temp.bottom += bounds->bottom - bounds->top;

    if (SectRect(&temp, &theScreen.scrnCRect, &area)) {

		dstLoc = (area.top + bounds->top - v) * row +
			 (area.left + bounds->left - h);
		srcLoc = area.top * theScreen.scrnCDims.h + area.left;

		dstC = c + dstLoc;
		srcC = *theScreen.chars + srcLoc;

		dstA = a + dstLoc;
		srcA = *theScreen.attrs + srcLoc;

		wid = area.right - area.left;

		if ( (wid == row) && (wid == theScreen.scrnCDims.h) ) {
			wid *= (area.bottom - area.top);
			memcpy(dstC, srcC, wid);
			memcpy(dstA, srcA, wid);
		}

		else {

			for (i = area.top; i < area.bottom; i++) {
				memcpy(dstC, srcC, wid);
				memcpy(dstA, srcA, wid);
				dstC += row;
				dstA += row;
				srcC += theScreen.scrnCDims.h;
				srcA += theScreen.scrnCDims.h;
			}

		}

    }

    return;
}

void XMoveScreenCursor(d, h, v)
int d;
long h, v;

{
	InvalCursor(d);

    theScreen.cursor.h += h;
    theScreen.cursor.v += v;

	InvalCursor(d);

    return;
}

void XSetScreenCursor(d, h, v)
int d;
long h, v;

{
	InvalCursor(d);

    theScreen.cursor.h = h;
    theScreen.cursor.v = v;

	InvalCursor(d);

    return;
}

void GetScreenCursor(h, v)
long *h, *v;
{
    *h = theScreen.cursor.h;
    *v = theScreen.cursor.v;
    return;
}

static long CheckCursorStatus()

{
	long oldStatus;
	long changed;

	oldStatus = theScreen.cursorStatus;

	if (theScreen.cursorLevel <= 0)
		theScreen.cursorStatus = 0;
	else if (!theScreen.cursorBlink)
		theScreen.cursorStatus = 1;
	else if (theScreen.cursorChangeTick <= TickCount())
		theScreen.cursorStatus = !oldStatus;

	changed = theScreen.cursorStatus != oldStatus;

	if (changed) {
		theScreen.cursorChangeTick = TickCount() + theScreen.cursorBlink;
		InvalCursor(0);
	}

	return(changed);
}

static void InvalDelayed()

{
	long v;
	short *left, *right;
	Rect inval;

	v = 0;
	left = *theScreen.updLeft;
	right = *theScreen.updRight;

	while (v < theScreen.scrnCDims.v) {

		if (!*right) {

			v++;
			left++;
			right++;

		}

		else {

			inval.top = v;
			inval.left = *left;
			inval.right = *right;

			do {
				v++;
				*left++ = theScreen.scrnCDims.h;
				*right++ = 0;
			} while ( (v < theScreen.scrnCDims.v) &&
					  (*left == inval.left)		 &&
					  (*right == inval.right)		);

			inval.bottom = v;

			InvalScreenRect(0, &inval);

			left = *theScreen.updLeft + v;
			right = *theScreen.updRight + v;

		}

	}

	return;

}

static void UpdateScreenLine(area, c, a, len)
Rect *area;
char *c;
char *a;
long len;

{
    long count;
    char attr;
    char *last, *prev;
	short face;
	Rect temp;

	temp = *area;

    last = a + len;

    while (a < last) {

		attr = *a;

		prev = a;
		while ( (a < last) && (*a == attr) ) a++;
		count = a - prev;

		temp.right = temp.left + count * theScreen.charPDims.h;

		face = normal;
		if (attr & attrUnderlined) face |= underline;
		if (attr & attrItalicized) face |= italic;
		TextFace(face);

		if (theScreen.colorFlag) {
			ForeColor(colors[AttrFore(attr)]);
			BackColor(colors[AttrBack(attr)]);
		}
		else {
			ForeColor(
				colors[
					(AttrFore(attr) == attrColorBack) ?
						theScreen.colorStdBack : theScreen.colorStdFore]);
			BackColor(
				colors[
					(AttrBack(attr) == attrColorBack) ?
						theScreen.colorStdBack : theScreen.colorStdFore]);
		}

		EraseRect(&temp);

		DrawText(c, 0, count);

		temp.left = temp.right;

		c += count;

    }

    return;
}

void UpdateScreen()

{
    long j;
	long top, dep, left, wid;
    Rect clear, area, curs;
	RgnHandle vis;
    char *c, *a;
	long cursorLoc;

	InvalDelayed();

    BeginUpdate(theScreen.window);

	SetOrigin(theScreen.origin.h, theScreen.origin.v);

	ClipRect(&theScreen.picLRect);

	if (!theScreen.colorFlag)
		BackColor(colors[theScreen.colorStdBack]);

	if (theScreen.picLRect.top < theScreen.scrnLRect.top) {
		clear = theScreen.picLRect;
		clear.bottom = theScreen.scrnLRect.top;
		EraseRect(&clear);
	}

	if (theScreen.picLRect.left < theScreen.scrnLRect.left) {
		clear = theScreen.picLRect;
		clear.right = theScreen.scrnLRect.left;
		EraseRect(&clear);
	}

	if (theScreen.picLRect.right > theScreen.scrnLRect.right) {
		clear = theScreen.picLRect;
		clear.left = theScreen.scrnLRect.right;
		EraseRect(&clear);
	}

	if (theScreen.picLRect.bottom > theScreen.scrnLRect.bottom) {
		clear = theScreen.picLRect;
		clear.top = theScreen.scrnLRect.bottom;
		EraseRect(&clear);
	}

	ClipRect(&theScreen.drawLRect);

	vis = theScreen.window->visRgn;

	top = theScreen.drawXCRect.top;
	dep = theScreen.drawXCRect.bottom - top;

	left = theScreen.drawXCRect.left;
	wid = theScreen.drawXCRect.right - left;

	HLock((Handle) theScreen.chars);
	HLock((Handle) theScreen.attrs);

    c = *theScreen.chars + top * theScreen.scrnCDims.h + left;
    a = *theScreen.attrs + top * theScreen.scrnCDims.h + left;

	area = theScreen.drawXLRect;
	area.bottom = area.top + theScreen.charPDims.v;

    for (j = 0; j < dep; j++) {

		if (RectInRgn(&area, vis)) {
			MoveTo(area.left, area.top + theScreen.info.ascent);
			UpdateScreenLine(&area, c, a, wid);
		}

		area.top = area.bottom;
		area.bottom += theScreen.charPDims.v;

		c += theScreen.scrnCDims.h;
		a += theScreen.scrnCDims.h;

    }

	HUnlock((Handle) theScreen.chars);
	HUnlock((Handle) theScreen.attrs);

	if (theScreen.cursorStatus) {
		curs.left = curs.right = theScreen.cursor.h * theScreen.charPDims.h;
		curs.right += theScreen.charPDims.h;
		curs.bottom = curs.top = (theScreen.cursor.v + 1) * theScreen.charPDims.v;
		if (theScreen.cursorLines <= theScreen.charPDims.v)
			curs.top -= theScreen.cursorLines;
		else
			curs.top -= theScreen.charPDims.v;
		if (theScreen.colorFlag)
			BackColor(colors[theScreen.cursorColor]);
		else {
			cursorLoc = theScreen.cursor.v * theScreen.scrnCDims.h
			  + theScreen.cursor.h;
			BackColor(
				colors[
					(AttrFore((*theScreen.attrs)[cursorLoc]) == attrColorBack) ?
						theScreen.colorStdBack : theScreen.colorStdFore]);
		}
		EraseRect(&curs);
	}

	SetOrigin(0, 0);

	TextFace(normal);

	ForeColor(blackColor);
	BackColor(whiteColor);

    MoveTo(0, 0);

	ClipRect(&theScreen.window->portRect);

	UpdtControl(theScreen.window, theScreen.window->visRgn);
	DrawGrowIcon(theScreen.window);

    EndUpdate(theScreen.window);

    return;
}

void IdleScreenMgr()

{
	long more;
	short mask;

	do {

		mask = everyEvent;
		if (theScreen.waitFlag) {
			mask -= mDownMask;
			mask -= keyDownMask;
			mask -= autoKeyMask;
		}

		if ( (!theScreen.backgrounding) && (theScreen.window == FrontWindow()) )
			CheckCursorStatus();

		if (theScreen.wneImplemented) {
			more = WaitNextEvent(mask, &theScreen.event, 0, NULL);
		}
		else {
			SystemTask();
			more = GetNextEvent(mask, &theScreen.event);
		}

		if (more)
			switch (theScreen.event.what) {

				case mouseDown:		DoMouseDown(&theScreen.event.where);
									break;

				case autoKey:
				case keyDown:		if (theScreen.event.modifiers & cmdKey) {
										if (theScreen.cmdKeyFlag) {
											if (theScreen.event.what != autoKey)
												DoMenuItem(MenuKey(theScreen.event.message & charCodeMask));
										}
										else
											DoKeyDown(true);
									}
									else
										DoKeyDown(false);
									break;

				case updateEvt:		DoUpdate((WindowPtr) theScreen.event.message);
									break;

				case activateEvt:	DoActivate((WindowPtr) theScreen.event.message,
											   theScreen.event.modifiers & activeFlag);
									break;

				case osEvent:		if ((theScreen.event.message >> 24)
							    == suspendResumeMessage) {
										if (theScreen.event.message & resumeMask) {
											theScreen.backgrounding = false;
											DoActivate((WindowPtr) FrontWindow(), true);
										}
										else {
											theScreen.backgrounding = true;
											DoActivate((WindowPtr) FrontWindow(), false);
										}
									}

			}

	} while (more);

	return;
}

void FlushScreenKeys()

{
	theScreen.keyFlush = theScreen.mouseFlush = TickCount();
	FlushQueue();
	return;
}

long CountScreenKeys()

{
	return(LenQueue());
}

int GetScreenKeys(keycode, modifiers, ascii, h, v)
char *keycode;
char *modifiers;
char *ascii;
short *h;
short *v;

{
	short flag;
	short th, tv;

	if (flag = PopQueue(keycode, modifiers, ascii, &th, &tv)) {
		if (h != NULL) *h = th;
		if (v != NULL) *v = tv;
	}

	return(flag);
}

void EnableScreenMouse(flag)
long flag;

{
	if ( (flag) && (!theScreen.mouseFlag) ) theScreen.mouseFlush = TickCount();
	theScreen.mouseFlag = flag;
	return;
}

void ClipScreenMouse(area)
Rect *area;

{
	theScreen.mouseLRect = theScreen.mouseCRect = *area;
	theScreen.mouseLRect.left *= theScreen.charPDims.h;
	theScreen.mouseLRect.top *= theScreen.charPDims.v;
	theScreen.mouseLRect.right *= theScreen.charPDims.h;
	theScreen.mouseLRect.bottom *= theScreen.charPDims.v;

	return;
}

void DefineScreenCursor(color, lines, blinkRate)
long color;
long lines;
long blinkRate;

{
	theScreen.cursorColor = color;
	theScreen.cursorLines = lines;
	theScreen.cursorBlink = blinkRate;

	InvalCursor(0);

	return;
}

void HideScreenCursor()

{
	theScreen.cursorLevel--;
	CheckCursorStatus();
	return;
}

void ShowScreenCursor()

{
	theScreen.cursorLevel++;
	CheckCursorStatus();
	return;
}

void SetScreenAboutProc(procPtr)
void (*procPtr)();

{
	theScreen.aboutProc = procPtr;
	return;
}

void SetScreenQuitProc(procPtr, flag)
void (*procPtr)();
long flag;

{
	theScreen.quitProc = procPtr;
	theScreen.quitReturns = flag;
	return;
}

long YesOrNo(text)
char *text;

{
	DialogPtr theDialog;
	short itemHit;
	short itsType;
	Handle itsHandle;
	Rect itsRect;
	Str255 ptext;
	long h, v;

	theDialog = GetNewDialog(yesOrNoDlgID, nil, (WindowPtr) -1);

	CenterScreenDLOG(yesOrNoDlgID, fixHalf, fixThird, &h, &v);
	MoveWindow((WindowPtr) theDialog, (short) h, (short) v, false);

	GetDItem(theDialog, ok, &itsType, &itsHandle, &itsRect);
	InsetRect(&itsRect, -4, -4);

	SetDItem(theDialog, yesOrNoDfltBorder, userItem,
		 (Handle) DrawDefaultBorder, &itsRect);

	if (text != NULL) {
		strncpy((char *)ptext, text, 255);
		ptext[255] = '\0';
		c2pstr((char *)ptext);
		GetDItem(theDialog, yesOrNoText, &itsType, &itsHandle, &itsRect);
		SetIText(itsHandle, ptext);
	}

	ShowWindow((WindowPtr) theDialog);

	do {
		ModalDialog(nil, &itemHit);
	} while ( (itemHit != ok) && (itemHit != cancel) );

	DisposDialog(theDialog);

	return(itemHit == ok);
}

void ShowScreen(visible)
long visible;

{
	if (visible)
		ShowWindow(theScreen.window);
	else
		HideWindow(theScreen.window);

	return;
}

void GetScreenBounds(bounds)
Rect *bounds;

{

#ifdef THINK_C

	*bounds = screenBits.bounds;
	bounds->top += GetMBarHeight();

#else			/* I think we can live without this. -- BS  */

	Point mouse;
	GDHandle gdh;

	if (!theScreen.env.hasColorQD) {
		*bounds = qd.screenBits.bounds;
		bounds->top += GetMBarHeight();
	}

	else {

		*bounds = (*GetMainDevice())->gdRect;

		GetMouse(&mouse);
		LocalToGlobal(&mouse);

		gdh = GetDeviceList();

		while (gdh != NULL) {

			if (PtInRect(mouse, &(*gdh)->gdRect)) {
				*bounds = (*gdh)->gdRect;
				if (gdh == GetMainDevice()) bounds->top += GetMBarHeight();
				gdh = NULL;
			}

			else
				gdh = GetNextDevice(gdh);

		}

	}
#endif

	return;
}


void CenterScreenDLOG(id, hRatio, vRatio, h, v)
long id;
Fixed hRatio, vRatio;
long *h, *v;

{
	long wid, hgt;
	DialogTHndl d;
	Rect bounds;

	d = (DialogTHndl) GetResource('DLOG', (short) id);

	if (d != NULL) {

		wid = (*d)->boundsRect.right - (*d)->boundsRect.left;
		hgt = (*d)->boundsRect.bottom - (*d)->boundsRect.top;

		GetScreenBounds(&bounds);

		wid = (bounds.right - bounds.left) - wid;
		hgt = (bounds.bottom - bounds.top) - hgt;

		*h = bounds.left + FixRound(hRatio * wid);
		*v = bounds.top + FixRound(vRatio * hgt);

	}

	return;
}

long DoScreenALRT(id, kind, hRatio, vRatio)
long id;
long kind;
Fixed hRatio, vRatio;

{
	long wid, hgt, h, v;
	long item;
	AlertTHndl a;
	Rect bounds;

	a = (AlertTHndl) GetResource('ALRT', (short) id);

	if (a != NULL) {

		wid = (*a)->boundsRect.right - (*a)->boundsRect.left;
		hgt = (*a)->boundsRect.bottom - (*a)->boundsRect.top;

		GetScreenBounds(&bounds);

		wid = (bounds.right - bounds.left) - wid;
		hgt = (bounds.bottom - bounds.top) - hgt;

		h = bounds.left + FixRound(hRatio * wid) - (*a)->boundsRect.left;
		v = bounds.top + FixRound(vRatio * hgt) - (*a)->boundsRect.top;

		OffsetRect(&(*a)->boundsRect, (short) h, (short) v);

		MoveHHi((Handle) a);
		HLock((Handle) a);

		switch (kind) {
			case akNormal:		item = Alert((short) id, NULL);			break;
			case akStop:		item = StopAlert((short) id, NULL);		break;
			case akNote:		item = NoteAlert((short) id, NULL);		break;
			case akCaution:		item = CautionAlert((short) id, NULL);	break;
		}

		HUnlock((Handle) a);

	}

	else

		item = -1;

	return(item);
}

void ConfigScreenMgr(force, theType, theID, ConfigProc)
long force;
ResType theType;
long theID;
long (*ConfigProc)(Handle theData);

{
	short saveResFile, homeResFile;
	short attrs;
	short itsID;
	ResType itsType;
	Str255 itsName;
	Handle theData;

	saveResFile = CurResFile();
	UseResFile(theScreen.infoResFile);

	theData = GetResource(theType, (short) theID);

	if (theData != NULL) {

		homeResFile = HomeResFile(theData);

		if ( (theScreen.reconfigFlag) ||
				(force) ||
				(homeResFile != theScreen.infoResFile) ) {

			if ((*ConfigProc)(theData)) {

				if (homeResFile != theScreen.infoResFile) {
					GetResInfo(theData, &itsID, &itsType, itsName);
					attrs = GetResAttrs(theData);
					attrs |= resChanged;
					DetachResource(theData);
					AddResource(theData, itsType, itsID, itsName);
					SetResAttrs(theData, attrs);
				}
				else
					ChangedResource(theData);

				WriteResource(theData);

			}

		}

	}

	UseResFile(saveResFile);

	return;
}

static pascal void AnimateCursor()

{
	short next;
	long oldA5;

	oldA5 = SetCurrentA5();

	next = (*theScreen.acur)->next + 1;
	if (next >= (*theScreen.acur)->frames) next = 0;
	SetCursor(*((*theScreen.acur)->table[next].cursHandle));
	(*theScreen.acur)->next = next;

	theScreen.vbl.vblCount = (short) theScreen.waitRate;

	(void) SetA5(oldA5);

	return;
}

void BeginScreenWait(rate)
long rate;

{
	if (!theScreen.waitFlag) {

		(*theScreen.acur)->next = 0;

		SetCursor(*((*theScreen.acur)->table[0].cursHandle));
		ShowCursor();

		theScreen.waitFlag = true;
		theScreen.waitRate = rate;

		theScreen.vbl.qType = vType;
#ifdef THINK_C
		theScreen.vbl.vblAddr = (int (*)())AnimateCursor;
#else
		theScreen.vbl.vblAddr = AnimateCursor;
#endif
		theScreen.vbl.vblCount = (short) theScreen.waitRate;
		theScreen.vbl.vblPhase = 0;

		(void) VInstall((QElemPtr) &theScreen.vbl);

	}

	return;
}

void EndScreenWait()

{
	if (theScreen.waitFlag) {

		(void) VRemove((QElemPtr) &theScreen.vbl);

		theScreen.waitFlag = false;

		InitCursor();

	}

	return;
}

Handle GetFileMHandle()

{
	return((Handle) GetMHandle(theScreen.cmdKeyFlag ? fileID1 : fileID2));
}

Handle GetAppMHandle()

{
	return((Handle) GetMHandle(theScreen.cmdKeyFlag ? appID1 : appID2));
}

long PushScreen()

{
	long errcode;
	char **chars, **attrs;
	SaveScreenHandle next;
	OSErr oops;

	next = (SaveScreenHandle) NewHandle(sizeof(SaveScreenRec));

	if (next != NULL) {

		chars = theScreen.chars;
		oops = HandToHand((Handle *) &chars);

		if (oops == noErr) {

			attrs = theScreen.attrs;
			oops = HandToHand((Handle *) &attrs);

			if (oops == noErr) {

				(*next)->link = theScreen.stack;
				(*next)->chars = chars;
				(*next)->attrs = attrs;
				(*next)->cursor = theScreen.cursor;

				theScreen.stack = next;

				errcode = scrnErrOk;

			}

			else {

				DisposHandle((Handle) chars);
				DisposHandle((Handle) next);

				errcode = scrnErrNoMem;

			}

		}

		else {

			DisposHandle((Handle) next);

			errcode = scrnErrNoMem;

		}

	}

	else {

		errcode = scrnErrNoMem;

	}

	return(errcode);
}

void PopScreen()

{
	if (theScreen.stack != NULL) {

		HLock((Handle) (*theScreen.stack)->chars);
		HLock((Handle) (*theScreen.stack)->attrs);

		XSetScreenImage(0,
			*(*theScreen.stack)->chars,
			*(*theScreen.stack)->attrs,
			theScreen.scrnCDims.h,
			&theScreen.scrnCRect,
			0, 0);

		XSetScreenCursor(0, (*theScreen.stack)->cursor.h,
				 (*theScreen.stack)->cursor.v);

		DisposeStackTop();

		UpdateScreen();

	}

	return;
}
