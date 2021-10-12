/*
 * Copyright (c) 2012-2019 Israel Jacquez
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

static vdp1_env_t _default_env = {
        .erase_color = COLOR_RGB1555_INITIALIZER(0, 0, 0, 0),
        .erase_points[0] = {
                0,
                0
        },
        .erase_points[1] = {
                /* Updated during runtime */
                0,
                /* Updated during runtime */
                0
        },
        .bpp = VDP1_ENV_BPP_16,
        .rotation = VDP1_ENV_ROTATION_0,
        .color_mode = VDP1_ENV_COLOR_MODE_RGB_PALETTE,
        .sprite_type = 0x0
};

static vdp1_env_t _current_env;

#ifdef DEBUG
static inline void __always_inline _env_assert(const vdp1_env_t *);
static inline void __always_inline _env_erase_assert(const vdp1_env_t *);
#else
#define _env_assert(x)
#define _env_erase_assert(x)
#endif /* DEBUG */

static inline void __always_inline _env_current_update(const vdp1_env_t *);

static void _env_default_erase_update(void);

void
_internal_vdp1_env_init(void)
{
        _state_vdp1()->current_env = &_current_env;

        _env_current_update(&_default_env);

        vdp1_env_stop();
        vdp1_env_default_set();
}

void
vdp1_env_default_init(vdp1_env_t *env)
{
        assert(env != NULL);

        _env_default_erase_update();

        (void)memcpy(env, &_default_env, sizeof(vdp1_env_t));
}

void
vdp1_env_default_set(void)
{
        _env_default_erase_update();

        vdp1_env_set(&_default_env);
}

void
vdp1_env_set(const vdp1_env_t *env)
{
        _env_assert(env);

        _env_current_update(env);

        /* Always clear TVM and VBE bits */
        _state_vdp1()->regs->tvmr = (env->rotation << 1) | env->bpp;

        _state_vdp1()->regs->ewdr = env->erase_color.raw;

        uint16_t x1;
        x1 = env->erase_points[0].x >> 3;
        /* Add 1 to compensate for the hardware substracting one after
         * shifting to the left by 3 */
        uint16_t x3;
        x3 = (env->erase_points[1].x + 1) >> 3;

        uint16_t y1;
        y1 = env->erase_points[0].y;
        uint16_t y3;
        y3 = env->erase_points[1].y;

        if (env->bpp == VDP1_ENV_BPP_8) {
                x1 >>= 1;
                x3 >>= 1;
        }

        if ((_state_vdp2()->regs->tvmd & 0xC0) == 0xC0) {
                y1 >>= 1;
                y3 >>= 1;
        }

        _state_vdp1()->regs->ewlr = (x1 << 9) | y1;
        _state_vdp1()->regs->ewrr = (x3 << 9) | y3;

        const uint16_t spclmd = env->color_mode << 5;

        _state_vdp2()->regs->spctl &= 0xFFC0;

        if ((env->sprite_type < 0x8) && (env->bpp == VDP1_ENV_BPP_16)) {
                /* Disable sprite window (SPWINEN) when SPCLMD bit is set */
                _state_vdp2()->regs->spctl &= ~(spclmd >> 1);
                _state_vdp2()->regs->spctl |= spclmd;
        }

        /* Types 0x0 to 0x7 are for low resolution (320 or 352), and types 0x8
         * to 0xF are for high resolution (640 or 704).
         *
         * The frame buffer bit-depth are 16-bits and 8-bits, respectively */
        uint8_t sprite_type;
        sprite_type = env->sprite_type;

        if ((sprite_type < 0x8) && (env->bpp == VDP1_ENV_BPP_8)) {
                sprite_type += 8;
        }

        _state_vdp2()->regs->spctl |= sprite_type & 0x000F;

        MEMORY_WRITE(16, VDP1(TVMR), _state_vdp1()->regs->tvmr);
        MEMORY_WRITE(16, VDP1(EWDR), _state_vdp1()->regs->ewdr);
        MEMORY_WRITE(16, VDP1(EWLR), _state_vdp1()->regs->ewlr);
        MEMORY_WRITE(16, VDP1(EWRR), _state_vdp1()->regs->ewrr);
}

void
vdp1_env_stop(void)
{
        (void)memset(&_state_vdp1()->regs->buffer[0],
            0x00,
            sizeof(vdp1_registers_t));

        MEMORY_WRITE(16, VDP1(TVMR), 0x0000);
        MEMORY_WRITE(16, VDP1(PTMR), 0x0000);
        MEMORY_WRITE(16, VDP1(FBCR), 0x0000);
        MEMORY_WRITE(16, VDP1(ENDR), 0x0000);
}

