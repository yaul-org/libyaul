/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <cpu.h>
#include <scu/ic.h>
#include <vdp2.h>

#include <string.h>

#include "vdp2-internal.h"

irq_mux_t vdp2_hblank_in_irq_mux;
irq_mux_t vdp2_vblank_in_irq_mux;
irq_mux_t vdp2_vblank_out_irq_mux;

static void vdp2_hblank_in(void);
static void vdp2_vblank_in(void);
static void vdp2_vblank_out(void);

void
vdp2_init(void)
{
        /* Avoid re-initializing by checking */
        static bool initialized = false;

        /* Reset all state */
        vdp2_state.display_w = 0;
        vdp2_state.display_h = 0;
        vdp2_state.interlaced = TVMD_INTERLACE_NONE;

        vdp2_state.nbg0.scroll.x = F16(0.0f);
        vdp2_state.nbg0.scroll.y = F16(0.0f);
        vdp2_state.nbg1.scroll.x = F16(0.0f);
        vdp2_state.nbg1.scroll.y = F16(0.0f);
        vdp2_state.nbg2.scroll.x = 0;
        vdp2_state.nbg2.scroll.y = 0;
        vdp2_state.nbg3.scroll.x = 0;
        vdp2_state.nbg3.scroll.y = 0;

        (void)memset(&vdp2_state.nbg0.bitmap_format, 0x00,
            sizeof(struct scrn_bitmap_format));
        (void)memset(&vdp2_state.nbg0.cell_format, 0x00,
            sizeof(struct scrn_cell_format));
        (void)memset(&vdp2_state.nbg1.bitmap_format, 0x00,
            sizeof(struct scrn_bitmap_format));
        (void)memset(&vdp2_state.nbg1.cell_format, 0x00,
            sizeof(struct scrn_cell_format));
        (void)memset(&vdp2_state.nbg2.cell_format, 0x00,
            sizeof(struct scrn_cell_format));
        (void)memset(&vdp2_state.nbg3.cell_format, 0x00,
            sizeof(struct scrn_cell_format));

        /* Reset all buffered registers */
        memset(&vdp2_state.buffered_regs, 0x0000, sizeof(vdp2_state.buffered_regs));

        vdp2_state.vram_ctl.vram_size = VRAM_CTL_SIZE_4MBIT;
        vdp2_state.vram_ctl.vram_mode = VRAM_CTL_MODE_PART_BANK_A | VRAM_CTL_MODE_PART_BANK_B;

        uint32_t vram_bank;
        for (vram_bank = 0; vram_bank < 4; vram_bank++) {
                vdp2_state.vram_ctl.vram_cycp.pt[vram_bank].t0 = VRAM_CTL_CYCP_NO_ACCESS;
                vdp2_state.vram_ctl.vram_cycp.pt[vram_bank].t1 = VRAM_CTL_CYCP_NO_ACCESS;
                vdp2_state.vram_ctl.vram_cycp.pt[vram_bank].t2 = VRAM_CTL_CYCP_NO_ACCESS;
                vdp2_state.vram_ctl.vram_cycp.pt[vram_bank].t3 = VRAM_CTL_CYCP_NO_ACCESS;
                vdp2_state.vram_ctl.vram_cycp.pt[vram_bank].t4 = VRAM_CTL_CYCP_NO_ACCESS;
                vdp2_state.vram_ctl.vram_cycp.pt[vram_bank].t5 = VRAM_CTL_CYCP_NO_ACCESS;
                vdp2_state.vram_ctl.vram_cycp.pt[vram_bank].t6 = VRAM_CTL_CYCP_NO_ACCESS;
                vdp2_state.vram_ctl.vram_cycp.pt[vram_bank].t7 = VRAM_CTL_CYCP_NO_ACCESS;
        }

        /* Initialize the buffered registers to sane values */

        /* RAM */
        /* Color RAM mode 1 */
        /* Partition both VRAM bank A and B by default */
        vdp2_state.buffered_regs.ramctl = 0x1300;

        vdp2_state.buffered_regs.cyca0l = 0xFFFF;
        vdp2_state.buffered_regs.cyca0u = 0xFFFF;
        vdp2_state.buffered_regs.cyca1l = 0xFFFF;
        vdp2_state.buffered_regs.cyca1u = 0xFFFF;
        vdp2_state.buffered_regs.cycb0l = 0xFFFF;
        vdp2_state.buffered_regs.cycb0u = 0xFFFF;
        vdp2_state.buffered_regs.cycb1l = 0xFFFF;
        vdp2_state.buffered_regs.cycb1u = 0xFFFF;

        /* Zoom */
        vdp2_state.buffered_regs.zmxin0 = 0x0001;
        vdp2_state.buffered_regs.zmxdn0 = 0x0000;
        vdp2_state.buffered_regs.zmyin0 = 0x0001;
        vdp2_state.buffered_regs.zmydn0 = 0x0000;
        vdp2_state.buffered_regs.zmxin1 = 0x0001;
        vdp2_state.buffered_regs.zmxdn1 = 0x0000;
        vdp2_state.buffered_regs.zmyin1 = 0x0001;
        vdp2_state.buffered_regs.zmydn1 = 0x0000;

        /* Priority function */
        vdp2_state.buffered_regs.prina = 0x0101;
        vdp2_state.buffered_regs.prinb = 0x0101;
        vdp2_state.buffered_regs.prir = 0x0001;

        /* Initialize */
        vdp2_state.buffered_regs.spctl = 0x0020;
        vdp2_state.buffered_regs.prisa = 0x0101;
        vdp2_state.buffered_regs.prisb = 0x0101;
        vdp2_state.buffered_regs.prisc = 0x0101;
        vdp2_state.buffered_regs.prisd = 0x0101;

        vdp2_tvmd_display_clear();

        if (initialized) {
                return;
        }

        /* Initialized */
        initialized = true;

        irq_mux_init(&vdp2_hblank_in_irq_mux);
        irq_mux_init(&vdp2_vblank_in_irq_mux);
        irq_mux_init(&vdp2_vblank_out_irq_mux);

        /* Disable interrupts */
        cpu_intc_disable();

        uint32_t mask;
        mask = IC_MASK_VBLANK_IN | IC_MASK_VBLANK_OUT | IC_MASK_HBLANK_IN;

        scu_ic_mask_chg(IC_MASK_ALL, mask);

        scu_ic_interrupt_set(IC_INTERRUPT_HBLANK_IN, &vdp2_hblank_in);
        scu_ic_interrupt_set(IC_INTERRUPT_VBLANK_IN, &vdp2_vblank_in);
        scu_ic_interrupt_set(IC_INTERRUPT_VBLANK_OUT, &vdp2_vblank_out);
        scu_ic_mask_chg(IC_MASK_ALL & ~mask, IC_MASK_NONE);

        /* Enable interrupts */
        cpu_intc_enable();
}

static void
vdp2_hblank_in(void)
{
        irq_mux_handle(&vdp2_hblank_in_irq_mux);
}

static void
vdp2_vblank_in(void)
{
        irq_mux_handle(&vdp2_vblank_in_irq_mux);
}

static void
vdp2_vblank_out(void)
{
        irq_mux_handle(&vdp2_vblank_out_irq_mux);
}
