/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <vdp2/scrn.h>

#include "vdp-internal.h"

void
vdp2_scrn_display_set(vdp2_scrn_disp_t disp_mask)
{
        _state_vdp2()->regs->bgon |= disp_mask;

#ifdef DEBUG
        uint16_t cc_count;

        /* When NBG0 is displayed and is set to 2,048 or 32,768 colors,
         * NBG2 cannot be displayed */
        if (((_state_vdp2()->regs->bgon & (1 << VDP2_SCRN_NBG0)) != 0x0000) &&
            ((_state_vdp2()->regs->bgon & (1 << VDP2_SCRN_NBG2)) != 0x0000)) {
                /* Determine the CC_COUNT for NBG0 */
                cc_count = (_state_vdp2()->regs->chctla & 0x0070) >> 4;
                assert((cc_count != VDP2_SCRN_CCC_PALETTE_2048) &&
                       (cc_count != VDP2_SCRN_CCC_RGB_32768));
        }

        /* When NBG0 is displayed and set to 16,1770,000 colors,
         * NBG1~NBG3 cannot be displayed */
        if (((_state_vdp2()->regs->bgon & (1 << VDP2_SCRN_NBG0)) != 0x0000) &&
           (((_state_vdp2()->regs->bgon & (1 << VDP2_SCRN_NBG1)) != 0x0000) ||
            ((_state_vdp2()->regs->bgon & (1 << VDP2_SCRN_NBG2)) != 0x0000) ||
            ((_state_vdp2()->regs->bgon & (1 << VDP2_SCRN_NBG3)) != 0x0000))) {
                /* Determine the CC_COUNT for NBG0 */
                cc_count = (_state_vdp2()->regs->chctla & 0x0070) >> 4;
                assert(cc_count != VDP2_SCRN_CCC_RGB_16770000);
        }

        /* When NBG1 is displayed and set to 2,048 or 32,768 colors,
         * NBG3 cannot be displayed */
        if (((_state_vdp2()->regs->bgon & (1 << VDP2_SCRN_NBG1)) != 0x0000) &&
            ((_state_vdp2()->regs->bgon & (1 << VDP2_SCRN_NBG3)) != 0x0000)) {
                /* Determine the CC_COUNT for NBG1 */
                cc_count = (_state_vdp2()->regs->chctla & 0x3000) >> 12;
                assert((cc_count != VDP2_SCRN_CCC_PALETTE_2048) &&
                       (cc_count != VDP2_SCRN_CCC_RGB_32768));
        }
#endif /* DEBUG */
}

void
vdp2_scrn_display_unset(vdp2_scrn_disp_t disp_mask)
{
        _state_vdp2()->regs->bgon &= ~disp_mask;
}

void
vdp2_scrn_display_clear(void)
{
        _state_vdp2()->regs->bgon = 0x0000;
}
