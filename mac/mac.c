/* mac/mac.c: mac support code

   Copyright (c) 1989-1991 Curtis McCauley, James E. Wilson

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

#ifndef THINK_C
#include <types.h>
#include <controls.h>
#include <dialogs.h>
#include <memory.h>
#include <resources.h>
#include <files.h>
#include <segload.h>
#include <packages.h>
#include <menus.h>
#include <osutils.h>
#include <sysequ.h>

#include <scrnmgr.h>
#include <dumpres.h>

#else

#include "ScrnMgr.h"
#include "DumpRes.h"

#define c2pstr(x)	(char *)CtoPstr((char *)x)
#define p2cstr(x)	(char *)PtoCstr((char *)x)

#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef THINK_C
#include <strings.h>
#endif
#include <setjmp.h>

#include "config.h"
#include "constant.h"
#include "types.h"
#include "externs.h"
#include "macrsrc.h"

long GetCommandSet(Handle theData);
long GetTextEditor(Handle theData);

void DoMacHelp(void), DoKillsDlg(void);

static long game_flag;
static long exit_code;

static short savedir, applvrefnum;

static char **save_chars, **save_attrs;
static long save_cursor_h, save_cursor_v;

#define PATHLEN					256

#define TAB_WIDTH				8

#define CODE_IGNORE				0
#define CODE_PASSTHRU			1
#define CODE_KEYPAD				2
#define CODE_ARROW				3
#define CODE_ENTER				4

static unsigned char codetable[0x80] = {
	/*				0	1	2	3	4	5	6	7	8	9	A	B	C	D	E	F	*/
	/*				-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	*/
	/* [00] */		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
	/* [10] */		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
	/* [20] */		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
	/* [30] */		0,	1,	1,	1,	1,	1,	1,	0,	0,	0,	0,	0,	0,	0,	0,	1,
	/* [40] */		1,	1,	3,	1,	1,	1,	3,	1,	3,	1,	1,	1,	4,	3,	1,	1,
	/* [50] */		1,	1,	1,	2,	2,	2,	2,	2,	2,	2,	1,	2,	2,	1,	1,	1,
	/* [60] */		0,	0,	0,	0,	0,	0,	1,	0,	1,	0,	1,	0,	1,	0,	1,	0,
	/* [70] */		1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	3,	3,	3,	3,	1
};

#define ARROW_UP_1				0x4D
#define ARROW_DOWN_1			0x48
#define ARROW_LEFT_1			0x46
#define ARROW_RIGHT_1			0x42

#define ARROW_UP_2				0x7E
#define ARROW_DOWN_2			0x7D
#define ARROW_LEFT_2			0x7B
#define ARROW_RIGHT_2			0x7C

#define CMDSET_TYPE				'CNFG'
#define CMDSET_ID				256

#define TE_TYPE					'CNFG'
#define TE_ID					257

#define SPIN_TICKS				15

static long editor, cmdsetopt;

static jmp_buf jb;

#define ABOUT_DLOG_ID			128

#define FM_NEW					1
#define FM_OPEN					2
#define FM_SAVE					4
#define FM_SAVE_AS				5
#define FM_QUIT					7

#define AM_HELP					1
#define AM_CMD_SET				2
#define AM_TEXT_ED				3
#define AM_SCORES				5

#define KEY_UNKNOWN				0xFF
#define KEY_DIR					0x90
#define KEY_DIR_SHIFT			0xA0
#define KEY_DIR_CONTROL			0xB0

#define NEXT_FINDER				1
#define NEXT_NEW				2
#define NEXT_OPEN				3
#define NEXT_WAIT				4
#define NEXT_QUIT				5

long savefileset;
char savefilename[64];
short savefilevol;

#define sqc_dlg_id				258
#define sqc_save_item			1
#define sqc_quit_item			2
#define sqc_cancel_item			3
#define sqc_defbrd_item			4

#define code_enter				0x03
#define code_return				0x0D

#define errAlrtID				1024

static short sqcdefault_item;
static Handle sqcdefault_handle;

static long save_cmd_level, file_menu_level, app_menu_level;
static long save_cmd_enabled, file_menu_enabled, app_menu_enabled;

#if 0
/* this stuff is no longer used */

#define malloc_zone_size		0x1000

static THz malloc_zone;

void init_malloc_zone()

{
	THz theOldZone;

	theOldZone = GetZone();

	InitZone(NULL, 64, (Ptr)((long) malloc_zone + malloc_zone_size),
		 (Ptr) malloc_zone);

	SetZone(theOldZone);

	return;
}

void *mac_malloc(n)
unsigned n;

{
	THz theOldZone;
	void *theNewPtr;

	theOldZone = GetZone();
	SetZone(malloc_zone);

	theNewPtr = (void *) NewPtrClear((Size) n);

	SetZone(theOldZone);

	return(theNewPtr);
}

void mac_free(p)
void *p;

{
#if 0
	THz theOldZone;

	theOldZone = GetZone();
	SetZone(malloc_zone);

	DisposPtr((Ptr) p);

	SetZone(theOldZone);
#else
#pragma unused(p)
#endif

	return;
}

#endif

long mac_time()

{
	long now;

	GetDateTime((unsigned long *) &now);
	return(now);
}

