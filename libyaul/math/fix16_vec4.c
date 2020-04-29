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
fix16_vec4_normalize(fix16_vec4_t *result __unused)
{
        /* XXX: Not yet implemented */
}

void
fix16_vec4_normalized(const fix16_vec4_t * restrict v0 __unused,
    fix16_vec4_t * restrict result __unused)
{
        /* XXX: Not yet implemented */
}

fix16_t
fix16_vec4_length(const fix16_vec4_t *v0 __unused)
{
        /* XXX: Not yet implemented */
        return FIX16(0.0f);
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
