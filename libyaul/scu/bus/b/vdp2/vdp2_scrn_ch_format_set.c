/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <vdp2/scrn.h>

#include "vdp2-internal.h"

#define SCRN_NBGX_PAGE_SIZE(format)                                            \
        /* 1-word with a 64x64 cell page is the smallest page size */          \
        (((64 * 64 * 2) / ((format)->scf_character_size)) *                    \
            ((format)->scf_pnd_size))

#define SCRN_NBGX_PLANE_SIZE(format)                                           \
        (((format)->scf_plane_size) * SCRN_NBGX_PAGE_SIZE(format))

void
vdp2_scrn_cell_format_set(struct scrn_cell_format *format)
{

#ifdef DEBUG
        assert(format != NULL);

        /* Check if the background passed is valid */
        assert((format->scf_scrn == SCRN_NBG0) ||
               (format->scf_scrn == SCRN_RBG1) ||
               (format->scf_scrn == SCRN_NBG1) ||
               (format->scf_scrn == SCRN_NBG2) ||
               (format->scf_scrn == SCRN_NBG3) ||
               (format->scf_scrn == SCRN_RBG0));

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
         * (0x800 bytes).
         *
         * If the dimension of the plane is 2x1, then the lead address
         * must be on 0x1000 byte boundary. */
        assert((format->scf_map.plane_a & (SCRN_NBGX_PLANE_SIZE(format) - 1)) == 0x0000);
        assert((format->scf_map.plane_b & (SCRN_NBGX_PLANE_SIZE(format) - 1)) == 0x0000);
        assert((format->scf_map.plane_c & (SCRN_NBGX_PLANE_SIZE(format) - 1)) == 0x0000);
        assert((format->scf_map.plane_d & (SCRN_NBGX_PLANE_SIZE(format) - 1)) == 0x0000);
#endif /* DEBUG */

        /* Map */
        uint16_t map_offset;
        uint16_t map_mask;
        uint16_t plane_a;
        uint16_t plane_b;
        uint16_t plane_c;
        uint16_t plane_d;

        /* Calculate the lead map (plane A) address mask bits */
        map_mask = (((0x0080 << (format->scf_character_size >> 2)) - 1) / format->scf_pnd_size) -
            (format->scf_plane_size - 1);

        plane_a = (format->scf_map.plane_a / SCRN_NBGX_PAGE_SIZE(format)) & map_mask;
        plane_b = (format->scf_map.plane_b / SCRN_NBGX_PAGE_SIZE(format)) & map_mask;
        plane_c = (format->scf_map.plane_c / SCRN_NBGX_PAGE_SIZE(format)) & map_mask;
        plane_d = (format->scf_map.plane_d / SCRN_NBGX_PAGE_SIZE(format)) & map_mask;

        /* Mask the upper 3-bits */
        map_offset = (plane_a & 0x01C0) >> 6;

        /* Pattern name control */
        uint16_t pncnx;
        uint16_t character_number;
        uint16_t palette_number;
        uint16_t sc_number; /* Supplementary palette number bits */
        uint16_t sp_number; /* Supplementary character number bits */

        switch (format->scf_pnd_size) {
        case 1:
                /* Pattern name data size: 1-word */

                character_number = format->scf_cp_table >> 5;
                palette_number = format->scf_color_palette >> 5;
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
                        sp_number = ((palette_number & 0x70) >> 4) << 5;
                        break;
                case SCRN_CCC_PALETTE_256:
                case SCRN_CCC_PALETTE_2048:
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
                                sc_number = (character_number & 0x7C00) >> 10;
                                break;
                        case (2 * 2):
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
                                sc_number = (character_number & 0x7000) >> 10;
                                break;
                        case (2 * 2):
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

        switch (format->scf_scrn) {
        case SCRN_RBG1:
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
                MEMORY_WRITE(16, VDP2(CHCTLB), vdp2_regs.chctlb);
                MEMORY_WRITE(16, VDP2(PLSZ), vdp2_regs.plsz);
                MEMORY_WRITE(16, VDP2(MPABN3), (plane_b << 8) | plane_a);
                MEMORY_WRITE(16, VDP2(MPCDN3), (plane_d << 8) | plane_c);
                MEMORY_WRITE(16, VDP2(PNCN3), pncnx);
                break;
        case SCRN_RBG0:
                /* Character color count */
                vdp2_regs.chctlb &= 0x8FFF;
                vdp2_regs.chctlb |= format->scf_cc_count << 12;

                /* Character size */
                vdp2_regs.chctlb &= 0xFEFF;
                vdp2_regs.chctlb |= format->scf_character_size << 6;

                /* Plane size */
                if ((vdp2_regs.rpmd & 0x0001) == 0x000) {
                        /* Rotation Parameter A */
                        vdp2_regs.plsz &= 0xFCFF;
                        vdp2_regs.plsz |= (format->scf_plane_size - 1) << 8;

                        MEMORY_WRITE(16, VDP2(MPABRA), (plane_b << 8) | plane_a);
                        MEMORY_WRITE(16, VDP2(MPCDRA), (plane_d << 8) | plane_c);
                        MEMORY_WRITE(16, VDP2(MPEFRA), 0x0000);
                        MEMORY_WRITE(16, VDP2(MPGHRA), 0x0000);
                        MEMORY_WRITE(16, VDP2(MPIJRA), 0x0000);
                        MEMORY_WRITE(16, VDP2(MPKLRA), 0x0000);
                        MEMORY_WRITE(16, VDP2(MPMNRA), 0x0000);
                        MEMORY_WRITE(16, VDP2(MPOPRA), 0x0000);
                } else {
                        /* Rotation Parameter B */
                        vdp2_regs.plsz &= 0xCFFF;
                        vdp2_regs.plsz |= (format->scf_plane_size - 1) << 12;

                        MEMORY_WRITE(16, VDP2(MPABRB), (plane_b << 8) | plane_a);
                        MEMORY_WRITE(16, VDP2(MPCDRB), (plane_d << 8) | plane_c);
                        MEMORY_WRITE(16, VDP2(MPEFRB), 0x0000);
                        MEMORY_WRITE(16, VDP2(MPGHRB), 0x0000);
                        MEMORY_WRITE(16, VDP2(MPIJRB), 0x0000);
                        MEMORY_WRITE(16, VDP2(MPKLRB), 0x0000);
                        MEMORY_WRITE(16, VDP2(MPMNRB), 0x0000);
                        MEMORY_WRITE(16, VDP2(MPOPRB), 0x0000);
                }

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(CHCTLB), vdp2_regs.chctlb);
                MEMORY_WRITE(16, VDP2(PLSZ), vdp2_regs.plsz);
                MEMORY_WRITE(16, VDP2(PNCN3), pncnx);
                break;
        }
}
