/*
 * Copyright (c) 2013-2014
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <string.h>

#include "color.h"

void
color_rgb555_rgb888_convert(const color_rgb555_t *color, color_rgb888_t *result)
{
        result->r = (color->r << 3) | (color->r & 0x07);
        result->g = (color->g << 3) | (color->g & 0x07);
        result->b = (color->b << 3) | (color->b & 0x07);
}

void
color_rgb888_fix16_rgb_convert(const color_rgb888_t *color,
    color_fix16_rgb_t *result)
{
        result->r = fix16_div(fix16_from_int(color->r), F16(255.0f));
        result->g = fix16_div(fix16_from_int(color->g), F16(255.0f));
        result->b = fix16_div(fix16_from_int(color->b), F16(255.0f));
}

void
color_rgb555_hsv_convert(const color_rgb555_t *color, color_fix16_hsv_t *result)
{
        /* Convert from RGB555 to RGB888 */
        color_rgb888_t rgb888;
        color_rgb555_rgb888_convert(color, &rgb888);

        color_rgb888_hsv_convert(&rgb888, result);
}

void
color_rgb888_hsv_convert(const color_rgb888_t *color, color_fix16_hsv_t *result)
{
        /* Convert from [0..255] to [0..1] scale */
        color_fix16_rgb_t rgb;
        color_rgb888_fix16_rgb_convert(color, &rgb);

        color_fix16_rgb_hsv_convert(&rgb, result);
}

void
color_fix16_rgb_hsv_convert(const color_fix16_rgb_t *color,
    color_fix16_hsv_t *result)
{
        fix16_t min;
        fix16_t max;
        fix16_t delta;

        min = color_fix16_rgb_min(color);
        max = color_fix16_rgb_max(color);

        result->v = max;

        delta = fix16_sub(max, min);
        if (delta < F16(0.0000153f)) {
                result->s = F16(0.0f);
                /* Undefined, maybe NAN? */
                result->h = F16(0.0f);
                return;
        }

        if (max <= F16(0.0f)) {
                /* If max is 0, then R = G = B = 0. So, s = 0, v is
                 * undefined */
                result->s = F16(0.0f);
                result->h = F16(0.0f);
                return;
        }

        result->s = fix16_div(delta, max);

        if (color->r >= max) {
                /* Between yellow & magenta */
                result->h = fix16_div(fix16_sub(color->g, color->b), delta);
        } else {
                if (color->g >= max) {
                        /* Between cyan & yellow */
                        result->h = fix16_add(F16(2.0f),
                            fix16_div(fix16_sub(color->b, color->r), delta));
                } else {
                        /* Between magenta & cyan */
                        result->h = fix16_add(F16(4.0f),
                            fix16_div(fix16_sub(color->r, color->g), delta));
                }
        }

        result->h = fix16_mul(result->h, F16(60.0f));

        if (result->h < F16(0.0f)) {
                result->h = fix16_add(result->h, F16(360.0f));
        }
}

void
fix16_color_hsv_rgb_convert(const color_rgb555_t *color,
    color_fix16_hsv_t *result)
{
}

void
color_hsv_lerp(const color_fix16_hsv_t *a, const color_fix16_hsv_t *b,
    fix16_t t, color_fix16_hsv_t *result)
{
}

void
color_hsv_str(const color_fix16_hsv_t *c0, char *buf, int32_t decimals)
{
        char component_buf[13] __aligned(16);
        size_t component_buf_len;

        char *buf_ptr;
        buf_ptr = buf;

        *buf_ptr++ = '(';
        fix16_to_str(c0->h, component_buf, decimals);
        component_buf_len = strlen(component_buf);
        (void)memcpy(buf_ptr, component_buf, component_buf_len);
        buf_ptr += component_buf_len;
        *buf_ptr++ = ',';

        fix16_to_str(c0->s, component_buf, decimals);
        component_buf_len = strlen(component_buf);
        (void)memcpy(buf_ptr, component_buf, component_buf_len);
        buf_ptr += component_buf_len;
        *buf_ptr++ = ',';

        fix16_to_str(c0->v, component_buf, decimals);
        component_buf_len = strlen(component_buf);
        (void)memcpy(buf_ptr, component_buf, component_buf_len);
        buf_ptr += component_buf_len;
        *buf_ptr++ = ')';

        *buf_ptr = '\0';
}