void alert_error(message)
char *message;

{
	Str255 pstr;

	strncpy((char *)pstr, message, 255);
	pstr[255] = '\0';
	(void) c2pstr(pstr);

	ParamText(pstr, NULL, NULL, NULL);

	DoScreenALRT(errAlrtID, akStop, fixHalf, fixHalf);

	return;
}

void fatal_error(message)
char *message;

{
	alert_error(message);
	exit(0);
}

char *truncstr(pstr, wid)
char *pstr;
short wid;

{
	unsigned char len;

	if (StringWidth(pstr) > wid) {
		len = (unsigned char) pstr[0];
		if (len > 0) pstr[len] = '\311';
		while ( (len > 1) && (StringWidth(pstr) > wid) ) {
			pstr[0] = (char) --len;
			pstr[len] = '\311';
		}
	}

	return(pstr);
}

void idle()

{
	long redraw;

	redraw = FALSE;

	if ((!save_cmd_enabled) && (save_cmd_level > 0)) {
		EnableItem((MenuHandle) GetFileMHandle(), FM_SAVE);
		EnableItem((MenuHandle) GetFileMHandle(), FM_SAVE_AS);
		save_cmd_enabled = TRUE;
	}
	else if ((save_cmd_enabled) && (save_cmd_level <= 0)) {
		DisableItem((MenuHandle) GetFileMHandle(), FM_SAVE);
		DisableItem((MenuHandle) GetFileMHandle(), FM_SAVE_AS);
		save_cmd_enabled = FALSE;
	}

	if ((!file_menu_enabled) && (file_menu_level > 0)) {
		EnableItem((MenuHandle) GetFileMHandle(), 0);
		file_menu_enabled = TRUE;
		redraw = TRUE;
	}
	else if ((file_menu_enabled) && (file_menu_level <= 0)) {
		DisableItem((MenuHandle) GetFileMHandle(), 0);
		file_menu_enabled = FALSE;
		redraw = TRUE;
	}

	if ((!app_menu_enabled) && (app_menu_level > 0)) {
		EnableItem((MenuHandle) GetAppMHandle(), 0);
		app_menu_enabled = TRUE;
		redraw = TRUE;
	}
	else if ((app_menu_enabled) && (app_menu_level <= 0)) {
		DisableItem((MenuHandle) GetAppMHandle(), 0);
		app_menu_enabled = FALSE;
		redraw = TRUE;
	}

	if (redraw)
		DrawMenuBar();

	IdleScreenMgr();
}

int macgetkey(ch, nowait)
char *ch;
int nowait;

{
	char keycode, modifiers, ascii;
	short h, v;

	do {

		if (nowait) {
			idle();
			if (!GetScreenKeys(&keycode, &modifiers, &ascii, &h, &v))
				return(FALSE);
		}
		else {
			do {
				idle();
			} while (!GetScreenKeys(&keycode, &modifiers, &ascii, &h, &v));
		}

	} while ( (modifiers & maskModMouse) ||
			  (codetable[keycode] == CODE_IGNORE) );

	if (ascii > 0x7F) ascii = KEY_UNKNOWN;

	switch (codetable[keycode]) {

		case CODE_KEYPAD:
			if ((ascii >= '1') && (ascii <= '9')) {
				ascii -= '0';
				if (modifiers & maskModControl)
					ascii += KEY_DIR_CONTROL;
				else if (modifiers & maskModShift)
					ascii += KEY_DIR_SHIFT;
				else
					ascii += KEY_DIR;
			}
			break;

		case CODE_ARROW:
			switch (keycode) {
				case ARROW_UP_1: case ARROW_UP_2:
					ascii = 8;
					break;
				case ARROW_DOWN_1: case ARROW_DOWN_2:
					ascii = 2;
					break;
				case ARROW_LEFT_1: case ARROW_LEFT_2:
					ascii = 4;
					break;
				case ARROW_RIGHT_1: case ARROW_RIGHT_2:
					ascii = 6;
					break;
				default:
					ascii = 5;
					break;
			}
			if (modifiers & maskModControl)
				ascii += KEY_DIR_CONTROL;
			else if (modifiers & maskModShift)
				ascii += KEY_DIR_SHIFT;
			else
				ascii += KEY_DIR;
			break;

		case CODE_ENTER:
			ascii = '\r';
			break;

	}

	if (ch != NULL) *ch = ascii;

	return(TRUE);
}

int extractdir(ch, shift_flag, ctrl_flag)
char ch;
int *shift_flag;
int *ctrl_flag;

{
	long rc;

	if ((ch & 0xF0) == KEY_DIR) {
		*shift_flag = FALSE;
		*ctrl_flag = FALSE;
		rc = ch & 0x0F;
	}
	else if ((ch & 0xF0) == KEY_DIR_SHIFT) {
		*shift_flag = TRUE;
		*ctrl_flag = FALSE;
		rc = ch & 0x0F;
	}
	else if ((ch & 0xF0) == KEY_DIR_CONTROL) {
		*shift_flag = FALSE;
		*ctrl_flag = TRUE;
		rc = ch & 0x0F;
	}
	else
		rc = -1;

	return(rc);
}

