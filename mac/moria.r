/* mac/moria.r: MacMoria resources, for Mac functionality

   Copyright (c) 1989-94 Curtis McCauley, James E. Wilson

   This file is part of Umoria.

   Umoria is free software; you can redistribute it and/or modify 
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Umoria is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of 
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License 
   along with Umoria.  If not, see <http://www.gnu.org/licenses/>. */

#include "Types.r"

include "ScrnMgr.rsrc";

type 'MRIA' as 'STR ';

type 'CNFG' (256 : 257) {
	longint;
};

type 'TEXT' {
	string;
};

resource 'MRIA' (0) {
	"Moria 5.4 for the Macintosh"
};

resource 'BNDL' (128, purgeable) {
	'MRIA',
	0,
	{ /* array TypeArray: 2 elements */
		/* [1] */
		'ICN#',
		{ /* array IDArray: 3 elements */
			/* [1] Application */
			0, 128,
			/* [2] Save file */
			1, 129,
			/* [3] Aux files */
			2, 130
		},
		/* [2] */
		'FREF',
		{ /* array IDArray: 5 elements */
			/* [1] Appilication */
			0, 128,
			/* [2] Save file */
			1, 129,
			/* [3] Info files */
			2, 130,
			/* [4] Score file */
			3, 131,
			/* [5] Config file */
			4, 132
		}
	}
};

resource 'FREF' (128, "Application", purgeable) {
	'APPL',
	0,
	""
};

resource 'FREF' (129, "Save Files", purgeable) {
	'SAVE',
	1,
	""
};

resource 'FREF' (130, "Info Files", purgeable) {
	'TEXT',
	2,
	""
};

resource 'FREF' (131, "Score File", purgeable) {
	'SCOR',
	2,
	""
};

resource 'FREF' (132, "Config File", purgeable) {
	'CNFG',
	2,
	""
};

resource 'ICON' (128, "Application", purgeable) {
	$"00 01 00 00 00 02 80 00 00 04 40 00 00 08 20 00"
	$"00 10 10 00 00 22 08 00 00 45 04 00 00 88 82 00"
	$"01 05 15 00 02 02 A8 80 04 40 40 40 08 A8 80 20"
	$"11 15 00 10 24 8A 00 28 40 51 3F 44 91 20 C0 8A"
	$"40 00 80 51 24 41 30 22 10 01 C8 14 09 1E 7F 8F"
	$"04 02 30 07 02 41 00 07 01 00 80 07 00 80 60 07"
	$"00 40 1F E7 00 20 02 1F 00 10 04 07 00 08 08 00"
	$"00 04 10 00 00 02 20 00 00 01 40 00 00 00 80"
};

resource 'ICON' (256, "Command Set", purgeable) {
	$"00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00"
	$"00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00"
	$"00 00 00 00 7F FF FF FE 80 00 00 01 9A AA AA A9"
	$"80 00 00 01 95 55 55 59 80 00 00 01 9A AA AA A9"
	$"80 00 00 01 85 7F FF 51 80 00 00 01 80 00 00 01"
	$"7F FF FF FE"
};

resource 'ICON' (257, "TEXT Editor", purgeable) {
	$"0F FF FE 00 08 00 03 00 08 00 02 80 08 00 02 40"
	$"08 00 02 20 08 00 02 10 08 00 03 F8 08 00 00 08"
	$"08 00 00 08 08 07 C0 08 08 0F E0 08 08 18 30 08"
	$"08 30 18 08 08 20 18 08 08 00 18 08 08 00 30 08"
	$"08 00 60 08 08 00 C0 08 08 01 80 08 08 01 80 08"
	$"08 01 80 08 08 01 80 08 08 00 00 08 08 01 80 08"
	$"08 03 C0 08 08 01 80 08 08 00 00 08 08 00 00 08"
	$"08 00 00 08 08 00 00 08 08 00 00 08 0F FF FF F8"
};

