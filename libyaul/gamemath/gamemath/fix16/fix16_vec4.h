/*
 * Copyright (c) 2012-2022
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 * Romulo Fernandes <abra185@gmail.com>
 */

#ifndef _YAUL_GAMEMATH_FIX16_VEC4_H_
#define _YAUL_GAMEMATH_FIX16_VEC4_H_

#include <gamemath/fix16.h>

/// @addtogroup MATH_FIX16_VECTOR
/// @defgroup MATH_FIX16_VEC4 4-vector
/// @ingroup MATH_FIX16_VECTOR
/// @{

/// @brief Not yet documented.
///
/// @param x Not yet documented.
/// @param y Not yet documented.
/// @param z Not yet documented.
/// @param w Not yet documented.
#define FIX16_VEC4_INITIALIZER(x, y, z, w)                                     \
{                                                                              \
    {                                                                          \
        FIX16(x),                                                              \
        FIX16(y),                                                              \
        FIX16(z),                                                              \
        FIX16(w),                                                              \
    }                                                                          \
}

/// @brief Not yet documented.
///
/// @param x Not yet documented.
/// @param y Not yet documented.
/// @param z Not yet documented.
/// @param w Not yet documented.
#define FIX16_VEC4(x, y, z, w)                                                 \
((fix16_vec4_t){                                                               \
    {                                                                          \
        FIX16(x),                                                              \
        FIX16(y),                                                              \
        FIX16(z),                                                              \
        FIX16(w),                                                              \
    }                                                                          \
})

/// @brief Not yet documented.
typedef union fix16_vec4 {
    struct {
        /// @brief Not yet documented.
        fix16_t x;
        /// @brief Not yet documented.
        fix16_t y;
        /// @brief Not yet documented.
        fix16_t z;
        /// @brief Not yet documented.
        fix16_t w;
    };

    /// @brief Not yet documented.
    fix16_t comp[4];
} __packed __aligned(4) fix16_vec4_t;

/// @brief Not yet documented.
///
/// @param result Not yet documented.
static inline void __always_inline
fix16_vec4_zero(fix16_vec4_t *result)
{
    result->x = FIX16_ZERO;
    result->y = FIX16_ZERO;
    result->z = FIX16_ZERO;
    result->w = FIX16_ZERO;
}

/// @brief Not yet documented.
///
/// @param[out] result Not yet documented.
/// @param      x      Not yet documented.
/// @param      y      Not yet documented.
/// @param      z      Not yet documented.
/// @param      w      Not yet documented.
static inline void __always_inline
fix16_vec4_set(fix16_vec4_t *result, fix16_t x, fix16_t y, fix16_t z, fix16_t w)
{
    result->x = x;
    result->y = y;
    result->z = z;
    result->w = w;
}

/// @brief Not yet documented.
///
/// @param      v0     Not yet documented.
/// @param[out] result Not yet documented.
static inline void __always_inline
fix16_vec4_dup(const fix16_vec4_t * __restrict v0,
  fix16_vec4_t * __restrict result)
{
    result->x = v0->x;
    result->y = v0->y;
    result->z = v0->z;
    result->w = v0->w;
}

/// @brief Not yet documented.
///
/// @param      v0     Not yet documented.
/// @param      v1     Not yet documented.
/// @param[out] result Not yet documented.
static inline void __always_inline
fix16_vec4_add(const fix16_vec4_t * __restrict v0,
  const fix16_vec4_t * __restrict v1, fix16_vec4_t * __restrict result)
{
    result->x = v0->x + v1->x;
    result->y = v0->y + v1->y;
    result->z = v0->z + v1->z;
    result->w = v0->w + v1->w;
}

