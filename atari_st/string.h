/* atari_st/string.h: string include file for MWC

   Copyright (c) 1989-92 Stephen A Jacobs, James E. Wilson

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

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