resource 'ICN#' (128, "Application", purgeable) {
	{ /* array: 2 elements */
		/* [1] */
		$"00 01 00 00 00 02 80 00 00 04 40 00 00 08 20 00"
		$"00 10 10 00 00 22 08 00 00 45 04 00 00 88 82 00"
		$"01 05 15 00 02 02 A8 80 04 40 40 40 08 A8 80 20"
		$"11 15 00 10 24 8A 00 28 40 51 3F 44 91 20 C0 8A"
		$"40 00 80 51 24 41 30 22 10 01 C8 14 09 1E 7F 8F"
		$"04 02 30 07 02 41 00 07 01 00 80 07 00 80 60 07"
		$"00 40 1F E7 00 20 02 1F 00 10 04 07 00 08 08 00"
		$"00 04 10 00 00 02 20 00 00 01 40 00 00 00 80",
		/* [2] */
		$"00 01 00 00 00 03 80 00 00 07 C0 00 00 0F E0 00"
		$"00 1F F0 00 00 3F F8 00 00 7F FC 00 00 FF FE 00"
		$"01 FF FF 00 03 FF FF 80 07 FF FF C0 0F FF FF E0"
		$"1F FF FF F0 3F FF FF F8 7F FF FF FC FF FF FF FE"
		$"7F FF FF FF 3F FF FF FE 1F FF FF FC 0F FF FF FF"
		$"07 FF FF FF 03 FF FF FF 01 FF FF FF 00 FF FF FF"
		$"00 7F FF FF 00 3F FE 1F 00 1F FC 07 00 0F F8 00"
		$"00 07 F0 00 00 03 E0 00 00 01 C0 00 00 00 80"
	}
};

resource 'ICN#' (129, "Save Files", purgeable) {
	{ /* array: 2 elements */
		/* [1] */
		$"0F FF FE 00 08 00 03 00 08 00 02 80 08 00 02 40"
		$"08 00 02 20 08 00 02 10 08 00 03 F8 08 00 00 08"
		$"08 00 00 08 08 7F FE 08 08 FF FF 08 09 80 01 88"
		$"09 9F F9 88 09 B0 09 88 09 B1 29 88 09 92 A9 88"
		$"09 91 29 88 09 B0 49 88 09 B0 09 88 09 9F F9 88"
		$"09 80 01 88 09 FF FF 88 09 FF FF 88 09 80 01 88"
		$"08 00 00 08 09 12 64 88 09 B5 55 48 09 55 65 C8"
		$"09 15 55 48 09 12 55 48 08 00 00 08 0F FF FF F8",
		/* [2] */
		$"0F FF FE 00 0F FF FF 00 0F FF FF 80 0F FF FF C0"
		$"0F FF FF E0 0F FF FF F0 0F FF FF F8 0F FF FF F8"
		$"0F FF FF F8 0F FF FF F8 0F FF FF F8 0F FF FF F8"
		$"0F FF FF F8 0F FF FF F8 0F FF FF F8 0F FF FF F8"
		$"0F FF FF F8 0F FF FF F8 0F FF FF F8 0F FF FF F8"
		$"0F FF FF F8 0F FF FF F8 0F FF FF F8 0F FF FF F8"
		$"0F FF FF F8 0F FF FF F8 0F FF FF F8 0F FF FF F8"
		$"0F FF FF F8 0F FF FF F8 0F FF FF F8 0F FF FF F8"
	}
};

resource 'ICN#' (130, "Aux Files", purgeable) {
	{ /* array: 2 elements */
		/* [1] */
		$"0F FF FE 00 08 00 03 00 08 00 02 80 09 FB AE 40"
		$"08 00 02 20 08 00 02 10 09 F7 7B F8 08 00 00 08"
		$"08 00 00 08 09 BB ED C8 08 00 00 08 08 00 00 08"
		$"09 F7 B7 C8 08 00 00 08 08 00 00 08 09 B5 FD C8"
		$"08 00 00 08 08 00 00 08 09 77 B6 C8 08 00 00 08"
		$"08 00 00 08 09 EB B7 48 08 00 00 08 08 00 00 08"
		$"08 00 00 08 09 12 64 88 09 B5 55 48 09 55 65 C8"
		$"09 15 55 48 09 12 55 48 08 00 00 08 0F FF FF F8",
		/* [2] */
		$"0F FF FE 00 0F FF FF 00 0F FF FF 80 0F FF FF C0"
		$"0F FF FF E0 0F FF FF F0 0F FF FF F8 0F FF FF F8"
		$"0F FF FF F8 0F FF FF F8 0F FF FF F8 0F FF FF F8"
		$"0F FF FF F8 0F FF FF F8 0F FF FF F8 0F FF FF F8"
		$"0F FF FF F8 0F FF FF F8 0F FF FF F8 0F FF FF F8"
		$"0F FF FF F8 0F FF FF F8 0F FF FF F8 0F FF FF F8"
		$"0F FF FF F8 0F FF FF F8 0F FF FF F8 0F FF FF F8"
		$"0F FF FF F8 0F FF FF F8 0F FF FF F8 0F FF FF F8"
	}
};

