/* mac/scrnmgr/ScrnMgr.r: resources for scrnmgr code

   Copyright (c) 1989-1991 Curtis McCauley, James E. Wilson

   You may copy this subroutine package freely, modify it as you desire,
   and distribute it at will, as long as the copyright notice in the source
   material is not disturbed, excepting that no one may use this package or
   any part of it for commercial purposes of any kind without the express
   written consent of its author. */

#include "Types.r"

type 'INFO'	{
	rect;
	integer;
	longint white, black, red, green, blue, cyan, magenta, yellow;
	longint white, black, red, green, blue, cyan, magenta, yellow;
};

type 'acur' {
	integer = $$Countof(cursors);
	integer = 0;
	array cursors {
		integer;
		integer = 0;
	};
};

resource 'acur' (128, "Rotating Watch") {
	{ 256, 257, 258, 259, 260, 261, 262, 263 }
};

resource 'CURS' (256) {
	$"3F 00 3F 00 3F 00 3F 00 40 80 84 40 84 40 84 60"
	$"9C 60 80 40 80 40 40 80 3F 00 3F 00 3F 00 3F",
	$"3F 00 3F 00 3F 00 3F 00 7F 80 FF C0 FF C0 FF C0"
	$"FF C0 FF C0 FF C0 7F 80 3F 00 3F 00 3F 00 3F",
	{8, 8}
};

resource 'CURS' (257) {
	$"3F 00 3F 00 3F 00 3F 00 40 80 80 40 81 40 82 60"
	$"9C 60 80 40 80 40 40 80 3F 00 3F 00 3F 00 3F",
	$"3F 00 3F 00 3F 00 3F 00 7F 80 FF C0 FF C0 FF C0"
	$"FF C0 FF C0 FF C0 7F 80 3F 00 3F 00 3F 00 3F",
	{8, 8}
};

resource 'CURS' (258) {
	$"3F 00 3F 00 3F 00 3F 00 40 80 80 40 80 40 80 60"
	$"9F 60 80 40 80 40 40 80 3F 00 3F 00 3F 00 3F",
	$"3F 00 3F 00 3F 00 3F 00 7F 80 FF C0 FF C0 FF C0"
	$"FF C0 FF C0 FF C0 7F 80 3F 00 3F 00 3F 00 3F",
	{8, 8}
};

resource 'CURS' (259) {
	$"3F 00 3F 00 3F 00 3F 00 40 80 80 40 80 40 80 60"
	$"9C 60 82 40 80 40 40 80 3F 00 3F 00 3F 00 3F",
	$"3F 00 3F 00 3F 00 3F 00 7F 80 FF C0 FF C0 FF C0"
	$"FF C0 FF C0 FF C0 7F 80 3F 00 3F 00 3F 00 3F",
	{8, 8}
};

resource 'CURS' (260) {
	$"3F 00 3F 00 3F 00 3F 00 40 80 80 40 80 40 80 60"
	$"9C 60 84 40 84 40 40 80 3F 00 3F 00 3F 00 3F",
	$"3F 00 3F 00 3F 00 3F 00 7F 80 FF C0 FF C0 FF C0"
	$"FF C0 FF C0 FF C0 7F 80 3F 00 3F 00 3F 00 3F",
	{8, 8}
};

resource 'CURS' (261) {
	$"3F 00 3F 00 3F 00 3F 00 40 80 80 40 80 40 80 60"
	$"9C 60 88 40 90 40 40 80 3F 00 3F 00 3F 00 3F",
	$"3F 00 3F 00 3F 00 3F 00 7F 80 FF C0 FF C0 FF C0"
	$"FF C0 FF C0 FF C0 7F 80 3F 00 3F 00 3F 00 3F",
	{8, 8}
};

resource 'CURS' (262) {
	$"3F 00 3F 00 3F 00 3F 00 40 80 80 40 80 40 80 60"
	$"BC 60 80 40 80 40 40 80 3F 00 3F 00 3F 00 3F",
	$"3F 00 3F 00 3F 00 3F 00 7F 80 FF C0 FF C0 FF C0"
	$"FF C0 FF C0 FF C0 7F 80 3F 00 3F 00 3F 00 3F",
	{8, 8}
};

