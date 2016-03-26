/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <vdp2/scrn.h>
#include <vdp2/vram.h>

#include "vdp2-internal.h"

void
vdp2_scrn_cell_format_set(struct scrn_cell_format *format)
{

#ifdef DEBUG
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
#endif /* DEBUG */

        /* Map */
        uint16_t map_offset;
        uint16_t map_mask;
        uint16_t plane_a;
        uint16_t plane_b;
        uint16_t plane_c;
        uint16_t plane_d;
        uint16_t plane_e;
        uint16_t plane_f;
        uint16_t plane_g;
        uint16_t plane_h;
        uint16_t plane_i;
        uint16_t plane_j;
        uint16_t plane_k;
        uint16_t plane_l;
        uint16_t plane_m;
        uint16_t plane_n;
        uint16_t plane_o;
        uint16_t plane_p;

        plane_a = ((format->scf_map.plane_a - VRAM_ADDR_4MBIT(0, 0)) / SCRN_CALCULATE_PAGE_SIZE(format)) & 0x003F;
        plane_b = ((format->scf_map.plane_b - VRAM_ADDR_4MBIT(0, 0)) / SCRN_CALCULATE_PAGE_SIZE(format)) & 0x003F;
        plane_c = ((format->scf_map.plane_c - VRAM_ADDR_4MBIT(0, 0)) / SCRN_CALCULATE_PAGE_SIZE(format)) & 0x003F;
        plane_d = ((format->scf_map.plane_d - VRAM_ADDR_4MBIT(0, 0)) / SCRN_CALCULATE_PAGE_SIZE(format)) & 0x003F;

        /* The map offset is effectively the bank number */
        map_offset = VRAM_BANK_4MBIT(plane_a);

        /* Pattern name control */
        uint16_t pncnx;
        uint16_t character_number;
        uint16_t palette_number;
        uint16_t sc_number; /* Supplementary character number bits */
        uint16_t sp_number; /* Supplementary palette number bits */

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

                character_number = format->scf_cp_table >> 5;
                palette_number = format->scf_color_palette >> 5;

                switch (format->scf_cc_count) {
                case SCRN_CCC_PALETTE_16:
                        sp_number = ((palette_number & 0x70) >> 4) << 5;
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
                        /* Auxiliary mode 0; flip function can be
                         * used */

                        /* Supplementary character number */
                        switch (format->scf_character_size) {
                        case (1 * 1):
                                /* Character number in pattern name table: bits 11~0
                                 * Character number in supplemental data: 14 13 12 11 10 */
                                sc_number = (character_number & 0x7C00) >> 10;
                                break;
                        case (2 * 2):
                                /* Character number in pattern name table: bits 11~0
                                 * Character number in supplemental data: 14 13 12  1  0 */
                                sc_number = ((character_number & 0x7000) >> 10) |
                                    (character_number & 0x03);
                                break;
                        }

                        pncnx = 0x8000 | sc_number | sp_number;
                        break;
                case 1:
                        /* Auxiliary mode 1; flip function cannot be
                         * used */

                        /* Supplementary character number */
                        switch (format->scf_character_size) {
                        case (1 * 1):
                                /* Character number in pattern name table: bits 11~0
                                 * Character number in supplemental data: 14 13 12 __ __ */
                                sc_number = (character_number & 0x7000) >> 10;
                                break;
                        case (2 * 2):
                                /* Character number in pattern name table: bits 11~0
                                 * Character number in supplemental data: 14 __ __  1  0 */
                                sc_number = ((character_number & 0x4000) >> 10) |
                                    (character_number & 0x03);
                                break;
                        }

                        pncnx = 0xC000 | sc_number | sp_number;
                }
                break;
        case 2:
                /* Pattern name data size: 2-words */
                pncnx = 0x0000;
                break;
        }

        switch (format->scf_scroll_screen) {
        case SCRN_NBG0:
                /* Character color count */
                vdp2_regs.chctla &= 0xFF8F;
                vdp2_regs.chctla |= format->scf_cc_count << 4;

                /* Character size */
                vdp2_regs.chctla &= 0xFFFE;
                vdp2_regs.chctla |= format->scf_character_size >> 2;

                /* Plane size */
                vdp2_regs.plsz &= 0xFFFC;
                vdp2_regs.plsz |= format->scf_plane_size - 1;

                /* Map */
                vdp2_regs.mpofn &= 0xFFF8;
                vdp2_regs.mpofn |= map_offset;

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(MPABN1), (plane_b << 8) | plane_a);
                MEMORY_WRITE(16, VDP2(MPCDN1), (plane_d << 8) | plane_c);
                MEMORY_WRITE(16, VDP2(CHCTLA), vdp2_regs.chctla);
                MEMORY_WRITE(16, VDP2(PLSZ), vdp2_regs.plsz);
                MEMORY_WRITE(16, VDP2(PNCN0), pncnx);
                MEMORY_WRITE(16, VDP2(PRINA), vdp2_regs.prina);
                break;
        case SCRN_NBG1:
                /* Character color count */
                vdp2_regs.chctla &= 0xCFFF;
                vdp2_regs.chctla |= format->scf_cc_count << 12;

                /* Character size */
                vdp2_regs.chctla &= 0xFEFF;
                vdp2_regs.chctla |= (format->scf_character_size >> 2) << 8;

                /* Plane size */
                vdp2_regs.plsz &= 0xFFF3;
                vdp2_regs.plsz |= (format->scf_plane_size - 1) << 2;

                /* Map */
                vdp2_regs.mpofn &= 0xFF8F;
                vdp2_regs.mpofn |= map_offset << 4;

                MEMORY_WRITE(16, VDP2(MPOFN), vdp2_regs.mpofn);
                MEMORY_WRITE(16, VDP2(MPABN1), (plane_b << 8) | plane_a);
                MEMORY_WRITE(16, VDP2(MPCDN1), (plane_d << 8) | plane_c);
                MEMORY_WRITE(16, VDP2(CHCTLA), vdp2_regs.chctla);
                MEMORY_WRITE(16, VDP2(PLSZ), vdp2_regs.plsz);
                MEMORY_WRITE(16, VDP2(PNCN1), pncnx);
                break;
        case SCRN_NBG2:
#ifdef DEBUG
                assert((format->scf_cc_count == SCRN_CCC_PALETTE_16) ||
                       (format->scf_cc_count == SCRN_CCC_PALETTE_256));
#endif /* DEBUG */

                /* Character color count */
                vdp2_regs.chctlb &= 0xFFFD;
                vdp2_regs.chctlb |= format->scf_cc_count << 1;

                /* Character Size */
                vdp2_regs.chctlb &= 0xFFFE;
                vdp2_regs.chctlb |= format->scf_character_size >> 2;

                /* Plane Size */
                vdp2_regs.plsz &= 0xFFCF;
                vdp2_regs.plsz |= (format->scf_plane_size - 1) << 4;

                /* Map */
                vdp2_regs.mpofn &= 0xF8FF;
                vdp2_regs.mpofn |= map_offset << 8;

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(MPOFN), vdp2_regs.mpofn);
                MEMORY_WRITE(16, VDP2(CHCTLB), vdp2_regs.chctlb);
                MEMORY_WRITE(16, VDP2(PLSZ), vdp2_regs.plsz);
                MEMORY_WRITE(16, VDP2(MPABN2), (plane_b << 8) | plane_a);
                MEMORY_WRITE(16, VDP2(MPCDN2), (plane_d << 8) | plane_c);
                MEMORY_WRITE(16, VDP2(PNCN2), pncnx);
                break;
        case SCRN_NBG3:
#ifdef DEBUG
                assert((format->scf_cc_count == SCRN_CCC_PALETTE_16) ||
                       (format->scf_cc_count == SCRN_CCC_PALETTE_256));
#endif /* DEBUG */

                /* Character color count */
                vdp2_regs.chctlb &= 0xFFDF;
                vdp2_regs.chctlb |= format->scf_cc_count << 5;

                /* Character size */
                vdp2_regs.chctlb &= 0xFFEF;
                vdp2_regs.chctlb |= format->scf_character_size << 2;

                /* Plane size */
                vdp2_regs.plsz &= 0xFF3F;
                vdp2_regs.plsz |= (format->scf_plane_size - 1) << 6;

                /* Map */
                vdp2_regs.mpofn &= 0x8FFF;
                vdp2_regs.mpofn |= map_offset << 12;

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(MPOFN), vdp2_regs.mpofn);
                MEMORY_WRITE(16, VDP2(CHCTLB), vdp2_regs.chctlb);
                MEMORY_WRITE(16, VDP2(PLSZ), vdp2_regs.plsz);
                MEMORY_WRITE(16, VDP2(MPABN3), (plane_b << 8) | plane_a);
                MEMORY_WRITE(16, VDP2(MPCDN3), (plane_d << 8) | plane_c);
                MEMORY_WRITE(16, VDP2(PNCN3), pncnx);
                break;
        case SCRN_RBG0:
#ifdef DEBUG
                assert((format->scf_rp_mode == 0) ||
                       (format->scf_rp_mode == 1) ||
                       (format->scf_rp_mode == 2) ||
                       (format->scf_rp_mode == 3));
