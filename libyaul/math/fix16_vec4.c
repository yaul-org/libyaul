/*
 * Copyright (c) 2013-2014
 * See LICENSE for details.
 *
 * Mattias Jansson
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <string.h>

#include "fix16.h"

void
fix16_vec4_normalize(fix16_vec4_t *result)
{
        fix16_t inv_length;
        inv_length = fix16_div(FIX16_ONE, fix16_vec4_length(result));

        fix16_vec4_scale(inv_length, result);
}

void
fix16_vec4_normalized(const fix16_vec4_t * restrict v0, fix16_vec4_t * restrict result)
{
        fix16_t inv_length;
        inv_length = fix16_div(FIX16_ONE, fix16_vec4_length(v0));

        fix16_vec4_scaled(inv_length, v0, result);
}

fix16_t
fix16_vec4_length(const fix16_vec4_t *v0)
{
        return fix16_sqrt(fix16_add(fix16_add(fix16_mul(v0->x, v0->x),
                                              fix16_mul(v0->y, v0->y)), fix16_mul(v0->z, v0->z)));
}

void
fix16_vec4_mat4_multiply(const fix16_mat4_t *m0,
    const fix16_vec4_t *v0, fix16_vec4_t *result)
{
        result->comp[0] = fix16_add(
                                  fix16_add(fix16_mul(v0->x, m0->row[0].x),
                                            fix16_mul(v0->y, m0->row[0].y)),
                                  fix16_add(fix16_mul(v0->z, m0->row[0].z),
                                            fix16_mul(v0->w, m0->row[0].w)));

        result->comp[1] = fix16_add(
                                  fix16_add(fix16_mul(v0->x, m0->row[1].x),
                                            fix16_mul(v0->y, m0->row[1].y)),
                                  fix16_add(fix16_mul(v0->z, m0->row[1].z),
                                            fix16_mul(v0->w, m0->row[1].w)));

        result->comp[2] = fix16_add(
                                  fix16_add(fix16_mul(v0->x, m0->row[2].x),
                                            fix16_mul(v0->y, m0->row[2].y)),
                                  fix16_add(fix16_mul(v0->z, m0->row[2].z),
                                            fix16_mul(v0->w, m0->row[2].w)));

        result->comp[3] = fix16_add(
                                  fix16_add(fix16_mul(v0->x, m0->row[3].x),
                                            fix16_mul(v0->y, m0->row[3].y)),
                                  fix16_add(fix16_mul(v0->z, m0->row[3].z),
                                            fix16_mul(v0->w, m0->row[3].w)));
}

void
fix16_vec4_mat4_multiply2(const fix16_mat4_t *m,
    const fix16_vec4_t *v, fix16_vec4_t *result)
{
        result->comp[0] = fix16_add(
                                  fix16_add(fix16_mul(v->x, m->row[0].x),
                                            fix16_mul(v->y, m->row[0].y)),
                                  fix16_add(fix16_mul(v->z, m->row[0].z), m->row[0].w));

        result->comp[1] = fix16_add(
                                  fix16_add(fix16_mul(v->x, m->row[1].x),
                                            fix16_mul(v->y, m->row[1].y)),
                                  fix16_add(fix16_mul(v->z, m->row[1].z), m->row[1].w));

        result->comp[2] = fix16_add(
                                  fix16_add(fix16_mul(v->x, m->row[2].x),
                                            fix16_mul(v->y, m->row[2].y)),
                                  fix16_add(fix16_mul(v->z, m->row[2].z), m->row[2].w));

        result->comp[3] = FIX16_ONE;
}

void
fix16_vec4_cross(const fix16_vec4_t * restrict u, const fix16_vec4_t * restrict v,
    fix16_vec4_t * restrict result)
{
        result->x = (u->y * v->z) - (u->z * v->y);
        result->y = (u->z * v->x) - (u->x * v->z);
        result->z = (u->x * v->y) - (u->y * v->x);
        result->w = FIX16_ONE;
}

void
fix16_vec4_str(const fix16_vec4_t *v0, char *buf, uint8_t decimals)
{
        /* Since each vector component is 16 bytes (padded). So,
         * ((16 * 4) + 5 + 1) (69) bytes is required */

        char component_buf[13] __aligned(4);
        size_t component_buf_len;

        char *buf_ptr;
        buf_ptr = buf;

        *buf_ptr++ = '(';
        fix16_to_str(v0->x, component_buf, decimals);
        component_buf_len = strlen(component_buf);
        (void)memcpy(buf_ptr, component_buf, component_buf_len);
        buf_ptr += component_buf_len;
        *buf_ptr++ = ',';

        fix16_to_str(v0->y, component_buf, decimals);
        component_buf_len = strlen(component_buf);
        (void)memcpy(buf_ptr, component_buf, component_buf_len);
        buf_ptr += component_buf_len;
        *buf_ptr++ = ',';

        fix16_to_str(v0->z, component_buf, decimals);
        component_buf_len = strlen(component_buf);
        (void)memcpy(buf_ptr, component_buf, component_buf_len);
        buf_ptr += component_buf_len;
        *buf_ptr++ = ',';

        fix16_to_str(v0->w, component_buf, decimals);
        component_buf_len = strlen(component_buf);
        (void)memcpy(buf_ptr, component_buf, component_buf_len);
        buf_ptr += component_buf_len;
        *buf_ptr++ = ')';

        *buf_ptr = '\0';
}