void mac_save_screen()

{
	Rect screen;

	screen.left = screen.top = 0;
	screen.right = SCRN_COLS;
	screen.bottom = SCRN_ROWS;

	HLock((Handle) save_chars);
	HLock((Handle) save_attrs);

	GetScreenImage(*save_chars, *save_attrs, SCRN_COLS, &screen, 0, 0);
	GetScreenCursor(&save_cursor_h, &save_cursor_v);

	HUnlock((Handle) save_chars);
	HUnlock((Handle) save_attrs);

	return;
}

/* This restore routine only touches as much of the screen as necessary. */

void mac_restore_screen()

{
	Rect screen;
	long v;
	char chars[SCRN_COLS], attrs[SCRN_COLS];
	char *c, *a, *c1, *c2, *a1, *a2;

	HLock((Handle) save_chars);
	HLock((Handle) save_attrs);

	c = *save_chars;
	a = *save_attrs;

	screen.top = 0;
	screen.bottom = 1;

	for (v = 0; v < SCRN_ROWS; v++) {

		screen.left = 0;
		screen.right = SCRN_COLS;

		GetScreenImage(chars, attrs, SCRN_COLS, &screen, 0, v);

		c1 = chars;
		c2 = c;

		a1 = attrs;
		a2 = a;

		while (screen.left < SCRN_COLS) {
			if (*c1++ != *c2++) break;
			if (*a1++ != *a2++) break;
			++screen.left;
		}

		c1 = chars + SCRN_COLS;
		c2 = c + SCRN_COLS;

		a1 = attrs + SCRN_COLS;
		a2 = a + SCRN_COLS;

		while (screen.right > screen.left) {
			if (*--c1 != *--c2) break;
			if (*--a1 != *--a2) break;
			--screen.right;
		}

		if (screen.right > screen.left)
			DSetScreenImage(c, a, SCRN_COLS, &screen, screen.left, v);

		c += SCRN_COLS;
		a += SCRN_COLS;

	}

	DSetScreenCursor(save_cursor_h, save_cursor_v);

	HUnlock((Handle) save_chars);
	HUnlock((Handle) save_attrs);

	return;
}

void mac_beep()

{
	SysBeep(15);
	return;
}

void macbeginwait()

{
	BeginScreenWait(SPIN_TICKS);
	return;
}

void macendwait()

{
	EndScreenWait();
	return;
}

OSErr GetDirID(wdVRefNum, vRefNum, dirID)
short wdVRefNum;
short *vRefNum;
long *dirID;

{
	OSErr err;
	WDPBRec wdpb;

	wdpb.ioNamePtr = NULL;
	wdpb.ioVRefNum = wdVRefNum;
	wdpb.ioWDIndex = 0;
	wdpb.ioWDProcID = 0;
	wdpb.ioWDVRefNum = 0;
	err = PBGetWDInfo(&wdpb, false);

	if (!err) {
		*vRefNum = wdpb.ioWDVRefNum;
		*dirID = wdpb.ioWDDirID;
	}

	return(err);
}

OSErr MakePath(vRefNum, dirID, fName, pathName)
short vRefNum;
long dirID;
char *fName;
char *pathName;

{
	char buf[PATHLEN], *bufptr;
	long len;
	OSErr err;
	Str255 vNameBuf;
	CInfoPBRec cipb;

	bufptr = buf + PATHLEN;

	*--bufptr = '\0';

	if (fName != NULL) {
		len = strlen(fName);
		strncpy(bufptr -= len, fName, len);
	}

	cipb.dirInfo.ioNamePtr = vNameBuf;
	cipb.dirInfo.ioVRefNum = vRefNum;
	cipb.dirInfo.ioFDirIndex = -1;

	do {
		cipb.dirInfo.ioDrDirID = dirID;
		err = PBGetCatInfo(&cipb, false);
		if (!err) {
			*--bufptr = ':';
			len = strlen(p2cstr(vNameBuf));
			strncpy(bufptr -= len, (char *)vNameBuf, len);
			dirID = cipb.dirInfo.ioDrParID;
		}
	} while ( (!err) && (dirID != fsRtParID) );

	strcpy(pathName, bufptr);

	return(err);
}

void checkdebugger()

{
#ifndef THINK_C
#if 1
	char keys[128];

	GetKeys ((KeyMap *) &keys);
	if (keys[4] & 0x80) Debugger();		/* if command key is down */
#else
	if (((char *) KeyMapLM)[6] & 0x80) Debugger();			/* if command key is down */
#endif
#endif

	return;
}

static void getstack(request)
long request;

{
	long cursize;
#ifdef THINK_C
	Ptr newlimit;
#else
	long newlimit;
#endif

	/* An extra level of indirection is apparently needed by MPW C
	   for accessing system globals.  */
#ifdef THINK_C
	cursize = (((long *) CurStackBase) - ((long *) ApplLimit)) - 8;
	if (cursize < request) {
		newlimit = (Ptr)(((long *) CurStackBase - request) - 8);
		if (newlimit > HeapEnd)
			ApplLimit = newlimit;
	}
#else
	cursize = (*((int *) CurStackBase) - *((int *) ApplLimit)) - 8;
	if (cursize < request) {
		newlimit = (*((int *) CurStackBase) - request) - 8;
		if (newlimit > *((int *) HeapEnd)) *((int *) ApplLimit) = newlimit;
	}
#endif

	return;
}

