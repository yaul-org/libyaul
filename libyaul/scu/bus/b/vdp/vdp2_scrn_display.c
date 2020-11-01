/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <vdp2/scrn.h>

#include "vdp-internal.h"

static const uint8_t _bgon_bits[] __aligned(4) = {
        0x01, /* NBG0 */
        0x02, /* NBG1 */
        0x04, /* NBG2 */
        0x08, /* NBG3 */
        0x10, /* RBG0 */
        0x20, /* RBG1 */
        0x00,
        0x00
};

void
vdp2_scrn_display_set(uint8_t scrn, bool transparent)
{
#ifdef DEBUG
        /* Check if the background passed is valid */
        assert((scrn == VDP2_SCRN_NBG0) ||
               (scrn == VDP2_SCRN_RBG1) ||
               (scrn == VDP2_SCRN_NBG1) ||
               (scrn == VDP2_SCRN_NBG2) ||
               (scrn == VDP2_SCRN_NBG3) ||
               (scrn == VDP2_SCRN_RBG0));
#endif /* DEBUG */

        const uint16_t bgon_bits = _bgon_bits[scrn];

        _state_vdp2()->regs->bgon |= bgon_bits;

        if (!transparent) {
                _state_vdp2()->regs->bgon |= bgon_bits << 8;
        }

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
vdp2_scrn_display_unset(uint8_t scrn)
{
#ifdef DEBUG
        /* Check if the background passed is valid */
        assert((scrn == VDP2_SCRN_NBG0) ||
               (scrn == VDP2_SCRN_RBG1) ||
               (scrn == VDP2_SCRN_NBG1) ||
               (scrn == VDP2_SCRN_NBG2) ||
               (scrn == VDP2_SCRN_NBG3) ||
               (scrn == VDP2_SCRN_RBG0));
#endif /* DEBUG */

        const uint16_t bgon_bits = _bgon_bits[scrn];

        _state_vdp2()->regs->bgon &= (~bgon_bits) | (~(bgon_bits << 8));
}

void
vdp2_scrn_display_clear(void)
{
        _state_vdp2()->regs->bgon = 0x0000;
}
