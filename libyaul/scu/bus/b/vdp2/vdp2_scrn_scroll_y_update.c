/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include "vdp2-internal.h"

void
vdp2_scrn_scroll_x_update(uint8_t scrn, fix16_t scroll_delta)
{
#ifdef DEBUG
        /* Check if the background passed is valid */
        assert((scrn == SCRN_NBG0) ||
               (scrn == SCRN_RBG1) ||
               (scrn == SCRN_NBG1) ||
               (scrn == SCRN_NBG2) ||
               (scrn == SCRN_NBG3) ||
               (scrn == SCRN_RBG1));
#endif /* DEBUG */

        /* All screen scroll values must be identified as positive
         * values */

        /* We don't want to overflow. Nasty stuff could happen */
        scroll_delta = fix16_clamp(scroll_delta, F16(0.0f), F16(2047.0f));

        uint16_t in;
        in = fix16_to_int(scroll_delta) & 0x07FF;

        /* Only take into account the upper 8 bits of the fractional */
        uint16_t dn;
        dn = fix16_sub(scroll_delta, fix16_from_int(in)) & 0xFF00;

        switch (scrn) {
        case SCRN_RBG1:
        case SCRN_NBG0:
                vdp2_state.buffered_regs.scyin0 += in;
                vdp2_state.buffered_regs.scyin0 &= 0x07FF;
                vdp2_state.buffered_regs.scydn0 += dn;
                vdp2_state.buffered_regs.scydn0 &= 0xFF00;

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(SCYIN0), vdp2_state.buffered_regs.scyin0);
                MEMORY_WRITE(16, VDP2(SCYDN0), vdp2_state.buffered_regs.scydn0);
                break;
        case SCRN_NBG1:
                vdp2_state.buffered_regs.scyin1 += in;
                vdp2_state.buffered_regs.scyin1 &= 0x07FF;
                vdp2_state.buffered_regs.scydn1 += dn;
                vdp2_state.buffered_regs.scydn1 &= 0xFF00;

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(SCYIN1), vdp2_state.buffered_regs.scyin1);
                MEMORY_WRITE(16, VDP2(SCYDN1), vdp2_state.buffered_regs.scyin1);
                break;
        case SCRN_NBG2:
                vdp2_state.buffered_regs.scyn2 += in;
                vdp2_state.buffered_regs.scyn2 &= 0x07FF;

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(SCYN2), vdp2_state.buffered_regs.scyn2);
                break;
        case SCRN_NBG3:
                vdp2_state.buffered_regs.scyn3 = in;
                vdp2_state.buffered_regs.scyn3 &= 0x07FF;

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(SCYN3), vdp2_state.buffered_regs.scyn3);
                break;
        default:
                return;
        }
}
