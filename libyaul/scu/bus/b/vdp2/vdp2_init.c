/*
 * Copyright (c) 2012-2014 Israel Jacquez
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
                }, {
                        /* VRAM-A1 */
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
                        VRAM_CTL_CYCP_NO_ACCESS
                }
        }
};

irq_mux_t vdp2_hblank_in_irq_mux;
irq_mux_t vdp2_vblank_in_irq_mux;
irq_mux_t vdp2_vblank_out_irq_mux;

static void vdp2_hblank_in(void);
static void vdp2_vblank_in(void);
static void vdp2_vblank_out(void);

void
vdp2_init(void)
{
        /* Avoid re-initializing */
        static bool initialized = false;

        /* Reset all buffer registers. */
        memset(&vdp2_regs, 0x0000, sizeof(struct vdp2_regs));

        /* Initialize the processor to sane values */
        MEMORY_WRITE(16, VDP2(TVMD), 0x0000);

        /* RAM */
        /* Color RAM mode 1 */
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
        vdp2_regs.bgon = 0x0000;
        MEMORY_WRITE(16, VDP2(BGON), vdp2_regs.bgon);
        vdp2_regs.chctla = 0x0000;
        MEMORY_WRITE(16, VDP2(CHCTLA), vdp2_regs.chctla);
        MEMORY_WRITE(16, VDP2(CHCTLB), vdp2_regs.chctlb);
        MEMORY_WRITE(16, VDP2(PNCN0), 0x0000);
        MEMORY_WRITE(16, VDP2(PNCN1), 0x0000);
        MEMORY_WRITE(16, VDP2(PNCN2), 0x0000);
        MEMORY_WRITE(16, VDP2(PNCN3), 0x0000);
        vdp2_regs.plsz = 0x0000;
        MEMORY_WRITE(16, VDP2(PLSZ), vdp2_regs.plsz);
        vdp2_regs.bmpna = 0x0000;
        MEMORY_WRITE(16, VDP2(BMPNA), vdp2_regs.bmpna);
        vdp2_regs.bmpnb = 0x0000;
        MEMORY_WRITE(16, VDP2(BMPNB), vdp2_regs.bmpnb);
        vdp2_regs.mpofn = 0x0000;
        MEMORY_WRITE(16, VDP2(MPOFN), vdp2_regs.mpofn);
        vdp2_regs.mpofr = 0x0000;
        MEMORY_WRITE(16, VDP2(MPOFR), vdp2_regs.mpofr);

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
        MEMORY_WRITE(16, VDP2(MPOFR), 0x0000);
        vdp2_regs.rpmd = 0x0000;
        MEMORY_WRITE(16, VDP2(RPMD), vdp2_regs.rpmd);
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
        vdp2_regs.prina = 0x0101;
        MEMORY_WRITE(16, VDP2(PRINA), vdp2_regs.prina);
        vdp2_regs.prinb = 0x0101;
        MEMORY_WRITE(16, VDP2(PRINB), vdp2_regs.prinb);
        vdp2_regs.prir = 0x0001;
        MEMORY_WRITE(16, VDP2(PRIR), vdp2_regs.prir);

        /* Color offset function */
        MEMORY_WRITE(16, VDP2(CLOFEN), 0x0000);
        MEMORY_WRITE(16, VDP2(CLOFSL), 0x0000);

        /* Initialize */
        vdp2_regs.spctl = 0x0020;
        MEMORY_WRITE(16, VDP2(SPCTL), vdp2_regs.spctl);
        vdp2_regs.prisa = 0x0101;
        MEMORY_WRITE(16, VDP2(PRISA), vdp2_regs.prisa);
        vdp2_regs.prisb = 0x0101;
        MEMORY_WRITE(16, VDP2(PRISB), vdp2_regs.prisb);
        vdp2_regs.prisc = 0x0101;
        MEMORY_WRITE(16, VDP2(PRISC), vdp2_regs.prisc);
        vdp2_regs.prisd = 0x0101;
        MEMORY_WRITE(16, VDP2(PRISD), vdp2_regs.prisd);

        vdp2_regs.craofa = 0x0000;
        MEMORY_WRITE(16, VDP2(CRAOFA), vdp2_regs.craofa);
        vdp2_regs.craofb = 0x0000;
        MEMORY_WRITE(16, VDP2(CRAOFB), vdp2_regs.craofb);

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
