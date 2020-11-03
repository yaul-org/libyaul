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

#include "fix16_sin.inc" 

static inline int32_t __always_inline 
_rad2brad_convert(const fix16_t radians)
{
        const fix16_t converted =
            fix16_mul(radians, FIX16(FIX16_LUT_TABLE_COUNT / (2.0f * M_PI)));

        return (fix16_int32_to(converted) + FIX16_LUT_TABLE_COUNT);
}

fix16_t
fix16_sin(const fix16_t radians)
{
        return fix16_bradians_sin(_rad2brad_convert(radians));
}

fix16_t
fix16_cos(const fix16_t radians)
{
        return fix16_bradians_cos(_rad2brad_convert(radians));
}

fix16_t
fix16_bradians_sin(const int32_t bradians)
{
        return _lut_brads_sin[(bradians + FIX16_LUT_TABLE_COUNT) & (FIX16_LUT_TABLE_COUNT - 1)];
}

fix16_t
fix16_bradians_cos(const int32_t bradians)
{
        return _lut_brads_sin[(bradians + (FIX16_LUT_TABLE_COUNT / 4) + FIX16_LUT_TABLE_COUNT) & (FIX16_LUT_TABLE_COUNT - 1)];
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
fix16_asin(const fix16_t x __unused)
{
        assert(false && "Not implemented");

        return FIX16(0.0f);
}

fix16_t
fix16_acos(const fix16_t x)
{
        assert(false && "Not implemented");

        return ((FIX16_PI >> 1) - fix16_asin(x));
}

fix16_t
fix16_atan(const fix16_t x)
{
        assert(false && "Not implemented");

        return fix16_atan2(x, FIX16(1.0f));
}

fix16_t
fix16_atan2(const fix16_t x __unused, const fix16_t y __unused)
{
        assert(false && "Not implemented");

        return FIX16(0.0f);
}
