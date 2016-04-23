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

        vdp2_state.scroll.nbg0.x = F16(0.0f);
        vdp2_state.scroll.nbg0.y = F16(0.0f);
        vdp2_state.scroll.nbg1.x = F16(0.0f);
        vdp2_state.scroll.nbg1.y = F16(0.0f);
        vdp2_state.scroll.nbg2.x = 0;
        vdp2_state.scroll.nbg2.y = 0;
        vdp2_state.scroll.nbg3.x = 0;
        vdp2_state.scroll.nbg3.x = 0;

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

        /* Reset all buffered registers */
        memset(&vdp2_state.buffered_regs, 0x0000,
            sizeof(vdp2_state.buffered_regs));

        /* Initialize the processor to sane values */
        MEMORY_WRITE(16, VDP2(TVMD), 0x0000);

        /* RAM */
        /* Color RAM mode 1 */
        /* Partition both VRAM bank A and B by default */
        MEMORY_WRITE(16, VDP2(RAMCTL), 0x1300);
        MEMORY_WRITE(16, VDP2(CYCA0L), 0xFFFF);
        MEMORY_WRITE(16, VDP2(CYCA0U), 0xFFFF);
        MEMORY_WRITE(16, VDP2(CYCA1L), 0xFFFF);
        MEMORY_WRITE(16, VDP2(CYCA1U), 0xFFFF);
        MEMORY_WRITE(16, VDP2(CYCB0L), 0xFFFF);
        MEMORY_WRITE(16, VDP2(CYCB0U), 0xFFFF);
        MEMORY_WRITE(16, VDP2(CYCB1L), 0xFFFF);
        MEMORY_WRITE(16, VDP2(CYCB1U), 0xFFFF);

        /* Scroll screen */
        MEMORY_WRITE(16, VDP2(BGON), 0x0000);
        MEMORY_WRITE(16, VDP2(CHCTLA), 0x0000);
        MEMORY_WRITE(16, VDP2(CHCTLB), 0x0000);
        MEMORY_WRITE(16, VDP2(PNCN0), 0x0000);
        MEMORY_WRITE(16, VDP2(PNCN1), 0x0000);
        MEMORY_WRITE(16, VDP2(PNCN2), 0x0000);
        MEMORY_WRITE(16, VDP2(PNCN3), 0x0000);
        MEMORY_WRITE(16, VDP2(PLSZ), 0x0000);
        MEMORY_WRITE(16, VDP2(BMPNA), 0x0000);
        MEMORY_WRITE(16, VDP2(BMPNB), 0x0000);
        MEMORY_WRITE(16, VDP2(MPOFN), 0x0000);
        MEMORY_WRITE(16, VDP2(MPOFR), 0x0000);

        MEMORY_WRITE(16, VDP2(MPABN0), 0x0000);
        MEMORY_WRITE(16, VDP2(MPCDN0), 0x0000);
        MEMORY_WRITE(16, VDP2(MPABN1), 0x0000);
        MEMORY_WRITE(16, VDP2(MPCDN1), 0x0000);
        MEMORY_WRITE(16, VDP2(MPABN2), 0x0000);
        MEMORY_WRITE(16, VDP2(MPCDN2), 0x0000);
        MEMORY_WRITE(16, VDP2(MPABN3), 0x0000);
        MEMORY_WRITE(16, VDP2(MPCDN3), 0x0000);

        MEMORY_WRITE(16, VDP2(SCXIN0), 0x0000);
        MEMORY_WRITE(16, VDP2(SCYIN0), 0x0000);
        MEMORY_WRITE(16, VDP2(SCXIN1), 0x0000);
        MEMORY_WRITE(16, VDP2(SCYIN1), 0x0000);
        MEMORY_WRITE(16, VDP2(SCXN2), 0x0000);
        MEMORY_WRITE(16, VDP2(SCYN2), 0x0000);
        MEMORY_WRITE(16, VDP2(SCXN3), 0x0000);
        MEMORY_WRITE(16, VDP2(SCYN3), 0x0000);

        MEMORY_WRITE(16, VDP2(ZMXIN0), 0x0001);
        MEMORY_WRITE(16, VDP2(ZMXDN0), 0x0000);
        MEMORY_WRITE(16, VDP2(ZMYIN0), 0x0001);
        MEMORY_WRITE(16, VDP2(ZMYDN0), 0x0000);
        MEMORY_WRITE(16, VDP2(ZMXIN1), 0x0001);
        MEMORY_WRITE(16, VDP2(ZMXDN1), 0x0000);
        MEMORY_WRITE(16, VDP2(ZMYIN1), 0x0001);
        MEMORY_WRITE(16, VDP2(ZMYDN1), 0x0000);

        /* Rotation scroll screen */
        MEMORY_WRITE(16, VDP2(MPOFR), 0x0000);
        MEMORY_WRITE(16, VDP2(RPMD), 0x0000);
        MEMORY_WRITE(16, VDP2(OVPNRA), 0x0000);
        MEMORY_WRITE(16, VDP2(OVPNRB), 0x0000);

        MEMORY_WRITE(16, VDP2(MPABRA), 0x0000);
        MEMORY_WRITE(16, VDP2(MPCDRA), 0x0000);
        MEMORY_WRITE(16, VDP2(MPEFRA), 0x0000);
        MEMORY_WRITE(16, VDP2(MPGHRA), 0x0000);
        MEMORY_WRITE(16, VDP2(MPIJRA), 0x0000);
        MEMORY_WRITE(16, VDP2(MPKLRA), 0x0000);
        MEMORY_WRITE(16, VDP2(MPMNRA), 0x0000);
        MEMORY_WRITE(16, VDP2(MPOPRA), 0x0000);
        MEMORY_WRITE(16, VDP2(MPABRB), 0x0000);
        MEMORY_WRITE(16, VDP2(MPCDRB), 0x0000);
        MEMORY_WRITE(16, VDP2(MPEFRB), 0x0000);
        MEMORY_WRITE(16, VDP2(MPGHRB), 0x0000);
        MEMORY_WRITE(16, VDP2(MPIJRB), 0x0000);
        MEMORY_WRITE(16, VDP2(MPKLRB), 0x0000);
        MEMORY_WRITE(16, VDP2(MPMNRB), 0x0000);
        MEMORY_WRITE(16, VDP2(MPOPRB), 0x0000);

        /* Line screen */
        /* Back screen */
        MEMORY_WRITE(16, VDP2(BKTAL), 0x0000);
        MEMORY_WRITE(16, VDP2(BKTAU), 0x0000);

        /* Priority function */
        vdp2_state.buffered_regs.prina = 0x0101;
        MEMORY_WRITE(16, VDP2(PRINA), vdp2_state.buffered_regs.prina);
        vdp2_state.buffered_regs.prinb = 0x0101;
        MEMORY_WRITE(16, VDP2(PRINB), vdp2_state.buffered_regs.prinb);
        vdp2_state.buffered_regs.prir = 0x0001;
        MEMORY_WRITE(16, VDP2(PRIR), vdp2_state.buffered_regs.prir);

        /* Color offset function */
        MEMORY_WRITE(16, VDP2(CLOFEN), 0x0000);
        MEMORY_WRITE(16, VDP2(CLOFSL), 0x0000);

        /* Initialize */
        vdp2_state.buffered_regs.spctl = 0x0021;
        MEMORY_WRITE(16, VDP2(SPCTL), vdp2_state.buffered_regs.spctl);
        vdp2_state.buffered_regs.prisa = 0x0101;
        MEMORY_WRITE(16, VDP2(PRISA), vdp2_state.buffered_regs.prisa);
        vdp2_state.buffered_regs.prisb = 0x0101;
        MEMORY_WRITE(16, VDP2(PRISB), vdp2_state.buffered_regs.prisb);
        vdp2_state.buffered_regs.prisc = 0x0101;
        MEMORY_WRITE(16, VDP2(PRISC), vdp2_state.buffered_regs.prisc);
        vdp2_state.buffered_regs.prisd = 0x0101;
        MEMORY_WRITE(16, VDP2(PRISD), vdp2_state.buffered_regs.prisd);

        MEMORY_WRITE(16, VDP2(CRAOFA), 0x0000);
        MEMORY_WRITE(16, VDP2(CRAOFB), 0x0000);

        if (initialized) {
                return;
        }

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

        /* Initialized */
        initialized = true;
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
