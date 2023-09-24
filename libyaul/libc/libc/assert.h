#ifndef _LIB_ASSERT_H_
#define _LIB_ASSERT_H_

#include <sys/cdefs.h>

#include <stdbool.h>

__BEGIN_DECLS

#undef assert

#ifdef DEBUG
#define assert(e) do {                                                         \
    if (!(e)) {                                                                \
        _assert(__FILE__, __XSTRING(__LINE__), __function_name, __STRING(e));  \
    }                                                                          \
} while (false)
#else
#define assert(e)
#endif /* DEBUG */

#if !defined(__cplusplus)
#ifndef static_assert
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
#define static_assert _Static_assert
#else
#define static_assert(e)                                                       \
    typedef char __CONCAT(STATIC_ASSERT_FAILED_AT_LINE,                        \
        __CONCAT(_, __LINE__))[(e) ? 1 : -1]
#endif
#endif /* !static_assert */
#endif /* !defined(__cplusplus) */

extern void _assert(const char * __restrict file, const char * __restrict line,
  const char * __restrict func, const char * __restrict failed_expr) __noreturn;

__END_DECLS

#endif /* !_LIB_ASSERT_H_ */
