/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <vdp2/scrn.h>

#include "vdp2-internal.h"

void
vdp2_scrn_display_set(uint8_t scrn, bool no_trans)
{
        /* Enable and disable scroll screens. */
        vdp2_regs.bgon |= 1 << (uint8_t)scrn;
        if (no_trans) {
                uint16_t trans_scrn;

                trans_scrn = (uint8_t)scrn + 8;
                vdp2_regs.bgon |= 1 << trans_scrn;
        }

#ifdef DEBUG
        uint16_t cc_count;

        /* When NBG0 is displayed and is set to 2,048 or 32,768 colors,
         * NBG2 cannot be displayed */
        if (((vdp2_regs.bgon & (1 << SCRN_NBG0)) != 0x0000) &&
            ((vdp2_regs.bgon & (1 << SCRN_NBG2)) != 0x0000)) {
                /* Determine the CC_COUNT for NBG0 */
                cc_count = (vdp2_regs.chctla & 0x0070) >> 4;
                assert((cc_count != SCRN_CCC_PALETTE_2048) &&
                       (cc_count != SCRN_CCC_RGB_32768));
        }

        /* When NBG0 is displayed and set to 16,1770,000 colors,
         * NBG1~NBG3 cannot be displayed */
        if (((vdp2_regs.bgon & (1 << SCRN_NBG0)) != 0x0000) &&
            (((vdp2_regs.bgon & (1 << SCRN_NBG1)) != 0x0000) ||
             ((vdp2_regs.bgon & (1 << SCRN_NBG2)) != 0x0000) ||
             ((vdp2_regs.bgon & (1 << SCRN_NBG3)) != 0x0000))) {
                /* Determine the CC_COUNT for NBG0 */
                cc_count = (vdp2_regs.chctla & 0x0070) >> 4;
                assert(cc_count != SCRN_CCC_RGB_16770000);
        }

        /* When NBG1 is displayed and set to 2,048 or 32,768 colors,
         * NBG3 cannot be displayed */
        if (((vdp2_regs.bgon & (1 << SCRN_NBG1)) != 0x0000) &&
            ((vdp2_regs.bgon & (1 << SCRN_NBG3)) != 0x0000)) {
                /* Determine the CC_COUNT for NBG1 */
                cc_count = (vdp2_regs.chctla & 0x3000) >> 12;
                assert((cc_count != SCRN_CCC_PALETTE_2048) &&
                       (cc_count != SCRN_CCC_RGB_32768));
        }
#endif /* DEBUG */

        /* Write to register. */
        MEMORY_WRITE(16, VDP2(BGON), vdp2_regs.bgon);
}
