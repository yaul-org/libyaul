/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include "vdp2-internal.h"

void
vdp2_scrn_display_set(uint8_t scrn, bool no_trans)
{
        uint16_t trans_scrn;

        /* Enable and disable scroll screens. */
        vdp2_regs.bgon |= 1 << (uint8_t)scrn;
        if (no_trans) {
                trans_scrn = (uint8_t)scrn + 8;
                vdp2_regs.bgon |= 1 << trans_scrn;
        }

        /* Write to register. */
        MEMORY_WRITE(16, VDP2(BGON), vdp2_regs.bgon);
}
