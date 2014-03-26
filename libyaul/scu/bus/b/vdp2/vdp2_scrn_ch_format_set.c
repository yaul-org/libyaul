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
        (((64 * 64 * 2) / ((cfg)->ch_cs)) * ((cfg)->ch_pnds))
#define SCRN_NBGX_PLANE_SIZE(cfg)                                              \
        (4 * SCRN_NBGX_PAGE_SIZE(cfg))
#define SCRN_NBGX_MAP_SIZE(cfg)                                                \
        (((cfg)->pls) * SCRN_NBGX_PLANE_SIZE(cfg))

static uint16_t
pattern_name_control(struct scrn_ch_format *cfg)
{
}

void
vdp2_scrn_ch_format_set(struct scrn_ch_format *cfg)
{

        assert(cfg != NULL);

        /* Character size */
#ifdef DEBUG
        assert((cfg->ch_cs == (1 * 1)) || (cfg->ch_cs == (2 * 2)));
#endif /* DEBUG */

        /* Plane size */
#ifdef DEBUG
        assert((cfg->ch_pls == (1 * 1)) ||
               (cfg->ch_pls == (2 * 1)) ||
               (cfg->ch_pls == (2 * 2)));
#endif /* DEBUG */

        /* Map */
#ifdef DEBUG
#endif /* DEBUG */

        uint16_t plane_a;
        uint16_t plane_b;
        uint16_t plane_c;
        uint16_t plane_d;

        uint16_t map_mask;

        /* Calculate the starting map (plane A) address mask bits */
        map_mask = (((0x0080 << (cfg->ch_cs >> 2)) - 1) / cfg->ch_pnds) -
            (cfg->ch_pls - 1);

        /* Pattern name control */
        uint16_t pncnx;

#ifdef DEBUG
        /* Character number supplement mode */
        assert((cfg->ch_cnsm == 0) || (cfg->ch_cnsm == 1));

        /* Pattern name data size */
        assert((cfg->ch_pnds == 1) || (cfg->ch_pnds == 2));
#endif /* DEBUG */

        switch (cfg->ch_pnds) {
        case 1:
                /* Pattern name data size: 1-word */

                /* Character number supplement mode */
                switch (cfg->ch_cnsm) {
                case 0:
                        /* Auxiliary mode 0; flip function can be
                         * used */

                        /* Supplementary character number */
                        switch (cfg->ch_cs) {
                        case (1 * 1):
                                pncnx = (0x8000 | (((cfg->ch_scn >> 5) & 0x7C00) >> 10));
                                break;
                        case (2 * 2):
                                pncnx = (0x8000 | (((cfg->ch_scn >> 5) & 0x7000) >> 10) |
                                    ((cfg->ch_scn >> 5) & 0x03));
                                break;
                        }
                        break;
                case 1:
                        /* Auxiliary mode 1; flip function cannot be
                         * used */

                        /* Supplementary character number */
                        switch (cfg->ch_cs) {
                        case (1 * 1):
                                pncnx = (0xC000 | ((cfg->ch_scn >> 5) & 0x7000) >> 10);
                                break;
                        case (2 * 2):
                                pncnx = (0xC000 | (((cfg->ch_scn >> 5) & 0x4000) >> 10) |
                                    (cfg->ch_scn & 0x03));
                                break;
                        }
                }
                break;
        case 2:
                /* Pattern name data size: 2-words */
                pncnx = 0x0000;
                break;
        }

        switch (cfg->ch_scrn) {
        case SCRN_RBG1:
        case SCRN_NBG0:
                /* Character size */
                vdp2_regs.chctla &= 0xFFFE;
                vdp2_regs.chctla |= cfg->ch_cs >> 2;

                /* Plane size */
                vdp2_regs.plsz &= 0xFFFC;
                vdp2_regs.plsz |= cfg->ch_pls - 1;

                /* Map */
#ifdef DEBUG
#endif /* DEBUG */
                /* MEMORY_WRITE(16, VDP2(MPABN0), ab); */
                /* MEMORY_WRITE(16, VDP2(MPCDN0), cd); */

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(CHCTLA), vdp2_regs.chctla);
                MEMORY_WRITE(16, VDP2(PLSZ), vdp2_regs.plsz);
                MEMORY_WRITE(16, VDP2(PNCN0), pncnx);
                MEMORY_WRITE(16, VDP2(PRINA), vdp2_regs.prina);
                break;
        case SCRN_NBG1:
                /* Character size */
                vdp2_regs.chctla &= 0xFEFF;
                vdp2_regs.chctla |= (cfg->ch_cs >> 2) << 8;

                /* Plane size */
                vdp2_regs.plsz &= 0xFFF3;
                vdp2_regs.plsz |= (cfg->ch_pls - 1) << 2;

                /* Map */
                plane_a = (cfg->ch_map[0] / SCRN_NBGX_PAGE_SIZE(cfg)) & map_mask;
                plane_b = (cfg->ch_map[1] / SCRN_NBGX_PAGE_SIZE(cfg)) & map_mask;
                plane_c = (cfg->ch_map[2] / SCRN_NBGX_PAGE_SIZE(cfg)) & map_mask;
                plane_d = (cfg->ch_map[3] / SCRN_NBGX_PAGE_SIZE(cfg)) & map_mask;

                vdp2_regs.mpofn &= 0xFFFB;
                /* Mask off the topmost 3 bits */
                vdp2_regs.mpofn |= (plane_a & 0x01C0) >> 6;

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
                vdp2_regs.chctlb |= cfg->ch_cs >> 2;

                /* Plane Size */
                vdp2_regs.plsz &= 0xFFCF;
                vdp2_regs.plsz |= (cfg->ch_pls - 1) << 4;

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(CHCTLB), vdp2_regs.chctlb);
                MEMORY_WRITE(16, VDP2(PLSZ), vdp2_regs.plsz);
                /* MEMORY_WRITE(16, VDP2(MPABN2), ab); */
                /* MEMORY_WRITE(16, VDP2(MPCDN2), cd); */
                MEMORY_WRITE(16, VDP2(PNCN2), pncnx);
                break;
        case SCRN_NBG3:
                /* Character size */
                vdp2_regs.chctlb &= 0xFFEF;
                vdp2_regs.chctlb |= cfg->ch_cs << 2;

                /* Plane size */
                vdp2_regs.plsz &= 0xFF3F;
                vdp2_regs.plsz |= (cfg->ch_pls - 1) << 6;

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(CHCTLB), vdp2_regs.chctlb);
                MEMORY_WRITE(16, VDP2(PLSZ), vdp2_regs.plsz);
                /* MEMORY_WRITE(16, VDP2(MPABN3), ab); */
                /* MEMORY_WRITE(16, VDP2(MPCDN3), cd); */
                MEMORY_WRITE(16, VDP2(PNCN3), pncnx);
                break;
        case SCRN_RBG0:
                /* Character size */
                vdp2_regs.chctlb &= 0xFEFF;
                vdp2_regs.chctlb |= cfg->ch_cs << 6;

                /* Plane size */
                if ((vdp2_regs.rpmd & 0x0001) == 0x000) {
                        /* Rotation Parameter A */
                        vdp2_regs.plsz &= 0xFCFF;
                        vdp2_regs.plsz |= (cfg->ch_pls - 1) << 8;

                        /* MEMORY_WRITE(16, VDP2(MPABRA), ab); */
                        /* MEMORY_WRITE(16, VDP2(MPCDRA), cd); */
                        MEMORY_WRITE(16, VDP2(MPEFRA), 0x0000);
                        MEMORY_WRITE(16, VDP2(MPGHRA), 0x0000);
                        MEMORY_WRITE(16, VDP2(MPIJRA), 0x0000);
                        MEMORY_WRITE(16, VDP2(MPKLRA), 0x0000);
                        MEMORY_WRITE(16, VDP2(MPMNRA), 0x0000);
                        MEMORY_WRITE(16, VDP2(MPOPRA), 0x0000);
                } else {
                        /* Rotation Parameter B */
                        vdp2_regs.plsz &= 0xCFFF;
                        vdp2_regs.plsz |= (cfg->ch_pls - 1) << 12;

                        /* MEMORY_WRITE(16, VDP2(MPABRB), ab); */
                        /* MEMORY_WRITE(16, VDP2(MPCDRB), cd); */
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
        default:
                return;
        }
}
