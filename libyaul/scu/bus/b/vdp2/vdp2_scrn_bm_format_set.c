/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp2/scrn.h>

#include "vdp2_internal.h"

static unsigned short bmpna = 0;
static unsigned short bmpnb = 0;

void
vdp2_scrn_bm_format_set(enum scrn_type scrn, struct scrn_bm_format *c)
{
        switch (scrn) {
        case SCRN_NBG0:
                /* Screen Display Format */
                vdp2_regs.chctla &= 0xfffd;
                vdp2_regs.chctla |= 0x0002;

                /* Bitmap Size */
                vdp2_regs.chctla &= 0xfff3;
                vdp2_regs.chctla |= c->bm_bs << 2;

                /* Special Priority */
                bmpna &= 0xffdf;
                bmpna |= c->bm_sp << 5;

                /* Supplementary Color Number */
                bmpna &= 0xffef;
                bmpna |= c->bm_scc << 4;

                /* Supplementary Palette Number */
                bmpna &= 0xfff8;
                bmpna |= (c->bm_spn & 0x7) << 0;

                /* Write to memory. */
                MEM_POKE(VDP2(CHCTLA), vdp2_regs.chctla);
                MEM_POKE(VDP2(BMPNA), bmpna);
                break;
        case SCRN_NBG1:
                /* Screen Display Format */
                vdp2_regs.chctla &= 0xfdff;
                vdp2_regs.chctla |= 0x0200;

                /* Bitmap Size */
                vdp2_regs.chctla &= 0xfeff;
                vdp2_regs.chctla |= c->bm_bs << 10;

                /* Special Priority */
                bmpna &= 0xdfff;
                bmpna |= c->bm_sp << 13;

                /* Supplementary Color Number */
                bmpna &= 0xefff;
                bmpna |= c->bm_scc << 12;

                /* Supplementary Palette Number */
                bmpna &= 0xf8ff;
                bmpna |= (c->bm_spn & 0x7) << 8;

                /* Write to memory. */
                MEM_POKE(VDP2(CHCTLA), vdp2_regs.chctla);
                MEM_POKE(VDP2(BMPNA), bmpna);
                break;
        case SCRN_RBG0:
                /* Screen Display Format */
                vdp2_regs.chctlb &= 0xfdff;
                vdp2_regs.chctlb |= 0x0200;

                /* Bitmap Size */
                vdp2_regs.chctla &= 0xfbff;
                vdp2_regs.chctla |= (c->bm_bs & 0x1) << 10;

                /* Special Priority */
                bmpnb &= 0xffdf;
                bmpnb |= c->bm_sp << 5;

                /* Supplementary Color Number */
                bmpnb &= 0xffef;
                bmpnb |= c->bm_scc << 4;

                /* Supplementary Palette Number */
                bmpnb &= 0xfff8;
                bmpnb |= (c->bm_spn & 0x7) << 0;

                /* Write to memory. */
                MEM_POKE(VDP2(CHCTLB), vdp2_regs.chctlb);
                MEM_POKE(VDP2(BMPNB), bmpnb);
                break;
        default:
                break;
        }
}
