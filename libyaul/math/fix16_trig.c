/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include "fix16.h"

static fix16_sin_table_t _sin_table __unused;
static fix16_cos_table_t _cos_table __unused;
static fix16_atan2_table_t _atan2_table __unused;

void
fix16_trig_tables_set(const fix16_sin_table_t *sin_table __unused,
    const fix16_cos_table_t *cos_table __unused,
    const fix16_atan2_table_t *atan2_table __unused)
{
}

fix16_t
fix16_sin(const fix16_t radians __unused)
{
        return FIX16(0.0f);
}

fix16_t
fix16_cos(const fix16_t radians)
{
        return fix16_sin(radians + (FIX16_PI >> 1));
}

fix16_t
fix16_tan(const fix16_t radians __unused)
{
        return FIX16(0.0f);
}

fix16_t
fix16_asin(const fix16_t x __unused)
{
        return FIX16(0.0f);
}

fix16_t
fix16_acos(const fix16_t x)
{
        return ((FIX16_PI >> 1) - fix16_asin(x));
}

fix16_t
fix16_atan(const fix16_t x)
{
        return fix16_atan2(x, FIX16_ONE);
}

fix16_t
fix16_atan2(const fix16_t x __unused, const fix16_t y __unused)
{
        return FIX16(0.0f);
}
