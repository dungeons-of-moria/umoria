/* mac/scrnmgr/ScrnTest.c: test driver for scrnmgr code

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
   along with Umoria.  If not, see <http://www.gnu.org/licenses/>.*/

/* This file was also released as part of an independent program, and
   may be redistributed and/or modified under the authors' original terms,
   which are given below. */

/* You may copy this subroutine package freely, modify it as you desire,
   and distribute it at will, as long as the copyright notice in the source
   material is not disturbed, excepting that no one may use this package or
   any part of it for commercial purposes of any kind without the express
   written consent of its author. */

/* Mac interface files required only for the call to TickCount() */

#ifndef THINK_C
#include <Types.h>
#include <Quickdraw.h>
#include <Events.h>
#else
#define NULL 0
#endif

#include "ScrnMgr.h"

static long done;

/*	This routine handles the items in the application menu.  It gets called
	by the screen manager whenever the user selects from that menu with the
	appropriate item number. */

/* 	Spins the watch cursor for n seconds, rotating the hands 4 times a
	second. */

static void Wait(n)
int n;

{
	long tick;

	tick = TickCount() + n * 60L;

	BeginScreenWait(15);
	while (TickCount() < tick)
	  SystemTask ();
	EndScreenWait();

	return;
}

static void DoAppMenu(theItem)
long theItem;

{
	long i;
#ifndef USE_PUSH
	char save_chars[80*25], save_attrs[80*25];
	long save_cursor_h, save_cursor_v;
	Rect screen;
#endif

	switch (theItem) {

		case 1:
			/* X out every character cell on the screen, with immediate updates. */
#ifdef USE_PUSH
			if (PushScreen() == scrnErrOk) {
				for (i = 0; i < 25*79; i++) {
					SetScreenChar('x', i % 79, i % 25);
					UpdateScreen();
				}
				Wait(10);
				PopScreen();
			}
			break;
#else
			screen.left = screen.top = 0;
			screen.right = 80;
			screen.bottom = 25;
			GetScreenImage(save_chars, save_attrs, 80, &screen, 0, 0);
			GetScreenCursor(&save_cursor_h, &save_cursor_v);
			for (i = 0; i < 25*79; i++) {
				SetScreenChar('x', i % 79, i % 25);
				UpdateScreen();
			}
			Wait(10);
			SetScreenImage(save_chars, save_attrs, 80, &screen, 0, 0);
			SetScreenCursor(save_cursor_h, save_cursor_v);
			break;
#endif

		case 2:
			/* Test the alert package. */
			DoScreenALRT(1024, akNote, fixHalf, fixHalf);
			break;

	}

	return;
}

/*	This routine gets called when the user selects quit from the file
	menu.  It was installed with a call to SetScreenQuitProc(). */

static void Quit()

{
	if (YesOrNo("Are you certain that you want to quit the test?")) {
		done = true;
	}

	return;
}

/*	All this routine does is to put some characters in the screen buffer. */

static void RunSomeTests()

{
	long i;
	Rect area;
	char msg[80];

	/* Fill the whole screen with blanks. */
	area.left = 0;
	area.top = 0;
	area.right = 80;
	area.bottom = 25;
	FillScreen(' ', MakeAttr(attrColorBlack, attrColorWhite), &area);

	/* Put up a humorous message. */
	SetScreenCursor(0, 0);
	WriteScreenString("Hello, world --");
	SetScreenCursor(10, 2);
	WriteScreenStringAttr("This is a test",
			      MakeAttr(attrColorRed,
				       attrColorWhite) | attrUnderlined);
	WriteScreenString(" of the Screen Manager.");
	SetScreenCursor(10, 3);
	WriteScreenString("This is only a ");
	WriteScreenStringAttr("test",
			      MakeAttr(attrColorWhite,
				       attrColorBlack) | attrItalicized);
	WriteScreenString(".  If this had been");
	SetScreenCursor(10, 4);
	WriteScreenString("an actual run, the program would bomb.");

	/* Show all the screen colors on a black background. */
	SetScreenCursor(0, 10);
	for (i = 0; i < 8; i++)
		WriteScreenCharAttr('a'+i, MakeAttr(i, attrColorBlack));

	/* Fill a subsection of the screen with white asterisks on a red
	   background. */
	area.left = 11;
	area.top = 11;
	area.right = 16;
	area.bottom = 16;
	FillScreen('*', MakeAttr(attrColorWhite, attrColorRed), &area);

	/* Show the pixel bounds of the entire display screen, minus the
	   menu bar. */
	GetScreenBounds(&area);
	sprintf(msg, "Bounds: %d %d %d %d", area.top, area.left,
		area.bottom, area.right);
	SetScreenString(msg, 0, 18);

	return;
}

