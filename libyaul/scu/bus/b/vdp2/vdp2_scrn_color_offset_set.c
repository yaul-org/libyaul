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
vdp2_scrn_color_offset_set(uint8_t scrn, uint8_t select)
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

        /* Check if the color offset is A or B */
        assert((select == SCRN_COLOR_OFFSET_A) ||
               (select == SCRN_COLOR_OFFSET_B));
#endif /* DEBUG */

        switch (scrn) {
        case SCRN_NBG0:
                _internal_state_vdp2.buffered_regs.clofen &= 0xFFFE;
                _internal_state_vdp2.buffered_regs.clofen |= 0x0001;

                _internal_state_vdp2.buffered_regs.clofsl &= 0xFFFE;
                _internal_state_vdp2.buffered_regs.clofsl |= select << 0;
                break;
        case SCRN_NBG1:
                _internal_state_vdp2.buffered_regs.clofen &= 0xFFFD;
                _internal_state_vdp2.buffered_regs.clofen |= 0x0002;

                _internal_state_vdp2.buffered_regs.clofsl &= 0xFFFD;
                _internal_state_vdp2.buffered_regs.clofsl |= select << 1;
                break;
        case SCRN_NBG2:
                _internal_state_vdp2.buffered_regs.clofen &= 0xFFFB;
                _internal_state_vdp2.buffered_regs.clofen |= 0x0004;

                _internal_state_vdp2.buffered_regs.clofsl &= 0xFFFB;
                _internal_state_vdp2.buffered_regs.clofsl |= select << 2;
                break;
        case SCRN_NBG3:
                _internal_state_vdp2.buffered_regs.clofen &= 0xFFF7;
                _internal_state_vdp2.buffered_regs.clofen |= 0x0008;

                _internal_state_vdp2.buffered_regs.clofsl &= 0xFFF7;
                _internal_state_vdp2.buffered_regs.clofsl |= select << 3;
                break;
        case SCRN_RBG0:
                _internal_state_vdp2.buffered_regs.clofen &= 0xFFEF;
                _internal_state_vdp2.buffered_regs.clofen |= 0x0010;

                _internal_state_vdp2.buffered_regs.clofsl &= 0xFFEF;
                _internal_state_vdp2.buffered_regs.clofsl |= select << 4;
                break;
        case SCRN_BACK:
                _internal_state_vdp2.buffered_regs.clofen &= 0xFFDF;
                _internal_state_vdp2.buffered_regs.clofen |= 0x0020;

                _internal_state_vdp2.buffered_regs.clofsl &= 0xFFDF;
                _internal_state_vdp2.buffered_regs.clofsl |= select << 5;
                break;
        case SCRN_SPRITE:
                _internal_state_vdp2.buffered_regs.clofen &= 0xFFBF;
                _internal_state_vdp2.buffered_regs.clofen |= 0x0040;

                _internal_state_vdp2.buffered_regs.clofsl &= 0xFFBF;
                _internal_state_vdp2.buffered_regs.clofsl |= select << 6;
                break;
        default:
                return;
        }
}
