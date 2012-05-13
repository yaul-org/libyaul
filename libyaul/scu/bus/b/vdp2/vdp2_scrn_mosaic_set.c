/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp2/scrn.h>

#include "vdp2_internal.h"

void
vdp2_scrn_mosaic_set(uint8_t scrn, uint8_t x, uint8_t y)
{
        /* If performing mosaic processing in NBG0 or NBG1, the mosaic
         * process is done on the display screen for screens that don't
         * cell-scroll vertically */

        vdp2_regs.mzctl &= 0x0FFF;
        vdp2_regs.mzctl |= (x & 0xF) << 12;

        vdp2_regs.mzctl &= 0xF0FF;
        vdp2_regs.mzctl |= (y & 0xF) << 8;

        switch (scrn) {
        case SCRN_RBG1:
        case SCRN_NBG0:
                vdp2_regs.mzctl &= 0xFFFE;
                vdp2_regs.mzctl |= 0x0001;
                break;
        case SCRN_NBG1:
                vdp2_regs.mzctl &= 0xFFFD;
                vdp2_regs.mzctl |= 0x0002;
                break;
        case SCRN_NBG2:
                vdp2_regs.mzctl &= 0xFFFB;
                vdp2_regs.mzctl |= 0x0004;
                break;
        case SCRN_NBG3:
                vdp2_regs.mzctl &= 0xFFF7;
                vdp2_regs.mzctl |= 0x0008;
                break;
        case SCRN_RBG0:
                vdp2_regs.mzctl &= 0xFFEF;
                vdp2_regs.mzctl |= 0x0010;
                break;
        default:
                return;
        }

        /* Write to memory */
        MEM_POKE(VDP2(MZCTL), vdp2_regs.mzctl);
}
