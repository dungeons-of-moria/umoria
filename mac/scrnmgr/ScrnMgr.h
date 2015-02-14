/* mac/scrnmgr/ScrnMgr.h: definitions for scrnmgr code

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

#ifndef __SCRNMGR__

#define __SCRNMGR__

#ifdef THINK_C
#include <MacTypes.h>
#else
#include <Types.h>
#endif

#ifdef THINK_C
#define nil 0				/* Think C doesn't have nil. -- BS  */
#endif

#define MakeAttr(f, b)		(char) ((f<<3) | b)

#define AttrFore(a)			(char) ((a>>3) & 0x7)
#define AttrBack(a)			(char) (a & 0x7)

#define attrColorWhite		0
#define attrColorBlack		1
#define attrColorRed		2
#define attrColorGreen		3
#define attrColorBlue		4
#define attrColorCyan		5
#define attrColorMagenta	6
#define attrColorYellow		7

#define attrColorBack		0
#define attrColorFore		1

#define attrNormal	    	MakeAttr(attrColorFore, attrColorBack)
#define attrReversed		MakeAttr(attrColorBack, attrColorFore)

#define attrUnderlined		0x80
#define attrItalicized		0x40

#define twoColor			0
#define multiColor			1

#define maskAttrFlags		0xC0
#define maskAttrColors		0x3F

#define maskModCommand		0x01
#define maskModShift		0x02
#define maskModCapsLock		0x04
#define maskModOption		0x08
#define maskModControl		0x10
#define maskModMouse		0x80

#define fixHalf				0x8000
#define fixThird			0x5555
#define fixQuarter			0x4000
#define fixFifth			0x3333
#define fixSixth			0x2AAA
#define fixSeventh			0x2492
#define fixEighth			0x2000
#define fixNinth			0x1C71
#define fixTenth			0x1999

#define akNormal			0
#define akStop				1
#define akNote				2
#define akCaution			3

#define closeBoxItem		0

#define scrnErrOk			0
#define scrnErrNoMem		-1

long InitScreenMgr(long h, long v, char *title,
	char *resFile, OSType rfCreator, OSType rfType,
	void (*fileMenuProc)(long item),
	void (*appMenuProc)(long item),
	long multiColorFlag);

void IdleScreenMgr(void);

void ShowScreen(long visible);

void CloseScreenMgr(void);

void SetScreenQuitProc(void (*quitProc)(void), long willReturnFlag);

void SetScreenAboutProc(void (*aboutProc)(void));

void XSetScreenChar(int d, char c, long h, long v);
void XSetScreenBuffer(int d, char *buffer, long rowBytes, Rect *area,
		      long h, long v);
void XSetScreenString(int d, char *str, long h, long v);

void XSetScreenCharAttr(int d, char c, char a, long h, long v);
void XSetScreenBufferAttr(int d, char *buffer, char a, long rowBytes,
			  Rect *area, long h, long v);
void XSetScreenStringAttr(int d, char *str, char a, long h, long v);

void XSetScreenImage(int d, char *c, char *a, long rowBytes, Rect *area,
		     long h, long v);

void XWriteScreenChar(int d, char c);
void XWriteScreenBuffer(int d, char *buffer, long rowBytes, Rect *area);
void XWriteScreenString(int d, char *str);

void XWriteScreenCharAttr(int d, char c, char a);
void XWriteScreenBufferAttr(int d, char *buffer, char a, long rowBytes,
			    Rect *area);
void XWriteScreenStringAttr(int d, char *str, char a);

void XWriteScreenImage(int d, char *c, char *a, long rowBytes, Rect *area);

void XFillScreen(int d, char c, char a, Rect *area);
void XEraseScreen(int d, Rect *area);

void XScrollScreen(int d, long dh, long dv, Rect *area, char a);

void XMoveScreenCursor(int d, long h, long v);
void XSetScreenCursor(int d, long h, long v);

#define SetScreenChar(c, h, v)						XSetScreenChar(0, c, h, v)
#define SetScreenBuffer(b, rb, area, h, v)	\
		XSetScreenBuffer(0, b, rb, area, h, v)
#define SetScreenString(str, h, v)					XSetScreenString(0, str, h, v)

#define SetScreenCharAttr(c, a, h, v)				XSetScreenCharAttr(0, c, a, h, v)
#define SetScreenBufferAttr(b, a, rb, area, h, v)	\
		XSetScreenBufferAttr(0, b, a, rb, area, h, v)
#define SetScreenStringAttr(str, a, h, v)		\
		XSetScreenStringAttr(0, str, a, h, v)

