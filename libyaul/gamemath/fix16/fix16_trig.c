/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdbool.h>

#include <cpu/divu.h>

#include <gamemath/fix16/fix16_trig.h>

#include "fix16_sin.inc"

typedef uint32_t brad_t;

static inline brad_t __always_inline
_convert(angle_t angle)
{
    /* Shifting by (16 - shift) is equivalent to:
     * ((angle * FIX16_LUT_SIN_TABLE_COUNT) / 65536.0) */
    return (((uint32_t)angle >> (16 - FIX16_LUT_SIN_TABLE_SHIFT)) + FIX16_LUT_SIN_TABLE_COUNT);
}

static inline fix16_t __always_inline
_sin(brad_t bradians)
{
    return _lut_brads_sin[bradians & (FIX16_LUT_SIN_TABLE_COUNT - 1)];
}

static inline fix16_t __always_inline
_cos(brad_t bradians)
{
    /* Shift by (π/2),
     *    angle_t: 0x4000,
     *   bradians: (FIX16_LUT_SIN_TABLE_COUNT / 4) = π/2 */
    return _lut_brads_sin[(bradians + (FIX16_LUT_SIN_TABLE_COUNT / 4)) & (FIX16_LUT_SIN_TABLE_COUNT - 1)];
}

fix16_t
fix16_sin(angle_t angle)
{
    return _sin(_convert(angle));
}

fix16_t
fix16_cos(angle_t angle)
{
    return _cos(_convert(angle));
}

void
fix16_sincos(angle_t angle, fix16_t *result_sin, fix16_t *result_cos)
{
    const brad_t bradians = _convert(angle);

    *result_sin = _sin(bradians);
    *result_cos = _cos(bradians);
}

fix16_t
fix16_tan(const angle_t angle)
{
    fix16_t cos;
    fix16_t sin;

    fix16_sincos(angle, &sin, &cos);

    cpu_divu_fix16_set(FIX16(1.0), cos);

    const fix16_t quotient = cpu_divu_quotient_get();
    const fix16_t result = fix16_mul(sin, quotient);

    return result;
}

angle_t
fix16_atan2(fix16_t y, fix16_t x)
{
    if (y == FIX16(0.0)) {
        return ((x >= FIX16(0.0)) ? RAD2ANGLE(0.0f) : RAD2ANGLE(M_PI));
    }

    angle_t phi;
    phi = RAD2ANGLE(0.0f);

    if (y < FIX16(0.0)) {
        x = -x;
        y = -y;
        phi = RAD2ANGLE(M_PI);
    }

    if (x <= FIX16(0.0)) {
        const fix16_t t = -x;

        x = y;
        y = t;
        phi += RAD2ANGLE(M_PI * 0.5f);
    }

    if (x <= y) {
        const fix16_t t = y - x;

        x = x + y;
        y = t;
        phi += RAD2ANGLE(M_PI * 0.25f);
    }

    cpu_divu_fix16_set(y, x);

    const fix16_t q = cpu_divu_quotient_get();

    /* Perform effectively dphi=atan(q), but instead convert to angle_t.
     *
     * Since q ranges from [-1,1], this can be scaled to π/4. However an
     * extra step is taken to convert to angle_t: Dividing by 2π:
     * q*((π/4)/(2π)) */
    const angle_t dphi = fix16_int32_mul(q, FIX16(0.125f));

    return (phi + dphi);
}
