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
     moriamerge.c

   COMMENTS
     usage : moriamerge file1 file2

     file1 : decoded moria.save
     file2 : decoded moria.save

     output : stdout : merge file2 monster memory
              into file1.

   AUTHOR
     Eric W. Bazin (currently bazin@carolus.cma.fr)

   HISTORY
     bazin - 17/10/1992: Created.
*/

#include <stdio.h>
#include <stdlib.h>


void rotate(s)
     unsigned short * s ;
{
  unsigned char c1,c2;

  c2 = (unsigned char) (*s % 256);
  c1 = (unsigned char) (*s >> 8);

  *s = (unsigned short) c2;
  *s = *s << 8;
  *s += (unsigned short) c1;
}

main(argc, argv)
     int argc;
     char *argv[];
{
  FILE * file1;
  FILE * file2;

  unsigned char c1[20];
  unsigned char c2[20];

  unsigned short m1;   /* monster number */
  unsigned short m2;
  unsigned short *k1;  /* kills */
  unsigned short *k2;
  unsigned short *d1;  /* death */
  unsigned short *d2;

  int i;

  if(argc != 3)
    {
      (void) fprintf(stderr,"Error : need 2 input files\n");
      exit(1);
    }
  if(!(file1=fopen(argv[1],"r")))
    {
      (void) fprintf(stderr,"Error : can't open %s\n",argv[1]);
      exit(1);
    }
  if(!(file2=fopen(argv[2],"r")))
    {
      (void) fprintf(stderr,"Error : can't open %s\n",argv[2]);
      exit(1);
    }

  
/* headers */
  (void)fread(c1,1,4,file1);
  (void)fread(c2,1,4,file2);
  (void)fwrite(c1,1,4,stdout);

/* monster memories */
  (void)fread(&m1,2,1,file1); rotate(&m1);
  (void)fread(&m2,2,1,file2); rotate(&m2);
  while (1) {
    if(m1 == 0xffff)
      {
	while (m2 != 0xffff)
	  {
	    rotate(&m2); (void)fwrite(&m2,2,1,stdout);
	    (void)fread(c2,1,20,file2);
	    (void)fwrite(c2,1,20,stdout);
	    (void)fread(&m2,2,1,file2); rotate(&m2);
	  }
	goto rest;
      }
    else if (m2 == 0xffff)
      {
	while (m1 != 0xffff)
	  {
	    rotate(&m1); (void)fwrite(&m1,2,1,stdout);
	    (void)fread(c1,1,20,file1);
	    (void)fwrite(c1,1,20,stdout);
	    (void)fread(&m1,2,1,file1); rotate(&m1);
	  }
	goto rest;
      }
    else 
      {
	if (m1 < m2)
	  {
	    rotate(&m1); (void)fwrite(&m1,2,1,stdout);
	    (void)fread(c1,1,20,file1);
	    (void)fwrite(c1,1,20,stdout);
	    (void)fread(&m1,2,1,file1); rotate(&m1);
	  }
	else if (m2 < m1)
	  {
	    rotate(&m2); (void)fwrite(&m2,2,1,stdout);
	    (void)fread(c2,1,20,file2);
	    (void)fwrite(c2,1,20,stdout);
	    (void)fread(&m2,2,1,file2); rotate(&m2);
	  }
	else
	  {
	    rotate(&m1); (void)fwrite(&m1,2,1,stdout);
	    (void)fread(c1,1,20,file1);
	    (void)fread(c2,1,20,file2);
	    for (i = 0; i < 8; i++)
	      {
		c1[i] |= c2[i];
	      }
	    k1 = (unsigned short *) (&(c1[8]));
	    k2 = (unsigned short *) (&(c2[8]));
	    d1 = (unsigned short *) (&(c1[10]));
	    d2 = (unsigned short *) (&(c2[10]));
	    rotate(k1);
	    rotate(k2);
	    rotate(d1);
	    rotate(d2);
	    *k1 += *k2;
	    *d1 += *d2;
	    rotate(k1);
	    rotate(d1);
	    for (i = 12; i < 20; i++)
	      {
		c1[i] |= c2[i];
	      }
	    (void)fwrite(c1,1,20,stdout);
	    (void)fread(&m1,2,1,file1); rotate(&m1);
	    (void)fread(&m2,2,1,file2); rotate(&m2);
	  }
      }
  }
 rest:
  (void)fwrite(&m1,2,1,stdout);
  while( EOF != fscanf(file1,"%c",c1) ) { printf("%c",c1[0]); }
}

