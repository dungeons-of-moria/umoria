/*	Copyright (C) Curtis McCauley, 1989.  All rights reserved.

	You may copy this subroutine package freely, modify it as you desire,
	and distribute it at will, as long as the copyright notice in the source
	material is not disturbed, excepting that no one may use this package or
	any part of it for commercial purposes of any kind without the express
	written consent of its author. */

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
	int colorStdFore;
	int colorStdBack;
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
	int wneImplemented;				/* do we have WaitNextEvent?			*/
	int backgrounding;				/* are we in the background?			*/
	int reconfigFlag;				/* true if user want to change options	*/
	int colorFlag;					/* true if multiple colors supported	*/
	int cmdKeyFlag;					/* true if cmd key equivalents ok		*/
	int mouseFlag;					/* true if mouse clicks are captured	*/
	int escMapFlag;					/* true if backquote maps to escape		*/
	int waitFlag;					/* true if in wait						*/
	int waitRate;					/* how fast to spin the cursor			*/
	int colorStdFore;				/* standard foreground color			*/
	int colorStdBack;				/* standard background color			*/
	int windowW;					/* its content width in pixels			*/
	int windowH;					/* its content height in pixels			*/
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
    char **chars;					/* handle to character buffer			*/
    char **attrs;					/* handle to attribute buffer			*/
	short **updLeft;				/* handle to left margin for updates	*/
	short **updRight;				/* handle to right margin for updates	*/
	int keyFlush;					/* tick count at last key flush			*/
	int mouseFlush;					/* tick count at last mouse flush		*/
	int qBegin;						/* index to first elem in queue			*/
	int qEnd;						/* index to next elem to be stored		*/
	Evt qBuffer[qSize];				/* queue of keystrokes					*/
	int cursorLevel;				/* greater than zero when cursor visible*/
	int cursorColor;				/* color of cursor lines				*/
	int cursorLines;				/* number of lines in cursor			*/
	int cursorBlink;				/* ticks between cursor blinks			*/
	int cursorStatus;				/* non-zero when cursor in on screen	*/
	int cursorChangeTick;			/* when to change cursor status			*/
	void (*aboutProc)(void);		/* what to call when about is selected  */
	void (*quitProc)(void);			/* what to call when quit is selected   */
	void (*fileMenuProc)(int item);	/* file menu handler					*/
	void (*appMenuProc)(int item);	/* app menu handler						*/
	int quitReturns;				/* true if quit proc will always return */
	AcurHandle acur;				/* table of cursors						*/
	VBLTask vbl;					/* for animating said cursors			*/
	SaveScreenHandle stack;			/* stack of saved screens				*/
} ScreenRec;

ScreenRec theScreen;

