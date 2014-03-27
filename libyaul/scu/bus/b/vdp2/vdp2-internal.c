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
vdp2_scrn_character_color_count_set(uint8_t scrn, uint32_t cc_count)
{

#ifdef DEBUG
        assert((cc_count == 16) ||
               (cc_count == 256) ||
               (cc_count == 2048) ||
               (cc_count == 32768) ||
               (cc_count == 16770000));
#endif /* DEBUG */

        switch (cc_count) {
        case 16:
                cc_count = SCRN_CCC_PALETTE_16;
                break;
        case 256:
                cc_count = SCRN_CCC_PALETTE_256;
                break;
        case 2048:
                cc_count = SCRN_CCC_PALETTE_2048;
                break;
        case 32768:
                cc_count = SCRN_CCC_RGB_32768;
                break;
        case 16770000:
                cc_count = SCRN_CCC_RGB_16770000;
                break;
        }

        switch (scrn) {
        case SCRN_RBG1:
        case SCRN_NBG0:
#ifdef DEBUG
                assert((cc_count == SCRN_CCC_PALETTE_16) ||
                       (cc_count == SCRN_CCC_PALETTE_256) ||
                       (cc_count == SCRN_CCC_PALETTE_2048) ||
                       (cc_count == SCRN_CCC_RGB_32768) ||
                       (cc_count == SCRN_CCC_RGB_16770000));
#endif /* DEBUG */

                vdp2_regs.chctla &= 0xFF8F;
                vdp2_regs.chctla |= cc_count << 4;

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(CHCTLA), vdp2_regs.chctla);
                break;
        case SCRN_NBG1:
#ifdef DEBUG
                assert((cc_count == SCRN_CCC_PALETTE_16) ||
                       (cc_count == SCRN_CCC_PALETTE_256) ||
                       (cc_count == SCRN_CCC_PALETTE_2048) ||
                       (cc_count == SCRN_CCC_RGB_32768));
#endif /* DEBUG */

                vdp2_regs.chctla &= 0xCFFF;
                vdp2_regs.chctla |= cc_count << 12;

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(CHCTLA), vdp2_regs.chctla);
                break;
        case SCRN_NBG2:
#ifdef DEBUG
                assert((cc_count == SCRN_CCC_PALETTE_16) ||
                       (cc_count == SCRN_CCC_PALETTE_256));
#endif /* DEBUG */

                vdp2_regs.chctlb &= 0xFFFD;
                vdp2_regs.chctlb |= cc_count << 1;

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(CHCTLB), vdp2_regs.chctlb);
                break;
        case SCRN_NBG3:
#ifdef DEBUG
                assert((cc_count == SCRN_CCC_PALETTE_16) ||
                       (cc_count == SCRN_CCC_PALETTE_256));
#endif /* DEBUG */

                vdp2_regs.chctlb &= 0xFFDF;
                vdp2_regs.chctlb |= cc_count << 5;

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(CHCTLB), vdp2_regs.chctlb);
                break;
        case SCRN_RBG0:
                vdp2_regs.chctlb &= 0x8FFF;
                vdp2_regs.chctlb |= cc_count << 12;

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(CHCTLB), vdp2_regs.chctlb);
                break;
        }
}
