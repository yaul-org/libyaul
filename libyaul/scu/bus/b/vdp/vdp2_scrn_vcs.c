/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <vdp2/scrn.h>

#include "vdp-internal.h"

void
vdp2_scrn_vcs_set(struct scrn_vcs_format *vcs)
{
        _state_vdp2()->regs.vcstau = VDP2_VRAM_BANK_4MBIT(vcs->vcs_vcsta);
        _state_vdp2()->regs.vcstal = (vcs->vcs_vcsta >> 1) & 0xFFFF;

        switch (vcs->vcs_scrn) {
        case SCRN_NBG0:
                _state_vdp2()->regs.scrctl &= 0xFFFE;
                _state_vdp2()->regs.scrctl |= 0x0001;
                break;
        case SCRN_NBG1:
                _state_vdp2()->regs.scrctl &= 0xFEFF;
                _state_vdp2()->regs.scrctl |= 0x0100;
                break;
        default:
                assert((vcs->vcs_scrn == SCRN_NBG0) ||
                       (vcs->vcs_scrn == SCRN_NBG1));
        }
}
