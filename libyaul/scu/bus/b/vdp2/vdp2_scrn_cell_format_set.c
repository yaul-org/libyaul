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

#include "vdp2-internal.h"

#ifdef DEBUG
static void _debug_check(const struct scrn_cell_format *);
#endif /* DEBUG */

static void _cell_plane_calc(const struct scrn_cell_format *, uint16_t *, uint16_t *);
static uint16_t _cell_pattern_name_control_calc(const struct scrn_cell_format *);

static void _nbg0_scrn_cell_format_set(const struct scrn_cell_format *);
static void _nbg1_scrn_cell_format_set(const struct scrn_cell_format *);
static void _nbg2_scrn_cell_format_set(const struct scrn_cell_format *);
static void _nbg3_scrn_cell_format_set(const struct scrn_cell_format *);
static void _rbg0_scrn_cell_format_set(const struct scrn_cell_format *);

void
vdp2_scrn_cell_format_set(const struct scrn_cell_format *format)
{
#ifdef DEBUG
        _debug_check(format);
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

#ifdef DEBUG
static void
_debug_check(const struct scrn_cell_format *format)
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
                                 * Character number in supplemental data: 14 13 12 11 10 */
                                sc_number = (character_number & 0x7C00) >> 10;
                                break;
                        case (2 * 2):
                                /* Character number in pattern name table: bits 11~2
                                 * Character number in supplemental data: 14 13 12  1  0 */
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
                                 * Character number in supplemental data: 14 13 12 __ __ */
                                sc_number = (character_number & 0x7000) >> 10;
                                break;
                        case (2 * 2):
                                /* Character number in pattern name table: bits 13~2
                                 * Character number in supplemental data: 14 __ __  1  0 */
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
        vdp2_state.buffered_regs.chctla &= 0xFF8F;
        vdp2_state.buffered_regs.chctla &= 0xFFFE;
        vdp2_state.buffered_regs.plsz &= 0xFFFC;
        vdp2_state.buffered_regs.mpofn &= 0xFFF8;
        vdp2_state.buffered_regs.mpabn0 = 0x0000;
        vdp2_state.buffered_regs.mpcdn0 = 0x0000;
        vdp2_state.buffered_regs.pncn0 = 0x0000;
        vdp2_state.buffered_regs.sfsel &= 0xFFFE;
        vdp2_state.buffered_regs.sfprmd &= 0xFFFC;

        /* Copy */
        (void)memcpy(&vdp2_state.nbg0.cell_format, format, sizeof(*format));

        uint16_t pncn0;
        pncn0 = _cell_pattern_name_control_calc(format);

        uint16_t planes[4];
        uint16_t map_offset;

        _cell_plane_calc(format, planes, &map_offset);

        /* Character color count */
        vdp2_state.buffered_regs.chctla |= format->scf_cc_count << 4;

        /* Character size */
        vdp2_state.buffered_regs.chctla |= format->scf_character_size >> 2;

        /* Plane size */
        vdp2_state.buffered_regs.plsz |= format->scf_plane_size - 1;

        /* Map */
        vdp2_state.buffered_regs.mpofn |= map_offset;
        vdp2_state.buffered_regs.mpabn0 = (planes[1] << 8) | planes[0];
        vdp2_state.buffered_regs.mpcdn0 = (planes[3] << 8) | planes[2];
        vdp2_state.buffered_regs.pncn0 = pncn0;

        /* Special function */
        vdp2_state.buffered_regs.sfsel |= (format->scf_sf_code & 0x01) << 0;
        vdp2_state.buffered_regs.sfprmd |= (format->scf_sf_mode & 0x03) << 0;
}

static void
_nbg1_scrn_cell_format_set(const struct scrn_cell_format *format)
{
        vdp2_state.buffered_regs.chctla &= 0xCFFF;
        vdp2_state.buffered_regs.chctla &= 0xFEFF;
        vdp2_state.buffered_regs.plsz &= 0xFFF3;
        vdp2_state.buffered_regs.mpofn &= 0xFF8F;
        vdp2_state.buffered_regs.mpabn1 = 0x0000;
        vdp2_state.buffered_regs.mpcdn1 = 0x0000;
        vdp2_state.buffered_regs.pncn1 = 0x0000;
        vdp2_state.buffered_regs.sfsel &= 0xFFFD;
        vdp2_state.buffered_regs.sfprmd &= 0xFFF3;

        /* Copy */
        (void)memcpy(&vdp2_state.nbg1.cell_format, format, sizeof(*format));

        uint16_t pncn1;
        pncn1 = _cell_pattern_name_control_calc(format);

        uint16_t planes[4];
        uint16_t map_offset;

        _cell_plane_calc(format, planes, &map_offset);

        /* Character color count */
        vdp2_state.buffered_regs.chctla |= format->scf_cc_count << 12;

        /* Character size */
        vdp2_state.buffered_regs.chctla |= (format->scf_character_size >> 2) << 8;

        /* Plane size */
        vdp2_state.buffered_regs.plsz |= (format->scf_plane_size - 1) << 2;

        /* Map */
        vdp2_state.buffered_regs.mpofn |= map_offset << 4;
        vdp2_state.buffered_regs.mpabn1 = (planes[1] << 8) | planes[0];
        vdp2_state.buffered_regs.mpcdn1 = (planes[3] << 8) | planes[2];
        vdp2_state.buffered_regs.pncn1 = pncn1;

        /* Special function */
        vdp2_state.buffered_regs.sfsel |= (format->scf_sf_code & 0x01) << 1;
        vdp2_state.buffered_regs.sfprmd |= (format->scf_sf_mode & 0x03) << 2;
}

