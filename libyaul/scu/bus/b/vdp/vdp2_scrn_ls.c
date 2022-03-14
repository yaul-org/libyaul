/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include <assert.h>

#include "vdp-internal.h"

void
vdp2_scrn_ls_set(const vdp2_scrn_ls_format_t *ls)
{
#ifdef DEBUG
        assert((ls->scroll_screen == VDP2_SCRN_NBG0) ||
               (ls->scroll_screen == VDP2_SCRN_NBG1));
#endif /* DEBUG */

        const uint16_t lstau = VDP2_VRAM_BANK(ls->table);
        const uint16_t lstal = (ls->table >> 1) & 0xFFFF;

        const uint16_t interval_bit = uint32_log2(ls->interval & 0x0F);

        switch (ls->scroll_screen) {
        case VDP2_SCRN_NBG0:
                _state_vdp2()->regs->scrctl &= 0xFFC1;
                _state_vdp2()->regs->scrctl |= ls->enable << 1;
                _state_vdp2()->regs->scrctl |= interval_bit << 4;

                _state_vdp2()->regs->lsta0u = lstau;
                _state_vdp2()->regs->lsta0l = lstal;
                break;
        case VDP2_SCRN_NBG1:
                _state_vdp2()->regs->scrctl &= 0xC1FF;
                _state_vdp2()->regs->scrctl |= ls->enable << 9;
                _state_vdp2()->regs->scrctl |= interval_bit << 12;

                _state_vdp2()->regs->lsta1u = lstau;
                _state_vdp2()->regs->lsta1l = lstal;
                break;
        default:
                break;
        }
}