void sfposition(vrefnum)
short vrefnum;

{
	short v;
	long d;

	GetDirID(vrefnum, &v, &d);
#ifdef THINK_C
	SFSaveDisk = -v;
	CurDirStore = d;
#else
	*((short *) SFSaveDisk) = -v;
	*((int *) CurDirStore) = d;
#endif

	return;
}

long doputfile(prompt, fname, vrefnum)
char *prompt;
char *fname;
short *vrefnum;

{
	char p[256], f[256];
	SFReply reply;
	Point loc;
	long h, v;

	CenterScreenDLOG(putDlgID, fixHalf, fixThird, &h, &v);

	loc.h = (short) h;
	loc.v = (short) v;

	strncpy(p, prompt, 255);
	strncpy(f, fname, 255);
	p[255] = '\0';
	f[255] = '\0';
	c2pstr(p);
	c2pstr(f);

	SFPutFile(loc, p, f, NULL, &reply);

	if (reply.good) {
		p2cstr(reply.fName);
		strcpy(fname, (char *)reply.fName);
		*vrefnum = reply.vRefNum;
	}

	return(reply.good);
}

int asksavegame(ask)
int ask;

{
	int rc;

	if (!ask && savefileset)
		rc = TRUE;

	else {
		rc = doputfile("Save game as:", savefilename, &savefilevol);
		if (rc) savefileset = TRUE;
	}

	return(rc);
}

static pascal Boolean savefilefilter(pb)
ParmBlkPtr pb;

{
	OSType fdtype, fdcreator;

	fdtype = pb->fileParam.ioFlFndrInfo.fdType;
	fdcreator = pb->fileParam.ioFlFndrInfo.fdCreator;

	return(!((fdcreator == MORIA_FCREATOR) && (fdtype == SAVE_FTYPE)));
}

int dogetfile(fname, vrefnum)
char *fname;
short *vrefnum;

{
	SFTypeList types;
	SFReply reply;
	Point loc;
	long h, v;

	CenterScreenDLOG(getDlgID, fixHalf, fixThird, &h, &v);

	loc.h = (short) h;
	loc.v = (short) v;

	types[0] = SAVE_FTYPE;

	SFGetFile(loc, NULL, savefilefilter, 1, types, NULL, &reply);

	if (reply.good) {
		p2cstr(reply.fName);
		strcpy(fname, (char *)reply.fName);
		*vrefnum = reply.vRefNum;
	}

	return(reply.good);
}

static pascal Boolean sqcfilter(dlg, evt, item)
DialogPtr dlg;
EventRecord *evt;
short *item;

{
#pragma unused(dlg)
	Boolean rc;
	char key;

	rc = FALSE;

	if (evt->what == keyDown) {
		key = evt->message & charCodeMask;
		if ( (key == code_enter) || (key == code_return) ) {
			rc = TRUE;
			HiliteControl((ControlHandle) sqcdefault_handle, inButton);
			*item = sqcdefault_item;
		}
	}

	return(rc);
}

short currentdirectory()

{
	short vrefnum;

	(void) GetVol(NULL, &vrefnum);
	return(vrefnum);
}

void changedirectory(vrefnum)
short vrefnum;

{
	(void) GetVol(NULL, &savedir);
	(void) SetVol(NULL, vrefnum);
	return;
}

void appldirectory()

{
	(void) GetVol(NULL, &savedir);
	(void) SetVol(NULL, applvrefnum);
	return;
}

void restoredirectory()

{
	(void) SetVol(NULL, savedir);
	return;
}

void mac_helpfile(filename, wait)
char *filename;
int wait;

{
	Str255 temp, temp2;	/* Buffer for line and tab-expanded line.  */
	short apRefNum;
	Handle apParam;
	FILE *file;
	int i, j, done;
	char ch;
	char *cp;		/* Source for tab expansion.  */
	Rect area;

	GetAppParms(temp, &apRefNum, &apParam);

	appldirectory();
	/* Ordinarily, the misc files are stored in the data fork of
	   the application.  */
#if 1
	file = fopen(p2cstr(temp), "r");
#else
	file = fopen("MacMoria.files", "r");
#endif
	restoredirectory();

	if (file != NULL) {

		while (!feof(file))
			if (fgets ((char *)temp, 255, file) != NULL)
				if (temp[0] == '#')
					if (strstr((char *)temp, filename) != NULL)
						break;

		if (feof(file)) {
			(void) fclose(file);
			file = NULL;
		}

	}

	if (file == NULL) {
		sprintf((char *)temp, "Cannot find text file: %s", filename);
		alert_error(temp);
	}

	else {

		area.left = area.top = 0;
		area.right = SCRN_COLS;
		area.bottom = SCRN_ROWS;

		if (wait)
			mac_save_screen();

		done = FALSE;

		while ((!done) && (!feof(file))) {
			DEraseScreen(&area);
			for (i = 0; (!done) && (i < SCRN_ROWS - 1); i++)
				if ((fgets((char *)temp, 255, file) != NULL) && (temp[0] != '#')) {
					/* Remove the trailing \n and
					   expand tabs.  */
				  	for (cp = (char *) temp, j = 0;
					     *cp != '\n'; cp++)
					  {
					    if ((temp2[j] = *cp) == '\t')
					      {
						do
						  {
						    temp2[j++] = ' ';
						  }
						while (j % TAB_WIDTH != 0);
					      }
					    else
					      j++;
					  }
					temp2[j] = '\0';
					DSetScreenString((char *)temp2, 0, i);
					}
				else
					done = TRUE;
			if (wait) {
				DSetScreenCursor(20, SCRN_ROWS - 1);
				DWriteScreenString(
					done ?
						"[Press any key to continue.]" :
						"[Press any key for next page or ESC to abort.]");
				macgetkey(&ch, FALSE);
				if (ch == ESCAPE)
					done = TRUE;
			}
		}

		if (wait)
			mac_restore_screen();

		fclose(file);

	}

	return;
}

