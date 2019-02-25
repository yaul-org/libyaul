/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <string.h>

#include <vdp1/env.h>
#include <vdp1/map.h>
#include <vdp1/cmdt.h>

#include <vdp2/tvmd.h>

#include "vdp-internal.h"

static inline void __always_inline _env_assert(const struct vdp1_env *);
static inline void __always_inline _env_erase_assert(const struct vdp1_env *);

void
vdp1_env_default_set(void)
{
        const struct vdp1_env *default_env;
        default_env = &_state_vdp1()->env;

        vdp1_env_set(default_env);
}

void
vdp1_env_set(const struct vdp1_env *env)
{
        _env_assert(env);

        /* Always clear TVM and VBE bits */
        _state_vdp1()->regs.tvmr = (env->env_rotation << 1) | env->env_bpp;

        _state_vdp1()->regs.ewdr = env->env_erase_color.raw;

        uint16_t x1;
        x1 = env->env_erase_points[0].x >> 3;
        /* Add 1 to compensate for the hardware substracting one after
         * shifting to the left by 3 */
        uint16_t x3;
        x3 = (env->env_erase_points[1].x + 1) >> 3;

        uint16_t y1;
        y1 = env->env_erase_points[0].y;
        uint16_t y3;
        y3 = env->env_erase_points[1].y;

        if (env->env_bpp == VDP1_ENV_BPP_8) {
                x1 >>= 1;
                x3 >>= 1;
        }

        if ((_state_vdp2()->regs.tvmd & 0xC0) == 0xC0) {
                y1 >>= 1;
                y3 >>= 1;
        }

        _state_vdp1()->regs.ewlr = (x1 << 9) | y1;
        _state_vdp1()->regs.ewrr = (x3 << 9) | y3;

        uint16_t spclmd;
        spclmd = env->env_color_mode << 5;

        _state_vdp2()->regs.spctl &= 0x3710;
        /* Disable sprite window (SPWINEN) when SPCLMD bit is set */
        _state_vdp2()->regs.spctl ^= (spclmd >> 1);
        _state_vdp2()->regs.spctl |= spclmd;

        /* Types 0x0 to 0x7 are for low resolution (320 or 352), and types 0x8
         * to 0xF are for high resolution (640 or 704).
         *
         * The frame buffer bit-depth are 16-bits and 8-bits, respectively */
        _state_vdp2()->regs.spctl |= env->env_sprite_type & 0x000F;

        /* Force stop drawing */
        MEMORY_WRITE(16, VDP1(PTMR), 0x0000);
        MEMORY_WRITE(16, VDP1(ENDR), 0x0000);

        MEMORY_WRITE(16, VDP1(TVMR), _state_vdp1()->regs.tvmr);
        MEMORY_WRITE(16, VDP1(EWDR), _state_vdp1()->regs.ewdr);
        MEMORY_WRITE(16, VDP1(EWLR), _state_vdp1()->regs.ewlr);
        MEMORY_WRITE(16, VDP1(EWRR), _state_vdp1()->regs.ewrr);
}

static inline void __always_inline
_env_assert(const struct vdp1_env *env)
{
        assert(env != NULL);

        assert((env->env_bpp == VDP1_ENV_BPP_8) || (env->env_bpp == VDP1_ENV_BPP_16));

        assert((env->env_rotation == VDP1_ENV_ROTATION_0) ||
               (env->env_rotation == VDP1_ENV_ROTATION_90));

        assert((env->env_color_mode == VDP1_ENV_COLOR_MODE_PALETTE) ||
               (env->env_color_mode == VDP1_ENV_COLOR_MODE_RGB_PALETTE));

        _env_erase_assert(env);

        assert((env->env_sprite_type >= 0x0) && (env->env_sprite_type <= 0xF));

        /* If frame buffer bit-depth is 8, only sprite types 0x8 to 0xF are
         * valid. If frame buffer bit-depth is 16, only sprite types 0x0 to 0x7
         * are valid */
        assert(((env->env_bpp == VDP1_ENV_BPP_8) && (env->env_sprite_type >= 0x8)) ||
               ((env->env_bpp == VDP1_ENV_BPP_16) && (env->env_sprite_type <= 0x7)));
}

static inline void __always_inline
_env_erase_assert(const struct vdp1_env *env)
{
        assert((env->env_erase_points[0].x >= 0) &&
               (env->env_erase_points[0].y >= 0));

        assert((env->env_erase_points[1].x > 0) &&
               (env->env_erase_points[1].y >= 0));

        assert((env->env_erase_points[0].x <= 1008) &&
               (env->env_erase_points[0].y < 512));

        assert((env->env_erase_points[1].x <= 1024) &&
               (env->env_erase_points[1].y < 512));

        assert((env->env_erase_points[0].x < env->env_erase_points[1].x) &&
               (env->env_erase_points[0].y <= env->env_erase_points[1].y));
}
