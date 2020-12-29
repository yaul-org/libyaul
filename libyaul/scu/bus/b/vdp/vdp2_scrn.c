/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <string.h>

#include <vdp2/cram.h>
#include <vdp2/scrn.h>
#include <vdp2/vram.h>

#include "vdp-internal.h"

static void _cell_plane_calc(const vdp2_scrn_cell_format_t *, uint16_t *,
    uint16_t *);
static uint16_t _cell_pattern_name_control_calc(
    const vdp2_scrn_cell_format_t *);

static void _nbg0_scrn_cell_format_set(const vdp2_scrn_cell_format_t *);
static void _nbg1_scrn_cell_format_set(const vdp2_scrn_cell_format_t *);
static void _nbg2_scrn_cell_format_set(const vdp2_scrn_cell_format_t *);
static void _nbg3_scrn_cell_format_set(const vdp2_scrn_cell_format_t *);
static void _rbg0_scrn_cell_format_set(const vdp2_scrn_cell_format_t *);

static void _nbg0_scrn_bitmap_format_set(const vdp2_scrn_bitmap_format_t *);
static void _nbg1_scrn_bitmap_format_set(const vdp2_scrn_bitmap_format_t *);
static void _rbg0_scrn_bitmap_format_set(const vdp2_scrn_bitmap_format_t *);

void
vdp2_scrn_cell_format_set(const vdp2_scrn_cell_format_t *format)
{
        switch (format->scroll_screen) {
        case VDP2_SCRN_NBG0:
                _nbg0_scrn_cell_format_set(format);
                break;
        case VDP2_SCRN_NBG1:
                _nbg1_scrn_cell_format_set(format);
                break;
        case VDP2_SCRN_NBG2:
                _nbg2_scrn_cell_format_set(format);
                break;
        case VDP2_SCRN_NBG3:
                _nbg3_scrn_cell_format_set(format);
                break;
        case VDP2_SCRN_RBG0:
                _rbg0_scrn_cell_format_set(format);
                break;
        default:
                return;
        }
}

void
vdp2_scrn_bitmap_format_set(const vdp2_scrn_bitmap_format_t *format)
{
        switch (format->scroll_screen) {
        case VDP2_SCRN_NBG0:
                _nbg0_scrn_bitmap_format_set(format);
                break;
        case VDP2_SCRN_NBG1:
                _nbg1_scrn_bitmap_format_set(format);
                break;
        case VDP2_SCRN_RBG0:
                _rbg0_scrn_bitmap_format_set(format);
                break;
        default:
                return;
        }
}

static void
_nbg0_scrn_bitmap_format_set(const vdp2_scrn_bitmap_format_t *format)
{
        _state_vdp2()->regs->chctla &= 0xFF80; /* Bits 0, 1, 2, 3, 4, 5, 6 */
        _state_vdp2()->regs->mpofn &= 0xFFF8;  /* Bits 0, 1, 2 */
        _state_vdp2()->regs->bmpna &= 0xFFC8;  /* Bits 0, 1, 2, 4, 5 */
        _state_vdp2()->regs->sfsel &= 0xFFFE;
        _state_vdp2()->regs->sfprmd &= 0xFFFC;

        uint16_t cc_count;
        cc_count = (format->cc_count & 0x07) << 4;

        uint16_t bank;
        bank = VDP2_VRAM_BANK(format->bitmap_pattern);

        const uint16_t palette_number = (((vdp2_cram_mode_get()) == 2) ?
            (format->color_palette >> 10) :
            (format->color_palette >> 9)) & 0x07;

        uint16_t width;
        width = format->bitmap_size.width >> 10;

        uint16_t height;
        height = format->bitmap_size.height >> 9;

        /* Screen display format */
        /* Character color count */
        /* Bitmap size */
        _state_vdp2()->regs->chctla |= 0x0002;
        _state_vdp2()->regs->chctla |= cc_count;
        _state_vdp2()->regs->chctla |= width << 3;
        _state_vdp2()->regs->chctla |= height << 2;

        /* Pattern boundry lead address */
        _state_vdp2()->regs->mpofn |= bank;

        /* Supplementary palette number */

        /* Specifying a palette number for 2,048-colors is invalid as it
         * uses up nearly all of CRAM (4 KiB).
         *
         * 256-color needs 3 bits as there are 8 possible color banks.
         *
         * For 16-colors, there are 128 possible banks, but only 8 are
         * available (the lower 4- bits of the palette number is 0). */
        switch (format->cc_count) {
        case VDP2_SCRN_CCC_PALETTE_16:
        case VDP2_SCRN_CCC_PALETTE_256:
                _state_vdp2()->regs->bmpna |= palette_number;
                break;
        default:
                break;
        }

        /* Special function type */
        _state_vdp2()->regs->bmpna |= (format->sf_type & 0x03) << 4;

        /* Special function */
        _state_vdp2()->regs->sfsel |= (format->sf_code & 0x01) << 0;
        _state_vdp2()->regs->sfprmd |= (format->sf_mode & 0x03) << 0;
}