static void
_nbg2_scrn_cell_format_set(const struct scrn_cell_format *format)
{
#ifdef DEBUG
        assert((format->scf_cc_count == SCRN_CCC_PALETTE_16) ||
               (format->scf_cc_count == SCRN_CCC_PALETTE_256));
#endif /* DEBUG */

        vdp2_state.buffered_regs.chctlb &= 0xFFFD;
        vdp2_state.buffered_regs.chctlb &= 0xFFFE;
        vdp2_state.buffered_regs.plsz &= 0xFFCF;
        vdp2_state.buffered_regs.mpofn &= 0xF8FF;
        vdp2_state.buffered_regs.mpabn2 = 0x0000;
        vdp2_state.buffered_regs.mpcdn2 = 0x0000;
        vdp2_state.buffered_regs.pncn2 = 0x0000;
        vdp2_state.buffered_regs.sfsel &= 0xFFFB;
        vdp2_state.buffered_regs.sfprmd &= 0xFFCF;

        /* Copy */
        (void)memcpy(&vdp2_state.nbg2.cell_format, format, sizeof(*format));

        uint16_t pncn2;
        pncn2 = _cell_pattern_name_control_calc(format);

        uint16_t planes[4];
        uint16_t map_offset;

        _cell_plane_calc(format, planes, &map_offset);

        /* Character color count */
        vdp2_state.buffered_regs.chctlb |= format->scf_cc_count << 1;

        /* Character Size */
        vdp2_state.buffered_regs.chctlb |= format->scf_character_size >> 2;

        /* Plane Size */
        vdp2_state.buffered_regs.plsz |= (format->scf_plane_size - 1) << 4;

        /* Map */
        vdp2_state.buffered_regs.mpofn |= map_offset << 8;
        vdp2_state.buffered_regs.mpabn2 = (planes[1] << 8) | planes[0];
        vdp2_state.buffered_regs.mpcdn2 = (planes[3] << 8) | planes[2];
        vdp2_state.buffered_regs.pncn2 = pncn2;

        /* Special function */
        vdp2_state.buffered_regs.sfsel |= (format->scf_sf_code & 0x01) << 2;
        vdp2_state.buffered_regs.sfprmd |= (format->scf_sf_mode & 0x03) << 4;
}

