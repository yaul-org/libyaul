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
vdp2_scrn_display_set(uint8_t scrn, bool transparent)
{
#ifdef DEBUG
        /* Check if the background passed is valid */
        assert((scrn == SCRN_NBG0) ||
               (scrn == SCRN_RBG1) ||
               (scrn == SCRN_NBG1) ||
               (scrn == SCRN_NBG2) ||
               (scrn == SCRN_NBG3) ||
               (scrn == SCRN_RBG0));
#endif /* DEBUG */

        /* Enable and disable scroll screens */
        vdp2_regs.bgon |= 1 << scrn;

        if (!transparent) {
                vdp2_regs.bgon |= 1 << (scrn + 8);
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

        /* Write to register */
        MEMORY_WRITE(16, VDP2(BGON), vdp2_regs.bgon);
}
