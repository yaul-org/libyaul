/*
 * See LICENSE for details.
 */

#ifndef _CPU_ENDIAN_H_
#define _CPU_ENDIAN_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define htobe16(x)      ((uint16_t)(x))
#define htole16(x)      bswap16((x))
#define be16toh(x)      ((uint16_t)(x))
#define le16toh(x)      bswap16((x))

#define htobe32(x)      ((uint32_t)(x))
#define htole32(x)      bswap32((x))
#define be32toh(x)      ((uint32_t)(x))

#define le32toh(x)      bswap32((x))

/* Via __builtin_bswap32() and __builtin_bswap16(), GCC emits a function
 * with no use of the swap instructions */

static inline uint32_t __attribute__ ((always_inline))
bswap16(uint32_t x)
{
        register uint32_t out;

        __asm__ volatile ("swap.b %[in], %[out]\n"
                : [out] "=&r" (out)
                : [in] "r" (x));

        return out;
}

static inline uint32_t __attribute__ ((always_inline))
bswap32(uint32_t x)
{
        register uint32_t out;

        register uint32_t t0;
        t0 = 0;
        register uint32_t t1;
        t1 = 0;

        __asm__ volatile ("swap.b %[in], %[ot0]\n"
                          "swap.w %[it0], %[ot1]\n"
                          "swap.b %[it1], %[out]\n"
                : [out] "=&r" (out), [ot0] "=r" (t0), [ot1] "=r" (t1)
                : [in] "0" "r" (x), [it0] "1" (t0), [it1] "2" (t1));

        return out;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_CPU_ENDIAN_H_ */
