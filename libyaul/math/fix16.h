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

#ifndef _YAUL_MATH_FIX16_H_
#define _YAUL_MATH_FIX16_H_

#include <sys/cdefs.h>

#include <stdint.h>

__BEGIN_DECLS

/// @addtogroup MATH
/// @{

#define FIXMATH_FUNC_ATTRS __attribute__ ((leaf, nothrow, const))

/* Macro for defining fix16_t constant values. The functions above can't be used
 * from e.g. global variable initializers, and their names are quite long also.
 * This macro is useful for constants springled alongside code, e.g.
 * FIX16(1.234).
 *
 * Note that the argument is evaluated multiple times, and also otherwise you
 * should only use this for constant values. For runtime-conversions, use the
 * inlined functions below */

#define FIX16(x) ((fix16_t)(((x) >= 0)                                         \
        ? ((x) * 65536.0f + 0.5f)                                              \
        : ((x) * 65536.0f - 0.5f)))

#define FIX16_MAX       (0x7FFFFFFF)
#define FIX16_MIN       (0x80000000)
#define FIX16_OVERFLOW  (0x80000000)

#define FIX16_2PI       (0x00064881)
#define FIX16_PI        (0x00032440)
#define FIX16_PI_2      (0x00019220)
#define FIX16_PI_4      (0x0000C910)
#define FIX16_E         (0x0002B7E1)
#define FIX16_ONE       (0x00010000)
#define FIX16_RAD2DEG   (0x00394BB8)
#define FIX16_DEG2RAD   (0x00000478)

/// Not yet documented.
typedef int32_t fix16_t;

static inline fix16_t __always_inline
fix16_int16_mul(const fix16_t a, const int16_t b)
{
        register fix16_t out;

        __asm__ volatile ("\tdmuls.l %[a], %[b]\n"
                          "\tsts macl, %[out]\n"
            /* Output */
            : [out] "=&r" (out)
            /* Input */
            : [a] "r" (a),
              [b] "r" (b)
            : "macl");

        return out;
}

static inline int16_t __always_inline
fix16_int16_muls(const fix16_t a, const fix16_t b)
{
        register int16_t out;

        __asm__ volatile ("\tdmuls.l %[a], %[b]\n"
                          "\tsts mach, %[out]\n"
            /* Output */
            : [out] "=&r" (out)
            /* Input */
            : [a] "r" (a),
              [b] "r" (b)
            : "mach");

        return out;
}

static inline fix16_t __always_inline
fix16_mul(const fix16_t a, const fix16_t b)
{
        register uint32_t mach;
        register fix16_t out;

        __asm__ volatile ("\tdmuls.l %[a], %[b]\n"
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

static inline fix16_t __always_inline
fix16_int32_from(int32_t value)
{
        return (value * FIX16_ONE);
}

static inline int32_t __always_inline
fix16_int32_to(const fix16_t value)
{
        return (value >> 16);
}

static inline int32_t __always_inline
fix16_round_int32_to(const fix16_t value)
{
        if (value >= 0) {
                return ((value + (FIX16_ONE >> 1)) / FIX16_ONE);
        }

        return ((value - (FIX16_ONE >> 1)) / FIX16_ONE);
}

static inline fix16_t __always_inline
fix16_integral(const fix16_t value)
{
        return (value & 0xFFFF0000);
}

static inline fix16_t __always_inline
fix16_fractional(const fix16_t value)
{
        return (value & 0x0000FFFF);
}

static inline fix16_t __always_inline
fix16_abs(const fix16_t value)
{
        return ((value < 0) ? -value : value);
}

static inline fix16_t __always_inline
fix16_sign(const fix16_t value)
{
        return ((value < 0) ? -FIX16_ONE : FIX16_ONE);
}

static inline fix16_t __always_inline
fix16_floor(const fix16_t value)
{
        return (value & 0xFFFF0000UL);
}

static inline fix16_t __always_inline
fix16_ceil(const fix16_t value)
{
        return (value & 0xFFFF0000UL) + ((value & 0x0000FFFFUL) ? FIX16_ONE : 0);
}

static inline fix16_t __always_inline
fix16_min(const fix16_t x, const fix16_t y)
{
        return ((x < y) ? x : y);
}

static inline fix16_t __always_inline
fix16_max(const fix16_t x, const fix16_t y)
{
        return ((x > y) ? x : y);
}

static inline fix16_t __always_inline
fix16_clamp(fix16_t value, fix16_t value_low, fix16_t value_high)
{
        return fix16_min(fix16_max(value, value_low), value_high);
}

static inline fix16_t __always_inline
fix16_rad_deg_to(fix16_t radians)
{
        return fix16_mul(radians, FIX16_RAD2DEG);
}

static inline fix16_t __always_inline
fix16_deg_rad_to(fix16_t degrees)
{
        return fix16_mul(degrees, FIX16_DEG2RAD);
}

extern fix16_t fix16_overflow_add(const fix16_t, const fix16_t) FIXMATH_FUNC_ATTRS;
extern fix16_t fix16_overflow_sub(const fix16_t, const fix16_t) FIXMATH_FUNC_ATTRS;

extern fix16_t fix16_lerp(const fix16_t, const fix16_t, const fix16_t) FIXMATH_FUNC_ATTRS;
extern fix16_t fix16_lerp8(const fix16_t, const fix16_t, const uint8_t) FIXMATH_FUNC_ATTRS;

extern fix16_t fix16_sqrt(const fix16_t) FIXMATH_FUNC_ATTRS;

extern uint32_t fix16_str(fix16_t, char *, int);

#define _FIX16_INCLUDE_ONCE

#include "fix16_trig.h"

#include "fix16_vec2.h"
#include "fix16_vec3.h"

#include "fix16_mat3.h"

#include "fix16_plane.h"

#undef _FIX16_INCLUDE_ONCE

#undef FIXMATH_FUNC_ATTRS

/// @}

__END_DECLS

#endif /* !_YAUL_MATH_FIX16_H_ */
