/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp2/scrn.h>

#include "vdp2_internal.h"

void vdp2_priority_spn_set(uint8_t scrn, uint8_t pri)
{
        /* When priority is zero, scroll screen is transparent. */
        pri &= 0x7;

        switch (scrn) {
        case SCRN_RBG1:
        case SCRN_NBG0:
                vdp2_regs.prina &= 0xFFF8;
                vdp2_regs.prina |= pri;

                /* Write to memory */
                MEM_POKE(VDP2(PRINA), vdp2_regs.prina);
                break;
        case SCRN_NBG1:
                vdp2_regs.prina &= 0xF8FF;
                vdp2_regs.prina |= pri << 8;

                /* Write to memory */
                MEM_POKE(VDP2(PRINA), vdp2_regs.prina);
                break;
        case SCRN_NBG2:
                vdp2_regs.prinb &= 0xFFF8;
                vdp2_regs.prinb |= pri;

                /* Write to memory */
                MEM_POKE(VDP2(PRINB), vdp2_regs.prinb);
                break;
        case SCRN_NBG3:
                vdp2_regs.prinb &= 0xF8FF;
                vdp2_regs.prinb |= pri << 8;

                /* Write to memory */
                MEM_POKE(VDP2(PRINB), vdp2_regs.prinb);
                break;
        case SCRN_RBG0:
                vdp2_regs.prir &= 0x0F00;
                vdp2_regs.prir |= pri;

                /* Write to memory */
                MEM_POKE(VDP2(PRIR), vdp2_regs.prir);
                break;
        default:
                return;
        }
}
