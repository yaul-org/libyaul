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
vdp2_scrn_color_offset_rgb_set(uint8_t select, int16_t r, int16_t g, int16_t b)
{
#ifdef DEBUG
        /* Check if the color offset is A or B */
        assert((select == SCRN_COLOR_OFFSET_A) ||
               (select == SCRN_COLOR_OFFSET_B));

        /* Check range: [-256,256] */
        assert(((r >= -256) && (r <= 255)) &&
               ((g >= -256) && (g <= 255)) &&
               ((b >= -256) && (b <= 255)));
#endif /* DEBUG */

        /* Each color component in the color offset has a range [-256,255]
         * Lower bound: -(2**(9-1)) & upper bound: (2**9)-1 */
        switch (select) {
        case SCRN_COLOR_OFFSET_A:
                vdp2_state.buffered_regs.coar = r & 0x01FF;
                vdp2_state.buffered_regs.coag = g & 0x01FF;
                vdp2_state.buffered_regs.coab = b & 0x01FF;

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(COAR), vdp2_state.buffered_regs.coar);
                MEMORY_WRITE(16, VDP2(COAG), vdp2_state.buffered_regs.coag);
                MEMORY_WRITE(16, VDP2(COAB), vdp2_state.buffered_regs.coab);
                break;
        case SCRN_COLOR_OFFSET_B:
                vdp2_state.buffered_regs.cobr = r & 0x01FF;
                vdp2_state.buffered_regs.cobg = g & 0x01FF;
                vdp2_state.buffered_regs.cobb = b & 0x01FF;

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(COBR), vdp2_state.buffered_regs.cobr);
                MEMORY_WRITE(16, VDP2(COBG), vdp2_state.buffered_regs.cobg);
                MEMORY_WRITE(16, VDP2(COBB), vdp2_state.buffered_regs.cobb);
                break;
        default:
                return;
        }
}
