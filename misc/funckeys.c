/* call e_inkey() in dungeon.c instead of inkey() */

int keypadon;

/* e_inkey
 * This inkey processes function keys also (using map_keypad)
 */

/* Gets single character from keyboard and returns a character.  If the key
 * pressed is a function key, it processes the key without waiting on the
 * escape.  At this time it processes function keys by using map_keypad and
 * needs funckeys.h.  If you don't want ANY escape processing, use inkey().
 * If you are using this function, Escape will need to be pressed twice to get
 * an escape.
 */

e_inkey(ch)
     char *ch;
{

  put_qio();			/* Dump IO buffer		 */
  if (!keypadon) {
    *ch = getch();
    return;
  }
  if ((*ch = getch()) != ESC)
    return;
  /* we now have an escape, if a [ does not follow it, send back a null */
  if ((*ch = getch()) != FCHAR ) {
    if (*ch == ESC)
      return;
    *ch = INPUT_ERROR;
    return;
  }
  map_keypad(ch);
}

/* map_keypad is sun specific still.  It should not be hard to change it for
 * any keypad.  It needs funckeys.h to work and is only called from e_inkey().
 * This seems to be too slow, and we are getting some processing errors that
 * look like short teleportation.
 */

map_keypad(ch)

     char *ch;
{
  int             c;
  int             key, rawkey, n;
  char            side;



#if SUN

  switch (c = getch()) {
  case 'A':
    side = 'r';
    key = 8;
    break;

  case 'B':
    side = 'r';
    key = 14;
    break;

  case 'C':
    side = 'r';
    key = 12;
    break;

  case 'D':
    side = 'r';
    key = 10;
    break;

  default:
    rawkey = c - '0';

    for (n = 0; c = getch(), n < 3; n++) {
      if (c == 'z')
	break;
      else if (c < '0' || c > '9') {
	*ch = INPUT_ERROR;
	return;
      } else
	rawkey = rawkey * 10 + c - '0';

    }
    if (c != 'z') {
      *ch = INPUT_ERROR;
      return;
    } else if (rawkey >= 208 && rawkey <= 222) {
      side = 'r';
      key = rawkey - 207;

    } else if (rawkey >= 192 && rawkey <= 201) {
      side = 'l';
      key = rawkey - 191;
    } else if (rawkey >= 224 && rawkey <= 232) {
      side = 'f';
      key = rawkey - 223;
    } else {
      *ch = INPUT_ERROR;
      return;
    }
    break;
  }
  if (side == 'r')
    *ch = rkeys[key];
  else if (side == 'l')
    *ch = lkeys[key];
  else if (side == 'f')
    *ch = fkeys[key];
  else
    *ch = INPUT_ERROR;
#endif
}

/* Prompts (optional) and returns False if ESC is the input character.  escp
 * is a boolean option that allows you to decide if you want function key
 * processing or not.
 *
 * If the prompt is NULL or empty, nothing is printed and nothing is erase.
 */
get_com(prompt, command, escp)
     char *prompt;
     char *command;
     int escp;
{
  int             com_val;
  int             res;
  int             do_erase;

  if ((prompt != NULL) && (strlen(prompt) > 0)) {
    oprint(prompt);
    do_erase = TRUE;
  } else
    do_erase = FALSE;

  if (escp)
    inkey(command);
  else
    e_inkey(command);
  com_val = *command;
  switch (com_val) {
  case ESC:
    res = FALSE;
    break;
  default:
    res = TRUE;
    break;
  }
  if (do_erase == TRUE)
    erase_line(msg_line, msg_line);
  return (res);
}