resource 'CURS' (263) {
	$"3F 00 3F 00 3F 00 3F 00 40 80 80 40 90 40 88 60"
	$"9C 60 80 40 80 40 40 80 3F 00 3F 00 3F 00 3F",
	$"3F 00 3F 00 3F 00 3F 00 7F 80 FF C0 FF C0 FF C0"
	$"FF C0 FF C0 FF C0 7F 80 3F 00 3F 00 3F 00 3F",
	{8, 8}
};

resource 'STR ' (128, "About Title", purgeable) {
	"About ScrnMgr\311"
};

resource 'STR ' (129, "Font Name", purgeable) {
	"Monaco"
};

resource 'DLOG' (128, "About", purgeable) {
	{44, 26, 162, 314},
	dBoxProc,
	invisible,
	noGoAway,
	0x0,
	128,
	""
};

resource 'DLOG' (129, "Colors", purgeable) {
	{56, 32, 228, 364},
	dBoxProc,
	invisible,
	noGoAway,
	0x0,
	129,
	""
};

resource 'DLOG' (130, "Yes or No", purgeable) {
	{40, 40, 142, 300},
	dBoxProc,
	invisible,
	noGoAway,
	0x0,
	130,
	""
};

resource 'DITL' (128, "About", purgeable) {
	{ /* array DITLarray: 4 elements */
		/* [1] */
		{73, 103, 97, 191},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{80, 158, 89, 167},
		UserItem {
			disabled
		},
		/* [3] */
		{16, 8, 32, 273},
		StaticText {
			enabled,
			"ScrnMgr: Version 1.0 22 July 1989"
		},
		/* [4] */
		{40, 8, 56, 120},
		StaticText {
			enabled,
			"Curtis McCauley"
		}
	}
};

resource 'DITL' (129, "Colors", purgeable) {
	{ /* array DITLarray: 23 elements */
		/* [1] */
		{40, 256, 60, 316},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{88, 256, 108, 316},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{32, 32, 48, 112},
		RadioButton {
			enabled,
			"White"
		},
		/* [4] */
		{48, 32, 64, 112},
		RadioButton {
			enabled,
			"Black"
		},
		/* [5] */
		{64, 32, 80, 112},
		RadioButton {
			enabled,
			"Red"
		},
		/* [6] */
		{80, 32, 96, 112},
		RadioButton {
			enabled,
			"Green"
		},
		/* [7] */
		{96, 32, 112, 112},
		RadioButton {
			enabled,
			"Blue"
		},
		/* [8] */
		{112, 32, 128, 112},
		RadioButton {
			enabled,
			"Cyan"
		},
		/* [9] */
		{128, 32, 144, 112},
		RadioButton {
			enabled,
			"Magenta"
		},
		/* [10] */
		{144, 32, 160, 112},
		RadioButton {
			enabled,
			"Yellow"
		},
		/* [11] */
		{32, 152, 48, 232},
		RadioButton {
			enabled,
			"White"
		},
		/* [12] */
		{48, 152, 64, 232},
		RadioButton {
			enabled,
			"Black"
		},
		/* [13] */
		{64, 152, 80, 232},
		RadioButton {
			enabled,
			"Red"
		},
		/* [14] */
		{80, 152, 96, 232},
		RadioButton {
			enabled,
			"Green"
		},
		/* [15] */
		{96, 152, 112, 232},
		RadioButton {
			enabled,
			"Blue"
		},
		/* [16] */
		{112, 152, 128, 232},
		RadioButton {
			enabled,
			"Cyan"
		},
		/* [17] */
		{128, 152, 144, 232},
		RadioButton {
			enabled,
			"Magenta"
		},
		/* [18] */
		{144, 152, 160, 232},
		RadioButton {
			enabled,
			"Yellow"
		},
		/* [19] */
		{16, 16, 168, 120},
		UserItem {
			disabled
		},
		/* [20] */
		{16, 136, 168, 240},
		UserItem {
			disabled
		},
		/* [21] */
		{8, 24, 24, 104},
		StaticText {
			disabled,
			"Foreground"
		},
		/* [22] */
		{8, 144, 24, 224},
		StaticText {
			disabled,
			"Background"
		},
		/* [23] */
		{48, 296, 56, 304},
		UserItem {
			disabled
		}
	}
};

