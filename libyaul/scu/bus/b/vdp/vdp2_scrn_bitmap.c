/*
 * Copyright (c) Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/cram.h>
#include <vdp2/scrn.h>
#include <vdp2/vram.h>

#include "vdp-internal.h"

void
vdp2_scrn_bitmap_format_set(const vdp2_scrn_bitmap_format_t *bitmap_format)
{
    vdp2_scrn_bitmap_ccc_set(bitmap_format);
    vdp2_scrn_bitmap_base_set(bitmap_format);
    vdp2_scrn_bitmap_size_set(bitmap_format);
}

void
vdp2_scrn_bitmap_ccc_set(const vdp2_scrn_bitmap_format_t *bitmap_format)
{
    /* Supplementary palette number */
    const uint16_t palette_number = (((vdp2_cram_mode_get()) == 2)
      ? (bitmap_format->palette_base >> 10)
      : (bitmap_format->palette_base >> 9)) & 0x07;

    uint16_t bmpna_bits;
    bmpna_bits = 0x0000;

    uint16_t *bmp_reg;
    bmp_reg = NULL;

    switch (bitmap_format->scroll_screen) {
    case VDP2_SCRN_NBG0:
        _state_vdp2()->shadow_regs.chctla &= 0xFF8C; /* Bits 0, 4, 5, 6 */
        _state_vdp2()->shadow_regs.chctla |= ((bitmap_format->ccc & 0x07) << 4) | 0x0002;

        bmp_reg = &_state_vdp2()->shadow_regs.bmpna;
        bmpna_bits = palette_number;

        *bmp_reg &= 0xFFC8; /* Bits 0, 1, 2, 4, 5 */
        break;
    case VDP2_SCRN_NBG1:
        _state_vdp2()->shadow_regs.chctla &= 0xCCFF; /* Bits 8, 9, 12, 13 */
        _state_vdp2()->shadow_regs.chctla |= ((bitmap_format->ccc & 0x03) << 12) | 0x0200;

        bmp_reg = &_state_vdp2()->shadow_regs.bmpna;
        bmpna_bits = palette_number << 8;

        *bmp_reg &= 0xC8FF; /* Bits 8, 9, 10, 12, 13 */
        break;
    case VDP2_SCRN_RBG0:
    case VDP2_SCRN_RBG0_PA:
    case VDP2_SCRN_RBG0_PB:
        _state_vdp2()->shadow_regs.chctlb &= 0x8CFF; /* Bits 8, 9, 12, 13, 14 */
        _state_vdp2()->shadow_regs.chctlb |= ((bitmap_format->ccc & 0x03) << 12) | 0x0200;

        bmp_reg = &_state_vdp2()->shadow_regs.bmpnb;
        bmpna_bits = palette_number;

        *bmp_reg &= 0xFFC8; /* Bits 0, 1, 2, 4, 5 */
        break;
    default:
        break;
    }

    /* Specifying a palette number for 2,048-colors is invalid as it uses up
     * nearly all of CRAM (4 KiB).
     *
     * 256-color needs 3 bits as there are 8 possible color banks.
     *
     * For 16-colors, there are 128 possible banks, but only 8 are available
     * (the lower 4- bits of the palette number is 0) */
    switch (bitmap_format->ccc) {
    case VDP2_SCRN_CCC_PALETTE_16:
    case VDP2_SCRN_CCC_PALETTE_256:
        *bmp_reg |= bmpna_bits;
        break;
    default:
        break;
    }
}

void
vdp2_scrn_bitmap_base_set(const vdp2_scrn_bitmap_format_t *bitmap_format)
{
    const uint16_t bank = VDP2_VRAM_BANK(bitmap_format->bitmap_base);

    switch (bitmap_format->scroll_screen) {
    case VDP2_SCRN_NBG0:
        _state_vdp2()->shadow_regs.mpofn &= 0xFFF8; /* Bits 0, 1, 2 */
        _state_vdp2()->shadow_regs.mpofn |= bank;
        break;
    case VDP2_SCRN_NBG1:
        _state_vdp2()->shadow_regs.mpofn &= 0xFF8F; /* Bits 4,5,6 */
        _state_vdp2()->shadow_regs.mpofn |= bank << 4;
        break;
    case VDP2_SCRN_RBG0:
    case VDP2_SCRN_RBG0_PA:
        _state_vdp2()->shadow_regs.mpofr &= 0xFFF8;
        _state_vdp2()->shadow_regs.mpofr |= bank;
        break;
    case VDP2_SCRN_RBG0_PB:
        _state_vdp2()->shadow_regs.mpofr &= 0xFF8F;
        _state_vdp2()->shadow_regs.mpofr |= bank << 4;
        break;
    default:
        break;
    }
}

void
vdp2_scrn_bitmap_size_set(const vdp2_scrn_bitmap_format_t *bitmap_format)
{
    switch (bitmap_format->scroll_screen) {
    case VDP2_SCRN_NBG0:
        _state_vdp2()->shadow_regs.chctla &= 0xFFF3;
        _state_vdp2()->shadow_regs.chctla |= bitmap_format->bitmap_size;
        break;
    case VDP2_SCRN_NBG1:
        _state_vdp2()->shadow_regs.chctla &= 0xF3FF;
        _state_vdp2()->shadow_regs.chctla |= bitmap_format->bitmap_size << 8;
        break;
    case VDP2_SCRN_RBG0_PA:
    case VDP2_SCRN_RBG0_PB:
        _state_vdp2()->shadow_regs.chctlb &= 0xFBFF;
        _state_vdp2()->shadow_regs.chctlb |= (bitmap_format->bitmap_size & 0x0004) << 10;
        break;
    default:
        break;
    }
}