static int colors[8] = {
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

static int WNETest(envp)
SysEnvRec *envp;

{
	int hasWNE;

	if (envp->machineType < 0)
		hasWNE = false;

	else
		hasWNE =
			NGetTrapAddress(_WaitNextEvent, ToolTrap) != GetTrapAddress(_Unimplemented);

	return(hasWNE);
}

static int NoEscTest(envp)
SysEnvRec *envp;

{
	int hasNoEsc;

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
	int end;
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

static int LenQueue()

{
	return(ABS(theScreen.qEnd - theScreen.qBegin));
}

static int PopQueue(keycode, modifiers, ascii, h, v)
char *keycode;
char *modifiers;
char *ascii;
short *h;
short *v;

{
	int flag;
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

	if ( PtInRect(temp, &theScreen.mouseLRect) && PtInRect(temp, &theScreen.picLRect) ) {
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
	InitGraf(&qd.thePort);
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

static int GetFontSizes()

{
	int i, j;
	short fsize, sizes[fontSizeCount];
	char **fname;

	fname = GetString(fontNameID);

	if (fname == NULL)
		theScreen.txFont = 0;
	else {
		HLock(fname);
		GetFNum(*fname, &theScreen.txFont);
		HUnlock(fname);
	}

	for (i = 0, fsize = minFontSize; (i < fontSizeCount) && (fsize < maxFontSize); fsize++)
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
	int i, n, offset;
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

	h = GetString(aboutStrID);
	if (h != NULL) {
		MoveHHi(h);
		HLock(h);
		SetItem(GetMHandle(appleID), aboutItem, *h);
		HUnlock(h);
	}

	for (i = 0; n = (*theScreen.fontSizes)[i]; i++) {
		NumToString(n, ftitle);
		if (i == 0)
			(void) c2pstr(strcat(p2cstr(ftitle), " point"));
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
int *colorStdForePtr;
int *colorStdBackPtr;

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

	topLeft.h = qd.thePort->portRect.left;
	topLeft.v = qd.thePort->portRect.top;

	bottomRight.h = qd.thePort->portRect.right;
	bottomRight.v = qd.thePort->portRect.bottom;

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
	int repos;
	int wid, hgt;
	Rect test, sect;
	GDHandle gdh;

	test.left = bounds->left;
	test.top = bounds->top - titleMargin;
	test.right = bounds->right;
	test.bottom = bounds->top;

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

	return;
}

static void SetScreenParameters()

{
	int stdWid, stdHgt;
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
	if (theScreen.sizeLimitPRect.bottom < 65) theScreen.sizeLimitPRect.bottom = 65;

	theScreen.dragLimitGRect = qd.screenBits.bounds;
	theScreen.dragLimitGRect.top += GetMBarHeight();
	InsetRect(&theScreen.dragLimitGRect, -4, -4);

	stdState = theScreen.stdStateGRect;
	stdWid = stdState.right - stdState.left - theScreen.sizeLimitPRect.right + 1;
	stdHgt = stdState.bottom - stdState.top - theScreen.sizeLimitPRect.bottom + 1;

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
	int oldW, oldH;
	int newW, newH;

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
		if (newH != theScreen.origin.h) SetCtlValue(theScreen.hScrollHandle, newH);
		if (newV != theScreen.origin.v) SetCtlValue(theScreen.vScrollHandle, newV);
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

int InitScreenMgr(h, v, title, resFile, rfCreator, rfType, fileMenuProc, appMenuProc, colorFlag)
int h, v;
char *title;
char *resFile;
OSType rfCreator, rfType;
void (*fileMenuProc)(int item);
void (*appMenuProc)(int item);
int colorFlag;

{
    int bytes, i;
	Rect bounds;
	WStateData **zoomInfo;
	Str63 pstrTitle, pstrResFile;

	Prolog();

	SysEnvirons(curSysEnvVers, &theScreen.env);

	theScreen.wneImplemented = WNETest(&theScreen.env);

	theScreen.escMapFlag = NoEscTest(&theScreen.env);

	theScreen.backgrounding = false;

	theScreen.reconfigFlag = ((char *) KeyMapLM)[7] & 0x04;

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

	AdjustPortBounds(&bounds);

	LoadCursors();

	theScreen.waitFlag = false;

	theScreen.txNum = -1;

	theScreen.fileMenuProc = fileMenuProc;
	theScreen.appMenuProc = appMenuProc;

	theScreen.colorFlag = colorFlag;

	switch (theScreen.env.keyBoardType) {

		case envAExtendKbd:
		case envStandADBKbd:
			theScreen.cmdKeyFlag = true;
			break;

		default:
			theScreen.cmdKeyFlag = false;
			break;

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
int num;

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
	if (theScreen.origin.h > theScreen.scrnXLRect.right - theScreen.picPRect.right)
		theScreen.origin.h = theScreen.scrnXLRect.right - theScreen.picPRect.right;
	theScreen.origin.v *= theScreen.charPDims.v;
	theScreen.origin.v /= oldCharPDims.v;
	if (theScreen.origin.v < theScreen.scrnXLRect.top)
		theScreen.origin.v = theScreen.scrnXLRect.top;
	if (theScreen.origin.v > theScreen.scrnXLRect.bottom - theScreen.picPRect.bottom)
		theScreen.origin.v = theScreen.scrnXLRect.bottom - theScreen.picPRect.bottom;

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
	int h, v;

	theDialog = GetNewDialog(aboutDlgID, nil, (WindowPtr) -1);

	CenterScreenDLOG(aboutDlgID, fixThird, fixThird, &h, &v);
	MoveWindow((WindowPtr) theDialog, (short) h, (short) v, false);

	GetDItem(theDialog, ok, &itsType, &itsHandle, &itsRect);
	InsetRect(&itsRect, -4, -4);

	SetDItem(theDialog, aboutDfltBorder, userItem, (Handle) DrawDefaultBorder, &itsRect);

	ShowWindow((WindowPtr) theDialog);

	do {
		ModalDialog(nil, &itemHit);
	} while (itemHit != ok);

	DisposDialog(theDialog);

	return;
}

static void DoColorsDialog()

{
	DialogPtr theDialog;
	int saveForeColor, saveBackColor;
	short itemHit;
	short itsType;
	Handle itsHandle, fgHandle, bgHandle;
	Rect itsRect;
	GrafPtr savePort;
	int redrawFlag;
	int h, v;

	theDialog = GetNewDialog(colorsDlgID, nil, (WindowPtr) -1);

	CenterScreenDLOG(colorsDlgID, fixHalf, fixThird, &h, &v);
	MoveWindow((WindowPtr) theDialog, (short) h, (short) v, false);

	GetDItem(theDialog, ok, &itsType, &itsHandle, &itsRect);
	InsetRect(&itsRect, -4, -4);

	SetDItem(theDialog, colorsDfltBorder, userItem, (Handle) DrawDefaultBorder, &itsRect);

	GetDItem(theDialog, foregroundRect, &itsType, &itsHandle, &itsRect);
	SetDItem(theDialog, foregroundRect, itsType, (Handle) DrawGroupRect, &itsRect);
	GetDItem(theDialog, backgroundRect, &itsType, &itsHandle, &itsRect);
	SetDItem(theDialog, backgroundRect, itsType, (Handle) DrawGroupRect, &itsRect);

	GetDItem(theDialog, foreColors+theScreen.colorStdFore, &itsType, &fgHandle, &itsRect);
	SetCtlValue((ControlHandle) fgHandle, true);

	GetDItem(theDialog, backColors+theScreen.colorStdBack, &itsType, &bgHandle, &itsRect);
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
int item;

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
int item;

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
int item;

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
int item, offset;

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
int item;

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
int item;

{
	DoFontChange(GetMHandle(fontSizeID), item, 1);
	return;
}

static void DoMenuItem(select)
int select;

{
	int menuID, menuItem;

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
int oldWidth, oldHeight, newWidth, newHeight;

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
int oldWidth, oldHeight, newWidth, newHeight;

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
	int newSize;
	int oldWidth, oldHeight;
	int newWidth, newHeight;

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
int part;

{
	WStateData **zoomInfo;
	Rect newSize;
	int oldWidth, oldHeight;
	int newWidth, newHeight;

	if (whichWindow == theScreen.window) {
		oldWidth = theScreen.windowW;
		oldHeight = theScreen.windowH;
		zoomInfo = (WStateData **) ((WindowPeek) whichWindow)->dataHandle;
		newSize = (part == inZoomIn) ? (*zoomInfo)->userState : (*zoomInfo)->stdState;
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
int thePart;

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
int thePart;

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
