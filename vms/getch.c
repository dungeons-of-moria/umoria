/* #define DEBUG	/* remove leading '/' and '*' to enable debugging */
/*
 *
 *	GETCH.C
 *
 *	input routines for VMS
 *	integrated with smcurses for VMS
 *
 *	Last Edit:
 *
 *	27-Jun-1989	jjd	added [no]crmode, [no]echo routines
 *	11-Feb-1990	jjd	adaptations for VMS umoria.
 *
 *	1986, 1987, 1988, 1989	Joshua Delahunty
 *				Grace Davis High School
 *				Modesto, CA
 *
 * int	opengetch()	- sets up terminal for getch() calls. returns VMS status
 * int	closegetch()	- opposite of above. returns VMS status of operation
 * char	_getch()	- returns an [optionally] unbuffered [non-]echoed
 *			  input character
 *
 * void	_echo()		- makes _getch() echo unbuffered characters
 * void	_noecho()	- _getch() does not echo unbuffered characters (default)
 *
 * void	crmode()	- _getch() returns unbuffered single chars (default)
 * void	nocrmode()	- _getch() returns echoed, buffered characters (getchar)
 *
 * void ungetch(c)	- saves character c, for the next call to _getch() for
 *			  input.  Sorry - only one character may be ungot.
 *
 */

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
	static unsigned short int	kb_chan = NULL,		/* channel # */
					charwaiting = FALSE,
					crmode_status = TRUE,
					echo_status = FALSE;

void crmode()	/* Character-Return MODE */
{
	if(kb_chan == NULL)
		opengetch();

	crmode_status = TRUE;
}

void nocrmode()	/* NO Character-Return MODE */
{
	if(kb_chan != NULL)
		closegetch();

	crmode_status = FALSE;
}

int opengetch()	/* does the actual assignment work */
{
	/* assign channel on keyboard */
	return(sys$assign(&chan,&kb_chan,0,0));
}

int closegetch()	/* performs the actual deassignment work */
{
	int rv;

	if(kb_chan != NULL) {
		/* deassign keyboard channel */
		rv = sys$dassgn(kb_chan);
		kb_chan = NULL;
		return(rv);
	}
}

void _echo()	/* turns ON echo of crmode-getch() characters */
{
	echo_status = TRUE;
}

void _noecho()	/* turns OFF echo of crmode-getch() characters */
{
	echo_status = FALSE;
}

char _getch()		/* grabs a character from the keyboard. */
			/*   returned immediately if crmode_status is set */
			/*     (otherwise, just calls for a buffered char) */
			/*   returns an ungot char if it exists */
			/* otherwise, returns a scanned character from the */
			/*   keyboard, optionally echoing it */
{
	int rv;
	char kb_buf;				/* buffer for input char */

#ifdef DEBUG
	putchar(':');
#endif
	if(crmode_status) {

		if(!charwaiting) {

			/* open channel if it hasn't been done already */
			if (kb_chan == NULL)
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

void _ungetch(c)
char c;
{
	ungotch = c;
	charwaiting = TRUE;
}
