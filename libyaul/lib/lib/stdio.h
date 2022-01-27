/*-
 * Copyright (c) 2005-2014 Rich Felker, et al.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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

#define UNGET 8

#define F_PERM  1
#define F_NORD  4
#define F_NOWR  8
#define F_EOF   16
#define F_ERR   32
#define F_SVB   64
#define F_APP   128

struct _IO_FILE {
        int fd;

        unsigned int flags;

        unsigned char *rpos;
        unsigned char *rend;

        unsigned char *wpos;
        unsigned char *wend;

        unsigned char *wbase;

        size_t (*read)(FILE *, unsigned char *, size_t);
        size_t (*write)(FILE *, const unsigned char *, size_t);
        off_t (*seek)(FILE *, off_t, int);
        int (*close)(FILE *);

        unsigned char *buf;
        size_t buf_size;

        void *cookie;

        FILE *prev;
        FILE *next;
};

extern FILE * const stdin;
extern FILE * const stdout;
extern FILE * const stderr;

FILE *fopen(const char *__restrict, const char *__restrict) __weak;
FILE *freopen(const char *__restrict, const char *__restrict, FILE *__restrict);
int fclose(FILE *);

int remove(const char *);
int rename(const char *, const char *);

int feof(FILE *);
int ferror(FILE *);
int fflush(FILE *);
void clearerr(FILE *);

int fseek(FILE *, long, int);
long ftell(FILE *);
void rewind(FILE *);

int fgetpos(FILE *__restrict, fpos_t *__restrict);
int fsetpos(FILE *, const fpos_t *);

size_t fread(void *__restrict, size_t, size_t, FILE *__restrict);
size_t fwrite(const void *__restrict, size_t, size_t, FILE *__restrict);

int fgetc(FILE *);
int getc(FILE *);
int getchar(void);
int ungetc(int, FILE *);

int fputc(int, FILE *);
int putc(int, FILE *);
int putchar(int);

char *fgets(char *__restrict, int, FILE *__restrict);

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
int vfprintf(FILE *__restrict, const char *__restrict, va_list);
int vsnprintf(char *__restrict, size_t, const char *__restrict, va_list);

void perror(const char *);

int setvbuf(FILE *__restrict, char *__restrict, int, size_t);
void setbuf(FILE *__restrict, char *__restrict);

char *tmpnam(char *);
FILE *tmpfile(void);

extern __hidden FILE __stdin_FILE;
extern __hidden FILE __stdout_FILE;
extern __hidden FILE __stderr_FILE;

__END_DECLS

#endif /* !_LIB_STDIO_H_ */
