/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <vdp2/scrn.h>

#include "vdp2-internal.h"

#define SCRN_NBGX_PAGE_SIZE(cfg)                                               \
        /* 1-word with a 64x64 cell page is the smallest page size */          \
        (((64 * 64 * 2) / ((cfg)->scf_character_size)) * ((cfg)->scf_pnd_size))
#define SCRN_NBGX_PLANE_SIZE(cfg)                                              \
        (4 * SCRN_NBGX_PAGE_SIZE(cfg))
#define SCRN_NBGX_MAP_SIZE(cfg)                                                \
        (((cfg)->pls) * SCRN_NBGX_PLANE_SIZE(cfg))

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

        /* Make sure that the lead address to character pattern table in
         * VRAM is on a 20-byte boundary */
        assert((format->scf_cp_table & 0x1F) == 0x00);

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
#endif /* DEBUG */

        /* Map */
        uint16_t map_offset;
        uint16_t map_mask;
        uint16_t plane_a;
        uint16_t plane_b;
        uint16_t plane_c;
        uint16_t plane_d;

        /* Calculate the starting map (plane A) address mask bits */
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

        character_number = format->scf_cp_table >> 5;
        switch (format->scf_pnd_size) {
        case 1:
                /* Pattern name data size: 1-word */

                /* Character number supplement mode */
                switch (format->scf_auxiliary_mode) {
                case 0:
                        /* Auxiliary mode 0; flip function can be
                         * used */

                        /* Supplementary character number */
                        switch (format->scf_character_size) {
                        case (1 * 1):
                                pncnx = (0x8000 | ((character_number & 0x7C00) >> 10));
                                break;
                        case (2 * 2):
                                pncnx = (0x8000 | ((character_number & 0x7000) >> 10) |
                                    (character_number & 0x03));
                                break;
                        }
                        break;
                case 1:
                        /* Auxiliary mode 1; flip function cannot be
                         * used */

                        /* Supplementary character number */
                        switch (format->scf_character_size) {
                        case (1 * 1):
                                pncnx = (0xC000 | ((character_number & 0x7000) >> 10));
                                break;
                        case (2 * 2):
                                pncnx = (0xC000 | ((character_number & 0x4000) >> 10) |
                                    (character_number & 0x03));
                                break;
                        }
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

        vdp2_scrn_character_color_count_set(format->scf_scrn, format->scf_cc_count);
}