char *nextstring(loc)
char **loc;

{
	char *str;
	unsigned char len;

	len = *((unsigned char *) (*loc));
	str = p2cstr(*loc);
	*loc += len + 1;

	return(str);
}

#if 0
/* no longer used */
long allocmalloczone(restart)
long restart;

{
	long rc;

	if (!restart) {
		malloc_zone = (THz) NewPtr(malloc_zone_size);
		rc = malloc_zone != NULL;
	}
	else
		rc = TRUE;

	if (rc && !restart)
		init_malloc_zone();

	return(rc);
}
#endif

int allocsavearea(restart)
int restart;

{
	int rc;

	if (!restart) {
		save_chars = (char **) NewHandle(SCRN_ROWS * SCRN_COLS);
		save_attrs = (char **) NewHandle(SCRN_ROWS * SCRN_COLS);
		rc = (save_chars != NULL) && (save_attrs != NULL);
	}
	else
		rc = TRUE;

	return(rc);
}

int setupmem(restart)
int restart;

{
	int i;
	memtable_type *m;

	for (i = 0, m = memtable; i < MAX_PTRS; i++, m++) {

		if (!restart) {
			*m->memPtr = (char *) NewPtrClear(m->elemCnt * m->elemSiz);
			if (*m->memPtr == NULL) return(FALSE);
		}

		else if (m->restartFlag) {
			memset(*m->memPtr, 0, m->elemCnt * m->elemSiz);
		}

	}

	return(TRUE);
}

int getresources(restart)
int restart;

{
	int i, rc;
	restable_type *r;

	for (i = 0, r = restable; i < MAX_RESOURCES; i++, r++) {

		if (!restart) *r->memPtr = NULL;

		if ( (!restart) || (r->restartFlag) ) {

			rc = LoadRes(
				r->memPtr,
				r->resType, r->resID,
				r->elemCnt, r->elemSiz,
				r->strProc);

			if (!rc) return(FALSE);

		}

	}

	return(TRUE);
}

int getrestart(restart)
int restart;

{
	int i, rc;
	unsigned size;
	char *p, *q;

	if (restart) {

		size = 0;
		for (i = 0; i < MAX_RESTART; i++) size += restart_vars[i].size;

		p = NULL;

		rc = LoadRes(
			&p,
			restartRsrc, restart_id,
			1, size,
			NULL);

		if (!rc) return(FALSE);

		q = p;
		for (i = 0; i < MAX_RESTART; i++) {
			BlockMove(q, restart_vars[i].ptr, restart_vars[i].size);
			q += restart_vars[i].size;
		}

		DisposPtr(p);

	}

	return(TRUE);
}

int clearvars(restart)
int restart;

{
	int i;
	clrtable_type *c;

	if (restart)
		for (i = 0, c = clrtable; i < MAX_CLRS; i++, c++)
			memset(c->ptr, 0, c->size);

	return(TRUE);
}

#ifndef THINK_C
unsigned sleep(time)
unsigned time;

{
#pragma unused(time)
	idle();
	return(0);
}
#endif

char *makefilename(buffer, suffix, append)
char *buffer, *suffix;
int append;


{
	long len;
	char *p;

	len = strlen(py.misc.name) + ((append) ? strlen(suffix)+3 : 0);

	if ( (strlen(py.misc.name) == 0) || (len > 31) )
		strcpy(buffer, suffix);

	else {
		strcpy(buffer, py.misc.name);
		if (append) {
			strcat(buffer, "'s ");
			strcat(buffer, suffix);
		}
		for (p = buffer; *p; p++)
			if (*p == ':') *p = '.';
	}

	return(buffer);
}

void initsavedefaults()

{
	savefileset = FALSE;

	(void) makefilename(savefilename, "Save File", FALSE);
	savefilevol = currentdirectory();

	return;
}

void setsavedefaults(name, vol)
char *name;
short vol;

{
	savefileset = TRUE;

	strncpy(savefilename, name, 63);
	savefilename[63] = '\0';

	savefilevol = vol;

	return;
}

int getsavedefaults(name, vol)
char *name;
short *vol;

{
	strcpy(name, savefilename);
	*vol = savefilevol;

	return(savefileset);
}

int getfinderfile()

