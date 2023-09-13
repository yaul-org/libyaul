/*
 * Copyright (c) 2013-2014
 * See LICENSE for details.
 *
 * Mattias Jansson
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <string.h>

#include <cpu/divu.h>

#include <gamemath/fix16/fix16_vec2.h>

void
fix16_vec2_normalize(fix16_vec2_t *v0)
{
    const fix16_t length = fix16_vec2_length(v0);

    cpu_divu_fix16_set(FIX16_ONE, length);

    const fix16_t scale = cpu_divu_quotient_get();

    fix16_vec2_scale(scale, v0);
}

void
fix16_vec2_normalized(const fix16_vec2_t * __restrict v0, fix16_vec2_t * __restrict result)
{
    const fix16_t length = fix16_vec2_length(v0);

    cpu_divu_fix16_set(FIX16_ONE, length);

    const fix16_t scale = cpu_divu_quotient_get();

    fix16_vec2_scaled(scale, v0, result);
}

fix16_t
fix16_vec2_length(const fix16_vec2_t *v0)
{
    const fix16_t r = fix16_vec2_inline_dot(v0, v0);
    const fix16_t sqrt = fix16_sqrt(r);

    return sqrt;
}

fix16_t
fix16_vec2_sqr_length(const fix16_vec2_t *v0)
{
    const fix16_t r = fix16_vec2_inline_dot(v0, v0);

    return r;
}

fix16_t
fix16_vec2_dot(const fix16_vec2_t *v0, const fix16_vec2_t *v1)
{
    return fix16_vec2_inline_dot(v0, v1);
}

size_t
fix16_vec2_str(const fix16_vec2_t *v0, char *buffer, int32_t decimals)
{
    char *buffer_ptr;
    buffer_ptr = buffer;

    *buffer_ptr++ = '(';
    buffer_ptr += fix16_str(v0->x, buffer_ptr, decimals);
    *buffer_ptr++ = ',';

    buffer_ptr += fix16_str(v0->y, buffer_ptr, decimals);
    *buffer_ptr++ = ')';

    *buffer_ptr = '\0';

    return (buffer_ptr - buffer);
}
