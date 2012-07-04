/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include <common.h>

#include "vdp2-internal.h"

static uint16_t
pattern_name_control(struct scrn_ch_format *cfg)
{
        uint16_t pnc;
        uint16_t scn;
        uint8_t scn_bits;

        switch (cfg->ch_pnds) {
        case 1:
                /* Pattern name data size: 1-word */
                /* Character number supplement mode */
                switch (cfg->ch_cnsm) {
                case 0:
                        /* Auxiliary mode 0 */

                        /* Character number in pattern name data is
                         * 10-bits. The flip function be used. */
                        scn_bits = 0x1F;
                        break;
                case 1:
                        /* Auxiliary mode 1 */

                        /* Character number in pattern name data is
                         * 12-bits. The flip function cannot be used. */
                        scn_bits = 0x1C;
                        pnc = 0x4000;
                        break;
                default:
                        scn_bits = 0;
                        break;
                }
                /* Calculate the supplementary character number from the
                 * character lead address */
                scn = (((cfg->ch_scn & 0xFFFFF) >> 5) >> 10) & scn_bits;

                pnc |= 0x8000 | /* Pattern name data size: 1-word */
                    /* Supplementary character number */
                    scn |
                    /* Supplementary palette number */
                    ((cfg->ch_spn & 0x07) << 5) |
                    /* Special color calculation (for pattern name supplement data) */
                    ((cfg->ch_scc & 0x01) << 8) |
                    ((cfg->ch_sp & 0x01) << 9);
                return pnc;
        case 2:
                /* Pattern name data size: 2-words */
                return 0;
        }

        return 0;
}

void
vdp2_scrn_ch_format_set(struct scrn_ch_format *cfg)
{
        uint16_t pnc;

        uint16_t mapsz;
        uint16_t amt;
        uint16_t ab;
        uint16_t cd;

        /* Pattern name control */
        pnc = pattern_name_control(cfg);

        /* Calculate map value from map lead address */
        mapsz = 64 * 64 * 2; /* in bytes */
        amt = common_log2_down((mapsz / cfg->ch_cs) * cfg->ch_pnds);
        ab = ((cfg->ch_map[0] & 0xFFFFF) >> amt) | (((cfg->ch_map[1] & 0xFFFFF) >> amt) << 8);
        cd = ((cfg->ch_map[2] & 0xFFFFF) >> amt) | (((cfg->ch_map[3] & 0xFFFFF) >> amt) << 8);

        switch (cfg->ch_scrn) {
        case SCRN_RBG1:
        case SCRN_NBG0:
                /* Character size */
                vdp2_regs.chctla &= 0xFFFE;
                vdp2_regs.chctla |= cfg->ch_cs >> 2;

                /* Plane size */
                vdp2_regs.plsz &= 0xFFFC;
                vdp2_regs.plsz |= cfg->ch_pls - 1;

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(CHCTLA), vdp2_regs.chctla);
                MEMORY_WRITE(16, VDP2(PLSZ), vdp2_regs.plsz);
                MEMORY_WRITE(16, VDP2(MPABN0), ab);
                MEMORY_WRITE(16, VDP2(MPCDN0), cd);
                MEMORY_WRITE(16, VDP2(PNCN0), pnc);
                MEMORY_WRITE(16, VDP2(PRINA), vdp2_regs.prina);
                break;
        case SCRN_NBG1:
                /* Character size */
                vdp2_regs.chctla &= 0xFEFF;
                vdp2_regs.chctla |= (cfg->ch_cs >> 2) << 6;

                /* Plane size */
                vdp2_regs.plsz &= 0xFFF3;
                vdp2_regs.plsz |= (cfg->ch_pls - 1) << 2;

                MEMORY_WRITE(16, VDP2(CHCTLA), vdp2_regs.chctla);
                MEMORY_WRITE(16, VDP2(PLSZ), vdp2_regs.plsz);
                MEMORY_WRITE(16, VDP2(MPABN1), ab);
                MEMORY_WRITE(16, VDP2(MPCDN1), cd);
                MEMORY_WRITE(16, VDP2(PNCN1), pnc);
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
                MEMORY_WRITE(16, VDP2(MPABN2), ab);
                MEMORY_WRITE(16, VDP2(MPCDN2), cd);
                MEMORY_WRITE(16, VDP2(PNCN2), pnc);
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
                MEMORY_WRITE(16, VDP2(MPABN3), ab);
                MEMORY_WRITE(16, VDP2(MPCDN3), cd);
                MEMORY_WRITE(16, VDP2(PNCN3), pnc);
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

                        MEMORY_WRITE(16, VDP2(MPABRA), ab);
                        MEMORY_WRITE(16, VDP2(MPCDRA), cd);
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

                        MEMORY_WRITE(16, VDP2(MPABRB), ab);
                        MEMORY_WRITE(16, VDP2(MPCDRB), cd);
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
                MEMORY_WRITE(16, VDP2(PNCN3), pnc);
                break;
        default:
                return;
        }
}
