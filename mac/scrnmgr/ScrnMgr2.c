
static void DoContentClick(whichWindow, cursorLoc)
WindowPtr whichWindow;
Point *cursorLoc;

{
	int thePart;
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
	int part;
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
	register int v;
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
int h, v;

{
    int loc;
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
int row;
Rect *bounds;
int h, v;

{
    int i;
    int wid;
    int srcLoc, dstLoc;
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
int h, v;

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
int h, v;

{
    int loc;
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
int row;
Rect *bounds;
int h, v;

{
    int i;
    int wid;
    int srcLoc, dstLoc;
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
int h, v;

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
int row;
Rect *bounds;
int h, v;

{
    int i;
    int wid;
    int srcLoc, dstLoc;
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
int row;
Rect *bounds;

{
    XSetScreenBuffer(d, c, row, bounds, theScreen.cursor.h, theScreen.cursor.v);
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
int row;
Rect *bounds;

{
    XSetScreenBufferAttr(d, c, a, row, bounds, theScreen.cursor.h, theScreen.cursor.v);
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
int row;
Rect *bounds;

{
    XSetScreenImage(d, c, a, row, bounds, theScreen.cursor.h, theScreen.cursor.v);
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
    int i;
    int wid;
    int dstLoc;
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
int offset;
int factor;

{
    int i, j;
    int loc;
	int nxt;
    char *srcc, *dstc;
    char *srca, *dsta;

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
int offset;
int factor;

{
    int i, j;
    int loc;
	int nxt;
    char *srcc, *dstc;
    char *srca, *dsta;

    loc = area->top * theScreen.scrnCDims.h + area->left;

    dstc = *theScreen.chars + loc;
    srcc = dstc - offset * factor;

    dsta = *theScreen.chars + loc;
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
int dh, dv;
Rect *bounds;
char attr;

{
    int wid, dep;
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
				ScrollScreenPositive(&area, dv, theScreen.scrnCDims.h);
				XFillScreen(d, ' ', attr, &clear);
			}

			else if (dv < 0) {
				clear = area;
				clear.top = area.bottom += dv;
				ScrollScreenNegative(&area, dv, theScreen.scrnCDims.h);
				XFillScreen(d, ' ', attr, &clear);
			}

			if (dh > 0) {
				clear = area;
				clear.right = area.left += dh;
				ScrollScreenPositive(&area, dh, 1);
				XFillScreen(d, ' ', attr, &clear);
			}

			else if (dh < 0) {
				clear = area;
				clear.left = area.right += dh;
				ScrollScreenNegative(&area, dh, 1);
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
int h, v;

{
    int loc;
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
int row;
Rect *bounds;
int h, v;

{
    int i;
    int wid;
    int srcLoc, dstLoc;
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
int h, v;

{
	InvalCursor(d);

    theScreen.cursor.h += h;
    theScreen.cursor.v += v;

	InvalCursor(d);

    return;
}

void XSetScreenCursor(d, h, v)
int d;
int h, v;

{
	InvalCursor(d);

    theScreen.cursor.h = h;
    theScreen.cursor.v = v;

	InvalCursor(d);

    return;
}

void GetScreenCursor(h, v)
int *h, *v;
{
    *h = theScreen.cursor.h;
    *v = theScreen.cursor.v;
    return;
}

static int CheckCursorStatus()

{
	int oldStatus;
	int changed;

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
	int v;
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
int len;

{
    int count;
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
    int j;
	int top, dep, left, wid;
    Rect clear, area, curs;
	RgnHandle vis;
    char *c, *a;
	int cursorLoc;

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
			cursorLoc = theScreen.cursor.v * theScreen.scrnCDims.h + theScreen.cursor.h;
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
	int more;
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

				case osEvent:		if ((theScreen.event.message >> 24) == suspendResumeMessage) {
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

int CountScreenKeys()

{
	return(LenQueue());
}

int GetScreenKeys(keycode, modifiers, ascii, h, v)
char *keycode;
char *modifiers;
char *ascii;
int *h;
int *v;

{
	int flag;
	short th, tv;

	if (flag = PopQueue(keycode, modifiers, ascii, &th, &tv)) {
		if (h != NULL) *h = th;
		if (v != NULL) *v = tv;
	}

	return(flag);
}

void EnableScreenMouse(flag)
int flag;

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
int color;
int lines;
int blinkRate;

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
int flag;

{
	theScreen.quitProc = procPtr;
	theScreen.quitReturns = flag;
	return;
}

int YesOrNo(text)
char *text;

{
	DialogPtr theDialog;
	short itemHit;
	short itsType;
	Handle itsHandle;
	Rect itsRect;
	Str255 ptext;
	int h, v;

	theDialog = GetNewDialog(yesOrNoDlgID, nil, (WindowPtr) -1);

	CenterScreenDLOG(yesOrNoDlgID, fixHalf, fixThird, &h, &v);
	MoveWindow((WindowPtr) theDialog, (short) h, (short) v, false);

	GetDItem(theDialog, ok, &itsType, &itsHandle, &itsRect);
	InsetRect(&itsRect, -4, -4);

	SetDItem(theDialog, yesOrNoDfltBorder, userItem, (Handle) DrawDefaultBorder, &itsRect);

	if (text != NULL) {
		strncpy(ptext, text, 255);
		ptext[255] = '\0';
		c2pstr(ptext);
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
int visible;

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

	return;
}


void CenterScreenDLOG(id, hRatio, vRatio, h, v)
int id;
Fixed hRatio, vRatio;
int *h, *v;

{
	int wid, hgt;
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

int DoScreenALRT(id, kind, hRatio, vRatio)
int id;
int kind;
Fixed hRatio, vRatio;

{
	int wid, hgt, h, v;
	int item;
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
int force;
ResType theType;
int theID;
int (*ConfigProc)(Handle theData);

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
	int oldA5;

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
int rate;

{
	if (!theScreen.waitFlag) {

		(*theScreen.acur)->next = 0;

		SetCursor(*((*theScreen.acur)->table[0].cursHandle));
		ShowCursor();

		theScreen.waitFlag = true;
		theScreen.waitRate = rate;

		theScreen.vbl.qType = vType;
		theScreen.vbl.vblAddr = AnimateCursor;
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

int PushScreen()

{
	int errcode;
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

		XSetScreenCursor(0, (*theScreen.stack)->cursor.h, (*theScreen.stack)->cursor.v);

		DisposeStackTop();

		UpdateScreen();

	}

	return;
}
