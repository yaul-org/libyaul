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
fix16_vector2_zero(fix16_vector2_t *result)
{
        result->x = F16(0.0f);
        result->y = F16(0.0f);
}

void
fix16_vector2_dup(const fix16_vector2_t *v0, fix16_vector2_t *result)
{
        result->x = v0->x;
        result->y = v0->y;
}

void
fix16_vector2_normalize(fix16_vector2_t *result)
{
}

void
fix16_vector2_normalized(const fix16_vector2_t *v0,
    fix16_vector2_t *result)
{
}

fix16_t
fix16_vector2_length(const fix16_vector2_t *v0)
{
        return fix16_sqrt(fix16_add(fix16_mul(v0->x, v0->x),
                fix16_mul(v0->y, v0->y)));
}

void
fix16_vector2_add(const fix16_vector2_t *v0, const fix16_vector2_t *v1,
    fix16_vector2_t *result)
{
        result->x = fix16_add(v0->x, v1->x);
        result->y = fix16_add(v0->y, v1->y);
}

void
fix16_vector2_sub(const fix16_vector2_t *v0, const fix16_vector2_t *v1,
    fix16_vector2_t *result)
{
        result->x = fix16_sub(v0->x, v1->x);
        result->y = fix16_sub(v0->y, v1->y);
}

void
fix16_vector2_scale(fix16_t scalar, fix16_vector2_t *result)
{
        result->x = fix16_mul(scalar, result->x);
        result->y = fix16_mul(scalar, result->y);
}

void
fix16_vector2_scaled(fix16_t scalar, const fix16_vector2_t *v0,
    fix16_vector2_t *result)
{
        result->x = fix16_mul(scalar, v0->x);
        result->y = fix16_mul(scalar, v0->y);
}

fix16_t
fix16_vector2_dot(const fix16_vector2_t *v0, const fix16_vector2_t *v1)
{
        return fix16_add(fix16_mul(v0->x, v1->x), fix16_mul(v0->y, v1->y));
}

void
fix16_vector2_str(const fix16_vector2_t *v0, char *buf, int decimals)
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
        *buf_ptr++ = ')';

        *buf_ptr = '\0';
}
