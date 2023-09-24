/*-
 * Copyright (c) Authors of libfixmath
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _YAUL_GAMEMATH_FIX16_H_
#define _YAUL_GAMEMATH_FIX16_H_

#include <sys/cdefs.h>

#include <stdint.h>

#include <gamemath/defs.h>

__BEGIN_DECLS

/// @defgroup MATH_FIX16 Fix16
/// @ingroup MATH

/// @defgroup MATH_FIX16_VECTOR Fix16 Vector
/// @ingroup MATH

/// @defgroup MATH_FIX16_MATRIX Fix16 Matrix
/// @ingroup MATH

/// @defgroup MATH_FIX16_PLANES Fix16 Planes
/// @ingroup MATH

/// @defgroup MATH_FIX16_TRIG Fix16 Trigonometry
/// @ingroup MATH

/// @addtogroup MATH_FIX16
/// @{

/// @brief Macro for defininge @p fix16_t constant values.
///
/// @note The argument is evaluated multiple times, and also otherwise you
/// should only use this for constant values. For runtime-conversions, use the
/// inlined functions below.
///
/// @param x The constant value.
#define FIX16(x) ((fix16_t)(((x) >= 0)                                         \
    ? ((x) * 65536.0f + 0.5f)                                                  \
    : ((x) * 65536.0f - 0.5f)))

/// @brief Maximum positive value.
#define FIX16_MAX       (0x7FFFFFFF)
/// @brief Minimum (negative) value
#define FIX16_MIN       (0x80000000)
/// @brief Value to indicate an overflow has occured.
#define FIX16_OVERFLOW  (0x80000000)

/// @brief Q16.16 representation of 2π.
#define FIX16_2PI       (0x00064881)
/// @brief Q16.16 representation of π.
#define FIX16_PI        (0x00032440)
/// @brief Q16.16 representation of π/2.
#define FIX16_PI_2      (0x00019220)
/// @brief Q16.16 representation of π/4.
#define FIX16_PI_4      (0x0000C910)
/// @brief Q16.16 representation of the exponential constant.
#define FIX16_E         (0x0002B7E1)
/// @brief Q16.16 representation of zero.
#define FIX16_ZERO      (0x00000000)
/// @brief Q16.16 representation of one.
#define FIX16_ONE       (0x00010000)
/// @brief Q16.16 representation of converting radians to degrees.
#define FIX16_RAD2DEG   (0x00394BB8)
/// @brief Q16.16 representation of converting degrees to radians.
#define FIX16_DEG2RAD   (0x00000478)

/// brief Fixed point Q16.16.
typedef int32_t fix16_t;

/// @brief Not yet documented.
///
/// @param a Operand.
/// @param b Operand.
///
/// @returns The value.
__BEGIN_ASM
static inline fix16_t __always_inline
fix16_int16_mul(const fix16_t a, const int16_t b)
{
    __register fix16_t out;

    __declare_asm("\tdmuls.l %[a], %[b]\n"
                  "\tsts macl, %[out]\n"
                  /* Output */
                  : [out] "=&r" (out)
                  /* Input */
                  : [a] "r" (a),
                    [b] "r" (b)
                  : "macl");

    return out;
}
__END_ASM

/// @brief Not yet documented.
///
/// @param a Operand.
/// @param b Operand.
///
/// @returns The value.
__BEGIN_ASM
static inline int32_t __always_inline
fix16_int32_mul(const fix16_t a, const fix16_t b)
{
    __register int16_t out;

    __declare_asm("\tdmuls.l %[a], %[b]\n"
                  "\tsts mach, %[out]\n"
                  /* Output */
                  : [out] "=&r" (out)
                  /* Input */
                  : [a] "r" (a),
                    [b] "r" (b)
                  : "mach");

    return out;
}
__END_ASM

/// @brief Not yet documented.
///
/// @param a Operand.
/// @param b Operand.
///
/// @returns The value.
__BEGIN_ASM
static inline fix16_t __always_inline
fix16_mul(const fix16_t a, const fix16_t b)
{
    __register uint32_t mach;
    __register fix16_t out;

    __declare_asm("\tdmuls.l %[a], %[b]\n"
                  "\tsts mach, %[mach]\n"
                  "\tsts macl, %[out]\n"
                  "\txtrct %[mach], %[out]\n"
                  /* Output */
                  : [mach] "=&r" (mach),
                    [out] "=&r" (out)
                  /* Input */
                  : [a] "r" (a),
                    [b] "r" (b)
                  : "mach", "macl");

    return out;
}
__END_ASM

/// @brief Not yet documented.
///
/// @param value Operand.
///
/// @returns The value.
static inline fix16_t __always_inline
fix16_int32_from(int32_t value)
{
    return (value * FIX16_ONE);
}

/// @brief Not yet documented.
///
/// @param value Operand.
///
/// @returns The value.
static inline int32_t __always_inline
fix16_int32_to(const fix16_t value)
{
    return (value >> 16);
}

