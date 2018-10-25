/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <vdp1/env.h>
#include <vdp1/map.h>

#include "vdp-internal.h"

void
vdp1_env_set(const struct vdp1_env *env)
{
        assert(env != NULL);

        assert((env->env_erase_points[0].x >= 0) &&
               (env->env_erase_points[0].y >= 0));

        assert((env->env_erase_points[1].x >= 0) &&
               (env->env_erase_points[1].y >= 0));

        assert((env->env_erase_points[0].x < 1024) &&
               (env->env_erase_points[0].y < 512));

        assert((env->env_erase_points[1].x < 1024) &&
               (env->env_erase_points[1].y < 512));

        assert((env->env_erase_points[0].x < env->env_erase_points[1].x) &&
               (env->env_erase_points[0].y <= env->env_erase_points[1].y));

        assert((env->env_color_mode == ENV_COLOR_MODE_PALETTE) ||
               (env->env_color_mode == ENV_COLOR_MODE_RGB_PALETTE));

        _state_vdp1()->regs.ewdr = env->env_color.raw;

        _state_vdp1()->regs.ewlr = ((env->env_erase_points[0].x >> 3) << 9) |
                                   env->env_erase_points[0].y;
        _state_vdp1()->regs.ewrr = (((env->env_erase_points[1].x - 1) >> 3) << 9) |
                                   env->env_erase_points[1].y;

        _state_vdp2()->regs.spctl &= 0x371F;
        _state_vdp2()->regs.spctl |= env->env_color_mode << 5;

        /* Force stop drawing */
        MEMORY_WRITE(16, VDP1(PTMR), 0x0000);
        MEMORY_WRITE(16, VDP1(ENDR), 0x0000);

        MEMORY_WRITE(16, VDP1(TVMR), _state_vdp1()->regs.tvmr);
        MEMORY_WRITE(16, VDP1(EWDR), _state_vdp1()->regs.ewdr);
        MEMORY_WRITE(16, VDP1(EWLR), _state_vdp1()->regs.ewlr);
        MEMORY_WRITE(16, VDP1(EWRR), _state_vdp1()->regs.ewrr);
}