static void
_nbg1_scrn_bitmap_format_set(const vdp2_scrn_bitmap_format_t *format)
{
        _state_vdp2()->regs->chctla &= 0xC0FF; /* Bits 8,9,10,11,12,13 */
        _state_vdp2()->regs->mpofn &= 0xFF8F;  /* Bits 4,5,6 */
        _state_vdp2()->regs->bmpna &= 0xC8FF;  /* Bits 8,9,10,12,13 */
        _state_vdp2()->regs->sfsel &= 0xFFFD;
        _state_vdp2()->regs->sfprmd &= 0xFFF3;

        uint16_t cc_count;
        cc_count = (format->cc_count & 0x03) << 12;

        uint16_t bank;
        bank = VDP2_VRAM_BANK(format->bitmap_pattern);

        const uint16_t palette_number = (((vdp2_cram_mode_get()) == 2) ?
            (format->color_palette >> 10) :
            (format->color_palette >> 9)) & 0x07;

        uint16_t width;
        width = format->bitmap_size.width >> 10;

        uint16_t height;
        height = format->bitmap_size.height >> 9;

        /* Screen display format */
        /* Character color count */
        /* Bitmap size */
        _state_vdp2()->regs->chctla |= 0x0200;
        _state_vdp2()->regs->chctla |= cc_count;
        _state_vdp2()->regs->chctla |= width << 10;
        _state_vdp2()->regs->chctla |= height << 10;

        /* Pattern boundry lead address */
        _state_vdp2()->regs->mpofn |= bank << 4;

        /* Supplementary palette number */
        switch (format->cc_count) {
        case VDP2_SCRN_CCC_PALETTE_16:
        case VDP2_SCRN_CCC_PALETTE_256:
                _state_vdp2()->regs->bmpna |= palette_number << 8;
                break;
        default:
                break;
        }

        /* Special function type */
        _state_vdp2()->regs->bmpna |= (format->sf_type & 0x03) << 12;

        /* Special function */
        _state_vdp2()->regs->sfsel |= (format->sf_code & 0x01) << 1;
        _state_vdp2()->regs->sfprmd |= (format->sf_mode & 0x03) << 2;
}

