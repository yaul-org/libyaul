/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp2/scrn.h>

#include <assert.h>

#include "vdp2_internal.h"

void
vdp2_scrn_ls_set(struct scrn_ls_format *ls)
{
        uint16_t lstau;
        uint16_t lstal;

        lstau = (ls->ls_lsta >> 17) & 0x0007;
        lstal = (ls->ls_lsta >> 1) & 0xFFFF;

        switch (ls->ls_scrn) {
        case SCRN_NBG0:
                vdp2_regs.scrctl &= 0xFFF9;
                vdp2_regs.scrctl |= ls->ls_fun;

                MEM_POKE(VDP2(LSTA0U), lstau);
                MEM_POKE(VDP2(LSTA0L), lstal);
                break;
        case SCRN_NBG1:
                vdp2_regs.scrctl &= 0xF9FD;
                vdp2_regs.scrctl |= ls->ls_fun;

                MEM_POKE(VDP2(LSTA1U), lstau);
                MEM_POKE(VDP2(LSTA1L), lstal);
                break;
        default:
                assert((ls->ls_scrn == SCRN_NBG0) || (ls->ls_scrn == SCRN_NBG1),
                    "invalid scroll screen");
        }

        /* Write to memory */
        MEM_POKE(VDP2(SCRCTL), vdp2_regs.scrctl);
}
