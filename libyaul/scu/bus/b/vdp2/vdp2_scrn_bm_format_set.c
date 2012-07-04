/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>
#include <vdp2/vram.h>

#include "vdp2-internal.h"

void
vdp2_scrn_bm_format_set(struct scrn_bm_format *b)
{
        uint8_t offset;

        switch (vram_ctl.vram_size) {
        case VRAM_CTL_SIZE_4MBIT:
                offset = VRAM_OFFSET_4MBIT(b->bm_pb);
                break;
        case VRAM_CTL_SIZE_8MBIT:
                offset = VRAM_OFFSET_4MBIT(b->bm_pb);
                break;
        default:
                return;
        }

        switch (b->bm_scrn) {
        case SCRN_NBG0:
                /* Screen display format */
                vdp2_regs.chctla &= 0xFFFD;
                vdp2_regs.chctla |= 0x0002;

                /* Pattern boundry lead address */
                vdp2_regs.mpofn &= 0xFFF8;
                vdp2_regs.mpofn |= offset;

                /* Bitmap size */
                vdp2_regs.chctla &= 0xFFF3;
                vdp2_regs.chctla |= b->bm_bs << 2;

                /* Special priority */
                vdp2_regs.bmpna &= 0xFFDF;
                vdp2_regs.bmpna |= b->bm_sp << 5;

                /* special color calculation */
                vdp2_regs.bmpna &= 0xFFEF;
                vdp2_regs.bmpna |= b->bm_scc << 4;

                /* Supplementary palette number */
                vdp2_regs.bmpna &= 0xFFF8;
                vdp2_regs.bmpna |= b->bm_spn & 0x07;

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(CHCTLA), vdp2_regs.chctla);
                MEMORY_WRITE(16, VDP2(BMPNA), vdp2_regs.bmpna);
                MEMORY_WRITE(16, VDP2(MPOFN), vdp2_regs.mpofn);
                break;
        case SCRN_NBG1:
                /* Screen display format */
                vdp2_regs.chctla &= 0xFDFF;
                vdp2_regs.chctla |= 0x0200;

                /* Pattern boundry lead address */
                vdp2_regs.mpofn &= 0xFF8F;
                vdp2_regs.mpofn |= offset << 4;

                /* Bitmap size */
                vdp2_regs.chctla &= 0xFEFF;
                vdp2_regs.chctla |= b->bm_bs << 10;

                /* Special priority */
                vdp2_regs.bmpna &= 0xDFFF;
                vdp2_regs.bmpna |= b->bm_sp << 13;

                /* Supplementary color number */
                vdp2_regs.bmpna &= 0xEFFF;
                vdp2_regs.bmpna |= b->bm_scc << 12;

                /* Supplementary palette number */
                vdp2_regs.bmpna &= 0xF8FF;
                vdp2_regs.bmpna |= (b->bm_spn & 0x07) << 8;

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(CHCTLA), vdp2_regs.chctla);
                MEMORY_WRITE(16, VDP2(BMPNA), vdp2_regs.bmpna);
                break;
        case SCRN_RBG0:
                /* Screen display format */
                vdp2_regs.chctlb &= 0xFDFF;
                vdp2_regs.chctlb |= 0x0200;

                /* Bitmap size */
                vdp2_regs.chctla &= 0xFBFF;
                vdp2_regs.chctla |= (b->bm_bs & 0x01) << 10;

                /* Special priority */
                vdp2_regs.bmpnb &= 0xFFDF;
                vdp2_regs.bmpnb |= b->bm_sp << 5;

                /* Supplementary color number */
                vdp2_regs.bmpnb &= 0xFFEF;
                vdp2_regs.bmpnb |= b->bm_scc << 4;

                /* Supplementary palette number */
                vdp2_regs.bmpnb &= 0xFFF8;
                vdp2_regs.bmpnb |= b->bm_spn & 0x07;

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(CHCTLB), vdp2_regs.chctlb);
                MEMORY_WRITE(16, VDP2(BMPNB), vdp2_regs.bmpnb);
                MEMORY_WRITE(16, VDP2(MPOFN), vdp2_regs.mpofn);
                break;
        default:
                return;
        }
}
