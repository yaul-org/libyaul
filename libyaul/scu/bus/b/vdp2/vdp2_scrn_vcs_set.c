/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <assert.h>

#include <vdp2/scrn.h>

#include "vdp2_internal.h"

void
vdp2_scrn_vcs_set(struct scrn_vcs_format *vcs)
{

        MEM_POKE(VDP2(VCSTAU), (vcs->vcs_vcsta >> 17) & 0x0007);
        MEM_POKE(VDP2(VCSTAL), (vcs->vcs_vcsta >> 1) & 0xFFFF);

        switch (vcs->vcs_scrn) {
        case SCRN_NBG0:
                vdp2_regs.scrctl &= 0xFFFE;
                vdp2_regs.scrctl |= 0x0001;
                break;
        case SCRN_NBG1:
                vdp2_regs.scrctl &= 0xFEFF;
                vdp2_regs.scrctl |= 0x0100;
                break;
        default:
                assert((vcs->vcs_scrn == SCRN_NBG0) || (vcs->vcs_scrn == SCRN_NBG1),
                    "invalid scroll screen");
        }

        /* Write to memory */
        MEM_POKE(VDP2(SCRCTL), vdp2_regs.scrctl);
}
