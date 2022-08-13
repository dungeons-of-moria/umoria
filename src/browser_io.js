/* Copyright (c) 2021 Jeffrey D. Hirschberg */
var IO = (function() {
  "use strict";

  const CURSOR_ID = "cursor";
  const ERR = -1;
  const SCREEN_ID = "screen";
  const SPACE = 32;
  const TERMINAL_ID = "terminal";

  const ASCII_MAP = new Map();

  ASCII_MAP.set("^@",        0x00);
  ASCII_MAP.set("^a",        0x01);
  ASCII_MAP.set("^b",        0x02);
  ASCII_MAP.set("^c",        0x03);
  ASCII_MAP.set("^d",        0x04);
  ASCII_MAP.set("^e",        0x05);
  ASCII_MAP.set("^f",        0x06);
  ASCII_MAP.set("^g",        0x07);
  ASCII_MAP.set("^h",        0x08);
  ASCII_MAP.set("Backspace", 0x08);
  ASCII_MAP.set("^i",        0x09);
  ASCII_MAP.set("^j",        0x0A);
  ASCII_MAP.set("^k",        0x0B);
  ASCII_MAP.set("^l",        0x0C);
  ASCII_MAP.set("^m",        0x0D);
  ASCII_MAP.set("Enter",     0x0D);
  ASCII_MAP.set("^n",        0x0E);
  ASCII_MAP.set("^o",        0x0F);
  ASCII_MAP.set("^p",        0x10);
  ASCII_MAP.set("^q",        0x11);
  ASCII_MAP.set("^r",        0x12);
  ASCII_MAP.set("^s",        0x13);
  ASCII_MAP.set("^t",        0x14);
  ASCII_MAP.set("^u",        0x15);
  ASCII_MAP.set("^v",        0x16);
  ASCII_MAP.set("^w",        0x17);
  ASCII_MAP.set("^x",        0x18);
  ASCII_MAP.set("^y",        0x19);
  ASCII_MAP.set("^z",        0x1A);
  ASCII_MAP.set("^[",        0x1B);
  ASCII_MAP.set("Escape",    0x1B);
  ASCII_MAP.set("^\\",       0x1C);
  ASCII_MAP.set("^]",        0x1D);
  ASCII_MAP.set("^^",        0x1E);
  ASCII_MAP.set("^_",        0x1F);
  ASCII_MAP.set(" ",         0x20);
  ASCII_MAP.set("!",         0x21);
  ASCII_MAP.set("\"",        0x22);
  ASCII_MAP.set("#",         0x23);
  ASCII_MAP.set("$",         0x24);
  ASCII_MAP.set("%",         0x25);
  ASCII_MAP.set("&",         0x26);
  ASCII_MAP.set("'",         0x27);
  ASCII_MAP.set("(",         0x28);
  ASCII_MAP.set(")",         0x29);
  ASCII_MAP.set("*",         0x2A);
  ASCII_MAP.set("+",         0x2B);
  ASCII_MAP.set(",",         0x2C);
  ASCII_MAP.set("-",         0x2D);
  ASCII_MAP.set(".",         0x2E);
  ASCII_MAP.set("/",         0x2F);
  ASCII_MAP.set("0",         0x30);
  ASCII_MAP.set("1",         0x31);
  ASCII_MAP.set("2",         0x32);
  ASCII_MAP.set("3",         0x33);
  ASCII_MAP.set("4",         0x34);
  ASCII_MAP.set("5",         0x35);
  ASCII_MAP.set("6",         0x36);
  ASCII_MAP.set("7",         0x37);
  ASCII_MAP.set("8",         0x38);
  ASCII_MAP.set("9",         0x39);
  ASCII_MAP.set(":",         0x3A);
  ASCII_MAP.set(";",         0x3B);
  ASCII_MAP.set("<",         0x3C);
  ASCII_MAP.set("=",         0x3D);
  ASCII_MAP.set(">",         0x3E);
  ASCII_MAP.set("?",         0x3F);
  ASCII_MAP.set("@",         0x40);
  ASCII_MAP.set("A",         0x41);
  ASCII_MAP.set("B",         0x42);
  ASCII_MAP.set("C",         0x43);
  ASCII_MAP.set("D",         0x44);
  ASCII_MAP.set("E",         0x45);
  ASCII_MAP.set("F",         0x46);
  ASCII_MAP.set("G",         0x47);
  ASCII_MAP.set("H",         0x48);
  ASCII_MAP.set("I",         0x49);
  ASCII_MAP.set("J",         0x4A);
  ASCII_MAP.set("K",         0x4B);
  ASCII_MAP.set("L",         0x4C);
  ASCII_MAP.set("M",         0x4D);
  ASCII_MAP.set("N",         0x4E);
  ASCII_MAP.set("O",         0x4F);
  ASCII_MAP.set("P",         0x50);
  ASCII_MAP.set("Q",         0x51);
  ASCII_MAP.set("R",         0x52);
  ASCII_MAP.set("S",         0x53);
  ASCII_MAP.set("T",         0x54);
  ASCII_MAP.set("U",         0x55);
  ASCII_MAP.set("V",         0x56);
  ASCII_MAP.set("W",         0x57);
  ASCII_MAP.set("X",         0x58);
  ASCII_MAP.set("Y",         0x59);
  ASCII_MAP.set("Z",         0x5A);
  ASCII_MAP.set("[",         0x5B);
  ASCII_MAP.set("\\",        0x5C);
  ASCII_MAP.set("]",         0x5D);
  ASCII_MAP.set("^",         0x5E);
  ASCII_MAP.set("_",         0x5F);
  ASCII_MAP.set("`",         0x60);
  ASCII_MAP.set("a",         0x61);
  ASCII_MAP.set("b",         0x62);
  ASCII_MAP.set("c",         0x63);
  ASCII_MAP.set("d",         0x64);
  ASCII_MAP.set("e",         0x65);
  ASCII_MAP.set("f",         0x66);
  ASCII_MAP.set("g",         0x67);
  ASCII_MAP.set("h",         0x68);
  ASCII_MAP.set("i",         0x69);
  ASCII_MAP.set("j",         0x6A);
  ASCII_MAP.set("k",         0x6B);
  ASCII_MAP.set("l",         0x6C);
  ASCII_MAP.set("m",         0x6D);
  ASCII_MAP.set("n",         0x6E);
  ASCII_MAP.set("o",         0x6F);
  ASCII_MAP.set("p",         0x70);
  ASCII_MAP.set("q",         0x71);
  ASCII_MAP.set("r",         0x72);
  ASCII_MAP.set("s",         0x73);
  ASCII_MAP.set("t",         0x74);
  ASCII_MAP.set("u",         0x75);
  ASCII_MAP.set("v",         0x76);
  ASCII_MAP.set("w",         0x77);
  ASCII_MAP.set("x",         0x78);
  ASCII_MAP.set("y",         0x79);
  ASCII_MAP.set("z",         0x7A);
  ASCII_MAP.set("{",         0x7B);
  ASCII_MAP.set("|",         0x7C);
  ASCII_MAP.set("}",         0x7D);
  ASCII_MAP.set("~",         0x7E);
  ASCII_MAP.set("^?",        0x7F);
  ASCII_MAP.set("Delete",    0x7F);

  var _beepContext = null;
  var _buffer = [];
  var _cursor;
  var _fsSynced = false;
  var _keyBuffer = [];
  var _numRows;
  var _rowDivs = [];
  var _rowHeight;
  var _screen;
  var _terminal;
  var _terminalMode = false;

  function beep() {
    let beepSound = _beepContext.createOscillator();
    let beepGain = _beepContext.createGain();
    
    beepSound.type = "sine";
    beepSound.frequency.value = "587.33";
    beepGain.gain.value = 0.5;
    beepGain.gain.exponentialRampToValueAtTime(
      0.01,
      _beepContext.currentTime + 0.25
    );

    beepSound.connect(beepGain).connect(_beepContext.destination);
    beepSound.start();
    beepSound.stop(_beepContext.currentTime + 0.25);
  }
  
  function closeFiles() {
    _fsSynced = false;

    return new Promise((resolve, reject) => {
      FS.syncfs(false, (err) => {
        if (err) {
          reject(err);
        }
        else {
          _fsSynced = true;
          resolve(_fsSynced);
        }
      });
    });
  }

  function endwin() {
    _screen.remove();
    _terminal.style.cssText = "display:block;";
    _terminalMode = true;
    document.body.style.cssText = "overflow:visible;";
  }

  function exit(message) {
    let p = document.createElement("p");
    p.textContent = message;
    _terminal.append(p);
  }
  
  function filesSynced() {
    return _fsSynced;
  }

  function flushInputBuffer() {
    _keyBuffer.length = 0;
  }

  function getch(timeout) {
    return new Promise((resolve, reject) => {
      if (timeout >= 0) {
        setTimeout(() => {
          let ch = _keyBuffer.length ? _keyBuffer.shift() : ERR;
          resolve(ch);
        }, timeout)
      }
      else {
        let keyBufferCheckID = setInterval(() => {
          if (_keyBuffer.length) {
            clearInterval(keyBufferCheckID);
            resolve(_keyBuffer.shift());
          }
        });
      }
    });
  }

  function initFiles(path) {
    _fsSynced = false;
    FS.mkdir(path);
    FS.mount(IDBFS, {}, path);

    return new Promise((resolve, reject) => {
      FS.syncfs(true, (err) => {
        if (err) {
          reject(err);
        }
        else {
          _fsSynced = true;
          resolve(_fsSynced);
        }
      });
    });
  }

  function initscr(cols, rows) {
    _terminalMode = false;
    _terminal.style.cssText = "display:none;";
    _initEventHandlers();
    _createScreen(cols, rows);
  }

  function mvcur(row, col) {
    let top = "top:" + (row * _rowHeight) + "px;";
    let left = "left:" + col + "ch;";
    _cursor.style.cssText = "animation-name:none;";
    _cursor.offsetHeight;
    _cursor.style.cssText = top + left + "animation-name:blink;";
    _cursor.textContent = _buffer[row][col];
  }

  function print(text) {
    /* Calling print implies a \n from C/C++ */
    if (_terminalMode) {
      let br = document.createElement("br");
      _terminal.append(text);
      _terminal.append(br);
    }
    else {
      alert(text);
    }
  }

  function refresh() {
    let row = 0;
    while(row < _numRows) {
      _rowDivs[row].textContent = _buffer[row].join("");
      row++;
    }
  }

  function setCell(row, col, code) {
    _buffer[row][col] = (code === SPACE) ? "\xa0" : String.fromCharCode(code);
  }

  function _createScreen(cols, rows) {
    let col = 0,
        row = 0;

    _numRows = rows;

    _screen = document.createElement("div");
    _screen.id = SCREEN_ID;

    while (row < rows) {
      let rowDiv = document.createElement("div");
      rowDiv.id = row;
      _screen.append(rowDiv);
      _buffer[row] = [];
      _rowDivs[row] = rowDiv;
      while (col < cols) {
        _buffer[row][col] = "\xa0";
        col++;
      }
      col = 0;
      row++;
    }

    _cursor = document.createElement("div");
    _cursor.id = CURSOR_ID;
    _cursor.textContent = "\xa0";
    _screen.append(_cursor);

    document.body.append(_screen);

    /* Call refresh() to make sure all row divs are proper width and
       height before doing anything with them.  I suppose I could achieve
       this through css, but then I'd need to compute the height based on
       the font size being used.  This just seems easier and less
       error-prone. */
    refresh();

    /* Once refresh() executes, use clientHeight to set _rowHeight,
       the height of each row, and the cursor.  clientHeight is always a
       whole number and as long as the font size doesn't change after
       this point,it will give consistent spacing between rows and
       for vertical cursor movement regardless of zoom level. */
    _rowHeight = _cursor.clientHeight;
    
    row = 0;
    
    while (row < rows) {
      _rowDivs[row].style.cssText = "height:" + _rowHeight + "px;";
      row++;
    }

    _cursor.style.cssText = "height:" + _rowHeight + "px;";
    
    mvcur(0, 0);
  }

  function _initEventHandlers() {
    document.addEventListener("keydown", function(event) {
      /* Need user gesture to start an AudioContext */
      if (_beepContext === null) {
        _beepContext = new window.AudioContext();
      }
      
      let key = "";

      event.preventDefault();

      /* If modifier only, return */
      if ((event.key === "Alt") ||
          (event.key === "Control") ||
          (event.key === "Shift")) {
        return;
      }
      
      /* The only modifier key that has to be explicitly mapped is
         Ctrl because there aren't any ASCII codes that use Alt, and
         Shift is already accounted for with unique symbols (capital
         letters, +, _ etc.) */

      if (event.ctrlKey) {
        key += "^";
      }

      key += event.key;

      if (ASCII_MAP.has(key)) {
        _keyBuffer.push(ASCII_MAP.get(key));
      }
      else {
        console.log("key " + key + " doesn't map to ASCII code");
      }
    });

    /* Block reloading the page until the user confirms that's what
       they want to do (data could be lost).  To ensure the event
       fires in some browsers, the handler must set event.returnValue
       to a string and return a string. */
    
    window.addEventListener("beforeunload", function(event) {
      event.preventDefault();
      event.returnValue = "";
    });
  }

  function _initTerminal() {
    _terminal = document.createElement("div");
    _terminal.id = TERMINAL_ID;
    document.body.append(_terminal);
  }

  window.addEventListener("load", () => {
    _initTerminal();
  });

  return {
    beep:beep,
    closeFiles:closeFiles,
    endwin:endwin,
    exit:exit,
    filesSynced:filesSynced,
    flushInputBuffer:flushInputBuffer,
    getch:getch,
    initFiles:initFiles,
    initscr:initscr,
    mvcur:mvcur,
    print:print,
    refresh:refresh,
    setCell:setCell
  }
})();

Module["onExit"] = async () => {
  let message = "Thank you for playing the browser-based version of Umoria!  Refresh/reload the page to play again.";    
  await IO.closeFiles()
          .catch(err => {console.log("Error closing files");});
  IO.exit(message);
}

Module["onRuntimeInitialized"] = async () => {
  let root = "/umoria";
  let scoresPath = root + "/scores.dat";
  let savePath = root + "/game.sav";
  let args = window.location.search.substr(1).trim().split("%20");

  /* If no arguments were passed clear args array */
  if (!args[0]) {
    args = [];
  }

  await IO.initFiles(root)
          .catch(err => {console.log("Error initializing files");});
  ccall("setSavePath", null, ["string", "string"], [scoresPath, savePath]);
  callMain(args);
}

Module["print"] = (text) => {IO.print(text)};

Module["printErr"] = (text) => {alert(text)};
