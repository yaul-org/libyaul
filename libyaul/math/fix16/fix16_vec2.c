/*
 * Copyright (c) 2013-2014
 * See LICENSE for details.
 *
 * Mattias Jansson
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <string.h>

#include <cpu/divu.h>

#include "fix16.h"

void
fix16_vec2_normalize(fix16_vec2_t *v0)
{
        const fix16_t length = fix16_vec2_length(v0);

        cpu_divu_fix16_set(FIX16(1.0f), length);

        const fix16_t scale = cpu_divu_quotient_get();

        fix16_vec2_scale(scale, v0);
}

void
fix16_vec2_normalized(const fix16_vec2_t * __restrict v0, fix16_vec2_t * __restrict result)
{
        const fix16_t length = fix16_vec2_length(v0);

        cpu_divu_fix16_set(FIX16(1.0f), length);

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

void
fix16_vec2_str(const fix16_vec2_t *v0, char *buffer, int decimals)
{
        char component_buffer[13] __aligned(16);
        size_t component_buffer_size;

        char *buffer_ptr;
        buffer_ptr = buffer;

        *buffer_ptr++ = '(';
        fix16_str(v0->x, component_buffer, decimals);
        component_buffer_size = strlen(component_buffer);
        memcpy(buffer_ptr, component_buffer, component_buffer_size);
        buffer_ptr += component_buffer_size;
        *buffer_ptr++ = ',';

        fix16_str(v0->y, component_buffer, decimals);
        component_buffer_size = strlen(component_buffer);
        memcpy(buffer_ptr, component_buffer, component_buffer_size);
        buffer_ptr += component_buffer_size;
        *buffer_ptr++ = ')';

        *buffer_ptr = '\0';
}
