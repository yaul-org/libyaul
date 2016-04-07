/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include "vdp2-internal.h"

void
vdp2_scrn_display_clear(void)
{
        vdp2_state.buffered_regs.bgon = 0x0000;

        /* Write to register */
        MEMORY_WRITE(16, VDP2(BGON), vdp2_state.buffered_regs.bgon);
}
