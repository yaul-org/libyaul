/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <string.h>

#include <vdp2/scrn.h>
#include <vdp2/vram.h>

#include "vdp-internal.h"

#ifdef DEBUG
static void _debug_check_cell(const struct scrn_cell_format *);
#endif /* DEBUG */

static void _cell_plane_calc(const struct scrn_cell_format *, uint16_t *,
    uint16_t *);
static uint16_t _cell_pattern_name_control_calc(
        const struct scrn_cell_format *);

static void _nbg0_scrn_cell_format_set(const struct scrn_cell_format *);
static void _nbg1_scrn_cell_format_set(const struct scrn_cell_format *);
static void _nbg2_scrn_cell_format_set(const struct scrn_cell_format *);
static void _nbg3_scrn_cell_format_set(const struct scrn_cell_format *);
static void _rbg0_scrn_cell_format_set(const struct scrn_cell_format *);

#ifdef DEBUG
static void _debug_check_bitmap(const struct scrn_bitmap_format *);
#endif /* DEBUG */

static void _nbg0_scrn_bitmap_format_set(const struct scrn_bitmap_format *);
static void _nbg1_scrn_bitmap_format_set(const struct scrn_bitmap_format *);
static void _rbg0_scrn_bitmap_format_set(const struct scrn_bitmap_format *);

void
vdp2_scrn_cell_format_set(const struct scrn_cell_format *format)
{
#ifdef DEBUG
        _debug_check_cell(format);
#endif /* DEBUG */

        switch (format->scf_scroll_screen) {
        case SCRN_NBG0:
                _nbg0_scrn_cell_format_set(format);
                break;
        case SCRN_NBG1:
                _nbg1_scrn_cell_format_set(format);
                break;
        case SCRN_NBG2:
                _nbg2_scrn_cell_format_set(format);
                break;
        case SCRN_NBG3:
                _nbg3_scrn_cell_format_set(format);
                break;
        case SCRN_RBG0:
                _rbg0_scrn_cell_format_set(format);
               break;
        }
}

void
vdp2_scrn_bitmap_format_set(struct scrn_bitmap_format *format)
{
#ifdef DEBUG
        _debug_check_bitmap(format);
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
_debug_check_bitmap(const struct scrn_bitmap_format *format)
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
        _state_vdp2()->regs.chctla &= 0xFF80;      /* Bits 0,1,2,3,4,5,6 */
        _state_vdp2()->regs.mpofn &= 0xFFF8;       /* Bits 0,1,2 */
        _state_vdp2()->regs.bmpna &= 0xFFC8;       /* Bits 0,1,2,4,5 */
        _state_vdp2()->regs.sfsel &= 0xFFFE;
        _state_vdp2()->regs.sfprmd &= 0xFFFC;

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
        _state_vdp2()->regs.chctla |= 0x0002;
        _state_vdp2()->regs.chctla |= cc_count;
        _state_vdp2()->regs.chctla |= width << 3;
        _state_vdp2()->regs.chctla |= height << 2;

        /* Pattern boundry lead address */
        _state_vdp2()->regs.mpofn |= bank;

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
                _state_vdp2()->regs.bmpna |= palette_number;
                break;
        }

        /* Special function type */
        _state_vdp2()->regs.bmpna |= (format->sbf_sf_type & 0x03) << 4;

        /* Special function */
        _state_vdp2()->regs.sfsel |= (format->sbf_sf_code & 0x01) << 0;
        _state_vdp2()->regs.sfprmd |= (format->sbf_sf_mode & 0x03) << 0;
}