void
vdp1_env_preamble_populate(vdp1_cmdt_t *cmdts,
    const int16_vec2_t *local_coords)
{
        assert(cmdts != NULL);

        int16_vec2_t stack_local_coords;

        if (local_coords == NULL) {
                local_coords = &stack_local_coords;

                stack_local_coords.x = _state_vdp2()->tv.resolution.x / 2;
                stack_local_coords.y = _state_vdp2()->tv.resolution.y / 2;
        }

        int16_vec2_t system_clip_coords;
        system_clip_coords.x = _state_vdp2()->tv.resolution.x;
        system_clip_coords.y = _state_vdp2()->tv.resolution.y;

        int16_vec2_t user_clip_ul;
        int16_vec2_zero(&user_clip_ul);

        int16_vec2_t user_clip_lr;
        user_clip_lr.x = system_clip_coords.x - 1;
        user_clip_lr.y = system_clip_coords.y - 1;

        vdp1_cmdt_t *cmdt;
        cmdt = &cmdts[0];

        vdp1_cmdt_system_clip_coord_set(cmdt);
        vdp1_cmdt_param_vertex_set(cmdt, CMDT_VTX_SYSTEM_CLIP, &system_clip_coords);
        cmdt++;

        vdp1_cmdt_user_clip_coord_set(cmdt);
        vdp1_cmdt_param_vertex_set(cmdt, CMDT_VTX_USER_CLIP_UL, &user_clip_ul);
        vdp1_cmdt_param_vertex_set(cmdt, CMDT_VTX_USER_CLIP_LR, &user_clip_lr);
        cmdt++;

        vdp1_cmdt_local_coord_set(cmdt);
        vdp1_cmdt_param_vertex_set(cmdt, CMDT_VTX_LOCAL_COORD, local_coords);
        cmdt++;

        vdp1_cmdt_end_set(cmdt);
}

static inline void __always_inline
_env_current_update(const vdp1_env_t *env) {
        (void)memcpy(&_current_env, env, sizeof(vdp1_env_t));
}

static void
_env_default_erase_update(void)
{
        /* If the system is PAL, or if there is a resolution change, update the
         * resolution */

        uint16_t width;
        width = _state_vdp2()->tv.resolution.x;

        uint16_t height;
        height = _state_vdp2()->tv.resolution.y;

        _default_env.erase_points[1].x = width;
        _default_env.erase_points[1].y = height;
}

#ifdef DEBUG
static inline void __always_inline
_env_assert(const vdp1_env_t *env)
{
        assert(env != NULL);

        assert((env->bpp == VDP1_ENV_BPP_8) || (env->bpp == VDP1_ENV_BPP_16));

        assert((env->rotation == VDP1_ENV_ROTATION_0) ||
               (env->rotation == VDP1_ENV_ROTATION_90));

        assert((env->color_mode == VDP1_ENV_COLOR_MODE_PALETTE) ||
               (env->color_mode == VDP1_ENV_COLOR_MODE_RGB_PALETTE));

        _env_erase_assert(env);

        assert((env->sprite_type >= 0x0) && (env->sprite_type <= 0xF));

        /* If frame buffer bit-depth is 8, only sprite types 0x8 to 0xF are
         * valid. If frame buffer bit-depth is 16, only sprite types 0x0 to 0x7
         * are valid */
        assert(((env->bpp == VDP1_ENV_BPP_8) && (env->sprite_type >= 0x8)) ||
               ((env->bpp == VDP1_ENV_BPP_16) && (env->sprite_type <= 0x7)));
}

static inline void __always_inline
_env_erase_assert(const vdp1_env_t *env)
{
        assert((env->erase_points[0].x >= 0) &&
               (env->erase_points[0].y >= 0));

        assert((env->erase_points[1].x > 0) &&
               (env->erase_points[1].y >= 0));

        assert((env->erase_points[0].x <= 1008) &&
               (env->erase_points[0].y < 512));

        assert((env->erase_points[1].x <= 1024) &&
               (env->erase_points[1].y < 512));

        assert((env->erase_points[0].x < env->erase_points[1].x) &&
               (env->erase_points[0].y <= env->erase_points[1].y));
}
#endif /* DEBUG */