#define SetScreenImage(c, a, rb, area, h, v)		\
		XSetScreenImage(0, c, a, rb, area, h, v)

#define WriteScreenChar(c)							XWriteScreenChar(0, c)
#define WriteScreenBuffer(b, rb, area)				XWriteScreenBuffer(0, b, rb, area)
#define WriteScreenString(str)						XWriteScreenString(0, str)

#define WriteScreenCharAttr(c, a)					XWriteScreenCharAttr(0, c, a)
#define WriteScreenBufferAttr(b, a, rb, area)		\
		XWriteScreenBufferAttr(0, b, a, rb, area)
#define WriteScreenStringAttr(str, a)				XWriteScreenStringAttr(0, str, a)

#define WriteScreenImage(c, a, rb, area)		\
		XWriteScreenImage(0, c, a, rb, area)

#define FillScreen(c, a, area)						XFillScreen(0, c, a, area)
#define EraseScreen(area)							XEraseScreen(0, area)

#define ScrollScreen(dh, dv, area, a)				XScrollScreen(0, dh, dv, area, a)

#define MoveScreenCursor(h, v)						XMoveScreenCursor(0, h, v)
#define SetScreenCursor(h, v)						XSetScreenCursor(0, h, v)

#define DSetScreenChar(c, h, v)						XSetScreenChar(1, c, h, v)
#define DSetScreenBuffer(b, rb, area, h, v)		\
		XSetScreenBuffer(1, b, rb, area, h, v)
#define DSetScreenString(str, h, v)					XSetScreenString(1, str, h, v)

#define DSetScreenCharAttr(c, a, h, v)				XSetScreenCharAttr(1, c, a, h, v)
#define DSetScreenBufferAttr(b, a, rb, area, h, v)	\
		XSetScreenBufferAttr(1, b, a, rb, area, h, v)
#define DSetScreenStringAttr(str, a, h, v)		\
		XSetScreenStringAttr(1, str, a, h, v)

#define DSetScreenImage(c, a, rb, area, h, v)		\
		XSetScreenImage(1, c, a, rb, area, h, v)

#define DWriteScreenChar(c)							XWriteScreenChar(1, c)
#define DWriteScreenBuffer(b, rb, area)				XWriteScreenBuffer(1, b, rb, area)
#define DWriteScreenString(str)						XWriteScreenString(1, str)

#define DWriteScreenCharAttr(c, a)					XWriteScreenCharAttr(1, c, a)
#define DWriteScreenBufferAttr(b, a, rb, area)		\
		XWriteScreenBufferAttr(1, b, a, rb, area)
#define DWriteScreenStringAttr(str, a)				XWriteScreenStringAttr(1, str, a)

#define DWriteScreenImage(c, a, rb, area)		\
		XWriteScreenImage(1, c, a, rb, area)

#define DFillScreen(c, a, area)						XFillScreen(1, c, a, area)
#define DEraseScreen(area)							XEraseScreen(1, area)

#define DScrollScreen(dh, dv, area, a)				XScrollScreen(1, dh, dv, area, a)

#define DMoveScreenCursor(h, v)						XMoveScreenCursor(1, h, v)
#define DSetScreenCursor(h, v)						XSetScreenCursor(1, h, v)

void DefineScreenCursor(long color, long lines, long blinkRate);
void HideScreenCursor(void);
void ShowScreenCursor(void);

void UpdateScreen(void);

void FlushScreenKeys(void);
long CountScreenKeys(void);
int GetScreenKeys(char *keyCode, char *modifiers, char *ascii, short *h,
		  short *v);

void EnableScreenMouse(long flag);
void ClipScreenMouse(Rect *clip);

void GetScreenCharAttr(char *c, char *a, long h, long v);
void GetScreenImage(char *c, char *a, long rowBytes, Rect *area, long h,
		    long v);

void GetScreenCursor(long *h, long *v);

long YesOrNo(char *question);
long DoScreenALRT(long id, long kind, Fixed hRatio, Fixed vRatio);

void GetScreenBounds(Rect *bounds);

void CenterScreenDLOG(long id, Fixed hRatio, Fixed vRatio, long *h, long *v);

pascal void DrawDefaultBorder();
pascal void DrawGroupRect();

void ConfigScreenMgr(long force, ResType theType, long theID,
		     long (*ConfigProc)(Handle theData));

void BeginScreenWait(long rate);
void EndScreenWait(void);

Handle GetFileMHandle(void);
Handle GetAppMHandle(void);

long PushScreen(void);
void PopScreen(void);

#endif
