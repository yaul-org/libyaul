/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <vdp2/scrn.h>

#include "vdp2-internal.h"

void
vdp2_scrn_mosaic_set(uint8_t scrn)
{
#ifdef DEBUG
        assert((scrn == SCRN_NBG0) ||
               (scrn == SCRN_NBG1) ||
               (scrn == SCRN_NBG2) ||
               (scrn == SCRN_NBG3) ||
               (scrn == SCRN_RBG0));
#endif /* DEBUG */

        /* If performing mosaic processing in NBG0 or NBG1, the vertical
         * cell-scroll function cannot be used.
         *
         * Mosaic processing is then ignored for NBG0 or NBG1. */

        switch (scrn) {
        case SCRN_RBG1:
        case SCRN_NBG0:
#ifdef DEBUG
                /* Make sure that the vertical cell scroll function is
                 * not being used */
                assert((vdp2_state.buffered_regs.scrctl & 0x0001) == 0x0000);
#endif /* DEBUG */

                vdp2_state.buffered_regs.mzctl &= 0xFFFE;
                vdp2_state.buffered_regs.mzctl |= 0x0001;
                break;
        case SCRN_NBG1:
#ifdef DEBUG
                /* Make sure that the vertical cell scroll function is
                 * not being used */
                assert((vdp2_state.buffered_regs.scrctl & 0x0100) == 0x0000);
#endif /* DEBUG */

                vdp2_state.buffered_regs.mzctl &= 0xFFFD;
                vdp2_state.buffered_regs.mzctl |= 0x0002;
                break;
        case SCRN_NBG2:
                vdp2_state.buffered_regs.mzctl &= 0xFFFB;
                vdp2_state.buffered_regs.mzctl |= 0x0004;
                break;
        case SCRN_NBG3:
                vdp2_state.buffered_regs.mzctl &= 0xFFF7;
                vdp2_state.buffered_regs.mzctl |= 0x0008;
                break;
        case SCRN_RBG0:
                vdp2_state.buffered_regs.mzctl &= 0xFFEF;
                vdp2_state.buffered_regs.mzctl |= 0x0010;
                break;
        }
}
