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
fix16_vec3_zero(fix16_vec3_t *result)
{
        result->x = FIX16(0.0f);
        result->y = FIX16(0.0f);
        result->z = FIX16(0.0f);
}

void
fix16_vec3_dup(const fix16_vec3_t *v0, fix16_vec3_t *result)
{
        result->x = v0->x;
        result->y = v0->y;
        result->z = v0->z;
}

void
fix16_vec3_normalize(fix16_vec3_t *result)
{
        fix16_t inv_length;
        inv_length = fix16_div(FIX16_ONE, fix16_vec3_length(result));

        fix16_vec3_scale(inv_length, result);
}

void
fix16_vec3_normalized(const fix16_vec3_t *v0, fix16_vec3_t *result)
{
        fix16_t inv_length;
        inv_length = fix16_div(FIX16_ONE, fix16_vec3_length(v0));

        fix16_vec3_scaled(inv_length, v0, result);
}

fix16_t
fix16_vec3_length(const fix16_vec3_t *v0)
{
        return fix16_sqrt(fix16_add(fix16_add(fix16_mul(v0->x, v0->x),
                                              fix16_mul(v0->y, v0->y)), fix16_mul(v0->z, v0->z)));
}

void
fix16_vec3_add(const fix16_vec3_t *v0, const fix16_vec3_t *v1,
                  fix16_vec3_t *result)
{
        result->x = fix16_add(v0->x, v1->x);
        result->y = fix16_add(v0->y, v1->y);
        result->z = fix16_add(v0->z, v1->z);
}

void
fix16_vec3_sub(const fix16_vec3_t *v0, const fix16_vec3_t *v1,
                  fix16_vec3_t *result)
{
        result->x = fix16_sub(v0->x, v1->x);
        result->y = fix16_sub(v0->y, v1->y);
        result->z = fix16_sub(v0->z, v1->z);
}

void
fix16_vec3_scale(fix16_t scalar, fix16_vec3_t *result)
{
        result->x = fix16_mul(scalar, result->x);
        result->y = fix16_mul(scalar, result->y);
        result->z = fix16_mul(scalar, result->z);
}

void
fix16_vec3_scaled(const fix16_t scalar, const fix16_vec3_t *v,
                     fix16_vec3_t *result)
{
        result->x = fix16_mul(scalar, v->x);
        result->y = fix16_mul(scalar, v->y);
        result->z = fix16_mul(scalar, v->z);
}

fix16_t
fix16_vec3_dot(const fix16_vec3_t *v0, const fix16_vec3_t *v1)
{
        fix16_t term[3];

        term[0] = fix16_mul(v0->x, v1->x);
        term[1] = fix16_mul(v0->y, v1->y);
        term[2] = fix16_mul(v0->z, v1->z);

        return fix16_add(term[0], fix16_add(term[1], term[2]));
}

fix16_t
fix16_vec3_angle(const fix16_vec3_t *v0, const fix16_vec3_t *v1)
{
        fix16_t v0_length;
        v0_length = fix16_vec3_length(v0);

        fix16_t v1_length;
        v1_length = fix16_vec3_length(v1);

        return fix16_acos(fix16_div(fix16_vec3_dot(v0, v1),
                                    fix16_mul(v0_length, v1_length)));
}

void
fix16_vec3_cross(const fix16_vec3_t *u, const fix16_vec3_t *v,
                    fix16_vec3_t *result)
{
        result->x = fix16_sub(fix16_mul(u->y, v->z), fix16_mul(u->z, v->y));
        result->y = fix16_sub(fix16_mul(u->z, v->x), fix16_mul(u->x, v->z));
        result->z = fix16_sub(fix16_mul(u->x, v->y), fix16_mul(u->y, v->x));
}

void
fix16_vec3_mat3_multiply(const fix16_mat3_t *m0,
                               const fix16_vec3_t *v0, fix16_vec3_t *result)
{
        result->comp[0] = fix16_add(
                                  fix16_add(fix16_mul(v0->x, m0->row[0].x), fix16_mul(v0->y, m0->row[0].y)),
                                  fix16_mul(v0->z, m0->row[0].z));

        result->comp[1] = fix16_add(
                                  fix16_add(fix16_mul(v0->x, m0->row[1].x), fix16_mul(v0->y, m0->row[1].y)),
                                  fix16_mul(v0->z, m0->row[1].z));

        result->comp[2] = fix16_add(
                                  fix16_add(fix16_mul(v0->x, m0->row[2].x), fix16_mul(v0->y, m0->row[2].y)),
                                  fix16_mul(v0->z, m0->row[2].z));
}

void
fix16_vec3_str(const fix16_vec3_t *v0, char *buf, int decimals)
{
        char component_buf[13] __aligned(16);
        size_t component_buf_len;

        char *buf_ptr;
        buf_ptr = buf;

        *buf_ptr++ = '(';
        fix16_to_str(v0->x, component_buf, decimals);
        component_buf_len = strlen(component_buf);
        memcpy(buf_ptr, component_buf, component_buf_len);
        buf_ptr += component_buf_len;
        *buf_ptr++ = ',';

        fix16_to_str(v0->y, component_buf, decimals);
        component_buf_len = strlen(component_buf);
        memcpy(buf_ptr, component_buf, component_buf_len);
        buf_ptr += component_buf_len;
        *buf_ptr++ = ',';

        fix16_to_str(v0->z, component_buf, decimals);
        component_buf_len = strlen(component_buf);
        memcpy(buf_ptr, component_buf, component_buf_len);
        buf_ptr += component_buf_len;
        *buf_ptr++ = ')';

        *buf_ptr = '\0';
}
