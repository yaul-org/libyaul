/*
 * Copyright (c) 2013-2014
 * See LICENSE for details.
 *
 * Mattias Jansson
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <string.h>

#include <cpu/divu.h>

#include <gamemath/fix16/fix16_vec3.h>

void
fix16_vec3_normalize(fix16_vec3_t *v0)
{
    const fix16_t length = fix16_vec3_length(v0);

    cpu_divu_fix16_set(FIX16_ONE, length);

    const fix16_t scale = cpu_divu_quotient_get();

    fix16_vec3_scale(scale, v0);
}

void
fix16_vec3_normalized(const fix16_vec3_t * __restrict v0,
  fix16_vec3_t * __restrict result)
{
    const fix16_t length = fix16_vec3_length(v0);

    cpu_divu_fix16_set(FIX16_ONE, length);

    const fix16_t scale = cpu_divu_quotient_get();

    fix16_vec3_scaled(scale, v0, result);
}

fix16_t
fix16_vec3_length(const fix16_vec3_t *v0)
{
    const fix16_t r = fix16_vec3_sqr_length(v0);
    const fix16_t length = fix16_sqrt(r);

    return length;
}

fix16_t
fix16_vec3_sqr_length(const fix16_vec3_t *v0)
{
    const fix16_vec3_t v0_shifted = {
        .x = v0->x,
        .y = v0->y,
        .z = v0->z
    };

    const fix16_vec3_t *v0_p1 = &v0_shifted;
    const fix16_vec3_t *v0_p2 = &v0_shifted;

    cpu_instr_clrmac();
    cpu_instr_macl(&v0_p1, &v0_p2);
    cpu_instr_macl(&v0_p1, &v0_p2);
    cpu_instr_macl(&v0_p1, &v0_p2);

    const fix16_t mach = cpu_instr_sts_mach();
    const fix16_t macl = cpu_instr_sts_macl();

    const fix16_t length = cpu_instr_xtrct(mach, macl);

    return length;
}

fix16_t
fix16_vec3_dot(const fix16_vec3_t *v0, const fix16_vec3_t *v1)
{
    return fix16_vec3_inline_dot(v0, v1);
}

void
fix16_vec3_cross(const fix16_vec3_t * const __restrict v0,
  const fix16_vec3_t * const __restrict v1, fix16_vec3_t * __restrict result)
{
    result->x = fix16_mul(v0->y, v1->z) - fix16_mul(v0->z, v1->y);
    result->y = fix16_mul(v0->z, v1->x) - fix16_mul(v0->x, v1->z);
    result->z = fix16_mul(v0->x, v1->y) - fix16_mul(v0->y, v1->x);
}

fix16_t
fix16_vec3_cross_mag(const fix16_vec3_t * __restrict v0,
  const fix16_vec3_t * __restrict v1)
{
    const fix16_t t1 = fix16_mul(v0->x, v1->y) - fix16_mul(v0->y, v1->x);
    const fix16_t t2 = fix16_mul(v0->y, v1->z) - fix16_mul(v0->z, v1->y);
    const fix16_t t3 = fix16_mul(v0->z, v1->x) - fix16_mul(v0->x, v1->z);

    const fix16_t t1_2 = fix16_mul(t1, t1);
    const fix16_t t2_2 = fix16_mul(t2, t2);
    const fix16_t t3_2 = fix16_mul(t3, t3);

    const fix16_t result = t1_2 + t2_2 + t3_2;

    return result;
}

size_t
fix16_vec3_str(const fix16_vec3_t *v0, char *buffer, int32_t decimals)
{
    char *buffer_ptr;
    buffer_ptr = buffer;

    *buffer_ptr++ = '(';
    buffer_ptr += fix16_str(v0->x, buffer_ptr, decimals);
    *buffer_ptr++ = ',';

    buffer_ptr += fix16_str(v0->y, buffer_ptr, decimals);
    *buffer_ptr++ = ',';

    buffer_ptr += fix16_str(v0->z, buffer_ptr, decimals);
    *buffer_ptr++ = ')';

    *buffer_ptr = '\0';

    return (buffer_ptr - buffer);
}
