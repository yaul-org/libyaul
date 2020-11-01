/*-
 * Copyright (c) 1991, 1993
 * The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _STDIO_H_
#define _STDIO_H_

#include <sys/cdefs.h>

#include <stddef.h>

/* typedef only __gnuc_va_list, used throughout the header */
#define __need___va_list

#include <stdarg.h>

__BEGIN_DECLS

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

int printf(const char * __restrict, ...) __printflike(1, 2);
int sprintf(char * __restrict, const char * __restrict, ...) __printflike(2, 3);
int vsprintf(char * __restrict, const char * __restrict, va_list);
int snprintf(char * __restrict, size_t, const char * __restrict, ...) __printflike(3, 4);

__END_DECLS

#endif /* !_STDIO_H_ */
