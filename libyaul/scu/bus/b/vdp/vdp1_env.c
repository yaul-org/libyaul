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
    .erase_color     = RGB1555_INITIALIZER(0, 0, 0, 0),
    .erase_points[0] = INT16_VEC2_INITIALIZER(0, 0),
    .erase_points[1] = INT16_VEC2_INITIALIZER(0, 0),
    .bpp             = VDP1_ENV_BPP_16,
    .rotation        = VDP1_ENV_ROTATION_0,
    .color_mode      = VDP1_ENV_COLOR_MODE_RGB_PALETTE,
    .sprite_type     = 0
};

static vdp1_env_t _current_env;

static inline void __always_inline _env_current_update(const vdp1_env_t *env);

static void _env_default_erase_update(void);

void
__vdp1_env_init(void)
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

const vdp1_env_t *
vdp1_env_get(void)
{
    return &_current_env;
}

void
vdp1_env_set(const vdp1_env_t *env)
{
    assert(env != NULL);

    _env_current_update(env);

    /* Always clear TVM and VBE bits */
    _state_vdp1()->shadow_ioregs.tvmr = (env->rotation << 1) | env->bpp;

    _state_vdp1()->shadow_ioregs.ewdr = env->erase_color.raw;

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

    if ((_state_vdp2()->shadow_regs.tvmd & 0xC0) == 0xC0) {
        y1 >>= 1;
        y3 >>= 1;
    }

    _state_vdp1()->shadow_ioregs.ewlr = (x1 << 9) | y1;
    _state_vdp1()->shadow_ioregs.ewrr = (x3 << 9) | y3;

    const uint16_t spclmd = env->color_mode << 5;

    _state_vdp2()->shadow_regs.spctl &= 0xFFC0;

    if ((env->sprite_type < 0x8) && (env->bpp == VDP1_ENV_BPP_16)) {
        /* Disable sprite window (SPWINEN) when SPCLMD bit is set */
        _state_vdp2()->shadow_regs.spctl &= ~(spclmd >> 1);
        _state_vdp2()->shadow_regs.spctl |= spclmd;
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

    _state_vdp2()->shadow_regs.spctl |= sprite_type & 0x000F;

    volatile vdp1_ioregs_t * const vdp1_ioregs = (volatile vdp1_ioregs_t *)VDP1_IOREG_BASE;

    vdp1_ioregs->tvmr = _state_vdp1()->shadow_ioregs.tvmr;
    vdp1_ioregs->ewdr = _state_vdp1()->shadow_ioregs.ewdr;
    vdp1_ioregs->ewlr = _state_vdp1()->shadow_ioregs.ewlr;
    vdp1_ioregs->ewrr = _state_vdp1()->shadow_ioregs.ewrr;
}

void
vdp1_env_stop(void)
{
    (void)memset(_state_vdp1()->shadow_ioregs.buffer, 0x00,
      sizeof(vdp1_ioregs_t));

    volatile vdp1_ioregs_t * const vdp1_ioregs = (volatile vdp1_ioregs_t *)VDP1_IOREG_BASE;

    vdp1_ioregs->tvmr = 0x0000;
    vdp1_ioregs->ptmr = 0x0000;
    vdp1_ioregs->fbcr = 0x0000;
    vdp1_ioregs->tvmr = 0x0000;
}

void
vdp1_env_preamble_populate(vdp1_cmdt_t *cmdts, const vdp1_env_coords_t *env_coords)
{
    assert(cmdts != NULL);

    vdp1_cmdt_t *cmdt;
    cmdt = cmdts;

    vdp1_cmdt_system_clip_coord_set(cmdt);
    vdp1_cmdt_vtx_system_clip_coord_set(cmdt, env_coords->system_clip);
    cmdt++;

    vdp1_cmdt_user_clip_coord_set(cmdt);
    vdp1_cmdt_vtx_user_clip_coord_set(cmdt, env_coords->user_clip_ul,
      env_coords->user_clip_lr);
    cmdt++;

    vdp1_cmdt_local_coord_set(cmdt);
    vdp1_cmdt_vtx_local_coord_set(cmdt, env_coords->local);
    cmdt++;

    vdp1_cmdt_end_set(cmdt);
}

static inline void __always_inline
_env_current_update(const vdp1_env_t *env)
{
    assert(env != NULL);

    (void)memcpy(&_current_env, env, sizeof(vdp1_env_t));
}

static void
_env_default_erase_update(void)
{
    /* If the system is PAL, or if there is a resolution change, update the
     * resolution */

    const uint16_t width = _state_vdp2()->tv.resolution.x;
    const uint16_t height = _state_vdp2()->tv.resolution.y;

    _default_env.erase_points[1].x = width;
    _default_env.erase_points[1].y = height;
}
