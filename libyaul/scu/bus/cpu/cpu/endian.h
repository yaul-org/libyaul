/*
 * See LICENSE for details.
 */

#ifndef _YAUL_CPU_ENDIAN_H_
#define _YAUL_CPU_ENDIAN_H_

#include <stdint.h>

__BEGIN_DECLS

/// @defgroup CPU_ENDIANNESS CPU Endianness

/// @addtogroup CPU_ENDIANNESS
/// @{

/// @brief Convert from host byte order to big-endian order.
#define cpu_htobe16(x)  ((uint16_t)(x))
/// @brief Convert from host byte order to little-endian order.
#define cpu_htole16(x)  cpu_bswap16((x))
/// @brief Convert from big-endian order to host byte order.
#define cpu_be16toh(x)  ((uint16_t)(x))
/// @brief Convert from little-endian order to host byte order.
#define cpu_le16toh(x)  cpu_bswap16((x))

/// @brief Convert from host byte order to big-endian order.
#define cpu_htobe32(x)  ((uint32_t)(x))
/// @brief Convert from host byte order to little-endian order.
#define cpu_htole32(x)  cpu_bswap32((x))
/// @brief Convert from big-endian order to host byte order.
#define cpu_be32toh(x)  ((uint32_t)(x))

/// @brief Convert from little-endian order to host byte order.
#define cpu_le32toh(x)  cpu_bswap32((x))

/// @brief Swap the upper and lower bytes of a 2-byte value.
///
/// @details The `__builtin_bswap16()` function that GCC emits does not use the
/// `swap.b` instruction.
///
/// @param x Value to be swapped.
///
/// @returns Swapped value.
static inline uint16_t __always_inline
cpu_bswap16(uint16_t x)
{
        register uint32_t out;

        __asm__ volatile ("swap.b %[in], %[out]\n"
                : [out] "=&r" (out)
                : [in] "r" (x));

        return out;
}

/// @brief Swap the upper and lower 2-bytes of a 4-byte value.
///
/// @details The `__builtin_bswap32()` function that GCC emits does not use the
/// `swap.w` instruction.
///
/// @param x Value to be swapped.
///
/// @returns Swapped value.
static inline uint32_t __always_inline
cpu_bswap32(uint32_t x)
{
        register uint32_t out;

        register uint32_t t0;
        t0 = 0;
        register uint32_t t1;
        t1 = 0;

        __asm__ volatile ("swap.b %[in], %[ot0]\n"
                          "swap.w %[it0], %[ot1]\n"
                          "swap.b %[it1], %[out]\n"
                : [out] "=&r" (out),
                  [ot0] "=r" (t0),
                  [ot1] "=r" (t1)
                : [in] "0" "r" (x),
                  [it0] "1" (t0),
                  [it1] "2" (t1));

        return out;
}

/// @}

__END_DECLS

#endif /* !_YAUL_CPU_ENDIAN_H_ */
