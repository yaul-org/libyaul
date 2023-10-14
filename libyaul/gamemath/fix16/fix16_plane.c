/*
 * Copyright (c)
 * See LICENSE for details.
 *
 * Mattias Jansson
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <string.h>

#include <gamemath/fix16/fix16_plane.h>

size_t
fix16_plane_str(const fix16_plane_t *plane, char *buffer, int32_t decimals)
{
    char *buffer_ptr;
    buffer_ptr = buffer;

    buffer_ptr += fix16_vec3_str(&plane->normal, buffer_ptr, decimals);
    *buffer_ptr++ = ',';
    buffer_ptr += fix16_vec3_str(&plane->d, buffer_ptr, decimals);

    return (buffer_ptr - buffer);
}