static void
_nbg1_scrn_bitmap_format_set(const struct scrn_bitmap_format *format)
{
        _state_vdp2()->regs.chctla &= 0xC0FF;      /* Bits 8,9,10,11,12,13 */
        _state_vdp2()->regs.mpofn &= 0xFF8F;       /* Bits 4,5,6 */
        _state_vdp2()->regs.bmpna &= 0xC8FF;       /* Bits 8,9,10,12,13 */
        _state_vdp2()->regs.sfsel &= 0xFFFD;
        _state_vdp2()->regs.sfprmd &= 0xFFF3;

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
        _state_vdp2()->regs.chctla |= 0x0200;
        _state_vdp2()->regs.chctla |= cc_count;
        _state_vdp2()->regs.chctla |= width << 10;
        _state_vdp2()->regs.chctla |= height << 10;

        /* Pattern boundry lead address */
        _state_vdp2()->regs.mpofn |= bank << 4;

        /* Supplementary palette number */
        switch (format->sbf_cc_count) {
        case SCRN_CCC_PALETTE_16:
        case SCRN_CCC_PALETTE_256:
                _state_vdp2()->regs.bmpna |= palette_number << 8;
                break;
        }

        /* Special function type */
        _state_vdp2()->regs.bmpna |= (format->sbf_sf_type & 0x03) << 12;

        /* Special function */
        _state_vdp2()->regs.sfsel |= (format->sbf_sf_code & 0x01) << 1;
        _state_vdp2()->regs.sfprmd |= (format->sbf_sf_mode & 0x03) << 2;
}

static void
_rbg0_scrn_bitmap_format_set(const struct scrn_bitmap_format *format)
{
        const uint8_t rot_bank_select[2][2] = {
                {
                        /* VRAM bank is not partitioned */
                        0x03,
                        0x03,
                }, {
                        /* VRAM bank is partitioned */
                        0x03,
                        0x0C
                }
        };

#ifdef DEBUG
        assert((format->sbf_bitmap_size.width > 0) &&
               ((format->sbf_bitmap_size.width & 0x1FF) == 0x0000));

        assert((format->sbf_bitmap_size.height > 0) &&
               ((format->sbf_bitmap_size.height & 0x00FF) == 0x0000));

        assert((format->sbf_rp_mode == 0) ||
               (format->sbf_rp_mode == 1) ||
               (format->sbf_rp_mode == 2) ||
               (format->sbf_rp_mode == 3));
#endif /* DEBUG */

        _state_vdp2()->regs.chctlb &= 0xC0FF; /* Bits 8, 9, 10, 12, 13, 14 */
        _state_vdp2()->regs.bmpnb &= 0xFFC8;  /* Bits 0, 1, 2, 4, 5 */
        _state_vdp2()->regs.rpmd &= 0xFFFC;   /* Bits 0, 1 */
        _state_vdp2()->regs.sfsel &= 0xFFEF;
        _state_vdp2()->regs.sfprmd &= 0xFCFF;
        _state_vdp2()->regs.ramctl &= 0xB300;

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
        _state_vdp2()->regs.chctlb |= 0x0200;
        _state_vdp2()->regs.chctlb |= cc_count;
        _state_vdp2()->regs.chctlb |= width << 10;
        _state_vdp2()->regs.chctlb |= height << 10;

        /* Supplementary palette number */
        switch (format->sbf_cc_count) {
        case SCRN_CCC_PALETTE_16:
        case SCRN_CCC_PALETTE_256:
                _state_vdp2()->regs.bmpnb |= palette_number;
                break;
        }

        /* Rotation parameter mode */
        _state_vdp2()->regs.rpmd |= format->sbf_rp_mode;

        switch (format->sbf_rp_mode) {
        case 0:
                /* Mode 0: Rotation Parameter A */

                /* Pattern boundry lead address */
                _state_vdp2()->regs.mpofr &= 0xFFF8;
                _state_vdp2()->regs.mpofr |= bank;
                break;
        case 1:
                /* Mode 1: Rotation Parameter B */

                /* Pattern boundry lead address */
                _state_vdp2()->regs.mpofr &= 0xFF8F;
                _state_vdp2()->regs.mpofr |= bank << 4;
                break;
        }

        /* Rotation data bank selection */
        /* Calculate the number of banks used */
        uint32_t pixel_count;
        pixel_count = format->sbf_bitmap_size.width * format->sbf_bitmap_size.height;
        uint32_t bitmap_size;
        bitmap_size = pixel_count;

        switch (format->sbf_cc_count) {
        case SCRN_CCC_PALETTE_16:
                bitmap_size >>= 1;
                break;
        case SCRN_CCC_PALETTE_256:
                break;
        case SCRN_CCC_PALETTE_2048:
        case SCRN_CCC_RGB_32768:
                bitmap_size <<= 1;
                break;
        case SCRN_CCC_RGB_16770000:
                bitmap_size <<= 2;
                break;
        }

        uint16_t ramctl_bits;
        ramctl_bits = _state_vdp2()->regs.ramctl;

        uint16_t bank_count;
        bank_count = (bitmap_size >> 18) + 1;

        uint8_t parted_bank_a;
        parted_bank_a = (ramctl_bits >> 8) & 0x01;

        uint8_t parted_bank_b;
        parted_bank_b = (ramctl_bits >> 9) & 0x01;

        for (uint16_t bank_offset = 0; bank_offset < bank_count; bank_offset++) {
                uint8_t which_bank;
                which_bank = bank + bank_offset;

                uint8_t which_subbank;
                which_subbank = which_bank & 0x01;

                switch (which_bank) {
                case 0:
                case 1:
                        ramctl_bits |= (uint16_t)rot_bank_select[parted_bank_a][which_subbank];
                        break;
                case 2:
                case 3:
                        ramctl_bits |= (uint16_t)rot_bank_select[parted_bank_b][which_subbank] << 4;
                        break;
                }
        }

        _state_vdp2()->regs.ramctl = ramctl_bits;

        /* Rotation parameter table */
        _state_vdp2()->regs.rptau = VRAM_BANK_4MBIT(format->sbf_rotation_table);
        _state_vdp2()->regs.rptal = format->sbf_rotation_table & 0xFFFE;

        /* Special function type */
        _state_vdp2()->regs.bmpnb |= (format->sbf_sf_type & 0x03) << 4;

        /* Special function */
        _state_vdp2()->regs.sfsel |= (format->sbf_sf_code & 0x01) << 4;
        _state_vdp2()->regs.sfprmd |= (format->sbf_sf_mode & 0x03) << 8;
}