{
	short message, count, i;
	AppFile appfile;

	CountAppFiles(&message, &count);

	for (i = 1; i <= count; i++) {
		GetAppFiles(i, &appfile);
		if (appfile.fType == SAVE_FTYPE) break;
	}

	if (i <= count) {
		setsavedefaults(p2cstr(appfile.fName), appfile.vRefNum);
	}

	return(i <= count);
}

long setfileinfo(fname, vrefnum, ftype)
char *fname;
short vrefnum;
long ftype;

{
	long fcreator;
	char temp[64];
	FileParam pb;
	OSErr err;

	fcreator = (ftype != INFO_FTYPE) ? MORIA_FCREATOR : editor;

	strcpy(temp, fname);
	(void) c2pstr(temp);

	pb.ioCompletion = NULL;
	pb.ioNamePtr = (unsigned char *)temp;
	pb.ioVRefNum = vrefnum;
	pb.ioFVersNum = 0;
	pb.ioFDirIndex = 0;

	err = PBGetFInfo((ParmBlkPtr) &pb, FALSE);

	if (err == noErr) {
		pb.ioFlFndrInfo.fdType = ftype;
		pb.ioFlFndrInfo.fdCreator = fcreator;
		err = PBSetFInfo((ParmBlkPtr) &pb, FALSE);
	}

	return(err == noErr);
}

#if 0
long getfileage(fname, vrefnum)
char *fname;
short vrefnum;

{
	char temp[64];
	FileParam pb;
	OSErr err;
	long age;

	strcpy(temp, fname);
	(void) c2pstr(temp);

	pb.ioCompletion = NULL;
	pb.ioNamePtr = temp;
	pb.ioVRefNum = vrefnum;
	pb.ioFVersNum = 0;
	pb.ioFDirIndex = 0;

	err = PBGetFInfo((ParmBlkPtr) &pb, FALSE);

	if (err == noErr) {
		GetDateTime((unsigned long *) &age);
		age -= pb.ioFlMdDat;
	}
	else
		age = 0;

	return(age);
}
#endif

void benediction()

{
	Rect scrn;

	scrn.left = 0;
	scrn.top = SCRN_ROWS-1;
	scrn.right = SCRN_COLS;
	scrn.bottom = SCRN_ROWS;

	EraseScreen(&scrn);

	SetScreenCursor(0, SCRN_ROWS-1);
	WriteScreenString("Please select a command from the file menu...");

	return;
}

void setsavecmdstatus(val, init)
int val, init;

{
	save_cmd_level = val;

	if (init)
		save_cmd_enabled = val > 0;

	return;
}

void enablesavecmd(flag)
int flag;

{
	save_cmd_level += flag ? 1 : -1;
	return;
}

void setfilemenustatus(val, init)
int val, init;

{
	file_menu_level = val;

	if (init)
		file_menu_enabled = val > 0;

	return;
}

void enablefilemenu(flag)
int flag;

{
	file_menu_level += flag ? 1 : -1;
	return;
}

void setappmenustatus(val, init)
int val, init;

{
	app_menu_level = val;

	if (init)
		app_menu_enabled = val > 0;

	return;
}

void enableappmenu(flag)
int flag;

{
	app_menu_level += flag ? 1 : -1;
	return;
}

void savequitorcancel(next)
int next;

{
	DialogPtr thedialog;
	short itemhit;
	short itstype;
	Rect itsrect;
	long h, v;
	long saveOk, dialogFinished;

	thedialog = GetNewDialog(sqc_dlg_id, nil, (WindowPtr) -1);

	CenterScreenDLOG(sqc_dlg_id, fixHalf, fixThird, &h, &v);
	MoveWindow((WindowPtr) thedialog, (short) h, (short) v, false);

	if ((save_cmd_level > 0) && !total_winner) {
		sqcdefault_item = sqc_save_item;
		saveOk = true;
	}
	else {
		sqcdefault_item = sqc_quit_item;
		saveOk = false;
	}

	GetDItem(thedialog, sqcdefault_item, &itstype, &sqcdefault_handle, &itsrect);
	InsetRect(&itsrect, -4, -4);

	SetDItem(thedialog, sqc_defbrd_item, userItem,
		 (Handle) DrawDefaultBorder, &itsrect);

	ShowWindow((WindowPtr) thedialog);

	do {
		ModalDialog(sqcfilter, &itemhit);
		if ( (!saveOk) && (itemhit == sqc_save_item) ) {
			if (total_winner)
				alert_error("Sorry.  Since you are a total \
winner, you cannot save this game.  Your character must be retired.");
			else
				alert_error("Sorry.  You cannot save at this \
point in the game.  It must be your turn to move in order to save.");
			dialogFinished = false;
		}
		else
			dialogFinished = (itemhit == sqc_save_item) ||
						(itemhit == sqc_quit_item) ||
						(itemhit == sqc_cancel_item);
	} while (!dialogFinished);

	DisposDialog(thedialog);

	switch (itemhit) {

		case sqc_save_item:
			if (asksavegame(FALSE)) {
				HiliteMenu(0);
				(void) strcpy (died_from, "(saved)");
				exit_code = next;
				if (save_char(FALSE))
					exit_game();
				/* Should only get here if save_char fails */
				UnloadSeg(save_char);
				UnloadSeg(exit_game);
				exit_code = NEXT_WAIT;
				(void) strcpy (died_from, "(alive and well)");
			}
			break;

		case sqc_quit_item:
			HiliteMenu(0);
			death = TRUE;
			(void) strcpy(died_from, "Quitting.");
			exit_code = next;
			exit_game();
			/* Should never get here */
			UnloadSeg(exit_game);
			exit_code = NEXT_WAIT;
			(void) strcpy (died_from, "(alive and well)");
			death = FALSE;
			break;

		default:
			HiliteMenu(0);
			break;

	}

	return;
}

