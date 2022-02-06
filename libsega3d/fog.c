/*
 * Copyright (c) 2020
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <string.h>

#include "sega3d-internal.h"

#define DEPTH_FOG_START         FIX16(1.f)
#define DEPTH_FOG_END           FIX16(512.0f)
#define DEPTH_FOG_DEPTH_COUNT   (64)
#define DEPTH_FOG_STEP          FIX16(1 / 8.0f)
#define DEPTH_FOG_COLOR_COUNT   (32)

static const color_rgb1555_t _depth_fog_colors[DEPTH_FOG_COLOR_COUNT] = {
        COLOR_RGB1555(1,  0, 16, 16),
        COLOR_RGB1555(1,  1, 16, 16),
        COLOR_RGB1555(1,  2, 16, 16),
        COLOR_RGB1555(1,  3, 16, 16),
        COLOR_RGB1555(1,  4, 16, 16),
        COLOR_RGB1555(1,  5, 16, 16),
        COLOR_RGB1555(1,  6, 16, 16),
        COLOR_RGB1555(1,  7, 16, 16),
        COLOR_RGB1555(1,  8, 16, 16),
        COLOR_RGB1555(1,  9, 16, 16),
        COLOR_RGB1555(1, 10, 16, 16),
        COLOR_RGB1555(1, 11, 16, 16),
        COLOR_RGB1555(1, 12, 16, 16),
        COLOR_RGB1555(1, 13, 16, 16),
        COLOR_RGB1555(1, 14, 16, 16),
        COLOR_RGB1555(1, 15, 16, 16),
        COLOR_RGB1555(1, 16, 16, 16),
        COLOR_RGB1555(1, 17, 16, 16),
        COLOR_RGB1555(1, 18, 16, 16),
        COLOR_RGB1555(1, 19, 16, 16),
        COLOR_RGB1555(1, 20, 16, 16),
        COLOR_RGB1555(1, 21, 16, 16),
        COLOR_RGB1555(1, 22, 16, 16),
        COLOR_RGB1555(1, 23, 16, 16),
        COLOR_RGB1555(1, 24, 16, 16),
        COLOR_RGB1555(1, 25, 16, 16),
        COLOR_RGB1555(1, 26, 16, 16),
        COLOR_RGB1555(1, 27, 16, 16),
        COLOR_RGB1555(1, 28, 16, 16),
        COLOR_RGB1555(1, 29, 16, 16),
        COLOR_RGB1555(1, 30, 16, 16),
        COLOR_RGB1555(1, 31, 16, 16)
};

static const uint8_t _depth_fog_z[DEPTH_FOG_DEPTH_COUNT] = {
         0,  0,  0,  0,  0,  0,  1,  1,  2,  2,  3,  3,  4,  5,  5,  6,
         7,  8,  9, 10, 11, 11, 12, 13, 14, 15, 16, 17, 17, 18, 19, 20,
        21, 21, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 27, 28, 28,
        28, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 30, 30, 30, 31
};

static const sega3d_fog_t default_fog = {
        .depth_colors = _depth_fog_colors,
        .depth_z = _depth_fog_z,
        .depth_count = DEPTH_FOG_DEPTH_COUNT,
        .step = DEPTH_FOG_STEP,
        .start_z = DEPTH_FOG_START,
        .end_z = DEPTH_FOG_END,
        .near_ambient_color = _depth_fog_colors[0],
        .far_ambient_color = _depth_fog_colors[31],
};

void
__fog_init(void)
{
        sega3d_fog_set(&default_fog);
}

void
sega3d_fog_set(const sega3d_fog_t *fog)
{
        __state->flags &= ~FLAGS_FOG_ENABLED;

        if (fog == NULL) {
                return;
        }

        assert(fog->depth_colors != NULL);
        assert(fog->depth_z != NULL);
        assert(fog->depth_count > 0);

        (void)memcpy(__state->fog, fog, sizeof(sega3d_fog_t));

        __state->flags |= FLAGS_FOG_ENABLED;
}

void
sega3d_fog_limits_set(FIXED start_z, FIXED end_z)
{
        __state->fog->start_z = start_z;
        __state->fog->end_z = end_z;
}
