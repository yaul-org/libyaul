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

#ifdef DEBUG
static void _debug_check(const struct scrn_bitmap_format *);
#endif /* DEBUG */

static void _nbg0_scrn_bitmap_format_set(const struct scrn_bitmap_format *);
static void _nbg1_scrn_bitmap_format_set(const struct scrn_bitmap_format *);
static void _rbg0_scrn_bitmap_format_set(const struct scrn_bitmap_format *);

void
vdp2_scrn_bitmap_format_set(struct scrn_bitmap_format *format)
{
#ifdef DEBUG
        _debug_check(format);
#endif /* DEBUG */

        switch (format->sbf_scroll_screen) {
        case SCRN_NBG0:
                _nbg0_scrn_bitmap_format_set(format);
                break;
        case SCRN_NBG1:
                _nbg1_scrn_bitmap_format_set(format);
                break;
        case SCRN_RBG0:
                _rbg0_scrn_bitmap_format_set(format);
               break;
        }
}

#ifdef DEBUG
static void
_debug_check(const struct scrn_bitmap_format *format)
{
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
}
#endif /* DEBUG */

static void
_nbg0_scrn_bitmap_format_set(const struct scrn_bitmap_format *format)
{
        _internal_state_vdp2.buffered_regs.chctla &= 0xFF80;      /* Bits 0,1,2,3,4,5,6 */
        _internal_state_vdp2.buffered_regs.mpofn &= 0xFFF8;       /* Bits 0,1,2 */
        _internal_state_vdp2.buffered_regs.bmpna &= 0xFFC8;       /* Bits 0,1,2,4,5 */
        _internal_state_vdp2.buffered_regs.sfsel &= 0xFFFE;
        _internal_state_vdp2.buffered_regs.sfprmd &= 0xFFFC;

        uint16_t cc_count;
        cc_count = (format->sbf_cc_count & 0x07) << 4;

        uint16_t bank;
        bank = VRAM_BANK_4MBIT(format->sbf_bitmap_pattern);

        uint16_t palette_number;
        palette_number = (format->sbf_color_palette >> 9) & 0x07;

        uint16_t width;
        width = format->sbf_bitmap_size.width >> 10;

        uint16_t height;
        height = format->sbf_bitmap_size.height >> 9;

        /* Screen display format */
        /* Character color count */
        /* Bitmap size */
        _internal_state_vdp2.buffered_regs.chctla |= 0x0002;
        _internal_state_vdp2.buffered_regs.chctla |= cc_count;
        _internal_state_vdp2.buffered_regs.chctla |= width << 3;
        _internal_state_vdp2.buffered_regs.chctla |= height << 2;

        /* Pattern boundry lead address */
        _internal_state_vdp2.buffered_regs.mpofn |= bank;

        /* Supplementary palette number */

        /* Specifying a palette number for 2,048-colors is invalid as it
         * uses up nearly all of CRAM (4 KiB).
         *
         * 256-color needs 3 bits as there are 8 possible color banks.
         *
         * For 16-colors, there are 128 possible banks, but only 8 are
         * available (the lower 4- bits of the palette number is 0). */
        switch (format->sbf_cc_count) {
        case SCRN_CCC_PALETTE_16:
        case SCRN_CCC_PALETTE_256:
                _internal_state_vdp2.buffered_regs.bmpna |= palette_number;
                break;
        }

        /* Special function type */
        _internal_state_vdp2.buffered_regs.bmpna |= (format->sbf_sf_type & 0x03) << 4;

        /* Special function */
        _internal_state_vdp2.buffered_regs.sfsel |= (format->sbf_sf_code & 0x01) << 0;
        _internal_state_vdp2.buffered_regs.sfprmd |= (format->sbf_sf_mode & 0x03) << 0;
}

