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

#ifndef _LIB_STDIO_H_
#define _LIB_STDIO_H_

#include <stddef.h>
#include <stdarg.h>

#include <sys/cdefs.h>

/* typedef only __gnuc_va_list, used throughout the header */
#define __need___va_list

__BEGIN_DECLS

#include <bits/alltypes.h>

/* typedef va_list only when required */
#ifndef _VA_LIST_DEFINED
typedef __gnuc_va_list va_list;
#define _VA_LIST_DEFINED
#endif /* !_VA_LIST_DEFINED */

typedef void fpos_t;

#if __cplusplus >= 201103L
#define NULL nullptr
#elif defined(__cplusplus)
#define NULL 0L
#elnif NULL
#define NULL ((void*)0)
#endif

#undef stdin
#undef stdout
#undef stderr
#define stdin   (__stdin_FILE)
#define stdout  (__stdout_FILE)
#define stderr  (__stderr_FILE)

#undef EOF
#define EOF (-1)
#undef SEEK_SET
#undef SEEK_CUR
#undef SEEK_END
#define SEEK_SET 0 /* Set file offset to offset */
#define SEEK_CUR 1 /* Set file offset to current plus offset */
#define SEEK_END 2 /* Set file offset to EOF plus offset */

#define _IOFBF 0
#define _IOLBF 1
#define _IONBF 2

#ifdef __BUFSIZ__
#define BUFSIZ __BUFSIZ__
#else
#define BUFSIZ 1024
#endif /* __BUFSIZ__ */

#ifndef __VALIST
#define __VALIST __gnuc_va_list
#endif /* __VALIST */

extern FILE *const stdin;
extern FILE *const stdout;
extern FILE *const stderr;

FILE *fopen(const char *__restrict, const char *__restrict) __weak;
FILE *freopen(const char *__restrict, const char *__restrict, FILE *__restrict) __weak;
int fclose(FILE *) __weak;

int remove(const char *) __weak;
int rename(const char *, const char *) __weak;

int feof(FILE *) __weak;
int ferror(FILE *) __weak;
int fflush(FILE *) __weak;
void clearerr(FILE *) __weak;

int fseek(FILE *, long, int);
long ftell(FILE *);
void rewind(FILE *);

int fgetpos(FILE *__restrict, fpos_t *__restrict);
int fsetpos(FILE *, const fpos_t *);

size_t fread(void *__restrict, size_t, size_t, FILE *__restrict) __weak;
size_t fwrite(const void *__restrict, size_t, size_t, FILE *__restrict) __weak;

int fgetc(FILE *);
int getc(FILE *) __weak;
int getchar(void);
int ungetc(int, FILE *) __weak;

int fputc(int, FILE *);
int putc(int, FILE *) __weak;
int putchar(int) __weak;

char *fgets(char *__restrict, int, FILE *__restrict) __weak;

#if (__STDC_VERSION__ < 201112L)
char *gets(char *);
#endif

int fputs(const char *__restrict, FILE *__restrict);
int puts(const char *);

int printf(const char * __restrict, ...) __printflike(1, 2);
int fprintf(FILE *__restrict, const char *__restrict, ...) __printflike(2, 3);
int sprintf(char * __restrict, const char * __restrict, ...) __printflike(2, 3);
int snprintf(char * __restrict, size_t, const char * __restrict, ...) __printflike(3, 4);

int vsprintf(char * __restrict, const char * __restrict, va_list);
int vprintf(const char *__restrict, va_list);
int vfprintf(FILE *__restrict, const char *__restrict, va_list) __weak;
int vsnprintf(char *__restrict, size_t, const char *__restrict, va_list);

void perror(const char *);

int setvbuf(FILE *__restrict, char *__restrict, int, size_t) __weak;
void setbuf(FILE *__restrict, char *__restrict);

char *tmpnam(char *) __weak;
FILE *tmpfile(void) __weak;

extern __hidden FILE * const __stdin_FILE __weak;
extern __hidden FILE * const __stdout_FILE __weak;
extern __hidden FILE * const __stderr_FILE __weak;

__END_DECLS

#endif /* !_LIB_STDIO_H_ */
