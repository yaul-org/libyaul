/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp2/scrn.h>

#include "vdp2_internal.h"

void
vdp2_scrn_ch_color_set(enum scrn_type scrn, enum scrn_ch_color_type chc)
{
        switch (scrn) {
        case SCRN_RBG1:
        case SCRN_NBG0:
                vdp2_regs.chctla &= 0xff8f;
                vdp2_regs.chctla |= chc << 4;

                /* Write to memory. */
                MEM_POKE(VDP2(CHCTLA), vdp2_regs.chctla);
                break;
        case SCRN_NBG1:
                vdp2_regs.chctla &= 0xcfff;
                vdp2_regs.chctla |= chc << 12;

                /* Write to memory. */
                MEM_POKE(VDP2(CHCTLA), vdp2_regs.chctla);
                break;
        case SCRN_NBG2:
                vdp2_regs.chctlb &= 0xfffd;
                vdp2_regs.chctlb |= chc << 1;

                /* Write to memory. */
                MEM_POKE(VDP2(CHCTLB), vdp2_regs.chctlb);
                break;
        case SCRN_NBG3:
                vdp2_regs.chctlb &= 0xffdf;
                vdp2_regs.chctlb |= chc << 5;

                /* Write to memory. */
                MEM_POKE(VDP2(CHCTLB), vdp2_regs.chctlb);
                break;
	case SCRN_RBG0:
		vdp2_regs.chctlb &= 0x8fff;
		vdp2_regs.chctlb |= chc << 12;

                /* Write to memory. */
		MEM_POKE(VDP2(CHCTLB), vdp2_regs.chctlb);
                break;
        default:
                break;
        }
}
