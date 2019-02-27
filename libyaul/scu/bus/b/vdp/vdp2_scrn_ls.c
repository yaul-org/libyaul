/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include <assert.h>

#include "vdp-internal.h"

void
vdp2_scrn_ls_set(const struct vdp2_scrn_ls_format *ls)
{
#ifdef DEBUG
        assert((ls->ls_scrn == VDP2_SCRN_NBG0) ||
               (ls->ls_scrn == VDP2_SCRN_NBG1));
#endif /* DEBUG */

        uint16_t lstau;
        uint16_t lstal;

        lstau = VDP2_VRAM_BANK_4MBIT(ls->ls_lsta);
        lstal = (ls->ls_lsta >> 1) & 0xFFFF;

        switch (ls->ls_scrn) {
        case VDP2_SCRN_NBG0:
                _state_vdp2()->regs.scrctl &= 0x3F01;
                _state_vdp2()->regs.scrctl |= ls->ls_fun;

                _state_vdp2()->regs.lsta0u = lstau;
                _state_vdp2()->regs.lsta0l = lstal;
                break;
        case VDP2_SCRN_NBG1:
                _state_vdp2()->regs.scrctl &= 0x3F01;
                _state_vdp2()->regs.scrctl |= ls->ls_fun;

                _state_vdp2()->regs.lsta1u = lstau;
                _state_vdp2()->regs.lsta1l = lstal;
                break;
        default:
                return;
        }
}
