#ifndef _MATH_FIX16_H_
#define _MATH_FIX16_H_

#include <sys/cdefs.h>

#include <stdint.h>

__BEGIN_DECLS

#define FIXMATH_FUNC_ATTRS __attribute__ ((leaf, nothrow, const))

/* Macro for defining fix16_t constant values. The functions above can't be used
 * from e.g. global variable initializers, and their names are quite long also.
 * This macro is useful for constants springled alongside code, e.g. FIX16(1.234).
 *
 * Note that the argument is evaluated multiple times, and also otherwise you
 * should only use this for constant values. For runtime-conversions, use the
 * functions above. */

#define FIX16(x) ((fix16_t)(((x) >= 0)                                         \
        ? ((x) * 65536.0f + 0.5f)                                              \
        : ((x) * 65536.0f - 0.5f)))

typedef int32_t fix16_t;

static const fix16_t FOUR_DIV_PI  = 0x145F3;            /*!< Fix16 value of 4/PI */
static const fix16_t _FOUR_DIV_PI2 = 0xFFFF9840;        /*!< Fix16 value of -4/PI² */
static const fix16_t X4_CORRECTION_COMPONENT = 0x399A;  /*!< Fix16 value of 0.225 */
static const fix16_t PI_DIV_4 = 0x0000C90F;             /*!< Fix16 value of PI/4 */
static const fix16_t THREE_PI_DIV_4 = 0x00025B2F;       /*!< Fix16 value of 3PI/4 */

static const fix16_t fix16_maximum  = 0x7FFFFFFF; /*!< the maximum value of fix16_t */
static const fix16_t fix16_minimum  = 0x80000000; /*!< the minimum value of fix16_t */
static const fix16_t fix16_overflow = 0x80000000; /*!< the value used to indicate overflows when FIXMATH_NO_OVERFLOW is not specified */

static const fix16_t FIX16_PI  = 205887;
static const fix16_t FIX16_E   = 178145;
static const fix16_t FIX16_ONE = 0x00010000;

static inline fix16_t __always_inline
fix16_int32_from(int32_t value) {
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
fix16_add(fix16_t a, fix16_t b)
{
        return a + b;
}

static inline fix16_t __always_inline
fix16_sub(fix16_t a, fix16_t b)
{
        return a - b;
}

static inline fix16_t __always_inline
fix16_mul(fix16_t a, fix16_t b)
{
        return a * b;
}

static inline fix16_t __always_inline
fix16_div(fix16_t a __unused, fix16_t b __unused)
{
        return FIX16_ONE;
}

static inline fix16_t __always_inline
fix16_mod(fix16_t a __unused, fix16_t b __unused)
{
        return FIX16_ONE;
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

extern fix16_t fix16_overflow_add(fix16_t, fix16_t) FIXMATH_FUNC_ATTRS;
extern fix16_t fix16_overflow_sub(fix16_t, fix16_t) FIXMATH_FUNC_ATTRS;

extern fix16_t fix16_lerp(fix16_t, fix16_t, fix16_t) FIXMATH_FUNC_ATTRS;
extern fix16_t fix16_lerp8(fix16_t, fix16_t, uint8_t) FIXMATH_FUNC_ATTRS;

extern fix16_t fix16_sin(fix16_t) FIXMATH_FUNC_ATTRS;
extern fix16_t fix16_cos(fix16_t) FIXMATH_FUNC_ATTRS;
extern fix16_t fix16_tan(fix16_t) FIXMATH_FUNC_ATTRS;

extern fix16_t fix16_asin(fix16_t) FIXMATH_FUNC_ATTRS;
extern fix16_t fix16_acos(fix16_t) FIXMATH_FUNC_ATTRS;
extern fix16_t fix16_atan(fix16_t) FIXMATH_FUNC_ATTRS;
extern fix16_t fix16_atan2(fix16_t, fix16_t) FIXMATH_FUNC_ATTRS;

extern fix16_t fix16_sqrt(fix16_t) FIXMATH_FUNC_ATTRS;
extern fix16_t fix16_log2(fix16_t) FIXMATH_FUNC_ATTRS;

extern void fix16_to_str(fix16_t, char *, int);

#include "fix16_vec2.h"

#include "fix16_vec3.h"
#include "fix16_mat3.h"

#include "fix16_vec4.h"
#include "fix16_mat4.h"

__END_DECLS

#endif /* !_MATH_FIX16_H_ */
