/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include <assert.h>

#include "vdp2-internal.h"

void
vdp2_scrn_reduction_set(uint8_t scrn, uint16_t horz_reduction)
{
#ifdef DEBUG
        /* Check if the background passed is valid */
        assert((scrn == SCRN_NBG0) ||
               (scrn == SCRN_NBG1));

        assert((horz_reduction == SCRN_REDUCTION_NONE) ||
               (horz_reduction == SCRN_REDUCTION_HALF) ||
               (horz_reduction == SCRN_REDUCTION_QUARTER));
#endif /* DEBUG */

        switch (scrn) {
        case SCRN_NBG0:
                vdp2_state.buffered_regs.zmctl &= 0xFFFC;
                vdp2_state.buffered_regs.zmctl |= horz_reduction;
                break;
        case SCRN_NBG1:
                vdp2_state.buffered_regs.zmctl &= 0xFCFF;
                vdp2_state.buffered_regs.zmctl |= horz_reduction << 8;
                break;
        default:
                return;
        }

        /* Write to memory */
        MEMORY_WRITE(16, VDP2(MZCTL), vdp2_state.buffered_regs.zmctl);
}