/*	The big picture. */

int main()

{
	char keycode, modifiers, ascii;
	int h, v;
	long i;
	Rect scrollRect1, scrollRect2;
	char coords[6], hex[6];

	/* This flag gets set in the quit proc. */
	done = false;

	/* Put the screen up on the monitor. */
	InitScreenMgr(80, 25, "This is a Test",
		"ScrnMgrConfig", '????', '????',
		NULL, DoAppMenu,
		multiColor);

	/* Point to the quit proc.  Indicate that the quit proc always returns. */
	SetScreenQuitProc(Quit, true);

	/* Configure the screen cursor.  It does not flash. */
	DefineScreenCursor(attrColorBlack, 2, 0);
	ShowScreenCursor();

	/* Put some characters in the buffer. */
	RunSomeTests();

	/* Layout some rects for keyboard input. */
	scrollRect1.left = 60;
	scrollRect1.top = 0;
	scrollRect1.right = 70;
	scrollRect1.bottom = 1;

	scrollRect2.left = 60;
	scrollRect2.top = 0;
	scrollRect2.right = 70;
	scrollRect2.bottom = 10;

	/* Detect mouse clicks in the specified area. */
	ClipScreenMouse(&scrollRect2);
	EnableScreenMouse(true);

	i = 0;

	while (!done) {

		/* Very important.  Allows screen manager to update the
		   screen if any characters have been stuffed in the buffer,
		   to pull any keystrokes off the event queue, etc. */
		IdleScreenMgr();

		/* Look for keyboard or mouse input. */
		if (GetScreenKeys(&keycode, &modifiers, &ascii, &h, &v)) {

			/* Is is not a mouse click?  Then it is a keystroke. */
			if (!(modifiers & maskModMouse)) {
				/* Scroll top line one char to the left.
				   Filling in with white on green. */
				ScrollScreen(-1, 0, &scrollRect1,
					     MakeAttr(attrColorWhite,
						      attrColorGreen));
				/* Put the character on the screen to the top right. */
				SetScreenChar(ascii, 69, 0);
				/* If top line has been filled, scroll lines down. */
				if (!(++i % 10)) {
					ScrollScreen(0, 1, &scrollRect2,
						     MakeAttr(attrColorWhite,
							      attrColorGreen));
					UpdateScreen();
				}
				/* Show the keycodes entered. */
				sprintf(hex, "%2.2X %2.2X",
					(int)(unsigned char) ascii,
					(int)(unsigned char) keycode);
				SetScreenString(hex, 40, 24);
				/* If the char 'D' has been typed, go into a wait.  Notice that the D does
				not appear on the screen until the wait has ended, at the IdleScreenMgr
				call. */
				if (ascii == 'D') Wait(30);
			}

			/* Otherwise, it is a mouse click.  Show the coordinates. */
			else {
				sprintf(coords, "%2.2d %2.2d", h, v);
				SetScreenString(coords, 0, 24);
			}

		}
	}

	/* Clean up. */
	CloseScreenMgr();

	return(0);
}