#ifdef DEBUG
static void
_debug_check_cell(const struct scrn_cell_format *format)
{
        assert(format != NULL);

        /* Check if the background passed is valid */
        assert((format->scf_scroll_screen == SCRN_NBG0) ||
               (format->scf_scroll_screen == SCRN_RBG1) ||
               (format->scf_scroll_screen == SCRN_NBG1) ||
               (format->scf_scroll_screen == SCRN_NBG2) ||
               (format->scf_scroll_screen == SCRN_NBG3) ||
               (format->scf_scroll_screen == SCRN_RBG0));

        /* Assert that the lead address to character pattern table in
         * VRAM is on a 20-byte boundary */
        assert((format->scf_cp_table & 0x1F) == 0x00);

        /* Assert that the lead address to the color palette in CRAM is
         * on a 20-byte boundary */
        assert((format->scf_color_palette & 0x1F) == 0x00);

        assert((format->scf_cc_count == SCRN_CCC_PALETTE_16) ||
               (format->scf_cc_count == SCRN_CCC_PALETTE_256) ||
               (format->scf_cc_count == SCRN_CCC_PALETTE_2048));

        /* Check the character number supplement mode */
        assert((format->scf_auxiliary_mode == 0) ||
               (format->scf_auxiliary_mode == 1));

        /* Check the character size */
        assert((format->scf_character_size == (1 * 1)) ||
               (format->scf_character_size == (2 * 2)));

        /* Check the plane size */
        assert((format->scf_plane_size == (1 * 1)) ||
               (format->scf_plane_size == (2 * 1)) ||
               (format->scf_plane_size == (2 * 2)));

        /* Check the pattern name data size */
        assert((format->scf_pnd_size == 1) ||
               (format->scf_pnd_size == 2));

        /* Check the lead address for each plane */
        /* The lead address must be on a boundary dependent on the size
         * of the plane. For example, if NBG0 is configured to use 2x2
         * cells, the dimensions of its page(s) will be 32x32 cells
         * (0x800 bytes). */
        assert((format->scf_map.plane_a & (SCRN_CALCULATE_PLANE_SIZE(format) - 1)) == 0x0000);
        assert((format->scf_map.plane_b & (SCRN_CALCULATE_PLANE_SIZE(format) - 1)) == 0x0000);
        assert((format->scf_map.plane_c & (SCRN_CALCULATE_PLANE_SIZE(format) - 1)) == 0x0000);
        assert((format->scf_map.plane_d & (SCRN_CALCULATE_PLANE_SIZE(format) - 1)) == 0x0000);
}
#endif /* DEBUG */

