/* vms/getch.c: input routines for VMS, integrated with smcurses for VMS

   Copyright (c) 1986-92 Joshua Delahunty, James E. Wilson

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include <iodef.h>
#include <ssdef.h>
#include <descrip.h>
#include <stdio.h>

#ifndef TRUE
# define TRUE 1
# define FALSE 0
#endif

/* the type of keyboard read we're doing */
#define FUNC IO$_TTYREADALL|IO$M_NOECHO|IO$M_TRMNOECHO

	static $DESCRIPTOR(chan, "tt:");
	static char ungotch;
	static unsigned short int kb_chan = 0;	/* channel # */
	static unsigned short int	charwaiting = FALSE,
					crmode_status = TRUE,
					echo_status = FALSE;

/* This code was tested and worked on a VAX 11/785 running VMS 5.2.
   contributed by Ralph Waters, rwaters@jabba.ess.harris.com.  */

/* Returns 1 is a character has been pressed, 0 otherwise.  */
int kbhit()
{
  /* sys$qiow ( [efn] ,chan ,func [,iosb] [,astadr] [,astprm]
		[,p1] [,p2] [,p3] [,p4] [,p5] [,p6] )			*/

  /* The sys$qiow call with the IO$_SENSEMODE|IO$M_TYPEAHDCNT function
     will return the following in p1:

     31        24 23        16 15                     0
     ------------|------------|------------------------
     | reserved  |   first    | number of characters  |
     |           | character  | in type-ahead buffer  |
     |-----------|------------|-----------------------|
     |                    reserved                    |
     |                                                |
     --------------------------------------------------
  */

  struct qio_return_type {
	 unsigned short int type_ahead_count;	/* type-ahead count */
	 unsigned char first_char;		/* first character in buffer */
	 unsigned char b_reserved;		/* reserved byte */
	 unsigned long int l_reserved; }	/* reserved long word */
    qio_return;

  sys$qiow (0, kb_chan, (IO$_SENSEMODE | IO$M_TYPEAHDCNT), 0, 0, 0,
	    &qio_return, 0, 0, 0, 0, 0);
  if (qio_return.type_ahead_count > 0)
    return(1);
  else
    return(0);
}

/* Another useful function courtesy of Ralph Waters.  */
#include <jpidef.h>

/* Stores the user's login name in the argument buf.  */
void user_name(buf)
char *buf;
{
  /* sys$getjpiw ( [efn], [pidadr], [prcnam], itmlst [,iosb]
                   [,astadr] [,astprm]					*/

  long int return_length;
  struct getjpi_itmlst_type {
    unsigned short int buffer_length;	/* length of return buffer */
    unsigned short int item_code;		/* item code to getjpi about */
    unsigned long int buffer_address;	/* address of return data */
    unsigned long int return_length_addr; }	/*actual size of return data */
  getjpi_itmlst;

  getjpi_itmlst.buffer_length = 12;	/* VMS usernames are 12 chars */
  getjpi_itmlst.item_code = JPI$_USERNAME;
  getjpi_itmlst.buffer_address = buf;
  getjpi_itmlst.return_length_addr = &return_length;

  sys$getjpiw (0, 0, 0, &getjpi_itmlst, 0, 0, 0);

  return;
}

/* After calling this, vms_getch() returns unbuffered single chars.  */
void vms_crmode()	/* Character-Return MODE */
{
	if(kb_chan == 0)
		opengetch();

	crmode_status = TRUE;
}

/* After calling this, vms_getch() returns echoed, buffered characters.  */
void vms_nocrmode()	/* NO Character-Return MODE */
{
	if(kb_chan != 0)
		closegetch();

	crmode_status = FALSE;
}

/* Sets up terminal for getch() calls, returns VMS status code.  */
int opengetch()	/* does the actual assignment work */
{
	/* assign channel on keyboard */
	return(sys$assign(&chan,&kb_chan,0,0));
}

/* Undoes affects of above, returns VMS status code for the operation.  */
int closegetch()	/* performs the actual deassignment work */
{
	int rv;

	if(kb_chan != 0) {
		/* deassign keyboard channel */
		rv = sys$dassgn(kb_chan);
		kb_chan = 0;
		return(rv);
	}
}

/* Returns an [optionally] unbuffered [non-]echoed input character.

   If crmode_status is not set, then the code returns one character from
   the buffered input.
   If crmode_status is set, then the code returns an `ungot' character if
   one exists, otherwise it tries to read one unbuffered character from the
   keyboard.  If echo_status is set, then the character will be echoed
   before returning.  */
char vms_getch()
{
	int rv;
	char kb_buf;				/* buffer for input char */

	if(crmode_status) {

		if(!charwaiting) {

			/* open channel if it hasn't been done already */
			if (kb_chan == 0)
				opengetch();

			/* que an i/o request for a character and wait */
			rv = sys$qiow(0,kb_chan,FUNC,0,0,0,&kb_buf,1,0,0,0,0);

			/* indicate if something is amiss */
			if(rv != SS$_NORMAL) {
				printf("?");
				exit(rv);
			}

		} else { /* charwaiting */

			kb_buf = ungotch;
			charwaiting = FALSE;
		}

		/* massage for getchar() compatibility */
		if (kb_buf == '\r') kb_buf='\n';

		/* echo char if we're supposed to */
		if (echo_status) putchar(kb_buf);

		return(kb_buf);

	} else	/* nocrmode */

		return(getchar());

}
