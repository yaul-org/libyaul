/*
 * Copyright (c) 2013-2014
 * See LICENSE for details.
 *
 * Mattias Jansson
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include "vector4.h"

void
fix16_vector4_normalize(fix16_vector4_t *result)
{
        fix16_t inv_length;
        inv_length = fix16_div(fix16_from_int(1), fix16_vector4_length(result));

        fix16_vector4_scale(inv_length, result);
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
        result->w = 1.0f;
}

void
fix16_vector4_scale(fix16_t scalar, fix16_vector4_t *result)
{
        result->x = fix16_mul(scalar, result->x);
        result->y = fix16_mul(scalar, result->y);
        result->z = fix16_mul(scalar, result->z);
        result->w = 1.0f;
}

fix16_t
fix16_vector4_dot(const fix16_vector4_t *v0, const fix16_vector4_t *v1)
{
        int16_t v0_x;
        int16_t v0_y;
        int16_t v0_z;
        int16_t v1_x;
        int16_t v1_y;
        int16_t v1_z;

        v0_x = fix16_from_int(v0->x);
        v0_y = fix16_from_int(v0->y);
        v0_z = fix16_from_int(v0->z);

        v1_x = fix16_from_int(v1->x);
        v1_y = fix16_from_int(v1->y);
        v1_z = fix16_from_int(v1->z);

        return (v0_x * v1_x) + (v0_y * v1_y) + (v0_z * v1_z);
}

void
fix16_vector4_cross(const fix16_vector4_t *v0 __unused__,
    const fix16_vector4_t *v1 __unused__,
    fix16_vector4_t *result __unused__)
{
}
