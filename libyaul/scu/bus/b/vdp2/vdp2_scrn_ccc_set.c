/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include "vdp2-internal.h"

void
vdp2_scrn_ccc_set(uint8_t scrn, uint8_t chc)
{
        switch (scrn) {
        case SCRN_RBG1:
        case SCRN_NBG0:
#ifdef DEBUG
                assert((chc == SCRN_CCC_CHC_16) ||
                       (chc == SCRN_CCC_CHC_256) ||
                       (chc == SCRN_CCC_CHC_2048) ||
                       (chc == SCRN_CCC_CHC_32768) ||
                       (chc == SCRN_CCC_CHC_16770000));
#endif /* DEBUG */

                vdp2_regs.chctla &= 0xFF8F;
                vdp2_regs.chctla |= chc << 4;

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(CHCTLA), vdp2_regs.chctla);
                break;
        case SCRN_NBG1:
#ifdef DEBUG
                assert((chc == SCRN_CCC_CHC_16) ||
                       (chc == SCRN_CCC_CHC_256) ||
                       (chc == SCRN_CCC_CHC_2048) ||
                       (chc == SCRN_CCC_CHC_32768));
#endif /* DEBUG */

                vdp2_regs.chctla &= 0xCFFF;
                vdp2_regs.chctla |= chc << 12;

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(CHCTLA), vdp2_regs.chctla);
                break;
        case SCRN_NBG2:
#ifdef DEBUG
                assert((chc == SCRN_CCC_CHC_16) || (chc == SCRN_CCC_CHC_256));
#endif /* DEBUG */

                vdp2_regs.chctlb &= 0xFFFD;
                vdp2_regs.chctlb |= chc << 1;

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(CHCTLB), vdp2_regs.chctlb);
                break;
        case SCRN_NBG3:
#ifdef DEBUG
                assert((chc == SCRN_CCC_CHC_16) || (chc == SCRN_CCC_CHC_256));
#endif /* DEBUG */

                vdp2_regs.chctlb &= 0xFFDF;
                vdp2_regs.chctlb |= chc << 5;

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(CHCTLB), vdp2_regs.chctlb);
                break;
        case SCRN_RBG0:
#ifdef DEBUG
                assert((chc == SCRN_CCC_CHC_16) ||
                       (chc == SCRN_CCC_CHC_256) ||
                       (chc == SCRN_CCC_CHC_2048) ||
                       (chc == SCRN_CCC_CHC_32768) ||
                       (chc == SCRN_CCC_CHC_16770000));
#endif /* DEBUG */

                vdp2_regs.chctlb &= 0x8FFF;
                vdp2_regs.chctlb |= chc << 12;

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(CHCTLB), vdp2_regs.chctlb);
        default:
                break;
        }
}