#endif /* DEBUG */


                plane_e = ((format->scf_map.plane_e - VRAM_ADDR_4MBIT(0, 0)) / SCRN_CALCULATE_PAGE_SIZE(format)) & 0x003F;
                plane_f = ((format->scf_map.plane_f - VRAM_ADDR_4MBIT(0, 0)) / SCRN_CALCULATE_PAGE_SIZE(format)) & 0x003F;
                plane_g = ((format->scf_map.plane_g - VRAM_ADDR_4MBIT(0, 0)) / SCRN_CALCULATE_PAGE_SIZE(format)) & 0x003F;
                plane_h = ((format->scf_map.plane_h - VRAM_ADDR_4MBIT(0, 0)) / SCRN_CALCULATE_PAGE_SIZE(format)) & 0x003F;
                plane_i = ((format->scf_map.plane_i - VRAM_ADDR_4MBIT(0, 0)) / SCRN_CALCULATE_PAGE_SIZE(format)) & 0x003F;
                plane_j = ((format->scf_map.plane_j - VRAM_ADDR_4MBIT(0, 0)) / SCRN_CALCULATE_PAGE_SIZE(format)) & 0x003F;
                plane_k = ((format->scf_map.plane_k - VRAM_ADDR_4MBIT(0, 0)) / SCRN_CALCULATE_PAGE_SIZE(format)) & 0x003F;
                plane_l = ((format->scf_map.plane_l - VRAM_ADDR_4MBIT(0, 0)) / SCRN_CALCULATE_PAGE_SIZE(format)) & 0x003F;
                plane_m = ((format->scf_map.plane_m - VRAM_ADDR_4MBIT(0, 0)) / SCRN_CALCULATE_PAGE_SIZE(format)) & 0x003F;
                plane_n = ((format->scf_map.plane_n - VRAM_ADDR_4MBIT(0, 0)) / SCRN_CALCULATE_PAGE_SIZE(format)) & 0x003F;
                plane_o = ((format->scf_map.plane_o - VRAM_ADDR_4MBIT(0, 0)) / SCRN_CALCULATE_PAGE_SIZE(format)) & 0x003F;
                plane_p = ((format->scf_map.plane_p - VRAM_ADDR_4MBIT(0, 0)) / SCRN_CALCULATE_PAGE_SIZE(format)) & 0x003F;

                /* Character color count */
                vdp2_regs.chctlb &= 0x8FFF;
                vdp2_regs.chctlb |= format->scf_cc_count << 12;

                /* Character size */
                vdp2_regs.chctlb &= 0xFEFF;
                vdp2_regs.chctlb |= (format->scf_character_size >> 2) << 8;

                /* Rotation parameter mode */
                vdp2_regs.rpmd &= 0xFFFE;
                vdp2_regs.rpmd |= format->scf_rp_mode;

                switch (format->scf_rp_mode) {
                case 0:
                        /* Mode 0: Rotation Parameter A */

                        /* Plane size */
                        vdp2_regs.plsz &= 0xFCFF;
                        vdp2_regs.plsz |= (format->scf_plane_size - 1) << 8;

                        /* Screen over process */

                        /* Map */
                        vdp2_regs.mpofr &= 0xFFF8;
                        vdp2_regs.mpofr |= map_offset;

                        MEMORY_WRITE(16, VDP2(MPABRA), (plane_b << 8) | plane_a);
                        MEMORY_WRITE(16, VDP2(MPCDRA), (plane_d << 8) | plane_c);
                        MEMORY_WRITE(16, VDP2(MPEFRA), (plane_e << 8) | plane_f);
                        MEMORY_WRITE(16, VDP2(MPGHRA), (plane_g << 8) | plane_h);
                        MEMORY_WRITE(16, VDP2(MPIJRA), (plane_i << 8) | plane_j);
                        MEMORY_WRITE(16, VDP2(MPKLRA), (plane_k << 8) | plane_l);
                        MEMORY_WRITE(16, VDP2(MPMNRA), (plane_m << 8) | plane_n);
                        MEMORY_WRITE(16, VDP2(MPOPRA), (plane_o << 8) | plane_p);
                        break;
                case 1:
                        /* Mode 1: Rotation Parameter B */

                        /* Plane size */
                        vdp2_regs.plsz &= 0xCFFF;
                        vdp2_regs.plsz |= (format->scf_plane_size - 1) << 12;

                        /* Screen over process */

                        /* Map */
                        vdp2_regs.mpofr &= 0xFF8F;
                        vdp2_regs.mpofr |= map_offset << 4;

                        MEMORY_WRITE(16, VDP2(MPABRB), (plane_b << 8) | plane_a);
                        MEMORY_WRITE(16, VDP2(MPCDRB), (plane_d << 8) | plane_c);
                        MEMORY_WRITE(16, VDP2(MPEFRB), (plane_e << 8) | plane_f);
                        MEMORY_WRITE(16, VDP2(MPGHRB), (plane_g << 8) | plane_h);
                        MEMORY_WRITE(16, VDP2(MPIJRB), (plane_i << 8) | plane_j);
                        MEMORY_WRITE(16, VDP2(MPKLRB), (plane_k << 8) | plane_l);
                        MEMORY_WRITE(16, VDP2(MPMNRB), (plane_m << 8) | plane_n);
                        MEMORY_WRITE(16, VDP2(MPOPRB), (plane_o << 8) | plane_p);
                        break;
                case 2:
                        break;
                case 3:
                        /* Set both Rotation parameter A and Rotation parameter B */
                        break;
                }

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(MPOFR), vdp2_regs.mpofr);
                MEMORY_WRITE(16, VDP2(CHCTLB), vdp2_regs.chctlb);
                MEMORY_WRITE(16, VDP2(PLSZ), vdp2_regs.plsz);
                MEMORY_WRITE(16, VDP2(PNCR), pncnx);
                MEMORY_WRITE(16, VDP2(RPMD), vdp2_regs.rpmd);
                break;
        }
}
