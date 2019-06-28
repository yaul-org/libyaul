#ifndef _STDLIB_H_
#define _STDLIB_H_

#include "_ansi.h"

#include <sys/cdefs.h>

#include <stddef.h>

_BEGIN_STD_C

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#define RAND_MAX __RAND_MAX

void abort(void) __noreturn;
int abs(int);
int atexit(void (*__func)(void));
int atoi(const char *nptr);
void exit(int status) __noreturn;
void free(void *) _NOTHROW;
void *malloc(size_t size) _NOTHROW;
int rand(void);
void srand(unsigned seed);

_END_STD_C

#endif /* _STDLIB_H_ */
