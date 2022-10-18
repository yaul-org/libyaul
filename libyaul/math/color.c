/*
 * Copyright (c) 2013-2014
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <string.h>

#include "color.h"

void
color_rgb1555_rgb888_convert(const color_rgb1555_t *color, color_rgb888_t *result)
{
        result->r = (color->r << 3) | (color->r & 0x07);
        result->g = (color->g << 3) | (color->g & 0x07);
        result->b = (color->b << 3) | (color->b & 0x07);
}

void
color_rgb888_rgb1555_convert(const color_rgb888_t *color, color_rgb1555_t *result)
{
        result->r = (color->r >> 3) & 0x1F;
        result->g = (color->g >> 3) & 0x1F;
        result->b = (color->b >> 3) & 0x1F;
}

void
color_rgb888_fix16_rgb_convert(const color_rgb888_t *color __unused,
    color_fix16_rgb_t *result __unused)
{
        /* XXX: Not yet implemented */
}

void
color_fix16_rgb_rgb888_convert(const color_fix16_rgb_t *color,
    color_rgb888_t *result)
{
        result->r = fix16_int32_to(color->r * FIX16(255.0f)) & 0xFF;
        result->g = fix16_int32_to(color->g * FIX16(255.0f)) & 0xFF;
        result->b = fix16_int32_to(color->b * FIX16(255.0f)) & 0xFF;
}

void
color_fix16_hsv_rgb1555_convert(const color_fix16_hsv_t *color, color_rgb1555_t *result)
{
        color_fix16_rgb_t rgb;
        color_fix16_hsv_fix16_rgb_convert(color, &rgb);

        /* Convert from [0..1] to [0..255] scale */
        color_rgb888_t rgb888;
        color_fix16_rgb_rgb888_convert(&rgb, &rgb888);

        color_rgb888_rgb1555_convert(&rgb888, result);
}

void
color_fix16_hsv_rgb888_convert(const color_fix16_hsv_t *color,
    color_rgb888_t *result)
{
        color_fix16_rgb_t rgb;
        color_fix16_hsv_fix16_rgb_convert(color, &rgb);

        /* Convert from [0..1] to [0..255] scale */
        color_fix16_rgb_rgb888_convert(&rgb, result);
}

void
color_fix16_hsv_fix16_rgb_convert(const color_fix16_hsv_t *color __unused,
    color_fix16_rgb_t *result __unused)
{
        /* XXX: Not yet implemented */
}

void
color_rgb1555_fix16_hsv_convert(const color_rgb1555_t *color,
    color_fix16_hsv_t *result)
{
        /* Convert from RGB1555 to RGB888 */
        color_rgb888_t rgb888;
        color_rgb1555_rgb888_convert(color, &rgb888);

        color_rgb888_fix16_hsv_convert(&rgb888, result);
}

void
color_rgb888_fix16_hsv_convert(const color_rgb888_t *color,
    color_fix16_hsv_t *result)
{
        /* Convert from [0..255] to [0..1] scale */
        color_fix16_rgb_t rgb;
        color_rgb888_fix16_rgb_convert(color, &rgb);

        color_fix16_rgb_fix16_hsv_convert(&rgb, result);
}

void
color_fix16_rgb_fix16_hsv_convert(const color_fix16_rgb_t *color __unused,
    color_fix16_hsv_t *result __unused)
{
        /* XXX: Not yet implemented */
}

void
color_fix16_hsv_lerp8(const color_fix16_hsv_t *a, const color_fix16_hsv_t *b,
    uint8_t t, color_fix16_hsv_t *result)
{

        result->h = fix16_lerp8(a->h, b->h, t);
        result->s = fix16_lerp8(a->s, b->s, t);
        result->v = fix16_lerp8(a->v, b->v, t);
}

void
color_fix16_hsv_lerp(const color_fix16_hsv_t *a, const color_fix16_hsv_t *b,
    fix16_t t, color_fix16_hsv_t *result)
{
        result->h = fix16_lerp(a->h, b->h, t);
        result->s = fix16_lerp(a->s, b->s, t);
        result->v = fix16_lerp(a->v, b->v, t);
}

void
color_fix16_hsv_str(const color_fix16_hsv_t *c0, char *buffer, int decimals)
{
        char component_buffer[13] __aligned(16);
        size_t component_buffer_size;

        char *buffer_ptr;
        buffer_ptr = buffer;

        *buffer_ptr++ = '(';
        fix16_str(c0->h, component_buffer, decimals);
        component_buffer_size = strlen(component_buffer);
        (void)memcpy(buffer_ptr, component_buffer, component_buffer_size);
        buffer_ptr += component_buffer_size;
        *buffer_ptr++ = ',';

        fix16_str(c0->s, component_buffer, decimals);
        component_buffer_size = strlen(component_buffer);
        (void)memcpy(buffer_ptr, component_buffer, component_buffer_size);
        buffer_ptr += component_buffer_size;
        *buffer_ptr++ = ',';

        fix16_str(c0->v, component_buffer, decimals);
        component_buffer_size = strlen(component_buffer);
        (void)memcpy(buffer_ptr, component_buffer, component_buffer_size);
        buffer_ptr += component_buffer_size;
        *buffer_ptr++ = ')';

        *buffer_ptr = '\0';
}