resource 'DLOG' (128, "About", purgeable) {
	{46, 26, 304, 274},
	dBoxProc,
	invisible,
	noGoAway,
	0x0,
	128,
	""
};

resource 'DITL' (128, "About", purgeable) {
	{	/* array DITLarray: 4 elements */
		/* [1] */
		{224, 192, 247, 235},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{232, 224, 241, 233},
		UserItem {
			disabled
		},
		/* [3] */
		{16, 8, 248, 184},
		StaticText {
			disabled,
			"Moria for the Macintosh\n"
			"Version 5.4\n"
			"Programmers:\n"
			"Robert Alan Koeneke\n"
			"Jimmey Wayne Todd\n\n"
			"UNIX Port:\n"
			"James E. Wilson\n\n"
			"Macintosh Port:\n"
			"Curtis W. McCauley"
		},
		/* [4] */
		{16, 195, 48, 227},
		Icon {
			disabled,
			128
		}
	}
};

resource 'STR ' (128, "About Title", purgeable) {
	"About Moria\311"
};

resource 'CNFG' (256, "Command Set", purgeable) {
	'-o\0x00\0x00'
};

resource 'DLOG' (256, "Command Set", purgeable) {
	{42, 34, 218, 404},
	dBoxProc,
	invisible,
	noGoAway,
	0x0,
	256,
	""
};

resource 'DITL' (256, "Command Set", purgeable) {
	{	/* array DITLarray: 9 elements */
		/* [1] */
		{24, 296, 48, 352},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{64, 296, 88, 352},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{40, 24, 56, 200},
		RadioButton {
			enabled,
			"Rogue-Like Commands"
		},
		/* [4] */
		{64, 24, 80, 200},
		RadioButton {
			enabled,
			"Original Commands"
		},
		/* [5] */
		{112, 72, 168, 352},
		StaticText {
			disabled,
			"Choose the desired command set.  The change "
			"will take effect when you start the next game."
		},
		/* [6] */
		{16, 16, 96, 232},
		UserItem {
			disabled
		},
		/* [7] */
		{8, 24, 24, 104},
		StaticText {
			enabled,
			"Choose One"
		},
		/* [8] */
		{32, 336, 40, 344},
		UserItem {
			disabled
		},
		/* [9] */
		{112, 16, 144, 48},
		Icon {
			disabled,
			256
		}
	}
};

resource 'CNFG' (257, "TEXT Editor", purgeable) {
	'MACA'
};

resource 'DLOG' (257, "TEXT Editor", purgeable) {
	{42, 34, 226, 426},
	dBoxProc,
	invisible,
	noGoAway,
	0x0,
	257,
	""
};

resource 'DITL' (257, "TEXT Editor", purgeable) {
	{	/* array DITLarray: 11 elements */
		/* [1] */
		{24, 320, 48, 376},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{64, 320, 88, 376},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{40, 24, 56, 200},
		RadioButton {
			enabled,
			"MacWrite"
		},
		/* [4] */
		{64, 24, 80, 176},
		RadioButton {
			enabled,
			"Microsoft Word"
		},
		/* [5] */
		{88, 24, 104, 176},
		RadioButton {
			enabled,
			"Other:  File Creator"
		},
		/* [6] */
		{88, 176, 104, 240},
		EditText {
			disabled,
			""
		},
		/* [7] */
		{136, 72, 176, 376},
		StaticText {
			disabled,
			"Choose what kind of documents you want Moria "
			"to create when it prints to a file."
		},
		/* [8] */
		{16, 8, 120, 264},
		UserItem {
			disabled
		},
		/* [9] */
		{8, 24, 24, 104},
		StaticText {
			enabled,
			"Choose One"
		},
		/* [10] */
		{32, 360, 40, 368},
		UserItem {
			disabled
		},
		/* [11] */
		{136, 16, 168, 48},
		Icon {
			disabled,
			257
		}
	}
};