static void
_rbg0_scrn_bitmap_format_set(const vdp2_scrn_bitmap_format_t *format)
{
        _state_vdp2()->regs->chctlb &= 0xC0FF; /* Bits 8, 9, 10, 12, 13, 14 */
        _state_vdp2()->regs->bmpnb &= 0xFFC8;  /* Bits 0, 1,  2,  4,  5 */
        _state_vdp2()->regs->rpmd &= 0xFFFC;   /* Bits 0, 1 */
        _state_vdp2()->regs->sfsel &= 0xFFEF;
        _state_vdp2()->regs->sfprmd &= 0xFCFF;
        _state_vdp2()->regs->ramctl &= 0xB300;

        uint16_t cc_count;
        cc_count = (format->cc_count & 0x03) << 12;

        uint16_t bank;
        bank = VDP2_VRAM_BANK(format->bitmap_pattern);

        const uint16_t palette_number = (((vdp2_cram_mode_get()) == 2) ?
            (format->color_palette >> 10) :
            (format->color_palette >> 9)) & 0x07;

        uint16_t width;
        width = format->bitmap_size.width >> 10;

        uint16_t height;
        height = format->bitmap_size.height >> 9;

        /* Screen display format */
        /* Character color count */
        /* Bitmap size */
        _state_vdp2()->regs->chctlb |= 0x0200;
        _state_vdp2()->regs->chctlb |= cc_count;
        _state_vdp2()->regs->chctlb |= width << 10;
        _state_vdp2()->regs->chctlb |= height << 10;

        /* Supplementary palette number */
        switch (format->cc_count) {
        case VDP2_SCRN_CCC_PALETTE_16:
        case VDP2_SCRN_CCC_PALETTE_256:
                _state_vdp2()->regs->bmpnb |= palette_number;
                break;
        default:
                break;
        }

        /* Rotation parameter mode */
        _state_vdp2()->regs->rpmd |= format->rp_mode;

        switch (format->rp_mode) {
        case 0:
                /* Mode 0: Rotation Parameter A */

                /* Pattern boundry lead address */
                _state_vdp2()->regs->mpofr &= 0xFFF8;
                _state_vdp2()->regs->mpofr |= bank;
                break;
        case 1:
                /* Mode 1: Rotation Parameter B */

                /* Pattern boundry lead address */
                _state_vdp2()->regs->mpofr &= 0xFF8F;
                _state_vdp2()->regs->mpofr |= bank << 4;
                break;
        }

        /* Select VRAM bank usage for bitmap pattern data */
        uint32_t pixel_count;
        pixel_count = format->bitmap_size.width * format->bitmap_size.height;
        uint32_t bitmap_size;
        bitmap_size = pixel_count;

        switch (format->cc_count) {
        case VDP2_SCRN_CCC_PALETTE_16:
                bitmap_size >>= 1;
                break;
        case VDP2_SCRN_CCC_PALETTE_256:
                break;
        case VDP2_SCRN_CCC_PALETTE_2048:
        case VDP2_SCRN_CCC_RGB_32768:
                bitmap_size <<= 1;
                break;
        case VDP2_SCRN_CCC_RGB_16770000:
                bitmap_size <<= 2;
                break;
        }

        /* VRAM data bank select */
        _state_vdp2()->regs->ramctl |= format->usage_banks.a0;
        _state_vdp2()->regs->ramctl |= format->usage_banks.a1 << 2;
        _state_vdp2()->regs->ramctl |= format->usage_banks.b0 << 4;
        _state_vdp2()->regs->ramctl |= format->usage_banks.b1 << 6;

        /* Rotation parameter table */
        _state_vdp2()->regs->rptau = VDP2_VRAM_BANK(format->rotation_table_base);
        _state_vdp2()->regs->rptal = format->rotation_table_base & 0xFFFE;

        /* Special function type */
        _state_vdp2()->regs->bmpnb |= (format->sf_type & 0x03) << 4;

        /* Special function */
        _state_vdp2()->regs->sfsel |= (format->sf_code & 0x01) << 4;
        _state_vdp2()->regs->sfprmd |= (format->sf_mode & 0x03) << 8;
}

static void
_cell_plane_calc(
        const vdp2_scrn_cell_format_t *format,
        uint16_t *planes,
        uint16_t *map_offset)
{
        uint16_t plane_count;
        plane_count = ((format->scroll_screen == VDP2_SCRN_RBG0) ||
                       (format->scroll_screen == VDP2_SCRN_RBG1)) ? 16 : 4;

        uint16_t page_size;
        page_size = VDP2_SCRN_CALCULATE_PAGE_SIZE(format);

        uint32_t i;
        for (i = 0; i < plane_count; i++) {
                /* XXX: There should be a way to mask out the top mask bits */
                uint32_t offset;
                offset = format->map_bases.planes[i] - VDP2_VRAM_ADDR(0, 0);

                planes[i] = (offset / page_size) & 0x003F;
        }

        /* Calculate the upper 3-bits of the 9-bits "map register" */
        uint32_t offset;
        offset = format->map_bases.planes[0] - VDP2_VRAM_ADDR(0, 0);

        uint16_t map_bits;
        map_bits = offset / page_size;

        *map_offset = (map_bits & 0x01C0) >> 6;
}

