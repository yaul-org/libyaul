/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>
#include <vdp2/vram.h>

#ifdef DEBUG
#include <assert.h>
#endif /* DEBUG */

#include "vdp2-internal.h"

void
vdp2_scrn_bitmap_format_set(struct scrn_bitmap_format *format)
{
#ifdef DEBUG
        assert(format != NULL);

        /* Check if the background passed is valid */
        assert((format->sbf_scroll_screen == SCRN_NBG0) ||
               (format->sbf_scroll_screen == SCRN_NBG1) ||
               (format->sbf_scroll_screen == SCRN_RBG0));

        /* assert that the lead address to the color palette in CRAM is
         * on a 20-byte boundary */
        assert((format->sbf_color_palette & 0x1F) == 0x00);

        assert((format->sbf_cc_count == SCRN_CCC_PALETTE_16) ||
               (format->sbf_cc_count == SCRN_CCC_PALETTE_256) ||
               (format->sbf_cc_count == SCRN_CCC_PALETTE_2048) ||
               (format->sbf_cc_count == SCRN_CCC_RGB_32768) ||
               (format->sbf_cc_count == SCRN_CCC_RGB_16770000));

        assert((format->sbf_bitmap_size.width == 512) ||
               (format->sbf_bitmap_size.width == 1024));

        assert((format->sbf_bitmap_size.height == 256) ||
               (format->sbf_bitmap_size.height == 512));
#endif /* DEBUG */

        uint32_t bank;

        switch (vram_ctl.vram_size) {
        case VRAM_CTL_SIZE_4MBIT:
                bank = VRAM_BANK_4MBIT(format->sbf_bitmap_pattern);
                break;
        case VRAM_CTL_SIZE_8MBIT:
                bank = VRAM_BANK_8MBIT(format->sbf_bitmap_pattern);
                break;
        }

        uint16_t palette_number;

        palette_number = (format->sbf_color_palette >> 9) & 0x07;

        switch (format->sbf_scroll_screen) {
        case SCRN_NBG0:
                /* Screen display format */
                vdp2_regs.chctla &= 0xFFFD;
                vdp2_regs.chctla |= 0x0002;

                /* Character color count */
                vdp2_regs.chctla &= 0xFF8F;
                vdp2_regs.chctla |= format->sbf_cc_count << 4;

                /* Pattern boundry lead address */
                vdp2_regs.mpofn &= 0xFFF8;
                vdp2_regs.mpofn |= bank;

                /* Bitmap size */
                vdp2_regs.chctla &= 0xFFF3;
                vdp2_regs.chctla |= ((format->sbf_bitmap_size.width >> 10) << 3) |
                    ((format->sbf_bitmap_size.height >> 9) << 2);

                /* Supplementary palette number */
                vdp2_regs.bmpna &= 0xFFF8;
                switch (format->sbf_cc_count) {
                case SCRN_CCC_PALETTE_16:
                case SCRN_CCC_PALETTE_256:
                        vdp2_regs.bmpna |= palette_number;
                        break;
                }

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(CHCTLA), vdp2_regs.chctla);
                MEMORY_WRITE(16, VDP2(BMPNA), vdp2_regs.bmpna);
                MEMORY_WRITE(16, VDP2(MPOFN), vdp2_regs.mpofn);
                break;
        case SCRN_NBG1:
                /* Screen display format */
                vdp2_regs.chctla &= 0xFDFF;
                vdp2_regs.chctla |= 0x0200;

                /* Character color count */
                vdp2_regs.chctla &= 0xCFFF;
                vdp2_regs.chctla |= format->sbf_cc_count << 12;

                /* Pattern boundry lead address */
                vdp2_regs.mpofn &= 0xFF8F;
                vdp2_regs.mpofn |= bank << 4;

                /* Bitmap size */
                vdp2_regs.chctla &= 0xF3FF;
                vdp2_regs.chctla |= ((format->sbf_bitmap_size.width >> 10) << 11) |
                    ((format->sbf_bitmap_size.height >> 9) << 10);

                /* Supplementary palette number */
                vdp2_regs.bmpna &= 0xF8FF;
                switch (format->sbf_cc_count) {
                case SCRN_CCC_PALETTE_16:
                case SCRN_CCC_PALETTE_256:
                        vdp2_regs.bmpna |= palette_number << 8;
                        break;
                }

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(CHCTLA), vdp2_regs.chctla);
                MEMORY_WRITE(16, VDP2(BMPNA), vdp2_regs.bmpna);
                MEMORY_WRITE(16, VDP2(MPOFN), vdp2_regs.mpofn);
                break;
        case SCRN_RBG0:
#ifdef DEBUG
                assert((format->sbf_bitmap_size.width == 512));

                assert((format->sbf_rp_mode == 0) ||
                       (format->sbf_rp_mode == 1) ||
                       (format->sbf_rp_mode == 2) ||
                       (format->sbf_rp_mode == 3));
#endif /* DEBUG */

                /* Screen display format */
                vdp2_regs.chctlb &= 0xFDFF;
                vdp2_regs.chctlb |= 0x0200;

                /* Character color count */
                vdp2_regs.chctlb &= 0x8FFF;
                vdp2_regs.chctlb |= format->sbf_cc_count << 12;

                /* Bitmap size */
                vdp2_regs.chctlb &= 0xFBFF;
                vdp2_regs.chctlb |= (format->sbf_bitmap_size.height >> 9) << 10;

                /* Supplementary palette number */
                vdp2_regs.bmpnb &= 0xFFF8;
                switch (format->sbf_cc_count) {
                case SCRN_CCC_PALETTE_16:
                case SCRN_CCC_PALETTE_256:
                        vdp2_regs.bmpnb |= palette_number;
                        break;
                }

                /* Rotation parameter mode */
                vdp2_regs.rpmd &= 0xFFFE;
                vdp2_regs.rpmd |= format->sbf_rp_mode;

                switch (format->sbf_rp_mode) {
                case 0:
                        /* Mode 0: Rotation Parameter A */

                        /* Map */
                        vdp2_regs.mpofr &= 0xFF8F;
                        vdp2_regs.mpofr |= bank << 4;
                        break;
                case 1:
                        /* Mode 1: Rotation Parameter B */

                        /* Map */
                        vdp2_regs.mpofr &= 0xFF8F;
                        vdp2_regs.mpofr |= bank << 4;
                        break;
                }

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(CHCTLB), vdp2_regs.chctlb);
                MEMORY_WRITE(16, VDP2(BMPNB), vdp2_regs.bmpnb);
                MEMORY_WRITE(16, VDP2(MPOFR), vdp2_regs.mpofr);
                break;
        }
}
