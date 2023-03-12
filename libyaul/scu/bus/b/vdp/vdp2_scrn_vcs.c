/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>
#include <vdp2/vram.h>

#include "vdp-internal.h"

void
vdp2_scrn_vcs_set(const vdp2_scrn_vcs_format_t *vcs_format)
{
    _state_vdp2()->shadow_regs.vcstau = VDP2_VRAM_BANK(vcs_format->table_base);
    _state_vdp2()->shadow_regs.vcstal = (vcs_format->table_base >> 1) & 0xFFFF;

    switch (vcs_format->scroll_screen) {
    case VDP2_SCRN_NBG0:
        _state_vdp2()->shadow_regs.scrctl &= 0xFFFE;
        _state_vdp2()->shadow_regs.scrctl |= 0x0001;
        break;
    case VDP2_SCRN_NBG1:
        _state_vdp2()->shadow_regs.scrctl &= 0xFEFF;
        _state_vdp2()->shadow_regs.scrctl |= 0x0100;
        break;
    default:
        break;
    }
}

void
vdp2_scrn_vcs_unset(vdp2_scrn_t scroll_screen)
{
    switch (scroll_screen) {
    case VDP2_SCRN_NBG0:
        _state_vdp2()->shadow_regs.scrctl &= 0xFFFE;
        break;
    case VDP2_SCRN_NBG1:
        _state_vdp2()->shadow_regs.scrctl &= 0xFEFF;
        break;
    default:
        break;
    }
}

void
vdp2_scrn_vcs_clear(void)
{
    _state_vdp2()->shadow_regs.scrctl &= 0xFEFE;
}
