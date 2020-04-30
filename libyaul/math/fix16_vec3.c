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
fix16_vec3_normalize(fix16_vec3_t *result __unused)
{
        /* XXX: Not yet implemented */
}

void
fix16_vec3_normalized(const fix16_vec3_t *v0 __unused,
    fix16_vec3_t *result __unused)
{
        /* XXX: Not yet implemented */
}

fix16_t
fix16_vec3_length(const fix16_vec3_t *v0 __unused)
{
        return FIX16(0.0f);
}

fix16_t
fix16_vec3_dot(const fix16_vec3_t *a, const fix16_vec3_t *b)
{
        return fix16_vec3_inline_dot(a, b);
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
