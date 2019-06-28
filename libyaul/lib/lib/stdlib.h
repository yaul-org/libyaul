#ifndef _STDLIB_H_
#define _STDLIB_H_

#include <sys/cdefs.h>

#include <stddef.h>

__BEGIN_DECLS

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

void abort(void) __noreturn;
int abs(int);
int atexit(void (*)(void));
int atoi(const char *);
void exit(int) __noreturn;
void free(void *);
void *malloc(size_t);
int rand(void);
void srand(unsigned);

__END_DECLS

#endif /* _STDLIB_H_ */
