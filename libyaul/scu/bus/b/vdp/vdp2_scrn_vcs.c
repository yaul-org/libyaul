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
vdp2_scrn_vcs_set(const struct vdp2_scrn_vcs_format *vcs)
{
#ifdef DEBUG
        assert((vcs->scroll_screen == VDP2_SCRN_NBG0) ||
               (vcs->scroll_screen == VDP2_SCRN_NBG1));
#endif /* DEBUG */

        _state_vdp2()->regs.vcstau = VDP2_VRAM_BANK_4MBIT(vcs->vcs_table);
        _state_vdp2()->regs.vcstal = (vcs->vcs_table >> 1) & 0xFFFF;

        switch (vcs->scroll_screen) {
        case VDP2_SCRN_NBG0:
                _state_vdp2()->regs.scrctl &= 0xFFFE;
                _state_vdp2()->regs.scrctl |= 0x0001;
                break;
        case VDP2_SCRN_NBG1:
                _state_vdp2()->regs.scrctl &= 0xFEFF;
                _state_vdp2()->regs.scrctl |= 0x0100;
                break;
        }
}

void
vdp2_scrn_vcs_unset(uint8_t scrn)
{
#ifdef DEBUG
        assert((scrn == VDP2_SCRN_NBG0) ||
               (scrn == VDP2_SCRN_NBG1));
#endif /* DEBUG */

        switch (scrn) {
        case VDP2_SCRN_NBG0:
                _state_vdp2()->regs.scrctl &= 0xFFFE;
                break;
        case VDP2_SCRN_NBG1:
                _state_vdp2()->regs.scrctl &= 0xFEFF;
                break;
        }
}

void
vdp2_scrn_vcs_clear(void)
{
        _state_vdp2()->regs.scrctl &= 0xFEFE;
}
