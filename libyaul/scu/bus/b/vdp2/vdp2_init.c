/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <bios.h>

#include <cpu/cache.h>
#include <cpu/intc.h>

#include <scu/ic.h>

#include <vdp2.h>

#include <string.h>

#include "vdp2-internal.h"

irq_mux_t _internal_vdp2_vblank_in_irq_mux;
irq_mux_t _internal_vdp2_vblank_out_irq_mux;

static void _vdp2_vblank_in(void);
static void _vdp2_vblank_out(void);

void
vdp2_init(void)
{
        extern void _internal_vdp2_commit_handler(void *);

        /* Avoid re-initializing by checking */
        static bool _initialized = false;

        /* Reset all state */
        _internal_state_vdp2.display_w = 0;
        _internal_state_vdp2.display_h = 0;
        _internal_state_vdp2.interlaced = TVMD_INTERLACE_NONE;

        _internal_state_vdp2.nbg0.scroll.x = F16(0.0f);
        _internal_state_vdp2.nbg0.scroll.y = F16(0.0f);
        _internal_state_vdp2.nbg1.scroll.x = F16(0.0f);
        _internal_state_vdp2.nbg1.scroll.y = F16(0.0f);
        _internal_state_vdp2.nbg2.scroll.x = 0;
        _internal_state_vdp2.nbg2.scroll.y = 0;
        _internal_state_vdp2.nbg3.scroll.x = 0;
        _internal_state_vdp2.nbg3.scroll.y = 0;

        (void)memset(&_internal_state_vdp2.nbg0.bitmap_format, 0x00,
            sizeof(struct scrn_bitmap_format));
        (void)memset(&_internal_state_vdp2.nbg0.cell_format, 0x00,
            sizeof(struct scrn_cell_format));
        (void)memset(&_internal_state_vdp2.nbg1.bitmap_format, 0x00,
            sizeof(struct scrn_bitmap_format));
        (void)memset(&_internal_state_vdp2.nbg1.cell_format, 0x00,
            sizeof(struct scrn_cell_format));
        (void)memset(&_internal_state_vdp2.nbg2.cell_format, 0x00,
            sizeof(struct scrn_cell_format));
        (void)memset(&_internal_state_vdp2.nbg3.cell_format, 0x00,
            sizeof(struct scrn_cell_format));

        _internal_state_vdp2.back.color = COLOR_RGB_DATA;
        _internal_state_vdp2.back.vram = VRAM_ADDR_4MBIT(0, 0x00000);
        _internal_state_vdp2.back.count = 1;

        /* Reset all buffered registers */
        memset(&_internal_state_vdp2.buffered_regs, 0x00,
            sizeof(_internal_state_vdp2.buffered_regs));

        _internal_state_vdp2.vram_ctl.vram_size = VRAM_CTL_SIZE_4MBIT;
        _internal_state_vdp2.vram_ctl.vram_mode = VRAM_CTL_MODE_PART_BANK_A | VRAM_CTL_MODE_PART_BANK_B;

        uint32_t vram_bank;
        for (vram_bank = 0; vram_bank < 4; vram_bank++) {
                _internal_state_vdp2.vram_ctl.vram_cycp.pt[vram_bank].t0 = VRAM_CTL_CYCP_NO_ACCESS;
                _internal_state_vdp2.vram_ctl.vram_cycp.pt[vram_bank].t1 = VRAM_CTL_CYCP_NO_ACCESS;
                _internal_state_vdp2.vram_ctl.vram_cycp.pt[vram_bank].t2 = VRAM_CTL_CYCP_NO_ACCESS;
                _internal_state_vdp2.vram_ctl.vram_cycp.pt[vram_bank].t3 = VRAM_CTL_CYCP_NO_ACCESS;
                _internal_state_vdp2.vram_ctl.vram_cycp.pt[vram_bank].t4 = VRAM_CTL_CYCP_NO_ACCESS;
                _internal_state_vdp2.vram_ctl.vram_cycp.pt[vram_bank].t5 = VRAM_CTL_CYCP_NO_ACCESS;
                _internal_state_vdp2.vram_ctl.vram_cycp.pt[vram_bank].t6 = VRAM_CTL_CYCP_NO_ACCESS;
                _internal_state_vdp2.vram_ctl.vram_cycp.pt[vram_bank].t7 = VRAM_CTL_CYCP_NO_ACCESS;
        }

        /* Initialize the buffered registers to sane values */

        /* RAM */
        /* Color RAM mode 1 */
        /* Partition both VRAM bank A and B by default */
        _internal_state_vdp2.buffered_regs.ramctl = 0x1300;

        _internal_state_vdp2.buffered_regs.cyca0l = 0xFFFF;
        _internal_state_vdp2.buffered_regs.cyca0u = 0xFFFF;
        _internal_state_vdp2.buffered_regs.cyca1l = 0xFFFF;
        _internal_state_vdp2.buffered_regs.cyca1u = 0xFFFF;
        _internal_state_vdp2.buffered_regs.cycb0l = 0xFFFF;
        _internal_state_vdp2.buffered_regs.cycb0u = 0xFFFF;
        _internal_state_vdp2.buffered_regs.cycb1l = 0xFFFF;
        _internal_state_vdp2.buffered_regs.cycb1u = 0xFFFF;

        /* Zoom */
        _internal_state_vdp2.buffered_regs.zmxin0 = 0x0001;
        _internal_state_vdp2.buffered_regs.zmxdn0 = 0x0000;
        _internal_state_vdp2.buffered_regs.zmyin0 = 0x0001;
        _internal_state_vdp2.buffered_regs.zmydn0 = 0x0000;
        _internal_state_vdp2.buffered_regs.zmxin1 = 0x0001;
        _internal_state_vdp2.buffered_regs.zmxdn1 = 0x0000;
        _internal_state_vdp2.buffered_regs.zmyin1 = 0x0001;
        _internal_state_vdp2.buffered_regs.zmydn1 = 0x0000;

        /* Priority function */
        _internal_state_vdp2.buffered_regs.prina = 0x0101;
        _internal_state_vdp2.buffered_regs.prinb = 0x0101;
        _internal_state_vdp2.buffered_regs.prir = 0x0001;

        /* Initialize */
        _internal_state_vdp2.buffered_regs.spctl = 0x0020;
        _internal_state_vdp2.buffered_regs.prisa = 0x0101;
        _internal_state_vdp2.buffered_regs.prisb = 0x0101;
        _internal_state_vdp2.buffered_regs.prisc = 0x0101;
        _internal_state_vdp2.buffered_regs.prisd = 0x0101;

        vdp2_tvmd_display_clear();

        if (_initialized) {
                return;
        }

        /* Initialized */
        _initialized = true;

        struct dma_xfer *dma_xfer_tbl;
        dma_xfer_tbl = &_internal_state_vdp2.commit_dma_xfer_tbl[0];

        /* Skip the first 8 VDP2 registers */
        dma_xfer_tbl[0].len = 2;
        dma_xfer_tbl[0].dst = VDP2(0);
        dma_xfer_tbl[0].src = CPU_CACHE_THROUGH |
            (uint32_t)&_internal_state_vdp2.buffered_regs.buffer[0];

        dma_xfer_tbl[1].len = sizeof(_internal_state_vdp2.buffered_regs) - 16;
        dma_xfer_tbl[1].dst = VDP2(16);
        dma_xfer_tbl[1].src = CPU_CACHE_THROUGH |
            (uint32_t)&_internal_state_vdp2.buffered_regs.buffer[8];

        dma_xfer_tbl[2].len = 1 * sizeof(color_rgb555_t);
        dma_xfer_tbl[2].dst = _internal_state_vdp2.back.vram;
        dma_xfer_tbl[2].src = DMA_INDIRECT_TBL_END | CPU_CACHE_THROUGH |
            (uint32_t)&_internal_state_vdp2.back.color;

        struct dma_level_cfg dma_level_cfg = {
                .dlc_mode = DMA_MODE_INDIRECT,
                .dlc_xfer.indirect = &_internal_state_vdp2.commit_dma_xfer_tbl[0],
                .dlc_stride = DMA_STRIDE_2_BYTES,
                .dlc_update = DMA_UPDATE_NONE,
                .dlc_starting_factor = DMA_START_FACTOR_ENABLE,
                .dlc_ihr = _internal_vdp2_commit_handler,
                .dlc_work = NULL
        };

        void *commit_dma_buffer;
        commit_dma_buffer = &_internal_state_vdp2.commit_dma_buffer[0];

        scu_dma_level_config_buffer(0, &dma_level_cfg, commit_dma_buffer);

        vdp2_commit_handler_set(NULL, NULL);

        irq_mux_init(&_internal_vdp2_vblank_in_irq_mux);
        irq_mux_init(&_internal_vdp2_vblank_out_irq_mux);

        /* Disable interrupts */
        uint32_t sr_mask;
        sr_mask = cpu_intc_mask_get();

        cpu_intc_mask_set(15);

        uint32_t scu_mask;
        scu_mask = IC_MASK_VBLANK_IN |
                   IC_MASK_VBLANK_OUT;

        scu_ic_mask_chg(IC_MASK_ALL, scu_mask);

        scu_ic_ihr_set(IC_INTERRUPT_VBLANK_IN, &_vdp2_vblank_in);
        scu_ic_ihr_set(IC_INTERRUPT_VBLANK_OUT, &_vdp2_vblank_out);

        scu_ic_mask_chg(~scu_mask, IC_MASK_NONE);

        /* Enable interrupts */
        cpu_intc_mask_set(sr_mask);
}

static void
_vdp2_vblank_in(void)
{
        irq_mux_handle(&_internal_vdp2_vblank_in_irq_mux);
}

static void
_vdp2_vblank_out(void)
{
        irq_mux_handle(&_internal_vdp2_vblank_out_irq_mux);
}
