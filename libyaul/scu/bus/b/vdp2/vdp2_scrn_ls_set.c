/*
 * Copyright (c) 2012 Israel Jacquez
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
        uint16_t lstau;
        uint16_t lstal;

        lstau = (ls->ls_lsta >> 17) & 0x0007;
        lstal = (ls->ls_lsta >> 1) & 0xFFFF;

        switch (ls->ls_scrn) {
        case SCRN_NBG0:
                vdp2_regs.scrctl &= 0xFFF9;
                vdp2_regs.scrctl |= ls->ls_fun;

                MEMORY_WRITE(16, VDP2(LSTA0U), lstau);
                MEMORY_WRITE(16, VDP2(LSTA0L), lstal);
                break;
        case SCRN_NBG1:
                vdp2_regs.scrctl &= 0xF9FD;
                vdp2_regs.scrctl |= ls->ls_fun;

                MEMORY_WRITE(16, VDP2(LSTA1U), lstau);
                MEMORY_WRITE(16, VDP2(LSTA1L), lstal);
                break;
        default:
                assert((ls->ls_scrn == SCRN_NBG0) ||
                    (ls->ls_scrn == SCRN_NBG1));
                /* NOTREACHED */
        }

        /* Write to memory */
        MEMORY_WRITE(16, VDP2(SCRCTL), vdp2_regs.scrctl);
}
