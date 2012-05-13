/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp2/scrn.h>

#include "vdp2_internal.h"

void
vdp2_scrn_bm_format_set(uint8_t scrn, struct scrn_bm_format *c)
{
        switch (scrn) {
        case SCRN_NBG0:
                /* Screen display format */
                vdp2_regs.chctla &= 0xFFFD;
                vdp2_regs.chctla |= 0x0002;

                /* Bitmap size */
                vdp2_regs.chctla &= 0xFFF3;
                vdp2_regs.chctla |= c->bm_bs << 2;

                /* Special priority */
                vdp2_regs.bmpna &= 0xFFDF;
                vdp2_regs.bmpna |= c->bm_sp << 5;

                /* Supplementary color number */
                vdp2_regs.bmpna &= 0xFFEF;
                vdp2_regs.bmpna |= c->bm_scc << 4;

                /* Supplementary palette number */
                vdp2_regs.bmpna &= 0xFFF8;
                vdp2_regs.bmpna |= (c->bm_spn & 0x7) << 0;

                /* Write to memory */
                MEM_POKE(VDP2(CHCTLA), vdp2_regs.chctla);
                MEM_POKE(VDP2(BMPNA), vdp2_regs.bmpna);
                break;
        case SCRN_NBG1:
                /* Screen display format */
                vdp2_regs.chctla &= 0xFDFF;
                vdp2_regs.chctla |= 0x0200;

                /* Bitmap size */
                vdp2_regs.chctla &= 0xFEFF;
                vdp2_regs.chctla |= c->bm_bs << 10;

                /* Special priority */
                vdp2_regs.bmpna &= 0xDFFF;
                vdp2_regs.bmpna |= c->bm_sp << 13;

                /* Supplementary color number */
                vdp2_regs.bmpna &= 0xEFFF;
                vdp2_regs.bmpna |= c->bm_scc << 12;

                /* Supplementary palette number */
                vdp2_regs.bmpna &= 0xF8FF;
                vdp2_regs.bmpna |= (c->bm_spn & 0x7) << 8;

                /* Write to memory */
                MEM_POKE(VDP2(CHCTLA), vdp2_regs.chctla);
                MEM_POKE(VDP2(BMPNA), vdp2_regs.bmpna);
                break;
        case SCRN_RBG0:
                /* Screen display format */
                vdp2_regs.chctlb &= 0xFDFF;
                vdp2_regs.chctlb |= 0x0200;

                /* Bitmap size */
                vdp2_regs.chctla &= 0xFBFF;
                vdp2_regs.chctla |= (c->bm_bs & 0x1) << 10;

                /* Special priority */
                vdp2_regs.bmpnb &= 0xFFDF;
                vdp2_regs.bmpnb |= c->bm_sp << 5;

                /* Supplementary color number */
                vdp2_regs.bmpnb &= 0xFFEF;
                vdp2_regs.bmpnb |= c->bm_scc << 4;

                /* Supplementary palette number */
                vdp2_regs.bmpnb &= 0xFFF8;
                vdp2_regs.bmpnb |= (c->bm_spn & 0x7) << 0;

                /* Write to memory */
                MEM_POKE(VDP2(CHCTLB), vdp2_regs.chctlb);
                MEM_POKE(VDP2(BMPNB), vdp2_regs.bmpnb);
                break;
        default:
                break;
        }
}