static uint16_t
_cell_pattern_name_control_calc(const vdp2_scrn_cell_format_t *format)
{
        uint16_t pncnx;
        pncnx = 0x0000;

        uint16_t aux;
        aux = 0x0000;

        uint16_t sf_type;
        sf_type = 0x0000;

        uint16_t sc_number; /* Supplementary character number bits */
        sc_number = 0;

        uint16_t sp_number; /* Supplementary palette number bits */
        sp_number = 0;

        uint16_t character_number;
        character_number = VDP2_SCRN_PND_CP_NUM(format->cp_table);

        uint16_t palette_number;

        switch ((vdp2_cram_mode_get())) {
        case 0:
        default:
                palette_number = VDP2_SCRN_PND_MODE_0_PAL_NUM(format->color_palette);
                break;
        case 1:
                palette_number = VDP2_SCRN_PND_MODE_1_PAL_NUM(format->color_palette);
                break;
        case 2:
                palette_number = VDP2_SCRN_PND_MODE_2_PAL_NUM(format->color_palette);
                break;
        }

        switch (format->pnd_size) {
        case 1:
                /* Pattern name data size: 1-word */

                /* Depending on the Color RAM mode, there are "invalid"
                 * CRAM banks.
                 *
                 * Mode 0 (1024 colors, mirrored, 64 banks)
                 * Mode 1 (2048 colors, 128 banks)
                 * Mode 2 (1024 colors) */

                switch (format->cc_count) {
                case VDP2_SCRN_CCC_PALETTE_16:
                        sp_number = ((palette_number >> 4) & 0x07) << 5;
                        break;
                case VDP2_SCRN_CCC_PALETTE_256:
                case VDP2_SCRN_CCC_PALETTE_2048:
                case VDP2_SCRN_CCC_RGB_32768:
                case VDP2_SCRN_CCC_RGB_16770000:
                default:
                        sp_number = 0x0000;
                        break;
                }

                /* Character number supplement mode */
                switch (format->auxiliary_mode) {
                case 0:
                        /* Auxiliary mode 0; flip function can be used */
                        /* Supplementary character number */
                        switch (format->character_size) {
                        case (1 * 1):
                                /* Character number in pattern name table: bits 9~0
                                 * Character number in supplemental data: bits 14 13 12 11 10 */
                                sc_number = (character_number & 0x7C00) >> 10;
                                break;
                        case (2 * 2):
                                /* Character number in pattern name table: bits 11~2
                                 * Character number in supplemental data: bits 14 13 12  1  0 */
                                sc_number = ((character_number & 0x7000) >> 10) |
                                             (character_number & 0x03);
                                break;
                        }
                        break;
                case 1:
                        /* Auxiliary mode 1; flip function cannot be used */
                        /* Supplementary character number */
                        switch (format->character_size) {
                        case (1 * 1):
                                /* Character number in pattern name table: bits 11~0
                                 * Character number in supplemental data: bits 14 13 12 __ __ */
                                sc_number = (character_number & 0x7000) >> 10;
                                break;
                        case (2 * 2):
                                /* Character number in pattern name table: bits 13~2
                                 * Character number in supplemental data: bits 14 __ __  1  0 */
                                sc_number = ((character_number & 0x4000) >> 10) |
                                             (character_number & 0x03);
                                break;
                        }

                        aux = 0x4000;
                }

                /* Special function type */
                sf_type = (format->sf_type & 0x03) << 8;

                pncnx = 0x8000 | aux | sf_type | sc_number | sp_number;
                break;
        case 2:
                /* Pattern name data size: 2-words */
                pncnx = 0x0000;
                break;
        }

        return pncnx;
}

