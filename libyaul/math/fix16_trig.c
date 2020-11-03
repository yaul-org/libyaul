/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdbool.h>

#include "math.h"
#include "fix16.h"

static fix16_sin_table_t _sin_table __unused;
static fix16_cos_table_t _cos_table __unused;
static fix16_atan2_table_t _atan2_table __unused;

#include "fix16_sin.inc" 

void
fix16_trig_tables_set(const fix16_sin_table_t *sin_table __unused,
    const fix16_cos_table_t *cos_table __unused,
    const fix16_atan2_table_t *atan2_table __unused)
{
        assert(false && "Not implemented");
}

fix16_t
fix16_sin(const fix16_t radians)
{
        fix16_t converted;
        converted = fix16_mul(radians, FIX16(FIX16_LUT_TABLE_COUNT / (2.0f * M_PI)));

        return fix16_bradians_sin(fix16_int32_to(converted));
}

fix16_t
fix16_cos(const fix16_t radians)
{
        fix16_t converted;
        converted = fix16_mul(radians, FIX16(FIX16_LUT_TABLE_COUNT / (2.0f * M_PI)));

        return fix16_bradians_cos(fix16_int32_to(converted));
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
fix16_tan(const fix16_t radians __unused)
{
        assert(false && "Not implemented");

        return FIX16(0.0f);
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

        return fix16_atan2(x, FIX16_ONE);
}

fix16_t
fix16_atan2(const fix16_t x __unused, const fix16_t y __unused)
{
        assert(false && "Not implemented");

        return FIX16(0.0f);
}