static void
_cell_plane_calc(
        const struct scrn_cell_format *format,
        uint16_t *planes,
        uint16_t *map_offset)
{
        uint16_t plane_count;
        plane_count = ((format->scf_scroll_screen == SCRN_RBG0) ||
                       (format->scf_scroll_screen == SCRN_RBG1)) ? 16 : 4;

        uint16_t page_size;
        page_size = SCRN_CALCULATE_PAGE_SIZE(format);

        uint32_t i;
        for (i = 0; i < plane_count; i++) {
                /* XXX: There should be a way to mask out the top mask bits */
                uint32_t offset;
                offset = format->scf_map.planes[i] - VRAM_ADDR_4MBIT(0, 0);

                planes[i] = (offset / page_size) & 0x003F;
        }

        /* Calculate the upper 3-bits of the 9-bits "map register" */
        uint32_t offset;
        offset = format->scf_map.planes[0] - VRAM_ADDR_4MBIT(0, 0);

        uint16_t map_bits;
        map_bits = offset / page_size;

        *map_offset = (map_bits & 0x01C0) >> 6;
}

static uint16_t
_cell_pattern_name_control_calc(const struct scrn_cell_format *format)
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
        character_number = CHARACTER_NUMBER(format->scf_cp_table);

        uint16_t palette_number;
        palette_number = PALETTE_NUMBER(format->scf_color_palette);

        switch (format->scf_pnd_size) {
        case 1:
                /* Pattern name data size: 1-word */
#ifdef DEBUG
                /* Depending on the Color RAM mode, there are "invalid"
                 * CRAM banks.
                 *
                 * Mode 0 (1024 colors, mirrored, 64 banks)
                 * Mode 1 (2048 colors, 128 banks)
                 * Mode 2 (1024 colors) */
#endif /* DEBUG */

                switch (format->scf_cc_count) {
                case SCRN_CCC_PALETTE_16:
                        sp_number = ((palette_number & 0x0070) >> 4) << 5;
                        break;
                case SCRN_CCC_PALETTE_256:
                case SCRN_CCC_PALETTE_2048:
                default:
                        sp_number = 0x0000;
                        break;
                }

                /* Character number supplement mode */
                switch (format->scf_auxiliary_mode) {
                case 0:
                        /* Auxiliary mode 0; flip function can be used */
                        /* Supplementary character number */
                        switch (format->scf_character_size) {
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
                        switch (format->scf_character_size) {
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
                sf_type = (format->scf_sf_type & 0x03) << 8;

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
_nbg0_scrn_cell_format_set(const struct scrn_cell_format *format)
{
        _state_vdp2()->regs.chctla &= 0xFF8F;
        _state_vdp2()->regs.chctla &= 0xFFFE;
        _state_vdp2()->regs.plsz &= 0xFFFC;
        _state_vdp2()->regs.mpofn &= 0xFFF8;
        _state_vdp2()->regs.mpabn0 = 0x0000;
        _state_vdp2()->regs.mpcdn0 = 0x0000;
        _state_vdp2()->regs.pncn0 = 0x0000;
        _state_vdp2()->regs.sfsel &= 0xFFFE;
        _state_vdp2()->regs.sfprmd &= 0xFFFC;

        uint16_t pncn0;
        pncn0 = _cell_pattern_name_control_calc(format);

        uint16_t planes[4];
        uint16_t map_offset;

        _cell_plane_calc(format, planes, &map_offset);

        /* Character color count */
        _state_vdp2()->regs.chctla |= format->scf_cc_count << 4;

        /* Character size */
        _state_vdp2()->regs.chctla |= format->scf_character_size >> 2;

        /* Plane size */
        _state_vdp2()->regs.plsz |= format->scf_plane_size - 1;

        /* Map */
        _state_vdp2()->regs.mpofn |= map_offset;
        _state_vdp2()->regs.mpabn0 = (planes[1] << 8) | planes[0];
        _state_vdp2()->regs.mpcdn0 = (planes[3] << 8) | planes[2];
        _state_vdp2()->regs.pncn0 = pncn0;

        /* Special function */
        _state_vdp2()->regs.sfsel |= (format->scf_sf_code & 0x01) << 0;
        _state_vdp2()->regs.sfprmd |= (format->scf_sf_mode & 0x03) << 0;
}

static void
_nbg1_scrn_cell_format_set(const struct scrn_cell_format *format)
{
        _state_vdp2()->regs.chctla &= 0xCFFF;
        _state_vdp2()->regs.chctla &= 0xFEFF;
        _state_vdp2()->regs.plsz &= 0xFFF3;
        _state_vdp2()->regs.mpofn &= 0xFF8F;
        _state_vdp2()->regs.mpabn1 = 0x0000;
        _state_vdp2()->regs.mpcdn1 = 0x0000;
        _state_vdp2()->regs.pncn1 = 0x0000;
        _state_vdp2()->regs.sfsel &= 0xFFFD;
        _state_vdp2()->regs.sfprmd &= 0xFFF3;

        uint16_t pncn1;
        pncn1 = _cell_pattern_name_control_calc(format);

        uint16_t planes[4];
        uint16_t map_offset;

        _cell_plane_calc(format, planes, &map_offset);

        /* Character color count */
        _state_vdp2()->regs.chctla |= format->scf_cc_count << 12;

        /* Character size */
        _state_vdp2()->regs.chctla |= (format->scf_character_size >> 2) << 8;

        /* Plane size */
        _state_vdp2()->regs.plsz |= (format->scf_plane_size - 1) << 2;

        /* Map */
        _state_vdp2()->regs.mpofn |= map_offset << 4;
        _state_vdp2()->regs.mpabn1 = (planes[1] << 8) | planes[0];
        _state_vdp2()->regs.mpcdn1 = (planes[3] << 8) | planes[2];
        _state_vdp2()->regs.pncn1 = pncn1;

        /* Special function */
        _state_vdp2()->regs.sfsel |= (format->scf_sf_code & 0x01) << 1;
        _state_vdp2()->regs.sfprmd |= (format->scf_sf_mode & 0x03) << 2;
}

static void
_nbg2_scrn_cell_format_set(const struct scrn_cell_format *format)
{
#ifdef DEBUG
        assert((format->scf_cc_count == SCRN_CCC_PALETTE_16) ||
               (format->scf_cc_count == SCRN_CCC_PALETTE_256));
#endif /* DEBUG */

        _state_vdp2()->regs.chctlb &= 0xFFFD;
        _state_vdp2()->regs.chctlb &= 0xFFFE;
        _state_vdp2()->regs.plsz &= 0xFFCF;
        _state_vdp2()->regs.mpofn &= 0xF8FF;
        _state_vdp2()->regs.mpabn2 = 0x0000;
        _state_vdp2()->regs.mpcdn2 = 0x0000;
        _state_vdp2()->regs.pncn2 = 0x0000;
        _state_vdp2()->regs.sfsel &= 0xFFFB;
        _state_vdp2()->regs.sfprmd &= 0xFFCF;

        uint16_t pncn2;
        pncn2 = _cell_pattern_name_control_calc(format);

        uint16_t planes[4];
        uint16_t map_offset;

        _cell_plane_calc(format, planes, &map_offset);

        /* Character color count */
        _state_vdp2()->regs.chctlb |= format->scf_cc_count << 1;

        /* Character Size */
        _state_vdp2()->regs.chctlb |= format->scf_character_size >> 2;

        /* Plane Size */
        _state_vdp2()->regs.plsz |= (format->scf_plane_size - 1) << 4;

        /* Map */
        _state_vdp2()->regs.mpofn |= map_offset << 8;
        _state_vdp2()->regs.mpabn2 = (planes[1] << 8) | planes[0];
        _state_vdp2()->regs.mpcdn2 = (planes[3] << 8) | planes[2];
        _state_vdp2()->regs.pncn2 = pncn2;

        /* Special function */
        _state_vdp2()->regs.sfsel |= (format->scf_sf_code & 0x01) << 2;
        _state_vdp2()->regs.sfprmd |= (format->scf_sf_mode & 0x03) << 4;
}

static void
_nbg3_scrn_cell_format_set(const struct scrn_cell_format *format)
{
#ifdef DEBUG
        assert((format->scf_cc_count == SCRN_CCC_PALETTE_16) ||
               (format->scf_cc_count == SCRN_CCC_PALETTE_256));
#endif /* DEBUG */

        _state_vdp2()->regs.chctlb &= 0xFFDF;
        _state_vdp2()->regs.chctlb &= 0xFFEF;
        _state_vdp2()->regs.plsz &= 0xFF3F;
        _state_vdp2()->regs.mpofn &= 0x8FFF;
        _state_vdp2()->regs.mpabn3 = 0x0000;
        _state_vdp2()->regs.mpcdn3 = 0x0000;
        _state_vdp2()->regs.pncn3 = 0x0000;
        _state_vdp2()->regs.sfsel &= 0xFFF7;
        _state_vdp2()->regs.sfprmd &= 0xFF3F;

        uint16_t pncn3;
        pncn3 = _cell_pattern_name_control_calc(format);

        uint16_t planes[4];
        uint16_t map_offset;

        _cell_plane_calc(format, planes, &map_offset);

        /* Character color count */
        _state_vdp2()->regs.chctlb |= format->scf_cc_count << 5;

        /* Character size */
        _state_vdp2()->regs.chctlb |= format->scf_character_size << 2;

        /* Plane size */
        _state_vdp2()->regs.plsz |= (format->scf_plane_size - 1) << 6;

        /* Map */
        _state_vdp2()->regs.mpofn |= map_offset << 12;
        _state_vdp2()->regs.mpabn3 = (planes[1] << 8) | planes[0];
        _state_vdp2()->regs.mpcdn3 = (planes[3] << 8) | planes[2];
        _state_vdp2()->regs.pncn3 = pncn3;

        /* Special function */
        _state_vdp2()->regs.sfsel |= (format->scf_sf_code & 0x01) << 3;
        _state_vdp2()->regs.sfprmd |= (format->scf_sf_mode & 0x03) << 6;
}

static void
_rbg0_scrn_cell_format_set(const struct scrn_cell_format *format)
{
#ifdef DEBUG
        assert((format->scf_rp_mode == 0) ||
               (format->scf_rp_mode == 1) ||
               (format->scf_rp_mode == 2) ||
               (format->scf_rp_mode == 3));
#endif /* DEBUG */

        _state_vdp2()->regs.chctlb &= 0x8FFF;
        _state_vdp2()->regs.chctlb &= 0xFEFF;
        _state_vdp2()->regs.rpmd &= 0xFFFE;
        _state_vdp2()->regs.plsz &= 0xFCFF;
        _state_vdp2()->regs.plsz &= 0xCFFF;
        _state_vdp2()->regs.mpofr &= 0xFFF8;
        _state_vdp2()->regs.mpofr &= 0xFF8F;
        _state_vdp2()->regs.mpabra = 0x0000;
        _state_vdp2()->regs.mpcdra = 0x0000;
        _state_vdp2()->regs.mpefra = 0x0000;
        _state_vdp2()->regs.mpghra = 0x0000;
        _state_vdp2()->regs.mpijra = 0x0000;
        _state_vdp2()->regs.mpklra = 0x0000;
        _state_vdp2()->regs.mpmnra = 0x0000;
        _state_vdp2()->regs.mpopra = 0x0000;
        _state_vdp2()->regs.mpabrb = 0x0000;
        _state_vdp2()->regs.mpcdrb = 0x0000;
        _state_vdp2()->regs.mpefrb = 0x0000;
        _state_vdp2()->regs.mpghrb = 0x0000;
        _state_vdp2()->regs.mpijrb = 0x0000;
        _state_vdp2()->regs.mpklrb = 0x0000;
        _state_vdp2()->regs.mpmnrb = 0x0000;
        _state_vdp2()->regs.mpoprb = 0x0000;
        _state_vdp2()->regs.pncr = 0x0000;
        _state_vdp2()->regs.sfsel &= 0xFFEF;
        _state_vdp2()->regs.sfprmd &= 0xFCFF;

        uint16_t pncr;
        pncr = _cell_pattern_name_control_calc(format);

        uint16_t planes[16];
        uint16_t map_offset;

        _cell_plane_calc(format, planes, &map_offset);

        /* Character color count */
        _state_vdp2()->regs.chctlb |= format->scf_cc_count << 12;

        /* Character size */
        _state_vdp2()->regs.chctlb |= (format->scf_character_size >> 2) << 8;

        /* Rotation parameter mode */
        _state_vdp2()->regs.rpmd |= format->scf_rp_mode;

        switch (format->scf_rp_mode) {
        case 0:
                /* Mode 0: Rotation Parameter A */

                /* Plane size */
                _state_vdp2()->regs.plsz |= (format->scf_plane_size - 1) << 8;

                /* Screen over process */

                /* Map */
                _state_vdp2()->regs.mpofr |= map_offset;

                _state_vdp2()->regs.mpabra = (planes[1] << 8) | planes[0];
                _state_vdp2()->regs.mpcdra = (planes[3] << 8) | planes[2];
                _state_vdp2()->regs.mpefra = (planes[5] << 8) | planes[4];
                _state_vdp2()->regs.mpghra = (planes[7] << 8) | planes[6];
                _state_vdp2()->regs.mpijra = (planes[9] << 8) | planes[8];
                _state_vdp2()->regs.mpklra = (planes[11] << 8) | planes[10];
                _state_vdp2()->regs.mpmnra = (planes[13] << 8) | planes[12];
                _state_vdp2()->regs.mpopra = (planes[15] << 8) | planes[14];
                break;
        case 1:
                /* Mode 1: Rotation Parameter B */

                /* Plane size */
                _state_vdp2()->regs.plsz |= (format->scf_plane_size - 1) << 12;

                /* Screen over process */

                /* Map */
                _state_vdp2()->regs.mpofr |= map_offset << 4;

                _state_vdp2()->regs.mpabrb = (planes[1] << 8) | planes[0];
                _state_vdp2()->regs.mpcdrb = (planes[3] << 8) | planes[2];
                _state_vdp2()->regs.mpefrb = (planes[5] << 8) | planes[4];
                _state_vdp2()->regs.mpghrb = (planes[7] << 8) | planes[6];
                _state_vdp2()->regs.mpijrb = (planes[9] << 8) | planes[8];
                _state_vdp2()->regs.mpklrb = (planes[11] << 8) | planes[10];
                _state_vdp2()->regs.mpmnrb = (planes[13] << 8) | planes[12];
                _state_vdp2()->regs.mpoprb = (planes[15] << 8) | planes[14];
                break;
        case 2:
                break;
        case 3:
                /* Set both Rotation parameter A and Rotation parameter B */
                break;
        }

        _state_vdp2()->regs.pncr = pncr;

        /* Special function */
        _state_vdp2()->regs.sfsel |= (format->scf_sf_code & 0x01) << 4;
        _state_vdp2()->regs.sfprmd |= (format->scf_sf_mode & 0x03) << 8;
}