static void
_nbg0_scrn_cell_format_set(const vdp2_scrn_cell_format_t *format)
{
        _state_vdp2()->regs->chctla &= 0xFF8F;
        _state_vdp2()->regs->chctla &= 0xFFFE;
        _state_vdp2()->regs->plsz &= 0xFFFC;
        _state_vdp2()->regs->mpofn &= 0xFFF8;
        _state_vdp2()->regs->mpabn0 = 0x0000;
        _state_vdp2()->regs->mpcdn0 = 0x0000;
        _state_vdp2()->regs->pncn0 = 0x0000;
        _state_vdp2()->regs->sfsel &= 0xFFFE;
        _state_vdp2()->regs->sfprmd &= 0xFFFC;

        uint16_t pncn0;
        pncn0 = _cell_pattern_name_control_calc(format);

        uint16_t planes[4];
        uint16_t map_offset;

        _cell_plane_calc(format, planes, &map_offset);

        /* Character color count */
        _state_vdp2()->regs->chctla |= format->cc_count << 4;

        /* Character size */
        _state_vdp2()->regs->chctla |= format->character_size >> 2;

        /* Plane size */
        _state_vdp2()->regs->plsz |= format->plane_size - 1;

        /* Map */
        _state_vdp2()->regs->mpofn |= map_offset;
        _state_vdp2()->regs->mpabn0 = (planes[1] << 8) | planes[0];
        _state_vdp2()->regs->mpcdn0 = (planes[3] << 8) | planes[2];
        _state_vdp2()->regs->pncn0 = pncn0;

        /* Special function */
        _state_vdp2()->regs->sfsel |= (format->sf_code & 0x01) << 0;
        _state_vdp2()->regs->sfprmd |= (format->sf_mode & 0x03) << 0;
}

static void
_nbg1_scrn_cell_format_set(const vdp2_scrn_cell_format_t *format)
{
        _state_vdp2()->regs->chctla &= 0xCFFF;
        _state_vdp2()->regs->chctla &= 0xFEFF;
        _state_vdp2()->regs->plsz &= 0xFFF3;
        _state_vdp2()->regs->mpofn &= 0xFF8F;
        _state_vdp2()->regs->mpabn1 = 0x0000;
        _state_vdp2()->regs->mpcdn1 = 0x0000;
        _state_vdp2()->regs->pncn1 = 0x0000;
        _state_vdp2()->regs->sfsel &= 0xFFFD;
        _state_vdp2()->regs->sfprmd &= 0xFFF3;

        uint16_t pncn1;
        pncn1 = _cell_pattern_name_control_calc(format);

        uint16_t planes[4];
        uint16_t map_offset;

        _cell_plane_calc(format, planes, &map_offset);

        /* Character color count */
        _state_vdp2()->regs->chctla |= format->cc_count << 12;

        /* Character size */
        _state_vdp2()->regs->chctla |= (format->character_size >> 2) << 8;

        /* Plane size */
        _state_vdp2()->regs->plsz |= (format->plane_size - 1) << 2;

        /* Map */
        _state_vdp2()->regs->mpofn |= map_offset << 4;
        _state_vdp2()->regs->mpabn1 = (planes[1] << 8) | planes[0];
        _state_vdp2()->regs->mpcdn1 = (planes[3] << 8) | planes[2];
        _state_vdp2()->regs->pncn1 = pncn1;

        /* Special function */
        _state_vdp2()->regs->sfsel |= (format->sf_code & 0x01) << 1;
        _state_vdp2()->regs->sfprmd |= (format->sf_mode & 0x03) << 2;
}

