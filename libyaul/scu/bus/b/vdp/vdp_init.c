/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <bios.h>

#include <cpu/cache.h>
#include <cpu/dmac.h>
#include <cpu/intc.h>

#include <vdp.h>

#include <string.h>

#include "vdp-internal.h"

static void _init_vdp1(void);
static void _memory_area_clear(const uint32_t, const uint16_t, const uint32_t);

static void _init_vdp2(void);

void
_internal_vdp_init(void)
{
        /* Should only be internal to vdp_init.c */
        extern void _internal_vdp_sync_init(void);

        /* VDP2 must be initialized first before VDP1 as the VDP1 writes to VDP2
         * registers */
        _init_vdp2();
        _init_vdp1();

        _internal_vdp_sync_init();
}

static void
_init_vdp1(void)
{
        (void)memset(&_state_vdp1()->regs, 0x00, sizeof(_state_vdp1()->regs));

        vdp1_env_init();
        vdp1_env_default_set();

        vdp1_vram_partitions_set(VDP1_VRAM_DEFAULT_CMDT_COUNT,
            VDP1_VRAM_DEFAULT_TEXTURE_SIZE,
            VDP1_VRAM_DEFAULT_GOURAUD_COUNT,
            VDP1_VRAM_DEFAULT_CLUT_COUNT);

        MEMORY_WRITE(16, VDP1(TVMR), 0x0000);
        MEMORY_WRITE(16, VDP1(PTMR), 0x0000);
        MEMORY_WRITE(16, VDP1(ENDR), 0x0000);

        _memory_area_clear(VDP1_VRAM(0x0000), 0x8000, VDP1_VRAM_SIZE);
        _memory_area_clear(VDP1_FB(0x0000), 0x0000, VDP1_FB_SIZE);
}

static void
_memory_area_clear(const uint32_t address, const uint16_t value,
    const uint32_t len)
{
        static cpu_dmac_cfg_t dmac_cfg = {
                .channel = 0,
                .src_mode = CPU_DMAC_SOURCE_FIXED,
                .src = 0x00000000,
                .dst = 0x00000000,
                .dst_mode = CPU_DMAC_DESTINATION_INCREMENT,
                .len = 0x00000000,
                .stride = CPU_DMAC_STRIDE_2_BYTES,
                .bus_mode = CPU_DMAC_BUS_MODE_CYCLE_STEAL,
                .ihr = NULL
        };

        dmac_cfg.dst = CPU_CACHE_THROUGH | address;
        dmac_cfg.src = CPU_CACHE_THROUGH | (uint32_t)&value;
        dmac_cfg.len = len;

        cpu_dmac_channel_wait(0);
        cpu_dmac_channel_config_set(&dmac_cfg);
        cpu_dmac_channel_start(0);
        cpu_dmac_channel_wait(0);
}

static void
_init_vdp2(void)
{
        static const vdp2_vram_ctl_t vram_ctl = {
                .vram_mode = VDP2_VRAM_CTL_MODE_PART_BANK_BOTH
        };

        (void)memset(&_state_vdp2()->regs, 0x00, sizeof(_state_vdp2()->regs));

        _state_vdp2()->tv.resolution.x = 0;
        _state_vdp2()->tv.resolution.y = 0;

        vdp2_tvmd_display_clear();

        vdp2_scrn_priority_set(VDP2_SCRN_NBG0, 1);
        vdp2_scrn_priority_set(VDP2_SCRN_NBG1, 1);
        vdp2_scrn_priority_set(VDP2_SCRN_NBG2, 1);
        vdp2_scrn_priority_set(VDP2_SCRN_NBG3, 1);
        vdp2_scrn_priority_set(VDP2_SCRN_RBG0, 1);

        vdp2_scrn_reduction_x_set(VDP2_SCRN_NBG0, Q0_3_8(1.0f));
        vdp2_scrn_reduction_y_set(VDP2_SCRN_NBG0, Q0_3_8(1.0f));

        vdp2_scrn_reduction_x_set(VDP2_SCRN_NBG1, Q0_3_8(1.0f));
        vdp2_scrn_reduction_y_set(VDP2_SCRN_NBG1, Q0_3_8(1.0f));

        vdp2_vram_control_set(&vram_ctl);

        vdp2_cram_mode_set(1);

        vdp2_scrn_back_screen_color_set(VDP2_VRAM_ADDR(0, 0x000000),
            COLOR_RGB1555(1, 0, 0, 0));

        vdp2_vram_cycp_clear();

        _memory_area_clear(VDP2_VRAM(0x0000), 0x0000, VDP2_VRAM_SIZE);
        _memory_area_clear(VDP2_CRAM(0x0000), 0x0000, VDP2_CRAM_SIZE);

        vdp2_tvmd_display_res_set(VDP2_TVMD_INTERLACE_NONE, VDP2_TVMD_HORZ_NORMAL_A,
            VDP2_TVMD_VERT_224);
}
