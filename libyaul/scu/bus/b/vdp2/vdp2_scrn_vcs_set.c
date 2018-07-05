/*
 * Copyright (c) 2012-2016 Israel Jacquez
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

        vdp2_state.buffered_regs.vcstau = (vcs->vcs_vcsta >> 17) & 0x0007;
        vdp2_state.buffered_regs.vcstal = (vcs->vcs_vcsta >> 1) & 0xFFFF;

        switch (vcs->vcs_scrn) {
        case SCRN_NBG0:
                vdp2_state.buffered_regs.scrctl &= 0xFFFE;
                vdp2_state.buffered_regs.scrctl |= 0x0001;
                break;
        case SCRN_NBG1:
                vdp2_state.buffered_regs.scrctl &= 0xFEFF;
                vdp2_state.buffered_regs.scrctl |= 0x0100;
                break;
        default:
                assert((vcs->vcs_scrn == SCRN_NBG0) ||
                       (vcs->vcs_scrn == SCRN_NBG1));
        }
}
