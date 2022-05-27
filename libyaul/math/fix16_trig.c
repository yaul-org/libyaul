/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdbool.h>

#include <cpu/divu.h>

#include "math.h"
#include "fix16.h"

#define _FIX16_TRIG_INCLUDE_ONCE
#include "fix16_sin.inc"
#include "fix16_atan.inc"
#undef _FIX16_TRIG_INCLUDE_ONCE

static inline int32_t __always_inline
_rad2brad_convert(fix16_t radians)
{
        const fix16_t converted =
            fix16_mul(radians, FIX16(FIX16_LUT_SIN_TABLE_COUNT / (2.0f * M_PI)));

        return (fix16_int32_to(converted) + FIX16_LUT_SIN_TABLE_COUNT);
}

fix16_t
fix16_sin(fix16_t radians)
{
        return fix16_bradians_sin(_rad2brad_convert(radians));
}

fix16_t
fix16_cos(fix16_t radians)
{
        return fix16_bradians_cos(_rad2brad_convert(radians));
}

void
fix16_sincos(fix16_t radians, fix16_t *result_sin, fix16_t *result_cos)
{
        const int32_t bradians = _rad2brad_convert(radians);

        *result_sin = fix16_bradians_sin(bradians);
        *result_cos = fix16_bradians_cos(bradians);
}

fix16_t
fix16_bradians_sin(int32_t bradians)
{
        const uint32_t index =
            (bradians + FIX16_LUT_SIN_TABLE_COUNT);

        return _lut_brads_sin[index & (FIX16_LUT_SIN_TABLE_COUNT - 1)];
}

fix16_t
fix16_bradians_cos(int32_t bradians)
{
        const uint32_t index =
            (bradians + (FIX16_LUT_SIN_TABLE_COUNT / 4) + FIX16_LUT_SIN_TABLE_COUNT);

        return _lut_brads_sin[index & (FIX16_LUT_SIN_TABLE_COUNT - 1)];
}

fix16_t
fix16_tan(const fix16_t radians)
{
        const int32_t bradians = _rad2brad_convert(radians);
        const fix16_t cos = fix16_bradians_cos(bradians);

        cpu_divu_fix16_set(FIX16(1.0f), cos);

        const fix16_t sin = fix16_bradians_sin(bradians);

        const fix16_t quotient = cpu_divu_quotient_get();
        const fix16_t result = fix16_mul(sin, quotient);

        return result;
}

fix16_t
fix16_atan2(fix16_t y, fix16_t x)
{
        if (y == 0) {
                return ((x >= 0) ? 0 : FIX16_PI);
        }

        int32_t phi;
        phi = 0;

        if (y < 0) {
                x = -x;
                y = -y;
                phi += 4;
        }

        if (x <= 0) {
                const fix16_t t = x;

                x = y;
                y = -t;
                phi += 2;
        }

        if (x <= y) {
                const fix16_t t = y - x;

                x = x + y;
                y = t;
                phi++;
        }

        cpu_divu_fix16_set(y, x);

        const fix16_t q = cpu_divu_quotient_get();
        const int32_t index = fix16_int32_to(q * FIX16_LUT_ATAN_TABLE_COUNT);
        const fix16_t dphi = _lut_brads_atan[index];

        return ((phi * (FIX16_PI / 4)) + dphi);
}
