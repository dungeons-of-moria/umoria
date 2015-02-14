/* vms/uexit.c: special exit command, to exit cleanly from VMS program.

   Copyright (c) 1990-94 Joshua Delahunty, James E. Wilson

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

/*	Unix programs usually return 0 for "good execution",
	and 1 for "some problem occured."  In VMS, we WANT the
	program to return a 1, or else we get an ugly
	"%NONAME-W-NOMSG, Message number 00000000"
	message upon image exit (yeech!).

	So, we convert 0 exit codes to 1's, leaving all others alone.  */

void uexit(exitcode)
int exitcode;
{
	if (exitcode == 0)	/* On Unix, clean, on VMS, yeech! */
		exit(1);	/* converted... */
	else
		exit(exitcode); /* returned unscathed... */
}
