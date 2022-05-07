#ifndef _LIB_STDLIB_H_
#define _LIB_STDLIB_H_

#include <sys/cdefs.h>

#include <stddef.h>

__BEGIN_DECLS

#define ATEXIT_MAX (32)

extern int abs(int number);
extern long labs(long number);

extern int atoi(const char *s);
extern long atol(const char *s);

unsigned long strtoul(const char *__restrict, char **__restrict, int);
long strtol(const char * __restrict nptr, char ** __restrict endptr, int base);

extern void *malloc(size_t n);
extern void *memalign(size_t n, size_t align);
extern void *realloc(void *old, size_t new_len);
extern void free(void *addr);

extern void abort(void) __noreturn;
extern int atexit(void (*function)(void));
extern void exit(int code) __noreturn;

__END_DECLS

#endif /* _LIB_STDLIB_H_ */
