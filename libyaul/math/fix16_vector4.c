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
fix16_vector4_zero(fix16_vector4_t *result)
{
        result->x = F16(0.0f);
        result->y = F16(0.0f);
        result->z = F16(0.0f);
        result->w = F16(0.0f);
}

void
fix16_vector4_dup(const fix16_vector4_t *v0, fix16_vector4_t *result)
{
        result->x = v0->x;
        result->y = v0->y;
        result->z = v0->z;
        result->w = v0->w;
}

void
fix16_vector4_normalize(fix16_vector4_t *result)
{
        fix16_t inv_length;
        inv_length = fix16_div(F16(1.0f), fix16_vector4_length(result));

        fix16_vector4_scale(inv_length, result);
}

void
fix16_vector4_normalized(const fix16_vector4_t *v0, fix16_vector4_t *result)
{
        fix16_t inv_length;
        inv_length = fix16_div(F16(1.0f), fix16_vector4_length(v0));

        fix16_vector4_scaled(inv_length, v0, result);
}

fix16_t
fix16_vector4_length(const fix16_vector4_t *v0)
{
        return fix16_sqrt(fix16_add(fix16_add(fix16_mul(v0->x, v0->x),
                    fix16_mul(v0->y, v0->y)), fix16_mul(v0->z, v0->z)));
}

void
fix16_vector4_add(const fix16_vector4_t *v0, const fix16_vector4_t *v1,
    fix16_vector4_t *result)
{
        result->x = fix16_add(v0->x, v1->x);
        result->y = fix16_add(v0->y, v1->y);
        result->z = fix16_add(v0->z, v1->z);
        result->w = 1.0f;
}

void
fix16_vector4_sub(const fix16_vector4_t *v0, const fix16_vector4_t *v1,
    fix16_vector4_t *result)
{
        result->x = fix16_sub(v0->x, v1->x);
        result->y = fix16_sub(v0->y, v1->y);
        result->z = fix16_sub(v0->z, v1->z);
        result->w = F16(1.0f);
}

void
fix16_vector4_scale(fix16_t scalar, fix16_vector4_t *result)
{
        result->x = fix16_mul(scalar, result->x);
        result->y = fix16_mul(scalar, result->y);
        result->z = fix16_mul(scalar, result->z);
        result->w = F16(1.0f);
}

void
fix16_vector4_scaled(fix16_t scalar, const fix16_vector4_t *v0,
    fix16_vector4_t *result)
{
        result->x = fix16_mul(scalar, v0->x);
        result->y = fix16_mul(scalar, v0->y);
        result->z = fix16_mul(scalar, v0->z);
        result->w = F16(1.0f);
}

void
fix16_vector4_matrix4_multiply(const fix16_matrix4_t *m0,
    const fix16_vector4_t *v0, fix16_vector4_t *result)
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
fix16_vector4_matrix4_multiply2(const fix16_matrix4_t *m,
    const fix16_vector4_t *v, fix16_vector4_t *result)
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

        result->comp[3] = F16(1.0f);
}

void
fix16_vector4_cross(const fix16_vector4_t *u, const fix16_vector4_t *v,
    fix16_vector4_t *result)
{
        result->x = fix16_sub(fix16_mul(u->y, v->z), fix16_mul(u->z, v->y));
        result->y = fix16_sub(fix16_mul(u->z, v->x), fix16_mul(u->x, v->z));
        result->z = fix16_sub(fix16_mul(u->x, v->y), fix16_mul(u->y, v->x));
        result->w = F16(1.0f);
}

void
fix16_vector4_str(const fix16_vector4_t *v0, char *buf, int decimals)
{
        /* Since each vector component is 16 bytes (padded). So,
         * ((16 * 4) + 5 + 1) (69) bytes is required */

        static char component_buf[13] __aligned(16);
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
        *buf_ptr++ = ',';

        fix16_to_str(v0->w, component_buf, decimals);
        component_buf_len = strlen(component_buf);
        memcpy(buf_ptr, component_buf, component_buf_len);
        buf_ptr += component_buf_len;
        *buf_ptr++ = ')';

        *buf_ptr = '\0';
}