/// @brief Not yet documented.
///
/// @param      v1     Not yet documented.
/// @param      v0     Not yet documented.
/// @param[out] result Not yet documented.
static inline void __always_inline
fix16_vec4_sub(const fix16_vec4_t * __restrict v1,
  const fix16_vec4_t * __restrict const v0, fix16_vec4_t * __restrict const result)
{
    result->x = v1->x - v0->x;
    result->y = v1->y - v0->y;
    result->z = v1->z - v0->z;
    result->w = v1->w - v0->w;
}

/// @brief Not yet documented.
///
/// @param      scalar Not yet documented.
/// @param[out] result Not yet documented.
static inline void __always_inline
fix16_vec4_scale(const fix16_t scalar, fix16_vec4_t *result)
{
    result->x = fix16_mul(scalar, result->x);
    result->y = fix16_mul(scalar, result->y);
    result->z = fix16_mul(scalar, result->z);
    result->w = fix16_mul(scalar, result->w);
}

/// @brief Not yet documented.
///
/// @param      scalar Not yet documented.
/// @param      v0     Not yet documented.
/// @param[out] result Not yet documented.
static inline void __always_inline
fix16_vec4_scaled(const fix16_t scalar, const fix16_vec4_t * __restrict v0,
  fix16_vec4_t * __restrict result)
{
    result->x = fix16_mul(scalar, v0->x);
    result->y = fix16_mul(scalar, v0->y);
    result->z = fix16_mul(scalar, v0->z);
    result->w = fix16_mul(scalar, v0->w);
}

/// @brief Not yet documented.
///
/// @param a Not yet documented.
/// @param b Not yet documented.
///
/// @returns The value.
__BEGIN_ASM
static inline fix16_t __always_inline
fix16_vec4_inline_dot(const fix16_vec4_t *a, const fix16_vec4_t *b)
{
    __register uint32_t aux0;
    __register uint32_t aux1;

    __declare_asm("\tclrmac\n"
                  "\tmac.l @%[a]+, @%[b]+\n"
                  "\tmac.l @%[a]+, @%[b]+\n"
                  "\tmac.l @%[a]+, @%[b]+\n"
                  "\tmac.l @%[a]+, @%[b]+\n"
                  "\tsts mach, %[aux0]\n"
                  "\tsts macl, %[aux1]\n"
                  "\txtrct %[aux0], %[aux1]\n"
                  : [a] "+r" (a),
                    [b] "+r" (b),
                    [aux0] "=&r" (aux0),
                    [aux1] "=&r" (aux1)
                  : "m" (*a),
                    "m" (*b)
                  : "mach", "macl", "memory");

    return aux1;
}
__END_ASM

/// @brief Not yet documented.
///
/// @param v0 Not yet documented.
///
/// @returns The value.
extern fix16_t fix16_vec4_length(const fix16_vec4_t *v0);

/// @brief Not yet documented.
///
/// @param v0 Not yet documented.
///
/// @returns The value.
extern fix16_t fix16_vec4_sqr_length(const fix16_vec4_t *v0);

/// @brief Not yet documented.
///
/// @param v0 Not yet documented.
extern void fix16_vec4_normalize(fix16_vec4_t *v0);

/// @brief Not yet documented.
///
/// @param      v0     Not yet documented.
/// @param[out] result Not yet documented.
extern void fix16_vec4_normalized(const fix16_vec4_t * __restrict v0,
  fix16_vec4_t * __restrict result);

/// @brief Not yet documented.
///
/// @param v0 Not yet documented.
/// @param v1 Not yet documented.
///
/// @returns The value.
extern fix16_t fix16_vec4_dot(const fix16_vec4_t *v0, const fix16_vec4_t *v1);

/// @brief Not yet documented.
///
/// @param      v0       Not yet documented.
/// @param[out] buffer   Not yet documented.
/// @param      decimals Not yet documunted.
///
/// @returns The string length, not counting the `NUL` character.
extern size_t fix16_vec4_str(const fix16_vec4_t *v0, char *buffer, int32_t decimals);

/// @}

#endif /* !_YAUL_GAMEMATH_FIX16_VEC4_H_ */
