/* mac/scrnmgr/ScrnTest.r: resources for scrnmgr test driver

   Copyright (C) 1989-1991 Curtis McCauley, James E. Wilson

   You may copy this subroutine package freely, modify it as you desire,
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
