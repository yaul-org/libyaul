/*
 * Copyright (c) 2013-2014
 * See LICENSE for details.
 *
 * Mattias Jansson
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <string.h>

#include "vector3.h"

void
fix16_vector3_zero(fix16_vector3_t *result)
{
        result->x = 0;
        result->y = 0;
        result->z = 0;
}

void
fix16_vector3_normalize(fix16_vector3_t *result)
{
        fix16_t inv_length;
        inv_length = fix16_div(fix16_from_int(1), fix16_vector3_length(result));

        fix16_vector3_scale(inv_length, result);
}

fix16_t
fix16_vector3_length(const fix16_vector3_t *v0)
{
        return fix16_sqrt(fix16_add(fix16_add(fix16_mul(v0->x, v0->x),
                    fix16_mul(v0->y, v0->y)), fix16_mul(v0->z, v0->z)));
}

void
fix16_vector3_add(const fix16_vector3_t *v0, const fix16_vector3_t *v1,
    fix16_vector3_t *result)
{
        result->x = fix16_add(v0->x, v1->x);
        result->y = fix16_add(v0->y, v1->y);
        result->z = fix16_add(v0->z, v1->z);
}

void
fix16_vector3_sub(const fix16_vector3_t *v0, const fix16_vector3_t *v1,
    fix16_vector3_t *result)
{
        result->x = fix16_sub(v0->x, v1->x);
        result->y = fix16_sub(v0->y, v1->y);
        result->z = fix16_sub(v0->z, v1->z);
}

void
fix16_vector3_scale(fix16_t scalar, fix16_vector3_t *result)
{
        result->x = fix16_mul(scalar, result->x);
        result->y = fix16_mul(scalar, result->y);
        result->z = fix16_mul(scalar, result->z);
}

fix16_t
fix16_vector3_dot(const fix16_vector3_t *v0, const fix16_vector3_t *v1)
{
        fix16_t term[3];

        term[0] = fix16_mul(v0->x, v1->x);
        term[1] = fix16_mul(v0->y, v1->y);
        term[2] = fix16_mul(v0->z, v1->z);

        return fix16_add(term[0], fix16_add(term[1], term[2]));
}

void
fix16_vector3_cross(const fix16_vector3_t *v0 __unused__,
    const fix16_vector3_t *v1 __unused__,
    fix16_vector3_t *result __unused__)
{
}

void
fix16_vector3_str(const fix16_vector3_t *v0, char *buf, int decimals)
{
        char component_buf[13] __aligned__(16);
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
