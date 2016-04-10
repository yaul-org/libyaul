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
                vdp2_state.buffered_regs.clofen &= 0xFFFE;
                vdp2_state.buffered_regs.clofen |= 0x0001;

                vdp2_state.buffered_regs.clofsl &= 0xFFFE;
                vdp2_state.buffered_regs.clofsl |= select << 0;
                break;
        case SCRN_NBG1:
                vdp2_state.buffered_regs.clofen &= 0xFFFD;
                vdp2_state.buffered_regs.clofen |= 0x0002;

                vdp2_state.buffered_regs.clofsl &= 0xFFFD;
                vdp2_state.buffered_regs.clofsl |= select << 1;
                break;
        case SCRN_NBG2:
                vdp2_state.buffered_regs.clofen &= 0xFFFB;
                vdp2_state.buffered_regs.clofen |= 0x0004;

                vdp2_state.buffered_regs.clofsl &= 0xFFFB;
                vdp2_state.buffered_regs.clofsl |= select << 2;
                break;
        case SCRN_NBG3:
                vdp2_state.buffered_regs.clofen &= 0xFFF7;
                vdp2_state.buffered_regs.clofen |= 0x0008;

                vdp2_state.buffered_regs.clofsl &= 0xFFF7;
                vdp2_state.buffered_regs.clofsl |= select << 3;
                break;
        case SCRN_RBG0:
                vdp2_state.buffered_regs.clofen &= 0xFFEF;
                vdp2_state.buffered_regs.clofen |= 0x0010;

                vdp2_state.buffered_regs.clofsl &= 0xFFEF;
                vdp2_state.buffered_regs.clofsl |= select << 4;
                break;
        case SCRN_BACK:
                vdp2_state.buffered_regs.clofen &= 0xFFDF;
                vdp2_state.buffered_regs.clofen |= 0x0020;

                vdp2_state.buffered_regs.clofsl &= 0xFFDF;
                vdp2_state.buffered_regs.clofsl |= select << 5;
                break;
        case SCRN_SPRITE:
                vdp2_state.buffered_regs.clofen &= 0xFFBF;
                vdp2_state.buffered_regs.clofen |= 0x0040;

                vdp2_state.buffered_regs.clofsl &= 0xFFBF;
                vdp2_state.buffered_regs.clofsl |= select << 6;
                break;
        default:
                return;
        }

        /* Write to memory */
        MEMORY_WRITE(16, VDP2(CLOFEN), vdp2_state.buffered_regs.clofen);
        MEMORY_WRITE(16, VDP2(CLOFSL), vdp2_state.buffered_regs.clofsl);
}
