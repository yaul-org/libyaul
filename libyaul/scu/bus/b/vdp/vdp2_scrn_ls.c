/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include <gamemath/uint32.h>

#include "vdp-internal.h"

void
vdp2_scrn_ls_set(const vdp2_scrn_ls_format_t *ls_format)
{
    const uint16_t lstau = VDP2_VRAM_BANK(ls_format->table_base);
    const uint16_t lstal = (ls_format->table_base >> 1) & 0xFFFF;

    const uint16_t interval_bit = uint32_log2(ls_format->interval & 0x0F);

    switch (ls_format->scroll_screen) {
    case VDP2_SCRN_NBG0:
        _state_vdp2()->shadow_regs.scrctl &= 0xFFC1;
        _state_vdp2()->shadow_regs.scrctl |= ls_format->type << 1;
        _state_vdp2()->shadow_regs.scrctl |= interval_bit << 4;

        _state_vdp2()->shadow_regs.lsta0u = lstau;
        _state_vdp2()->shadow_regs.lsta0l = lstal;
        break;
    case VDP2_SCRN_NBG1:
        _state_vdp2()->shadow_regs.scrctl &= 0xC1FF;
        _state_vdp2()->shadow_regs.scrctl |= ls_format->type << 9;
        _state_vdp2()->shadow_regs.scrctl |= interval_bit << 12;

        _state_vdp2()->shadow_regs.lsta1u = lstau;
        _state_vdp2()->shadow_regs.lsta1l = lstal;
        break;
    default:
        break;
    }
}
