/*
 * Copyright (c) 2012 Israel Jacquez
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
        typedef char CC_CONCAT(STATIC_ASSERT_AT_LINE,                          \
                CC_CONCAT(_, __LINE__))[(exp) ? 1 : -1]
#endif /* __GNUC__ */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static __inline__ uint32_t
common_log2_down(uint32_t value)
{
        uint32_t l;

        l = 0;
        while ((value >> l) > 1)
                ++l;

        return l;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_COMMON_H_ */
