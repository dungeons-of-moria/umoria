/* mac/scrnmgr/ScrnTest.r: resources for scrnmgr test driver

   Copyright (C) 1989-1991 Curtis McCauley, James E. Wilson

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

#include "Types.r"

include "ScrnMgr.rsrc";

resource 'STR ' (128, "About Title", purgeable) {
	"About ScrnTest\311"
};

resource 'DITL' (1024, "Dummy Alert", purgeable) {
	{	/* array DITLarray: 2 elements */
		/* [1] */
		{88, 224, 108, 284},
		Button {
			enabled,
			"Ok"
		},
		/* [2] */
		{8, 64, 72, 288},
		StaticText {
			disabled,
			"This is a dummy alert."
		}
	}
};

resource 'ALRT' (1024, "Dummy Alert", purgeable) {
	{40, 42, 158, 352},
	1024,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, sound1,
		/* [2] */
		OK, visible, sound1,
		/* [3] */
		OK, visible, sound1,
		/* [4] */
		OK, visible, sound1
	}
};

resource 'MENU' (133, "ScrnTest") {
	133,
	textMenuProc,
	allEnabled,
	enabled,
	"ScrnTest",
	{	/* array: 2 elements */
		/* [1] */
		"X Out", noIcon, "", "", plain,
		/* [2] */
		"Dummy Alert", noIcon, "A", "", plain
	}
};

resource 'MENU' (233, "ScrnTest (No Keys)") {
	233,
	textMenuProc,
	allEnabled,
	enabled,
	"ScrnTest",
	{	/* array: 2 elements */
		/* [1] */
		"X Out", noIcon, "", "", plain,
		/* [2] */
		"Dummy Alert", noIcon, "", "", plain
	}
};

resource 'SIZE' (-1) {
	dontSaveScreen,
	acceptSuspendResumeEvents,
	enableOptionSwitch,
	canBackground,
	multiFinderAware,
	backgroundAndForeground,
	dontGetFrontClicks,
	ignoreChildDiedEvents,
	not32BitCompatible,
	reserved,
	reserved,
	reserved,
	reserved,
	reserved,
	reserved,
	reserved,
	128 * 1024,
	128 * 1024
};