resource 'DITL' (130, "Yes or No", purgeable) {
	{ /* array DITLarray: 5 elements */
		/* [1] */
		{16, 184, 36, 244},
		Button {
			enabled,
			"Yes"
		},
		/* [2] */
		{64, 184, 84, 244},
		Button {
			enabled,
			"No"
		},
		/* [3] */
		{19, 227, 33, 241},
		UserItem {
			disabled
		},
		/* [4] */
		{9, 48, 89, 168},
		StaticText {
			disabled,
			"Are you sure?"
		},
		/* [5] */
		{8, 8, 40, 40},
		Icon {
			disabled,
			0
		}
	}
};

resource 'MENU' (128, "Apple") {
	128,
	textMenuProc,
	0x7FFFFFFD,
	enabled,
	apple,
	{	/* array: 2 elements */
		/* [1] */
		"About ScrnMgr\311", noIcon, "", "", plain,
		/* [2] */
		"-", noIcon, "", "", plain
	}
};

resource 'MENU' (129, "File") {
	129,
	textMenuProc,
	0x7FFFFFFB,
	enabled,
	"File",
	{	/* array: 4 elements */
		/* [1] */
		"Open", noIcon, "O", "", plain,
		/* [2] */
		"Close", noIcon, "W", "", plain,
		/* [3] */
		"-", noIcon, "", "", plain,
		/* [4] */
		"Quit", noIcon, "Q", "", plain
	}
};

resource 'MENU' (130, "Edit") {
	130,
	textMenuProc,
	0x7FFFFFC0,
	enabled,
	"Edit",
	{	/* array: 6 elements */
		/* [1] */
		"Undo", noIcon, "Z", "", plain,
		/* [2] */
		"-", noIcon, "", "", plain,
		/* [3] */
		"Cut", noIcon, "X", "", plain,
		/* [4] */
		"Copy", noIcon, "C", "", plain,
		/* [5] */
		"Paste", noIcon, "V", "", plain,
		/* [6] */
		"Clear", noIcon, "", "", plain
	}
};

resource 'MENU' (229, "File (No Keys)") {
	129,
	textMenuProc,
	0x7FFFFFFB,
	enabled,
	"File",
	{	/* array: 4 elements */
		/* [1] */
		"Open", noIcon, "", "", plain,
		/* [2] */
		"Close", noIcon, "", "", plain,
		/* [3] */
		"-", noIcon, "", "", plain,
		/* [4] */
		"Quit", noIcon, "", "", plain
	}
};

resource 'MENU' (230, "Edit (No Keys)") {
	130,
	textMenuProc,
	0x7FFFFFC0,
	enabled,
	"Edit",
	{	/* array: 6 elements */
		/* [1] */
		"Undo", noIcon, "", "", plain,
		/* [2] */
		"-", noIcon, "", "", plain,
		/* [3] */
		"Cut", noIcon, "", "", plain,
		/* [4] */
		"Copy", noIcon, "", "", plain,
		/* [5] */
		"Paste", noIcon, "", "", plain,
		/* [6] */
		"Clear", noIcon, "", "", plain
	}
};

resource 'MENU' (131, "Screen") {
	131,
	textMenuProc,
	0x7FFFFFFD,
	enabled,
	"Screen",
	{	/* array: 2 elements */
		/* [1] */
		"Colors\311", noIcon, "", "", plain,
		/* [2] */
		"-", noIcon, "", "", plain,
	}
};

resource 'MENU' (132, "FontSize") {
	132,
	textMenuProc,
	allEnabled,
	enabled,
	"FontSize",
	{	/* array: 0 elements */
	}
};

resource 'MBAR' (128, "Main Menu") {
	{	/* array MenuArray: 3 elements */
		/* [1] */
		128,
		/* [2] */
		129,
		/* [3] */
		130
	}
};

resource 'MBAR' (228, "Main Menu (No Keys)") {
	{	/* array MenuArray: 3 elements */
		/* [1] */
		128,
		/* [2] */
		229,
		/* [3] */
		230
	}
};

resource 'INFO' (1, "Screen Info") {
	{ 50, 10, 250, 310 },
	9,
	black,
	white
};
