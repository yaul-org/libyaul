/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include <assert.h>

#include "vdp2-internal.h"

void
vdp2_scrn_ls_set(struct scrn_ls_format *ls)
{
#ifdef DEBUG
        assert((ls->ls_scrn == SCRN_NBG0) ||
               (ls->ls_scrn == SCRN_NBG1));
#endif /* DEBUG */

        uint16_t lstau;
        uint16_t lstal;

        lstau = (ls->ls_lsta >> 17) & 0x0007;
        lstal = (ls->ls_lsta >> 1) & 0xFFFF;

        switch (ls->ls_scrn) {
        case SCRN_NBG0:
                vdp2_state.buffered_regs.scrctl &= 0x3F01;
                vdp2_state.buffered_regs.scrctl |= ls->ls_fun;

                vdp2_state.buffered_regs.lsta0u = lstau;
                vdp2_state.buffered_regs.lsta0l = lstal;
                break;
        case SCRN_NBG1:
                vdp2_state.buffered_regs.scrctl &= 0x3F01;
                vdp2_state.buffered_regs.scrctl |= ls->ls_fun;

                vdp2_state.buffered_regs.lsta1u = lstau;
                vdp2_state.buffered_regs.lsta1l = lstal;
                break;
        default:
                return;
        }
}
