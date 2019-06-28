#ifndef _ASSERT_H_
#define _ASSERT_H_

#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "_ansi.h"

#undef assert

#define assert(__e) ((__e) ? (void)0 : __assert_func(__FILE__, __LINE__,       \
        __ASSERT_FUNC, #__e))

#ifndef __ASSERT_FUNC
/* Use g++'s demangled names in C++ */
#if defined(__cplusplus) && defined(__GNUC__)
#define __ASSERT_FUNC __PRETTY_FUNCTION__
/* C99 requires the use of __func__ */
#elif __STDC_VERSION__ >= 199901L
#define __ASSERT_FUNC __func__
/* Failed to detect __func__ support */
#else
#define __ASSERT_FUNC ((char *)0)
#endif
#endif /* !__ASSERT_FUNC */

void __assert(const char *, int, const char *) __noreturn;
void __assert_func(const char *, int, const char *, const char *) __noreturn;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_ASSERT_H_ */