resource 'DLOG' (258, "Save, Quit, or Cancel", purgeable) {
	{40, 40, 168, 318},
	dBoxProc,
	invisible,
	noGoAway,
	0x0,
	258,
	""
};

resource 'DITL' (258, "Save, Quit, or Cancel", purgeable) {
	{	/* array DITLarray: 6 elements */
		/* [1] */
		{64, 16, 84, 76},
		Button {
			enabled,
			"Save"
		},
		/* [2] */
		{96, 16, 116, 76},
		Button {
			enabled,
			"Quit"
		},
		/* [3] */
		{96, 200, 116, 260},
		Button {
			enabled,
			"Cancel"
		},
		/* [4] */
		{64, 200, 80, 216},
		UserItem {
			disabled
		},
		/* [5] */
		{8, 56, 48, 264},
		StaticText {
			disabled,
			"What do you want to do with the game in progress?"
		},
		/* [6] */
		{8, 16, 40, 48},
		Icon {
			disabled,
			0
		}
	}
};

resource 'MENU' (129, "File") {
	129,
	textMenuProc,
	0x7FFFFFC3,
	enabled,
	"File",
	{	/* array: 7 elements */
		/* [1] */
		"New", noIcon, "N", "", plain,
		/* [2] */
		"Open\311", noIcon, "O", "", plain,
		/* [3] */
		"-", noIcon, "", "", plain,
		/* [4] */
		"Save", noIcon, "S", "", plain,
		/* [5] */
		"Save As\311", noIcon, "", "", plain,
		/* [6] */
		"-", noIcon, "", "", plain,
		/* [7] */
		"Quit", noIcon, "Q", "", plain
	}
};

resource 'MENU' (229, "File (No Keys)") {
	229,
	textMenuProc,
	0x7FFFFFC3,
	enabled,
	"File",
	{	/* array: 7 elements */
		/* [1] */
		"New", noIcon, "", "", plain,
		/* [2] */
		"Open\311", noIcon, "", "", plain,
		/* [3] */
		"-", noIcon, "", "", plain,
		/* [4] */
		"Save", noIcon, "", "", plain,
		/* [5] */
		"Save As\311", noIcon, "", "", plain,
		/* [6] */
		"-", noIcon, "", "", plain,
		/* [7] */
		"Quit", noIcon, "", "", plain
	}
};

resource 'MENU' (133, "Moria") {
	133,
	textMenuProc,
	0x7FFFFF17,
	enabled,
	"Moria",
	{	/* array: 5 elements */
		/* [1] */
		"Help\311", noIcon, "H", "", plain,
		/* [2] */
		"Command Set\311", noIcon, "", "", plain,
		/* [3] */
		"TEXT Editor\311", noIcon, "", "", plain,
		/* [4] */
		"-", noIcon, "", "", plain,
		/* [5] */
		"High Scores\311", noIcon, "", "", plain
	}
};

resource 'MENU' (233, "Moria (No Keys)") {
	233,
	textMenuProc,
	0x7FFFFF17,
	enabled,
	"Moria",
	{	/* array: 5 elements */
		/* [1] */
		"Help\311", noIcon, "", "", plain,
		/* [2] */
		"Command Set\311", noIcon, "", "", plain,
		/* [3] */
		"TEXT Editor\311", noIcon, "", "", plain,
		/* [4] */
		"-", noIcon, "", "", plain,
		/* [5] */
		"High Scores\311", noIcon, "", "", plain
	}
};

resource 'DLOG' (512, "Mac Help", purgeable) {
	{40, 34, 310, 496},
	dBoxProc,
	invisible,
	noGoAway,
	0x0,
	512,
	""
};

resource 'DITL' (512, "Mac Help", purgeable) {
	{	/* array DITLarray: 6 elements */
		/* [1] */
		{16, 392, 36, 452},
		Button {
			enabled,
			"Ok"
		},
		/* [2] */
		{24, 432, 32, 440},
		UserItem {
			disabled
		},
		/* [3] */
		{48, 8, 264, 440},
		UserItem {
			disabled
		},
		/* [4] */
		{48, 439, 264, 455},
		Control {
			enabled,
			512
		},
		/* [5] */
		{8, 52, 40, 380},
		StaticText {
			disabled,
			"Moria for the Macintosh Version 5.4"
		},
		/* [6] */
		{8, 8, 40, 40},
		Icon {
			disabled,
			128
		}
	}
};

