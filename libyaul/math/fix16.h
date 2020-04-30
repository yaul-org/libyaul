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

#ifndef _MATH_FIX16_H_
#define _MATH_FIX16_H_

#include <sys/cdefs.h>

#include <stdint.h>

__BEGIN_DECLS

#define FIXMATH_FUNC_ATTRS __attribute__ ((leaf, nothrow, const))

/* Macro for defining fix16_t constant values. The functions above can't be used
 * from e.g. global variable initializers, and their names are quite long also.
 * This macro is useful for constants springled alongside code, e.g.
 * FIX16(1.234).
 *
 * Note that the argument is evaluated multiple times, and also otherwise you
 * should only use this for constant values. For runtime-conversions, use the
 * functions above. */

#define FIX16(x) ((fix16_t)(((x) >= 0)                                         \
        ? ((x) * 65536.0f + 0.5f)                                              \
        : ((x) * 65536.0f - 0.5f)))

#define FIX16_MAX       (0x7FFFFFFF)
#define FIX16_MIN       (0x80000000)
#define FIX16_OVERFLOW  (0x80000000)

#define FIX16_PI        (0x0003243F)
#define FIX16_E         (0x0002B7E1)
#define FIX16_ONE       (0x00010000)

typedef int32_t fix16_t;

static inline fix16_t __always_inline
fix16_int32_from(int32_t value)
{
        return value * FIX16_ONE;
}

static inline int32_t __always_inline
fix16_int32_to(const fix16_t value)
{
        return value >> 16;
}

static inline int32_t __always_inline
fix16_round_int32_to(const fix16_t value)
{
        if (value >= 0) {
                return (value + (FIX16_ONE >> 1)) / FIX16_ONE;
        }

        return (value - (FIX16_ONE >> 1)) / FIX16_ONE;
}

static inline fix16_t __always_inline
fix16_integral(fix16_t value)
{
        return value & 0xFFFF0000;
}

static inline fix16_t __always_inline
fix16_fractional(fix16_t value)
{
        return value & 0x0000FFFF;
}

static inline fix16_t __always_inline
fix16_abs(fix16_t value)
{
        return ((value < 0) ? -value : value);
}

static inline fix16_t __always_inline
fix16_floor(fix16_t value)
{
        return (value & 0xFFFF0000UL);
}

static inline fix16_t __always_inline
fix16_ceil(fix16_t value)
{
        return (value & 0xFFFF0000UL) + ((value & 0x0000FFFFUL) ? FIX16_ONE : 0);
}

static inline fix16_t __always_inline
fix16_min(fix16_t x, fix16_t y)
{
        return (x < y ? x : y);
}

static inline fix16_t __always_inline
fix16_max(fix16_t x, fix16_t y)
{
        return (x > y ? x : y);
}

static inline fix16_t __always_inline
fix16_clamp(fix16_t value, fix16_t value_low, fix16_t value_high)
{
        return fix16_min(fix16_max(value, value_low), value_high);
}

static inline fix16_t __always_inline
fix16_rad_deg_to(fix16_t radians)
{
        static const fix16_t fix16_rad2deg = 3754936;

        return radians * fix16_rad2deg;
}

static inline fix16_t __always_inline
fix16_deg_rad_to(fix16_t degrees)
{
        static const fix16_t fix16_deg2rad = 1144;

        return degrees * fix16_deg2rad;
}

static inline uint32_t __always_inline
fix16_mul(const fix16_t a, const fix16_t b)
{
        register uint32_t mach;
        register fix16_t out;

        __asm__ volatile ("\tdmuls.l %[a], %[b]\n"
                          "\tsts mach, %[mach]\n"
                          "\tsts macl, %[out]\n"
                          "\nxtrct %[mach], %[out]"
            /* Output */
            : [mach] "=&r" (mach),
              [out] "=&r" (out)
            /* Input */
            : [a] "r" (a),
              [b] "r" (b)
            : "mach", "macl");

        return out;
}

extern fix16_t fix16_overflow_add(const fix16_t, const fix16_t) FIXMATH_FUNC_ATTRS;
extern fix16_t fix16_overflow_sub(const fix16_t, const fix16_t) FIXMATH_FUNC_ATTRS;

extern fix16_t fix16_lerp(const fix16_t, const fix16_t, const fix16_t) FIXMATH_FUNC_ATTRS;
extern fix16_t fix16_lerp8(const fix16_t, const fix16_t, const uint8_t) FIXMATH_FUNC_ATTRS;

extern fix16_t fix16_sqrt(const fix16_t) FIXMATH_FUNC_ATTRS;

extern void fix16_to_str(fix16_t, char *, int);

#include "fix16_trig.h"

#include "fix16_vec2.h"

#include "fix16_vec3.h"
#include "fix16_mat3.h"

#include "fix16_vec4.h"
#include "fix16_mat4.h"

#undef FIXMATH_FUNC_ATTRS

__END_DECLS

#endif /* !_MATH_FIX16_H_ */
