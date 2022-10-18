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

static void _vdp1_init(void);
static void _vdp2_init(void);

static vdp1_registers_t _vdp1_registers __aligned(16);
static vdp1_vram_partitions_t _vdp1_vram_partitions;

static vdp2_registers_t _vdp2_registers __aligned(16);

void
__vdp_init(void)
{
        /* Should only be internal to vdp_init.c */
        extern void __vdp_sync_init(void);

        /* VDP2 must be initialized first before VDP1 as the VDP1 writes to VDP2
         * registers */
        _vdp2_init();
        _vdp1_init();

        __vdp_sync_init();
}

static void
_vdp1_init(void)
{
        extern void __vdp1_env_init(void);

        _state_vdp1()->regs = &_vdp1_registers;
        _state_vdp1()->vram_partitions = &_vdp1_vram_partitions;

        (void)memset(_state_vdp1()->regs, 0x00, sizeof(vdp1_registers_t));

        __vdp1_env_init();

        vdp1_vram_partitions_set(VDP1_VRAM_DEFAULT_CMDT_COUNT,
                                 VDP1_VRAM_DEFAULT_TEXTURE_SIZE,
                                 VDP1_VRAM_DEFAULT_GOURAUD_COUNT,
                                 VDP1_VRAM_DEFAULT_CLUT_COUNT);

        cpu_dmac_memset(0, (void *)VDP1_VRAM(0x0000), 0x00000000, VDP1_VRAM_SIZE);

        for (uint32_t fb = 0; fb < VDP1_FB_COUNT; fb++) {
                /* Wait until the at the start of VBLANK-IN */
                vdp2_tvmd_vblank_in_next_wait(1);

                /* Change frame buffer */
                MEMORY_WRITE(16, VDP1(FBCR), 0x0003);

                /* Wait until the change of frame buffer takes effect */
                vdp2_tvmd_vcount_wait(0);

                cpu_dmac_memset(0, (void *)VDP1_FB(0x0000), 0x00000000, VDP1_FB_SIZE);
        }

        /* Force draw end */
        MEMORY_WRITE(16, VDP1_VRAM(0x0000), 0x8000);
}

static void
_vdp2_init(void)
{
        extern void __vdp2_vram_init(void);

        MEMORY_WRITE(16, VDP2(TVMD), 0x0000);

        _state_vdp2()->regs = &_vdp2_registers;

        (void)memset(_state_vdp2()->regs, 0x00, sizeof(vdp2_registers_t));

        _state_vdp2()->tv.resolution.x = 320;
        _state_vdp2()->tv.resolution.y = 224;

        vdp2_scrn_priority_set(VDP2_SCRN_NBG0, 1);
        vdp2_scrn_priority_set(VDP2_SCRN_NBG1, 1);
        vdp2_scrn_priority_set(VDP2_SCRN_NBG2, 1);
        vdp2_scrn_priority_set(VDP2_SCRN_NBG3, 1);
        vdp2_scrn_priority_set(VDP2_SCRN_RBG0, 1);

        vdp2_scrn_reduction_x_set(VDP2_SCRN_NBG0, Q0_3_8(1.0f));
        vdp2_scrn_reduction_y_set(VDP2_SCRN_NBG0, Q0_3_8(1.0f));

        vdp2_scrn_reduction_x_set(VDP2_SCRN_NBG1, Q0_3_8(1.0f));
        vdp2_scrn_reduction_y_set(VDP2_SCRN_NBG1, Q0_3_8(1.0f));

        vdp2_scrn_back_color_set(VDP2_VRAM_ADDR(3, 0x0001FFFE), COLOR_RGB1555(0, 0, 0, 0));

        __vdp2_vram_init();

        cpu_dmac_memset(0, (void *)VDP2_VRAM(0x0000), 0x00000000, VDP2_VRAM_SIZE);
        cpu_dmac_memset(0, (void *)VDP2_CRAM(0x0000), 0x00000000, VDP2_CRAM_SIZE);

        vdp2_cram_mode_set(1);
}
