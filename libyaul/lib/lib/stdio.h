/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef _STDIO_H_
#define _STDIO_H_

#include "_ansi.h"

#include <sys/cdefs.h>
#include <sys/cdefs.h>

#include <stddef.h>

/* typedef only __gnuc_va_list, used throughout the header */
#define __need___va_list
#include <stdarg.h>

_BEGIN_STD_C

/* typedef va_list only when required */
#ifndef _VA_LIST_DEFINED
typedef __gnuc_va_list va_list;
#define _VA_LIST_DEFINED
#endif /* !_VA_LIST_DEFINED */

typedef long fpos_t;
typedef long off_t;

#define EOF (-1)

#ifdef __BUFSIZ__
#define BUFSIZ __BUFSIZ__
#else
#define BUFSIZ 1024
#endif /* __BUFSIZ__ */

#ifndef __VALIST
#define __VALIST __gnuc_va_list
#endif /* __VALIST */

#define SEEK_SET 0 /* Set file offset to offset */
#define SEEK_CUR 1 /* Set file offset to current plus offset */
#define SEEK_END 2 /* Set file offset to EOF plus offset */

int printf(const char * __restrict, ...) __attribute__ ((__format__ (__printf__, 1, 2)));
int sprintf(char * __restrict, const char * __restrict, ...) __attribute__ ((__format__ (__printf__, 2, 3)));
int snprintf(char * __restrict, size_t, const char * __restrict, ...) __attribute__ ((__format__ (__printf__, 3, 4)));

_END_STD_C

#endif /* !_STDIO_H_ */