void loaddata(restart)
int restart;
{
	if (
#if 0
	    		(!allocmalloczone(restart))	||
#endif
			(!allocsavearea(restart))	||
			(!setupmem(restart))		||
			(!getresources(restart))	||
			(!getrestart(restart))		||
			(!clearvars(restart))			)

				fatal_error("Insufficient memory to play Moria.  Sorry.");

	return;
}

void goback()

{
	longjmp(jb, exit_code);
}

int startfinder()

{
	int next;
	int argc;
	long local_cmdsetopt;
	char *argv[2];

	if ((next = setjmp(jb)) != 0) return(next);

	clear_screen();

	local_cmdsetopt = cmdsetopt;

	argc = 0;
	argv[argc++] = NULL;
	argv[argc++] = (char *) &local_cmdsetopt;

	game_flag = TRUE;
	exit_code = NEXT_WAIT;

	moria_main(argc, argv);

	/* should never get here */
	return(NEXT_WAIT);
}

int startnew()

{
	int next;
	int argc;
	long local_cmdsetopt, local_newgameopt;
	char *argv[3];

	if ((next = setjmp(jb)) != 0) return(next);

	clear_screen();

	local_cmdsetopt = cmdsetopt;
	local_newgameopt = '-n\0\0';

	argc = 0;
	argv[argc++] = NULL;
	argv[argc++] = (char *) &local_cmdsetopt;
	argv[argc++] = (char *) &local_newgameopt;

	game_flag = TRUE;
	exit_code = NEXT_WAIT;

	moria_main(argc, argv);

	/* should never get here */
	return(NEXT_WAIT);
}

int startopen()

{
	int next;
	int argc;
	long local_cmdsetopt;
	char *argv[2];

	if ((next = setjmp(jb)) != 0) return(next);

	clear_screen();

	if (dogetfile(savefilename, &savefilevol)) {

		savefileset = TRUE;

		local_cmdsetopt = cmdsetopt;

		argc = 0;
		argv[argc++] = NULL;
		argv[argc++] = (char *) &local_cmdsetopt;

		game_flag = TRUE;
		exit_code = NEXT_WAIT;

		moria_main(argc, argv);

	}

	/* should not get here unless user cancels standard file dialog */
	return(NEXT_WAIT);
}

int waitforchoice()

{
	int next;

	if ((next = setjmp(jb)) != 0) return(next);

	game_flag = FALSE;

	benediction();

	do {
		idle();
	} while(TRUE);

	return(NEXT_WAIT);
}

void dofmnew()

{
	if (game_flag && character_generated)
		savequitorcancel(NEXT_NEW);

	else {
		HiliteMenu(0);
		exit_code = NEXT_NEW;
		goback();
	}

	return;
}

void dofmopen()

{
	if (game_flag && character_generated)
		savequitorcancel(NEXT_OPEN);

	else {
		HiliteMenu(0);
		exit_code = NEXT_OPEN;
		goback();
	}

	return;
}

void dofmsave(ask)
int ask;

{
	if (game_flag && character_generated && total_winner) {
		alert_error("Sorry.  Since you are a total winner, you cannot\
 save this game.  Your character must be retired.");
		HiliteMenu(0);
	}
	else if (game_flag && character_generated && asksavegame(ask)) {
		HiliteMenu(0);
		(void) strcpy (died_from, "(saved)");
		if (save_char(FALSE))
			exit_game();
		/* Should only get here if save_char fails */
		UnloadSeg(save_char);
		UnloadSeg(exit_game);
		(void) strcpy (died_from, "(alive and well)");
	}
	else
		HiliteMenu(0);

	return;
}

void dofmquit()

{
	if (game_flag && character_generated)
		savequitorcancel(NEXT_QUIT);

	else {
		HiliteMenu(0);
		exit_code = NEXT_QUIT;
		goback();
	}

	return;
}

void dofilemenu(item)
long item;

{
	switch (item) {

		case FM_NEW:		dofmnew();
							break;

		case FM_OPEN:		dofmopen();
							break;

		case FM_SAVE:		dofmsave(FALSE);
							break;

		case FM_SAVE_AS:	dofmsave(TRUE);
							break;

		case FM_QUIT:		dofmquit();
							break;

		case closeBoxItem:	if (file_menu_level > 0) dofmquit();
							break;

		default:			HiliteMenu(0);
							break;

	}

	return;
}

void doappmenu(item)
long item;

