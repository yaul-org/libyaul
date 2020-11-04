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
fix16_vec3_normalize(fix16_vec3_t *result __unused)
{
        const fix16_t length = fix16_vec3_length(result);

        cpu_divu_fix16_set(FIX16(1.0f), length);

        const fix16_t scale = cpu_divu_quotient_get();

        fix16_vec3_scale(scale, result);
}

void
fix16_vec3_normalized(const fix16_vec3_t * __restrict v0,
    fix16_vec3_t * __restrict result)
{
        const fix16_t length = fix16_vec3_length(v0);

        cpu_divu_fix16_set(FIX16(1.0f), length);

        const fix16_t scale = cpu_divu_quotient_get();

        fix16_vec3_scaled(scale, v0, result);
}

fix16_t
fix16_vec3_length(const fix16_vec3_t *v0)
{
        const fix16_t r = fix16_vec3_inline_dot(v0, v0);
        const fix16_t sqrt = fix16_sqrt(r);

        return sqrt;
}

fix16_t
fix16_vec3_sqr_length(const fix16_vec3_t *v0)
{
        const fix16_t r = fix16_vec3_inline_dot(v0, v0);

        return r;
}

fix16_t
fix16_vec3_dot(const fix16_vec3_t *a, const fix16_vec3_t *b)
{
        return fix16_vec3_inline_dot(a, b);
}

void
fix16_vec3_cross(const fix16_vec3_t * __restrict v0,
    const fix16_vec3_t * __restrict v1, fix16_vec3_t * __restrict result)
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

void
fix16_vec3_str(const fix16_vec3_t *v0, char *buf, int decimals)
{
        char component_buf[13] __aligned(16);
        size_t component_buf_len;

        char *buf_ptr;
        buf_ptr = buf;

        *buf_ptr++ = '(';
        fix16_str(v0->x, component_buf, decimals);
        component_buf_len = strlen(component_buf);
        memcpy(buf_ptr, component_buf, component_buf_len);
        buf_ptr += component_buf_len;
        *buf_ptr++ = ',';

        fix16_str(v0->y, component_buf, decimals);
        component_buf_len = strlen(component_buf);
        memcpy(buf_ptr, component_buf, component_buf_len);
        buf_ptr += component_buf_len;
        *buf_ptr++ = ',';

        fix16_str(v0->z, component_buf, decimals);
        component_buf_len = strlen(component_buf);
        memcpy(buf_ptr, component_buf, component_buf_len);
        buf_ptr += component_buf_len;
        *buf_ptr++ = ')';

        *buf_ptr = '\0';
}
