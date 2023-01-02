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
fix16_vec4_normalize(fix16_vec4_t *v0)
{
        const fix16_t length = fix16_vec4_length(v0);

        cpu_divu_fix16_set(FIX16_ONE, length);

        const fix16_t scale = cpu_divu_quotient_get();

        fix16_vec4_scale(scale, v0);
}

void
fix16_vec4_normalized(const fix16_vec4_t * __restrict v0,
    fix16_vec4_t * __restrict result)
{
        const fix16_t length = fix16_vec4_length(v0);

        cpu_divu_fix16_set(FIX16_ONE, length);

        const fix16_t scale = cpu_divu_quotient_get();

        fix16_vec4_scaled(scale, v0, result);
}

fix16_t
fix16_vec4_length(const fix16_vec4_t *v0)
{
        const fix16_t r = fix16_vec4_sqr_length(v0);
        const fix16_t length = fix16_sqrt(r);

        return length;
}

fix16_t
fix16_vec4_sqr_length(const fix16_vec4_t *v0)
{
        const fix16_vec4_t v0_shifted = {
                .x = v0->x,
                .y = v0->y,
                .z = v0->z,
                .w = v0->w
        };

        const fix16_vec4_t *v0_p1 = &v0_shifted;
        const fix16_vec4_t *v0_p2 = &v0_shifted;

        cpu_instr_clrmac();
        cpu_instr_macl(&v0_p1, &v0_p2);
        cpu_instr_macl(&v0_p1, &v0_p2);
        cpu_instr_macl(&v0_p1, &v0_p2);
        cpu_instr_macl(&v0_p1, &v0_p2);

        const fix16_t mach = cpu_instr_sts_mach();
        const fix16_t macl = cpu_instr_sts_macl();

        const fix16_t length = cpu_instr_xtrct(mach, macl);

        return length;
}

fix16_t
fix16_vec4_dot(const fix16_vec4_t *v0, const fix16_vec4_t *v1)
{
        return fix16_vec4_inline_dot(v0, v1);
}

size_t
fix16_vec4_str(const fix16_vec4_t *v0, char *buffer, int32_t decimals)
{
        char *buffer_ptr;
        buffer_ptr = buffer;

        *buffer_ptr++ = '(';
        buffer_ptr += fix16_str(v0->x, buffer_ptr, decimals);
        *buffer_ptr++ = ',';

        buffer_ptr += fix16_str(v0->y, buffer_ptr, decimals);
        *buffer_ptr++ = ',';

        buffer_ptr += fix16_str(v0->z, buffer_ptr, decimals);
        *buffer_ptr++ = ',';

        buffer_ptr += fix16_str(v0->w, buffer_ptr, decimals);
        *buffer_ptr++ = ')';

        *buffer_ptr = '\0';

        return (buffer_ptr - buffer);
}
