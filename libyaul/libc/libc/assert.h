#ifndef _LIB_ASSERT_H_
#define _LIB_ASSERT_H_

#include <sys/cdefs.h>

__BEGIN_DECLS

#undef assert

#ifdef DEBUG
#define assert(e) ((e)                                                         \
    ? (void)0                                                                  \
    : _assert(__FILE__, __XSTRING(__LINE__), __function_name,                  \
        __STRING(e)))
#else
#define assert(e)
#endif

#if !defined(__cplusplus)
#ifndef static_assert
#define static_assert(__e)                                                     \
    typedef char __CONCAT(STATIC_ASSERT_FAILED_AT_LINE,                        \
        __CONCAT(_, __LINE__))[(__e) ? 1 : -1]
#endif /* !static_assert */
#endif /* !defined(__cplusplus) */

extern void _assert(const char * __restrict file, const char * __restrict line,
  const char * __restrict func, const char * __restrict failed_expr) __noreturn;

__END_DECLS

#endif /* !_LIB_ASSERT_H_ */
