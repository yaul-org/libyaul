/*
 * Copyright (c) 2013-2014
 * See LICENSE for details.
 *
 * Mattias Jansson
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <string.h>

#include "fix16.h"

uint32_t
fix16_plane_str(const fix16_plane_t *plane, char *buffer, int decimals)
{
        char *buffer_ptr;
        buffer_ptr = buffer;

        const uint32_t normal_len =
            fix16_vec3_str(&plane->normal, buffer_ptr, decimals);

        buffer_ptr += normal_len - 1;

        const uint32_t d_len =
            fix16_vec3_str(&plane->d, buffer_ptr, decimals);

        return (normal_len + d_len);
}
