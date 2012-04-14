/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp2/scrn.h>

#include <common.h>

#include "vdp2_internal.h"

static uint16_t
pattern_name_control(struct scrn_ch_format *cfg)
{
        uint16_t pnc;
        uint16_t scn;

        switch (cfg->ch_pnds) {
        case 1:
                /* Character number supplement mode */
                switch (cfg->ch_cnsm) {
                case 0:
                        /* Auxiliary mode 0 */

                        /* Character number in pattern name data is
                         * 10-bits. The flip function be used. */
                        scn = 0x1F;
                        break;
                case 1:
                        /* Auxiliary mode 1 */

                        /* Character number in pattern name data is
                         * 12-bits. The flip function cannot be used. */
                        scn = 0x1C;
                        pnc = 0x4000;
                        break;
                default:
                        scn = 0;
                }
                /* Calculate the supplementary character number from the
                 * character lead address */
                scn = (((cfg->ch_scn & 0xFFFFF) >> 5) >> 10) & scn;

                pnc |= 0x8000 | /* Pattern name data size: 1-word */
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
vdp2_scrn_ch_format_set(enum scrn_type scrn, struct scrn_ch_format *c)
{
        uint16_t i;
        uint16_t pnc;

        uint16_t mapsz;
        uint16_t amt;
        uint16_t ab;
        uint16_t cd;

        /* Pattern name control */
        pnc = pattern_name_control(c);

        /* Calculate map value from map lead address */
        mapsz = 64 * 64 * 2; /* in bytes */
        amt = common_log2_down((mapsz / c->ch_cs) * c->ch_pnds);
        ab = ((c->ch_map[0] & 0xFFFFF) >> (amt - 8)) | ((c->ch_map[1] & 0xFFFFF) >> amt);
        cd = ((c->ch_map[2] & 0xFFFFF) >> (amt - 8)) | ((c->ch_map[3] & 0xFFFFF) >> amt);

        switch (scrn) {
        case SCRN_RBG1:
        case SCRN_NBG0:
                /* Character size */
                vdp2_regs.chctla &= 0xFFFE;
                vdp2_regs.chctla |= c->ch_cs >> 2;

                /* Plane size */
                vdp2_regs.plsz &= 0xFFFC;
                vdp2_regs.plsz |= c->ch_pls - 1;

                /* Write to memory. */
                MEM_POKE(VDP2(CHCTLA), vdp2_regs.chctla);
                MEM_POKE(VDP2(PLSZ), vdp2_regs.plsz);
                MEM_POKE(VDP2(MPABN0), ab);
                MEM_POKE(VDP2(MPCDN0), cd);
                MEM_POKE(VDP2(PNCN0), pnc);
                break;
        case SCRN_NBG1:
                /* Character size */
                vdp2_regs.chctla &= 0xFFEF;
                vdp2_regs.chctla |= c->ch_cs << 6;

                /* Plane size */
                vdp2_regs.plsz &= 0xFFF3;
                vdp2_regs.plsz |= (c->ch_pls - 1) << 2;

                MEM_POKE(VDP2(CHCTLA), vdp2_regs.chctla);
                MEM_POKE(VDP2(PLSZ), vdp2_regs.plsz);
                MEM_POKE(VDP2(MPABN1), ab);
                MEM_POKE(VDP2(MPCDN1), cd);
                MEM_POKE(VDP2(PNCN1), pnc);
                break;
        case SCRN_NBG2:
                /* Character Size */
                vdp2_regs.chctlb &= 0xFFFE;
                vdp2_regs.chctlb |= c->ch_cs >> 2;

                /* Plane Size */
                vdp2_regs.plsz &= 0xFFCF;
                vdp2_regs.plsz |= (c->ch_pls - 1) << 4;

                /* Write to memory. */
                MEM_POKE(VDP2(CHCTLB), vdp2_regs.chctlb);
                MEM_POKE(VDP2(PLSZ), vdp2_regs.plsz);
                MEM_POKE(VDP2(MPABN2), ab);
                MEM_POKE(VDP2(MPCDN2), cd);
                MEM_POKE(VDP2(PNCN2), pnc);
                break;
        case SCRN_NBG3:
                /* Character Size */
                vdp2_regs.chctlb &= 0xFFEF;
                vdp2_regs.chctlb |= c->ch_cs << 2;

                /* Plane Size */
                vdp2_regs.plsz &= 0xFF3F;
                vdp2_regs.plsz |= (c->ch_pls - 1) << 6;

                /* Write to memory. */
                MEM_POKE(VDP2(CHCTLB), vdp2_regs.chctlb);
                MEM_POKE(VDP2(PLSZ), vdp2_regs.plsz);
                MEM_POKE(VDP2(MPABN3), ab);
                MEM_POKE(VDP2(MPCDN3), cd);
                MEM_POKE(VDP2(PNCN3), pnc);
                break;
        case SCRN_RBG0:
                /* Character Size */
                vdp2_regs.chctlb &= 0xfeff;
                vdp2_regs.chctlb |= (c->ch_cs & 0x1) << 8;

                /* Plane Size */
                if ((vdp2_regs.rpmd & 0x1) == 0) {
                        /* Rotation Parameter A */
                        vdp2_regs.plsz &= 0xfcff;
                        vdp2_regs.plsz |= (c->ch_pls - 1) << 8;

                        for (i = 0; i < 0x10; i++)
                                MEM_POKE(VDP2(MPABRA + i), ab);
                } else {
                        /* Rotation Parameter B */
                        vdp2_regs.plsz &= 0xcfff;
                        vdp2_regs.plsz |= (c->ch_pls - 1) << 12;

                        for (i = 0; i < 0x10; i++)
                                MEM_POKE(VDP2(MPABRB + i), ab);
                }

                /* Write to memory. */
                MEM_POKE(VDP2(CHCTLB), vdp2_regs.chctlb);
                MEM_POKE(VDP2(PLSZ), vdp2_regs.plsz);
                MEM_POKE(VDP2(PNCN3), pnc);
                break;
        default:
                break;
        }
}