static void
_nbg2_scrn_cell_format_set(const vdp2_scrn_cell_format_t *format)
{
        _state_vdp2()->regs->chctlb &= 0xFFFD;
        _state_vdp2()->regs->chctlb &= 0xFFFE;
        _state_vdp2()->regs->plsz &= 0xFFCF;
        _state_vdp2()->regs->mpofn &= 0xF8FF;
        _state_vdp2()->regs->mpabn2 = 0x0000;
        _state_vdp2()->regs->mpcdn2 = 0x0000;
        _state_vdp2()->regs->pncn2 = 0x0000;
        _state_vdp2()->regs->sfsel &= 0xFFFB;
        _state_vdp2()->regs->sfprmd &= 0xFFCF;

        uint16_t pncn2;
        pncn2 = _cell_pattern_name_control_calc(format);

        uint16_t planes[4];
        uint16_t map_offset;

        _cell_plane_calc(format, planes, &map_offset);

        /* Character color count */
        _state_vdp2()->regs->chctlb |= format->cc_count << 1;

        /* Character Size */
        _state_vdp2()->regs->chctlb |= format->character_size >> 2;

        /* Plane Size */
        _state_vdp2()->regs->plsz |= (format->plane_size - 1) << 4;

        /* Map */
        _state_vdp2()->regs->mpofn |= map_offset << 8;
        _state_vdp2()->regs->mpabn2 = (planes[1] << 8) | planes[0];
        _state_vdp2()->regs->mpcdn2 = (planes[3] << 8) | planes[2];
        _state_vdp2()->regs->pncn2 = pncn2;

        /* Special function */
        _state_vdp2()->regs->sfsel |= (format->sf_code & 0x01) << 2;
        _state_vdp2()->regs->sfprmd |= (format->sf_mode & 0x03) << 4;
}

static void
_nbg3_scrn_cell_format_set(const vdp2_scrn_cell_format_t *format)
{
        _state_vdp2()->regs->chctlb &= 0xFFDF;
        _state_vdp2()->regs->chctlb &= 0xFFEF;
        _state_vdp2()->regs->plsz &= 0xFF3F;
        _state_vdp2()->regs->mpofn &= 0x8FFF;
        _state_vdp2()->regs->mpabn3 = 0x0000;
        _state_vdp2()->regs->mpcdn3 = 0x0000;
        _state_vdp2()->regs->pncn3 = 0x0000;
        _state_vdp2()->regs->sfsel &= 0xFFF7;
        _state_vdp2()->regs->sfprmd &= 0xFF3F;

        uint16_t pncn3;
        pncn3 = _cell_pattern_name_control_calc(format);

        uint16_t planes[4];
        uint16_t map_offset;

        _cell_plane_calc(format, planes, &map_offset);

        /* Character color count */
        _state_vdp2()->regs->chctlb |= format->cc_count << 5;

        /* Character size */
        _state_vdp2()->regs->chctlb |= format->character_size << 2;

        /* Plane size */
        _state_vdp2()->regs->plsz |= (format->plane_size - 1) << 6;

        /* Map */
        _state_vdp2()->regs->mpofn |= map_offset << 12;
        _state_vdp2()->regs->mpabn3 = (planes[1] << 8) | planes[0];
        _state_vdp2()->regs->mpcdn3 = (planes[3] << 8) | planes[2];
        _state_vdp2()->regs->pncn3 = pncn3;

        /* Special function */
        _state_vdp2()->regs->sfsel |= (format->sf_code & 0x01) << 3;
        _state_vdp2()->regs->sfprmd |= (format->sf_mode & 0x03) << 6;
}

