/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp2/scrn.h>

#include "vdp2_internal.h"

static void
comp_map(struct scrn_ch_format *c, unsigned short *b, unsigned short *p)
{
        unsigned char ch_cs;

        ch_cs = c->ch_cs << 1;

        /* Obtain map size in bytes. */
        *p = (13 - ch_cs) + (c->ch_pnds >> 1) + c->ch_pls;

        /* Calculate the number amount of bits needed. */
        *b = (1 << ((8 - c->ch_pnds) + ch_cs)) - (1 << c->ch_pls);
}

static void
comp_pnc(struct scrn_ch_format *c, enum scrn_type scrn)
{
        unsigned short pnc;

        pnc = 0;

        if (c->ch_pnds == 1) {
                pnc |= (c->ch_scn & 0x1f) << 0;
                pnc |= (c->ch_spn & 0x7) << 5;
                pnc |= (c->ch_scc & 0x1) << 8;
                pnc |= (c->ch_sp & 0x1) << 9;
                pnc |= (c->ch_cnsm & 0x1) << 14;
                pnc |= 0x8000;
        }

        MEM_POKE(VDP2(PNCN0 + scrn), pnc);
}

void
vdp2_scrn_ch_format_set(enum scrn_type scrn, struct scrn_ch_format *c)
{
        unsigned short ab;
        unsigned short b;
        unsigned short cd;
        unsigned short i;
        unsigned short p;

	/* Pattern Name Data Size */
        comp_pnc(c, scrn);

	/* Map */
        comp_map(c, &b, &p);

        ab = (((c->ch_map[0] >> p) & b) << 8) | ((c->ch_map[1] >> p) & b);
        cd = (((c->ch_map[1] >> p) & b) << 8) | ((c->ch_map[2] >> p) & b);

        switch (scrn) {
	case SCRN_RBG1:
        case SCRN_NBG0:
                /* Character Size */
                vdp2_regs.chctla &= 0xfffe;
                vdp2_regs.chctla |= (c->ch_cs & 0x1) << 0;

                /* Plane Size */
                vdp2_regs.plsz &= 0xfffc;
                vdp2_regs.plsz |= c->ch_pls << 0;

                /* Write to memory. */
                MEM_POKE(VDP2(CHCTLA), vdp2_regs.chctla);
                MEM_POKE(VDP2(PLSZ), vdp2_regs.plsz);
                MEM_POKE(VDP2(MPABN0), ab);
                MEM_POKE(VDP2(MPCDN0), cd);
                break;
        case SCRN_NBG1:
                /* Character Size */
                vdp2_regs.chctla &= 0xffef;
                vdp2_regs.chctla |= (c->ch_cs & 0x1) << 8;

                /* Plane Size */
                vdp2_regs.plsz &= 0xfff3;
                vdp2_regs.plsz |= c->ch_pls << 2;

                MEM_POKE(VDP2(CHCTLA), vdp2_regs.chctla);
                MEM_POKE(VDP2(PLSZ), vdp2_regs.plsz);
                MEM_POKE(VDP2(MPABN1), ab);
                MEM_POKE(VDP2(MPCDN1), cd);
                break;
        case SCRN_NBG2:
                /* Character Size */
                vdp2_regs.chctlb &= 0xfffe;
                vdp2_regs.chctlb |= (c->ch_cs & 0x1) << 0;

                /* Plane Size */
                vdp2_regs.plsz &= 0xffcf;
                vdp2_regs.plsz |= c->ch_pls << 4;

                /* Write to memory. */
                MEM_POKE(VDP2(CHCTLB), vdp2_regs.chctlb);
                MEM_POKE(VDP2(PLSZ), vdp2_regs.plsz);
                MEM_POKE(VDP2(MPABN2), ab);
                MEM_POKE(VDP2(MPCDN2), cd);
                break;
        case SCRN_NBG3:
                /* Character Size */
                vdp2_regs.chctlb &= 0xffef;
                vdp2_regs.chctlb |= (c->ch_cs & 0x1) << 4;

                /* Plane Size */
                vdp2_regs.plsz &= 0xff3f;
                vdp2_regs.plsz |= c->ch_pls << 6;

                /* Write to memory. */
                MEM_POKE(VDP2(CHCTLB), vdp2_regs.chctlb);
                MEM_POKE(VDP2(PLSZ), vdp2_regs.plsz);
                MEM_POKE(VDP2(MPABN3), ab);
                MEM_POKE(VDP2(MPCDN3), cd);
                break;
        case SCRN_RBG0:
                /* Character Size */
                vdp2_regs.chctlb &= 0xfeff;
                vdp2_regs.chctlb |= (c->ch_cs & 0x1) << 8;

                /* Plane Size */
                if ((vdp2_regs.rpmd & 0x1) == 0) {
                        /* Rotation Parameter A */
                        vdp2_regs.plsz &= 0xfcff;
                        vdp2_regs.plsz |= c->ch_pls << 8;

                        for (i = 0; i < 0x10; i++)
                                MEM_POKE(VDP2(MPABRA + i), ab);
                } else {
                        /* Rotation Parameter B */
                        vdp2_regs.plsz &= 0xcfff;
                        vdp2_regs.plsz |= c->ch_pls << 12;

                        for (i = 0; i < 0x10; i++)
                                MEM_POKE(VDP2(MPABRB + i), ab);
                }

                /* Write to memory. */
                MEM_POKE(VDP2(CHCTLB), vdp2_regs.chctlb);
                MEM_POKE(VDP2(PLSZ), vdp2_regs.plsz);
                break;
	default:
                break;
        }
}
