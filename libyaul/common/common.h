/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _COMMON_H_
#define _COMMON_H_

#include <inttypes.h>

/*
 * A C preprocessing macro for concatenating to strings.
 *
 * We need use two macros (CC_CONCAT and CC_CONCAT_S) in order to allow
 * concatenation of two defined macros.
 */
#define CC_CONCAT_S(s1, s2)     s1 ## s2
#define CC_CONCAT(s1, s2)       CC_CONCAT_S(s1, s2)

#ifdef __GNUC__
#define STATIC_ASSERT(exp)                                                     \
        typedef char CC_CONCAT(STATIC_ASSERT_FAILED_AT_LINE,                   \
                CC_CONCAT(_, __LINE__))[(exp) ? 1 : -1]
#endif /* __GNUC__ */

#ifndef __aligned
#define __aligned(x)    __attribute__ ((aligned((x))))
#endif /* !__aligned */

#ifndef __always_inline
#define __always_inline __attribute__ ((always_inline))
#endif /* !__always_inline */

#define __may_alias     __attribute__ ((__may_alias__))
#define __noreturn      __attribute__ ((noreturn))

#ifndef __packed
#define __packed        __attribute__ ((packed))
#endif /* !__packed */

#ifndef __section
#define __section(x)    __attribute__ ((section((x))))
#endif /* !__section */

#ifndef __unused
#define __unused        __attribute__ ((unused))
#endif /* !__unused */

#ifndef __used
#define __used          __attribute__ ((used))
#endif /* !__used */

#define __visibility(x) __attribute__ ((visibility((x))))

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static __inline uint32_t
common_log2_down(uint32_t value)
{
        uint32_t l;

        l = 0;
        while ((value >> l) > 1) {
                ++l;
        }

        return l;
}

static __inline uint32_t
common_round_pow2(uint32_t value)
{
        value--;
        value |= value >> 1;
        value |= value >> 2;
        value |= value >> 4;
        value |= value >> 8;
        value |= value >> 16;
        value++;
        return value;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_COMMON_H_ */