{
	switch (item) {

		case AM_HELP:		DoMacHelp();
							UnloadSeg(DoMacHelp);
							break;

		case AM_CMD_SET:	ConfigScreenMgr(TRUE, CMDSET_TYPE,
							CMDSET_ID, GetCommandSet);
							UnloadSeg(GetCommandSet);
							cmdsetopt = **((long **) GetResource(CMDSET_TYPE, CMDSET_ID));
							HiliteMenu(0);
							break;

		case AM_TEXT_ED:	ConfigScreenMgr(TRUE, TE_TYPE, TE_ID, GetTextEditor);
							UnloadSeg(GetTextEditor);
							editor = **((long **) GetResource(TE_TYPE, TE_ID));
							HiliteMenu(0);
							break;

		case AM_SCORES:		DoScoresDlg();
							UnloadSeg(DoScoresDlg);
							break;

		default:			HiliteMenu(0);
							break;

	}

	return;
}

void unloadsegments()

{
#ifndef THINK_C
	extern void create_character(),
		creatures(),
		exit_game(),
		eat(),
		file_character(),
		generate_cave(),
		ident_char(),
		cast(),
		pray(),
		quaff(),
		roff_recall(),
		save_char(),
		read_scroll(),
		use(),
		store_init(),
		aim(),
		wizard_light();

	UnloadSeg(create_character);			/* Create			*/
	UnloadSeg(creatures);					/* Creature			*/
	UnloadSeg(exit_game);					/* Death			*/
	UnloadSeg(eat);							/* Eat				*/
	UnloadSeg(file_character);				/* Files			*/
	UnloadSeg(generate_cave);				/* Generate			*/
	UnloadSeg(ident_char);					/* Help				*/
	UnloadSeg(cast);						/* Magic			*/
	UnloadSeg(pray);						/* Prayer			*/
	UnloadSeg(quaff);						/* Potions			*/
	UnloadSeg(roff_recall);					/* Recall			*/
	UnloadSeg(save_char);					/* Save				*/
	UnloadSeg(read_scroll);					/* Scrolls			*/
	UnloadSeg(use);							/* Staffs			*/
	UnloadSeg(store_init);					/* Store			*/
	UnloadSeg(aim);							/* Wands			*/
	UnloadSeg(wizard_light);				/* Wizard			*/
#endif

	return;
}

void salutation()

{
	mac_helpfile(MORIA_MOR, FALSE);
}

int main()

{
	int next, savedgame, restart_flag;
#ifndef THINK_C
	extern void _DataInit();
#endif

	checkdebugger();

#ifndef THINK_C
	UnloadSeg(_DataInit);
#endif

	getstack(0x7000L);

  	MaxApplZone();

	/* Do this early to prevent fragmentation */
	loaddata(FALSE);

	InitScreenMgr(SCRN_COLS, SCRN_ROWS,
		"Moria", "Moria Config",
		MORIA_FCREATOR, CONFIG_FTYPE,
		dofilemenu, doappmenu,
		twoColor);

	/* Get permanently set options */
	/* If user has not set them yet, a dialog will be invoked */
	/* Otherwise, they will be loaded from the preferences file */

	ConfigScreenMgr(FALSE, CMDSET_TYPE, CMDSET_ID, GetCommandSet);
#ifndef THINK_C
	UnloadSeg(GetCommandSet);
#endif
	cmdsetopt = **((long **) GetResource(CMDSET_TYPE, CMDSET_ID));

	ConfigScreenMgr(FALSE, TE_TYPE, TE_ID, GetTextEditor);
#ifndef THINK_C
	UnloadSeg(GetTextEditor);
#endif
	editor = **((long **) GetResource(TE_TYPE, TE_ID));

	DefineScreenCursor(attrColorFore, 2, GetCaretTime());
	ShowScreenCursor();

	/* Save the application volume */
	GetVol(NULL, &applvrefnum);

	/* These should be based up the menu resources */
	setsavecmdstatus(0, TRUE);
	setfilemenustatus(1, TRUE);
	setappmenustatus(1, TRUE);

	restart_flag = FALSE;

	initsavedefaults ();

	/* Find out if user has started from a saved game */
	savedgame = getfinderfile();

	if (savedgame) {
		enablefilemenu(FALSE);
		salutation();
		pause_line(23);
		enablefilemenu(TRUE);
		next = NEXT_FINDER;
	}
	else {
		salutation();
		next = NEXT_WAIT;
	}

	do {
		setsavecmdstatus(0, FALSE);
		setfilemenustatus(1, FALSE);
		setappmenustatus(1, FALSE);
		switch (next) {
			case NEXT_FINDER:
				if (restart_flag) loaddata(TRUE);
				next = startfinder();
				unloadsegments();
				restart_flag = TRUE;
				break;
			case NEXT_NEW:
				if (restart_flag) loaddata(TRUE);
				next = startnew();
				unloadsegments();
				restart_flag = TRUE;
				break;
			case NEXT_OPEN:
				if (restart_flag) loaddata(TRUE);
				next = startopen();
				unloadsegments();
				restart_flag = TRUE;
				break;
			case NEXT_WAIT:
				next = waitforchoice();
				break;
			default:
				msg_print("What was that?");
				next = NEXT_WAIT;
				break;
		}
	} while (next != NEXT_QUIT);

	CloseScreenMgr();

	/* That's all, folks... */
	return(0);
}
