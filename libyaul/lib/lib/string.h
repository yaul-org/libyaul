#ifndef _STRING_H_
#define _STRING_H_

#include "_ansi.h"

#include <sys/cdefs.h>

#include <stddef.h>

_BEGIN_STD_C

void *memchr(const void *, int, size_t);
int memcmp(const void *, const void *, size_t);
void *memcpy(void * __restrict, const void * __restrict, size_t);
void *memmove(void *, const void *, size_t);
void *memset(void *, int, size_t);
char *strcat(char * __restrict, const char * __restrict);
char *strchr(const char *, int);
int strcmp(const char *, const char *);
char *strcpy(char * __restrict, const char * __restrict);
size_t strcspn(const char *, const char *);
size_t strlen(const char *);
size_t strnlen(const char *, size_t);
char *strncat(char * __restrict, const char * __restrict, size_t);
int strncmp(const char *, const char *, size_t);
char *strncpy(char * __restrict, const char * __restrict, size_t);
char *strpbrk(const char *, const char *);
char *strrchr(const char *, int);
size_t strspn(const char *, const char *);
char *strstr(const char *, const char *);
char *strtok(char * __restrict, const char * __restrict);
char *strndup(const char *, size_t);

_END_STD_C

#endif /* _STRING_H_ */
