/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include "vdp-internal.h"

uint8_t
vdp2_scrn_priority_get(uint8_t scrn)
{
#ifdef DEBUG
        assert((scrn == SCRN_NBG0) ||
               (scrn == SCRN_NBG1) ||
               (scrn == SCRN_NBG2) ||
               (scrn == SCRN_NBG3) ||
               (scrn == SCRN_RBG0) ||
               (scrn == SCRN_RBG1));
#endif /* DEBUG */

        switch (scrn) {
        case SCRN_RBG1:
        case SCRN_NBG0:
                return _state_vdp2()->regs.prina & 0x0007;
        case SCRN_NBG1:
                return (_state_vdp2()->regs.prina & 0x0700) >> 8;
        case SCRN_NBG2:
                return _state_vdp2()->regs.prinb & 0x0007;
        case SCRN_NBG3:
                return (_state_vdp2()->regs.prinb & 0x0700) >> 8;
        case SCRN_RBG0:
                return _state_vdp2()->regs.prir & 0x0007;
        default:
                return 0;
        }
}
