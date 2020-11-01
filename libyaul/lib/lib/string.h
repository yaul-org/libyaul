#ifndef _STRING_H_
#define _STRING_H_

#include <sys/cdefs.h>

#include <stddef.h>

__BEGIN_DECLS

extern int bcmp(const void *, const void *, size_t);
extern void bcopy(const void *, void *, size_t);
extern void bzero(void *, size_t);
extern char *index(const char *, int);
extern char *rindex(const char *, int);

extern void *memcpy(void *__restrict, const void * __restrict, size_t);
extern void *memmove(void *, const void *, size_t);
extern void *memset(void *, int, size_t);
extern int memcmp(const void *, const void *, size_t);
extern void *memchr(const void *, int, size_t);

extern char *strcpy(char * __restrict, const char * __restrict);
extern char *strncpy(char * __restrict, const char * __restrict, size_t);

extern char *strcat(char * __restrict, const char * __restrict);
extern char *strncat(char * __restrict, const char * __restrict, size_t);

extern int strcmp(const char *, const char *);
extern int stricmp(const char *, const char *);
extern int strncmp(const char *, const char *, size_t);
extern int strncasecmp(const char *, const char *, size_t);

extern char *strchr(const char *, int);
extern char *strrchr(const char *, int);

extern size_t strcspn(const char *, const char *);
extern size_t strspn(const char *, const char *);
extern char *strpbrk(const char *, const char *);
extern char *strstr(const char *, const char *);
extern char *strtok(char * __restrict, const char * __restrict);

extern size_t strlen(const char *);

extern char *strtok_r(char * __restrict, const char * __restrict, char ** __restrict);
extern char *stpcpy(char * __restrict, const char * __restrict);
extern char *stpncpy(char * __restrict, const char * __restrict, size_t);
extern size_t strnlen(const char *, size_t);
extern char *strdup(const char *);
extern char *strndup(const char *, size_t);

extern void *memccpy(void * __restrict, const void * __restrict, int, size_t);

extern char *strsep(char **, const char *);
extern size_t strlcat(char *, const char *, size_t);
extern size_t strlcpy(char *, const char *, size_t);

extern int strverscmp(const char *, const char *);
extern char *strchrnul(const char *, int);
extern char *strcasestr(const char *, const char *);
extern void *memmem(const void *, size_t, const void *, size_t);
extern void *memrchr(const void *, int, size_t);
extern void *mempcpy(void *, const void *, size_t);

__END_DECLS

#endif /* _STRING_H_ */