static void
_nbg3_scrn_cell_format_set(const struct scrn_cell_format *format)
{
#ifdef DEBUG
        assert((format->scf_cc_count == SCRN_CCC_PALETTE_16) ||
               (format->scf_cc_count == SCRN_CCC_PALETTE_256));
#endif /* DEBUG */

        vdp2_state.buffered_regs.chctlb &= 0xFFDF;
        vdp2_state.buffered_regs.chctlb &= 0xFFEF;
        vdp2_state.buffered_regs.plsz &= 0xFF3F;
        vdp2_state.buffered_regs.mpofn &= 0x8FFF;
        vdp2_state.buffered_regs.mpabn3 = 0x0000;
        vdp2_state.buffered_regs.mpcdn3 = 0x0000;
        vdp2_state.buffered_regs.pncn3 = 0x0000;
        vdp2_state.buffered_regs.sfsel &= 0xFFF7;
        vdp2_state.buffered_regs.sfprmd &= 0xFF3F;

        /* Copy */
        (void)memcpy(&vdp2_state.nbg3.cell_format, format,
            sizeof(struct scrn_cell_format));

        uint16_t pncn3;
        pncn3 = _cell_pattern_name_control_calc(format);

        uint16_t planes[4];
        uint16_t map_offset;

        _cell_plane_calc(format, planes, &map_offset);

        /* Character color count */
        vdp2_state.buffered_regs.chctlb |= format->scf_cc_count << 5;

        /* Character size */
        vdp2_state.buffered_regs.chctlb |= format->scf_character_size << 2;

        /* Plane size */
        vdp2_state.buffered_regs.plsz |= (format->scf_plane_size - 1) << 6;

        /* Map */
        vdp2_state.buffered_regs.mpofn |= map_offset << 12;
        vdp2_state.buffered_regs.mpabn3 = (planes[1] << 8) | planes[0];
        vdp2_state.buffered_regs.mpcdn3 = (planes[3] << 8) | planes[2];
        vdp2_state.buffered_regs.pncn3 = pncn3;

        /* Special function */
        vdp2_state.buffered_regs.sfsel |= (format->scf_sf_code & 0x01) << 3;
        vdp2_state.buffered_regs.sfprmd |= (format->scf_sf_mode & 0x03) << 6;
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

        vdp2_state.buffered_regs.chctlb &= 0x8FFF;
        vdp2_state.buffered_regs.chctlb &= 0xFEFF;
        vdp2_state.buffered_regs.rpmd &= 0xFFFE;
        vdp2_state.buffered_regs.plsz &= 0xFCFF;
        vdp2_state.buffered_regs.plsz &= 0xCFFF;
        vdp2_state.buffered_regs.mpofr &= 0xFFF8;
        vdp2_state.buffered_regs.mpofr &= 0xFF8F;
        vdp2_state.buffered_regs.mpabra = 0x0000;
        vdp2_state.buffered_regs.mpcdra = 0x0000;
        vdp2_state.buffered_regs.mpefra = 0x0000;
        vdp2_state.buffered_regs.mpghra = 0x0000;
        vdp2_state.buffered_regs.mpijra = 0x0000;
        vdp2_state.buffered_regs.mpklra = 0x0000;
        vdp2_state.buffered_regs.mpmnra = 0x0000;
        vdp2_state.buffered_regs.mpopra = 0x0000;
        vdp2_state.buffered_regs.mpabrb = 0x0000;
        vdp2_state.buffered_regs.mpcdrb = 0x0000;
        vdp2_state.buffered_regs.mpefrb = 0x0000;
        vdp2_state.buffered_regs.mpghrb = 0x0000;
        vdp2_state.buffered_regs.mpijrb = 0x0000;
        vdp2_state.buffered_regs.mpklrb = 0x0000;
        vdp2_state.buffered_regs.mpmnrb = 0x0000;
        vdp2_state.buffered_regs.mpoprb = 0x0000;
        vdp2_state.buffered_regs.pncr = 0x0000;
        vdp2_state.buffered_regs.sfsel &= 0xFFEF;
        vdp2_state.buffered_regs.sfprmd &= 0xFCFF;

        uint16_t pncr;
        pncr = _cell_pattern_name_control_calc(format);

        uint16_t planes[16];
        uint16_t map_offset;

        _cell_plane_calc(format, planes, &map_offset);

        /* Character color count */
        vdp2_state.buffered_regs.chctlb |= format->scf_cc_count << 12;

        /* Character size */
        vdp2_state.buffered_regs.chctlb |= (format->scf_character_size >> 2) << 8;

        /* Rotation parameter mode */
        vdp2_state.buffered_regs.rpmd |= format->scf_rp_mode;

        switch (format->scf_rp_mode) {
        case 0:
                /* Mode 0: Rotation Parameter A */

                /* Plane size */
                vdp2_state.buffered_regs.plsz |= (format->scf_plane_size - 1) << 8;

                /* Screen over process */

                /* Map */
                vdp2_state.buffered_regs.mpofr |= map_offset;

                vdp2_state.buffered_regs.mpabra = (planes[1] << 8) | planes[0];
                vdp2_state.buffered_regs.mpcdra = (planes[3] << 8) | planes[2];
                vdp2_state.buffered_regs.mpefra = (planes[5] << 8) | planes[4];
                vdp2_state.buffered_regs.mpghra = (planes[7] << 8) | planes[6];
                vdp2_state.buffered_regs.mpijra = (planes[9] << 8) | planes[8];
                vdp2_state.buffered_regs.mpklra = (planes[11] << 8) | planes[10];
                vdp2_state.buffered_regs.mpmnra = (planes[13] << 8) | planes[12];
                vdp2_state.buffered_regs.mpopra = (planes[15] << 8) | planes[14];
                break;
        case 1:
                /* Mode 1: Rotation Parameter B */

                /* Plane size */
                vdp2_state.buffered_regs.plsz |= (format->scf_plane_size - 1) << 12;

                /* Screen over process */

                /* Map */
                vdp2_state.buffered_regs.mpofr |= map_offset << 4;

                vdp2_state.buffered_regs.mpabrb = (planes[1] << 8) | planes[0];
                vdp2_state.buffered_regs.mpcdrb = (planes[3] << 8) | planes[2];
                vdp2_state.buffered_regs.mpefrb = (planes[5] << 8) | planes[4];
                vdp2_state.buffered_regs.mpghrb = (planes[7] << 8) | planes[6];
                vdp2_state.buffered_regs.mpijrb = (planes[9] << 8) | planes[8];
                vdp2_state.buffered_regs.mpklrb = (planes[11] << 8) | planes[10];
                vdp2_state.buffered_regs.mpmnrb = (planes[13] << 8) | planes[12];
                vdp2_state.buffered_regs.mpoprb = (planes[15] << 8) | planes[14];
                break;
        case 2:
                break;
        case 3:
                /* Set both Rotation parameter A and Rotation parameter B */
                break;
        }

        vdp2_state.buffered_regs.pncr = pncr;

        /* Special function */
        vdp2_state.buffered_regs.sfsel |= (format->scf_sf_code & 0x01) << 4;
        vdp2_state.buffered_regs.sfprmd |= (format->scf_sf_mode & 0x03) << 8;
}