/// @brief Not yet documented.
///
/// @param value Operand.
///
/// @returns The value.
static inline int32_t __always_inline
fix16_round_int32_to(const fix16_t value)
{
    if (value >= 0) {
        return ((value + (FIX16_ONE >> 1)) / FIX16_ONE);
    }

    return ((value - (FIX16_ONE >> 1)) / FIX16_ONE);
}

/// @brief Not yet documented.
///
/// @param value Operand.
///
/// @returns The value.
static inline fix16_t __always_inline
fix16_integral(const fix16_t value)
{
    return (value & 0xFFFF0000);
}

/// @brief Not yet documented.
///
/// @param value Operand.
///
/// @returns The value.
static inline fix16_t __always_inline
fix16_fractional(const fix16_t value)
{
    return (value & 0x0000FFFF);
}

/// @brief Not yet documented.
///
/// @param value Operand.
///
/// @returns The value.
static inline fix16_t __always_inline
fix16_abs(const fix16_t value)
{
    return ((value < 0) ? -value : value);
}

/// @brief Not yet documented.
///
/// @param value Operand.
///
/// @returns The value.
static inline fix16_t __always_inline
fix16_sign(const fix16_t value)
{
    return ((value < 0) ? -FIX16_ONE : FIX16_ONE);
}

/// @brief Not yet documented.
///
/// @param value Operand.
///
/// @returns The value.
static inline fix16_t __always_inline
fix16_floor(const fix16_t value)
{
    return (value & 0xFFFF0000UL);
}

/// @brief Not yet documented.
///
/// @param value Operand.
///
/// @returns The value.
static inline fix16_t __always_inline
fix16_ceil(const fix16_t value)
{
    return (value & 0xFFFF0000UL) + ((value & 0x0000FFFFUL) ? FIX16_ONE : 0);
}

/// @brief Not yet documented.
///
/// @param x Operand.
/// @param y Operand.
///
/// @returns The value.
static inline fix16_t __always_inline
fix16_min(const fix16_t x, const fix16_t y)
{
    return min(x, y);
}

/// @brief Not yet documented.
///
/// @param x Operand.
/// @param y Operand.
///
/// @returns The value.
static inline fix16_t __always_inline
fix16_max(const fix16_t x, const fix16_t y)
{
    return max(x, y);
}

/// @brief Not yet documented.
///
/// @param value      Operand.
/// @param value_low  Operand.
/// @param value_high Operand.
///
/// @returns The value.
static inline fix16_t __always_inline
fix16_clamp(fix16_t value, fix16_t value_low, fix16_t value_high)
{
    return fix16_min(fix16_max(value, value_low), value_high);
}

/// @brief Not yet documented.
///
/// @param radians Operand.
///
/// @returns The value.
static inline fix16_t __always_inline
fix16_rad_deg_to(fix16_t radians)
{
    return fix16_mul(radians, FIX16_RAD2DEG);
}

/// @brief Not yet documented.
///
/// @param degrees Operand.
///
/// @returns The value.
static inline fix16_t __always_inline
fix16_deg_rad_to(fix16_t degrees)
{
    return fix16_mul(degrees, FIX16_DEG2RAD);
}

/// @brief Not yet documented.
///
/// @param a Operand.
/// @param b Operand.
///
/// @returns The value.
extern fix16_t fix16_overflow_add(fix16_t a, fix16_t b);

/// @brief Not yet documented.
///
/// @param a Operand.
/// @param b Operand.
///
/// @returns The value.
extern fix16_t fix16_overflow_sub(fix16_t a, fix16_t b);

/// @brief Not yet documented.
///
/// @param dividend Operand.
/// @param divisor  Operand.
///
/// @returns The value.
extern fix16_t fix16_div(fix16_t dividend, fix16_t divisor);

/// @brief Not yet documented.
///
/// @param a Operand.
/// @param b Operand.
/// @param t Operand.
///
/// @returns The value.
extern fix16_t fix16_lerp(fix16_t a, fix16_t b, fix16_t t);

/// @brief Not yet documented.
///
/// @param a Operand.
/// @param b Operand.
/// @param t Operand.
///
/// @returns The value.
extern fix16_t fix16_lerp8(fix16_t a, fix16_t b, const uint8_t t);

/// @brief Not yet documented.
///
/// @param value Operand.
///
/// @returns The value.
extern fix16_t fix16_sqrt(fix16_t value);

/// @brief Not yet documented.
///
/// @param      value    Not yet documented.
/// @param[out] buffer   Not yet documented.
/// @param      decimals Not yet documented.
///
/// @returns The string length, not counting the `NUL` character.
extern size_t fix16_str(fix16_t value, char *buffer, int32_t decimals);

/// @}

__END_DECLS

#endif /* !_YAUL_GAMEMATH_FIX16_H_ */
