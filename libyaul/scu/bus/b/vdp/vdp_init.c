/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <bios.h>

#include <cpu/cache.h>
#include <cpu/intc.h>

#include <vdp.h>

#include <string.h>

#include "vdp-internal.h"

static void _init_vdp1(void);

static void _init_vdp2(void);

void
vdp_init(void)
{
        /* Reset all buffered registers and initialize to sane values */
        (void)memset(&_state_vdp1()->regs, 0x00, sizeof(_state_vdp1()->regs));
        (void)memset(&_state_vdp2()->regs, 0x00, sizeof(_state_vdp2()->regs));

        vdp_sync_init();

        _init_vdp1();

        _init_vdp2();

        uint8_t sr_mask;
        sr_mask = cpu_intc_mask_get();

        cpu_intc_mask_set(0);

        struct vdp1_cmdt_list cmdt_list;
        struct vdp1_cmdt cmdt;

        vdp1_cmdt_list_init(&cmdt_list, &cmdt, 1);

        vdp1_cmdt_end(&cmdt_list);

        vdp1_sync_draw(&cmdt_list);
        vdp_sync(0);

        cpu_intc_mask_set(sr_mask);
}

static void
_init_vdp1(void)
{
        _state_vdp1()->tv.interlace_pass = 0;

        struct vdp1_env *default_env;
        default_env = &_state_vdp1()->env;

        default_env->env_erase_color = COLOR_RGB555(0, 0, 0);
        default_env->env_erase_points[0].x = 0;
        default_env->env_erase_points[0].y = 0;
        default_env->env_erase_points[1].x = 320;
        default_env->env_erase_points[1].y = 224;
        default_env->env_bpp = ENV_BPP_16;
        default_env->env_rotation = ENV_ROTATION_0;
        default_env->env_color_mode = ENV_COLOR_MODE_RGB_PALETTE;
        default_env->env_limits.cmdt_count = 2048;
        default_env->env_limits.texture_size = 0x0006BFE0;
        default_env->env_limits.gouraud_count = 1024;
        default_env->env_limits.clut_count = 256;

        vdp1_env_default_set();
}

static void
_init_vdp2(void)
{
        _state_vdp2()->tv.resolution.x = 0;
        _state_vdp2()->tv.resolution.y = 0;
        _state_vdp2()->tv.interlace = TVMD_INTERLACE_NONE;

        vdp2_tvmd_display_clear();

        vdp2_scrn_scroll_x_set(SCRN_NBG0, F16(0.0f));
        vdp2_scrn_scroll_y_set(SCRN_NBG0, F16(0.0f));
        vdp2_scrn_scroll_x_set(SCRN_NBG1, F16(0.0f));
        vdp2_scrn_scroll_y_set(SCRN_NBG1, F16(0.0f));
        vdp2_scrn_scroll_x_set(SCRN_NBG2, F16(0.0f));
        vdp2_scrn_scroll_y_set(SCRN_NBG2, F16(0.0f));
        vdp2_scrn_scroll_x_set(SCRN_NBG1, F16(0.0f));
        vdp2_scrn_scroll_y_set(SCRN_NBG1, F16(0.0f));

        vdp2_scrn_priority_set(SCRN_NBG0, 1);
        vdp2_scrn_priority_set(SCRN_NBG1, 1);
        vdp2_scrn_priority_set(SCRN_NBG2, 1);
        vdp2_scrn_priority_set(SCRN_NBG3, 1);
        vdp2_scrn_priority_set(SCRN_RBG0, 1);

        vdp2_sprite_type_set(0x0);
        vdp2_sprite_priority_set(0, 0);
        vdp2_sprite_priority_set(1, 0);
        vdp2_sprite_priority_set(2, 0);
        vdp2_sprite_priority_set(3, 0);
        vdp2_sprite_priority_set(4, 0);
        vdp2_sprite_priority_set(5, 0);
        vdp2_sprite_priority_set(6, 0);
        vdp2_sprite_priority_set(7, 0);

        vdp2_scrn_reduction_x_set(SCRN_NBG0, Q0_3_8(1.0f));
        vdp2_scrn_reduction_y_set(SCRN_NBG0, Q0_3_8(1.0f));

        vdp2_scrn_reduction_x_set(SCRN_NBG1, Q0_3_8(1.0f));
        vdp2_scrn_reduction_y_set(SCRN_NBG1, Q0_3_8(1.0f));

        const struct vram_ctl vram_ctl = {
                .cram_mode = VRAM_CTL_CRAM_MODE_1,
                .vram_size = VRAM_CTL_SIZE_4MBIT,
                .vram_mode = VRAM_CTL_MODE_PART_BANK_BOTH
        };

        vdp2_vram_control_set(&vram_ctl);

        vdp2_scrn_back_screen_color_set(VRAM_ADDR_4MBIT(0, 0x000000),
            COLOR_RGB555(0, 0, 0));

        vdp2_vram_cycp_clear();

        vdp2_tvmd_display_res_set(TVMD_INTERLACE_NONE, TVMD_HORZ_NORMAL_A,
            TVMD_VERT_224);
}