static void
_rbg0_scrn_cell_format_set(const vdp2_scrn_cell_format_t *format)
{
        _state_vdp2()->regs->chctlb &= 0x8FFF;
        _state_vdp2()->regs->chctlb &= 0xFEFF;
        _state_vdp2()->regs->rpmd &= 0xFFFE;
        _state_vdp2()->regs->plsz &= 0xFCFF;
        _state_vdp2()->regs->plsz &= 0xCFFF;
        _state_vdp2()->regs->mpofr &= 0xFFF8;
        _state_vdp2()->regs->mpofr &= 0xFF8F;
        _state_vdp2()->regs->mpabra = 0x0000;
        _state_vdp2()->regs->mpcdra = 0x0000;
        _state_vdp2()->regs->mpefra = 0x0000;
        _state_vdp2()->regs->mpghra = 0x0000;
        _state_vdp2()->regs->mpijra = 0x0000;
        _state_vdp2()->regs->mpklra = 0x0000;
        _state_vdp2()->regs->mpmnra = 0x0000;
        _state_vdp2()->regs->mpopra = 0x0000;
        _state_vdp2()->regs->mpabrb = 0x0000;
        _state_vdp2()->regs->mpcdrb = 0x0000;
        _state_vdp2()->regs->mpefrb = 0x0000;
        _state_vdp2()->regs->mpghrb = 0x0000;
        _state_vdp2()->regs->mpijrb = 0x0000;
        _state_vdp2()->regs->mpklrb = 0x0000;
        _state_vdp2()->regs->mpmnrb = 0x0000;
        _state_vdp2()->regs->mpoprb = 0x0000;
        _state_vdp2()->regs->pncr = 0x0000;
        _state_vdp2()->regs->sfsel &= 0xFFEF;
        _state_vdp2()->regs->sfprmd &= 0xFCFF;

        uint16_t pncr;
        pncr = _cell_pattern_name_control_calc(format);

        uint16_t planes[16];
        uint16_t map_offset;

        _cell_plane_calc(format, planes, &map_offset);

        /* Character color count */
        _state_vdp2()->regs->chctlb |= format->cc_count << 12;

        /* Character size */
        _state_vdp2()->regs->chctlb |= (format->character_size >> 2) << 8;

        /* Rotation parameter mode */
        _state_vdp2()->regs->rpmd |= format->rp_mode;

        switch (format->rp_mode) {
        case 0:
                /* Mode 0: Rotation Parameter A */

                /* Plane size */
                _state_vdp2()->regs->plsz |= (format->plane_size - 1) << 8;

                /* Screen over process */

                /* Map */
                _state_vdp2()->regs->mpofr |= map_offset;

                _state_vdp2()->regs->mpabra = (planes[1] << 8) | planes[0];
                _state_vdp2()->regs->mpcdra = (planes[3] << 8) | planes[2];
                _state_vdp2()->regs->mpefra = (planes[5] << 8) | planes[4];
                _state_vdp2()->regs->mpghra = (planes[7] << 8) | planes[6];
                _state_vdp2()->regs->mpijra = (planes[9] << 8) | planes[8];
                _state_vdp2()->regs->mpklra = (planes[11] << 8) | planes[10];
                _state_vdp2()->regs->mpmnra = (planes[13] << 8) | planes[12];
                _state_vdp2()->regs->mpopra = (planes[15] << 8) | planes[14];
                break;
        case 1:
                /* Mode 1: Rotation Parameter B */

                /* Plane size */
                _state_vdp2()->regs->plsz |= (format->plane_size - 1) << 12;

                /* Screen over process */

                /* Map */
                _state_vdp2()->regs->mpofr |= map_offset << 4;

                _state_vdp2()->regs->mpabrb = (planes[1] << 8) | planes[0];
                _state_vdp2()->regs->mpcdrb = (planes[3] << 8) | planes[2];
                _state_vdp2()->regs->mpefrb = (planes[5] << 8) | planes[4];
                _state_vdp2()->regs->mpghrb = (planes[7] << 8) | planes[6];
                _state_vdp2()->regs->mpijrb = (planes[9] << 8) | planes[8];
                _state_vdp2()->regs->mpklrb = (planes[11] << 8) | planes[10];
                _state_vdp2()->regs->mpmnrb = (planes[13] << 8) | planes[12];
                _state_vdp2()->regs->mpoprb = (planes[15] << 8) | planes[14];
                break;
        case 2:
                break;
        case 3:
                /* Set both Rotation parameter A and Rotation parameter B */
                break;
        }

        _state_vdp2()->regs->pncr = pncr;

        /* VRAM data bank select */
        _state_vdp2()->regs->ramctl |= format->usage_banks.a0;
        _state_vdp2()->regs->ramctl |= format->usage_banks.a1 << 2;
        _state_vdp2()->regs->ramctl |= format->usage_banks.b0 << 4;
        _state_vdp2()->regs->ramctl |= format->usage_banks.b1 << 6;

        /* Rotation parameter table */
        _state_vdp2()->regs->rptau = VDP2_VRAM_BANK(format->rotation_table);
        _state_vdp2()->regs->rptal = format->rotation_table & 0xFFFE;

        /* Special function */
        _state_vdp2()->regs->sfsel |= (format->sf_code & 0x01) << 4;
        _state_vdp2()->regs->sfprmd |= (format->sf_mode & 0x03) << 8;
}
