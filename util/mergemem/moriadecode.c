/* 
   NAME
     moriadecode.c

   COMMENTS
     decodes (i.e. xors) a moria.save file

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
      (void) printf("%c",c ^ xor_byte);
      xor_byte = c;
    }
}
