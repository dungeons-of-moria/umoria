/* This file is part of Umoria.

   Umoria is free software; you can redistribute it and/or modify 
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Umoria is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of 
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Genral Public License for more details.

   You should have received a copy of the GNU General Public License 
   along with Umoria.  If not, see <http://www.gnu.org/licenses/>. */

/* 
   NAME
     moriadecode.c

   COMMENTS
     encodes (i.e. xors) a decoded moria.save file

     this is a filter
     (reads in stdin, writes in stdout)

   AUTHOR
     Eric W. Bazin (currently bazin@carolus.cma.fr)

   HISTORY
     bazin - 17/10/1992: Created.
*/

#include <stdio.h>
#include <stdlib.h>



main(argc, argv)
     int argc;
     char *argv[];
{
  unsigned char c;
  unsigned char xor_byte;

/* version maj */
  (void) scanf("%c",&c);
  (void) printf("%c",c);
/* version min */
  (void) scanf("%c",&c);
  (void) printf("%c",c);
/* patch level */
  (void) scanf("%c",&c);
  (void) printf("%c",c);
/* xor_byte */
  (void) scanf("%c",&c);
  (void) printf("%c",c);
  xor_byte = c ;
/* rest of file */
  while(EOF != scanf("%c",&c))
    {
      xor_byte ^= c;
      (void) printf("%c",xor_byte);
    }
}
