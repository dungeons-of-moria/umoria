/*
 * generic.h
 *
 * useful generic union type, used here and there
 *
 * Author:
 * Joseph Nathan Hall
 */

#ifndef _GENERIC_H_
#define _GENERIC_H_

typedef union {
	int	i;
	long	l;
	float	f;
	double	d;
	char	c;
	char	*s;
	char	**s_A;
	char	*v;
	} generic_t, *generic_Pt;

#define GEN_TYPE_NONE 0
#define GEN_TYPE_INT 1
#define GEN_TYPE_LONG 2
#define GEN_TYPE_FLOAT 3
#define GEN_TYPE_DOUBLE 4
#define GEN_TYPE_CHAR 5
#define GEN_TYPE_STRING 6
#define GEN_TYPE_STRING_A 7
#define GEN_TYPE_VOID_PTR 8

#endif /* _GENERIC_H_ */
