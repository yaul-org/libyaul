/*
 * Copyright (c) 2012-2022
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 * Romulo Fernandes <abra185@gmail.com>
 */

#ifndef _YAUL_GAMEMATH_FIX16_H_
#error "Header file must not be directly included"
#endif /* !_YAUL_GAMEMATH_FIX16_H_ */

/// @addtogroup MATH_FIX16_VECTOR
/// @defgroup MATH_FIX16_VEC2 2-vector
/// @ingroup MATH_FIX16_VECTOR
/// @{

/// @brief Not yet documented.
///
/// @param x Not yet documented.
/// @param y Not yet documented.
#define FIX16_VEC2_INITIALIZER(x, y)                                           \
{                                                                              \
    {                                                                          \
        FIX16(x),                                                              \
        FIX16(y)                                                               \
    }                                                                          \
}

/// @brief Not yet documented.
///
/// @param x Not yet documented.
/// @param y Not yet documented.
#define FIX16_VEC2(x, y)                                                       \
((fix16_vec2_t){                                                               \
    {                                                                          \
        FIX16(x),                                                              \
        FIX16(y)                                                               \
    }                                                                          \
})

/// @param x Not yet documented.
typedef union fix16_vec2 {
    struct {
        /// @param x Not yet documented.
        fix16_t x;
        /// @param x Not yet documented.
        fix16_t y;
    };

    /// @param x Not yet documented.
    fix16_t comp[2];
} __packed __aligned(4) fix16_vec2_t;

/// @brief Not yet documented.
///
/// @param result Not yet documented.
static inline void __always_inline
fix16_vec2_zero(fix16_vec2_t *result)
{
    result->x = FIX16_ZERO;
    result->y = FIX16_ZERO;
}

/// @brief Not yet documented.
///
/// @param      v0     Not yet documented.
/// @param[out] result Not yet documented.
static inline void __always_inline
fix16_vec2_dup(const fix16_vec2_t * __restrict v0,
  fix16_vec2_t * __restrict result)
{
    result->x = v0->x;
    result->y = v0->y;
}

/// @brief Not yet documented.
///
/// @param      v0     Not yet documented.
/// @param      v1     Not yet documented.
/// @param[out] result Not yet documented.
static inline void __always_inline
fix16_vec2_add(const fix16_vec2_t * __restrict v0,
  const fix16_vec2_t * __restrict v1, fix16_vec2_t * __restrict result)
{
    result->x = v0->x + v1->x;
    result->y = v0->y + v1->y;
}

/// @brief Not yet documented.
///
/// @param      v1     Not yet documented.
/// @param      v0     Not yet documented.
/// @param[out] result Not yet documented.
static inline void __always_inline
fix16_vec2_sub(const fix16_vec2_t * __restrict v1,
  const fix16_vec2_t * __restrict v0, fix16_vec2_t * __restrict result)
{
    result->x = v1->x - v0->x;
    result->y = v1->y - v0->y;
}

/// @brief Not yet documented.
///
/// @param      scalar Not yet documented.
/// @param[out] result Not yet documented.
static inline void __always_inline
fix16_vec2_scale(const fix16_t scalar, fix16_vec2_t *result)
{
    result->x = fix16_mul(scalar, result->x);
    result->y = fix16_mul(scalar, result->y);
}

/// @brief Not yet documented.
///
/// @param      scalar Not yet documented.
/// @param      v0     Not yet documented.
/// @param[out] result Not yet documented.
static inline void __always_inline
fix16_vec2_scaled(const fix16_t scalar, const fix16_vec2_t * __restrict v0,
  fix16_vec2_t * __restrict result)
{
    result->x = fix16_mul(scalar, v0->x);
    result->y = fix16_mul(scalar, v0->y);
}

/// @brief Not yet documented.
///
/// @param a Not yet documented.
/// @param b Not yet documented.
///
/// @returns The value.
static inline fix16_t __always_inline
fix16_vec2_inline_dot(const fix16_vec2_t *a, const fix16_vec2_t *b)
{
    __register uint32_t aux0;
    __register uint32_t aux1;

    __declare_asm("\tclrmac\n"
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

/// @brief Not yet documented.
///
/// @param v0 Not yet documented.
extern fix16_t fix16_vec2_length(const fix16_vec2_t *v0);

/// @brief Not yet documented.
///
/// @param v0 Not yet documented.
extern fix16_t fix16_vec2_sqr_length(const fix16_vec2_t *v0);

/// @brief Not yet documented.
///
/// @param v0 Not yet documented.
extern void fix16_vec2_normalize(fix16_vec2_t *v0);

/// @brief Not yet documented.
///
/// @param      v0     Not yet documented.
/// @param[out] result Not yet documented.
extern void fix16_vec2_normalized(const fix16_vec2_t * __restrict v0, fix16_vec2_t * __restrict result);

/// @brief Not yet documented.
///
/// @param v0 Not yet documented.
/// @param v1 Not yet documented.
///
/// @returns The value.
extern fix16_t fix16_vec2_dot(const fix16_vec2_t *v0, const fix16_vec2_t *v1);

/// @brief Not yet documented.
///
/// @param      v0       Not yet documented.
/// @param[out] buffer   Not yet documented.
/// @param      decimals Not yet documunted.
///
/// @returns The string length, not counting the `NUL` character.
extern size_t fix16_vec2_str(const fix16_vec2_t *v0, char *buffer, int32_t decimals);

/// @}