resource 'CNTL' (512, "Mac Help Scroll Bar", purgeable) {
	{0, 0, 216, 16},
	0,
	visible,
	0,
	0,
	scrollBarProc,
	0,
	""
};

resource 'TEXT' (512, "Mac Help", purgeable) {
	"This is an beta version of Mac Moria, implementation 2.0b1, which is "
	"based on the Umoria 5.4 sources.  Please send comments and bug repor"
	"ts to wilson@toad.com (Jim Wilson).\n\n"
	"Users of previous versions o"
	"f Mac Moria should discard the old program.  Otherwise, the Finder mig"
	"ht get confused when you double-click on a saved game to start Moria.\n"
	"\n*****************************\n\nMoria is a dungeon exploration game.  "
	"The name comes from \322The Lord of the Rings\323 trilogy written by J. R"
	". R. Tolkien.  Moria is pronounced with the accent on the first syllable,"
	" e.g. MORE-ee-ah.  For details on the game itself, please read the Mor"
	"ia Docs file that is distributed along with Moria.\n\nMoria was original"
	"ly written for VAX/VMS systems.  As such,  it is heavily dependent on "
	"the keyboard for game play.  The mouse is not used at all, except for "
	"standard things like menus and dialogs.  To get a list of the availabl"
	"e keyboard commands, hit the \324?\325 key while playing the game.\n\nSome"
	" of the commands are specified as being control characters, i.e.  saving t"
	"he game is ^X.  To type these characters hold down the control key and"
	" then press the appropriate character key.  If you do not have a contr"
	"ol key, use the command key (apple/cloverleaf key) instead of the cont"
	"rol key.  If you prefer, you may enter these commands using two keystr"
	"okes, by first typing the '^' key followed by the letter.\n\nThe game al"
	"so uses the escape key (ESC) quite a bit.  If you do not have an escap"
	"e key on your keyboard, you can use the ` key or the Clear key on the "
	"numeric keypad instead.  An escape can also by typed as ^[, i.e. contr"
	"ol (or command) left bracket.\n\nFile Menu\n\nThe game fully supports the "
	"standard Macintosh file menu.  Use New to start playing a new game,  o"
	"r Open to restart a previous game.   Save As will always ask you for t"
	"he name of the file to save your character in.  Save will usually save"
	" the game in a file with the same name as the name of the current char"
	"acter.  If you are playing a new game, or have changed the name of you"
	"r character, Save will instead ask you for the name of the save file. "
	" The Quit option, of course, is used to exit the game.\n\nA game can onl"
	"y be saved at the point where it is your turn to move.  At all other t"
	"imes the Save command will not be available.\n\nThe File Menu is disable"
	"d while saving and restoring games.  There is no cause for alarm if you "
	"see this occur.  After the operation is completed, there will be a prompt"
	" that says \322press any key to continue\323.  After you press a key, t"
	"he File Menu will be reenabled.\n\nIf your character dies, or you decide"
	" to stop running him, you will be prompted via a standard file dialog "
	"for the name of a file in which to save his memories.  Pressing cancel"
	" will avoid saving the memories.\n\nEdit Menu\n\nThe Edit Menu items are n"
	"ot used by the game.  The Edit menu is disabled unless there is a Desk"
	" Accessory open.\n\nScreen/FontSize Menu\n\nIf you have a color system, th"
	"e Color item allows you to choose the background and foreground colors"
	" of the game window.  The game must use a constant width font, so only"
	" the Monaco font is supported.  You can, however, specify which point "
	"size that you want to use.  The menu will list every point size availa"
	"ble on your Macintosh for the Monaco font.\n\nAdvanced users can choose "
	"a different font by using ResEdit or a similar program to edit the STR"
	" \"Font Name\" resource.\n\nMoria Menu\n\nHelp brings up this dialog.\n\n"
	"Command Set allows you to choose between the original Moria command set "
	"and an optional command set similar to the UNIX game Rogue.  The main dif"
	"ference between the two is that the original command set uses the nume"
	"ric keypad for movement, whereas the Rogue-like command set uses the \324"
	"hjkl\325 keys for, respectively, left down up and right.  The program wil"
	"l remember your setting and use it as a default the next time you star"
	"t a new character.  The command set can be chosen for the current char"
	"acter by using the set options ('=') command.\n\nNote that the numeric k"
	"eypad, if your system has one, is always available for indicating dire"
	"ctions, regardless of the command set chosen.  The arrow keys will wor"
	"k as well.\n\nText Editor allows you to choose the type of TEXT file cre"
	"ated by Moria, that is, it allows you to specify which application wil"
	"l be used by the Finder when you open a TEXT file created by Moria.  Y"
	"ou can choose MacWrite or Microsoft Word files.  You can also specify "
	"the signature of the desired application if you prefer something else."
	"\n\nHigh Scores displays all of the scores stored in the scorefile.  You"
	" can clear these scores by just deleting the High Scores file.  This f"
	"eature is not implemented.\n\nConfiguration Info\n\nMoria is fully Multifi"
	"nder compatible/aware.  It requires 450 K of memory under Multifinder."
	"  The game will run on any Macintosh with 1 Megabyte or more of memory"
	".\n\nMoria comes with a documentation file, called Moria Docs.  The Mori"
	"a Docs file is an explanation of the game system.  Please remember to "
	"always distribute this file with the game, since it contains much impo"
	"rtant information.\n\nMoria will create two files when it is run.  The H"
	"igh Scores file keeps a list of the scores of all of the previous game"
	"s.  If this file is not present, a new one will be created in the fold"
	"er which contains the game.  This file can be kept either in the folde"
	"r that contains the application, or in the System Folder.\n\nMoria also "
	"creates a file called Moria Config in the System Folder.  This file co"
	"ntains all of the configuration info for Moria, such as the size and l"
	"ocation of windows, the font size, the command set, etc.  This file mu"
	"st remain in the Sytem Folder.\n\nThe text displayed when Moria starts u"
	"p is stored in the data fork of this program.  You can edit it with mo"
	"st text editors by first changing the file type of the program from 'A"
	"PPL' to 'TEXT' with ResEdit.  Remember to change the file type back ag"
	"ain when you are through.  Be careful not to disturb the lines which b"
	"egin with a '#'.\n\0x00"
};

