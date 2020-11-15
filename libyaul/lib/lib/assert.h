#ifndef _ASSERT_H_
#define _ASSERT_H_

#include <sys/cdefs.h>

__BEGIN_DECLS

#undef assert

#if defined(DEBUG) || (HAVE_ASSERT_SUPPORT == 1)
#define assert(e) ((e)                                                         \
        ? (void)0                                                              \
        : _assert(__FILE__, __XSTRING(__LINE__), __ASSERT_FUNC,                \
              __STRING(e)))
#else
#define assert(e)
#endif

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

extern void _assert(const char * __restrict, const char * __restrict,
    const char * __restrict, const char * __restrict) __noreturn;

__END_DECLS

#endif /* !_ASSERT_H_ */
