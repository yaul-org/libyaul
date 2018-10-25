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

/* Check if boundaries are correct */
static_assert((VDP1_CMDT_MEMORY_SIZE +
               VDP1_GST_MEMORY_SIZE +
               VDP1_TEXURE_MEMORY_SIZE +
               VDP1_CLUT_MEMORY_SIZE) == VDP1_VRAM_SIZE);

static void _init_vdp1(void);

static void _init_vdp2(void);

void
vdp_init(void)
{
        /* Reset all buffered registers and initialize to sane values */
        (void)memset(&_state_vdp1()->regs, 0x00, sizeof(_state_vdp1()->regs));
        (void)memset(&_state_vdp2()->regs, 0x00, sizeof(_state_vdp2()->regs));

        _init_vdp1();

        _init_vdp2();

        vdp_sync_init();

        uint8_t sr_mask;
        sr_mask = cpu_intc_mask_get();

        cpu_intc_mask_set(0);

        struct vdp1_cmdt cmdt_end;

        vdp1_cmdt_end(&cmdt_end);

        vdp1_sync_draw(&cmdt_end, 1);
        vdp2_sync_commit();
        vdp_sync(0);

        cpu_intc_mask_set(sr_mask);
}

static void
_init_vdp1(void)
{
        const struct vdp1_env env = {
                .env_color = COLOR_RGB555(0, 0, 0),
                .env_erase_points = {
                        INT16_VECTOR2_INITIALIZER(0, 0),
                        INT16_VECTOR2_INITIALIZER(320, 224)
                },
                .env_bpp = ENV_BPP_16,
                .env_rotation = ENV_ROTATION_0,
                .env_color_mode = ENV_COLOR_MODE_RGB_PALETTE
        };

        vdp1_env_set(&env);
}

static void
_init_vdp2(void)
{
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

        vdp2_sprite_type_set(0);
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

        struct dma_xfer *xfer;

        /* Write VDP2(TVMD) first */
        xfer = &_state_vdp2()->commit.xfer_table[COMMIT_XFER_VDP2_REG_TVMD];
        xfer->len = 2;
        xfer->dst = VDP2(0);
        xfer->src = CPU_CACHE_THROUGH | (uint32_t)&_state_vdp2()->regs.tvmd;

        /* Skip the first 8 VDP2 registers */
        xfer = &_state_vdp2()->commit.xfer_table[COMMIT_XFER_VDP2_REGS];
        xfer->len = sizeof(_state_vdp2()->regs) - 16;
        xfer->dst = VDP2(16);
        xfer->src = CPU_CACHE_THROUGH | (uint32_t)&_state_vdp2()->regs.buffer[8];

        xfer = &_state_vdp2()->commit.xfer_table[COMMIT_XFER_BACK_SCREEN_BUFFER];
        xfer->len = 0;
        xfer->dst = 0x00000000;
        xfer->src = 0x00000000;

        struct dma_level_cfg dma_level_cfg = {
                .dlc_mode = DMA_MODE_INDIRECT,
                .dlc_xfer.indirect = &_state_vdp2()->commit.xfer_table[0],
                .dlc_stride = DMA_STRIDE_2_BYTES,
                .dlc_update = DMA_UPDATE_NONE
        };

        void *reg_buffer;
        reg_buffer = &_state_vdp2()->commit.reg_buffer[0];

        scu_dma_config_buffer(reg_buffer, &dma_level_cfg);

        vdp2_commit_handler_set(NULL, NULL);

        vdp2_scrn_back_screen_color_set(VRAM_ADDR_4MBIT(0, 0x000000),
            COLOR_RGB555(0, 0, 0));

        vdp2_vram_cycp_clear();
}
