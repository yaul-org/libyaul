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
vdp2_scrn_color_offset_clear(void)
{
        vdp2_state.buffered_regs.clofen = 0x0000;
        vdp2_state.buffered_regs.clofsl = 0x0000;

        /* Write to memory */
        MEMORY_WRITE(16, VDP2(CLOFEN), vdp2_state.buffered_regs.clofen);
        MEMORY_WRITE(16, VDP2(CLOFSL), vdp2_state.buffered_regs.clofsl);
}
