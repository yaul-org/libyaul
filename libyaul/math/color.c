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
color_rgb888_rgb555_convert(const color_rgb888_t *color, color_rgb555_t *result)
{
        result->r = (color->r >> 3) & 0x1F;
        result->g = (color->g >> 3) & 0x1F;
        result->b = (color->b >> 3) & 0x1F;
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
color_fix16_rgb_rgb888_convert(const color_fix16_rgb_t *color,
    color_rgb888_t *result)
{
        result->r = fix16_to_int(fix16_mul(color->r, F16(255.0f))) & 0xFF;
        result->g = fix16_to_int(fix16_mul(color->g, F16(255.0f))) & 0xFF;
        result->b = fix16_to_int(fix16_mul(color->b, F16(255.0f))) & 0xFF;
}

void
color_hsv_rgb555_convert(const color_fix16_hsv_t *color, color_rgb555_t *result)
{
        color_fix16_rgb_t rgb;
        color_hsv_fix16_rgb_convert(color, &rgb);

        /* Convert from [0..1] to [0..255] scale */
        color_rgb888_t rgb888;
        color_fix16_rgb_rgb888_convert(&rgb, &rgb888);

        color_rgb888_rgb555_convert(&rgb888, result);
}

void
color_hsv_rgb888_convert(const color_fix16_hsv_t *color, color_rgb888_t *result)
{
        color_fix16_rgb_t rgb;
        color_hsv_fix16_rgb_convert(color, &rgb);

        /* Convert from [0..1] to [0..255] scale */
        color_fix16_rgb_rgb888_convert(&rgb, result);
}

void
color_hsv_fix16_rgb_convert(const color_fix16_hsv_t *color,
    color_fix16_rgb_t *result)
{
        fix16_t c;
        c = fix16_mul(color->v, color->s);

        fix16_t x;
        /* X=C*(1-|((H/60.0)%2)-1|) */
        x = fix16_mul(c, fix16_sub(F16(1.0f), fix16_abs(fix16_sub(fix16_mod(
                                fix16_div(color->h, F16(60.0f)),
                                F16(2.0f)), F16(1.0f)))));

        fix16_t m;
        m = fix16_sub(color->v, c);

        color_fix16_rgb_t rgb;
        rgb.r = F16(0.0f);
        rgb.g = F16(0.0f);
        rgb.b = F16(0.0f);

        if ((color->h >= F16(0.0f)) && (color->h < F16(60.0f))) {
                rgb.r = c;
                rgb.g = x;
                rgb.b = F16(0.0f);
        } else if ((color->h >= F16(60.0f)) && (color->h < F16(120.0f))) {
                rgb.r = x;
                rgb.g = c;
                rgb.b = F16(0.0f);
        } else if ((color->h >= F16(120.0f)) && (color->h < F16(180.0f))) {
                rgb.r = F16(0.0f);
                rgb.g = c;
                rgb.b = x;
        } else if ((color->h >= F16(180.0f)) && (color->h < F16(240.0f))) {
                rgb.r = F16(0.0f);
                rgb.g = x;
                rgb.b = c;
        } else if ((color->h >= F16(240.0f)) && (color->h < F16(300.0f))) {
                rgb.r = x;
                rgb.g = F16(0.0f);
                rgb.b = c;
        } else if ((color->h >= F16(300.0f)) && (color->h < F16(360.0f))) {
                rgb.r = c;
                rgb.g = F16(0.0f);
                rgb.b = x;
        }

        result->r = fix16_add(rgb.r, m);
        result->g = fix16_add(rgb.g, m);
        result->b = fix16_add(rgb.b, m);
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
