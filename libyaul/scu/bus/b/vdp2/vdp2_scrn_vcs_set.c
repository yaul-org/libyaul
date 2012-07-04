/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <vdp2/scrn.h>

#include "vdp2-internal.h"

void
vdp2_scrn_vcs_set(struct scrn_vcs_format *vcs)
{

        MEMORY_WRITE(16, VDP2(VCSTAU), (vcs->vcs_vcsta >> 17) & 0x0007);
        MEMORY_WRITE(16, VDP2(VCSTAL), (vcs->vcs_vcsta >> 1) & 0xFFFF);

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
                assert((vcs->vcs_scrn == SCRN_NBG0) ||
                    (vcs->vcs_scrn == SCRN_NBG1));
        }

        /* Write to memory */
        MEMORY_WRITE(16, VDP2(SCRCTL), vdp2_regs.scrctl);
}
