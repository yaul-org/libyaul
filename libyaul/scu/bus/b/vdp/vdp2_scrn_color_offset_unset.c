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
vdp2_scrn_color_offset_unset(uint8_t scrn)
{
#ifdef DEBUG
        /* Check if the background passed is valid */
        assert((scrn == SCRN_NBG0) ||
               (scrn == SCRN_RBG1) ||
               (scrn == SCRN_NBG1) ||
               (scrn == SCRN_NBG2) ||
               (scrn == SCRN_NBG3) ||
               (scrn == SCRN_RBG0) ||
               (scrn == SCRN_BACK) ||
               (scrn == SCRN_SPRITE));
#endif /* DEBUG */

        switch (scrn) {
        case SCRN_NBG0:
                _state_vdp2()->regs.clofen &= 0xFFFE;
                break;
        case SCRN_NBG1:
                _state_vdp2()->regs.clofen &= 0xFFFD;
                break;
        case SCRN_NBG2:
                _state_vdp2()->regs.clofen &= 0xFFFB;
                break;
        case SCRN_NBG3:
                _state_vdp2()->regs.clofen &= 0xFFF7;
                break;
        case SCRN_RBG0:
                _state_vdp2()->regs.clofen &= 0xFFEF;
                break;
        case SCRN_BACK:
                _state_vdp2()->regs.clofen &= 0xFFDF;
                break;
        case SCRN_SPRITE:
                _state_vdp2()->regs.clofen &= 0xFFBF;
                break;
        default:
                return;
        }
}
