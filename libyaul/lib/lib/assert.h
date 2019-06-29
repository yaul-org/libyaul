#ifndef _ASSERT_H_
#define _ASSERT_H_

#include <sys/cdefs.h>

__BEGIN_DECLS

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

#if !defined(__cplusplus)
#ifndef static_assert
#define static_assert(__e)                                                     \
        typedef char __CONCAT(STATIC_ASSERT_FAILED_AT_LINE,                    \
                __CONCAT(_, __LINE__))[(__e) ? 1 : -1]
#endif /* !static_assert */
#endif /* !defined(__cplusplus) */

void __assert(const char *, int, const char *) __noreturn;
void __assert_func(const char *, int, const char *, const char *) __noreturn;

__END_DECLS

#endif /* !_ASSERT_H_ */