type 'DFPR' {
	unsigned hex integer = $600A;					/* branch around header */
	unsigned hex integer;							/* flags */
	unsigned literal longint;						/* resource type */
	integer;										/* resource id */
	integer;										/* version */
	unsigned hex integer = $4EF9;					/* jump instruction */
	unsigned hex longint = $0;						/* target address for jump */
};

type 'LDEF' as 'DFPR';

resource 'LDEF' (514, "High Scores", purgeable) {
	$0, 'LDEF', 514, 0
};

resource 'DITL' (514, "High Scores", purgeable) {
	{	/* array DITLarray: 5 elements */
		/* [1] */
		{15, 422, 35, 482},
		Button {
			enabled,
			"Ok"
		},
		/* [2] */
		{24, 464, 32, 472},
		UserItem {
			disabled
		},
		/* [3] */
		{48, 2, 281, 490},
		UserItem {
			disabled
		},
		/* [4] */
		{8, 56, 40, 336},
		StaticText {
			disabled,
			"Moria High Scores\n"
			"Positions ^0 to ^1"
		},
		/* [5] */
		{8, 8, 40, 40},
		Icon {
			disabled,
			128
		}
	}
};

resource 'DLOG' (514, "High Scores", purgeable) {
	{40, 34, 327, 526},
	dBoxProc,
	invisible,
	noGoAway,
	0x0,
	514,
	""
};

resource 'DITL' (1024, "General Error", purgeable) {
	{	/* array DITLarray: 2 elements */
		/* [1] */
		{80, 304, 100, 364},
		Button {
			enabled,
			"Ok"
		},
		/* [2] */
		{7, 68, 71, 364},
		StaticText {
			disabled,
			"^0"
		}
	}
};

resource 'ALRT' (1024, "General Error", purgeable) {
	{42, 42, 148, 416},
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
	375 * 1024,
	375 * 1024
};