static void
_nbg1_scrn_bitmap_format_set(const struct scrn_bitmap_format *format)
{
        _internal_state_vdp2.buffered_regs.chctla &= 0xC0FF;      /* Bits 8,9,10,11,12,13 */
        _internal_state_vdp2.buffered_regs.mpofn &= 0xFF8F;       /* Bits 4,5,6 */
        _internal_state_vdp2.buffered_regs.bmpna &= 0xC8FF;       /* Bits 8,9,10,12,13 */
        _internal_state_vdp2.buffered_regs.sfsel &= 0xFFFD;
        _internal_state_vdp2.buffered_regs.sfprmd &= 0xFFF3;

        uint16_t cc_count;
        cc_count = (format->sbf_cc_count & 0x03) << 12;

        uint16_t bank;
        bank = VRAM_BANK_4MBIT(format->sbf_bitmap_pattern);

        uint16_t palette_number;
        palette_number = (format->sbf_color_palette >> 9) & 0x07;

        uint16_t width;
        width = format->sbf_bitmap_size.width >> 10;

        uint16_t height;
        height = format->sbf_bitmap_size.height >> 9;

        /* Screen display format */
        /* Character color count */
        /* Bitmap size */
        _internal_state_vdp2.buffered_regs.chctla |= 0x0200;
        _internal_state_vdp2.buffered_regs.chctla |= cc_count;
        _internal_state_vdp2.buffered_regs.chctla |= width << 10;
        _internal_state_vdp2.buffered_regs.chctla |= height << 10;

        /* Pattern boundry lead address */
        _internal_state_vdp2.buffered_regs.mpofn |= bank << 4;

        /* Supplementary palette number */
        switch (format->sbf_cc_count) {
        case SCRN_CCC_PALETTE_16:
        case SCRN_CCC_PALETTE_256:
                _internal_state_vdp2.buffered_regs.bmpna |= palette_number << 8;
                break;
        }

        /* Special function type */
        _internal_state_vdp2.buffered_regs.bmpna |= (format->sbf_sf_type & 0x03) << 12;

        /* Special function */
        _internal_state_vdp2.buffered_regs.sfsel |= (format->sbf_sf_code & 0x01) << 1;
        _internal_state_vdp2.buffered_regs.sfprmd |= (format->sbf_sf_mode & 0x03) << 2;
}

static void
_rbg0_scrn_bitmap_format_set(const struct scrn_bitmap_format *format)
{
#ifdef DEBUG
        assert((format->sbf_bitmap_size.width == 512));

        assert((format->sbf_rp_mode == 0) ||
               (format->sbf_rp_mode == 1) ||
               (format->sbf_rp_mode == 2) ||
               (format->sbf_rp_mode == 3));
#endif /* DEBUG */

        _internal_state_vdp2.buffered_regs.chctlb &= 0xC0FF;      /* Bits 8,9,10,12,13,14*/
        _internal_state_vdp2.buffered_regs.bmpnb &= 0xFFC8;       /* Bits 0,1,2,4,5*/
        _internal_state_vdp2.buffered_regs.rpmd &= 0xFFFC;        /* Bits 0,1 */
        _internal_state_vdp2.buffered_regs.sfsel &= 0xFFEF;
        _internal_state_vdp2.buffered_regs.sfprmd &= 0xFCFF;

        uint16_t cc_count;
        cc_count = (format->sbf_cc_count & 0x03) << 12;

        uint16_t bank;
        bank = VRAM_BANK_4MBIT(format->sbf_bitmap_pattern);

        uint16_t palette_number;
        palette_number = (format->sbf_color_palette >> 9) & 0x07;

        uint16_t width;
        width = format->sbf_bitmap_size.width >> 10;

        uint16_t height;
        height = format->sbf_bitmap_size.height >> 9;

        /* Screen display format */
        /* Character color count */
        /* Bitmap size */
        _internal_state_vdp2.buffered_regs.chctlb |= 0x0200;
        _internal_state_vdp2.buffered_regs.chctlb |= cc_count;
        _internal_state_vdp2.buffered_regs.chctlb |= width << 10;
        _internal_state_vdp2.buffered_regs.chctlb |= height << 10;

        /* Supplementary palette number */
        switch (format->sbf_cc_count) {
        case SCRN_CCC_PALETTE_16:
        case SCRN_CCC_PALETTE_256:
                _internal_state_vdp2.buffered_regs.bmpnb |= palette_number;
                break;
        }

        /* Rotation parameter mode */
        _internal_state_vdp2.buffered_regs.rpmd |= format->sbf_rp_mode;

        switch (format->sbf_rp_mode) {
        case 0:
                /* Mode 0: Rotation Parameter A */

                /* Pattern boundry lead address */
                _internal_state_vdp2.buffered_regs.mpofr &= 0xFFF8;
                _internal_state_vdp2.buffered_regs.mpofr |= bank;
                break;
        case 1:
                /* Mode 1: Rotation Parameter B */

                /* Pattern boundry lead address */
                _internal_state_vdp2.buffered_regs.mpofr &= 0xFF8F;
                _internal_state_vdp2.buffered_regs.mpofr |= bank << 4;
                break;
        }

        /* Special function type */
        _internal_state_vdp2.buffered_regs.bmpnb |= (format->sbf_sf_type & 0x03) << 4;

        /* Special function */
        _internal_state_vdp2.buffered_regs.sfsel |= (format->sbf_sf_code & 0x01) << 4;
        _internal_state_vdp2.buffered_regs.sfprmd |= (format->sbf_sf_mode & 0x03) << 8;
}
