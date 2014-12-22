/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include "vdp2-internal.h"

void
vdp2_scrn_mosaic_unset(uint8_t scrn)
{
#ifdef DEBUG
        assert((scrn == SCRN_NBG0) ||
               (scrn == SCRN_NBG1) ||
               (scrn == SCRN_NBG2) ||
               (scrn == SCRN_NBG3) ||
               (scrn == SCRN_RBG0));
#endif /* DEBUG */

        switch (scrn) {
        case SCRN_RBG1:
        case SCRN_NBG0:
                vdp2_regs.mzctl &= 0xFFFE;
                break;
        case SCRN_NBG1:
                vdp2_regs.mzctl &= 0xFFFD;
                break;
        case SCRN_NBG2:
                vdp2_regs.mzctl &= 0xFFFB;
                break;
        case SCRN_NBG3:
                vdp2_regs.mzctl &= 0xFFF7;
                break;
        case SCRN_RBG0:
                vdp2_regs.mzctl &= 0xFFEF;
                break;
        }

        /* Write to memory */
        MEMORY_WRITE(16, VDP2(MZCTL), vdp2_regs.mzctl);
}
