/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <cpu.h>
#include <scu/ic.h>
#include <vdp2.h>

#include <string.h>

#include "vdp2-internal.h"

struct vdp2_regs vdp2_regs;

struct vram_ctl vram_ctl = {
        .vram_size = VRAM_CTL_SIZE_4MBIT,
        .vram_mode = VRAM_CTL_MODE_PART_BANK_A | VRAM_CTL_MODE_PART_BANK_B,
        .vram_cycp.pt = {
                {
                        /* VRAM-A0 */
                        VRAM_CTL_CYCP_NO_ACCESS,
                        VRAM_CTL_CYCP_NO_ACCESS,
                        VRAM_CTL_CYCP_NO_ACCESS,
                        VRAM_CTL_CYCP_NO_ACCESS,
                        VRAM_CTL_CYCP_NO_ACCESS,
                        VRAM_CTL_CYCP_NO_ACCESS,
                        VRAM_CTL_CYCP_NO_ACCESS,
                        VRAM_CTL_CYCP_NO_ACCESS
                }, {/* VRAM-A1 */
                        VRAM_CTL_CYCP_NO_ACCESS,
                        VRAM_CTL_CYCP_NO_ACCESS,
                        VRAM_CTL_CYCP_NO_ACCESS,
                        VRAM_CTL_CYCP_NO_ACCESS,
                        VRAM_CTL_CYCP_NO_ACCESS,
                        VRAM_CTL_CYCP_NO_ACCESS,
                        VRAM_CTL_CYCP_NO_ACCESS,
                        VRAM_CTL_CYCP_NO_ACCESS
                }, {
                        /* VRAM-B0 */
                        VRAM_CTL_CYCP_NO_ACCESS,
                        VRAM_CTL_CYCP_NO_ACCESS,
                        VRAM_CTL_CYCP_NO_ACCESS,
                        VRAM_CTL_CYCP_NO_ACCESS,
                        VRAM_CTL_CYCP_NO_ACCESS,
                        VRAM_CTL_CYCP_NO_ACCESS,
                        VRAM_CTL_CYCP_NO_ACCESS,
                        VRAM_CTL_CYCP_NO_ACCESS
                }, {
                        /* VRAM-B1 */
                        VRAM_CTL_CYCP_NO_ACCESS,
                        VRAM_CTL_CYCP_NO_ACCESS,
                        VRAM_CTL_CYCP_NO_ACCESS,
                        VRAM_CTL_CYCP_NO_ACCESS,
                        VRAM_CTL_CYCP_NO_ACCESS,
                        VRAM_CTL_CYCP_NO_ACCESS,
                        VRAM_CTL_CYCP_NO_ACCESS,
                        VRAM_CTL_CYCP_NO_ACCESS}
        }
};

irq_mux_t vdp2_vblank_in_irq_mux;
irq_mux_t vdp2_vblank_out_irq_mux;

static void vdp2_vblank_in(void);
static void vdp2_vblank_out(void);

void
vdp2_init(void)
{
        /* Avoid re-initializing */
        static bool initialized = false;

        uint32_t mask;

        /* Initialize the processor to sane values. */
        MEMORY_WRITE(16, VDP2(TVMD), 0x0000);

        /* RAM */
        /* Partition both VRAM bank A and B by default */
        MEMORY_WRITE(16, VDP2(RAMCTL), 0x0300);
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

        /* Rotation scroll screen */
        MEMORY_WRITE(16, VDP2(RPMD), 0x0000);

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

        /* Sprite data */
        MEMORY_WRITE(16, VDP2(SPCTL), 0x0000);
        MEMORY_WRITE(16, VDP2(PRISA), 0x0101);
        MEMORY_WRITE(16, VDP2(PRISB), 0x0101);
        MEMORY_WRITE(16, VDP2(PRISC), 0x0101);
        MEMORY_WRITE(16, VDP2(PRISD), 0x0101);

        /* Priority function */
        MEMORY_WRITE(16, VDP2(PRINA), 0x0101);
        MEMORY_WRITE(16, VDP2(PRINB), 0x0101);
        MEMORY_WRITE(16, VDP2(PRIR), 0x0001);

        /* Color offset function */
        MEMORY_WRITE(16, VDP2(CLOFEN), 0x0000);
        MEMORY_WRITE(16, VDP2(CLOFSL), 0x0000);

        /* Reset all buffer registers. */
        memset(&vdp2_regs, 0x0000, sizeof(struct vdp2_regs));

        if (initialized)
                return;

        irq_mux_init(&vdp2_vblank_in_irq_mux);
        irq_mux_init(&vdp2_vblank_out_irq_mux);

        /* Disable interrupts */
        cpu_intc_disable();

        mask = IC_MASK_VBLANK_IN | IC_MASK_VBLANK_OUT;
        scu_ic_mask_chg(IC_MASK_ALL, mask);

        scu_ic_interrupt_set(IC_INTERRUPT_VBLANK_IN,
            &vdp2_vblank_in);
        scu_ic_interrupt_set(IC_INTERRUPT_VBLANK_OUT,
            &vdp2_vblank_out);
        scu_ic_mask_chg(IC_MASK_ALL & ~mask, IC_MASK_NONE);

        /* Enable interrupts */
        cpu_intc_enable();

        /* Initialized */
        initialized = true;
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
