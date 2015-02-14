/* atari_st/string.h: string include file for MWC

   Copyright (c) 1989-94 Stephen A Jacobs, James E. Wilson

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

#ifndef STRING_H
#define STRING_H

char *index();
char *rindex();
char *strcat();
int strcmp();
char *strcpy();
int strlen();
char *strncat();
int strncmp();
char *strncpy();
char *memchr();
int memcmp();
char *memcpy();
char *memset();
char *strchr();
unsigned int strcspn();
char *strerror();
char *strpbrk();
char *strrchr();
unsigned int strspn();
char *strstr();
char *strtok();

#endif
