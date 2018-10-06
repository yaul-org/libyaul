/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include <assert.h>

#include "vdp-internal.h"

void
vdp2_scrn_reduction_set(uint8_t scrn, uint16_t reduction_amt)
{
#ifdef DEBUG
        /* Check if the background passed is valid */
        assert((scrn == SCRN_NBG0) ||
               (scrn == SCRN_NBG1));

        assert((reduction_amt == SCRN_REDUCTION_NONE) ||
               (reduction_amt == SCRN_REDUCTION_HALF) ||
               (reduction_amt == SCRN_REDUCTION_QUARTER));
#endif /* DEBUG */

        switch (scrn) {
        case SCRN_NBG0:
                _state_vdp2()->regs.zmctl &= 0xFFFC;
                _state_vdp2()->regs.zmctl |= reduction_amt;
                break;
        case SCRN_NBG1:
                _state_vdp2()->regs.zmctl &= 0xFCFF;
                _state_vdp2()->regs.zmctl |= reduction_amt << 8;
                break;
        default:
                return;
        }
}
